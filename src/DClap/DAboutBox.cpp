// DAboutBox.cp
// d.g.gilbert


#include "DAboutBox.h"
#include "DApplication.h"
#include "DWindow.h"
#include "DTask.h"


Local char *dclapinfo =
"using Don's C++ Class Application Library (DCLAP)" LINEEND
"D.Gilbert, Indiana Univ. Biology Dept." LINEEND
"software@bio.indiana.edu";

Local char *ncbiinfo =
"and the VIBRANT cross-platform toolkit from" LINEEND
"National Center for Biotechnology Information" LINEEND
"National Library of Medicine, N.I.H." LINEEND
"info@ncbi.nlm.nih.gov";

Local char *shortinfo =
"using Don's Class App Library (DCLAP), software@bio.indiana.edu" LINEEND
"and VIBRANT cross-platform toolkit, info@ncbi.nlm.nih.gov";


//class DAboutBoxPanel : public DPanel 

DAboutBoxPanel::DAboutBoxPanel(DWindow* itsWindow, const char* aboutText) :
	DPanel( 0, itsWindow, 25 * Nlm_stdCharWidth, 12 * Nlm_stdLineHeight, simple),
	fKeepItShort(false),
	fAboutLines(0),
	fDclapLines(0),
	fNcbiLines(0),
	fAboutText(NULL)
{
	if (aboutText)
		fAboutText= StrDup(aboutText);
	else {
		char *name = (char*)gApplication->Shortname();
		if ( name ) {
			fAboutText= StrDup( name);
			if (DApplication::kVersion) StrExtendCat( &fAboutText, DApplication::kVersion);
			StrExtendCat( &fAboutText, ", an app written");
			}
		else
			fAboutText= StrDup("An application written");
		}
	 SizeInfo();
}

DAboutBoxPanel::~DAboutBoxPanel() 
{
	MemFree(fAboutText);
}
		
void DAboutBoxPanel::Release(Nlm_PoinT mouse) 
{
	DWindow* mywind= this->GetWindow();
	if (mywind) mywind->Close();
}
 

void DAboutBoxPanel::SizeOneText(const char* text, long& maxlen, short& linecount)
{
	char *cp, *cp0;
	long	len;
	for (linecount= 0, cp= (char*)text; cp && *cp; ) {
		cp0= cp;
		cp= StrChr( cp, NEWLINE);  
		linecount++;
		if (!cp) len= StrLen(cp0);
		else { len= cp-cp0; cp++; }
		if (len>maxlen) maxlen= len;
		}
}

void DAboutBoxPanel::SizeInfo()
{
	long 	maxlen = 0;

	SizeOneText( fAboutText, maxlen, fAboutLines);
	fKeepItShort = (fAboutLines > 20);

	if (fKeepItShort) {
		SizeOneText( shortinfo, maxlen, fDclapLines);
		}
	else {
		SizeOneText( dclapinfo, maxlen, fDclapLines);
		SizeOneText( ncbiinfo, maxlen, fNcbiLines);
		}

#if THIS_ISNT_WORKING_TO_RESIZE_WINDOW
	maxlen= Min(80, maxlen); // fix for actual screen size...
	//this->ViewRect(r);
	this->GetPosition(r);
	//Nlm_GetRect( (Nlm_GraphiC)fPanel, &r);
	r.right = r.left + Nlm_stdCharWidth * maxlen;
	r.bottom= r.top + 12 + Nlm_stdLineHeight * (fAboutLines + fDclapLines + fNcbiLines);
	//Nlm_SetRect( (Nlm_GraphiC)fPanel, &r);
	this->SetPosition(r);
#endif
}



void DAboutBoxPanel::Draw()
{
	Nlm_RecT  r;
	short	lineheight;
	this->ViewRect(r);
	Nlm_InsetRect (&r, 2, 2);
	Nlm_SelectFont(Nlm_systemFont);
	lineheight=  Nlm_LineHeight();

	r.top += 6;
	r.bottom = r.top + 1 + fAboutLines * lineheight;
	Dgg_DrawTextbox( &r, fAboutText, StrLen(fAboutText), 'c', false);

	if (fKeepItShort) {
		r.top = r.bottom + 6;
		Nlm_Blue();
		r.bottom = r.top + 1 + fDclapLines * lineheight;
		Dgg_DrawTextbox( &r, shortinfo, StrLen(shortinfo), 'c', false);
		}

	else {
		r.top = r.bottom + 6;
		Nlm_Blue();
		r.bottom = r.top + 1 + fDclapLines * lineheight;
		Dgg_DrawTextbox( &r, dclapinfo, StrLen(dclapinfo), 'c', false);

		r.top = r.bottom + 6;
		Nlm_Red();
		r.bottom = r.top + 1 + fNcbiLines * lineheight;
		Dgg_DrawTextbox( &r, ncbiinfo, StrLen(ncbiinfo), 'c', false);
		}
}



//class DAboutBoxWindow : public DWindow



DAboutBoxWindow::DAboutBoxWindow(const char* aboutText) :
	DWindow( 0, gApplication, fixed)
{
	fModal = false;
	fMypanel = new DAboutBoxPanel(this, aboutText);  // create my panel
	if (PoseModally()) ;
}


DAboutBoxWindow::~DAboutBoxWindow()
{
	//delete fMypanel; /// nooooo.... dview::deletesubviews
}
		
void DAboutBoxWindow::Close() 
{ 
#if 0
	if (fModal) {
		fModal= false;
		DTask* closetask= new DTask(DApplication::kClose, DTask::kMenu,
						gApplication);
		this->PostTask(closetask);
		}
	else
#endif
	DWindow::Close();
	//delete this; // mswin gets sick here if win == modal kind
}


 


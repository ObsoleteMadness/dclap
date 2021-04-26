// DFindDlog.cpp


#include <ncbi.h>
#include <dgg.h>
#include <Dvibrant.h>
#include <DControl.h>
#include <DDialogText.h>
#include <DApplication.h>
#include <DMenu.h>
#include <DWindow.h>
#include <DTask.h>
#include <DUtil.h>
#include "DFindDlog.h"


Boolean DFindDialog::fBackwards= false;
Boolean DFindDialog::fCaseSense= false;
Boolean DFindDialog::fFullWord= false;
char* DFindDialog::fTarget[DFindDialog::kMaxTarg];
char* DFindDialog::fReplace[DFindDialog::kMaxReplace];


void DFindDialog::FindAgain()
{
	Boolean saveback= fBackwards;
	fBackwards= gKeys->shift(); 
	DoFind();
	fBackwards= saveback;
}

Boolean DFindDialog::IsMyAction(DTaskMaster* action) 
{	
	DView* aview= (DView*) action;

	switch (action->Id()) {
		case cBackwards : fBackwards= aview->GetStatus(); return true;
		case cCaseSense : fCaseSense= aview->GetStatus(); return true;
		case cFullWord  : fFullWord= aview->GetStatus(); return true;

		case findId:
		case replaceId:
		case replaceFindId:
		case replaceAllId:
			switch (action->Id()) {
				case replaceFindId: 
					GetReplace(); DoReplace(); 
					// fall into findId
				case findId		: 
					GetFind(); DoFind(); 	
					break;
					
				case replaceId: 
					GetReplace(); DoReplace(); 
					break;
					
				case replaceAllId: 
					GetReplace(); DoReplaceAll(); 
					break;
				}
			fModal= false;
			this->Close(); 
			return true;
			
		default:
			return DWindow::IsMyAction(action);	
		}
}


DFindDialog::DFindDialog() :
  DWindow( 0, gApplication, DWindow::fixed, -10, -10, -50, -20, "Find", kDontFreeOnClose) 
	//fBackwards(false), fCaseSense(false), fFullWord(false)
{
	fFindText= NULL;
	fReplaceText= NULL;
#if 0
	short i;
	for (i=0; i<kMaxTarg; i++) fTarget[i] = NULL;
	for (i=0; i<kMaxReplace; i++) fReplace[i] = NULL;
#endif
}

void DFindDialog::Open()
{
	if (!fFindText) BuildDlog();	 
	this->Select(); // for motif
	DWindow::Open();
}

void DFindDialog::InitFindDialog()
{
	short i;
	fBackwards= false;
	fCaseSense= false;
	fFullWord= false;
	for (i=0; i<kMaxTarg; i++) fTarget[i] = NULL;
	for (i=0; i<kMaxReplace; i++) fReplace[i] = NULL;
}

const char* DFindDialog::GetReplace()
{
	if (fReplaceText) {
		char* str= fReplaceText->GetText();
		Boolean gotstr= (str && *str);
		if (gotstr && (!fReplace[0] || StringCmp( str, fReplace[0]) != 0)) {
			MemFree(fReplace[kMaxReplace-1]);
			for (short i=kMaxReplace-1; i>0; i--) fReplace[i] = fReplace[i-1];
			fReplace[0]= str;
			}
		else
			MemFree( str);
		return fReplace[0];
		}
	else return NULL;
}

const char* DFindDialog::GetFind()
{
	if (fFindText) {
		char* str= fFindText->GetText();
		Boolean gotstr= (str && *str);
		if (gotstr && (!fTarget[0] || StringCmp( str, fTarget[0]) != 0)) {
			MemFree(fTarget[kMaxTarg-1]);
			for (short i=kMaxTarg-1; i>0; i--) fTarget[i] = fTarget[i-1];
			fTarget[0]= str;
			}
		else
			MemFree( str);
		return fTarget[0];
		}
	else return NULL;
}
	

DFindDialog::~DFindDialog()
{
#if 0
	short i;
	for (i=0; i<kMaxTarg; i++) MemFree(fTarget[i]);
	for (i=0; i<kMaxReplace; i++) MemFree(fReplace[i]);
#endif
}


void DFindDialog::BuildDlog()
{
	DView * super;
	DPrompt* pr;
	DCheckBox* ck;
	DCluster * maincluster;
	DDefaultButton* db;
	DButton* bb;
	char	* target;
	Nlm_PoinT npt;
	
	super= this;

#if 0	
		// this cluster messes up location of DEditText....
	maincluster= new DCluster( 0, super, 60, 40, true, NULL); 
	super= maincluster;
#endif

	pr= new DPrompt( 0, super, "   Find", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	target= fTarget[0];
	fFindText= new DEditText( 0, super, target, 50, Nlm_programFont); 
	this->SetEditText( fFindText);

	super->NextSubviewBelowLeft();
	pr= new DPrompt( 0, super, "Replace", 0, 0, Nlm_programFont);	 		
	super->NextSubviewToRight();
	target= fReplace[0];
	fReplaceText= new DEditText( 0, super, target, 50, Nlm_programFont); 


	maincluster= new DCluster( 0, super, 60, 10, true, NULL); 
	super= maincluster;

	super->NextSubviewBelowLeft();
	ck= new DCheckBox(cBackwards, super, "Backwards");
	ck->SetStatus(fBackwards);
	super->NextSubviewToRight();
	ck= new DCheckBox(cCaseSense, super, "Case sensitive");
	ck->SetStatus(fCaseSense);
	super->NextSubviewToRight();
	ck= new DCheckBox(cFullWord, super, "Entire word");
	ck->SetStatus(fFullWord);
	//super->NextSubviewToRight();

	super= this;
	//super->NextSubviewToRight();
	super->NextSubviewBelowLeft();

#if 1
	maincluster= new DCluster( 0, super, 10, 40, true, NULL); 
	super= maincluster;
#endif

	bb= new DButton( replaceId, super, "Replace");
	super->NextSubviewToRight();
	bb= new DButton( replaceFindId, super, "Replace & Find");
	super->NextSubviewToRight();
	bb= new DButton( replaceAllId, super, "Replace All");
	super->NextSubviewToRight();
 	super->GetNextPosition( &npt);
 	npt.x += 8;   
 	super->SetNextPosition( npt);
	db= new DDefaultButton( findId, super, "Find");
	//db->SetResize( DView::fixed, DView::moveinsuper); 

	super = this;
}
	



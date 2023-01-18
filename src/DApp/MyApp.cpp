// MyApp.cp
// d.g.gilbert -- test application using DClAp library
//


#include <DClap.h>
#include "DChildApp.h"
#include <DRich.h>

// test what parts of rich text processors are dragging
//#include "Profiler.h"



class DMyApp : public DRichApplication
{
public:
	enum myapptasks { 
		kSpecial1=1001, kSpecial2,
		kControlsDemo, kListboxDemo, kDialogTextDemo, kIconButtonsDemo,
		kTextDocDemo, kSlateWindow,kAutoWindow,
		kMGroup1, kMGroup2, kMGroup3,
		kDocWindow, kFixedWindow, kFrozenWindow, kRoundWindow, kAlertWindow,
		kFloatingWindow, kShadowWindow, kPlainWindow };
	void IMyApp(void);	 
	virtual ~DMyApp();
	
	virtual void ControlsDemo(void);	 
	virtual void ListboxDemo(void);	 
	virtual void DialogTextDemo(void);	 
	virtual void IconButtonsDemo(void);	 
	virtual void TextDocDemo(char* filename = NULL);	
	virtual void SlateWindow(void);	
	virtual void AutoPanelWindow(void);	

	virtual void SetUpMenus(void);	// override
	virtual	Boolean IsMyAction(DTaskMaster* action); //override
	virtual	void UpdateMenus(void); // override

};



// DIE 16bit SCUM -- damn 16bit os/compiler can't handle char const '1234'
// which i used through for very hande ID values

enum myappIDvalues {
cMnuS = 2000, cMgpS,cClux,cRada,cRadb,cRadc,cSlaW,cSlat,cClu4,
cLaunchApp,
cRad1,cRad2,cRad3,cDlg1,cClu1,cPgp1,cClu2,cEdt1,cEdt2,cEdt3,
cClu3,cPBut,cDBut,cChkb,cClu5,cSwbx,cMnuW,cGVue,cIcoB,cTDPl,
cSwb2,cClu6,cPopl,cDlgL,cclu0,cPgp2,cclu1,cLiBx,cDlgT,
cclu2,cAPWn,cETBx,cDlg2,cTdoc,cWinD,cWinF,cWinZ
};


extern "C" short Main(void)
{
  DMyApp* myapp = new DMyApp();
  myapp->IMyApp();
  myapp->Run();
  delete myapp; 
  return 0;
}


void DMyApp::IMyApp(void)
{
	IApplication();
	//gIconList->Read("icons");
	//fRTFsetup= new DRTFsetup(); // depends on gFileManager created in IApplication
}	 


DMyApp::~DMyApp()
{
	//delete fRTFsetup;
}

#if 0
static unsigned short pictureIcon[] = {
 2215, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x13a0,  0x500, 0x13a0,  0x480,
 0x13a8, 0x2440, 0x13a0,  0x420, 0x13a0, 0xe7f0, 0x13a1, 0x1010,
 0x13a2,  0x810, 0x13a2,  0x810, 0x13aa,  0x890, 0x13a1, 0x1010,
 0x13e0, 0xe010, 0x1220,   0x10, 0x1220,   0x10, 0x1220,   0x10,
 0x122b, 0xfe90, 0x1222,  0x210, 0x11c2,  0x210, 0x1322, 0x1fd0,
 0x1322, 0x1050, 0x1212, 0x1050, 0x121b, 0xf250, 0x1210, 0x1050,
 0x1230, 0x1050, 0x1120, 0x1fd0, 0x11a0,   0x10, 0x10e0,   0x10,
 0x1230, 0x2090, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0,
  0,0,0,0,0,0 
};

static DIcon gPictureIcon( (IconStore*) pictureIcon);
#endif


void DMyApp::SetUpMenus(void)
{
	DApplication::SetUpMenus();
 
	DMenu* sMenu = NewMenu(cMnuS, "Test");
 
	sMenu->AddItem( DMyApp::kSpecial1, "Check 1", true);
	sMenu->AddItem( DMyApp::kSpecial2, "Check 2", true);
	DMenuChoiceGroup* cGroup= new DMenuChoiceGroup(cMgpS, sMenu);
	cGroup->AddItem( DMyApp::kMGroup1, "GroupItem 1");
	cGroup->AddItem( DMyApp::kMGroup2, "GroupItem 2");
	cGroup->AddItem( DMyApp::kMGroup3, "GroupItem 3");
	sMenu->AddSeparator();								
	sMenu->AddItem( DMyApp::kControlsDemo, "Controls demo");
	sMenu->AddItem( DMyApp::kListboxDemo, "Listbox demo");
	sMenu->AddItem( DMyApp::kDialogTextDemo, "DialogText & LaunchApp demo");
	sMenu->AddItem( DMyApp::kIconButtonsDemo, "IconButtons demo");
	sMenu->AddItem( DMyApp::kTextDocDemo, "TextDoc demo");

#if THIS_IS_BUGGY
	sMenu->AddItem( DMyApp::kSlateWindow, "Slate test");
	sMenu->AddItem( DMyApp::kAutoWindow, "AutoPanel test");
#endif
	sMenu->suicide(1);

#if 1
	// need a GetMenu(cMnuW), & menu::RemoveItem(...)
	DMenu* wMenu= (DMenu*) gViewCentral->GetView(cMnuW);
	if (!wMenu) wMenu= sMenu;
	wMenu->AddSeparator();								
	wMenu->AddItem( DMyApp::kDocWindow, "Doc window");
	wMenu->AddItem( DMyApp::kFixedWindow, "Fixed window");
	wMenu->AddItem( DMyApp::kFrozenWindow, "Frozen window");
	wMenu->AddItem( DMyApp::kRoundWindow, "Round window");
	wMenu->AddItem( DMyApp::kAlertWindow, "Alert window");
	wMenu->AddItem( DMyApp::kFloatingWindow, "Floating window");
	wMenu->AddItem( DMyApp::kShadowWindow, "Shadow window");
	wMenu->AddItem( DMyApp::kPlainWindow, "Plain window");
	wMenu->suicide(1);
#endif	
}


#if 0
// Cut this DMyAutoPanel into two parts:
//	DTableView.cp/.h class -- general Table/Gridview for DClap
//	DGopherView subclass

class DMyAutoPanel : public DPanel
{
public:
		DGopherList* 	fItems;
		short		fMaxRows, fMaxCols;
		short		fItemHeight, fItemWidth;
		short		fTop, fLeft, fSelectedItem;
		short		*fWidths, *fHeights;
		Boolean	fHasVbar, fHasHbar;
		Nlm_RecT	fRect;
		
		DMyAutoPanel(long id, DView* itsSuperior, DGopherList* itsItems,
					short pixwidth, short pixheight, short itemwidth, short itemheight):
			DPanel( id, itsSuperior, pixwidth, pixheight, DPanel::autonomous),
			fItems(itsItems), fItemWidth(itemwidth), fItemHeight(itemheight),
			fTop(0), fLeft(0), fMaxRows(1), fMaxCols(1), fSelectedItem(-1),
			fWidths(NULL), fHeights(NULL)
		{
			Nlm_BaR sb = Nlm_GetSlateVScrollBar ((Nlm_SlatE) GetNlmObject());
		  fHasVbar= (sb != NULL);
		  sb = Nlm_GetSlateHScrollBar ((Nlm_SlatE) GetNlmObject());
		  fHasHbar= (sb != NULL);
		  FindLocation();
		}
		
		void SetGridSize( short rows, short cols)
		{
			short i;
			if (fWidths && rows!=fMaxRows) {
				fWidths= (short*)MemMore( fWidths, rows * sizeof(short));
				for (i= fMaxRows; i<rows; i++) fWidths[i]= fItemWidth;
				}
			if (fHeights && cols!=fMaxCols) {
				fHeights= (short*)MemMore( fHeights, cols * sizeof(short));
				for (i= fMaxCols; i<cols; i++) fHeights[i]= fItemHeight;
				}
			fMaxRows= rows; 
			fMaxCols= cols;
		}
		
		void SetItemWidth(short atcol, short ncols, short itemwidth)
		{
			short i;
			if (!fWidths && ncols < fMaxCols) {
				// make fWidths
				fWidths= (short*)MemNew( fMaxCols*sizeof(short));
				for (i= 0; i<fMaxCols; i++) fWidths[i]= fItemWidth;
				}
			short ncol= Min(fMaxCols, atcol+ncols);
			for (i= atcol; i<ncol; i++) fWidths[i]= itemwidth;
		}
		
		
		void FindLocation()
		{
			this->GetPosition(fRect); // ?? this isn't changing except w/ resize ??
		  if (fHasVbar) fRect.right  -= Nlm_vScrollBarWidth;
		  if (fHasHbar) fRect.bottom -= Nlm_hScrollBarHeight;
			Nlm_InsetRect( &fRect, 2, 2);
		}
		
		virtual void Resize(DView* superview, Nlm_PoinT sizechange)
		{
				// !! need this fRect change to deal w/ Resize() update event, 
				// which isn't put in a nice event queue, but is done immediately !!
			fRect.right  += sizechange.x;
			fRect.bottom += sizechange.y;
			DPanel::Resize(superview, sizechange);
			FindLocation();
		}

		virtual void Scroll(Boolean vertical, DView* scrollee, short newval, short oldval)
		{
			Nlm_RecT	r = fRect;
			short delta;
			short diff= newval-oldval;
			if (vertical) {
				fTop += diff;
				delta= diff * fItemHeight;
				Nlm_ScrollRect (&r, 0, -delta);
				if (diff<0) r.bottom= r.top - delta;
				else r.top= r.bottom - delta;
				}
			else {
				if (fWidths) {
					//short i, n;
					//delta= 0;
					//if (diff<0) n= -diff; else n=diff;
				  //for ( i=0 ; n>0; i++,n--) delta += fWidths[fLeft+i];
					fLeft += diff;
					}
				else {
					fLeft += diff;
					delta= diff * fItemWidth;
					Nlm_ScrollRect (&r, -delta, 0);
					if (diff<0) r.right= r.left - delta;
					else r.left= r.right - delta;
					}
				}
			Nlm_InvalRect(&r);
			}
			
		virtual void GetRowRect( short row, Nlm_RecT& r)
		{
			r= fRect;
			r.top += (row-fTop) * fItemHeight;
			r.bottom = r.top + fItemHeight;
		}

		virtual void GetColRect( short col, Nlm_RecT& r)
		{
			short i;
			r= fRect;
			if (fWidths) {
				for (i= fLeft; i<col; i++) r.left += fWidths[i];
				r.right= r.left + fWidths[col];
				}
			else {
				r.left += (col-fLeft) * fItemWidth;
				r.right = r.left + fItemWidth;
				}
		}

		virtual void GetCellRect( short row, short col, Nlm_RecT& r)
		{
			short i;
			r= fRect;
			if (fWidths) {
				for (i= fLeft; i<col; i++) r.left += fWidths[i];
				r.right= r.left + fWidths[col];
				}
			else {
				r.left += (col-fLeft) * fItemWidth;
				r.right = r.left + fItemWidth;
				}
			r.top += (row-fTop) * fItemHeight;
			r.bottom = r.top + fItemHeight;
		}
			
		virtual void ClickAt(long row, long col)
		{
			if (Nlm_dblClick) {
				DGopher* ag= fItems->GopherAt(row); 
				Message(MSG_OK, "Click on '%s'", (char*)ag->GetName());
				}
			else {
				Nlm_RecT	r;
				if (fSelectedItem>=0) {
					GetRowRect( fSelectedItem, r);
					Nlm_InvertRect( &r); //Nlm_InvalRect( &r);
					}
				if (fSelectedItem == row) fSelectedItem= -1;
				else fSelectedItem = row;
				if (fSelectedItem>=0) {
					GetRowRect( fSelectedItem, r);
					Nlm_InvertRect( &r);  
					}
				}
		}
			
		virtual void Click(Nlm_PoinT mouse)
			{
			// cellview::clickat(row, col);
			short row, col;
			row= fTop  + (mouse.y - fRect.top)  / fItemHeight;
			if (fWidths) {
				short xcol= fRect.left;
				col= fLeft;
				while (xcol < mouse.x) {
					xcol += fWidths[col];
					col++;
					}
				}
			else
				col= fLeft + (mouse.x - fRect.left) / fItemWidth;
			if (row >=0 && row < fMaxRows && col >= 0 && col < fMaxCols) 
				ClickAt( row, col);
			}

		virtual void DrawCell(Nlm_RecT r, short row, short col)
		{
			DGopher* ag= fItems->GopherAt(row); 
			switch (col) {
				case 0: //ag->DrawIcon(r,1); break;
					{
					DIcon *ic= *((DIcon**)gIconList->At(1)); 
					if (ic) ic->Draw(r);
					}
					break;
				case 1: ag->DrawTitle(r); break;
				case 2: ag->DrawPath(r); break;
				case 3: ag->DrawHost(r); break;
				case 4: ag->DrawPort(r); break;
				case 5: ag->DrawPlus(r); break;
				case 6: ag->DrawKind(r); break;
				case 7: ag->DrawDate(r); break;
				case 8: ag->DrawSize(r); break;
				case 9: ag->DrawAdmin(r); break;
				}
		}
			
		virtual void DrawRow(Nlm_RecT r, short row)
		{
			short col = fLeft; 
			if (fWidths) r.right= r.left + fWidths[col];
			else r.right= r.left + fItemWidth;
			while (r.left < fRect.right && col < fMaxCols) {
	    	if (Nlm_RectInRgn (&r, Nlm_updateRgn))  
	    		DrawCell( r, row, col);
				if (fWidths) {
					Nlm_OffsetRect( &r, fWidths[col], 0);
					r.right= r.left + fWidths[col+1];
					}
				else
					Nlm_OffsetRect( &r, fItemWidth, 0);
				col++;
				}
		}
		
		virtual void Draw()
		{
			Nlm_RecT	r = fRect;
			short 		row = fTop; 
			r.bottom= r.top + fItemHeight;
			Nlm_SelectFont(Nlm_programFont); //fFont);
			while (r.top < fRect.bottom && row < fMaxRows) {
	    	if (Nlm_RectInRgn (&r, Nlm_updateRgn)) {  
	    		DrawRow( r, row);
	    		if (row == fSelectedItem) Nlm_InvertRect( &r);
	    		}
				Nlm_OffsetRect( &r, 0, fItemHeight);
				row++;
				}
		}
		
};

#endif




// from vibincld.h
extern "C" Nlm_GphPrcsPtr Nlm_GetClassPtr PROTO((Nlm_GraphiC a));

#if 0

extern "C" void Nlm_SetVisible PROTO((Nlm_GraphiC a, Nlm_Boolean visibl));
	// ?? make this a DView:: method
void SetVisible(DView* aview, Boolean turnon)
{
	Nlm_SetVisible( (Nlm_GraphiC)aview->GetNlmObject(), turnon);
	if (aview->fSubordinates) {
		long i, n= aview->fSubordinates->GetSize();
		for (i= 0; i<n; i++) {
			DView* subord= (DView*)aview->fSubordinates->At(i);
			if (subord) SetVisible(subord, turnon); 
			}		
		}
}

#endif




class DControlTable : public DTableView
{
public:
	DList* fItems;
	//DWindow* fDummyWin;
	
	DControlTable(long id, DView* itsSuperior, DList* itsItems,
				short pixwidth, short pixheight, 
				short nrows, short ncols,
				short itemwidth, short itemheight):
	DTableView( id, itsSuperior, pixwidth, pixheight, nrows, ncols, itemwidth, itemheight),
	//fDummyWin(NULL),
	fItems(itsItems)
	{}

	//virtual void Drag(Nlm_PoinT mouse);
	//virtual void Hold(Nlm_PoinT mouse);
	//virtual void Release(Nlm_PoinT mouse);
	
	virtual void DoubleClickAt(long row, long col)
	{
		DCluster* ac= (DCluster*) fItems->At(row); 
		Message(MSG_OK, "Click on '%s'", (char*)ac->GetTitle());
	}

	virtual void Click(Nlm_PoinT mouse)
	{
		long row, col;
		PointToCell( mouse, row, col);
		DCluster* ac= (DCluster*) fItems->At(row); 
#ifdef WIN_MAC
  	Nlm_GphPrcsPtr  classPtr;
  	Nlm_BGphPnt			groupclick;
		Nlm_GraphiC g= (Nlm_GraphiC)ac->GetNlmObject();
 		classPtr = Nlm_GetClassPtr( g);
 		groupclick= classPtr->click;
		if (groupclick && (*groupclick)(g, mouse)) ;
#endif
	}
	
	virtual void DrawRow(Nlm_RecT r, short row)
	{
		DCluster* ac= (DCluster*) fItems->At(row); 
		if (ac) {
#if 0
			Nlm_InsetRect(&r, 2, 2);
			ac->SetPosition( r);
#endif
			ac->Show();
			}
	}

	
	void SetClusterPositions()
	{
		Nlm_RecT r, myrect= GetRect();
		for (short arow= GetTop(); arow< GetMaxRows(); arow++ ) {
			GetRowRect( arow, r);
			if (Nlm_RectInRect( &r, &myrect)) {
				DCluster* ac= (DCluster*) fItems->At(arow); 
				if (ac) {
					Nlm_InsetRect( &r, 2, 2);
					ac->SetPosition( r);
					Nlm_ValidRect( &r); // ??
					}
				}
			}
	}
	
	void Scroll(Boolean vertical, DView* scrollee, short newval, short oldval)
	{
#if 1
		DTableView::Scroll(vertical, scrollee, newval, oldval);
		this->SetClusterPositions();
#else
		Nlm_RecT	r = fRect;
		short diff= newval-oldval;
		if (vertical) {
			fTop += diff;
			}
		else {
			fLeft += diff;
			}
		this->Select(); // need for motif !
		Nlm_InvalRect(&r);
#endif
	}
	
};



void DMyApp::AutoPanelWindow()
{
	const short kRowHeight=  40;
	const short kRowWidth = 300;
	const short kNumItems =  30;  // pixel maxint enforces smaller num -- rowHeight * num > maxint
	DRadioButton* rb;
	DCluster* cluster;
	Nlm_RecT	arect, brect;
	short	rowHeight = kRowHeight;
	short rowWidth  = kRowWidth;

	//DWindow* dummy= new DWindow(cDumb, this, DWindow::fixed, 10, 10, 10, 10, NULL);
		
	DWindow* win= new DWindow(cAPWn, this, DWindow::document, -1, -1, -20, -20, "AutoPanel test");

	DList* glist = new DList();
	DControlTable *sl= 
		new DControlTable(cGVue, win, glist, kRowWidth, 250, kNumItems, 1, kRowWidth, kRowHeight);
	sl->SetResize( DView::relsuper, DView::relsuper);
	//sl->fDummyWin= dummy; // testing offscreen redraw
	
	DView* super= sl;
	for (short i= 1; i<=kNumItems; i++) {
		cluster= new DCluster(cClux, super, 100, 50, true, "Radio buttons"); //true==hidden ??
		//cluster->SetPosition( arect); //??
		rb= new DRadioButton(cRada, cluster, "Left");
		rb= new DRadioButton(cRadb, cluster, "Center");
		rb= new DRadioButton(cRadc, cluster, "Right");
		cluster->Hide(); //??
		glist->InsertLast(cluster);
		}

	sl->SetClusterPositions();
	
	win->Open();
	win->suicide(1);
}




class DMySlatePanel : public DSlatePanel
{
public:
		DCluster* fCluster;
		
		DMySlatePanel(long id, DSlate* itsSuperior, DCluster* itsCluster,
				short pixwidth, short pixheight):
			DSlatePanel( id, itsSuperior, pixwidth, pixheight),
			fCluster(itsCluster)
			{}
			
		virtual void Click(Nlm_PoinT mouse)
		{
			Message(MSG_OK, "Panel hit is ID #%d", this->Id());
		}
			
		virtual void Draw()
		{
			Nlm_RecT	r;
			char	snum[80];
			short	hoff, voff;
			
			this->GetPosition(r); // this doesn't take into account scroll

			this->GetOffset( hoff, voff);
			Nlm_OffsetRect( &r, -hoff, -voff); //<< !! this is needed to deal w/ scroll
			
			Nlm_FrameRect(&r);
			Nlm_InsetRect(&r, 2, 2);
							
			Nlm_LongToStr( this->Id(), snum, 0, 80);
			Nlm_DrawString( &r, snum, 'c', false);
			
			if (fCluster) {
				fCluster->SetPosition(r); //?? does this call draw??
				fCluster->Show();
	  		//fCluster->Select(); 	//??
				//Nlm_ValidRect( &r); 	// stop loop?
				}
		}
};



void DMyApp::SlateWindow()
{
	const short kRowHeight = 50;
	const short kNumItems = 20;
	DMySlatePanel *pan;
	DRadioButton* rb;
	DCluster* cluster;
	Nlm_RecT	arect, brect;
	short	rowHeight, rowWidth;
	DEditText*	et;
	DView*	super;
	
	DWindow* win= new DWindow(cSlaW, this, DWindow::document,-10, -10, -20, -20, "Slate test");

	DSlate *sl= new DSlate(cSlat, win, 20/*wid*/, 15/*hgt*/, DSlate::scrolling);

	cluster= new DCluster(cClu4, win, 100, 50, false, "Radio1 buttons");
	rb= new DRadioButton(cRad1, cluster, "Left1");
	rb= new DRadioButton(cRad2, cluster, "Center1");
	rb= new DRadioButton(cRad3, cluster, "Right1");
	
	sl->GetPosition( arect);
	cluster->GetPosition( brect);
	rowHeight= brect.bottom - brect.top;
	rowWidth= brect.right - brect.left;
  arect.top += 4;
  arect.left += 4;
 	arect.bottom = arect.top + rowHeight;
  arect.right = arect.left + rowWidth;
  Nlm_PoinT zeropt;
  zeropt.x = arect.left;
  zeropt.y = arect.top;

#if 0
  Nlm_GphPrcsPtr  classPtr;
  Nlm_GGphGph			slatelink;
  
 	classPtr = Nlm_GetClassPtr( (Nlm_GraphiC)sl->GetNlmObject());
 	slatelink= classPtr->linkIn;
#endif

	for (short i= 0; i<kNumItems; i++) {

		pan= new DMySlatePanel( 1001+i, sl, NULL, rowWidth, rowHeight);
		pan->SetPosition( arect);
#if 0
  	classPtr = Nlm_GetClassPtr( (Nlm_GraphiC)pan->GetNlmObject());
  	classPtr->linkIn= slatelink;
#endif
	
		//super= pan; //<< this makes NO NlmObjects as new cluster fails to find a superior draw object
		super= sl; // this bombs for as yet unknown reasons
		//super= win;  
		
		zeropt.x= arect.left;
		zeropt.y= arect.top;
		super->SetNextPosition(zeropt);
		cluster= new DCluster(cClux, super, 100, 50, false, "Radio buttons");
		//cluster->SetPosition( arect); //??
		pan->fCluster= cluster;
		rb= new DRadioButton(cRada, cluster, "Left");
		rb= new DRadioButton(cRadb, cluster, "Center");
		rb= new DRadioButton(cRadc, cluster, "Right");
		cluster->Hide(); //??
		
		//et= new DEditText(cEdt2, cluster, "Edit this text", 20);
		//win->SetEditText(et);
		//et->SetPosition( arect);

		//sl->RegisterRow( arect.top/*position*/, 2+rowHeight, 1/*count*/); 
										// ^^ must use pixels, not nums
		
		Nlm_OffsetRect( &arect, 0, 2+rowHeight);
		}
			
	
	//Nlm_BaR sb= Nlm_GetSlateVScrollBar((Nlm_SlatE)sl->GetNlmObject());
	//Nlm_SetRange (sb, 1, 5, 10); << don't need w/ RegisterRow working

	win->Open();
	win->suicide(1);
}


void DMyApp::ControlsDemo()
{
	DCluster* cluster;
	DWindow* win= new DWindow(cDlg1, this,  DWindow::fixed, -10, -10, -50, -20, "Controls Dialog");

	cluster= new DCluster(cClu1, win, 200, 16, false, "Prompt");
	DPrompt* pr= new DPrompt(cPgp1, cluster, "This is a prompt item", 0, 0, Nlm_programFont);	

	cluster= new DCluster(cClu2, win, 200, 16, false, "EditText");
	DEditText* et;
	et= new DEditText(cEdt1, cluster, "edit this text", 20);
	win->SetEditText(et);
	cluster->NextSubviewBelowLeft();
	
	et= new DEditText(cEdt2, cluster, "No, edit THIS text!", 20);
	win->SetEditText(et);
	cluster->NextSubviewBelowLeft();
	
	et= new DEditText(cEdt3, cluster, "Please edit *this* text?", 20);
	win->SetEditText(et);
	cluster->SetValue(2);

	cluster= new DCluster(cClu3, win, 100, 50, false, "Buttons");
	DButton* bb= new DButton(cPBut,cluster, "Plain button");
	DDefaultButton* db= new DDefaultButton(cDBut,cluster, "Default");
	DCheckBox* ck= new DCheckBox(cChkb, cluster, "Check box");
	ck->SetStatus(true);

	cluster= new DCluster(cClu4, win, 100, 50, false, "Radio buttons");
	DRadioButton* rb;
	rb= new DRadioButton(cRad1, cluster, "Left");
	rb= new DRadioButton(cRad2, cluster, "Center");
	rb= new DRadioButton(cRad3, cluster, "Right");
	cluster->SetValue(2);

	cluster= new DCluster(cClu5, win, 100, 20, false, "Switch box");
	DSwitchBox* sb= new DSwitchBox(cSwbx, cluster, true,  false);
	sb->SetValues(5,15);
	sb= new DSwitchBox(cSwb2, cluster, true,  true);
	sb->SetValues(15,50);

	cluster= new DCluster(cClu6, win, 100, 20, false, "Popup list");
	DPopupList* pl= new DPopupList(cPopl, cluster, true);

	pl->AddItem("Item 1");
	pl->AddItem("brown shoes");
	pl->AddItem("Shoes for Industry");
	pl->AddItem("Shoes for Defense");
	pl->SetValue(2);

	DIconButton* ib = new DIconButton(cIcoB, win, &gPictureIcon);

	win->AddOkayCancelButtons();
	win->Open();
}


void DMyApp::ListboxDemo()
{
	DCluster* cluster;
	DWindow* win= new DWindow(cDlgL, this, DWindow::document, -10, -10, -20, -20, "List Box");

	cluster= new DCluster(cclu0, win, 200, 16, false, "Prompt");
	DPrompt* pr= new DPrompt(cPgp2, cluster, "This is a test of list box", 0, 0, Nlm_programFont);
	pr->suicide(1);
	cluster->suicide(1);

	cluster= new DCluster(cclu1, win, 100, 26, false, "List box");
	DListBox* lb= new DListBox(cLiBx, cluster, 20, 5, false/*DListBox::kSingleScroll*/);
	lb->StartAppending();
	lb->Append("Item 1");
	lb->Append("brown shoes");
	lb->Append("yellow canaries");
	lb->Append("white as snow");
	lb->Append("foxfire");
	lb->Append("purple sage");
	lb->Append("red roses");
	lb->Append("blue violets");
	lb->Append("green jack-in-the-pulpit");
	lb->Append("Item Last");
	lb->DoneAppending();
	lb->SetValue(1); // doesn't seem active on DListBox display
	lb->SetResize( DView::relsuper, DView::relsuper);
	cluster->SetResize( DView::relsuper, DView::relsuper);
	lb->suicide(1);
	cluster->suicide(1);

	win->AddOkayCancelButtons();
	win->Open();
	win->suicide(1);
}


void DMyApp::DialogTextDemo()
{
	DCluster* cluster;
	DWindow* win= new DWindow(cDlgT, this, DWindow::document, -10, -10, -20, -20, "Dialog Text");

	Nlm_FonT itsfont = Nlm_programFont; //Nlm_ParseFont ("Times,12");
				// ^^ need a font manager object to hide some of this ?
	cluster= new DCluster(cclu2, win, 20, 10, false, "Editable text box");
	DDialogScrollText* dt = new DDialogScrollText(cETBx, cluster,
											 20, 10, itsfont, true/*kDoWrap*/);
	win->SetEditText(dt); 
	dt->SetResize( DView::relsuper, DView::relsuper);
	cluster->SetResize( DView::relsuper, DView::relsuper);
	dt->suicide(1);
	cluster->suicide(1);

	DButton* bb= new DButton( cLaunchApp, win, "Launch app specified in text box");

	win->AddOkayCancelButtons();
	win->Open();
	win->suicide(1);
}



class DIconWindow : public DWindow
{
public:
	enum {
		cSaveBut = 3232
		};

	DIconList* fIconList;

	DIconWindow(long id, DTaskMaster* itsSuperior, DIconList* itsIconList,
			short width = -5, short height = -5, short left = -50, short top = -20, char* title = NULL) :
		DWindow( id, itsSuperior, DWindow::fixed, width, height, left, top, title),
		fIconList(itsIconList)
		{}
			
	virtual ~DIconWindow() 
		{ 
			if (fIconList) { fIconList->FreeAllObjects(); delete fIconList; }
		}

	Boolean IsMyAction(DTaskMaster* action) 
		{	
			switch(action->Id()) {
				case cSaveBut:
					//Message(MSG_OK, "Save button hit");
					fIconList->WriteAsCode(NULL);
					return true;
				default: 
					return DWindow::IsMyAction(action);
				}
		}
	
};


static unsigned short testicons1[] = {
 2215, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x13a0,  0x500, 0x13a0,  0x480,
 0x13a8, 0x2440, 0x13a0,  0x420, 0x13a0, 0xe7f0, 0x13a1, 0x1010,
 0x13a2,  0x810, 0x13a2,  0x810, 0x13aa,  0x890, 0x13a1, 0x1010,
 0x13e0, 0xe010, 0x1220,   0x10, 0x1220,   0x10, 0x1220,   0x10,
 0x122b, 0xfe90, 0x1222,  0x210, 0x11c2,  0x210, 0x1322, 0x1fd0,
 0x1322, 0x1050, 0x1212, 0x1050, 0x121b, 0xf250, 0x1210, 0x1050,
 0x1230, 0x1050, 0x1120, 0x1fd0, 0x11a0,   0x10, 0x10e0,   0x10,
 0x1230, 0x2090, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0 
,
 1214, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x1000,  0x500, 0x1000,  0x480,
 0x1000,  0x440, 0x1000,  0x420, 0x1000,  0x7f0, 0x1000,   0x10,
 0x1049, 0x2410, 0x10aa, 0xaa10, 0x10aa, 0xaa10, 0x1049, 0x2410,
 0x1000,   0x10, 0x1000,   0x10, 0x1092, 0x9210, 0x10aa, 0xaa10,
 0x10aa, 0xaa10, 0x1092, 0x9210, 0x1000,   0x10, 0x1000,   0x10,
 0x1049, 0x2410, 0x10aa, 0xaa10, 0x10aa, 0xae10, 0x1049, 0x2a10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0 
,
	22213, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0x9040, 0xa840, 0xa840, 0x9240, 0x8240,
 0x8240, 0x8240, 0x8040, 0xffc0 
,
	22214, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0x8040, 0x8040, 0x8040, 0x8040, 0x8040,
 0x8040, 0x8040, 0x8040, 0xffc0 
,
 1214, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x1000,  0x500, 0x1000,  0x480,
 0x1000,  0x440, 0x1000,  0x420, 0x1000,  0x7f0, 0x1000,   0x10,
 0x1049, 0x2410, 0x10aa, 0xaa10, 0x10aa, 0xaa10, 0x1049, 0x2410,
 0x1000,   0x10, 0x1000,   0x10, 0x1092, 0x9210, 0x10aa, 0xaa10,
 0x10aa, 0xaa10, 0x1092, 0x9210, 0x1000,   0x10, 0x1000,   0x10,
 0x1049, 0x2410, 0x10aa, 0xaa10, 0x10aa, 0xae10, 0x1049, 0x2a10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10,
 0x1000,   0x10, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0 
,
  1215, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x13a0,  0x500, 0x13a0,  0x480,
 0x13a8, 0x2440, 0x13a0,  0x420, 0x13a0, 0xe7f0, 0x13a1, 0x1010,
 0x13a2,  0x810, 0x13a2,  0x810, 0x13aa,  0x890, 0x13a1, 0x1010,
 0x13e0, 0xe010, 0x1220,   0x10, 0x1220,   0x10, 0x1220,   0x10,
 0x122b, 0xfe90, 0x1222,  0x210, 0x11c2,  0x210, 0x1322, 0x1fd0,
 0x1322, 0x1050, 0x1212, 0x1050, 0x121b, 0xf250, 0x1210, 0x1050,
 0x1230, 0x1050, 0x1120, 0x1fd0, 0x11a0,   0x10, 0x10e0,   0x10,
 0x1230, 0x2090, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0
,
	22214, 0, 13, 13, 12,
 0xff00, 0x8180, 0x81c0, 0x8040, 0x8040, 0x8040, 0x8040, 0x8040,
 0x8040, 0x8040, 0x8040, 0xffc0 
,
  1215, 0, 32, 32, 64,
 0x1fff, 0xfc00, 0x1000,  0x600, 0x13a0,  0x500, 0x13a0,  0x480,
 0x13a8, 0x2440, 0x13a0,  0x420, 0x13a0, 0xe7f0, 0x13a1, 0x1010,
 0x13a2,  0x810, 0x13a2,  0x810, 0x13aa,  0x890, 0x13a1, 0x1010,
 0x13e0, 0xe010, 0x1220,   0x10, 0x1220,   0x10, 0x1220,   0x10,
 0x122b, 0xfe90, 0x1222,  0x210, 0x11c2,  0x210, 0x1322, 0x1fd0,
 0x1322, 0x1050, 0x1212, 0x1050, 0x121b, 0xf250, 0x1210, 0x1050,
 0x1230, 0x1050, 0x1120, 0x1fd0, 0x11a0,   0x10, 0x10e0,   0x10,
 0x1230, 0x2090, 0x1000,   0x10, 0x1000,   0x10, 0x1fff, 0xfff0
,
  0,0,0,0,0,0 
};


void DMyApp::IconButtonsDemo()
{
	DCluster* cluster;
	DWindow* win;
	
	char* prompt = "Please select a file containing icon data (X or Mac format)";
	win= new DWindow(0, this, DWindow::fixed, 500, 30, -1, -1, "Icon file");
	DPrompt* dp= new DPrompt(0, win, prompt);	 		
	win->Open();
	
	char* filename= (char*)gFileManager->GetInputFileName(".icn","TEXT");
	win->CloseAndFree();
	
	if (filename) {
		DIconList* myIconList= new DIconList();
		myIconList->StoreFromCode(testicons1, 3);
		myIconList->Read(filename);

		win= new DIconWindow(cDlg2, this, myIconList, -10, -10, -20, -20, "Icon Buttons");
	
		char title[80];
		StrCpy(title, "Icon buttons from ");
		StrNCat(title, (char*) gFileManager->FilenameFromPath(filename), 80);
		cluster= new DCluster(cclu2, win, 100, 50, false, title);
		short count= Min( 100, myIconList->GetSize());
		for (short ic= 0; ic<count; ic++) {
	 		if (ic % 10 == 0) cluster->NextSubviewBelowLeft();
	 		else cluster->NextSubviewToRight();
			DIcon* theIcon= myIconList->IconAt(ic);
			DIconButton* ib = new DIconButton(cIcoB, cluster, theIcon);
			}
		
		DButton* bb= new DButton(DIconWindow::cSaveBut, win, "Save as C code");
		win->AddOkayCancelButtons();
		win->Open();
		}
}






 
 
void DMyApp::TextDocDemo(char* filename)
{
	if (!filename)
		filename= (char*) gFileManager->GetInputFileName(NULL,NULL);
	if (filename) {
		filename= StrDup(filename);
		char* shortname= (char*)gFileManager->FilenameFromPath(filename);
		gCursor->watch();
		DWindow* win= new DWindow(cTdoc, this, DWindow::document, -1, -1, -10, -10, shortname);
		DRichView* td= new DRichView(cTDPl, win, 490, 300);
		td->SetResize( DView::matchsuper, DView::relsuper);

#if __profile__
#  ifdef powerc 
#  define TimeBase PPCTimeBase
#  else
#  define TimeBase microsecondsTimeBase;
#  endif
	if (!ProfilerInit(collectSummary, TimeBase, 2000, 40)) {

		td->ShowFile( filename);

		ProfilerDump("\pMyApp.prof");
		ProfilerTerm();
		}
	else 
#endif
		td->ShowFile( filename);
			
		win->Open();
		MemFree(filename);
		gCursor->arrow();
		}
}


#if 0
void DMyApp::TextDocDemo()
{
	// use this as kApp::Open...
	// enable the kApp::SaveAs & kApp::Print for frontwindows having any DDocPanels...
	char* filename= (char*) gFileManager->GetInputFileName(".txt","TEXT");
	if (filename) {
		filename= StrDup(filename);
		char* shortname= (char*)gFileManager->FilenameFromPath(filename);
		gCursor->watch();
		DWindow* win= new DWindow(cTdoc, this, DWindow::document, -1, -1, -10, -10, shortname);
		DTextDocPanel* td= new DTextDocPanel(cTDPl, win, 490, 300);
		td->SetResize( DView::matchsuper, DView::relsuper);
		td->ShowFile(filename);
		td->suicide(1);
		
		//win->AddOkayCancelButtons();
		win->Open();
		win->suicide(1);
		MemFree(filename);
		gCursor->arrow();
		}
}
#endif


#ifdef NOT_WIN_MAC
#include <EPPC.h> /* Apple highlevel events */
#include <Processes.h>  

char* long2str( long val)
{
	static char buf[4];
  buf[0]= (val >> 24) & 0xff;
  buf[1]= (val >> 16) & 0xff;
  buf[2]= (val >> 8) & 0xff;
  buf[3]= val & 0xff;
	return buf;	
}

extern ProcessSerialNumber  gChildProcessSN;
extern "C" void Nlm_PtoCstr (Nlm_CharPtr str); 

void CheckObits()
{
#if 1
	short err, itry = 0;
	ProcessInfoRec	info;
	do {
	  itry++;
		err= GetProcessInformation( &gChildProcessSN, &info);
		if (!err) Message( MSG_OK, "Child process %s, time %ld", 
							Nlm_PtoCstr( (Nlm_CharPtr)info.processName), info.processActiveTime );
	} while (err == 0 && itry < 20);
#else
	TargetID	sender;
	unsigned long msgRefcon = 0;
	EventRecord evt;
	short err, itry= 0;
	unsigned long buflen= 511;
	char* buf= (char*) MemNew(buflen+1);
	while ( WaitNextEvent( highLevelEventMask, &evt, 4, NULL) || itry<5) {
		itry++;	
		err= AcceptHighLevelEvent( &sender, &msgRefcon, buf, &buflen);
		Message(MSG_OK, "HL Event %s, message %s", 
									long2str(evt.message), long2str(msgRefcon));
		}
	MemFree(buf);
#endif
}

#else

void CheckObits()
{

}

#endif

Boolean DMyApp::IsMyAction(DTaskMaster* action) 
{
	DWindow* win= NULL;
	
	switch(action->Id()) {
		
				// some menu item messages
		case DMyApp::kControlsDemo: this->ControlsDemo(); return true;
		case DMyApp::kListboxDemo:	this->ListboxDemo(); return true;
		case DMyApp::kIconButtonsDemo: this->IconButtonsDemo(); return true;
		case DMyApp::kDialogTextDemo: this->DialogTextDemo(); return true;
		case DMyApp::kTextDocDemo: this->TextDocDemo(); return true;
		case DMyApp::kSlateWindow: this->SlateWindow(); return true;
		case DMyApp::kAutoWindow: this->AutoPanelWindow(); return true;
		
				// some button and control item messages
		case cPBut:
			Message(MSG_OK,"Plain button hit");
			return true;
	
		case cDBut:
			Message(MSG_OK, "Default button hit");
			return true;

		case cIcoB:
			Message(MSG_OK, "Icon button selected");
			return true;

		case cLaunchApp: {
			DButton* but= (DButton*) action; 
			if ( but && ((win= but->GetWindow()) != NULL) && win->HasEditText()) {
#if 1
				char* app= win->fEditText->GetText();
				char* cmdline= StrChr(app,' ');
				if (cmdline) *cmdline++= 0;
				DChildApp* child= new DChildApp( app, cmdline); 
#if 0
				child->AddFile( DChildFile::kStdout, "Stdout.dclap");
#endif
				if (child->Launch())
					Message(MSG_OK, "Launched app %s with '%s'", app, cmdline);
				else 
					Message(MSG_OK, "Failed to launch %s with '%s'", app, cmdline);
				MemFree( app);
#else
				char * app, * cmdline;
				//DTempFile* temp = new DTempFile();
				DFile* temp = new DFile("temp.out","a");
				temp->Close();
				app= win->fEditText->GetText();
				StrExtendCat( &app, " > ");
				StrExtendCat( &app, (char*)temp->GetName());
				
				cmdline= StrChr(app,' ');
				if (cmdline) *cmdline++= 0;
				if ( Dgg_LaunchApp( app, cmdline)) 
					Message(MSG_OK, "Launched app %s with '%s'", app, cmdline);
				else 
					Message(MSG_OK, "Failed to launch %s with '%s'", app, cmdline);
	
				// ?? wait for 'obit' appleEvent that child is finished ??
				CheckObits();
				
				TextDocDemo( (char*)temp->GetName());
				delete  temp;
				MemFree( app);
#endif
				}
			}
			return true;

		case cPopl:
			{
			short item = 0;
			DPopupList* pl= (DPopupList*) action;  
			if (pl) item= pl->GetValue();
			Message(MSG_OK, "Popup list item selected is #%d", item);
			}
			return true;

		case cOKAY:
		case cCANC:
			{
			DButton* but= (DButton*) action; 
			if ( but && ((win= but->GetWindow()) != NULL) ) {
				if (action->Id() == cOKAY) {
					// do Okay close handling...
					}
				win->CloseAndFree();
				}
			}
			return true;

			// menu grouping item message
		case cMgpS:
			{
			char* prompt = NULL;
			DMenuChoiceGroup* cGroup= (DMenuChoiceGroup*) action;  
			switch (cGroup->GetValue()) {
				case 1: prompt = "Group item 1 selected"; break;
				case 2: prompt = "group item 2 selected -- Hi Ho!"; break;
				case 3: prompt = "<<<Group item 3 selected>>>"; break;
				default: prompt = "Other group item selected"; break;
				}
			win= new DWindow(0, this, DWindow::fixed, 250, 50, -50, -20, "Menu Group result");
			DPrompt* dp= new DPrompt(0, win, prompt);
			dp->suicide(1);
	 		win->Open();
			win->suicide(1);
			return true;
			}
			
		case DMyApp::kDocWindow:
			win= new DWindow(0, this, DWindow::document, 200, 100, -50, -20, "Document");
	 		win->Open();
	 		win->suicide(1);
			return true;
		case DMyApp::kFixedWindow:
			win= new DWindow(0, this, DWindow::fixed, 200, 100, -50, -20, "Fixed");
	 		win->Open();
			win->suicide(1);
			return true;
		case DMyApp::kFrozenWindow:
			win= new DWindow(0, this, DWindow::frozen, 200, 100, -50, -20, "Frozen");
	 		win->Open();
			win->suicide(1);
			return true;
		case DMyApp::kRoundWindow:
			win= new DWindow(0, this, DWindow::round, 200, 100, -50, -20, "Round");
	 		win->Open();
			win->suicide(1);
			return true;
		case DMyApp::kAlertWindow:
			win= new DWindow(0, this, DWindow::alert, 120, 100);
	 		win->Open();
			win->suicide(1);
			return true;
		case DMyApp::kFloatingWindow:
			win= new DWindow(0, this, DWindow::floating, 120, 100);
	 		win->Open();
			win->suicide(1);
			return true;
		case DMyApp::kShadowWindow:
			win= new DWindow(0, this, DWindow::shadow, 120, 100);
	 		win->Open();
			win->suicide(1);
			return true;
		case DMyApp::kPlainWindow:
			win= new DWindow(0, this, DWindow::plain, 120, 100);
	 		win->Open();
			win->suicide(1);
			return true;
	   
		default: 
			return DApplication::IsMyAction(action);
		}
}



void DMyApp::UpdateMenus(void)
{
	DApplication::UpdateMenus();
	
	//gViewCentral->EnableView(DMyApp::kSpecial1);
	//gViewCentral->EnableView(DMyApp::kSpecial2);
}

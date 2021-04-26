// DPanel.h
// d.g.gilbert

#ifndef _DPANEL_
#define _DPANEL_

#include "Dvibrant.h"
#include "DView.h"
#include "DFile.h"
#include "DMethods.h"
#include "DCommand.h"

class DScrollBar;
class DTracker;

// DPanel is a general drawing area in a window (or in a view inside a window...)
// User must subclass Draw() method to display something.

class DPanel : public DView
{	
public:
	enum PanelKind { simple, autonomous, slatebound, subclasscreates };
	Nlm_PaneL	fPanel;
	PanelKind	fPanelKind;
	Boolean		fMouseStillDown;
	DTracker* fTracker;
	
	DPanel(long id, DView* itsSuperior, short pixwidth, short pixheight, PanelKind kind = simple);
	virtual ~DPanel();

	virtual void Draw(void) = 0;
	virtual void Scroll(Boolean vertical, DView* scrollee, short newval, short oldval);
	void RegisterRect(Nlm_RecT r) { Nlm_RegisterRect(fPanel, &r); }
	virtual void CharHandler(char c); // override stub, only for autonomous >> see class DKeyCallback instead
	void SetSlateBorder(Boolean turnon);

		// mouse tracking
	virtual void Click(Nlm_PoinT mouse);
	virtual void Drag(Nlm_PoinT mouse);
	virtual void Hold(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void SetTracker( DTracker* mouseTracker);
	virtual Nlm_Boolean IsDoneTracking();
	virtual void TrackMouse( short aTrackPhase,
					Nlm_PoinT& anchorPoint, Nlm_PoinT& previousPoint,
					Nlm_PoinT& nextPoint,	Nlm_Boolean mouseDidMove);
	virtual void TrackFeedback( short aTrackPhase,
					const Nlm_PoinT& anchorPoint, const Nlm_PoinT& previousPoint,
					const Nlm_PoinT& nextPoint, Nlm_Boolean mouseDidMove, Nlm_Boolean turnItOn);
};



class DAutoPanel : public DPanel
{	
public:	
	DAutoPanel(long id, DView* itsSuperior, short pixwidth, short pixheight,
			Boolean hasVscroll, Boolean hasHscroll);
};



// DSlate is mainly (?) a scrolling handler for DPanel.  See below & Vibrant docs

class DSlate : public DView
{	
public:
	Nlm_SlatE	fSlate;
	enum SlateKind { scrolling, normal, hidden, general };
	DSlate(long id, DView* itsSuperior, short width, short height, SlateKind kind = normal);

	void VirtualSlate( short before, short after);

	void SetCharHandler(void);
	virtual void CharHandler(char c); // override as needed

	void CaptureFocus(void) { Nlm_CaptureSlateFocus( fSlate); }
		
	DScrollBar* GetVScrollBar(void) {
		return (DScrollBar*) Nlm_GetObject( (Nlm_GraphiC)Nlm_GetSlateVScrollBar(fSlate));
		}
	DScrollBar* GetHScrollBar(void) {
		return (DScrollBar*) Nlm_GetObject( (Nlm_GraphiC)Nlm_GetSlateHScrollBar(fSlate));
		}
	
	void SetSlateBorder(Boolean turnon) {	Nlm_SetSlateBorder(fSlate, turnon); }
	
	void RegisterRow( short position, short height, short count) {
		Nlm_RegisterRow( fSlate, position, height, count);
		}
	void RegisterColumn( short position, short width, short count) {
		Nlm_RegisterColumn( fSlate, position, width, count);
		}

};




class DNotePanel : public DPanel
{	
	Nlm_IcoN fIconBut;
	char*		fText;
	ulong	fTextLen;
	Nlm_FonT fFont;
	Justify	fJust;
	Boolean	fEnabled, fDimmed;
public:
	DNotePanel(long id, DView* itsSuperior, const char* title, 
			short pixwidth = 0, short pixheight = 0, Nlm_FonT font = Nlm_programFont, Justify just = justleft);
	virtual void GetSize(short& width, short& height);
	virtual void SetTitle(char* title);
	virtual char* GetTitle(char* title = NULL, ulong maxsize = 256); 
	virtual void Draw();
	virtual void Click(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void Invalid(short newval, short oldval);
	virtual void Enable();	
	virtual void Disable();
	virtual void SetDim(Boolean isDim, Boolean redraw= false);  
};


class DIcon;

class DIconButton : public DPanel
{	
public:
	Nlm_IcoN fIconBut;
	DIcon*	fIcon;
	Boolean	fEnabled;
	
	DIconButton(long id, DView* itsSuperior, DIcon* itsIcon);
	virtual void Draw();
	virtual void Click(Nlm_PoinT mouse);
	virtual void Release(Nlm_PoinT mouse);
	virtual void Invalid(short newval, short oldval);
	virtual void SetIcon( DIcon* itsIcon, Boolean redraw = false);
	void GetIconSize(short& pixwidth, short& pixheight);
	virtual void Enable();	
	virtual void Disable();
};


class DSlatePanel : public DPanel
{
public:
	DSlatePanel(long id, DSlate* itsSuperior, short pixwidth, short pixheight):
		DPanel(id, itsSuperior, pixwidth, pixheight, DPanel::slatebound) 
		{}
};


class DDisplay : public DView	
{
public:
	Nlm_DisplaY	fDisplay;  // Nlm_DisplaY == form of Nlm_Slate
	enum DisplayKind	{ normal, scrolling };
	DDisplay(long id, DView* itsSuperior, short width, short height, 
					 DisplayKind kind = scrolling);
};



		// DDocPanel is Vibrant's newer DocumentPanel type (document.h)
class DDocPanel : public DPanel, public DPrintHandler, public DSaveHandler
{
public:
	Nlm_DoC	fDocpanel;
	
	DDocPanel(long id, DView* itsSuperior, short pixwidth, short pixheight);
	~DDocPanel();

	virtual void Draw(void) {}
	
	virtual void Print(void)	{ 
		Nlm_PrintDocument(fDocpanel); 
		}
	virtual void Save(DFile* f) { 
		if (f) Nlm_SaveDocument(fDocpanel, f->fFile); 
		} 

	virtual	char* DataToChar(Pointer ptr); // override to get below methods to draw.

	void  SetTabs( short tabstops) { Nlm_SetDocTabstops(fDocpanel, tabstops); }
	
	void 	Append( Pointer data, short lines, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat,
						Nlm_FonT font = Nlm_programFont, Boolean docOwnsData = true);
	void 	Replace( short item, Pointer data, short lines, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat,
						Nlm_FonT font = Nlm_programFont, Boolean docOwnsData = true);
	void 	Insert( short item, Pointer data, short lines, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat,
						Nlm_FonT font = Nlm_programFont, Boolean docOwnsData = true);

	void	Delete( short item) {
		Nlm_DeleteItem(fDocpanel, item);
		}
			
	virtual char* GetText(short item, short row, short col) {
		return Nlm_GetDocText(fDocpanel, item, row, col);
		}
	void 	MapPoint( Nlm_PoinT pt, short& item, short& row, short& col, Nlm_RectPtr rct) {
		Nlm_MapDocPoint(fDocpanel, pt, &item, &row, &col, rct);
		}
	void	GetStats( short& itemCount, short& lineCount) {
	 	Nlm_GetDocParams(fDocpanel, &itemCount, &lineCount);
		}
	void	GetItemStats( short item, short& startsAt, short& rowCount, short& columnCount, short& lineHeight) {
		Nlm_GetItemParams(fDocpanel,item, &startsAt, &rowCount, &columnCount, &lineHeight);
		}
	void	GetColStats( short item, short column, short& pixelPosition, short& pixelWidth, short& pixelInset, Justify& just) {
		char justc;
		Nlm_GetColParams(fDocpanel, item, column, &pixelPosition, &pixelWidth, &pixelInset, &justc);
		just= (Justify)justc;
		}
	Boolean GetScrollStats( short& offset, short& firstShown, short& firstLine) {
		return Nlm_GetScrlParams(fDocpanel, &offset, &firstShown, &firstLine);
		}
	void 	SetData(void* data);
	VoidPtr GetData(void) { 
		return Nlm_GetDocData(fDocpanel); 
		}

	virtual Boolean	IsVisible( short item, short& top, short& bottom, short& firstline) {
		return Nlm_ItemIsVisible(fDocpanel, item, &top, &bottom, &firstline);
		}
			
	void	Update( short from, short to) {
		Nlm_UpdateDocument(fDocpanel, from, to);
		}
	void	AdjustScroll(void) {
		Nlm_AdjustDocScroll(fDocpanel);
		}
	void	SetAutoAdjust( Boolean autoAdjust) {
		Nlm_SetDocAutoAdjust(fDocpanel, autoAdjust);
		}

};


class DTextDocPanel : public DDocPanel
{
public:
	
	DTextDocPanel(long id, DView* itsSuperior, short pixwidth, short pixheight) :
		DDocPanel( id, itsSuperior, pixwidth, pixheight)
		{}	

	void 	Append( char* text, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat, Nlm_FonT font = Nlm_programFont) 
		{
		Nlm_AppendText(fDocpanel, text, parFormat, colFormat, font);
		}
	void 	Replace( short item, char* text, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat, Nlm_FonT font = Nlm_programFont) 
		{
		Nlm_ReplaceText(fDocpanel, item, text, parFormat, colFormat, font);
		}
	void 	Insert( short item, char* text, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat, Nlm_FonT font = Nlm_programFont) 
		{
		Nlm_InsertText(fDocpanel, item, text, parFormat, colFormat, font);
		}
						
	void	ShowFile( char* filename, Nlm_FonT font = Nlm_programFont) {
		Nlm_DisplayFile(fDocpanel, filename, font);
		}
	void	ShowFileFancy( char* filename, Nlm_ParPtr parFormat, Nlm_ColPtr colFormat, Nlm_FonT font = Nlm_programFont, short tabStops = 4) {
		Nlm_DisplayFancy(fDocpanel, filename, parFormat, colFormat, font, tabStops);
		}
};




#if 0
// vibrant info on panel & slate
/***  SIMPLE UNIVERSAL DRAWING OBJECT  ***/

/*
*  A panel object is a rectangular box on a window that provides a universal
*  drawing environment. Panels translate click actions and drawing requests
*  to instance-specific procedures.  The SimplePanel and AutonomousPanel objects
*  are autonomous in that they create a single object within a window or group.
*  (The more general and complex slate/panel combinations are discussed in the
*  next section.)  SetPanelClick assigns click, drag, hold and release callbacks
*  to a given panel.  SimplePanels have only the minimum parameters to specify a
*  panel, and it is expected that the specific application callbacks for click,
*  draw, etc., will handle the entire behavior of the panel.
*
*  AutonomousPanels allow the creation of higher-level graphical objects that
*  can function like built-in Vibrant control objects while allowing much fancier
*  graphical display and manipulation.  The extra parameter specifies the number
*  of extra bytes to be placed on top of the panel instance data, and the data
*  can be accessed with SetPanelExtra and GetPanelExtra.  The reset callback is
*  called via the Reset class function, and should be used to free any instance-
*  specific allocated memory that may be pointed to in the extra data.  In order
*  to override certain class functions (e.g., to allow SetTitle to apply to a
*  particular autonomous panel), the classPtr function can point to a GphPrcs
*  array (in static or heap memory).  Any function pointer that is not NULL will
*  override the standard function.
*
*  The purpose of providing separate slates and panels (see next section) is to
*  allow multiple independent panels to be placed in the same scrolling unit.
*  The slate handles scrolling, and the individual panel children have their
*  own click and draw callbacks.  Slates are distinguished internally from
*  autonomous panels because their click through reset procedures are all NULL.
*/


/***  GENERALIZED SCROLLABLE DRAWING OBJECT  ***/

/*
*  A slate object is a rectangular box on a window that provides a universal
*  drawing environment to its panel object children. Panels translate click
*  actions and drawing requests to instance-specific procedures.  The parent
*  slate must be notified of individual row and column pixel offsets, and of
*  the corresponding row heights and column widths, in order for slate scroll
*  bars to work automatically.  The panel must be notified of the actual size
*  of its virtual rectangle.  The panel instance procedures should use the
*  GetOffset procedure to determine the pixel offset for purposes of drawing
*  and responding to clicks.  The VirtualSlate procedure allows specification
*  of virtual scroll positions before and after the real scroll positions in
*  a slate.  When the user drags the scroll bar thumb into a virtual area, the
*  virtual action callback should be used to load new data into the slate.
*  The GeneralSlate allows the specification of an alternative scroll bar
*  callback procedure for a scrolling slate, as well as extra instance space
*  and an overriding class function, in order to effectively have an autonomous
*  scrolling panel.  The default scroll bar procedure should be sufficient
*  for most situations.  Superimposed panels, or derivatives of panels, can be
*  used to build complicated behaviors using simpler objects.
*/

/*
*  The currently selected display object in the front window receives
*  the output that is sent to the terminal file.
	// Nlm_DisplaY is a form of Nlm_SlatE (see below)
*/
#endif


#endif

// DRichViewNu.h
// d.g.gilbert

#ifndef _DRICHVIEWNU_
#define _DRICHVIEWNU_

#include "Dvibrant.h"
#include "DView.h"
#include "DFile.h"
#include "DMethods.h"
#include "DPanel.h"
#include "DWindow.h"
#include "DApplication.h"

#include "DRichStyle.h"

class DList;
class DDialogText;

class DRichView : public DPanel, public DPrintHandler, public DSaveHandler
{
public:
	short 	fNumLines, fNumItems, fBarmax, fTabCount, fDocFormat;
	Boolean	fAutoAdjust, fIsVirtual, fDodraw;
	DList	*	fStyles; // of DRichStyle
	DList	*	fParags; // of DParagraph
	Nlm_RecT	fDrawr;
	Nlm_PoinT	fAtpt;
	DRichStyle * fCurrentStyle;
		// selection handling;
	Boolean 		fSelected, fSelInPict;
	short				fSelParag, fSelChar, fSelEndParag, fSelEndChar;
	Nlm_RecT		fSelRect; // for pict item
	DRichStyle	fSelStyle; // for pict only??
	DRichStyle* fMapStyle;

	static void  InitRichView();

	DRichView(long id, DView* itsSuperior, short pixwidth, short pixheight, 
						Boolean hasVscroll = true);
	virtual ~DRichView();

	virtual void Draw();
	virtual void Print();
	virtual void Save( DFile* f);

	virtual void ViewRect( Nlm_RecT& r);
	virtual char* DataToChar(char* ptr);  

	virtual void Append( void* data, short lines, DParagraph* parFormat,  
						DRichStyle* styles, short nstyles, Boolean docOwnsData = true);
	virtual void Append( char* text, DParagraph* parFormat, 
						DRichStyle* styles, short nstyles, Nlm_FonT aFont = NULL);
	
	virtual void Replace( short item, void* data, short lines, DParagraph* parFormat,  
						DRichStyle* styles, short nstyles, Boolean docOwnsData = true);
	virtual void 	Replace( short item, char* text, DParagraph* parFormat,  
						DRichStyle* styles, short nstyles, Nlm_FonT aFont = NULL);
	
	virtual void Insert( short item, void* data, short lines, DParagraph* parFormat, 
						DRichStyle* styles, short nstyles, Boolean docOwnsData = true);
	virtual void Insert( short item, char* text, DParagraph* parFormat, 
						DRichStyle* styles, short nstyles, Nlm_FonT aFont = NULL);
						
	virtual void Delete( short item);
	virtual void DeleteAll();
						
	virtual void	ShowFile( char* filename, Nlm_FonT font = Nlm_programFont);
	virtual void	ShowFileFancy( char* filename, DParagraph* parFormat, 
						DRichStyle* styles, short nstyles, short tabStops = 4);
	virtual void ShowDoc( char* doctext, ulong doclength = 0); 
			
	void  SetTabs( short theTabstops);
	virtual char* GetText( short item, short row);
	void 	MapPoint( Nlm_PoinT pt, short& item, short& row,  Nlm_RectPtr rct,
									char* wordAt, short maxword, DRichStyle* styleAt);

	void		GetItemStats( short item, short& startsAt, short& rowCount, short& lineHeight);
	Boolean GetScrollStats( short& offset, short& firstShown, short& firstLine);
	void 		SetData(void* data);
	VoidPtr GetData();
	virtual Boolean	IsVisible( short item, short& top, short& bottom, short& firstline);

	virtual void SizeToSuperview( DView* super, Boolean horiz, Boolean vert);
	virtual void Resize( DView* superview, Nlm_PoinT sizechange);
	virtual void SetSlateBorder( Boolean turnon);

	Boolean HasSelection() { return fSelected; }
	Boolean SelectionIsPict() { return fSelInPict; }
	virtual	void	MarkSelection( Nlm_RecT selrect);
	virtual void 	ClearSelection();
	virtual void  HiliteSelection(Boolean turnon);
	virtual Boolean GetSelection( Boolean& inpict,
							short& selparag, short& selchar, 
							short& selendparag, short& selendchar,
							Nlm_RecT& selrect, DRichStyle* selstyle);
	virtual char* GetSelectedText();
	virtual void  SetSelectedText( short startparag, short startchar, 
																 short endparag, short endchar);
	virtual Boolean HiliteText( short startparag, short startchar, 
															short endparag, short endchar, Boolean turnon); 
	virtual void SelectAll();

	virtual Boolean Doc2ViewOffset(short& xoff, short& yoff);
	virtual	void MapView2Doc( Nlm_RecT& selr);
	virtual void MapView2Doc( Nlm_PoinT& mpt);
	virtual	Boolean MapDoc2View( Nlm_RecT& selr);

	virtual Boolean FindURL( short& atparag, short& atchar, short& atlen,
														short startparag, short startchar); 
	virtual Boolean Find( char* match, short& atparag, short& atchar,
							short startparag = 0, short startchar = 0, Boolean caseSensitive = false); 
	virtual void InsertStyle( short atparag, short atchar, short atlength, DRichStyle* theStyle);
 
	virtual void Click( Nlm_PoinT mouse);
	virtual void DoubleClick( Nlm_PoinT mouse);
	virtual void SingleClick( Nlm_PoinT mouse);

	void	ScrollDoc( Nlm_BaR sb, short newval, short oldval);
	void	ResetDoc();	
	void	Update( short from, short to);
	void	AdjustScroll();
	void	SetAutoAdjust( Boolean autoAdjust);

	DParagraph* GetParaPtr( short item); 

	virtual void SetStyle( DRichStyle* theStyle);
	DRichStyle* GetStyle( short styleid);
	DRichStyle* GetStyleAtIndex( DParagraph* aParag, short styleindex);
	DRichStyle* GetStyleAtChar( DParagraph* aParag, short atchar);
	DRichStyle* GetStyleAtChar( short atparag, short atchar);
	short FindOrAddStyle( DRichStyle* theStyle);

	short GetNumRows( short item);
	short GetLineStart( short item);
	short GetLineHeight( short item);
	short GetItemNum( short desiredLine);
	short UpdateLineStarts( short first);
	void  GetDocStats( short& numParags, short& numLines) ;
	void 	ShowTextFile( char* filename, Nlm_FonT font);
	void  UpdateStyleHeight( DRichStyle* theStyle);

	short CharWidth(char ch);

protected:
	enum handleMode {
		kAppend,
		kInsert,
		kReplace
		};

	enum {
		kNoCharWidth = 255
		};

	unsigned char fCharWidths[256];

	void HandleAddInsRep( handleMode mode, short item, char* text, 
					DParagraph* parFormat, DRichStyle* styles, short nstyles, Nlm_FonT aFont);

	void  ZeroCharWidths();
	short SkipPastNewLine( char* text, short cnt);
	void  AppendParag( DParagraph* aParag);
	DParagraph* MakeNewParag( void* data, short lines, DParagraph* parFormat,  
					DRichStyle* styles, short nstyles, Boolean docOwnsData);
	void 	ClearCurrentStyle();
	virtual short DrawStyledItem( DParagraph* itemPtr, Nlm_RectPtr rpara, short item,
	                  		short frst, Boolean forPrinter, Nlm_PoinT* mapPoint);
	short StyleLineWidth( char* texts, short maxwid, DParagraph* itemPtr, short& istyle);
	short FitTextToWidth( char* texts, short maxwid, Boolean byPixels,
	                  		DParagraph* itemPtr, short& istyle);
	void 	FormatText( short item, DParagraph* itemPtr, Boolean byPixels);
	void 	CollectRange( short item, short& lowest, short& highest);
	void 	CacheAndFormat( short item, DParagraph* itemPtr, Nlm_RectPtr r);
	void 	CacheIfNever( short item, DParagraph* itemPtr, Nlm_RectPtr r);
	void 	FreeCachedItem( short item, short& lowest, short& highest);
	short VisLinesAbove( Nlm_RectPtr r, short item, short line, short& lowest, short& highest);
	short VisLinesBelow( Nlm_RectPtr r, short item, short line, short& lowest, short& highest);
	short PixelsBetween( Nlm_RectPtr r, short firstLine,
	                   		short lastLine, short& lowest, short& highest);
	void 	SaveTableItem( DParagraph* itemPtr, FILE *f, Boolean tabStops);
	void  UpdateItemHeights( DParagraph* aParag);
	void 	ParseText(DParagraph* itemPtr, char* text, Boolean byPixels, Nlm_RecT r);
};


class	DRichTextDoc : public DWindow
{
public:
	static Nlm_RecT gRichDocRect;
	DRichView* fRichView;
	Nlm_FonT	fFont;
	DDialogText * fFloatingEditText;  

	DRichTextDoc( long id = 0, Boolean makeRichView = true, Nlm_FonT itsFont = Nlm_programFont);
	virtual void OpenText( char* doctext, ulong doclength = 0);
	virtual void Open( DFile* aFile);
	virtual void Open( char* filename);
	virtual void Open();
	virtual void Close();
	virtual void ResizeWin();
	virtual Boolean IsMyAction(DTaskMaster* action); 
};

class DRichApplication : public DApplication
{	
public:
	DRichApplication();
	virtual void OpenDocument(DFile* aFile);
};



#endif

// DRichMoreStyle.h
// d.g.gilbert

#ifndef _DRICHMORESTYLE_
#define _DRICHMORESTYLE_

#include "DRichStyle.h"


struct PictLinkIndex {
	short			fKind;
  short			fLinkid;		// gopher/html link id 
	Nlm_RecT	fLoc;
};


class DPictStyle : public DStyleObject
{
public:
	// this is a quick fix to having DPictStyle & others subclassing DRichStyle
	// -- before the DRichStyle subclasses, must extensively revise DRS handling
	// so that statically allocated (fixed size) DRS classes are gone & a DRS::Clone
	// works  -- this is tied to problem of bad DObject::objSize for static alloc

	enum kinds {
		kPictKindStart = kDefaultObject,
		kPictOther,
	 	kPictMac,
	 	kPictWinMeta,
	 	kPictPMMeta,
	 	kPictDIBits,
	 	kPictWinBits,
	 	kPictGIF,
	 	kPictNetPic,
	 	kPictNetLink,
	 	kPictMapLink
		};

	PictLinkIndex*	fLinks;
	short		fNumLinks;

	DPictStyle();
	DPictStyle( short kind, void* data, ulong dsize, Boolean owndata= false);
  virtual ~DPictStyle();
	virtual void AddLink( short kind, short linkid, Nlm_RecT loc);
	virtual short GetLink( Nlm_PoinT atp, short& kind);
	virtual short GetLink( short index, short& kind, Nlm_RecT& loc);
  virtual void Draw(Nlm_RecT area);
	virtual Boolean HasNetPict();
  virtual Boolean CanDraw();
};




class DControlStyle : public DStyleObject
{
public:
	enum kinds {
		// ?? make these subclasses !?
		kControlKindStart = 23640,
		kButton,
		kSubmitButton, // GoChooseF, GoAskF
		kResetButton,
	 	kRadioButton,
	 	kCheckBox,
	 	kPopup,
	 	kPrompt,
	 	kEditText,
	 	kDialogScrollText,
	 	kPasswordText,
	 	kHidden,
		kForm,
		kControlKindEnd
		};

	DView * fControl;
	DView * fSuperview;
	char	* fDefaultText;
	char	* fVarname;  // variable name for html form
	long		fId;
	short		fWidth, fHeight; 
	short 	fSelected; // for kCheckBox, kRadio, kPopup
	Boolean fInstalled;
	
	DControlStyle(DView* super);
	DControlStyle(DView* super, short kind, void* data, ulong dsize, Boolean owndata= true);
  virtual ~DControlStyle();
  virtual void Draw(Nlm_RecT area);
	virtual void SetData( short kind, void* data, ulong dsize, Boolean owndata);
	virtual void ControlData( char* varName, char* defaultText);
  virtual Boolean CanDraw();
	virtual void Install();
	virtual const char* Varname();
	virtual const char* Value();
	virtual void getsize();
	virtual short Width();
	virtual short Height();
};


class DPromptCStyle : public DControlStyle {
public:
	DPromptCStyle(DView* super) : DControlStyle(super) { fKind= kPrompt; }
	virtual void Install();
};


class DEditTextCStyle : public DControlStyle {
public:
	DEditTextCStyle(DView* super) : DControlStyle(super) { fKind= kEditText; }
	virtual void Install();
	virtual const char* Value();
};

class DDialogTextCStyle : public DControlStyle {
public:
	DDialogTextCStyle(DView* super): DControlStyle(super) { fKind= kDialogScrollText; }
	virtual void Install();
	virtual const char* Value();
};

class DButtonCStyle : public DControlStyle {
public:
	DButtonCStyle(DView* super): DControlStyle(super)  { fKind= kButton; }
	virtual void Install();
};

class DDefaultButtonCStyle : public DControlStyle {
public:
	DDefaultButtonCStyle(DView* super): DControlStyle(super)  { fKind= kSubmitButton; }
	virtual void Install();
};

class DCheckboxCStyle : public DControlStyle {
public:
	DCheckboxCStyle(DView* super): DControlStyle(super) { fKind= kCheckBox; }
	virtual void Install();
	virtual const char* Value();
};

class DRadioCStyle : public DControlStyle {
public:
	DRadioCStyle(DView* super): DControlStyle(super) { fKind= kRadioButton; }
	virtual void Install();
	virtual const char* Value();
};

class DPopupCStyle : public DControlStyle {
public:
	DPopupCStyle(DView* super): DControlStyle(super) { fKind= kPopup; }
	virtual void Install();
	virtual const char* Value();
};

class DPasswordCStyle : public DControlStyle {
public:
	DPasswordCStyle(DView* super): DControlStyle(super) { fKind= kPasswordText; }
	virtual void Install();
	virtual const char* Value();
};

class DHiddenCStyle : public DControlStyle {
public:
	DHiddenCStyle(DView* super) : DControlStyle(super) { fKind= kHidden; }
	virtual void Install();
	virtual const char* Value();
};






#endif

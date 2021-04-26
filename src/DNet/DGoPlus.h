// DGopherPlus.h

#ifndef __DGOPHERPLUS__
#define __DGOPHERPLUS__



enum	GoplusValid {
	kGopherPlusNo = 0,
	kGopherPlusYes = 1,
	kGopherPlusDontKnow  = -1
	};
	
enum	GoaskItem {
	kGopherAskUnknown = 0,
	kGopherAskAsk = 1,
	kGopherAskAskF = 2,
	kGopherAskChoose = 3,
	kGopherAskChooseF = 4,
	kGopherAskNote = 5,
	kGopherAskAskP = 6,
	kGopherAskAskL = 7,
	kGopherAskSelect = 8
	};


class DGopherItemView : public DObject {
public:
	enum { kUnknownStatus= -1, kMinStatus= 0, kMaxStatus = 29999 };
	char*	fViewVal;
	long	fIkind, fNkind, fNrequest;
	long	fIhandler, fNhandler;
	long	fIlanguage, fNlanguage;
	long	fIdatasize, fNdatasize;	
	long  fViewStatus;
	
	DGopherItemView();
	DGopherItemView(const char* data);
	virtual ~DGopherItemView();
	
	virtual void Initialize(const char* line);
	virtual DObject* Clone();
	
	virtual const char*	ViewVal();
	virtual const char*	ViewRequest();
	virtual const char*	ViewStatus();
	virtual	const char*	Kind();
	virtual	const char* Handler();
	virtual	const char*	Language();
	virtual	const char*	DataSize();
};




class DGopherItemAsk : public DObject {
public:
	short	fKind,	fChoice;
	char * fQuestion, *	fAnswer, * fNametag, * fValuetag;
	 
	DGopherItemAsk();
	DGopherItemAsk(short kind, const char* question, const char* answer, 
									const char* nametag = NULL, const char* valuetag = NULL);
	DGopherItemAsk(const char* data, Boolean isHTMLform = false);
	virtual ~DGopherItemAsk();
	
	virtual void Initialize(short kind, const char* question, const char* answer, 
									const char* nametag = NULL, const char* valuetag = NULL);
	virtual void Initialize(const char* line, Boolean isHTMLform = false);
	virtual DObject* Clone();
	virtual void SetAnswer( const char* answer);
	virtual void ItemChoice( short choice);
};


class DGoMenuBlock : public DObject {
public:
	char*	fData;	// raw from server
	DGopherList *	fGolist;
	DGopher * 	fParent;
	
	DGoMenuBlock(DGopher* itsParent, const char* itsData);
	virtual ~DGoMenuBlock();
};

class DGoMenuItem : public DObject {
public:
	Boolean	fIsDefault;
	DGoMenuItem() {}
};

class DGoMenuRect : public DGoMenuItem {
public:
	Nlm_RecT	fRect;
	DGoMenuRect(const char* itsData);
};

class DGoMenuBytes : public DGoMenuItem {
public:
	long	fCharstart, fCharend;
	DGoMenuBytes(const char* itsData);
};

class DGoMenuLine : public DGoMenuItem {
public:
	long	fLinestart, fCharstart, fCharend;
	DGoMenuLine(const char* itsData);
};

class DGoMenuString : public DGoMenuItem {
public:
	char*	fFindstring;
	short	fFindcount;
	DGoMenuString(const char* itsData);
};






#endif

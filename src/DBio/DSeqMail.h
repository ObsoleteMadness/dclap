// DSeqMail.h
// d.g.gilbert


#ifndef _DSEQMAIL_
#define _DSEQMAIL_

#include <DSendMailDialog.h>


class DSequence;

class	DNCBIBlast : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380, cMatrix, cStrand, cFilter, 
			cOptions, cBaseKind, cProtMeth, cNucMeth, cProtLib, cNucLib };
	DSequence* fSeq;
	DPopupList *fNmeth, *fNlib, *fAmeth, *fAlib, *fMatrix, *fStrand, *fFilter;
	DEditText	*fDesc, *fAlign, *fExpect, *fCutoff;
	DCheckBox	*fHistogram;
	DCluster	*fBaseKind;
	DButton		*fOptions;
	DWindow		*fOptionWin;
	
	DNCBIBlast(long id, DTaskMaster* itsSuperior, DSequence* theSeq);
	virtual ~DNCBIBlast();
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual void Options();
	virtual Boolean IsMyAction(DTaskMaster* action);  
};


class	DNCBIFetch : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380, cDataLib};
	DPopupList *fLib;
	DEditText	*fStart, *fDocs, *fLines;
	DCheckBox	*fTitles;
	
	DNCBIFetch(long id, DTaskMaster* itsSuperior);
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};


class	DEMBLBlitz : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380, cBaseKind };
	DSequence* fSeq;
	DEditText	*fTitle, *fAlign, *fScores, *fIndel, *fPam;
	DCluster	*fBaseKind;
	
	DEMBLBlitz(long id, DTaskMaster* itsSuperior, DSequence* theSeq);
	virtual ~DEMBLBlitz();
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};

class	DEMBLFasta : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380, cBaseKind, cDataLib };
	DSequence* fSeq;
	DPopupList *fLib;
	DEditText	*fAlign, *fWord, *fScores, *fTitle;
	DCheckBox	*fBothStrands;
	DCluster	*fBaseKind;

	DEMBLFasta(long id, DTaskMaster* itsSuperior, DSequence* theSeq);
	virtual ~DEMBLFasta();
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};

class	DEMBLQuicks : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380 };
	DSequence* fSeq;
	DPopupList *fLib, *fBest;
	DEditText	*fMatch, *fWindow, *fStringency, *fTitle;
	DCheckBox	*fBothStrands, *fPerfect;

	DEMBLQuicks(long id, DTaskMaster* itsSuperior, DSequence* theSeq);
	virtual ~DEMBLQuicks();
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};

class	DEMBLFetch : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380};
	
	DEMBLFetch(long id, DTaskMaster* itsSuperior);
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};


class	DFHCRCfetch : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380};
	
	DFHCRCfetch(long id, DTaskMaster* itsSuperior);
	DView* InstallSubject(DView* super, char* subjStr);
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};


class	DFHCRCblocks : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380 };
	DSequence* fSeq;

	DFHCRCblocks(long id, DTaskMaster* itsSuperior, DSequence* theSeq);
	virtual ~DFHCRCblocks();
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};


class	DUWFGeneID : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380 };
	DSequence* fSeq;
	DEditText	*fCodeX, *fCodeY, *fFirstX, *fFirstY, *fLastX, *fLastY, *fAltX, *fAltY;
	DCheckBox	*fNetGene, *fGeneBlast, *fExonBlast, *fSmall;

	DUWFGeneID(long id, DTaskMaster* itsSuperior, DSequence* theSeq);
	virtual ~DUWFGeneID();
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};

class	DORNLGrail : public DSendMailDialog
{
public:
	static char* kTitle;
	static char* kAddress;
	static char* kHelp;
	enum cmds { cHelpButton = 380 };
	DSequence* fSeq;
	DCheckBox	*fGrail2, *fExon;

	DORNLGrail(long id, DTaskMaster* itsSuperior, DSequence* theSeq);
	virtual ~DORNLGrail();
	virtual char*  BuildMessage();
	virtual void Open();  
	virtual Boolean IsMyAction(DTaskMaster* action);  
};




#endif

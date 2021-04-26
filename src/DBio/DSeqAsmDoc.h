// DSeqAsmDoc.h
// d.g.gilbert

#ifndef _DSEQASMDOC_
#define _DSEQASMDOC_

#include "DSeqDoc.h"

class DAsmSequence;

class DSeqAsmDoc : public	DSeqDoc 
{
public:
	enum  { 
		kSeqAsmdoc = 610,
		};
		 
	static Boolean fgTestAutoseqFile;

 	//DSeqList	*	fSeqList;
	//DAsmView	*	fAsmView;
	//DAlnView	*	fAlnView;
	//DAlnIndex 	* fAlnIndex;
	DAsmSequence * fAsmSeq;
	short		fAutoseqFilesRead;   // track if all parts of fAsmSeq are read in
	long		fNtrace;
	Boolean	fHaveAsmseq;
		
	DSeqAsmDoc( long id, DSeqList* itsSeqList, char* name = NULL);
	virtual ~DSeqAsmDoc();

	static Boolean IsAutoseqFile(DFile* aFile);
	static void SetUpMenu(short menuId, DMenu*& aMenu); // !! can't do virtual statics !!
	static void NewSeqAsmDoc();
	static void GetSeqAsmGlobals();
	static void SaveSeqAsmGlobals();
	
	virtual void WriteTo (DFile* aFile = NULL);			// revise for iostreams
	virtual Boolean ReadFrom(DFile* aFile = NULL, Boolean append = true);	// revise for iostreams
	virtual void ReadTrace(DFile* aFile, short tracenum);	 
	virtual void ReadTraceStats(DFile* aFile, Boolean rewrite = false, 
								DFile* toFile = NULL);	 

	virtual void Open();
	virtual void Open(DFile* aFile);
	virtual void Close();
	virtual void Revert();
	virtual void Activate();
	virtual void Deactivate();
	virtual void ProcessTask(DTask* theTask);
	virtual Boolean IsMyTask(DTask* theTask);
	virtual Boolean DoMenuTask(long tasknum, DTask* theTask);
	virtual Boolean IsMyAction(DTaskMaster* action);
	virtual void FreeData();

	virtual void MakeGlobalsCurrent();
	virtual void SortView( DSeqList::Sorts sortorder);
	
	virtual void AddSeqToList(DSequence* item);
	virtual void AddNewSeqToList();
	virtual void OpenSeqedWindow(DSequence* aSeq);
	 					
	virtual short SelectionToFile(Boolean AllatNoSelection, 
													char* aFileName, short seqFormat); //return # written 
	virtual void GetSelection( long selectFlags, 
														DSeqList*& aSeqList, long& start, long& nbases);
	virtual void MakeSeqPrint(Boolean doREMap);
	virtual void Print(); 


protected:
	virtual void AddViews(); 
	virtual void AddTopViews(DView* super);
	virtual void AddModePopup(DView* super);
	virtual void AddAlnIndex(DView* super, short width, short height);
	virtual void AddAlnView(DView* super, short width, short height);

};



#endif 

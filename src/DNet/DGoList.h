// DGopherList.h  
// Internet Gopher client
// by D. Gilbert, Mar 1992
// dclap version jan 94

#ifndef __DGOPHERLIST__
#define __DGOPHERLIST__

#include <DList.h>

class		DGopher;
class		DView;
class   DFile;


class DGopherList : public DList {
public:
	enum Sorts {
		kSortByItem = 0,
		kSortByName = 1,
		kSortBySize = 2,
		kSortByKind = 3,
		kSortByDate = 4,
		kSortByHost = 5 
		};
	
	DGopher			*fParentMenu, *fNewGopher;
	DView				*fStatusLine;
	Boolean			fListUnknowns;
	Sorts				fSortOrder;
	
	static Boolean IsGopherLine( char *line);
	static short CompareGophers( void* gopher1, void* gopher2);
	static DGopher* NewGopher(char itemType, char* info, char* plusinfo, 
														short& status, DGopherList* ownerList);
	static DGopher* CopyToNewKind(char itemType, DGopher* srcGopher, 
														DGopherList* ownerList, Boolean copyMacStuff=false);
	static DGopher* GopherFromURL( char* url, long urlsize, DGopherList* itsList, 
														Boolean doInsertLast = true, char itemType = 0);
	static DGopher* LocalGopher( DFile* theFile);
	static DGopher* LocalGopher( char* name);
	static short  LocalFolder2GopherList( DFile* outFile, char* folderpath);

	DGopherList( DGopher *parentMenu = NULL);
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);
	
	DGopher* GopherAt(long index) { return (DGopher*) At(index); }
	
	virtual void SetStatus(DView *statusline);
	virtual Boolean MakeNewGopher( char *pInfo, char *eInfo, 
																	char *pPlus, char *ePlus, short status);
	virtual void ReadLinks( char* linkptr);
	virtual void ReadLinks( char *linkptr, long len);
	virtual char* WriteLinks(short indent = 0);
	virtual char* WriteLinksForServer();
	virtual char* WriteLinksForDisplay( Boolean showDate=true, Boolean showSize=true,
							Boolean showKind=true, Boolean showPath=false, Boolean showHost=false, 
							Boolean showPort=false, Boolean showAdmin=false);
	virtual Boolean SortList(Sorts sortorder = kSortByItem);
	virtual void FreeAllObjects();
};



#endif

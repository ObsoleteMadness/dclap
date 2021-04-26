// DIconLib.h

#ifndef _DICONLIB_
#define _DICONLIB_

#include "ncbi.h"
#include "DObject.h"
#include "DList.h"

class DFile;

struct IconStore {
	short id;
	short smalli;
	short	width;
	short	height;
	short	bitmapsize;
	unsigned short bitmap;
	};

class DIcon : public DObject
{
public:
	long	fId;
	Boolean fSmall; // mac #icn versus #ICN
	short	fWidth;
	short	fHeight;
	long	fBitmapSize;
	char*	fBitmap;

	DIcon();
	DIcon( long id, short width, short height, const char* bitmap, long bitmapsize);
	DIcon( IconStore* store);
	virtual ~DIcon();
	virtual void Draw(Nlm_RecT area);
	virtual void Install( short width, short height, const char* bitmap, long bitmapsize);
	virtual void WriteCode( DFile* iconf);
};


class DIconList : public DList
{
public:
	DIconList();
	virtual DIcon* IconAt(long index) { return (DIcon*) At(index); }
	virtual DIcon* IconById(long id);
	virtual Boolean Read(char* filename);
	virtual Boolean ReadXbitmapFormat(char* filename); 
	virtual Boolean ReadMacRezFormat(char* filename); 
	virtual void ReadAppIcons();
	virtual void WriteAsCode( char* filename);
	virtual void StoreFromCode( unsigned short* ilist, short nlist = 0);
};


extern DIconList* gIconList;


  
#endif

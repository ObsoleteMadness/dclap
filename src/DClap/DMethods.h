// DMethods.h
// testing methods/behaviors to associate w/ some views
// e.g., printing & saving to disk

#ifndef _DMETHODS_
#define _DMETHODS_

class DFile;

class DSaveHandler 
{
	short fDirty;
public:
	DSaveHandler()   { fDirty= 0; }
	virtual short IsDirty()  { return fDirty; }
	virtual void  Dirty() 	 { fDirty++; }
	virtual void  UnDirty()  { if (fDirty) fDirty--; }
	virtual void  NotDirty() { fDirty= 0; }
	virtual void Save(DFile* f) {}
	virtual short DirtySaveCancelled(char* doctitle=NULL);
};

class DPrintHandler 
{
public:
	virtual void Print() {}
};

#endif

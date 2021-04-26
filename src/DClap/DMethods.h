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
	short IsDirty()  { return fDirty; }
	void  Dirty() 	 { fDirty++; }
	void  UnDirty()  { if (fDirty) fDirty--; }
	void  NotDirty() { fDirty= 0; }
	virtual void Save(DFile* f) {}
	virtual short DirtySaveCancelled(char* doctitle=NULL);
};

class DPrintHandler 
{
public:
	virtual void Print() {}
};

#endif

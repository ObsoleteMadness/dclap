// DObject.h
// d.g.gilbert

#ifndef _DOBJECT_
#define  _DOBJECT_

#include "Dvibrant.h"

class DObject {
	ulong	  fClassSize;
	short		fOwners;
	char	* fClassName;
protected:
	DObject *	BasicClone( void);
	void	  setClassSize( ulong classSize)  {  fClassSize = classSize; }
	void		SetClassName( char	*className)  { fClassName= className; }
	void		SetClass( char	*className, ulong classSize)  { fClassName= className; fClassSize = classSize; }
public:										
	DObject();
	void*		operator new(size_t objSize); 
	void		 operator delete(void* objStorage);
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);
	virtual	void newOwner(void) { fOwners++; }	// allow multiple owners/deleters of an object
	ulong	  GetClassSize(void)  { return fClassSize; }
	short		GetOwnerCount(void) { return fOwners; }
	char*		GetClassName(void)  { return fClassName; }
	virtual DObject	* Clone(void);
};

#endif

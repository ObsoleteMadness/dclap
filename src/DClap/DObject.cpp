// DObject.cp

#include "Dvibrant.h"
#include "DObject.h"



DObject::DObject() 
{ 
  fOwners= 1; 
  fClassName= "DObject";
  //fClassSize= sizeof(DObject); // this is bad news -- 
  		// !! DObjects created statically don't know their fClassSize !?
  		// at least for Clone this causes problems.
}

void killit( DObject* victim)
{
	delete victim;
}

Boolean DObject::suicide(void) 
{ 
	if (--fOwners < 1) { 
		killit(this); //delete this; 
		return true; 
		}
	else 
		return false;
}

Boolean DObject::suicide(short ownercount)
{
	if ((fOwners= ownercount) < 1) {
		killit(this); //delete this; 
		return true; 
		}
	else 
		return false;
}

 
void*	DObject::operator new( size_t objSize) 
{	
			// assume c++ knows how to new an object ...
	void* theObject = ::operator new( objSize); // = Nlm_MemNew( objSize); 
			// but we still want to remember its size!
	((DObject*) theObject)->fClassSize= objSize;
	return  theObject;
} 

void DObject::operator delete( void* objStorage)
{
			// assume c++ knows how to delete an object ...
 	::operator delete(objStorage);  // Nlm_MemFree(objStorage);  
}
 


DObject*	DObject::Clone(void)
{
	return this->BasicClone();
}

DObject*	DObject::BasicClone(void)
{
#if 0
		// this is okay if we use Nlm_Mem for new/delete
	DObject* result= (DObject *) Nlm_MemDup( this, fClassSize);
#else
		// this is best if we use default new/delete
	DObject* result= (DObject *) ::operator new(fClassSize); 
	Nlm_MemMove(result, this, fClassSize);
#endif
  result->fOwners= 1; // reset this as if it were a new()
	return  result;
}  




// DList.h
// d.g.gilbert

#ifndef _DLIST_
#define _DLIST_

#include "Dvibrant.h"
#include "DObject.h"



enum CompareConsts { 
	kEmptyIndex = -1,
	kEmptySize = 0,
	kMaxArrayIndex = -2 
	};

typedef	short (*CompareFunc)( void*, void*);	 // compare two list/array Items 



class DArray : public DObject
{	
public:
	DArray(short itemSize = sizeof(long), long initialSize = 5);
	virtual ~DArray();
	virtual DObject* Clone();

	virtual long GetSize() { return fSize; }  
	virtual Boolean IsEmpty(void) { return (fSize == kEmptySize); }  
	virtual void SetArraySize(long theSize);	
	
	virtual void GetItemsAt(long index, void* itemPtr, long count);
	virtual void InsertItemsBefore(long index, void* itemPtr, long count);
	virtual void ReplaceItemsAt(long index, void* itemPtr, long count);
	virtual void DeleteItemsAt(long index, long count);
	virtual void DeleteAll(void);

	virtual long Search(CompareFunc compare, void *itemPtr);

protected:
	long 	fSize;		 							// count of items in array
	short fItemSize;				 			// byte size of items, power of 2
	short fItemSizeShift; 				// power of 2 for size << shift
	long 	fArrayIncrement;	 			// amount to increase array byte size by when needed
	long	fArraySize;	 						// current allocated byte size of array
	void* fArrayStore;						// storage space for array items

	virtual void* ComputeAddress(long index);
};



class DList : public DArray	 
{		
public:
	enum ListFlags { 
		kDontOwnObjects = 0, kOwnObjects = 2, 
		kDontDeleteObjects = 0, kDeleteObjects= 4 
		};
	Boolean	fOwnObjects;
	Boolean	fDeleteObjects;
	CompareFunc	fCompareFunc;
	
	DList(CompareFunc itsComparer = NULL, short options = kDontOwnObjects + kDontDeleteObjects);		
	virtual ~DList(); 

	virtual DObject* At(long index);
	DObject* operator[](long index) { return At(index); }
	virtual DObject* First(void);
	virtual DObject* Last(void);

	virtual void AtPut(long index, DObject* newItem);
	virtual void InsertBefore(long index, DObject* item);
	virtual void InsertFirst(DObject* item);
	virtual void InsertLast(DObject* item);
	virtual void InsertInOrder(DObject* item);				// insert in sort order
	
	virtual long  GetEqualItemNo(DObject* item);  		// uses fCompareFunc(item, indexobj)
	virtual long  GetIdentityItemNo(DObject* item);		// uses long(item) == long(indexobj)

	virtual void Sort(void);													// uses fCompareFunc(obj1,obj2)
	virtual void SortBy(CompareFunc CompareItems);

	virtual void AtDelete(long index);
	virtual void Delete(DObject* item);
	
	virtual void Push(DObject* item); 	// last-in-first-out stack
	virtual DObject* Pop(void); 				// last-in-first-out stack
	
	virtual void Queue(DObject* item);  // first-in-first-out queue
	virtual DObject* Dequeue(void);   	// first-in-first-out queue

	virtual void FreeAllObjects(void); 
		
private:
	void QuickSort(long beginIndex, long endIndex, CompareFunc CompareItems);
	long QSPartition(long beginIndex, long endIndex, CompareFunc CompareItems);
	long QSRandomPartition(long beginIndex, long endIndex, CompareFunc CompareItems);
};





DList*	FreeListIfObject(DList* aList);



#endif

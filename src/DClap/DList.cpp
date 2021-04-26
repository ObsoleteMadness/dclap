// DList.cp
// This code owes it's structure in large part to Apple's MacApp UList unit
// however, d.gilbert has substatially rewritten it.


#include <ncbi.h>
#include <dgg.h>
#include "DList.h"


static const short kArrayIncrement = 6;		 

typedef	DObject* DObjectPtr;


// class DArray


DArray::DArray(short itemSize, long initialSize)
{
  SetClass("DArray",sizeof(DArray));
	fArraySize = kEmptySize;
	fArrayIncrement = kArrayIncrement;
	fSize = kEmptySize;
	fArrayStore = NULL;

	fItemSize = itemSize;
	fItemSizeShift = 0;
	if (odd(itemSize)) ++itemSize; // make sure we have even size for shifting
	while (((itemSize - 1) >> fItemSizeShift) > 0) ++fItemSizeShift;
	this->SetArraySize(initialSize);
} 

DArray::~DArray()
{
	fArrayStore= MemFree(fArrayStore);
}  

void DArray::DeleteAll()
{
	if (fSize > kEmptySize) this->DeleteItemsAt(0, fSize); //1,fSize
} 

DObject* DArray::Clone()
{
	DArray* result = (DArray*) DObject::Clone();
	result->fArrayStore= MemDup(fArrayStore,  (ulong) (fArraySize << fItemSizeShift));
	return result;
}

void* DArray::ComputeAddress(long index)
{
	return (void*) ((char*)fArrayStore + (index << fItemSizeShift)); 
}  



void DArray::DeleteItemsAt(long index, long count)
{
	VoidPtr indexPtr, nextItemPtr, lastItemPtr;
	long countBytes;

	countBytes = count << fItemSizeShift;
	indexPtr = this->ComputeAddress(index);
	nextItemPtr = this->ComputeAddress(index + count);
	lastItemPtr = this->ComputeAddress(fSize); //1based: + 1);

	if (nextItemPtr < lastItemPtr)		
		MemMove(indexPtr, nextItemPtr, (long)lastItemPtr - (long)nextItemPtr);

	this->SetArraySize(fSize - count);		
	fSize -= count;
}   




void DArray::GetItemsAt(long index, void* itemPtr, long count)
{
	if (count > kEmptySize)
		MemMove( itemPtr, this->ComputeAddress(index), 
			((count - 1) << fItemSizeShift) + fItemSize);		 
}  


void DArray::ReplaceItemsAt(long index, void* itemPtr, long count)
{
	VoidPtr indexPtr= this->ComputeAddress(index);
	long countBytes= count << fItemSizeShift;
	MemMove( indexPtr, itemPtr, countBytes);
} 


void DArray::InsertItemsBefore(long index, void* itemPtr, long count)
{
	VoidPtr indexPtr, nextIndexPtr, lastItemPtr;
	long countBytes;

	this->SetArraySize(fSize + count);				 
	indexPtr = this->ComputeAddress(index);
	nextIndexPtr = this->ComputeAddress(index + count);
	lastItemPtr = this->ComputeAddress(fSize);	//1based: fSize + 1
	countBytes = count << fItemSizeShift;

	if (index < fSize)		//1based: <=			 
		MemMove( nextIndexPtr, indexPtr, (long)lastItemPtr - (long)indexPtr);

	if ((countBytes == sizeof(long)) && !odd((long)itemPtr) && !odd((long)indexPtr))
		*((long*)indexPtr) = *((long*)itemPtr);		// shortcut for longs 
	else
		MemMove(indexPtr, itemPtr, countBytes); 

	fSize += count;
}  




void DArray::SetArraySize(long theSize)
{
	long newSize;
	
	if ((theSize > fArraySize) || (fArraySize - theSize >= fArrayIncrement)) {
		if (fArrayIncrement)
			newSize = (theSize + fArrayIncrement) 
								- (theSize + fArrayIncrement) % fArrayIncrement;
		else
			newSize = theSize;
		if (newSize != fArraySize)  {
			ulong chunk= (ulong) (newSize << fItemSizeShift);
			if (fArrayStore == NULL) fArrayStore = MemNew( chunk);
			else fArrayStore= MemMore(fArrayStore, chunk);
			}
		fArraySize = newSize;
	  }
} 


long DArray::Search(CompareFunc compare, void *itemPtr)
{
	register char*  itemAt = (char*) fArrayStore;
	long 	itemsize= 1 << fItemSizeShift;
		
		// speed this up w/ a bisection method == start in middle & bounce around ??
		// NO -- can't assume this array is SORTED !!
#if 0
	long lo, hi, mid;
	short cmp;
	lo= 0; 
	hi= fSize;
	do {
		mid= (hi+lo) >> 1;  // div 2;
		itemAt= (char*) fArrayStore + mid * itemsize;
		cmp= (*compare)( itemPtr, itemAt);
		if (cmp == 0) return mid;
		else if (cmp > 0) lo= mid;
		else hi= mid;
	} while (hi > lo+1);
	
	if (mid != hi) {
		itemAt= (char*) fArrayStore + hi * itemsize;
		cmp= (*compare)( itemPtr, itemAt);
		if (cmp == 0) return hi;
		}
	else if (mid != lo) {
		itemAt= (char*) fArrayStore + lo * itemsize;
		cmp= (*compare)( itemPtr, itemAt);
		if (cmp == 0) return lo;
		}
	return kEmptyIndex;
#else
	register long i;
	for (i = 0; i < fSize; i++) {
		if ( (*compare)( itemPtr, itemAt) == 0) 
			return i;
		itemAt += itemsize;
		}
	return kEmptyIndex;
#endif
}	 





 

// class DList

// default fCompareFunc
short DefaultCompare(void* item1, void* item2)
{
	if ((unsigned long) item1 > (unsigned long) item2)
		return 1;
	else if ((unsigned long) item1 < (unsigned long) item2)
		return -1;
	else
		return 0;
} 


DList::DList(CompareFunc itsComparer, short options): 
	DArray(sizeof(DObject*), kEmptySize)
{
  SetClass("DList",sizeof(DList));
	if (itsComparer) fCompareFunc= itsComparer;
	else fCompareFunc= ::DefaultCompare;
	fOwnObjects= options & kOwnObjects;
	fDeleteObjects = options & kDeleteObjects;		
} 

DList::~DList()
{
	if (fDeleteObjects) this->FreeAllObjects();
} 

		
DObject* DList::At(long index)
{
	//return (*this)[index];
	if (index<0 || index>=fSize) return NULL;
	DObject** vp= (DObject**) ((char*)fArrayStore + (index << fItemSizeShift)); 
	if (vp) return *vp;
	else return NULL;
} 



void DList::FreeAllObjects()
{
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) {
		DObject* obj= At(i);
		if (obj) {
		  if (obj->GetOwnerCount() <= 1) delete obj; 
		  	// ^^^^ BIG MEM LEAK !! THIS delete DOESN"T CALL DESTRUCTOR !
		  else obj->suicide(); // this alone doens't call obj destructors !!
		  }
		}
	this->DeleteAll();
} 


long DList::GetEqualItemNo(DObject* item)
{
	long index;

	if (item == NULL) return kEmptyIndex;
	index = this->Search( fCompareFunc, &item);
	return index;  // == kEmptyIndex if no match
} 



#if 1
// choose your sort

void DList::QuickSort(long beginIndex, long endIndex, CompareFunc CompareItems)
// recursive qs
{
	long i, j;
 	DObjectPtr	iobj, jobj, endobj;
 	
	if (beginIndex < endIndex) {
		i = beginIndex; 
		j = endIndex;  
		endobj= this->At(endIndex);
		do {
			iobj= this->At(i);			
			while (i < j && CompareItems( iobj, endobj) <= 0) 
				iobj= this->At(++i);			
			jobj= this->At(j);
			while (j > i && CompareItems( jobj, endobj) >= 0) 
				jobj= this->At(--j);
			if (i<j) {	// swap
				this->ReplaceItemsAt(i, &jobj, 1); //was &	 
				this->ReplaceItemsAt(j, &iobj, 1); //was &	 
				}
		} while (i < j);
        
	 	this->ReplaceItemsAt(i, &endobj, 1);  
		this->ReplaceItemsAt(endIndex, &iobj, 1);  
		if (i - beginIndex < endIndex - i) {
			QuickSort( beginIndex, i-1, CompareItems);
	  	QuickSort( i+1, endIndex, CompareItems);
	  	}
		else {
			QuickSort( i+1, endIndex, CompareItems);
	 		QuickSort( beginIndex, i-1, CompareItems);
	 		}
		}
}

long DList::QSPartition(long beginIndex, long endIndex,	 
								   CompareFunc CompareItems)
{
	return 0;
}

long DList::QSRandomPartition(long beginIndex, long endIndex,	 
								   CompareFunc CompareItems)
{
	return 0;
}


#else
// this was Apple's algorithm

void DList::QuickSort(long beginIndex, long endIndex, CompareFunc CompareItems)
{
	if (beginIndex < endIndex) {
		long left = this->QSRandomPartition(beginIndex, endIndex, CompareItems);
		this->QuickSort(beginIndex, left, CompareItems);
		this->QuickSort(left+1, endIndex, CompareItems);	 
	}
} 


long DList::QSPartition(long beginIndex, long endIndex, CompareFunc CompareItems)
{
	if (beginIndex >= endIndex)
		return endIndex;
	{
		DObjectPtr pivot = this->At(beginIndex);				// x
		long i = beginIndex - 1;	// p - 1
		long j = endIndex + 1;	// r + 1
		while (true)
		{
			DObjectPtr secondKey;
			do
			{
				--j;
				secondKey = this->At(j);	// A[j]
			} while (CompareItems( pivot, secondKey) <= -1);
			do
			{
				++i;
				secondKey = this->At(i);	// A[i]
			} while (CompareItems( pivot, secondKey) >= 1);
			if (i < j)
			{	// Swap the Items
				DObjectPtr leftObject = this->At(i);
				DObjectPtr rightObject = this->At(j);
				this->ReplaceItemsAt(i, &rightObject, 1); // was &
				this->ReplaceItemsAt(j, &leftObject, 1); //was &
			}
			else
				return j;
		}
	}
} 


long RandomArrayIndex(long beginIndex, long endIndex)
{
	if (beginIndex == endIndex) return beginIndex;
	//long randomValue = rand() % labs(endIndex - beginIndex);// labs() not found by g++
	long randomValue = endIndex - beginIndex;
	if (randomValue < 0) randomValue = -randomValue;
	randomValue= rand() % randomValue;
	return beginIndex + randomValue;
}

long DList::QSRandomPartition(long beginIndex, long endIndex, CompareFunc CompareItems )
{
	long i = RandomArrayIndex(beginIndex, endIndex);
	
	// exchange
	DObjectPtr beginIndexObject = this->At(beginIndex);
	DObjectPtr ithObject = this->At(i);
	this->ReplaceItemsAt(beginIndex, &ithObject, 1); //was &	// A[beginIndex] = A[i]
	this->ReplaceItemsAt(i, &beginIndexObject, 1); //was &	// A[i] = A[beginIndex]

	return this->QSPartition(beginIndex, endIndex, CompareItems);
} 

#endif


void DList::Sort()
{
	if (this->GetSize() > 0)
		this->QuickSort(0, fSize-1, fCompareFunc); // was 1,fsize
} 

void DList::SortBy(CompareFunc CompareItems)
{
	if (this->GetSize() > 0)
		this->QuickSort(0, fSize-1, CompareItems); // was 1,fsize
} 


void DList::AtDelete(long index)
{
	if (fDeleteObjects)  At(index)->suicide(); //delete At(index); 
		// ^^ this will make a mess of Pop() and Dequeue() method results, who call here
	this->DeleteItemsAt(index, 1);
}

void DList::Delete(DObject* item)
{
	long index;
	if (item == NULL) return;
	index = this->GetIdentityItemNo(item);
	if (index != kEmptyIndex) this->AtDelete(index);
} 

 
DObject* DList::First()
{
	if (fSize <= kEmptySize)
		return NULL;
	else
		return this->At(0); //1based: 1
} 

DObject* DList::Last()
{
	if (fSize <= kEmptySize)
		return NULL;
	else
		return this->At(fSize-1);
}  


long DList::GetIdentityItemNo(DObject* item)
{
	if (item == NULL) return kEmptyIndex;
	long i, n= this->GetSize();
	for (i= 0; i<n; i++) if (At(i) == item) return i;
	return kEmptyIndex; 
} 


void DList::AtPut(long index, DObject* newItem)
{
	if (fOwnObjects && newItem) newItem->newOwner();
	this->ReplaceItemsAt( index, &newItem, 1); //was &newItem
} 


void DList::InsertBefore(long index, DObject* item)
{
	if (fOwnObjects && item) item->newOwner();
	this->InsertItemsBefore(index, &item, 1); // was &item !!!
}


void DList::InsertInOrder(DObject* item)
{
	long index= Search( fCompareFunc, &item);
	if (index > kEmptyIndex)
		InsertBefore( index, item);
	else
		InsertBefore( fSize, item);
} 

void DList::InsertFirst(DObject* item)
{
	this->InsertBefore(0, item); //1
} 

void DList::InsertLast(DObject* item)
{
	this->InsertBefore(fSize, item); // + 1
} 


void DList::Push(DObject* item)
{
	this->InsertLast(item);
} 

DObject* DList::Pop()
{
	DObject* topOfStack;

	if (fSize <= kEmptySize)
		topOfStack = NULL;
	else {
		topOfStack = this->At(fSize-1);
		//this->AtDelete(fSize-1);
		this->DeleteItemsAt(fSize-1, 1); 	// this bypasses fDeleteObjects check ...
		}
	return topOfStack;
} 



void DList::Queue(DObject* item)
{
	this->InsertLast(item);
} 

DObject* DList::Dequeue()
{
	DObject* beginningOfStack;

	if (fSize <= kEmptySize)
		beginningOfStack = NULL;
	else {
		beginningOfStack = this->At(0);	//1based: 1
		//this->AtDelete(0); 				//1based: 1
		this->DeleteItemsAt(0, 1); 	// this bypasses fDeleteObjects check ...
		}
	return beginningOfStack;
} 





DList*	FreeListIfObject(DList* aList)
{
	if (aList) {
		//aList->FreeAllObjects(); // destructor now calls FreeAllObjects()
		delete aList; 
		}
	return NULL;
}

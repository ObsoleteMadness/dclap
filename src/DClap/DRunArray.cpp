// DRunArray.cpp
// d.g.gilbert


// DRunArray.h
// d.g.gilbert

#ifndef _DRUNARRAY_
#define _DRUNARRAY_

class DRunArray 
{
public:
		enum { kIndexBad = 0, kIndexFirst	= 1, kIndexLast = 0x7FFFFFFF };
						  
		DRunArray();
		virtual ~DRunArray( long inItemSize);
		
		//virtual Nlm_Boolean selected(long index);
		//virtual void select(long index);
		//virtual void select(long start, long finish);
		
		//virtual void set(long index, long value);
		//virtual void set(long start, long finish, long value);
		//virtual long get(long index);
		
		virtual void* getItemPtr( long itemIndex) const;
		virtual void InsertItemsAt( long inCount, long inAtIndex,
													const void *inItem, long inItemSize);
		virtual void RemoveItemsAt( long inCount, long inAtIndex);
		virtual void AssignItemsAt( long inCount, long inAtIndex, 
													const void *inValue, long inItemSize);
		Boolean goodIndex(long index) 
			{ 
			return (index >= kIndexFirst && index <= kIndexLast); 
			} 
		
protected:
		long * fRuns;
		char * fValues;
		long	 fNruns, fNitems, fValueSize;
		struct RunRec { long	start, end; };

		long 	getRunIndex( long itemIndex);
		void* getRunItemPtr( long runIndex) const 
			{ 
			return (fValues + fValueSize * runIndex); 
			}
		long getRunStart( long runIndex) const
			{
			return (((RunRec*)(fRuns))[runIndex].start);
			}

		void InsertRun( long inRunIndex, long inCount, const void *inItem);
		void SplitRun( long inRunIndex, long inItemIndex, long inCount, const void *inItem);
		void ExtendRun( long inRunIndex, long inCount);
};


#endif /* _DRUNARRAY_ */



#include "DRunArray.h"
#include <ncbi.h>
#include <dgg.h>


DRunArray::DRunArray( long inItemSize) :
	fRuns(NULL), fValues(NULL),
	fNruns(0), fNitems(0), fValueSize(inItemSize)
{
}



DRunArray::~DRunArray()
{
	MemFree(fRuns);
	MemFree(fValues);
}


//	Insert new items into a RunArray at the specified index

void DRunArray::InsertItemsAt( long inCount, long inAtIndex,
						const void *inItem, long inItemSize)
{
	if (inAtIndex > fNitems) inAtIndex = fNitems + 1;  
	else if (inAtIndex < 1) inAtIndex = 1;
	
	if (fNitems == 0)  
		InsertRun(0, inCount, inItem);
		
			// append to array
	else if (inAtIndex > fNitems) {
		if (Nlm_MemCmp( inItem, getRunItemPtr(fNruns-1), fValueSize) == 0) 
			ExtendRun(fNruns - 1, inCount);  
		else 
			InsertRun(fNruns, inCount, inItem);
		} 
		
			// insert at start of array
	else if (inAtIndex == 1) {	 
		if (Nlm_MemCmp(inItem, getRunItemPtr(0), fValueSize) == 0)  
			ExtendRun(0, inCount);  
		else 
			InsertRun(0, inCount, inItem);
		} 
		
		// insert in middle
	else {	 
		long	withinRun = getRunIndex(inAtIndex);
		if (Nlm_MemCmp(inItem, getRunItemPtr(withinRun), fValueSize) == 0)  
			ExtendRun(withinRun, inCount); 
		else if (inAtIndex == getRunStart(withinRun)) {
			if (Nlm_MemCmp(inItem, getRunItemPtr(withinRun - 1), fValueSize) == 0) 
				ExtendRun(withinRun - 1, inCount); 
			else  
				InsertRun(withinRun, inCount, inItem);			
			} 
		else 					// Inserting into the middle of  a Run
			SplitRun(withinRun, inAtIndex, inCount, inItem);
		}
	fNitems += inCount;
}


//	Remove items from the Array starting at the specified index

void DRunArray::RemoveItemsAt( long inCount, long inAtIndex)
{
	if ( goodIndex(inAtIndex) && (inCount > 0)) {
	
		if ((inAtIndex + inCount) > fNitems) {
										// remove from tail
			if (inAtIndex == 1) {
										// delete all
				fValues= (char*) ::MemMore( fValues, 0);
				fRuns= (long*) ::MemMore( fRuns, 0);
				fNruns= 0;
				} 
			else {
										// Last Run will be the one containing
										//   the item before the one being removed
				long	lastRun = getRunIndex(inAtIndex - 1);
				fValues= (char*) ::MemMore( fValues, (lastRun + 1) * fValueSize);
				fRuns= (long*) ::MemMore( fRuns, (lastRun + 1) * sizeof(RunRec));
				((RunRec*)(fRuns))[lastRun].end = inAtIndex - 1;
				fNruns = lastRun + 1;
				}
			fNitems = inAtIndex - 1;
			} 
		else if (inAtIndex == 1) {	 
									// remove from top
										// First Run will be the one containing
										//   the item after the deleted ones
			long	firstRun = getRunIndex(inCount + 1);
			if (firstRun > 0) {
										// Remove Runs from start up to, but
										//   not including, firstRun
										// Shift down data for remaining items
				::MemMove( getRunItemPtr(firstRun), fValues,
									(fNruns - firstRun) * fValueSize);
										// Shift down remaining Run records
				::MemMove( fRuns + firstRun * sizeof(RunRec),
									fRuns, (fNruns - firstRun) * sizeof(RunRec));
										// Reduce sizes of data and runs handles to new number of runs
				fNruns -= firstRun;	
				fValues= (char*) ::MemMore( fValues, fNruns * fValueSize);
				fRuns= (long*) ::MemMore( fRuns, fNruns * sizeof(RunRec));
				}
			
										// Adjust indexes for first Run
			RunRec	*run = (RunRec*) fRuns;
			run[0].start = 1;
			run[0].end -= inCount;
			
										// Adjust indexes for the second and  succeeding Runs
			for (long i = 1; i < fNruns; i++) {
				run[i].start -= inCount;
				run[i].end -= inCount;
				}			
			fNitems -= inCount;
			} 
		else {						 
						// remove from middle
										// The Runs between, but not  including, oneEnd and twoStart  will be deleted
			long oneEnd = getRunIndex(inAtIndex - 1);
			long twoStart = getRunIndex(inAtIndex + inCount);
			if (oneEnd == twoStart)  	// Items to delete are all within  a single Run
				((RunRec*)(fRuns))[oneEnd].end -= inCount;
			else {
				((RunRec*)(fRuns))[oneEnd].end = inAtIndex - 1;
				((RunRec*)(fRuns))[twoStart].start = inAtIndex;
				((RunRec*)(fRuns))[twoStart].end -= inCount;
				
				if (twoStart > (oneEnd + 1)) {
										// Items to delete span at least  one entire Run, so one or  more Runs must be removed
										// Shift down item data
					::MemMove( getRunItemPtr(twoStart), getRunItemPtr(oneEnd + 1),
									(fNruns - twoStart) * fValueSize);
										// Shift down Run records
					::MemMove(fRuns + twoStart * sizeof(RunRec),
									fRuns + (oneEnd + 1) * sizeof(RunRec),
									(fNruns - twoStart) * sizeof(RunRec));
									
										// Adjust sizes of item and Run Handles to new number of Runs
					fNruns -= (twoStart - oneEnd - 1);
					fValues= (char*) ::MemMore( fValues, fNruns * fValueSize);
					fRuns= (long*) ::MemMore( fRuns, fNruns * sizeof(RunRec));
										// Removing Runs have moved twoStart next to oneEnd
					twoStart = oneEnd + 1;
					}
				}
										// Adjust indexes for Runs after twoStart
			RunRec* run = (RunRec*) mRunsH;
			for (long i = twoStart + 1; i < fNruns; i++) {
				run[i].start -= inCount;
				run[i].end -= inCount;
				}
			fNitems -= inCount;
			}
		}
}

void DRunArray::AssignItemsAt( long inCount, long inAtIndex, 
					const void *inValue, long inItemSize)
{
	if (goodIndex(inAtIndex) && (inCount > 0)) {
		RemoveItemsAt(inCount, inAtIndex);
		InsertItemsAt(inCount, inAtIndex, inValue, inItemSize);
		}
}

void* DRunArray::getItemPtr( long itemIndex) const
{
	return getRunItemPtr( getRunIndex(itemIndex));
}


//---------

//	Return the index of the Run containing the specified item
//	Run index is zero-based

long DRunArray::getRunIndex( long itemIndex)
{
	long index = kIndexBad;
	RunRec* run = (RunRec*) fRuns;
	for (long i = 0; i < fNruns; i++) {		 
		if (itemIndex <= run[i].end) {
			index = i;
			break;
		}
	}
	return index;
}


//	Insert a new Run with the specified location, length, and value

void DRunArray::InsertRun( long inRunIndex, long inCount, const void *inItem)
{
										// Add room for another data item
	if (fValues == NULL) 
		fValues= (char*) ::MemNew((fNruns + 1) * fValueSize);
	else  
		fValues= (char*) ::MemMore( fValues, (fNruns+1) * fValueSize);
	if (!fValues) Nlm_Message(MSG_FATAL, "DRunArray:: memory full");
		  
										// Add room for another RunRec
	if (fRuns == NULL)  
		fRuns= (long*) ::MemNew((fNruns + 1) *  sizeof(RunRec));
	else  
		fRuns= (long*) ::MemMore( fRuns, (fNruns+1) *  sizeof(RunRec));
	if (!fRuns) Nlm_Message(MSG_FATAL, "DRunArray:: memory full");
	 	
										// Adjust Run indexes
	RunRec* run = (RunRec*) fRuns;
	
	if (fNruns == 0) {				// Array was empty
		run[0].start = 1;
		run[0].end = inCount;
		} 
	else if (inRunIndex == fNruns) {
										// Insert at end of Array
		run[fNruns].start = run[fNruns - 1].end + 1;
		run[fNruns].end = run[fNruns].start + inCount - 1;
		} 
	else if (inRunIndex < fNruns) {
										// Shift up item data
		::MemMove( getRunItemPtr(inRunIndex), getRunItemPtr(inRunIndex + 1),
								(fNruns - inRunIndex) * fValueSize);
										// Shift up run data
		::MemMove(fRuns + inRunIndex * sizeof(RunRec),
						fRuns + (inRunIndex + 1) * sizeof(RunRec),
						(fNruns - inRunIndex) * sizeof(RunRec));
		run[inRunIndex].end = run[inRunIndex].start + inCount - 1;
		}
	
										// Shift up indexes of Runs past insertion point
	fNruns += 1;
	for (long i = inRunIndex + 1; i < fNruns; i++) {
		run[i].start += inCount;
		run[i].end += inCount;
		}
	
										// Store data for new items
	::MemMove(inItem, getRunItemPtr(inRunIndex), fValueSize);
}



//	Insert a new Run into the middle of an existing Run, thereby splitting
//	the existing Run into two pieces.

void DRunArray::SplitRun( long inRunIndex, long inItemIndex, long inCount, const void *inItem)
{
										// Add room for two data items
	fValues= (char*) ::MemMore( fValues, (fNruns + 2) * fValueSize);
	if (!fValues) Nlm_Message(MSG_FATAL, "DRunArray:: memory full");
										// Add room for two RunRecords
	fRuns= (long*) ::MemMore( fRuns, (fNruns + 2) * sizeof(RunRec));
	if (!fRuns) Nlm_Message(MSG_FATAL, "DRunArray:: memory full");
	
	RunRec	*run = (RunRec*) fRuns;
	if (inRunIndex < fNruns) {
										// Shift up item data
		::MemMove( getRunItemPtr(inRunIndex + 1), getRunItemPtr(inRunIndex + 3),
						(fNruns - inRunIndex - 1) * fValueSize);
										// Shift up RunRecords
		::MemMove(fRuns + (inRunIndex + 1) * sizeof(RunRec),
						fRuns + (inRunIndex + 3) * sizeof(RunRec),
						(fNruns - inRunIndex - 1) * sizeof(RunRec));
		}
										// Store data for new items
	::MemMove(inItem, getRunItemPtr(inRunIndex + 1), fValueSize);
										// Copy data for second half of split run
	::MemMove( getRunItemPtr(inRunIndex), getRunItemPtr(inRunIndex + 2), fValueSize);
	
	long	secondSplitRange = run[inRunIndex].end - inItemIndex;
	run[inRunIndex].end = inItemIndex - 1;
	run[inRunIndex+1].start = inItemIndex;
	run[inRunIndex+1].end = inItemIndex + inCount - 1;
	run[inRunIndex+2].start = run[inRunIndex+1].end + 1;
	run[inRunIndex+2].end = run[inRunIndex+2].start + secondSplitRange;
	
										// Shift up indexes of Runs past insertion point
	fNruns += 2;
	for (long i = inRunIndex + 3; i < fNruns; i++) {
		run[i].start += inCount;
		run[i].end += inCount;
		}
}



void DRunArray::ExtendRun( long	inRunIndex, long inCount)
{
	RunRec	*run = (RunRec*) fRuns;
	run[inRunIndex].end += inCount;
										// Shift up indexes of Runs past insertion point
	for (long i = inRunIndex + 1; i < fNruns; i++) {
		run[i].start += inCount;
		run[i].end += inCount;
		}
}

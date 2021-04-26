//----------------------------------------------------------------------------------------
// UGridView.cp 
// Copyright © 1987-1993 by Apple Computer Inc. All rights reserved.
//----------------------------------------------------------------------------------------

#ifndef __UGRIDVIEW__
#include <UGridView.h>
#endif

#ifndef __STDIO__
#include <stdio.h>
#endif

#ifndef __UGEOMETRY__
#include <UGeometry.h>
#endif

#ifndef __ERRORS__
#include <Errors.h>
#endif

#ifndef __TOOLUTILS__
#include <ToolUtils.h>
#endif

#ifndef __PACKAGES__
#include <Packages.h>
#endif

#ifndef __STDLIB__
#include <stdlib.h>
#endif

#ifndef __UADORNERS__
#include <UAdorners.h>
#endif

#ifndef __UMACAPPUTILITIES__
#include <UMacAppUtilities.h>
#endif

#ifndef __UMEMORY__
#include <UMemory.h>
#endif

#ifndef __UDEBUG__
#include <UDebug.h>
#endif

#ifndef __UMACAPPGLOBALS__
#include <UMacAppGlobals.h>
#endif

#ifndef __FONTS__
#include <Fonts.h>
#endif

//----------------------------------------------------------------------------------------
RgnHandle pPixelsToHighlight;
RgnHandle pPreviousSelection;
RgnHandle pDifference;
RgnHandle pVisibleCells;
RgnHandle pInvalidateRgn;


//========================================================================================
// GLOBAL Procedures
//========================================================================================
#undef Inherited

//----------------------------------------------------------------------------------------
// InitUGridView: 
//----------------------------------------------------------------------------------------
#pragma segment GVInit

void InitUGridView()
{
	if (qTemplateViews)
	{
		// So the linker doesn't dead strip these 
		macroDontDeadStrip(TGridView);
		macroDontDeadStrip(TTextGridView);
		macroDontDeadStrip(TTextListView);

		RegisterStdType("TGridView", kStdGridView);
		RegisterStdType("TTextGridView", kStdTextGridView);
		RegisterStdType("TTextListView", kStdTextListView);
	}

	pPixelsToHighlight = MakeNewRgn();
	pPreviousSelection = MakeNewRgn();
	pDifference = MakeNewRgn();
	pVisibleCells = MakeNewRgn();
	pInvalidateRgn = MakeNewRgn();
	gUGridViewInitialized = TRUE;
} // InitUGridView 


//========================================================================================
// CLASS TRunArray
//========================================================================================
#undef Inherited
#define Inherited TObject

#pragma segment GVOpen
DefineClass(TRunArray, Inherited);

//----------------------------------------------------------------------------------------
// TRunArray constructor
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

TRunArray::TRunArray()
{
	fChunks = NULL;
	fLastChunk = 0;
	fLastIndex = 1;
	fLastItem = 0;
	fLastTotal = 0;
	fNoOfChunks = 0;
	fNoOfItems = 0;
	fTotal = 0;
} // TRunArray::TRunArray

//----------------------------------------------------------------------------------------
// TRunArray::IRunArray: 
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

void TRunArray::IRunArray()
{
	this->IObject();

	FailInfo fi;
	Try(fi)
	{
		fChunks = (ChunkArrayHandle)(NewPermHandle(0));
		fi.Success();
	}
	else
	{
		this->Free();
		fi.ReSignal();
	}
} // TRunArray::IRunArray 

//----------------------------------------------------------------------------------------
// TRunArray::Free: 
//----------------------------------------------------------------------------------------
#pragma segment GVClose

void TRunArray::Free()					// override 
{
	fChunks = (ChunkArrayHandle)DisposeIfHandle((Handle)fChunks);// Blow chunks 

	Inherited::Free();
} // TRunArray::Free 

//----------------------------------------------------------------------------------------
// TRunArray::DeleteItems: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TRunArray::DeleteItems(short firstItem,
								   short noOfItems)
{
	long offset;
	long result;
	short i;
	short num;
	long theTotal;
	short index;

	if (!FindChunk(firstItem, num, index, theTotal))
	{
#if qDebug
		CStr255 theString;
		ConcatNumber("Unable to find chunk for item ", firstItem, theString);
		ProgramBreak(theString);
#endif

		return;
	}

	for (i = 1; i <= noOfItems; ++i)
	{
		fTotal -= (*fChunks)[num].value;

		--(*fChunks)[num].count;

		if ((*fChunks)[num].count < index)
		{
			index = 1;
			if ((*fChunks)[num].count == 0)
			{
				// need to delete that chunk 
				offset = num * sizeof(RunArrayChunk);
				result = Munger((Handle)fChunks, offset, NULL, sizeof(RunArrayChunk), &result, 0);
				FailMemError();
				--fNoOfChunks;

				// Thanks JDR 10/28/89 
				// see if we can consolidate chunks 
				if ((num > 0) && (num < fNoOfChunks) && ((*fChunks)[num - 1].value == (*fChunks)[num].value))
				{
					index = (*fChunks)[num - 1].count + 1;
					(*fChunks)[num - 1].count += (*fChunks)[num].count;
					// need to delete that chunk 
					result = Munger((Handle)fChunks, offset, NULL, sizeof(RunArrayChunk), &result, 0);
					FailMemError();
					--num;
					--fNoOfChunks;
				}
			}
			else
				++num;
		}
	}

	fNoOfItems -= noOfItems;

	// reset the cache 
	fLastItem = 0;
	fLastChunk = 0;
	fLastTotal = 0;
	fLastIndex = 1;
} // TRunArray::DeleteItems 

//----------------------------------------------------------------------------------------
// TRunArray::FindChunk: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

Boolean TRunArray::FindChunk(short item,
									short& chunk,
									short& indexInChunk,
									long& theTotal)
{
	short thisItem;
	short count;
	short delta;
	Boolean result;

	if ((fNoOfChunks <= 0) || (item > fNoOfItems) || (item <= 0))
	{
		chunk = 0;
		theTotal = 0;
		indexInChunk = 0;
		result = FALSE;
		item = 0;
	}
	else if (item == fLastItem)
	{
		// check for the very easy case 
		chunk = fLastChunk;
		theTotal = fLastTotal;
		indexInChunk = fLastIndex;
		result = TRUE;
	}
	else
	{
		delta = abs(item - fLastItem);

		if ((delta >= item) || (item <= (*fChunks)[0].count))
		{
			// start from the first chunk 
			chunk = 0;
			theTotal = 0;
			thisItem = 0;
		}
		else if (delta > (fNoOfItems - item + 1))
		{
			// start from the end chunk 
			chunk = fNoOfChunks - 1;
			count = (*fChunks)[chunk].count;
			theTotal = fTotal - (count * (*fChunks)[chunk].value);
			thisItem = fNoOfItems - count;
		}
		else
		{										// start from the previous values 
			chunk = fLastChunk;
			theTotal = fLastTotal;
			thisItem = fLastItem - fLastIndex;
		}

		if (item > thisItem)
		{
			while ((thisItem + (*fChunks)[chunk].count) < item)
			{
				count = (*fChunks)[chunk].count;
				theTotal += count * (*fChunks)[chunk].value;
				thisItem += count;
				++chunk;
			}
		}
		else
		{
			do
			{
				--chunk;
				count = (*fChunks)[chunk].count;
				theTotal -= count * (*fChunks)[chunk].value;
				thisItem -= count;
			} while (thisItem >= item);
		}
		indexInChunk = item - thisItem;
		result = TRUE;
	}
	// cache the last values 
	fLastItem = item;
	fLastChunk = chunk;
	fLastTotal = theTotal;
	fLastIndex = indexInChunk;

	return result;
} // TRunArray::FindChunk 

//----------------------------------------------------------------------------------------
// TRunArray::FindItem: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TRunArray::FindItem(long theTotal)
{
	short result = 0;
	if ((theTotal >= 0) && (theTotal <= fTotal) && (fNoOfChunks > 0))
		if (fNoOfChunks == 1)
		{
			if ((*fChunks)[0].value > 0)
				result = (short)Min(((theTotal - 1) / (*fChunks)[0].value) + 1, fNoOfItems);//!!! Cast
		}
		else if (theTotal == 0)
			result = 1;
		else
		{
			++theTotal;
			short runningCount = 0;
			for (short i = 0; i <= fNoOfChunks - 1; ++i)
			{
				RunArrayChunk chunk = (*fChunks)[i];

				theTotal -= chunk.value * chunk.count;
				runningCount += chunk.count;
				if (theTotal <= 0)
				{
					result = (short)(runningCount + (theTotal / chunk.value));//!!! Cast
					return result;
				}
			}
			result = fNoOfItems;
		}
	return result;
} // TRunArray::FindItem 

//----------------------------------------------------------------------------------------
// TRunArray::GetValue: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TRunArray::GetValue(short item)
{
	short num;
	long theTotal;
	short index;

	if (fNoOfChunks == 1)
		return (*fChunks)[0].value;
	else if (this->FindChunk(item, num, index, theTotal))
		return (*fChunks)[num].value;
	else
		return 0;
} // TRunArray::GetValue 

//----------------------------------------------------------------------------------------
// TRunArray::InsertItems: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TRunArray::InsertItems(short firstItem,
								   short noOfItems,
								   short value)
{
	short num;
	long theTotal;
	short index;
	long oldSize;
	long result;
	struct
	{
		RunArrayChunk chunk1;
		RunArrayChunk chunk2;
	} tempChunks;


	// Check if we can just increment the last size count 

	if ((firstItem > fNoOfItems) && (fNoOfChunks > 0) && ((*fChunks)[fNoOfChunks - 1].value == value))
		(*fChunks)[fNoOfChunks - 1].count += noOfItems;

	// check if we can increment any size count 
	else if (this->FindChunk(firstItem, num, index, theTotal) && ((*fChunks)[num].value == value))

		(*fChunks)[num].count += noOfItems;

	// check if this would actually fit as the last item in the previous chunk 
	// Thanks Martin FrickŽ, 10/31/89

	else if ((num > 0) && (index == 1) && ((*fChunks)[num - 1].value == value))
		(*fChunks)[num - 1].count += noOfItems;

	// We need to create a new chunk, possibly two 
	else
	{
		oldSize = GetHandleSize((Handle)fChunks);
		tempChunks.chunk1.value = value;
		tempChunks.chunk1.count = noOfItems;

		if ((index <= 1) || (firstItem > fNoOfItems))
		{
			// need to add one chunk 
			if (firstItem > fNoOfItems)
				num = fNoOfChunks;				// add a row on the end 

			result = Munger((Handle)fChunks, num * sizeof(RunArrayChunk), NULL, 0, &tempChunks, sizeof(RunArrayChunk));
			FailMemError();
			++fNoOfChunks;
		}
		else
		{
			// need to add two 
			tempChunks.chunk2.count = (*fChunks)[num].count - index + 1;
			tempChunks.chunk2.value = (*fChunks)[num].value;
			(*fChunks)[num].count = index - 1;
			result = Munger((Handle)fChunks, (num + 1) * sizeof(RunArrayChunk), NULL, 0, &tempChunks, 2 * sizeof(RunArrayChunk));
			FailMemError();
			fNoOfChunks += 2;
		}
		if (GetHandleSize((Handle)fChunks) <= oldSize)
			Failure(memFullErr, 0);
	}

	// reset the cache 
	fLastItem = 0;
	fLastChunk = 0;
	fLastTotal = 0;
	fLastIndex = 1;

	fNoOfItems += noOfItems;
	fTotal += noOfItems * value;
} // TRunArray::InsertItems 

//----------------------------------------------------------------------------------------
// TRunArray::SumValues: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

long TRunArray::SumValues(short firstItem,
								 short noOfItems)
{
	short chunk;
	short indexInChunk;
	long total;
	long precedingTotal;
	long result;

	result = 0;

	if (fNoOfChunks == 1)
		result = noOfItems * (*fChunks)[0].value;
	else if (firstItem == 1)
	{
		if (this->FindChunk(noOfItems, chunk, indexInChunk, total))
			result = total + (indexInChunk * (*fChunks)[chunk].value);
	}
	else if (this->FindChunk(firstItem, chunk, indexInChunk, total))
	{
		precedingTotal = total + ((indexInChunk - 1) * (*fChunks)[chunk].value);
		if (this->FindChunk(firstItem + noOfItems - 1, chunk, indexInChunk, total))
			result = total + (indexInChunk * ((*fChunks)[chunk].value) - precedingTotal);
	}
	return result;
} // TRunArray::SumValues 


//========================================================================================
// CLASS TGridView
//========================================================================================
#undef Inherited
#define Inherited TView

#pragma segment GVOpen
DefineClass(TGridView, Inherited);

//----------------------------------------------------------------------------------------
// TGridView constructor
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

TGridView::TGridView()
{
	fNumOfRows = 0;
	fNumOfCols = 0;

	fAdornRows = FALSE;
	fAdornCols = FALSE;
	fRowInset = 0;
	fColInset = 0;
	fColWidths = NULL;
	fRowHeights = NULL;

	fSelections = NULL;
	fHLRegion = NULL;
	fTemporarySelections = NULL;

	fSingleSelection = TRUE;
} // TGridView::TGridView

//----------------------------------------------------------------------------------------
// TGridView::IGridView: 
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

void TGridView::IGridView(TDocument* itsDocument,
								 // Its document 
								 TView* itsSuperView,
								 // Its parent view 
								 const VPoint& itsLocation,
								 // Top, Left in parent's coords 
								 const VPoint& itsSize,
								 // Ignored for SizeVariable
								 SizeDeterminer itsHSizeDet,
								 SizeDeterminer itsVSizeDet,
								 // Size determiners 
								 short numOfRows,
								 // Number of rows initially 
								 short numOfCols,
								 // Number of columns initially 
								 short rowHeight,
								 // Height of initial rows 
								 short colWidth,
								 // Height of initial columns 
								 Boolean adornRows,
								 // Adornment for Rows? 
								 Boolean adornCols,
								 // Adornment for Columns? 
								 short rowInset,
								 // horizontal space between cells 
								 short colInset,
								 // vertical space between cells 
								 Boolean singleSelection)// single cell selection? 

{
#if qDebug
	if (!gUGridViewInitialized)
	{
		ProgramBreak("InitUGridView must be called before creating a grid view.");
		Failure(noErr, 0);
	}
#endif

	this->IView(itsDocument, itsSuperView, itsLocation, itsSize, itsHSizeDet, itsVSizeDet);

	fAdornRows = adornRows;
	fAdornCols = adornCols;

	// Make sure the insets are evenly divided between top/bottom or left/right 
	fRowInset = rowInset;
	if (odd(rowInset))
		++fRowInset;

	fColInset = colInset;
	if (odd(colInset))
		++fColInset;

	FailInfo fi;
	Try(fi)
	{
		TRunArray* aRunArray = new TRunArray;
		aRunArray->IRunArray();
		fColWidths = aRunArray;
		
		aRunArray = new TRunArray;
		aRunArray->IRunArray();
		fRowHeights = aRunArray;

		fSelections = MakeNewRgn();				// region to hold current selections 
		fHLRegion = MakeNewRgn();				// region to hold current highlighted cells 
		fTemporarySelections = MakeNewRgn();	// used by SetSelectionRect 

		fSingleSelection = singleSelection;

		if (numOfCols > 0)
			this->InsColFirst(numOfCols, colWidth);
		if (numOfRows > 0)
			this->InsRowFirst(numOfRows, rowHeight);

		this->AddAdorner(gSelectionAdorner, kDrawView, FALSE);// wants DoHighlightSelection 

		fi.Success();
	}
	else										// Recover
	{
		this->Free();
		fi.ReSignal();
	}

} // TGridView::IGridView 

//----------------------------------------------------------------------------------------
// TGridView::Clone: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

TObject* TGridView::Clone()				// override 
{
	TGridView * aClonedGridView = (TGridView *)(Inherited::Clone());
	VOLATILE(aClonedGridView);

	aClonedGridView->fNumOfCols = 0;
	aClonedGridView->fNumOfRows = 0;
	aClonedGridView->fColWidths = NULL;		// For failure handling
	aClonedGridView->fRowHeights = NULL;
	
	// This code is neccessary because MacApp does not support cloning properly
	// It could be replaced with TGridView(inputView).fColWidths.Clone otherwise

	FailInfo fi;
	Try(fi)
	{
		TRunArray* aRunArray = new TRunArray;
		aRunArray->IRunArray();
		aClonedGridView->fColWidths = aRunArray;
		
		aRunArray = new TRunArray;
		aRunArray->IRunArray();
		aClonedGridView->fRowHeights = aRunArray;

		aClonedGridView->fSelections = MakeNewRgn();// region to hold current selections 
		aClonedGridView->fHLRegion = MakeNewRgn();// region to hold current highlighted cells 
		aClonedGridView->fTemporarySelections = MakeNewRgn();// used by SetSelectionRect 

		if (fNumOfCols > 0)
			aClonedGridView->InsColFirst(fNumOfCols, this->GetColWidth(1));
		if (fNumOfRows > 0)
			aClonedGridView->InsRowFirst(fNumOfRows, this->GetRowHeight(1));

		fi.Success();
	}
	else										// Recover
	{
		aClonedGridView->Free();
		fi.ReSignal();
	}

	return aClonedGridView;
} // TGridView::Clone 

//----------------------------------------------------------------------------------------
// TGridView::Free: 
//----------------------------------------------------------------------------------------
#pragma segment GVClose

void TGridView::Free()					// override 
{
	// Dispose regions 
	fSelections = DisposeIfRgnHandle(fSelections);
	fHLRegion = DisposeIfRgnHandle(fHLRegion);
	fTemporarySelections = DisposeIfRgnHandle(fTemporarySelections);

	fColWidths = (TRunArray *)(FreeIfObject(fColWidths));
	fRowHeights = (TRunArray *)(FreeIfObject(fRowHeights));

	Inherited::Free();
} // TGridView::Free 

//----------------------------------------------------------------------------------------
// TGridView::GetStandardSignature: 
//----------------------------------------------------------------------------------------
#pragma segment GVWriteResource

IDType TGridView::GetStandardSignature()	// override 
{
	return kStdGridView;
} // TGridView::GetStandardSignature 

//----------------------------------------------------------------------------------------
// TGridView::ReadFields: 
//----------------------------------------------------------------------------------------
#pragma segment GVReadResource

void TGridView::ReadFields(TStream* aStream)// override 
{
#if qDebug
	if (!gUGridViewInitialized)
	{
		ProgramBreak("InitUGridView must be called before creating a grid view.");
		Failure(noErr, 0);
	}
#endif

	short numOfCols,
	 numOfRows,
	 colWidth,
	 rowHeight;

	Inherited::ReadFields(aStream);

	FailInfo fi;
	Try(fi)
	{
		numOfRows = aStream->ReadInteger();
		numOfCols = aStream->ReadInteger();

		rowHeight = aStream->ReadInteger();
		colWidth = aStream->ReadInteger();

		fRowInset = aStream->ReadInteger();
		fColInset = aStream->ReadInteger();

		fAdornRows = aStream->ReadBoolean();
		fAdornCols = aStream->ReadBoolean();

		fSingleSelection = aStream->ReadBoolean();

		if (odd(fRowInset))
			++fRowInset;

		if (odd(fColInset))
			++fColInset;

		TRunArray* aRunArray = new TRunArray;
		aRunArray->IRunArray();
		fColWidths = aRunArray;

		aRunArray = new TRunArray;
		aRunArray->IRunArray();
		fRowHeights = aRunArray;

		fSelections = MakeNewRgn();				// region to hold current selections 
		fHLRegion = MakeNewRgn();				// region to hold current highlighted cells 
		fTemporarySelections = MakeNewRgn();	// used by SetSelectionRect 

		if (numOfCols > 0)
			this->InsColFirst(numOfCols, colWidth);
		if (numOfRows > 0)
			this->InsRowFirst(numOfRows, rowHeight);

		fi.Success();
	}
	else										// Recover
		{
		this->Free();
		fi.ReSignal();
	}
} // TGridView::ReadFields 

//----------------------------------------------------------------------------------------
// TGridView::WriteFields: 
//----------------------------------------------------------------------------------------
#pragma segment GVWriteResource

void TGridView::WriteFields(TStream* aStream)// override 
{
	short rowHeight = 0,
	 colWidth = 0;

	Inherited::WriteFields(aStream);

	if (fNumOfRows > 0)
		rowHeight = this->GetRowHeight(1);

	if (fNumOfCols > 0)
		colWidth = this->GetColWidth(1);

	aStream->WriteInteger(fNumOfRows);
	aStream->WriteInteger(fNumOfCols);
	aStream->WriteInteger(rowHeight);
	aStream->WriteInteger(colWidth);
	aStream->WriteInteger(fRowInset);
	aStream->WriteInteger(fColInset);
	aStream->WriteBoolean(fAdornRows);
	aStream->WriteBoolean(fAdornCols);
	aStream->WriteBoolean(fSingleSelection);
} // TGridView::WriteFields 

//----------------------------------------------------------------------------------------
// TGridView::AdornCol: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::AdornCol(short aCol,
								const VRect& area)
{
	CRect qdRect;

	PenNormal();
	this->ViewToQDRect(area, qdRect);

	MoveTo(qdRect.right, qdRect.top);
	Line(0, qdRect.GetLength(vSel));
	if (aCol == 1)
	{
		MoveTo(qdRect.left, qdRect.top);
		Line(0, qdRect.GetLength(vSel));
	}
} // TGridView::AdornCol 

//----------------------------------------------------------------------------------------
// TGridView::AdornRow: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::AdornRow(short aRow,
								const VRect& area)
{
	CRect qdRect;

	PenNormal();
	this->ViewToQDRect(area, qdRect);

	MoveTo(qdRect.left, qdRect.bottom);
	Line(qdRect.GetLength(hSel), 0);
	if (aRow == 1)
	{
		MoveTo(qdRect.left, qdRect.top);
		Line(qdRect.GetLength(hSel), 0);
	}
} // TGridView::AdornRow 

//----------------------------------------------------------------------------------------
// TGridView::CalcMinFrame: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::CalcMinFrame(VRect& minFrame)// override 
{
	Inherited::CalcMinFrame(minFrame);

	// Set the amount of room needed for that many items 
	minFrame[botRight] = minFrame[topLeft] + VPoint(fColWidths->fTotal, fRowHeights->fTotal);
} // TGridView::CalcMinFrame 

//----------------------------------------------------------------------------------------
// TGridView::CanSelectCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

Boolean TGridView::CanSelectCell(GridCell aCell)
{
	return ((aCell.h >= 1) && (aCell.v >= 1) && (aCell.h <= fNumOfCols) && (aCell.v <= fNumOfRows));
} // TGridView::CanSelectCell 

//----------------------------------------------------------------------------------------
// TGridView::CellToVRect: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::CellToVRect(GridCell aCell,
								   VRect& aRect)
{
	short width;
	short height;

	if ((aCell.h < 1) || (aCell.v < 1) || (aCell.h > fNumOfCols) || (aCell.v > fNumOfRows))
	{
#if qRangeCheck && qDebugMsg
		fprintf(stderr, "aCell.h == %6d fNumOfCols == %6d", aCell.h, fNumOfCols);
		fprintf(stderr, "aCell.v == %6d fNumOfRows == %6d", aCell.v, fNumOfRows);
		ProgramBreak("Range Check in CellToVRect");
#endif

		aRect = gZeroVRect;
	}
	else										// all the params look OK 
	{
		width = fColWidths->GetValue(aCell.h);
		if (fColWidths->fNoOfChunks == 1)
			aRect.left = width * (aCell.h - 1);
		else
			aRect.left = fColWidths->SumValues(1, aCell.h - 1);
		aRect.right = aRect.left + width;

		height = fRowHeights->GetValue(aCell.v);
		if (fRowHeights->fNoOfChunks == 1)
			aRect.top = height * (aCell.v - 1);
		else
			aRect.top = fRowHeights->SumValues(1, aCell.v - 1);
		aRect.bottom = aRect.top + height;
	}
} // TGridView::CellToVRect 

//----------------------------------------------------------------------------------------
// TGridView::ColToVRect: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::ColToVRect(short aCol,
								  short numOfCols,
								  VRect& aRect)
{
	long width;
	long leftEdge;

	if ((aCol < 1) || (numOfCols < 1) || (aCol + numOfCols - 1 > fNumOfCols))
	{
#if qDebugMsg && qRangeCheck
		fprintf(stderr, "fNumOfCols == %1d aCol == %1d", fNumOfCols, aCol);
		ProgramBreak("Range Check in ColToVRect");
#endif

		aRect = gZeroVRect;
	}
	else										// all the params look OK 
	{
		if (fColWidths->fNoOfChunks == 1)		// only one column width 
		{
			width = this->GetColWidth(1);
			leftEdge = width * (aCol - 1);
			width *= numOfCols;
		}
		else
		{
			leftEdge = fColWidths->SumValues(1, aCol - 1);
			width = fColWidths->SumValues(aCol, numOfCols);
		}

		aRect = VRect(leftEdge, 0, leftEdge + width, fRowHeights->fTotal);
	}
} // TGridView::ColToVRect 

//----------------------------------------------------------------------------------------
// TGridView::AddStrip: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::AddStrip(RgnHandle thePixels,
								VHSelect direction,
								short& startOfStrip,
								short endOfStrip,
								CRect& stripRect,
								short row,
								short col,
								VRect& pixels,
								VRect& previousPixels,
								CRect& prevStripRect)
{
	CRect qdRect;

	if (direction == vSel)
		stripRect = CRect(col, startOfStrip, col, endOfStrip);
	else
		stripRect = CRect(startOfStrip, row, endOfStrip, row);

#if qDebugMsg
	if (gIntenseDebugging)
	{
		fprintf(stderr, "Adding cells = %s\n", (char*)stripRect);
	}
#endif

	pixels = previousPixels;

	if (stripRect.top != prevStripRect.top)
		pixels.top = this->fRowHeights->SumValues(1, stripRect.top - 1);
	if (stripRect.bottom != prevStripRect.bottom)
	{
		if (stripRect.bottom == stripRect.top)
			pixels.bottom = pixels.top + this->fRowHeights->GetValue(stripRect.bottom);
		else
			pixels.bottom = this->fRowHeights->SumValues(1, stripRect.bottom);
	}
	if (stripRect.left != prevStripRect.left)
		pixels.left = this->fColWidths->SumValues(1, stripRect.left - 1);
	if (stripRect.right != prevStripRect.right)
	{
		if (stripRect.right == stripRect.left)
			pixels.right = pixels.left + this->fColWidths->GetValue(stripRect.right);
		else
			pixels.right = this->fColWidths->SumValues(1, stripRect.right);
	}

	previousPixels = pixels;

	CTemporaryRegion tempRgn;

	SetEmptyRgn(tempRgn);
	this->ViewToQDRect(pixels, qdRect);
	RectRgn(tempRgn, qdRect);
	UnionRgn(tempRgn, thePixels, thePixels);

	prevStripRect = stripRect;
	startOfStrip = 0;
} // TGridView::AddStrip 


//----------------------------------------------------------------------------------------
// TGridView::CellsToPixels: 
//----------------------------------------------------------------------------------------
void TGridView::CellsToPixels(RgnHandle theCells,
									 RgnHandle thePixels)
{
	CRect stripRect;
	GridCell aCell;
	short row,
	 col;
	VRect pixels;
	VRect previousPixels;
	short startOfStrip;

	SetEmptyRgn(thePixels);

	if (!EmptyRgn(theCells) && this->Focus())
	{
		if ((*theCells)->rgnSize == 10)			// the region is a rectangle 
		{
			CRect cellBounds = (**theCells).rgnBBox;
			pixels = VRect(fColWidths->SumValues(1, cellBounds.left - 1), fRowHeights->SumValues(1, cellBounds.top - 1), fColWidths->SumValues(1, cellBounds.right - 1), fRowHeights->SumValues(1, cellBounds.bottom - 1));
			this->ViewToQDRect(pixels, (CRect &) (*thePixels)->rgnBBox);
		}
		else
		{
			// Reduce the cells to only those that are visible 
			CRect visibleCells;
			VRect visibleVRect;
			this->GetVisibleRect(visibleVRect);
			// !!!FIX the following 2 lines after MPW C compiler bug is corrected.
			GridCell temp = this->VPointToLastCell(visibleVRect[topLeft]);
			visibleCells[topLeft] = temp;
			// !!!FIX the following 2 lines after MPW C compiler bug is corrected.
			temp = this->VPointToLastCell(visibleVRect[botRight]);
			visibleCells[botRight] = temp;
			SetRectRgn(pVisibleCells, visibleCells.left, visibleCells.top, visibleCells.right + 1, visibleCells.bottom + 1);
			SectRgn(theCells, pVisibleCells, pVisibleCells);
			CRect cellBounds = (**pVisibleCells).rgnBBox;

			CRect prevStripRect = gZeroRect;
			VHSelect direction = LongerSide(cellBounds);
			if (direction == vSel)
			{
				for (col = cellBounds.left; col < cellBounds.right; ++col)
				{
					aCell.h = col;
					startOfStrip = 0;
					for (row = cellBounds.top; row < cellBounds.bottom; ++row)
					{
						aCell.v = row;
						if (PtInRgn(aCell, pVisibleCells))
						{
							if (startOfStrip == 0)
								startOfStrip = row;
						}
						else if (startOfStrip > 0)
							this->AddStrip(thePixels, direction, startOfStrip, row - 1, stripRect, row, col, pixels, previousPixels, prevStripRect);
					}
					if (startOfStrip > 0)
						this->AddStrip(thePixels, direction, startOfStrip, cellBounds.bottom - 1, stripRect, row, col, pixels, previousPixels, prevStripRect);
				}
			}
			else
			{
				for (row = cellBounds.top; row < cellBounds.bottom; ++row)
				{
					aCell.v = row;
					startOfStrip = 0;
					for (col = cellBounds.left; col < cellBounds.right; ++col)
					{
						aCell.h = col;
						if (PtInRgn(aCell, pVisibleCells))
						{
							if (startOfStrip == 0)
								startOfStrip = col;
						}
						else if (startOfStrip > 0)
							this->AddStrip(thePixels, direction, startOfStrip, col - 1, stripRect, row, col, pixels, previousPixels, prevStripRect);
					}
					if (startOfStrip > 0)
						this->AddStrip(thePixels, direction, startOfStrip, cellBounds.right - 1, stripRect, row, col, pixels, previousPixels, prevStripRect);
				}
			}
		}
	}
} // TGridView::CellsToPixels 

//----------------------------------------------------------------------------------------
// TGridView::DoHighlightSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::DoHighlightSelection(HLState fromHL,
											HLState toHL)// override 
{
	if (!EmptyRgn(fHLRegion))
		this->HighlightCells(fHLRegion, fromHL, toHL);
} // TGridView::DoHighlightSelection 

//----------------------------------------------------------------------------------------
// TGridView::HighlightCells: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::HighlightCells(RgnHandle theCells,
									  HLState fromHL,
									  HLState toHL)
{
	if ((fromHL != toHL) && this->Focus())
	{
		this->CellsToPixels(theCells, pPixelsToHighlight);
		switch (fromHL + toHL)
		{
			case hlOffDim:
				PenNormal();
				PenMode(patXor);
				UseSelectionColor();
				FrameRgn(pPixelsToHighlight);
				break;
			case hlOnDim:
				{
					CTemporaryRegion innerRgn;
					CopyRgn(pPixelsToHighlight, innerRgn);
					InsetRgn(innerRgn, 1, 1);
					PenNormal();
					UseSelectionColor();
					InvertRgn(innerRgn);
				}
				break;
			case hlOffOn:
				PenNormal();
				UseSelectionColor();
				InvertRgn(pPixelsToHighlight);
				break;
		}
	}
}												// TGridView::HighlightCells 

//----------------------------------------------------------------------------------------
// TGridView::DoMenuCommand: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::DoMenuCommand(CommandNumber aCommandNumber)
{
	switch (aCommandNumber)
	{
		case cSelectAll:
			{
				CCellIterator iter(this);

				for (GridCell aCell = iter.FirstCell(); iter.More(); aCell = iter.NextCell())
				{
					if (this->CanSelectCell(aCell))
						this->SelectCell(aCell, kExtend, kHighlight, kSelect);
				}
				break;
			}

		default:
			Inherited::DoMenuCommand(aCommandNumber);
			break;
	}
}												// TGridView::DoMenuCommand 

//----------------------------------------------------------------------------------------
// TGridView::DoMouseCommand: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::DoMouseCommand(VPoint& theMouse,
									  TToolboxEvent* event,
									  CPoint)	// override 
{
	GridCell aCell;

	if ((this->IdentifyPoint(theMouse, aCell) != badChoice)  && this->CanSelectCell(aCell))
	{
		TCellSelectCommand * aCellSelectCommand = new TCellSelectCommand;
		aCellSelectCommand->ICellSelectCommand(this, theMouse, event->IsShiftKeyPressed(), event->IsCommandKeyPressed());
		this->PostCommand(aCellSelectCommand);
	}
} // TGridView::DoMouseCommand 

//----------------------------------------------------------------------------------------
// TGridView::Draw: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::Draw(const VRect& area)	// override 
{
	VRect aRect;
	VRect bRect;
	register short i;
	GridCell startCell;
	GridCell stopCell;
	VRect viewArea(area);
	GridCell startCellToDraw;
	VRect cellsArea;
	VRect localArea(area);						// use localArea (because area is const)

	if ((fNumOfRows > 0) && (fNumOfCols > 0))
	{
		// make sure we have something to draw 

		startCell = this->VPointToLastCell(viewArea[topLeft]);
		stopCell = this->VPointToLastCell(viewArea[botRight]);

		this->CellToVRect(startCell, aRect);
		this->CellToVRect(stopCell, bRect);
		bRect[topLeft] = aRect[topLeft];
		localArea = bRect;

		startCellToDraw = startCell;
		cellsArea = localArea;
		if (viewArea.top >= aRect.bottom - (fRowInset / 2))//((fRowInset) >> 1)
		{
			++startCellToDraw.v;
			cellsArea.top += aRect.GetLength(vSel);
		}

		this->DrawRangeOfCells(startCellToDraw, stopCell, cellsArea);

		if (fAdornCols)
		{
			short colWidth = 0;
			aRect = localArea;

			register Boolean constantWidth = fColWidths->fNoOfChunks == 1;
			if (constantWidth)					// only one width 
				colWidth = this->GetColWidth(1);

			for (i = startCell.h; i <= stopCell.h; ++i)
			{
				if (constantWidth)
					aRect.right = aRect.left + colWidth;
				else
					aRect.right = aRect.left + this->GetColWidth(i);

				this->AdornCol(i, aRect);
				aRect.left = aRect.right;
			}
		}

		if (fAdornRows)
		{
			short rowHeight = 0;
			aRect = localArea;

			register Boolean constantHeight = fRowHeights->fNoOfChunks == 1;
			if (constantHeight)					// only one height 
				rowHeight = this->GetRowHeight(1);

			for (i = startCell.v; i <= stopCell.v; ++i)
			{
				if (constantHeight)
					aRect.bottom = aRect.top + rowHeight;
				else
					aRect.bottom = aRect.top + this->GetRowHeight(i);

				this->AdornRow(i, aRect);
				aRect.top = aRect.bottom;
			}
		}
	}

	Inherited::Draw(localArea);
} // TGridView::Draw 

//----------------------------------------------------------------------------------------
// TGridView::DrawRangeOfCells: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::DrawRangeOfCells(GridCell startCell,
										GridCell stopCell,
										const VRect& aRect)
{
	VCoordinate colWidth;
	VCoordinate rowHeight;

	VRect localVRect(aRect);					// added because argument is const
	localVRect.left += fColInset / 2;
	localVRect.top += fRowInset / 2;

	VCoordinate left = localVRect.left;			// save it for each iteration of for loop

	if (fColWidths->fNoOfChunks == 1)			// only one width 
		colWidth = this->GetColWidth(1);
	if (fRowHeights->fNoOfChunks == 1)			// only one height 
		rowHeight = this->GetRowHeight(1);

	CTemporaryRegion drawableRegion;
	this->GetDrawableRegion(drawableRegion);

	for (short j = startCell.v; j <= stopCell.v; ++j)
	{
		localVRect.bottom = localVRect.top - fRowInset;
		if (fRowHeights->fNoOfChunks == 1)		// only one height 
			localVRect.bottom += rowHeight;
		else
			localVRect.bottom += this->GetRowHeight(j);

		localVRect.left = left;					// start back at the left for the next row 

		for (short i = startCell.h; i <= stopCell.h; ++i)
		{
			localVRect.right = localVRect.left - fColInset;
			if (fColWidths->fNoOfChunks == 1)	// only one height 
				localVRect.right += colWidth;
			else
				localVRect.right += this->GetColWidth(i);

			// check if the rect is drawable
			CRect qdRect;
			this->ViewToQDRect(localVRect, qdRect);
			if (RectInRgn(qdRect, drawableRegion))
				this->DrawCell(GridCell(i, j), localVRect);

			localVRect.left = localVRect.right + fColInset;
		}
		localVRect.top = localVRect.bottom + fRowInset;
	}
} // TGridView::DrawRangeOfCells 

//----------------------------------------------------------------------------------------
// TGridView::DrawCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::DrawCell(GridCell		/* aCell */,
								const VRect&	/* aRect */)
{
} // TGridView::DrawCell 

//----------------------------------------------------------------------------------------
// TGridView::DelColAt: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::DelColAt(short aCol,
								short numOfCols)
{
	VRect aVRect;

	if ((aCol < 1) || (numOfCols < 1) || (aCol + numOfCols - 1 > fNumOfCols))
	{
		if (numOfCols != 0)
		{
#if qDebugMsg
			fprintf(stderr, "fNumOfCols == %1d   aCol == %1d\n", fNumOfCols, aCol);
			ProgramBreak("Range Check in DelColAt");
			return;
#endif

		}
	}
	else
	{
		long maxCol = Max(1, aCol);
		this->ColToVRect((short)maxCol, (short)Max(1, fNumOfCols - aCol + 1), aVRect);//!!! cast
		fColWidths->DeleteItems(aCol, numOfCols);
		fNumOfCols -= numOfCols;
		this->AdjustFrame();
		this->InvalidateVRect(aVRect);
	}
} // TGridView::DelColAt 

//----------------------------------------------------------------------------------------
// TGridView::DelRowAt: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::DelRowAt(short aRow,
								short numOfRows)
{
	if ((aRow < 1) || (numOfRows < 1) || (aRow + numOfRows - 1 > fNumOfRows))
	{
		if (numOfRows != 0)
		{
#if qDebugMsg
			fprintf(stderr, "fNumOfRows == %1d  aRow == %1d\n", fNumOfRows, aRow);
			ProgramBreak("Range Check in DelRowAt");
			return;
#endif
		}
	}
	else
	{
		long maxRow = Max(1, aRow);
		VRect aVRect;
		this->RowToVRect((short)maxRow, (short)Max(1, fNumOfRows - aRow + 1), aVRect);//!!! cast
		fRowHeights->DeleteItems(aRow, numOfRows);
		fNumOfRows -= numOfRows;
		this->AdjustFrame();
		this->InvalidateVRect(aVRect);
	}
} // TGridView::DelRowAt 

//----------------------------------------------------------------------------------------
// TGridView::DelColFirst: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::DelColFirst(short numOfCols)
{
	this->DelColAt(1, numOfCols);
} // TGridView::DelColFirst 

//----------------------------------------------------------------------------------------
// TGridView::DelRowFirst: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::DelRowFirst(short numOfRows)
{
	this->DelRowAt(1, numOfRows);
} // TGridView::DelRowFirst 

//----------------------------------------------------------------------------------------
// TGridView::DelColLast: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::DelColLast(short numOfCols)
{
	this->DelColAt(fNumOfCols - numOfCols + 1, numOfCols);
} // TGridView::DelColLast 

//----------------------------------------------------------------------------------------
// TGridView::DelRowLast: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::DelRowLast(short numOfRows)
{
	this->DelRowAt(fNumOfRows - numOfRows + 1, numOfRows);
} // TGridView::DelRowLast 

//----------------------------------------------------------------------------------------
// TGridView::FirstSelectedCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

GridCell TGridView::FirstSelectedCell()
{
	CRect bounds;
	GridCell aCell;
	GridCell result;

	result = gZeroPt;
	if (this->IsAnyCellSelected())
	{
		bounds = (*fSelections)->rgnBBox;
		if ((**fSelections).rgnSize == 10)		// whole rectangle 
			result = bounds[topLeft];
		else
			for (short i = bounds.top; i <= bounds.bottom - 1; ++i)
		{
			aCell.v = i;
			for (short j = bounds.left; j <= bounds.right - 1; ++j)
			{
				aCell.h = j;
				if (PtInRgn(aCell, fSelections))
				{
					result = aCell;
					return result;
				}
			}
		}
	}
	return result;
} // TGridView::FirstSelectedCell 

//----------------------------------------------------------------------------------------
// TGridView::GetColWidth: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TGridView::GetColWidth(short aCol)
{
	if ((aCol < 1) || (aCol > fNumOfCols))
	{
#if qRangeCheck && qDebugMsg
		fprintf(stderr, "fNumOfCols == &1d  aCol == %1d\n", fNumOfCols, aCol);
		ProgramBreak("Range Check in GetColWidth");
#endif

		return 0;
	}
	else
		return fColWidths->GetValue(aCol);
} // TGridView::GetColWidth 

//----------------------------------------------------------------------------------------
// TGridView::GetRowHeight: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TGridView::GetRowHeight(short aRow)
{
	if ((aRow < 1) || (aRow > fNumOfRows))
	{
#if qRangeCheck && qDebugMsg
		fprintf(stderr, "fNumOfRows == &1d  aRow == %1d\n", fNumOfRows, aRow);
		ProgramBreak("Range Check in GetRowHeight");
#endif

		return 0;
	}
	else
		return fRowHeights->GetValue(aRow);
} // TGridView::GetRowHeight 

//----------------------------------------------------------------------------------------
// TGridView::InvalidateCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::InvalidateCell(GridCell aCell)
{
	VRect aVRect;

	this->CellToVRect(aCell, aVRect);
	this->InvalidateVRect(aVRect);
} // TGridView::InvalidateCell 

//----------------------------------------------------------------------------------------
// TGridView::InvalidateSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::InvalidateSelection()
{
	if (this->Focus())
	{
		this->CellsToPixels(fSelections, pInvalidateRgn);
		this->InvalidateRegion(pInvalidateRgn);
	}
} // TGridView::InvalidateSelection 

//----------------------------------------------------------------------------------------
// TGridView::IdentifyPoint: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

GridViewPart TGridView::IdentifyPoint(const VPoint& thePoint,
											 GridCell& aCell)
{
	GridViewPart aGridViewPart = badChoice;	// default value

	aCell = this->VPointToCell(thePoint);

	if (aCell != gZeroPt)
	{
		VRect aVRect;
		this->CellToVRect(aCell, aVRect);
		aVRect.Inset(VPoint(fColInset / 2, fRowInset / 2));
		aGridViewPart = inCell;

		if (fColInset > 0)
		{
			if (thePoint.h < aVRect.left)
			{
				aGridViewPart = inColumn;		// To the left of the cell 
			}
			else if (thePoint.h >= aVRect.right)	// Remember PtInRgn will report a CPoint
                                                // as in a region only if the pixel to the
                                                // right and below the CPoint is contained
                                                // in the region.
			{
				aGridViewPart = inColumn;		// To the right of the cell 
				++aCell.h;
				if (aCell.h > fNumOfCols)
					aGridViewPart = badChoice;
			}
		}

		if (fRowInset > 0)
		{
			if (thePoint.v < aVRect.top)
			{
				if (aGridViewPart == inColumn)
					aGridViewPart = inVertex;	// Click on both 
				else
					aGridViewPart = inRow;		// Above the cell 
			}
			else if (thePoint.v >= aVRect.bottom)// Remember PtInRgn will report a CPoint
                                                // as in a region only if the pixel to the
                                                // right and below the CPoint is contained
                                                // in the region.
			{
				if (aGridViewPart == inColumn)
					aGridViewPart = inVertex;	// Click on both 
				else
					aGridViewPart = inRow;		// Above the cell 
				++aCell.v;
				if (aCell.v > fNumOfRows)
					aGridViewPart = badChoice;
			}
		}
	}
	return aGridViewPart;
} // TGridView::IdentifyPoint 

//----------------------------------------------------------------------------------------
// TGridView::InsColBefore: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::InsColBefore(short aCol,
									short numOfCols,
									short aWidth)
{
	if ((aCol < 1) || (numOfCols < 1))
	{
		if (numOfCols != 0)
		{
#if qDebugMsg && qRangeCheck
			fprintf(stderr, "fNumOfCols == %1d aCol == %1d", fNumOfCols, aCol);
			ProgramBreak("Range Check in InsColBefore");
#endif

		}
	}
	else
	{
		fColWidths->InsertItems(aCol, numOfCols, aWidth);
		fNumOfCols += numOfCols;
		this->AdjustFrame();
		long maxCol = Max(1, aCol);
		VRect aVRect;
		this->ColToVRect((short)maxCol, (short)Max(1, fNumOfCols - aCol + 1), aVRect);
		this->InvalidateVRect(aVRect);
	}
} // TGridView::InsColBefore 

//----------------------------------------------------------------------------------------
// TGridView::InsRowBefore: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::InsRowBefore(short aRow,
									short numOfRows,
									short aHeight)
{
	if ((aRow < 1) || (numOfRows < 1))
	{
		if (numOfRows != 0)
		{
#if qDebugMsg && qRangeCheck
			fprintf(stderr, "fNumOfRows == %1d aRow == %1d", fNumOfRows, aRow);
			ProgramBreak("Range Check in InsRowBefore");
#endif

		}
	}
	else
	{
		fRowHeights->InsertItems(aRow, numOfRows, aHeight);
		fNumOfRows += numOfRows;
		this->AdjustFrame();
		long maxRow = Max(1, aRow);
		VRect aVRect;
		this->RowToVRect((short)maxRow, (short)Max(1, fNumOfRows - aRow + 1), aVRect);//!!! Cast
		this->InvalidateVRect(aVRect);
	}
} // TGridView::InsRowBefore 

//----------------------------------------------------------------------------------------
// TGridView::InsColLast: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::InsColLast(short numOfCols,
								  short aWidth)
{
	this->InsColBefore(fNumOfCols + 1, numOfCols, aWidth);
} // TGridView::InsColLast 

//----------------------------------------------------------------------------------------
// TGridView::InsRowLast: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::InsRowLast(short numOfRows,
								  short aHeight)
{
	this->InsRowBefore(fNumOfRows + 1, numOfRows, aHeight);
} // TGridView::InsRowLast 

//----------------------------------------------------------------------------------------
// TGridView::InsColFirst: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::InsColFirst(short numOfCols,
								   short aWidth)
{
	this->InsColBefore(1, numOfCols, aWidth);
} // TGridView::InsColFirst 

//----------------------------------------------------------------------------------------
// TGridView::InsRowFirst: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::InsRowFirst(short numOfRows,
								   short aHeight)
{
	this->InsRowBefore(1, numOfRows, aHeight);
} // TGridView::InsRowFirst 

//----------------------------------------------------------------------------------------
// TGridView::IsCellSelected: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

Boolean TGridView::IsCellSelected(GridCell aCell)
{
	return PtInRgn(aCell, fSelections);
} // TGridView::IsCellSelected 

//----------------------------------------------------------------------------------------
// TGridView::IsAnyCellSelected: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

Boolean TGridView::IsAnyCellSelected()
{
	return !EmptyRgn(fSelections);
} // TGridView::IsAnyCellSelected 

//----------------------------------------------------------------------------------------
// TGridView::LastSelectedCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

GridCell TGridView::LastSelectedCell()
{
	CRect bounds;
	GridCell aCell;
	GridCell result;

	result = gZeroPt;
	if (this->IsAnyCellSelected())
	{
		bounds = (**fSelections).rgnBBox;
		if ((**fSelections).rgnSize == 10)		// whole rectangle 
		{
			aCell.h = bounds.right - 1;
			aCell.v = bounds.bottom - 1;
			result = aCell;
		}
		else
			for (short i = bounds.bottom - 1; i >= bounds.top; i--)
		{
			aCell.v = i;
			for (short j = bounds.right - 1; j >= bounds.left; j--)
			{
				aCell.h = j;
				if (PtInRgn(aCell, fSelections))
				{
					result = aCell;
					return result;
				}
			}
		}
	}
	return result;
} // TGridView::LastSelectedCell 

//----------------------------------------------------------------------------------------
// TGridView::RowToVRect: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::RowToVRect(short aRow,
								  short numOfRows,
								  VRect& aRect)
{
	if ((aRow < 1) || (numOfRows < 1) || (aRow + numOfRows - 1 > fNumOfRows))
	{
#if qDebugMsg && qRangeCheck
		fprintf(stderr, "fNumOfRows == %1d aRow == %1d", fNumOfRows, aRow);
		ProgramBreak("Range Check in RowToVRect");
#endif

		aRect = gZeroVRect;
	}
	else										// all the params look OK 
		{
		long height;
		long topEdge;

		if (fRowHeights->fNoOfChunks == 1)		// only one row height 
		{
			height = fRowHeights->GetValue(1);
			topEdge = height * (aRow - 1);
			height *= numOfRows;
		}
		else
		{
			topEdge = fRowHeights->SumValues(1, aRow - 1);
			height = fRowHeights->SumValues(aRow, numOfRows);
		}

		aRect = VRect(0, topEdge, fColWidths->fTotal, topEdge + height);
	}
} // TGridView::RowToVRect 

//----------------------------------------------------------------------------------------
// TGridView::ScrollSelectionIntoView: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::ScrollSelectionIntoView(Boolean redraw)
{

	if (this->IsAnyCellSelected())
	{
		VRect topLeftRect;
		VRect botRightRect;
		VRect selectionRect;
		
		CRect rgnBBox = (**fSelections).rgnBBox;

		this->CellToVRect(rgnBBox[topLeft], topLeftRect);
		GridCell botRightCell(rgnBBox.right - 1, rgnBBox.bottom - 1);
		this->CellToVRect(botRightCell, botRightRect);
		selectionRect = topLeftRect | botRightRect;
		long hCoord = Max(topLeftRect.GetLength(hSel), botRightRect.GetLength(hSel));
		VPoint minToSee(hCoord, Max(topLeftRect.GetLength(vSel), botRightRect.GetLength(vSel)));
		this->RevealRect(selectionRect, minToSee, redraw);
	}
	else
		Inherited::ScrollSelectionIntoView(redraw);
} // TGridView::ScrollSelectionIntoView 

//----------------------------------------------------------------------------------------
// TGridView::SetColWidth: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::SetColWidth(short aCol,
								   short numOfCols,
								   short aWidth)
{
#if qDebugMsg
	if ((aCol < 1) || (numOfCols < 1) || (aCol + numOfCols - 1 > fNumOfCols))
	{
		fprintf(stderr, "fNumOfCols == %1d aCol == %1d", fNumOfCols, aCol);
		ProgramBreak("Range Check in SetColWidth");
	}
	else
#endif

	if ((fColWidths->fNoOfChunks > 1) || (this->GetColWidth(1) != aWidth))
	{
		VRect aRect;
		long colMax = Max(1, aCol);			// to remove outlined inline warning

		// invalidate the old screen area of the cols
		this->ColToVRect((short)colMax, (short)Max(1, fNumOfCols - aCol + 1), aRect);
		this->InvalidateVRect(aRect);

		// set the new col widths and adjust the view's frame
		fColWidths->DeleteItems(aCol, numOfCols);
		fColWidths->InsertItems(aCol, numOfCols, aWidth);
		this->AdjustFrame();

		// invalidate the new screen area of the cols
		this->ColToVRect((short)colMax, (short)Max(1, fNumOfCols - aCol + 1), aRect);
		this->InvalidateVRect(aRect);
	}
} // TGridView::SetColWidth 

//----------------------------------------------------------------------------------------
// TGridView::SetRowHeight: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::SetRowHeight(short aRow,
									short numOfRows,
									short aHeight)
{
#if qDebugMsg
	if ((aRow < 1) || (numOfRows < 1) || (aRow + numOfRows - 1 > fNumOfRows))
	{
		fprintf(stderr, "fNumOfRows == %1d aRow == %1d", fNumOfRows, aRow);
		ProgramBreak("Range Check in SetRowHeight.");
	}
	else
#endif

	if ((fRowHeights->fNoOfChunks != 1) || (this->GetRowHeight(1) != aHeight))
	{
		VRect aVRect;
		long rowMax = Max(1, aRow);		// To remove outlined inline warning

		// invalidate the old screen area of the rows
		this->RowToVRect((short)rowMax, (short)Max(1, fNumOfRows - aRow + 1), aVRect);
		this->InvalidateVRect(aVRect);

		// set the new row height and adjust the view's frame
		fRowHeights->DeleteItems(aRow, numOfRows);
		fRowHeights->InsertItems(aRow, numOfRows, aHeight);
		this->AdjustFrame();

		// invalidate the new screen area of the rows
		this->RowToVRect((short)rowMax, (short)Max(1, fNumOfRows - aRow + 1), aVRect);
		this->InvalidateVRect(aVRect);
	}
} // TGridView::SetRowHeight 

//----------------------------------------------------------------------------------------
// TGridView::SelectCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::SelectCell(GridCell theCell,
								  Boolean extendSelection,
								  Boolean highlight,
								  Boolean select)
{
	this->SetSelectionRect(CRect(theCell, theCell), extendSelection, highlight, select);
} // TGridView::SelectCell 

//----------------------------------------------------------------------------------------
// TGridView::SetEmptySelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::SetEmptySelection(Boolean highlight)
{
	SetEmptyRgn(fTemporarySelections);
	this->SetSelection(fTemporarySelections, kDontExtend, highlight, kSelect);
} // TGridView::SetEmptySelection 

//----------------------------------------------------------------------------------------
// TGridView::SetSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::SetSelection(RgnHandle cellsToSelect,
									Boolean extendSelection,
									Boolean highlight,
									Boolean select)
{
#if qDebug
	CRect bounds = (**cellsToSelect).rgnBBox;

	if (fSingleSelection && ((bounds.GetLength(hSel) > 1) || (bounds.GetLength(vSel) > 1)))
		ProgramBreak("Attempt to select multiple cells when fSingleSelection is TRUE");
	if (!EmptyRgn(cellsToSelect))
		if ((bounds.left < 1) || (bounds.top < 1) || (bounds.right > fNumOfCols + 1) || (bounds.bottom > fNumOfRows + 1))
			ProgramBreak("Attempted selection is outside the range of cells");
#endif

	if (highlight)
		CopyRgn(fSelections, pPreviousSelection);// save the old selection 

	CTemporaryRegion tempRgn;

	SetRectRgn(tempRgn, 1, 1, fNumOfCols + 1, fNumOfRows + 1);
	SectRgn(cellsToSelect, tempRgn, tempRgn);

	if (extendSelection && select)				// extend the selection region 
		UnionRgn(tempRgn, fSelections, fSelections);
	else if (select)							// reset the selection region 
		CopyRgn(tempRgn, fSelections);
	else										// need to de-select the new region 
		DiffRgn(fSelections, tempRgn, fSelections);

	this->UserSelectionChanged(this);			// so doc updates its designator 

	CopyRgn(fSelections, fHLRegion);

	if (highlight)
	{
		HLState currentHL = this->GetCurrentHL();

		// Turn the deselected cells off 
		DiffRgn(pPreviousSelection, fSelections, pDifference);
		this->HighlightCells(pDifference, currentHL, hlOff);

		// Turn the newly selected cells on 
		DiffRgn(fSelections, pPreviousSelection, pDifference);
		this->HighlightCells(pDifference, hlOff, currentHL);
	}
} // TGridView::SetSelection 

//----------------------------------------------------------------------------------------
// TGridView::SetSelectionRect: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TGridView::SetSelectionRect(const CRect& selectionRect,
										Boolean extendSelection,
										Boolean highlight,
										Boolean select)

{
	if (selectionRect == gZeroRect)
		SetEmptyRgn(fTemporarySelections);
	else
		SetRectRgn(fTemporarySelections, selectionRect.left, selectionRect.top, selectionRect.right + 1, selectionRect.bottom + 1);

	this->SetSelection(fTemporarySelections, extendSelection, highlight, select);
} // TGridView::SetSelectionRect 

//----------------------------------------------------------------------------------------
// TGridView::SetSingleSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TGridView::SetSingleSelection(Boolean theSetting)
{
	fSingleSelection = theSetting;
} // TGridView::SetSingleSelection 

//----------------------------------------------------------------------------------------
// TGridView::VPointToCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

GridCell TGridView::VPointToCell(const VPoint& aPoint)
{
	GridCell aCell(fColWidths->FindItem(aPoint.h), fRowHeights->FindItem(aPoint.v));

	if ((aCell.h == 0) || (aCell.v == 0))
		return gZeroPt;
	else
		return aCell;
} // TGridView::VPointToCell 

//----------------------------------------------------------------------------------------
// TGridView::VPointToLastCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

GridCell TGridView::VPointToLastCell(const VPoint& aPoint)
{
	GridCell aCell;

	aCell.h = fColWidths->FindItem(aPoint.h);
	if (aCell.h == 0)							// If its invalid, return the last column 
		aCell.h = fNumOfCols;

	aCell.v = fRowHeights->FindItem(aPoint.v);
	if (aCell.v == 0)							// If its invalid, return the last row 
		aCell.v = fNumOfRows;

	return aCell;
} // TGridView::VPointToLastCell 


//========================================================================================
// CLASS TTextGridView
//========================================================================================
#undef Inherited
#define Inherited TGridView

#pragma segment GVOpen
DefineClass(TTextGridView, Inherited);

//----------------------------------------------------------------------------------------
// TTextGridView constructor
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

TTextGridView::TTextGridView()
{
	fTextStyle = gSystemStyle;
	fTextStyleRsrcID = kNoResource;
	fJustification = teFlushDefault;
	fPreferOutline = kDontPreferOutline;
	fLineHeight = 0;
	fLineAscent = 0;
} // TTextGridView::TTextGridView

//----------------------------------------------------------------------------------------
// TTextGridView::ITextGridView: 
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

void TTextGridView::ITextGridView(TDocument* itsDocument,
										 // Its document 
										 TView* itsSuperView,
										 // Its parent view 
										 const VPoint& itsLocation,
										 // Top, Left in parent's coords 
										 const VPoint& itsSize,
										 SizeDeterminer itsHSizeDet,
										 SizeDeterminer itsVSizeDet,
										 // Size determiners 
										 short numOfRows,
										 // Number of rows initially 
										 short numOfCols,
										 // Number of columns initially 
										 short rowHeight,
										 // Row height, or zero for font height 
										 short colWidth,
										 // Width of items in the columns 
										 Boolean adornRows,
										 // Adornment for Rows? 
										 Boolean adornCols,
										 // Adornment for Columns? 
										 short rowInset,
										 // horizontal space between cells 
										 short colInset,
										 // vertical space between cells 
										 Boolean singleSelection,
										 // single cell selection? 
										 const TextStyle& itsTextStyle)// size, color, etc. font info 

{
	if (rowHeight == 0)
	{
		// Compute the rowHeight for call to IGridView based on the line height and the row inset.
		GrafPtr savedPort;
		GetPort(&savedPort);					// save the port
		SetPort(gWorkPort);						// set port to the work port

		SetPortTextStyle(itsTextStyle);			// set the port's text style
		PenNormal();

		FontInfo theFontInfo;
		rowHeight = MAGetFontInfo(theFontInfo) + rowInset;

		SetPort(savedPort);						// restore port
	}

	this->IGridView(itsDocument, itsSuperView, itsLocation, itsSize, itsHSizeDet, itsVSizeDet, numOfRows, numOfCols, rowHeight, colWidth, adornRows, adornCols, rowInset, colInset, singleSelection);

	fTextStyle = itsTextStyle;

	this->SetUpFont();

	if ((fNumOfCols == 1) && (fSizeDeterminer[hSel] != sizeFixed) && (this->GetColWidth(1) == 0) && fSuperView)
		this->SetColWidth(1, fNumOfCols, (short)fSuperView->fSize.h);//!!! Note cast
} // TTextGridView::ITextGridView 

//----------------------------------------------------------------------------------------
// TTextGridView::GetStandardSignature: 
//----------------------------------------------------------------------------------------
#pragma segment GVWriteResource

IDType TTextGridView::GetStandardSignature()// override 
{
	return kStdTextGridView;
} // TTextGridView::GetStandardSignature 

//----------------------------------------------------------------------------------------
// TTextGridView::ReadFields: 
//----------------------------------------------------------------------------------------
#pragma segment GVReadResource

void TTextGridView::ReadFields(TStream* aStream)// override 
{
	Inherited::ReadFields(aStream);

	FailInfo fi;
	Try(fi)
	{
		fTextStyleRsrcID = aStream->ReadInteger();
		if (fTextStyleRsrcID != kNoResource)
		{
			TextStyle itsTextStyle;
			MAGetTextStyle(fTextStyleRsrcID, itsTextStyle);
			fTextStyle = itsTextStyle;
		}

		fPreferOutline = aStream->ReadBoolean();
		fi.Success();
	}
	else
	{
		this->Free();
		fi.ReSignal();
	}

	this->SetUpFont();

	if (fNumOfRows > 0)
		if (this->GetRowHeight(1) == 0)			// set row height from font 
			this->SetRowHeight(1, fNumOfRows, fLineHeight + fRowInset);

	if ((fNumOfCols == 1) && (fSizeDeterminer[hSel] != sizeFixed) && (this->GetColWidth(1) == 0))
		this->SetColWidth(1, fNumOfCols, (short)fSize.h);//!!! Note cast
} // TTextGridView::ReadFields 

//----------------------------------------------------------------------------------------
// TTextGridView::WriteFields: 
//----------------------------------------------------------------------------------------
#pragma segment GVWriteResource

void TTextGridView::WriteFields(TStream* aStream)// override 
{
	Inherited::WriteFields(aStream);

	aStream->WriteInteger(fTextStyleRsrcID);
	aStream->WriteBoolean(fPreferOutline);
} // TTextGridView::WriteFields 

//----------------------------------------------------------------------------------------
// TTextGridView::DrawCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextGridView::DrawCell(GridCell aCell,
									const VRect& aRect)// override 
{
	CStr255 theText;

	this->GetText(aCell, theText);

	if (this->GetColWidth(aCell.h) > 0)
	{
		CRect r;

		this->ViewToQDRect(aRect, r);
		MADrawString(theText, r, fJustification, fPreferOutline);
	}
} // TTextGridView::DrawCell 

//----------------------------------------------------------------------------------------
// TTextGridView::Focus: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

Boolean TTextGridView::Focus()			// override 
{
	if (Inherited::Focus())
	{
		this->SetPen();
		return TRUE;
	}
	else
		return FALSE;
} // TTextGridView::Focus 

//----------------------------------------------------------------------------------------
// TTextGridView::SetUpFont: 
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

void TTextGridView::SetUpFont()
{
	GrafPtr savedPort;

	// set port to the work port
	GetPort(&savedPort);
	SetPort(gWorkPort);

	{											// block for CWhileOutlinePreferred (which has a constructor/destructor)
		CWhileOutlinePreferred setOP(fPreferOutline);
		FontInfo theFontInfo;

		this->SetPen();
		fLineHeight = MAGetFontInfo(theFontInfo);// returns height of font 
		fLineAscent = theFontInfo.ascent + (theFontInfo.leading / 2);
	}

	// restore port
	SetPort(savedPort);
} // TTextGridView::SetUpFont 

//----------------------------------------------------------------------------------------
// TTextGridView::SetPen: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextGridView::SetPen()
{
	TextStyle itsTextStyle = fTextStyle;
	SetPortTextStyle(itsTextStyle);
	PenNormal();
} // TTextGridView::SetPen 

//----------------------------------------------------------------------------------------
// TTextGridView::GetText: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextGridView::GetText(GridCell		/* aCell */,
								   CStr255&	aString )
{
	aString = gEmptyString;
} // TTextGridView::GetText 


//========================================================================================
// CLASS TTextListView
//========================================================================================
#undef Inherited
#define Inherited TTextGridView

#pragma segment GVOpen
DefineClass(TTextListView, Inherited);

//----------------------------------------------------------------------------------------
// TTextListView::TTextListView: Empty constructor to satisfy the compiler.
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

TTextListView::TTextListView()
{
} // TTextListView::TTextListView

//----------------------------------------------------------------------------------------
// TTextListView::ITextListView: 
//----------------------------------------------------------------------------------------
#pragma segment GVOpen

void TTextListView::ITextListView(TDocument* itsDocument,
										 // Its document 
										 TView* itsSuperView,
										 // Its parent view 
										 const VPoint& itsLocation,
										 // Top, Left in parent's coords 
										 const VPoint& itsSize,
										 SizeDeterminer itsHSizeDet,
										 SizeDeterminer itsVSizeDet,
										 // Size determiners 
										 short numOfItems,
										 // Number of items initially 
										 short rowHeight,
										 // Row height, or zero for font height 
										 short colWidth,
										 // Width of items in the columns 
										 Boolean adornRows,
										 // Draw the row adornments? 
										 Boolean adornCols,
										 // Draw the col adornment? 
										 short rowInset,
										 // Amount to inset the rows 
										 short colInset,
										 // Amount to inset the column 
										 Boolean singleSelection,
										 // single cell selection? 
										 const TextStyle& itsTextStyle)// size, color, etc. font info 

{
	this->ITextGridView(itsDocument, itsSuperView, itsLocation, itsSize, itsHSizeDet, itsVSizeDet, numOfItems, 1, rowHeight, colWidth, adornRows, adornCols, rowInset, colInset, singleSelection, itsTextStyle);
} // TTextListView::ITextListView 

//----------------------------------------------------------------------------------------
// TTextListView::GetStandardSignature: 
//----------------------------------------------------------------------------------------
#pragma segment GVWriteResource

IDType TTextListView::GetStandardSignature()// override 
{
	return kStdTextListView;
} // TTextListView::GetStandardSignature 

//----------------------------------------------------------------------------------------
// TTextListView::CanSelectCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

Boolean TTextListView::CanSelectCell(GridCell aCell)// override 
{
	return (Inherited::CanSelectCell(aCell) && this->CanSelectItem(aCell.v));
} // TTextListView::CanSelectCell 

//----------------------------------------------------------------------------------------
// TTextListView::CanSelectItem: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

Boolean TTextListView::CanSelectItem(short anItem)
{
	return ((anItem >= 1) && (anItem <= fNumOfRows));
} // TTextListView::CanSelectItem 

//----------------------------------------------------------------------------------------
// TTextListView::DelItemAt: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TTextListView::DelItemAt(short anItem,
									 short numOfItems)
{
	this->DelRowAt(anItem, numOfItems);
} // TTextListView::DelItemAt 

//----------------------------------------------------------------------------------------
// TTextListView::DelItemFirst: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TTextListView::DelItemFirst(short numOfItems)
{
	this->DelItemAt(1, numOfItems);
} // TTextListView::DelItemFirst 

//----------------------------------------------------------------------------------------
// TTextListView::DelItemLast: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TTextListView::DelItemLast(short numOfItems)
{
	this->DelItemAt(fNumOfRows - numOfItems + 1, numOfItems);
} // TTextListView::DelItemLast 

//----------------------------------------------------------------------------------------
// TTextListView::DoKeySelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::DoKeySelection(const CStr255& selectionString)
{
	CStr255 itemText;
	short itemIndex;
	short lastItemIndex = GetItemIndexOrdered(1);

	for (short i = 1; i <= fNumOfRows; i++)
	{
		itemIndex = GetItemIndexOrdered(i);
		GetItemText(itemIndex, itemText);
		itemText.Delete(selectionString.Length() + 1, itemText.Length() - selectionString.Length());
		lastItemIndex = itemIndex;
		if (itemText >= selectionString)
			break;
	}

	this->SelectItem(lastItemIndex, FALSE, TRUE, TRUE);
	this->ScrollSelectionIntoView(TRUE);
} // TTextListView::DoKeySelection 

//----------------------------------------------------------------------------------------
// TTextListView::FirstSelectedItem: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TTextListView::FirstSelectedItem()
{
	GridCell aGridCell(this->FirstSelectedCell());
	return aGridCell.v;
} // TTextListView::FirstSelectedItem 

//----------------------------------------------------------------------------------------
// TTextListView::GetItemHeight: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TTextListView::GetItemHeight(short anItem)
{
	return this->GetRowHeight(anItem);
} // TTextListView::GetItemHeight 

//----------------------------------------------------------------------------------------
// TTextListView::GetItemWidth: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TTextListView::GetItemWidth()
{
	return this->GetColWidth(1);
} // TTextListView::GetItemWidth 

//----------------------------------------------------------------------------------------
// TTextListView::GetItemText: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::GetItemText(short,
									   CStr255& aString)
{
	aString = gEmptyString;
} // TTextListView::GetItemText 

//----------------------------------------------------------------------------------------
// TTextListView::GetItemIndexOrdered: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TTextListView::GetItemIndexOrdered(short nthItem)
{
	return nthItem;
} // TTextListView::GetItemIndexOrdered 

//----------------------------------------------------------------------------------------
// TTextListView::GetText: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::GetText(GridCell aCell,
								   CStr255& aString)// override 
{
	this->GetItemText(aCell.v, aString);
} // TTextListView::GetText 

//----------------------------------------------------------------------------------------
// TTextListView::InsItemBefore: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::InsItemBefore(short anItem,
										 short numOfItems)
{
	this->InsRowBefore(anItem, numOfItems, fLineHeight + fRowInset);
} // TTextListView::InsItemBefore 

//----------------------------------------------------------------------------------------
// TTextListView::InsItemFirst: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::InsItemFirst(short numOfItems)
{
	this->InsItemBefore(1, numOfItems);
} // TTextListView::InsItemFirst 

//----------------------------------------------------------------------------------------
// TTextListView::InsItemLast: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::InsItemLast(short numOfItems)
{
	this->InsItemBefore(fNumOfRows + 1, numOfItems);
} // TTextListView::InsItemLast 

//----------------------------------------------------------------------------------------
// TTextListView::InvalidateItem: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::InvalidateItem(short anItem)
{
	GridCell aCell(1, anItem);
	this->InvalidateCell(aCell);
} // TTextListView::InvalidateItem 

//----------------------------------------------------------------------------------------
// TTextListView::IsItemSelected: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

Boolean TTextListView::IsItemSelected(short anItem)
{
	GridCell aCell(1, anItem);
	return this->IsCellSelected(aCell);
} // TTextListView::IsItemSelected 

//----------------------------------------------------------------------------------------
// TTextListView::LastSelectedItem: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

short TTextListView::LastSelectedItem()
{
	GridCell aGridCell(this->LastSelectedCell());
	return aGridCell.v;
} // TTextListView::LastSelectedItem 

//----------------------------------------------------------------------------------------
// TTextListView::SetFrame: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TTextListView::SetFrame(const VRect& newFrame,
									Boolean invalidate)// override 
{
	VPoint oldSize(fSize);

	Inherited::SetFrame(newFrame, invalidate);

	if (fNumOfCols == 1 && fSize != oldSize)
	{
		fColWidths->fTotal += fSize.h - (*(fColWidths->fChunks))[0].value;
		(*(fColWidths->fChunks))[0].value = (short)fSize.h;//!!! Note cast
	}
} // TTextListView::SetFrame 

//----------------------------------------------------------------------------------------
// TTextListView::SelectCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::SelectCell(GridCell theCell,
									  Boolean extendSelection,
									  Boolean highlight,
									  Boolean select)// override 
{
	this->SelectItem(theCell.v, extendSelection, highlight, select);
} // TTextListView::SelectCell 

//----------------------------------------------------------------------------------------
// TTextListView::SelectItem: 
//----------------------------------------------------------------------------------------
#pragma segment GVRes

void TTextListView::SelectItem(short anItem,
									  Boolean extendSelection,
									  Boolean highlight,
									  Boolean select)
{
	GridCell aCell((short)Min(1, anItem), anItem);

	Inherited::SelectCell(aCell, extendSelection, highlight, select);
} // TTextListView::SelectItem 

//----------------------------------------------------------------------------------------
// TTextListView::SetItemHeight: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TTextListView::SetItemHeight(short anItem,
										 short numOfItems,
										 short aHeight)
{
	this->SetRowHeight(anItem, numOfItems, aHeight);
} // TTextListView::SetItemHeight 

//----------------------------------------------------------------------------------------
// TTextListView::SetItemWidth: 
//----------------------------------------------------------------------------------------
#pragma segment GVNonRes

void TTextListView::SetItemWidth(short aWidth)
{
	this->SetColWidth(1, 1, aWidth);
} // TTextListView::SetItemWidth 


//========================================================================================
// CLASS TCellSelectCommand
//========================================================================================
#undef Inherited
#define Inherited TTracker

#pragma segment GVSelCommand
DefineClass(TCellSelectCommand, Inherited);

//----------------------------------------------------------------------------------------
// TCellSelectCommand constructor
//----------------------------------------------------------------------------------------
#pragma segment GVSelCommand

TCellSelectCommand::TCellSelectCommand()
{
	fAnchorCell = gZeroPt;						// At least set it to something 
	fCommandKey = FALSE;
	fDeselecting = FALSE;
	fDifference = NULL;
	fGridView = NULL;
	fPreviousCell = CPoint(-1, -1);
	fPreviousSelection = NULL;
	fShiftKey = FALSE;
	fThisSelection = NULL;
} // TCellSelectCommand::TCellSelectCommand

//----------------------------------------------------------------------------------------
// TCellSelectCommand::ICellSelectCommand: 
//----------------------------------------------------------------------------------------
#pragma segment GVSelCommand

void TCellSelectCommand::ICellSelectCommand(TGridView* itsView,
												   const VPoint& itsMouse,
												   Boolean theShiftKey,
												   Boolean theCommandKey)
{
	this->ITracker(cNoCommand, itsView, kCantUndo, kDoesNotCauseChange, NULL, itsView,
					itsView->GetScroller(kAnySuperView), itsMouse);
	fShiftKey = theShiftKey;
	fCommandKey = theCommandKey;
	fViewConstrain = FALSE;

	fGridView = itsView;

	FailInfo fi;
	Try(fi)
	{
		fPreviousSelection = MakeNewRgn();
		CopyRgn(fGridView->fSelections, fPreviousSelection);
		fThisSelection = fGridView->fHLRegion;
		SetEmptyRgn(fThisSelection);
		fDifference = MakeNewRgn();
		fi.Success();
	}
	else										// Recover
	{
		this->Free();
		fi.ReSignal();
	}
} // TCellSelectCommand::ICellSelectCommand 

//----------------------------------------------------------------------------------------
// TCellSelectCommand::Free: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TCellSelectCommand::Free()			// override 
{
	fPreviousSelection = DisposeIfRgnHandle(fPreviousSelection);
	fDifference = DisposeIfRgnHandle(fDifference);

	fThisSelection = NULL;						// I don't own it so I don't dispose it.
                                                // But, I sure don't need a reference to
                                                // it any more.

	Inherited::Free();
} // TCellSelectCommand::Free 

//----------------------------------------------------------------------------------------
// TCellSelectCommand::ComputeAnchorCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TCellSelectCommand::ComputeAnchorCell(GridCell& clickedCell)
{
	fAnchorCell = clickedCell;

	if (fShiftKey && (!EmptyRgn(fPreviousSelection)))
	{
		const CRect & bounds = (**fPreviousSelection).rgnBBox;

		if (fAnchorCell.h >= bounds.left)
			fAnchorCell.h = bounds.left;
		else
			fAnchorCell.h = bounds.right - 1;
		if (fAnchorCell.v >= bounds.top)
			fAnchorCell.v = bounds.top;
		else
			fAnchorCell.v = bounds.bottom - 1;
	}
} // TCellSelectCommand::ComputeAnchorCell 

//----------------------------------------------------------------------------------------
// TCellSelectCommand::ComputeNewSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TCellSelectCommand::ComputeNewSelection(GridCell& clickedCell)
{
	if (fGridView->CanSelectCell(clickedCell))
	{
		CRect r;

		if (fGridView->fSingleSelection || (!fShiftKey))
			SetRect(r, clickedCell.h, clickedCell.v, clickedCell.h + 1, clickedCell.v + 1);
		else
		{
			Pt2Rect(fAnchorCell, clickedCell, r);
			++r.right;
			++r.bottom;
		}
		RectRgn(fThisSelection, r);
		if (fCommandKey && (!fGridView->fSingleSelection))
			if (fDeselecting)
				DiffRgn(fPreviousSelection, fThisSelection, fThisSelection);
			else
				UnionRgn(fPreviousSelection, fThisSelection, fThisSelection);
	}
} // TCellSelectCommand::ComputeNewSelection 

//----------------------------------------------------------------------------------------
// TCellSelectCommand::HighlightNewSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TCellSelectCommand::HighlightNewSelection()
{
	// Turn off previously selected cells 
	DiffRgn(fPreviousSelection, fThisSelection, fDifference);
	fGridView->HighlightCells(fDifference, hlOn, hlOff);

	// Turn on newly selected cells
	DiffRgn(fThisSelection, fPreviousSelection, fDifference);
	fGridView->HighlightCells(fDifference, hlOff, hlOn);
} // TCellSelectCommand::HighlightNewSelection 

//----------------------------------------------------------------------------------------
// TCellSelectCommand::TrackFeedback: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TCellSelectCommand::TrackFeedback(TrackPhase/* aTrackPhase */,
											  const VPoint&/* anchorPoint */ ,
											  const VPoint&/* previousPoint */ ,
											  const VPoint&/* nextPoint */ ,
											  Boolean/* mouseDidMove */,
											  Boolean/* turnItOn */)// override 
{
} // TCellSelectCommand::TrackFeedback 

//----------------------------------------------------------------------------------------
// TCellSelectCommand::TrackMouse: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

TTracker* TCellSelectCommand::TrackMouse(TrackPhase aTrackPhase,
												VPoint&/* anchorPoint */ ,
												VPoint&/* previousPoint */ ,
												VPoint& nextPoint,
												Boolean mouseDidMove)// override 
{
	if (mouseDidMove)
	{
		GridCell clickedCell;
		VPoint clippedPoint = nextPoint;
		VRect viewExtent;

		fGridView->GetExtent(viewExtent);
		clippedPoint.ConstrainTo(viewExtent);
		clickedCell = fGridView->VPointToCell(clippedPoint);
		if (aTrackPhase == trackBegin)
		{
			this->ComputeAnchorCell(clickedCell);
			if (fCommandKey)
				fDeselecting = PtInRgn(fAnchorCell, fGridView->fSelections);
		}

		if (clickedCell != fPreviousCell)
		{
			this->ComputeNewSelection(clickedCell);
			this->HighlightNewSelection();

			CopyRgn(fThisSelection, fPreviousSelection);
			fPreviousCell = clickedCell;
		}
	}
	return this;
} // TCellSelectCommand::TrackMouse 

//----------------------------------------------------------------------------------------
// TCellSelectCommand::DoIt: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TCellSelectCommand::DoIt()			// override 
{
	if (fGridView->fSingleSelection)
		fGridView->SelectCell(((CRect &) (*fThisSelection)->rgnBBox)[topLeft], kDontExtend, kDontHighlight, kSelect);
	else
		fGridView->SetSelection(fThisSelection, kDontExtend, kDontHighlight, kSelect);
} // TCellSelectCommand::DoIt 


//========================================================================================
// CLASS TRCSelectCommand
//========================================================================================
#undef Inherited
#define Inherited TCellSelectCommand

#pragma segment GVSelCommand
DefineClass(TRCSelectCommand, Inherited);

//----------------------------------------------------------------------------------------
// TRCSelectCommand::TRCSelectCommand: Empty constructor to satisfy the compiler.
//----------------------------------------------------------------------------------------
#pragma segment ConstructorRes

TRCSelectCommand::TRCSelectCommand()
{
} // TRCSelectCommand::TRCSelectCommand

//----------------------------------------------------------------------------------------
// TRCSelectCommand::IRCSelectCommand: 
//----------------------------------------------------------------------------------------
#pragma segment GVSelCommand

void TRCSelectCommand::IRCSelectCommand(TGridView* itsView,
												   const VPoint& itsMouse,
												   Boolean theShiftKey,
												   Boolean theCommandKey)
{
	this->ICellSelectCommand(itsView, itsMouse, theShiftKey, theCommandKey);
} // TRCSelectCommand::IRCSelectCommand

//----------------------------------------------------------------------------------------
// TRCSelectCommand::ComputeNewSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TRCSelectCommand::ComputeNewSelection(GridCell& clickedCell)// override 
{
	if (fGridView->CanSelectCell(clickedCell))
	{
		CRect r;

		if (fGridView->fSingleSelection)
			r = CRect(clickedCell.h, clickedCell.v, clickedCell.h + 1, clickedCell.v + 1);
		else
		{
			Pt2Rect(fAnchorCell, clickedCell, r);
			++r.right;
			++r.bottom;
		}
		RectRgn(fThisSelection, r);
		if (fCommandKey && (!fGridView->fSingleSelection))
			if (fDeselecting)
				DiffRgn(fPreviousSelection, fThisSelection, fThisSelection);
			else
				UnionRgn(fPreviousSelection, fThisSelection, fThisSelection);
	}
} // TRCSelectCommand::ComputeNewSelection 

//----------------------------------------------------------------------------------------
// TRCSelectCommand::TrackMouse: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

TTracker* TRCSelectCommand::TrackMouse(TrackPhase aTrackPhase,
											  VPoint&/* anchorPoint */ ,
											  VPoint&/* previousPoint */ ,
											  VPoint& nextPoint,
											  Boolean mouseDidMove)// override 
{
	GridCell clickedCell;
	VRect viewExtent;
	VPoint clippedPoint;

	if (mouseDidMove)
	{
		fGridView->GetExtent(viewExtent);
		clippedPoint = nextPoint;
		clippedPoint.ConstrainTo(viewExtent);
		clickedCell = fGridView->VPointToCell(clippedPoint);
		if (aTrackPhase == trackBegin)
		{
			this->ComputeAnchorCell(clickedCell);
			if (fCommandKey)
				fDeselecting = PtInRgn(fAnchorCell, fGridView->fSelections);
		}

		if (clickedCell != fPreviousCell)
		{
			if (!fShiftKey && (aTrackPhase != trackBegin))
			{
				this->ComputeAnchorCell(clickedCell);
				if (fCommandKey)
					fDeselecting = PtInRgn(fAnchorCell, fGridView->fSelections);
			}
			this->ComputeNewSelection(clickedCell);
			this->HighlightNewSelection();

			CopyRgn(fThisSelection, fPreviousSelection);
			fPreviousCell = clickedCell;
		}
	}
	return this;
} // TRCSelectCommand::TrackMouse 


//========================================================================================
// CLASS TRowSelectCommand
//========================================================================================
#undef Inherited
#define Inherited TRCSelectCommand

#pragma segment GVSelCommand
DefineClass(TRowSelectCommand, Inherited);

//----------------------------------------------------------------------------------------
// TRowSelectCommand::TRowSelectCommand: Empty constructor to satisfy the compiler.
//----------------------------------------------------------------------------------------
#pragma segment ConstructorRes

TRowSelectCommand::TRowSelectCommand()
{
} // TRowSelectCommand::TRowSelectCommand

//----------------------------------------------------------------------------------------
// TRowSelectCommand::IRowSelectCommand: 
//----------------------------------------------------------------------------------------
#pragma segment GVSelCommand

void TRowSelectCommand::IRowSelectCommand(TGridView* itsView,
												 const VPoint& itsMouse,
												 Boolean theShiftKey,
												 Boolean theCommandKey)
{
	this->IRCSelectCommand(itsView, itsMouse, theShiftKey, theCommandKey);
} // TRowSelectCommand::IRowSelectCommand 

//----------------------------------------------------------------------------------------
// TRowSelectCommand::ComputeAnchorCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TRowSelectCommand::ComputeAnchorCell(GridCell& clickedCell)// override 
{
	Inherited::ComputeAnchorCell(clickedCell);
	fAnchorCell.h = 1;
} // TRowSelectCommand::ComputeAnchorCell 

//----------------------------------------------------------------------------------------
// TRowSelectCommand::ComputeNewSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TRowSelectCommand::ComputeNewSelection(GridCell& clickedCell)// override 
{
	clickedCell.h = fGridView->fNumOfCols;

	Inherited::ComputeNewSelection(clickedCell);
} // TRowSelectCommand::ComputeNewSelection 


//========================================================================================
// CLASS TColumnSelectCommand
//========================================================================================
#undef Inherited
#define Inherited TRCSelectCommand

#pragma segment GVSelCommand
DefineClass(TColumnSelectCommand, Inherited);

//----------------------------------------------------------------------------------------
// TColumnSelectCommand::TColumnSelectCommand: Empty constructor to satisfy the compiler.
//----------------------------------------------------------------------------------------
#pragma segment ConstructorRes

TColumnSelectCommand::TColumnSelectCommand()
{
} // TColumnSelectCommand::TColumnSelectCommand

//----------------------------------------------------------------------------------------
// TColumnSelectCommand::IColumnSelectCommand: 
//----------------------------------------------------------------------------------------
#pragma segment GVSelCommand

void TColumnSelectCommand::IColumnSelectCommand(TGridView* itsView,
													   const VPoint& itsMouse,
													   Boolean theShiftKey,
													   Boolean theCommandKey)
{
	this->IRCSelectCommand(itsView, itsMouse, theShiftKey, theCommandKey);
} // TColumnSelectCommand::IColumnSelectCommand 

//----------------------------------------------------------------------------------------
// TColumnSelectCommand::ComputeAnchorCell: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TColumnSelectCommand::ComputeAnchorCell(GridCell& clickedCell)// override 
{
	Inherited::ComputeAnchorCell(clickedCell);

	fAnchorCell.v = 1;
} // TColumnSelectCommand::ComputeAnchorCell 

//----------------------------------------------------------------------------------------
// TColumnSelectCommand::ComputeNewSelection: 
//----------------------------------------------------------------------------------------
#pragma segment GVDoCommand

void TColumnSelectCommand::ComputeNewSelection(GridCell& clickedCell)// override 
{
	clickedCell.v = fGridView->fNumOfRows;

	Inherited::ComputeNewSelection(clickedCell);
} // TColumnSelectCommand::ComputeNewSelection 


//========================================================================================
// CLASS CRowIterator
//========================================================================================
#undef Inherited
#define Inherited CIterator

const short CRowIterator::kNullRow = -1;

//----------------------------------------------------------------------------------------
// CRowIterator::CRowIterator:
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CRowIterator::CRowIterator(TGridView* itsGridView,
						   short startRow,
						   short stopRow,
						   Boolean itsForward)
{
	this->IRowIterator(itsGridView, startRow, stopRow, itsForward);
} // CRowIterator::CRowIterator 


//----------------------------------------------------------------------------------------
// CRowIterator::CRowIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CRowIterator::CRowIterator(TGridView* itsGridView,
						   Boolean itsForward)
{
	this->IRowIterator(itsGridView, 1, itsGridView->fNumOfRows, itsForward);
} // CRowIterator::CRowIterator 


//----------------------------------------------------------------------------------------
// CRowIterator::CRowIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CRowIterator::CRowIterator(TGridView* itsGridView)
{
	this->IRowIterator(itsGridView, 1, itsGridView->fNumOfRows, kIterateForward);
} // CRowIterator::CRowIterator 


//----------------------------------------------------------------------------------------
// CRowIterator::IRowIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CRowIterator::IRowIterator(TGridView* itsGridView,
								short startRow,
								short stopRow,
								Boolean itsForward)
{
	if (itsGridView)
	{
		fFirstRow = (short)Max(1, startRow);	//!!! Cast
		fLastRow = (short)Min(itsGridView->fNumOfRows, stopRow);//!!! Cast
		fIterateForward = itsForward;
		Reset();								// this sets it to the beginning
	}
	else
	{
		fFirstRow = kNullRow;
		fLastRow = kNullRow;
		fIterateForward = kIterateForward;
		Reset();								// this sets it to the beginning
	}
} // CRowIterator::IRowIterator 


//----------------------------------------------------------------------------------------
// CRowIterator::FirstRow: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

short CRowIterator::FirstRow()
{
	Reset();
	return fCurrentRow;
} // CRowIterator::FirstRow 


//----------------------------------------------------------------------------------------
// CRowIterator::NextRow: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

short CRowIterator::NextRow()
{
	if (this->More())							// don't advance if we're at the end of the road!
		Advance();
	return fCurrentRow;
} // CRowIterator::NextRow 


//----------------------------------------------------------------------------------------
// CRowIterator::More: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

Boolean CRowIterator::More()
{
	return (fCurrentRow != kNullRow);
} // CRowIterator::More 


//----------------------------------------------------------------------------------------
// CRowIterator::Reset: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CRowIterator::Reset()
{
	if (fFirstRow <= fLastRow)
		if (fIterateForward)
			fCurrentRow = fFirstRow;
		else
			fCurrentRow = fLastRow;
	else
		fCurrentRow = kNullRow;					// there is nothing to iterate over!
} // CRowIterator::Reset 

//----------------------------------------------------------------------------------------
// CRowIterator::Advance: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CRowIterator::Advance()
{
	//This assumes that More() is FALSE -- otherwise Advance() may start iterating
	//from 0, since kNullRow is most probably less than fLastRow
	if (fIterateForward)
	{
		if (fCurrentRow < fLastRow)
			++fCurrentRow;
		else
			fCurrentRow = kNullRow;				// I know it's not really an index, butÉ
	}
	else
	{
		if (fCurrentRow > fFirstRow)
			--fCurrentRow;
		else
			fCurrentRow = kNullRow;
	}
} // CRowIterator::Advance 


//========================================================================================
// CLASS CColumnIterator
//========================================================================================
#undef Inherited
#define Inherited CIterator

const short CColumnIterator::kNullColumn = -1;

//----------------------------------------------------------------------------------------
// CColumnIterator::CColumnIterator:
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CColumnIterator::CColumnIterator(TGridView* itsGridView,
								 short startColumn,
								 short stopColumn,
								 Boolean itsForward)
{
	this->IColumnIterator(itsGridView, startColumn, stopColumn, itsForward);
} // CColumnIterator::CColumnIterator 

//----------------------------------------------------------------------------------------
// CColumnIterator::CColumnIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CColumnIterator::CColumnIterator(TGridView* itsGridView,
								 Boolean itsForward)
{
	this->IColumnIterator(itsGridView, 1, itsGridView->fNumOfCols, itsForward);
} // CColumnIterator::CColumnIterator 

//----------------------------------------------------------------------------------------
// CColumnIterator::CColumnIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CColumnIterator::CColumnIterator(TGridView* itsGridView)
{
	this->IColumnIterator(itsGridView, 1, itsGridView->fNumOfCols, kIterateForward);
} // CColumnIterator::CColumnIterator 

//----------------------------------------------------------------------------------------
// CColumnIterator::IColumnIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CColumnIterator::IColumnIterator(TGridView* itsGridView,
									  short startColumn,
									  short stopColumn,
									  Boolean itsForward)
{
	if (itsGridView)
	{
		fFirstColumn = (short)Max(1, startColumn);//!!! Cast
		fLastColumn = (short)Min(itsGridView->fNumOfCols, stopColumn);//!!! Cast
		fIterateForward = itsForward;
		Reset();								// this sets it to the beginning
	}
	else
	{
		fFirstColumn = kNullColumn;
		fLastColumn = kNullColumn;
		fIterateForward = kIterateForward;
		Reset();								// this sets it to the beginning
	}
} // CColumnIterator::IColumnIterator

//----------------------------------------------------------------------------------------
// CColumnIterator::FirstColumn: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

short CColumnIterator::FirstColumn()
{
	Reset();
	return fCurrentColumn;
} // CColumnIterator::FirstColumn 

//----------------------------------------------------------------------------------------
// CColumnIterator::NextColumn: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

short CColumnIterator::NextColumn()
{
	if (this->More())							// don't advance if we're at the end of the road!
		Advance();
	return fCurrentColumn;
} // CColumnIterator::NextColumn 

//----------------------------------------------------------------------------------------
// CColumnIterator::More: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

Boolean CColumnIterator::More()
{
	return (fCurrentColumn != kNullColumn);
} // CColumnIterator::More 

//----------------------------------------------------------------------------------------
// CColumnIterator::Reset: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CColumnIterator::Reset()
{
	if (fFirstColumn <= fLastColumn)
		if (fIterateForward)
			fCurrentColumn = fFirstColumn;
		else
			fCurrentColumn = fLastColumn;
	else
		fCurrentColumn = kNullColumn;			// there is nothing to iterate over!
} // CColumnIterator::Reset 

//----------------------------------------------------------------------------------------
// CColumnIterator::Advance: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CColumnIterator::Advance()
{
	//This assumes that More() is FALSE -- otherwise Advance() may start iterating
	//from 0, since kNullRow is most probably less than fLastRow
	if (fIterateForward)
	{
		if (fCurrentColumn < fLastColumn)
			++fCurrentColumn;
		else
			fCurrentColumn = kNullColumn;		// I know it's not really an index, butÉ
	}
	else
	{
		if (fCurrentColumn > fFirstColumn)
			--fCurrentColumn;
		else
			fCurrentColumn = kNullColumn;
	}
} // CColumnIterator::Advance 


//========================================================================================
// CLASS CCellIterator
//========================================================================================
#undef Inherited
#define Inherited CIterator

const GridCell CCellIterator::kNullCell = GridCell(-1, -1);
const Boolean CCellIterator::kIterateRowMajor = TRUE;

//----------------------------------------------------------------------------------------
// CCellIterator::CCellIterator:
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CCellIterator::CCellIterator(TGridView* itsGridView,
							 GridCell startCell,
							 GridCell stopCell,
							 Boolean itsRowForward,
							 Boolean itsColumnForward,
							 Boolean itsRowMajor) :
	// initialize the Row and Column Iterators first!
	fRowIterator(itsGridView, startCell.v, stopCell.v, itsRowForward),
	fColumnIterator(itsGridView, startCell.h, stopCell.h, itsColumnForward)

{
	fIterateRowMajor = itsRowMajor;				// don't worry about whether or not
                                                // itsGridView is valid -- let the Row and
                                                // Column Iterators worry about it.
} // CCellIterator::CCellIterator 

//----------------------------------------------------------------------------------------
// CCellIterator::CCellIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CCellIterator::CCellIterator(TGridView* itsGridView,
							 Boolean itsRowForward,
							 Boolean itsColumnForward,
							 Boolean itsRowMajor) :
	// initialize the Row and Column Iterators first!
	fRowIterator(itsGridView, itsRowForward),
	fColumnIterator(itsGridView, itsColumnForward)
{
	fIterateRowMajor = itsRowMajor;				// don't worry about whether or not
                                                // itsGridView is valid -- let the Row and
                                                // Column Iterators worry about it.
} // CCellIterator::CCellIterator 

//----------------------------------------------------------------------------------------
// CCellIterator::CCellIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CCellIterator::CCellIterator(TGridView* itsGridView) :
	// initialize the Row and Column Iterators first!
	fRowIterator(itsGridView),
	fColumnIterator(itsGridView)
{
	fIterateRowMajor = kIterateRowMajor;		// don't worry about whether or not
                                                // itsGridView is valid -- let the Row and
                                                // Column Iterators worry about it.
} // CCellIterator::CCellIterator 

//----------------------------------------------------------------------------------------
// CCellIterator::FirstCell: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

GridCell CCellIterator::FirstCell()
{
	this->Reset();
	return fCurrentCell;
} // CCellIterator::FirstCell 

//----------------------------------------------------------------------------------------
// CCellIterator::NextCell: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

GridCell CCellIterator::NextCell()
{
	this->Advance();
	return fCurrentCell;
} // CCellIterator::NextCell 

//----------------------------------------------------------------------------------------
// CCellIterator::More: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

Boolean CCellIterator::More()
{
	return (fRowIterator.More() || fColumnIterator.More());
} // CCellIterator::More 

//----------------------------------------------------------------------------------------
// CCellIterator::Reset: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CCellIterator::Reset()
{
	fCurrentCell.v = fRowIterator.FirstRow();
	fCurrentCell.h = fColumnIterator.FirstColumn();
} // CCellIterator::Reset 

//----------------------------------------------------------------------------------------
// CCellIterator::Advance: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CCellIterator::Advance()
{
	if (fIterateRowMajor)
	{
		fCurrentCell.h = fColumnIterator.NextColumn();//advance the column
		if (!fColumnIterator.More())
		{
			fCurrentCell.v = fRowIterator.NextRow();//advance the row
			if (fRowIterator.More())			//must go through the columns again
			{
				fCurrentCell.h = fColumnIterator.FirstColumn();
			}
		}
	}
	else
	{
		fCurrentCell.v = fRowIterator.NextRow();//advance the row
		if (!fRowIterator.More())
		{
			fCurrentCell.h = fColumnIterator.NextColumn();//advance the column
			if (fColumnIterator.More())			//must go through the rows again
			{
				fCurrentCell.v = fRowIterator.FirstRow();
			}
		}
	}
} // CCellIterator::Advance 



//========================================================================================
// CLASS CCellInRegionIterator
//========================================================================================
#undef Inherited
#define Inherited CCellIterator

//----------------------------------------------------------------------------------------
// CCellInRegionIterator::CCellInRegionIterator: an iterator class designed to replace the
// EachInRegion method of TGridView. APW 7-15
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CCellInRegionIterator::CCellInRegionIterator(TGridView* itsGridView,
											 RgnHandle aRegion,
											 Boolean itsRowForward,
											 Boolean itsColumnForward,
											 Boolean itsRowMajor) :
	CCellIterator(itsGridView, 
				  ((CRect &) (*aRegion)->rgnBBox)[topLeft], 
				  ((CRect &) (*aRegion)->rgnBBox)[botRight] - CPoint(1, 1), 
				  itsRowForward, itsColumnForward, itsRowMajor)
{
	fIsRectangularRegion = ((**aRegion).rgnSize == 10);// it's a Rectangle
	fRegion = aRegion;
} // CCellInRegionIterator::CCellInRegionIterator 

//----------------------------------------------------------------------------------------
// CCellInRegionIterator::CCellInRegionIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

CCellInRegionIterator::CCellInRegionIterator(TGridView* itsGridView,
											 RgnHandle aRegion) :
	CCellIterator(itsGridView, 
				  ((CRect &) (*aRegion)->rgnBBox)[topLeft], 
				  ((CRect &) (*aRegion)->rgnBBox)[botRight] - CPoint(1, 1), 
				  kIterateForward, kIterateForward, kIterateRowMajor)
{
	fIsRectangularRegion = ((**aRegion).rgnSize == 10);// it's a Rectangle
	fRegion = aRegion;
} // CCellInRegionIterator::CCellInRegionIterator 

//----------------------------------------------------------------------------------------
// CCellInRegionIterator::Reset: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes

void CCellInRegionIterator::Reset()			//override!
{
	CCellIterator::Reset();						// should be Inherited::Reset

	if (!fIsRectangularRegion)					// if it's rectangular, all cells are in
                                                // the region.
	{
		if ((this->More()) &&!CellIsInRegion())	// Otherwise, check to see if it's in the
                                                // region.
			this->Advance();					// This is now guaranteed to be a selected
                                                // cell
	}
} // CCellInRegionIterator::Reset 

//----------------------------------------------------------------------------------------
// CCellInRegionIterator::Advance: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes
void CCellInRegionIterator::Advance()		//override!

{
	CCellIterator::Advance();					//first advance

	if (!fIsRectangularRegion)					// do we have to check to see if it is
                                                // selected?
	{
		while ((this->More()) &&!CellIsInRegion())
		{
												// Yes, so while we have an
												// unselected cell, keep 
												// iterating until we
			CCellIterator::Advance();			// run out or find one.
		}
	}
} // CCellInRegionIterator::Advance 

//----------------------------------------------------------------------------------------
// CCellInRegionIterator::CellIsInRegion: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes
Boolean CCellInRegionIterator::CellIsInRegion()

{
	return PtInRgn(fCurrentCell, fRegion);
} // CCellInRegionIterator::CellIsInRegion 


//========================================================================================
// CLASS CSelectedCellIterator
//========================================================================================
#undef Inherited
#define Inherited CCellInRegionIterator

//----------------------------------------------------------------------------------------
// CSelectedCellIterator::CSelectedCellIterator: an iterator class designed to replace the
// EachSelectedCellDo method of TGridView. APW 7-15
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes
CSelectedCellIterator::CSelectedCellIterator(TGridView* itsGridView,
											 Boolean itsRowForward,
											 Boolean itsColumnForward,
											 Boolean itsRowMajor) :
	CCellInRegionIterator(itsGridView, itsGridView->fSelections, itsRowForward, itsColumnForward, itsRowMajor)
{
} // CSelectedCellIterator::CSelectedCellIterator 

//----------------------------------------------------------------------------------------
// CSelectedCellIterator::CSelectedCellIterator: 
//----------------------------------------------------------------------------------------
#pragma segment IteratorRes
CSelectedCellIterator::CSelectedCellIterator(TGridView* itsGridView) :
	CCellInRegionIterator(itsGridView, itsGridView->fSelections)
{
} // CSelectedCellIterator::CSelectedCellIterator 


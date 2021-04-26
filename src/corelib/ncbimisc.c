/*   ncbimisc.c
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE                          
*               National Center for Biotechnology Information
*                                                                          
*  This software/database is a "United States Government Work" under the   
*  terms of the United States Copyright Act.  It was written as part of    
*  the author's official duties as a United States Government employee and 
*  thus cannot be copyrighted.  This software/database is freely available 
*  to the public for use. The National Library of Medicine and the U.S.    
*  Government have not placed any restriction on its use or reproduction.  
*                                                                          
*  Although all reasonable efforts have been taken to ensure the accuracy  
*  and reliability of the software and data, the NLM and the U.S.          
*  Government do not and cannot warrant the performance or results that    
*  may be obtained by using this software or data. The NLM and the U.S.    
*  Government disclaim all warranties, express or implied, including       
*  warranties of performance, merchantability or fitness for any particular
*  purpose.                                                                
*                                                                          
*  Please cite the author in any work or product based on this material.   
*
* ===========================================================================
*
* File Name:  ncbimisc.c
*
* Author:  Gish, Kans, Ostell, Schuler
*
* Version Creation Date:   10/23/91
*
* $Revision: 2.11 $
*
* File Description: 
*   	miscellaneous functions
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 04-15-93 Schuler     Changed _cdecl to LIBCALL
* 06-15-93 Schuler     SGML functions moved to ncbisgml.c
* 02-16-94 Epstein     Retired Gestalt functions and definitions
*
* ==========================================================================
*/
 
#include <ncbi.h>
#include <ncbiwin.h>

/*
TRIPLE_MARK is the character inserted before the thousands, millions,
billions, etc. digit positions.  Change TRIPLE_MARK to a period
for the International scene, or define it as '\0' if no magnitude
markers are desired.
*/
#define TRIPLE_MARK	','

#define MISC_COMMAS 1 /* insert commas only when |value| >= 10,000 */
#define MISC_ALLCOMMAS 2 /* insert commas for any |value| >= 1,000 */
#define MISC_ANYCOMMAS (MISC_COMMAS|MISC_ALLCOMMAS)
#define MISC_PLUSSIGNS 4 /* prepend a plus sign (+) to positive values */


/*
buf[NBUFS][] is a circularly-maintained list of buffers for storing
the results of calls to Nlm_Ltostr() and Nlm_Ultostr().  Up to NBUFS
usages of either function in a single printf() may be made.
NBUFS should be defined large enough to satisfy _all_ likely occurrences.
*/
#define NBUFS 10 /* No. of static buffers for the ASCII results */

static int	bufno = 0; /* current buffer marker in the circular list */
static char	buf[NBUFS][(CHAR_BIT)*sizeof(long)/2];

/* divray[] is a fixed array of power-of-10 divisors which must be initialized*/
static unsigned long divray[CHAR_BIT*sizeof(unsigned long)/2];
/* divray_max is related to the maximum precision available in a long int */
static int	divray_max;

/* commaray[] is a fixed array that identifies positions where commas belong */
static char	commaray[DIM(divray)];

/* divray_init() initializes divray[] */
static void divray_init PROTO((void));

/* ultostr() is the basic (unsigned) integer-to-ASCII conversion engine */
static void ultostr PROTO((char *buf, unsigned long value, int commas));

/* ulwidth() is the basic length-determiner for integer-ASCII conversions */
static int ulwidth PROTO((unsigned long value, int commas));

/* heapify() is the basic heap-sort function used by Nlm_HeapSort() */
static void	heapify PROTO((Nlm_CharPtr b0, Nlm_CharPtr b, Nlm_CharPtr lim, Nlm_CharPtr last, Nlm_sizeT w, int (LIBCALLBACK *compar) (Nlm_VoidPtr, Nlm_VoidPtr) ));

/* divray_init -- initialize array of divisors and array of marker locations */
static void
divray_init (void)

{
	unsigned long	j = ULONG_MAX, k = 1;

	for (divray_max=0; divray_max < DIM(divray) && j != 0; ++divray_max) {
		divray[divray_max] = k;
		if ((divray_max+1)%3 == 0)
			commaray[divray_max] = (TRIPLE_MARK != '\0');
		j /= 10;
		k *= 10;
	}
	--divray_max;
}

/* ultostr is the basic (unsigned) integer->ASCII conversion engine */
static void
ultostr (char *buf, unsigned long value, int commas)

{
	unsigned long	value_orig = value;
	int	i, quotient;

	if (divray_max == 0)
		divray_init();

	/*
	Insert commas when value_orig >= 10000 (the Macintosh Way),
	unless MISC_ALLCOMMAS is set.
	*/
	commas = ((commas&MISC_ALLCOMMAS) && value >= 1000)
				|| (commas && value > (10*1000));

	for (i=divray_max; i > 0 && divray[i] > value; --i)
		;

	for (; i >= 0; --i) {
		if (commas && commaray[i] != NULLB && value != value_orig)
			*buf++ = TRIPLE_MARK;
		quotient = (int)(value / divray[i]);
		*buf++ = (char) '0' + (char) quotient;
		switch (quotient) {
		case 0: break;
		case 1: value -=   divray[i]; break;
		case 2: value -= 2*divray[i]; break;
		case 3: value -= 3*divray[i]; break;
		case 4: value -= 4*divray[i]; break;
		case 5: value -= 5*divray[i]; break;
		case 6: value -= 6*divray[i]; break;
		case 7: value -= 7*divray[i]; break;
		case 8: value -= 8*divray[i]; break;
		case 9: value -= 9*divray[i]; break;
		default: value -= quotient*divray[i]; break; /* shouldn't be taken */
		}
	}
	*buf = NULLB; /* tack on a NUL terminator */
	return;
}

/* Nlm_Ltostr -- convert a signed long integer to ASCII */
char * LIBCALL
Nlm_Ltostr (long value, int opts)

{
	char	*bp0, *bp;

    bp0 = bp = &buf[bufno][0];
	if (++bufno >= NBUFS)
		bufno = 0;

	if (value < 0) {
		*bp++ = '-';
		value = -value;
	}
	else
		if (opts&MISC_PLUSSIGNS && value > 0)
			*bp++ = '+';

	ultostr(bp, (unsigned long)value, opts&MISC_ANYCOMMAS);

    return bp0;
}

/* Nlm_Ultostr convert an unsigned long integer to ASCII */
char * LIBCALL
Nlm_Ultostr (unsigned long value, int opts)

{
	char	*bp0, *bp;

    bp = bp0 = &buf[bufno][0];
	if (++bufno >= NBUFS)
		bufno = 0;

	if (opts&MISC_PLUSSIGNS && value > 0)
		*bp++ = '+';

	ultostr(bp, value, opts&MISC_ANYCOMMAS);

    return bp0;
}

/*
Return the length (in characters) of the ASCII base 10 representation
of the specified integer.

If "opts&MISC_COMMAS" is non-zero, consider the additional length required
for commas before the thousands, millions, billions, etc.  positions.

If "opts&MISC_ALLCOMMAS" is non-zero, insert commas even when the value
of the integer is less than 10,000.

If "opts&MISC_PLUSSIGNS" is non-zero, consider the length of a plus sign
in front of any positive value, as well as the standard minus sign in front
of negative values.
*/
int LIBCALL
Nlm_Lwidth (long value, int opts)

{
	int	len;

	if (value < 0) {
		len = 1; /* account for the minus sign */
		value = -value;
	}
	else
		/* account for a plus sign */
		len = (opts&MISC_PLUSSIGNS) && (value > 0);
			
	return len + ulwidth(value, opts&MISC_ANYCOMMAS);
}

/*
Return the length (in characters) of the ASCII base 10 representation
of the specified unsigned integer.
*/
int LIBCALL
Nlm_Ulwidth (unsigned long value, int opts)

{
	int	len;

	len = ulwidth(value, opts&MISC_ANYCOMMAS);
	return len + ((opts&MISC_PLUSSIGNS) && (value > 0));
}

static int
ulwidth (unsigned long value, int commas)

{
	int	j, len;

	if (divray_max == 0)
		divray_init();

	for (len=divray_max; len > 0 && divray[len] > value; --len)
		;

	if ((commas&MISC_ALLCOMMAS) || (commas && value >= (10*1000)) ) {
		for (j = len-1; j > 1; --j)
			len += (commaray[j] != 0);
	}
	return len+1;
}

/*
	Nlm_HeapSort -- sort a list using an heap sort algorithm

	Performance is guaranteed O(NlogN).  Compared to BSD UNIX(TM) qsort,
	Nlm_HeapSort averages about half as fast--which may be acceptable
	for a portable, public domain function which qsort is not.

	This code was derived from original work by Professor Webb Miller
	(Penn. State University), but don't blame him for this mess or any errors.

	7/31/90 WRG
	6/18/92 Modified for segmented memory safety.  JMO
*/

void LIBCALL
Nlm_HeapSort (Nlm_VoidPtr b, Nlm_sizeT nel, Nlm_sizeT width, int (LIBCALLBACK *compar )PROTO ((Nlm_VoidPtr, Nlm_VoidPtr )))	/* Element comparison routine */

{
	register Nlm_CharPtr	base = (Nlm_CharPtr)b;
	register Nlm_sizeT	i;
	register char	ch;
	register Nlm_CharPtr 	base0 = (Nlm_CharPtr)base, lim, basef;

	if (nel < 2)
		return;

	lim = &base[((nel-2)/2)*width];
	basef = &base[(nel-1)*width];
	i = nel/2;
	for (base = &base0[(i - 1)*width]; i > 0; base = base - width) {
		heapify(base0, base, lim, basef, width, compar);
		i--;
	}

	for (base = &base0[(nel-1)*width]; base > base0; base -= width) {
		for (i=0; i<width; ++i) {
			ch = base0[i];
			base0[i] = base[i];
			base[i] = ch;
		}
		lim = base0 + ((base-base0)/2 - width);
		if (base > base0+width)
			heapify(base0, base0, lim, base-width, width, compar);
	}
}

static void
heapify (Nlm_CharPtr base0, Nlm_CharPtr base, Nlm_CharPtr lim, Nlm_CharPtr last, Nlm_sizeT width, int (LIBCALLBACK *compar )PROTO ((Nlm_VoidPtr, Nlm_VoidPtr )))

{
	register Nlm_sizeT	i;
	register char	ch;
	register Nlm_CharPtr left_son, large_son;

	left_son = base0 + 2*(base-base0) + width;
	while (base <= lim) {
		if (left_son == last)
			large_son = left_son;
		else
			large_son = (*compar)(left_son, left_son+width) >= 0 ?
							left_son : left_son+width;
		if ((*compar)(base, large_son) < 0) {
			for (i=0; i<width; ++i) {
				ch = base[i];
				base[i] = large_son[i];
				large_son[i] = ch;
			}
			base = large_son;
			left_son = base0 + 2*(base-base0) + width;
		} else
			break;
	}
}

/*****************************************************************************
*
*   ValNodeNew(vnp)
*      adds after last node in list if vnp not NULL
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeNew (ValNodePtr vnp)

{
	ValNodePtr newnode;

	newnode = (ValNodePtr) Nlm_MemNew(sizeof(ValNode));
	if (vnp != NULL)
    {
        while (vnp->next != NULL)
            vnp = vnp->next;
		vnp->next = newnode;
    }
	return newnode;
}

/*****************************************************************************
*
*   ValNodeAdd(head)
*      adds after last node in list if *head not NULL
*      If *head is NULL, sets it to the new ValNode
*      returns pointer to the NEW node added
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeAdd (ValNodePtr PNTR head)

{
	ValNodePtr newnode;

	if (head != NULL)
	{
		newnode = ValNodeNew(*head);
		if (*head == NULL)
			*head = newnode;
	}
	else
		newnode = ValNodeNew(NULL);

	return newnode;
}

/*****************************************************************************
*
*   ValNodeLink(head, newnode)
*      adds newnode at end of chain
*      if (*head == NULL) *head = newnode
*      ALWAYS returns pointer to START of chain
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeLink (ValNodePtr PNTR head, ValNodePtr newnode)
{
   ValNodePtr vnp;

   if (head == NULL)
	 return newnode;

   vnp = *head;
   
   if (vnp != NULL )
   {
        while (vnp->next != NULL)
            vnp = vnp->next;
        vnp->next = newnode;
    }
    else
		*head = newnode;

   return *head;
}

/*****************************************************************************
*
*   ValNodeAddStr (head, choice, str)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.ptrvalue = str
*         does NOT copy str
*      if str == NULL, does not add a ValNode
*   
*****************************************************************************/
ValNodePtr LIBCALL ValNodeAddStr (ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_CharPtr str)
{
	ValNodePtr newnode;

	if (str == NULL) return NULL;

	newnode = ValNodeAdd(head);
	if (newnode != NULL)
	{
		newnode->choice = (Nlm_Uint1)choice;
		newnode->data.ptrvalue = (Nlm_VoidPtr)str;
	}

	return newnode;
}

/*****************************************************************************
*
*   ValNodeCopyStr (head, choice, str)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.ptrvalue = str
*         makes a COPY of str
*      if str == NULL, does not add a ValNode
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeCopyStr (ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_CharPtr str)
{
	ValNodePtr newnode;

	if (str == NULL) return NULL;

	newnode = ValNodeAdd(head);
	if (newnode != NULL)
	{
		newnode->choice = (Nlm_Uint1)choice;
		newnode->data.ptrvalue = StringSave(str);
	}

	return newnode;
}

/*****************************************************************************
*
*   ValNodeAddInt (head, choice, value)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.intvalue = value
*   
*****************************************************************************/
ValNodePtr LIBCALL ValNodeAddInt (ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_Int4 value)
{
	ValNodePtr newnode;

	newnode = ValNodeAdd(head);
	if (newnode != NULL)
	{
		newnode->choice = (Nlm_Uint1)choice;
		newnode->data.intvalue = value;
	}

	return newnode;
}

/*****************************************************************************
*
*   ValNodeAddBoolean (head, choice, value)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.boolvalue = value
*   
*****************************************************************************/
ValNodePtr LIBCALL ValNodeAddBoolean (ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_Boolean value)
{
	ValNodePtr newnode;

	newnode = ValNodeAdd(head);
	if (newnode != NULL)
	{
		newnode->choice = (Nlm_Uint1)choice;
		newnode->data.boolvalue = value;
	}

	return newnode;
}

/*****************************************************************************
*
*   ValNodeAddFloat (head, choice, value)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.realvalue = value
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeAddFloat (ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_FloatHi value)
{
	ValNodePtr newnode;

	newnode = ValNodeAdd(head);
	if (newnode != NULL)
	{
		newnode->choice = (Nlm_Uint1)choice;
		newnode->data.realvalue = value;
	}

	return newnode;
}

/*****************************************************************************
*
*   ValNodeAddPointer (head, choice, value)
*      adds like ValNodeAdd()
*      sets newnode->choice = choice (if choice does not matter, use 0)
*      sets newnode->data.realvalue = value
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeAddPointer (ValNodePtr PNTR head, Nlm_Int2 choice, Nlm_VoidPtr value)
{
	ValNodePtr newnode;

	newnode = ValNodeAdd(head);
	if (newnode != NULL)
	{
		newnode->choice = (Nlm_Uint1)choice;
		newnode->data.ptrvalue = value;
	}

	return newnode;
}


/*****************************************************************************
*
*   ValNodeFree(vnp)
*   	frees whole chain of ValNodes
*       Does NOT free associated data pointers
*           see ValNodeFreeData()
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeFree (ValNodePtr vnp)
{
	ValNodePtr next;

	while (vnp != NULL)
	{
		next = vnp->next;
		Nlm_MemFree(vnp);
		vnp = next;
	}
	return NULL;
}

/*****************************************************************************
*
*   ValNodeFreeData(vnp)
*   	frees whole chain of ValNodes
*       frees associated data pointers - BEWARE of this if these are not
*           allocated single memory block structures.
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeFreeData (ValNodePtr vnp)
{
	ValNodePtr next;

	while (vnp != NULL)
	{
		Nlm_MemFree(vnp->data.ptrvalue);
		next = vnp->next;
		Nlm_MemFree(vnp);
		vnp = next;
	}
	return NULL;
}

/*****************************************************************************
*
*   ValNodePtr ValNodeExtract(headptr, choice)
*       removes first node in chain where ->choice == choice
*       rejoins chain after removing the node
*       sets node->next to NULL
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeExtract (ValNodePtr PNTR headptr, Nlm_Int2 choice)
{
    ValNodePtr last = NULL,
        vnp = * headptr;

    while (vnp != NULL)
    {
        if (vnp->choice == (Nlm_Uint1)choice)
        {
            if (last == NULL)    /* first one */
                * headptr = vnp->next;
            else
                last->next = vnp->next;

            vnp->next = NULL;
            return vnp;
        }
        else
        {
            last = vnp;
            vnp = vnp->next;
        }
    }

    return NULL;    /* not found */
}

/*****************************************************************************
*
*   ValNodePtr ValNodeExtractList(headptr, choice)
*       removes ALL nodes in chain where ->choice == choice
*       rejoins chain after removing the nodes
*       returns independent chain of extracted nodes
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeExtractList (ValNodePtr PNTR headptr, Nlm_Int2 choice)
{
    ValNodePtr last = NULL, first = NULL, vnp;

    while ((vnp = ValNodeExtract(headptr, choice)) != NULL)
    {
		if (last == NULL)
		{
			last = vnp;
			first = vnp;
		}
		else
			last->next = vnp;
		last = vnp;
	}

    return first;
}

/*****************************************************************************
*
*   ValNodeFindNext (head, curr, choice)
*   	Finds next ValNode with vnp->choice == choice after curr
*       If curr == NULL, starts at head of list
*       If choice < 0 , returns all ValNodes
*       Returns NULL, when no more found
*
*****************************************************************************/
ValNodePtr LIBCALL ValNodeFindNext (ValNodePtr head, ValNodePtr curr, Nlm_Int2 choice)
{
	if (head == NULL) return NULL;

	if (curr == NULL)
		curr = head;
	else
		curr = curr->next;

	while (curr != NULL)
	{
		if ((choice < 0) || (curr->choice == (Nlm_Uint1)choice))
			return curr;
		curr = curr->next;
	}

	return curr;
}

/*****************************************************************************
*
*   Start Of Node List Functions
*
*****************************************************************************/

ValNodePtr LIBCALL NodeListNew (void)

{
  ValNodePtr  vnp;

  vnp = ValNodeNew (NULL);
  return vnp;
}

ValNodePtr LIBCALL NodeListFree (ValNodePtr head)

{
  if (head != NULL) {
    ValNodeFreeData (head);
  }
  return NULL;
}

Nlm_Int2 LIBCALL NodeListLen (ValNodePtr head)

{
  Nlm_Int2  item;

  item = 0;
  if (head != NULL) {
    while (head->next != NULL) {
      head = head->next;
      item++;
    }
  }
  return item;
}

ValNodePtr LIBCALL NodeListFind (ValNodePtr head, Nlm_Int2 item, Nlm_Boolean extend)

{
  ValNodePtr  vnp;

  vnp = NULL;
  if (head != NULL && item > 0) {
    vnp = head;
    while (vnp->next != NULL && item > 0) {
      vnp = vnp->next;
      item--;
    }
    if (extend) {
      while (item > 0) {
        vnp = ValNodeNew (vnp);
        item--;
      }
    } else if (item > 0) {
      vnp = NULL;
    }
  }
  return vnp;
}

Nlm_Boolean LIBCALL NodeListRead (ValNodePtr head, Nlm_Int2 item, Nlm_VoidPtr ptr, Nlm_sizeT size)

{
  Nlm_Boolean  copied;
  Nlm_BytePtr  dst;
  Nlm_BytePtr  src;
  ValNodePtr   vnp;

  copied = FALSE;
  if (head != NULL && item > 0 && ptr != NULL) {
    vnp = NodeListFind (head, item, FALSE);
    if (vnp != NULL && vnp->data.ptrvalue != NULL) {
      dst = (Nlm_BytePtr) ptr;
      src = (Nlm_BytePtr) (vnp->data.ptrvalue);
      while (size > 0) {
        *dst = *src;
        dst++;
        src++;
        size--;
      }
      copied = TRUE;
    } else {
      Nlm_MemFill (ptr, 0, size);
    }
  }
  return copied;
}

static Nlm_Boolean LIBCALL Nlm_WriteToNode (ValNodePtr vnp, Nlm_VoidPtr ptr, Nlm_sizeT size)

{
  Nlm_Boolean  copied;
  Nlm_BytePtr  dst;
  Nlm_BytePtr  src;

  copied = FALSE;
  if (vnp != NULL) {
    vnp->data.ptrvalue = MemFree (vnp->data.ptrvalue);
    if (ptr != NULL) {
      vnp->data.ptrvalue = MemNew (size);
      if (vnp->data.ptrvalue != NULL) {
        dst = (Nlm_BytePtr) (vnp->data.ptrvalue);
        src = (Nlm_BytePtr) ptr;
        while (size > 0) {
          *dst = *src;
          dst++;
          src++;
          size--;
        }
        copied = TRUE;
      }
    }
  }
  return copied;
}

Nlm_Boolean LIBCALL NodeListWrite (ValNodePtr head, Nlm_Int2 item, Nlm_VoidPtr ptr, Nlm_sizeT size)

{
  Nlm_Boolean  copied;
  ValNodePtr   vnp;

  copied = FALSE;
  if (head != NULL && item > 0 && ptr != NULL) {
    vnp = NodeListFind (head, item, TRUE);
    copied = Nlm_WriteToNode (vnp, ptr, size);
  }
  return copied;
}

Nlm_Boolean LIBCALL NodeListAppend (ValNodePtr head, Nlm_VoidPtr ptr, Nlm_sizeT size)

{
  Nlm_Boolean  copied;
  ValNodePtr   vnp;

  copied = FALSE;
  if (head != NULL && ptr != NULL) {
    vnp = ValNodeNew (head);
    copied = Nlm_WriteToNode (vnp, ptr, size);
  }
  return copied;
}

Nlm_Boolean LIBCALL NodeListInsert (ValNodePtr head, Nlm_Int2 item, Nlm_VoidPtr ptr, Nlm_sizeT size)

{
  Nlm_Boolean  copied;
  ValNodePtr   prev;
  ValNodePtr   vnp;

  copied = FALSE;
  if (head != NULL && item > 0 && ptr != NULL) {
    if (item > 1) {
      prev = NodeListFind (head, item - 1, FALSE);
    } else {
      prev = head;
    }
    if (prev != NULL) {
      vnp = ValNodeNew (NULL);
      if (vnp != NULL) {
        vnp->next = prev->next;
        prev->next = vnp;
        copied = Nlm_WriteToNode (vnp, ptr, size);
      }
    }
  }
  return copied;
}

Nlm_Boolean LIBCALL NodeListReplace (ValNodePtr head, Nlm_Int2 item, Nlm_VoidPtr ptr, Nlm_sizeT size)

{
  Nlm_Boolean  copied;
  ValNodePtr   vnp;

  copied = FALSE;
  if (head != NULL && item > 0 && ptr != NULL) {
    vnp = NodeListFind (head, item, FALSE);
    copied = Nlm_WriteToNode (vnp, ptr, size);
  }
  return copied;
}

Nlm_Boolean LIBCALL NodeListDelete (ValNodePtr head, Nlm_Int2 item)

{
  Nlm_Boolean  deleted;
  ValNodePtr   prev;
  ValNodePtr   vnp;

  deleted = FALSE;
  if (head != NULL && item > 0) {
    if (item > 1) {
      prev = NodeListFind (head, item - 1, FALSE);
    } else {
      prev = head;
    }
    if (prev != NULL) {
      vnp = prev->next;
      if (vnp != NULL) {
        prev->next = vnp->next;
        Nlm_MemFree (vnp->data.ptrvalue);
        Nlm_MemFree (vnp);
        deleted = TRUE;
      }
    }
  }
  return deleted;
}


/*****************************************************************************
*
*   End Of Node List Functions
*
*****************************************************************************/


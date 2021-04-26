/* casn.c
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
* File Name: casn.c
*
* Author:  Greg Schuler
*
* Version Creation Date: 9/23/92
*
* $Revision: 2.4 $
*
* File Description:
	functions to decompress a compressed ASN,1 (CASN) file.
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
* 04-21-93 Schuler     CASN_ReadBuff declared as LIBCALLBACK
* 06-28-93 Schuler     New function:  CASN_Seek().
*
* ==========================================================================
*/

#include <asn.h>
#include <casn.h>

struct casn_ioblock
{
	Int2     magic;
	Int2     compr;
	Int4     doc_count;
	Int2     doc_type;
	Int4     uid_min;
	Int4     uid_max;
	Int2    *huff_left;
	Int2    *huff_right;
	FILE    *fd;
	AsnIoPtr aio;
	AsnType *atp;
	Int4	 bytes;
	Uint2	 byte;
	Uint2	 mask;
};

/*
	Header structure:

	Int2 magic_number 		0x11BD
	Int2 header_format		1
	Int4  doc_count			number of docs Int2 the file
	Int4  uid_min;			smallest UID in this file
	Int4  uid_max;			largest UID in this file
	Int2 doc_type			1 = "Medline-entry, 2= "Seq-entry"
	Int2 huffman_count
	Int2 huffman_left[]
	Int2 huffman_right[]
*/

#define	ERR_CONTEXT		(CTX_RESERVED+1)

#define CURRENT_FILEFORMAT	1
#define MAGIC_FILEFORMAT	4541
#define MAGIC_IOBLOCK		3958

enum CASN_Compr { CASN_ComprNone, CASN_ComprHuff };

Int2 _huff_count;
static char * _asn_type[] = { "", "Medline-entry", "Seq-entry" };
static char * file_emsg = "Unrecognized compressed file format [%s]\n";

static Int2 compr_none_read (CASN_Handle handle, CharPtr buff, Int2 count);
static Int2 compr_huff_read (CASN_Handle handle, CharPtr buff, Int2 count);

#define HUFFMAN_SENTINEL	_huff_count

Int2 LIBCALLBACK CASN_ReadBuff(Pointer param, CharPtr buffer, Uint2 count);

static Uint4 rd_integer (FILE *fd, Int2 bytes);
#define RD_SHORT(f)   (Int2)rd_integer(fd,2)
#define RD_USHORT(f)  (Uint2)rd_integer(fd,2)
#define RD_LONG(f)    (Int4)rd_integer(fd,4)
#define RD_ULONG(f)   rd_integer(fd,4)



/* --------------- High-Level Functions --------------- */

CASN_Handle LIBCALL CASN_Open(CharPtr fname)
{
	Int2	i, j;
	CASN_Handle handle;
	FILE *fd =NULL;
	Int2	doc_type;
	Int4	l1, l2, l3;

	if (!(fd = FileOpen(fname,"rb")))
	{
		ErrPost(ERR_CONTEXT,CASN_ErrFileOpen,"Unable to open file %s\n",fname);
		return NULL;
	}

	/* check to see that the file is recognizable */
	i = RD_SHORT(fd);
	j = RD_SHORT(fd);
	if (i != MAGIC_FILEFORMAT  ||  j > CURRENT_FILEFORMAT)
	{
		FileClose(fd);
		ErrPost(ERR_CONTEXT,CASN_ErrFileFormat,file_emsg,fname);
		return NULL;
	}

	l1 = RD_LONG(fd);
	l2 = RD_LONG(fd);
	l3 = RD_LONG(fd);

	doc_type = RD_SHORT(fd);
	_huff_count = RD_SHORT(fd);

	if (!(handle = CASN_New(doc_type,_huff_count)))
	{
		FileClose(fd);
		return NULL;
	}

	for (i=0; i<_huff_count; ++i)
		handle->huff_left[i] = (Int2) RD_SHORT(fd);
	for (i=0; i<_huff_count; ++i)
		handle->huff_right[i] = (Int2) RD_SHORT(fd);

	if (!(handle->aio = AsnIoNew(ASNIO_BIN+ASNIO_IN,NULL,handle,CASN_ReadBuff,NULL)))
	{
		FileClose(fd);
		CASN_Free(handle);
		return NULL;
	}
	handle->atp = AsnTypeFind(AsnAllModPtr(),_asn_type[doc_type]);

	handle->fd = fd;
	handle->doc_count = l1;
	handle->uid_min = l2;
	handle->uid_max = l3;
	return handle;
}

void LIBCALL CASN_Close (CASN_Handle handle)
{
	AsnIoClose(handle->aio);
	FileClose(handle->fd);
	CASN_Free(handle);
}

AsnIoPtr LIBCALL CASN_GetAsnIoPtr (CASN_Handle handle)
{
	return handle->aio;
}

Int2 LIBCALL CASN_DocType(CASN_Handle handle)
{
	return handle->doc_type;
}

Int4 LIBCALL CASN_DocCount(CASN_Handle handle)
{
	return handle->doc_count;
}

MedlineEntryPtr LIBCALL CASN_NextMedlineEntry(CASN_Handle handle)
{
	AsnTypePtr atp;

	atp = AsnReadId(handle->aio,AsnAllModPtr(),handle->atp);
	return atp ? MedlineEntryAsnRead(handle->aio,atp) : NULL;
}


SeqEntryPtr LIBCALL CASN_NextSeqEntry(CASN_Handle handle)
{
	AsnTypePtr atp;

	atp = AsnReadId(handle->aio,AsnAllModPtr(),handle->atp);
	return atp ? SeqEntryAsnRead(handle->aio,atp) : NULL;
}


int LIBCALL CASN_Seek (CASN_Handle handle, long offset, int origin)
{
	handle->compr = -1;          /* to reset the Huffman state */
	AsnIoReset(handle->aio);     /* to reset the ASN state */
	return fseek(handle->fd,offset,origin);
}


/* --------------- Low-Level Functions --------------- */

CASN_Handle  LIBCALL CASN_New (Int2 doc_type, Int2 huff_count)
{
	CASN_Handle handle;
	Int2Ptr	left;
	Int2Ptr 	right;

	if (!(handle = MemNew(sizeof(struct casn_ioblock))))
		return NULL;
	if (!(left = MemNew(huff_count*sizeof(Int2))))
		return NULL;
	if (!(right = MemNew(huff_count*sizeof(Int2))))
		return NULL;

	handle->magic = MAGIC_IOBLOCK;
	handle->doc_type = doc_type;
	handle->compr = -1;
	handle->huff_left = left;
	handle->huff_right = right;
	return handle;
}


void LIBCALL CASN_Free (CASN_Handle handle)
{
	MemFree(handle->huff_left);
	MemFree(handle->huff_right);
	MemFree(handle);
}


Int2 LIBCALLBACK CASN_ReadBuff (Pointer param, CharPtr buff, Uint2 count)
{
	CASN_Handle handle = (CASN_Handle) param;
	Int2 retval = 0;

	while (! retval)   /* has to allow for 0 bytes from compressed read */
	{
	if (handle->compr < 0)
	{
		Int2 c;

		/* read the "decompression protocol identifier" */
		if ((c = fgetc(handle->fd)) == EOF)
			return 0;

		if (c == CASN_ComprNone)
		{
			handle->bytes = (Int4) rd_integer(handle->fd,3);
		}
		else if (c == CASN_ComprHuff)
		{
			handle->byte = 0;
			handle->mask = 0;
		}
		else
		{
			ErrPost(ERR_CONTEXT,CASN_ErrFileFormat,file_emsg,"ReadBuff");
			return 0;
		}
		handle->compr = c;
	}

	switch(handle->compr)
	{
		case CASN_ComprNone:
			return compr_none_read(handle,buff,count);

		case CASN_ComprHuff:
			retval = compr_huff_read(handle,buff,count);
			if (retval)
				return retval;
			break;

		default:
			ErrPost(ERR_CONTEXT,CASN_ErrFileFormat,file_emsg,"ReadBuff");
			return 0;
	}
	}

	return 0;
}


static Int2 compr_none_read (CASN_Handle handle, CharPtr buff, Int2 count)
{
	size_t bytes = (size_t) MIN(handle->bytes,(Int4)count);

	bytes = FileRead(buff,1,bytes,handle->fd);
	handle->bytes -= bytes;
	if (handle->bytes <= 0)
	{
		/* reset for stream read of next entry */
		handle->compr = -1;
	}
	return bytes;
}


static Int2 compr_huff_read (CASN_Handle handle, CharPtr buff, Int2 count)
{
	register Uint2 mask = handle->mask;
	register Uint2 byte = handle->byte;
	FILE *fd = handle->fd;
	CharPtr p = buff;
	Int2 i, cnt = 0;
	Int2 c;
	Int2 k;


	while (cnt < count)
	{
		for (i=0; i>=0; )
		{
			if (mask == 0)
			{
				if ((c = fgetc(fd)) == EOF)
				{
					/* should never reach this poInt2 */
					i = HUFFMAN_SENTINEL - 257;
					break;
				}
				else
				{
					byte = (Uint2) c;
					mask = 0x80;
				}
			}

			if (byte & mask)
				i = handle->huff_left[i];
			else
				i = handle->huff_right[i];

			mask >>= 1;
		}

		if ((k = i + 257) == HUFFMAN_SENTINEL)
		{
			handle->compr = -1; /* reset for next record */
			break;
		}

		*p++ = (char) k;
		cnt++;
	}

	handle->mask = mask;
	handle->byte = byte;

	return cnt;
}

static Uint4 rd_integer (FILE *fd, Int2 bytes)
{
	Int2 i, c;
	Uint4 value = 0;

	for (i=0; i<bytes; ++i)
	{
		if ((c = fgetc(fd)) ==EOF)  break;
		value <<= 8;
		value |= c;
	}
	return value;
}



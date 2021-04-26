// DSequence.cp 
// d.g.gilbert



#include "DMegaSequence.h"
#include "DSeqFile.h"
#include "DREnzyme.h"
#include <ncbi.h>
#include <dgg.h>
#include <DUtil.h>
#include <DFile.h>
#include "ureadseq.h"
 









// DMegaSequence -----------------


DMegaSequence::DMegaSequence()
{
	fIsMega		= TRUE;
}


DMegaSequence::~DMegaSequence()  
{
}


DObject* DMegaSequence::Clone() // override 
{	
	DMegaSequence* aSeq= (DMegaSequence*) DSequence::Clone();
#if 0
	aSeq->fBases= NULL; aSeq->SetBases( fBases, true);  
	aSeq->fMasks= NULL; aSeq->SetMasks( fMasks, true); 
	aSeq->fInfo=  StrDup( fInfo);
	aSeq->fName=  StrDup( fName);
	aSeq->fSearchRec.targ= StrDup( fSearchRec.targ);
#endif
	return aSeq;
}

void DMegaSequence::CopyContents( DSequence* fromSeq)
{
	DSequence::CopyContents( fromSeq);
}


char* DMegaSequence::Bases() const {
	return fBases; // fix later
	}

char* DMegaSequence::Masks() const {
	return fMasks; // fix later
	}
	
	
enum { kHugePartSize = 3000000 }; // = kMegaPartSize 

void DMegaSequence::countparts( char* seq, long& len, long& nparts, long& partsize)
{
	char* cp= seq;
	partsize= kHugePartSize;  
	len= 0;
	nparts= 0;
	if (!seq) return;
	
	while ( *cp != '\0' ) {
		long  n;
		for (n=0; *cp && n<kHugePartSize; n++) cp++;
		len += n;
		nparts++;	
		}	
	if (nparts == 1) partsize= len;
}


void DMegaSequence::SetBases( char*& theBases, Boolean duplicate)
{
	long  newlen= (theBases) ? StrLen(theBases) : 0;
	if (fBases) { 
		if (fLength>0 && 
			(fLength != newlen || MemCmp( theBases, fBases, newlen)!= 0))
				fChanged= TRUE;
		MemFree(fBases); 
		fBases= NULL; fLength= 0; 
		}

	if (theBases) {
		countparts( theBases, fLength, fNparts, fPartsize);
		
		if (fNparts==1) {
			if (duplicate) fBases = StrDup(theBases);  
			else { fBases= theBases; theBases= NULL; }
			}
			
		else if (fNparts>1) {
				// temp for testing !
			fLength= fPartsize;
			fBases= (char*)MemNew( fLength+1);
			MemCpy( fBases, theBases, fLength);
			fBases[fLength]= 0;
			}
		
		}
	fModTime= gUtil->time();
	fMasksOk= false; // assume SetBases allways called before SetMask...
}


void DMegaSequence::UpdateBases( char* theBases, long theLength)
{
	fBases =  theBases;  
	fLength= theLength;
	fMasksOk= false; // assume this allways called before SetMask...
}


void DMegaSequence::SetMasks( char*& theMasks, Boolean duplicate)
{
	if (fMasks) MemFree(fMasks);
	if (theMasks) {
		if (duplicate) fMasks = StrDup(theMasks);  
		else { fMasks= theMasks; theMasks= NULL; }
		FixMasks();
		}
	else {
		fMasks= NULL;
		fMasksOk= false;
		}
}

void DMegaSequence::FixMasks()
{
	ulong i, mlen= 0;
	if (!fMasks) { 
		mlen= 1+fLength;
		fMasks= (char*) MemGet(mlen,true);  
		for (i=0; i<mlen; i++) fMasks[i] |= 0x80; // set high bit so Strxxx sees it as non-null
		fMasks[mlen-1]= 0;
		}
	else { 
		mlen= 1+StrLen(fMasks);
		if (fLength+1 != mlen) {
			fMasks= (char*) Nlm_MemExtend( fMasks, fLength+1, mlen);
			mlen= fLength+1; 
			for (i=0; i<mlen; i++) fMasks[i] |= 0x80; 
			fMasks[mlen-1]= 0;
			} 
		}
	fMasksOk= true;
}

short DMegaSequence::MaskAt(long baseindex, short masklevel)
{
	short b = kMaskEmpty;
	if (fMasks && fMasksOk && baseindex>=0 && baseindex<fLength) {
		b= (unsigned char) fMasks[baseindex];
		switch (masklevel) {
			case 0: b &= 0x7f; break; // return full mask - top (0x80) bit
			case 1: b &= 1;  break;  
			case 2: b &= 2;  break;
			case 3: b &= 4;  break;
			case 4: b &= 8;  break;
			default: b = kMaskEmpty; break;
			}
		}
	return b;
}

Boolean DMegaSequence::IsMasked(unsigned char maskbyte, short masklevel)
{
	short b= (unsigned char) maskbyte;
	switch (masklevel) {
		case 0: b &= 0x7f; break; // return full mask - top (0x80) bit
		case 1: b &= 1;  break;  
		case 2: b &= 2;  break;
		case 3: b &= 4;  break;
		case 4: b &= 8;  break;
		default: b = kMaskEmpty; break;
		}
	return (b > 0);
}


inline void SetMaskByte( char& b, short masklevel, short maskval)
{
	switch (masklevel) {
		case 1: if (maskval) b |= 1; else b &= ~1; break;
		case 2: if (maskval) b |= 2; else b &= ~2; break;
		case 3: if (maskval) b |= 4; else b &= ~4; break;
		case 4: if (maskval) b |= 8; else b &= ~8; break;
		// reserve bits 5, 6, 7 & 8 for now 
		// -- 7 & 8 may go unused to store data in printchar form
		default: break;
		}
	//if (b) masklevel--; // debugging b val
}

void DMegaSequence::SetMaskAt(long baseindex, short masklevel, short maskval)
{
	if (fMasks && fMasksOk && baseindex>=0 && baseindex<fLength)  
		::SetMaskByte(fMasks[baseindex],masklevel, maskval);
}


void DMegaSequence::SetMask(short masklevel, short maskval)
{
	long  baseindex;
	if (fMasks && fMasksOk) 
	 for (baseindex=0; baseindex<fLength; baseindex++) 
		::SetMaskByte(fMasks[baseindex], masklevel, maskval);
}


inline void FlipMaskByte( char& b, short masklevel)
{
	switch (masklevel) {
		case 1: b ^= 1;  break;  
		case 2: b ^= 2;  break;
		case 3: b ^= 4;  break;
		case 4: b ^= 8;  break;
		default:  break;
		}
	//if (b) masklevel--; // debugging b val
}

void DMegaSequence::FlipMaskAt(long baseindex, short masklevel)
{
	if (fMasks && fMasksOk && baseindex>=0 && baseindex<fLength)  
		::FlipMaskByte(fMasks[baseindex],masklevel);
}

void DMegaSequence::FlipMask(short masklevel)
{
	long  baseindex;
	if (fMasks && fMasksOk) 
	 for (baseindex=0; baseindex<fLength; baseindex++)  
		FlipMaskByte(fMasks[baseindex],masklevel);
}



void DMegaSequence::UpdateFlds()
{
	long 		seqlen;
	short 	kind;
	unsigned long check;
	fLength= StrLen(fBases);
	GetSeqStats(fBases, fLength, &kind, &check, &seqlen);
	//fValidLength= seqlen; //?? is fLength == StrLen(fBases) or is it <= that, only valid bases??
	fKind			= kind;
	fChecksum	= check;
	fChanged	= FALSE;
}





void DMegaSequence::SetSelection( long start, long nbases)
{
	fSelStart= start;
	fSelBases= nbases;
}

void DMegaSequence::ClearSelection()
{
	fSelStart= 0;
	fSelBases= 0;
}

void	DMegaSequence::GetSelection( long& start, long& nbases)
{
	if (fSelBases==0) {
		start= 0;
		nbases= fLength;
		}	
	else if (fLength>0) {
		start= fSelStart;
		nbases= Min( fSelBases, fLength - fSelStart);
		}	
	else {
		start= 0;
		nbases= 0;
		}
}


DSequence* DMegaSequence::CopyRange()
{  
			//note: start == 0 == 1st position 
	DMegaSequence* aSeq= (DMegaSequence*) Clone();
	if (fSelStart > 0 || fSelBases > 0) {
		char* h= aSeq->fBases;
		char* m= aSeq->fMasks;
		Boolean domask= (fMasks && fMasksOk);

		long len= aSeq->fLength;
		if (fSelStart > 0) {
			len -= fSelStart;
			MemMove( h, h + fSelStart, len);
			if (domask) MemMove( m, m + fSelStart, len);
			}
		if (fSelBases > 0 && fSelBases < len) len= fSelBases;
		h= (char*) MemMore( h, len+1);
		h[len]= '\0'; //((char*)h+len) = '\0';
		aSeq->fBases= h;
		if (domask) {
			m= (char*) MemMore( m, len+1);
			m[len]= '\0';    
			aSeq->fMasks= m;
			}
		}
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}


DSequence* DMegaSequence::CutRange()
{
/* aSeq.Cutrange means:  
		(a) selected range (fSelStart..fSelStart+fSelBases) is copied into Result; &&
		(b) selected range is removed from this
*/
 
	DMegaSequence* aSeq= (DMegaSequence*) CopyRange();
	if (fSelStart > 0 || fSelBases > 0) {
		if (fSelStart<0) fSelStart= 0;
		char* h= this->fBases;
		char* m= this->fMasks;
		Boolean domask= (fMasks && fMasksOk);
		long len= this->fLength;
		long top= len - fSelStart - fSelBases;
		if (fSelBases > 0 && fSelBases < len) {
			MemMove( h+fSelStart, h+fSelStart+fSelBases, top);
			if (domask) MemMove( m+fSelStart, m+fSelStart+fSelBases, top);
			}
		len= fSelStart + top;
		h= (char*) MemMore( h, len+1);
		*((char*)h+len)= '\0';   	//h[len]= '\0';
		this->fBases= h;
		if (domask) {
			m= (char*) MemMore( m, len+1);
			*((char*)m+len)= '\0';    
			this->fMasks= m;
			}
		}
	UpdateFlds();	
	Modified();
	return aSeq;
}

void DMegaSequence::InsertSpacers( long insertPoint, long insertCount, char spacer)
{ 
	long i;
	char *cp;
	char* h= this->fBases;
	char* m= this->fMasks;
	Boolean domask= (fMasks && fMasksOk);
	long len= this->fLength;
	if (insertPoint<0) insertPoint= 0;
	else if (insertPoint>len-1) insertPoint= len-1;
	h= (char*) MemMore( h, len+insertCount+1);
	cp = h+insertPoint;
	MemMove( cp+insertCount, cp, len-insertPoint);
	for ( i= 0; i<insertCount; i++)  *cp++= spacer;
	h[len+insertCount]= '\0';
	this->fBases= h;
	if (domask) {
		m= (char*) MemMore( m, len+insertCount+1);
		cp = m+insertPoint;
		char cmask= *cp;
		MemMove( cp+insertCount, cp, len-insertPoint);
		for ( i= 0; i<insertCount; i++)  *cp++= cmask;
		m[len+insertCount]= '\0';
		this->fMasks= m;
		}
	this->UpdateFlds();	
	this->Modified();
}


DSequence* DMegaSequence::PasteBases( long insertPoint, char* fromBases, char* fromMasks)
{ 
	long add;
	DMegaSequence* aSeq= (DMegaSequence*) Clone();
	if (fromBases) add= StrLen( fromBases);  else add= 0;
	if (add>0) {
		char* h= aSeq->fBases;
		char* m= aSeq->fMasks;
		Boolean domask= (fMasks && fMasksOk);
		long len = aSeq->fLength;
		if (insertPoint<=0) {
			fromBases= StrDup( fromBases);
			StrExtendCat( &fromBases, h);
			MemFree( h);
			h= fromBases;
			if (domask && fromMasks) {
				fromMasks= StrDup( fromMasks);
				StrExtendCat( &fromMasks, m);
				MemFree( m);
				m= fromMasks;
				}
			}		
		else if (insertPoint>=len) { 
			StrExtendCat( &h, fromBases);
			if (domask) StrExtendCat( &m, fromMasks);
			}
		else { 
			h= (char*) MemMore( h, len+add);
			MemMove( h+insertPoint+add, h+insertPoint, len-insertPoint);
			MemMove( h+insertPoint+add, fromBases, add);
			*((char*)h+len+add)= '\0';
			if (domask) {
				m= (char*) MemMore( m, len+add);
				MemMove( m+insertPoint+add, m+insertPoint, len-insertPoint);
				MemMove( m+insertPoint+add, fromMasks, add);
				*((char*)m+len+add)= '\0';
				}
			}
		aSeq->fBases= h;
		aSeq->fMasks= m;
		}
	aSeq->UpdateFlds();	
	aSeq->Modified();
	return aSeq;
}


DSequence* DMegaSequence::Reverse()
{
	long i;
	DMegaSequence* aSeq= (DMegaSequence*) Clone();	
	if (fSelStart < 0) fSelStart= 0;
	long len= aSeq->fLength - fSelStart;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;
	char* pc= this->fBases+fSelStart + len;
	char* pr= aSeq->fBases+fSelStart;
	for (i= 0; i<len; i++) *pr++= *--pc;
	Boolean domask= (fMasks && fMasksOk);
	if (domask) {
		pc= this->fMasks+fSelStart + len;
		pr= aSeq->fMasks+fSelStart;
		for (i= 0; i<len; i++) *pr++= *--pc;
		}
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}


DSequence* DMegaSequence::Complement()
// A->T, T->A, G->C, C->G  
{
	Boolean		isrna;
	DMegaSequence* aSeq= (DMegaSequence*) Clone();	
	
	if (fKind == kRNA) isrna= TRUE;
	else if (fKind == kDNA || fKind == kNucleic) isrna= FALSE;
	else return aSeq;	
		
	if (fSelStart < 0) fSelStart= 0;
	long len= aSeq->fLength - fSelStart;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;
	char* pc= this->fBases+fSelStart;
	char* pr= aSeq->fBases+fSelStart;
	NucleicComplement( isrna, pc, pr, len);
		
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}



DSequence* DMegaSequence::ChangeCase(Boolean toUpper)
// g->G or G->g
{
	DMegaSequence* aSeq= (DMegaSequence*) Clone();	
	
	if (fSelStart < 0) fSelStart= 0;
	char* h= aSeq->fBases;
	long len= aSeq->fLength - fSelStart;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;
	char* pc= this->fBases+fSelStart;
	char* pr= aSeq->fBases+fSelStart;
	
	long i;
	if (toUpper) for (i= 0; i<len; i++) {
		*pr++= toupper(*pc); pc++;
		}
	else for (i= 0; i<len; i++) {
		*pr++= tolower(*pc); pc++;
		}	
		
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}

DSequence* DMegaSequence::Dna2Rna(Boolean toRna)
// T->U or U->T 
{
	Boolean		isrna;
	DMegaSequence* aSeq= (DMegaSequence*) Clone();	
	
	if (fKind==kRNA) isrna= TRUE;
	else if (fKind == kDNA || fKind == kNucleic) isrna= FALSE;
	else return aSeq;	
	 
	if (fSelStart < 0) fSelStart= 0;
	long len= aSeq->fLength - fSelStart;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;
	char* pc= this->fBases+fSelStart;
	char* pr= aSeq->fBases+fSelStart;
	
	long i;
	char b, c;
	if (toRna) for (i= 0; i<len; i++) {
		b= *pc++;
		if (b=='T') c= 'U';
		else if (b=='t') c= 'u';
		else c= b;
		*pr++= c;
		}
	else for (i= 0; i<len; i++) {
		b= *pc++;
		if (b=='U') c= 'T';
		else if (b=='u') c= 't';
		else c= b;
		*pr++= c;
		}	
		
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}


DSequence* DMegaSequence::OnlyORF(char nonorf)
// convert sequence to ORF part only 
{
	return DSequence::OnlyORF(nonorf);	
}

DSequence* DMegaSequence::LockIndels( Boolean doLock)
// indelSoft -> indelHard or vice versa 
{	
	char b, c, fromc, toc;
	DMegaSequence* aSeq= (DMegaSequence*) Clone();	
	
	if (fSelStart < 0) fSelStart= 0;
	long  len= aSeq->fLength - fSelStart;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;
	char* pc= this->fBases+fSelStart;
	char* pr= aSeq->fBases+fSelStart;
	
	if (doLock) { fromc= indelSoft; toc= indelHard; }
	else { fromc= indelHard; toc= indelSoft; }
	
	for (long i= 0; i<len; i++) {
		b= *pc++;
		if (b == fromc) c= toc;
		else c= b;
		*pr++= c;
		}	
		
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}


DSequence* DMegaSequence::Slide( long slideDist)
/* insert "-" below (+dist) or above (-dist) of seq range,
	squeeze out "-" above (below) of range
*/
{ 	
	long 		slideSave, newlen, fulllen, len, i, j= 0;
	char 		b, cutchar, cutmask;
	char		*pc, *pr;
	
	DMegaSequence* aSeq= (DMegaSequence*) Clone();	
	if (fSelStart < 0) fSelStart= 0;
	fulllen= aSeq->fLength;
	len= fulllen - fSelStart;
	Boolean domask= (fMasks && fMasksOk);
	Boolean dosqueeze= true; //(indelSoft & 0x80 != 0); // quick fix flag
	indelSoft &= 0x7f;
	
	if (slideDist < 0) {
		// tough... do if below works...
		// really need ability to extend seq in 5' (lower) direction 
		//! build pr in reverse, then flip it....

		if (fSelBases > 0 && fSelBases < len) len= fSelBases;
		slideDist= -slideDist;
		slideSave= slideDist;
		aSeq->fBases= (char*) MemMore(aSeq->fBases, 2+fulllen+slideDist);
		pr= aSeq->fBases;
		pc= this->fBases + fulllen;
		for (i= fulllen-1; i >= fSelStart+len; i--) *pr++= *--pc;
			 
		if (pr == aSeq->fBases || *(pr-1) == indelEdge) cutchar= indelEdge;
		else cutchar= indelSoft; //??
		for (i=0; i<slideDist; i++) *pr++= cutchar;

		pc= this->fBases+fSelStart+len;
		for (i= fSelStart+len-1; i >= fSelStart; i--) *pr++= *--pc;
			
		pc= this->fBases+fSelStart;
		for (i= fSelStart-1; i>=0; i--) {
			b= *--pc;
			if (dosqueeze && slideDist>0 && (b==indelSoft || b==indelEdge)) slideDist--;
			else {
				*pr++= b;
				}
			}

		newlen= pr - aSeq->fBases;
		char* htmp= (char*) MemNew(newlen+1);
		pc= htmp;
		for (i=0; i<newlen; i++) *pc++= *--pr; //pr[j-i+1];
		pc= '\0';
		MemFree(aSeq->fBases);
		aSeq->fBases = htmp;  
		aSeq->fLength= newlen;
		aSeq->fOrigin= fulllen - newlen; //== this->fLength - aSeq->fLength

		if (domask) {
			slideDist= slideSave;
			aSeq->fMasks= (char*) MemMore(aSeq->fMasks, 2+fulllen+slideDist);
			pr= aSeq->fMasks;
			pc= this->fMasks + fulllen;
			for (i= fulllen-1; i >= fSelStart+len; i--) *pr++= *--pc;
				 
			cutmask= 0x80; // required empty mask  
			for (i=0; i<slideDist; i++) *pr++= cutmask;
	
			pc= this->fMasks+fSelStart+len;
#ifdef WIN_MSWIN
			// mswin is crashing here for no obvious reason
			//pc--; len--;
			for (i= fSelStart + len - 1; i >= fSelStart; i--) {
				 register short amask;
				 --pc;
				 amask= *pc;    //mswin bomb is here !
				 *pr= amask;
				 pr++;
         }
#else
			for (i= fSelStart+len-1; i >= fSelStart; i--) *pr++= *--pc;
#endif
			pc= this->fMasks+fSelStart;
			for (i= fSelStart-1; i>=0; i--) {
				b= *--pc;
				if (slideDist>0 && (b==cutmask)) slideDist--;
				else {
					*pr++= b;
					}
				}
				
			newlen= pr - aSeq->fMasks;
			htmp= (char*) MemNew(newlen+1);
			pc= htmp;
			for (i=0; i<newlen; i++) *pc++= *--pr; //pr[j-i+1];
			pc= '\0';
			MemFree(aSeq->fMasks);
			aSeq->fMasks = htmp;  
			}
			
		}	
		
	else if (slideDist > 0) {
		fulllen= len;
		if (fSelBases > 0 && fSelBases < len) len= fSelBases;
		aSeq->fBases= (char*) MemMore(aSeq->fBases, 2+fSelStart+fulllen+slideDist); //make it large enough

		pr= aSeq->fBases;
	  if (fSelStart==0 || pr[fSelStart-1]==indelEdge) cutchar= indelEdge; 
		else cutchar= indelSoft; 

		pc= this->fBases+fSelStart;
		pr= aSeq->fBases+fSelStart;

		for (j= 0; j<slideDist; j++) *pr++= cutchar;
		for (i= 0; i<len; i++) *pr++= *pc++; 
	
		for (i= len; i<fulllen; i++) {
			b= *pc++;
			if (dosqueeze && slideDist>0 && (b==indelSoft || b==indelEdge)) 
				slideDist--;
			else {
				*pr++= b;
				}
			}
		
		*pr= 0;
		newlen= pr - aSeq->fBases; 
		aSeq->fBases= (char*)MemMore(aSeq->fBases, newlen+1);  
		
		if (domask) {
			aSeq->fMasks= (char*) MemMore(aSeq->fMasks, 2+fSelStart+fulllen+slideDist);
		  cutmask= 0x80; 
			pc= this->fMasks+fSelStart;
			pr= aSeq->fMasks+fSelStart;
	
			for (j= 0; j<slideDist; j++) *pr++= cutmask;
			for (i= 0; i<len; i++) *pr++= *pc++; 

			for (i= len; i<fulllen; i++) {
				b= *pc++;
				if (slideDist>0 && (b==cutmask)) // this is BAD? must be done w/ base slide !?
					slideDist--;
				else {
					*pr++= b;
					}
				}
			
			*pr= 0;
			newlen= pr - aSeq->fMasks; 
			aSeq->fMasks= (char*)MemMore(aSeq->fMasks, newlen+1);  
			}
		}
			
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}


DSequence* DMegaSequence::Compress()
// pull out "-", ".", ? anything but nuc/aa codes ? 
{
	long  len, fulllen, i;
	char 	b, c;
	char * mc, * mr;
	Boolean domask= (fMasks && fMasksOk);
 
	DMegaSequence* aSeq= (DMegaSequence*) Clone();	
	if (fSelStart < 0) fSelStart= 0;
	len= aSeq->fLength - fSelStart;
	fulllen= len;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;
	char* pc= this->fBases+fSelStart;
	char* pr= aSeq->fBases+fSelStart;
	if (domask) {
		mc= this->fMasks+fSelStart;
		mr= aSeq->fMasks+fSelStart;
		}
		
	if (domask) for (i= 0; i<len; i++) {
		b= *pc++;
		c= *mc++;
		if (b!=indelSoft && b!=indelEdge) { 
			*pr++= b;
			*mr++= c;
			}
		}
	else for (i= 0; i<len; i++) {
		b= *pc++;
		if (b!=indelSoft && b!=indelEdge) 
			*pr++= b;
		}
	for (i=len; i<fulllen; i++) *pr++= *pc++;
	if (domask) for (i=len; i<fulllen; i++) *mr++= *mc++;

	long newlen= pr - aSeq->fBases;
	aSeq->fBases= (char*) MemMore( aSeq->fBases, newlen+1);
	aSeq->fBases[newlen]= 0;
	if (domask) {
		aSeq->fMasks= (char*) MemMore( aSeq->fMasks, newlen+1);
		aSeq->fMasks[newlen]= 0;
		}
		
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}

 


DSequence* DMegaSequence::CompressFromMask(short masklevel)
{
	long  len, fulllen, i;
	char 	b, c;
	char * mc, * mr;
	Boolean domask= (fMasks && fMasksOk && masklevel>0);

	if (!domask) return NULL; // or aSeq ???
	DMegaSequence* aSeq= (DMegaSequence*) Clone();	
	//if (!domask) return aSeq;  
 
	if (fSelStart < 0) fSelStart= 0;
	len= aSeq->fLength - fSelStart;
	fulllen= len;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;
	char* pc= this->fBases+fSelStart;
	char* pr= aSeq->fBases+fSelStart;
	mc= this->fMasks+fSelStart;
	mr= aSeq->fMasks+fSelStart;
		 
	for (i= 0; i<len; i++) {
		b= *pc++;
		c= *mc++;
		if (IsMasked(c, masklevel)) { 
			*pr++= b;
			*mr++= c;
			}
		}
	for (i=len; i<fulllen; i++) *pr++= *pc++;
	for (i=len; i<fulllen; i++) *mr++= *mc++;

	long newlen= pr - aSeq->fBases;
	aSeq->fBases= (char*) MemMore( aSeq->fBases, newlen+1);
	aSeq->fBases[newlen]= 0;
	aSeq->fMasks= (char*) MemMore( aSeq->fMasks, newlen+1);
	aSeq->fMasks[newlen]= 0;
		
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}

 

DSequence* DMegaSequence::Translate(Boolean keepUnselected)
{	
	char  c;
	char	acodon[3];
	Boolean isrna, nocodon;
	char	*pr, *pc, *mr, *mc;
	long	len, fulllen, newlen, i, j, k;
	
	DMegaSequence* aSeq= (DMegaSequence*) Clone();		
	if (DCodons::NotAvailable()) return aSeq; // safer than returning NULL !?
	// !? what do we do w/ fMasks !?
	Boolean domask= (fMasks && fMasksOk);
	
	if (fSelStart < 0) fSelStart= 0;
	fulllen= this->fLength - fSelStart;
	len= fulllen;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;
	
	switch (fKind) {
	
		case kRNA:
		case kDNA:
		case kNucleic: 
			{
			if (keepUnselected) {
				pr= aSeq->fBases + fSelStart; // start at selection, leaving untranslated 5'
				if (domask) mr= aSeq->fMasks + fSelStart;
				}
			else {
				pr= aSeq->fBases;
 				if (domask) mr= mr= aSeq->fMasks;
 				}
 				
			pc= this->fBases + fSelStart;
			if (domask) mc= this->fMasks + fSelStart;
			isrna= (fKind == kRNA);
			i= fSelStart;
			while (i < len) {
				for (j=0; j<3; j++) {
					c= toupper( *pc); pc++; // toupper is sometimes a MACRO ! no (*pc++) !
					if (isrna && c=='U') c= 'T';
					acodon[j]= c;
					}
				i += 3;
				for (k= 0, nocodon= true; k<64; k++) 
					if ( MemCmp(DCodons::codon(k),acodon,3) == 0) { 
						*pr++= DCodons::amino(k); //gCodonTable[k].amino;
						nocodon= false;
						break;
						}
				if (nocodon) *pr++= '?';
				if (domask) {
					*mr++= *mc;
					mc += 3;
					}
				}
				 // copy over untranslated portion
			if (keepUnselected) {
				long savei= i;
				for ( ; i < fulllen; i++)  *pr++= *pc++; 
				if (domask) for ( i= savei; i < fulllen; i++)  *mr++= *mc++; 
				}
			}
			break;
			
		case kAmino: 
			{
			char *bestcodon;
			long  selend;
			
			if (len < fulllen) {
				newlen= fulllen + len*3;
				selend= fSelStart + len;
				}
			else {
				newlen= fulllen * 3;
				selend= len;
				}
			pc= this->fBases + fSelStart;
			aSeq->fBases= (char*) MemMore( aSeq->fBases, newlen);
			if (keepUnselected)
				pr= aSeq->fBases + fSelStart;
			else
				pr= aSeq->fBases;
				
			if (domask) {
				mc= this->fMasks + fSelStart;
				aSeq->fMasks= (char*) MemMore( aSeq->fMasks, newlen);
				if (keepUnselected)
					mr= aSeq->fMasks + fSelStart;
				else
					mr= aSeq->fMasks;
				}
				
			for (i= fSelStart; i<selend; i++) {
				c= toupper( *pc); pc++;// toupper is sometimes a MACRO ! no (*pc++) !
				bestcodon= (char*) DCodons::FindBestCodon(c);
				MemCpy( pr, bestcodon, 3);  
				pr += 3;
				if (domask) {
					for (j=0; j<3; j++) *mr++= *mc;
					mc++;
					}
				}
				 // copy over untranslated portion
			if (keepUnselected) {
				long savei= i;
				for ( ; i < fulllen; i++)  *pr++= *pc++; 
				if (domask) for ( i=savei; i < fulllen; i++)  *mr++= *mc++; 
				}
			}
			break;
		
		default:
			return NULL;
		 
	  }
		
	*pr= '\0';
	newlen= pr - aSeq->fBases;
	aSeq->fBases= (char*) MemMore( aSeq->fBases, newlen);
	if (domask) {
		*mr= '\0';
		newlen= mr - aSeq->fMasks;
		aSeq->fMasks= (char*) MemMore( aSeq->fMasks, newlen);
		}
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;	
}




void DMegaSequence::DoWrite(DFile* aFile, short format)
{  
	if (fLength>0) {
		DSeqFile::WriteSeqWrapper( aFile, fBases, fLength, format, fName);
		if (fMasks && fMasksOk && DSeqFile::gWriteMasks) 
			DSeqFile::WriteMaskWrapper( aFile, fMasks, fLength, format, fName);
		}
}


void DMegaSequence::DoWriteSelection(DFile* aFile, short format)
{
	if (fSelBases==0) 
		DoWrite( aFile, format);
	else if (fLength>0) {
		long aStart= Min( fSelStart, fLength);
		long aLength= Min( fSelBases, fLength - fSelStart);
		DSeqFile::WriteSeqWrapper(  aFile, fBases+aStart, aLength, format, fName);
		if (fMasks && fMasksOk && DSeqFile::gWriteMasks)  
			DSeqFile::WriteMaskWrapper( aFile, fMasks+aStart, aLength, format, fName);
		}
}









void DMegaSequence::SearchORF( long& start, long& stop) 
{
	char  startc[2];
	long  i, j, k;
	short	ns;
	CodonStat  stops[10];
	
	start= kSearchNotFound;
	stop = kSearchNotFound;
	if (DCodons::NotAvailable()) return;
	
	for (ns= 0, i= 0; i<64; i++) {
		if (ns<10 && DCodons::amino(i) == '*') 
			stops[ns++] = DCodons::fCodons[i];
		}
	if (ns == 0) return;
		
	switch (fKind) {  
		case kRNA:
		case kDNA:
		case kNucleic: 
	 		start= Search( DCodons::startcodon(), FALSE);
	 		if (start == kSearchNotFound) return;
	 		for (j= start+3; j<fLength; j += 3) {
	 			for (k=0; k<ns; k++) {
	 				if (StrNICmp( fBases+j, stops[k].codon, 3) == 0) {
	 					stop= j+3; // +3 to include this codon
	 					return;
	 					}
	 				}
	 			}
			break;
			
		case kAmino:
			startc[0]= DCodons::startamino();
			startc[1]= 0;
	 		start= Search( startc, FALSE);
	 		if (start == kSearchNotFound) return;
	 		for (j= start+1; j<fLength; j++) {
	 			for (k=0; k<ns; k++) {
	 				if (toupper(fBases[j]) == toupper(stops[k].amino)) {
	 					stop= j+1; // +1 to include this codon
	 					return;
	 					}
	 				}
	 			}
			break;
			
		default:
			break;
		}
}



void DMegaSequence::NucleicSearch(char* source, char* target, SearchRec& aSearchRec)
{
	long k, j, tBits, lens, ti;
	Boolean done;
 
					//find target[ti] where tBits == kMaskA,C,G,T for fastest search
					//This part is wasted time for SearchAgain... store tBits/ti in saverec...
	lens= StrLen(target);
	for (j= 0, ti= 0, tBits= 0; j<lens && !tBits; j++) {
		ti= j;
		tBits= NucleicBits( target[ti]);
		tBits &= kMaskNucs; //	bClr( tBits, kBitExtra); //drop RNA bit
		}

	do {
reloop:
			aSearchRec.indx += aSearchRec.step; 
			aSearchRec.lim  -= aSearchRec.step;	
			
			if (aSearchRec.step>0) {
				for (j= 0; j<aSearchRec.lim; j++)
				 // if (IsNucleicMatch( tBits, NucleicBits( source[j+aSearchRec.indx]))) 
				 if ( tBits & NucleicBits( source[j+aSearchRec.indx]) ) 
				 {
					k= j; 
					goto nextindx;
				 }
				k= aSearchRec.lim;
				}			
			else {  //step<0 IS BAD .. see UTextDoc version...
				for (j= 0; j > -aSearchRec.lim; j--) 
				  //if (IsNucleicMatch( tBits, NucleicBits( source[j+aSearchRec.indx])))
				  if ( tBits & NucleicBits( source[j+aSearchRec.indx]) ) 
			    {
					k= j; 
					goto nextindx;
					}
				k= -aSearchRec.lim;
				}
				
nextindx:  
			aSearchRec.indx += k; 
			aSearchRec.lim  -= k;
				//? not enough source left to match full target 
			if (aSearchRec.step>0) {
				if (aSearchRec.lim < lens-ti) aSearchRec.lim= 0;
				}			
			else {
				if (aSearchRec.lim < ti) aSearchRec.lim= 0; //? or lim < ti-1 
				}
			done= (aSearchRec.lim==0);
			
			if (!done) {
				long srcstart= aSearchRec.indx - ti;
				for (j= 0; j<lens; j++) 
				 if (j != ti) {
					//if (!IsNucleicMatch( NucleicBits( target[j]), NucleicBits( source[j+srcstart]))) 
				  if (!(kMaskNucs & NucleicBits( target[j]) & NucleicBits( source[j+srcstart]))) 
					  goto reloop;
					}
				done= true;
				}
		} while (!done);
}  
	

void DMegaSequence::ProteinSearch(char* source, char* target, SearchRec& aSearchRec)
{
	//just toupper match of chars 
	char  	tChar; 
	Boolean  done;
	long		lens, k, j;
 
	lens= StrLen( target);
	tChar= toupper( target[0]);
	
	do {
reloop:
			aSearchRec.indx += aSearchRec.step; 
			aSearchRec.lim  -= aSearchRec.step;	
			
			if (aSearchRec.step>0) {
				for (j= 0; j<aSearchRec.lim; j++) 
				 if (tChar == toupper( source[j+aSearchRec.indx])) {
					k= j;
					goto nextindx;
					}
				k= aSearchRec.lim;
				}			
			else {
				for (j= 0; j > -aSearchRec.lim; j--)
				 if (tChar == toupper(source[j+aSearchRec.indx])) {
					k= j; 
					goto nextindx;
					}
				k= -aSearchRec.lim;
				}
				
nextindx:  
			aSearchRec.indx += k; 
			aSearchRec.lim  -= k;
				//? not enough source left to match full target 
			if (aSearchRec.step>0) {
				if (aSearchRec.lim < lens-1) aSearchRec.lim= 0;
				}			
			else {
				if (aSearchRec.lim < 1) aSearchRec.lim= 0;  
				}
			done= (aSearchRec.lim==0);
			
			if (!done) {
				for (j= 1; j<lens; j++) {
					if (toupper( target[j]) != toupper( source[j+aSearchRec.indx])) 
						goto reloop;
					}
				done= true;
				}
		} while (!done);
}  


	

long DMegaSequence::Search( char* target, Boolean backwards)
/* search for target in sequence;
	search == abs index into fBases[0..fLength] or kSearchNotFound
  if (target='') then find next  
  if (backwards) then backwards search
*/
{
	long		limit, aStart;
	SearchRec  aSearchRec;
 
	aStart= fSelStart;
	if (!target || !*target) {  //search again
		if (fSearchRec.targ && (fSearchRec.step == -1 || fSearchRec.step == 1)) {  
			aSearchRec= fSearchRec;
			target= aSearchRec.targ;
			}		
		else {
			return kSearchNotFound;
			}
		}		
	else {
		limit= fLength; //StrLen(fBases);
		if (aStart < 0) aStart= 0;
		if (backwards) {
			if (!aStart) aStart= limit-1;
			else limit= aStart+1;  //? off-by-one ??
			}		
		else {
			if (aStart > limit) aStart= 0; 
			else limit -= aStart;
			}
		if (fSelBases > 0 && fSelBases < limit) limit= fSelBases;
		if (backwards) aSearchRec.step= -1;
		else aSearchRec.step= 1;
		aSearchRec.indx= aStart - aSearchRec.step;  //indx always in [0..handsize-1], except 1st is -1 
		aSearchRec.lim = limit + aSearchRec.step;  //lim always in [1..handsize] or 0=done, except 1st is hand+1 
		aSearchRec.targ= StrDup(target);
		}

	switch (fKind) {  
		case kRNA:
		case kDNA:
		case kNucleic: 
			NucleicSearch(fBases,target,aSearchRec); 
			break;
		default:
			ProteinSearch(fBases,target,aSearchRec);
			break;
		}
	
	fSearchRec= aSearchRec;
	if (aSearchRec.lim == 0) return kSearchNotFound;
	else return aSearchRec.indx;
}  



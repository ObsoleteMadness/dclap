// DSequence.cp 
// d.g.gilbert



#include "DSequence.h"
#include "DSeqFile.h"
#include "DSeqList.h"
#include "DREnzyme.h"
#include <ncbi.h>
#include <dgg.h>
#include <DUtil.h>
#include <DFile.h>
#include "ureadseq.h"
 

char* DSequence::kConsensus = "Consensus";
char  DSequence::indelHard = '-';
char  DSequence::indelSoft = '~';
char  DSequence::indelEdge = '.';
 

Local char* gAminos		=	"ABCDEFGHIKLMNPQRSTVWXYZ*_.-~?";
Local char* gIubbase	= "ACGTUMRWSYKVHDBXN_.-*~?"; 	//did include ".", for what?
Local char* gPrimenuc	= "ACGTU_.-*~?";
Local char* protonly	= "EFIPQZ";
Local char* stdsymbols= "_.-*?";
Local char* allsymbols= "_.-*?!=/**/[]()!&#$%^&=+;:/|`~'\"\\";
Local char* seqsymbols= allsymbols;






// DSequence statics -----------------


long DSequence::NucleicBits(char nuc)
{
	//char nuc= toupper(nuc);
	switch (nuc) {
		case 'a': case 'A': return kMaskA;  
		case 'c': case 'C': return kMaskC;   
		case 'g': case 'G': return kMaskG;   
		case 't': case 'T': return kMaskT;   
		case 'u': case 'U': return kMaskU;  // ==  ( kMaskT | kMaskExtra); 
		case 'y': case 'Y': return (kMaskC | kMaskT);  
		case 'k': case 'K': return (kMaskG | kMaskT);  
		case 's': case 'S': return (kMaskG | kMaskC);  
		case 'r': case 'R': return (kMaskG | kMaskA);  
		case 'm': case 'M': return (kMaskA | kMaskC);  
		case 'w': case 'W': return (kMaskA | kMaskT);  
		case 'h': case 'H': return (kMaskA | kMaskC | kMaskT);  
		case 'b': case 'B': return (kMaskG | kMaskC | kMaskT);  
		case 'v': case 'V': return (kMaskG | kMaskC | kMaskA);  
		case 'd': case 'D': return (kMaskG | kMaskT | kMaskA);  
		case ' ': case '_': return 0;  //? spacers
		case  0 : return 0;  
		case 'n': case 'N': return kMaskNucs; 
		default : 
			{
			if (nuc == indelHard || nuc == indelSoft)  return kMaskIndel;
			else if (nuc == indelEdge) return 0; //return kMaskNucs; ???
			else return 0; //kMaskNucs;  //?? match all or match none ??
			}
		}
}


Boolean DSequence::IsNucleicMatch( long xNucBits, long yNucBits)
{
		// note: Indel/spaces match nothing, including other indel/space ...
	return 0 != ( (kMaskNucs & xNucBits) & (kMaskNucs & yNucBits));
}


char DSequence::NucleicConsensus( long xNucBits, long yNucBits)
{ 
	char nuc; 
	Boolean isrna, indel;

	if (xNucBits==0 || yNucBits==0) 
		return ' ';  //?? either a space yields space ?
	else {
		indel= (kMaskIndel == xNucBits || kMaskIndel == yNucBits);
		isrna= (kMaskU == xNucBits || kMaskU == yNucBits);
		switch ( (kMaskNucs & xNucBits) | (kMaskNucs & yNucBits)) {
			case kMaskA: nuc= 'A'; break;
			case kMaskC: nuc= 'C'; break;
			case kMaskG: nuc= 'G'; break;
			case kMaskT: 
				if (isrna) nuc= 'U'; else nuc= 'T';
				break;
					//? do other ambig codes ?
			default:	nuc= '.'; break; //? use this as Consensus other ?
			}
		if (indel) nuc= tolower(nuc); //?
		}
	return nuc;
}



void DSequence::NucleicComplement( Boolean isrna, char* fromSeq, char* toSeq, long len)
{
// note: fromSeq may == toSeq 
	register char b, c;
	Boolean islow;
 	while (len--) {
		b= *fromSeq++;
		islow= islower(b);
		if (islow) b= toupper(b);
		switch (b) {
			case 'C': c= 'G'; break;
			case 'G': c= 'C'; break;
			case 'A': if (isrna) c= 'U'; else c= 'T'; break;
			case 'T':
			case 'U': c= 'A'; break;
			case 'R': c= 'Y'; break;
			case 'Y': c= 'R'; break;
			case 'M': c= 'K'; break;
			case 'K': c= 'M'; break;
			case 'S': c= 'S'; break;  // was mistaken 'w'
			case 'W': c= 'W'; break;  // was mistaken 's'
			case 'H': c= 'D'; break;
			case 'D': c= 'H'; break;
			case 'B': c= 'V'; break;
			case 'V': c= 'B'; break;
			default : c= b;   break;
			}
		if (islow) c= tolower(c);
		*toSeq++= c;
		}
}



const char* DSequence::Amino123( char amino1)
{
 switch (toupper( amino1)) {
	 case 'A': return "Ala";
	 case 'M': return "Met";
	 case 'R': return "Arg";
	 case 'F': return "Phe";
	 case 'N': return "Asn";
	 case 'P': return "Pro";
	 case 'D': return "Asp";
	 case 'S': return "Ser";
	 case 'C': return "Cys";
	 case 'T': return "Thr";
	 case 'Q': return "Gln";
	 case 'W': return "Trp";
	 case 'E': return "Glu";
	 case 'Y': return "Tyr";
	 case 'G': return "Gly";
	 case 'V': return "Val";
	 case 'H': return "His";
	 case 'I': return "Ile";
	 case 'B': return "Asx";
	 case 'L': return "Leu";
	 case 'Z': return "Glx";
	 case 'K': return "Lys";
	 case 'X': return "Xaa";
	 case '*': return "End";
	 case ' ': return "   ";
	 default : return "???"; //??
	 }
}


char DSequence::Amino321( char* amino)
{
	char	aa[4];
	aa[0]= toupper(amino[0]);
	aa[1]= tolower(amino[1]);
	aa[2]= tolower(amino[2]);
	aa[3]= 0;
	
			 if (!StrCmp(aa,"Ala")) return 'A';
	else if (!StrCmp(aa,"Met")) return 'M';
	else if (!StrCmp(aa,"Arg")) return 'R';         
	else if (!StrCmp(aa,"Phe")) return 'F';
	else if (!StrCmp(aa,"Asn")) return 'N';       
	else if (!StrCmp(aa,"Pro")) return 'P';
	else if (!StrCmp(aa,"Asp")) return 'D';    
	else if (!StrCmp(aa,"Ser")) return 'S';
	else if (!StrCmp(aa,"Cys")) return 'C';         
	else if (!StrCmp(aa,"Thr")) return 'T';
	else if (!StrCmp(aa,"Gln")) return 'Q';       
	else if (!StrCmp(aa,"Trp")) return 'W';
	else if (!StrCmp(aa,"Glu")) return 'E';    
	else if (!StrCmp(aa,"Tyr")) return 'Y';
	else if (!StrCmp(aa,"Gly")) return 'G';          
	else if (!StrCmp(aa,"Val")) return 'V';
	else if (!StrCmp(aa,"His")) return 'H';
	else if (!StrCmp(aa,"Ile")) return 'I';       
	else if (!StrCmp(aa,"Asx")) return 'B';
	else if (!StrCmp(aa,"Leu")) return 'L';          
	else if (!StrCmp(aa,"Glx")) return 'Z';
	else if (!StrCmp(aa,"Lys")) return 'K';           
	else if (!StrCmp(aa,"End")) return '*';           
	else if (!StrCmp(aa,"   ")) return ' '; //??           
	else return 'X';
}


char* DSequence::Nucleic23Protein( char* nucs, long nbases)
//!! Need to do this w/ NucBits so ambiguity codes work !!
//!! The CodonTable.codon == acodon should be IsNucleicMatch( bits(codon), bits(acodon)) 
{
	long	i;  //borland 16bit don't like longs for iterators !?
	char  *ap, *kp, *aminos;
 
	if (DCodons::NotAvailable()) return NULL; // (gCodonTable[0].amino == 0)

	aminos= StrDup( nucs);
	for (i= 0, ap= aminos; i<nbases; i++) {
		register char c= toupper( *ap);
		if (c=='U') c= 'T';
		*ap++= c;
		}

	for (i= 0, ap= aminos, kp= aminos; i<nbases; i++) {
		for (long j=0; j<64; j++)
			if ( MemCmp(DCodons::codon(j), ap, 3) == 0) { //gCodonTable[j].codon
				*kp++= DCodons::amino(j); // gCodonTable[j].amino;
				break;
				}
		}
	*kp= '\0';
	aminos= (char*) MemMore( aminos, kp - aminos + 1);
	return aminos;
} 









// DSequence -----------------


DSequence::DSequence()
{
	fBases= (char*) MemGet(1,true); 
  fMasks= NULL; //(char*) MemGet(1,true); 
  fMasksOk= false;
	fInfo	= (char*) MemGet(1,true); 	
	fName = StrDup("Untitled"); 		//!? need unique name, eg. Noname-1,noname-2...
	fKind = kOtherSeq; 		//? user default
	fLength		= 0;
	fChecksum	= 0;
	fIndex		= 0; //dummy
	fModTime	= gUtil->time();
	fChanged	= FALSE; //TRUE;
	fIsMega		= FALSE;
	fShowORF	= FALSE;
	fShowRE		= FALSE;
	fShowTrace = FALSE;

	fDeleted	= FALSE; 
	fOpen			= FALSE;
	fSearchRec.targ= NULL;
	fOrigin	= 0;
	fSelStart= 0;
	fSelBases= 0;
}


DSequence::~DSequence()  
{
	MemFree(fBases);
	MemFree(fMasks);
	MemFree(fInfo);
	MemFree(fName);
	MemFree(fSearchRec.targ);
}


DObject* DSequence::Clone() // override 
{	
	DSequence* aSeq= (DSequence*) DObject::Clone();
	aSeq->fBases= NULL; aSeq->SetBases( fBases, true);  
	aSeq->fMasks= NULL; aSeq->SetMasks( fMasks, true); 
	aSeq->fInfo=  StrDup( fInfo);
	aSeq->fName=  StrDup( fName);
	aSeq->fSearchRec.targ= StrDup( fSearchRec.targ);
	return aSeq;
}

void DSequence::CopyContents( DSequence* fromSeq)
{
	SetBases( fromSeq->fBases, true); 
	SetMasks( fromSeq->fMasks, true); 
	SetInfo( fromSeq->fInfo); 
	SetName( fromSeq->fName); 
	fKind			= fromSeq->fKind; 		 
	fLength		= fromSeq->fLength;
	fChecksum	= fromSeq->fChecksum;
	fModTime	= fromSeq->fModTime;
	fChanged	= fromSeq->fChanged;
	fDeleted	= fromSeq->fDeleted; 
	fOpen			= fromSeq->fOpen;
	fIndex		= fromSeq->fIndex;
	fSearchRec= fromSeq->fSearchRec;
	fSearchRec.targ= StrDup( fromSeq->fSearchRec.targ);
}

void DSequence::SetBases( char*& theBases, Boolean duplicate)
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
		if (duplicate) fBases = StrDup(theBases); 
		else { fBases= theBases; theBases= NULL; }
		fLength= newlen;  
		}
	fModTime= gUtil->time();
	fMasksOk= false; // assume SetBases always called before SetMask...
}


void DSequence::UpdateBases( char* theBases, long theLength)
{
	fBases =  theBases;  
	fLength= theLength;
	fMasksOk= false; // assume this allways called before SetMask...
}


void DSequence::SetMasks( char*& theMasks, Boolean duplicate)
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

void DSequence::FixMasks()
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

short DSequence::MaskAt(long baseindex, short masklevel)
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
				// 5-7 reserved for now
			case 5: b &= 16;  break;
			case 6: b &= 32;  break;
			case 7: b &= 64;  break;
			default: b = kMaskEmpty; break;
			}
		}
	return b;
}

Boolean DSequence::IsMasked(unsigned char maskbyte, short masklevel)
{
	short b= (unsigned char) maskbyte;
	switch (masklevel) {
		case 0: b &= 0x7f; break; // return full mask - top (0x80) bit
		case 1: b &= 1;  break;  
		case 2: b &= 2;  break;
		case 3: b &= 4;  break;
		case 4: b &= 8;  break;
				// 5-7 reserved for now
		case 5: b &= 16;  break;
		case 6: b &= 32;  break;
		case 7: b &= 64;  break;
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
		case 5: if (maskval) b |= 16; else b &= ~16; break;
		case 6: if (maskval) b |= 32; else b &= ~32; break;
		case 7: if (maskval) b |= 64; else b &= ~64; break;
		// -- 7 & 8 may go unused to store data in printchar form
		default: break;
		}
	//if (b) masklevel--; // debugging b val
}

void DSequence::SetMaskAt(long baseindex, short masklevel, short maskval)
{
	if (fMasks && fMasksOk && baseindex>=0 && baseindex<fLength)  
		::SetMaskByte(fMasks[baseindex],masklevel, maskval);
}


void DSequence::SetMask(short masklevel, short maskval)
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
				// 5-7 reserved for now?
		case 5: b ^= 16;  break;
		case 6: b ^= 32;  break;
		case 7: b ^= 64;  break;
		default:  break;
		}
	//if (b) masklevel--; // debugging b val
}

void DSequence::FlipMaskAt(long baseindex, short masklevel)
{
	if (fMasks && fMasksOk && baseindex>=0 && baseindex<fLength)  
		::FlipMaskByte(fMasks[baseindex],masklevel);
}

void DSequence::FlipMask(short masklevel)
{
	long  baseindex;
	if (fMasks && fMasksOk) 
	 for (baseindex=0; baseindex<fLength; baseindex++)  
		FlipMaskByte(fMasks[baseindex],masklevel);
}


void DSequence::SetInfo( char* theInfo)
{
	if (fInfo) MemFree(fInfo);
	fInfo = StrDup(theInfo);  
}

void DSequence::SetName( char* theName)
{
	if (fName) MemFree(fName);
	fName = StrDup(theName);  
}

void DSequence::SetKind( short theKind)
{
	fKind= theKind;  
}

void DSequence::SetIndex( short theIndex)
{
	fIndex= theIndex;  
}

char* DSequence::KindStr() const 
{
	switch (fKind) {
		case kDNA 			: return "DNA";
		case kRNA 			: return "RNA";
		case kNucleic   : return "Nucleic";
		case kAmino 		: return "Protein";
		default					:
		case kOtherSeq  : return "Unknown";
		}
}


void DSequence::SetTime( unsigned long theTime)
{
	fModTime= theTime;
}

void DSequence::SetOrigin( long theOrigin)
{
	fOrigin= theOrigin;
}

void DSequence::Modified()  
{
	fModTime	= gUtil->time();
	fChanged	= TRUE;
}

void DSequence::Reformat(short format)
{
	// later...? 
}

void DSequence::UpdateFlds()
{
	long 		seqlen;
	short 	kind;
	unsigned long check;
	fLength= StrLen(fBases);
	GetSeqStats(fBases, fLength, &kind, &check, &seqlen);
	//fValidLength= seqlen; //?? is fLength == StrLen(fBases) or is it <= that, only valid bases??
	fKind			= kind;
	fChecksum	= check;
	//fChanged	= FALSE;
}





void DSequence::SetSelection( long start, long nbases)
{
	fSelStart= start;
	fSelBases= nbases;
}

void DSequence::ClearSelection()
{
	fSelStart= 0;
	fSelBases= 0;
}

void	DSequence::GetSelection( long& start, long& nbases)
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


DSequence* DSequence::CopyRange()
{  
			//note: start == 0 == 1st position 
	DSequence* aSeq= (DSequence*) Clone();
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


DSequence* DSequence::CutRange()
{
/* aSeq.Cutrange means:  
		(a) selected range (fSelStart..fSelStart+fSelBases) is copied into Result; &&
		(b) selected range is removed from this
*/
 
	DSequence* aSeq= CopyRange();
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

void DSequence::InsertSpacers( long insertPoint, long insertCount, char spacer)
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


DSequence* DSequence::PasteBases( long insertPoint, char* fromBases, char* fromMasks)
{ 
	long add;
	DSequence* aSeq= (DSequence*) Clone();
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


DSequence* DSequence::Reverse()
{
	long i;
	DSequence* aSeq= (DSequence*) Clone();	
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


DSequence* DSequence::Complement()
// A->T, T->A, G->C, C->G  
{
	Boolean		isrna;
	DSequence* aSeq= (DSequence*) Clone();	
	
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



DSequence* DSequence::ChangeCase(Boolean toUpper)
// g->G or G->g
{
	DSequence* aSeq= (DSequence*) Clone();	
	
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

DSequence* DSequence::Dna2Rna(Boolean toRna)
// T->U or U->T 
{
	Boolean		isrna;
	DSequence* aSeq= (DSequence*) Clone();	
	
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


DSequence* DSequence::LockIndels( Boolean doLock)
// indelSoft -> indelHard or vice versa 
{	
	char b, c, fromc, toc;
	DSequence* aSeq= (DSequence*) Clone();	
	
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


DSequence* DSequence::Slide( long slideDist)
/* insert "-" below (+dist) or above (-dist) of seq range,
	squeeze out "-" above (below) of range
*/
{ 	
	long 		slideSave, newlen, fulllen, len, i, j= 0;
	char 		b, cutchar, cutmask;
	char		*pc, *pr;
	
	DSequence* aSeq= (DSequence*) Clone();	
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


DSequence* DSequence::Compress()
// pull out "-", ".", ? anything but nuc/aa codes ? 
{
	long  len, fulllen, i;
	char 	b, c;
	char * mc, * mr;
	Boolean domask= (fMasks && fMasksOk);
 
	DSequence* aSeq= (DSequence*) Clone();	
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

 


DSequence* DSequence::CompressFromMask(short masklevel)
{
	long  len, fulllen, i;
	char 	b, c;
	char * mc, * mr;
	Boolean domask= (fMasks && fMasksOk && masklevel>0);

	if (!domask) return NULL; // or aSeq ???
	DSequence* aSeq= (DSequence*) Clone();	
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

 

DSequence* DSequence::Translate(Boolean keepUnselected)
{	
	char  c;
	char	acodon[3];
	Boolean isrna, nocodon;
	char	*pr, *pc, *mr, *mc;
	long	len, fulllen, newlen, i, j, k;
	
	DSequence* aSeq= (DSequence*) Clone();		
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


DSequence* DSequence::OnlyORF(char nonorf)
// convert sequence to ORF part only 
{
	DSequence* aSeq= (DSequence*) Clone();	
	
	if (fSelStart < 0) fSelStart= 0;
	long len= aSeq->fLength - fSelStart;
	if (fSelBases > 0 && fSelBases < len) len= fSelBases;

	long start = 0, fini= 0;
	long anchor= fSelStart;
	do {
		aSeq->SearchORF( start, fini);
		if (start != kSearchNotFound) {
			char* pr= aSeq->fBases + anchor;
			if (start>len) start= len;
			if (fini<start) fini= start+1;
			for ( ; anchor<start; anchor++) *pr++ = nonorf; 
			anchor= fini+1;
			fSelStart= anchor;
			}
	} while (start != kSearchNotFound);
		
	aSeq->UpdateFlds();
	aSeq->Modified();
	return aSeq;
}


void DSequence::SetORFmask(short masklevel, short frame)
{
	if (!fMasks || !fMasksOk) {
		FixMasks();
		if (!fMasks || !fMasksOk) return;
		}
		
	// revise this to do multiple frames in one go,
	//  offseting each anchor +1 beyond START of last orf, not after STOP
	// since separate frame calls return the same orfs !
	
	frame %= 3; // drop reverse frames
	long savesel= fSelStart;
	long savelen= fSelBases;
	fSelStart= frame;
	fSelBases= fLength - frame;

	long start = 0, fini= 0;
	long anchor= fSelStart;
	do {
		this->SearchORF( start, fini);
		if (start != kSearchNotFound) {
			long i;
			for (i= anchor; i<start; i++) SetMaskAt( i, masklevel, 0);
			for (i= start; i<=fini ; i++) SetMaskAt( i, masklevel, 1);
			if (fini == kSearchNotFound) {
				start= kSearchNotFound;
				}
			else {
			  anchor= fini+1;
			  fSelStart= anchor; // start+1 // !? want overlapped orfs !?
			  }
			}
	} while (start != kSearchNotFound);
	
	fSelStart= savesel;
	fSelBases= savelen;
}

void DSequence::SetShowORF( short turnon) 
{ 
	fShowORF= turnon > 0; 
	if (turnon > 1) {
		if (turnon & 2) SetORFmask(5, 0);
		//if (turnon & 4) SetORFmask(6, 1);
		//if (turnon & 8) SetORFmask(7, 2);
		}
}

void DSequence::SetShowRE( short turnon) 
{ fShowRE= turnon > 0; 
}

void DSequence::SetShowTrace( short turnon) 
{ fShowTrace= turnon > 0; 
}


//Boolean domask= (fMasks && fMasksOk);


void DSequence::DoWrite(DFile* aFile, short format)
{  
	if (fLength>0) {
		DSeqFile::WriteSeqWrapper( aFile, fBases, fLength, format, fName);
		if (fMasks && fMasksOk && DSeqFile::gWriteMasks) 
			DSeqFile::WriteMaskWrapper( aFile, fMasks, fLength, format, fName);
		}
}


void DSequence::DoWriteSelection(DFile* aFile, short format)
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




Boolean DSequence::GoodChar(char& aChar) 
{
	char* aacodes = gAminos;
	char* iupaccodes = gIubbase;
	char* nacodes = gPrimenuc;
	char	c;
	char* codes = NULL;
 
  if (aChar=='\n' || aChar=='\r')  
		return FALSE;
	else if (aChar<' ' || aChar>'~')  
		return TRUE; //allow return,tab,delete, ? option chars
 
  switch (fKind) {
	  case kDNA			:
		case kRNA			: codes= nacodes; break;
		case kNucleic	: codes= iupaccodes; break;
		case kAmino		: codes= aacodes; break;
		default:
	  case kOtherSeq: return TRUE;
		}
		
  c= toupper(aChar);
	while (*codes) {
		if (c == *codes) {
  		switch (fKind) {
				case kDNA: if (c == 'U') {
					if (aChar=='u') aChar= 't'; else aChar= 'T';
					}
				break;
				case kRNA: if (c == 'T') {
					if (aChar=='t') aChar= 'u'; else aChar= 'U';
					}
				break;
				}
			return TRUE;
			}
		codes++;
		}
	return FALSE;
}


Boolean DSequence::IsConsensus()
{
	return (fName && StringCmp(fName,kConsensus)==0);
}





long DSequence::SearchAgain() 
{
	return Search( "", FALSE);
}



void DSequence::SearchORF( long& start, long& stop) 
{
	char  startc[2];
	long  i, j, k;
	short	ns, aaMinORFsize;
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
	 		  if ( j+3 - start >= DSeqList::gMinORFsize) 
	 		 	 for (k=0; k<ns; k++) {
#if 1
	 		 	 	short jc, jt;
	 		 	 	for (jc= 0, jt=0; jc<3; ) {
		 		 	 	long tbit= NucleicBits( fBases[j+jt]);
					  long sbit= NucleicBits( stops[k].codon[jc]); 
					  if (tbit == kMaskIndel) jt++;
						else if (!(kMaskNucs & tbit & sbit)) goto nextcodon;
						else { jt++; jc++; }
						}
					stop= j+3; 
					return;
#else					
	 				if (StrNICmp( fBases+j, stops[k].codon, 3) == 0) {
	 					stop= j+3;
	 					return;
	 					}
#endif				
	 	nextcodon:
	 				continue;
	 				}
	 			}
			break;
			
		case kAmino:
			startc[0]= DCodons::startamino();
			startc[1]= 0;
	 		aaMinORFsize= (DSeqList::gMinORFsize+2) / 3;
	 		start= Search( startc, FALSE);
	 		if (start == kSearchNotFound) return;
	 		for (j= start+1; j<fLength; j++) {
	 		  if ( j - start >= aaMinORFsize) 
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




void DSequence::NucleicSearch(char* source, char* target, SearchRec& aSearchRec)
{
	long k, j, tBits, lens, ti;
	Boolean done;
 
					//find target[ti] where tBits == kMaskA,C,G,T for fastest search
					//This part is wasted time for SearchAgain... store tBits/ti in saverec...
	lens= StrLen(target);
	for (j= 0, ti= 0, tBits= 0; j<lens && !tBits; j++) {
		ti= j;
		tBits= NucleicBits( target[ti]);
		tBits &= kMaskNucs;  //drop RNA bit
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
				long js, jt;
				for (js= 0, jt= 0; jt<lens; ) {
#if 1
				  long tbit= NucleicBits( target[jt]);
				  long sbit= NucleicBits( source[js+srcstart]); 
				  if (tbit == kMaskIndel) jt++;
				  else if (sbit == kMaskIndel) js++;
					else if (!(kMaskNucs & tbit & sbit)) goto reloop;
					else { jt++; js++; }
#else 
				  if (!(kMaskNucs & NucleicBits( target[jt]) & NucleicBits( source[js+srcstart]))) 
					  goto reloop;
					else { jt++; js++; }
#endif
					}
				done= true;
				}
		} while (!done);
}  
	

void DSequence::ProteinSearch(char* source, char* target, SearchRec& aSearchRec)
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


	

long DSequence::Search( char* target, Boolean backwards)
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



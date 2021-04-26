// DSeqFile.cp 
// d.g.gilbert
	

#include <ncbi.h>
#include <dgg.h>
#include <stdio.h>
#include <DFile.h>
#include "DSeqFile.h"
#include "DSeqList.h"
#include "ureadseq.h"
#include "DMegaSequence.h"
#include "DSequence.h"



Global char* gOutputName = NULL;

Boolean DSeqFile::gWriteMasks = true;
Boolean DSeqFile::gSaveMasks= true;
static char* kMaskName = "#Mask";

// static
void DSeqFile::DontSaveMasks()
{
	gSaveMasks= gWriteMasks;
	gWriteMasks= false;
}

void DSeqFile::DoSaveMasks()
{
	gWriteMasks= gSaveMasks;
}


Boolean dupBases = false; // !?

//static
DSequence* DSeqFile::MakeSequence(char* name, char*& bases, char* info, long modtime)
{
	char *b;
	long nbases;
	Boolean ismega;
	DSequence* aSeq;
	
	for (b= bases, nbases=0; 
			(b) && (*b) && nbases<DMegaSequence::kMegaPartSize; 
			b++, nbases++)
				;
	ismega= nbases>=DMegaSequence::kMegaPartSize; 
	 
	if (ismega) {
		aSeq= new DMegaSequence();
		} 
	else {
		aSeq= new DSequence();
		}
	aSeq->SetName(name);
	aSeq->SetBases(bases, dupBases);
	aSeq->SetInfo(info);
 	if (modtime!=0) aSeq->SetTime( (unsigned long)modtime);
	aSeq->UpdateFlds(); 	 
	return aSeq;
}



#if NEED_LATER
// must be in scope of a DSeqFile:: method
Local const  struct formatTable {
  char  *name;
  short num;
  } formname[] = {
    {"ig",  kIG},
    {"stanford", kIG},
    {"genbank", kGenBank},
    {"gb", kGenBank},
    {"nbrf", kNBRF},
    {"embl", kEMBL},
    {"gcg", kGCG},
    {"uwgcg", kGCG},
    {"dnastrider", kStrider},
    {"strider", kStrider},
    {"fitch", kFitch},
    {"pearson", kPearson},
    {"fasta", kPearson},
    {"zuker", kZuker},
    {"olsen", kOlsen},
    {"phylip", kPhylip},
    {"phylip3.2", kPhylip2},
    {"phylip3.3", kPhylip3},
    {"phylip3.4", kPhylip4},
    {"phylip-interleaved", kPhylip4},
    {"phylip-sequential", kPhylip2},
    {"plain", kPlain},
    {"raw", kPlain},
    {"pir", kPIR},
    {"codata", kPIR},
    {"asn.1", kASN1},
    {"msf", kMSF},
    {"paup", kPAUP},
    {"nexus", kPAUP},
    {"pretty", kPretty},
    {0,0}
  };
#endif

enum { kMaxFormatNames = 18 }; // != kMaxFormat ??
static char *gSeqFileFormats[] = { //DSeqFile::kMaxFormat+1
	    "IG|Stanford",
	    "GenBank|GB",
	    "NBRF",
	    "EMBL",
	    "GCG",
	    "DNAStrider",
	    "Fitch",
	    "Pearson|Fasta",
	    "Zuker [in only]",
	    "Olsen [in only]",
	    "Phylip3.2|Phylip2",
	    "Phylip|Phylip4",
	    "Plain|Raw",
	    "PIR|CODATA",
	    "MSF",
	    "ASN.1",
	    "PAUP|NEXUS",
	    "Pretty [out only]",// # 18
	    //"AutoSeq [in only]", 
	    0};

const char* DSeqFile::FormatName(short index)
{ 
	if (index<1 || index>kMaxFormatNames) return "";
	else return gSeqFileFormats[index-1];
}

short  DSeqFile::FormatFromName(const char* name)
{
	if (!name) return kUnknown;
	long   i, len1, namelen= StrLen(name);
	char  	*form1, *form2;
	if (namelen<2) return kUnknown;
	for (i=0; i<kMaxFormat; i++) {
		form1= (char*) gSeqFileFormats[i];
		form2= StrChr(form1,'|'); 
		if (form2) { len1= form2 - form1; form2++; }
		else len1= StrLen( form1);
		if (StrNICmp(name, form1,len1) == 0) return i+1;
		else if (form2 && StrICmp(name, form2) == 0) return i+1;
		}
	return kUnknown;
}


char* DSeqFile::FixSeqID( char* s, short leftmargin)
{
	register char * cp, * ep;
	for (cp= s; *cp && isspace(*cp); cp++) ;
	for (ep= cp; *ep && !isspace(*ep); ep++) ;
	if (ep>cp && ep[-1] == ',') ep--; // fix for fasta "name, #bases, # checksum"
	if (*ep) *ep= 0;
	return cp;
}

short DSeqFile::SeqFileFormatWrapper( DFile* aFile) 
{
	long	skiplines = 0;
	short	aFormat, err = 0;

	aFile->Open("r");  
	aFormat= seqFileFormatFp( aFile->fFile, &skiplines, &err);
	aFile->Close();  
	return	aFormat;
}


short DSeqFile::ReadSeqFile( DFile* aFile, DSeqList* aSeqList) 
{
	char	seqid[256], *seqidptr, *bases, *seqlist;
	long	skiplines = 0, skipi, seqlen;
	short aFormat, atseq = 0, whichSeq = 0, nseq = 0, err = 0;
	char  *defname;
	Boolean notdone = true, needrewind = false;
	DSequence* aSeq;
	
	aFile->OpenFile("r");	
	aFormat = seqFileFormatFp( aFile->fFile, &skiplines, &err);
	aFile->OpenFile(); // rewind
	defname= FixSeqID( gDefSeqName, 0);
	
	switch (aFormat) {
		case kUnknown:
		case kNoformat:
			aFile->CloseFile();
			return aFormat;
		 
			// all multi-pass (interleaved) formats go here !		
		case kOlsen:
		case kMSF:
		case kPAUP:
		case kPhylip2:
		case kPhylip4:
			needrewind = true;
			aFile->CloseFile();
			seqlist = listSeqs( aFile->GetName(), skiplines, aFormat, &nseq, &err);
			MemFree(seqlist); 
			break;
		}
		
	skipi= skiplines;
	while (notdone) {
			// this one-pass thru gFileRef may well be buggy for current readSeq
			// need to rewind file & count whichSeq++ for each sequence...?
		if (needrewind) {
			whichSeq++; 
			atseq= 0; 
			aFile->OpenFile("r");
			skipi= skiplines;
			}
		else {
			whichSeq = 1; 
			atseq= 0;
			}
		seqlen = 0; seqidptr= seqid; *seqidptr= 0;
		bases = readSeqFp( whichSeq, aFile->fFile, skipi, aFormat, &seqlen, 
														&atseq, &err, seqidptr);
		skipi= 0; 
		
				// install bases into new TSequence, and stick TSequence into list
		if (bases) {
			if (seqlen > 0) {
				char *name, * cp;
				Boolean ismask= false;
				
				if (*seqidptr != 0) {
					if ( (cp= StrStr(seqidptr, kMaskName)) != NULL) {
						ismask= true;
						*cp= 0;
						}
					name= FixSeqID( seqidptr, 0);
					}
				else 
					name= defname;
					
				if (ismask) {
					aSeq= aSeqList->FindNamedSeq( name);
					if (aSeq) {
						// convert mask...
						register char *mp;
						for (mp= bases; *mp; mp++) { 
							*mp -= '?'; // to zero relative
							*mp |= 0x80; // add high bit
							}
						aSeq->SetMasks( bases);
						}
					}
				else {
					aSeq = MakeSequence( name, bases, NULL, 0); //gFileRef->moddate
					aSeqList->InsertLast(aSeq);
					}
				}
			else notdone= false;
			MemFree(bases); // readSeq made this ptr w/ malloc call... MakeSeq copies this data... 
			}
		else notdone= false; // nodata -- if format=0, no data is read
			
		if (needrewind) notdone &= (whichSeq < nseq); // stop now
		else notdone &= !aFile->Eof();  
		}
		
	aFile->CloseFile();
	return aFormat;
}




void	DSeqFile::WriteSeqHeader( DFile* aFile,  
								short format, short nseqs, long nbases, char* outputName,
								Boolean& needSameSize, Boolean& isInterleaved)
{
	const char *kASN1headline = "Bioseq-set ::= {\nseq-set {\n";
	char	line[256], name[64];
	
	// this is a ureadseq.h call
	// need to do somewhere once, then let user set prefs...
 
#define gPrettyInit1(p) { \
  p.isactive=false;\
  p.baseonlynum=true;\
  p.numline= p.atseq= 0;\
  p.numright= p.numtop= true; \
  p.numleft= p.numbot= false;\
  p.nameright= true; \
  p.nameleft= p.nametop= false;\
  p.noleaves= p.domatch= p.degap= false;\
  p.matchchar='.';\
  p.gapchar='-';\
  p.namewidth=8;\
  p.numwidth=5;\
  p.interline=1;\
  p.spacer=10;\
  p.seqwidth=50;\
  p.tab=0; }
 
	gPrettyInit1(gPretty);
	//gPretty.interline= 2; // not active, along w/ numtop,numbot,nametop...
	// need to work into this class
	
	
		
	needSameSize= false;
	isInterleaved= false;
	aFile->Open("a"); 

	if (outputName) StrNCpy( name, outputName, 63);
	else StrCpy( name, DFileManager::kUntitled);
	
	switch (format) {
		case kPIR:
			aFile->WriteLine( "\\\\\\\n");
			break;
		case kASN1:
			aFile->WriteLine( (char*)kASN1headline);
			break;

		case kMSF: 	// this is for top of interleaved file
			{
			isInterleaved= true;
     	unsigned long checksum = 0, checkall = 0;
			//checksum= GCGchecksum(seq, seqlen, &checkall);
			sprintf(line,"\n %s  MSF: %d  Type: N  June 01, 1776 12:00  Check: %d ..\n\n",
										name, nbases, checkall);
			aFile->WriteLine( line);
			}
			break;

		case kPhylip2:	 
			isInterleaved= false;
			goto allphyl;
			break;

		case kPhylip4: 	 
			isInterleaved= true;
		allphyl:
			{
//    if (phylvers >= 4) fprintf(foo," %d %d\n", seqout, seqlen);
//    else fprintf(foo," %d %d YF\n", seqout, seqlen);
			if (isInterleaved) sprintf( line, " %d %d\n", nseqs, nbases);  // " %d %d F\n"
			else sprintf( line, " %d %d I \n", nseqs, nbases);  // " %d %d FI \n"
			aFile->WriteLine( line);
			needSameSize= true;
			}
			break;

		case kPAUP:
			isInterleaved= true;
			needSameSize= true;
			aFile->WriteLine( "#NEXUS\n");
			sprintf( line,"[%s -- data title]\n\n", name);
			aFile->WriteLine( line);
			break;

		case kPretty:
			isInterleaved= true;
			gPretty.isactive= true;
			break;
			
		default:
			break;
		}
		
	aFile->CloseFile();
}



void	DSeqFile::DoInterleave( DFile* toFile, DFile* fromFile, 
								short outform, short seqtype, short nseqs, long nbases,
								short nlines, short noutindex, long* outindex)
{
	short		leaf, iline, iseq;
	char		*cp, line[256];
	
	toFile->Open("a");
	fromFile->Open("r");
	
	for (leaf=0; leaf<nlines; leaf++) {
	
		if (outform == kMSF && leaf == 1) { // do after writing seq names
			toFile->WriteLine( "//\n\n");
			}
			
		if (outform == kPAUP && leaf==1) { // do after writing seq names
			switch (seqtype) {
				case DSequence::kDNA     : cp= "dna"; break;
				case DSequence::kRNA     : cp= "rna"; break;
				case DSequence::kNucleic : cp= "dna"; break;
				case DSequence::kAmino   : cp= "protein"; break;
				case DSequence::kOtherSeq: cp= "dna"; break;
				}
			toFile->WriteLine( "\nbegin data;\n");
			sprintf(line," dimensions ntax=%d nchar=%d;\n", nseqs, nbases); 
			toFile->WriteLine( line);
			sprintf(line," format datatype=%s interleave missing=%c", cp, gPretty.gapchar); // gapchar == indelHard ??? 
			toFile->WriteLine( line);
			//if (gPretty.domatch) { sprintf(line," matchchar=%c", gPretty.matchchar); 
				//Mwrite( toFile, line); }
			toFile->WriteLine( ";\n  matrix\n");
			}

		for (iseq=0; iseq<noutindex; iseq++) {
			fromFile->SetDataMark( outindex[iseq]);
			
			for (iline=0; iline<=leaf; iline++)
				if (fromFile->ReadLine( line, 256) != 0) *line= 0;
				
			if (fromFile->Tell() <= outindex[iseq+1])
				toFile->WriteLine( line, false);  //remember- ReadLine a.k.a fgets retains newline
			}

     for (iline=0; iline<gPretty.interline; iline++)
        toFile->WriteLine("\n"); //LineEnd 
			
		}
		
	fromFile->Close();
	toFile->Close();
}


void	DSeqFile::WriteSeqTrailer( DFile* aFile,
								short format, short /*nseqs*/, short /*nlinesperseq*/, long /*nbases*/)
{
	switch (format) {
  
  	case kASN1:  
			aFile->Open("a"); 
			aFile->WriteLine("} }\n"); 
			aFile->Close(); 
			break;
	
  	case kPAUP:	
			aFile->Open("a"); 
			aFile->WriteLine(";\n  end;\n");
			aFile->Close(); 
			break;	
		}
}



void DSeqFile::WriteSeqWrapper( DFile* aFile,
									char *seq, long seqlen, short outform, char *seqid)
{ 
	if (!aFile->IsOpen())  aFile->Open("a"); 
	seqid= StrDup(seqid);
	char* cp= FixSeqID(seqid, 0);
	gLinesPerSeqWritten= writeSeq( aFile->fFile, seq, seqlen, outform, cp);
	MemFree(seqid);
	//aFile->Close(); //?? want left open for appending by caller ? e.g., seqmail
}  

void DSeqFile::WriteMaskWrapper( DFile* aFile,
									char *mask, long seqlen, short outform, char *seqid)
{ 
	char maskid[128];
	
	if (!aFile->IsOpen())  aFile->Open("a"); 
	seqid= StrDup(seqid);
	char* cp= FixSeqID(seqid, 0);
	StrNCpy( maskid, cp, sizeof(maskid)-5);
	StrNCat( maskid, kMaskName, sizeof(maskid));
	MemFree(seqid);
		// 	massage mask into writable form
	mask= StrDup(mask); 
	Boolean hasdata= false;
	for (cp= mask; *cp; cp++) { 
		*cp &= 0x7F; // drop hi bit
		if (*cp != 0) hasdata= true;
		*cp += '?';  // set 0 == '?', mask range is mostly 'A-Za-z'
	 }
	if (hasdata)
		gLinesPerSeqWritten= writeSeq( aFile->fFile, mask, seqlen, outform, maskid);
	MemFree(mask);
}  


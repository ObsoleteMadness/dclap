// DSeqMail.cpp
// d.g.gilbert


#include <ncbi.h>
#include <dgg.h>
#include <DControl.h>
#include <DPanel.h>
#include <DTCP.h>
#include <DSMTPclient.h>
#include <DUtil.h>
#include <DFile.h>
#include <DSendMailDialog.h>
#include <DSequence.h>
#include <DSeqFile.h>
#include <DApplication.h>

#include "DSeqMail.h"


Local Nlm_FonT  gNoteFont = NULL;
#define SETNOTEFONT()	if (!gNoteFont)	gNoteFont= Nlm_ParseFont("Times,9");

class DOptWindow : public DWindow
{
public:
	DOptWindow(DView* superior, char* title) : 
		DWindow(0, superior, DWindow::fixed, -5,  -5, -60, -30, title, kDontFreeOnClose) 
			{}
	void CloseAndFree() { this->Close(); }
};



// class DNCBIBlast


char* DNCBIBlast::kAddress = "blast@ncbi.nlm.nih.gov";
char* DNCBIBlast::kTitle = "BLAST Search at NCBI";
#ifdef OS_DOS
  char* DNCBIBlast::kHelp 	= "ncbi-bl.hel";	 
#else
  char* DNCBIBlast::kHelp 	= "ncbi-blast.help";	 
#endif																	 
			
																				
DNCBIBlast::DNCBIBlast( long id, DTaskMaster* itsSuperior, DSequence* theSeq) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle),
	fOptionWin(NULL)
{
	fSeq= (DSequence*) theSeq->Clone();
}

DNCBIBlast::~DNCBIBlast()
{
	if (fOptionWin) delete fOptionWin; // !? is this handled by DView::DeleteSubviews() ???!!!
	delete fSeq;
}

char* DNCBIBlast::BuildMessage()
{
	enum blastMethod { blastp, tblastn, blastn, blastx } meth;
	char		buf[512], name[128], *cp;
	short	 	item;
	DPopupList *pmeth, *plib;
	char		*msg = NULL;
	Boolean isamino= fBaseKind->GetValue() == 2;
	
			// want IOSTREAMS here !!!!!!!!!!!!!!!!!!!! not damn sprintf's
	DTempFile* qfile = new DTempFile();

	pmeth= (isamino) ? fAmeth : fNmeth;
	pmeth->GetSelectedItem(item,name,128);
	cp= StrChr( name, ' '); if (cp) *cp= 0;
	sprintf( buf, "PROGRAM %s\n", name); qfile->WriteLine(buf);
	
	if (StringCmp(name,"blastp") == 0) meth= blastp;
	else if (StringCmp(name,"blastx") == 0) meth= blastx;
	else if (StringCmp(name,"tblastn") == 0) meth= tblastn;
	else meth= blastn;
	 
	if (meth == blastp || meth == blastx) plib= fAlib;
	else plib= fNlib;
	plib->GetSelectedItem(item,name,128);
	cp= StrChr( name, ' '); if (cp) *cp= 0;
	sprintf( buf, "DATALIB %s\n", name);	qfile->WriteLine(buf);

	if (fOptionWin) {
		fDesc->GetTitle(name,128); 
		sprintf( buf, "DESCRIPTION %s\n", name); qfile->WriteLine(buf);
		
		fAlign->GetTitle(name,128); 
		sprintf( buf, "ALIGNMENTS %s\n", name);  qfile->WriteLine(buf);
	
		fExpect->GetTitle(name,128); 
		if (*name) { sprintf( buf, "EXPECT %s\n", name);	qfile->WriteLine(buf); }
	
		fCutoff->GetTitle(name,128); 
		if (*name) { sprintf( buf, "CUTOFF %s\n", name);	qfile->WriteLine(buf); }
	
		if (meth != blastn) {
			fMatrix->GetSelectedItem(item,name,128);
			cp= StrChr( name, ' '); if (cp) *cp= 0;
			sprintf( buf, "MATRIX %s\n", name);	qfile->WriteLine(buf);
			}
	
		if (meth != blastn) {
			fStrand->GetSelectedItem(item,name,128);
			if (item > 1) {
				cp= StrChr( name, ' '); if (cp) *cp= 0;
				sprintf( buf, "STRAND %s\n", name);	qfile->WriteLine(buf);
				}
			}
	
		fFilter->GetSelectedItem(item,name,128);
		if (item > 1) {
			cp= StrChr( name, ' '); if (cp) *cp= 0;
			sprintf( buf, "FILTER %s\n", name);	qfile->WriteLine(buf);
			}
			
		if (fHistogram->GetStatus() && meth != blastx) { 
			sprintf( buf, "HISTOGRAM yes\n");	qfile->WriteLine(buf); 
			}
		}
		
	sprintf( buf, "BEGIN\n", name);	qfile->WriteLine(buf);
	
		//!! must use bases from fMsg text
	msg= fMsg->GetText();
	fSeq->SetBases( msg);
	MemFree(msg);
	DSeqFile::DontSaveMasks();
	fSeq->DoWrite( qfile, DSeqFile::kPearson); 
	DSeqFile::DoSaveMasks();
	ulong msgbytes;
	msg= qfile->ReadIntoMemory( msgbytes);
	delete qfile;

	return msg;
}



Boolean DNCBIBlast::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {

		case cProtMeth:
			if (fAmeth->GetValue() == 1) {
				fNlib->Disable();
				fAlib->Enable();
				}
			else {
				fAlib->Disable();
				fNlib->Enable();
				}
			return true;

		case cNucMeth:
			if (fNmeth->GetValue() == 2) {
				fNlib->Disable();
				fAlib->Enable();
				}
			else {
				fAlib->Disable();
				fNlib->Enable();
				}
			return true;
			
		case cBaseKind:
			if (fBaseKind->GetValue() == 2) {
				fNmeth->Hide();
				fAmeth->Show();
				}
			else {
				fAmeth->Hide();
				fNmeth->Show();
				}
			return true;
			
		case cOptions:
			if (!fOptionWin) Options(); // build window
			if ( fOptionWin) fOptionWin->Open();
			return true;
			
		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}




void DNCBIBlast::Options()
{
	fOptionWin= new DOptWindow(this,"Blast Options");
	DView* super = fOptionWin;

			// this needs to be a subsidiary window...
	//DCluster* clu= new DCluster(0,super,0,0,false,"Options");
	//super= clu;
	
	(void) new DPrompt(0, super, "Max. descriptions of matching seqs");
	super->NextSubviewToRight();
	fDesc= new DEditText(0,super,"100",6);
	super->NextSubviewBelowLeft();
	
	(void) new DPrompt(0, super, "Max. alignments of high scoring pairs");
	super->NextSubviewToRight();
	fAlign= new DEditText(0,super,"50",6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Expectation cutoff (real number)");
	super->NextSubviewToRight();
	fExpect= new DEditText(0,super,NULL,6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Cutoff score from expectation (integer)");
	super->NextSubviewToRight();
	fCutoff= new DEditText(0,super,NULL,6);
	super->NextSubviewBelowLeft();

	fHistogram= new DCheckBox(0,super,"Show Histogram");
	super->NextSubviewBelowLeft();
	
	(void) new DPrompt(0, super, "Subsitition matrix");
	super->NextSubviewToRight();
	fMatrix= new DPopupList( cMatrix, super);
	fMatrix->AddItem("blosum62");
	fMatrix->AddItem("pam40");
	fMatrix->AddItem("pam120");
	fMatrix->AddItem("pam250");
	fMatrix->SetValue(1);
	super->NextSubviewBelowLeft();
	
	(void) new DPrompt(0, super, "Strand");
	super->NextSubviewToRight();
	fStrand= new DPopupList( cStrand, super);
	fStrand->AddItem("both");
	fStrand->AddItem("top | plus | +");
	fStrand->AddItem("bottom | minus | -");
	fStrand->SetValue(1);
	super->NextSubviewBelowLeft();
	
	(void) new DPrompt(0, super, "Filter junk");
	super->NextSubviewToRight();
	fFilter= new DPopupList( cFilter, super);
	fFilter->AddItem(" no filtering");
	fFilter->AddItem("SEG - mask out low compositional complexity");
	fFilter->AddItem("XNU - mask out short-periodicity internal repeats");
	fFilter->AddItem("SEG+XNU - filter SEG then XNU");
	fFilter->AddItem("XNU+SEG - filter XNU then SEG");
	fFilter->SetValue(1);
	super->NextSubviewBelowLeft();

}



void DNCBIBlast::Open()
{
	DView* super = this;
	DCluster* clu;

	char *shorthelp1= 
"The BLAST algorithm is a heuristic for finding ungapped,"LINEEND
"locally optimal sequence alignments.";
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth,0,gNoteFont);
	
	super->NextSubviewBelowLeft();

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;

	clu= new DCluster(0,super,0,0,true,"");
	super= clu;
	
	fBaseKind= new DCluster(cBaseKind,super,0,0,false,"Bases are");
	super= fBaseKind;
	(void) new DRadioButton(0,super,"Nucleic");
	super->NextSubviewToRight();
	(void) new DRadioButton(0,super,"Amino");
	if (fSeq->IsAmino()) fBaseKind->SetValue(2); else fBaseKind->SetValue(1);
	
	super= clu;
	
	super->NextSubviewToRight();
	fOptions= new DButton( cOptions, super, "Options");
	fOptions->SetResize( DView::fixed, DView::moveinsuper);
 
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Search method");
	super->NextSubviewBelowLeft();
	
	fAmeth= new DPopupList( cProtMeth, super);
	fAmeth->AddItem("blastp - amino query vs. protein lib");
	fAmeth->AddItem("tblastn - amino query vs. all frames of nucleic lib");	
	fAmeth->SetValue(1);

	fNmeth= new DPopupList( cNucMeth, super);
	fNmeth->AddItem("blastn - nucleic query vs. nucleic lib");
	fNmeth->AddItem("blastx - nucleic in all frames vs. amino lib");	
	fNmeth->SetValue(1);

	if (fSeq->IsAmino()) fNmeth->Hide(); else fAmeth->Hide();
	super->NextSubviewBelowLeft();
	
	
	(void) new DPrompt(0, super, "Nucleic library");
	super->NextSubviewBelowLeft();
	fNlib= new DPopupList( cNucLib, super);
	fNlib->AddItem("nr - non-redundant nucleic lib, full & updates");
	fNlib->AddItem("genbank - latest full GenBank");
	fNlib->AddItem("gbupdate - updates to GenBank");
	fNlib->AddItem("embl - latest full EMBL");
	fNlib->AddItem("emblu - updates to EMBL");
	fNlib->AddItem("pdb - from Brookhaven Protein Data Bank");
	fNlib->AddItem("vector - vector subset of GenBank");
	fNlib->AddItem("kabatnuc - from Kabat's sequences of immunological interest");
	fNlib->AddItem("dbest - expressed sequence tags data");
	fNlib->AddItem("epd - eukaroytic promotors");
	fNlib->SetValue(1);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Protein library");
	super->NextSubviewBelowLeft();
	fAlib= new DPopupList( cProtLib, super);
	fAlib->AddItem("nr - non-redundant protein lib, full & updates");
	fAlib->AddItem("swissprot - latest full Swiss-Prot");
	fAlib->AddItem("pir - latest full PIR");
	fAlib->AddItem("spupdate - updates to swiss-prot");
	fAlib->AddItem("genpept - GenPept, translated from full GenBank");
	fAlib->AddItem("gpupdate - updates to GenPept");
	fAlib->AddItem("pdb - from Brookhaven Protein Data Bank");
	fAlib->AddItem("kabatpro - from Kabat's sequences of immunological interest");
	fAlib->AddItem("tfd - transcription factors protein database");
	fAlib->AddItem("palu - 6-frame translations of human Alu repeats");
	fAlib->SetValue(1);
	super->NextSubviewBelowLeft();

	super= this;
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Your query sequence");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super, fSeq->Bases());
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}






// class DNCBIFetch


  char* DNCBIFetch::kAddress = "retrieve@ncbi.nlm.nih.gov";
  char* DNCBIFetch::kTitle = "Fetch from NCBI";

#ifdef OS_DOS
  char* DNCBIFetch::kHelp 	= "ncbi-ret.hel";	 
#else
  char* DNCBIFetch::kHelp 	= "ncbi-retrieve.help";	 
#endif											
						 
																				
DNCBIFetch::DNCBIFetch( long id, DTaskMaster* itsSuperior) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle)
{
}


char* DNCBIFetch::BuildMessage()
{
	char		buf[512], name[128], *cp;
	short	 	item;
	char		*msg = NULL;
	
			// want IOSTREAMS here !!!!!!!!!!!!!!!!!!!! not damn sprintf's
	DTempFile* qfile = new DTempFile();

	fLib->GetSelectedItem(item,name,128);
	cp= StrChr( name, ' '); if (cp) *cp= 0;
	sprintf( buf, "DATALIB %s\n", name);	qfile->WriteLine(buf);

	fDocs->GetTitle(name,128); 
	sprintf( buf, "MAXDOCS %s\n", name); qfile->WriteLine(buf);
	
	fLines->GetTitle(name,128); 
	sprintf( buf, "MAXLINES %s\n", name);  qfile->WriteLine(buf);

	if (fTitles->GetStatus()) { 
		sprintf( buf, "TITLES yes\n");	qfile->WriteLine(buf); 
		}

	fStart->GetTitle(name,128); 
	if (*name) { sprintf( buf, "STARTDOC %s\n", name);	qfile->WriteLine(buf); }
		
	sprintf( buf, "BEGIN\n", name);	qfile->WriteLine(buf);
	
	msg= fMsg->GetText();
	qfile->WriteLine(msg);
	MemFree(msg);
	
	ulong msgbytes;
	msg= qfile->ReadIntoMemory( msgbytes);
	delete qfile;

	return msg;
}



Boolean DNCBIFetch::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {
			
		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}



void DNCBIFetch::Open()
{
	DView* super = this;

	char *shorthelp1= 
"NCBI Retrieve server allows you to fetch database entries.";
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth, 0, gNoteFont);
	super->NextSubviewBelowLeft();

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;

	(void) new DPrompt(0, super, "Max. documents to send");
	super->NextSubviewToRight();
	fDocs= new DEditText(0,super,"20",6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Max. lines to send");
	super->NextSubviewToRight();
	fLines= new DEditText(0,super,"1000",6);
	super->NextSubviewBelowLeft();
	
	(void) new DPrompt(0, super, "Starting document number");
	super->NextSubviewToRight();
	fStart= new DEditText(0,super,"1",6);
	super->NextSubviewBelowLeft();
	
	fTitles= new DCheckBox(0,super,"Display titles only");
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Data library");
	super->NextSubviewBelowLeft();
	fLib= new DPopupList( cDataLib, super);
	fLib->AddItem("genbank - latest full GenBank and updates");
	fLib->AddItem("gbupdate - updates to GenBank");
	fLib->AddItem("gbonly - latest full GenBank without updates");
	fLib->AddItem("embl - latest full EMBL");
	fLib->AddItem("emblupdate - updates to EMBL");
	fLib->AddItem("swissprot - latest full Swiss-Prot");
	fLib->AddItem("swissprotupdate - updates to swiss-prot");
	fLib->AddItem("pir - latest full PIR");
	//fLib->AddItem("pdb - from Brookhaven Protein Data Bank");
	fLib->AddItem("vector - vector subset of GenBank");
	fLib->AddItem("vecbase - Vecbase, 1987 version");
	fLib->AddItem("genpept - GenPept, translated from full GenBank");
	fLib->AddItem("gpupdate - updates to GenPept");
	fLib->AddItem("kabatnuc - dna of Kabat's immunological seqs");
	fLib->AddItem("kabatpro - protein of Kabat's immunological seqs");
	fLib->AddItem("epd - eukaroytic promotors");
	fLib->AddItem("tfd - transcription factors database");
	fLib->SetValue(1);
	super->NextSubviewBelowLeft();

	super= this;
	super->NextSubviewBelowLeft();

	char *shorthelp= 
"Your query can contain several lines with locus names, accession numbers, authors'"LINEEND
"names, and/or text words. The logical operators AND, OR, and NOT are supported.";

	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp, 25*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Your query:");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super);
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}






// class DEMBLBlitz


//char* DEMBLBlitz::kAddress = "seqpup@bio.indiana.edu";
char* DEMBLBlitz::kAddress = "BLITZ@EMBL-Heidelberg.DE";
char* DEMBLBlitz::kTitle = "Blitz Search at EMBL";

#ifdef OS_DOS
char* DEMBLBlitz::kHelp 	= "embl-bli.hel";	 
#else
char* DEMBLBlitz::kHelp 	= "embl-blitz.help";	 
#endif																	 
			
																				
DEMBLBlitz::DEMBLBlitz( long id, DTaskMaster* itsSuperior, DSequence* theSeq) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle)
{
	fSeq= (DSequence*) theSeq->Clone();
}

DEMBLBlitz::~DEMBLBlitz()
{
	delete fSeq;
}

char* DEMBLBlitz::BuildMessage()
{
	char		buf[512], name[128];
	char		*msg = NULL;
	Boolean isamino= fBaseKind->GetValue() == 2;
	
			// want IOSTREAMS here !!!!!!!!!!!!!!!!!!!! not damn sprintf's
	DTempFile* qfile = new DTempFile();
 
	fTitle->GetTitle(name,128); 
	if (*name) { sprintf( buf, "TITLE %s\n", name); qfile->WriteLine(buf); }
	
	fAlign->GetTitle(name,128); 
	if (*name) { sprintf( buf, "ALIGN %s\n", name);  qfile->WriteLine(buf); }

	fScores->GetTitle(name,128); 
	if (*name) { sprintf( buf, "NAME %s\n", name);	qfile->WriteLine(buf); }

	fIndel->GetTitle(name,128); 
	if (*name) { sprintf( buf, "INDEL %s\n", name);	qfile->WriteLine(buf); }

	if (isamino) {
		fPam->GetTitle(name,128); 
		sprintf( buf, "PAM %s\n", name);	qfile->WriteLine(buf);
		}
		
	sprintf( buf, "SEQ\n", name);	qfile->WriteLine(buf);
	
		//!! must use bases from fMsg text
	msg= fMsg->GetText();
	fSeq->SetBases( msg);
	MemFree(msg);
	DSeqFile::DontSaveMasks();
	fSeq->DoWrite( qfile, DSeqFile::kPlain); 
	DSeqFile::DoSaveMasks();
		
	sprintf( buf, "END\n");	qfile->WriteLine(buf);
	
	ulong msgbytes;
	msg= qfile->ReadIntoMemory( msgbytes);
	delete qfile;

	return msg;
}



Boolean DEMBLBlitz::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {

		case cBaseKind:
			if (fBaseKind->GetValue() == 2) {
				fPam->Enable();
				}
			else {
				fPam->Disable();
				}
			return true;
			
		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}



void DEMBLBlitz::Open()
{
	DView* super = this;

	char *shorthelp1= 
"MPsrch allows you to perform sensitive and fast comparisons of your protein sequences against"LINEEND
"the Swiss-Prot database using the Smith and Waterman best local similarity algorithm.";
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;

	fBaseKind= new DCluster(cBaseKind,super,0,0,false,"Bases are");
	super= fBaseKind;
	(void) new DRadioButton(0,super,"Nucleic");
	super->NextSubviewToRight();
	(void) new DRadioButton(0,super,"Amino");
	if (fSeq->IsAmino()) fBaseKind->SetValue(2); else fBaseKind->SetValue(1);
	
	super= this;
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Max. scores to report");
	super->NextSubviewToRight();
	fScores= new DEditText(0,super,"50",6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Max. best alignments to report");
	super->NextSubviewToRight();
	fAlign= new DEditText(0,super,"50",6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Indel penalty (option)");
	super->NextSubviewToRight();
	fIndel= new DEditText(0,super,NULL,6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "PAM value from 1..500 (protein option)");
	super->NextSubviewToRight();
	fPam= new DEditText(0,super,NULL,6);
	if (!fSeq->IsAmino()) fPam->Disable();
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Title of this search");
	super->NextSubviewBelowLeft();
	char  title[80];
	StrCpy(title, "Blitz of ");
	StrNCat(title, fSeq->Name(), 30);
	fTitle= new DEditText(0,super,title,30);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Your query sequence:");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super, fSeq->Bases());
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}







// class DEMBLFasta


char* DEMBLFasta::kAddress = "FASTA@EMBL-Heidelberg.DE";
char* DEMBLFasta::kTitle = "FastA Search at EMBL";
#ifdef OS_DOS
char* DEMBLFasta::kHelp 	= "embl-fas.hel";	 
#else
char* DEMBLFasta::kHelp 	= "embl-fasta.help";	 
#endif																	 
			
																				
DEMBLFasta::DEMBLFasta( long id, DTaskMaster* itsSuperior, DSequence* theSeq) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle)
{
	fSeq= (DSequence*) theSeq->Clone();
}

DEMBLFasta::~DEMBLFasta()
{
	delete fSeq;
}

char* DEMBLFasta::BuildMessage()
{
	char		buf[512], name[128], *cp;
	short	 	item;
	char		*msg = NULL;
	Boolean isamino= fBaseKind->GetValue() == 2;
	
			// want IOSTREAMS here !!!!!!!!!!!!!!!!!!!! not damn sprintf's
	DTempFile* qfile = new DTempFile();
 
	fTitle->GetTitle(name,128); 
	if (*name) { sprintf( buf, "TITLE %s\n", name); qfile->WriteLine(buf); }
	
	fScores->GetTitle(name,128); 
	if (*name) { sprintf( buf, "LIST %s\n", name);	qfile->WriteLine(buf); }

	fAlign->GetTitle(name,128); 
	if (*name) { sprintf( buf, "ALIGN %s\n", name);  qfile->WriteLine(buf); }

	fWord->GetTitle(name,128); 
	if (*name) { sprintf( buf, "WORD %s\n", name);	qfile->WriteLine(buf); }

	fLib->GetSelectedItem(item,name,128);
	cp= StrChr( name, ' '); if (cp) *cp= 0;
	if (*name) { sprintf( buf, "LIB %s\n", name);	qfile->WriteLine(buf); }
	 
	if (!fBothStrands->GetStatus()) { 
		sprintf( buf, "ONE\n");	qfile->WriteLine(buf); 
		}
	
	if (isamino && fSeq->LengthF()<50) {
		sprintf( buf, "PROT\n");	qfile->WriteLine(buf);
		}
		
	sprintf( buf, "SEQ %s\n", name);	qfile->WriteLine(buf);
	
		//!! must use bases from fMsg text
	msg= fMsg->GetText();
	fSeq->SetBases( msg);
	MemFree(msg);
	DSeqFile::DontSaveMasks();
	fSeq->DoWrite( qfile, DSeqFile::kPlain); 
	DSeqFile::DoSaveMasks();
		
	sprintf( buf, "END\n");	qfile->WriteLine(buf);
	
	ulong msgbytes;
	msg= qfile->ReadIntoMemory( msgbytes);
	delete qfile;

	return msg;
}



Boolean DEMBLFasta::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {

		case cBaseKind:
			if (fBaseKind->GetValue() == 2) {
				//fPam->Enable();
				}
			else {
				//fPam->Disable();
				}
			return true;
			
		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}



void DEMBLFasta::Open()
{
	DView* super = this;

	char *shorthelp1= 
"This is based on the FASTA program developed by Pearson and Lipman.  It allows you to perform"LINEEND
"fast and sensitive comparisons of your nucleic acid or protein sequences against various databases.";
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;


	(void) new DPrompt(0, super, "Data library:");
	super->NextSubviewBelowLeft();
	fLib= new DPopupList( cDataLib, super);
	fLib->AddItem("EMALL - latest full EMBL & updates");
	fLib->AddItem("EMNEW - updates to EMBL");
	fLib->AddItem(" --- ");
	fLib->AddItem("EFUN - EMBL fungi division only");
	fLib->AddItem("EINV - EMBL invertebrates division only");
	fLib->AddItem("EMAM - EMBL mammals division only");
	fLib->AddItem("EORG - EMBL organelles division only");
	fLib->AddItem("EPHG - EMBL phages division only");
	fLib->AddItem("EPLN - EMBL plants division only");
	fLib->AddItem("EPRI - EMBL primates division only");
	fLib->AddItem("EPRO - EMBL prokaryotes division only");
	fLib->AddItem("EROD - EMBL rodents division only");
	fLib->AddItem("ESYN - EMBL synthetic division only");
	fLib->AddItem("EUNA - EMBL unannotated division only");
	fLib->AddItem("EVRL - EMBL viruses division only");
	fLib->AddItem("EVRT - EMBL vertebrates division only");
	fLib->AddItem(" --- ");
	fLib->AddItem("GBALL - latest full GenBank & updates");
	fLib->AddItem("GBNEW - updates to GenBank");
	fLib->AddItem("GBONLY - latest full GenBank");
	fLib->AddItem("GENEMBL - latest full Genbank & EMBL");
	fLib->AddItem("GENEW - updates to GenBank & EMBL");
	fLib->AddItem(" --- ");
	fLib->AddItem("SWALL - latest full swiss-prot & updates");
	fLib->AddItem("SWNEW - updates to swiss-prot");
	fLib->AddItem("SW - latest full swiss-prot");
	fLib->AddItem("NBRF - latest full NBRF|PIR");
	fLib->AddItem("PIRONLY - entries in PIR, but not swiss-prot");
	fLib->AddItem("SWISSPIRALL - latest full swiss-prot & PIR");
	fLib->AddItem("BROOKHAVEN - from Brookhaven protein data bank");
	fLib->AddItem("NRL - from Brookhaven PDB, NBRF version");
	fLib->SetValue(1);
	super->NextSubviewBelowLeft();


	(void) new DPrompt(0, super, "Max. scores to report");
	super->NextSubviewToRight();
	fScores= new DEditText(0,super,"50",6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Max. best alignments to report");
	super->NextSubviewToRight();
	fAlign= new DEditText(0,super,"10",6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Word size (optional)");
	super->NextSubviewToRight();
	fWord= new DEditText(0,super,NULL,6);
	super->NextSubviewBelowLeft();

	fBaseKind= new DCluster(cBaseKind,super,0,0,false,"Bases are");
	super= fBaseKind;
	(void) new DRadioButton(0,super,"Nucleic");
	super->NextSubviewToRight();
	(void) new DRadioButton(0,super,"Amino");
	if (fSeq->IsAmino()) fBaseKind->SetValue(2); else fBaseKind->SetValue(1);
	
	super= this;
	super->NextSubviewToRight();
	fBothStrands= new DCheckBox(0,super,"Search both strands");
	fBothStrands->SetStatus(true);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Title of this search");
	super->NextSubviewBelowLeft();
	char  title[80];
	StrCpy(title, "FastA of ");
	StrNCat(title, fSeq->Name(), 30);
	fTitle= new DEditText(0,super,title,30);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Your query sequence:");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super, fSeq->Bases());
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}









// class DEMBLQuicks


char* DEMBLQuicks::kAddress = "QUICK@EMBL-Heidelberg.DE";
char* DEMBLQuicks::kTitle = "QuickSearch at EMBL";
#ifdef OS_DOS
char* DEMBLQuicks::kHelp 	= "emblquic.hel";	 
#else
char* DEMBLQuicks::kHelp 	= "embl-quicksearch.help";	 
#endif																	 
			
																				
DEMBLQuicks::DEMBLQuicks( long id, DTaskMaster* itsSuperior, DSequence* theSeq) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle),
	fSeq(NULL)
{
	if (theSeq->IsAmino()) {
			Message(MSG_OK,"This method not available for Amino sequences.");
			delete this;
			return;
			}
	fSeq= (DSequence*) theSeq->Clone();
}

DEMBLQuicks::~DEMBLQuicks()
{
	if (fSeq) delete fSeq;
}

char* DEMBLQuicks::BuildMessage()
{
	char		buf[512], name[128], *cp;
	short	 	item;
	char		*msg = NULL;
	
			// want IOSTREAMS here !!!!!!!!!!!!!!!!!!!! not damn sprintf's
	DTempFile* qfile = new DTempFile();
 
	fTitle->GetTitle(name,128); 
	if (*name) { sprintf( buf, "TITLE %s\n", name); qfile->WriteLine(buf); }
	
	fLib->GetSelectedItem(item,name,128);
	cp= StrChr( name, ' '); if (cp) *cp= 0;
	if (*name) { sprintf( buf, "LIB %s\n", name);	qfile->WriteLine(buf); }
	 
	fStringency->GetTitle(name,128); 
	if (*name) { sprintf( buf, "STRINGENCY %s\n", name);  qfile->WriteLine(buf); }

	fWindow->GetTitle(name,128); 
	if (*name) { sprintf( buf, "WINDOW %s\n", name);	qfile->WriteLine(buf); }

	if (fPerfect->GetStatus()) { 
		sprintf( buf, "PERFECT\n");	qfile->WriteLine(buf); 
		}
	else {
		fMatch->GetTitle(name,128); 
		if (*name) { sprintf( buf, "MATCH %s\n", name);	qfile->WriteLine(buf); }
		}

	fBest->GetSelectedItem(item,name,128);
	if (item == 2) { sprintf( buf, "BEST\n");	qfile->WriteLine(buf); }

	if (!fBothStrands->GetStatus()) { 
		sprintf( buf, "ONE\n");	qfile->WriteLine(buf); 
		}
		
	sprintf( buf, "SEQ\n", name);	qfile->WriteLine(buf);
	
		//!! must use bases from fMsg text
	msg= fMsg->GetText();
	fSeq->SetBases( msg);
	MemFree(msg);
	DSeqFile::DontSaveMasks();
	fSeq->DoWrite( qfile, DSeqFile::kPlain); 
	DSeqFile::DoSaveMasks();
		
	sprintf( buf, "END\n");	qfile->WriteLine(buf);
	
	ulong msgbytes;
	msg= qfile->ReadIntoMemory( msgbytes);
	delete qfile;

	return msg;
}



Boolean DEMBLQuicks::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {

		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}



void DEMBLQuicks::Open()
{
	DView* super = this;

	char *shorthelp1= 
"This is based on the QUICKSEARCH and QUICKSHOW programs developed by John Devereux"LINEEND
"as implemented in the GCG package.  It allows you to perform very rapid comparisons"LINEEND
"of your nucleic acid sequences against the EMBL and GenBank databases including the"LINEEND
"most recent entries.";
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;

	(void) new DPrompt(0, super, "Data library:");
	super->NextSubviewBelowLeft();
	fLib= new DPopupList( 0, super);
	fLib->AddItem("ALL - all EMBL and GenBank entries");
	fLib->AddItem("GENEW - updates to GenBank & EMBL");
	fLib->SetValue(1);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Percent identity for match");
	super->NextSubviewToRight();
	fMatch= new DEditText(0,super,"90",6);
	super->NextSubviewBelowLeft();

	fPerfect= new DCheckBox(0,super,"Perfect matches only");
	super->NextSubviewToRight();
	fBothStrands= new DCheckBox(0,super,"Search both strands");
	fBothStrands->SetStatus(true);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Window (option)");
	super->NextSubviewToRight();
	fWindow= new DEditText(0,super,NULL,6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Stringency (option)");
	super->NextSubviewToRight();
	fStringency= new DEditText(0,super,NULL,6);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Aligment method");
	super->NextSubviewToRight();
	fBest= new DPopupList( 0, super);
	fBest->AddItem("Needleman-Wunsch");
	fBest->AddItem("Local-homology");
	fBest->SetValue(1);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Title of this search");
	super->NextSubviewBelowLeft();
	char  title[80];
	StrCpy(title, "Quicksearch of ");
	StrNCat(title, fSeq->Name(), 30);
	fTitle= new DEditText(0,super,title,30);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Your query sequence:");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super, fSeq->Bases());
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}




// class DEMBLFetch

char* DEMBLFetch::kAddress = "NETSERV@EMBL-Heidelberg.DE";
char* DEMBLFetch::kTitle = "Fetch from EMBL";
#ifdef OS_DOS
char* DEMBLFetch::kHelp 	= "embl-fet.hel";	 
#else
char* DEMBLFetch::kHelp 	= "embl-fetch.help";	 
#endif											
						 
																				
DEMBLFetch::DEMBLFetch( long id, DTaskMaster* itsSuperior) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle)
{
}

char* DEMBLFetch::BuildMessage()
{
	return fMsg->GetText();
}

Boolean DEMBLFetch::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {
			
		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}


void DEMBLFetch::Open()
{
	DView* super = this;

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;

	char *shorthelp1= 
"Send one command per line. The general syntax is 'GET database:accnumber' where database is"LINEEND
"either NUC (EMBL or GenBank) or PROT (Swiss-Prot), eg. 'GET NUC:J00179'"LINEEND
"Include the command HELP to get introductory information.";

	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Your query:");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super);
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}






// class DFHCRCfetch

char* DFHCRCfetch::kAddress = "blocks@howard.fhcrc.org";
char* DFHCRCfetch::kTitle = "Fetch from FHCRC BLOCKS";
#ifdef OS_DOS
char* DFHCRCfetch::kHelp 	= "fhcrc-bl.hel";	 
#else
char* DFHCRCfetch::kHelp 	= "fhcrc-blocks.help";	 
#endif											
	 
																				
DFHCRCfetch::DFHCRCfetch( long id, DTaskMaster* itsSuperior) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle)
{
}

char* DFHCRCfetch::BuildMessage()
{
	return "";  // sendmail requires nonNULL msg string
}

Boolean DFHCRCfetch::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {
			
		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			{
			char sub[128];
			fSubj->GetTitle(sub, 80);
			if (StrNICmp(sub,"GET ",4) != 0) {
				char newsub[128];
				StrCpy(newsub, "GET ");
				StrCat(newsub, sub);
				fSubj->SetTitle(newsub);
				}
			return DSendMailDialog::IsMyAction(action);	
			}
		}
}

DView* DFHCRCfetch::InstallSubject(DView* super, char* subjStr)
{
	DPrompt* pr= new DPrompt(0, super, "Blocks to get:", 0, 0, Nlm_programFont);
	super->NextSubviewToRight();

	DDialogText* esubj= new DEditText(cSubj, super, subjStr, 20);
	this->SetEditText(esubj);
	super->NextSubviewBelowLeft();
	return esubj;
}


void DFHCRCfetch::Open()
{
	DView* super = this;

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one

	char *shorthelp1=
"Specify one BLOCKS number to get per message, eg. 'GET BL00044'";
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 20*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	DView *esubj= this->InstallSubject(super, "GET "); 
	DView *eccopy= NULL;

	//(void) new DPrompt(0, super, "Your query:");
	//super->NextSubviewBelowLeft();
	DView *emsg= NULL; //= this->InstallMessage(super);
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}



// class DFHCRCblocks


char* DFHCRCblocks::kAddress = "blocks@howard.fhcrc.org";
char* DFHCRCblocks::kTitle = "BLOCKS Search at FHCRC";
#ifdef OS_DOS
char* DFHCRCblocks::kHelp 	= "fhcrc-bl.hel";	 
#else
char* DFHCRCblocks::kHelp 	= "fhcrc-blocks.help";	 
#endif																	 
			
																				
DFHCRCblocks::DFHCRCblocks( long id, DTaskMaster* itsSuperior, DSequence* theSeq) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle)
{
	fSeq= (DSequence*) theSeq->Clone();
}

DFHCRCblocks::~DFHCRCblocks()
{
	delete fSeq;
}

char* DFHCRCblocks::BuildMessage()
{
	char		*msg = NULL;
	
			// want IOSTREAMS here !!!!!!!!!!!!!!!!!!!! not damn sprintf's
	DTempFile* qfile = new DTempFile();
 
	msg= fMsg->GetText();
	fSeq->SetBases( msg);
	MemFree(msg);
	DSeqFile::DontSaveMasks();
	fSeq->DoWrite( qfile, DSeqFile::kPearson); 
	DSeqFile::DoSaveMasks();
			
	ulong msgbytes;
	msg= qfile->ReadIntoMemory( msgbytes);
	delete qfile;

	return msg;
}


Boolean DFHCRCblocks::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {

		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}



void DFHCRCblocks::Open()
{
	DView* super = this;

	char* shorthelp1 =
"As an aid to detection and verification of protein sequence homology, the BLOCKS e-mail"LINEEND
"searcher compares a protein or DNA sequence to the current database of protein blocks."LINEEND
"Blocks are short multiply aligned ungapped segments corresponding to the most highly"LINEEND
"conserved regions of proteins.";
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;

	(void) new DPrompt(0, super, "Your query sequence:");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super, fSeq->Bases());
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}









// class DUWFGeneID


char* DUWFGeneID::kAddress = "geneid@bir.cedb.uwf.edu";
char* DUWFGeneID::kTitle = "GeneID at U.W.Florida";
#ifdef OS_DOS
char* DUWFGeneID::kHelp 	= "geneid.hel";	 
#else
char* DUWFGeneID::kHelp 	= "geneid.help";	 
#endif																	 
			
																				
DUWFGeneID::DUWFGeneID( long id, DTaskMaster* itsSuperior, DSequence* theSeq) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle),
	fSeq(NULL)
{
	fSeq= (DSequence*) theSeq->Clone();
}

DUWFGeneID::~DUWFGeneID()
{
	if (fSeq) delete fSeq;
}

char* DUWFGeneID::BuildMessage()
{
	char		buf[512], name[128], name2[128];
	char		*msg = NULL;
	
			// want IOSTREAMS here !!!!!!!!!!!!!!!!!!!! not damn sprintf's
	DTempFile* qfile = new DTempFile();

 	sprintf( buf, "Genomic Sequence");	qfile->WriteLine(buf);

	if (fSmall->GetStatus()) { 
		sprintf( buf, " -small_output");	qfile->WriteLine(buf); 
		}
	if (!fExonBlast->GetStatus()) { 
		sprintf( buf, " -noexonblast");	qfile->WriteLine(buf); 
		}
	if (fGeneBlast->GetStatus()) { 
		sprintf( buf, " -geneblast");	qfile->WriteLine(buf); 
		}
	if (fNetGene->GetStatus()) { 
		sprintf( buf, " -netgene");	qfile->WriteLine(buf); 
		}
	 
	fCodeX->GetTitle(name,128); 
	fCodeY->GetTitle(name2,128); 
	if (*name && *name2) {	
		sprintf( buf, " -known_coding %s %s",name,name2);	
		qfile->WriteLine(buf); 
		}

	fFirstX->GetTitle(name,128); 
	fFirstY->GetTitle(name2,128); 
	if (*name && *name2) {	
		sprintf( buf, " -first_exon %s %s",name,name2);	
		qfile->WriteLine(buf); 
		}

	fLastX->GetTitle(name,128); 
	fLastY->GetTitle(name2,128); 
	if (*name && *name2) {	
		sprintf( buf, " -last_exon %s %s",name,name2);	
		qfile->WriteLine(buf); 
		}
		
	fAltX->GetTitle(name,128); 
	fAltY->GetTitle(name2,128); 
	if (*name && *name2) {	
		sprintf( buf, " -alt_splicing %s %s",name,name2);	
		qfile->WriteLine(buf); 
		}
 
 	sprintf( buf, "\n\n");	qfile->WriteLine(buf);

	msg= fMsg->GetText();
	fSeq->SetBases( msg);
	MemFree(msg);
	DSeqFile::DontSaveMasks();
	fSeq->DoWrite( qfile, DSeqFile::kPearson); 
	DSeqFile::DoSaveMasks();
			
	ulong msgbytes;
	msg= qfile->ReadIntoMemory( msgbytes);
	delete qfile;

	return msg;
}



Boolean DUWFGeneID::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {

		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}



void DUWFGeneID::Open()
{
	DView* super = this;
	DCluster* clu;

	char *shorthelp1= 
"GeneID is an Artificial Intelligence system for analyzing vertebrate genomic DNA and"LINEEND
"prediction of exons and gene structure. You have the option of having NetGene splice"LINEEND
"site analysis also.  GeneID currently offers no analysis of cDNA or protein sequences.";
     
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;

	clu= new DCluster(0,super,0,0,false,"Options");
	super= clu;

	(void) new DPrompt(0, super, "NetGene analysis also");
	super->NextSubviewToRight();
	fNetGene= new DCheckBox(0,super,"");
	super->NextSubviewToRight();
	
	(void) new DPrompt(0, super, "Brief output");
	super->NextSubviewToRight();
	fSmall= new DCheckBox(0,super,"");
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Compare predicted gene model to aa databases w/ blast");
	super->NextSubviewToRight();
	fGeneBlast= new DCheckBox(0,super, "");
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Use exon blast (protein database search)");
	super->NextSubviewToRight();
	fExonBlast= new DCheckBox(0,super,"");
	fExonBlast->SetStatus(true);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Region of known coding (start,end)");
	super->NextSubviewToRight();
	fCodeX= new DEditText(0,super,NULL,4);
	super->NextSubviewToRight();
	fCodeY= new DEditText(0,super,NULL,4);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Coordinates of known first exon (start,end)");
	super->NextSubviewToRight();
	fFirstX= new DEditText(0,super,NULL,4);
	super->NextSubviewToRight();
	fFirstY= new DEditText(0,super,NULL,4);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Coordinates of known last exon (start,end)");
	super->NextSubviewToRight();
	fLastX= new DEditText(0,super,NULL,4);
	super->NextSubviewToRight();
	fLastY= new DEditText(0,super,NULL,4);
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Region where splicing is blocked (start,end)");
	super->NextSubviewToRight();
	fAltX= new DEditText(0,super,NULL,4);
	super->NextSubviewToRight();
	fAltY= new DEditText(0,super,NULL,4);
	super->NextSubviewBelowLeft();

	super= this;
	
	(void) new DPrompt(0, super, "Your query sequence:");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super, fSeq->Bases());
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}










// class DORNLGrail

// note: this method can handle multi-seq input -- fix later for fSeqList

char* DORNLGrail::kAddress = "GRAIL@ornl.gov";
char* DORNLGrail::kTitle = "Grail at ORNL";
#ifdef OS_DOS
char* DORNLGrail::kHelp 	= "grail.hel";	 
#else
char* DORNLGrail::kHelp 	= "grail.help";	 
#endif																	 
			
																				
DORNLGrail::DORNLGrail( long id, DTaskMaster* itsSuperior, DSequence* theSeq) :
	DSendMailDialog(id, itsSuperior,	-5,  -5,  -50,  -20, kTitle),
	fSeq(NULL)
{
	fSeq= (DSequence*) theSeq->Clone();
}

DORNLGrail::~DORNLGrail()
{
	if (fSeq) delete fSeq;
}

char* DORNLGrail::BuildMessage()
{
	char		buf[512];
	char		*msg = NULL;
	
			// want IOSTREAMS here !!!!!!!!!!!!!!!!!!!! not damn sprintf's
	DTempFile* qfile = new DTempFile();

	short nseq= 1; // fix later for SeqList input
 	sprintf( buf, "Sequences %d", nseq);	qfile->WriteLine(buf);

	if (fGrail2->GetStatus()) { 
		sprintf( buf, " -2");	qfile->WriteLine(buf); 
		}
	if (fExon->GetStatus()) { 
		sprintf( buf, " -E");	qfile->WriteLine(buf); 
		}
 
 	sprintf( buf, "\n");	qfile->WriteLine(buf);

	msg= fMsg->GetText();
	fSeq->SetBases( msg);
	MemFree(msg);
	DSeqFile::DontSaveMasks();
	fSeq->DoWrite( qfile, DSeqFile::kPearson); 
	DSeqFile::DoSaveMasks();
			
	ulong msgbytes;
	msg= qfile->ReadIntoMemory( msgbytes);
	delete qfile;

	return msg;
}



Boolean DORNLGrail::IsMyAction(DTaskMaster* action) 
{	
	switch(action->Id()) {

		case cHelpButton:
			gApplication->OpenHelp(kHelp);
			return true;			
		 
		default:
			return DSendMailDialog::IsMyAction(action);	
		}
}



void DORNLGrail::Open()
{
	DView* super = this;
	DCluster* clu;

	char *shorthelp1= 
"GRAIL provides analysis of protein coding potential of a DNA sequence, and an option"LINEEND
"for protein sequence database search of putative coding regions.  The coding"LINEEND
"recognition portion of the system uses a neural network which combines a series of"LINEEND
"coding prediction algorithms.";
     
	SETNOTEFONT();
	(void) new DNotePanel(0,super,shorthelp1, 25*Nlm_stdCharWidth,0,gNoteFont);
	super->NextSubviewBelowLeft();

	DView *eto  = this->InstallTo(super, kAddress);
	DView *efrom= this->InstallFrom(super);  // don't have to display this one
	DView *esubj= NULL; 
	DView *eccopy= NULL;

	clu= new DCluster(0,super,0,0,false,"Options");
	super= clu;

	(void) new DPrompt(0, super, "Use Grail version 2");
	super->NextSubviewToRight();
	fGrail2= new DCheckBox(0,super, "");
	super->NextSubviewBelowLeft();

	(void) new DPrompt(0, super, "Compare found exon to protein database");
	super->NextSubviewToRight();
	fExon= new DCheckBox(0,super, "");
	super->NextSubviewBelowLeft();

	super= this;
	
	(void) new DPrompt(0, super, "Your query sequence:");
	super->NextSubviewBelowLeft();
	DView *emsg= this->InstallMessage(super, fSeq->Bases());
	
	SetDlogItems( eto, efrom, eccopy, esubj, emsg);
	
	AddOkayCancelButtons(cSEND,"Send");
	this->NextSubviewToRight();
	DButton* hb= new DButton( cHelpButton, this, "Help");
	hb->SetResize( DView::fixed, DView::moveinsuper);
	
	DWindow::Open();
}


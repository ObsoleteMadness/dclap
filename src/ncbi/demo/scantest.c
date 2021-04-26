#include <ncbi.h>
#include <casn.h>
#include <seqport.h>
#include <objall.h>

#define ONLY_DNA 1
#define ONLY_PRT 2
#define ONLY_CDS 3

#define DESIRED ONLY_DNA

#define CHARSPERLINE  50

typedef struct expstruct {
  FILE      *fp;
  AsnIoPtr  aip;
  Boolean   is_na;
  Uint1     feat;
} ExpStruct, PNTR ExpStructPtr;

static void PrintSequence (BioseqPtr bsp, SeqFeatPtr sfp,
                           FILE *fp, Boolean is_na)

{
  Char        buffer [255];
  Uint1       code;
  Int2        count;
  Uint1       repr;
  Uint1       residue;
  SeqPortPtr  spp;
  CharPtr     title;
  CharPtr     tmp;

  if (bsp != NULL && fp != NULL) {
    if ((Boolean) ISA_na (bsp->mol) == is_na) {
      repr = Bioseq_repr (bsp);
      if (repr == Seq_repr_raw || repr == Seq_repr_const) {
        title = BioseqGetTitle (bsp);
        tmp = StringMove (buffer, ">");
        tmp = SeqIdPrint (bsp->id, tmp, PRINTID_FASTA_LONG);
        tmp = StringMove (tmp, " ");
        StringNCpy (tmp, title, 200);
        fprintf (fp, "%s\n", buffer);
        if (is_na) {
          code = Seq_code_iupacna;
        } else {
          code = Seq_code_iupacaa;
        }
        if (sfp != NULL) {
          spp = SeqPortNewByLoc (sfp->location, code);
        } else {
          spp = SeqPortNew (bsp, 0, -1, 0, code);
        }
        if (spp != NULL) {
          count = 0;
          while ((residue = SeqPortGetResidue (spp)) != SEQPORT_EOF) {
            if (! IS_residue (residue)) {
              buffer [count] = '\0';
              fprintf (fp, "%s\n", buffer);
              count = 0;
              switch (residue) {
                case SEQPORT_VIRT :
                  fprintf (fp, "[Gap]\n");
                  break;
                case SEQPORT_EOS :
                  fprintf (fp, "[EOS]\n");
                  break;
                default :
                  fprintf (fp, "[Invalid Residue]\n");
                  break;
              }
            } else {
              buffer [count] = residue;
              count++;
              if (count >= CHARSPERLINE) {
                buffer [count] = '\0';
                fprintf (fp, "%s\n", buffer);
                count = 0;
              }
            }
          }
          if (count != 0) {
            buffer [count] = '\0';
            fprintf (fp, "%s\n", buffer);
          }
          SeqPortFree (spp);
        }
      }
    }
  }
}

static void GetSeqFeat (AsnExpOptStructPtr aeosp)

{
  BioseqPtr     bsp;
  ExpStructPtr  esp;
  SeqFeatPtr    sfp;

  if (aeosp->dvp->intvalue == START_STRUCT) {
    esp = (ExpStructPtr) aeosp->data;
    sfp = (SeqFeatPtr) aeosp->the_struct;
    if (esp != NULL && esp->fp != NULL && sfp != NULL &&
        sfp->data.choice == esp->feat) {
      bsp = BioseqFind (SeqLocId (sfp->location));
      if (bsp != NULL) {
        PrintSequence (bsp, sfp, esp->fp, esp->is_na);
      }
    }
  }
}

static void SeqEntryToFeat (SeqEntryPtr sep, FILE *fp,
                            Boolean is_na, Uint1 feat)

{
  AsnExpOptPtr  aeop;
  AsnIoPtr      aip;
  ExpStructPtr  esp;

  if (sep != NULL && fp != NULL) {
    esp = MemNew (sizeof (ExpStruct));
    if (esp != NULL) {
      aip = AsnIoNullOpen ();
      if (aip != NULL) {
        esp->fp = fp;
        esp->aip = AsnIoNew (ASNIO_TEXT_OUT, fp, NULL, NULL, NULL);
        esp->is_na = is_na;
        esp->feat = feat;
        aeop = AsnExpOptNew (aip, "Seq-feat", (Pointer) esp, GetSeqFeat);
        if (aeop != NULL) {
          SeqEntryAsnWrite (sep, aip, NULL);
          fflush (fp);
          AsnExpOptFree (aip, aeop);
        }
        AsnIoClose (aip);
      }
      MemFree (esp);
    }
  }
}

static void GetFastaSeq (SeqEntryPtr sep, Pointer data,
                         Int4 index, Int2 indent)

{
  BioseqPtr     bsp;
  ExpStructPtr  esp;

  if (IS_Bioseq (sep)) {
    bsp = (BioseqPtr) sep->data.ptrvalue;
    esp = (ExpStructPtr) data;
    if (bsp != NULL && esp != NULL && esp->fp != NULL) {
      PrintSequence (bsp, NULL, esp->fp, esp->is_na);
    }
  }
}

static void SeqEntryScan (SeqEntryPtr sep, FILE *fp, Boolean is_na)

{
  ExpStruct  es;

  if (sep != NULL && fp != NULL) {
    es.fp = fp;
    es.aip = NULL;
    es.is_na = is_na;
    es.feat = 0;
    SeqEntryExplore (sep, (Pointer) &es, GetFastaSeq);
  }
}

static void ProcessSeqEntry (SeqEntryPtr sep, FILE *fp, Int2 desired)

{
  if (sep != NULL && fp != NULL) {
    if (desired == ONLY_DNA) {
      SeqEntryScan (sep, fp, TRUE);
    } else if (desired == ONLY_PRT) {
      SeqEntryScan (sep, fp, FALSE);
    } else if (desired == ONLY_CDS) {
      SeqEntryToFeat (sep, fp, TRUE, 3);
    }
  }
}

static void ScanSequenceFiles (CharPtr root, Int2 desired)

{
  CASN_Handle  casnh;
  Char         filename [FILENAME_MAX];
  Int2         filenum;
  FILE         *fp;
  Boolean      goOn;
  Char         path [PATH_MAX];
  SeqEntryPtr  sep;

  filenum = 1;
  goOn = TRUE;
  fp = FileOpen ("scan.out", "w");
  if (fp != NULL) {
    while (goOn) {
#ifdef OS_MAC
      sprintf (filename, "seqasn.%03d;1", (int) filenum);
#else
      sprintf (filename, "seqasn.%03d", (int) filenum);
#endif
      StringCpy (path, root);
      FileBuildPath (path, "sequence", NULL);
      FileBuildPath (path, NULL, filename);
      if ((casnh = CASN_Open (path)) != NULL) {
        if (CASN_DocType (casnh) == CASN_TypeSeq) {
          while (goOn && ((sep = CASN_NextSeqEntry (casnh)) != NULL)) {
            ProcessSeqEntry (sep, fp, desired);
            SeqEntryFree (sep);
          }
        }
        CASN_Close (casnh);
        filenum++;
        FileClose (fp);
        fp = FileOpen ("scan.out", "a");
        if (fp == NULL) {
          goOn = FALSE;
          Message (MSG_FATAL, "Unable to reopen output file");
        }
      } else {
        goOn = FALSE;
      }
    }
    FileClose (fp);
  } else {
    Message (MSG_FATAL, "Unable to open output file");
  }
}

extern Int2 Main (void)

{
  Char  root [PATH_MAX];

  if (GetAppParam ("NCBI", "NCBI", NULL, "", root, sizeof (root) - 1)) {
    if (GetAppParam ("NCBI", "NCBI", "ROOT", "", root, sizeof (root) - 1)) {
      if (AllObjLoad () && SeqCodeSetLoad ()) {
        ScanSequenceFiles (root, DESIRED);
      } else {
        Message (MSG_FATAL, "Unable to load parse tables");
      }
    } else {
      Message (MSG_FATAL, "Unable to find root path");
    }
  } else {
    Message (MSG_FATAL, "Unable to find ncbi configuration file");
  }
}

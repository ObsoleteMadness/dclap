/*  tomedlin.c
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
* File Name:  tomedlin.c
*
* Author:  Jonathan Kans
*   
* Version Creation Date: 10/15/91
*
* $Revision: 2.4 $
*
* File Description:  conversion to medlars format
*
* Modifications:  
* --------------------------------------------------------------------------
* Date	   Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
*
* ==========================================================================
*/

#include <objmedli.h>
#include <tofile.h>
#include <tomedlin.h>

#ifdef VAR_ARGS
#include <varargs.h>
#else
#include <stdarg.h>
#endif

#ifdef COMP_MPW
#pragma segment CdrSegD
#endif

#define BUFSIZE 8192

static CharPtr  buffer;
static CharPtr  pos;

/* ----- Function Prototypes ----- */

static void ClearString PROTO((void));
static void AddString PROTO((CharPtr string));
static void AddChar PROTO((Char ch));
static CharPtr LastCharPos PROTO((CharPtr str, Char ch));
static CharPtr CleanAuthorString PROTO((CharPtr auth));
static CharPtr CDECL StringAppend VPROTO((CharPtr first, ...));

/* ----- Function Bodies ----- */

static void ClearString (void)

{
  pos = buffer;
  *pos = '\0';
}

static void AddString (CharPtr string)

{
  pos = StringMove (pos, string);
  *pos = '\0';
}

static void AddChar (Char ch)

{
  Char  str [3];

  str [0] = ch;
  str [1] = '\0';
  pos = StringMove (pos, str);
  *pos = '\0';
}

static ColData table [2] = {{0, 6, 0, 'l', TRUE, TRUE, FALSE},
                            {0, 72, 0, 'l', TRUE, TRUE, TRUE}};

static Char *months[13] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

Boolean MedlineEntryToDataFile (MedlineEntryPtr mep, FILE *fp)

{
  CharPtr         abstract;
  AffilPtr        affil;
  AuthListPtr     authors;
  CitArtPtr       cit;
  CitJourPtr      citjour;
  DatePtr         date;
  ValNodePtr      gene;
  Int2            i;
  ImprintPtr      imp;
  CharPtr         issue;
  MedlineMeshPtr  mesh;
  ValNodePtr      names;
  CharPtr         p;
  CharPtr         pages;
  ParData         para;
  ValNodePtr      qual;
  Boolean         rsult;
  Char            str [32];
  MedlineRnPtr    substance;
  ValNodePtr      title;
  CharPtr         volume;
  ValNodePtr      xref;

  rsult = TRUE;
  buffer = MemNew (BUFSIZE);
  if (buffer != NULL) {
    para.openSpace = FALSE;
    ClearString ();
    AddString ("UI  -\t");
    sprintf (str, "%ld", mep->uid);
    AddString (str);
    AddString ("\n");
    rsult = (Boolean) (SendTextToFile (fp, buffer, &para, table) && rsult);
    ClearString ();
    cit = mep->cit;
    if (cit != NULL) {
      authors = cit->authors;
      if (authors != NULL && (authors->choice == 2 || authors->choice == 3)) {
        names = authors->names;
        while (names != NULL) {
          AddString ("AU  -\t");
          AddString (names->data.ptrvalue);
          AddString ("\n");
          names = names->next;
        }
      }
      title = cit->title;
      while (title != NULL) {
        if (title->choice == 1) {
          AddString ("TI  -\t");
          AddString (title->data.ptrvalue);
          AddString ("\n");
        }
        title = title->next;
      }
    }
    rsult = (Boolean) (SendTextToFile (fp, buffer, &para, table) && rsult);
    ClearString ();
    mesh = mep->mesh;
    while (mesh != NULL) {
      AddString ("MH  -\t");
      if (mesh->mp) {
        AddString ("*");
      }
      AddString (mesh->term);
      qual = mesh->qual;
      while (qual != NULL) {
        AddString ("/");
        if (qual->choice != 0) {
          AddString ("*");
        }
        p = (CharPtr) qual->data.ptrvalue;
        while (*p != '\0') {
          *p = TO_UPPER (*p);
          p++;
        }
        AddString (qual->data.ptrvalue);
        qual = qual->next;
      }
      AddString ("\n");
      mesh = mesh->next;
    }
    rsult = (Boolean) (SendTextToFile (fp, buffer, &para, table) && rsult);
    ClearString ();
    substance = mep->substance;
    while (substance != NULL) {
      AddString ("RN  -\t");
      switch (substance->type) {
        case 0:
          AddString ("0 (");
          AddString (substance->name);
          AddString (")");
          break;
        case 1:
          AddString (substance->cit);
          AddString (" (");
          AddString (substance->name);
          AddString (")");
          break;
        case 2:
          AddString ("EC ");
          AddString (substance->cit);
          AddString (" (");
          AddString (substance->name);
          AddString (")");
          break;
        default:
          break;
      }
      AddString ("\n");
      substance = substance->next;
    }
    rsult = (Boolean) (SendTextToFile (fp, buffer, &para, table) && rsult);
    ClearString ();
    gene = mep->gene;
    while (gene != NULL) {
      AddString ("GS  -\t");
      AddString (gene->data.ptrvalue);
      AddString ("\n");
      gene = gene->next;
    }
    xref = mep->xref;
    while (xref != NULL) {
      AddString ("SI  -\t");
      switch (xref->choice) {
        case 1:
          AddString ("DDBJ/");
          break;
        case 2:
          AddString ("CARBBANK/");
          break;
        case 3:
          AddString ("EMBL/");
          break;
        case 4:
          AddString ("HDB/");
          break;
        case 5:
          AddString ("GENBANK/");
          break;
        case 6:
          AddString ("HGML/");
          break;
        case 7:
          AddString ("MIM/");
          break;
        case 8:
          AddString ("MSD/");
          break;
        case 9:
          AddString ("PDB/");
          break;
        case 10:
          AddString ("PIR/");
          break;
        case 11:
          AddString ("PRFSEQDB/");
          break;
        case 12:
          AddString ("PSD/");
          break;
        case 13:
          AddString ("SWISSPROT/");
          break;
        default:
          AddString ("?/");
          break;
      }
      AddString (xref->data.ptrvalue);
      AddString ("\n");
      xref = xref->next;
    }
    rsult = (Boolean) (SendTextToFile (fp, buffer, &para, table) && rsult);
    ClearString ();
    citjour = cit->fromptr;
    if (citjour != NULL) {
      imp = citjour->imp;
      if (imp != NULL) {
        date = imp->date;
        if (date != NULL) {
          switch (date->data [0]) {
            case 0:
              AddString ("DP  -\t");
              StringNCpy (str, date->str, sizeof (str) - 1);
              AddString (str);
              AddString ("\n");
              break;
            case 1:
              AddString ("DP  -\t");
              sprintf (str, "%d", date->data [1] + 1900);
              AddString (str);
              AddString (" ");
              i = date->data [2];
              if (i >= 0 && i <= 11) {
                AddString (months [i]);
              }
              if (date->data [3] > 0) {
                AddString (" ");
                sprintf (str, "%d", date->data [3]);
                AddString (str);
              }
              AddString ("\n");
            default:
              break;
          }
        }
      }
      title = citjour->title;
      while (title != NULL) {
        if (title->choice == 6) {
          AddString ("TA  -\t");
          AddString (title->data.ptrvalue);
          AddString ("\n");
        }
        title = title->next;
      }
      if (imp != NULL) {
        pages = imp->pages;
        if (pages != NULL) {
          AddString ("PG  -\t");
          AddString (pages);
          AddString ("\n");
        }
        issue = imp->issue;
        if (issue != NULL) {
          AddString ("IP  -\t");
          AddString (issue);
          AddString ("\n");
        }
        volume = imp->volume;
        if (volume != NULL) {
          AddString ("VI  -\t");
          AddString (volume);
          AddString ("\n");
        }
      }
    }
    rsult = (Boolean) (SendTextToFile (fp, buffer, &para, table) && rsult);
    ClearString ();
    abstract = mep->abstract;
    if (abstract != NULL) {
      AddString ("AB  -\t");
      AddString (abstract);
      AddString ("\n");
    }
    rsult = (Boolean) (SendTextToFile (fp, buffer, &para, table) && rsult);
    ClearString ();
    if (cit != NULL && authors != NULL) {
      affil = authors->affil;
      if (affil != NULL && affil->choice == 1) {
        AddString ("AD  -\t");
        AddString (affil->affil);
        AddString ("\n");
      }
    }
    if (citjour != NULL && imp != NULL) {
      AddString ("SO  -\t");
      title = citjour->title;
      while (title != NULL) {
        if (title->choice == 6) {
          AddString (title->data.ptrvalue);
        }
        title = title->next;
      }
      AddString (" ");
      if (date != NULL) {
        switch (date->data [0]) {
          case 0:
            StringNCpy (str, date->str, sizeof (str) - 1);
            AddString (str);
            AddString (";");
            break;
          case 1:
            sprintf (str, "%d", date->data [1] + 1900);
            AddString (str);
            AddString (" ");
            i = date->data [2];
            if (i >= 0 && i <= 11) {
              AddString (months [i]);
            }
            if (date->data [3] > 0) {
              AddString (" ");
              sprintf (str, "%d", date->data [3]);
              AddString (str);
            }
            AddString (";");
          default:
            break;
        }
      }
      if (volume != NULL) {
        AddString (volume);
      }
      if (issue != NULL) {
        AddString ("(");
        AddString (issue);
        AddString (")");
      }
      if (pages != NULL) {
        AddString (":");
        AddString (pages);
      }
      AddString ("\n");
    }
    rsult = (Boolean) (SendTextToFile (fp, buffer, &para, table) && rsult);
    ClearString ();
    buffer = MemFree (buffer);
  }
  return rsult;
}

#ifdef VAR_ARGS
static CharPtr CDECL StringAppend (first, va_alist)
CharPtr first;
va_dcl
#else
static CharPtr CDECL StringAppend (CharPtr first, ...)
#endif

{
  va_list  args;
  Uint2    len;
  CharPtr  rsult;
  CharPtr  str;

#ifdef VAR_ARGS
  va_start (args);
#else
  va_start (args, first);
#endif
  len = (Uint2) StringLen (first);
  str = va_arg (args, CharPtr);
  while (str != NULL) {
    len += StringLen (str);
    str = va_arg (args, CharPtr);
  }
  va_end(args);

#ifdef VAR_ARGS
  va_start (args);
#else
  va_start (args, first);
#endif
  rsult = MemNew (len + 1);
  StringCpy (rsult, first);
  str = va_arg (args, CharPtr);
  while (str != NULL) {
    StringCat (rsult, str);
    str = va_arg (args, CharPtr);
  }
  va_end(args);
  return rsult;
}

static CharPtr LastCharPos (CharPtr str, Char ch)

{
  CharPtr  p;
  CharPtr  ps;
  CharPtr  q;

  p = str;
  ps = NULL;
  q = str + StringLen (str);
  while (p < q) {
    if (*p == ch) {
      ps = p;
    }
    p++;
  }
  return ps;
}

static CharPtr CleanAuthorString (CharPtr auth)

{
  CharPtr  first;
  Int2     i;
  CharPtr  last;
  Uint4    len;
  CharPtr  p;
  CharPtr  ps;
  CharPtr  str;

  if (StringLen (auth) > 0) {
    if (StringCmp (auth, "et al") == 0) {
      str = StringSave ("...");
    } else {
      first = NULL;
      last = StringSave (auth);
      ps = LastCharPos (last, ' ');
      p = NULL;
      if (ps != NULL) {
        p = ps + 1;
        while (*p != '\0') {
          if (! IS_UPPER(*p)) {
            *ps = '\0';
            p = ps;
          } else {
            p++;
          }
        }
      }
      ps = LastCharPos (last, ' ');
      if (ps != NULL) {
        p = ps + 1;
        *ps = '\0';
      }
      if (p != NULL) {
        len = StringLen (p);
        first = MemNew (3 * (size_t) len + 1);
        i = 0;
        while (*p != '\0') {
          first [i] = *p;
          i++;
          first [i] = '.';
          i++;
          first [i] = ' ';
          i++;
          p++;
        }
      }
      if (first != NULL) {
        str = StringAppend (first, (CharPtr) last, (CharPtr) NULL);
        last = MemFree (last);
        first = MemFree (first);
      } else {
        str = last;
      }
    }
  } else {
    str = StringSave ("");
  }
  return str;
}

MedlinePtr ParseMedline (MedlineEntryPtr mep)

{
  AffilPtr         affil;
  AuthListPtr      authors;
  CharPtr          chptr;
  CitArtPtr        cit;
  CitJourPtr       citjour;
  DatePtr          date;
  ValNodePtr       gene;
  ImprintPtr       imp;
  CharPtr          last;
  MedlineMeshPtr   mesh;
  MedlinePtr       mPtr;
  ValNodePtr       names;
  ValNodePtr       qual;
  Char             str [32];
  MedlineRnPtr     substance;
  CharPtr          this;
  ValNodePtr       title;

  buffer = MemNew (BUFSIZE);
  if (buffer != NULL) {
    ClearString ();
    mPtr = MemNew (sizeof (MedlineData));
    if (mPtr != NULL && mep != NULL) {
      sprintf (str, "%1ld", mep->uid);
      mPtr->uid = StringSave (str);
      cit = mep->cit;
      if (cit != NULL) {
        if (cit->from == 1) {
          citjour = (CitJourPtr) cit->fromptr;
          if (citjour != NULL) {
            title = citjour->title;
            while (title != NULL) {
              if (title->choice == 6) {
                AddString (title->data.ptrvalue);
                AddString (" ");
                mPtr->journal = StringSave (buffer);
                ClearString ();
              }
              title = title->next;
            }
            imp = citjour->imp;
            if (imp != NULL) {
              date = imp->date;
              if (date != NULL) {
                switch (date->data [0]) {
                  case 0:
                    StringNCpy (str, date->str, sizeof (str) - 1);
                    chptr = StringChr (str, ' ');
                    if (chptr != NULL) {
                      *chptr = '\0';
                    }
                    mPtr->year = StringSave (str);
                    break;
                  case 1:
                    sprintf (str, "%0d", (Int2) (date->data [1] + 1900));
                    mPtr->year = StringSave (str);
                  default:
                    break;
                }
              }
              mPtr->volume = StringSave (imp->volume);
              mPtr->pages = StringSave (imp->pages);
            }
          }
        }
        title = cit->title;
        while (title != NULL) {
          switch (title->choice) {
            case 1:
              mPtr->title = StringSave ((CharPtr) title->data.ptrvalue);
              break;
            case 2:
              break;
            case 3:
              mPtr->transl = StringSave ((CharPtr) title->data.ptrvalue);
              break;
            default:
              break;
          }
          title = title->next;
        }
        authors = cit->authors;
        if (authors != NULL) {
          if (authors->choice == 2 || authors->choice == 3) {
            names = authors->names;
            while (names != NULL) {
              last = mPtr->authors;
              if (last != NULL) {
                this = CleanAuthorString ((CharPtr) names->data.ptrvalue);
                if (names->next != NULL) {
                  mPtr->authors = StringAppend (last, (CharPtr) ", ",
                                                (CharPtr) this, (CharPtr) NULL);
                } else {
                  mPtr->authors = StringAppend (last, (CharPtr) " & ",
                                                (CharPtr) this, (CharPtr) NULL);
                }
                last = MemFree (last);
                this = MemFree (this);
              } else {
                mPtr->authors = CleanAuthorString ((CharPtr) names->data.ptrvalue);
              }
              names = names->next;
            }
          }
          affil = authors->affil;
          if (affil != NULL) {
            if (affil->choice == 1) {
              mPtr->affil = StringSave (affil->affil);
            }
          }
        }
      }
      mPtr->abstract = StringSave (mep->abstract);
      mesh = mep->mesh;
      while (mesh != NULL) {
        if (mesh->mp) {
          StringNCpy (str, "*", sizeof (str));
        } else {
          StringNCpy (str, "", sizeof (str));
        }
        last = mPtr->mesh;
        if (last != NULL) {
          mPtr->mesh = StringAppend (last, (CharPtr) "\n  ", (CharPtr) str,
                                     (CharPtr) mesh->term, (CharPtr) NULL);
          last = MemFree (last);
        } else {
          mPtr->mesh = StringAppend ((CharPtr) "  ", (CharPtr) str,
                                     (CharPtr) mesh->term, (CharPtr) NULL);
        }
        qual = mesh->qual;
        while (qual != NULL) {
          if (qual->choice != 0) {
            StringNCpy (str, "*", sizeof (str));
          } else {
            StringNCpy (str, "", sizeof (str));
          }
          last = mPtr->mesh;
          mPtr->mesh = StringAppend (last, (CharPtr) "/", (CharPtr) str,
                                    (CharPtr) qual->data.ptrvalue, (CharPtr) NULL);
          last = MemFree (last);
          qual = qual->next;
        }
        mesh = mesh->next;
      }
      if (mPtr->mesh != NULL) {
        last = mPtr->mesh;
        mPtr->mesh = StringAppend (last, (CharPtr) "\n", (CharPtr) NULL);
        last = MemFree (last);
      }
      gene = mep->gene;
      if (gene != NULL) {
        ClearString ();
        while (gene != NULL) {
          AddString ("  ");
          AddString (gene->data.ptrvalue);
          AddString ("\n");
          gene = gene->next;
        }
        mPtr->gene = StringSave (buffer);
      }
      substance = mep->substance;
      if (substance != NULL) {
        ClearString ();
        while (substance != NULL) {
          AddString ("  ");
          switch (substance->type) {
            case 0:
              AddString (substance->name);
              break;
            case 1:
              AddString (substance->name);
              AddString (" (");
              AddString ("CAS ");
              AddString (substance->cit);
              AddString (")");
              break;
            case 2:
              AddString (substance->name);
              AddString (" (");
              AddString ("EC ");
              AddString (substance->cit);
              AddString (")");
              break;
            default:
              break;
          }
          AddString ("\n");
          substance = substance->next;
        }
        mPtr->substance = StringSave (buffer);
      }
    }
    ClearString ();
    buffer = MemFree (buffer);
  }
  return mPtr;
}

MedlinePtr FreeMedline (MedlinePtr mPtr)

{
  if (mPtr != NULL) {
    mPtr->journal = MemFree (mPtr->journal);
    mPtr->volume = MemFree (mPtr->volume);
    mPtr->pages = MemFree (mPtr->pages);
    mPtr->year = MemFree (mPtr->year);
    mPtr->title = MemFree (mPtr->title);
    mPtr->transl = MemFree (mPtr->transl);
    mPtr->authors = MemFree (mPtr->authors);
    mPtr->affil = MemFree (mPtr->affil);
    mPtr->abstract = MemFree (mPtr->abstract);
    mPtr->mesh = MemFree (mPtr->mesh);
    mPtr->gene = MemFree (mPtr->gene);
    mPtr->substance = MemFree (mPtr->substance);
    mPtr->uid = MemFree (mPtr->uid);
    mPtr = MemFree (mPtr);
  }
  return NULL;
}

static ColData  colFmt [3] = {{0, 0, 0, 'l', TRUE, TRUE, FALSE},
                              {0, 0, 0, 'l', TRUE, TRUE, FALSE},
                              {0, 0, 0, 'l', TRUE, TRUE, TRUE}};

static ColData  mshFmt [1] = {{0, 80, 0, 'l', FALSE, FALSE, TRUE}};

Boolean MedlineEntryToDocFile (MedlineEntryPtr mep, FILE *fp)

{
  MedlinePtr  mPtr;
  ParData     para;
  Boolean     rsult;

  rsult = TRUE;
  if (fp != NULL && mep != NULL) {
    mPtr = ParseMedline (mep);
    if (mPtr != NULL) {
      buffer = MemNew (BUFSIZE);
      if (buffer != NULL) {
        para.openSpace = FALSE;
        ClearString ();
        AddString (mPtr->journal);
        AddString ("\t");
        AddString (mPtr->volume);
        AddString (":\t");
        AddString (mPtr->pages);
        AddString ("  (");
        AddString (mPtr->year);
        AddString (")");
        AddString (" [");
        AddString (mPtr->uid);
        AddString ("]");
        AddString ("\n");
        rsult = (Boolean) (SendTextToFile (fp, buffer, &para, colFmt) && rsult);
        ClearString ();
        AddString (mPtr->title);
        AddString ("\n");
        rsult = (Boolean) (SendTextToFile (fp, buffer, NULL, NULL) && rsult);
        ClearString ();
        if (mPtr->transl != NULL) {
          AddString ("[");
          AddString (mPtr->transl);
          AddString ("]\n");
          rsult = (Boolean) (SendTextToFile (fp, buffer, NULL, NULL) && rsult);
          ClearString ();
        }
        AddString (mPtr->authors);
        AddString ("\n");
        rsult = (Boolean) (SendTextToFile (fp, buffer, NULL, NULL) && rsult);
        ClearString ();
        if (mPtr->affil != NULL) {
          AddString (mPtr->affil);
          AddString ("\n");
          rsult = (Boolean) (SendTextToFile (fp, buffer, NULL, NULL) && rsult);
          ClearString ();
        }
        if (mPtr->abstract != NULL) {
          AddString (mPtr->abstract);
          AddString ("\n");
          rsult = (Boolean) (SendTextToFile (fp, buffer, NULL, NULL) && rsult);
          ClearString ();
        }
        if (mPtr->mesh != NULL) {
          rsult = (Boolean) (SendTextToFile (fp, "MeSH Terms:\n", NULL, NULL) && rsult);
          rsult = (Boolean) (SendTextToFile (fp, mPtr->mesh, &para, mshFmt) && rsult);
        }
        if (mPtr->gene != NULL) {
          rsult = (Boolean) (SendTextToFile (fp, "Gene Symbols:\n", NULL, NULL) && rsult);
          rsult = (Boolean) (SendTextToFile (fp, mPtr->gene, &para, mshFmt) && rsult);
        }
        if (mPtr->substance != NULL) {
          rsult = (Boolean) (SendTextToFile (fp, "Substances:\n", NULL, NULL) && rsult);
          rsult = (Boolean) (SendTextToFile (fp, mPtr->substance, &para, mshFmt) && rsult);
        }
        buffer = MemFree (buffer);
      }
      mPtr = FreeMedline (mPtr);
    }
  }
  return rsult;
}

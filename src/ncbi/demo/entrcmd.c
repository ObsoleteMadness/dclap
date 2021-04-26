/*   entrcmd.c
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
* File Name:  entrcmd.c
*
* Author:  Epstein
*
* Version Creation Date:   1/4/94
*
* $Revision: 1.3 $
*
* File Description: 
*       non-interactive command line interface for Entrez
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#include <ncbi.h>
#include <accentr.h>
#include <accutils.h>
#include <tofasta.h>
#include <toreport.h>
#include <tomedlin.h>
#include <togenbnk.h>

Args myargs[] = {
        {"Initial database", "m",NULL, NULL, TRUE,'d',ARG_STRING,0.0,0,NULL},
        {"Boolean expression", NULL, NULL, NULL, TRUE, 'e', ARG_STRING, 0.0,0,NULL},
        {"Comma-delimited list of UIDs", NULL, NULL, NULL, TRUE, 'u', ARG_STRING, 0.0,0,NULL},
        {"Program of commands", NULL, NULL, NULL, FALSE, 'p', ARG_STRING, 0.0,0,NULL},
        {"Display status report", "F", NULL, NULL, TRUE, 's', ARG_BOOLEAN, 0.0,0,NULL},
        {"Produce WWW/HTML formatted output (recommended value is /htbin)", NULL, NULL, NULL, TRUE, 'w', ARG_STRING, 0.0,0,NULL},
        {"Detailed help", "F", NULL, NULL, TRUE, 'h', ARG_BOOLEAN, 0.0,0,NULL},
        {"For WWW output, use Forms", "F", NULL, NULL, TRUE, 'f', ARG_BOOLEAN, 0.0,0,NULL},
        {"'Check' WWW output Forms", "F", NULL, NULL, TRUE, 'c', ARG_BOOLEAN, 0.0,0,NULL},
        {"Name of export file for named UID list", NULL, NULL, NULL, TRUE, 'x', ARG_STRING,0.0,0,NULL},
        {"Comma-delimited list of files to import for named UID list", NULL, NULL, NULL, TRUE, 'i', ARG_STRING,0.0,0,NULL},
        {"Produce a list of terms (term)", NULL, NULL, NULL, TRUE, 't', ARG_STRING, 0.0,0,NULL}};

#define ENTREZ_FLD_MNEMONIC_LENGTH 4
#define DEFAULT_TERMLIST_LEN      40

typedef struct savlist {
    CharPtr name;
    Int4Ptr uids;
    DocType db;
    Int2 num;
} SavList, PNTR SavListPtr;

static CharPtr wwwPrefix = NULL;
static CharPtr theTerm = NULL;
static Boolean useForms = FALSE;
static Boolean checkForms = FALSE;
static Int2    numTerms;
static Int2    termsBefore;
static FILE *  exportFilePtr = NULL;


static Boolean IsGenBank (SeqEntryPtr sep)
{
    BioseqPtr  bsp;
    Uint1      repr;
    Boolean    rsult;

    rsult = FALSE;
    if (sep->choice == 1) {
        bsp = (BioseqPtr) sep->data.ptrvalue;
        repr = Bioseq_repr (bsp);
        if (repr == Seq_repr_raw || repr == Seq_repr_const) {
            if (ISA_na (bsp->mol)) {
                rsult = TRUE;
            } else {
                Message (MSG_ERROR, "Protein record cannot be viewed in GenBank form.");
            }
        } else {
            Message (MSG_ERROR, "Bad sequence repr %d", (int) repr);
        }
    } else {
        rsult = TRUE;
    }
    return rsult;
}


static void
PrintGenbank(SeqEntryPtr sep)
{
    AsnOptionPtr optionHead = NULL;
    DataVal val;

    optionHead = NULL;
    val.intvalue = TRUE;
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_QUIET, val, NULL);
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_NO_NCBI, val, NULL);
    val.ptrvalue = StringSave ("\?\?\?");
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_DIV,
                  val, DefAsnOptionFree);
    val.ptrvalue = StringSave ("\?\?-\?\?\?-\?\?\?\?");
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_DATE,
                  val, DefAsnOptionFree);
    val.intvalue = TRUE;
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_RELEASE, val, NULL);
    val.intvalue = 1;
    AsnOptionNew (&optionHead, OP_TOGENBNK, OP_TOGENBNK_NUCL_PROT, val, NULL);
    if (IsGenBank (sep)) {
        SeqEntryToGenbank (stdout, sep, optionHead);
    }
    AsnOptionFree (&optionHead, OP_TOGENBNK, 0);
    FilePuts ("\n\n\n\n", stdout);
}

static Boolean PrintDSP(DocSumPtr dsp, DocUid uid)
{

    Int2 titleLen;
    Int2 size;
    CharPtr pEnd;
    CharPtr pStart;
    
    if (dsp == NULL)
        return TRUE;

    pStart = dsp->title;
    titleLen = StrLen(pStart);
    
    if ( titleLen <= 55 ) {
      size = titleLen;
    } else {
        pEnd = pStart + 55;
        while (IS_WHITESP(*pEnd) == FALSE)
            pEnd -=1;
        size = pEnd - pStart;
    }

    printf("%-20.20s %-*.*s\n",dsp->caption,size,size,pStart);
    printf("%c",(dsp->no_abstract ? ' ' : '*'));
    printf("                    ");

    pStart = pStart + size;
  
    while ((titleLen = StrLen(pStart)) > 0 ) {
        if ( titleLen <= 55 ) {
            size = titleLen;
        } else {
            pEnd = pStart + 55;
            while (IS_WHITESP(*pEnd) == FALSE)
                pEnd -=1;
            size = pEnd - pStart;
        }

        printf("%-*.*s\n",size,size,pStart+1);
        printf("%-21.21s","");
        pStart = pStart + size;
    }

    printf("\n");

    DocSumFree(dsp);
    return TRUE;
}

static Boolean PrintDSPMwww(DocSumPtr dsp, DocUid uid)
{
    CharPtr p;
    Boolean noNeighbors = FALSE;
    LinkSetPtr lsp;
    Int2 medNeighbors;
    Int2 protNeighbors;
    Int2 nucNeighbors;
    
    if (dsp == NULL)
        return TRUE;

    printf ("<DL>\n<DT>\n");
    if (useForms)
    {
        printf ("<inPUT TYPE=\"checkbox\" NAME=\"nei\" VALUE=\"%d\"%s>\n", uid,
                checkForms ? " CHECKED" : "");
    } else {
        lsp = NULL;
        EntrezLinkUidList(&lsp, TYP_ML, TYP_ML, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            medNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            medNeighbors = 0;
        }
        EntrezLinkUidList(&lsp, TYP_ML, TYP_AA, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            protNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            protNeighbors = 0;
        }
        EntrezLinkUidList(&lsp, TYP_ML, TYP_NT, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            nucNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            nucNeighbors = 0;
        }
        noNeighbors = !medNeighbors && !protNeighbors && !nucNeighbors;
    }

    printf ("%s\n<P>\n<DD>\n", dsp->caption);
    for (p = dsp->title; *p; p++)
    {
        switch (*p)
        {
        case '&': printf ("&amp"); break;
        case '<': printf ("&lt"); break;
        case '>': printf ("&gt"); break;
        default: printf ("%c", *p);
        }
    }
    printf ("<I>");
    if (dsp->no_abstract)
    {
        printf (" (no abstract available)");
    }
    printf (" (View ");
    printf ("<A HREF=\"%s/entrezmr?%d\">Report format</A>,\n", wwwPrefix, uid);
    printf ("<A HREF=\"%s/entrezml?%d\">MEDLARS format</A>,\n", wwwPrefix, uid);
    if (useForms || noNeighbors)
    {
        printf ("or ");
    }
    printf ("<A HREF=\"%s/entrezma?%d\">ASN.1 format</A>", wwwPrefix, uid);
    if (! useForms)
    {
        if (medNeighbors != 0)
            printf (", %s<A HREF=\"%s/entrezmmnei?%d\">%d MEDLINE neighbor%s</A>\n", !protNeighbors && !nucNeighbors ? "or " : "", wwwPrefix, uid, medNeighbors, medNeighbors == 1 ? "" : "s");
        if (protNeighbors != 0)
            printf (", %s<A HREF=\"%s/entrezmpnei?%d\">%d Protein link%s</A>\n", !nucNeighbors ? "or " : "", wwwPrefix, uid, protNeighbors, protNeighbors == 1 ? "" : "s");
        if (nucNeighbors != 0)
            printf (", or <A HREF=\"%s/entrezmnnei?%d\">%d Nucleotide link%s</A>\n", wwwPrefix, uid, nucNeighbors, nucNeighbors == 1 ? "" : "s");
    }
    printf (")\n<P></I></DL>\n");
    
    DocSumFree(dsp);
    return TRUE;
}

static Boolean PrintDSPNwww(DocSumPtr dsp, DocUid uid)
{
    CharPtr p;
    Boolean noNeighbors = FALSE;
    LinkSetPtr lsp;
    Int2 medNeighbors;
    Int2 protNeighbors;
    Int2 nucNeighbors;
    
    if (dsp == NULL)
        return TRUE;

    printf ("<DL>\n<DT>\n");
    if (useForms)
    {
        printf ("<inPUT TYPE=\"checkbox\" NAME=\"nei\" VALUE=\"%d\"%s>\n", uid,
                checkForms ? " CHECKED" : "");
    } else {
        lsp = NULL;
        EntrezLinkUidList(&lsp, TYP_NT, TYP_ML, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            medNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            medNeighbors = 0;
        }
        EntrezLinkUidList(&lsp, TYP_NT, TYP_AA, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            protNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            protNeighbors = 0;
        }
        EntrezLinkUidList(&lsp, TYP_NT, TYP_NT, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            nucNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            nucNeighbors = 0;
        }
        noNeighbors = !medNeighbors && !protNeighbors && !nucNeighbors;
    }

    printf ("%s\n<P>\n<DD>\n", dsp->caption);
    for (p = dsp->title; *p; p++)
    {
        switch (*p)
        {
        case '&': printf ("&amp"); break;
        case '<': printf ("&lt"); break;
        case '>': printf ("&gt"); break;
        default: printf ("%c", *p);
        }
    }
    printf ("<I> (View ");
    printf ("<A HREF=\"%s/entreznr?%d\">Report format</A>,\n", wwwPrefix, uid);
    printf ("<A HREF=\"%s/entrezng?%d\">GenBank format</A>,\n", wwwPrefix, uid);
    printf ("<A HREF=\"%s/entreznf?%d\">FASTA format</A>,\n", wwwPrefix, uid);
    if (useForms || noNeighbors)
    {
        printf ("or ");
    }
    printf ("<A HREF=\"%s/entrezna?%d\">ASN.1 format</A>", wwwPrefix, uid);
    if (! useForms)
    {
        if (medNeighbors != 0)
            printf (", %s<A HREF=\"%s/entreznmnei?%d\">%d MEDLINE link%s</A>\n", !protNeighbors && !nucNeighbors ? "or " : "", wwwPrefix, uid, medNeighbors, medNeighbors == 1 ? "" : "s");
        if (protNeighbors != 0)
            printf (", %s<A HREF=\"%s/entreznpnei?%d\">%d Protein link%s</A>\n", !nucNeighbors ? "or " : "", wwwPrefix, uid, protNeighbors, protNeighbors == 1 ? "" : "s");
        if (nucNeighbors != 0)
            printf (", or <A HREF=\"%s/entreznnnei?%d\">%d Nucleotide neighbor%s</A>\n", wwwPrefix, uid, nucNeighbors, nucNeighbors == 1 ? "" : "s");
    }
    printf (")<P></I></DL>\n");
    
    DocSumFree(dsp);
    return TRUE;
}

static Boolean PrintDSPPwww(DocSumPtr dsp, DocUid uid)
{
    CharPtr p;
    Boolean noNeighbors = FALSE;
    LinkSetPtr lsp;
    Int2 medNeighbors;
    Int2 protNeighbors;
    Int2 nucNeighbors;
    
    if (dsp == NULL)
        return TRUE;

    printf ("<DL>\n<DT>\n");
    if (useForms)
    {
        printf ("<inPUT TYPE=\"checkbox\" NAME=\"nei\" VALUE=\"%d\"%s>\n", uid,
                checkForms ? " CHECKED" : "");
    } else {
        lsp = NULL;
        EntrezLinkUidList(&lsp, TYP_AA, TYP_ML, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            medNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            medNeighbors = 0;
        }
        EntrezLinkUidList(&lsp, TYP_AA, TYP_AA, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            protNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            protNeighbors = 0;
        }
        EntrezLinkUidList(&lsp, TYP_AA, TYP_NT, 1, &uid, FALSE);
        if (lsp != NULL)
        {
            nucNeighbors = lsp->num;
            LinkSetFree(lsp);
            lsp = NULL;
        } else {
            nucNeighbors = 0;
        }
        noNeighbors = !medNeighbors && !protNeighbors && !nucNeighbors;
    }

    printf ("%s\n<P>\n<DD>\n", dsp->caption);
    for (p = dsp->title; *p; p++)
    {
        switch (*p)
        {
        case '&': printf ("&amp"); break;
        case '<': printf ("&lt"); break;
        case '>': printf ("&gt"); break;
        default: printf ("%c", *p);
        }
    }
    printf ("<I> (View ");
    printf ("<A HREF=\"%s/entrezpr?%d\">Report format</A>,\n", wwwPrefix, uid);
    printf ("<A HREF=\"%s/entrezpf?%d\">FASTA format</A>,\n", wwwPrefix, uid);
    if (useForms || noNeighbors)
    {
        printf ("or ");
    }
    printf ("<A HREF=\"%s/entrezpa?%d\">ASN.1 format</A>", wwwPrefix, uid);
    if (! useForms)
    {
        if (medNeighbors != 0)
            printf (", %s<A HREF=\"%s/entrezpmnei?%d\">%d MEDLINE link%s</A>\n", !protNeighbors && !nucNeighbors ? "or " : "", wwwPrefix, uid, medNeighbors, medNeighbors == 1 ? "" : "s");
        if (protNeighbors != 0)
            printf (", %s<A HREF=\"%s/entrezppnei?%d\">%d Protein neighbor%s</A>\n", !nucNeighbors ? "or " : "", wwwPrefix, uid, protNeighbors, protNeighbors == 1 ? "" : "s");
        if (nucNeighbors != 0)
            printf (", or <A HREF=\"%s/entrezpnnei?%d\">%d Nucleotide link%s</A>\n", wwwPrefix, uid, nucNeighbors, nucNeighbors == 1 ? "" : "s");
    }
    printf (")<P></I></DL>\n");
    
    DocSumFree(dsp);
    return TRUE;
}

static void
ReportBadType (DocType db, CharPtr outputSpec)
{
    Message(MSG_POST, "Invalid output format \"%s\" for database \"%s\"",
            outputSpec, db == TYP_ML ? "MEDLINE" : (db == TYP_NT ?
            "Nucleotide" : (db == TYP_AA ? "Protein" : "unknown")));
}

static Boolean
ProcessOutput(LinkSetPtr lsp, DocType db, CharPtr outputSpec, long processingCount, long totalCount, Boolean parseOnly)
{
    long i;
    AsnIoPtr aip;
    MedlineEntryPtr mep;
    SeqEntryPtr sep;

    if (StringCmp(outputSpec, "") == 0 || StringCmp(outputSpec, "no") == 0)
        return TRUE;
    if (StringCmp(outputSpec, "mu") == 0)
    {
        if (db != TYP_ML)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            if (exportFilePtr != NULL)
            {
                fprintf(exportFilePtr, "%d\n", db);
                for (i = 0; i < processingCount; i++)
                {
                    fprintf(exportFilePtr, "%ld\n", lsp->uids[i]);
                }
                FileClose(exportFilePtr);
                exportFilePtr = NULL;
            } else {
                printf ("\n");
                for (i = 0; i < processingCount; i++)
                {
                    printf("%ld\n", lsp->uids[i]);
                }
                printf ("\n");
                fflush(stdout);
            }
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "md") == 0)
    {
        if (db != TYP_ML)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            if (wwwPrefix != NULL && processingCount < totalCount)
            {
                printf ("Warning: only %ld document summaries are being displayed\n", processingCount);
                printf ("out of %d total entries.<P>\n", totalCount);
            }
            EntrezDocSumListGet((Int2) processingCount, db, lsp->uids,
                                wwwPrefix == NULL ? PrintDSP : PrintDSPMwww);
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "mr") == 0)
    {
        if (db != TYP_ML)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            for (i = 0; i < processingCount; i++)
            {
                mep = EntrezMedlineEntryGet(lsp->uids[i]);
                if (mep != NULL)
                {
                    MedlineEntryToDocFile(mep, stdout);
                    MedlineEntryFree(mep);
                    printf ("\n\n");
                }
            }
            fflush(stdout);
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "ma") == 0)
    {
        if (db != TYP_ML)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            aip = AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL, NULL, NULL);
            for (i = 0; i < processingCount; i++)
            {
                mep = EntrezMedlineEntryGet(lsp->uids[i]);
                if (mep != NULL)
                {
                    MedlineEntryAsnWrite(mep, aip, NULL);
                    AsnIoReset(aip);
                    MedlineEntryFree(mep);
                }
            }
            AsnIoClose(aip);
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "ml") == 0)
    {
        if (db != TYP_ML)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            for (i = 0; i < processingCount; i++)
            {
                mep = EntrezMedlineEntryGet(lsp->uids[i]);
                if (mep != NULL)
                {
                    MedlineEntryToDataFile(mep, stdout);
                    printf("\n");
                    MedlineEntryFree(mep);
                }
            }
        }
        return TRUE;
    }


    if (StringCmp(outputSpec, "su") == 0)
    {
        if (db != TYP_NT && db != TYP_AA)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            if (exportFilePtr != NULL)
            {
                fprintf(exportFilePtr, "%d\n", db);
                for (i = 0; i < processingCount; i++)
                {
                    fprintf(exportFilePtr, "%ld\n", lsp->uids[i]);
                }
                FileClose(exportFilePtr);
                exportFilePtr = NULL;
            } else {
                printf ("\n");
                for (i = 0; i < processingCount; i++)
                {
                    printf("%ld\n", lsp->uids[i]);
                }
                printf ("\n");
                fflush(stdout);
            }
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "sd") == 0)
    {
        if (db != TYP_NT && db != TYP_AA)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            if (wwwPrefix != NULL && processingCount < totalCount)
            {
                printf ("Warning: only %ld document summaries are being displayed\n", processingCount);
                printf ("out of %d total entries.<P>\n", totalCount);
            }
            EntrezDocSumListGet((Int2) processingCount, db, lsp->uids,
                                wwwPrefix == NULL ? PrintDSP : (db == TYP_NT ?
                                PrintDSPNwww : PrintDSPPwww));
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "sa") == 0)
    {
        if (db != TYP_NT && db != TYP_AA)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            aip = AsnIoNew(ASNIO_TEXT_OUT, stdout, NULL, NULL, NULL);
            for (i = 0; i < processingCount; i++)
            {
                sep = EntrezSeqEntryGet(lsp->uids[i], 3);
                if (sep != NULL)
                {
                    SeqEntryAsnWrite(sep, aip, NULL);
                    AsnIoReset(aip);
                    SeqEntryFree(sep);
                }
            }
            AsnIoClose(aip);
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "sr") == 0)
    {
        if (db != TYP_NT && db != TYP_AA)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            for (i = 0; i < processingCount; i++)
            {
                sep = EntrezSeqEntryGet(lsp->uids[i], 3);
                if (sep != NULL)
                {
                    SeqEntryToFile (sep, stdout, TRUE, 50, FALSE);
                    SeqEntryFree(sep);
                }
            }
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "sg") == 0)
    {
        if (db != TYP_NT)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            for (i = 0; i < processingCount; i++)
            {
                sep = EntrezSeqEntryGet(lsp->uids[i], 3);
                if (sep != NULL)
                {
                    PrintGenbank(sep);
                    SeqEntryFree(sep);
                }
            }
        }
        return TRUE;
    }
    if (StringCmp(outputSpec, "sf") == 0)
    {
        if (db != TYP_NT && db != TYP_AA)
        {
            ReportBadType(db, outputSpec);
            return FALSE;
        }
        if (! parseOnly && lsp != NULL)
        {
            for (i = 0; i < processingCount; i++)
            {
                sep = EntrezSeqEntryGet(lsp->uids[i], 3);
                if (sep != NULL)
                {
                    SeqEntryConvert (sep, Seq_code_iupacna);
                    SeqEntryToFasta (sep, stdout, db == TYP_NT);
                    SeqEntryFree(sep);
                }
                printf ("\n");
            }

        }
        return TRUE;
    }

    Message(MSG_POST, "Unknown output format \"%s\"", outputSpec);
    return FALSE;
}

static Int2
RunProgram(CharPtr programStr, LinkSetPtr lsp, DocType db, Boolean parseOnly)
{
    Int1 wrongDelim = '.';
    DocType newdb;
    Int2 len;
    int i;
    CharPtr c;
    Char outputSpec[3];
    long processingCount;
    Char numStr[12];
    Int2 count;
    Int2 numToCopy;
    LinkSetPtr newlsp;

    if (programStr == NULL)
    {
        return -1;
    }

    if (lsp == NULL && !parseOnly)
    {
        return -1;
    }

    len = StrLen(programStr);
    outputSpec[2] = '\0';
    c = programStr;

    for (c = programStr; c < programStr + len; c += count + 1)
    {
        count = StrCSpn(c, ",.");
        if (c[count] == wrongDelim)
        {
            Message(MSG_POST, "Invalid delimiter");
            /* offset to offending delimeter */
            return (count + 1 + c - programStr);
        }

        processingCount = INT2_MAX;

        if (wrongDelim == '.')
        { /* process output */
            switch (count) {
            case 0:
                outputSpec[0] = '\0';
                break;
            case 1:
                Message(MSG_POST, "Invalid output specification \"%c\"", c[1]);
                return ( 2 + c - programStr);
            case 2:
                outputSpec[0] = c[0];
                outputSpec[1] = c[1];
                break;
            default:
                outputSpec[0] = c[0];
                outputSpec[1] = c[1];
                numToCopy = MIN(count - 2, sizeof(numStr) - 1);
                StrNCpy(numStr, c + 2, numToCopy);
                numStr[numToCopy] = '\0';
                if (StrSpn(numStr, "0123456789") != numToCopy)
                {
                  Message(MSG_POST, "Non-numeric character detected");
                  return ( count + c - programStr);
                }
                sscanf(numStr, "%ld", &processingCount);
                break;
            }
            if (lsp != NULL && !parseOnly)
            {
                processingCount = MIN(processingCount, lsp->num);
            }
            if (! ProcessOutput(lsp, db, outputSpec, processingCount,
                                lsp != NULL ? lsp->num : processingCount,
                                parseOnly))
            {
                /* note that error will be posted by ProcessOutput() */
                return ( 3 + c - programStr);
            }
        } else { /* process neighboring */
            if (count == 0)
            {
                Message(MSG_POST, "Null neighboring specification");
                return ( 1 + c - programStr);
            }
            if (count > 1)
            {
                numToCopy = MIN(count - 1, sizeof(numStr) - 1);
                StrNCpy(numStr, c + 1, numToCopy);
                numStr[numToCopy] = '\0';
                if (StrSpn(numStr, "0123456789") != numToCopy)
                {
                  Message(MSG_POST, "Non-numeric character detected");
                  return ( count + c - programStr);
                }
                sscanf(numStr, "%ld", &processingCount);
            }
            switch (*c) {
            case 'p':
                newdb = TYP_AA;
                break;
            case 'm':
                newdb = TYP_ML;
                break;
            case 'n':
                newdb = TYP_NT;
                break;
            default:
                Message(MSG_POST, "Invalid neighboring specification <%s>", *c);
                return ( 1 + c - programStr);
            }

            if (lsp != NULL && !parseOnly)
            {
                processingCount = MIN(processingCount, lsp->num);
                newlsp = NULL;
                EntrezLinkUidList(&newlsp, db, newdb, (Int2) processingCount, lsp->uids, FALSE);
                LinkSetFree(lsp);
                lsp = newlsp;
            }
            db = newdb;
        }

        wrongDelim = wrongDelim == '.' ? ',' : '.';
    }

    if (lsp != NULL && !parseOnly)
        LinkSetFree(lsp);

    return 0;
}

static Boolean
beginTermProc(CharPtr term, Int4 special, Int4 total)
{
    if (term != NULL)
    {
        printf ("%s\n", term);
        MemFree (term);
        return TRUE;
    } else {
        return FALSE;
    }
}

static Boolean
findOneTermProc(CharPtr term, Int4 special, Int4 total)
{
    if (term != NULL)
    {
        MemFree (term);
        return TRUE;
    } else {
        return FALSE;
    }
}

static Boolean
collectNumTermsProc(CharPtr term, Int4 special, Int4 total)
{
    static Boolean inited = FALSE;
    static CharPtr PNTR arrayOfTerm = NULL;
    static Int2 head;
    static Boolean sawOurTerm;
    Int4 i;

    if (! special == -1)
    { /* flag indicating reset */
        for (i = 0; i < termsBefore; i++)
        {
            MemFree(arrayOfTerm[i]);
        }
        MemFree(arrayOfTerm);
        arrayOfTerm = NULL;
        inited = FALSE;
        return;
    }

    if (term == NULL)
    {
        return;
    }

    if (! inited)
    {
        inited = TRUE;
        arrayOfTerm = MemNew(termsBefore * sizeof(CharPtr));
        for (i = 0; i < termsBefore; i++)
        {
            arrayOfTerm[i] = NULL;
        }
        head = 0;
        sawOurTerm = FALSE;
    }

    if (sawOurTerm)
    {
        printf ("%s\n", term);
        MemFree (term);
        if (--head <= 0)
            return FALSE; /* no more terms, please */
        else
            return TRUE;
    } else {
        if (StringICmp(term, theTerm) >= 0)
        {
            sawOurTerm = TRUE;
            for (i = 0; i < termsBefore; i++)
            {
                if (arrayOfTerm[i] == NULL)
                {
                    head = 0; /* didn't wrap around */
                    break;
                }
            }

            /* print out the queue */
            i = head;
            do {
                if (arrayOfTerm[i] == NULL)
                    break;
                printf ("%s\n", arrayOfTerm[i]);
                i = (i + 1) % termsBefore;
            } while (i != head);
            /* number of remaining records to be displayed after this one */
            head = numTerms - (termsBefore + 1);
            printf ("%s\n", term);
            MemFree (term);
            return TRUE;
        }
    }

    if (arrayOfTerm[head] != NULL)
    {
        MemFree(arrayOfTerm[head]);
    }
    arrayOfTerm[head] = term;
    head = (head + 1) % termsBefore;
    return TRUE;
}

static Boolean
TermProcessing(CharPtr programStr, CharPtr termString, DocType db, Boolean parseOnly)
{
    Boolean specialAndTotal = FALSE;
    Boolean totalOnly = FALSE;
    Boolean termOnly = FALSE;
    Boolean centerOnTerm = FALSE;
    Boolean beginWithTerm = FALSE;
    Boolean endWithTerm = FALSE;
    Boolean inclusive;
    Char    fldStr[ENTREZ_FLD_MNEMONIC_LENGTH+1];
    DocField fld;
    Int2    firstPage;
    CharPtr countIndex;
    Int2    ratio;

    theTerm = termString;

    if (programStr == NULL || termString == NULL || (int) StrLen(programStr) <
        (3 + ENTREZ_FLD_MNEMONIC_LENGTH))
        return FALSE;

    switch (programStr[0])
    {
    case 's': /* output with special+total, tab-delimeted */
        specialAndTotal = TRUE; break;
    case 't':
        totalOnly = TRUE; break;
    case 'o':
        termOnly = TRUE; break;
    default:
        return FALSE;
    }
    switch (programStr[1])
    {
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        ratio = programStr[1] - '0';
        centerOnTerm = TRUE; break;
    case 'c': /* center on the term */
        ratio = 4;
        centerOnTerm = TRUE; break;
    case 'b':
        beginWithTerm = TRUE; break;
    case 'e':
        endWithTerm = TRUE; break;
    default:
        return FALSE;
    }

    switch (programStr[2])
    { /* ignored for centerOnTerm cases, above */
    case 'n': /* non-inclusive */
        inclusive = FALSE; break;
    case 'i': /* inclusive */
        inclusive = TRUE; break;
    default:
        return FALSE;
    }

    StrNCpy (fldStr, &programStr[3], ENTREZ_FLD_MNEMONIC_LENGTH);
    fldStr[ENTREZ_FLD_MNEMONIC_LENGTH] = '\0';
    if ((fld = EntrezStringToField(db, fldStr)) < 0)
        return FALSE;
    countIndex = &programStr[3+ENTREZ_FLD_MNEMONIC_LENGTH];
    numTerms = DEFAULT_TERMLIST_LEN;
    if (*countIndex != '\0' &&
        StrSpn(countIndex, "0123456789") == StrLen(countIndex))
    {
        numTerms = atoi(countIndex);
    }
        
    if (parseOnly)
        return TRUE;

    if (centerOnTerm)
    {
        EntrezTermListByTerm(db, fld, termString, 1, findOneTermProc, &firstPage);
        if (firstPage > 0)
        {
            firstPage--;
        }
        termsBefore = (Int2) (numTerms / ((float) ratio / 2));
        EntrezTermListByPage(db, fld, firstPage, 4, collectNumTermsProc);
        collectNumTermsProc(NULL, -1, -1); /* reset */
    } else {
        EntrezTermListByTerm(db, fld, termString, numTerms, beginTermProc, &firstPage);
    }
}

static ValNodePtr
ParseImportedFiles(CharPtr str)
{
    CharPtr localStr;
    CharPtr token;
    FILE *fp;
    Char s[100];
    DocType db;
    ValNodePtr head = NULL;
    ValNodePtr node;
    SavListPtr slp;
    Int2 linesread;
    Int4Ptr uids;
    CharPtr p;

    localStr = StringSave(str);
    token = StrTok(localStr, ", ");

    while (token != NULL)
    {
        if ((fp = FileOpen(token, "r")) == NULL)
        {
            Message(MSG_POST, "Error opening file %s", token);
            MemFree(localStr);
            return NULL;
        }
        linesread = 0;
        while (FileGets(s, (sizeof s) - 1, fp) != NULL)
        {
            linesread++;
            if (StrSpn(s, "0123456789 \n\r") != StrLen(s))
            {
                Message(MSG_POST, "Invalid character at line %d of file %s", linesread, token);
                FileClose(fp);
                MemFree(localStr);
                return NULL;
            }
            if (linesread == 1)
            {
                db = atoi(s);
                if (db != TYP_ML && db != TYP_AA && db != TYP_NT)
                {
                    Message(MSG_POST, "Invalid database type %d in file %s", db, token);
                    FileClose(fp);
                    MemFree(localStr);
                    return NULL;
                }
            }
        }
        fseek(fp, 0, SEEK_SET); /* rewind to beginning */
        uids = (Int4Ptr) MemNew(sizeof(Int4) * linesread);
        linesread = -1; /* skip over db this time */
        while (FileGets(s, (sizeof s) - 1, fp) != NULL)
        {
            if (linesread >= 0)
            {
                uids[linesread] = atoi(s);
            }
            linesread++;
        }
        FileClose(fp);
        slp = (SavListPtr) MemNew(sizeof(SavList));
        slp->uids = uids;
        slp->db = db;
        slp->num = linesread;
        if ((p = StringRChr(token, DIRDELIMCHR)) == NULL)
        {
            slp->name = MemNew(StrLen(token) + 2);
            StrCpy(&slp->name[1], token);
        } else {
            slp->name = StringSave(p);
        }
        slp->name[0] = '*'; /* to make the name unique, like in Entrez */
        if (head == NULL)
        {
            head = ValNodeNew(NULL);
            node = head;
        } else {
            node = ValNodeNew(head);
        }
        node->data.ptrvalue = (Pointer) slp;

        token = StrTok(NULL, ", ");
    }

    return head;
}
            

static LinkSetPtr
ParseUidList(CharPtr str)
{
    CharPtr localStr;
    CharPtr token;
    long uid;
    int i;
    Int4 count = 0;
    Int4Ptr vector;
    LinkSetPtr lsp;

    /* loop through twice ... the first time count, the second time, store values */
    for (i = 0; i < 2; i++)
    {
        localStr = StringSave(str);
        token = StrTok(localStr, ", ");
        count = 0;
        while (token != NULL)
        {
            if (StrSpn(token, "0123456789") != StrLen(token))
            {
                Message(MSG_POST, "parsing error at position %d", ((long) token) - ((long) localStr));
                MemFree(localStr);
                return NULL;
            }
            if (i == 1)
            {
                sscanf(token, "%ld", &uid);
                vector[count] = (Int4) uid;
            }
            count++;
            token = StrTok(NULL, ", ");
        }
        if (i == 0)
        {
            vector = MemNew(count * sizeof(Int4));
        }
        MemFree(localStr);
    }

    if (count == 0)
    {
        return NULL;
    }
    lsp = LinkSetNew();
    lsp->num = count;
    lsp->uids = vector;
    return lsp;
}

static CharPtr
FormatPositionalErr(Int2 beginErr, Int2 endErr, Int2 startLen)
{
    int i;
    CharPtr str;

    /* prepare text describing where error occurred */
    str = MemNew(endErr + startLen + 2);
    for (i = 0; i < endErr + startLen - 1; i++)
    {
        str[i] = ' ';
    }
    str[i++] =  '^';
    str[beginErr + startLen] = '^';
    str[i] =  '\0';

    return str;
}

static int
compUidsDescending(VoidPtr a, VoidPtr b)
{
    Int4Ptr x = (Int4Ptr) a;
    Int4Ptr y = (Int4Ptr) b;

    return (*y - *x);  /* note descending order */
}

static void
SortUidsDescending(LinkSetPtr lsp)
{
    Boolean sorted;
    int k;
    Int4 temp;

    if (lsp == NULL)
        return;

    /* try to sort uids in descending order */

    for (sorted = TRUE, k = 1; k < lsp->num; k++)
    {
        if (lsp->uids[k-1] < lsp->uids[k])
        {
            sorted = FALSE;
            break;
        }
    }

    if (! sorted)
    {   /* assume that the existing order is reversed */
        for (k = (lsp->num / 2) - 1; k >= 0; k--)
        {
            temp = lsp->uids[k];
            lsp->uids[k] = lsp->uids[lsp->num - 1 - k];
            lsp->uids[lsp->num - 1 - k] = temp;
        }

        /* now check that it's sorted */
        for (sorted = TRUE, k = 1; k < lsp->num; k++)
        {
            if (lsp->uids[k-1] < lsp->uids[k])
            {
                sorted = FALSE;
                break;
            }
        }

        if (! sorted)
        { /* as a last resort, sort them using heapsort */
            HeapSort(lsp->uids, lsp->num, sizeof(Int4), compUidsDescending);
        }
    }
}

static void
PrintHelp(void)
{
    printf ("Entrcmd is a non-interactive command-line interface which allows a user to\n");
    printf ("perform a series of neighboring and output operations, based upon an initial\n");
    printf ("set of UIDs or a boolean expression which describes a set of UIDs.\n");
    printf ("Alternatively, it can be used to display an alphabetically sorted list of\n");
    printf ("terms near an initial term.\n");
    printf ("\n");
    printf ("Type 'entrcmd' with no arguments for a brief summary of command-line options.\n");
    printf ("\n");
    printf ("    EXPRESSION SYNTAX (-e option)\n");
    printf ("\n");
    printf ("The following grammar is based upon Backus-Naur form.  Braces ({}) are used to\n");
    printf ("specify optional fields, and ellipses (...) represents an arbitrary number\n");
    printf ("of repititions.  In most Backus-Naur forms, the vertical bar (|) and brackets\n");
    printf ("([]) are used as meta-symbols.  However, in the following grammar, the\n");
    printf ("vertical bar and brackets are terminal symbols, and three stacked vertical\n");
    printf ("bars are used to represent alternation.\n");
    printf ("\n");
    printf ("expression ::= diff { - diff ... }\n");
    printf ("diff ::= term { | term ... }\n");
    printf ("term ::= factor { & factor ... }\n");
    printf ("                     |\n");
    printf ("factor ::= qualtoken | ( expression )\n");
    printf ("                     |\n");
    printf ("qualtoken ::= token { [ fld { ,S } ] }\n");
    printf ("\n");
    printf ("\n");
    printf ("token is a string of characters which either contains no special characters,\n");
    printf ("or which is delimited by double-quotes (\").  Double-quote marks and\n");
    printf ("backslashes (\\) which appear with a quoted token must be quoted by an\n");
    printf ("additional backslash.\n");
    printf ("\n");
    printf ("fld is an appropriate string describing a field.  The possible values are\n");
    printf ("described in the following table.  For all databases, an asterisk(*) is a\n");
    printf ("possible value for fld, signifying the union of all possible fields for that\n");
    printf ("database.  '*' is also the default field, if no field qualifier is specified.\n");
    printf ("\n");
    printf ("  | fld| Databases and descriptions\n");
    printf ("  +----+--------------------------------------------------------------------\n");
    printf ("  |WORD| For MEDLINE, \"Abstract or Title\"; for Sequences, \"Text Terms\"\n");
    printf ("  |MESH| MEDLINE only, \"MeSH term\"\n");
    printf ("  |AUTH| For all databases, \"Author Name\"\n");
    printf ("  |JOUR| For all databases, \"Journal Title\"\n");
    printf ("  |GENE| For all databases, \"Gene Name\"\n");
    printf ("  |KYWD| For MEDLINE, \"Substance\", for Sequences \"Keyword\"\n");
    printf ("  |ECNO| For MEDLINE and protein, \"E.C. number\"\n");
    printf ("  |ORGN| For all databases, \"Organism\"\n");
    printf ("  |ACCN| For Sequence databases, \"Accession\"\n");
    printf ("  |PROT| For protein, \"Protein Name\"\n");
    printf ("\n");
    printf ("The presence of \",S\"  after a field specifier implies the same semantics\n");
    printf ("as \"special\" in Entrez.  Entrez \"total\" semantics are the default.\n");
    printf ("\n");
    printf ("\n");
    printf ("    PROGRAM OF COMMANDS (-p option)\n");
    printf ("\n");
    printf ("For the \"-e\" and \"-u\" options, the program of commands consists of a sequence of\n");
    printf ("neighboring operations alternated with optional output commands.  All output\n");
    printf ("commands, except the first, must be preceded by a period (.), and all\n");
    printf ("neighboring commands must be preceded by a comma (,).\n");
    printf ("\n");
    printf ("The output commands are:\n");
    printf ("   no    None (default)             sg    Sequence GenBank flat file format\n");
    printf ("   ma    MEDLINE ASN.1 format       sa    Sequence ASN.1 format\n");
    printf ("   md    MEDLINE docsums            sd    Sequence docsums\n");
    printf ("   ml    MEDLARS format             sf    Sequence FASTA format\n");
    printf ("   mr    MEDLINE report format      sr    Sequence report format\n");
    printf ("   mu    MEDLINE UIDs               su    Sequence UIDs\n");
    printf ("Each output command may be followed by an optional count indicating how\n");
    printf ("many articles to display.  The default is to display all the articles.\n");
    printf ("\n");
    printf ("If the \"-x\" command line option appears (\"export to a saved UID list\"), then\n");
    printf ("the first \"mu\" or \"su\" command results in those UIDs being written to that\n");
    printf ("\"saved UID list\" file, rather than being written to the standard output.\n");
    printf ("\n");
    printf ("Neighboring commands indicate the database to neighbor \"to\", and\n");
    printf ("consists of the first letter of each of the possible databases:\n");
    printf ("(medline, protein, nucleotide) followed by an optional count of\n");
    printf ("how many of the current set of articles should be included in the\n");
    printf ("neighboring operation.\n");
    printf ("\n");
    printf ("Example:\n");
    printf ("  Find the articles written by \"Kay LE\", but not by \"Forman-Kay JD\".  Find\n");
    printf ("  their MEDLINE neighbors.  Print document summaries for all of these\n");
    printf ("  neighbors.  Of these neighbors, neighbor the first 5 entries to the protein\n");
    printf ("  database.  Print up to 10 of these sequences in Sequence Report format.\n");
    printf ("\n");
    printf ("    entrcmd -e '\"Kay LE\" [AUTH] - \"Forman-Kay JD\" [AUTH]' -p ,m.md,p5.sr10\n");
    printf ("\n");
    printf ("\n");
    printf ("If the \"-t\" option is used, then the program of commands is different from\n");
    printf ("what is described above.  Rather, it consists of a seven character string,\n");
    printf ("optionally followed by the number of terms which should be displayed.\n");
    printf ("The default number of terms is 40.\n");
    printf ("\n");
    printf ("The string is of the form '123FLDD', where 1, 2, and 3 are as follows,\n");
    printf ("and FLDD is one of the field specifications described above (AUTH, etc.).\n");
    printf ("\n");
    printf ("1 - one of 's', 't', or 'o', where 's' means that the special term counts\n");
    printf ("    should be displayed after the term, 't' means that the special and\n");
    printf ("    total term counts should be displayed after the term, and 'o' means\n");
    printf ("    that only the term itself should be displayed\n");
    printf ("2 - one of 'b', 'c', 'e', or an integer from 2 to 8, where:\n");
    printf ("    'b' - display terms beginning with the specified term\n");
    printf ("    'c' - \"center\" terms; i.e., display half the terms before the specified\n");
    printf ("          term, and half the terms after the specified term\n");
    printf ("    'e' - display terms ending with the specified term\n");
    printf ("    k   - an integer from 2 to 9, indicating that (2/k)ths of the terms\n");
    printf ("          should be alphabetically before the specified term.  Note that\n");
    printf ("          '4' is the same as 'c'.  The value '9' is recommended for\n");
    printf ("          scrolled displays.\n");
    printf ("3 - One of 'i' or 'n', indicating for the 'b' and 'e' options above whether\n");
    printf ("    the specified term is to be included in the output, where 'i' means\n");
    printf ("    inclusive, and 'n' means non-inclusive.  This value is ignored for\n");
    printf ("    other values of the previous character, but must be present as a\n");
    printf ("    place-holder.\n");
    printf ("\n");
    printf ("[ WARNING: MANY OF THESE TERM SPECIFICATIONS OPTIONS (COMBINATIONS OF 1,\n");
    printf ("2, AND 3 ABOVE) ARE CURRENTLY UNIMPLEMENTED ]\n");
    printf ("\n");
    printf ("\n");
    printf ("    WORLD WIDE WEB STYLE OUTPUT (-w option)\n");
    printf ("\n");
    printf ("The entrcmd program can also generate output which is appropriate for\n");
    printf ("display in an HTML document, to be \"served\" by a WWW server.  In particular,\n");
    printf ("some output text contains HTML hypertext links to other data, as well as\n");
    printf ("HTML formatting information.  The parameter to the -w option is the\n");
    printf ("directory prefix for the linked hypertext items; \"/htbin\" is recommended.\n");
    printf ("\n");
    printf ("If the \"-w\" option is selected, then the \"-f\" option may also be selected.\n");
    printf ("This indicates that the HTML output should be of a form which is\n");
    printf ("appropriate for a HTML \"FORM\".  This output can only be processed by\n");
    printf ("advanced WWW clients, but potentially provides a nicer interface, where\n");
    printf ("each document summary has an associated checkbox, resulting in a display\n");
    printf ("which is similar to the Entrez CD-ROM application.  The \"-c\" option, if used\n");
    printf ("in conjunction with \"-f\", indicates that these checkboxes should be\n");
    printf ("\"pre-checked\", i.e., selected.  This potentially provides the equivalent\n");
    printf ("of the Entrez \"select all\" operation for neighboring.\n");
}

Int2 Main()
{
    int Numarg = sizeof(myargs)/sizeof(Args);
    DocType db = TYP_ML;
    Boolean exprSpecified = FALSE;
    Boolean uidsSpecified = FALSE;
    Boolean termSpecified = FALSE;
    CharPtr boolString;
    short erract;
    ErrDesc err;
    Int2 beginErr;
    Int2 endErr;
    CharPtr str;
    Int2 uidCount;
    LinkSetPtr lsp = NULL;
    CharPtr programStr;
    CharPtr termString;
    CharPtr exportFile;
    CharPtr importFileList;
    Int2 progErr;
    ValNodePtr savlist = NULL;
    ValNodePtr np;
    SavListPtr slp;

    if ( ! GetArgs("Entrez command-line $Revision: 1.3 $", Numarg, myargs))
        return 1;

    if (myargs[6].intvalue)
    {
        PrintHelp();
        return 0;
    }

    if (myargs[0].strvalue != NULL)
    {
        switch(myargs[0].strvalue[0]) {
        case 'm': db = TYP_ML; break;
        case 'n': db = TYP_NT; break;
        case 'p': db = TYP_AA; break;
        default:
            Message(MSG_FATAL, "Invalid database type <%s>", myargs[0].strvalue);
        }
    }

    if (myargs[1].strvalue != NULL && myargs[1].strvalue[0] != '\0')
    {
        exprSpecified = TRUE;
        boolString = myargs[1].strvalue;
    }
    if (myargs[2].strvalue != NULL && myargs[2].strvalue[0] != '\0')
        uidsSpecified = TRUE;

    if (myargs[11].strvalue != NULL && myargs[11].strvalue[0] != '\0')
    {
        termSpecified = TRUE;
        termString = myargs[11].strvalue;
    }

    if (((exprSpecified != 0) + (uidsSpecified != 0) + (termSpecified != 0)) != 1)
    {
        Message(MSG_FATAL, "Exactly one of the -e, -t and -u options must be specified");
    }

    if (uidsSpecified)
    {
        lsp = ParseUidList(myargs[2].strvalue);
        if (lsp == NULL)
        {
            Message(MSG_FATAL, "Syntax error on UID list");
        }
    }

    if (exprSpecified)
    {
        ErrGetOpts(&erract, NULL);
        ErrSetOpts(ERR_CONTINUE, 0);
        ErrFetch(&err);
        if (! EntrezTLParseString(boolString, db, -1, &beginErr, &endErr))
        {
            ErrShow();
            Message(MSG_POST, "Syntax error: %s", boolString);
            if (endErr < 0)
            {
                endErr = 0;
            }

            str = FormatPositionalErr(beginErr, endErr, StrLen("Syntax error: "));

            Message(MSG_POST, str);
            MemFree(str);
            return 2;
        }
        ErrSetOpts(erract, 0);
    }

    wwwPrefix = myargs[5].strvalue;

    programStr = myargs[3].strvalue;

    useForms = myargs[7].intvalue;
    checkForms = myargs[8].intvalue;
    exportFile = myargs[9].strvalue;
    importFileList = myargs[10].strvalue;

    if (termSpecified)
    {
        if (! TermProcessing(programStr, termString, db, TRUE))
        {
            Message(MSG_POST, "Invalid term program specification %s", programStr);
            return 3;
        }
    } else {
        if ((progErr = RunProgram(programStr, NULL, db, TRUE)) != 0)
        {
            Message(MSG_POST, "Program error: %s", programStr);
            if (progErr > 0)
                str = FormatPositionalErr(progErr - 1, progErr - 1,
                                          StrLen("Program error: "));
            else
                str = StringSave("Validation error");
    
            Message(MSG_POST, str);
            MemFree(str);
            return 4;
        }
        if (exportFile != NULL)
        {
            exportFilePtr = FileOpen(exportFile, "w");
        }
        if (importFileList != NULL)
        {
            if ((savlist = ParseImportedFiles(importFileList)) == NULL)
            {
                Message(MSG_POST, "Fatal error processing imported files");
            }
        }
    }

    /* note that we defer EntrezInit() until we're sure that there are no */
    /* parsing errors                                                     */
    if (! EntrezInit("entrcmd", FALSE, NULL))
    {
        Message(MSG_POST, "Unable to access Entrez dataset");
        return 5;
    }

    if (myargs[4].intvalue)
    {
        str = EntrezDetailedInfo();
        printf ("                               STATUS REPORT\n\n\n%s\n\n", str);
        fflush(stdout);
    }

    while (savlist != NULL)
    { /* create named UID lists, as needed */
        slp = (SavListPtr) savlist->data.ptrvalue;
        EntrezCreateNamedUidList(slp->name, slp->db, 0, slp->num, slp->uids);
        MemFree(slp->name);
        MemFree(slp->uids);
        MemFree(slp);
        np = savlist->next;
        MemFree(savlist);
        savlist = np;
    }

    if (exprSpecified)
    { /* note that we deferred evaluation until after EntrezInit() */
        lsp = EntrezTLEvalString(boolString, db, -1, NULL, NULL);
        if (db == TYP_ML && lsp != NULL)
        {
            SortUidsDescending(lsp);
        }
    }

    if (termSpecified)
    {
        TermProcessing(programStr, termString, db, FALSE);
    } else {
        RunProgram(programStr, lsp, db, FALSE);
    }
    EntrezFini();

    return 0;
}

/*   accutils.c
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
* File Name:  accutils.c
*
* Author:  J. Epstein
*
* Version Creation Date:   10/18/93
*
* $Revision: 1.7 $
*
* File Description: 
*       Utilities which make use of the Entrez "data access library"
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*
* ==========================================================================
*/

#include <accutils.h>

#define LEXCHAR_LPAREN                1
#define LEXCHAR_RPAREN                2
#define LEXCHAR_LBRACKET              3
#define LEXCHAR_RBRACKET              4
#define LEXCHAR_QUOTE                 5
#define LEXCHAR_AND                   6
#define LEXCHAR_OR                    7
#define LEXCHAR_NOT                   8
#define LEXCHAR_COMMA                 9
#define LEXCHAR_ATSIGN               10
#define LEXCHAR_BACKSLASH            11
#define LEXCHAR_WHITESPACE           12
#define LEXCHAR_SEMICOLON            13
#define LEXCHAR_EOL                  14
#define LEXCHAR_NULL                 15
#define LEXCHAR_OTHER                16

#define LEXSTATE_IDLE                 0
#define LEXSTATE_BACKSLASHED          1
#define LEXSTATE_INQUOTE              2
#define LEXSTATE_INQUOTE_AFTERBSLASH  3
#define LEXSTATE_INSTRING             4
#define LEXSTATE_ERROR                5

#define LEXTOK_LPAREN                 1
#define LEXTOK_RPAREN                 2
#define LEXTOK_LBRACKET               3
#define LEXTOK_RBRACKET               4
#define LEXTOK_AND                    5
#define LEXTOK_OR                     6
#define LEXTOK_NOT                    7
#define LEXTOK_COMMA                  8
#define LEXTOK_ATSIGN                 9
#define LEXTOK_STRING                10

static Int2 lexPosition = 0;
static CharPtr lexString = NULL;
static Int2 lexState = LEXSTATE_IDLE;

static ValNodePtr nextnode = NULL;
static ValNode nextRealNode;
static Int2 lastGood = -1;
static Int2 lastBad = -1;

static
Int2 LexClassifyChar(Char c)
{
    Int2 retval;

    switch(c) {
    case '(':   retval = LEXCHAR_LPAREN; break;
    case ')':   retval = LEXCHAR_RPAREN; break;
    case '[':   retval = LEXCHAR_LBRACKET; break;
    case ']':   retval = LEXCHAR_RBRACKET; break;
    case '"':   retval = LEXCHAR_QUOTE; break;
    case '&':   retval = LEXCHAR_AND; break;
    case '|':   retval = LEXCHAR_OR; break;
    case '-':   retval = LEXCHAR_NOT; break;
    case ',':   retval = LEXCHAR_COMMA; break;
    case '@':   retval = LEXCHAR_ATSIGN; break;
    case '\\':  retval = LEXCHAR_BACKSLASH; break;
    case ' ':
    case '\t':  retval = LEXCHAR_WHITESPACE; break;
    case ';':   retval = LEXCHAR_SEMICOLON; break;
    case '\0':  retval = LEXCHAR_NULL; break;
    case '\r':
    case '\n':  retval = LEXCHAR_EOL; break;
    default:    retval = LEXCHAR_OTHER; break;
    }

    return retval;
}

/* Returns -1 if no token available, else the position of the token */
static Int2
EntrezLexExpression(CharPtr str, ValNodePtr vnp)
{
    Int2 startPos;
    Int2 classChar;
    Int2 token;
    Boolean done;
    Char c;
    CharPtr lexToken = NULL;
    CharPtr lexTokenStart;
    Int2 len;

    if (str == NULL && lexString == NULL)
        return -1;
    if (str != NULL)
    {
        MemFree(lexString);
        lexString = StringSave(str);
        MemFree(lexToken);
        lexPosition = 0;
        lexState = LEXSTATE_IDLE;
    }
    if (vnp == NULL)
        return -1;

    lexToken = MemNew(StringLen(&lexString[lexPosition]) + 1);
    lexTokenStart = lexToken;

    startPos = lexPosition;
    len = StringLen(lexString);
    if (lexPosition >= len)
    {
        lexState = LEXSTATE_ERROR;
        token = -1;
    }
    for (done = FALSE; ! done && lexPosition <= len; lexPosition++)
    {
        c = lexString[lexPosition];
        classChar = LexClassifyChar(c);
        switch (lexState) {
        case LEXSTATE_IDLE:
            switch (classChar) {
            case LEXCHAR_LPAREN:
                token = LEXTOK_LPAREN; done = TRUE; break;
            case LEXCHAR_RPAREN:
                token = LEXTOK_RPAREN; done = TRUE; break;
            case LEXCHAR_LBRACKET:
                token = LEXTOK_LBRACKET; done = TRUE; break;
            case LEXCHAR_RBRACKET:
                token = LEXTOK_RBRACKET; done = TRUE; break;
            case LEXCHAR_AND:
                token = LEXTOK_AND; done = TRUE; break;
            case LEXCHAR_OR:
                token = LEXTOK_OR; done = TRUE; break;
            case LEXCHAR_NOT:
                token = LEXTOK_NOT; done = TRUE; break;
            case LEXCHAR_COMMA:
                token = LEXTOK_COMMA; done = TRUE; break;
            case LEXCHAR_ATSIGN:
                token = LEXTOK_ATSIGN; done = TRUE; break;
            case LEXCHAR_QUOTE:
                lexState = LEXSTATE_INQUOTE; break;
            case LEXCHAR_BACKSLASH:
                lexState = LEXSTATE_BACKSLASHED; break;
            case LEXCHAR_EOL:
            case LEXCHAR_WHITESPACE:
                startPos = lexPosition + 1; break;
            case LEXCHAR_SEMICOLON:
            case LEXCHAR_NULL:
                lexState = LEXSTATE_ERROR; done = TRUE; break;
            case LEXCHAR_OTHER:
            default:
                lexState = LEXSTATE_INSTRING; *lexToken++ = c; break;
            }
            break;
        case LEXSTATE_BACKSLASHED:
            switch (classChar) {
            case LEXCHAR_NULL:
            case LEXCHAR_EOL:
                *lexToken++ = '\0'; done = TRUE; lexState = LEXSTATE_IDLE; break;
            case LEXCHAR_LPAREN:
            case LEXCHAR_RPAREN:
            case LEXCHAR_LBRACKET:
            case LEXCHAR_RBRACKET:
            case LEXCHAR_QUOTE:
            case LEXCHAR_AND:
            case LEXCHAR_OR:
            case LEXCHAR_NOT:
            case LEXCHAR_COMMA:
            case LEXCHAR_ATSIGN:
            case LEXCHAR_BACKSLASH:
            case LEXCHAR_WHITESPACE:
            case LEXCHAR_SEMICOLON:
            case LEXCHAR_OTHER:
            default:
                lexState = LEXSTATE_INSTRING; *lexToken++ = c; break;
            }
            break;
        case LEXSTATE_INQUOTE:
            switch (classChar) {
            case LEXCHAR_QUOTE:
                token = LEXTOK_STRING;
                *lexToken++ = '\0';
                done = TRUE;
                lexState = LEXSTATE_IDLE;
                break;
            case LEXCHAR_BACKSLASH:
                lexState = LEXSTATE_INQUOTE_AFTERBSLASH; break;
            case LEXCHAR_NULL:
            case LEXCHAR_EOL:
                lexState = LEXSTATE_ERROR; done = TRUE; break;
            default:
                *lexToken++ = c; break;
            }
            break;
        case LEXSTATE_INQUOTE_AFTERBSLASH:
            switch (classChar) {
            case LEXCHAR_NULL:
            case LEXCHAR_EOL:
                lexState = LEXSTATE_ERROR; done = TRUE; break;
            default:
                lexState = LEXSTATE_INQUOTE; *lexToken++ = c; break;
            }
            break;
        case LEXSTATE_INSTRING:
            switch (classChar) {
            case LEXCHAR_WHITESPACE:
            case LEXCHAR_SEMICOLON:
            case LEXCHAR_NULL:
            case LEXCHAR_EOL:
                token = LEXTOK_STRING;
                *lexToken++ = '\0';
                done = TRUE;
                lexState = LEXSTATE_IDLE;
                break;
            case LEXCHAR_BACKSLASH:
                lexState = LEXSTATE_BACKSLASHED;
                break;
            case LEXCHAR_QUOTE:
                lexState = LEXSTATE_INQUOTE;
                break;
            case LEXCHAR_OTHER:
                *lexToken++ = c; break;
            default:
                token = LEXTOK_STRING;
                *lexToken++ = '\0';
                done = TRUE;
                lexState = LEXSTATE_IDLE;
                lexPosition--; /* push back the last character */
                break;
            }
            break;
        case LEXSTATE_ERROR:
            done = TRUE;
            break;
        }
    }

    vnp->choice = (Uint1) token;
    vnp->data.ptrvalue = NULL;
    if (token == LEXTOK_STRING)
    {
        vnp->data.ptrvalue = lexTokenStart;
    } else {
        MemFree(lexTokenStart);
    }
    if (lexState == LEXSTATE_ERROR)
        return -1;
    else
        return startPos;
}

static void StrNextNode(void)
{
    nextnode = &nextRealNode;

    if (EntrezLexExpression(NULL, nextnode) < 0)
    {
        nextnode = NULL;
    }

    lastGood = lastBad;
    lastBad = lexPosition;
}

static Boolean StrExpression PROTO((ValNodePtr elst, DocType db, DocField fld));

static Boolean FindTermRequired(CharPtr term)
{
    Int2 len;

    if (term == NULL)
        return FALSE;
    len = StrLen(term);
    return ((len > 3 && term[len-1] == '.' && term[len-2] == '.' &&
        term[len-3] == '.') || (len > 1 && term[len-1] == '*') ||
        StrChr(term, '?') != NULL);
}

/* traverse all the possible fields, and take the "union" of this term */
/* over all the possibilities for the specified database               */
static Boolean
AddAllFields(ValNodePtr elst, CharPtr term, DocType db, Boolean special)
{
    EntrezInfoPtr eip;
    Boolean first = TRUE;
    Int4 specialCount;
    Int4 totalCount;
    EntrezFieldDataPtr fields;
    DocField fld;
    Boolean findTermRequired;

    if ((eip = EntrezGetInfo()) == NULL || elst == NULL)
        return FALSE;
    if (db < 0 || db >= eip->type_count)
        return FALSE;
    findTermRequired = FindTermRequired(term);
    fields = eip->types[db].fields;
    for (fld = 0; fld < eip->field_count; fld++)
    {
        if (fields == NULL || fields[fld].num_terms <= 0)
            continue;
        if (first)
        {
            EntrezTLAddLParen(elst);
            first = FALSE;
        } else {
            EntrezTLAddOR(elst);
        }
        if (findTermRequired)
        {
            EntrezFindTerm(db, fld, term, &specialCount, &totalCount);
        }
        EntrezTLAddTerm(elst, term, db, fld, special);
    }

    if (! first)
    {
        EntrezTLAddRParen(elst);
    }

    return TRUE;
}

static Boolean
StrFactor(ValNodePtr elst, DocType db, DocField fld)
{
    if (nextnode == NULL)
    {
        ErrPost(CTX_NCBICD, 1, "Null factor");
        return FALSE;
    }
    if (nextnode->choice == LEXTOK_LPAREN)
    {
        if (elst != NULL)
            EntrezTLAddLParen (elst);
        StrNextNode();
        if (! StrExpression(elst, db, fld))
        {
            return FALSE;
        }
        if (nextnode != NULL && nextnode->choice == LEXTOK_RPAREN)
        {
            if (elst != NULL)
                EntrezTLAddRParen (elst);
            StrNextNode();
        } else {
            ErrPost(CTX_NCBICD, 1, "Missing right paren");
            return FALSE;
        }
    } else {
        if (nextnode->choice == LEXTOK_STRING)
        {
            CharPtr term;
            Boolean special = FALSE;
            CharPtr fldStr;
            Boolean allFields = FALSE;

            term = (CharPtr) (nextnode->data.ptrvalue);
            StrNextNode();
            if (nextnode != NULL && nextnode->choice == LEXTOK_LBRACKET)
            {
                StrNextNode();
                if (nextnode == NULL || nextnode->choice != LEXTOK_STRING)
                {
                    ErrPost(CTX_NCBICD, 1, "missing field id after bracket");
                    MemFree(term);
                    return FALSE;
                }
                fldStr = (CharPtr) (nextnode->data.ptrvalue);
                if (fldStr != NULL && StrCmp(fldStr, "*") == 0)
                {
                    allFields = TRUE;
                } else {
                    fld = EntrezStringToField(db, fldStr);
                }
                MemFree(nextnode->data.ptrvalue);
                if (!allFields && fld < 0)
                {
                    ErrPost(CTX_NCBICD, 1, "bad field identifier");
                    MemFree(term);
                    return FALSE;
                }
                StrNextNode();
                if (nextnode == NULL || (nextnode->choice != LEXTOK_COMMA &&
                    nextnode->choice != LEXTOK_RBRACKET))
                {
                    ErrPost(CTX_NCBICD, 1, "missing right bracket");
                    MemFree(term);
                    return FALSE;
                }
                if (nextnode->choice == LEXTOK_COMMA)
                {
                    StrNextNode();
                    if (nextnode == NULL || nextnode->choice != LEXTOK_STRING ||
                        StringCmp(nextnode->data.ptrvalue, "S") != 0)
                    {
                        ErrPost(CTX_NCBICD, 1, "field qualifier error");
                        MemFree(term);
                        return FALSE;
                    }
                    MemFree(nextnode->data.ptrvalue);
                    special = TRUE;
                    StrNextNode();
                    if (nextnode == NULL || nextnode->choice != LEXTOK_RBRACKET)
                    {
                        ErrPost(CTX_NCBICD, 1, "missing right bracket");
                        MemFree(term);
                        return FALSE;
                    }
                }
                StrNextNode();
            }
            if (elst != NULL)
            {
                /* if ( the default specified by caller is -1 ==> all ) then */
                if (fld < 0)
                {
                    allFields = TRUE;
                }
                if (allFields)
                {
                   AddAllFields(elst, term, db, special);
                } else {
                    Int4 specialCount;
                    Int4 totalCount;

                    if (FindTermRequired(term))
                    {
                        EntrezFindTerm(db, fld, term, &specialCount, &totalCount);
                    }
                    EntrezTLAddTerm(elst, term, db, fld, special);
                }
            }
            MemFree (term);
        } else {
            ErrPost(CTX_NCBICD, 1, "invalid token");
            return FALSE;
        }
    }

    return TRUE;
}


static Boolean
StrTerm(ValNodePtr elst, DocType db, DocField fld)
{
    if (! StrFactor(elst, db, fld))
        return FALSE;
    while (nextnode != NULL && nextnode->choice == LEXTOK_AND)
    {
        if (elst != NULL)
            EntrezTLAddAND(elst);
        StrNextNode();
        if (! StrFactor(elst, db, fld))
            return FALSE;
    }

    return TRUE;
}
   
static Boolean
StrDiff(ValNodePtr elst, DocType db, DocField fld)
{
    if (! StrTerm(elst, db, fld))
        return FALSE;
    while (nextnode != NULL && nextnode->choice == LEXTOK_OR)
    {
        if (elst != NULL)
            EntrezTLAddOR(elst);
        StrNextNode();
        if (! StrTerm(elst, db, fld))
            return FALSE;
    }

    return TRUE;
}
   
static Boolean
StrExpression(ValNodePtr elst, DocType db, DocField fld)
{
    if (! StrDiff(elst, db, fld))
        return FALSE;
    while (nextnode != NULL && nextnode->choice == LEXTOK_NOT)
    {
        if (elst != NULL)
            EntrezTLAddBUTNOT(elst);
        StrNextNode();
        if (! StrDiff(elst, db, fld))
            return FALSE;
    }

    return TRUE;
}

CharPtr LIBCALL EntrezFieldToString(DocType db, DocField fld)
{
    CharPtr fldStr;

    switch(fld)
    {
    case FLD_WORD:
        fldStr = "WORD"; break;
    case FLD_MESH:
        fldStr = "MESH"; break;
    case FLD_AUTH:
        fldStr = "AUTH"; break;
    case FLD_JOUR:
        fldStr = "JOUR"; break;
    case FLD_GENE:
        fldStr = "GENE"; break;
    case FLD_KYWD:
        fldStr = "KYWD"; break;
    case FLD_ECNO:
        fldStr = "ECNO"; break;
    case FLD_ORGN:
        fldStr = "ORGN"; break;
    case FLD_ACCN:
        fldStr = "ACCN"; break;
    case FLD_PROT:
        fldStr = "PROT"; break;
    default:
        fldStr = "????";
    }

    return StringSave(fldStr);
}

DocField LIBCALL EntrezStringToField(DocType db, CharPtr str)
{
    if (str == NULL)
        return -1;
    if (StringCmp(str, "WORD") == 0)
        return FLD_WORD;
    if (StringCmp(str, "MESH") == 0)
        return FLD_MESH;
    if (StringCmp(str, "AUTH") == 0)
        return FLD_AUTH;
    if (StringCmp(str, "JOUR") == 0)
        return FLD_JOUR;
    if (StringCmp(str, "GENE") == 0)
        return FLD_GENE;
    if (StringCmp(str, "KYWD") == 0)
        return FLD_KYWD;
    if (StringCmp(str, "ECNO") == 0)
        return FLD_ECNO;
    if (StringCmp(str, "ORGN") == 0)
        return FLD_ORGN;
    if (StringCmp(str, "ACCN") == 0)
        return FLD_ACCN;
    if (StringCmp(str, "PROT") == 0)
        return FLD_PROT;
    return -1;
}


LinkSetPtr LIBCALL EntrezTLEvalString(CharPtr str, DocType db, DocField fld, Int2Ptr begin, Int2Ptr end)
{
    LinkSetPtr lsp;
    ValNodePtr elst;

    if (begin != NULL)
    {
        *begin = -1;
    }
    if (end != NULL)
    {
        *end = -1;
    }

    if (str == NULL)
    {
        return NULL;
    }

    if (db != TYP_ML && db != TYP_SEQ && db != TYP_AA && db != TYP_NT)
    {
        return NULL;
    }

    if ((elst = EntrezTLNew(db)) == NULL)
    {
        return NULL;
    }

    EntrezLexExpression(str, NULL);
    StrNextNode();

    lsp = NULL;
    if (StrExpression(elst, db, fld) && nextnode == NULL)
    {
        lsp = EntrezTLEval(elst);
    }
    EntrezTLFree(elst);

    if (lastGood < lastBad)
    {
        lastGood = lastBad;
    }
    if (begin != NULL)
    {
        *begin = lastGood;
    }
    if (end != NULL)
    {
        *end = lastBad;
    }

    return lsp;
}

Boolean LIBCALL EntrezTLParseString(CharPtr str, DocType db, DocField fld, Int2Ptr begin, Int2Ptr end)
{
    Boolean retval;

    if (begin != NULL)
    {
        *begin = -1;
    }
    if (end != NULL)
    {
        *end = -1;
    }

    if (str == NULL)
    {
        return FALSE;
    }

    if (db != TYP_ML && db != TYP_SEQ && db != TYP_AA && db != TYP_NT)
    {
        return FALSE;
    }
    EntrezLexExpression(str, NULL);
    StrNextNode();

    retval = StrExpression(NULL, db, fld) && nextnode == NULL;

    if (lastGood < lastBad)
    {
        lastGood = lastBad;
    }
    if (begin != NULL)
    {
        *begin = lastGood;
    }
    if (end != NULL)
    {
        *end = lastBad;
    }

    return retval;
}

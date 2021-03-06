#define programName        "fastDNAml"
#define programVersion     "1.1.1a"
#define programVersionInt   10101
#define programDate        "November 2, 1994"
#define programDateInt      19941102



/*  fastDNAml
 *
 *  Olsen, G. J., Matsuda, H., Hagstrom, R., and Overbeek, R.  1994.
 *  fastDNAml:  A tool for construction of phylogenetic trees of DNA
 *  sequences using maximum likelihood.  Comput. Appl. Biosci. 10: 41-48.
 *
 *
 *  Based in (large) part on the program dnaml by Joseph Felsenstein.
 *
 *  Felsenstein, J.  1981.  Evolutionary trees from DNA sequences:
 *  A maximum likelihood approach.  J. Mol. Evol. 17: 368-376.
 *
 *
 *  Copyright notice from dnaml:
 *
 *  version 3.3. (c) Copyright 1986, 1990 by the University of Washington
 *  and Joseph Felsenstein.  Written by Joseph Felsenstein.  Permission is
 *  granted to copy and use this program provided no fee is charged for it
 *  and provided that this copyright notice is not removed.
 */

/*  Conversion to C and changes in sequential code by Gary Olsen, 1991-1994
 *
 *  p4 version by Hideo Matsuda and Ross Overbeek, 1991-1993
 */

/*
 *  1.0    March 14, 1992
 *         Initial "release" version
 *
 *  1.0.1  March 18, 1992
 *         Add ntaxa to tree comments
 *         Set minimum branch length on reading tree
 *         Add blanks around operators in treeString (for prolog parsing)
 *         Add program version to treeString comments
 *
 *  1.0.2  April 6, 1992
 *         Improved option line diagnostics
 *         Improved auxiliary line diagnostics
 *         Removed some trailing blanks from output
 *
 *  1.0.3  April 6, 1992
 *         Checkpoint trees that do not need any optimization
 *         Print restart tree likelihood before optimizing
 *         Fix treefile option so that it really toggles
 *
 *  1.0.4  July 13, 1992
 *         Add support for tree fact (instead of true Newick tree) in
 *            processTreeCom, treeReadLen, str_processTreeCom and
 *            str_treeReadLen
 *         Use bit operations in randum
 *         Correct error in bootstrap mask used with weighting mask
 *
 *  1.0.5  August 22, 1992
 *         Fix reading of underscore as first nonblank character in name
 *         Add strchr and strstr functions to source code
 *         Add output treefile name to message "Tree also written ..."
 *
 *  1.0.6  November 20, 1992
 *         Change (! nsites) test in setupTopol to (nsites == 0) for MIPS R4000
 *         Add vectorizing compiler directives for CRAY
 *         Include updates and corrections to parallel code from H. Matsuda
 *
 *  1.0.7  March 25, 1993
 *         Remove translation of underlines in taxon names
 *
 *  1.0.8  April 30, 1993
 *         Remove version number from fastDNAml.h file name
 *
 *  1.0.9  August 12, 1993
 *         Version was never released.
 *         Redefine treefile formats and default:
 *             0  None
 *             1  Newick
 *             2  Prolog
 *             3  PHYLIP (Default)
 *         Remove quote marks and comment from PHYLIP treefile format.
 *
 *  1.1.0  September 3-5, 1993
 *         Arrays of size maxpatterns moved from stack to heap (mallocs) in
 *            evaluateslope, makenewz, and cmpBestTrees.
 *         Correct [maxsites] to [maxpatterns] in temporary array definitions
 *            in Vectorize code of newview and evaluate.  (These should also
 *            get converted to use malloc() at some point.)
 *         Change randum to use 12 bit segments, not 6.  Change its seed
 *            parameter to long *.
 *         Remove the code that took the absolute value of random seeds.
 *         Correct integer divide artifact in setting default transition/
 *            transversion parameter values.
 *         When transition/transversion ratio is "reset", change to small
 *            value, not the program default.
 *         Report the "reset" transition/transversion ratio in the output.
 *         Move global data into analdef, rawDNA, and crunchedDNA structures.
 *         Change names of routines white and digit to whitechar and digitchar.
 *         Convert y[] to yType, which is normally char, but is int if the
 *            Vectorize flag is set.
 *         Split option line reading out of getoptions routine.
 *
 *  1.1.1  September 30, 1994
 *         Incorporate changes made in 1.0.A (Feb. 11, 1994):
 *            Remove processing of quotation marks within comments.
 *            Break label finding into copy to string and find tip.
 *            Generalize tree reading to read trees when names are and are not
 *               already known.
 *            Remove absolute value from randum seed reading.
 *         Include integer version number and program date.
 *         Remove maxsite, maxpatterns and maxsp limitations.
 *         Incorporate code for retaining multiple trees.
 *         Activate code for Hasegawa & Kishino test of tree differences.
 *         Make quick add the default, with Q turning it off.
 *         Make emperical frequencies the option with F turning it off.
 *         Allow a residue frequency option line anywhere in the options.
 *         Increase string length passed to treeString (should be length
 *               checked, but ...)
 *         Introduce (Sept.30) and fix (Oct. 26) bug in bootstrap sampling.
 *         Fix error when user frequencies are last line and start with F.
 */

#ifdef Master
#  undef  Master
#  define Master     1
#  define Slave      0
#  define Sequential 0
#else
#  ifdef Slave
#    undef Slave
#    define Master     0
#    define Slave      1
#    define Sequential 0
#  else
#    ifdef Sequential
#      undef Sequential
#    endif
#    define Master     0
#    define Slave      0
#    define Sequential 1
#  endif
#endif

#ifdef  CRAY
#  define  Vectorize
#endif

#ifdef  Vectorize
#  define maxpatterns  10000  /* maximum number of different site patterns */
#endif

#include <stdio.h>
#include <math.h>
#include "fastDNAml.h"  /*  Requires version 1.1  */

#if Master || Slave
#  include "p4.h"
#  include "comm_link.h" 
#endif

/*  Global variables */

xarray       *usedxtip, *freextip;

#if Sequential     /*  Use standard input */
#  undef   DNAML_STEP
#  define  DNAML_STEP  0
#ifdef NoSTDIN
  FILE *INFILE;
#else
#  define  INFILE  stdin
#endif
#endif

#if Master
#  define MAX_SEND_AHEAD 400
  char   *best_tr_recv = NULL;     /* these are used for flow control */
  double  best_lk_recv;
  int     send_ahead = 0;          /* number of outstanding sends */

#  ifdef DNAML_STEP
#    define  DNAML_STEP  1
#  endif
#  define  INFILE   Seqf
#  define  OUTFILE  Outf
  FILE  *INFILE, *OUTFILE;
  comm_block comm_slave;
#endif

#if Slave
#  undef   DNAML_STEP
#  define  DNAML_STEP  0
#  define  INFILE   Seqf
#  define  OUTFILE  Outf
  FILE  *INFILE, *OUTFILE;
  comm_block comm_master;
#endif

#if Debug
  FILE *debug;
#endif

#if DNAML_STEP
  int begin_step_time, end_step_time;
#  define  REPORT_ADD_SPECS  p4_send(DNAML_ADD_SPECS, DNAML_HOST_ID, NULL, 0)
#  define  REPORT_SEND_TREE  p4_send(DNAML_SEND_TREE, DNAML_HOST_ID, NULL, 0)
#  define  REPORT_RECV_TREE  p4_send(DNAML_RECV_TREE, DNAML_HOST_ID, NULL, 0)
#  define  REPORT_STEP_TIME \
   {\
       char send_buf[80]; \
       end_step_time = p4_clock(); \
       (void) sprintf(send_buf, "%d", end_step_time-begin_step_time); \
       p4_send(DNAML_STEP_TIME, DNAML_HOST_ID, send_buf,strlen(send_buf)+1); \
       begin_step_time = end_step_time; \
   }
#else
#  define  REPORT_ADD_SPECS
#  define  REPORT_SEND_TREE
#  define  REPORT_RECV_TREE
#  define  REPORT_STEP_TIME
#endif

/*=======================================================================*/
/*                              PROGRAM                                  */
/*=======================================================================*/
/*                    Best tree handling for dnaml                       */
/*=======================================================================*/

/*  Tip value comparisons
 *
 *  Use void pointers to hide type from other routines.  Only tipValPtr and
 *  cmpTipVal need to be changed to alter the nature of the values compared
 *  (e.g., names instead of node numbers).
 *
 *    cmpTipVal(tipValPtr(nodeptr p), tipValPtr(nodeptr q)) == -1, 0 or 1.
 *
 *  This provides direct comparison of tip values (for example, for
 *  definition of tr->start).
 */


void  *tipValPtr (p)  nodeptr  p; { return  (void *) & p->number; }


int  cmpTipVal (v1, v2)
    void  *v1, *v2;
  { /* cmpTipVal */
    int  i1, i2;

    i1 = *((int *) v1);
    i2 = *((int *) v2);
    return  (i1 < i2) ? -1 : ((i1 == i2) ? 0 : 1);
  } /* cmpTipVal */


/*  These are the only routines that need to UNDERSTAND topologies */


topol  *setupTopol (maxtips, nsites)
    int     maxtips, nsites;
  { /* setupTopol */
    topol   *tpl;

    if (! (tpl = (topol *) Malloc(sizeof(topol))) || 
        ! (tpl->links = (connptr) Malloc((2*maxtips-3) * sizeof(connect))) || 
        (nsites && ! (tpl->log_f
                = (double *) Malloc(nsites * sizeof(double))))) {
      printf("ERROR: Unable to get topology memory");
      tpl = (topol *) NULL;
      }

    else {
      if (nsites == 0)  tpl->log_f = (double *) NULL;
      tpl->likelihood  = unlikely;
      tpl->start       = (node *) NULL;
      tpl->nextlink    = 0;
      tpl->ntips       = 0;
      tpl->nextnode    = 0;
      tpl->opt_level   = 0;     /* degree of branch swapping explored */
      tpl->scrNum      = 0;     /* position in sorted list of scores */
      tpl->tplNum      = 0;     /* position in sorted list of trees */
      tpl->log_f_valid = 0;     /* log_f value sites */
      tpl->prelabeled  = TRUE;
      tpl->smoothed    = FALSE; /* branch optimization converged? */
      }

    return  tpl;
  } /* setupTopol */


void  freeTopol (tpl)
    topol  *tpl;
  { /* freeTopol */
    Free(tpl->links);
    if (tpl->log_f)  Free(tpl->log_f);
    Free(tpl);
  } /* freeTopol */


int  saveSubtree (p, tpl)
    nodeptr  p;
    topol   *tpl;
    /*  Save a subtree in a standard order so that earlier branches
     *  from a node contain lower value tips than do second branches from
     *  the node.  This code works with arbitrary furcations in the tree.
     */
  { /* saveSubtree */
    connptr  r, r0;
    nodeptr  q, s;
    int      t, t0, t1;

    r0 = tpl->links;
    r = r0 + (tpl->nextlink)++;
    r->p = p;
    r->q = q = p->back;
    r->z = p->z;
    r->descend = 0;                     /* No children (yet) */

    if (q->tip) {
      r->valptr = tipValPtr(q);         /* Assign value */
      }

    else {                              /* Internal node, look at children */
      s = q->next;                      /* First child */
      do {
        t = saveSubtree(s, tpl);        /* Generate child's subtree */

        t0 = 0;                         /* Merge child into list */
        t1 = r->descend;
        while (t1 && (cmpTipVal(r0[t1].valptr, r0[t].valptr) < 0)) {
          t0 = t1;
          t1 = r0[t1].sibling;
          }
        if (t0) r0[t0].sibling = t;  else  r->descend = t;
        r0[t].sibling = t1;

        s = s->next;                    /* Next child */
        } while (s != q);

      r->valptr = r0[r->descend].valptr;   /* Inherit first child's value */
      }                                 /* End of internal node processing */

    return  r - r0;
  } /* saveSubtree */


nodeptr  minSubtreeTip (p0)
    nodeptr  p0;
  { /* minTreeTip */
    nodeptr  minTip, p, testTip;

    if (p0->tip) return p0;

    p = p0->next;
    minTip = minSubtreeTip(p->back);
    while ((p = p->next) != p0) {
      testTip = minSubtreeTip(p->back);
      if (cmpTipVal(tipValPtr(testTip), tipValPtr(minTip)) < 0)
        minTip = testTip;
      }
    return minTip;
  } /* minTreeTip */


nodeptr  minTreeTip (p)
    nodeptr  p;
  { /* minTreeTip */
    nodeptr  minp, minpb;

    minp  = minSubtreeTip(p);
    minpb = minSubtreeTip(p->back);
    return cmpTipVal(tipValPtr(minp), tipValPtr(minpb)) < 0 ? minp : minpb;
  } /* minTreeTip */


void saveTree (tr, tpl)
    tree   *tr;
    topol  *tpl;
    /*  Save a tree topology in a standard order so that first branches
     *  from a node contain lower value tips than do second branches from
     *  the node.  The root tip should have the lowest value of all.
     */
  { /* saveTree */
    connptr  r;
    double  *tr_log_f, *tpl_log_f;
    int  i;

    tpl->nextlink = 0;                             /* Reset link pointer */
    r = tpl->links + saveSubtree(minTreeTip(tr->start), tpl);  /* Save tree */
    r->sibling = 0;

    tpl->likelihood = tr->likelihood;
    tpl->start      = tr->start;
    tpl->ntips      = tr->ntips;
    tpl->nextnode   = tr->nextnode;
    tpl->opt_level  = tr->opt_level;
    tpl->prelabeled = tr->prelabeled;
    tpl->smoothed   = tr->smoothed;

    if (tpl_log_f = tpl->log_f) {
      tr_log_f  = tr->log_f;
      i = tpl->log_f_valid = tr->log_f_valid;
      while (--i >= 0)  *tpl_log_f++ = *tr_log_f++;
      }
    else {
      tpl->log_f_valid = 0;
      }
  } /* saveTree */


void copyTopol (tpl1, tpl2)
    topol  *tpl1, *tpl2;
  { /* copyTopol */
    connptr  r1, r2, r10, r20;
    double  *tpl1_log_f, *tpl2_log_f;
    int  i;

    r10 = tpl1->links;
    r20 = tpl2->links;
    tpl2->nextlink = tpl1->nextlink; 

    r1 = r10;
    r2 = r20;
    i = 2 * tpl1->ntips - 3;
    while (--i >= 0) {
      r2->z = r1->z;
      r2->p = r1->p;
      r2->q = r1->q;
      r2->valptr = r1->valptr;
      r2->descend = r1->descend; 
      r2->sibling = r1->sibling; 
      r1++;
      r2++;
      }

    if (tpl1->log_f_valid && tpl2->log_f) {
      tpl1_log_f = tpl1->log_f;
      tpl2_log_f = tpl2->log_f;
      tpl2->log_f_valid = i = tpl1->log_f_valid;
      while (--i >= 0)  *tpl2_log_f++ = *tpl1_log_f++;
      }
    else {
      tpl2->log_f_valid = 0;
      }

    tpl2->likelihood = tpl1->likelihood;
    tpl2->start      = tpl1->start;
    tpl2->ntips      = tpl1->ntips;
    tpl2->nextnode   = tpl1->nextnode;
    tpl2->opt_level  = tpl1->opt_level;
    tpl2->prelabeled = tpl1->prelabeled;
    tpl2->scrNum     = tpl1->scrNum;
    tpl2->tplNum     = tpl1->tplNum;
    tpl2->smoothed   = tpl1->smoothed;
  } /* copyTopol */


boolean restoreTree (tpl, tr)
    topol  *tpl;
    tree   *tr;
  { /* restoreTree */
    void  hookup();
    boolean  initrav();

    connptr  r;
    nodeptr  p, p0;
    double  *tr_log_f, *tpl_log_f;
    int  i;

/*  Clear existing connections */

    for (i = 1; i <= 2*(tr->mxtips) - 2; i++) {  /* Uses p = p->next at tip */
      p0 = p = tr->nodep[i];
      do {
        p->back = (nodeptr) NULL;
        p = p->next;
        } while (p != p0);
      }

/*  Copy connections from topology */

    for (r = tpl->links, i = 0; i < tpl->nextlink; r++, i++) {
      hookup(r->p, r->q, r->z);
      }

    tr->likelihood = tpl->likelihood;
    tr->start      = tpl->start;
    tr->ntips      = tpl->ntips;
    tr->nextnode   = tpl->nextnode;
    tr->opt_level  = tpl->opt_level;
    tr->prelabeled = tpl->prelabeled;
    tr->smoothed   = tpl->smoothed;

    if (tpl_log_f = tpl->log_f) {
      tr_log_f = tr->log_f;
      i = tr->log_f_valid = tpl->log_f_valid;
      while (--i >= 0)  *tr_log_f++ = *tpl_log_f++;
      }
    else {
      tr->log_f_valid = 0;
      }

    return (initrav(tr, tr->start) && initrav(tr, tr->start->back));
  } /* restoreTree */


int initBestTree (bt, newkeep, numsp, sites)
    bestlist  *bt;
    int        newkeep, numsp, sites;
  { /* initBestTree */
    int  i, nlogf;


    bt->nkeep = 0;

    if (bt->ninit <= 0) {
      if (! (bt->start = setupTopol(numsp, sites)))  return  0;
      bt->ninit = -1;
      bt->nvalid = 0;
      bt->numtrees = 0;
      bt->best = unlikely;
      bt->improved = FALSE;
      bt->byScore = (topol **) Malloc((newkeep+1) * sizeof(topol *));
      bt->byTopol = (topol **) Malloc((newkeep+1) * sizeof(topol *));
      if (! bt->byScore || ! bt->byTopol) {
        fprintf(stderr, "initBestTree: Malloc failure\n");
        return 0;
        }
      }
    else if (ABS(newkeep) > bt->ninit) {
      if (newkeep <  0) newkeep = -(bt->ninit);
      else newkeep = bt->ninit;
      }

    if (newkeep < 1) {    /*  Use negative newkeep to clear list  */
      newkeep = -newkeep;
      if (newkeep < 1) newkeep = 1;
      bt->nvalid = 0;
      bt->best = unlikely;
      }

    if (bt->nvalid >= newkeep) {
      bt->nvalid = newkeep;
      bt->worst = bt->byScore[newkeep]->likelihood;
      }
    else {
      bt->worst = unlikely;
      }

    for (i = bt->ninit + 1; i <= newkeep; i++) {
      nlogf = (i <= maxlogf) ? sites : 0;
      if (! (bt->byScore[i] = setupTopol(numsp, nlogf)))  break;
      bt->byTopol[i] = bt->byScore[i];
      bt->ninit = i;
      }

    return  (bt->nkeep = MIN(newkeep, bt->ninit));
  } /* initBestTree */



int resetBestTree (bt)
    bestlist  *bt;
  { /* resetBestTree */
    bt->best = unlikely;
    bt->worst = unlikely;
    bt->nvalid = 0;
    bt->improved = FALSE;
  } /* resetBestTree */


boolean  freeBestTree(bt)
    bestlist  *bt;
  { /* freeBestTree */
    while (bt->ninit >= 0)  freeTopol(bt->byScore[(bt->ninit)--]);
    freeTopol(bt->start);
    return TRUE;
  } /* freeBestTree */


/*  Compare two trees, assuming that each is in standard order.  Return
 *  -1 if first preceeds second, 0 if they are identical, or +1 if first
 *  follows second in standard order.  Lower number tips preceed higher
 *  number tips.  A tip preceeds a corresponding internal node.  Internal
 *  nodes are ranked by their lowest number tip.
 */

int  cmpSubtopol (p10, p1, p20, p2)
    connptr  p10, p1, p20, p2;
  { /* cmpSubtopol */
    connptr  p1d, p2d;
    int  cmp;

    if (! p1->descend && ! p2->descend)          /* Two tips */
      return cmpTipVal(p1->valptr, p2->valptr);

    if (! p1->descend) return -1;                /* p1 = tip, p2 = node */
    if (! p2->descend) return  1;                /* p2 = tip, p1 = node */

    p1d = p10 + p1->descend;
    p2d = p20 + p2->descend;
    while (1) {                                  /* Two nodes */
      if (cmp = cmpSubtopol(p10, p1d, p20, p2d))  return cmp; /* Subtrees */
      if (! p1d->sibling && ! p2d->sibling)  return  0; /* Lists done */
      if (! p1d->sibling) return -1;             /* One done, other not */
      if (! p2d->sibling) return  1;             /* One done, other not */
      p1d = p10 + p1d->sibling;                  /* Neither done */
      p2d = p20 + p2d->sibling;
      }
  } /* cmpSubtopol */



int  cmpTopol (tpl1, tpl2)
    void  *tpl1, *tpl2;
  { /* cmpTopol */
    connptr  r1, r2;
    int      cmp;

    r1 = ((topol *) tpl1)->links;
    r2 = ((topol *) tpl2)->links;
    cmp = cmpTipVal(tipValPtr(r1->p), tipValPtr(r2->p));
    if (cmp) return cmp;
    return  cmpSubtopol(r1, r1, r2, r2);
  } /* cmpTopol */



int  cmpTplScore (tpl1, tpl2)
    void  *tpl1, *tpl2;
  { /* cmpTplScore */
    double  l1, l2;

    l1 = ((topol *) tpl1)->likelihood;
    l2 = ((topol *) tpl2)->likelihood;
    return  (l1 > l2) ? -1 : ((l1 == l2) ? 0 : 1);
  } /* cmpTplScore */



/*  Find an item in a sorted list of n items.  If the item is in the list,
 *  return its index.  If it is not in the list, return the negative of the
 *  position into which it should be inserted.
 */

int  findInList (item, list, n, cmpFunc)
    void  *item;
    void  *list[];
    int    n;
    int  (* cmpFunc)();
  { /* findInList */
    int  mid, hi, lo, cmp;

    if (n < 1) return  -1;                    /*  No match; first index  */

    lo = 1;
    mid = 0;
    hi = n;
    while (lo < hi) {
      mid = (lo + hi) >> 1;
      cmp = (* cmpFunc)(item, list[mid-1]);
      if (cmp) {
        if (cmp < 0) hi = mid;
        else lo = mid + 1;
        }
      else  return  mid;                        /*  Exact match  */
      }

    if (lo != mid) {
       cmp = (* cmpFunc)(item, list[lo-1]);
       if (cmp == 0) return lo;
       }
    if (cmp > 0) lo++;                         /*  Result of step = 0 test  */
    return  -lo;
  } /* findInList */



int  findTreeInList (bt, tr)
    bestlist  *bt;
    tree      *tr;
  { /* findTreeInList */
    topol  *tpl;

    tpl = bt->byScore[0];
    saveTree(tr, tpl);
    return  findInList((void *) tpl, (void **) (& (bt->byTopol[1])),
                       bt->nvalid, cmpTopol);
  } /* findTreeInList */


int  saveBestTree (bt, tr)
    bestlist  *bt;
    tree      *tr;
  { /* saveBestTree */
    double *tr_log_f, *tpl_log_f;
    topol  *tpl, *reuse;
    int  tplNum, scrNum, reuseScrNum, reuseTplNum, i, oldValid, newValid;

    tplNum = findTreeInList(bt, tr);
    tpl = bt->byScore[0];
    oldValid = newValid = bt->nvalid;

    if (tplNum > 0) {                      /* Topology is in list  */
      reuse = bt->byTopol[tplNum];         /* Matching topol  */
      reuseScrNum = reuse->scrNum;
      reuseTplNum = reuse->tplNum;
      }
                                           /* Good enough to keep? */
    else if (tr->likelihood < bt->worst)  return 0;

    else {                                 /* Topology is not in list */
      tplNum = -tplNum;                    /* Add to list (not replace) */
      if (newValid < bt->nkeep) bt->nvalid = ++newValid;
      reuseScrNum = newValid;              /* Take worst tree */
      reuse = bt->byScore[reuseScrNum];
      reuseTplNum = (newValid > oldValid) ? newValid : reuse->tplNum;
      if (tr->likelihood > bt->start->likelihood) bt->improved = TRUE;
      }

    scrNum = findInList((void *) tpl, (void **) (& (bt->byScore[1])),
                         oldValid, cmpTplScore);
    scrNum = ABS(scrNum);

    if (scrNum < reuseScrNum)
      for (i = reuseScrNum; i > scrNum; i--)
        (bt->byScore[i] = bt->byScore[i-1])->scrNum = i;

    else if (scrNum > reuseScrNum) {
      scrNum--;
      for (i = reuseScrNum; i < scrNum; i++)
        (bt->byScore[i] = bt->byScore[i+1])->scrNum = i;
      }

    if (tplNum < reuseTplNum)
      for (i = reuseTplNum; i > tplNum; i--)
        (bt->byTopol[i] = bt->byTopol[i-1])->tplNum = i;

    else if (tplNum > reuseTplNum) {
      tplNum--;
      for (i = reuseTplNum; i < tplNum; i++)
        (bt->byTopol[i] = bt->byTopol[i+1])->tplNum = i;
      }

    if (tpl_log_f = tpl->log_f) {
      tr_log_f  = tr->log_f;
      i = tpl->log_f_valid = tr->log_f_valid;
      while (--i >= 0)  *tpl_log_f++ = *tr_log_f++;
      }
    else {
      tpl->log_f_valid = 0;
      }

    tpl->scrNum = scrNum;
    tpl->tplNum = tplNum;
    bt->byTopol[tplNum] = bt->byScore[scrNum] = tpl;
    bt->byScore[0] = reuse;

    if (scrNum == 1)  bt->best = tr->likelihood;
    if (newValid == bt->nkeep) bt->worst = bt->byScore[newValid]->likelihood;

    return  scrNum;
  } /* saveBestTree */


int  startOpt (bt, tr)
    bestlist  *bt;
    tree      *tr;
  { /* startOpt */
    int  scrNum;

    scrNum = saveBestTree(bt, tr);
    copyTopol(bt->byScore[scrNum], bt->start);
    bt->improved = FALSE;
    return  scrNum;
  } /* startOpt */


int  setOptLevel (bt, opt_level)
    bestlist *bt;
    int       opt_level;
  { /* setOptLevel */
    int  tplNum, scrNum;

    tplNum = findInList((void *) bt->start, (void **) (&(bt->byTopol[1])),
                        bt->nvalid, cmpTopol);
    if (tplNum > 0) {
      bt->byTopol[tplNum]->opt_level = opt_level;
      scrNum = bt->byTopol[tplNum]->scrNum;
      }
    else {
      scrNum = 0;
      }

    return  scrNum;
  } /* setOptLevel */


int  recallBestTree (bt, rank, tr)
    bestlist  *bt;
    int        rank;
    tree      *tr;
  { /* recallBestTree */
    if (rank < 1)  rank = 1;
    if (rank > bt->nvalid)  rank = bt->nvalid;
    if (rank > 0)  if (! restoreTree(bt->byScore[rank], tr)) return FALSE;
    return  rank;
  } /* recallBestTree */


/*=======================================================================*/
/*                       End of best tree routines                       */
/*=======================================================================*/


#if 0
  void hang(msg) char *msg; {printf("Hanging around: %s\n", msg); while(1);}
#endif


boolean getnums (rdta)
    rawDNA   *rdta;
    /* input number of species, number of sites */
  { /* getnums */
    printf("\n%s, version %s, %s\n\n",
            programName,
            programVersion,
            programDate);
    printf("Based on Joseph Felsenstein's\n\n");
    printf("   Nucleic acid sequence Maximum Likelihood method, ");
    printf("version 3.3\n\n");

    if (fscanf(INFILE, "%d %d", & rdta->numsp, & rdta->sites) != 2) {
      printf("ERROR: Problem reading number of species and sites\n");
      return FALSE;
      }
    printf("%d Species, %d Sites\n\n", rdta->numsp, rdta->sites);

    if (rdta->numsp < 4) {
      printf("TOO FEW SPECIES\n");
      return FALSE;
      }

    if (rdta->sites < 1) {
      printf("TOO FEW SITES\n");
      return FALSE;
      }

    return TRUE;
  } /* getnums */


boolean digitchar (ch) int ch; {return (ch >= '0' && ch <= '9'); }


boolean whitechar (ch) int ch;
   { return (ch == ' ' || ch == '\n' || ch == '\t');
     }


void uppercase (chptr)
      int *chptr;
    /* convert character to upper case -- either ASCII or EBCDIC */
  { /* uppercase */
    int  ch;

    ch = *chptr;
    if ((ch >= 'a' && ch <= 'i') || (ch >= 'j' && ch <= 'r')
                                 || (ch >= 's' && ch <= 'z'))
      *chptr = ch + 'A' - 'a';
  } /* uppercase */


int base36 (ch)
    int ch;
  { /* base36 */
    if      (ch >= '0' && ch <= '9') return (ch - '0');
    else if (ch >= 'A' && ch <= 'I') return (ch - 'A' + 10);
    else if (ch >= 'J' && ch <= 'R') return (ch - 'J' + 19);
    else if (ch >= 'S' && ch <= 'Z') return (ch - 'S' + 28);
    else if (ch >= 'a' && ch <= 'i') return (ch - 'a' + 10);
    else if (ch >= 'j' && ch <= 'r') return (ch - 'j' + 19);
    else if (ch >= 's' && ch <= 'z') return (ch - 's' + 28);
    else return -1;
  } /* base36 */


int itobase36 (i)
    int  i;
  { /* itobase36 */
    if      (i <  0) return '?';
    else if (i < 10) return (i      + '0');
    else if (i < 19) return (i - 10 + 'A');
    else if (i < 28) return (i - 19 + 'J');
    else if (i < 36) return (i - 28 + 'S');
    else return '?';
  } /* itobase36 */


int findch (c)
    int  c;
  { /* findch */
    int ch;

    while ((ch = getc(INFILE)) != EOF && ch != c) ;
    return  ch;
  } /* findch */


#if Master || Slave
int str_findch (treestrp, c)
    char **treestrp;
    int  c;
  { /* str_findch */
    int ch;

    while ((ch = *(*treestrp)++) != NULL && ch != c) ;
    return  ch;
  } /* str_findch */
#endif


boolean inputboot(adef)
    analdef      *adef;
    /* read the bootstrap auxilliary info */
  { /* inputboot */
    if (! adef->boot) {
      printf("ERROR: Unexpected Bootstrap auxiliary data line\n");
      return FALSE;
      }
    else if (fscanf(INFILE, "%ld", & adef->boot) != 1 ||
             findch('\n') == EOF) {
      printf("ERROR: Problem reading boostrap random seed value\n");
      return FALSE;
      }

    return TRUE;
  } /* inputboot */


boolean inputcategories (rdta)
    rawDNA       *rdta;
    /* read the category rates and the categories for each site */
  { /* inputcategories */
    int  i, j, ch, ci;

    if (rdta->categs >= 0) {
      printf("ERROR: Unexpected Categories auxiliary data line\n");
      return FALSE;
      }
    if (fscanf(INFILE, "%d", & rdta->categs) != 1) {
      printf("ERROR: Problem reading number of rate categories\n");
      return FALSE;
      }
    if (rdta->categs < 1 || rdta->categs > maxcategories) {
      printf("ERROR: Bad number of categories: %d\n", rdta->categs);
      printf("Must be in range 1 - %d\n", maxcategories);
      return FALSE;
      }

    for (j = 1; j <= rdta->categs
           && fscanf(INFILE, "%lf", &(rdta->catrat[j])) == 1; j++) ;

    if ((j <= rdta->categs) || (findch('\n') == EOF)) {
      printf("ERROR: Problem reading rate values\n");
      return FALSE;
      }

    for (i = 1; i <= nmlngth; i++)  (void) getc(INFILE);

    i = 1;
    while (i <= rdta->sites) {
      ch = getc(INFILE);
      ci = base36(ch);
      if (ci >= 0 && ci <= rdta->categs)
        rdta->sitecat[i++] = ci;
      else if (! whitechar(ch)) {
        printf("ERROR: Bad category character (%c) at site %d\n", ch, i);
        return FALSE;
        }
      }

    if (findch('\n') == EOF) {      /* skip to end of line */
      printf("ERROR: Missing newline at end of category data\n");
      return FALSE;
      }

    return TRUE;
  } /* inputcategories */


boolean inputextra (adef)
    analdef  *adef;
  { /* inputextra */
    if (fscanf(INFILE,"%d", & adef->extra) != 1 ||
        findch('\n') == EOF) {
      printf("ERROR: Problem reading extra info value\n");
      return FALSE;
      }

    return TRUE;
  } /* inputextra */


boolean inputfreqs (rdta)
    rawDNA   *rdta;
  { /* inputfreqs */
    if (fscanf(INFILE, "%lf%lf%lf%lf",
               & rdta->freqa, & rdta->freqc,
               & rdta->freqg, & rdta->freqt) != 4 ||
        findch('\n') == EOF) {
      printf("ERROR: Problem reading user base frequencies data\n");
      return  FALSE;
      }

    rdta->freqread = TRUE;
    return TRUE;
  } /* inputfreqs */


boolean inputglobal (tr)
    tree     *tr;
    /* input the global option information */
  { /* inputglobal */
    int  ch;

    if (tr->global != -2) {
      printf("ERROR: Unexpected Global auxiliary data line\n");
      return FALSE;
      }
    if (fscanf(INFILE, "%d", &(tr->global)) != 1) {
      printf("ERROR: Problem reading rearrangement region size\n");
      return FALSE;
      }
    if (tr->global < 0) {
      printf("WARNING: Global region size too small;\n");
      printf("         value reset to local\n\n");
      tr->global = 1;
      }
    else if (tr->global == 0)  tr->partswap = 0;
    else if (tr->global > tr->mxtips - 3) {
      tr->global = tr->mxtips - 3;
      }

    while ((ch = getc(INFILE)) != '\n') {  /* Scan for second value */
      if (! whitechar(ch)) {
        if (ch != EOF)  (void) ungetc(ch, INFILE);
        if (ch == EOF || fscanf(INFILE, "%d", &(tr->partswap)) != 1
                      || findch('\n') == EOF) {
          printf("ERROR: Problem reading insert swap region size\n");
          return FALSE;
          }
        else if (tr->partswap < 0)  tr->partswap = 1;
        else if (tr->partswap > tr->mxtips - 3) {
          tr->partswap = tr->mxtips - 3;
          }

        if (tr->partswap > tr->global)  tr->global = tr->partswap;
        break;   /*  Break while loop */
        }
      }

    return TRUE;
  } /* inputglobal */


boolean inputjumble (adef)
    analdef  *adef;
  { /* inputjumble */
    if (! adef->jumble) {
      printf("ERROR: Unexpected Jumble auxiliary data line\n");
      return FALSE;
      }
    else if (fscanf(INFILE, "%ld", & adef->jumble) != 1 ||
             findch('\n') == EOF) {
      printf("ERROR: Problem reading jumble random seed value\n");
      return FALSE;
      }
    else if (adef->jumble == 0) {
      printf("WARNING: Jumble random number seed is zero\n\n");
      }

    return TRUE;
  } /* inputjumble */


boolean inputkeep (adef)
    analdef  *adef;
  { /* inputkeep */
    if (fscanf(INFILE, "%d", & adef->nkeep) != 1 ||
        findch('\n') == EOF || adef->nkeep < 1) {
      printf("ERROR: Problem reading number of kept trees\n");
      return FALSE;
      }

    return TRUE;
  } /* inputkeep */


boolean inputoutgroup (adef, tr)
    analdef  *adef;
    tree     *tr;
  { /* inputoutgroup */
    if (! adef->root || tr->outgr > 0) {
      printf("ERROR: Unexpected Outgroup auxiliary data line\n");
      return FALSE;
      }
    else if (fscanf(INFILE, "%d", &(tr->outgr)) != 1 ||
             findch('\n') == EOF) {
      printf("ERROR: Problem reading outgroup number\n");
      return FALSE;
      }
    else if ((tr->outgr < 1) || (tr->outgr > tr->mxtips)) {
      printf("ERROR: Bad outgroup: '%d'\n", tr->outgr);
      return FALSE;
      }

    return TRUE;
  } /* inputoutgroup */


boolean inputratio (rdta)
    rawDNA   *rdta;
  { /* inputratio */
    if (rdta->ttratio >= 0.0) {
      printf("ERROR: Unexpected Transition/transversion auxiliary data\n");
      return FALSE;
      }
    else if (fscanf(INFILE,"%lf", & rdta->ttratio)!=1 ||
             findch('\n') == EOF) {
      printf("ERROR: Problem reading transition/transversion ratio\n");
      return FALSE;
      }

    return TRUE;
  } /* inputratio */


/*  Y 0 is treeNone   (no tree)
    Y 1 is treeNewick
    Y 2 is treeProlog
    Y 3 is treePHYLIP
 */

boolean inputtreeopt (adef)
    analdef  *adef;
  { /* inputtreeopt */
    if (! adef->trout) {
      printf("ERROR: Unexpected Treefile auxiliary data\n");
      return FALSE;
      }
    else if (fscanf(INFILE,"%d", & adef->trout) != 1 ||
             findch('\n') == EOF) {
      printf("ERROR: Problem reading output tree-type number\n");
      return FALSE;
      }
    else if ((adef->trout < 0) || (adef->trout > treeMaxType)) {
      printf("ERROR: Bad output tree-type number: '%d'\n", adef->trout);
      return FALSE;
      }

    return TRUE;
  } /* inputtreeopt */


boolean inputweights (adef, rdta, cdta)
    analdef      *adef;
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    /* input the character weights 0, 1, 2 ... 9, A, B, ... Y, Z */
  { /* inputweights */
    int i, ch, wi;

    if (! adef->userwgt || cdta->wgtsum > 0) {
      printf("ERROR: Unexpected Weights auxiliary data\n");
      return FALSE;
      }

    for (i = 2; i <= nmlngth; i++)  (void) getc(INFILE);
    cdta->wgtsum = 0;
    i = 1;
    while (i <= rdta->sites) {
      ch = getc(INFILE);
      wi = base36(ch);
      if (wi >= 0)
        cdta->wgtsum += rdta->wgt[i++] = wi;
      else if (! whitechar(ch)) {
        printf("ERROR: Bad weight character: '%c'", ch);
        printf("       Weights in dnaml must be a digit or a letter.\n");
        return FALSE;
        }
      }

    if (findch('\n') == EOF) {      /* skip to end of line */
      printf("ERROR: Missing newline at end of weight data\n");
      return FALSE;
      }

    return TRUE;
  } /* inputweights */


boolean getoptions (adef, rdta, cdta, tr)
    analdef      *adef;
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    tree         *tr;
  { /* getoptions */
    int     ch, i, extranum;

    adef->boot    =           0;  /* Don't bootstrap column weights */
    adef->empf    =        TRUE;  /* Use empirical base frequencies */
    adef->extra   =           0;  /* No extra runtime info unless requested */
    adef->interleaved =    TRUE;  /* By default, data format is interleaved */
    adef->jumble  =       FALSE;  /* Use random addition sequence */
    adef->nkeep   =           0;  /* Keep only the one best tree */
    adef->prdata  =       FALSE;  /* Don't echo data to output stream */
    adef->qadd    =        TRUE;  /* Smooth branches globally in add */
    adef->restart =       FALSE;  /* Restart from user tree */
    adef->root    =       FALSE;  /* User-defined outgroup rooting */
    adef->trout   = treeDefType;  /* Output tree file */
    adef->trprint =        TRUE;  /* Print tree to output stream */
    rdta->categs  =           0;  /* No rate categories */
    rdta->catrat[1] =       1.0;  /* Rate values */
    rdta->freqread =      FALSE;  /* User-defined frequencies not read yet */
    rdta->ttratio =         2.0;  /* Transition/transversion rate ratio */
    tr->global    =          -1;  /* Default search locale for optimum */
    tr->mxtips    = rdta->numsp;
    tr->outgr     =           1;  /* Outgroup number */
    tr->partswap  =           1;  /* Default to swap locally after insert */
    tr->userlen   =       FALSE;  /* User-supplied branch lengths */
    adef->usertree =      FALSE;  /* User-defined tree topologies */
    adef->userwgt =       FALSE;  /* User-defined position weights */
    extranum      =           0;

    while ((ch = getc(INFILE)) != '\n' && ch != EOF) {
      uppercase(& ch);
      switch (ch) {
          case '1' : adef->prdata  = ! adef->prdata; break;
          case '3' : adef->trprint = ! adef->trprint; break;
          case '4' : adef->trout   = treeDefType - adef->trout; break;
          case 'B' : adef->boot    =  1; extranum++; break;
          case 'C' : rdta->categs  = -1; extranum++; break;
          case 'E' : adef->extra   = -1; break;
          case 'F' : adef->empf    = ! adef->empf; break;
          case 'G' : tr->global    = -2; break;
          case 'I' : adef->interleaved = ! adef->interleaved; break;
          case 'J' : adef->jumble  = 1; extranum++; break;
          case 'K' : extranum++; break;
          case 'L' : tr->userlen   = TRUE; break;
          case 'O' : adef->root    = TRUE; tr->outgr = 0; extranum++; break;
          case 'Q' : adef->qadd    = FALSE; break;
          case 'R' : adef->restart = TRUE; break;
          case 'T' : rdta->ttratio = -1.0; extranum++; break;
          case 'U' : adef->usertree = TRUE; break;
          case 'W' : adef->userwgt = TRUE; cdta->wgtsum = 0; extranum++; break;
          case 'Y' : adef->trout   = treeDefType - adef->trout; break;
          case ' ' : break;
          case '\t': break;
          default  :
              printf("ERROR: Bad option character: '%c'\n", ch);
              return FALSE;
          }
      }

    if (ch == EOF) {
      printf("ERROR: End-of-file in options list\n");
      return FALSE;
      }

    if (adef->usertree && adef->restart) {
      printf("ERROR:  The restart and user-tree options conflict:\n");
      printf("        Restart adds rest of taxa to a starting tree;\n");
      printf("        User-tree does not add any taxa.\n\n");
      return FALSE;
      }

    if (adef->usertree && adef->jumble) {
      printf("WARNING:  The jumble and user-tree options conflict:\n");
      printf("          Jumble adds taxa to a tree in random order;\n");
      printf("          User-tree does not use taxa addition.\n");
      printf("          Jumble option cancelled for this run.\n\n");
      adef->jumble = FALSE;
      }

    if (tr->userlen && tr->global != -1) {
      printf("ERROR:  The global and user-lengths options conflict:\n");
      printf("        Global optimizes a starting tree;\n");
      printf("        User-lengths constrain the starting tree.\n\n");
      return FALSE;
      }

    if (tr->userlen && ! adef->usertree) {
      printf("WARNING:  User lengths required user tree option.\n");
      printf("          User-tree option set for this run.\n\n");
      adef->usertree = TRUE;
      }

    rdta->wgt      = (int *)    Malloc((rdta->sites + 1) * sizeof(int));
    rdta->wgt2     = (int *)    Malloc((rdta->sites + 1) * sizeof(int));
    rdta->sitecat  = (int *)    Malloc((rdta->sites + 1) * sizeof(int));
    cdta->alias    = (int *)    Malloc((rdta->sites + 1) * sizeof(int));
    cdta->aliaswgt = (int *)    Malloc((rdta->sites + 1) * sizeof(int));
    cdta->patcat   = (int *)    Malloc((rdta->sites + 1) * sizeof(int));
    cdta->patrat   = (double *) Malloc((rdta->sites + 1) * sizeof(double));
    cdta->wr       = (double *) Malloc((rdta->sites + 1) * sizeof(double));
    cdta->wr2      = (double *) Malloc((rdta->sites + 1) * sizeof(double));
    if ( ! rdta->wgt || ! rdta->wgt2     || ! rdta->sitecat || ! cdta->alias
                     || ! cdta->aliaswgt || ! cdta->patcat  || ! cdta->patrat
                     || ! cdta->wr       || ! cdta->wr2) {
      fprintf(stderr, "getoptions: Malloc failure\n");
      return 0;
      }

    /*  process lines with auxiliary data */

    while (extranum--) {
      ch = getc(INFILE);
      uppercase(& ch);
      switch (ch) {
        case 'B':  if (! inputboot(adef)) return FALSE; break;
        case 'C':  if (! inputcategories(rdta)) return FALSE; break;
        case 'E':  if (! inputextra(adef)) return FALSE; extranum++; break;
        case 'F':  if (! inputfreqs(rdta)) return FALSE; break;
        case 'G':  if (! inputglobal(tr)) return FALSE; extranum++; break;
        case 'J':  if (! inputjumble(adef)) return FALSE; break;
        case 'K':  if (! inputkeep(adef)) return FALSE; break;
        case 'O':  if (! inputoutgroup(adef, tr)) return FALSE; break;
        case 'T':  if (! inputratio(rdta)) return FALSE; break;
        case 'W':  if (! inputweights(adef, rdta, cdta)) return FALSE; break;
        case 'Y':  if (! inputtreeopt(adef)) return FALSE; extranum++; break;

        default:
            printf("ERROR: Auxiliary options line starts with '%c'\n", ch);
            return FALSE;
        }
      }

    if (! adef->userwgt) {
      for (i = 1; i <= rdta->sites; i++) rdta->wgt[i] = 1;
      cdta->wgtsum = rdta->sites;
      }

    if (adef->userwgt && cdta->wgtsum < 1) {
      printf("ERROR:  Missing or bad user-supplied weight data.\n");
      return FALSE;
      }

    if (adef->boot) {
      printf("Bootstrap random number seed = %ld\n\n", adef->boot);
      }

    if (adef->jumble) {
      printf("Jumble random number seed = %ld\n\n", adef->jumble);
      }

    if (adef->qadd) {
      printf("Quick add (only local branches initially optimized) in effect\n\n");
      }

    if (rdta->categs > 0) {
      printf("Site category   Rate of change\n\n");
      for (i = 1; i <= rdta->categs; i++)
        printf("           %c%13.3f\n", itobase36(i), rdta->catrat[i]);
      putchar('\n');
      for (i = 1; i <= rdta->sites; i++) {
        if ((rdta->wgt[i] > 0) && (rdta->sitecat[i] < 1)) {
          printf("ERROR: Bad category (%c) at site %d\n",
                  itobase36(rdta->sitecat[i]), i);
          return FALSE;
          }
        }
      }
    else if (rdta->categs < 0) {
      printf("ERROR: Category auxiliary data missing from input\n");
      return FALSE;
      }
    else {                                        /* rdta->categs == 0 */
      for (i = 1; i <= rdta->sites; i++) rdta->sitecat[i] = 1;
      rdta->categs = 1;
      }

    if (tr->outgr < 1) {
      printf("ERROR: Outgroup auxiliary data missing from input\n");
      return FALSE;
      }

    if (rdta->ttratio < 0.0) {
      printf("ERROR: Transition/transversion auxiliary data missing from input\n");
      return FALSE;
      }

    if (tr->global < 0) {
      if (tr->global == -2) tr->global = tr->mxtips - 3;  /* Default global */
      else                  tr->global = adef->usertree ? 0 : 1;/* No global */
      }

    if (adef->restart) {
      printf("Restart option in effect.  ");
      printf("Sequence addition will start from appended tree.\n\n");
      }

    if (adef->usertree && ! tr->global) {
      printf("User-supplied tree topology%swill be used.\n\n",
        tr->userlen ? " and branch lengths " : " ");
      }
    else {
      if (! adef->usertree) {
        printf("Rearrangements of partial trees may cross %d %s.\n",
               tr->partswap, tr->partswap == 1 ? "branch" : "branches");
        }
      printf("Rearrangements of full tree may cross %d %s.\n\n",
             tr->global, tr->global == 1 ? "branch" : "branches");
      }

    if (! adef->usertree && adef->nkeep == 0) adef->nkeep = 1;

    return TRUE;
  } /* getoptions */


boolean getbasefreqs (rdta)
    rawDNA   *rdta;
  { /* getbasefreqs */
    int  ch;

    if (rdta->freqread) return TRUE;

    ch = getc(INFILE);
    if (! ((ch == 'F') || (ch == 'f')))  (void) ungetc(ch, INFILE);

    if (fscanf(INFILE, "%lf%lf%lf%lf",
               & rdta->freqa, & rdta->freqc,
               & rdta->freqg, & rdta->freqt) != 4 ||
        findch('\n') == EOF) {
      printf("ERROR: Problem reading user base frequencies\n");
      return  FALSE;
      }

    return TRUE;
  } /* getbasefreqs */


boolean getyspace (rdta)
    rawDNA   *rdta;
  { /* getyspace */
    long   size;
    int    i;
    yType *y0;

    if (! (rdta->y = (yType **) Malloc((rdta->numsp + 1) * sizeof(yType *)))) {
      printf("ERROR: Unable to obtain space for data array pointers\n");
      return  FALSE;
      }

    size = 4 * (rdta->sites / 4 + 1);
    if (! (y0 = (yType *) Malloc((rdta->numsp + 1) * size * sizeof(yType)))) {
      printf("ERROR: Unable to obtain space for data array\n");
      return  FALSE;
      }

    for (i = 0; i <= rdta->numsp; i++) {
      rdta->y[i] = y0;
      y0 += size;
      }

    return  TRUE;
  } /* getyspace */


boolean setupTree (tr, nsites)
    tree  *tr;
    int    nsites;
  { /* setupTree */
    nodeptr  p0, p, q;
    int      i, j, tips, inter;

    tips  = tr->mxtips;
    inter = tr->mxtips - 1;

    if (!(p0 = (nodeptr) Malloc((tips + 3*inter) * sizeof(node)))) {
      printf("ERROR: Unable to obtain sufficient tree memory\n");
      return  FALSE;
      }

    if (!(tr->nodep = (nodeptr *) Malloc((2*tr->mxtips) * sizeof(nodeptr)))) {
      printf("ERROR: Unable to obtain sufficient tree memory, too\n");
      return  FALSE;
      }

    tr->nodep[0] = (node *) NULL;    /* Use as 1-based array */

    for (i = 1; i <= tips; i++) {    /* Set-up tips */
      p = p0++;
      p->x      = (xarray *) NULL;
      p->tip    = (yType *) NULL;
      p->number = i;
      p->next   = p;
      p->back   = (node *) NULL;

      tr->nodep[i] = p;
      }

    for (i = tips + 1; i <= tips + inter; i++) { /* Internal nodes */
      q = (node *) NULL;
      for (j = 1; j <= 3; j++) {
        p = p0++;
        p->x      = (xarray *) NULL;
        p->tip    = (yType *) NULL;
        p->number = i;
        p->next   = q;
        p->back   = (node *) NULL;
        q = p;
        }
      p->next->next->next = p;
      tr->nodep[i] = p;
      }

    tr->likelihood  = unlikely;
    tr->start       = (node *) NULL;
    tr->outgrnode   = tr->nodep[tr->outgr];
    tr->ntips       = 0;
    tr->nextnode    = 0;
    tr->opt_level   = 0;
    tr->prelabeled  = TRUE;
    tr->smoothed    = FALSE;
    tr->log_f_valid = 0;

    tr->log_f = (double *) Malloc(nsites * sizeof(double));
    if (! tr->log_f) {
      printf("ERROR: Unable to obtain sufficient tree memory, trey\n");
      return  FALSE;
      }

    return TRUE;
  } /* setupTree */


void freeTreeNode (p)       /* Free tree node (sector) associated data */
    nodeptr  p;
  { /* freeTreeNode */
    if (p) {
      if (p->x) {
        if (p->x->a) Free(p->x->a);
        Free(p->x);
        }
      }
  } /* freeTreeNode */


void freeTree (tr)
    tree  *tr;
  { /* freeTree */
    nodeptr  p, q;
    int  i, tips, inter;

    tips  = tr->mxtips;
    inter = tr->mxtips - 1;

    for (i = 1; i <= tips; i++) freeTreeNode(tr->nodep[i]);

    for (i = tips + 1; i <= tips + inter; i++) {
      if (p = tr->nodep[i]) {
        if (q = p->next) {
          freeTreeNode(q->next);
          freeTreeNode(q);
          }
        freeTreeNode(p);
        }
      }

    Free(tr->nodep[1]);       /* Free the actual nodes */
  } /* freeTree */


boolean getdata (adef, rdta, tr)
    analdef  *adef;
    rawDNA   *rdta;
    tree     *tr;
    /* read sequences */
  { /* getdata */
    int   i, j, k, l, basesread, basesnew, ch;
    int   meaning[256];          /*  meaning of input characters */ 
    char *nameptr;
    boolean  allread, firstpass;

    for (i = 0; i <= 255; i++) meaning[i] = 0;
    meaning['A'] =  1;
    meaning['B'] = 14;
    meaning['C'] =  2;
    meaning['D'] = 13;
    meaning['G'] =  4;
    meaning['H'] = 11;
    meaning['K'] = 12;
    meaning['M'] =  3;
    meaning['N'] = 15;
    meaning['O'] = 15;
    meaning['R'] =  5;
    meaning['S'] =  6;
    meaning['T'] =  8;
    meaning['U'] =  8;
    meaning['V'] =  7;
    meaning['W'] =  9;
    meaning['X'] = 15;
    meaning['Y'] = 10;
    meaning['?'] = 15;
    meaning['-'] = 15;

    basesread = basesnew = 0;

    allread = FALSE;
    firstpass = TRUE;
    ch = ' ';

    while (! allread) {
      for (i = 1; i <= tr->mxtips; i++) {     /*  Read data line */

        if (firstpass) {                      /*  Read species names */
          j = 1;
          while (whitechar(ch = getc(INFILE))) {  /*  Skip blank lines */
            if (ch == '\n')  j = 1;  else  j++;
            }

          if (j > nmlngth) {
            printf("ERROR: Blank name for species %d; ", i);
            printf("check number of species,\n");
            printf("       number of sites, and interleave option.\n");
            return  FALSE;
            }

          nameptr = tr->nodep[i]->name;
          for (k = 1; k < j; k++)  *nameptr++ = ' ';

          while (ch != '\n' && ch != EOF) {
            if (whitechar(ch))  ch = ' ';
            *nameptr++ = ch;
            if (++j > nmlngth) break;
            ch = getc(INFILE);
            }

          while (*(--nameptr) == ' ') ;          /*  remove trailing blanks */
          *(++nameptr) = '\0';                   /*  add null termination */

          if (ch == EOF) {
            printf("ERROR: End-of-file in name of species %d\n", i);
            return  FALSE;
            }
          }    /* if (firstpass) */

        j = basesread;
        while ((j < rdta->sites)
               && ((ch = getc(INFILE)) != EOF)
               && ((! adef->interleaved) || (ch != '\n'))) {
          uppercase(& ch);
          if (meaning[ch] || ch == '.') {
            j++;
            if (ch == '.') {
              if (i != 1) ch = rdta->y[1][j];
              else {
                printf("ERROR: Dot (.) found at site %d of sequence 1\n", j);
                return  FALSE;
                }
              }
            rdta->y[i][j] = ch;
            }
          else if (whitechar(ch) || digitchar(ch)) ;
          else {
            printf("ERROR: Bad base (%c) at site %d of sequence %d\n",
                    ch, j, i);
            return  FALSE;
            }
          }

        if (ch == EOF) {
          printf("ERROR: End-of-file at site %d of sequence %d\n", j, i);
          return  FALSE;
          }

        if (! firstpass && (j == basesread)) i--;        /* no data on line */
        else if (i == 1) basesnew = j;
        else if (j != basesnew) {
          printf("ERROR: Sequences out of alignment\n");
          printf("%d (instead of %d) residues read in sequence %d\n",
                  j - basesread, basesnew - basesread, i);
          return  FALSE;
          }

        while (ch != '\n' && ch != EOF) ch = getc(INFILE);  /* flush line */
        }                                                  /* next sequence */
      firstpass = FALSE;
      basesread = basesnew;
      allread = (basesread >= rdta->sites);
      }

    /*  Print listing of sequence alignment */

    if (adef->prdata) {
      j = nmlngth - 5 + ((rdta->sites + ((rdta->sites - 1)/10))/2);
      if (j < nmlngth - 1) j = nmlngth - 1;
      if (j > 37) j = 37;
      printf("Name"); for (i=1;i<=j;i++) putchar(' '); printf("Sequences\n");
      printf("----"); for (i=1;i<=j;i++) putchar(' '); printf("---------\n");
      putchar('\n');

      for (i = 1; i <= rdta->sites; i += 60) {
        l = i + 59;
        if (l > rdta->sites) l = rdta->sites;

        if (adef->userwgt) {
          printf("Weights   ");
          for (j = 11; j <= nmlngth+3; j++) putchar(' ');
          for (k = i; k <= l; k++) {
            putchar(itobase36(rdta->wgt[k]));
            if (((k % 10) == 0) && (k < l)) putchar(' ');
            }
          putchar('\n');
          }

        if (rdta->categs > 1) {
          printf("Categories");
          for (j = 11; j <= nmlngth+3; j++) putchar(' ');
          for (k = i; k <= l; k++) {
            putchar(itobase36(rdta->sitecat[k]));
            if (((k % 10) == 0) && (k < l)) putchar(' ');
            }
          putchar('\n');
          }

        for (j = 1; j <= tr->mxtips; j++) {
          nameptr = tr->nodep[j]->name;
          k = nmlngth+3;
          while (ch = *nameptr++) {putchar(ch); k--;}
          while (--k >= 0) putchar(' ');

          for (k = i; k <= l; k++) {
            ch = rdta->y[j][k];
            if ((j > 1) && (ch == rdta->y[1][k])) ch = '.';
            putchar(ch);
            if (((k % 10) == 0) && (k < l)) putchar(' ');
            }
          putchar('\n');
          }
        putchar('\n');
        }
      }

    for (j = 1; j <= tr->mxtips; j++)    /* Convert characters to meanings */
      for (i = 1; i <= rdta->sites; i++) {
        rdta->y[j][i] = meaning[rdta->y[j][i]];
        }

    return  TRUE;
  } /* getdata */


boolean  getntrees (adef)
    analdef  *adef;
  { /* getntrees */

    if (fscanf(INFILE, "%d", &(adef->numutrees)) != 1 || findch('\n') == EOF) {
      printf("ERROR: Problem reading number of user trees\n");
      return  FALSE;
      }

    if (adef->nkeep == 0) adef->nkeep = adef->numutrees;

    return  TRUE;
  } /* getntrees */


boolean getinput (adef, rdta, cdta, tr)
    analdef      *adef;
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    tree         *tr;
  { /* getinput */
    if (! getnums(rdta))                       return FALSE;
    if (! getoptions(adef, rdta, cdta, tr))    return FALSE;
    if (! adef->empf && ! getbasefreqs(rdta))  return FALSE;
    if (! getyspace(rdta))                     return FALSE;
    if (! setupTree(tr, rdta->sites))          return FALSE;
    if (! getdata(adef, rdta, tr))             return FALSE;
    if (adef->usertree && ! getntrees(adef))   return FALSE;

    return TRUE;
  } /* getinput */


void makeboot (adef, rdta, cdta)
    analdef      *adef;
    rawDNA       *rdta;
    crunchedDNA  *cdta;
  { /* makeboot */
    int  i, j, nonzero;
    double  randum();

    nonzero = 0;
    for (i = 1; i <= rdta->sites; i++)  if (rdta->wgt[i] > 0) nonzero++;

    for (j = 1; j <= nonzero; j++) cdta->aliaswgt[j] = 0;
    for (j = 1; j <= nonzero; j++)
      cdta->aliaswgt[(int) (nonzero*randum(& adef->boot)) + 1]++;

    j = 0;
    cdta->wgtsum = 0;
    for (i = 1; i <= rdta->sites; i++) {
      if (rdta->wgt[i] > 0)
        cdta->wgtsum += (rdta->wgt2[i] = rdta->wgt[i] * cdta->aliaswgt[++j]);
      else
        rdta->wgt2[i] = 0;
      }
  } /* makeboot */


void sitesort (rdta, cdta)
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    /* Shell sort keeping sites, weights in same order */
  { /* sitesort */
    int  gap, i, j, jj, jg, k;
    boolean  flip, tied;

    for (gap = rdta->sites / 2; gap > 0; gap /= 2) {
      for (i = gap + 1; i <= rdta->sites; i++) {
        j = i - gap;

        do {
          jj = cdta->alias[j];
          jg = cdta->alias[j+gap];
          flip = (rdta->sitecat[jj] >  rdta->sitecat[jg]);
          tied = (rdta->sitecat[jj] == rdta->sitecat[jg]);
          for (k = 1; (k <= rdta->numsp) && tied; k++) {
            flip = (rdta->y[k][jj] >  rdta->y[k][jg]);
            tied = (rdta->y[k][jj] == rdta->y[k][jg]);
            }
          if (flip) {
            cdta->alias[j]     = jg;
            cdta->alias[j+gap] = jj;
            j -= gap;
            }
          } while (flip && (j > 0));

        }  /* for (i ...   */
      }    /* for (gap ... */
  } /* sitesort */


void sitecombcrunch (rdta, cdta)
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    /* combine sites that have identical patterns (and nonzero weight) */
  { /* sitecombcrunch */
    int  i, sitei, j, sitej, k;
    boolean  tied;

    i = 0;
    cdta->alias[0] = cdta->alias[1];
    cdta->aliaswgt[0] = 0;

    for (j = 1; j <= rdta->sites; j++) {
      sitei = cdta->alias[i];
      sitej = cdta->alias[j];
      tied = (rdta->sitecat[sitei] == rdta->sitecat[sitej]);

      for (k = 1; tied && (k <= rdta->numsp); k++)
        tied = (rdta->y[k][sitei] == rdta->y[k][sitej]);

      if (tied) {
        cdta->aliaswgt[i] += rdta->wgt2[sitej];
        }
      else {
        if (cdta->aliaswgt[i] > 0) i++;
        cdta->aliaswgt[i] = rdta->wgt2[sitej];
        cdta->alias[i] = sitej;
        }
      }

    cdta->endsite = i;
    if (cdta->aliaswgt[i] > 0) cdta->endsite++;
  } /* sitecombcrunch */


boolean makeweights (adef, rdta, cdta)
    analdef      *adef;
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    /* make up weights vector to avoid duplicate computations */
  { /* makeweights */
    int  i;

    if (adef->boot)  makeboot(adef, rdta, cdta);
    else  for (i = 1; i <= rdta->sites; i++)  rdta->wgt2[i] = rdta->wgt[i];

    for (i = 1; i <= rdta->sites; i++)  cdta->alias[i] = i;
    sitesort(rdta, cdta);
    sitecombcrunch(rdta, cdta);

    printf("Total weight of positions in analysis = %d\n", cdta->wgtsum);
    printf("There are %d distinct data patterns (columns)\n\n", cdta->endsite);

    return TRUE;
  } /* makeweights */


boolean makevalues (rdta, cdta)
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    /* set up fractional likelihoods at tips */
  { /* makevalues */
    double  temp, wtemp;
    int  i, j;

    for (i = 1; i <= rdta->numsp; i++) {    /* Pack and move tip data */
      for (j = 0; j < cdta->endsite; j++) {
        rdta->y[i-1][j] = rdta->y[i][cdta->alias[j]];
        }
      }

    for (j = 0; j < cdta->endsite; j++) {
      cdta->patcat[j] = i = rdta->sitecat[cdta->alias[j]];
      cdta->patrat[j] = temp = rdta->catrat[i];
      cdta->wr[j]  = wtemp = temp * cdta->aliaswgt[j];
      cdta->wr2[j] = temp * wtemp;
      }

    return TRUE;
  } /* makevalues */


boolean empiricalfreqs (rdta, cdta)
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    /* Get empirical base frequencies from the data */
  { /* empiricalfreqs */
    double  sum, suma, sumc, sumg, sumt, wj, fa, fc, fg, ft;
    int     i, j, k, code;
    yType  *yptr;

    rdta->freqa = 0.25;
    rdta->freqc = 0.25;
    rdta->freqg = 0.25;
    rdta->freqt = 0.25;
    for (k = 1; k <= 8; k++) {
      suma = 0.0;
      sumc = 0.0;
      sumg = 0.0;
      sumt = 0.0;
      for (i = 0; i < rdta->numsp; i++) {
        yptr = rdta->y[i];
        for (j = 0; j < cdta->endsite; j++) {
          code = *yptr++;
          fa = rdta->freqa * ( code       & 1);
          fc = rdta->freqc * ((code >> 1) & 1);
          fg = rdta->freqg * ((code >> 2) & 1);
          ft = rdta->freqt * ((code >> 3) & 1);
          wj = cdta->aliaswgt[j] / (fa + fc + fg + ft);
          suma += wj * fa;
          sumc += wj * fc;
          sumg += wj * fg;
          sumt += wj * ft;
          }
        }
      sum = suma + sumc + sumg + sumt;
      rdta->freqa = suma / sum;
      rdta->freqc = sumc / sum;
      rdta->freqg = sumg / sum;
      rdta->freqt = sumt / sum;
      }

    return TRUE;
  } /* empiricalfreqs */


void reportfreqs (adef, rdta)
    analdef      *adef;
    rawDNA       *rdta;
  { /* reportfreqs */
    double  suma, sumb;

    if (adef->empf) printf("Empirical ");
    printf("Base Frequencies:\n\n");
    printf("   A    %10.5f\n",   rdta->freqa);
    printf("   C    %10.5f\n",   rdta->freqc);
    printf("   G    %10.5f\n",   rdta->freqg);
    printf("  T(U)  %10.5f\n\n", rdta->freqt);
    rdta->freqr = rdta->freqa + rdta->freqg;
    rdta->invfreqr = 1.0/rdta->freqr;
    rdta->freqar = rdta->freqa * rdta->invfreqr;
    rdta->freqgr = rdta->freqg * rdta->invfreqr;
    rdta->freqy = rdta->freqc + rdta->freqt;
    rdta->invfreqy = 1.0/rdta->freqy;
    rdta->freqcy = rdta->freqc * rdta->invfreqy;
    rdta->freqty = rdta->freqt * rdta->invfreqy;
    printf("Transition/transversion ratio = %10.6f\n\n", rdta->ttratio);
    suma = rdta->ttratio*rdta->freqr*rdta->freqy
         - (rdta->freqa*rdta->freqg + rdta->freqc*rdta->freqt);
    sumb = rdta->freqa*rdta->freqgr + rdta->freqc*rdta->freqty;
    rdta->xi = suma/(suma+sumb);
    rdta->xv = 1.0 - rdta->xi;
    if (rdta->xi <= 0.0) {
      printf("WARNING: This transition/transversion ratio\n");
      printf("         is impossible with these base frequencies!\n");
      printf("Transition/transversion parameter reset\n\n");
      rdta->xi = 0.000001;
      rdta->xv = 1.0 - rdta->xi;
      rdta->ttratio = (sumb * rdta->xi / rdta->xv 
                       + rdta->freqa * rdta->freqg
                       + rdta->freqc * rdta->freqt)
                    / (rdta->freqr * rdta->freqy);
      printf("Transition/transversion ratio = %10.6f\n\n", rdta->ttratio);
      }
    printf("(Transition/transversion parameter = %10.6f)\n\n",
            rdta->xi/rdta->xv);
    rdta->fracchange = 2.0 * rdta->xi * (rdta->freqa * rdta->freqgr
                                       + rdta->freqc * rdta->freqty)
                     + rdta->xv * (1.0 - rdta->freqa * rdta->freqa
                                       - rdta->freqc * rdta->freqc
                                       - rdta->freqg * rdta->freqg
                                       - rdta->freqt * rdta->freqt);
  } /* reportfreqs */


boolean linkdata2tree (rdta, cdta, tr)
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    tree         *tr;
    /* Link data array to the tree tips */
  { /* linkdata2tree */
    int  i;

    for (i = 1; i <= tr->mxtips; i++) {    /* Associate data with tips */
      tr->nodep[i]->tip = &(rdta->y[i-1][0]);
      }

    tr->rdta       = rdta;
    tr->cdta       = cdta;
    return TRUE;
  } /* linkdata2tree */


xarray *setupxarray (npat)
    int npat;
  { /* setupxarray */
    xarray  *x;
    xtype   *data;

    x = (xarray *) Malloc(sizeof(xarray));
    if (x) {
      data = (xtype *) Malloc(4 * npat * sizeof(xtype));
      if (data) {
        x->a = data;
        x->c = data += npat;
        x->g = data += npat;
        x->t = data +  npat;
        x->prev = x->next = x;
        x->owner = (node *) NULL;
        }
      else {
        Free(x);
        return (xarray *) NULL;
        }
      }
    return x;
  } /* setupxarray */


boolean linkxarray (req, min, npat, freexptr, usedxptr)
    int  req, min, npat;
    xarray **freexptr, **usedxptr;
    /*  Link a set of xarrays */
  { /* linkxarray */
    xarray  *first, *prev, *x;
    int  i;

    first = prev = (xarray *) NULL;
    i = 0;

    do {
      x = setupxarray(npat);
      if (x) {
        if (! first) first = x;
        else {
          prev->next = x;
          x->prev = prev;
          }
        prev = x;
        i++;
        }
      else {
        printf("ERROR: Failure to get requested xarray memory\n");
        if (i < min)  return  FALSE;
        }
      } while ((i < req) && x);

    if (first) {
      first->prev = prev;
      prev->next = first;
      }

    *freexptr = first;
    *usedxptr = (xarray *) NULL;

    return  TRUE;
  } /* linkxarray */


boolean setupnodex (tr)
    tree  *tr;
  { /* setupnodex */
    nodeptr  p;
    int  i;

    for (i = tr->mxtips + 1; (i <= 2*(tr->mxtips) - 2); i++) {
      p = tr->nodep[i];
      if (! (p->x = setupxarray(tr->cdta->endsite))) {
        printf("ERROR: Failure to get internal node xarray memory\n");
        return  FALSE;
        }
      }

    return  TRUE;
  } /* setupnodex */


xarray *getxtip (p)
    nodeptr  p;
  { /* getxtip */
    xarray  *new;
    boolean  splice;

    if (! p) return (xarray *) NULL;

    splice = FALSE;

    if (p->x) {                  /* array is there; move to tail of list */
      new = p->x;
      if (new == new->prev) ;             /* linked to self; leave it */
      else if (new == usedxtip) usedxtip = usedxtip->next; /* at head */
      else if (new == usedxtip->prev) ;   /* already at tail */
      else {                              /* move to tail of list */
        new->prev->next = new->next;
        new->next->prev = new->prev;
        splice = TRUE;
        }
      }

    else if (freextip) {                 /* take from unused list */
      p->x = new = freextip;
      new->owner = p;
      if (new->prev != new) {            /* not only member of freelist */
        new->prev->next = new->next;
        new->next->prev = new->prev;
        freextip = new->next;
        }
      else
        freextip = (xarray *) NULL;

      splice = TRUE;
      }

    else if (usedxtip) {                 /* take from head of used list */
      usedxtip->owner->x = (xarray *) NULL;
      p->x = new = usedxtip;
      new->owner = p;
      usedxtip = usedxtip->next;
      }

    else {
      printf("ERROR: Unable to locate memory for tip %d.\n", p->number);
      return  (xarray *) NULL;
      }

    if (splice) {
      if (usedxtip) {                  /* list is not empty */
        usedxtip->prev->next = new;
        new->prev = usedxtip->prev;
        usedxtip->prev = new;
        new->next = usedxtip;
        }
      else
        usedxtip = new->prev = new->next = new;
      }

    return  new;
  } /* getxtip */


xarray *getxnode (p)
    nodeptr  p;
    /* Ensure that internal node p has memory */
  { /* getxnode */
    nodeptr  s;

    if (! (p->x)) {  /*  Move likelihood array on this node to sector p */
      if ((s = p->next)->x || (s = s->next)->x) {
        p->x = s->x;
        s->x = (xarray *) NULL;
        }
      else {
        printf("ERROR: Unable to locate memory at node %d.\n", p->number);
        exit(1);
        }
      }
    return  p->x;
  } /* getxnode */


boolean newview (tr, p)                      /*  Update likelihoods at node */
    tree    *tr;
    nodeptr  p;
  { /* newview */
    double   z1, lz1, xvlz1, z2, lz2, xvlz2;
    nodeptr  q, r;
    xtype   *x1a, *x1c, *x1g, *x1t, *x2a, *x2c, *x2g, *x2t,
            *x3a, *x3c, *x3g, *x3t;
    int  i;

    if (p->tip) {             /*  Make sure that data are at tip */
      int code;
      yType *yptr;

      if (p->x) return TRUE;  /*  They are already there */

      if (! getxtip(p)) return FALSE; /*  They are not, so get memory */
      x3a = &(p->x->a[0]);    /*  Move tip data to xarray */
      x3c = &(p->x->c[0]);
      x3g = &(p->x->g[0]);
      x3t = &(p->x->t[0]);
      yptr = p->tip;
      for (i = 0; i < tr->cdta->endsite; i++) {
        code = *yptr++;
        *x3a++ =  code       & 1;
        *x3c++ = (code >> 1) & 1;
        *x3g++ = (code >> 2) & 1;
        *x3t++ = (code >> 3) & 1;
        }
      return TRUE;
      }

/*  Internal node needs update */

    q = p->next->back;
    r = p->next->next->back;

    while ((! p->x) || (! q->x) || (! r->x)) {
      if (! q->x) if (! newview(tr, q))  return FALSE;
      if (! r->x) if (! newview(tr, r))  return FALSE;
      if (! p->x) if (! getxnode(p)) return FALSE;
      }

    x1a = &(q->x->a[0]);
    x1c = &(q->x->c[0]);
    x1g = &(q->x->g[0]);
    x1t = &(q->x->t[0]);
    z1 = q->z;
    lz1 = (z1 > zmin) ? log(z1) : log(zmin);
    xvlz1 = tr->rdta->xv * lz1;

    x2a = &(r->x->a[0]);
    x2c = &(r->x->c[0]);
    x2g = &(r->x->g[0]);
    x2t = &(r->x->t[0]);
    z2 = r->z;
    lz2 = (z2 > zmin) ? log(z2) : log(zmin);
    xvlz2 = tr->rdta->xv * lz2;

    x3a = &(p->x->a[0]);
    x3c = &(p->x->c[0]);
    x3g = &(p->x->g[0]);
    x3t = &(p->x->t[0]);

    { double  zz1table[maxcategories+1], zv1table[maxcategories+1],
              zz2table[maxcategories+1], zv2table[maxcategories+1],
              *zz1ptr, *zv1ptr, *zz2ptr, *zv2ptr, *rptr;
      double  fx1r, fx1y, fx1n, suma1, sumg1, sumc1, sumt1,
              fx2r, fx2y, fx2n, ki, tempi, tempj;
      int  *cptr;

#     ifdef Vectorize
        double zz1[maxpatterns], zv1[maxpatterns],
               zz2[maxpatterns], zv2[maxpatterns];
        int  cat;
#     else
        double zz1, zv1, zz2, zv2;
        int cat;
#     endif

      rptr   = &(tr->rdta->catrat[1]);
      zz1ptr = &(zz1table[1]);
      zv1ptr = &(zv1table[1]);
      zz2ptr = &(zz2table[1]);
      zv2ptr = &(zv2table[1]);

#     ifdef Vectorize
#       pragma IVDEP
#     endif

      for (i = 1; i <= tr->rdta->categs; i++) {   /* exps for each category */
        ki = *rptr++;
        *zz1ptr++ = exp(ki *   lz1);
        *zv1ptr++ = exp(ki * xvlz1);
        *zz2ptr++ = exp(ki *   lz2);
        *zv2ptr++ = exp(ki * xvlz2);
        }

      cptr = &(tr->cdta->patcat[0]);

#     ifdef Vectorize
#       pragma IVDEP
        for (i = 0; i < tr->cdta->endsite; i++) {
          cat = *cptr++;
          zz1[i] = zz1table[cat];
          zv1[i] = zv1table[cat];
          zz2[i] = zz2table[cat];
          zv2[i] = zv2table[cat];
        }

#       pragma IVDEP
        for (i = 0; i < tr->cdta->endsite; i++) {
          fx1r = tr->rdta->freqa * *x1a + tr->rdta->freqg * *x1g;
          fx1y = tr->rdta->freqc * *x1c + tr->rdta->freqt * *x1t;
          fx1n = fx1r + fx1y;
          tempi = fx1r * tr->rdta->invfreqr;
          tempj = zv1[i] * (tempi-fx1n) + fx1n;
          suma1 = zz1[i] * (*x1a++ - tempi) + tempj;
          sumg1 = zz1[i] * (*x1g++ - tempi) + tempj;
          tempi = fx1y * tr->rdta->invfreqy;
          tempj = zv1[i] * (tempi-fx1n) + fx1n;
          sumc1 = zz1[i] * (*x1c++ - tempi) + tempj;
          sumt1 = zz1[i] * (*x1t++ - tempi) + tempj;

          fx2r = tr->rdta->freqa * *x2a + tr->rdta->freqg * *x2g;
          fx2y = tr->rdta->freqc * *x2c + tr->rdta->freqt * *x2t;
          fx2n = fx2r + fx2y;
          tempi = fx2r * tr->rdta->invfreqr;
          tempj = zv2[i] * (tempi-fx2n) + fx2n;
          *x3a++ = suma1 * (zz2[i] * (*x2a++ - tempi) + tempj);
          *x3g++ = sumg1 * (zz2[i] * (*x2g++ - tempi) + tempj);
          tempi = fx2y * tr->rdta->invfreqy;
          tempj = zv2[i] * (tempi-fx2n) + fx2n;
          *x3c++ = sumc1 * (zz2[i] * (*x2c++ - tempi) + tempj);
          *x3t++ = sumt1 * (zz2[i] * (*x2t++ - tempi) + tempj);
          }

#     else  /*  Not Vectorize  */
        for (i = 0; i < tr->cdta->endsite; i++) {
          cat = *cptr++;
          zz1 = zz1table[cat];
          zv1 = zv1table[cat];
          fx1r = tr->rdta->freqa * *x1a + tr->rdta->freqg * *x1g;
          fx1y = tr->rdta->freqc * *x1c + tr->rdta->freqt * *x1t;
          fx1n = fx1r + fx1y;
          tempi = fx1r * tr->rdta->invfreqr;
          tempj = zv1 * (tempi-fx1n) + fx1n;
          suma1 = zz1 * (*x1a++ - tempi) + tempj;
          sumg1 = zz1 * (*x1g++ - tempi) + tempj;
          tempi = fx1y * tr->rdta->invfreqy;
          tempj = zv1 * (tempi-fx1n) + fx1n;
          sumc1 = zz1 * (*x1c++ - tempi) + tempj;
          sumt1 = zz1 * (*x1t++ - tempi) + tempj;

          zz2 = zz2table[cat];
          zv2 = zv2table[cat];
          fx2r = tr->rdta->freqa * *x2a + tr->rdta->freqg * *x2g;
          fx2y = tr->rdta->freqc * *x2c + tr->rdta->freqt * *x2t;
          fx2n = fx2r + fx2y;
          tempi = fx2r * tr->rdta->invfreqr;
          tempj = zv2 * (tempi-fx2n) + fx2n;
          *x3a++ = suma1 * (zz2 * (*x2a++ - tempi) + tempj);
          *x3g++ = sumg1 * (zz2 * (*x2g++ - tempi) + tempj);
          tempi = fx2y * tr->rdta->invfreqy;
          tempj = zv2 * (tempi-fx2n) + fx2n;
          *x3c++ = sumc1 * (zz2 * (*x2c++ - tempi) + tempj);
          *x3t++ = sumt1 * (zz2 * (*x2t++ - tempi) + tempj);
          }
#     endif  /*  Vectorize or not  */

      return TRUE;
      }
  } /* newview */


double evaluate (tr, p)
    tree    *tr;
    nodeptr  p;
  { /* evaluate */
    double   sum, z, lz, xvlz,
             ki, fx1a, fx1c, fx1g, fx1t, fx1r, fx1y, fx2r, fx2y,
             suma, sumb, sumc, term;

#   ifdef Vectorize
       double   zz[maxpatterns], zv[maxpatterns];
#   else
       double   zz,zv;
#   endif

    double   zztable[maxcategories+1], zvtable[maxcategories+1],
            *zzptr, *zvptr;
    double  *log_f, *rptr;
    xtype   *x1a, *x1c, *x1g, *x1t, *x2a, *x2c, *x2g, *x2t;
    nodeptr  q;
    int  cat, *cptr, i, *wptr;

    q = p->back;
    while ((! p->x) || (! q->x)) {
      if (! (p->x)) if (! newview(tr, p)) return badEval;
      if (! (q->x)) if (! newview(tr, q)) return badEval;
      }

    x1a = &(p->x->a[0]);
    x1c = &(p->x->c[0]);
    x1g = &(p->x->g[0]);
    x1t = &(p->x->t[0]);

    x2a = &(q->x->a[0]);
    x2c = &(q->x->c[0]);
    x2g = &(q->x->g[0]);
    x2t = &(q->x->t[0]);

    z = p->z;
    if (z < zmin) z = zmin;
    lz = log(z);
    xvlz = tr->rdta->xv * lz;

    rptr  = &(tr->rdta->catrat[1]);
    zzptr = &(zztable[1]);
    zvptr = &(zvtable[1]);

#   ifdef Vectorize
#     pragma IVDEP
#   endif

    for (i = 1; i <= tr->rdta->categs; i++) {
      ki = *rptr++;
      *zzptr++ = exp(ki *   lz);
      *zvptr++ = exp(ki * xvlz);
      }

    wptr = &(tr->cdta->aliaswgt[0]);
    cptr = &(tr->cdta->patcat[0]);
    log_f = tr->log_f;
    tr->log_f_valid = tr->cdta->endsite;
    sum = 0.0;

#   ifdef Vectorize
#     pragma IVDEP
      for (i = 0; i < tr->cdta->endsite; i++) {
        cat   = *cptr++;
        zz[i] = zztable[cat];
        zv[i] = zvtable[cat];
      }

#     pragma IVDEP
      for (i = 0; i < tr->cdta->endsite; i++) {
        fx1a = tr->rdta->freqa * *x1a++;
        fx1g = tr->rdta->freqg * *x1g++;
        fx1c = tr->rdta->freqc * *x1c++;
        fx1t = tr->rdta->freqt * *x1t++;
        suma = fx1a * *x2a + fx1c * *x2c + fx1g * *x2g + fx1t * *x2t;
        fx2r = tr->rdta->freqa * *x2a++ + tr->rdta->freqg * *x2g++;
        fx2y = tr->rdta->freqc * *x2c++ + tr->rdta->freqt * *x2t++;
        fx1r = fx1a + fx1g;
        fx1y = fx1c + fx1t;
        sumc = (fx1r + fx1y) * (fx2r + fx2y);
        sumb = fx1r * fx2r * tr->rdta->invfreqr + fx1y * fx2y * tr->rdta->invfreqy;
        suma -= sumb;
        sumb -= sumc;
        term = log(zz[i] * suma + zv[i] * sumb + sumc);
        sum += *wptr++ * term;
        *log_f++ = term;
        }

#   else  /*  Not Vectorize  */
      for (i = 0; i < tr->cdta->endsite; i++) {
        cat  = *cptr++;
        zz   = zztable[cat];
        zv   = zvtable[cat];
        fx1a = tr->rdta->freqa * *x1a++;
        fx1g = tr->rdta->freqg * *x1g++;
        fx1c = tr->rdta->freqc * *x1c++;
        fx1t = tr->rdta->freqt * *x1t++;
        suma = fx1a * *x2a + fx1c * *x2c + fx1g * *x2g + fx1t * *x2t;
        fx2r = tr->rdta->freqa * *x2a++ + tr->rdta->freqg * *x2g++;
        fx2y = tr->rdta->freqc * *x2c++ + tr->rdta->freqt * *x2t++;
        fx1r = fx1a + fx1g;
        fx1y = fx1c + fx1t;
        sumc = (fx1r + fx1y) * (fx2r + fx2y);
        sumb = fx1r * fx2r * tr->rdta->invfreqr + fx1y * fx2y * tr->rdta->invfreqy;
        suma -= sumb;
        sumb -= sumc;
        term = log(zz * suma + zv * sumb + sumc);
        sum += *wptr++ * term;
        *log_f++ = term;
        }
#   endif  /*  Vectorize or not  */

    tr->likelihood = sum;
    return  sum;
  } /* evaluate */




double makenewz (tr, p, q, z0, maxiter)
    tree    *tr;
    nodeptr  p, q;
    double   z0;
    int  maxiter;
  { /* makenewz */
    double  *abi, *bci, *sumci, *abptr, *bcptr, *sumcptr;
    double   dlnLidlz, dlnLdlz, d2lnLdlz2, z, zprev, zstep, lz, xvlz,
             ki, suma, sumb, sumc, ab, bc, inv_Li, t1, t2,
             fx1a, fx1c, fx1g, fx1t, fx1r, fx1y, fx2r, fx2y;
    double   zztable[maxcategories+1], zvtable[maxcategories+1];
    double   *zzptr, *zvptr;
    double  *rptr, *wrptr, *wr2ptr;
    xtype   *x1a, *x1c, *x1g, *x1t, *x2a, *x2c, *x2g, *x2t;
    int      cat, *cptr, i, curvatOK;


    while ((! p->x) || (! q->x)) {
      if (! (p->x)) if (! newview(tr, p)) return badZ;
      if (! (q->x)) if (! newview(tr, q)) return badZ;
      }

    x1a = &(p->x->a[0]);
    x1c = &(p->x->c[0]);
    x1g = &(p->x->g[0]);
    x1t = &(p->x->t[0]);
    x2a = &(q->x->a[0]);
    x2c = &(q->x->c[0]);
    x2g = &(q->x->g[0]);
    x2t = &(q->x->t[0]);

    { unsigned scratch_size;
      scratch_size = sizeof(double) * tr->cdta->endsite;
      if ((abi   = (double *) Malloc(scratch_size)) &&
          (bci   = (double *) Malloc(scratch_size)) &&
          (sumci = (double *) Malloc(scratch_size))) ;
      else {
        printf("ERROR: makenewz unable to obtain space for arrays\n");
        return badZ;
        }
      }
    abptr   = abi;
    bcptr   = bci;
    sumcptr = sumci;

#   ifdef Vectorize
#     pragma IVDEP
#   endif

    for (i = 0; i < tr->cdta->endsite; i++) {
      fx1a = tr->rdta->freqa * *x1a++;
      fx1g = tr->rdta->freqg * *x1g++;
      fx1c = tr->rdta->freqc * *x1c++;
      fx1t = tr->rdta->freqt * *x1t++;
      suma = fx1a * *x2a + fx1c * *x2c + fx1g * *x2g + fx1t * *x2t;
      fx2r = tr->rdta->freqa * *x2a++ + tr->rdta->freqg * *x2g++;
      fx2y = tr->rdta->freqc * *x2c++ + tr->rdta->freqt * *x2t++;
      fx1r = fx1a + fx1g;
      fx1y = fx1c + fx1t;
      *sumcptr++ = sumc = (fx1r + fx1y) * (fx2r + fx2y);
      sumb       = fx1r * fx2r * tr->rdta->invfreqr + fx1y * fx2y * tr->rdta->invfreqy;
      *abptr++   = suma - sumb;
      *bcptr++   = sumb - sumc;
      }

    z = z0;
    do {
      zprev = z;
      zstep = (1.0 - zmax) * z + zmin;
      curvatOK = FALSE;

      do {
        if (z < zmin) z = zmin;
        else if (z > zmax) z = zmax;

        lz    = log(z);
        xvlz  = tr->rdta->xv * lz;
        rptr  = &(tr->rdta->catrat[1]);
        zzptr = &(zztable[1]);
        zvptr = &(zvtable[1]);

#       ifdef Vectorize
#         pragma IVDEP
#       endif

        for (i = 1; i <= tr->rdta->categs; i++) {
          ki = *rptr++;
          *zzptr++ = exp(ki *   lz);
          *zvptr++ = exp(ki * xvlz);
          }

        abptr   = abi;
        bcptr   = bci;
        sumcptr = sumci;
        cptr    = &(tr->cdta->patcat[0]);
        wrptr   = &(tr->cdta->wr[0]);
        wr2ptr  = &(tr->cdta->wr2[0]);
        dlnLdlz = 0.0;                 /*  = d(ln(likelihood))/d(lz) */
        d2lnLdlz2 = 0.0;               /*  = d2(ln(likelihood))/d(lz)2 */

#       ifdef Vectorize
#         pragma IVDEP
#       endif

        for (i = 0; i < tr->cdta->endsite; i++) {
          cat    = *cptr++;              /*  ratecategory(i) */
          ab     = *abptr++ * zztable[cat];
          bc     = *bcptr++ * zvtable[cat];
          sumc   = *sumcptr++;
          inv_Li = 1.0/(ab + bc + sumc);
          t1     = ab * inv_Li;
          t2     = tr->rdta->xv * bc * inv_Li;
          dlnLidlz   = t1 + t2;
          dlnLdlz   += *wrptr++  * dlnLidlz;
          d2lnLdlz2 += *wr2ptr++ * (t1 + tr->rdta->xv * t2 - dlnLidlz * dlnLidlz);
          }

        if ((d2lnLdlz2 >= 0.0) && (z < zmax))
          zprev = z = 0.37 * z + 0.63;  /*  Bad curvature, shorten branch */
        else
          curvatOK = TRUE;

        } while (! curvatOK);

      if (d2lnLdlz2 < 0.0) {
        z *= exp(-dlnLdlz / d2lnLdlz2);
        if (z < zmin) z = zmin;
        if (z > 0.25 * zprev + 0.75)    /*  Limit steps toward z = 1.0 */
          z = 0.25 * zprev + 0.75;
        }

      if (z > zmax) z = zmax;

      } while ((--maxiter > 0) && (ABS(z - zprev) > zstep));

    Free(abi);
    Free(bci);
    Free(sumci);

    return  z;
  } /* makenewz */


boolean update (tr, p)
    tree    *tr;
    nodeptr  p;
  { /* update */
    nodeptr  q;
    double   z0, z;

    q = p->back;
    z0 = q->z;
    if ((z = makenewz(tr, p, q, z0, newzpercycle)) == badZ) return FALSE;
    p->z = q->z = z;
    if (ABS(z - z0) > deltaz)  tr->smoothed = FALSE;
    return TRUE;
  } /* update */


boolean smooth (tr, p)
    tree    *tr;
    nodeptr  p;
  { /* smooth */
    nodeptr  q;

    if (! update(tr, p))               return FALSE; /*  Adjust branch */
    if (! p->tip) {                                  /*  Adjust descendants */
        q = p->next;
        while (q != p) {
          if (! smooth(tr, q->back))   return FALSE;
          q = q->next;
          }

#     if ReturnSmoothedView
        if (! newview(tr, p)) return FALSE;
#     endif
      }

    return TRUE;
  } /* smooth */


boolean smoothTree (tr, maxtimes)
    tree  *tr;
    int    maxtimes;
  { /* smoothTree */
    nodeptr  p, q;

    p = tr->start;

    while (--maxtimes >= 0) {
      tr->smoothed = TRUE;
      if (! smooth(tr, p->back))       return FALSE;
      if (! p->tip) {
        q = p->next;
        while (q != p) {
          if (! smooth(tr, q->back))   return FALSE;
          q = q->next;
          }
        }
      if (tr->smoothed)  break;
      }

    return TRUE;
  } /* smoothTree */


boolean localSmooth (tr, p, maxtimes)    /* Smooth branches around p */
    tree    *tr;
    nodeptr  p;
    int  maxtimes;
  { /* localSmooth */
    nodeptr  q;

    if (p->tip) return FALSE;            /* Should be an error */

    while (--maxtimes >= 0) {
      tr->smoothed = TRUE;
      q = p;
      do {
        if (! update(tr, q)) return FALSE;
        q = q->next;
        } while (q != p);
      if (tr->smoothed)  break;
      }

    tr->smoothed = FALSE;             /* Only smooth locally */
    return TRUE;
  } /* localSmooth */


void hookup (p, q, z)
    nodeptr  p, q;
    double   z;
  { /* hookup */
    p->back = q;
    q->back = p;
    p->z = q->z = z;
  } /* hookup */


boolean insert (tr, p, q, glob)   /* Insert node p into branch q <-> q->back */
    tree    *tr;
    nodeptr  p, q;
    boolean  glob;             /* Smooth tree globally? */

/*                q
                 /.
             add/ .
               /  .
             pn   .
    s ---- p      .remove
             pnn  .
               \  .
             add\ .
                 \.      pn  = p->next;
                  r      pnn = p->next->next;
 */

  { /* insert */
    nodeptr  r, s;

    r = q->back;
    s = p->back;

#   if BestInsertAverage && ! Master
    { double  zqr, zqs, zrs, lzqr, lzqs, lzrs, lzsum, lzq, lzr, lzs, lzmax;

      if ((zqr = makenewz(tr, q, r, q->z,     iterations)) == badZ) return FALSE;
      if ((zqs = makenewz(tr, q, s, defaultz, iterations)) == badZ) return FALSE;
      if ((zrs = makenewz(tr, r, s, defaultz, iterations)) == badZ) return FALSE;

      lzqr = (zqr > zmin) ? log(zqr) : log(zmin);  /* long branches */
      lzqs = (zqs > zmin) ? log(zqs) : log(zmin);
      lzrs = (zrs > zmin) ? log(zrs) : log(zmin);
      lzsum = 0.5 * (lzqr + lzqs + lzrs);

      lzq = lzsum - lzrs;
      lzr = lzsum - lzqs;
      lzs = lzsum - lzqr;
      lzmax = log(zmax);

      if      (lzq > lzmax) {lzq = lzmax; lzr = lzqr; lzs = lzqs;} /* short */
      else if (lzr > lzmax) {lzr = lzmax; lzq = lzqr; lzs = lzrs;}
      else if (lzs > lzmax) {lzs = lzmax; lzq = lzqs; lzr = lzrs;}

      hookup(p->next,       q, exp(lzq));
      hookup(p->next->next, r, exp(lzr));
      hookup(p,             s, exp(lzs));
      }

#   else
    { double  z;
      z = sqrt(q->z);
      hookup(p->next,       q, z);
      hookup(p->next->next, r, z);
      }

#   endif

    if (! newview(tr, p)) return FALSE;   /*  So that p is valid at update */
    tr->opt_level = 0;

#   if ! Master         /*  Smoothings are done by slave */
      if (glob) {                                    /* Smooth whole tree */
        if (! smoothTree(tr, smoothings)) return FALSE;
        }
      else {                                         /* Smooth locale of p */
        if (! localSmooth(tr, p, smoothings)) return FALSE;
        }

#   else
      tr->likelihood = unlikely;
#   endif
    return  TRUE;
  } /* insert */


nodeptr  removeNode (tr, p)
    tree    *tr;
    nodeptr  p;

/*                q
                 .|
          remove. |
               .  |
             pn   |
    s ---- p      |add
             pnn  |
               .  |
          remove. |
                 .|      pn  = p->next;
                  r      pnn = p->next->next;
 */

    /* remove p and return where it was */
  { /* removeNode */
    double   zqr;
    nodeptr  q, r;

    q = p->next->back;
    r = p->next->next->back;
    zqr = q->z * r->z;
#   if ! Master
      if ((zqr = makenewz(tr, q, r, zqr, iterations)) == badZ) return (node *) NULL;
#   endif
    hookup(q, r, zqr);

    p->next->next->back = p->next->back = (node *) NULL;
    return  q;
  } /* removeNode */


boolean initrav (tr, p)
    tree    *tr;
    nodeptr  p;
  { /* initrav */
    nodeptr  q;

    if (! p->tip) {
      q = p->next;
      do {
        if (! initrav(tr, q->back))  return FALSE;
        q = q->next;
        } while (q != p);
      if (! newview(tr, p))         return FALSE;
      }

    return TRUE;
  } /* initrav */


nodeptr buildNewTip (tr, p)
    tree  *tr;
    nodeptr  p;
  { /* buildNewTip */
    nodeptr  q;

    q = tr->nodep[(tr->nextnode)++];
    hookup(p, q, defaultz);
    return  q;
  } /* buildNewTip */


boolean buildSimpleTree (tr, ip, iq, ir)
    tree  *tr;
    int    ip, iq, ir;
  { /* buildSimpleTree */
    /* p, q and r are tips meeting at s */
    nodeptr  p, s;
    int  i;

    i = MIN(ip, iq);
    if (ir < i)  i = ir; 
    tr->start = tr->nodep[i];
    tr->ntips = 3;
    p = tr->nodep[ip];
    hookup(p, tr->nodep[iq], defaultz);
    s = buildNewTip(tr, tr->nodep[ir]);

    return insert(tr, s, p, FALSE);  /* Smoothing is local to s */
  } /* buildSimpleTree */


#ifndef HasStrLib

char * strchr (str, chr)
    char *str;
    int   chr;
 { /* strchr */
    int  c;

    while (c = *str)  {if (c == chr) return str; str++;}
    return  (char *) NULL;
 } /* strchr */


char * strstr (str1, str2)
    char *str1, *str2;
 { /* strstr */
    char *s1, *s2;
    int  c;

    while (*(s1 = str1)) {
      s2 = str2;
      do {
        if (! (c = *s2++))  return str1;
        } 
        while (*s1++ == c);
      str1++;
      }
    return  (char *) NULL;
 } /* strstr */
 
#endif


boolean readKeyValue (string, key, format, value)
    char *string, *key, *format;
    void *value;
  { /* readKeyValue */

    if (! (string = (char*)strstr(string, key)))  return FALSE;
    string += strlen(key);
    if (! (string = (char*)strchr(string, '=')))  return FALSE;
    string++;
    return  sscanf(string, format, value);  /* 1 if read, otherwise 0 */
  } /* readKeyValue */


#if Master || Slave

double str_readTreeLikelihood (treestr)
    char *treestr;
  { /* str_readTreeLikelihood */
    double lk1;
    char    *com, *com_end;
    boolean  readKeyValue();

    if ((com = strchr(treestr, '[')) && (com < strchr(treestr, '('))
                                     && (com_end = strchr(com, ']'))) {
      com++;
      *com_end = 0;
      if (readKeyValue(com, likelihood_key, "%lg", (void *) &(lk1))) {
        *com_end = ']';
        return lk1;
        }
      }

    fprintf(stderr, "ERROR reading likelihood in receiveTree\n");
    return  badEval;
  } /* str_readTreeLikelihood */


boolean sendTree (comm, tr)
    comm_block  *comm;
    tree        *tr;
  { /* sendTree */
    char  *treestr;
    char  *treeString();
#   if Master
      void sendTreeNum();
#   endif

    comm->done_flag = tr->likelihood > 0.0;
    if (comm->done_flag)
      write_comm_msg(comm, NULL);

    else {
      treestr = (char *) Malloc((tr->ntips * (nmlngth+32)) + 256);
      if (! treestr) {
        fprintf(stderr, "sendTree: Malloc failure\n");
        return 0;
        }

#     if Master
        if (send_ahead >= MAX_SEND_AHEAD) {
          double new_likelihood;
          int  n_to_get;

          n_to_get = (send_ahead+1)/2;
          sendTreeNum(n_to_get);
          send_ahead -= n_to_get;
          read_comm_msg(& comm_slave, treestr);
          new_likelihood = str_readTreeLikelihood(treestr);
          if (new_likelihood == badEval)  return FALSE;
          if (! best_tr_recv || (new_likelihood > best_lk_recv)) {
            if (best_tr_recv)  Free(best_tr_recv);
            best_tr_recv = Malloc(strlen(treestr) + 1);
            strcpy(best_tr_recv, treestr);
            best_lk_recv = new_likelihood;
            }
          }
        send_ahead++;
#     endif           /*  End #if Master  */

      REPORT_SEND_TREE;
      (void) treeString(treestr, tr, tr->start->back, 1);
      write_comm_msg(comm, treestr);

      Free(treestr);
      }

    return TRUE;
  } /* sendTree */


boolean  receiveTree (comm, tr)
    comm_block  *comm;
    tree        *tr;
  { /* receiveTree */
    char   *treestr;
    boolean status;
    boolean str_treeReadLen();

    treestr = (char *) Malloc((tr->ntips * (nmlngth+32)) + 256);
    if (! treestr) {
      fprintf(stderr, "receiveTree: Malloc failure\n");
      return 0;
      }

    read_comm_msg(comm, treestr);
    if (comm->done_flag) {
      tr->likelihood = 1.0;
      status = TRUE;
      }

    else {
#     if Master
        if (best_tr_recv) {
          if (str_readTreeLikelihood(treestr) < best_lk_recv) {
            strcpy(treestr, best_tr_recv);  /* Overwrite new tree with best */
            }
          Free(best_tr_recv);
          best_tr_recv = NULL;
          }
#     endif           /*  End #if Master  */

      status = str_treeReadLen(treestr, tr);
      }

    Free(treestr);
    return status;
  } /* receiveTree */


void requestForWork ()
  { /* requestForWork */
    p4_send(DNAML_REQUEST, DNAML_DISPATCHER_ID, NULL, 0);
  } /* requestForWork */
#endif                  /* End #if Master || Slave  */


#if Master
void sendTreeNum(n_to_get)
    int n_to_get;
  { /* sendTreeNum */
    char scr[512];

    sprintf(scr, "%d", n_to_get);
    p4_send(DNAML_NUM_TREE, DNAML_MERGER_ID, scr, strlen(scr)+1);
  } /* sendTreeNum */


boolean  getReturnedTrees (tr, bt, n_tree_sent)
    tree     *tr;
    bestlist *bt;
    int n_tree_sent; /* number of trees sent to slaves */
  { /* getReturnedTrees */
    void sendTreeNum();
    boolean receiveTree();

    sendTreeNum(send_ahead);
    send_ahead = 0;

    if (! receiveTree(& comm_slave, tr))  return FALSE;
    tr->smoothed = TRUE;
    (void) saveBestTree(bt, tr);

    return TRUE;
  } /* getReturnedTrees */
#endif


void cacheZ (tr)
    tree  *tr;
  { /* cacheZ */
    nodeptr  p;
    int  nodes;

    nodes = tr->mxtips  +  3 * (tr->mxtips - 2);
    p = tr->nodep[1];
    while (nodes-- > 0) {p->z0 = p->z; p++;}
  } /* cacheZ */


void restoreZ (tr)
    tree  *tr;
  { /* restoreZ */
    nodeptr  p;
    int  nodes;

    nodes = tr->mxtips  +  3 * (tr->mxtips - 2);
    p = tr->nodep[1];
    while (nodes-- > 0) {p->z = p->z0; p++;}
  } /* restoreZ */


boolean testInsert (tr, p, q, bt, fast)
    tree     *tr;
    nodeptr   p, q;
    bestlist *bt;
    boolean   fast;
  { /* testInsert */
    double  qz;
    nodeptr  r;

    r = q->back;             /* Save original connection */
    qz = q->z;
    if (! insert(tr, p, q, ! fast)) return FALSE;

#   if ! Master
      if (evaluate(tr, fast ? p->next->next : tr->start) == badEval) return FALSE;
      (void) saveBestTree(bt, tr);
#   else  /* Master */
      tr->likelihood = unlikely;
      if (! sendTree(& comm_slave, tr))  return FALSE;
#   endif

    /* remove p from this branch */

    hookup(q, r, qz);
    p->next->next->back = p->next->back = (nodeptr) NULL;
    if (! fast) {            /* With fast add, other values are still OK */
      restoreZ(tr);          /*   Restore branch lengths */
#     if ! Master            /*   Regenerate x values */
        if (! initrav(tr, p->back))  return FALSE;
        if (! initrav(tr, q))        return FALSE;
        if (! initrav(tr, r))        return FALSE;
#     endif
      }

    return TRUE;
  } /* testInsert */


int addTraverse (tr, p, q, mintrav, maxtrav, bt, fast)
    tree     *tr;
    nodeptr   p, q;
    int       mintrav, maxtrav;
    bestlist *bt;
    boolean   fast;
  { /* addTraverse */
    int  tested, newtested;

    tested = 0;
    if (--mintrav <= 0) {           /* Moved minimum distance? */
      if (! testInsert(tr, p, q, bt, fast))  return badRear;
      tested++;
      }

    if ((! q->tip) && (--maxtrav > 0)) {    /* Continue traverse? */
      newtested = addTraverse(tr, p, q->next->back,
                              mintrav, maxtrav, bt, fast);
      if (newtested == badRear) return badRear;
      tested += newtested;
      newtested = addTraverse(tr, p, q->next->next->back,
                              mintrav, maxtrav, bt, fast);
      if (newtested == badRear) return badRear;
      tested += newtested;
      }

    return tested;
  } /* addTraverse */


int  rearrange (tr, p, mintrav, maxtrav, bt)
    tree     *tr;
    nodeptr   p;
    int       mintrav, maxtrav;
    bestlist *bt;
    /* rearranges the tree, globally or locally */
  { /* rearrange */
    double   p1z, p2z, q1z, q2z;
    nodeptr  p1, p2, q, q1, q2;
    int      tested, mintrav2, newtested;

    tested = 0;
    if (maxtrav < 1 || mintrav > maxtrav)  return tested;

/* Moving subtree forward in tree. */

    if (! p->tip) {
      p1 = p->next->back;
      p2 = p->next->next->back;
      if (! p1->tip || ! p2->tip) {
        p1z = p1->z;
        p2z = p2->z;
        if (! removeNode(tr, p)) return badRear;
        cacheZ(tr);
        if (! p1->tip) {
          newtested = addTraverse(tr, p, p1->next->back,
                                  mintrav, maxtrav, bt, FALSE);
          if (newtested == badRear) return badRear;
          tested += newtested;
          newtested = addTraverse(tr, p, p1->next->next->back,
                                  mintrav, maxtrav, bt, FALSE);
          if (newtested == badRear) return badRear;
          tested += newtested;
          }

        if (! p2->tip) {
          newtested = addTraverse(tr, p, p2->next->back,
                                  mintrav, maxtrav, bt, FALSE);
          if (newtested == badRear) return badRear;
          tested += newtested;
          newtested = addTraverse(tr, p, p2->next->next->back,
                                  mintrav, maxtrav, bt, FALSE);
          if (newtested == badRear) return badRear;
          tested += newtested;
          }

        hookup(p->next,       p1, p1z);  /*  Restore original tree */
        hookup(p->next->next, p2, p2z);
        if (! (initrav(tr, tr->start)
            && initrav(tr, tr->start->back))) return badRear;
        }
      }   /* if (! p->tip) */

/* Moving subtree backward in tree.  Minimum move is 2 to avoid duplicates */

    q = p->back;
    if (! q->tip && maxtrav > 1) {
      q1 = q->next->back;
      q2 = q->next->next->back;
      if (! q1->tip && (!q1->next->back->tip || !q1->next->next->back->tip) ||
          ! q2->tip && (!q2->next->back->tip || !q2->next->next->back->tip)) {
        q1z = q1->z;
        q2z = q2->z;
        if (! removeNode(tr, q)) return badRear;
        cacheZ(tr);
        mintrav2 = mintrav > 2 ? mintrav : 2;

        if (! q1->tip) {
          newtested = addTraverse(tr, q, q1->next->back,
                                  mintrav2 , maxtrav, bt, FALSE);
          if (newtested == badRear) return badRear;
          tested += newtested;
          newtested = addTraverse(tr, q, q1->next->next->back,
                                  mintrav2 , maxtrav, bt, FALSE);
          if (newtested == badRear) return badRear;
          tested += newtested;
          }

        if (! q2->tip) {
          newtested = addTraverse(tr, q, q2->next->back,
                                  mintrav2 , maxtrav, bt, FALSE);
          if (newtested == badRear) return badRear;
          tested += newtested;
          newtested = addTraverse(tr, q, q2->next->next->back,
                                  mintrav2 , maxtrav, bt, FALSE);
          if (newtested == badRear) return badRear;
          tested += newtested;
          }

        hookup(q->next,       q1, q1z);  /*  Restore original tree */
        hookup(q->next->next, q2, q2z);
        if (! (initrav(tr, tr->start)
            && initrav(tr, tr->start->back))) return badRear;
        }
      }   /* if (! q->tip && maxtrav > 1) */

/* Move other subtrees */

    if (! p->tip) {
      newtested = rearrange(tr, p->next->back,       mintrav, maxtrav, bt);
      if (newtested == badRear) return badRear;
      tested += newtested;
      newtested = rearrange(tr, p->next->next->back, mintrav, maxtrav, bt);
      if (newtested == badRear) return badRear;
      tested += newtested;
      }

    return  tested;
  } /* rearrange */

#ifdef NoGetPID 
/* macintosh */
/* this is NOT define we want, but what Mac define fits all compilers !? */
long getpid(void) 
{
	return 0;
}
#endif

FILE *fopen_pid (filenm, mode, name_pid)
    char *filenm, *mode, *name_pid;
  { /* fopen_pid */

    (void) sprintf(name_pid, "%s.%d", filenm, getpid());
    return  fopen(name_pid, mode);
  } /* fopen_pid */


#if DeleteCheckpointFile
void  unlink_pid (filenm)
    char *filenm;
  { /* unlink_pid */
    char scr[512];

    (void) sprintf(scr, "%s.%d", filenm, getpid());
    unlink(scr);
  } /* unlink_pid */
#endif


void  writeCheckpoint (tr)
    tree  *tr;
  { /* writeCheckpoint */
    char   filename[128];
    FILE  *checkpointf;
    void   treeOut();

    checkpointf = fopen_pid(checkpointname, "a", filename);
    if (checkpointf) {
      treeOut(checkpointf, tr, treeNewick);
      (void) fclose(checkpointf);
      }
  } /* writeCheckpoint */


node * findAnyTip(p)
    nodeptr  p;
  { /* findAnyTip */
    return  p->tip ? p : findAnyTip(p->next->back);
  } /* findAnyTip */


boolean  optimize (tr, maxtrav, bt)
    tree     *tr;
    int       maxtrav;
    bestlist *bt;
  { /* optimize */
    nodeptr  p;
    int    mintrav, tested;

    if (tr->ntips < 4)  return  TRUE;

    writeCheckpoint(tr);                    /* checkpoint the starting tree */

    if (maxtrav > tr->ntips - 3)  maxtrav = tr->ntips - 3;
    if (maxtrav <= tr->opt_level)  return  TRUE;

    printf("      Doing %s rearrangements\n",
             (maxtrav == 1)            ? "local" :
             (maxtrav < tr->ntips - 3) ? "regional" : "global");

    /* loop while tree gets better  */

    do {
      (void) startOpt(bt, tr);
      mintrav = tr->opt_level + 1;

      /* rearrange must start from a tip or it will miss some trees */

      p = findAnyTip(tr->start);
      tested = rearrange(tr, p->back, mintrav, maxtrav, bt);
      if (tested == badRear)  return FALSE;

#     if Master
        if (! getReturnedTrees(tr, bt, tested)) return FALSE;
#     endif

      bt->numtrees += tested;
      (void) setOptLevel(bt, maxtrav);
      if (! recallBestTree(bt, 1, tr)) return FALSE;   /* recover best tree */

      printf("      Tested %d alternative trees\n", tested);
      if (bt->improved) {
        printf("      Ln Likelihood =%14.5f\n", tr->likelihood);
        }

      writeCheckpoint(tr);                  /* checkpoint the new tree */
      } while (maxtrav > tr->opt_level);

    return TRUE;
  } /* optimize */


void coordinates (tr, p, lengthsum, tdptr)
    tree     *tr;
    nodeptr   p;
    double    lengthsum;
    drawdata *tdptr;
  { /* coordinates */
    /* establishes coordinates of nodes */
    double  x, z;
    nodeptr  q, first, last;

    if (p->tip) {
      p->xcoord = NINT(over * lengthsum);
      p->ymax = p->ymin = p->ycoord = tdptr->tipy;
      tdptr->tipy += down;
      if (lengthsum > tdptr->tipmax) tdptr->tipmax = lengthsum;
      }

    else {
      q = p->next;
      do {
        z = q->z;
        if (z < zmin) z = zmin;
        x = lengthsum - tr->rdta->fracchange * log(z);
        coordinates(tr, q->back, x, tdptr);
        q = q->next;
        } while (p == tr->start->back ? q != p->next : q != p);

      first = p->next->back;
      q = p;
      while (q->next != p) q = q->next;
      last = q->back;
      p->xcoord = NINT(over * lengthsum);
      p->ycoord = (first->ycoord + last->ycoord)/2;
      p->ymin = first->ymin;
      p->ymax = last->ymax;
      }
  } /* coordinates */


void drawline (tr, i, scale)
    tree   *tr;
    int     i;
    double  scale;
    /* draws one row of the tree diagram by moving up tree */
    /* Modified to handle 1000 taxa, October 16, 1991 */
  { /* drawline */
    nodeptr  p, q, r, first, last;
    int  n, j, k, l, extra;
    boolean  done;

    p = q = tr->start->back;
    extra = 0;

    if (i == p->ycoord) {
      k = q->number - tr->mxtips;
      for (j = k; j < 1000; j *= 10) putchar('-');
      printf("%d", k);
      extra = 1;
      }
    else printf("   ");

    do {
      if (! p->tip) {
        r = p->next;
        done = FALSE;
        do {
          if ((i >= r->back->ymin) && (i <= r->back->ymax)) {
            q = r->back;
            done = TRUE;
            }
          r = r->next;
          } while (! done && (p == tr->start->back ? r != p->next : r != p));

        first = p->next->back;
        r = p;
        while (r->next != p) r = r->next;
        last = r->back;
        if (p == tr->start->back) last = p->back;
        }

      done = (p->tip) || (p == q);
      n = NINT(scale*(q->xcoord - p->xcoord));
      if ((n < 3) && (! q->tip)) n = 3;
      n -= extra;
      extra = 0;

      if ((q->ycoord == i) && (! done)) {
        if (p->ycoord != q->ycoord) putchar('+');
        else                        putchar('-');

        if (! q->tip) {
          k = q->number - tr->mxtips;
          l = n - 3;
          for (j = k; j < 100; j *= 10)  l++;
          for (j = 1; j <= l; j++) putchar('-');
          printf("%d", k);
          extra = 1;
          }
        else for (j = 1; j <= n-1; j++) putchar('-');
        }

      else if (! p->tip) {
        if ((last->ycoord > i) && (first->ycoord < i) && (i != p->ycoord)) {
          putchar('!');
          for (j = 1; j <= n-1; j++) putchar(' ');
          }
        else for (j = 1; j <= n; j++) putchar(' ');
        }

      else
        for (j = 1; j <= n; j++) putchar(' ');

      p = q;
      } while (! done);

    if ((p->ycoord == i) && p->tip) {
      printf(" %s", p->name);
      }

    putchar('\n');
  } /* drawline */


void printTree (tr, adef)
    tree     *tr;
    analdef  *adef;
    /* prints out diagram of the tree */
  { /* printTree */
    drawdata  tipdata;
    double  scale;
    int  i, imax;

    if (adef->trprint) {
      putchar('\n');
      tipdata.tipy = 1;
      tipdata.tipmax = 0.0;
      coordinates(tr, tr->start->back, (double) 0.0, & tipdata);
      scale = 1.0 / tipdata.tipmax;
      imax = tipdata.tipy - down;
      for (i = 1; i <= imax; i++)  drawline(tr, i, scale);
      printf("\nRemember: ");
      if (adef->root) printf("(although rooted by outgroup) ");
      printf("this is an unrooted tree!\n\n");
      }
  } /* printTree */


double sigma (tr, p, sumlrptr)
    tree    *tr;
    nodeptr  p;
    double  *sumlrptr;
    /* compute standard deviation */
  { /* sigma */
    double  slope, sum, sumlr, z, zv, zz, lz,
            rat, suma, sumb, sumc, d2, d, li, temp, abzz, bczv, t3,
            fx1a, fx1c, fx1g, fx1t, fx1r, fx1y, fx2r, fx2y, w;
    double  *rptr;
    xtype   *x1a, *x1c, *x1g, *x1t, *x2a, *x2c, *x2g, *x2t;
    nodeptr  q;
    int  i, *wptr;

    q = p->back;
    while ((! p->x) || (! q->x)) {
      if (! (p->x)) if (! newview(tr, p)) return -1.0;
      if (! (q->x)) if (! newview(tr, q)) return -1.0;
      }

    x1a = &(p->x->a[0]);
    x1c = &(p->x->c[0]);
    x1g = &(p->x->g[0]);
    x1t = &(p->x->t[0]);

    x2a = &(q->x->a[0]);
    x2c = &(q->x->c[0]);
    x2g = &(q->x->g[0]);
    x2t = &(q->x->t[0]);

    z = p->z;
    if (z < zmin) z = zmin;
    lz = log(z);

    wptr = &(tr->cdta->aliaswgt[0]);
    rptr = &(tr->cdta->patrat[0]);
    sum = sumlr = slope = 0.0;

#   ifdef Vectorize
#     pragma IVDEP
#   endif

    for (i = 0; i < tr->cdta->endsite; i++) {
      rat  = *rptr++;
      zz   = exp(rat            * lz);
      zv   = exp(rat * tr->rdta->xv * lz);

      fx1a = tr->rdta->freqa * *x1a++;
      fx1g = tr->rdta->freqg * *x1g++;
      fx1c = tr->rdta->freqc * *x1c++;
      fx1t = tr->rdta->freqt * *x1t++;
      fx1r = fx1a + fx1g;
      fx1y = fx1c + fx1t;
      suma = fx1a * *x2a + fx1c * *x2c + fx1g * *x2g + fx1t * *x2t;
      fx2r = tr->rdta->freqa * *x2a++ + tr->rdta->freqg * *x2g++;
      fx2y = tr->rdta->freqc * *x2c++ + tr->rdta->freqt * *x2t++;
      sumc = (fx1r + fx1y) * (fx2r + fx2y);
      sumb = fx1r * fx2r * tr->rdta->invfreqr + fx1y * fx2y * tr->rdta->invfreqy;
      abzz = zz * (suma - sumb);
      bczv = zv * (sumb - sumc);
      li = sumc + abzz + bczv;
      t3 = tr->rdta->xv * bczv;
      d  = abzz + t3;
      d2 = rat * (abzz*(rat-1.0) + t3*(rat * tr->rdta->xv - 1.0));

      temp = rat * d / li;
      w = *wptr++;
      slope += w *  temp;
      sum   += w * (temp * temp - d2/li);
      sumlr += w * log(li / (suma + 1.0E-300));
      }

    *sumlrptr = sumlr;
    return (sum > 1.0E-300) ? z*(-slope + sqrt(slope*slope + 3.841*sum))/sum
                            : 1.0;
  } /* sigma */


void describe (tr, p)
    tree    *tr;
    nodeptr  p;
    /* print out information for one branch */
  { /* describe */
    double   z, s, sumlr;
    nodeptr  q;
    char    *nameptr;
    int      k, ch;

    q = p->back;
    printf("%4d          ", q->number - tr->mxtips);
    if (p->tip) {
      nameptr = p->name;
      k = nmlngth;
      while (ch = *nameptr++) {putchar(ch); k--;}
      while (--k >= 0) putchar(' ');
      }
    else {
      printf("%4d", p->number - tr->mxtips);
      for (k = 4; k < nmlngth; k++) putchar(' ');
      }

    z = q->z;
    if (z <= zmin) printf("    infinity");
    else printf("%15.5f", -log(z) * tr->rdta->fracchange);

    s = sigma(tr, q, & sumlr);
    printf("     (");
    if (z + s >= zmax) printf("     zero");
    else printf("%9.5f", (double) -log(z + s) * tr->rdta->fracchange);
    putchar(',');
    if (z - s <= zmin) printf("    infinity");
    else printf("%12.5f", (double) -log(z - s) * tr->rdta->fracchange);
    putchar(')');

    if      (sumlr > 2.995 ) printf(" **");
    else if (sumlr > 1.9205) printf(" *");
    putchar('\n');

    if (! p->tip) {
      describe(tr, p->next->back);
      describe(tr, p->next->next->back);
      }
  } /* describe */


void summarize (tr)
    tree  *tr;
    /* print out branch length information and node numbers */
  { /* summarize */
    printf("Ln Likelihood =%14.5f\n", tr->likelihood);
    putchar('\n');
    printf(" Between        And             Length");
    printf("      Approx. Confidence Limits\n");
    printf(" -------        ---             ------");
    printf("      ------- ---------- ------\n");

    describe(tr, tr->start->back->next->back);
    describe(tr, tr->start->back->next->next->back);
    describe(tr, tr->start);
    putchar('\n');
    printf("     *  = significantly positive, P < 0.05\n");
    printf("     ** = significantly positive, P < 0.01\n\n\n");
  } /* summarize */


/*===========  This is a problem if tr->start->back is a tip!  ===========*/
/*  All routines should be contrived so that tr->start->back is not a tip */

char *treeString (treestr, tr, p, form)
    char  *treestr;
    tree  *tr;
    nodeptr  p;
    int  form;
    /* write string with representation of tree */
    /*   form == 1 -> Newick tree */
    /*   form == 2 -> Prolog fact */
    /*   form == 3 -> PHYLIP tree */
  { /* treeString */
    double  x, z;
    char  *nameptr;
    int    c;

    if (p == tr->start->back) {
      if (form != treePHYLIP) {
        if (form == treeProlog) {
          (void) sprintf(treestr, "phylip_tree(");
          while (*treestr) treestr++;            /* move pointer to null */
          }

        (void) sprintf(treestr, "[&&%s: version = '%s'",
                                 programName, programVersion);
        while (*treestr) treestr++;

        (void) sprintf(treestr, ", %s = %15.13g",
                                 likelihood_key, tr->likelihood);
        while (*treestr) treestr++;

        (void) sprintf(treestr, ", %s = %d", ntaxa_key, tr->ntips);
        while (*treestr) treestr++;

        (void) sprintf(treestr,", %s = %d", opt_level_key, tr->opt_level);
        while (*treestr) treestr++;

        (void) sprintf(treestr, ", %s = %d", smoothed_key, tr->smoothed);
        while (*treestr) treestr++;

        (void) sprintf(treestr, "]%s", form == treeProlog ? ", " : " ");
        while (*treestr) treestr++;
        }
      }

    if (p->tip) {
      if (form != treePHYLIP) *treestr++ = '\'';
      nameptr = p->name;
      while (c = *nameptr++) {
        if (form != treePHYLIP) {if (c == '\'') *treestr++ = '\'';}
        else if (c == ' ') {c = '_';}
        *treestr++ = c;
        }
      if (form != treePHYLIP) *treestr++ = '\'';
      }

    else {
      *treestr++ = '(';
      treestr = treeString(treestr, tr, p->next->back, form);
      *treestr++ = ',';
      treestr = treeString(treestr, tr, p->next->next->back, form);
      if (p == tr->start->back) {
        *treestr++ = ',';
        treestr = treeString(treestr, tr, p->back, form);
        }
      *treestr++ = ')';
      }

    if (p == tr->start->back) {
      (void) sprintf(treestr, ":0.0%s\n", (form != treeProlog) ? ";" : ").");
      }
    else {
      z = p->z;
      if (z < zmin) z = zmin;
      x = -log(z) * tr->rdta->fracchange;
      (void) sprintf(treestr, ": %8.6f", x);  /* prolog needs the space */
      }

    while (*treestr) treestr++;     /* move pointer up to null termination */
    return  treestr;
  } /* treeString */


void treeOut (treefile, tr, form)
    FILE  *treefile;
    tree  *tr;
    int    form;
    /* write out file with representation of final tree */
  { /* treeOut */
    int    c;
    char  *cptr, *treestr;

    treestr = (char *) Malloc((tr->ntips * (nmlngth+32)) + 256);
    if (! treestr) {
      fprintf(stderr, "treeOut: Malloc failure\n");
      exit(1);
      }

    (void) treeString(treestr, tr, tr->start->back, form);
    cptr = treestr;
    while (c = *cptr++) putc(c, treefile);

    Free(treestr);
  } /* treeOut */


/*=======================================================================*/
/*                         Read a tree from a file                       */
/*=======================================================================*/


/*  1.0.A  Processing of quotation marks in comment removed
 */

int treeFinishCom (fp, strp)
    FILE   *fp;
    char  **strp;
  { /* treeFinishCom */
    int  ch;

    while ((ch = getc(fp)) != EOF && ch != ']') {
      if (strp != NULL) *(*strp)++ = ch;    /* save character  */
      if (ch == '[') {                      /* nested comment; find its end */
        if ((ch = treeFinishCom(fp, strp)) == EOF)  break;
        if (strp != NULL) *(*strp)++ = ch;  /* save closing ]  */
        }
      }

    if (strp != NULL) **strp = '\0';        /* terminate string  */
    return  ch;
  } /* treeFinishCom */


int treeGetCh (fp)
    FILE  *fp;                   /* get next nonblank, noncomment character */
  { /* treeGetCh */
    int  ch;

    while ((ch = getc(fp)) != EOF) {
      if (whitechar(ch)) ;
      else if (ch == '[') {                   /* comment; find its end */
        if ((ch = treeFinishCom(fp, (char **) NULL)) == EOF)  break;
        }
      else  break;
      }

    return  ch;
  } /* treeGetCh */


boolean  treeLabelEnd (ch)
      int ch;
  { /* treeLabelEnd */
    switch (ch) {
        case EOF:  case '\0':  case '\t':  case '\n':  case ' ':
        case ':':  case ',':   case '(':   case ')':   case '[':
        case ';':
          return TRUE;
        default:
          break;
        }
    return FALSE;
  } /* treeLabelEnd */


boolean  treeGetLabel (fp, lblPtr, maxlen)
    FILE  *fp;
    char  *lblPtr;
    int    maxlen;
  { /* treeGetLabel */
    int      ch;
    boolean  done, quoted, lblfound;

    if (--maxlen < 0) lblPtr = (char *) NULL;  /* reserves space for '\0' */
    else if (lblPtr == NULL) maxlen = 0;

    ch = getc(fp);
    done = treeLabelEnd(ch);

    lblfound = ! done;
    quoted = (ch == '\'');
    if (quoted && ! done) {ch = getc(fp); done = (ch == EOF);}

    while (! done) {
      if (quoted) {
        if (ch == '\'') {ch = getc(fp); if (ch != '\'') break;}
        }

      else if (treeLabelEnd(ch)) break;

      else if (ch == '_') ch = ' ';  /* unquoted _ goes to space */

      if (--maxlen >= 0) *lblPtr++ = ch;
      ch = getc(fp);
      if (ch == EOF) break;
      }

    if (ch != EOF)  (void) ungetc(ch, fp);

    if (lblPtr != NULL) *lblPtr = '\0';

    return lblfound;
  } /* treeGetLabel */


boolean  treeFlushLabel (fp)
    FILE  *fp;
  { /* treeFlushLabel */
    return  treeGetLabel(fp, (char *) NULL, (int) 0);
  } /* treeFlushLabel */


int  treeFindTipByLabel (str, tr)
    char  *str;  /*  label string pointer */
    tree  *tr;
  { /* treeFindTipByLabel */
    nodeptr  q;
    char    *nameptr;
    int      ch, i, n;
    boolean  found;

    for (n = 1; n <= tr->mxtips; n++) {
      q = tr->nodep[n];
      if (! (q->back)) {          /*  Only consider unused tips */
        i = 0;
        nameptr = q->name;
        while ((found = (str[i++] == (ch = *nameptr++))) && ch) ;
        if (found) return n;
        }
      }

    printf("ERROR: Cannot find tree species: %s\n", str);

    return  0;
  } /* treeFindTipByLabel */


int  treeFindTipName (fp, tr)
    FILE  *fp;
    tree  *tr;
  { /* treeFindTipName */
    char    *nameptr, str[nmlngth+2];
    int      n;

    if (tr->prelabeled) {
      if (treeGetLabel(fp, str, nmlngth+2))
        n = treeFindTipByLabel(str, tr);
      else
        n = 0;
      }

    else if (tr->ntips < tr->mxtips) {
      n = tr->ntips + 1;
      nameptr = tr->nodep[n]->name;
      if (! treeGetLabel(fp, nameptr, nmlngth+1)) n = 0;
      }

    else {
      n = 0;
      }

    return  n;
  } /* treeFindTipName */


void  treeEchoContext (fp1, fp2, n)
    FILE  *fp1, *fp2;
    int    n;
 { /* treeEchoContext */
   int      ch;
   boolean  waswhite;

   waswhite = TRUE;

   while (n > 0 && ((ch = getc(fp1)) != EOF)) {
     if (whitechar(ch)) {
       ch = waswhite ? '\0' : ' ';
       waswhite = TRUE;
       }
     else {
       waswhite = FALSE;
       }

     if (ch > '\0') {putc(ch, fp2); n--;}
     }
 } /* treeEchoContext */


boolean treeProcessLength (fp, dptr)
    FILE    *fp;
    double  *dptr;
  { /* treeProcessLength */
    int  ch;

    if ((ch = treeGetCh(fp)) == EOF)  return FALSE;    /*  Skip comments */
    (void) ungetc(ch, fp);

    if (fscanf(fp, "%lf", dptr) != 1) {
      printf("ERROR: treeProcessLength: Problem reading branch length\n");
      treeEchoContext(fp, stdout, 40);
      printf("\n");
      return  FALSE;
      }

    return  TRUE;
  } /* treeProcessLength */


boolean  treeFlushLen (fp)
    FILE  *fp;
  { /* treeFlushLen */
    double  dummy;
    int     ch;

    if ((ch = treeGetCh(fp)) == ':') return treeProcessLength(fp, & dummy);

    if (ch != EOF) (void) ungetc(ch, fp);
    return TRUE;
  } /* treeFlushLen */


boolean  treeNeedCh (fp, c1, where)
    FILE  *fp;
    int    c1;
    char  *where;
  { /* treeNeedCh */
    int  c2;

    if ((c2 = treeGetCh(fp)) == c1)  return TRUE;

    printf("ERROR: Expecting '%c' %s tree; found:", c1, where);
    if (c2 == EOF) {
      printf("End-of-File");
      }
    else {
      ungetc(c2, fp);
      treeEchoContext(fp, stdout, 40);
      }
    putchar('\n');
    return FALSE;
  } /* treeNeedCh */


boolean  addElementLen (fp, tr, p)
    FILE    *fp;
    tree    *tr;
    nodeptr  p;
  { /* addElementLen */
    double   z, branch;
    nodeptr  q;
    int      n, ch;

    if ((ch = treeGetCh(fp)) == '(') {     /*  A new internal node */
      n = (tr->nextnode)++;
      if (n > 2*(tr->mxtips) - 2) {
        if (tr->rooted || n > 2*(tr->mxtips) - 1) {
          printf("ERROR: Too many internal nodes.  Is tree rooted?\n");
          printf("       Deepest splitting should be a trifurcation.\n");
          return FALSE;
          }
        else {
          tr->rooted = TRUE;
          }
        }
      q = tr->nodep[n];
      if (! addElementLen(fp, tr, q->next))        return FALSE;
      if (! treeNeedCh(fp, ',', "in"))             return FALSE;
      if (! addElementLen(fp, tr, q->next->next))  return FALSE;
      if (! treeNeedCh(fp, ')', "in"))             return FALSE;
      (void) treeFlushLabel(fp);
      }

    else {                               /*  A new tip */
      ungetc(ch, fp);
      if ((n = treeFindTipName(fp, tr)) <= 0)          return FALSE;
      q = tr->nodep[n];
      if (tr->start->number > n)  tr->start = q;
      (tr->ntips)++;
      }                                  /* End of tip processing */

    if (tr->userlen) {
      if (! treeNeedCh(fp, ':', "in"))             return FALSE;
      if (! treeProcessLength(fp, & branch))       return FALSE;
      z = exp(-branch / tr->rdta->fracchange);
      if (z > zmax)  z = zmax;
      hookup(p, q, z);
      }
    else {
      if (! treeFlushLen(fp))                       return FALSE;
      hookup(p, q, defaultz);
      }

    return TRUE;
  } /* addElementLen */


int saveTreeCom (comstrp)
    char  **comstrp;
  { /* saveTreeCom */
    int  ch;
    boolean  inquote;

    inquote = FALSE;
    while ((ch = getc(INFILE)) != EOF && (inquote || ch != ']')) {
      *(*comstrp)++ = ch;                        /* save character  */
      if (ch == '[' && ! inquote) {              /* comment; find its end */
        if ((ch = saveTreeCom(comstrp)) == EOF)  break;
        *(*comstrp)++ = ch;                      /* add ] */
        }
      else if (ch == '\'') inquote = ! inquote;  /* start or end of quote */
      }

    return  ch;
  } /* saveTreeCom */


boolean processTreeCom (fp, tr)
    FILE  *fp;
    tree  *tr;
  { /* processTreeCom */
    int   text_started, functor_read, com_open;

    /*  Accept prefatory "phylip_tree(" or "pseudoNewick("  */

    functor_read = text_started = 0;
    (void) fscanf(fp, " p%nhylip_tree(%n", & text_started, & functor_read);
    if (text_started && ! functor_read) {
      (void) fscanf(fp, "seudoNewick(%n", & functor_read);
      if (! functor_read) {
        printf("Start of tree 'p...' not understood.\n");
        return FALSE;
        }
      }

    com_open = 0;
    (void) fscanf(fp, " [%n", & com_open);

    if (com_open) {                                  /* comment; read it */
      char  com[1024], *com_end;

      com_end = com;
      if (treeFinishCom(fp, & com_end) == EOF) {     /* omits enclosing []s */
        printf("Missing end of tree comment\n");
        return FALSE;
        }

      (void) readKeyValue(com, likelihood_key, "%lg",
                               (void *) &(tr->likelihood));
      (void) readKeyValue(com, opt_level_key,  "%d",
                               (void *) &(tr->opt_level));
      (void) readKeyValue(com, smoothed_key,   "%d",
                               (void *) &(tr->smoothed));

      if (functor_read) (void) fscanf(fp, " ,");   /* remove trailing comma */
      }

    return (functor_read > 0);
  } /* processTreeCom */


nodeptr uprootTree (tr, p)
    tree   *tr;
    nodeptr p;
  { /* uprootTree */
    nodeptr  q, r, s, start;
    int      n;

    if (p->tip || p->back) {
      printf("ERROR: Unable to uproot tree.\n");
      printf("       Inappropriate node marked for removal.\n");
      return (nodeptr) NULL;
      }

    n = --(tr->nextnode);               /* last internal node added */
    if (n != tr->mxtips + tr->ntips - 1) {
      printf("ERROR: Unable to uproot tree.  Inconsistent\n");
      printf("       number of tips and nodes for rooted tree.\n");
      return (nodeptr) NULL;
      }

    q = p->next->back;                  /* remove p from tree */
    r = p->next->next->back;
    hookup(q, r, tr->userlen ? (q->z * r->z) : defaultz);

    start = (r->tip || (! q->tip)) ? r : r->next->next->back;

    if (tr->ntips > 2 && p->number != n) {
      q = tr->nodep[n];            /* transfer last node's conections to p */
      r = q->next;
      s = q->next->next;
      hookup(p,             q->back, q->z);   /* move connections to p */
      hookup(p->next,       r->back, r->z);
      hookup(p->next->next, s->back, s->z);
      if (start->number == q->number) start = start->back->back;
      q->back = r->back = s->back = (nodeptr) NULL;
      }
    else {
      p->back = p->next->back = p->next->next->back = (nodeptr) NULL;
      }

    tr->rooted = FALSE;
    return  start;
  } /* uprootTree */


boolean treeReadLen (fp, tr)
    FILE  *fp;
    tree  *tr;
  { /* treeReadLen */
    nodeptr  p;
    int      i, ch;
    boolean  is_fact;

    for (i = 1; i <= tr->mxtips; i++) tr->nodep[i]->back = (node *) NULL;
    tr->start       = tr->nodep[tr->mxtips];
    tr->ntips       = 0;
    tr->nextnode    = tr->mxtips + 1;
    tr->opt_level   = 0;
    tr->log_f_valid = 0;
    tr->smoothed    = FALSE;
    tr->rooted      = FALSE;

    is_fact = processTreeCom(fp, tr);

    p = tr->nodep[(tr->nextnode)++];
    if (! treeNeedCh(fp, '(', "at start of"))       return FALSE;
    if (! addElementLen(fp, tr, p))                 return FALSE;
    if (! treeNeedCh(fp, ',', "in"))                return FALSE;
    if (! addElementLen(fp, tr, p->next))           return FALSE;
    if (! tr->rooted) {
      if ((ch = treeGetCh(fp)) == ',') {        /*  An unrooted format */
        if (! addElementLen(fp, tr, p->next->next)) return FALSE;
        }
      else {                                    /*  A rooted format */
        tr->rooted = TRUE;
        if (ch != EOF)  (void) ungetc(ch, fp);
        }
      }
    else {
      p->next->next->back = (nodeptr) NULL;
      }
    if (! treeNeedCh(fp, ')', "in"))                return FALSE;
    (void) treeFlushLabel(fp);
    if (! treeFlushLen(fp))                         return FALSE;
    if (is_fact) {
      if (! treeNeedCh(fp, ')', "at end of"))       return FALSE;
      if (! treeNeedCh(fp, '.', "at end of"))       return FALSE;
      }
    else {
      if (! treeNeedCh(fp, ';', "at end of"))       return FALSE;
      }

    if (tr->rooted) {
      p->next->next->back = (nodeptr) NULL;
      tr->start = uprootTree(tr, p->next->next);
      if (! tr->start)                              return FALSE;
      }
    else {
      tr->start = p->next->next->back;  /* This is start used by treeString */
      }

    return  (initrav(tr, tr->start) && initrav(tr, tr->start->back));
  } /* treeReadLen */


/*=======================================================================*/
/*                        Read a tree from a string                      */
/*=======================================================================*/


#if Master || Slave
int str_treeFinishCom (treestrp, strp)
    char  **treestrp;     /*  tree string pointer */
    char  **strp;         /*  comment string pointer */
  { /* str_treeFinishCom */
    int  ch;

    while ((ch = *(*treestrp)++) != NULL && ch != ']') {
      if (strp != NULL) *(*strp)++ = ch;    /* save character  */
      if (ch == '[') {                      /* nested comment; find its end */
        if ((ch = str_treeFinishCom(treestrp)) == NULL)  break;
        if (strp != NULL) *(*strp)++ = ch;  /* save closing ]  */
        }
      }
    if (strp != NULL) **strp = '\0';        /* terminate string  */
    return  ch;
  } /* str_treeFinishCom */


int str_treeGetCh (treestrp)
    char **treestrp;  /*  tree string pointer */
    /* get next nonblank, noncomment character */
  { /* str_treeGetCh */
    int  ch;
    
    while ((ch = *(*treestrp)++) != NULL) {
      if (whitechar(ch)) ;
      else if (ch == '[') {                  /* comment; find its end */
        if ((ch = str_treeFinishCom(treestrp, (char *) NULL)) == NULL)  break;
        }
      else  break;
      }

    return  ch;
  } /* str_treeGetCh */


boolean  str_treeGetLabel (treestrp, lblPtr, maxlen)
    char **treestrp;  /*  tree string pointer */
    char  *lblPtr;
    int    maxlen;
  { /* str_treeGetLabel */
    int      ch;
    boolean  done, quoted, lblfound;

    if (--maxlen < 0) lblPtr = (char *) NULL;  /* reserves space for '\0' */
    else if (lblPtr == NULL) maxlen = 0;

    ch = *(*treestrp)++;
    done = treeLabelEnd(ch);

    lblfound = ! done;
    quoted = (ch == '\'');
    if (quoted && ! done) {ch = *(*treestrp)++; done = (ch == '\0');}

    while (! done) {
      if (quoted) {
        if (ch == '\'') {ch = *(*treestrp)++; if (ch != '\'') break;}
        }

      else if (treeLabelEnd(ch)) break;

      else if (ch == '_') ch = ' ';  /* unquoted _ goes to space */

      if (--maxlen >= 0) *lblPtr++ = ch;
      ch = *(*treestrp)++;
      if (ch == '\0') break;
      }

    (*treestrp)--;

    if (lblPtr != NULL) *lblPtr = '\0';

    return lblfound;
  } /* str_treeGetLabel */


boolean  str_treeFlushLabel (treestrp)
    char **treestrp;  /*  tree string pointer */
  { /* str_treeFlushLabel */
    return  str_treeGetLabel(treestrp, (char *) NULL, (int) 0);
  } /* str_treeFlushLabel */


int  str_treeFindTipName (treestrp, tr)
    char **treestrp;  /*  tree string pointer */
    tree  *tr;
  { /* str_treeFindTipName */
    nodeptr  q;
    char    *nameptr, str[nmlngth+2];
    int      ch, i, n;

    if (tr->prelabeled) {
      if (str_treeGetLabel(treestrp, str, nmlngth+2))
        n = treeFindTipByLabel(str, tr);
      else
        n = 0;
      }

    else if (tr->ntips < tr->mxtips) {
      n = tr->ntips + 1;
      nameptr = tr->nodep[n]->name;
      if (! str_treeGetLabel(treestrp, nameptr, nmlngth+1)) n = 0;
      }

    else {
      n = 0;
      }

    return  n;
  } /* str_treeFindTipName */


boolean str_treeProcessLength (treestrp, dptr)
    char   **treestrp;   /*  tree string ponter */
    double  *dptr;
  { /* str_treeProcessLength */
    int     used;

    if(! str_treeGetCh(treestrp))  return FALSE;    /*  Skip comments */
    (*treestrp)--;

    if (sscanf(*treestrp, "%lf%n", dptr, & used) != 1) {
      printf("ERROR: str_treeProcessLength: Problem reading branch length\n");
      printf("%40s\n", *treestrp);
      *dptr = 0.0;
      return FALSE;
      }
    else {
      *treestrp += used;
      }

    return  TRUE;
  } /* str_treeProcessLength */


boolean  str_treeFlushLen (treestrp)
    char **treestrp;   /*  tree string ponter */
  { /* str_treeFlushLen */
    int  ch;

    if ((ch = str_treeGetCh(treestrp)) == ':')
      return str_treeProcessLength(treestrp, (double *) NULL);
    else {
      (*treestrp)--;
      return TRUE;
      }
  } /* str_treeFlushLen */


boolean  str_treeNeedCh (treestrp, c1, where)
    char **treestrp;   /*  tree string pointer */
    int    c1;
    char  *where;
  { /* str_treeNeedCh */
    int  c2, i;

    if ((c2 = str_treeGetCh(treestrp)) == c1)  return TRUE;

    printf("ERROR: Missing '%c' %s tree; ", c1, where);
    if (c2 == '\0') 
      printf("end-of-string");
    else {
      putchar('"');
      for (i = 24; i-- && (c2 != '\0'); c2 = *(*treestrp)++)  putchar(c2);
      putchar('"');
      }

    printf(" found instead\n");
    return FALSE;
  } /* str_treeNeedCh */


boolean  str_addElementLen (treestrp, tr, p)
    char **treestrp;  /*  tree string pointer */
    tree    *tr;
    nodeptr  p;
  { /* str_addElementLen */
    double   z, branch;
    nodeptr  q;
    int      n, ch;

    if ((ch = str_treeGetCh(treestrp)) == '(') { /*  A new internal node */
      n = (tr->nextnode)++;
      if (n > 2*(tr->mxtips) - 2) {
        if (tr->rooted || n > 2*(tr->mxtips) - 1) {
          printf("ERROR: too many internal nodes.  Is tree rooted?\n");
          printf("Deepest splitting should be a trifurcation.\n");
          return  FALSE;
          }
        else {
          tr->rooted = TRUE;
          }
        }
      q = tr->nodep[n];
      if (! str_addElementLen(treestrp, tr, q->next))          return FALSE;
      if (! str_treeNeedCh(treestrp, ',', "in"))               return FALSE;
      if (! str_addElementLen(treestrp, tr, q->next->next))    return FALSE;
      if (! str_treeNeedCh(treestrp, ')', "in"))               return FALSE;
      if (! str_treeFlushLabel(treestrp))                      return FALSE;
      }

    else {                           /*  A new tip */
      n = str_treeFindTipName(treestrp, tr, ch);
      if (n <= 0) return FALSE;
      q = tr->nodep[n];
      if (tr->start->number > n)  tr->start = q;
      (tr->ntips)++;
      }                              /* End of tip processing */

    /*  Master and Slave always use lengths */

    if (! str_treeNeedCh(treestrp, ':', "in"))                 return FALSE;
    if (! str_treeProcessLength(treestrp, & branch))           return FALSE;
    z = exp(-branch / tr->rdta->fracchange);
    if (z > zmax)  z = zmax;
    hookup(p, q, z);

    return  TRUE;
  } /* str_addElementLen */


boolean str_processTreeCom(tr, treestrp)
    tree   *tr;
    char  **treestrp;
  { /* str_processTreeCom */
    char  *com, *com_end;
    int  text_started, functor_read, com_open;

    com = *treestrp;

    functor_read = text_started = 0;
    sscanf(com, " p%nhylip_tree(%n", & text_started, & functor_read);
    if (functor_read) {
      com += functor_read;
      }
    else if (text_started) {
      com += text_started;
      sscanf(com, "seudoNewick(%n", & functor_read);
      if (! functor_read) {
        printf("Start of tree 'p...' not understood.\n");
        return  FALSE;
        }
      else {
        com += functor_read;
        }
      }

    com_open = 0;
    sscanf(com, " [%n", & com_open);
    com += com_open;

    if (com_open) {                              /* comment; read it */
	if (!(com_end = strchr(com, ']'))) {
        printf("Missing end of tree comment.\n");
        return  FALSE;
        }

      *com_end = 0;
      (void) readKeyValue(com, likelihood_key, "%lg",
                               (void *) &(tr->likelihood));
      (void) readKeyValue(com, opt_level_key,  "%d",
                               (void *) &(tr->opt_level));
      (void) readKeyValue(com, smoothed_key,   "%d",
                               (void *) &(tr->smoothed));
      *com_end = ']';
      com_end++;

      if (functor_read) {                          /* remove trailing comma */
        text_started = 0;
        sscanf(com_end, " ,%n", & text_started);
        com_end += text_started;
        }

      *treestrp = com_end;
      }

    return (functor_read > 0);
  } /* str_processTreeCom */


boolean str_treeReadLen (treestr, tr)
    char  *treestr;
    tree  *tr;
    /* read string with representation of tree */
  { /* str_treeReadLen */
    nodeptr  p;
    int  i;
    boolean  is_fact, found;

    for (i = 1; i <= tr->mxtips; i++) tr->nodep[i]->back = (node *) NULL;
    tr->start       = tr->nodep[tr->mxtips];
    tr->ntips       = 0;
    tr->nextnode    = tr->mxtips + 1;
    tr->opt_level   = 0;
    tr->log_f_valid = 0;
    tr->smoothed    = Master;
    tr->rooted      = FALSE;

    is_fact = str_processTreeCom(tr, & treestr);

    p = tr->nodep[(tr->nextnode)++];
    if (! str_treeNeedCh(& treestr, '(', "at start of"))       return FALSE;
    if (! str_addElementLen(& treestr, tr, p))                 return FALSE;
    if (! str_treeNeedCh(& treestr, ',', "in"))                return FALSE;
    if (! str_addElementLen(& treestr, tr, p->next))           return FALSE;
    if (! tr->rooted) {
      if (str_treeGetCh(& treestr) == ',') {        /*  An unrooted format */
        if (! str_addElementLen(& treestr, tr, p->next->next)) return FALSE;
        }
      else {                                       /*  A rooted format */
        p->next->next->back = (nodeptr) NULL;
        tr->rooted = TRUE;
        treestr--;
        }
      }
    if (! str_treeNeedCh(& treestr, ')', "in"))                return FALSE;
    if (! str_treeFlushLabel(& treestr))                       return FALSE;
    if (! str_treeFlushLen(& treestr))                         return FALSE;
    if (is_fact) {
      if (! str_treeNeedCh(& treestr, ')', "at end of"))       return FALSE;
      if (! str_treeNeedCh(& treestr, '.', "at end of"))       return FALSE;
      }
    else {
      if (! str_treeNeedCh(& treestr, ';', "at end of"))       return FALSE;
      }

    if (tr->rooted)  if (! uprootTree(tr, p->next->next))     return FALSE;
    tr->start = p->next->next->back;  /* This is start used by treeString */

    return  (initrav(tr, tr->start) && initrav(tr, tr->start->back));
  } /* str_treeReadLen */
#endif


boolean treeEvaluate (tr, bt)         /* Evaluate a user tree */
    tree     *tr;
    bestlist *bt;
  { /* treeEvaluate */

    if (Slave || ! tr->userlen) {
      if (! smoothTree(tr, 4 * smoothings)) return FALSE;
      }

    if (evaluate(tr, tr->start) == badEval)  return FALSE;

#   if ! Slave
      (void) saveBestTree(bt, tr);
#   endif
    return TRUE;
  } /* treeEvaluate */


#if Master || Slave
FILE *freopen_pid (filenm, mode, stream)
    char *filenm, *mode;
    FILE *stream;
  { /* freopen_pid */
    char scr[512];

    (void) sprintf(scr, "%s.%d", filenm, getpid());
    return  freopen(scr, mode, stream);
  } /* freopen_pid */
#endif


boolean  showBestTrees (bt, tr, adef, treefile)
    bestlist *bt;
    tree     *tr;
    analdef  *adef;
    FILE     *treefile;
  { /* showBestTrees */
    int     rank;

    for (rank = 1; rank <= bt->nvalid; rank++) {
      if (rank > 1) {
        if (rank != recallBestTree(bt, rank, tr))  break;
        }
      if (evaluate(tr, tr->start) == badEval) return FALSE;
      if (tr->outgrnode->back)  tr->start = tr->outgrnode;
      printTree(tr, adef);
      summarize(tr);
      if (treefile)  treeOut(treefile, tr, adef->trout);
      }

    return TRUE;
  } /* showBestTrees */


boolean cmpBestTrees (bt, tr)
    bestlist *bt;
    tree     *tr;
  { /* cmpBestTrees */
    double  sum, sum2, sd, temp, wtemp, bestscore;
    double *log_f0, *log_f0_ptr;      /* Save a copy of best log_f */
    double *log_f_ptr;
    int     i, j, num, besttips;

    num = bt->nvalid;
    if ((num <= 1) || (tr->cdta->wgtsum <= 1)) return TRUE;

    if (! (log_f0 = (double *) Malloc(sizeof(double) * tr->cdta->endsite))) {
      printf("ERROR: cmpBestTrees unable to obtain space for log_f0\n");
      return FALSE;
      }

    printf("Tree      Ln L        Diff Ln L       Its S.D.");
    printf("   Significantly worse?\n\n");

    for (i = 1; i <= num; i++) {
      if (i != recallBestTree(bt, i, tr))  break;
      if (! (tr->log_f_valid))  {
        if (evaluate(tr, tr->start) == badEval) return FALSE;
        }

      printf("%3d%14.5f", i, tr->likelihood);
      if (i == 1) {
        printf("  <------ best\n");
        besttips = tr->ntips;
        bestscore = tr->likelihood;
        log_f0_ptr = log_f0;
        log_f_ptr  = tr->log_f;
        for (j = 0; j < tr->cdta->endsite; j++)  *log_f0_ptr++ = *log_f_ptr++;
        }
      else if (tr->ntips != besttips)
        printf("  (different number of species)\n");
      else {
        sum = sum2 = 0.0;
        log_f0_ptr = log_f0;
        log_f_ptr  = tr->log_f;
        for (j = 0; j < tr->cdta->endsite; j++) {
          temp  = *log_f0_ptr++ - *log_f_ptr++;
          wtemp = tr->cdta->aliaswgt[j] * temp;
          sum  += wtemp;
          sum2 += wtemp * temp;
          }
        sd = sqrt( tr->cdta->wgtsum * (sum2 - sum*sum / tr->cdta->wgtsum)
                                    / (tr->cdta->wgtsum - 1) );
        printf("%14.5f%14.4f", tr->likelihood - bestscore, sd);
        printf("           %s\n", (sum > 1.95996 * sd) ? "Yes" : " No");
        }
      }

    Free(log_f0);
    printf("\n\n");

    return TRUE;
  } /* cmpBestTrees */


boolean  makeUserTree (tr, bt, adef)
    tree     *tr;
    bestlist *bt;
    analdef  *adef;
  { /* makeUserTree */
    char   filename[128];
    FILE  *treefile;
    int    nusertrees, which;

    nusertrees = adef->numutrees;

    printf("User-defined %s:\n\n", (nusertrees == 1) ? "tree" : "trees");

    treefile = adef->trout ? fopen_pid("treefile", "w", filename) : (FILE *) NULL;

    for (which = 1; which <= nusertrees; which++) {
      if (! treeReadLen(INFILE, tr)) return FALSE;
      if (! treeEvaluate(tr, bt))    return FALSE;
      if (tr->global <= 0) {
        if (tr->outgrnode->back)  tr->start = tr->outgrnode;
        printTree(tr, adef);
        summarize(tr);
        if (treefile)  treeOut(treefile, tr, adef->trout);
        }
      else {
        printf("%6d:  Ln Likelihood =%14.5f\n", which, tr->likelihood);
        }
      }

    if (tr->global > 0) {
      putchar('\n');
      if (! recallBestTree(bt, 1, tr))  return FALSE;
      printf("      Ln Likelihood =%14.5f\n", tr->likelihood);
      if (! optimize(tr, tr->global, bt))  return FALSE;
      if (tr->outgrnode->back)  tr->start = tr->outgrnode;
      printTree(tr, adef);
      summarize(tr);
      if (treefile)  treeOut(treefile, tr, adef->trout);
      }

    if (treefile) {
      (void) fclose(treefile);
      printf("Tree also written to %s\n", filename);
      }

    putchar('\n');

    (void) cmpBestTrees(bt, tr);
    return TRUE;
  } /* makeUserTree */


#if Slave
boolean slaveTreeEvaluate (tr, bt)
    tree     *tr;
    bestlist *bt;
  { /* slaveTreeEvaluate */
    boolean done;

    do {
       requestForWork();
       if (! receiveTree(& comm_master, tr))        return FALSE;
       done = tr->likelihood > 0.0;
       if (! done) {
         if (! treeEvaluate(tr, bt))                return FALSE;
         if (! sendTree(& comm_master, tr))         return FALSE;
         }
       } while (! done);

    return TRUE;
  } /* slaveTreeEvaluate */
#endif


double randum (seed)
    long  *seed;
    /* random number generator, modified to use 12 bit chunks */
  { /* randum */
    long  sum, mult0, mult1, seed0, seed1, seed2, newseed0, newseed1, newseed2;

    mult0 = 1549;
    seed0 = *seed & 4095;
    sum  = mult0 * seed0;
    newseed0 = sum & 4095;
    sum >>= 12;
    seed1 = (*seed >> 12) & 4095;
    mult1 =  406;
    sum += mult0 * seed1 + mult1 * seed0;
    newseed1 = sum & 4095;
    sum >>= 12;
    seed2 = (*seed >> 24) & 255;
    sum += mult0 * seed2 + mult1 * seed1;
    newseed2 = sum & 255;

    *seed = newseed2 << 24 | newseed1 << 12 | newseed0;
    return  0.00390625 * (newseed2
                          + 0.000244140625 * (newseed1
                                              + 0.000244140625 * newseed0));
  } /* randum */


boolean makeDenovoTree (tr, bt, adef)
    tree     *tr;
    bestlist *bt;
    analdef  *adef;
  { /* makeDenovoTree */
    char   filename[128];
    FILE  *treefile;
    nodeptr  p;
    int  *enterorder;      /*  random entry order */
    int  i, j, k, nextsp, newsp, maxtrav, tested;

    double randum();


    enterorder = (int *) Malloc(sizeof(int) * (tr->mxtips + 1));
    if (! enterorder) {
       fprintf(stderr, "makeDenovoTree: Malloc failure for enterorder\n");
       return 0;
       }

    if (adef->restart) {
      printf("Restarting from tree with the following sequences:\n");
      tr->userlen = TRUE;
      if (! treeReadLen(INFILE, tr))          return FALSE;
      if (! smoothTree(tr, smoothings))       return FALSE;
      if (evaluate(tr, tr->start) == badEval) return FALSE;
      if (saveBestTree(bt, tr) < 1)           return FALSE;

      for (i = 1, j = tr->ntips; i <= tr->mxtips; i++) { /* find loose tips */
        if (! tr->nodep[i]->back) {
          enterorder[++j] = i;
          }
        else {
          printf("   %s\n", tr->nodep[i]->name);

#         if Master
            if (i>3) REPORT_ADD_SPECS;
#         endif
          }
        }
      putchar('\n');
      }

    else {                                           /* start from scratch */
      tr->ntips = 0;
      for (i = 1; i <= tr->mxtips; i++) enterorder[i] = i;
      }

    if (adef->jumble) for (i = tr->ntips + 1; i <= tr->mxtips; i++) {
      j = randum(&(adef->jumble))*(tr->mxtips - tr->ntips) + tr->ntips + 1;
      k = enterorder[j];
      enterorder[j] = enterorder[i];
      enterorder[i] = k;
      }

    bt->numtrees = 1;
    if (tr->ntips < tr->mxtips)  printf("Adding species:\n");

    if (tr->ntips == 0) {
      for (i = 1; i <= 3; i++) {
        printf("   %s\n", tr->nodep[enterorder[i]]->name);
        }
      tr->nextnode = tr->mxtips + 1;
      if (! buildSimpleTree(tr, enterorder[1], enterorder[2], enterorder[3]))
        return FALSE;
      }

    while (tr->ntips < tr->mxtips || tr->opt_level < tr->global) {
      maxtrav = (tr->ntips == tr->mxtips) ? tr->global : tr->partswap;
      if (maxtrav > tr->ntips - 3)  maxtrav = tr->ntips - 3;

      if (tr->opt_level >= maxtrav) {
        nextsp = ++(tr->ntips);
        newsp = enterorder[nextsp];
        p = tr->nodep[newsp];
        printf("   %s\n", p->name);

#       if Master
          if (nextsp % DNAML_STEP_TIME_COUNT == 1) {
            REPORT_STEP_TIME;
            }
          REPORT_ADD_SPECS;
#       endif

        (void) buildNewTip(tr, p);

        resetBestTree(bt);
        cacheZ(tr);
        tested = addTraverse(tr, p->back, findAnyTip(tr->start)->back,
                             1, tr->ntips - 2, bt, adef->qadd);
        if (tested == badRear) return FALSE;
        bt->numtrees += tested;

#       if Master
          getReturnedTrees(tr, bt, tested);
#       endif

        printf("      Tested %d alternative trees\n", tested);

        (void) recallBestTree(bt, 1, tr);
        if (! tr->smoothed) {
          if (! smoothTree(tr, smoothings))        return FALSE;
          if (evaluate(tr, tr->start) == badEval)  return FALSE;
          (void) saveBestTree(bt, tr);
          }

        if (tr->ntips == 4)  tr->opt_level = 1;  /* All 4 taxon trees done */
        maxtrav = (tr->ntips == tr->mxtips) ? tr->global : tr->partswap;
        if (maxtrav > tr->ntips - 3)  maxtrav = tr->ntips - 3;
        }

      printf("      Ln Likelihood =%14.5f\n", tr->likelihood);
      if (! optimize(tr, maxtrav, bt)) return FALSE;
      }

    printf("\nExamined %d %s\n", bt->numtrees,
                                 bt->numtrees != 1 ? "trees" : "tree");

    treefile = adef->trout ? fopen_pid("treefile", "w", filename) : (FILE *) NULL;
    (void) showBestTrees(bt, tr, adef, treefile);
    if (treefile) {
      (void) fclose(treefile);
      printf("Tree also written to %s\n\n", filename);
      }

    (void) cmpBestTrees(bt, tr);

#   if DeleteCheckpointFile
      unlink_pid(checkpointname);
#   endif

    Free(enterorder);

    return TRUE;
  } /* makeDenovoTree */

/*==========================================================================*/
/*                             "main" routine                               */
/*==========================================================================*/

#if defined(MACINTOSH) || defined(MSDOS)
int RealMain(int argc, char** argv)
#else
#if Sequential
  main ()
#else
  slave ()
#endif
#endif
  { /* DNA Maximum Likelihood */
#   if Master
      int starttime, inputtime, endtime;
#   endif

#   if Sequential
		/* dgg addition */
      long starttime, inputtime, endtime;
      double millisecs;
#   endif

#   if Master || Slave
      int my_id, nprocs, type, from, sz;
      char *msg;
#   endif

    analdef      *adef;
    rawDNA       *rdta;
    crunchedDNA  *cdta;
    tree         *tr;    /*  current tree */
    bestlist     *bt;    /*  topology of best found tree */


#   if Debug
      {
        char debugfilename[128];
        debug = fopen_pid("dnaml_debug", "w", debugfilename);
        }
#   endif

#   if Master
      starttime = p4_clock();
      nprocs = p4_num_total_slaves();

      if ((OUTFILE = freopen_pid("master.out", "w", stdout)) == NULL) {
        fprintf(stderr, "Could not open output file\n");
        exit(1);
        }

      /* Receive input file name from host */
      type = DNAML_FILE_NAME;
      from = DNAML_HOST_ID;
      msg  = NULL;
      p4_recv(& type, & from, & msg, & sz);
      if ((INFILE = fopen(msg, "r")) == NULL) {
        fprintf(stderr, "master could not open input file %s\n", msg);
        exit(1);
        }
      p4_msg_free(msg);

      open_link(& comm_slave);
#   endif

#   if Sequential
      starttime = clock();
#ifdef NoSTDIN
			{
			char *fname = "infile";
      if ((INFILE = fopen(fname, "r")) == NULL) 
      do {
      	char filename[512];
      	filename[0]= 0;
        fprintf(stderr, "Could not open input file '%s'\n", fname);
        fprintf(stderr, "Enter path-name for data file: ");
        scanf( "%s", filename);
        fname= filename;
     	  INFILE = fopen(fname, "r");
      } while (!INFILE && *fname > ' ');
      if (INFILE == NULL) {
     		fprintf(stderr, "Could not open input file '%s'\n", fname);
    		exit(1);
      	}
      }
#endif
#		endif

#  if DNAML_STEP
      begin_step_time = starttime;
#  endif

#   if Slave
      my_id = p4_get_my_id();
      nprocs = p4_num_total_slaves();

      /* Receive input file name from host */
      type = DNAML_FILE_NAME;
      from = DNAML_HOST_ID;
      msg  = NULL;
      p4_recv(& type, & from, & msg, & sz);
      if ((INFILE = fopen(msg, "r")) == NULL) {
        fprintf(stderr, "slave could not open input file %s\n",msg);
        exit(1);
        }
      p4_msg_free(msg);

#     ifdef P4DEBUG
        if ((OUTFILE = freopen_pid("slave.out", "w", stdout)) == NULL) {
          fprintf(stderr, "Could not open output file\n");
          exit(1);
          }
#     else
        if ((OUTFILE = freopen("/dev/null", "w", stdout)) == NULL) {
          fprintf(stderr, "Could not open output file\n");
          exit(1);
          }
#     endif

      open_link(& comm_master);
#   endif


/*  Get data structure memory  */

    if (! (adef = (analdef *) Malloc(sizeof(analdef)))) {
      printf("ERROR: Unable to get memory for analysis definition\n\n");
      return 1;
      }

    if (! (rdta = (rawDNA *) Malloc(sizeof(rawDNA)))) {
      printf("ERROR: Unable to get memory for raw DNA\n\n");
      return 1;
      }

    if (! (cdta = (crunchedDNA *) Malloc(sizeof(crunchedDNA)))) {
      printf("ERROR: Unable to get memory for crunched DNA\n\n");
      return 1;
      }

    if ((tr = (tree *)     Malloc(sizeof(tree))) &&
        (bt = (bestlist *) Malloc(sizeof(bestlist)))) ;
    else {
      printf("ERROR: Unable to get memory for trees\n\n");
      return 1;
      }
    bt->ninit = 0;

    if (! getinput(adef, rdta, cdta, tr))                            return 1;

#   if Master
      inputtime = p4_clock();
      printf("Input time %d milliseconds\n", inputtime - starttime);
      REPORT_STEP_TIME;
#   endif

#   if Slave
      (void) fclose(INFILE);
#   endif

#   if Sequential
      inputtime = clock();
      millisecs= TIMETOSECS(inputtime - starttime);
      printf("Input time %6.4f seconds\n",millisecs);
#   ifdef NoSTDIN
      (void) fclose(INFILE);
#   endif
#   endif


/*  The material below would be a loop over jumbles and/or boots */

    if (! makeweights(adef, rdta, cdta))                             return 1;
    if (! makevalues(rdta, cdta))                                    return 1;
    if (adef->empf && ! empiricalfreqs(rdta, cdta))                  return 1;
    reportfreqs(adef, rdta);
    if (! linkdata2tree(rdta, cdta, tr))                             return 1;

    if (! linkxarray(3, 3, cdta->endsite, & freextip, & usedxtip))   return 1;
    if (! setupnodex(tr))                                            return 1;

#   if Slave
      if (! slaveTreeEvaluate(tr, bt))                               return 1;
#   else
      if (! initBestTree(bt, adef->nkeep, tr->mxtips, tr->cdta->endsite))    return 1;
      if (! adef->usertree) {
        if (! makeDenovoTree(tr, bt, adef))                          return 1;
        }
      else {
        if (! makeUserTree(tr, bt, adef))                            return 1;
        }
      if (! freeBestTree(bt))                                        return 1;
#   endif

/*  Endpoint for jumble and/or boot loop */

#   if Master
      tr->likelihood = 1.0;             /* terminate slaves */
      (void) sendTree(& comm_slave, tr);
#   endif

    freeTree(tr);

#   if Master
      close_link(& comm_slave);
      (void) fclose(INFILE);

      REPORT_STEP_TIME;
      endtime = p4_clock();
      printf("Execution time %d milliseconds\n", endtime - inputtime);
      (void) fclose(OUTFILE);
#   endif

#   if Slave
      close_link(& comm_master);
      (void) fclose(OUTFILE);
#   endif

#   if Sequential
		/* dgg addition */
      endtime = clock();
      millisecs= TIMETOSECS(endtime - inputtime);
      printf("Execution time %6.4f seconds\n", millisecs);
#   endif

#   if Debug
      (void) fclose(debug);
#   endif

#   if Master || Slave
      p4_send(DNAML_DONE, DNAML_HOST_ID, NULL, 0);
#   else
      return 0;
#   endif
  } /* DNA Maximum Likelihood */

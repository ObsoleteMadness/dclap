#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include "clustalw.h"

/*
 *   Prototypes
 */
extern void *ckalloc(size_t bytes);
extern void ckfree(void *);

void calc_seq_weights(int first_seq, int last_seq);
void create_sets(int first_seq, int last_seq);
int calc_similarities(int nseqs);
int calc_distances(int nseqs);
int read_tree(char *treefile, int first_seq, int last_seq);

void skip_space(FILE *fd);
treeptr avail(void);
void clear_tree(treeptr p);
void set_info(treeptr p, treeptr parent, int pleaf, char *pname, float pdist);
treeptr reroot(treeptr ptree);
treeptr insert_root(treeptr p, float diff);
float calc_root_mean(treeptr root, float *maxdist);
float calc_mean(treeptr root, treeptr nptr, float *maxdist);
void create_tree(treeptr ptree, treeptr parent);
void create_node(treeptr pptr, treeptr parent);
treeptr insert_node(treeptr pptr);
void order_nodes(void);
int calc_weight(int leaf);
void group_seqs(treeptr root, int *groups, int nseqs);
void save_set(int n, int *groups);
void mark_group1(treeptr p, int *groups, int n);
void mark_group2(treeptr p, int *groups, int n);

/*
 *   Global variables
 */

extern Boolean distance_tree;
extern int debug;
extern double **tmat;
extern int **sets;
extern int nsets;
extern char **names;
extern int *seq_weight;

char ch;
FILE *fd;
treeptr lptr[MAXN];
treeptr olptr[MAXN];
treeptr nptr[MAXTREE];
treeptr ptrs[MAXTREE];
int nnodes = 0;
int ntotal = 0;
int rooted_tree = TRUE;
static treeptr seq_tree,root;
static int *groups, numseq;

void calc_seq_weights(int first_seq, int last_seq)
{
  int   i, j, k, nseqs;
  int   temp, sum, *weight;


/*
  If there are more than three sequences....
*/
  nseqs = last_seq-first_seq;
  if ((nseqs > 3) && (distance_tree == TRUE))
     {
/*
  Calculate sequence weights based on Phylip tree.
*/
      weight = (int *)ckalloc((nseqs+1) * sizeof(int));

      for (i=first_seq; i<last_seq; i++)
           weight[i] = calc_weight(i);

/*
  Normalise the weights, such that the sum of the weights = 1000
*/

         sum = 0;
         for (i=first_seq; i<last_seq; i++)
            sum += weight[i];

         if (sum == 0)
          {
            for (i=first_seq; i<last_seq; i++)
               weight[i] = 1;
            sum = i;
          }

         for (i=first_seq; i<last_seq; i++)
           {
              seq_weight[i] = (weight[i] * 1000) / sum;
              if (seq_weight[i] < 1) seq_weight[i] = 1;
           }

       ckfree((void *)weight);

     }

   else
     {
/*
  Otherwise, use identity weights.
*/
        temp = 1000 / nseqs;
        for (i=first_seq; i<last_seq; i++)
           seq_weight[i] = temp;
     }

}

void create_sets(int first_seq, int last_seq)
{
  int   i, j, k, nseqs;

  nsets = 0;
  nseqs = last_seq-first_seq;
  if (nseqs > 3)
     {
/*
  If there are more than three sequences....
*/
       groups = (int *)ckalloc((nseqs+1) * sizeof(int));
       group_seqs(root, groups, nseqs);
       ckfree((void *)groups);

     }

   else
     {
       groups = (int *)ckalloc((nseqs+1) * sizeof(int));
       for (i=0;i<nseqs-1;i++)
         {
           for (j=0;j<nseqs;j++)
              if (j<=i) groups[j] = 1;
              else if (j==i+1) groups[j] = 2;
              else groups[j] = 0;
           save_set(nseqs, groups);
         }
       ckfree((void *)groups);
     }

}

int read_tree(char *treefile, int first_seq, int last_seq)
{

  char c;
  char name1[MAXNAMES+1], name2[MAXNAMES+1];
  int i, j, k, n, value, rootnode, rootdirection;
  int found;

  numseq = 0;
  nnodes = 0;
  ntotal = 0;
  rooted_tree = TRUE;

#ifdef VMS
  if ((fd = fopen(treefile,"r","rat=cr","rfm=var")) == NULL)
#else
  if ((fd = fopen(treefile, "r")) == NULL)
#endif
    {
      fprintf(stderr, "Error: cannot open %s\n", treefile);
      return(0);
    }

  skip_space(fd);
  ch = (char)getc(fd);
  if (ch != '(')
    {
      fprintf(stderr, "Error: Wrong format in tree file %s\n", treefile);
      return(0);
    }
  rewind(fd);

  distance_tree = TRUE;

  seq_tree = avail();
  set_info(seq_tree, NULL, 0, "", 0.0);

  create_tree(seq_tree,NULL);
  fclose(fd);

/*
  If the tree is unrooted, reroot the tree - ie. minimise the difference
  between the mean root->leaf distances for the left and right branches of
  the tree.
*/

  if (distance_tree == FALSE)
     {
  	if (rooted_tree == FALSE)
          {
       	     fprintf(stderr,
             "Error: input tree is unrooted and has no distances, cannot align sequences\n");
             return(0);
          }
     }

  if (rooted_tree == FALSE)
     {
        root = reroot(seq_tree);
     }
  else
     {
        root = seq_tree;
     }

/*
  calculate the 'order' of each node.
*/
  order_nodes();


  if (numseq != last_seq-first_seq)
     {
         fprintf(stderr," Error: tree not compatible with alignment (%d sequences in alignment and %d in tree\n",
         (pint)last_seq-first_seq,(pint)numseq);
         return(0);
     }

  if (numseq > 3)
     {
/*
  If there are more than three sequences....
*/
/*
  assign the sequence nodes (in the same order as in the alignment file)
*/
      for (i=first_seq; i<last_seq; i++)
       {
         if (strlen(names[i+1]) > MAXNAMES)
             fprintf(stderr,"Warning: name %s is too long for PHYLIP tree format (max 10 chars)\n", names[i+1]);

         for (k=0; k< strlen(names[i+1]) && k<MAXNAMES ; k++)
           {
             c = names[i+1][k];
             if ((c>0x40) && (c<0x5b)) c=c | 0x20;
             if (c == ' ') c = '_';
             name2[k] = c;
           }
         name2[k]='\0';
         found = FALSE;
         for (j=0; j<numseq; j++)
           {
            for (k=0; k< strlen(lptr[j]->name) && k<MAXNAMES ; k++)
              {
                c = lptr[j]->name[k];
                if ((c>0x40) && (c<0x5b)) c=c | 0x20;
                name1[k] = c;
              }
            name1[k]='\0';
            if (strcmp(name1, name2) == 0)
              {
                olptr[i] = lptr[j];
                found = TRUE;
              }
           }
         if (found == FALSE)
           {
             fprintf(stderr," Error: tree not compatible with alignment: %s not found\n",
                                      name2);
             return(0);
           }
       }

     }
   return(1);
}

void create_tree(treeptr ptree, treeptr parent)
{
   treeptr p;

   int i, type;
   float dist;
   char name[MAXNAMES+1];

/*
  is this a node or a leaf ?
*/
  skip_space(fd);
  ch = (char)getc(fd);
  if (ch == '(')
    {  
/*
   this must be a node....
*/
      type = NODE;
      name[0] = '\0';
      ptrs[ntotal] = nptr[nnodes] = ptree;
      nnodes++;
      ntotal++;

      create_node(ptree, parent);

      p = ptree->left;
      create_tree(p, ptree);
           
      if ( ch == ',')
       {
          p = ptree->right;
          create_tree(p, ptree);
          if ( ch == ',')
            {
               ptree = insert_node(ptree);
               ptrs[ntotal] = nptr[nnodes] = ptree;
               nnodes++;
               ntotal++;
               p = ptree->right;
               create_tree(p, ptree);
               rooted_tree = FALSE;
            }
       }

      skip_space(fd);
      ch = (char)getc(fd);
    }
/*
   ...otherwise, this is a leaf
*/
  else
    {
      type = LEAF;
      ptrs[ntotal++] = lptr[numseq++] = ptree;
/*
   get the sequence name
*/
      name[0] = ch;
      ch = (char)getc(fd);
      i = 1;
      while ((ch != ':') && (ch != ',') && (ch != ')'))
        {
          if (i < MAXNAMES) name[i++] = ch;
          ch = (char)getc(fd);
        }
      name[i] = '\0';
      if (ch != ':')
         {
           distance_tree = FALSE;
           dist = 0.0;
         }
    }

/*
   get the distance information
*/
  dist = 0.0;
  if (ch == ':')
     {
       skip_space(fd);
       fscanf(fd,"%f",&dist);
       skip_space(fd);
       ch = (char)getc(fd);
     }
   set_info(ptree, parent, type, name, dist);


}

void create_node(treeptr pptr, treeptr parent)
{
  treeptr t;

  pptr->parent = parent;
  t = avail();
  pptr->left = t;
  t = avail();
  pptr->right = t;
    
}

treeptr insert_node(treeptr pptr)
{

   treeptr newnode;

   newnode = avail();
   create_node(newnode, pptr->parent);

   newnode->left = pptr;
   pptr->parent = newnode;

   set_info(newnode, pptr->parent, NODE, "", 0.0);

   return(newnode);
}

void skip_space(FILE *fd)
{
  int   c;
  
  do
     c = getc(fd);
  while(isspace(c));

  ungetc(c, fd);
}

treeptr avail(void)
{
   treeptr p;
   p = ckalloc(sizeof(stree));
   p->left = NULL;
   p->right = NULL;
   p->parent = NULL;
   p->dist = 0.0;
   p->leaf = 0;
   p->order = 0;
   p->name[0] = '\0';
   return(p);
}

void clear_tree(treeptr p)
{
   if (p==NULL) p = root;
   if (p->left != NULL)
     {
       clear_tree(p->left);
     }
   if (p->right != NULL)
     {
       clear_tree(p->right);
     }
   p->left = NULL;
   p->right = NULL;
   ckfree((void *)p);
}

void set_info(treeptr p, treeptr parent, int pleaf, char *pname, float pdist)
{
   p->parent = parent;
   p->leaf = pleaf;
   p->dist = pdist;
   p->order = 0;
   strcpy(p->name, pname);
   if (p->leaf == TRUE)
     {
        p->left = NULL;
        p->right = NULL;
     }
}

treeptr reroot(treeptr ptree)
{

   treeptr p,q,t, rootnode, rootptr;
   float   lmean, rmean;
   float   dist, diff, mindiff, mindepth = 1.0, maxdist;
   int     i,j;
   int     first = TRUE;

/*
  find the difference between the means of leaf->node
  distances on the left and on the right of each node
*/
   rootptr = ptree;
   for (i=0; i<ntotal; i++)
     {
        p = ptrs[i];
        if (p->parent == NULL)
           diff = calc_root_mean(p, &maxdist);
        else
           diff = calc_mean(ptree, p, &maxdist);

        if ((diff == 0) || ((diff > 0) && (diff < 2 * p->dist)))
          {
              if ((maxdist < mindepth) || (first == TRUE))
                 {
                    first = FALSE;
                    rootptr = p;
                    mindepth = maxdist;
                    mindiff = diff;
                 }
           }

     }

/*
  insert a new node as the ancestor of the node which produces the shallowest
  tree.
*/
   if (rootptr == ptree)
     {
        mindiff = rootptr->left->dist + rootptr->right->dist;
        rootptr = rootptr->right;
     }
   rootnode = insert_root(rootptr, mindiff);
  
   diff = calc_root_mean(rootnode, &maxdist);

   return(rootnode);
}

treeptr insert_root(treeptr p, float diff)
{
   treeptr newp, prev, prevp, q, t;
   float dist, prevdist, prevdiff, td;

   newp = avail();

   t = p->parent;
   prevdist = t->dist;

   p->parent = newp;

   dist = p->dist;

   p->dist = diff / 2;
   if (p->dist < 0.0) p->dist = 0.0;
   if (p->dist > dist) p->dist = dist;

   t->dist = dist - p->dist; 

   newp->left = t;
   newp->right = p;
   newp->parent = NULL;
   newp->dist = 0.0;
   newp->leaf = NODE;

   if (t->left == p) t->left = t->parent;
   else t->right = t->parent;

   prev = t;
   q = t->parent;

   t->parent = newp;

   while (q != NULL)
     {
        if (q->left == prev)
           {
              q->left = q->parent;
              q->parent = prev;
              td = q->dist;
              q->dist = prevdist;
              prevdist = td;
              prev = q;
              q = q->left;
           }
        else
           {
              q->right = q->parent;
              q->parent = prev;
              td = q->dist;
              q->dist = prevdist;
              prevdist = td;
              prev = q;
              q = q->right;
           }
    }

/*
   remove the old root node
*/
   q = prev;
   if (q->left == NULL)
      {
         dist = q->dist;
         q = q->right;
         q->dist += dist;
         q->parent = prev->parent;
         if (prev->parent->left == prev)
            prev->parent->left = q;
         else
            prev->parent->right = q;
         prev->right = NULL;
      }
   else
      {
         dist = q->dist;
         q = q->left;
         q->dist += dist;
         q->parent = prev->parent;
         if (prev->parent->left == prev)
            prev->parent->left = q;
         else
            prev->parent->right = q;
         prev->left = NULL;
      }

   return(newp);
}

float calc_root_mean(treeptr root, float *maxdist)
{
   float dist , lsum = 0.0, rsum = 0.0, lmean,rmean,diff;
   treeptr p;
   int depth = 0, i,j , n;
   int nl, nr;
   int direction, found;
/*
   for each leaf, determine whether the leaf is left or right of the root.
*/
   dist = (*maxdist) = 0;
   nl = nr = 0;
   for (i=0; i< numseq; i++)
     {
         p = lptr[i];
         dist = 0.0;
         while (p->parent != root)
           {
               dist += p->dist;
               p = p->parent;
           }
         if (p == root->left) direction = LEFT;
         else direction = RIGHT;
         dist += p->dist;

         if (direction == LEFT)
           {
             lsum += dist;
             nl++;
           }
         else
           {
             rsum += dist;
             nr++;
           }
        if (dist > (*maxdist)) *maxdist = dist;
     }

   lmean = lsum / nl;
   rmean = rsum / nr;

   diff = lmean - rmean;
   return(diff);
}


float calc_mean(treeptr root, treeptr nptr, float *maxdist)
{
   float dist , lsum = 0.0, rsum = 0.0, lmean,rmean,diff;
   treeptr p, path2root[MAXN];
   float dist2node[MAXN];
   int depth = 0, i,j , n;
   int nl , nr;
   int direction, found;

/*
   determine all nodes between the selected node and the root;
*/
   depth = (*maxdist) = dist = 0;
   nl = nr = 0;
   p = nptr;
   while (p != NULL)
     {
         path2root[depth] = p;
         dist += p->dist;
         dist2node[depth] = dist;
         p = p->parent;
         depth++;
     }
 
/*
   *nl = *nr = 0;
   for each leaf, determine whether the leaf is left or right of the node.
   (RIGHT = descendant, LEFT = not descendant)
*/
   for (i=0; i< numseq; i++)
     {
       p = lptr[i];
       if (p == nptr)
         {
            direction = RIGHT;
            dist = 0.0;
         }
       else
         {
         direction = LEFT;
         dist = 0.0;
/*
   find the common ancestor.
*/
         found = FALSE;
         n = 0;
         while ((found == FALSE) && (p->parent != NULL))
           {
               for (j=0; j< depth; j++)
                 if (p->parent == path2root[j])
                    { 
                      found = TRUE;
                      n = j;
                    }
               dist += p->dist;
               p = p->parent;
           }
         if (p == nptr) direction = RIGHT;
         }

         if (direction == LEFT)
           {
             lsum += dist;
             lsum += dist2node[n-1];
             nl++;
           }
         else
           {
             rsum += dist;
             nr++;
           }

        if (dist > (*maxdist)) *maxdist = dist;
     }

   lmean = lsum / nl;
   rmean = rsum / nr;
   
   diff = lmean - rmean;
   return(diff);
}

void order_nodes(void)
{
   int i;
   treeptr p;

   for (i=0; i<numseq; i++)
     {
        p = lptr[i];
        while (p != NULL)
          {
             p->order++;
             p = p->parent;
          }
     }
}


int calc_weight(int leaf)
{

  treeptr p;
  float weight = 0.0;

  p = olptr[leaf];
  while (p->parent != NULL)
    {
       weight += p->dist / p->order;
       p = p->parent;
    }

  weight *= 100.0;

  return((int)weight);

}

void group_seqs(treeptr p, int *next_groups, int nseqs)
{
    int i, n;
    int *tmp_groups;

    tmp_groups = (int *)ckalloc((nseqs+1) * sizeof(int));
    for (i=0;i<nseqs;i++)
         tmp_groups[i] = 0;

    if (p->left != NULL)
      {
         if (p->left->leaf == NODE)
            {
               group_seqs(p->left, next_groups, nseqs);
               for (i=0;i<nseqs;i++)
                 if (next_groups[i] != 0) tmp_groups[i] = 1;
            }
         else
            {
               mark_group1(p->left, tmp_groups, nseqs);
            }
               
      }

    if (p->right != NULL)
      {
         if (p->right->leaf == NODE)
            {
               group_seqs(p->right, next_groups, nseqs);
               for (i=0;i<nseqs;i++)
                    if (next_groups[i] != 0) tmp_groups[i] = 2;
            }
         else 
            {
               mark_group2(p->right, tmp_groups, nseqs);
            }
         save_set(nseqs, tmp_groups);
      }
    for (i=0;i<nseqs;i++)
      next_groups[i] = tmp_groups[i];

    ckfree((void *)tmp_groups);

}

void mark_group1(treeptr p, int *groups, int n)
{
    int i;

    for (i=0;i<n;i++)
       {
         if (olptr[i] == p)
              groups[i] = 1;
         else
              groups[i] = 0;
       }
}

void mark_group2(treeptr p, int *groups, int n)
{
    int i;

    for (i=0;i<n;i++)
       {
         if (olptr[i] == p)
              groups[i] = 2;
         else if (groups[i] != 0)
              groups[i] = 1;
       }
}

void save_set(int n, int *groups)
{
    int i;

    for (i=0;i<n;i++)
      sets[nsets+1][i+1] = groups[i];
    nsets++;
}



int calc_similarities(int nseqs)
{
   int depth = 0, i,j, k, n;
   int direction, found;
   treeptr p, *path2root;
   float dist , sum = 0.0;
   float *dist2node;
   double **dmat;

   path2root = (treeptr *)ckalloc((nseqs) * sizeof(treeptr));
   dist2node = (float *)ckalloc((nseqs) * sizeof(float));
   dmat = (double **)ckalloc((nseqs) * sizeof(double *));
   for (i=0;i<nseqs;i++)
     dmat[i] = (double *)ckalloc((nseqs) * sizeof(double));

   if (nseqs > 3)
    {
/*
   for each leaf, determine all nodes between the leaf and the root;
*/
      for (i = 0;i<nseqs; i++)
       { 
          depth = dist = 0;
          p = olptr[i];
          while (p != NULL)
            {
                path2root[depth] = p;
                dist += p->dist;
                dist2node[depth] = dist;
                p = p->parent;
                depth++;
            }
 
/*
   for each pair....
*/
          for (j=0; j < i; j++)
            {
              p = olptr[j];
              dist = 0.0;
/*
   find the common ancestor.
*/
              found = FALSE;
              n = 0;
              while ((found == FALSE) && (p->parent != NULL))
                {
                    for (k=0; k< depth; k++)
                      if (p->parent == path2root[k])
                         { 
                           found = TRUE;
                           n = k;
                         }
                    dist += p->dist;
                    p = p->parent;
                }
   
              dmat[i][j] = dist + dist2node[n-1];
            }
        }

        dist = 0.0;
        for (i=0;i<nseqs;i++)
          {
             dmat[i][i] = 0.0;
             for (j=0;j<i;j++)
               {
                  if (dmat[i][j] < 0.01) dmat[i][j] = 0.01;
                  if ((dmat[i][j] > 1.0) && (dmat[i][j] < 1.1))
                          dmat[i][j] = 1.0;
                  if (dmat[i][j] > dist) dist = dmat[i][j];
               }
          }
        if (dist > 1.01) 
          {
             fprintf(stderr,"Warning: distances in tree are out of range (all distances must be between 0.0 and 1.0 (biggest dist is %f))\n",dist);
             return(0);
          }
     }
   else
     {
        for (i=0;i<nseqs;i++)
          {
             for (j=0;j<i;j++)
               {
                  dmat[i][j] = tmat[i+1][j+1];
               }
          }
     }

   ckfree((void *)path2root);
   ckfree((void *)dist2node);
   for (i=0;i<nseqs;i++)
     {
        tmat[i][i] = 0.0;
        for (j=0;j<i;j++)
          {
             tmat[i+1][j+1] = 100.0 - (dmat[i][j]) * 100.0;
             tmat[j+1][i+1] = tmat[i+1][j+1];
          }
     }

   for (i=0;i<nseqs;i++) ckfree((void *)dmat[i]);
   ckfree((void *)dmat);

   return(1);
}

int calc_distances(int nseqs)
{
   int depth = 0, i,j, k, n;
   int direction, found;
   treeptr p, *path2root;
   float dist , sum = 0.0;
   float *dist2node;

   path2root = (treeptr *)ckalloc((nseqs) * sizeof(treeptr));
   dist2node = (float *)ckalloc((nseqs) * sizeof(float));

   if (nseqs > 3)
    {
/*
   for each leaf, determine all nodes between the leaf and the root;
*/
      for (i = 0;i<nseqs; i++)
       { 
          depth = dist = 0;
          p = olptr[i];
          while (p != NULL)
            {
                path2root[depth] = p;
                dist += p->dist;
                dist2node[depth] = dist;
                p = p->parent;
                depth++;
            }
 
/*
   for each pair....
*/
          for (j=0; j < i; j++)
            {
              p = olptr[j];
              dist = 0.0;
/*
   find the common ancestor.
*/
              found = FALSE;
              n = 0;
              while ((found == FALSE) && (p->parent != NULL))
                {
                    for (k=0; k< depth; k++)
                      if (p->parent == path2root[k])
                         { 
                           found = TRUE;
                           n = k;
                         }
                    dist += p->dist;
                    p = p->parent;
                }
   
              tmat[i+1][j+1] = dist + dist2node[n-1];
            }
        }

        dist = 0.0;
        for (i=0;i<nseqs;i++)
          {
             tmat[i+1][i+1] = 0.0;
             for (j=0;j<i;j++)
               {
                  if (tmat[i+1][j+1] < 0.01) tmat[i+1][j+1] = 0.01;
                  if (tmat[i+1][j+1] > dist) dist = tmat[i+1][j+1];
                  tmat[j+1][i+1] = tmat[i+1][j+1];
               }
          }
        if (dist > 1.0) 
          {
             fprintf(stderr,"Warning: distances in tree are out of range (all distances must be between 0.0 and 1.0)\n");
             return(0);
          }
     }

   ckfree((void *)path2root);
   ckfree((void *)dist2node);

   return(1);
}

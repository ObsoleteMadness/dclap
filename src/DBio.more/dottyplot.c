/* dottyplot.c -- c version of dotplot routine 

	by d.g.gilbert May 1989
	MWP-C compiler
	
	!! revise to do line segments along diagonals
		that have contiguous points
	! add event check to cancel loop if user is leaning
		on cmd-. key  (see other MPW tool samples)
		
--------------------------------------*/

#include <Values.h>
#include <Types.h>
#include <QuickDraw.h>
#include <ToolUtils.h>
#include <Memory.h>
#include <OSUtils.h>
#include	<CursorCtl.h>


#define min(a,b)  (a<b)?a:b
#define max(a,b)	(a>b)?a:b

		
			
pascal void windowpt( char *vseq, unsigned short nv, 
											char *hseq, unsigned short nh,
											unsigned short window, 
											unsigned short nmatch, 
											char doself, 
											char allpts, 
											long **pich) 
	{
		register char *matchp, *hp, *vp;
		register int	i;
		char		*matchvec;
		int  		matches, diag, xstart, ystart, yfinish, halfwind, 
				  	h, v, ndiag, j, lasti, n2;
		long		npic, maxpic;

 
/************ !!?? define don't work !!
#define dodot(k)		{ 			\
		if (npic >= maxpic) {		\ 
			maxpic += 100;  			\
			SetHandleSize((Handle)pich, maxpic*sizeof(long)); \
			if (MemError() != 0) goto done;	\
			}		\
		(*pich)[npic++] = (h+k) + ((v-k)<<16); \	 
		}
 ***********/
		
		/* InitCursorCtl(nil); -- do in caller */
		matchvec = NewPtr( nv+window+10);
		n2= nh+nv-1;
 		halfwind = window >> 1;
		
		maxpic	= GetHandleSize((Handle)pich) / sizeof(long);
		npic		= 0;
		
    			/* Compare for each register shift (diagonal) */
		if (doself) ndiag= nv; else ndiag = n2;
		for (diag=1; diag<=ndiag; diag++) {
			RotateCursor(diag);
		  ystart=  max(0, nv-diag);
			yfinish= min( nv-1, n2-diag);
			xstart=  ystart - nv + diag;
			
					/* Compare the two sequences along this diagonal
						and fill in matchVec while clearing pointVec */
			matchp= matchvec;
			hp= &(hseq[xstart]); 
			vp= &(vseq[ystart]);  
			for (i= ystart; i<= yfinish; i++) {
			  *(matchp++)= (*(hp++) == *(vp++));
				}	
			
		 /*!!! ^^^^^ revise this & data store to find/keep line segments !!! */
		 /* if matchp[i] && matchp[i-1] ... */
		 
      			/* clear a window's width at the end */
		 	for (i= 0; i<window; i++) *(matchp++)= 0;		
			
						/* Count the number of matches in the first window */
			for (i=0, matchp= matchvec, matches=0; i<window; i++) {
				if (*(matchp++)) matches++;
				}
				
					/* step window along  diagonal, transfer comparison 
						results	to pointVec when nMatch is met.	*/
			h = xstart;  
			v = nv - ystart;
			if (!allpts) { h += halfwind; v -= halfwind; }
			for (i=lasti=0; i< yfinish-ystart-window; i++) {			
				if (matches >= nmatch) {	
				
	      	if (allpts) {
						for (j= max(i,lasti); j<i+window; j++) {
							if (matchvec[j]) {
							  /* dodot(j);  */
								if (npic >= maxpic) {	 
									maxpic += 100;  
									SetHandleSize((Handle)pich, maxpic*sizeof(long)); 
									if (MemError() != 0) goto done;	
									}	
								(*pich)[npic++] = (h+j) + ((v-j)<<16);	
								}
							}
						lasti= i+window;
						}
						
					else {	/* MoveTo( h + i, v - i); Line(0, 0); */	 
					  /* dodot(i); */
						if (npic >= maxpic) {	 
							maxpic += 100;  
							SetHandleSize((Handle)pich, maxpic*sizeof(long)); 
							if (MemError() != 0) goto done;	
							}	
					  (*pich)[npic++] = (h+i) + ((v-i)<<16);
						}
					}
				matches += (matchvec[i + window] - matchvec[i]);
				}
			} /* diag */
			
	done:			
    DisposPtr( matchvec);	
		SetHandleSize((Handle)pich, npic*sizeof(long));
		return;
} /* windowpt */
			
			


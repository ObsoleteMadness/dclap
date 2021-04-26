/********************************************************************
 * lindner
 * 3.2
 * 1993/11/02 06:14:11
 * /home/mudhoney/GopherSrc/CVS/gopher+/object/url.h,v
 * Exp
 *
 * Paul Lindner, University of Minnesota CIS.
 *
 * Copyright 1991, 1992 by the Regents of the University of Minnesota
 * see the file "Copyright" in the distribution for conditions of use.
 *********************************************************************
 * MODULE: url.h
 * Simplified method of getting urls..
 *********************************************************************
 * Revision History:
 * url.h,v
 * Revision 3.2  1993/11/02  06:14:11  lindner
 * Add url html hack
 *
 *
 *********************************************************************/

#ifndef URL_H
#define URL_H

#include "STRstring.h"

struct url_struct {
     String* url;
};

typedef struct url_struct Url;

#define URLget(a)   (STRget((a)->url))
#define URLset(a,b) (STRset((a)->url,(b)))

Url  *URLnew();
void URLdestroy();
void URLfromGS();
void URLmakeHTML();
char *URLgetTransport();
char *URLgetHost();
char *URLgetPort();

#endif /* URL_H */

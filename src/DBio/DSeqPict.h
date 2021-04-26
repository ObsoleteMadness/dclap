//DSeqPict.h
// d.g.gilbert, 1995


#ifndef _DSEQPICT_
#define _DSEQPICT_

#include <dgg.h>

class DWindow;
class DRichView;

enum { kNucCodesID = 11883, kAminoCodesID };

void CodePicture( DWindow* aDoc, char* data, ulong datalen, 
									short pixwidth, short pixheight, DRichView* itsView = NULL);
void NucCodesPicture();
void AminoCodesPicture();
void AboutSeqPupPicture();

#endif


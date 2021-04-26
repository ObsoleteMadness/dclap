// DId.h
// d.g.gilbert
//
// this is a sop to that damn 16bit os/compiler that
// can't handle long 4 char constants like '4chr'
// so instead we convert all such to some long numeric
//  DIE,  16 BIT TRASH !


#ifndef _DID_
#define _DID_


enum DClapCommands {
 cOKAY = 29000, cCANC, cSEND, cTEXT, cHelp, cEDIT, cAppl, cAppW, cRead, cInfo,
 cEnd
};

 

#endif

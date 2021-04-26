// DViewTypes.h
// d.g.gilbert

#ifndef _DVIEWTYPES_
#define _DVIEWTYPES_


// this is an extensible list of view / taskmaster types
// not sure of its uses at present, may change

enum ViewTypes { 
	kNotype = 0, 
	kMenu, 
	kMenuItem,
	kDisplay,
	kWindow, 
	kPanel, 
	kSlate, 
	kControl, 
	kDialogText,
	kLastViewtype = 32000 };


#endif

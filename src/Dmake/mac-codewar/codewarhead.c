/* codewarhead.c */
/* header for global definitions in codewarrior */
/* dgg here -- I know this shouldn't go here, but need some way to define
   it for all source, and dang codewar doesn't seem to have any way to globally
   set defines !!!!!! */
   
#define DCLAP
#define COMP_CODEWAR

#include <ncbilcl.h>
//#include <ncbi.h>

//
//	MacHeaders.c
//
//	Script to generate the 'MacHeaders' precompiled header for Metrowerks C/C++.
//  Copyright © 1993 metrowerks inc.  All rights reserved.
//

#ifndef powerc
 #pragma pointers_in_D0		//	required for c-style toolbox glue function: c2pstr and p2cstr
							//	the inverse operation (pointers_in_A0) is performed at the end ...
#endif

//
//	To allow the use of ToolBox calls which have now become obsolete on PowerPC, but
//	which are still needed for System 6 applications, we need to #define OBSOLETE.  If
//	your application will never use these calls then you can comment out this #define.
//	NB: This is only for 68K ...
//

#ifndef powerc
 #define OBSOLETE	1
 #define mc68000
#endif

//
//	Metrowerks-specific definitions
//
//	These definitions are commonly used but not in Apple's headers. We define
//	them in our precompiled header so we can use the Apple headers without modification.
//

#define PtoCstr		p2cstr
#define CtoPstr		c2pstr

#ifndef powerc
 #include <MixedMode.h>
 long GetCurrentA5(void)
  ONEWORDINLINE(0x200D);
#endif


//
//	Header files not compiled into MacHeaders
//

/***************
#define PtoCString	p2cstr
#define CtoPString	c2pstr

#define Length(s)	(*(unsigned char *)(s))
#define topLeft(r)	(((Point *) &(r))[0])
#define botRight(r)	(((Point *) &(r))[1])

#define TRUE		true
#define FALSE		false

#include <Controls.h>
#include <Desk.h>
#include <Devices.h>
#include <Dialogs.h>
#include <DiskInit.h>
#include <Errors.h>
#include <Events.h>
#include <Files.h>
#include <Fonts.h>
#include <Lists.h>
#include <LowMem.h> // included for Universal Headers ...
#include <Memory.h>
#include <Menus.h>
#include <OSEvents.h>
#include <OSUtils.h>
#include <Packages.h>
#include <Quickdraw.h>
#include <Resources.h>
#include <Scrap.h>
#include <SegLoad.h>
#include <StandardFile.h>
#include <Strings.h> // included for Universal Headers ...
#include <TextEdit.h>
#include <ToolUtils.h>
#include <Types.h>
#include <Windows.h>

#include <ADSP.h>
#include <AIFF.h>
#include <Aliases.h>
#include <AppleEvents.h>
#include <AppleTalk.h>
#include <Assembler.h>
#include <Balloons.h>
#include <CommResources.h>
#include <Components.h>
#include <Connections.h>
#include <ConnectionTools.h>
#include <CRMSerialDevices.h>
#include <CTBUtilities.h>
#include <DatabaseAccess.h>
#include <DeskBus.h>
#include <Disks.h>
#include <Editions.h>
#include <ENET.h>
#include <EPPC.h>
#include <FileTransfers.h>
#include <FileTransferTools.h>
#include <Finder.h>
#include <FixMath.h>
#include <Folders.h>
#include <GestaltEqu.h>
#include <Graf3D.h>
#include <Icons.h>
#include <ImageCodec.h>
#include <ImageCompression.h>
#include <Language.h>
#include <MediaHandlers.h>
#include <MIDI.h>
#include <Movies.h>
#include <MoviesFormat.h>
#include <Notification.h>
#include <Palette.h>
#include <Palettes.h>
#include <Picker.h>
#include <PictUtil.h>
#include <Power.h>
#include <PPCToolBox.h>
#include <Printing.h>
#include <PrintTraps.h>
#include <Processes.h>
#include <QDOffscreen.h>
#include <QuickTimeComponents.h>
#include <Retrace.h>
#include <ROMDefs.h>
#include <SANE.h>
#include <Script.h>
#include <SCSI.h>
#include <Serial.h>
#include <ShutDown.h>
#include <Slots.h>
#include <Sound.h>
#include <SoundInput.h>
#include <Start.h>
#include <Terminals.h>
#include <TerminalTools.h>
#include <Timer.h>
#include <Traps.h>
#include <Values.h>
#include <Video.h>
*********/

#ifndef powerc
 #pragma pointers_in_A0		//	required for c-style toolbox glue function: c2pstr and p2cstr
#endif

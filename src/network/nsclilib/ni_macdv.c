/*      
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
* File Name:    ni_macdv.c
*
* Author:       epstein
*
* Version Creation Date:        1/13/95
*
* $Revision: 4.0 $
*
* File Description: 
*   Determine which Macintosh MacTCP device is in-use (e.g., "Ethernet", "PPP")
*
* Note: This file is almost entirely based upon source code kindly provided
*       by Robert S. Mah (rmah@panix.com).  This in turn is based upon
*       the Apple technical document "MacTCP 2.0 LAP Tech Note".
*
*       This file is to be used for Macintosh computers only.
*
*
* Modifications:  
* --------------------------------------------------------------------------
* Date     Name        Description of modification
* -------  ----------  -----------------------------------------------------
*01/17/95  Kans        Add CodeWarrior-compatibility
* ==========================================================================
*
*
* RCS Modification History:
* $Log: ni_macdv.c,v $
 * Revision 4.0  1995/07/26  13:56:32  ostell
 * force revision to 4.0
 *
 * Revision 1.3  1995/05/17  17:52:34  epstein
 * add RCS log revision history
 *
*/

#include <ncbi.h>
#include <ncbiwin.h>

#include <MacTCPCommonTypes.h>
#include <Files.h>
#include <Errors.h>
#include <Resources.h>
#include <Memory.h>
#include <GestaltEqu.h>
#include <Folders.h>

#ifdef COMP_METRO
typedef OSErr (*OSErrProcPtr)();
typedef Ptr (*PtrProcPtr)();
typedef Boolean (*BooleanProcPtr)();
typedef void (*voidProcPtr)();

#endif

typedef unsigned char uchar;

extern Boolean ResLoad;

#if GENERATINGPOWERPC
#pragma options align=mac68k
#endif


/*
 * Holds IP and LAP-specific configuration information.  This is held in the
 * 'ipln' configuration resource in either the MacTCP driver or the "MacTCP
 * Prep" files.
 */
typedef struct IPConfig {
        long            version;
        long            flags;
        long            dfl_ip_addr;
        long            dfl_net_mask;
        long            dfl_broadcast_mask;
        long            dfl_gateway_addr;
        b_8                     server_lap_address[8];
        long            configIPAddr;
        long            configNetMask;
        long            dfl_dyn_low;
        long            dfl_dyn_high;
        char            dfl_zone[34];           /* ### */
        Boolean         load;
        Boolean         admin;
        Boolean         netLock;
        Boolean         subnetLock;
        Boolean         nodeLock;
        Boolean         filler1;
        long            activeLap;
        long            slot;
        char            filename[33];           /* ### */
} IPConfig;


/*
 * LAPInfo is allocated during system start-up and holds all of the
LAP-specific information.
 */
struct LAPInfo {
        b_32                    our_ip_addr;            /* LAP's IP address =
*/
        b_32                    our_net_mask;           /* LAP's IP net-mask=
 */
        b_32                    ip_broadcast_addr;      /* IP's broadcast
address */
        IPConfig lc;                                     /* copy of
IP LAP configuration resource */
        OSErrProcPtr    lapInit;                        /* pointer to
once-only LAP init routine */
        OSErrProcPtr    lapOpen;                        /* LAP open routine =
*/
        OSErrProcPtr    lapClose;                       /* LAP close routine=
 */
        voidProcPtr             lapUnload;                      /* LAP
unload routine, undoes LapInit */
        OSErrProcPtr    lapAttach;                      /* LAP attach PH
routine */
        OSErrProcPtr    lapDetach;                      /* LAP detach=
 routine */
        OSErrProcPtr    lapOutput;                      /* LAP output=
 routine */
        OSErrProcPtr    lapControl;                     /* LAP control
routine */
        voidProcPtr             lapFault;                       /* LAP
fault isolation routine */
        OSErrProcPtr    lapStatistics;          /* LAP statistic reading
routine */
        voidProcPtr             lapConfigure;           /* LAP-specific
configuration routines */
        BooleanProcPtr  lapProbe;                       /* send a
LAP-specific address probe packet */
        BooleanProcPtr  lapRegister;            /* register our IP address
on the network */
        voidProcPtr             lapFindGateway;         /* LAP-specific
means of finding a gateway */
        BooleanProcPtr  lapGwyCheck;            /* LAP-specific means of
verifying gateway up */
        /* IP parameters */
        ip_addr                 dfl_dns_addr;           /* address of DNS
from config protocol */
        Handle                  dnslHndl;                       /* handle
to DNS configuration resource */
        Ptr                             dnsCache;                       /*
pointer to space allocated for dns cache */
        long                    dnsCacheSize;           /* size of cache
allocated, in bytes */
        /* LAP parameters */
        long                    headerSize;                     /* LAP
header space requirements */
        long                    trailerSize;            /* LAP trailer
space requirements */
        long                    outMaxPacketSize;       /* size of maximum
output packet */
        long                    inMaxPacketSize;        /* size of maximum
input packet */
        long                    maxDataSize;            /* size of maximum
data packet */
        long                    numConnections;         /* number of
separate network connections */
        unsigned long   versionFlags;           /* version number flags */
        voidProcPtr             ip_ph;                          /* pointer
to IP's protocol handler */
        Ptr                             ipGlobals;                      /*
pointer to IP's A5 */
        short                   link_unit;                      /* unit
number of link driver */
        Boolean                 addressConflict;        /* TRUE if address
conflict discovered */
        long                    lapType;                        /* IP LAP
hardware type number */
        long                    lapAddrLength;          /* size of LAP
address field */
        unsigned char   *lapAddrPtr;            /* pointer to LAP address
field */
        unsigned long   reserved;                       /* MacTCP reserved
field */
};




#if GENERATINGPOWERPC
#pragma options align=reset
#endif




char * GetLAPType( void );
static char * ParseLAPType(IPConfig **configH );
static OSErr SearchFolderForRsrc( FSSpec *target, short vRefNum, long dirID,
                     OSType ftype, OSType fcrea, OSType rsrcType );


//--------------------------------------------------------------------------
// Returns the current LAP driver type
// Note: We should fix this to search the control panels folder for any file
// of the correct type containing a 'ipln' resource.
//--------------------------------------------------------------------------

char * GetLAPType( void )
{
        IPConfig **configH;
        long responce;
        char safety1[100];
        FSSpec fspec;
        char safety2[100];
        short refNum = -1;
        char *lapType = NULL;

        if( Gestalt( 'mtcp', &responce ) != noErr )
                return NULL;

        // First check the MacTCP Prep file
        if( FindFolder( kOnSystemDisk, kPreferencesFolderType, false,
                        &fspec.vRefNum, &fspec.parID ) != noErr )
                goto try_mactcp_drvr;

        if( SearchFolderForRsrc( &fspec, fspec.vRefNum, fspec.parID,
                                'mtpp', 'mtcp', 'ipln' ) != noErr )
                goto try_mactcp_drvr;

        if( (refNum = FSpOpenResFile( &fspec, fsRdPerm )) == -1 )
                goto try_mactcp_drvr;

        configH = (IPConfig**) Get1IndResource( 'ipln', 1 );
        if( configH != NULL && *configH != NULL ){
                lapType = ParseLAPType( configH );
        }
        CloseResFile( refNum );

        if( lapType != NULL )
                return lapType;

        // Otherwise check the MacTCP driver/control panel

try_mactcp_drvr:
        if( FindFolder( kOnSystemDisk, 'cntl', false,
                         &fspec.vRefNum, &fspec.parID ) != noErr )
                goto exit;

        if( SearchFolderForRsrc( &fspec, fspec.vRefNum, fspec.parID,
                                'cdev', 'ztcp', 'ipln' ) != noErr )
                goto exit;

        if( (refNum = FSpOpenResFile( &fspec, fsRdPerm )) == -1 )
                goto exit;

        configH = (IPConfig**) Get1IndResource( 'ipln', 1 );
        if( configH != NULL && *configH != NULL ){
                lapType = ParseLAPType( configH );
        }
        CloseResFile( refNum );

exit:
        return lapType;
}



//--------------------------------------------------------------------------
// Takes a 'ipln' resource and tries to figure out which LAP driver we are
// using.
//--------------------------------------------------------------------------

static char * ParseLAPType(IPConfig **configH )
{
        const IPConfig *ipc;
        long    result;
        static  char   laptype[256];
        uchar   hs;

        hs = HGetState( (Handle) configH );
        HLock( (Handle) configH );
        ipc = *configH;

        // Check if MacTCP is using LocalTalk or Ethernet
        if( ipc->activeLap == 128 || ipc->activeLap == 129 ){
                if (ipc->activeLap == 128)
                        return "LocalTalk";
                else
                        return "Ethernet";
        }

        memcpy(laptype, ipc->filename, ipc->filename[0] + 1);
        p2cstr((unsigned char *)laptype);
        
        return laptype;
}



//--------------------------------------------------------------------------
// Search a folder for a file of a specific type and creator (specify 0L for
// any) that contains a resource of a given type.
// Returns a FSpec to that file.
//--------------------------------------------------------------------------

static OSErr
SearchFolderForRsrc( FSSpec *target, short vRefNum, long dirID,
                     OSType ftype, OSType fcrea, OSType rsrcType )
{
    FSSpec              fspec;
    HParamBlockRec      fi;
    Handle              rsrcH;
    short               refNum;
    OSErr               err;
    Boolean             oldResLoad;

    err = fnfErr;
#ifdef NEW_UNIVERSAL_HEADERS /* JAE */
    oldResLoad = LMGetResLoad();
#else
    oldResLoad = /* ResLoad */ true;
#endif /* NEW_UNIVERSAL_HEADERS */
    SetResLoad( false );

    fspec.vRefNum = vRefNum;
    fspec.parID = dirID;

    fi.fileParam.ioCompletion = NULL;
    fi.fileParam.ioNamePtr = fspec.name;
    fi.fileParam.ioVRefNum = vRefNum;
    fi.fileParam.ioDirID = dirID;
    fi.fileParam.ioFDirIndex = 1;

    while( PBHGetFInfoSync( &fi ) == noErr )
    {
        // scan folder for resource files of specific type & creator
        if( (fcrea == 0L || fi.fileParam.ioFlFndrInfo.fdCreator == fcrea)
            && (ftype == 0L || fi.fileParam.ioFlFndrInfo.fdType == ftype) )
        {
            // found the conforming file, try to open it
            if( (refNum=FSpOpenResFile(&fspec,fsRdPerm)) != -1 )
            {
                // see if a resource is there
                rsrcH = GetIndResource( rsrcType, 1 );
                CloseResFile( refNum );

                // if there, return noErr
                if( rsrcH != NULL ){
                    err = noErr;
                    *target = fspec;
                    break;
                }
            }
        }
        // check next file in folder
        fi.fileParam.ioFDirIndex++;
        fi.fileParam.ioDirID = dirID;  // PBHGetFInfo() clobbers ioDirID
    }
    SetResLoad( oldResLoad );
    return err;
}

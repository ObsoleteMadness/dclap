/* 	DNR.c - DNR library for MPW

	(c) Copyright 1988 by Apple Computer.  All rights reserved
	
*/

#define MPW3 true

#include <Types.h>
#include <OSUtils.h>
#include <Errors.h>
#include <Files.h>
#include <Resources.h>
#ifdef MPW3 
#include <Memory.h>
#endif

/* #include <AddressXlation.h>*/
typedef pascal void (*EnumResultProcPtr)(struct cacheEntryRecord *cacheEntryRecordPtr, char *userDataPtr);

typedef pascal void (*ResultProcPtr)(struct hostInfo *hostInfoPtr, char *userDataPtr);


#define OPENRESOLVER	1
#define CLOSERESOLVER	2
#define STRTOADDR		3
#define	ADDRTOSTR		4
#define	ENUMCACHE		5
#define ADDRTONAME		6

Handle codeHndl = NULL;

typedef OSErr (*OSErrProcPtr)();
OSErrProcPtr dnr = NULL;

typedef struct hostInfo  *hostInfoPtr; /* dgg for codewar */

// jack for PPC calls -- dgg

enum {
	uppOPENRESOLVERProcInfo = kCStackBased
		 | RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long)))
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(char*)))
};

enum {
	uppCLOSERESOLVERProcInfo = kCStackBased
		 | RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long)))
};

enum {
	uppSTRTOADDRProcInfo = kCStackBased
		 | RESULT_SIZE(SIZE_CODE(sizeof(OSErr)))
		 | STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long)))
		 | STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(char*)))
		 | STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(hostInfoPtr)))
		 | STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(ResultProcPtr)))
		 | STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(char*)))
};


typedef OSErr (*CLOSERESOLVERProcPtr)(short);

typedef OSErr (*ADDRTONAMEProcPtr)( short,  
	unsigned long addr, hostInfoPtr rtnStruct, 
	ResultProcPtr resultproc, char *userDataPtr);

typedef OSErr (*ENUMCACHEProcPtr)( short,  
	EnumResultProcPtr resultproc, char *userDataPtr);

typedef OSErr (*ADDRTOSTRProcPtr)( short,  
	unsigned long addr,char * addrStr);
	
typedef OSErr (*STRTOADDRProcPtr)( short,  
	char *hostName, hostInfoPtr rtnStruct, ResultProcPtr resultproc,char * userDataPtr);

typedef OSErr (*OPENRESOLVERProcPtr)( short,  
	char *fileName);


/* OpenOurRF is called to open the MacTCP driver resources */

short OpenOurRF() {
	SysEnvRec info;
	HParamBlockRec fi;
	Str255 filename;
	
	SysEnvirons(1, &info);
	
	fi.fileParam.ioCompletion	= NULL;
	fi.fileParam.ioNamePtr		= (StringPtr)&filename;
	fi.fileParam.ioVRefNum		= info.sysVRefNum;
	fi.fileParam.ioDirID		= 0;
	fi.fileParam.ioFDirIndex	= 1;
	
	while (PBHGetFInfo(&fi, false) == noErr) {
		/* scan system folder for driver resource files of specific type & creator */
		if (fi.fileParam.ioFlFndrInfo.fdType == 'cdev' && fi.fileParam.ioFlFndrInfo.fdCreator == 'mtcp') {
			/* found the MacTCP driver file */
			SetVol(0,info.sysVRefNum);
			return(OpenResFile( (const unsigned char *)&filename));
		}
		
		/* check next file in system folder */
		fi.fileParam.ioFDirIndex++;
		fi.fileParam.ioDirID = 0;
	}
	return(-1);
}



OSErr OpenResolver(char *fileName)
{
	short refnum;
	OSErr rc;

	if (dnr != NULL)
		/* resolver already loaded in */
		return(noErr);
		
	/* open the MacTCP driver to get DNR resources. Search for it based on
	   creator & type rather than simply file name */	
	refnum = OpenOurRF();

	/* ignore failures since the resource may have been installed in the 
	   System file if running on a Mac 512Ke */
	   
	/* load in the DNR resource package */
	codeHndl = GetIndResource('dnrp', 1);
	if (codeHndl == NULL) {
		/* can't open DNR */
		return(ResError());
		}
	
	DetachResource(codeHndl);
	if (refnum != -1) {
		CloseWD(refnum); /* dgg */
		CloseResFile(refnum);
		}
		
	/* lock the DNR resource since it cannot be relocated while opened */
	MoveHHi(codeHndl);// dgg -- dnr handle will be open/locked long time...
	HLock(codeHndl);
	dnr = (OSErrProcPtr) *codeHndl;

	/* call open resolver */
#if USESROUTINEDESCRIPTORS
//#if defined(powerc) || defined (__powerc)
	rc = CallUniversalProc((UniversalProcPtr)(dnr),
		 uppOPENRESOLVERProcInfo, OPENRESOLVER, fileName);
#else
	rc = (*dnr)(OPENRESOLVER, fileName);
#endif

	if (rc != noErr) {
		/* problem with open resolver, flush it */
		HUnlock(codeHndl);
		DisposHandle(codeHndl);
		dnr = NULL;
		}

	return(rc);
}


OSErr CloseResolver()
{
	if (dnr == NULL) return(notOpenErr);
		
	/* call close resolver */
#if USESROUTINEDESCRIPTORS
	(void) CallUniversalProc((UniversalProcPtr)(dnr),
		 uppCLOSERESOLVERProcInfo, CLOSERESOLVER);
#else
	(void) (*dnr)(CLOSERESOLVER);
#endif

	/* release the DNR resource package */
	HUnlock(codeHndl);
	DisposHandle(codeHndl);
	dnr = NULL;
	return(noErr);
}

OSErr StrToAddr(char *hostName, hostInfoPtr rtnStruct, ResultProcPtr resultproc,char * userDataPtr)
{
	if (dnr == NULL) return(notOpenErr);
	
#if USESROUTINEDESCRIPTORS
	return( CallUniversalProc((UniversalProcPtr)(dnr),
		 uppSTRTOADDRProcInfo, STRTOADDR, hostName, rtnStruct, resultproc, userDataPtr));
#else
	return((*dnr)(STRTOADDR, hostName, rtnStruct, resultproc, userDataPtr));
#endif
}
	
OSErr AddrToStr(unsigned long addr,char * addrStr)
{
	if (dnr == NULL) return(notOpenErr);

	(*dnr)(ADDRTOSTR, addr, addrStr);
	return(noErr);
}
	
OSErr EnumCache(EnumResultProcPtr resultproc, char *userDataPtr)
{
	if (dnr == NULL) return(notOpenErr);
		
	return((*dnr)(ENUMCACHE, resultproc, userDataPtr));
}
	
	
OSErr AddrToName(unsigned long addr, hostInfoPtr rtnStruct, 
	ResultProcPtr resultproc, char *userDataPtr)
{
	if (dnr == NULL) return(notOpenErr);
		
	return((*dnr)(ADDRTONAME, addr, rtnStruct, resultproc, userDataPtr));
}
	
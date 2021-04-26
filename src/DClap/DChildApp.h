// DChildApp.h
// d.g.gilbert

#ifndef _DCHILDAPP_
#define _DCHILDAPP_

#include "DTaskMaster.h"
#include "DFile.h"

class DList;
class DNetOb;
class DBOP;

class DChildFile : public DFile
{	
public:
	enum kinds { kOutput, kInput, kStdin, kStdout, kStderr };
	enum actions { kNoAction, kOpenText, kOpenPict };
	enum { kDontDelete = false, kDeleteWhenDone = true };
	short fAction; 	
	short fKind;
	Boolean	fDelete;
	char * fName2;
	
	DChildFile( const char* filename, short kind= kOutput, Boolean deleteWhenDone = false,
							short doneAction= kOpenText, const char* openmode = NULL,
							const char* ftype = NULL, const char* fcreator = NULL);
	virtual ~DChildFile();
	virtual void ClearStorage();
};

class DChildApp : public DTaskMaster
{	
public:
	enum CallMethods {
			kNone,
			kLocalexec,
			kBOPexec,
			kHTTPget,
			kHTTPpost,
			kOther
			};
			
	char  * fName, * fCmdline;
	const char * fStdin, * fStdout, * fStderr;
	long		fProcessNum;
	short		fLaunched;
	short		fResult;
	short		fCallMethod;
	Boolean	fReusable; 	// don't delete from manager list after run
	DList * fFiles;  		// of DChildFile
	DBOP  * fBopper;		// network childapps
	DNetOb * fNob;			// network childapps
	
	DChildApp();
	DChildApp( char* appname, char* cmdline= NULL, Boolean showStdout = true, 
						char* Stdinfile = NULL, Boolean showStderr = true);
	virtual ~DChildApp();
	virtual void AddFile( DChildFile* aFile);
	virtual void AddFile( short filekind, char* name = NULL);
	virtual void AddInputBuffer( short filekind, char* buffer, ulong buflen);
	virtual void ClearFiles();
	virtual Boolean Launch();
	virtual Boolean LaunchBop();
	virtual Boolean LaunchLocal();
	virtual void Finished();
	virtual void FileAction( DChildFile* aFile);
};


class DExternalHandler : public DChildApp
{	
public:
	DExternalHandler( char* cmdlineOrDocname);
	virtual Boolean Launch();	
	virtual void Finished();
};

class DChildAppManager : public DTaskMaster
{	
public:
	enum { kChildManagerTask = 4782, kStatusTask };
	static DList* 	gChildList; // of DChildApp
	static long 		gLastTime;
	static short  	BuryDeadChildApp( long theEvent);
	static Boolean  CheckStatus();
	DChildAppManager();
	virtual Boolean IsMyTask(DTask* theTask);
};

extern DChildAppManager* gChildAppManager;

#endif

// DFile.cp


#include <ncbi.h>
#include <dgg.h>
#include "DFile.h"
#include "Dvibrant.h"
#include "DId.h"
#include <stdio.h>

#if defined(COMP_CWI) || defined(OS_NT)
#define WIN32 
#undef FAR
#undef NEAR
#include <windows.h>
#include <mapiwin.h>

#elif defined(OS_DOS)
#include <dir.h>
#include <dos.h>
#endif

// class DFileManager
// made into all static methods, Dec94

DFileManager*		gFileManager = NULL;

const char* DFileManager::kUntitled = "Untitled";

char	DFileManager::fName[640];

const char* DFileManager::GetInputFileName( const char* extension,  const char* mactype)
{
	Boolean okay= Nlm_GetInputFileName( (char*)fName, 256, (char*)extension, (char*)mactype);
	if (okay) return fName;
	else return NULL;
}

const char* DFileManager::GetOutputFileName( const char* defaultname) 
{
	Boolean okay=  Nlm_GetOutputFileName( (char*)fName, 256, (char*)defaultname);
	if (okay) return fName;
	else return NULL;
}

const char* DFileManager::GetFolderName()
{
	// this is a quick hack -- currently this selects a file w/in a folder,
	// then chops filename off -- confusing to users, need folder select dialog
	Boolean okay= Nlm_GetInputFileName( (char*)fName, 256, NULL, NULL);
	if (okay) return DFileManager::PathOnlyFromPath(fName);
	else return NULL;
}
	
const char* DFileManager::GetProgramPath()
{
	Nlm_ProgramPath( (char*)fName, 512);
	return fName;
}

const char* DFileManager::FilenameFromPath( const char* pathname)
{
	return Nlm_FileNameFind( (char*)pathname);		 
}

const char* DFileManager::PathOnlyFromPath( const char* pathname)
{
	char* nameonly= Nlm_FileNameFind( (char*)pathname);
	long len= StrLen(pathname) - StrLen(nameonly);
	StrNCpy(fName, pathname, len);
	fName[len]= 0;
	return fName;
}



short DFileManager::FileOrFolderExists( const char* pathname)
{

#ifdef OS_MAC
	long	theDirID;
	Boolean isFolder = false;
	char	namebuf[256];
	
	StrNCpy(namebuf, pathname, sizeof(namebuf));
	Nlm_CtoPstr( namebuf);
	short err= GetDirID( 0, 0, namebuf, &theDirID, &isFolder);
	if (err == 0) {
		if (isFolder) return kIsFolder;
		else return kIsFile;
		}
	else return kNothing;
#endif
	
#ifdef OS_UNIX
	struct stat    statbuf;
  if (! stat( pathname, &statbuf)) {
		if ( S_ISDIR(statbuf.st_mode)) return kIsFolder;
		else return kIsFile;
		}
	else return kNothing;
#endif

#if defined(COMP_CWI) || defined(OS_NT)
	WIN32_FIND_DATA finfo;
	HANDLE hFindFile;
	
  MemFill( &finfo, 0, sizeof(finfo));
  finfo.dwFileAttributes= 
  	FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_NORMAL;
	hFindFile= FindFirstFile( (char*)pathname, &finfo);
	if (hFindFile!=0) {
		(void) FindClose(hFindFile);
		if ( finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return kIsFolder;
		else return kIsFile;
		}
	else return kNothing;
	
#elif defined(OS_DOS)
#if defined(COMP_SYMC)
  FIND* finfo= findfirst( pathname, FA_RDONLY | FA_DIREC);
  if (finfo) {
		if ( finfo->attribute & FA_DIREC) return kIsFolder;
		else return kIsFile;
    }
#else
	struct  ffblk  finfo;
	
  MemFill( &finfo, 0, sizeof(finfo));
	short err= findfirst( pathname, &finfo, FA_RDONLY | FA_DIREC);
	if (err==0) {
		if ( finfo.ff_attrib & FA_DIREC) return kIsFolder;
		else return kIsFile;
		}
	else return kNothing;
#endif
#endif

// for others...
	return FileExists( pathname);
}


Boolean DFileManager::FileExists( const char* pathname)
{
	FILE* aFile= Nlm_FileOpen( (char*)pathname, "r");
	if (aFile) {
		Nlm_FileClose( aFile);
		return true;
		}
	else
		return false;
}


Boolean DFileManager::IsRelativePath(const char* path)
{
	char* lpath= (char*) path;
	
	if (!lpath || !*lpath) return false;
	
#ifdef OS_MAC
	return (*lpath == ':');
#endif

#if defined(OS_DOS) || defined (OS_NT)
	if (*lpath == '\\' || lpath[1] == ':') return false;
	else return true;
#endif

#ifdef OS_UNIX
	return (*lpath != '/');
#endif

#ifdef OS_VMS
	// [some.path]file.name
	// [-]file.name
	// disk:[path.to.somewhere]file.name
	if (StrChr(lpath,':') != 0) return false;
	else {
		char* cp= StrChr(lpath,'[');
		if (!cp) return true;
		else if (cp[1] == '.' || cp[1] == '-') return true;
		else return false;
		}
#endif
	return false;
}


char*	DFileManager::TempFolder( char* namestore)
{
	char* path;
	if (!namestore) namestore= fName;
	*namestore= 0;
	
#ifdef OS_MAC
	path= Nlm_TmpNam(namestore); // uses FindFolder(kOnSystemDisk, kTemporaryFolderType ..
	if (path) {
		char* nameonly= Nlm_FileNameFind( path);
		long len= StrLen(path) - StrLen(nameonly);
		if (len > 512) len= 512;
		StrNCpy( namestore, path, len);	
		namestore[len]= 0;
		}
#endif

#if (defined(OS_UNIX) || defined(OS_DOS) || defined (OS_NT) || defined(OS_VMS))
	path= getenv("TMPDIR");
	if (!path) path= getenv("TMP");
	if (!path) path= getenv("TEMP");
#ifdef OS_UNIX
	if (!path) path= "/tmp";
#endif
	if (path) StrNCpy( namestore, path, 512);
#endif

	return namestore;
}


char*	DFileManager::TempFilename( char* namestore)
{
	return Nlm_TmpNam( namestore);
}
		
Boolean DFileManager::CreateFolder( const char* pathname)
{
	return Nlm_CreateDir( (char*) pathname);
}



const char* DFileManager::BuildPath(const char* rootpath, const char* subfolder, const char* filename)
{
	if (rootpath) StrNCpy(fName, rootpath, 256);
	else fName[0]= 0;
	return Nlm_FileBuildPath(fName, (char*) subfolder, (char*) filename);
}


void DFileManager::UnixToLocalPath(char*& pathname)
{
	if (!pathname || !*pathname) return;
#ifndef OS_UNIX
#ifdef OS_VMS
	// can't handle this yet .. can stdio calls under vms translate unix/style/paths?
#else
#ifdef OS_MAC
	char buf[512], *np = buf, c;
	char* cp= pathname;
	long  n = 0;
	
	if (*cp == '/') cp++; 		// absolute: start at 1st pathname
	else { *np++= DIRDELIMCHR; n++; } 	// relative: must insert DIRDEL before start !
	do {
		c= *cp++;
		if (c == '/') c= DIRDELIMCHR;
		*np++ = c; 
		n++;
	} while (c);
	MemCpy(pathname, buf, n); // risky !!

#else
	// MDOS, other?
	char* cp= pathname;
	do {
		cp= StrChr( cp, '/');
		if (cp) *cp++= DIRDELIMCHR; 
	} while (cp);
#endif
#endif
#endif
}

const char* DFileManager::FileSuffix(const char* pathname)
{
  long 	len = StrLen(pathname);
  char* cp = (char*)pathname + len;
  while (cp > pathname && *cp != DIRDELIMCHR) {
		if (*cp == '.') return cp;
		cp--;
  	}
  return NULL;
}


void DFileManager::ReplaceSuffix(char* filename, long maxname, const char* suffix)
{
	long curlen, newlen;
	char* psuf= (char*)FileSuffix(filename);
	if (suffix) newlen= StrLen(suffix) + 1;
  else newlen= 0;
	if (psuf) {
		curlen= StrLen(filename) - StrLen(psuf);
		if (newlen > maxname - curlen) newlen= maxname - curlen;
		if (suffix) StrNCpy( psuf, suffix, newlen);
		else *psuf= 0;
		}
	else if (suffix) {
		curlen= StrLen(filename);
		if (newlen > maxname - curlen) newlen= maxname - curlen;
		if (suffix) StrNCat( filename, suffix, newlen);
		}
}





//class DFile : public DObject

	
DFile::DFile() 
{
	fFile= NULL;
	fName= NULL;
	fMode= StrDup("r");
	fType= NULL;
	fSire= NULL;
	fEof= true;
}

	
DFile::DFile(const char* filename, const char* openmode,  
						const char* type, const char* creator)
{
	fFile= NULL;
	fName= NULL;
	fMode= NULL;
	fType= NULL;
	fSire= NULL;
	fEof= true;
	Initialize( filename, openmode, type, creator);
}

void DFile::Initialize(const char* filename, const char* openmode,
						const char* type, const char* creator)
{
	char* name= StrDup( (char*) filename); // do before free in case these are some of self's data
	char* mode= StrDup( (char*) openmode);
	if (fFile) Close(); 
	if (fName) MemFree(fName);
	if (fMode) MemFree(fMode);
	fName= name;
	fMode= mode;
	fType= (char*)type;
	fSire= (char*)creator;
	fEof= true;
}
	
DFile::~DFile() 
{
	if (fFile) Close();
	MemFree( fName);
	MemFree( fMode);
}

Boolean DFile::suicide(void) 
{ 
	if (GetOwnerCount() <= 1) { 
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide();
}

Boolean DFile::suicide(short ownercount)
{
	if (ownercount < 1) {
		delete this; 
		return true; 
		}
	else 
		return DObject::suicide(ownercount);
}

Boolean DFile::Exists()
{
#if 1
	if (fFile) return true; // this is only around if file exists
	else if (!fName) return false;
	else {
		short kind= DFileManager::FileOrFolderExists(fName);
		if (kind == DFileManager::kIsFile) return true;
		else if (kind == DFileManager::kIsFolder) return false; //?? what to do?? false or true?
		else return DFileManager::FileExists(fName); // try another test?
		}
#else
	if (fFile) return true;
	else if (!fName) return false;
	else {
		Boolean exists;
			// !! can't use w/ fOpen mode -- wipes out data if "w" is mode
		if (fMode && *fMode == 'r') {
			fFile= Nlm_FileOpen(fName, fMode);
			exists= (fFile != NULL);
			}
		else {
			fFile= Nlm_FileOpen(fName, "r");
			exists= (fFile != NULL);
			Close();
			}			
		return exists;
		}
#endif
}

void DFile::Create(const char* filetype, const char* creator)
{
	// this doesn't delete existing file...! on MacOS
	fType= (char*)filetype;
	fSire= (char*)creator;
#ifndef OS_MAC
 if (! DFileManager::FileExists(fName))
#endif
	Nlm_FileCreate(fName, (char*)filetype, (char*)creator);
}

Boolean DFile::Delete()
{
	Close();
	return Nlm_FileRemove(fName);
}

Boolean DFile::Rename(const char* newname)
{
	//?? can we leave file open during this?
	Boolean okay= Nlm_FileRename(fName, (char*)newname);
	if (okay) {
		MemFree(fName);
		fName= StrDup(newname);
		}
	return okay;
}

void DFile::SetMode(const char* openmode)
{
	if (openmode && StringCmp(openmode, fMode) != 0) {
		Nlm_FileClose( fFile);
		char* mode= StrDup( (char*) openmode);
		MemFree(fMode);
		fMode= mode;
		}
}

		// result in all funcs here is error code or 0
short DFile::Open(const char* openmode)
{
	if (openmode) {
		if (StringCmp(openmode, fMode) == 0) 
			openmode= NULL;
		else {
			char* mode= StrDup( (char*) openmode);
			if (fMode) MemFree(fMode);
			fMode= mode;
			}
		}
	if (fFile) 
		if (openmode) {
			Nlm_FileClose( fFile);
			fFile= Nlm_FileOpen(fName, fMode);
			}
		else fseek(fFile, 0, 0); // reset
	else {
		if ((fSire || fType) && (*fMode == 'w' || *fMode == 'a'))
			Create( fType, fSire);
		fFile= Nlm_FileOpen(fName, fMode);
		}
	fEof= (fFile == NULL);
	return (fFile) ? 0 : -1;
}


short DFile::Close()
{
	if (fFile) Nlm_FileClose( fFile);
	fEof= true;
	fFile= NULL;
	return 0;
}

short DFile::GetDataLength(ulong& filelen)
{
	filelen= Nlm_FileLength(fName);
	return 0; // any error code is trapped in Nlm_
}

short DFile::GetDataMark(ulong& fileindex)
{
	// not yet available in ncbi toolkit...
	short err= 0;
	fileindex= 0;
	if (!fFile) return -1;
	long findex= ftell(fFile);
	if (findex<0) {
		err= findex;
		fileindex= 0;
		}
	else
		fileindex= findex;
	return err;
}

short DFile::SetDataMark(ulong	fileindex)
{
			// not yet available in ncbi toolkit...
	if (!fFile) return -1;
	else return fseek(fFile, fileindex, 0);
}


Boolean DFile::EndOfFile()
{
	if (!fFile) return -1;
	else {
		if (!fEof) fEof= feof(fFile); // !! need this !! this isn't always RIGHT !!
		return fEof;
		}
}

short DFile::GetFileType(long& fileType) 
{
		// this is a silly hack -- needs work?
	if (fMode && strchr(fMode,'t')) fileType= cTEXT;
	else fileType= 0;
	return 0;
}

short DFile::ReadData( void* buffer, ulong& count) 
{
	long newcount= Nlm_FileRead(buffer, 1, count, fFile);
	if (newcount == count) return 0;
	else { 
		if (newcount==0) fEof= true;
		count= newcount; 
		return -1; 
		} 
}

short DFile::ReadLine( char* line, ulong count) 
{
	void *buf= Nlm_FileGets(line, count, fFile);
	if (buf) return 0;
	else {
		fEof= true;
		return -1;
		}
}


short DFile::ReadUntil( void* buffer, ulong& count, char stopchar) 
{
				// no ncbi proc for this...
	register int	c;
	register char* bp = (char*) buffer;
	long maxcount= count;

	if (!fFile || !buffer) return -1;
	count= 0;
	while (true) {
		c = fgetc( fFile);
		if (c<0) {
			//if (c == EOF) 
			fEof= true;
			return c;
			}
		else {
			*bp++= c;
			count++;
			if (c == stopchar || count >= maxcount)  
				return 0; // ?? do we stuff stopchar into buffer?
			}
		}
	return 0;
}


short DFile::WriteLine( char* line, Boolean addNewline) 
{
	short result;
	result= Nlm_FilePuts( line, fFile);
	if (addNewline) result += Nlm_FilePuts( LineEnd, fFile);
	return result;
}

short DFile::WriteData( void* buffer, ulong& count)
{
	ulong newcount= Nlm_FileWrite(buffer, 1, count, fFile);
	if (count != newcount) { count= newcount; return -1; } //??
	else return 0;
}



// class DTempFile

DTempFile::DTempFile() : 
	DFile()
{
	// ?? text/binary mode flag
	char	namestore[512];
	Initialize( gFileManager->TempFilename(namestore), "a", NULL, NULL);
	Open();
}
	
DTempFile::~DTempFile()
{
	Delete();
}	

char* DTempFile::ReadIntoMemory(ulong& bytesread, Boolean deleteAfterRead)
{
	short 	err;
	ulong	count;
	char	* data;
	
	Open("r");
	err= GetDataLength( count);
	data= (char*) MemNew( count+1);
	if (data) {
		err= ReadData( data, count);
		bytesread= count;
		data[count]= 0; // add null term
		}
	Close();
	if (deleteAfterRead) Delete();
	return data;
}

 
// DFile.h

#ifndef _DFILE_
#define _DFILE_


#include "DObject.h"
#include "Dvibrant.h"


class	DFileManager	: public	DObject
{
	static char	fName[640];
public:
	enum FileTypes { kNothing, kIsFile, kIsFolder };
	static const char* kUntitled;
	
	static const char* GetInputFileName( const char* extension,  const char* mactype);
	static const char* GetOutputFileName( const char* defaultname);		
	static const char* GetFolderName();
	static const char* GetProgramPath();
	static const char* FilenameFromPath( const char* pathname);
	static const char* PathOnlyFromPath( const char* pathname);
	static char*	TempFilename( char* namestore = NULL);
	static char*  TempFolder( char* namestore = NULL);
	static Boolean CreateFolder( const char* pathname);
	static const char* BuildPath( const char* rootpath, const char* subfolder, 
													const char* filename = NULL);
	static const char* FileSuffix( const char* pathname);
	static void ReplaceSuffix( char* filename, long maxname, const char* suffix);
	static Boolean FileExists( const char* pathname);
	static short FileOrFolderExists( const char* pathname);
	static void UnixToLocalPath( char*& pathname);
	static Boolean IsRelativePath(const char* path);
	
};

extern	DFileManager*		gFileManager;



class DFile : public DObject
{
public:	
	FILE*	fFile;
	char*	fName;
	char*	fMode;
	char* fType;
	char* fSire;
	Boolean fEof;
	
	DFile();
	DFile(const char* filename, const char* openmode = "r",
				const char* ftype = NULL, const char* fcreator = NULL);
	virtual ~DFile();
	virtual	Boolean suicide(void);  // prefered to delete 
	virtual	Boolean suicide(short ownercount);

	virtual void Initialize(const char* filename, const char* openmode = "r",
					const char* ftype = NULL, const char* fcreator = NULL);
	virtual const char*	GetName() { return fName; }
	virtual const char*	GetShortname() { return gFileManager->FilenameFromPath(fName); }
	
			// short result in most funcs here is error code or 0
	virtual short Open(const char* openmode = NULL);
	short OpenFile(const char* openmode = NULL) { return this->Open(openmode); }
	virtual short Close();
	short CloseFile() { return this->Close(); }
	virtual short GetDataLength(ulong& filelen);
	virtual short GetDataMark(ulong& fileindex);
	ulong Tell() { ulong fileindex; GetDataMark( fileindex); return fileindex; }
	ulong LengthF() { ulong filelen; GetDataLength( filelen); return filelen; }
	virtual short SetDataMark(ulong fileindex);
	short Seek(ulong fileindex) { return SetDataMark( fileindex); }
	virtual Boolean Exists();
	virtual Boolean EndOfFile();
	Boolean Eof() { return EndOfFile(); }
	virtual Boolean IsOpen() { return fFile != NULL; }
	virtual void SetMode(const char* openmode);

	virtual Boolean Delete();
	virtual Boolean Rename(const char* newname);
	virtual void Create(const char* filetype, const char* creator);  // type,creator only honored on mac

	virtual short GetFileType(long& fileType);
	virtual short ReadData( void* buffer, ulong& count);
	virtual short ReadUntil( void* buffer, ulong& count, char stopchar);
	virtual short ReadLine( char* line, ulong count);
	virtual short WriteData( void* buffer, ulong& count);
	virtual short WriteLine( char* line, Boolean addNewline= false);
	short WriteLn( char* line) { return WriteLine( line, true); }
};



	// DTempFile is mainly a quick hack till we get iostreams written into critical places
	// use tempfile to write to then read back into memory:
	//   DTempFile* tmp = new DTempFile();  
	//   tmp->WriteLine(data); tmp->WriteLine(data); ...
	//	 ulong datasize;
	//	 char* data= tmp->ReadIntoMemory(datasize);
	//	 delete tmp;
	
class DTempFile : public DFile
{
public:
	DTempFile();
	virtual ~DTempFile(); // delete temp file
	virtual char* ReadIntoMemory(ulong& bytesread, Boolean deleteAfterRead= true);
};



#endif // _DFILE_

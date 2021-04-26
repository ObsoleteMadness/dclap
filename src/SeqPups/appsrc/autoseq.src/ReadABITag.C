#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<iostream.h>
#include	"RInclude.H"
#include	"RInlines.H"

typedef ushort int2;						// Two byte short assumption
typedef ulong  int4;						// Four byte long assumption
const size_t INDEXPO		= 26;			// offset of offset to index
const int	INDEX_ENTRY_LENGTH= 28;

char	FatalString[200];
void	Fatal(char* error)
	{ cerr << "disect: FATAL: " << error << endl; exit(1); }

//	============================================================================
//	readABIInt2
//	========================================|===================================
bool
readABIInt2(
	FILE*	fp,
	int2*	i2)
	{
	unsigned char buf[sizeof(int2)];

	if (fread(buf, sizeof(buf), 1, fp) != 1) return (FALSE);
	*i2 = (int2)
		(((unsigned short)buf[1]) +
		 ((unsigned short)buf[0]<<8));
	return (TRUE);
	}

//	============================================================================
//	readABIInt4
//	========================================|===================================
bool
readABIInt4(
	FILE*	fp,
	int4*	i4)
	{
	unsigned char buf[sizeof(int4)];

	if (fread(buf, sizeof(buf), 1, fp) != 1) return (FALSE);
	*i4 = (int4)
		(((unsigned long)buf[3]) +
		 ((unsigned long)buf[2]<<8) +
		 ((unsigned long)buf[1]<<16) +
		 ((unsigned long)buf[0]<<24));
	return (TRUE);
	}

//	============================================================================
//	getIndexEntryLW
//	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -
//	From the ABI results file connected to `fp' whose index starts at byte
//	offset `indexO', return in `val' the `lw'th long word from the `count'th
//	entry labeled `label'. The result indicates success.
//	========================================|===================================
bool
getIndexEntryLW1(
	FILE*	fp,
	long	indexO,
	long	label,
	long	count,
	int		lw,
	int4*	val)
	{
	int		entryNum=-1;
	int		i;
	int4	entryLabel, entryLw1;

	do
		{
		entryNum++;
		cout << entryNum << ": ";
		cout << 1;
		if (fseek(fp, indexO+(entryNum*INDEX_ENTRY_LENGTH), SEEK_SET) != 0)
			return FALSE;
		cout << 2;
		if (!readABIInt4(fp, &entryLabel))
			return FALSE;
		cout << 3;
		if (!readABIInt4(fp, &entryLw1))
			return FALSE;
		cout << 4 << endl;
		} while (!(entryLabel == label && entryLw1 == count));

		cout << 5;

	for(i=2; i<=lw; i++)
		if (!readABIInt4(fp, val)) return FALSE;

		cout << 6;
	return TRUE;
	}

//	============================================================================
//	getIndexEntryW
//	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -  -	 -	-  -
//	From the ALF results file connected to `fp' whose index starts at byte
//	offset `indexO', return in `val' the `lw'th	 word (int2) from the entry
//	labeled `label'. The result indicates success.
//	========================================|===================================
bool
getIndexEntryW(
	FILE*	fp,
	long	indexO,
	long	label,
	int		lw,
	int2*	val)
	{
	int		entryNum=-1;
	int		i;
	int4	entryLabel;
	int4	jval;

	do
		{
		entryNum++;
		if (fseek(fp, indexO+(entryNum*INDEX_ENTRY_LENGTH), SEEK_SET) != 0)
			return FALSE;
		if (!readABIInt4(fp, &entryLabel))
			return FALSE;
		}
		while (entryLabel != label);


	for(i=2; i<lw; i++)
		if (!readABIInt4(fp, &jval)) return FALSE;
	
	if (!readABIInt2(fp, val)) return FALSE;

	return TRUE;
	}

const long DATAtag			= ((long) ((((('D'<<8)+'A')<<8)+'T')<<8)+'A');
//
// MAIN
//
int
main(int argc, char* argv[])
	{
	FILE*	fp;
	long	label = DATAtag; //Pack4Chars(argv[2]);
	ulong	count = atol(argv[3]);
	int4	indexO;
	char	tag[5]={NULL};
	int4	dummyVal;

	if (argc!=4)
		{
		cerr << "ABItag <filename> <tag> <which>" << endl;
		exit(1);
		}
	
	if (!(fp = fopen(argv[1],"rb")))
		{
		strcpy(FatalString,"couldn't open '");
		strcpy(FatalString,argv[1]);
		strcpy(FatalString,"'.");
		Fatal(FatalString);
		}

	// get the index offset
	if ((fseek(fp, INDEXPO, SEEK_SET) != 0) || (!readABIInt4(fp, &indexO)))
		Fatal("couldn't read indexO");

	cout << "FILE:   " << argv[1] << endl;
	cout << "indexO: " << indexO << endl;
	cout << "TAG:    " << Unpack4Chars(label,tag) << endl;

	cout << "count\t0\t1\t2\t3\t4\t5\t6" << endl;	
	bool keepGoing = TRUE;
	for(uint i=0 ; keepGoing ; i++)
		{
		cout << i << "\t";
		for(uint j=0; keepGoing && (j<=5); j++)
			if (!(getIndexEntryLW1(fp, indexO, DATAtag, i, j, &dummyVal)))
				keepGoing = FALSE;
			else
				cout << dummyVal << "\t";
		cout << endl;
		}

	fclose(fp);
	}


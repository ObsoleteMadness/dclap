//	============================================================================
//	RInlines.H														80 spaces
//	Reece Hart, reece@ibc.wustl.edu									tab=4 spaces
//	-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
//	This file contains homegrown inline functions which are used in	many of my
//	sources.
//	========================================|===================================

#ifndef _H_RInlines							// include this file only once
#define _H_RInlines

#include	<stddef.h>
#include	<math.h>
#include	"RInclude.H"

#ifndef WIN_MAC  // dgg 
const double _PI = 3.14159265358979;
#endif

//
//	Swap
//	swap the values of any two elements
//
template<class T>
inline
void
Swap(T& elem1, T& elem2)
	{ T dummy=elem2; elem2=elem1; elem1=dummy; }

//
//	`Pluralizer' -- return "s" if number != 1
//
template<class T>
inline
char*
Plural(T number)
	{ return ((number==1)?"":"s"); }

//
//	return "yes" or "no"
//
inline
char*
YesNo(bool b)
	{ return (b==TRUE?"yes":"no"); }

//
//	return "true" or "false"
//
inline
char*
TrueFalse(bool b)
	{ return (b==TRUE?"true":"false"); }

//
//	tag with a character if true
//	by default, * if true, space if false
//
inline
char
Tag(bool b,char Ttag='*',char Ftag=' ')
	{ return (b==TRUE?Ttag:Ftag); }


//
//	simple interpolation/extrapolation function
//
inline
double
Interpolate(double x1, double y1, double x2, double y2, double at_x)
	{
	return y1 + (y2-y1)/(x2-x1)*(at_x-x1);
	}

//
//	Exponential Decay
//		x0 * exp(-k*i)
//
inline
double
ExpDecay(double x0, double k, double i)
	{
	return	x0 * exp(-k*i);
	}

//
//	Z
//	Compute standard z score for a sample
//
inline
double
Z(double obs, double mean, double stddev)
	{
	return (obs-mean)/stddev;
	}

//
//	StdNormal
//	Compute probability density for a Z score
//
inline
double
StdNormal(double z)
	{
	return exp(-pow(z,2)/2) / sqrt(2*_PI);
	}

//
//	Gaussian distribution using std deviation
//
inline
double
GaussianSD(double obs, double mean, double stddev)
	{
	return exp(-pow((obs-mean)/stddev,2)/2) / (stddev*sqrt(2*_PI));
	}

//
//	Gaussian distribution using variance
//
inline
double
GaussianV(double obs, double mean, double var)
	{
	return exp(-pow(obs-mean,2)/2/var) / (sqrt(var*2*_PI));
	}

//
//	Pack 4 characters into a single ulong (4:1 compression)
//
inline
long
Pack4Chars(char* c)
	{ return ((long) (((((c[0]<<8)+c[1])<<8)+c[2])<<8)+c[3]); }

//
//	Unpack the 4 characters from the ulong.
//
inline
char*
Unpack4Chars(long packed, char* chars)
	{
	chars[0] = (char)(packed>>24) & 0xFF;
	chars[1] = (char)(packed>>16) & 0xFF;
	chars[2] = (char)(packed>>8)  & 0xFF;
	chars[3] = (char)(packed)     & 0xFF;
	return chars;
	}

//
// Invert
//   Inverts an array of arbitrary objects in memory (in place).
//
// Algorithm:
// 1. initialize left and right pointers to the extreme ends of the array
// 2. while (right > left)
//      swap *left and *right
//		increment left pointer
//		decrement right pointer
// 	  elihw
// Pointers to objects were chosen over indices in an array to avoid
// the pointer arithmetic involved in array addressing, which I've assumed
// to be greater than the simple add/subtract mechanism for the pointers below.
//
template<class T>
inline
void
Invert(T* arrayPtr, size_t len)
	{
	register T*	left = arrayPtr;			// 'left' ptr; init to 1st T
	register T*	right = arrayPtr + len - 1;	// 'right' ptr; init to last T

	while (right-left > 0)
		{
		T temp = *left;
		*left = *right;
		*right = temp;

		left++;
		right--;
        }
	}

//
// FlipEndian
//	  BIGENDIAN <-> LILENDIAN converter
// Assumes that sizeof(T)%8 == 0.  
//template<class T>
//T
//FlipEndian(T item)
//	{
//	Invert((byte*)&item,sizeof(T)/8);
//	return item;
//
//	}
//

#endif										// conditional inclusion


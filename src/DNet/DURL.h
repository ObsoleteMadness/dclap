// DURL.h
// by D. Gilbert, Sep 94

#ifndef __DURL__
#define __DURL__


class		DNetOb;
class       DGopher;

class DURL : public DObject {
public:

	enum {
		alphaChars	 = 1,
		alphaPlusChars = 2,
		pathChars    = 4,
		HEX_ESCAPE 	= '%'
		};
	
	enum Parts {
		kPartNone = 0,
		kPartProtocol = 1,
		kPartHost = 2,
		kPartPort = 4,
		kPartPath = 8
		};
			
	static short IsURL( const char *line, char*& url, long maxline = 0);
	static Boolean ParseURL( DNetOb* nob, const char *url, long urlsize = 0, 
					Boolean verbatim = true);
	static Boolean ParseURL( DGopher* gob, const char* url, long urlsize = 0,
		Boolean verbatim = true);
	static char* EncodeChars( const char* str, unsigned char mask = pathChars);
	static char* DecodeChars( char * str);
	static char* GetParts( const char* url, long whichparts, long urlsize = 0);
	
	DURL();	
	
};


#endif


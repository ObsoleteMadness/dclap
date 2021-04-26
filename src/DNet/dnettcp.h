/* dnettcp.h 

*/


#ifndef _DNET_TCP_
#define _DNET_TCP_

#ifdef __cplusplus
extern "C" {
#endif


enum	SockConnectErrors {
	errGeneral= -1,
	errHost		= -2,
	errSocket	= -3,
	errConnect= -4
	};


long 	SockOpen(char* hostname, unsigned short port);
short	SockClose(long theSocket);

long	SockRead( long itsSocket, void *buffer, long buflen);
long	SockWrite( long itsSocket, void *buffer, long buflen);
short	SockSelect( short numsocks, long readsocks[],long writesocks[],
					long errsocks[], long time);

long	Hostname2IP( char* hostname);
long	MyIPaddress(void);
short	MyHostname( char* name, short namelen);
short	SockHostname( long itsSocket, char* name, short namelen);

#ifdef __cplusplus
}
#endif

#endif /* _DNET_TCP_ */

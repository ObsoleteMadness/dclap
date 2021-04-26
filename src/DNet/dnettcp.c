/* dnettcp.c
	 d.g.gilbert
	 used as interface b/n "dirty" c and c++
	 for some of the tcp sockets basic stuff
*/


#include "dnettcp.h"
#include <ncbi.h>

#if defined(COMP_CODEWAR) && defined(OS_MAC)
#define __STDC__

/* weird, this proto isn't in any of those many tcp headers !! */
int gethostname(char * machname, long buflen);
#endif

typedef struct sockaddr *SockAddPtr; /* codewar 4 hates (struct *) casts !? */

#define __NI_LIB__
#define _NCBINET_
#define _NI_TYPES_

#ifdef WIN_MSWIN
#define NETP_INET_WSOCK 1
	/* those who want non-winsock will have to fiddle here... */
#endif

#ifdef OS_MAC

/* #include "macsockd.h" << this defines s_socketstuff as socketstuff, conflicting standard c lib */
/* just do a subset.... */
#define socket				s_socket
#define connect				s_connect
#define getsockname		s_getsockname
#define setsockopt		s_setsockopt

#define __TYPES__       /* avoid Mac <Types.h> */
#define __MEMORY__      /* avoid Mac <Memory.h> */
#define ipBadLapErr     /* avoid Mac <MacTCPCommonTypes.h> */
#define APPL_SOCK_DEF
#define DONT_DEFINE_INET
#include "sock_ext.h"
extern void bzero PROTO((CharPtr target, long numbytes));
#endif /* OS_MAC */


	/* this is from NCBI network/nsclilib/ -- it defines & includes system-specific headers */
#include "ni_net.h"




int InitSocks PROTO((void));

static int gSocks_on = 0;

int InitSocks()
{
#ifdef WIN_MSWIN
	if (!gSocks_on) {
		int err;
		WSADATA data;
		WORD version = (1 << 8) + 1; /* MAKEWORD(1,1); */
		err= WSAStartup(version, &data);
		gSocks_on= (err == 0);
		}
	return gSocks_on;
#else
	return 1;
#endif
}



long SockOpen(char* hostname, unsigned short port)
{
			/* from Paul Lindner, UMinnesota gopher Socket.c  */
	 struct sockaddr_in Server;
	 struct hostent*	hostPtr;
	 long sockfd = 0;
	 long ERRinet = -1;
#ifdef _CRAY
	 ERRinet = 0xFFFFFFFF;  /* -1 doesn't sign extend on 64 bit machines */
#endif
	 long errnum = 0;

     /*** Find the hostname address ***/
	if (!InitSocks()) return -1;
     
#ifdef WIN_MSWIN
	if (WSAIsBlocking()) WSACancelBlockingCall(); /* ??? */
#endif

	MemFill( &Server, '\0', sizeof(Server));  /* dgg, mswin fix? */
	Server.sin_addr.s_addr = inet_addr(hostname);
	if (Server.sin_addr.s_addr == ERRinet) {
		hostPtr = gethostbyname(hostname);
		if (hostPtr != NULL) {
			MemFill( &Server, '\0', sizeof(Server));
			MemCopy( &Server.sin_addr, hostPtr->h_addr, hostPtr->h_length);
			Server.sin_family = hostPtr->h_addrtype;
			}
		else {
			errnum= errHost;
			goto errexit; /* return (errHost); */
			}
    } 
	else
	  Server.sin_family = AF_INET;

	Server.sin_port = (unsigned short) htons(port);

     /*** Open the socket ***/

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		errnum= errSocket;
		goto errexit; /* return (errSocket);  */
		}

#ifndef WIN_MSWIN
/* ?? winsock is having some problems */
#ifndef UCX
	setsockopt(sockfd, SOL_SOCKET, ~SO_LINGER, 0, 0);
#endif
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 0, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, 0, 0);
#endif
	
		/*** Connect ***/

	if (connect(sockfd, (SockAddPtr) &Server, sizeof(Server)) < 0) {
		SockClose(sockfd);
		errnum= errConnect;
		goto errexit; /* return (errnum); */
		}
	
	return(sockfd);

errexit:
#ifdef WIN_MSWIN
	WSACancelBlockingCall(); /* ??? */
#endif
	return(errnum);
/* #else -- !NETLIB && !CMUIP  << left out these versions, dgg */
}


short SockClose(long theSocket)
{
#ifdef WIN_MSWIN
	return closesocket(theSocket);
#else
#ifdef WIN_MAC
	return s_close(theSocket);
#else
	return close(theSocket);
#endif
#endif
}

	
	
long	MyIPaddress(void)
{
	if (!InitSocks()) return 0;
#if defined(WIN_MSWIN) || defined(OS_UNIX_SYSV)
  	/* winsok seems to lack gethostid() */
    /* also missing in Solaris2 */
	return inet_addr("localhost");
#else
	return gethostid();
#endif
}


short	MyHostname( char* name, short namelen)
{
	if (!InitSocks()) return -1;
	return (short) gethostname( name, namelen);
}

long	Hostname2IP( char* hostname)
{
	if (!InitSocks()) return -1;
	return (long) gethostbyname( (char*)hostname);
}


short	SockHostname( long itsSocket, char* name, short namelen)
{
#if defined(OS_UNIX_IRIX) || defined(COMP_SYMC)
	int	len= namelen;
#else
	long	len= namelen;
#endif
	short err;
	err= getsockname( itsSocket, (SockAddPtr) name, &len); 
	if (!err) { if (len>=namelen) --len; name[len]= '\0'; }
	return err;
}

long	SockRead( long itsSocket, void *buffer, long buflen)
{
#ifdef WIN_MSWIN
	return recv(itsSocket, buffer, buflen, 0);
#else
#ifdef WIN_MAC
	return s_read(itsSocket, buffer, buflen);
#else
	return read(itsSocket, buffer, buflen);
#endif
#endif
}

long	SockWrite( long itsSocket, void *buffer, long buflen)
{
#ifdef WIN_MSWIN
	return send(itsSocket, buffer, buflen, 0);
#else
#ifdef WIN_MAC
	return s_write(itsSocket, buffer, buflen);
#else
	return write(itsSocket, buffer, buflen);
#endif
#endif
}

short	SockSelect( short numsocks, long readsocks[],long writesocks[],
								long errsocks[], long time)
{
	return select( numsocks, (fd_set*)readsocks, (fd_set*)writesocks, 
						(fd_set*)errsocks, (struct timeval *) time);
}


#if defined(NOT_COMP_CODEWAR) && defined(OS_MAC)
/* damn that buggy linker, can't *find* this code in netdb.c */

//# include <Stdio.h>
//# include <Types.h>
//# include <Resources.h>
//# include <Errors.h>
//# include <OSUtils.h>

# include <s_types.h>
//# include <netdb.h>
//# include <neti_in.h>
//# include <s_socket.h>
# include <s_time.h>
# include <neterrno.h>

//# include "sock_str.h"
/*# include "sock_int.h"*/

//#include <Ctype.h>
#include <a_namesr.h>
#include <s_param.h>



extern SpinFn spinroutine;
extern pascal void DNRDone(struct hostInfo *hostinfoPtr,Boolean *done);

extern int h_errno;

struct hostent * gethostbyname(char *name)
{
	Boolean done;
	int i;
	
	sock_init();

	for (i=0; i<NUM_ALT_ADDRS; i++)
		macHost.addr[i] = 0;
	done = false;
	if (StrToAddr(name,&macHost,(ResultProcPtr) DNRDone,(char *) &done) == cacheFault)
	{
		SPINP(!done,SP_NAME,0L)
	}
	switch (macHost.rtnCode)
	{
		case noErr: break;
		
		case nameSyntaxErr:	h_errno = HOST_NOT_FOUND;	return(NULL);
		case cacheFault:	h_errno = NO_RECOVERY;		return(NULL);
		case noResultProc:	h_errno = NO_RECOVERY;		return(NULL);
		case noNameServer:	h_errno = HOST_NOT_FOUND;	return(NULL);
		case authNameErr:	h_errno = HOST_NOT_FOUND;	return(NULL);
		case noAnsErr:		h_errno = TRY_AGAIN;		return(NULL);
		case dnrErr:		h_errno = NO_RECOVERY;		return(NULL);
		case outOfMemory:	h_errno = TRY_AGAIN;		return(NULL);
		case notOpenErr:	h_errno = NO_RECOVERY;		return(NULL);
		default:			h_errno = NO_RECOVERY;		return(NULL);
	}
	
	/* was the 'name' an IP address? */
	if (macHost.cname[0] == 0)
	{
		h_errno = HOST_NOT_FOUND;
		return(NULL);
	}
	
	/* for some reason there is a dot at the end of the name */
	i = strlen(macHost.cname) - 1;
	if (macHost.cname[i] == '.')
		macHost.cname[i] = 0;
	
	for (i=0; i<NUM_ALT_ADDRS && macHost.addr[i]!=0; i++)
	{
		addrPtrs[i] = &macHost.addr[i];
	}
	addrPtrs[i] = NULL;
	
	return(&unixHost);
}

#endif

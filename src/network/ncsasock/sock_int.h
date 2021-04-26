/*
 * BSD-style socket emulation library for the Mac
 * Original author: Tom Milligan
 * Current author: Charlie Reiman - creiman@ncsa.uiuc.edu
 *
 * This source file is placed in the public domian.
 * Any resemblance to NCSA Telnet, living or dead, is purely coincidental.
 *
 *      National Center for Supercomputing Applications
 *      152 Computing Applications Building
 *      605 E. Springfield Ave.
 *      Champaign, IL  61820
 */

/*
 * Internal prototypes for the socket library.
 * There is duplication between socket.ext.h and socket.int.h, but
 * there are too many complications in combining them.
 */

#ifndef _SOCK_INT_
#define _SOCK_INT_

typedef enum spin_msg
	{
	SP_MISC,			/* some weird thing */
	SP_SELECT,			/* in a select call */
	SP_NAME,			/* getting a host by name */
	SP_ADDR,			/* getting a host by address */
	SP_TCP_READ,		/* TCP read call */
	SP_TCP_WRITE,		/* TCP write call */
	SP_UDP_READ,		/* UDP read call */
	SP_UDP_WRITE,		/* UDP write call */
	SP_SLEEP			/* sleeping */
	} spin_msg;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __socket_ext__
typedef int (*SpinFn)(spin_msg msg,long param);
#endif /* __socket_ext__ */

/* spin routine prototype, doesn't work. Sigh. */
/*extern int (*spinroutine)(spin_msg mesg,long param);*/

/* tcpglue.c */

OSErr xPBControlSync(TCPiopb *pb);
OSErr xPBControl(TCPiopb *pb, TCPIOCompletionProc completion);


OSErr xOpenDriver(void);
OSErr xTCPCreate(int buflen, TCPNotifyProc notify, TCPiopb *pb);
OSErr xTCPPassiveOpen(TCPiopb *pb, short port, TCPIOCompletionProc completion);
OSErr xTCPActiveOpen(TCPiopb *pb, short port, long rhost, short rport, TCPIOCompletionProc completion);
OSErr xTCPRcv(TCPiopb *pb, char *buf, int buflen, int timeout, TCPIOCompletionProc completion);
OSErr xTCPNoCopyRcv(TCPiopb *,rdsEntry *,int,int,TCPIOCompletionProc);
OSErr xTCPBufReturn(TCPiopb *pb,rdsEntry *rds,TCPIOCompletionProc completion);
OSErr xTCPSend(TCPiopb *pb, wdsEntry *wds, Boolean push, Boolean urgent, TCPIOCompletionProc completion);
OSErr xTCPClose(TCPiopb *pb,TCPIOCompletionProc completion);
OSErr xTCPAbort(TCPiopb *pb);
OSErr xTCPRelease(TCPiopb *pb);
int xTCPBytesUnread(SocketPtr sp);
int xTCPBytesWriteable(SocketPtr sp);
int xTCPWriteBytesLeft(SocketPtr sp);
int xTCPState(TCPiopb *pb);
OSErr xUDPCreate(SocketPtr sp,int buflen,ip_port port);
OSErr xUDPRead(SocketPtr sp,UDPIOCompletionProc completion);
OSErr xUDPBfrReturn(SocketPtr sp);
OSErr xUDPWrite(SocketPtr sp,ip_addr host,ip_port port,miniwds *wds,
		UDPIOCompletionProc completion);
OSErr xUDPRelease(SocketPtr sp);
ip_addr xIPAddr(void);
long xNetMask(void);
unsigned short xMaxMTU(void);


/* socket.tcp.c */
pascal void sock_tcp_notify(
	StreamPtr tcpStream,
	unsigned short eventCode,
	Ptr userDataPtr,
	unsigned short terminReason,
	struct ICMPReport *icmpMsg);
int sock_tcp_new_stream(SocketPtr sp);
int sock_tcp_connect(SocketPtr sp, struct sockaddr_in *addr);
int sock_tcp_listen(SocketPtr sp);
int sock_tcp_accept(SocketPtr sp, struct sockaddr_in *from, Int4 *fromlen);
int sock_tcp_accept_once(SocketPtr sp, struct sockaddr_in *from, Int4 *fromlen);
int sock_tcp_recv(SocketPtr sp, char *buffer, int buflen, int flags);
int sock_tcp_can_read(SocketPtr sp);
int sock_tcp_send(SocketPtr sp, char *buffer, int count, int flags);
int sock_tcp_can_write(SocketPtr sp);
int sock_tcp_close(SocketPtr sp);

/* socket.udp.c */

int sock_udp_new_stream(SocketPtr sp);
int sock_udp_connect(SocketPtr sp,struct sockaddr_in *addr);
int sock_udp_recv(SocketPtr sp, char *buffer, int buflen, int flags, struct sockaddr_in *from, int *fromlen);
int sock_udp_can_recv(SocketPtr sp);
int sock_udp_send(SocketPtr sp, struct sockaddr_in *to, char *buffer, int count, int flags);
int sock_udp_can_send(SocketPtr sp);
int sock_udp_close(SocketPtr sp);

/* socket.util.c */

int sock_init(void);
void sock_close_all(void);
int sock_free_fd(int f);
void sock_dup_fd(int s,int s1);
void sock_clear_fd(int s);
void sock_init_fd(int s);
int sock_err(int err_code);
void sock_copy_addr(void *from, void *to, Int4 *tolen);
void sock_dump(void);
void sock_print(char *title, SocketPtr sp);
StreamHashEntPtr sock_find_shep(StreamPtr);
StreamHashEntPtr sock_new_shep(StreamPtr);
void *sock_fetch_pb(SocketPtr);


#ifdef NOTNOW_COMP_CODEWAR
/* these are from sock_ext.h -- can't include w/ sock_int.h due to conflict of spin_msg dup !*/
/* Socket.c */
 
int s_socket(Int4 domain, Int4 type, short protocol);
int s_bind(Int4 s, struct sockaddr *name, Int4 namelen);
int s_connect(Int4 s, struct sockaddr *addr, Int4 addrlen);
int s_listen(Int4 s, Int4 qlen);
int s_accept(Int4 s, struct sockaddr *addr, Int4 *addrlen);
int s_accept_once(Int4 s, struct sockaddr *addr, Int4 *addrlen);
int s_close(Int4 s);
int s_read(Int4 s, void *buffer, Int4 buflen);
int s_recv(Int4 s, void *buffer, Int4 buflen, Int4 flags);
int s_recvfrom(Int4 s, void *buffer, Int4 buflen, Int4 flags, struct sockaddr *sa_from, Int4 *fromlen);
int s_write(Int4 s, void *buffer, Int4 buflen);
int s_writev(Int4 s, struct iovec *iov, Int4 count);
int s_send(Int4 s, void *buffer, Int4 buflen, Int4 flags);
int s_sendto (Int4 s, void *buffer, Int4 buflen, Int4 flags, struct sockaddr *to, Int4 tolen);
int s_sendmsg(Int4 s,struct msghdr *msg,Int4 flags);
int s_really_send(Int4 s, void *buffer, Int4 count, Int4 flags, struct sockaddr_in *to);
int s_select(Int4 width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int s_getdtablesize(void);
int s_getsockname(Int4 s, struct sockaddr *name, Int4 *namelen);
int s_getpeername(Int4 s, struct sockaddr *name, Int4 *namelen);
int s_shutdown(Int4 s, Int4 how);
int s_fcntl(Int4 s, unsigned Int4 cmd, Int4 arg); 
int s_dup(Int4 s);
int s_dup2(Int4 s, Int4 s1);
int s_ioctl(Int4 d, Int4 request, Int4 *argp);  /* argp is really a caddr_t */
int s_setsockopt(Int4 s, Int4 level, Int4 optname, char *optval, Int4 optlen);
int s_setspin(SpinFn routine);
SpinFn s_getspin(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /*_SOCK_INT_*/

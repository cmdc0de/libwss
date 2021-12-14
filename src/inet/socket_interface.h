
#ifndef SOCKET_INTERFACE_H_
#define SOCKET_INTERFACE_H_

#include "socket_typedefs.h"
#include "../error_type.h"
#include "inetaddress_v4.h"

namespace wss {
	/**
	*	@author Demetrius Comes
	*
	*	Base socket class, pretty much just here to define the interface each os derived class needs to support.
	*/
	class BaseSocketInterface {
	public:
		///Variable we set the FD to if we don't have a socket
		static const int BAD_SOCKET = OSSOCKTRAITS::BAD_SOCKET;
		///the type we use for socket FD
		typedef OSSOCKTRAITS::SOCKET SOCKET;
		typedef OSSOCKTRAITS::socklen_t socklen_t;
		///Error returned from functions like send and recieve if an error occurred.
		static const int SOCK_ERROR = OSSOCKTRAITS::SOCK_ERROR;
		enum ERROR_CODE {
			SNO_OS_SUPPORT	= OSSOCKTRAITS::SNO_OS_SUPPORT,
			SNO_ERROR		= OSSOCKTRAITS::SNO_ERROR,
			SEWOULDBLOCK	= OSSOCKTRAITS::SEWOULDBLOCK,
			SEINPROGRESS	= OSSOCKTRAITS::SEINPROGRESS,
			SEALREADY		 =  OSSOCKTRAITS::SEALREADY,
			SENOTSOCK        =  OSSOCKTRAITS::SENOTSOCK,
			SEDESTADDRREQ    = OSSOCKTRAITS::SEDESTADDRREQ,
			SEMSGSIZE        = OSSOCKTRAITS::SEMSGSIZE,
			SEPROTOTYPE      = OSSOCKTRAITS::SEPROTOTYPE,
			SENOPROTOOPT     = OSSOCKTRAITS::SENOPROTOOPT,
			SEPROTONOSUPPORT = OSSOCKTRAITS::SEPROTONOSUPPORT,
			SESOCKTNOSUPPORT = OSSOCKTRAITS::SESOCKTNOSUPPORT,
			SEOPNOTSUPP      = OSSOCKTRAITS::SEOPNOTSUPP,
			SEPFNOSUPPORT    = OSSOCKTRAITS::SEPFNOSUPPORT,
			SEAFNOSUPPORT    = OSSOCKTRAITS::SEAFNOSUPPORT,
			SEADDRINUSE      = OSSOCKTRAITS::SEADDRINUSE,
			SEADDRNOTAVAIL   = OSSOCKTRAITS::SEADDRNOTAVAIL,
			SENETDOWN        = OSSOCKTRAITS::SENETDOWN,
			SENETUNREACH     = OSSOCKTRAITS::SENETUNREACH,
			SENETRESET       = OSSOCKTRAITS::SENETRESET,
			SECONNABORTED    = OSSOCKTRAITS::SECONNABORTED,
			SECONNRESET      = OSSOCKTRAITS::SECONNRESET,
			SENOBUFS         = OSSOCKTRAITS::SENOBUFS,
			SEISCONN         = OSSOCKTRAITS::SEISCONN,
			SENOTCONN        = OSSOCKTRAITS::SENOTCONN,
			SESHUTDOWN       = OSSOCKTRAITS::SESHUTDOWN,
			SETOOMANYREFS    = OSSOCKTRAITS::SETOOMANYREFS,
			SETIMEDOUT       = OSSOCKTRAITS::SETIMEDOUT,
			SECONNREFUSED    = OSSOCKTRAITS::SECONNREFUSED,
			SELOOP           = OSSOCKTRAITS::SELOOP,
			SENAMETOOLONG    = OSSOCKTRAITS::SENAMETOOLONG,
			SEHOSTDOWN       = OSSOCKTRAITS::SEHOSTDOWN,
			SEHOSTUNREACH    = OSSOCKTRAITS::SEHOSTUNREACH,
			SENOTEMPTY       = OSSOCKTRAITS::SENOTEMPTY,
			SEPROCLIM        = OSSOCKTRAITS::SEPROCLIM,
			SEUSERS          = OSSOCKTRAITS::SEUSERS,
			SEDQUOT          = OSSOCKTRAITS::SEDQUOT,
			SESTALE          = OSSOCKTRAITS::SESTALE,
			SEREMOTE         = OSSOCKTRAITS::SEREMOTE,
			SENOMEM			 = OSSOCKTRAITS::SENOMEM,
			SEPIPE			 = OSSOCKTRAITS::SEPIPE,
			SEBADF			 = OSSOCKTRAITS::SEBADF,
			SEMFILE			 = OSSOCKTRAITS::SEMFILE,
			SEPERM			 = OSSOCKTRAITS::SEPERM,
			SEDEADLK	     = OSSOCKTRAITS::SEDEADLK,
			SEINTR			 = OSSOCKTRAITS::SEINTR,
			SEIO			 = OSSOCKTRAITS::SEIO,
			SEINVAL			 = OSSOCKTRAITS::SEINVAL,
		};
	public:
		/**
		* @date  11/6/2003 9:44:01 AM
		* @return  ErrorType 
		*  
		*/
		static ErrorType initialize();
	public:
		virtual ~BaseSocketInterface();
		/**
		* @date  11/6/2003 9:46:09 AM
		* @return  ErrorType 
		*  
		*  Allows you to SO_LINGER off
		*	By default a socket will Linger on close if unsent data is present
		*	
		*/
		ErrorType setLingerOff();
		/**
		* @date  11/6/2003 9:46:48 AM
		* @return  ErrorType 
		* @param  const int &opt
		*  
		*  opt = size of send buffer
		*/
		ErrorType setSendBuffer(const int &opt);
		/**
		* @date  11/6/2003 9:47:11 AM
		* @return  ErrorType 
		* @param  const int &opt
		*  
		*  opt = size of receive buffer
		*/
	    ErrorType setReceiveBuffer(const int &opt);
		/**
		* @date  11/6/2003 9:47:37 AM
		* @return  ErrorType 
		* @param  const bool &opt
		*  
		*  Allows the combination of port number and ip address to be 
		*	reused.
		*	Allows the socket to be bound to an address that is already in use. 
		*/
		ErrorType setReuseAddress(const bool &opt);
		/**
		* @date  11/6/2003 10:41:51 AM
		* @return  void 
		*  
		*  closes the socket
		*	For TCP closing a socket causes an implict shutdown of both the write and the read
		*	sides of the connection.
		*/
		void closeSocket();
		/**
		* @date  11/6/2003 1:18:57 PM
		* @return  bool 
		*  
		*  returns true if socket is non blocking
		*/
		bool isNonBlocking() const {return !IsBlocking();}
		/**
		* @date  11/6/2003 1:19:08 PM
		* @return  bool 
		*  
		*  returns true if socket is blocking
		*/
		bool IsBlocking() const {return mIsBlocking;}
		/**
		* @date  11/6/2003 10:01:00 AM
		* @return  ErrorType 
		*  
		*  returns the last error on the socket
		*/
		ErrorType getLastError();
		/**
		* @date  11/6/2003 1:10:49 PM
		* @return  ErrorType 
		*  
		*  sets the socket non blocking
		*/
		ErrorType nonBlock();
		/**
		* @return  ErrorTypeClass 
		*  
		*  sets the socket to turn off nagel algorithm
		*/
		ErrorType setNagelOff();
		/**
		* @date  11/7/2003 10:18:52 PM
		* @return  uint32 
		*  
		*  returns the total amount of data that can be
		*	received in a single call to recv (receive)
		*
		*	Use to determine the amount of data pending in the network's input buffer that 
		*	can be read from socket s. The argp parameter points to an 
		*	unsigned long value in which ioctlsocket stores the result. 
		*	We return the amount of data that can be read in a single call to the recv 
		*	function, which may not be the same as the total amount of data queued 
		*	on the socket. If s is message oriented (for example, type SOCK_DGRAM), 
		*	This call still returns the amount of pending data in the network buffer, 
		*	however, the amount that can actually be read in a single call to the 
		*	recv function is limited to the data size written in the send or sendto 
		*	function call. 
		*	
		*	returns SOCK_ERROR on error (call getLastError() to see what it was)
		*	or the number of bytes that can be read.
		*/
		int32_t bytesToRead();
		/**
		* @date  11/6/2003 1:18:41 PM
		* @return  int 
		*  
		*  returns current send buffer size
		*/
		int getSendBufferSize();
		/**
		* @date  11/6/2003 1:18:47 PM
		* @return  int 
		*  
		*  returns current receive buffer size
		*/
		int getReceiveBufferSize();
		/**
		* @date  11/6/2003 1:11:03 PM
		* @return  ErrorType 
		*  
		*  binds the socket to 127.0.0.1 and any port
		*/
		ErrorType bind();
		/**
		* @date  11/6/2003 1:11:23 PM
		* @return  ErrorType 
		* @param  const PortNum &Port
		*  
		*  Binds the socket to any 127.0.0.1 and to the specified port
		*/
		ErrorType bind(const PortNum &Port);
		/**
		* @date  11/6/2003 1:12:00 PM
		* @return  ErrorType 
		* @param  const InetAddressV4 &addr
		*  
		*  Binds the socket to the specified address but any port
		*/
		ErrorType bind(const InetAddressV4 &addr);
		/**
		* @date  11/6/2003 1:12:14 PM
		* @return  ErrorType 
		* @param  const InetAddressV4 &addr
		* @param  const PortNum &Port
		*  
		*  Binds the socket to the specified addrress and port
		*/
		ErrorType bind(const InetAddressV4 &addr, const PortNum &Port);
		/**
		* @date  12/4/2003 3:38:26 PM
		* @return  ErrorType 
		* @param  InetAddressV4 &addr
		* @param  PortNum &port
		*  
		*  returns local address and port of this socket
		*/
		ErrorType getLocalAddrAndPort(InetAddressV4 &addr, PortNum &port);
	protected:
		/**
		* @date  11/6/2003 10:06:06 AM
		* @param  SOCKET s
		* @param  bool bBlocking socket - since this ctor is only used by a listening
		*	socket and any socket accepted from a listening socket inherits it's blocking
		*	type
		*  
		*	If there is a way in windows to ask the socket if it is non blocking
		*	then this constructor could do away and so could the mbIsBlocking member variable
		*	it is only used to keep blocking state and needs to be passed in 
		*	from a listening socket
		*/
		BaseSocketInterface(SOCKET s, bool bIsBlocking);
		/**
		* @date  11/6/2003 10:06:44 AM
		* @param  SOCKET s
		*  
		*  Ctor used by createTCPSocket 
		*/
		BaseSocketInterface(SOCKET s);
	protected:
		void setSocket(SOCKET t) {
			mSock = t;
			onSetSocket();
		}
		virtual void onSetSocket()=0;
		virtual void onCloseSocket()=0;
		SOCKET getSocket() const {return mSock;}
		ERROR_CODE getLastErrorCode() const {return mLastError;}
		void setLastErrorCode(ERROR_CODE e);
		void setLastErrorCode(); 
		void setBlocking(bool b) {mIsBlocking = b;}
	private:
		SOCKET			mSock;				//the actual socket fd
		bool			mIsBlocking;		//true if nonblock has been called
		ERROR_CODE		mLastError;			//last error on socket
	};
	/**
	*	@author Demetrius class
	*	@date 11-6-03
	*
	*	class wrapper for socket functions
	*	Names of the public variables and enum must be the same accross operating systems
	*/
	class TCPSocketInterface : public BaseSocketInterface
	{
	public:
		///The possible states a socket can be in
		enum SOCKET_STATE {
			NOT_SOCKET		=	0,
			NO_CONNECTION		=	(1<<0),
			CONNECTING		=	(1<<1),
			CONNECTED		=	(1<<2),
			WRITE_CLOSED		=	(1<<3),
			READ_CLOSED		=	(1<<4),
			FULL_CLOSED		=	(WRITE_CLOSED|READ_CLOSED),
			LISTENING		=	(1<<5)
		};
	public:
		/**
		* @date  11/6/2003 9:45:07 AM
		* @return  Win32SocketTraits* 
		*  
		*  Creates a TCP socket and returns a Win32SocketTraits pointer
		*	if the pointer is null, the socket did not create.
		*/
		static TCPSocketInterface* createTCPSocket();
	public:
		/**
		* @date  11/6/2003 9:45:45 AM
		*  
		*  dtor - will close socket and release all OS level resources
		*/
		virtual ~TCPSocketInterface();
		/**
		* @date  11/6/2003 10:41:33 AM
		* @return  int 
		* @param  const std::basic_string<char> &s
		*  
		*  convience function that calls
		*	send(const char *buf, uint32 size);
		*/
		int send(const std::basic_string<char> &s);
		/**
		* @date  11/6/2003 10:38:59 AM
		* @return  int 
		* @param  const char *buf
		* @param  uint32 size
		*  
		*  Send data to the connected socket
		*	return values:
		*		SOCK_ERROR means there was an error during sending the socket has been closed
		*		0	mean there was an error but one that can be recovered from 
		*				meaning a EWOULD_BLOCK (on a non blocking socket)
		*		> 0	the number of bytes that have been sent to the remote host
		*
		*	FOR UDP SOCKETS:
		*		EMSGSIZE = packet was to large to be transmitted with a udp socket
		*	this the data is actually transmitted I am closing the socket which seems
		*	reasonable, if not let me know.  -Demetrius
		*/
		int send(const char *buf, uint32_t size);
		/**
		* @date  11/6/2003 10:56:52 AM
		* @return  int 
		* @param  char *pBuf
		* @param  sint32 nSizeOfBuf
		*  
		*	return values and what they mean
		*	0 = socket was close either do to an error or a graceful shutdown
		*	-1 for a non blocking socket that returns EWOULD_BLOCK
		*	> 0 for the number of bytes actually received.
		*	You can still check for a graceful shutdown by getting a 0 back from receive
		*	then if getLastError().getErrorCode()==SNO_ERROR it is a graceful shutdown.
		*
		*  This is different than Berkely sockets return codes
		*	0 for graceful close
		*	-1 for error
		*	>0 for bytes received on a successfull call
		*	we'll change it a bit so users of this library do not have to do the following:
		*	if(receive()==-1) {
		*		if(getLastError().getOSError()!=EWOULD_BLOCK) {
		*			actual error
		*		else 
		*			all is ok
		*	}
		*   This is done because Non blocking sockets for our world are much more popular
		*/
		int receive(char *pBuf, uint32_t nSizeOfBuf);
		/**
		* @date  11/6/2003 1:10:24 PM
		* @return  ErrorType 
		*  
		*  closed the read side of the tcp connection
		*/
		ErrorType shutRead();
		/**
		* @date  11/6/2003 1:10:37 PM
		* @return  ErrorType 
		*  
		*  closes the write side of the tcp connection
		*/
		ErrorType shutwrite();
		/**
		* @date  11/6/2003 1:07:41 PM
		* @return  ErrorType 
		* @param  const InetAddressV4 &addr
		* @param  short nPort
		*  
		*  just calls connect(const InetAddressV4 &addr, short nPort, struct timeval *waitTime)
		*	with an infinite wait.
		*/
		ErrorType connect(const InetAddressV4 &addr, short nPort);
		/**
		* @date  2/23/2004 5:53:32 PM
		* @return  ErrorType 
		* @param  const InetAddressV4 &addr
		* @param  short nPort
		* @param  uint32 waitTimeMS
		*  
		*  just calls const InetAddressV4 &addr, short nPort, struct timeval *waitTime
		*	converting Millis to timeval
		*/
		ErrorType connect(const InetAddressV4 &addr, short nPort, uint32_t waitTimeMS);
		/**
		* @date  11/6/2003 1:04:16 PM
		* @return  ErrorType 
		* @param  const InetAddressV4 &addr
		* @param  short nPort
		* @param  struct timeval *waitTime
		*  
		*  connects to a remote address and port
		*	if waitTime is null then the connect is done with no wait
		*	if waitTime.tv_sec and .tv_usec are 0 then it's an infinite wait
		*
		*	if the connect fails for any reason besides a timeout (in which the ErrorType.getOSError()
		*		will return SETIMEOUT) the socket will be closed.
		*
		*/
		ErrorType connect(const InetAddressV4 &addr, short nPort, struct timeval *waitTime);
		/**
		* @date  11/6/2003 1:12:29 PM
		* @return  ErrorType 
		* @param  int backlog
		*  
		*  Sets this socket up as a listening socket
		*/
		ErrorType listen(int backlog);
		/**
		* @date  11/6/2003 1:12:44 PM
		* @return  TCPSocketInterface* 
		*  
		*  if return is null no new sockets to accept
		*   if a socket fatal error is incurred this listening socket may be closed
		*	after this call...Call listener.getLastError() or listener.isInState(LISTENING)
		*	to find out
		*/
		TCPSocketInterface* accept();
		/**
		* @date  11/6/2003 1:19:26 PM
		* @return  bool 
		* @param  SOCKET_STATE s
		*  
		*  returns (internalState & s)
		*/
		bool isInState(SOCKET_STATE s) const;
		/**
		* @date  11/6/2003 1:19:48 PM
		* @return  bool 
		*  
		*  Convience function to check to see if socket is in connected state
		*/
		bool isConnected() const {return isInState(CONNECTED);}
		/**
		* @date  11/6/2003 10:00:44 AM
		* @return  ErrorType 
		* @param  InetAddressV4 &addr
		* @param  PortNum &port
		*  
		*  get address of system you are connected to
		*/
		ErrorType getPeerAddress(InetAddressV4 &addr, PortNum &port);
	private:
		/**
		* @date  11/6/2003 10:06:06 AM
		* @param  SOCKET s
		* @param  const InetAddressV4 &mAddr
		* @param  const PortNum &port
		* @param  bool bBlocking socket - since this ctor is only used by a listening
		*	socket and any socket accepted from a listening socket inherits it's blocking
		*	type
		*  
		*  Ctor used by the accept function to return a newly connected socket
		*/
		TCPSocketInterface(SOCKET s, const InetAddressV4 &mAddr, const PortNum &port, bool bIsBlocking);
		/**
		* @date  11/6/2003 10:06:44 AM
		* @param  SOCKET s
		*  
		*  Ctor used by createTCPSocket 
		*/
		TCPSocketInterface(SOCKET s);
	private:
		virtual void onSetSocket() {
			if(getSocket()==BAD_SOCKET) {
				setSocketState(NOT_SOCKET);
			}
		}
		virtual void onCloseSocket();
		SOCKET_STATE getSocketState() const {return mSockState;}
		void setSocketState(SOCKET_STATE r);
	private:
		SOCKET_STATE	mSockState;			//current socket state
		InetAddressV4	mRemoteAddr;		//address of socket
		PortNum			mRemotePort;		//port of socket
	private:
		//broken
		TCPSocketInterface(const TCPSocketInterface &);
		TCPSocketInterface &operator=(const TCPSocketInterface &r);
	};
} 
#endif

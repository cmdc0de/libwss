

#ifndef WSS_TCP_H
#define WSS_TCP_H

#include "socket_typedefs.h"
#include "../error_type.h"
#include <memory>

namespace wss {

/**
*	@Author Demetrius Comes
*
*	Common base class for socket classes
*/
template<typename SocketType>
class BaseSocket {
public:
	/**
	* @date  11/6/2003 3:11:39 PM
	* @return  ErrorType 
	*  
	*  sets the socket non blocking
	*/
	ErrorType setNonBlocking() {return getImpl()->nonBlock();}
	/**
	* @return  ErrorType
	*  
	*  sets the socket to turn off nagel algorithm
	*/
	ErrorType setNagelOff() {return getImpl()->setNagelOff();}
	/**
	* @date  11/7/2003 10:40:39 PM
	* @return  bool 
	*  
	*  
	*/
	bool isNonBlocking() {return getImpl()->isNonBlocking();}
	/**
	* @date  11/6/2003 3:11:50 PM
	* @return  ErrorType 
	*  
	*  returns the last error on the socket
	*	even after the socket is closed
	*/
	ErrorType getLastError() {return getImpl()->getLastError();}
	/**
	* @date  11/6/2003 3:13:31 PM
	* @return  ErrorType 
	*  
	*  Allows you to SO_LINGER off
	*	By default a socket will Linger on close if unsent data is present
	*/
	ErrorType setLingerOff() {return getImpl()->setLingerOff();}
	/**
	* @date  11/6/2003 3:14:26 PM
	* @return  ErrorType 
	* @param  const int &opt
	*  
	*  sets the size of the send buffer
	*/
	ErrorType setSendBuffer(const int &opt) {return getImpl()->setSendBuffer(opt);}
	/**
	* @date  11/6/2003 1:18:41 PM
	* @return  int 
	*  
	*  returns current send buffer size
	*/
	int getSendBufferSize() {return getImpl()->getSendBufferSize();}
	/**
	* @date  11/6/2003 1:18:47 PM
	* @return  int 
	*  
	*  returns current receive buffer size
	*/
	int getReceiveBufferSize() {return getImpl()->getReceiveBufferSize();}
	/**
	* @date  11/6/2003 3:14:28 PM
	* @return  ErrorType 
	* @param  const int &opt
	*  
	*  
	*/
	ErrorType setReceiveBuffer(const int &opt) {return getImpl()->setReceiveBuffer(opt);}
	/**
	* @date  11/6/2003 3:14:30 PM
	* @return  ErrorType 
	* @param  const bool &opt
	*  
	* Allows the combination of port number and ip address to be 
	*	reused.
	*	Allows the socket to be bound to an address that is already in use. 
	*/
	ErrorType setReuseAddress(const bool &opt) {return getImpl()->setReuseAddress(opt);}
	/**
	* @date  11/6/2003 1:11:03 PM
	* @return  ErrorType 
	*  
	*  binds the socket to 127.0.0.1 and any port
	*/
	ErrorType bind() {return getImpl()->bind();}
	/**
	* @date  11/6/2003 1:11:23 PM
	* @return  ErrorType 
	* @param  const PortNum &Port
	*  
	*  Binds the socket to any 127.0.0.1 and to the specified port
	*/
	ErrorType bind(const PortNum &Port) {return getImpl()->bind(Port);}
	/**
	* @date  11/6/2003 1:12:00 PM
	* @return  ErrorType 
	* @param  const InetAddressV4 &addr
	*  
	*  Binds the socket to the specified address but any port
	*/
	ErrorType bind(const InetAddressV4 &addr)  {return getImpl()->bind(addr);}
	/**
	* @date  11/6/2003 1:12:14 PM
	* @return  ErrorType 
	* @param  const InetAddressV4 &addr
	* @param  const PortNum &Port
	*  
	*  Binds the socket to the specified addrress and port
	*/
	ErrorType bind(const InetAddressV4 &addr, const PortNum &Port) {return getImpl()->bind(addr,Port);}
	/**
	* @date  11/7/2003 10:33:45 PM
	* @return  uint32 
	*  
	*  	Use to determine the amount of data pending in the network's input buffer that 
	*	can be read from socket s. We return the amount of data that can be read in a single call to the recv 
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
	uint32_t bytesToRead() {return getImpl()->bytesToRead();}
	/**
	* @date  2/1/2004 5:01:40 PM
	* @return  void 
	*  
	*  
	*/
	void closeSocket() {getImpl()->closeSocket();}
	/**
	* @date  2/1/2004 5:01:44 PM
	*  
	*  
	*/
	virtual ~BaseSocket() {}
	/**
	* @date  2/20/2004 6:43:40 PM
	* @return  bool 
	*  
	*  only checks to insure we have a internal representation
	*	i.e. mImpl != 0
	*/
	bool ok() {return (mImpl.get()!=0);}
protected:
	BaseSocket(const std::shared_ptr<SocketType> &os) : mImpl(os) {}
	BaseSocket(const BaseSocket &bs) : mImpl(bs.getImpl()) {}
	const std::shared_ptr<SocketType> &getImpl() const {return mImpl;}
	std::shared_ptr<SocketType> &getImpl() {return mImpl;}
private:
	std::shared_ptr<SocketType> mImpl;
};

/**
*	@author Demetrius Comes
*
*	TCP Server Socket:  represents a socket created from a Listen socket's call to getNewConnections
*/
class TCPServerSocket : public BaseSocket<TCPSocketInterface> {
public:
	friend class ListenerSocket;
public:
	/**
	* @date  11/6/2003 3:18:05 PM
	* @param  const TCPServerSocket &r
	*  
	*  copy ctor - we allow the copy ctor because we use the pimpl pattern
	*	with a reference count
	*/
	TCPServerSocket(const TCPServerSocket &r) : BaseSocket<TCPSocketInterface> (r) {}
	virtual ~TCPServerSocket() {}
	/**
	* @date  11/6/2003 3:12:09 PM
	* @return  ErrorType 
	* @param  InetAddressV4 &addr
	* @param  PortNum &port) {return getImpl()->getAddress(addr
	* @param  port
	*  
	* returns address and port of the remoted side of a connected socket 
	*/
	ErrorType getPeerAddress(InetAddressV4 &addr, PortNum &port) {return getImpl()->getPeerAddress(addr,port);}
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
	int send(const char *data, uint32_t size) {
		return getImpl()->send(data,size);
	}
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
	int receive(char *data, uint32_t size) {
		return getImpl()->receive(data,size);
	}
protected:
	TCPServerSocket(TCPSocketInterface *os) 
		: BaseSocket<TCPSocketInterface>(std::shared_ptr<TCPSocketInterface>(os)) {}
};

/**
*	@Author Demetrius Comes
*	
*	TCPClientSocket adds a public ctor and the connect function
*		to the TCPServerSocket
*/
class TCPClientSocket : public TCPServerSocket {
public:
	/**
	* @date  11/6/2003 3:30:09 PM
	* @param  TcpOsSocketTraits *os
	*  
	*  tcp client socket ctor
	*/
	TCPClientSocket(TCPSocketInterface *os) 
		: TCPServerSocket(os) {}
public:
	/**
	* @date  11/6/2003 3:31:28 PM
	* @param  const TCPSocket &r
	*  
	*  
	*/
	TCPClientSocket(const TCPClientSocket &r) : TCPServerSocket (r) {}
	/**
	* @date  2/1/2004 5:02:33 PM
	* @return  TCPClientSocket 
	*  
	*  
	*/
	static TCPClientSocket create() {
		TCPSocketInterface *tcp = TCPSocketInterface::createTCPSocket();
		return TCPClientSocket(tcp);
	}
	/**
	* @date  11/6/2003 3:31:29 PM
	*  
	*  
	*/
	virtual ~TCPClientSocket() {}
	/**
	* @date  11/6/2003 1:07:41 PM
	* @return  ErrorType 
	* @param  const InetAddressV4 &addr
	* @param  short nPort
	*  
	*  just calls connect(const InetAddressV4 &addr, short nPort, struct timeval *waitTime)
	*	with an infinite wait.
	*/
	ErrorType connect(const InetAddressV4 &addr, short nPort) {
		return getImpl()->connect(addr,nPort);
	}
	/**
	* @date  11/6/2003 1:04:16 PM
	* @return  ErrorType 
	* @param  const InetAddressV4 &addr
	* @param  short nPort
	* @param  uint32 waitTimeMS
	*  
	*  connects to a remote address and port
	*	if waitTimeMS 0 then is will be a select with no wait
	*	otherwise waitTimeMS will be the max time to wait for the socket to connect
	*
	*	if the connect fails for any reason besides a timeout (in which the ErrorType.getOSError()
	*		will return SETIMEOUT) the socket will be closed.
	*
	*/
	ErrorType connect(const InetAddressV4 &addr, short nPort, uint32_t waitTimeMS);
};

/**
*	@author Demetrius Comes
*/
class ListenerSocket : public BaseSocket<TCPSocketInterface> {
public:
	enum {DEFAULT_BACKLOG = 5};
public:
	/**
	* @date  1/13/2005 2:14:35 PM
	* @param  const RefCountedPtr<TCPSocketInterface> &impl
	*  
	*  
	*/
	ListenerSocket(const std::shared_ptr<TCPSocketInterface> &impl) : BaseSocket<TCPSocketInterface>(impl) {
	}
	/**
	* @date  1/13/2005 2:14:37 PM
	* @param  const ListenerSocket &ls
	*  
	*  
	*/
	ListenerSocket(const ListenerSocket &ls) : BaseSocket<TCPSocketInterface>(ls) {
	}
	/**
	* @date  1/13/2005 2:14:39 PM
	* @return  ErrorType 
	* @param  const PortNum &port
	*  
	*  
	*/
	ErrorType listen(const PortNum &port) {
		return listen(InetAddressV4(INADDR_ANY),port,DEFAULT_BACKLOG,false);
	}
	/**
	* @date  1/13/2005 2:14:40 PM
	* @return  ErrorType 
	* @param  const PortNum &port
	* @param  int nBackLog
	*  
	*  
	*/
	ErrorType listen(const PortNum &port, int nBackLog) {
		return listen(InetAddressV4(INADDR_ANY),port,nBackLog,false);
	}
	/**
	* @date  1/13/2005 2:14:42 PM
	* @return  ErrorType 
	* @param  const PortNum &port
	* @param  int nBackLog
	* @param  bool blocking
	*  
	*  
	*/
	ErrorType listen(const PortNum &port, int nBackLog, bool blocking) {
		return listen(InetAddressV4(INADDR_ANY),port,nBackLog,blocking);
	}
	/**
	* @date  1/13/2005 2:14:44 PM
	* @return  ErrorType 
	* @param  const InetAddressV4 &addr
	* @param  const PortNum &port
	* @param  int nBackLog
	* @param  bool blocking
	*  
	*  
	*/
	ErrorType listen(const InetAddressV4 &addr, const PortNum &port,int nBackLog, bool blocking) {
		ErrorType et = bind(addr,port);
		if(et) {
			et = getImpl()->listen(nBackLog);
			if(!blocking) {
				et = setNonBlocking();
			}
		}
		return et;
	}
	/**
	* @date  1/13/2005 2:14:45 PM
	* @return  ListenerSocket 
	*  
	*  
	*/
	static ListenerSocket create() {
		TCPSocketInterface *tcp = TCPSocketInterface::createTCPSocket();
		return ListenerSocket(std::shared_ptr<TCPSocketInterface>(tcp));
	}
	/**
	* @date  1/13/2005 2:14:52 PM
	* @return  ErrorType 
	* @param  TCPSocketType *&outGoing
	*  
	*  template
	*/
	template<typename TCPSocketType>
	ErrorType getNewConnection(TCPSocketType *&outGoing) {
		(outGoing) = 0;
		TCPSocketInterface* newSock = getImpl()->accept();
		ErrorType et;
		if(newSock!=0) {
			outGoing = (new TCPSocketType(newSock));
		} else {
			et= getImpl()->getLastError();
			if(et || (!et && et.getOSError()==TCPSocketInterface::SEWOULDBLOCK && getImpl()->isNonBlocking())) {
				et = ErrorType();
			}
		}
		return et;
	}
	virtual ~ListenerSocket();
};
}
#endif


#include "linux_socket_traits.h"
#include "socket_typedefs.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/resource.h>

using namespace wss;

#define INET_SOCK_ERROR ErrorType(ErrorType::codeOSSpecific,errno)
#define SET_AND_RETURN setLastErrorCode();return ErrorType(ErrorType::codeOSSpecific,errno)

//////////////////////////////////////////////////
//static
/////////////////////////////////////////////
ErrorType BaseSocketInterface::initialize() {
	static bool inited = false;
	if(!inited) {
		inited = true;
		rlimit lim;
		if(getrlimit(RLIMIT_NOFILE,&lim)<0) {
			WSSDEBUGMSG("Can't get rlimit for number of files");
		} else {
			WSSDEBUGMSG("Current user limit is: " << lim.rlim_cur);
			WSSDEBUGMSG("Current system limit is: " << lim.rlim_max);
		}
	}
	return ErrorType();
}

void BaseSocketInterface::setLastErrorCode() {
	setLastErrorCode(ERROR_CODE(errno));
}

ErrorType BaseSocketInterface::nonBlock() {
	int flags;
	flags = fcntl(getSocket(), F_GETFL, 0);
	flags |= O_NONBLOCK;
	if(fcntl(getSocket(), F_SETFL, flags) < 0) {
		SET_AND_RETURN;
	}
	return setBlocking(false), ErrorType();
}

int32 BaseSocketInterface::bytesToRead() {
	uint32 val;
	int retVal = ::ioctl(getSocket(),FIONREAD,&val);
	if(retVal==SOCK_ERROR) {
		setLastErrorCode();
		return SOCK_ERROR;
	}
	return val;
}

void BaseSocketInterface::closeSocket() {
	if(getSocket()!=BAD_SOCKET) {
		::close(getSocket());
		setSocket(BAD_SOCKET);
		onCloseSocket();
	}
}


////////////////////////////////////////////////////////////
//  TCP
///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
//		STATIC
///////////////////////////////////////////////////////
TCPSocketInterface* TCPSocketInterface::createTCPSocket() {
	SOCKET s = (SOCKET)::socket(PF_INET,SOCK_STREAM,0);
	if(s==BAD_SOCKET) {
		WSSINFOMSG("createTCPSocket() failed because: " << ErrorType::getOSErrorString(errno));
		return 0;
	}
	return (new TCPSocketInterface(s));
}

//////////////////////////////////////////////////////
//	NON STATICS
////////////////////////////////////////////////////

//though Berkely sockets return
//	0 for graceful close
//	-1 for error
// >0 for bytes received on a successfull call
//  we'll change it a bit so users of this library do not have to do the following:
//	if(receive()==-1) {
//		if(getLastError().getOSError()!=EWOULD_BLOCK) {
//			actual error
//		else 
//			all is ok
//  }
//	instead we do the following
//	0 = socket was close either do to an error or a graceful shutdown
//	-1 for a non blocking socket that returns EWOULD_BLOCK
//	> 0 for the number of bytes actually received.
//  You can still check for a graceful shutdown by getting a 0 back from receive
//	then if getLastError().getErrorCode()==SNO_ERROR it is a graceful shutdown.
int TCPSocketInterface::receive(char *pBuf, sint32 nSizeOfBuf) {
	int ret = ::recv(getSocket(),pBuf,nSizeOfBuf-1,0);
	if(ret==SOCK_ERROR) {
		int nError = errno;
		if((nError!=SEWOULDBLOCK)) {
			ret = 0;
		}
		setLastErrorCode(ERROR_CODE(nError));
	} else if (ret==0) {
		setLastErrorCode(SECONNRESET);
		closeSocket();
	} else {
		pBuf[ret] = '\0';
	}
	return ret;
}

//return 0 for would block
//return >0 for bytes sent which can be < then size
//return -1 on error
int TCPSocketInterface::send(const char *buf, uint32 size) {
	int retVal = ::send(getSocket(),buf,size,0);
	if(retVal==SOCK_ERROR) {
		int nError = errno;
		setLastErrorCode(ERROR_CODE(nError));
		if(SEWOULDBLOCK==nError) {
			return 0;
		}
	}
	return retVal;
}


///////////////////////////////////////////////////
//The shutdown() call causes all or part of a full-duplex connection on the 
//socket associated with s to be shut down. If how is 0, further receives will be 
//disallowed. If how is 1, further sends will be disallowed. If how is 2, 
//further sends and receives will be disallowed. 
ErrorType TCPSocketInterface::shutRead() {
	if(SOCK_ERROR==::shutdown(getSocket(),0)) {
		SET_AND_RETURN;
	} else {
		setSocketState(READ_CLOSED);
		return ErrorType();
	}
}

///////////////////////////////////////////////////
//The shutdown() call causes all or part of a full-duplex connection on the 
//socket associated with s to be shut down. If how is 0, further receives will be 
//disallowed. If how is 1, further sends will be disallowed. If how is 2, 
//further sends and receives will be disallowed. 
///////////////////////////////////////////////////
ErrorType TCPSocketInterface::shutwrite() {
	if(SOCK_ERROR==::shutdown(getSocket(),1)) {
		SET_AND_RETURN;
	} else {
		setSocketState(WRITE_CLOSED);
		return ErrorType();
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//	Unix network programming:  W Richard Stvens
//
//	Berkely-derived implementation have the following tow rules regarding select and nonblocking connects:
//		1.  when the connection complets successfully the descriptor becomes writable
//		2.  when the connect establishment ecounters an error the descriptor becomes both readable and wriable
//
//	These two rules regarding select fall out from our rules in Section 6.3 about the conditions that make
//	a descriptor ready.  A TCP socket is writable if there is available space in the send buffer (which will always
//	be the case for a conneting socket, since we have not yet written anything to the socket) and the socket
//	is connected (which occurs only when the three-way handshake completes).  A pending error causes a 
//	socket to be both readable and writable
//
//  null wait time mean NO WAIT
///////////////////////////////////////////////////////////////////////////////////
ErrorType TCPSocketInterface::connect(const InetAddressV4 &addr, short nPort, struct timeval *waitTime) {
	struct sockaddr_in sockaddr;
	sockaddr.sin_addr = addr.getAddress();
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(nPort);
	setSocketState(CONNECTING);
	int retVal = ::connect(getSocket(),(struct sockaddr *) &sockaddr,sizeof(sockaddr_in));
	//did the connect work?
	if(retVal==SOCK_ERROR) {
		int nError = errno;
		//if not but because we are non blocking
		if(EWOULDBLOCK==nError || EINPROGRESS==nError) {
			fd_set readSet, writeSet, exceptionSet;
			FD_ZERO(&writeSet);
			FD_ZERO(&readSet);
			FD_ZERO(&exceptionSet);
			FD_SET(getSocket(),&writeSet);
			FD_SET(getSocket(),&readSet);
			int nSockets = select((int)getSocket()+1,&readSet,&writeSet,&exceptionSet,waitTime);
			//if select returns an error set the error (which will close the socket and return)
			if(nSockets==SOCK_ERROR) {
				SET_AND_RETURN;
			} else if (nSockets==0) {
				//if select returns 0 then this is a timeout situation
				setLastErrorCode(SETIMEDOUT);
				return ErrorType(ErrorType::codeOSSpecific,SETIMEDOUT);
			} else {
				// if the socket is in the exception set OR if in BOTH read and write sets we have an error
				if (FD_ISSET(getSocket(), &exceptionSet)) {
					//socket is in exception set close it
					setLastErrorCode(SECONNREFUSED);
					closeSocket();
					return ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
				} else if (FD_ISSET(getSocket(),&writeSet) || FD_ISSET(getSocket(),&readSet)) {
					int error;
					socklen_t len = sizeof(error);
					if(getsockopt(getSocket(),SOL_SOCKET,SO_ERROR,(char *)&error,&len) < 0) {
						WSSERRMSG("There is a pending error on the socket.");
						setLastErrorCode(SECONNREFUSED);
					} else {
						if(error==SNO_ERROR) {
							setLastErrorCode(SNO_ERROR);
							setSocketState(CONNECTED);
						} else {
							setLastErrorCode(BaseSocketInterface::ERROR_CODE(error));
							return ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
						}
					}
				} else {
					WSSERRMSG("Select error:  Socket is not in any fd_set???????");
					setLastErrorCode(SENOTSOCK);
					return getLastError();
				}
			}
		} else {
			setLastErrorCode(ERROR_CODE(nError));
			return INET_SOCK_ERROR;
		}
	} else {
		setLastErrorCode(SNO_ERROR);
		setSocketState(CONNECTED);
	}
	return ErrorType();
}

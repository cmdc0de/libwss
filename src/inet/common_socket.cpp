#include "socket_typedefs.h"

#ifdef WSS_LINUX
#include <netinet/tcp.h>
#endif

using namespace wss;

BaseSocketInterface::~BaseSocketInterface() {
}

ErrorType BaseSocketInterface::setLingerOff() {
	struct linger ld;
	ld.l_onoff = 0;
	ld.l_linger = 0;
	if(SOCK_ERROR==setsockopt(getSocket(),SOL_SOCKET,SO_LINGER,(char *) &ld, sizeof(ld))) {
		return setLastErrorCode(), ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	return ErrorType();
}

ErrorType BaseSocketInterface::setSendBuffer(const int &opt) {
	if(SOCK_ERROR==(setsockopt(getSocket(),SOL_SOCKET, SO_SNDBUF, (char *) &opt, sizeof(opt)))) {
		return setLastErrorCode(), ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	return ErrorType();
}

ErrorType BaseSocketInterface::setReceiveBuffer(const int &opt) {
	if(SOCK_ERROR==(setsockopt(getSocket(),SOL_SOCKET, SO_RCVBUF, (char *) &opt, sizeof(opt)))) {
		return setLastErrorCode(), ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	return ErrorType();
}

ErrorType BaseSocketInterface::setReuseAddress(const bool &opt) {
	if(SOCK_ERROR==(setsockopt(getSocket(), SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)))) {
		return setLastErrorCode(), ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	return ErrorType();
}

ErrorType BaseSocketInterface::getLastError() {
	if(getLastErrorCode()!=SNO_ERROR) {
		return ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	return ErrorType();
}

ErrorType BaseSocketInterface::setNagelOff() {
	int val = 1;
	int retVal = setsockopt(getSocket(), IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));
	if(SOCK_ERROR==retVal) {
		return ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	return ErrorType();
}


BaseSocketInterface::BaseSocketInterface(SOCKET s) 
: mSock(s), mIsBlocking(true), mLastError(SNO_ERROR) {
}

BaseSocketInterface::BaseSocketInterface(SOCKET s, bool bIsBlocking) 
	: mSock(s), mIsBlocking(bIsBlocking), mLastError(SNO_ERROR) {
}

void BaseSocketInterface::setLastErrorCode(ERROR_CODE e) {
	switch(e) {
	case SEWOULDBLOCK:
		if(isNonBlocking()) {
			mLastError = SNO_ERROR;
		} else {
			mLastError = e;
			closeSocket();
		}
		break;
	//case SETIMEDOUT:
	case SNO_ERROR:
        mLastError = e;
		break;
	default:
		mLastError = e;
		closeSocket();
		break;
	}
}

int BaseSocketInterface::getSendBufferSize() {
	int size = -1;
	socklen_t optLen = sizeof(size);
	int retVal = getsockopt(getSocket(),SOL_SOCKET,SO_SNDBUF,(char *)&size,&optLen);
	if(retVal==SOCK_ERROR) {
		setLastErrorCode();
	}
    return size;
}

int BaseSocketInterface::getReceiveBufferSize() {
	int size = -1;
	socklen_t optLen = sizeof(size);
	int retVal = getsockopt(getSocket(),SOL_SOCKET,SO_RCVBUF,(char *)&size,&optLen);
	if(retVal==SOCK_ERROR) {
		setLastErrorCode();
	}
    return size;
}

ErrorType BaseSocketInterface::bind() {
	ErrorType et = bind(InetAddressV4(INADDR_ANY),PortNum(0));
	return et;
}

ErrorType BaseSocketInterface::bind(const PortNum &Port) {
	InetAddressV4 addr(INADDR_ANY);
	ErrorType et = bind(addr,Port);
	return et;
}

ErrorType BaseSocketInterface::bind(const InetAddressV4 &addr) {
	ErrorType et = bind(addr,PortNum(0));
	return et;
}

ErrorType BaseSocketInterface::getLocalAddrAndPort(InetAddressV4 &addr, PortNum &port) {
	struct ::sockaddr_in sockaddr;
	socklen_t size = sizeof(::sockaddr_in);
	if(SOCK_ERROR!=::getsockname(getSocket(),(struct sockaddr *)&sockaddr,&size)) {
		addr = sockaddr.sin_addr;
		port = ntohs(sockaddr.sin_port);
		return setLastErrorCode(), ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	return ErrorType();
}

ErrorType BaseSocketInterface::bind(const InetAddressV4 &addr, const PortNum &port) {
	struct ::sockaddr_in sockaddr;
	socklen_t size = sizeof(::sockaddr_in);
	sockaddr.sin_addr = addr.getAddress();
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = port.getNetworkType();
	if(SOCK_ERROR==::bind(getSocket(),(struct sockaddr *) &sockaddr, size)) {
		return setLastErrorCode(), ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	return ErrorType();
}

////////////////////////////////////////////////////////////////////////////
//
///TCP
//
////////////////////////////////////////////////////////////////////////////
///////////////////
//NON Statics

TCPSocketInterface::TCPSocketInterface(SOCKET s) 
: BaseSocketInterface(s), mSockState(NO_CONNECTION), mRemoteAddr(), mRemotePort() {
}

TCPSocketInterface::TCPSocketInterface(SOCKET s, const InetAddressV4 &addr, 
									 const PortNum &port, bool bIsBlocking) 
	: BaseSocketInterface(s,bIsBlocking), mSockState(CONNECTED),
	mRemoteAddr(addr), mRemotePort(port) {

}

TCPSocketInterface::~TCPSocketInterface() {
	closeSocket();
}

void TCPSocketInterface::onCloseSocket() {
	setSocketState(FULL_CLOSED);
}

//The only states that get OR'd are READ_CLOSED and WRITE_CLOSED
void TCPSocketInterface::setSocketState(SOCKET_STATE r) {
	if((r&READ_CLOSED) || (r&WRITE_CLOSED)) {
		mSockState = SOCKET_STATE(getSocketState() | r);
	} else {
        mSockState=r;
	}
}

bool TCPSocketInterface::isInState(SOCKET_STATE s) const{
	return ((s&getSocketState())!=0);
}

int TCPSocketInterface::send(const std::basic_string<char> &s) {
	return send(s.c_str(),(uint32_t)s.length());
}

ErrorType TCPSocketInterface::connect(const InetAddressV4 &addr, short nPort) {
	struct timeval tv;  //will mean infinite wait
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	return connect(addr,nPort,&tv);
}

ErrorType TCPSocketInterface::connect(const InetAddressV4 &addr, short nPort, uint32_t waitTimeMS) {
	if(waitTimeMS==0) {
		return connect(addr,nPort,(timeval *)0);
	} else {
		struct timeval tv;
		tv.tv_sec = waitTimeMS/1000;
		tv.tv_usec = waitTimeMS%1000;
		return connect(addr,nPort,&tv);
	}
}

TCPSocketInterface* TCPSocketInterface::accept() {
	InetAddressV4 addr;
	PortNum	port;
	struct sockaddr_in peer;
	socklen_t i = sizeof(peer);
	SOCKET desc = (SOCKET)::accept(getSocket(),(struct sockaddr *) &peer,&i);
	if(desc==BAD_SOCKET) {
		setLastErrorCode();
		return 0;
	}
    addr = peer.sin_addr;
	port = ntohs(peer.sin_port);
	setLastErrorCode(SNO_ERROR);
	return (new TCPSocketInterface(desc, addr, port,IsBlocking()));
}

ErrorType TCPSocketInterface::getPeerAddress(InetAddressV4 &addr, PortNum &port) {
	if(mRemoteAddr.isValid()) {
		addr = mRemoteAddr;
		port = mRemotePort;
	} else {
		struct sockaddr_in sin;
		socklen_t sinSize = sizeof(sin);
		if(SOCK_ERROR!=getpeername(getSocket(),(sockaddr *)&sin,&sinSize)) {
			mRemoteAddr = addr = InetAddressV4(sin.sin_addr);
			mRemotePort = port = ntohs(sin.sin_port);
		} else {
			return setLastErrorCode(), ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
		}
	}
	return ErrorType();
}

ErrorType TCPSocketInterface::listen(int backlog) {
	if(SOCK_ERROR==::listen(getSocket(),backlog)) {
		return setLastErrorCode(), ErrorType(ErrorType::codeOSSpecific,getLastErrorCode());
	}
	setSocketState(LISTENING);
	return ErrorType();
}


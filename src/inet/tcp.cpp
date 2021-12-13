
#include "socket_typedefs.h"
#include "tcp.h"

using namespace wss;

//////////////////////////////////////////////
//	TCPClientSocket
ErrorType TCPClientSocket::connect(const InetAddressV4 &addr, short nPort, uint32 waitTimeMS) {
	return getImpl()->connect(addr,nPort,waitTimeMS);
}

ListenerSocket::~ListenerSocket() {

}

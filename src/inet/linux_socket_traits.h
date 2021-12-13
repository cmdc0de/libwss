
#ifndef WSS_LINUX_SOCKET_TRAITS_H_
#define WSS_LINUX_SOCKET_TRAITS_H_

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "inetaddress_v4.h"

namespace wss {
	/**
	*	@author Demetrius class
	*	@date 11-6-03
	*
	*	class wrapper for socket functions
	*	Names of the public variables and enum must be the same accross operating systems
	*/
	class LinuxSocketTraits
	{
	public:
		///Variable we set the FD to if we don't have a socket
		static const int BAD_SOCKET = -1;
		///the type we use for socket FD
		typedef int SOCKET;
		typedef ::socklen_t socklen_t;
		///Error returned from functions like send and recieve if an error occurred.
		static const int SOCK_ERROR = -1;
		///A mapping of the OS specific error an generic error name we can use cross platform
		enum ERROR_CODE {
			SNO_OS_SUPPORT	= -1,
			SNO_ERROR		= 0,
			SEWOULDBLOCK	= EWOULDBLOCK,
			SEINPROGRESS	= EINPROGRESS,
			SEALREADY		= EALREADY,
			SENOTSOCK       = ENOTSOCK ,
			SEDESTADDRREQ   = EDESTADDRREQ ,
			SEMSGSIZE       = EMSGSIZE,
			SEPROTOTYPE     = EPROTOTYPE,
			SENOPROTOOPT    = ENOPROTOOPT,
			SEPROTONOSUPPORT= EPROTONOSUPPORT,
			SESOCKTNOSUPPORT= ESOCKTNOSUPPORT,
			SEOPNOTSUPP     = EOPNOTSUPP,
			SEPFNOSUPPORT   = EPFNOSUPPORT,
			SEAFNOSUPPORT   = EAFNOSUPPORT,
			SEADDRINUSE     = EADDRINUSE,
			SEADDRNOTAVAIL  = EADDRNOTAVAIL,
			SENETDOWN       = ENETDOWN,
			SENETUNREACH    = ENETUNREACH,
			SENETRESET      = ENETRESET,
			SECONNABORTED   = ECONNABORTED,
			SECONNRESET     = ECONNRESET,
			SENOBUFS        = ENOBUFS,
			SEISCONN        = EISCONN,
			SENOTCONN       = ENOTCONN,
			SESHUTDOWN      = ESHUTDOWN,
			SETOOMANYREFS   = ETOOMANYREFS,
			SETIMEDOUT      = ETIMEDOUT,
			SECONNREFUSED   = ECONNREFUSED,
			SELOOP          = ELOOP,
			SENAMETOOLONG   = ENAMETOOLONG,
			SEHOSTDOWN      = EHOSTDOWN,
			SEHOSTUNREACH   = EHOSTUNREACH,
			SENOTEMPTY      = ENOTEMPTY,
			SEPROCLIM       = SNO_OS_SUPPORT,  //too many processes
			SEUSERS         = EUSERS,
			SEDQUOT         = EDQUOT,
			SESTALE         = ESTALE,
			SEREMOTE        = EREMOTE,
			SENOMEM			= ENOMEM,
			SEPIPE			= EPIPE,
			SEBADF			= EBADF,
			SEMFILE			= EMFILE,
			SEPERM			= EPERM,
			SEDEADLK		= EDEADLK,
			SEINTR			= EINTR,
			SEIO			= EIO,
			SEINVAL			= EINVAL
		};
	};
} 
#endif

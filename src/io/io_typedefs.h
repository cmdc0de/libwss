#ifndef WSS_IO_TYPEDEFS_H_
#define WSS_IO_TYPEDEFS_H_

#include "file.h"
#include "directory.h"


#ifdef WSS_WINDOWS
	#include "iotraits_win32.h"

	typedef wss::Win32IOTraits<TCHAR> TIOTRAITS;
	typedef wss::Win32IOTraits<wchar_t> WIOTRAITS;
	typedef wss::Win32IOTraits<char> NIOTRAITS;
#elif defined(WSS_LINUX)
	#include "iotraits_linux.h"
	#ifdef _MBCS
		typedef char TCHAR;
	#elif defined (_UNICODE)
		typedef wchar TCHAR;
	#else
		typedef char TCHAR;
	#endif
	typedef wss::LinuxIOTraits<TCHAR> TIOTRAITS;
	typedef wss::LinuxIOTraits<wchar_t> WIOTRAITS;
	typedef wss::LinuxIOTraits<char> NIOTRAITS;
#elif defined(WSS_MAC)
	#include "iotraits_linux.h"
	#ifdef _MBCS
		typedef char TCHAR;
	#elif defined (_UNICODE)
		typedef wchar TCHAR;
	#else
		typedef char TCHAR;
	#endif
	typedef wss::LinuxIOTraits<TCHAR> TIOTRAITS;
	typedef wss::LinuxIOTraits<wchar_t> WIOTRAITS;
	typedef wss::LinuxIOTraits<char> NIOTRAITS;
#endif


typedef wss::Directory<TIOTRAITS,TCHAR> TDIR;
typedef wss::File<TIOTRAITS,TCHAR> TFILE;
typedef wss::Directory<NIOTRAITS,char> NDIR;
typedef wss::File<NIOTRAITS,char> NFILE;
typedef wss::Directory<WIOTRAITS,wchar_t> WDIR;
typedef wss::File<WIOTRAITS,wchar_t> WFILE;


#endif

#ifdef WSS_WINDOWS

#include "iotraits_win32.h"
using namespace wss;

const TCHAR *Win32IOTraits<TCHAR>::PATH_SEP = (const TCHAR *)"\\";
const TCHAR *Win32IOTraits<TCHAR>::ALL_FILES_WILDCARD = (const TCHAR *)"*.*";
const TCHAR Win32IOTraits<TCHAR>::PATH_SEP_CHAR = (const TCHAR)'\\';

#endif

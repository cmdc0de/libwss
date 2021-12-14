#ifdef WSS_LINUX
#include "iotraits_linux.h"
using namespace wss;

TEMPLATE_SPECIALIZATION
const char *LinuxIOTraits<char>::PATH_SEP = (const char *)"/";

TEMPLATE_SPECIALIZATION
const char *LinuxIOTraits<char>::ALL_FILES_WILDCARD = (const char  *)"*";

TEMPLATE_SPECIALIZATION
const char LinuxIOTraits<char>::PATH_SEP_CHAR = (const char)'/';

TEMPLATE_SPECIALIZATION
const short *LinuxIOTraits<short>::PATH_SEP = (const short *)"/";
TEMPLATE_SPECIALIZATION
const short *LinuxIOTraits<short>::ALL_FILES_WILDCARD = (const short  *)"*";
TEMPLATE_SPECIALIZATION
const short LinuxIOTraits<short>::PATH_SEP_CHAR = (const short)'/';
#endif

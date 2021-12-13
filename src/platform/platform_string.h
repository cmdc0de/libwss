#ifndef PLATFORM_STRING_H
#define PLATFORM_STRING_H

#include <cstdarg>

namespace wss { namespace platform {
	int32 vsnprintf(char * dest,int32 dest_string_size,const char * format,va_list args);
	int32 vsnprintf(wchar_t * dest,int32 dest_string_size,const wchar_t * format,va_list args);

	int32 snprintf(char *dest, int32 dest_string_size, const char * format, ...);
	int32 snprintf(wchar_t *dest, int32 dest_string_size, const wchar_t * format, ...);
}
}

#endif //PLATFORM_STRING_H
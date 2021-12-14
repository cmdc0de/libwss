#ifndef PLATFORM_STRING_H
#define PLATFORM_STRING_H

#include <cstdarg>

namespace wss { namespace platform {
	int vsnprintf(char * dest,int dest_string_size,const char * format,va_list args);
	int vsnprintf(wchar_t * dest,int dest_string_size,const wchar_t * format,va_list args);

	int snprintf(char *dest, int dest_string_size, const char * format, ...);
	int snprintf(wchar_t *dest, int dest_string_size, const wchar_t * format, ...);
}
}

#endif //PLATFORM_STRING_H

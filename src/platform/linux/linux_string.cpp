#include "../platform_string.h"

#include <wchar.h>
#include <cstdarg>

namespace wss { namespace platform 
{
	int32 vsnprintf(char * dest,int dest_string_size,const char * format,va_list args) {
		return ::vsnprintf(dest, dest_string_size, format, args);
	}

	int32 vsnprintf(wchar_t * dest,int dest_string_size,const wchar_t * format,va_list args) {
		return ::vswprintf(dest, dest_string_size, format, args);
	}

	int32 snprintf(char *dest, int32 dest_string_size, const char * format, ...) {
		va_list va;
		va_start(va, format);
		int32 ret = vsnprintf(dest, dest_string_size, format, va);
		va_end(va);
		return ret;
	}
	int32 snprintf(wchar_t *dest, int32 dest_string_size, const wchar_t * format, ...) {
		va_list va;
		va_start(va, format);
		int32 ret = vsnprintf(dest, dest_string_size, format, va);
		va_end(va);
		return ret;
	}
}
}

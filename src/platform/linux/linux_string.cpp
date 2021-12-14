#include "../platform_string.h"

#include <wchar.h>
#include <cstdarg>

using namespace wss::platform;

int vsnprintf(char * dest,int dest_string_size,const char * format,va_list args) {
	return ::vsnprintf(dest, dest_string_size, format, args);
}

int vsnprintf(wchar_t * dest,int dest_string_size,const wchar_t * format,va_list args) {
	return ::vswprintf(dest, dest_string_size, format, args);
}

int snprintf(char *dest, int dest_string_size, const char * format, ...) {
		va_list va;
		va_start(va, format);
		int ret = wss::platform::vsnprintf(dest, dest_string_size, format, va);
		va_end(va);
		return ret;
}

int snprintf(wchar_t *dest, int dest_string_size, const wchar_t * format, ...) {
		va_list va;
		va_start(va, format);
		int ret = wss::platform::vsnprintf(dest, dest_string_size, format, va);
		va_end(va);
		return ret;
}

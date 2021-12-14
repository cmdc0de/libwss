#ifndef PLATFORM_STRING_H
#define PLATFORM_STRING_H

#include <cstdio>
#include <errno.h>


namespace wss { namespace platform {
	typedef void* LIBHANDLE;
	error_t fopen(FILE** f, const char * file_name, const char * mode);
	int fscanf(FILE *f, const char * fmt, ...);
	LIBHANDLE LoadDynamicLibrary(const char *file);
	bool FreeDynamicLibraryhandle(LIBHANDLE handle);
	void *getProcAddress(LIBHANDLE handle, const char *funName);
}
}

#endif //PLATFORM_STRING_H

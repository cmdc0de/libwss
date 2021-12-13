#include "../platform_file.h"
#include <dlfcn.h>

namespace wss { namespace platform {
	errno_t fopen(FILE** f, const char * file_name, const char * mode) {
		*f = ::fopen(file_name, mode);
		return *f != 0;
	}
	LIBHANDLE LoadDynamicLibrary(const char *file) {
		return dlopen(file, RTLD_NOW);
	}
	void *getProcAddress(LIBHANDLE handle, const char *funName) {
		return dlsym(handle,funName);
	}
	bool FreeDynamicLibraryhandle(LIBHANDLE handle) {
		return dlclose(handle)==0;
	}
}
}

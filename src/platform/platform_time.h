#ifndef PLATFORM_TIME_H
#define PLATFORM_TIME_H

#include <errno.h>
#include <ctime>

namespace wss { namespace platform {
	error_t gmtime(tm* _tm, const time_t* time);
	error_t localtime(tm* _tm, const time_t* time);
}
}

#endif //PLATFORM_TIME_H

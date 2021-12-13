#ifndef PLATFORM_TIME_H
#define PLATFORM_TIME_H

#include <errno.h>
#include <ctime>

namespace wss { namespace platform {
	errno_t gmtime(tm* _tm, const time_t* time);
	errno_t localtime(tm* _tm, const time_t* time);
}
}

#endif //PLATFORM_TIME_H
#include "../platform_time.h"

namespace wss { namespace platform {
	errno_t gmtime(tm* _tm, const time_t* time) {
		gmtime_r(time, _tm);
		return 0;
	}

	errno_t localtime(tm* _tm, const time_t* time) {
		return ::localtime_r(time, _tm) == 0;
	}
}
}

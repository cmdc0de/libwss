#include "../platform_time.h"

using namespace wss::platform;

	error_t gmtime(tm* _tm, const time_t* time) {
		gmtime_r(time, _tm);
		return 0;
	}

	error_t localtime(tm* _tm, const time_t* time) {
		return ::localtime_r(time, _tm) == 0;
	}

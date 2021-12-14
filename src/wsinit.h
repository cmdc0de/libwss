#ifndef WSS_WS_INIT_H
#define WSS_WS_INIT_H

#include <spdlog/spdlog.h>
#include "error_type.h"

namespace wss {

class WSInit {
public:
	static ErrorType init(std::shared_ptr<spdlog::logger> &logger);
	static WSInit &get();
public:
	std::shared_ptr<spdlog::logger> &getLogger();
private:
	WSInit(std::shared_ptr<spdlog::logger> &l);
private:
	std::shared_ptr<spdlog::logger> MyLog;
private:
	static WSInit *MySelf;
};

#define GET_LOGGER WSInit::get().getLogger()

}

#endif


#include "wsinit.h"
using namespace wss;

WSInit *WSInit::MySelf = nullptr;

static ErrorType WSInit::init(std::shared_ptr<spdlog::logger> &logger) {
	if(MySelf) {
		delete MySelf;
	} 
	MySelf = new WSInit(logger);
	return ErrorType;
}

static WSInit &WSInit::get() {
	return *MySelf;
}

std::shared_ptr<spdlog::logger> &WSInit::getLogger() {
	return MyLog;
}

WSInit::WSInit(std::shared_ptr<spdlog::logger> &l) : MyLog(l) {

}


#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "IODeviceManager"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../common/easylogging/easylogging++.h"
#include "IODeviceManager.hpp"
#include "../../common/json/json.hpp"

using json = nlohmann::json;

IODeviceManager::IODeviceManager(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

IODeviceManager::~IODeviceManager()
{
}

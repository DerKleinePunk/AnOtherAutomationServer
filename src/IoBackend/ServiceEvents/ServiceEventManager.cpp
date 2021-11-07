#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "ServiceEventManager"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "ServiceEventManager.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"

ServiceEventManager::ServiceEventManager(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

ServiceEventManager::~ServiceEventManager()
{
}

bool ServiceEventManager::Init()
{
    return false;
}
    
void ServiceEventManager::Deinit()
{

}

void ServiceEventManager::FireNewEvent(const std::string& name, const std::string& parameter)
{

}
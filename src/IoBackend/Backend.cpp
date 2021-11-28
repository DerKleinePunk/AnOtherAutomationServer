#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "Backend"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "Backend.hpp"
#include "../common/easylogging/easylogging++.h"

void Backend::EventCallback(const std::string& name, const std::string& parameter)
{
    if(name == "MqttValue") {
        _webServer->SendWebSocketBroadcast("MqttValue" + parameter);
    }
}

Backend::Backend(WebServer* webServer, ServiceEventManager* serviceEventManager)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _webServer = webServer;
    _serviceEventManager = serviceEventManager;
}

Backend::~Backend()
{
}

void Backend::Init()
{
    auto callback = std::bind(&Backend::EventCallback, this, std::placeholders::_1, std::placeholders::_2);
    _serviceEventManager->RegisterMe(std::string(""), callback);
};
#pragma once

#include "WebServer/WebServer.hpp"
#include "ServiceEvents/ServiceEventManager.hpp"

class Backend
{
private:
    WebServer* _webServer;
    ServiceEventManager* _serviceEventManager;

    void EventCallback(const std::string& name, const std::string& parameter);
    
public:
    Backend(WebServer* webServer, ServiceEventManager* serviceEventManager);
    ~Backend();

    void Init();
};



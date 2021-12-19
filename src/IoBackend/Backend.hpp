#pragma once

#include "WebServer/WebServer.hpp"
#include "ServiceEvents/ServiceEventManager.hpp"
#include "Config.hpp"
#include "PythonRunner/PythonRunner.hpp"

class Backend
{
private:
    WebServer* _webServer;
    ServiceEventManager* _serviceEventManager;
    const Config* _config;
    PythonRunner* _runner;

    void EventCallback(const std::string& name, const std::string& parameter);
    void Run(const std::string& name, const std::map<std::string, std::string>& parameters);

public:
    Backend(WebServer* webServer, ServiceEventManager* serviceEventManager, const Config* config, PythonRunner* runner);
    ~Backend();

    void Init();
};



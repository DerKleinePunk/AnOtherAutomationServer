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

    for(auto entry : _config->GetEventRoot()) {
        if(entry.Name == name) {
            Run(entry.Function, entry.Parameters);
        }
    }
}

void Backend::Run(const std::string& name, const std::map<std::string, std::string>& parameters)
{
    LOG(DEBUG) << "Try to run " << name << " with ";// << parameters;
    if(name == "CallPython") {
        std::string script;
        std::string function;
        
        auto entry = parameters.find("Script");
        if( entry != parameters.end()) {
            script = entry->second;
        }

        entry = parameters.find("Function");
        if( entry != parameters.end()) {
            function = entry->second;
        }

        if(script.size() > 0 && function.size() > 0 ) {
            _runner->RunScript(script, function, "Hello");
        } else {
            LOG(ERROR) << "missing Parameter to Run Function CallPython";
        }
        
    }
}

Backend::Backend(WebServer* webServer, ServiceEventManager* serviceEventManager, const Config* config, PythonRunner* runner)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _webServer = webServer;
    _serviceEventManager = serviceEventManager;
    _config = config;
    _runner = runner;
}

Backend::~Backend()
{
}

void Backend::Init()
{
    auto callback = std::bind(&Backend::EventCallback, this, std::placeholders::_1, std::placeholders::_2);
    _serviceEventManager->RegisterMe(std::string(""), callback);
};
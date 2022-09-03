#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "Backend"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "Backend.hpp"

#include <map>

#include "../common/easylogging/easylogging++.h"
#include "../common/json/json.hpp"

using json = nlohmann::json;

void Backend::EventCallback(const SystemEvent event, const std::string& parameter)
{
    if(event == SystemEvent::MqttValue) {
        auto jsonText = json::parse(parameter);
        jsonText["Event"] = "MqttValue";
        _webServer->SendWebSocketBroadcast(jsonText.dump());
    } else if(event == SystemEvent::ValueChanged) {
        auto jsonText = json::parse(parameter);
        jsonText["Event"] = "ValueChanged";
        _webServer->SendWebSocketBroadcast(jsonText.dump());
    }

    for(auto entry : _config->GetEventRoot()) {
        if(entry.Event == event) {
            auto jsonText = json::parse(parameter);
            auto currentParameter = jsonText.get<std::map<std::string, std::string>>();
            entry.Parameters.insert(currentParameter.begin(), currentParameter.end());

            Run(entry.Function, entry.Parameters);
        }
    }
}

void Backend::Run(const std::string& function, const std::map<std::string, std::string>& parameters)
{
    LOG(DEBUG) << "Try to run " << function << " with "; // << parameters;
    if(function == "CallPython") {
        std::string script;
        std::string function;

        auto entry = parameters.find("Script");
        if(entry != parameters.end()) {
            script = entry->second;
        }

        entry = parameters.find("Function");
        if(entry != parameters.end()) {
            function = entry->second;
        }

        json parameterJson = parameters;

        if(script.size() > 0 && function.size() > 0) {
            _runner->RunScript(script, function, parameterJson.dump());
        } else {
            LOG(ERROR) << "missing Parameter to Run Function CallPython";
        }

    } else if(function == "SetVar") {
        // Todo ?!?
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
    std::vector<SystemEvent> eventList;
    _serviceEventManager->RegisterMe(eventList, callback, "Backend");
};
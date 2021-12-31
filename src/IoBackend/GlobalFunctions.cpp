#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "GlobalFunctions"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../common/easylogging/easylogging++.h"
#include "../common/utils/commonutils.h"
#include "GlobalFunctions.hpp"

GlobalFunctions::GlobalFunctions(Config* config, ServiceEventManager* serviceEventManager, NetworkManager* networkManager)
{
    _config = config;
    _serviceEventManager = serviceEventManager;
    _networkManager = networkManager;
}

GlobalFunctions::~GlobalFunctions()
{
}

bool GlobalFunctions::IsApiKeyOk(const std::string& value)
{
    return value == _config->GetApiKey();
}

const std::string GlobalFunctions::GetContentTypeFromFileName(const std::string& fileName) const
{
    if(utils::hasEnding(fileName, ".html") || utils::hasEnding(fileName, ".htm"))
    {
        return "text/html";
    }

    if(utils::hasEnding(fileName, ".css"))
    {
        return "text/css";
    }

    return "text/plain";
}

uint16_t GlobalFunctions::GetServerPort() const
{
    return _config->GetServerPort();
}

void GlobalFunctions::FireNewEvent(const std::string& name, const std::string& parameter)
{
    _serviceEventManager->FireNewEvent(name, parameter); 
}

void GlobalFunctions::RegisterForEvent(const std::string& eventFilter, EventDelegate function) {
    _serviceEventManager->RegisterMe(eventFilter, function);
}

std::string GlobalFunctions::ScanAccessPoints(const std::string& interfaceName)
{
    const json result = _networkManager->ScanAccessPoints(interfaceName);
    return result.dump();
}

void GlobalFunctions::SetInternalVariable(const std::string& name, const std::string& value)
{
    auto changed = false;
    if(_internalVariables.find(name) != _internalVariables.end()) {
        if(_internalVariables[name] != value) {
            _internalVariables[name] = value;
            changed = true;
        }
    } else {
        _internalVariables.insert(std::pair<std::string, std::string>(name, value));
        changed = true;
    }

    if(changed) {
        json j;
        j["name"] = name;
        j["value"] = value;
        FireNewEvent("ValueChanged", j.dump());
    }
}

std::string GlobalFunctions::GetInternalVariable(const std::string& name) const
{
    std::string result("");

    const auto findIt = _internalVariables.find(name);
    if( findIt != _internalVariables.end()) {
        result = findIt->second; 
    }

    return result;
}

void GlobalFunctions::ShowInternalVars()
{
    for(const auto entry : _internalVariables) {
        std::cout << entry.first << " => " << entry.second << std::endl;
    }
}
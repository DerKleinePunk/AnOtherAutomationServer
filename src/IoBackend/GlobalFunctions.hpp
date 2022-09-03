#pragma once

#include "Config.hpp"
#include "ServiceEvents/ServiceEventManager.hpp"
#include "SystemFunktions/NetworkManager.hpp"

class GlobalFunctions
{
private:
    Config* _config;
    ServiceEventManager* _serviceEventManager;
    NetworkManager* _networkManager;
    std::map<std::string,std::string> _internalVariables;

public:
    GlobalFunctions(Config* config, ServiceEventManager* serviceEventManager, NetworkManager* networkManager);
    ~GlobalFunctions();

    bool IsApiKeyOk(const std::string& value);
    std::string GetApiKey() const;

    const std::string GetContentTypeFromFileName(const std::string& fileName) const;
    uint16_t GetServerPort() const;

    void FireNewEvent(const SystemEvent event, const std::string& parameter);
    void RegisterForEvent(const std::vector<SystemEvent>& eventFilter, EventDelegate function, const std::string name);
    void RegisterForEvent(const SystemEvent event, EventDelegate function, const std::string name);


    std::string ScanAccessPoints(const std::string& interfaceName = "");

    void SetInternalVariable(const std::string& name, const std::string& value);
    std::string GetInternalVariable(const std::string& name) const;
    std::string GetInternalVariable(const std::string& name, const std::string& defaultValue);
    void ShowInternalVars();

};


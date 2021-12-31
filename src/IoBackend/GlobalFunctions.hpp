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
    const std::string GetContentTypeFromFileName(const std::string& fileName) const;
    uint16_t GetServerPort() const;

    void FireNewEvent(const std::string& name, const std::string& parameter);
    void RegisterForEvent(const std::string& eventFilter, EventDelegate function);

    std::string ScanAccessPoints(const std::string& interfaceName = "");

    void SetInternalVariable(const std::string& name, const std::string& value);
    std::string GetInternalVariable(const std::string& name) const;
    void ShowInternalVars();

};


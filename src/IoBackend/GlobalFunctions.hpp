#pragma once

#include "Config.hpp"
#include "ServiceEvents/ServiceEventManager.hpp"
#include "SystemFunktions/NetworkManager.hpp"
#include "../common/database/DatabaseManager.h"
#include "Dtos/AutomationPage.hpp"

class GlobalFunctions
{
private:
    Config* _config;
    ServiceEventManager* _serviceEventManager;
    NetworkManager* _networkManager;
    DatabaseManager* _databaseManager;
    std::map<std::string,std::string> _internalVariables;

    void OnDatabaseStartup(DatabaseState state) const;

public:
    GlobalFunctions(Config* config, ServiceEventManager* serviceEventManager, NetworkManager* networkManager, DatabaseManager* databaseManager);
    ~GlobalFunctions();

    bool IsApiKeyOk(const std::string& value);
    std::string GetApiKey() const;

    const std::string GetContentTypeFromFileName(const std::string& fileName) const;
    uint16_t GetServerPort() const;
    std::map<const std::string,std::string> GetGobalHttpHeaders();

    void FireNewEvent(const SystemEvent event, const std::string& parameter);
    void RegisterForEvent(const std::vector<SystemEvent>& eventFilter, EventDelegate function, const std::string name);
    void RegisterForEvent(const SystemEvent event, EventDelegate function, const std::string name);

    std::string ScanAccessPoints(const std::string& interfaceName = "");

    void SetInternalVariable(const std::string& name, const std::string& value);
    std::string GetInternalVariable(const std::string& name) const;
    std::string GetInternalVariable(const std::string& name, const std::string& defaultValue);
    void ShowInternalVars();

    std::vector<AutomationPage> GetAutomationPages();
};


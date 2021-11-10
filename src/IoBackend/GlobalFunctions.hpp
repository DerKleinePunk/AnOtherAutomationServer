#pragma once

#include "Config.hpp"
#include "ServiceEvents/ServiceEventManager.hpp"

class GlobalFunctions
{
private:
    Config* _config;
    ServiceEventManager* _serviceEventManager;
public:
    GlobalFunctions(Config* config, ServiceEventManager* serviceEventManager);
    ~GlobalFunctions();

    bool IsApiKeyOk(const std::string& value);
    const std::string GetContentTypeFromFileName(const std::string& fileName) const;
    uint16_t GetServerPort() const;

    void FireNewEvent(const std::string& name, const std::string& parameter);
};


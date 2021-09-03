#pragma once

#include <string>

struct ConfigFile
{
    std::string MqttServer;
    uint16_t ServerPort;
    std::string ApiKey;
};

class Config
{
private:
    ConfigFile _configFile;
    std::string _filenameConfig;
public:
    Config(const std::string& filename = "");
    ~Config();

    void Load ();
    void Save ();

    std::string GetMqttServer() const;
    uint16_t GetServerPort() const;
    std::string GetApiKey() const;
};

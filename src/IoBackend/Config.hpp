#pragma once

#include <string>
#include <vector>
#include <map>

struct EventNode
{
    std::string Name;
    std::string Function;
    std::map<std::string, std::string> Parameters;
};

struct ConfigFile
{
    std::string MqttServer;
    uint16_t ServerPort;
    std::string ApiKey;
    std::vector<std::string> WatchTopics;
    std::vector<EventNode> EventRoot;
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
    std::vector<std::string> GetWatchTopics() const;
    std::vector<EventNode> GetEventRoot() const;
};

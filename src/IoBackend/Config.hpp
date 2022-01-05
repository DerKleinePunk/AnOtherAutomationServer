#pragma once

#include <string>
#include <vector>
#include <map>
#include "GlobalTypes.hpp"

struct EventNode
{
    SystemEvent Event;
    std::string Function;
    std::map<std::string, std::string> Parameters;
};

enum class ResourceType {
    None,
    GPIOPin,
    MCP23017,
    GPSMouse
};

std::ostream& operator<<(std::ostream& os, const ResourceType type);

struct Resource
{
    ResourceType Type;
};

struct MCP23017Resource : public Resource
{
    std::uint8_t Address;
    std::uint8_t EnablePin;
    bool UseEnable;
    std::string OutputMap;
    std::string VarName[16];
};  

struct GPSMouseResource : public Resource
{
    std::string ComPort;
};

struct GPIOPinResource : public Resource
{
    std::uint8_t Address;
    bool Output;
    std::string VarName;
};

struct ConfigFile
{
    std::string MqttServer;
    std::uint16_t ServerPort;
    std::string ApiKey;
    std::string UiWebSitePath;
    std::vector<std::string> WatchTopics;
    std::vector<EventNode> EventRoot;
    std::vector<Resource*> Resources;
};

class Config
{
private:
    ConfigFile _configFile;
    std::string _filenameConfig;
    bool _loadFailed;
public:
    Config(const std::string& filename = "");
    Config(const Config& orig) = delete;
	Config(Config&& other) = delete;
	Config& operator=(const Config& other) = delete;
	Config& operator=(Config&& other) = delete;
    ~Config();

    void Load ();
    void Save ();

    std::string GetMqttServer() const;
    uint16_t GetServerPort() const;
    std::string GetApiKey() const;
    std::vector<std::string> GetWatchTopics() const;
    std::vector<EventNode> GetEventRoot() const;
    std::vector<Resource*> GetResources() const;
};

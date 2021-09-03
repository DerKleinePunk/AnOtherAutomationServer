#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "Config"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include <fstream>
#include <iomanip>

#include "../common/easylogging/easylogging++.h"
#include "../common/json/json.hpp"
#include "../common/utils/commonutils.h"
#include "Config.hpp"

using json = nlohmann::json;

void to_json(json& j, const ConfigFile& p)
{
    j = json{ 
        { "MqttServer", p.MqttServer }, 
        { "ServerPort", p.ServerPort }, 
        { "ApiKey", p.ApiKey }, 
        { "WatchTopics", p.WatchTopics } 
    };
}

void from_json(const json& j, ConfigFile& p)
{
    auto it_value = j.find("MqttServer");
    if(it_value != j.end()) {
        p.MqttServer = j.at("MqttServer").get<std::string>();
    } else {
        p.MqttServer = "192.168.2.99";
    }

    it_value = j.find("ServerPort");
    if(it_value != j.end()) {
        p.ServerPort = j.at("ServerPort").get<uint16_t>();
    } else {
        p.ServerPort = 8080;
    }

    it_value = j.find("ApiKey");
    if(it_value != j.end()) {
        p.ApiKey = j.at("ApiKey").get<std::string>();
    } else {
        p.ApiKey = "12345678";
    }

    it_value = j.find("WatchTopics");
    if(it_value != j.end()) {
        p.WatchTopics = j.at("WatchTopics").get<std::vector<std::string>>();
    } else {
        p.WatchTopics.push_back("devices/#");
    }
}

Config::Config(const std::string& filename)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    if(filename.size() == 0) {
        _filenameConfig = "IoBackendConfig.json";
    } else {
        _filenameConfig = filename;
    }

    LOG(DEBUG) << "ConfigFile FileName is " << _filenameConfig;
}

Config::~Config()
{
}

void Config::Load()
{
    LOG(DEBUG) << "Try Loading Config from " << _filenameConfig;

    if(utils::FileExists(_filenameConfig)) {
        try {
            std::ifstream ifs(_filenameConfig);
            const auto jConfig = json::parse(ifs);
            _configFile = jConfig;
            ifs.close();
        } catch(std::domain_error& exp) {
            LOG(ERROR) << "Loadconfig  " << exp.what();
        } catch(std::exception& exp) {
            LOG(ERROR) << "Loadconfig  " << exp.what();
        }
    } else {
        LOG(DEBUG) << "Create new config file";
        _configFile.MqttServer = "192.168.2.99";
        _configFile.ServerPort = 8080;
        _configFile.ApiKey = "12345678";
        _configFile.WatchTopics.push_back("devices/#");
        std::ofstream o(_filenameConfig);
        const json jConfig = _configFile;
        o << std::setw(4) << jConfig << std::endl;
        o.close();
    }
}

void Config::Save()
{
    std::ofstream o(_filenameConfig);
    const json jConfig = _configFile;
    o << std::setw(4) << jConfig << std::endl;
    o.close();
}

uint16_t Config::GetServerPort() const
{
    return _configFile.ServerPort;
}

std::string Config::GetMqttServer() const
{
    return _configFile.MqttServer;
}

std::string Config::GetApiKey() const
{
    return _configFile.ApiKey;
}

std::vector<std::string> Config::GetWatchTopics() const
{
    return _configFile.WatchTopics;
}
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

NLOHMANN_JSON_SERIALIZE_ENUM( ResourceType, {
    {ResourceType::None, nullptr},
    {ResourceType::GPIOPin, "GPIOPin"},
    {ResourceType::GPSMouse, "GPSMouse"},
    {ResourceType::MCP23017, "MCP23017"},
})

NLOHMANN_JSON_SERIALIZE_ENUM( SystemEvent, {
    {SystemEvent::ChangeValue, "ChangeValue"},
    {SystemEvent::Idle, "Idle"},
    {SystemEvent::MqttValue, "MqttValue"},
    {SystemEvent::PublishMqtt, "PublishMqtt"},
    {SystemEvent::ValueChanged, "ValueChanged"},
})

std::ostream& operator<<(std::ostream& os, const ResourceType type)
{
    switch(type) {
        case ResourceType::None:
            os << "None";
            break;
        case ResourceType::GPIOPin:
            os << "GPIOPin";
            break;
        case ResourceType::GPSMouse:
            os << "GPSMouse";
            break;
         case ResourceType::MCP23017:
            os << "MCP23017";
            break;
        default:
            os.setstate(std::ios_base::failbit);
    }
    return os;
}

void to_json(json& j, const EventNode& p)
{
    j = json{ 
        { "Event", p.Event }, 
        { "Function", p.Function },
        { "Parameters", p.Parameters }  
    };
}

void to_json(json& j, const MCP23017Resource* p)
{
    j = json{ 
        { "Type", p->Type },
        { "Address" , p->Address},
        { "EnablePin" , p->EnablePin},
        { "UseEnable" , p->UseEnable},
        { "OutputMap" , p->OutputMap},
        { "VarName" , p->VarName},
    };
}

void to_json(json& j, const GPSMouseResource* p)
{
    j = json{ 
        { "Type", p->Type },
        { "ComPort" , p->ComPort}
    };
}

void to_json(json& j, Resource* p)
{
    if(p->Type == ResourceType::MCP23017) {
        to_json(j, reinterpret_cast<MCP23017Resource*>(p));
    } else if(p->Type == ResourceType::GPSMouse) {
        to_json(j, reinterpret_cast<GPSMouseResource*>(p));
    }
}

void to_json(json& j, const ConfigFile& p)
{
    j = json { 
        { "MqttServer", p.MqttServer }, 
        { "ServerPort", p.ServerPort }, 
        { "ApiKey", p.ApiKey },
        { "UiWebSitePath", p.UiWebSitePath}, 
        { "WatchTopics", p.WatchTopics },
        { "EventRoot", p.EventRoot },
        { "Resources", p.Resources }
    };
}

void from_json(const json& j, EventNode& p)
{
    auto it_value = j.find("Event");
    if(it_value != j.end()) {
        p.Event = j.at("Event").get<SystemEvent>();
    } else {
        p.Event = SystemEvent::Idle;
    }

    it_value = j.find("Function");
    if(it_value != j.end()) {
        p.Function = j.at("Function").get<std::string>();
    } else {
        p.Function = "EmptyFunctionName";
    }

    it_value = j.find("Parameters");
    if(it_value != j.end()) {
        p.Parameters = j.at("Parameters").get<std::map<std::string, std::string>>();
    } else {
        //p.Parameters = "Parameters";
    }
}

void from_json(const json& j, Resource& p)
{
    auto it_value = j.find("Type");
    if(it_value != j.end()) {
        p.Type = j.at("Type").get<ResourceType>();
    } else {
        LOG(WARNING) << "Type Not found in Resource Entry";
    }
}

void from_json(const json& j, MCP23017Resource* p)
{
    auto it_value = j.find("Type");
    if(it_value != j.end()) {
        p->Type = ResourceType::MCP23017;
        
    } else {
        LOG(WARNING) << "Type Not found in Resource Entry";
    }

    it_value = j.find("Address");
    if(it_value != j.end()) {
        p->Address = j.at("Address").get<uint8_t>();
    } else {
        p->Address = 0x20;
    }

    it_value = j.find("EnablePin");
    if(it_value != j.end()) {
        p->EnablePin = j.at("EnablePin").get<uint8_t>();
    } else {
        p->EnablePin = 0x06;
    }

    it_value = j.find("UseEnable");
    if(it_value != j.end()) {
        p->UseEnable = j.at("UseEnable").get<bool>();
    } else {
        p->UseEnable = false;
    }

    it_value = j.find("OutputMap");
    if(it_value != j.end()) {
        p->OutputMap = j.at("OutputMap").get<std::string>();
    } else {
        p->OutputMap = "1111111100000000";
    }

    it_value = j.find("VarName");
    if(it_value != j.end()) {
        std::size_t i = 0;
        for(;i<j.at("VarName").size();i++) {
            p->VarName[i] = j.at("VarName")[i];
        }
    } else {
        p->VarName[0] = "RelaisBoard0";
        p->VarName[1] = "Automatik";
        for(std::size_t i = 2;i<16;i++) {
            p->VarName[i] = "Empty" + std::to_string(i);
        }
    }
       
}

void from_json(const json& j, GPSMouseResource* p)
{
    auto it_value = j.find("Type");
    if(it_value != j.end()) {
        p->Type = ResourceType::GPSMouse;
        
    } else {
        LOG(WARNING) << "Type Not found in Resource Entry";
    }

    it_value = j.find("Address");
    if(it_value != j.end()) {
        p->ComPort = j.at("Address").get<std::string>();
    } else {
        p->ComPort = "None";
    }
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
        p.ApiKey = "123456789";
    }

    it_value = j.find("WatchTopics");
    if(it_value != j.end()) {
        p.WatchTopics = j.at("WatchTopics").get<std::vector<std::string>>();
    } else {
        p.WatchTopics.push_back("devices/#");
    }

    it_value = j.find("EventRoot");
    if(it_value != j.end()) {
        p.EventRoot = j.at("EventRoot").get<std::vector<EventNode>>();
    } else {
        EventNode node;
        node.Event = SystemEvent::MqttValue;
        node.Function = "CallPython";
        node.Parameters.insert(std::pair<std::string, std::string>("Script", "sample"));
        node.Parameters.insert(std::pair<std::string, std::string>("Function", "callbackMqtt"));
        p.EventRoot.push_back(node);
    }

    it_value = j.find("Resources");
    if(it_value != j.end()) {
        auto resources = j.at("Resources");
        for (json::iterator it = resources.begin(); it != resources.end(); ++it)
        {
            Resource valueType;
            from_json(it.value(), valueType);
            if(valueType.Type == ResourceType::MCP23017) {
                MCP23017Resource* mcpType = new MCP23017Resource();
                from_json(it.value(), mcpType);
                p.Resources.push_back(mcpType);
            } else if(valueType.Type == ResourceType::GPSMouse){
                GPSMouseResource* mcpType = new GPSMouseResource();
                from_json(it.value(), mcpType);
                p.Resources.push_back(mcpType);
            } else {
                LOG(WARNING) << valueType.Type << " Unkownen Type";
            }
        }
    } else {
        MCP23017Resource* entry = new MCP23017Resource();
        entry->Type = ResourceType::MCP23017;
        entry->Address = 0x20;
        p.Resources.push_back(entry);

        GPSMouseResource* entry2 = new GPSMouseResource();
        entry2->Type = ResourceType::GPSMouse;
        entry2->ComPort = "none";
        p.Resources.push_back(entry2);
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
    _loadFailed = false;
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
            _loadFailed = true;
        } catch(std::exception& exp) {
            LOG(ERROR) << "Loadconfig  " << exp.what();
            _loadFailed = true;
        }
    } else {
        LOG(DEBUG) << "Create new config file";
        _configFile.MqttServer = "192.168.2.99";
        _configFile.ServerPort = 8080;
        _configFile.ApiKey = "12345678";
        _configFile.WatchTopics.push_back("devices/#");
        _configFile.WatchTopics.push_back("SimpleIo/Variable/#");
        
        EventNode node;
        node.Event = SystemEvent::MqttValue;
        node.Function = "CallPython";
        node.Parameters.insert(std::pair<std::string, std::string>("Script", "sample"));
        node.Parameters.insert(std::pair<std::string, std::string>("Function", "callbackMqtt"));
        _configFile.EventRoot.push_back(node);

        MCP23017Resource* entry = new MCP23017Resource();
        entry->Type = ResourceType::MCP23017;
        entry->Address = 0x20;
        entry->VarName[0] = "RelaisBoard0";
        entry->VarName[1] = "Automatik";
        _configFile.Resources.push_back(entry);

        GPSMouseResource* entry2 = new GPSMouseResource();
        entry2->Type = ResourceType::GPSMouse;
        entry2->ComPort = "none";
        _configFile.Resources.push_back(entry2);

        std::ofstream o(_filenameConfig);
        const json jConfig = _configFile;
        o << std::setw(4) << jConfig << std::endl;
        o.close();
    }
}

void Config::Save()
{
    if(_loadFailed) return;

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

std::vector<EventNode> Config::GetEventRoot() const
{
    return _configFile.EventRoot;
}

std::vector<Resource*> Config::GetResources() const
{
    return _configFile.Resources;
}
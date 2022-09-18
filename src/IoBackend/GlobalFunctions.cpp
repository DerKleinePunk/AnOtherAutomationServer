#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "GlobalFunctions"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../common/easylogging/easylogging++.h"
#include "../common/utils/commonutils.h"
#include "GlobalFunctions.hpp"

void GlobalFunctions::OnDatabaseStartup(DatabaseState state) const {
	if(state == DatabaseState::StartupNew) {
		//Todo CreateTablesMusikInfo();
	}
}

GlobalFunctions::GlobalFunctions(Config* config, ServiceEventManager* serviceEventManager, NetworkManager* networkManager, DatabaseManager* databaseManager)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _config = config;
    _serviceEventManager = serviceEventManager;
    _networkManager = networkManager;
    _databaseManager = databaseManager;
    auto CDatabaseStartupdelegate = std::bind(&GlobalFunctions::OnDatabaseStartup, this, std::placeholders::_1);
    _databaseManager->AddStateCallBack(CDatabaseStartupdelegate);
}

GlobalFunctions::~GlobalFunctions()
{
}

bool GlobalFunctions::IsApiKeyOk(const std::string& value)
{
    return value == _config->GetApiKey();
}

std::string GlobalFunctions::GetApiKey() const
{
    return _config->GetApiKey();
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

std::map<const std::string,std::string> GlobalFunctions::GetGobalHttpHeaders()
{
    return _config->GetGobalHttpHeaders();
}

void GlobalFunctions::FireNewEvent(const SystemEvent event, const std::string& parameter)
{
    _serviceEventManager->FireNewEvent(event, parameter); 
}

/**
 * @brief Register Callback for System Events
 * 
 * @param eventFilter array of the Filter for EvemtTypes empty array => All Events
 * @param function the callback function
 * @param Name the Name Consumer (Loging)
 */
void GlobalFunctions::RegisterForEvent(const std::vector<SystemEvent>& eventFilter, EventDelegate function, const std::string name) {
    _serviceEventManager->RegisterMe(eventFilter, function, name);
}

/**
 * @brief Register Callback for System Events
 * 
 * @param eventFilter the Filter for EvemtTypes
 * @param function the callback function
 * @param Name the Name Consumer (Loging)
 */
void GlobalFunctions::RegisterForEvent(const SystemEvent event, EventDelegate function, const std::string name)
{
    _serviceEventManager->RegisterMe(event, function, name);
}

std::string GlobalFunctions::ScanAccessPoints(const std::string& interfaceName)
{
    const json result = _networkManager->ScanAccessPoints(interfaceName);
    return result.dump();
}

void GlobalFunctions::SetInternalVariable(const std::string& name, const std::string& value)
{
    LOG(DEBUG) << "SetInternalVariable " << name << " to " << value;

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
        FireNewEvent(SystemEvent::ValueChanged, j.dump());
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

std::string GlobalFunctions::GetInternalVariable(const std::string& name, const std::string& defaultValue)
{
    std::string result(defaultValue);

    const auto findIt = _internalVariables.find(name);
    if( findIt != _internalVariables.end()) {
        result = findIt->second; 
    } else {
        _internalVariables.insert(std::pair<std::string, std::string>(name, defaultValue));
    }

    return result;
}

void GlobalFunctions::ShowInternalVars()
{
    for(const auto entry : _internalVariables) {
        std::cout << entry.first << " => " << entry.second << std::endl;
    }
}

std::vector<AutomationPage> GlobalFunctions::GetAutomationPages()
{
    //Todo _databaseManager->GetAutomationPages();
    //Todo Replace Server Name / Url

    std::vector<AutomationPage> pages;
    AutomationPage page1;
    AutomationPage page2;
    pages.push_back(page1);
    pages.push_back(page2);
    pages[0].Description = "Haus";
    pages[0].Name = "pageHaus";
    pages[0].Icon = "http://localhost:8080/resources/icon1.png";
    pages[1].Description = "Stall";
    pages[1].Name = "pageStall";
    pages[1].Icon = "http://localhost:8080/resources/icon2.png";

    return pages;
}   
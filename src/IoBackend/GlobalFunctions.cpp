#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "GlobalFunctions"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "GlobalFunctions.hpp"

#include "../common/database//Statement.h"
#include "../common/database/TableField.h"
#include "../common/easylogging/easylogging++.h"
#include "../common/utils/commonutils.h"

void GlobalFunctions::CreateAutoTables() const
{
    std::vector<TableField> fields;
    TableField field;

    field.name_ = "ID";
    field.SetType(DatabaseFieldTypes::INTEGER);
    field.SetAutoInc();
    fields.push_back(field);

    field.name_ = "NAME";
    field.SetType(DatabaseFieldTypes::VARCHAR);
    field.SetNoKey();
    field.size_ = 256;
    field.notNull_ = true;
    fields.push_back(field);

    field.name_ = "DESCRIPTION";
    field.SetType(DatabaseFieldTypes::VARCHAR);
    field.SetNoKey();
    field.size_ = 512;
    field.notNull_ = true;
    fields.push_back(field);

    field.name_ = "ICON";
    field.SetType(DatabaseFieldTypes::VARCHAR);
    field.SetNoKey();
    field.size_ = 512;
    field.notNull_ = true;
    fields.push_back(field);

    _databaseManager->CreateTable("AUTOMATION_PAGES", fields);

    auto strSQL = _databaseManager->PrepareSQL("CREATE UNIQUE INDEX AUTOMATION_PAGES_NAME ON AUTOMATION_PAGES(NAME);");
    _databaseManager->DoDml(strSQL);

    fields.clear();

    field.name_ = "ID";
    field.SetType(DatabaseFieldTypes::INTEGER);
    field.SetAutoInc();
    field.SetNoKey();
    fields.push_back(field);

    field.name_ = "NAME";
    field.SetType(DatabaseFieldTypes::VARCHAR);
    field.SetNoKey();
    field.size_ = 256;
    field.notNull_ = true;
    fields.push_back(field);

    _databaseManager->CreateTable("AUTOMATION_ELEMENT_TYPES", fields);

    strSQL =
    _databaseManager->PrepareSQL("CREATE UNIQUE INDEX AUTOMATION_ELEMENT_TYPES_NAME ON AUTOMATION_PAGES(NAME);");
    _databaseManager->DoDml(strSQL);

    fields.clear();

    field.name_ = "ID";
    field.SetType(DatabaseFieldTypes::INTEGER);
    field.size_ = -1;
    field.notNull_ = true;
    field.SetNoKey();
    field.SetAutoInc();
    fields.push_back(field);

    field.name_ = "PAGEID";
    field.SetType(DatabaseFieldTypes::INTEGER);
    field.size_ = -1;
    field.notNull_ = true;
    field.SetDefText(" REFERENCES AUTOMATION_PAGES(ID)");
    field.SetNoKey();
    fields.push_back(field);

    field.name_ = "TYPEID";
    field.SetType(DatabaseFieldTypes::INTEGER);
    field.size_ = -1;
    field.notNull_ = true;
    field.SetDefText(" REFERENCES AUTOMATION_ELEMENT_TYPES(ID)");
    field.SetNoKey();
    fields.push_back(field);

    field.name_ = "NAME";
    field.SetType(DatabaseFieldTypes::VARCHAR);
    field.SetNoKey();
    field.size_ = 256;
    field.notNull_ = true;
    fields.push_back(field);

    field.name_ = "DESCRIPTION";
    field.SetType(DatabaseFieldTypes::VARCHAR);
    field.SetNoKey();
    field.size_ = 512;
    field.notNull_ = false;
    fields.push_back(field);

    field.name_ = "SETPARAMETER";
    field.SetType(DatabaseFieldTypes::VARCHAR);
    field.SetNoKey();
    field.size_ = 256;
    field.notNull_ = true;
    fields.push_back(field);

    field.name_ = "VALUEPARAMETER";
    field.SetType(DatabaseFieldTypes::VARCHAR);
    field.SetNoKey();
    field.size_ = 256;
    field.notNull_ = true;
    fields.push_back(field);

    _databaseManager->CreateTable("AUTOMATION_ELEMENT", fields);

    strSQL = _databaseManager->PrepareSQL("CREATE UNIQUE INDEX AUTOMATION_ELEMENT_NAME ON AUTOMATION_ELEMENT(NAME);");
    _databaseManager->DoDml(strSQL);
}

void GlobalFunctions::CheckElementTypeTable() const
{
    auto typeId = _databaseManager->GetIdFromStringKey("AUTOMATION_ELEMENT_TYPES", "NAME", "ONOFFBUTTON");
    if(typeId == -1) {
        auto strSQL =
        _databaseManager->PrepareSQL("INSERT INTO AUTOMATION_ELEMENT_TYPES (NAME) VALUES ('ONOFFBUTTON')");
        _databaseManager->DoDml(strSQL);
    }
}

void GlobalFunctions::CreateDefaultData() const
{
    auto strSQL = _databaseManager->PrepareSQL(
    "INSERT INTO AUTOMATION_PAGES (NAME, DESCRIPTION, ICON) VALUES ('pageHaus', 'Haus', '@HOST@/resources/icon1.png')");
    _databaseManager->DoDml(strSQL);

    strSQL = _databaseManager->PrepareSQL(
    "INSERT INTO AUTOMATION_ELEMENT (PAGEID, TYPEID, NAME, DESCRIPTION, SETPARAMETER, VALUEPARAMETER) VALUES (1, 1, 'lightYard', 'Licht Hof', 'SETHOFLIGHT', 'HOFLIGHT')");
    _databaseManager->DoDml(strSQL);

    strSQL = _databaseManager->PrepareSQL("INSERT INTO AUTOMATION_PAGES (NAME, DESCRIPTION, ICON) VALUES ('pageBarn', "
                                          "'Stall', '@HOST@/resources/icon2.png')");
    _databaseManager->DoDml(strSQL);

    strSQL = _databaseManager->PrepareSQL("INSERT INTO AUTOMATION_ELEMENT (PAGEID, TYPEID, NAME, DESCRIPTION, SETPARAMETER, VALUEPARAMETER) VALUES "
                                          "(2, 1, 'lightLooseBarn', 'Licht Offenstall', 'SETLOOSEBARN', 'LOOSEBARN')");
    _databaseManager->DoDml(strSQL);
}

void GlobalFunctions::OnDatabaseStartup(DatabaseState state) const
{
    if(state == DatabaseState::StartupNew) {
        LOG(DEBUG) << "Create New Database";

        CreateAutoTables();
        CheckElementTypeTable();
        CreateDefaultData();
    } else {
        CheckElementTypeTable();
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
    if(utils::hasEnding(fileName, ".html") || utils::hasEnding(fileName, ".htm")) {
        return "text/html";
    }

    if(utils::hasEnding(fileName, ".css")) {
        return "text/css";
    }

    return "text/plain";
}

uint16_t GlobalFunctions::GetServerPort() const
{
    return _config->GetServerPort();
}

std::map<const std::string, std::string> GlobalFunctions::GetGobalHttpHeaders()
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
void GlobalFunctions::RegisterForEvent(const std::vector<SystemEvent>& eventFilter, EventDelegate function, const std::string name)
{
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
    if(findIt != _internalVariables.end()) {
        result = findIt->second;
    }

    return result;
}

std::string GlobalFunctions::GetInternalVariable(const std::string& name, const std::string& defaultValue)
{
    std::string result(defaultValue);

    const auto findIt = _internalVariables.find(name);
    if(findIt != _internalVariables.end()) {
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

/**
 * @brief Retrun the List of Automatik pages
 **/
std::vector<AutomationPage> GlobalFunctions::GetAutomationPages()
{
    std::vector<AutomationPage> pages;
    
    auto sqlText = "select NAME, DESCRIPTION, ICON from AUTOMATION_PAGES";

    auto query = _databaseManager->CreateQuery(sqlText);
    if(query->Eof()) {
        delete query;
        return pages;
    }

    if(query->Execute() > 0) {
        while(!query->Eof()) {
            AutomationPage page;
            page.Name = query->GetColumnText(0);
            page.Description = query->GetColumnText(1);
            page.Icon = query->GetColumnText(2);
            pages.push_back(page);
            query->NextRow();
        }
    }
    delete query;

    return pages;
}

/**
 * @brief Retrun the List Elements of an Automatik Page
 **/
AutomationElements GlobalFunctions::GetAutomationElements(const std::string& pageName)
{
    AutomationElements elements;

    std::string sqlText = "select ID from AUTOMATION_PAGES where NAME = '";
    sqlText += pageName + "'";
    
    auto query = _databaseManager->CreateQuery(sqlText);
    if(query->Eof()) {
        delete query;
        return elements;
    }

    std::string pageId;
    if(query->Execute() > 0) {
        if(!query->Eof()) {
            pageId = query->GetColumnText(0);
            query->NextRow();
        }
    }

    delete query;

    sqlText = "select AUTOMATION_ELEMENT.NAME, DESCRIPTION, AUTOMATION_ELEMENT_TYPES.NAME, SETPARAMETER, VALUEPARAMETER from AUTOMATION_ELEMENT, AUTOMATION_ELEMENT_TYPES where TYPEID = AUTOMATION_ELEMENT_TYPES.ID and PAGEID = ";
    sqlText += pageId + " order by AUTOMATION_ELEMENT.ID";

    query = _databaseManager->CreateQuery(sqlText);
    if(query->Eof()) {
        delete query;
        return elements;
    }

    if(query->Execute() > 0) {
        while(!query->Eof()) {
            elements.Name = pageName;
            AutomationElement element;
            element.Id = query->GetColumnText(0);
            element.Description = query->GetColumnText(1);
            element.TypeName = StringToAutomationElementType(query->GetColumnText(2));
            element.SetParameter = query->GetColumnText(3);
            element.ValueParameter = query->GetColumnText(4);
            element.Value = GetInternalVariable(element.Id, "OFF");
            elements.Elements.push_back(element);
            query->NextRow();
        }
    }

    delete query;
    return elements;
}
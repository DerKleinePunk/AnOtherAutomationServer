#pragma once

#include "../../common/json/json.hpp"
#include "../../common/utils/commonutils.h"

using json = nlohmann::json;

enum class AutomationElementType {
    UNDEF,
    ONOFFBUTTON,
};

NLOHMANN_JSON_SERIALIZE_ENUM( AutomationElementType, {
    {AutomationElementType::UNDEF, "UNDEF"},
    {AutomationElementType::ONOFFBUTTON, "ONOFFBUTTON"},
   
})

AutomationElementType StringToAutomationElementType(const std::string& value);
std::ostream& operator<<(std::ostream& os, const AutomationElementType c);

struct AutomationElement
{
    std::string Description;
    std::string Id;
    AutomationElementType TypeName;
    std::string SetParameter;
    std::string ValueParameter;
    std::string Value;
};

struct AutomationElements {
    std::string Name;
    std::vector<AutomationElement> Elements;
};

void to_json(json& j, const AutomationElement& p);
void to_json(json& j, const AutomationElements& p);
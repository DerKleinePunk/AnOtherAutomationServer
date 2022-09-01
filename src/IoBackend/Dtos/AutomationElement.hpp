#pragma once

#include "../../common/json/json.hpp"
#include "../../common/utils/commonutils.h"

using json = nlohmann::json;

enum class AutomationElementType {
    ONOFFBUTTON,
    
};

NLOHMANN_JSON_SERIALIZE_ENUM( AutomationElementType, {
    {AutomationElementType::ONOFFBUTTON, "ONOFFBUTTON"},
})

struct AutomationElement
{
    std::string Description;
    std::string Id;
    AutomationElementType TypeName;
    std::string Value;
};

struct AutomationElements {
    std::string Name;
    std::vector<AutomationElement> Elements;
};


void to_json(json& j, const AutomationElement& p);
void to_json(json& j, const AutomationElements& p);
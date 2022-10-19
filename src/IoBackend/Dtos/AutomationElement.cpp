#include "AutomationElement.hpp"

// Todo find better way NLOHMANN_JSON_SERIALIZE_ENUM ?
AutomationElementType StringToAutomationElementType(const std::string& value)
{
    if(value == "ONOFFBUTTON") {
        return AutomationElementType::ONOFFBUTTON;
    }
    if(value == "VALUETEXT") {
        return AutomationElementType::VALUETEXT;
    }
    return AutomationElementType::UNDEF;
}

std::ostream& operator<<(std::ostream& os, const AutomationElementType c)
{
    switch(c) {
    case AutomationElementType::UNDEF:
        os << "UNDEF";
        break;
    case AutomationElementType::ONOFFBUTTON:
        os << "ONOFFBUTTON";
        break;
    case AutomationElementType::VALUETEXT:
        os << "VALUETEXT";
        break;
    }
    return os;
}

void to_json(json& j, const AutomationElement& p)
{
    j = json{
        { "description", p.Description },
        { "id", p.Id },
        { "typeName", p.TypeName },
        { "setParameter", p.SetParameter },
        { "valueParameter", p.ValueParameter },
        { "value", p.Value },
    };
}

void to_json(json& j, const AutomationElements& p)
{
    j = json{ { "name", p.Name }, { "elements", p.Elements } };
}
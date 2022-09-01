#include "AutomationElement.hpp"

void to_json(json& j, const AutomationElement& p)
{
    j = json{ 
        { "description", p.Description }, 
        { "id", p.Id },
        { "typeName", p.TypeName },
        { "value", p.Value},
    };
}

void to_json(json& j, const AutomationElements& p)
{
    j = json{ 
        { "name", p.Name }, 
        { "elements", p.Elements }  
    };
}
#include "AutomationPage.hpp"

void to_json(json& j, const AutomationPage& p)
{
    j = json{ 
        { "description", p.Description }, 
        { "name", p.Name },
        { "icon", p.Icon }  
    };
}
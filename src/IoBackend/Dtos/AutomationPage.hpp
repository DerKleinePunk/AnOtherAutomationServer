#pragma once

#include "../../common/json/json.hpp"
#include "../../common/utils/commonutils.h"

using json = nlohmann::json;

//"[{\"description\" : \"Page 1\", \"name\" : \"page1\", \"icon\" : \"http://localhost:8000/resources/icon1.png\"}]"

struct AutomationPage
{
    std::string Description;
    std::string Name;
    std::string Icon;
};

void to_json(json& j, const AutomationPage& p);
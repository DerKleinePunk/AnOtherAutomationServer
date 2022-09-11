#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "AutomationResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "AutomationResource.hpp"

#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"
#include "../Dtos/AutomationElement.hpp"
#include "../Dtos/AutomationPage.hpp"

HttpResponse* AutomationResource::HandlePages(HttpRequest& request, const std::string& method, HttpResponse* result)
{
    const auto pages = _globalFunctions->GetAutomationPages();
    const json jResult = pages;

    result->SetContent("application/json", jResult.dump());
    result->SetCode(HTTP_STATUS_OK);

    return result;
}

HttpResponse*
AutomationResource::HandleLoadPage(HttpRequest& request, const std::string& method, HttpResponse* result, const std::string& pageName)
{
    AutomationElements elements;
    if(pageName == "pageHaus") {

        elements.Name = "pageHaus";
        AutomationElement element;
        element.Description = "Hof beleuchtung";
        element.Id = "hofbel";
        element.TypeName = AutomationElementType::ONOFFBUTTON;
        element.Value = _globalFunctions->GetInternalVariable(element.Id, "OFF");
        elements.Elements.push_back(element);

    } else if(pageName == "pageStall") {
        elements.Name = "pageStall";
        AutomationElement element;
        element.Description = "Offenstall Beleuchtung";
        element.Id = "offenstallbel";
        element.TypeName = AutomationElementType::ONOFFBUTTON;
        element.Value = _globalFunctions->GetInternalVariable(element.Id, "ON");
        elements.Elements.push_back(element);
    }

    if(elements.Elements.size() == 0) {
        result->SetCode(HTTP_STATUS_NOT_FOUND);
        return result;
    }

    const json jResult = elements;

    result->SetContent("application/json", jResult.dump());
    result->SetCode(HTTP_STATUS_OK);

    return result;
}

AutomationResource::AutomationResource(GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _globalFunctions = globalFunctions;
}

AutomationResource::~AutomationResource()
{
}

HttpResponse* AutomationResource::Process(HttpRequest& request, const std::string& url, const std::string& method)
{
    auto result = new HttpResponse();
    const auto apiKey = request.GetHeader(std::string("X-API-KEY"), true);

    LOG(DEBUG) << url << " Api Call Get with apiKey " << apiKey;

    if(method == "OPTIONS") {
        //Only Response Cors Headers from Main
        //Chrome Use this Checking Cors
        result->SetCode(HTTP_STATUS_NO_CONTENT);
        result->SetContent("", "");
        result->SetHeader("Access-Control-Max-Age", "120"); // duration (in seconds)
        return result;
    }

    if(!_globalFunctions->IsApiKeyOk(apiKey)) {
        result->SetContent("application/json", "{\"error\" : \"UNAUTHORIZED\"}");
        result->SetCode(HTTP_STATUS_UNAUTHORIZED);
        return result;
    }

    if(url == "pages") {
        return HandlePages(request, method, result);
    }

    if(utils::hasBegining(url, "page/")) {
        auto pageName = url;
        utils::replaceAll(pageName, "page/", "");
        return HandleLoadPage(request, method, result, pageName);
    }

    if(utils::hasBegining(url, "Variable")) {
        if(method == "POST") {
            const auto id = request.GetParameter("id");
            const auto value = request.GetParameter("value");
            if(!id.empty() && !value.empty())
            {
                _globalFunctions->SetInternalVariable(id, value);
                result->SetContent("application/json", "{\"error\" : \"NONE\"}");
                result->SetCode(201);
                return result;
            }
            else
            {
                 _globalFunctions->SetInternalVariable(id, value);
                result->SetContent("application/json", "{\"error\" : \"BAD REQUEST\"}");
                result->SetCode(HTTP_STATUS_BAD_REQUEST);
                return result;
            }
        }
    }

    result->SetCode(HTTP_STATUS_FOUND);
    return result;
}
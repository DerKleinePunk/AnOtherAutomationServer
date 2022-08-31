#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "AutomationResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "AutomationResource.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"

HttpResponse* AutomationResource::HandlePages(HttpRequest& request, const std::string& method, HttpResponse* result)
{
    result->SetContent("application/json", "[{\"description\" : \"Page 1\", \"name\" : \"page1\", \"icon\" : \"http://localhost:8000/resources/icon1.png\"}]");
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
    const auto arg = request.GetParameter(std::string("a"));
    const auto apiKey = request.GetHeader(std::string("X-API-KEY"), true);

    LOG(DEBUG) << url << " Api Call Get with " << arg << " apiKey " << apiKey;

    if(!_globalFunctions->IsApiKeyOk(apiKey)) {
        result->SetContent("application/json", "{\"error\" : \"UNAUTHORIZED\"}");
        result->SetCode(HTTP_STATUS_UNAUTHORIZED);
        return result; 
    }

    if(url == "pages")
    {
        return HandlePages(request, method, result);
    }
    result->SetCode(HTTP_STATUS_FOUND);
    return result;
}
#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "ApiResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "ApiResource.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"

ApiResource::ApiResource(GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _globalFunctions = globalFunctions;
}

ApiResource::~ApiResource()
{
}

HttpResponse* ApiResource::Process(HttpRequest& request, const std::string& url, const std::string& method)
{
    auto result = new HttpResponse();
    const auto arg = request.GetParameter(std::string("a"));
    const auto apiKey = request.GetHeader(std::string("X-API-KEY"));

    LOG(DEBUG) << url << " Api Call Get with " << arg << " apiKey " << apiKey;

    if(!_globalFunctions->IsApiKeyOk(apiKey)) 
    {
        result->SetContent("application/json", "{\"error\" : \"UNAUTHORIZED\"}");
        result->SetCode(HTTP_STATUS_UNAUTHORIZED);
        return result; 
    }

    if(arg.size() > 0) {
        auto text = std::string("{\"parameter\" : \"");
        text += arg;
        text += "\"}";
        result->SetContent("application/json", text);
    } else {
        result->SetContent("application/json", "{\"parameter\" : \"value\"}");
    }

    if(method == "POST")
    {
        LOG(DEBUG) << "Body " << request.GetBody();
        result->SetCode(201);
    }

    return result;  
}
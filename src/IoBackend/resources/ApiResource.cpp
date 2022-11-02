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
    const auto apiKey = request.GetHeader(std::string("X-API-KEY"), true);

    LOG(DEBUG) << url << " Api Call Get with " << arg << " apiKey " << apiKey;

    if(!_globalFunctions->IsApiKeyOk(apiKey)) {
        result->SetContent("application/json", "{\"error\" : \"UNAUTHORIZED\"}");
        result->SetCode(HTTP_STATUS_UNAUTHORIZED);
        return result; 
    }

    if(url == "wlan" && method == "GET") {
        result->SetContent("application/json", _globalFunctions->ScanAccessPoints());
        result->SetCode(200);
        return result; 
    }

    if(url == "wlan" && method == "PUT") {
        unsigned int resultCode = 404;
        const auto connectionName = request.GetParameter(std::string("connectionName"));
        const auto passwort = request.GetParameter(std::string("passwort"));
        if(_globalFunctions->ConnectAccessPoint(connectionName, passwort))
        {
            result->SetContent("application/json", "{\"result\" : \"ok\"}");
            resultCode = 200;
        } else {
            result->SetContent("application/json", "{\"result\" : \"error\"}");
        }
        result->SetCode(resultCode);
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
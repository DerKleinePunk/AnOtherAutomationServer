#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "AuthResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "AuthResource.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"

HttpResponse* AuthResource::HandleLogin(HttpRequest& request, const std::string& method, HttpResponse* result)
{
    if(method != "POST") {
        result->SetCode(HTTP_STATUS_BAD_REQUEST);
        result->SetContent("application/json", "{\"error\" : \"Bad Request\"}");
        return result;
    }

    const auto user = request.GetParameter(std::string("user"));
    const auto pass = request.GetHeader(std::string("pass"));

    if(user != "mn" && pass != "mn") {
        result->SetContent("application/json", "{\"error\" : \"UNAUTHORIZED\"}");
        result->SetCode(HTTP_STATUS_UNAUTHORIZED);
        return result; 
    }

    std::string token("12345678");

    //"X-API-KEY=123456789;SameSite=Strict;path=/"
    result->SetCookie("X-API-KEY", token);

    result->SetContent("application/json", "{\"token\" : \"" + token + "\"}");
    result->SetCode(201); //Created
    return result; 
}

AuthResource::AuthResource(GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _globalFunctions = globalFunctions;
}

AuthResource::~AuthResource()
{
}

HttpResponse* AuthResource::Process(HttpRequest& request, const std::string& url, const std::string& method)
{
    auto result = new HttpResponse();
    if(url == "")
    {
        result->SetCode(HTTP_STATUS_BAD_REQUEST);
        result->SetContent("application/json", "{\"error\" : \"Bad Request\"}");
        return result;
    }

    if(url == "login")
    {
        return HandleLogin(request, method, result);
    }

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
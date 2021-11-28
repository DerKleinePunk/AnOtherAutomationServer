#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "MqttResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "MqttResource.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"

MqttResource::MqttResource(GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _globalFunctions = globalFunctions;
}

MqttResource::~MqttResource()
{
}

HttpResponse* MqttResource::Process(HttpRequest& request, const std::string& url, const std::string& method)
{
    auto result = new HttpResponse();
    const auto topic = request.GetParameter(std::string("topic"));
    const auto value = request.GetParameter(std::string("value"));
    const auto apiKey = request.GetHeader(std::string("X-API-KEY"));

    LOG(DEBUG) << url << " Api Call Get with " << topic << " apiKey " << apiKey;

    if(!_globalFunctions->IsApiKeyOk(apiKey)) 
    {
        result->SetContent("application/json", "{\"error\" : \"UNAUTHORIZED\"}");
        result->SetCode(HTTP_STATUS_UNAUTHORIZED);
        return result; 
    }

    if(topic.size() > 0) {
        auto text = std::string("{\"parameter\" : \"");
        text += topic;
        text += "\"}";
        result->SetContent("application/json", text);
    } else {
        result->SetContent("application/json", "{\"parameter\" : \"value\"}");
    }

    if(method == "POST")
    {
        LOG(DEBUG) << "Body " << request.GetBody();
        if(url == "set") {
            _globalFunctions->FireNewEvent("PublishMqtt", request.GetBody());
        }
        result->SetCode(201);
    }

    return result;  
}
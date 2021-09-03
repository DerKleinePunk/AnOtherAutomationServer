#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "MqttResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "MqttResource.h"
#include "../../common/easylogging/easylogging++.h"

MqttResource::MqttResource(GlobalFunctions* globalFunctions)
{
    _globalFunctions = globalFunctions;
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

MqttResource::~MqttResource()
{
}

const std::shared_ptr<httpserver::http_response> MqttResource::render_GET(const httpserver::http_request& request) {
    const auto apiKey = request.get_header("X-API-KEY");

    if(!_globalFunctions->IsApiKeyOk(apiKey)) {
        LOG(WARNING) << "Api Call with wrong Key";
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("", 401));
    }

    const auto arg = request.get_arg("arg1");
    
    LOG(DEBUG) << "Api Call Get with " << arg;
}

const std::shared_ptr<httpserver::http_response> MqttResource::render_POST(const httpserver::http_request& request) {
    const auto apiKey = request.get_header("X-API-KEY");

    if(!_globalFunctions->IsApiKeyOk(apiKey)) {
        LOG(WARNING) << "Api Call with wrong Key";
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("", 401));
    }

    const auto arg = request.get_arg("arg1");
    const auto content = request.get_content();

    LOG(DEBUG) << "Api Call with " << arg << " " << content;
}
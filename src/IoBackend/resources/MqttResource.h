#pragma once

#include <httpserver.hpp>
#include "../../common/json/json.hpp"
#include "../GlobalFunctions.hpp"

using json = nlohmann::json;

class MqttResource : public httpserver::http_resource
{
private:
    GlobalFunctions* _globalFunctions;
public:
    MqttResource(GlobalFunctions* globalFunctions);
    ~MqttResource();

    const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request&);
    const std::shared_ptr<httpserver::http_response> render_POST(const httpserver::http_request&);
};


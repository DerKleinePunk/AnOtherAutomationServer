#pragma once

#include "../WebServer/HttpRequest.hpp"
#include "../WebServer/HttpResource.hpp"
#include "../GlobalFunctions.hpp"

class MqttResource : public HttpResource
{
private:
    GlobalFunctions* _globalFunctions;
public:
    MqttResource(GlobalFunctions* globalFunctions);
    ~MqttResource();

    HttpResponse* Process(HttpRequest& request, const std::string& url, const std::string& method);
};



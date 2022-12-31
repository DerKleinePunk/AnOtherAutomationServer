#pragma once
#include "../WebServer/HttpRequest.hpp"
#include "../WebServer/HttpResource.hpp"
#include "../GlobalFunctions.hpp"

class ConfigResource : public HttpResource
{
private:
    GlobalFunctions* _globalFunctions;
    HttpResponse* HandleBlockly(HttpRequest& request, const std::string& method, HttpResponse* result, const std::string& pageName);

public:
    ConfigResource(GlobalFunctions* globalFunctions);
    ~ConfigResource();

    HttpResponse* Process(HttpRequest& request, const std::string& url, const std::string& method);
};



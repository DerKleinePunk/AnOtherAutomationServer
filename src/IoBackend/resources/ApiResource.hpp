#pragma once
#include "../WebServer/HttpRequest.hpp"
#include "../WebServer/HttpResource.hpp"
#include "../GlobalFunctions.hpp"

class ApiResource : public HttpResource
{
private:
    GlobalFunctions* _globalFunctions;
public:
    ApiResource(GlobalFunctions* globalFunctions);
    ~ApiResource();

    HttpResponse* Process(HttpRequest& request, const std::string& url, const std::string& method);
};


#pragma once
#include "../WebServer/HttpRequest.hpp"
#include "../WebServer/HttpResource.hpp"
#include "../GlobalFunctions.hpp"

class AuthResource : public HttpResource
{
private:
    GlobalFunctions* _globalFunctions;
    HttpResponse* HandleLogin(HttpRequest& request, const std::string& method, HttpResponse* result);

public:
    AuthResource(GlobalFunctions* globalFunctions);
    ~AuthResource();

    HttpResponse* Process(HttpRequest& request, const std::string& url, const std::string& method);
};


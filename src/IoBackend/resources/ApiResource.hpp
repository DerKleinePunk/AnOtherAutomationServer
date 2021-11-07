#pragma once
#include "../WebServer/HttpRequest.hpp"
#include "../WebServer/HttpResource.hpp"

class ApiResource : public HttpResource
{
private:
    /* data */
public:
    ApiResource(/* args */);
    ~ApiResource();

    HttpResponse* Process(HttpRequest& request, const std::string& url, const std::string& method);
};


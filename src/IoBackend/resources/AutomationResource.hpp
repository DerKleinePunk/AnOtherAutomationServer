#pragma once
#include "../WebServer/HttpRequest.hpp"
#include "../WebServer/HttpResource.hpp"
#include "../GlobalFunctions.hpp"

class AutomationResource : public HttpResource
{
private:
    GlobalFunctions* _globalFunctions;
    HttpResponse* HandlePages(HttpRequest& request, const std::string& method, HttpResponse* result);
    HttpResponse* HandleLoadPage(HttpRequest& request, const std::string& method, HttpResponse* result, const std::string& pageName);

public:
    AutomationResource(GlobalFunctions* globalFunctions);
    ~AutomationResource();

    HttpResponse* Process(HttpRequest& request, const std::string& url, const std::string& method);
};

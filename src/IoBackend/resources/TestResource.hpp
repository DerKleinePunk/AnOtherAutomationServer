#pragma once
#include "../WebServer/HttpRequest.hpp"
#include "../WebServer/HttpResource.hpp"

class TestResource : public HttpResource
{
private:
    /* data */
public:
    TestResource(/* args */);
    ~TestResource();

    void Process(HttpRequest& request);
};


#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class HttpResource
{
private:
    /* data */
public:
    virtual ~HttpResource() = default;

    virtual HttpResponse* Process(HttpRequest& request) = 0;
};



#pragma once
#include "HttpRequest.hpp"

class HttpResource
{
private:
    /* data */
public:
    virtual ~HttpResource() = default;

    virtual void Process(HttpRequest& request) = 0;
};



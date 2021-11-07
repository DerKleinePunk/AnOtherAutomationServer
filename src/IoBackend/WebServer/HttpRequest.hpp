#pragma once
#include <libwebsockets.h>
#include <string>

class HttpRequest
{
private:
    struct lws* _wsi;
public:
    HttpRequest(struct lws *wsi);
    ~HttpRequest();

    std::string GetParameter(const std::string& name);
    std::string GetHeader(const std::string& name);
};


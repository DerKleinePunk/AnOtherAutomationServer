#pragma once
#include <libwebsockets.h>
#include <string>

class HttpRequest
{
private:
    struct lws* _wsi;
    std::string _body;
    std::string _host;
public:
    HttpRequest(struct lws *wsi, const std::string* body, const std::string* host);
    ~HttpRequest();

    std::string GetParameter(const std::string& name);
    std::string GetHeader(const std::string& name, bool canbeCookie = false);
    std::string GetBody() const;
    std::string GetHost() const;
};


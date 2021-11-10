#pragma once
#include <libwebsockets.h>
#include <string>

class HttpRequest
{
private:
    struct lws* _wsi;
    std::string _body;
public:
    HttpRequest(struct lws *wsi, const std::string* body);
    ~HttpRequest();

    std::string GetParameter(const std::string& name);
    std::string GetHeader(const std::string& name);
    std::string GetBody() const;
};


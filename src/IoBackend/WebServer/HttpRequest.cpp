#include "HttpRequest.hpp"

HttpRequest::HttpRequest(struct lws *wsi)
{
    _wsi = wsi;
}

HttpRequest::~HttpRequest()
{
}

std::string HttpRequest::GetParameter(const std::string& name)
{
    auto result = std::string("");
    char value[100];

    int z = lws_get_urlarg_by_name_safe(_wsi, name.c_str(), value, sizeof(value) - 1);
    if(z>0) {
        result = value;
    }
    return result;
}
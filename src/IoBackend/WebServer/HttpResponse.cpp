#include "HttpResponse.hpp"
#include <libwebsockets.h>

HttpResponse::HttpResponse(/* args */)
{
    _contentType = "text/html";
    _content = "<!DOCTYPE html><html lang=\"de\"><head><meta charset=\"utf-8\"><title>empty content</title></head><body></body></html>";
    _code = HTTP_STATUS_OK;
}

HttpResponse::~HttpResponse()
{
}

const std::string& HttpResponse::GetContentType()
{
    return _contentType;
}

//Return true if last block
bool HttpResponse::GetNextBlock(char* buffer)
{
    //Todo Handle Handle BufferSize !
    auto size = _content.size();

    strncpy(buffer, _content.c_str(), size);

    return true;
}

void HttpResponse::SetContent(const std::string& type, const std::string& content)
{
    _contentType = type;
    _content = content;
}

void HttpResponse::SetCode(unsigned int code)
{
    _code = code;
}

void HttpResponse::SetCookie(const std::string& name, const std::string& value)
{
    if(_cookies.find(name) != _cookies.end())
    {
        _cookies[name] = value;
        return;
    }
    _cookies.insert(std::pair<const std::string, std::string>(name, value));
    
}

void HttpResponse::SetHeader(const std::string& name, const std::string& value)
{
    if(_headers.find(name) != _headers.end())
    {
        _headers[name] = value;
        return;
    }
    _headers.insert(std::pair<const std::string, std::string>(name, value));
}

unsigned int HttpResponse::GetCode() 
{
    return _code;
}

std::map<const std::string,std::string> HttpResponse::GetCookies()
{
    return _cookies;
}

std::map<const std::string,std::string> HttpResponse::GetHeaders()
{
    return _headers;
}
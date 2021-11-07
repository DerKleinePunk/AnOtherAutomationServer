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

unsigned int HttpResponse::GetCode() 
{
    return _code;
}
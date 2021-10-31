#include "HttpResponse.hpp"
#include "cstring"

HttpResponse::HttpResponse(/* args */)
{
    _contentType = "text/html";
    _content = "<!DOCTYPE html><html lang=\"de\"><head><meta charset=\"utf-8\"><title>DynPage Simple</title></head><body></body></html>";
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
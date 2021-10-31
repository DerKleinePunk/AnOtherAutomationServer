#pragma once
#include <string>

class HttpResponse
{
private:
    std::string _contentType;
    std::string _content;

public: 
    HttpResponse(/* args */);
    ~HttpResponse();

    const std::string& GetContentType();
    bool GetNextBlock(char* buffer);
};

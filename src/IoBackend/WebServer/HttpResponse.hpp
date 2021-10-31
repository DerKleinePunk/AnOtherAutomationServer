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

    //Getter
    const std::string& GetContentType();
    bool GetNextBlock(char* buffer);

    //Setter
    void SetContent(const std::string& type, const std::string& content);

};

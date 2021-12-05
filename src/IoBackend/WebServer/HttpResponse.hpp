#pragma once
#include <string>
#include <map>

class HttpResponse
{
private:
    std::string _contentType;
    std::string _content;
    unsigned int _code;
    std::map<const std::string,std::string> _cookies;

public: 
    HttpResponse(/* args */);
    ~HttpResponse();

    //Getter
    const std::string& GetContentType();
    bool GetNextBlock(char* buffer);

    //Setter
    void SetContent(const std::string& type, const std::string& content);
    void SetCode(unsigned int code);
    void SetCookie(const std::string& name, const std::string& value);

    //Getter
    unsigned int GetCode();
    std::map<const std::string,std::string> GetCookies();
};

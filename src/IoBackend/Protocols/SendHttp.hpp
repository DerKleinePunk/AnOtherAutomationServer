#pragma once
#include <string>
#include "../../common/json/json.hpp"

extern "C"
{
    #include <curl/curl.h>
}

using json = nlohmann::json;


class SendHttp {
    std::string readBuffer;
    long _responseCode;
    void LogMessage(CURL *handle, curl_infotype type, char *data, size_t size, void *userp);

public:
    SendHttp();
    ~SendHttp();

    int PostRequest(const std::string& apiKey, const std::string& url, const std::string& body);
    int PostRequest(const std::string& apiKey, const std::string& url,json const& message);
    void AddResultData(void *data, size_t size);
    std::string GetResult(long& responseCode) const;

    int GetRequest(const std::string& apiKey, const std::string& url);
};
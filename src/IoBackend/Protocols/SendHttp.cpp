#ifndef ELPP_DEFAULT_LOGGER
#   define ELPP_DEFAULT_LOGGER "SendHttp"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#   define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include <string.h>
#include <string>
#include <stdexcept>
#include <memory>
#include "SendHttp.hpp"

#include "../../common/easylogging/easylogging++.h"
#include "../../common/json/json.hpp"
#include "../../common/utils/commonutils.h"
#include "../../common/version.hpp"

//https://gist.github.com/jay/2a6c54cc6da442489561
//https://curl.haxx.se/libcurl/c/CURLOPT_DEBUGFUNCTION.html

typedef std::function<void(CURL *handle, curl_infotype type, char *data, size_t size, void *userp)> LOGCALLBACK;
static LOGCALLBACK logCallback = nullptr;


static int my_trace(CURL *handle, curl_infotype type, char *data, size_t size, void *userp)
{
    if(logCallback){
        logCallback(handle, type, data, size, userp);
    }

    return 0;
}

static int WriteCallback(void *data, size_t size, size_t nmemb, void *userp)
{ 
    size_t realsize = size * nmemb;
    auto caller = (SendHttp*)userp;
    if(caller != nullptr) {
        caller->AddResultData(data, realsize);
    }
    return realsize;
}

void SendHttp::LogMessage(CURL *handle, curl_infotype type, char *data, size_t size, void *userp) {
    const char *text;
    switch (type) {
        case CURLINFO_TEXT:
            {
                std::string line(data);
                utils::replaceAll(line, "\n", "");
                utils::replaceAll(line, "\r", "");
                LOG(DEBUG) << line;
            }
            return;
        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
        case CURLINFO_END:
            text = "END";
            break;
        default:
            text = "curl_infotype Unknown";
    }

    if(!VLOG_IS_ON(3)) return;
    
    size_t i;
    size_t c;
    unsigned int width=0x10;
    std::string dataText("");
    auto ptr = (unsigned char *)data;

    for(i=0; i<size; i+= width) {
        dataText += utils::string_format("%4.4lx: ", (long)i);

        /* show hex to the left */
        for(c = 0; c < width; c++) {
        if(i+c < size)
            dataText += utils::string_format("%02x ", ptr[i+c]);
        else
            dataText +="   ";
        }
    
        /* show data on the right */
        for(c = 0; (c < width) && (i+c < size); c++) {
            char x = (ptr[i+c] >= 0x20 && ptr[i+c] < 0x80) ? ptr[i+c] : '.';
            dataText += x;
        }
    
        dataText += "\n";
    }

    VLOG(3) << utils::string_format("%s, %10.10ld bytes (0x%8.8lx)\n%s", text,  (long)size, (long)size, dataText.c_str());
 }

SendHttp::SendHttp(){
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    curl_global_init(CURL_GLOBAL_ALL);
    logCallback = std::bind(&SendHttp::LogMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
}

SendHttp::~SendHttp(){
    curl_global_cleanup();
}

int SendHttp::PostRequest(const std::string& apiKey, const std::string& url, const std::string& body) {
    auto result = -1;
    auto curl = curl_easy_init();
    if(curl) {
        LOG(DEBUG) << "Start Post to " << url;
        LOG(DEBUG) << "apiKey len " << std::to_string(apiKey.length()) << " Body " << body;

        char errbuf[CURL_ERROR_SIZE];
        memset(errbuf, 0, CURL_ERROR_SIZE);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist *headers=NULL;
        headers = curl_slist_append(headers, "content-type: application/json");
        if(!apiKey.empty()) {
            const std::string header = "X-API-KEY: " + apiKey;
            headers = curl_slist_append(headers, header.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string agent = "AutomationServer V" + std::string(PROJECT_VER);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
        readBuffer.clear();
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

        auto res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &_responseCode);
        if(res != CURLE_OK) {
            result = -2;
            if(strlen(errbuf) > 0) {
                LOG(ERROR) << "curl say " << errbuf;
            } else {
                LOG(ERROR) << "curl say not good but error var is empty";
            }
        } else {
            result = 0;
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return result;
}

int SendHttp::PostRequest(const std::string& apiKey, const std::string& url,json const& message) {
    return PostRequest(apiKey, url, message.dump());
}

void SendHttp::AddResultData(void *data, size_t size) {
    readBuffer.append((char*)data, size);
}

std::string SendHttp::GetResult(long& responseCode) const {
    responseCode = _responseCode;
    return readBuffer;
}

int SendHttp::GetRequest(const std::string& apiKey, const std::string& url)
{
    auto result = -1;
    auto curl = curl_easy_init();
    if(curl) {
        LOG(DEBUG) << "Start Get from " << url;
        LOG(DEBUG) << "apiKey len " << std::to_string(apiKey.length());

        char errbuf[CURL_ERROR_SIZE];
        memset(errbuf, 0, CURL_ERROR_SIZE);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        struct curl_slist *headers=NULL;
        headers = curl_slist_append(headers, "content-type: application/json");
        if(!apiKey.empty()) {
            const std::string header = "X-API-KEY: " + apiKey;
            headers = curl_slist_append(headers, header.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        std::string agent = "AutomationServer V" + std::string(PROJECT_VER);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, agent.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
        readBuffer.clear();
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

        auto res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &_responseCode);
        if(res != CURLE_OK) {
            result = -2;
            if(strlen(errbuf) > 0) {
                LOG(ERROR) << "curl say " << errbuf;
            } else {
                LOG(ERROR) << "curl say not good but error var is empty";
            }
        } else {
            result = 0;
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return result;
}
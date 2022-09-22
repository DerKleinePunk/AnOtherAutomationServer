#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "HttpRequest"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "HttpRequest.hpp"

#include "../../common/easylogging/easylogging++.h"
#include "../../common/json/json.hpp"
#include "../../common/utils/commonutils.h"

using json = nlohmann::json;

HttpRequest::HttpRequest(struct lws* wsi, const std::string* body, const std::string* host)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _wsi = wsi;

    if(body != nullptr) {
        _body = std::string(*body);
    }

    if(host != nullptr) {
        _host = std::string(*host);
    }
}

HttpRequest::~HttpRequest()
{
}

std::string HttpRequest::GetParameter(const std::string& name)
{
    auto result = std::string("");
    char value[100];

    int z = lws_get_urlarg_by_name_safe(_wsi, name.c_str(), value, sizeof(value) - 1);
    if(z > 0) {
        result = value;
    } else if(!_body.empty()) {
        auto jsonText = json::parse(_body);

        auto it_value = jsonText.find(name);
        if(it_value != jsonText.end()) {
            result = jsonText.at(name).get<std::string>();
        }
    }
    return result;
}

std::string HttpRequest::GetHeader(const std::string& name, bool canbeCookie)
{
    auto result = std::string("");
    auto nameIntern = name;

    if(nameIntern[nameIntern.length() - 1] != ':') {
        nameIntern += ":";
    }

    nameIntern = utils::str_tolower(nameIntern);

    auto size = lws_hdr_custom_length(_wsi, nameIntern.c_str(), nameIntern.length());
    if(size > 0) {
        auto value = new char[size + 1];
        int z = lws_hdr_custom_copy(_wsi, value, size + 1, nameIntern.c_str(), nameIntern.length());
        if(z > 0) {
            result = std::string(value, size);
        } else if(z < 0) {
            LOG(ERROR) << nameIntern << " reading header error";
        }
        delete[] value;
    } else if(size < 0) {
        if(canbeCookie) {
            size_t max = 32;
            char textBuffer[32];
            if(lws_http_cookie_get(_wsi, name.c_str(), textBuffer, &max) == 0) {
                result = std::string(textBuffer, max);
            } else {
                LOG(WARNING) << nameIntern << " header or cookie not found";
            }
        } else {
            LOG(WARNING) << nameIntern << " header or cookie not found";
        }
    }
    return result;
}

std::string HttpRequest::GetBody() const
{
    return _body;
}

std::string HttpRequest::GetHost() const
{
    return _host;
}
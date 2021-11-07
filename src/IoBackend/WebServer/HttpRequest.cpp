#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "HttpRequest"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "HttpRequest.hpp"
#include "../../common/utils/commonutils.h"
#include "../../common/easylogging/easylogging++.h"


HttpRequest::HttpRequest(struct lws *wsi)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
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

std::string HttpRequest::GetHeader(const std::string& name)
{
    auto result = std::string("");
    auto nameIntern = name;

    if(nameIntern[nameIntern.length()-1] != ':')
    {
        nameIntern += ":";
    }

    nameIntern = utils::str_tolower(nameIntern);

    auto size = lws_hdr_custom_length(_wsi, nameIntern.c_str(), nameIntern.length());
    if(size > 0 ){
        auto value = new char[size+1];
        int z = lws_hdr_custom_copy(_wsi, value, size + 1, nameIntern.c_str(), nameIntern.length());
        if (z>0) {
            result = std::string(value, size);
        } else if(z < 0){
            LOG(ERROR) << nameIntern << " reading header error";
        }
        delete [] value;
    } else if(size < 0){
        LOG(WARNING) << nameIntern << " header not found";
    }
    return result;
}
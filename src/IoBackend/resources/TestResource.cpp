#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "TestResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "TestResource.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"

TestResource::TestResource(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

TestResource::~TestResource()
{
}

HttpResponse* TestResource::Process(HttpRequest& request)
{
    auto result = new HttpResponse();
    const auto arg = request.GetParameter(std::string("a"));

    LOG(DEBUG) << "Api Call Get with " << arg; //<< "Path " << path;

    if(arg == "3") {
        auto text = std::string("<!DOCTYPE html><html lang=\"de\"><head><meta charset=\"utf-8\"><title>DynPage Simple</title></head><body><p>a was 3</p></body></html>");
        result->SetContent("text/html", text);
    }

    return result;
}
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

void TestResource::Process(HttpRequest& request)
{
    const auto arg = request.GetParameter(std::string("a"));

    LOG(DEBUG) << "Api Call Get with " << arg; //<< "Path " << path;
}
#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "HtmlPageResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "HtmlPageResource.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"

HtmlPageResource::HtmlPageResource(GlobalFunctions* globalFunctions)
{
    _globalFunctions = globalFunctions;
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

HtmlPageResource::~HtmlPageResource()
{
}

const std::shared_ptr<httpserver::http_response> HtmlPageResource::render_GET(const httpserver::http_request& request)
{
    const auto arg = request.get_arg("arg1");
    const auto path = request.get_path();
    const auto pathPieces = request.get_path_pieces();
    
    LOG(DEBUG) << "Api Call Get with " << arg << "Path " << path;

    auto localPath = std::string("./");
    auto iterPath = pathPieces.begin();
    iterPath++;
    while(iterPath != pathPieces.end()) {
        localPath += *iterPath;
        iterPath++;
    }

    if(utils::FileExists(localPath)) {
        const auto contentType = _globalFunctions->GetContentTypeFromFileName(localPath);
        return std::shared_ptr<httpserver::http_response>(new httpserver::file_response(localPath, 200, contentType));
    }

    return std::shared_ptr<httpserver::http_response>(new httpserver::string_response("page not found", 404));
}
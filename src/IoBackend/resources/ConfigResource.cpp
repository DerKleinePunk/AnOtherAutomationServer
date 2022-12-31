#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "ConfigResource"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "ConfigResource.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"
#include "../../common/json/json.hpp"

using json = nlohmann::json;

HttpResponse* ConfigResource::HandleBlockly(HttpRequest& request, const std::string& method, HttpResponse* result, const std::string& pageName)
{
    if(method == "POST") {
        const auto fileName = request.GetParameter(std::string("fileName"));
        const auto flow = request.GetParameter(std::string("flow"));

        LOG(DEBUG) << method << " Blockly with " << fileName << " flow " << flow;

        std::ofstream o(fileName);
        o << flow;
        o.close();

        result->SetCode(201); // Created
        return result;
    } else if(method == "UPDATE" || method == "PUT") {
        const auto fileName = request.GetParameter(std::string("fileName"));
        const auto flow = request.GetParameter(std::string("flow"));
    } else if(method == "GET") {
        const auto fileName = request.GetParameter(std::string("fileName"));

        std::string content;
        std::string line;
        std::ifstream o(fileName);
        if(o.fail()) {
            result->SetCode(HTTP_STATUS_FOUND);
            return result;
        }
        
        while (std::getline(o, line)) {
            content += line;
        }
        o.close();
    
        result->SetContent("application/json", content);
        result->SetCode(HTTP_STATUS_OK);

        LOG(DEBUG) << method << " Blockly send " << fileName << " bytes " << content.size();
    
        return result;
    }

    result->SetCode(HTTP_STATUS_FOUND);
    return result;
}

ConfigResource::ConfigResource(GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _globalFunctions = globalFunctions;
}

ConfigResource::~ConfigResource()
{
}

HttpResponse* ConfigResource::Process(HttpRequest& request, const std::string& url, const std::string& method)
{
    auto result = new HttpResponse();
    try
    {
        const auto arg = request.GetParameter(std::string("a"));
        const auto apiKey = request.GetHeader(std::string("X-API-KEY"), true);

        LOG(DEBUG) << url << " Api Call Get with " << arg << " apiKey " << apiKey;

        if(!_globalFunctions->IsApiKeyOk(apiKey)) {
            result->SetContent("application/json", "{\"error\" : \"UNAUTHORIZED\"}");
            result->SetCode(HTTP_STATUS_UNAUTHORIZED);
            return result; 
        }

        if(utils::hasBegining(url, "blockly")) {
            auto pageName = url;
            utils::replaceAll(pageName, "blockly", "");
            return HandleBlockly(request, method, result, pageName);
        }

        result->SetCode(HTTP_STATUS_FOUND);
        return result;
    }
    catch(json::parse_error& error)
    {
        std::string content = "{\"error\" : \"";
        content += error.what();
        content += "\"}";
        result->SetContent("application/json", content);
        result->SetCode(HTTP_STATUS_BAD_REQUEST);

        return result;
    }
    
}
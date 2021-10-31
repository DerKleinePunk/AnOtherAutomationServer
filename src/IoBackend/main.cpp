#include <httpserver.hpp>
#include <iostream>
#include "../common/easylogging/easylogging++.h"
#include "../common/utils/commonutils.h"
#include "Config.hpp"
#include "Mqtt/MqttConnector.h"
#include "resources/MqttResource.h"
#include "resources/HtmlPageResource.hpp"
#include "resources/TestResource.hpp"
#include "GlobalFunctions.hpp"
#include "WebServer/WebServer.hpp"

INITIALIZE_EASYLOGGINGPP

void custom_access_log(const std::string& url)
{
    LOG(INFO) << "ACCESSING: " << url;
}

void custom_error_log(const std::string& url)
{
    LOG(ERROR) << "ERROR: " << url;
}

const std::shared_ptr<httpserver::http_response> not_found_custom(const httpserver::http_request& req) {

    const auto headers = req.get_headers();
    for(auto const& header : headers)
    {
        LOG(DEBUG) << header.first << " : " << header.second;
    }
    
    if(req.get_path() == "/" && req.get_header("Accept").find("text/html") != std::string::npos)
    {
        const auto responce = new httpserver::string_response("See Other", 303, "text/plain");
        responce->with_header("Location", "ui/index.html");
        return std::shared_ptr<httpserver::string_response>(responce);
    }
    return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Not found custom", 404, "text/plain"));
}

const std::shared_ptr<httpserver::http_response> not_allowed_custom(const httpserver::http_request& req) {
    return std::shared_ptr<httpserver::string_response>(new httpserver::string_response("Not allowed custom", 405, "text/plain"));
}

void PreRollOutCallback(const char* fullPath, std::size_t s)
{
    char newPath[1024];
    char newPath2[1024];

    std::string str(fullPath);

    std::string pathNoExtension = str.substr(0, str.find_last_of('.'));

    int ver = 99;

    sprintf(newPath, "%s.log.%d", pathNoExtension.c_str(), ver);
    if(utils::FileExists(newPath)) {
        remove(newPath);
    }

    for(ver = 98; ver >= 1; ver--) {
        sprintf(newPath, "%s.log.%d", pathNoExtension.c_str(), ver);
        sprintf(newPath2, "%s.log.%d", pathNoExtension.c_str(), ver + 1);
        if(utils::FileExists(newPath)) {
            rename(newPath, newPath2);
        }
    }

    rename(fullPath, newPath);
}

int main(int argc, char** argv)
{
    std::cout << "Starting Simple IO Backend Server" << std::endl;
    START_EASYLOGGINGPP(argc, argv);

    if(utils::FileExists("loggerIoBackend.conf")) {
        // Load configuration from file
        el::Configurations conf("loggerIoBackend.conf");
        // Now all the loggers will use configuration from file and new loggers
        el::Loggers::setDefaultConfigurations(conf, true);
    }

    el::Helpers::setThreadName("MainThread");

    LOG(INFO) << "Starting";

    Config* config = new Config();

    config->Load();
    GlobalFunctions globalFunctions(config);

    const auto ownWebServer = new WebServer(&globalFunctions);

    httpserver::webserver ws = httpserver::create_webserver(config->GetServerPort())
                               .log_access(custom_access_log)
                               .log_error(custom_error_log)
                               .not_found_resource(not_found_custom)
                               .method_not_allowed_resource(not_allowed_custom)
                               .debug();

    const auto connector = new MqttConnector(config);
    if(!connector->Init()) {
        LOG(ERROR) << "Mqtt Connector starting Failed";
    } else {
        LOG(INFO) << "Mqtt Connecting";
    }

    MqttResource mqttResource(&globalFunctions);
    ws.register_resource("/mqtt/{arg1}", &mqttResource);

    HtmlPageResource htmlPageResource(&globalFunctions);
    ws.register_resource("/ui/{arg1}", &htmlPageResource);

    if(ws.start(false)) {
        LOG(INFO) << "LipHttp WebServer Running";
    }

    if(!ownWebServer->RegisterResource("/dynpage", new TestResource())){
        LOG(ERROR) << "RegisterResource failed";
    }

    if(ownWebServer->Start()) {
        LOG(INFO) << "Own WebServer Running";
    }

    std::cout << "Enter q to Stop" << std::endl;

    std::string input;
    std::cin >> input;

    while(input != "q") {
        if(input == "t") {
            ownWebServer->SendWebSocketBroadcast("Hello from Server");
        }
        std::cout << "Enter q to Stop" << std::endl;
        std::cin >> input;
    }
    
    ws.stop();

    ownWebServer->Deinit();
    delete ownWebServer;
    
    connector->Deinit();
    delete connector;

    config->Save();

    LOG(INFO) << "Last LogEntry";
    el::Helpers::uninstallPreRollOutCallback();

    return 0;
}
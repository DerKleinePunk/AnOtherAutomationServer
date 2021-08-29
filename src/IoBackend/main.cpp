#include <httpserver.hpp>
#include <iostream>
#include "../common/easylogging/easylogging++.h"
#include "../common/utils/commonutils.h"
#include "Config.hpp"
#include "Mqtt/MqttConnector.h"

INITIALIZE_EASYLOGGINGPP

void custom_access_log(const std::string& url)
{
    LOG(INFO) << "ACCESSING: " << url;
}

void custom_error_log(const std::string& url)
{
    LOG(ERROR) << "ERROR: " << url;
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

    el::Helpers::setThreadName("Main");

    Config* config = new Config();

    config->Load();

    httpserver::webserver ws = httpserver::create_webserver(config->GetServerPort())
                               .log_access(custom_access_log)
                               .log_error(custom_error_log)
                               .debug();

    const auto connector = new MqttConnector(config);
    if(!connector->Init()) {
        std::cout << "Connector starting Failed" << std::endl;
    }

    ws.start(false);

    std::cout << "Enter q to Stop" << std::endl;

    std::string input;
    std::cin >> input;

    while(input != "q") {
        std::cout << "Enter q to Stop" << std::endl;
        std::cin >> input;
    }
    
    ws.stop();
    
    connector->Deinit();
    delete connector;

    config->Save();

    LOG(INFO) << "Last LogEntry";
    el::Helpers::uninstallPreRollOutCallback();

    return 0;
}
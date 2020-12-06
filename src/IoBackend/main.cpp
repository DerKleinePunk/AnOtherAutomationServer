#include <iostream>
#include <httpserver.hpp>
#include "../common/easylogging/easylogging++.h"
#include "../common/utils/commonutils.h"
#include "Mqtt/MqttConnector.h"

INITIALIZE_EASYLOGGINGPP

void custom_access_log(const std::string& url) {
    std::cout << "ACCESSING: " << url << std::endl;
}

void custom_error_log(const std::string& url) {
    std::cout << "ERROR: " << url << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "Starting Automation IO Backend Server" << std::endl;
    START_EASYLOGGINGPP(argc, argv);

    if(utils::FileExists("loggerIoBackend.conf")) {
        // Load configuration from file
        el::Configurations conf("loggerIoBackend.conf");
        // Now all the loggers will use configuration from file and new loggers
        el::Loggers::setDefaultConfigurations(conf, true);
    }

    el::Helpers::setThreadName("Main");

    httpserver::webserver ws = httpserver::create_webserver(8080 /*config->GetServerport()*/)
        .log_access(custom_access_log)
        .log_error(custom_error_log)
        .debug();

    const auto connector = new MqttConnector();
    if(!connector->Init())
    {
        std::cout << "Connector starting Failed" << std::endl;
    }

    std::cout << "Enter q to Stop" << std::endl;

    std::string input;
    std::cin >> input;

    connector->Deinit();
    delete connector;
    
    return 0;
}
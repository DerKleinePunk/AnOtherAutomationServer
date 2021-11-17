#include <iostream>
#include "../common/easylogging/easylogging++.h"
#include "../common/utils/commonutils.h"
#include "../common/utils/CommandLineArgs.h"
#include "Config.hpp"
#include "Mqtt/MqttConnector.h"
#include "resources/TestResource.hpp"
#include "resources/ApiResource.hpp"
#include "GlobalFunctions.hpp"
#include "WebServer/WebServer.hpp"
#include "PythonRunner/PythonRunner.hpp"
#include "SystemFunktions/NetworkManager.hpp"
#include "ServiceEvents/ServiceEventManager.hpp"

INITIALIZE_EASYLOGGINGPP

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

void WriteFunktionText()
{
    std::cout << "q Exit" << std::endl;
    std::cout << "t send WebSocket Broadcast" << std::endl;
    std::cout << "r run sample.py" << std::endl;
    std::cout << "r1 run sample.py Function simpleFunc" << std::endl;
    std::cout << "s ScanAccessPoints" << std::endl;
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

    utils::CommandLineArgs commandLineArgs;
    commandLineArgs.Pharse(argc, argv);

    LOG(INFO) << "Starting";

    Config* config = new Config();
    config->Load();

    auto networkManager = new NetworkManager();
    auto eventManager = new ServiceEventManager();
    eventManager->Init();
    
    GlobalFunctions globalFunctions(config, eventManager);

    const auto ownWebServer = new WebServer(&globalFunctions);

    const auto connector = new MqttConnector(config);
    if(!connector->Init()) {
        LOG(ERROR) << "Mqtt Connector starting Failed";
    } else {
        LOG(INFO) << "Mqtt Connecting";
    }
   
    auto runner = new PythonRunner(commandLineArgs.GetBasePath() + "/SimpelIoBackend.bin");
    runner->Init();

    if(!ownWebServer->RegisterResource("/dynpage", new TestResource())){
        LOG(ERROR) << "RegisterResource failed";
    }

    if(!ownWebServer->RegisterResource("/api/*", new ApiResource(&globalFunctions))){
        LOG(ERROR) << "RegisterResource failed";
    }

    if(ownWebServer->Start()) {
        LOG(INFO) << "Own WebServer Running";
    }

    WriteFunktionText();

    std::string input;
    std::cin >> input;

    while(input != "q") {
        if(input == "t") {
            ownWebServer->SendWebSocketBroadcast("Hello from Server");
        } else if(input == "r") {
            runner->RunScript("sample");
        } else if(input == "r1") {
            runner->RunScript("sample", "simpleFunc");
        } else if(input == "s") {
            const json result = networkManager->ScanAccessPoints();
             std::cout << result.dump() << std::endl;
        } else {
            std::cout << input << " command not found" << std::endl;
        }
        WriteFunktionText();
        std::cin >> input;
    }

    eventManager->Deinit();
    delete eventManager;

    delete networkManager;

    ownWebServer->Deinit();
    delete ownWebServer;

    runner->Deinit();
    delete runner;
    
    connector->Deinit();
    delete connector;

    config->Save();

    LOG(INFO) << "Last LogEntry";
    el::Helpers::uninstallPreRollOutCallback();

    return 0;
}
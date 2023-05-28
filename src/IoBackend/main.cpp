#include <iostream>

#include "../common/easylogging/easylogging++.h"
#include "../common/utils/CommandLineArgs.h"
#include "../common/utils/commonutils.h"
#include "../common/database/DatabaseManager.h"

#include "Backend.hpp"
#include "Config.hpp"
#include "GlobalFunctions.hpp"
#include "Mqtt/MqttConnector.h"
#include "PythonRunner/PythonRunner.hpp"
#include "ServiceEvents/ServiceEventManager.hpp"
#include "SystemFunktions/GPIOManager.hpp"
#include "SystemFunktions/IODeviceManager.hpp"
#include "SystemFunktions/NetworkManager.hpp"
#include "WebServer/WebServer.hpp"
#include "resources/ApiResource.hpp"
#include "resources/AuthResource.hpp"
#include "resources/AutomationResource.hpp"
#include "resources/ConfigResource.hpp"
#include "resources/MqttResource.hpp"
#include "resources/TestResource.hpp"

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
    std::cout << "s1 Connect AccessPoint" << std::endl;
    std::cout << "t0 write mqtt test" << std::endl;
    std::cout << "i0 show internal vars" << std::endl;
    std::cout << "scan use mDns" << std::endl;
}

int main(int argc, char** argv)
{
    std::cout << "Starting Simple IO Backend Server" << std::endl;
    START_EASYLOGGINGPP(argc, argv);

    utils::CommandLineArgs commandLineArgs;
    commandLineArgs.Pharse(argc, argv);

    LOG(INFO) << "App Basepath is " << commandLineArgs.GetBasePath();

    if(utils::FileExists(commandLineArgs.GetBasePath() + "/loggerIoBackend.conf")) {
        // Load configuration from file
        el::Configurations conf(commandLineArgs.GetBasePath() + "/loggerIoBackend.conf");
        // Now all the loggers will use configuration from file and new loggers
        el::Loggers::setDefaultConfigurations(conf, true);
    }

    el::Helpers::setThreadName("MainThread");
    el::Helpers::installPreRollOutCallback(PreRollOutCallback);

    LOG(INFO) << "Starting";

    Config* config = new Config();
    config->Load();

    auto networkManager = new NetworkManager();
    LOG(INFO) << "NetworkManager started";

    auto eventManager = new ServiceEventManager();
    eventManager->Init();
    LOG(INFO) << "ServiceEventManager started";

    auto databaseManger = new DatabaseManager(commandLineArgs.GetBasePath() + "/");
    
    GlobalFunctions globalFunctions(config, eventManager, networkManager, databaseManger);

    const auto ownWebServer = new WebServer(&globalFunctions);

    const auto connector = new MqttConnector(config, eventManager);
    if(!connector->Init()) {
        LOG(ERROR) << "Mqtt Connector starting Failed";
    } else {
        LOG(INFO) << "Mqtt Connecting";
    }

    auto runner = new PythonRunner(commandLineArgs.GetBasePath() + "/SimpelIoBackend.bin", &globalFunctions);
    runner->Init();

    auto gpioManager = new GPIOManager(config, &globalFunctions);
    if(gpioManager->Init()) {
        LOG(INFO) << "gpio manager started";
    } else {
        LOG(ERROR) << "gpio manager starting Failed";
    }

    auto ioDeviceManager = new IODeviceManager(config, &globalFunctions);
    if(ioDeviceManager->Init()) {
        LOG(INFO) << "io device manager started";
    } else {
        LOG(ERROR) << "io device manager starting Failed";
    }

    if(!ownWebServer->RegisterResource("/dynpage", new TestResource())) {
        LOG(ERROR) << "RegisterResource failed";
    }

    if(!ownWebServer->RegisterResource("/api/mqtt/*", new MqttResource(&globalFunctions))) {
        LOG(ERROR) << "RegisterResource failed";
    }

    if(!ownWebServer->RegisterResource("/api/auth/*", new AuthResource(&globalFunctions))) {
        LOG(ERROR) << "RegisterResource failed";
    }

    if(!ownWebServer->RegisterResource("/api/config/*", new ConfigResource(&globalFunctions))) {
        LOG(ERROR) << "RegisterResource failed";
    }

    if(!ownWebServer->RegisterResource("/api/automation/*", new AutomationResource(&globalFunctions))) {
        LOG(ERROR) << "RegisterResource failed";
    }

    if(!ownWebServer->RegisterResource("/api/system/*", new ApiResource(&globalFunctions))) {
        LOG(ERROR) << "RegisterResource failed";
    }


    if(ownWebServer->Start()) {
        LOG(INFO) << "Own WebServer Running";
    }

    auto backend = new Backend(ownWebServer, eventManager, config, runner);

    databaseManger->Init();
    backend->Init();

    WriteFunktionText();

    std::string input;
    std::cin >> input;

    try {
        while(input != "q") {
            if(input == "t") {
                ownWebServer->SendWebSocketBroadcast("Hello from Server");
            } else if(input == "r") {
                runner->RunScript("sample");
            } else if(input == "r1") {
                runner->RunScript("sample", "simpleFunc");
            } else if(input == "s") {
                const auto result = networkManager->ScanAccessPoints();
                for(const auto device : result) {
                    std::cout << device.Iface << std::endl;
                    for(const auto apInfo : device.AccessPoints) {
                        std::cout << "     " << apInfo.Ssid << std::endl;
                    }
                }
                json jsonText = result;
                std::cout << jsonText.dump() << std::endl;
            } else if(input == "s1") {
                if(networkManager->ConnectAccessPoint("ROBONETZ", "1234")) {
                    LOG(INFO) << "WLAN ROBONETZ connected";
                } else {
                    LOG(ERROR) << "WLAN ROBONETZ not connected";
                }
            } else if(input == "t0") {
                connector->Publish("cmnd/tasmota_852612/POWER", "TOGGLE");
            } else if(input == "i0") {
                globalFunctions.ShowInternalVars();
            } else if(input == "scan") {
                globalFunctions.ScanDevices("_http._tcp");
                globalFunctions.ScanDevices("_esphomelib");
            } else {
                std::cout << input << " command not found" << std::endl;
            }
            WriteFunktionText();
            std::cin >> input;
        }
    } catch(const std::exception& e) {
        LOG(ERROR) << e.what();
    }

    delete backend;

    gpioManager->Deinit();
    delete gpioManager;

    eventManager->Deinit();
    delete eventManager;

    ownWebServer->Deinit();
    delete ownWebServer;

    runner->Deinit();
    delete runner;

    connector->Deinit();
    delete connector;

    config->Save();
    
    databaseManger->Deinit();
    delete databaseManger;

    delete networkManager;
    
    LOG(INFO) << "Last LogEntry";
    el::Helpers::uninstallPreRollOutCallback();

    return 0;
}
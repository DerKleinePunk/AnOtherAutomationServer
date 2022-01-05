#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "IODeviceManager"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../common/easylogging/easylogging++.h"
#include "IODeviceManager.hpp"
#include "../../common/json/json.hpp"

using json = nlohmann::json;

void IODeviceManager::EventCallback(const SystemEvent event, const std::string& parameter)
{
    LOG(DEBUG) << "EventCallback " << event << " with " << parameter;

    if(event == SystemEvent::Idle) {
        UpdateSystemVars();
    }
}

void IODeviceManager::UpdateSystemVars()
{
    std::string val;
    std::string preparedTemp;
    //double temperature;

    std::ifstream temperatureFile ("/sys/class/thermal/thermal_zone0/temp");

    if (!temperatureFile.is_open()) {
        LOG(ERROR) << "Failed to open temperatureFile!";
    } else {
        // The temperature is stored in 5 digits.  The first two are degrees in C.  The rest are decimal precision.
        temperatureFile >> val;

        temperatureFile.close();

        preparedTemp = val.insert(2, 1, '.');
        preparedTemp = preparedTemp.substr(0, 5); //Zwei stellen nach dem Comma reichen völlig
        //temperature = std::stod(preparedTemp);

        if(preparedTemp != _lastTempText) {
            _globalFunctions->SetInternalVariable("CPUTEMP", preparedTemp);
            _lastTempText = preparedTemp;
        }
    }
}

IODeviceManager::IODeviceManager(Config* config,GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _config = config;
    _globalFunctions = globalFunctions;
    _mneaReader = nullptr;
}

IODeviceManager::~IODeviceManager()
{
}

bool IODeviceManager::Init()
{
    for(auto resource : _config->GetResources()) {
        if(resource->Type == ResourceType::GPSMouse) {
            auto gpsMouse = reinterpret_cast<GPSMouseResource*>(resource);
            if(_mneaReader == nullptr) {
                if(gpsMouse->ComPort.size() >0 && gpsMouse->ComPort != "none") {
                    _mneaReader = new NMEAReader(gpsMouse->ComPort);
                    if(_mneaReader->Init("4800") != 0) {
                        LOG(ERROR) << "_mneaReader Failed";
                    }
                }
            } else {
                LOG(WARNING) << "Only One NEMA Device Supported";
            }
        }
    }

    auto callback = std::bind(&IODeviceManager::EventCallback, this, std::placeholders::_1, std::placeholders::_2);
    std::vector<SystemEvent> myEvents;
    myEvents.push_back(SystemEvent::ChangeValue);
    myEvents.push_back(SystemEvent::Idle);
    _globalFunctions->RegisterForEvent(myEvents, callback);

    if(_mneaReader != nullptr) {
        
    }

    LOG(DEBUG) << "Init <- success";

    return true;
}
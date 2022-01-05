#pragma once

#include "../Config.hpp"
#include "../GlobalFunctions.hpp"
#include "../Protocols/NMEA/NMEAReader.hpp"

class IODeviceManager
{
private:
    Config* _config;
    GlobalFunctions* _globalFunctions;
    std::string _lastTempText;
    NMEAReader* _mneaReader;

    void EventCallback(const SystemEvent event, const std::string& parameter);
    void UpdateSystemVars();
public:
    IODeviceManager(Config* config,GlobalFunctions* globalFunctions);
    IODeviceManager(const IODeviceManager& orig) = delete;
	IODeviceManager(IODeviceManager&& other) = delete;
	IODeviceManager& operator=(const IODeviceManager& other) = delete;
	IODeviceManager& operator=(IODeviceManager&& other) = delete;
    ~IODeviceManager();

    bool Init();
};

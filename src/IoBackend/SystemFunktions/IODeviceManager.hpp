#pragma once

#include "../Config.hpp"
#include "../GlobalFunctions.hpp"

class IODeviceManager
{
private:
    GlobalFunctions* _globalFunctions;
public:
    IODeviceManager(GlobalFunctions* globalFunctions);
    IODeviceManager(const IODeviceManager& orig) = delete;
	IODeviceManager(IODeviceManager&& other) = delete;
	IODeviceManager& operator=(const IODeviceManager& other) = delete;
	IODeviceManager& operator=(IODeviceManager&& other) = delete;
    ~IODeviceManager();

    bool Init();
};

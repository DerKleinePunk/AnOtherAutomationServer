#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "GPIOManager"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../common/easylogging/easylogging++.h"
#include "GPIOManager.hpp"

/**
 * @brief Construct a new GPIOManager::GPIOManager object
 * 
 * @param config 
 */
GPIOManager::GPIOManager(Config* config)
{
    _config = config;
    _i2cBus = nullptr;
}

GPIOManager::~GPIOManager()
{
}

/**
 * @brief Init the GPIO Manager
 * 
 * @return true 
 * @return false 
 */
bool GPIOManager::Init()
{
    auto useI2C = false;

    for(auto resource : _config->GetResources()) {
        if(resource->Type == ResourceType::MCP23017) {
            _mcp23017.push_back(reinterpret_cast<MCP23017Resource*>(resource));
            useI2C = true;
        } else if(resource->Type == ResourceType::GPIOPin) {
            _gpioPins.push_back(reinterpret_cast<GPIOPinResource*>(resource));
        }
    }

    //Todo Check I²C Pins are Free (GPIOPin Config)
    if(useI2C) {
        try
        {
           _i2cBus = new I2CBus("/dev/i2c-1");
        }
        catch(const std::exception& e)
        {
            LOG(ERROR) << e.what();
        }
    }

    return false;
}

void GPIOManager::Deinit()
{
    if(_i2cBus != nullptr) {
        delete _i2cBus;
    }
}
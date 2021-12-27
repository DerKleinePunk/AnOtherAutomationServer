#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "GPIOManager"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../common/easylogging/easylogging++.h"
#include "GPIOManager.hpp"

LocalGpioPin::LocalGpioPin(std::uint8_t port, bool output, const std::string& mqttName):
    GpioPin(port, output)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _mqttName = mqttName;
    _isEnablePin = false;
}

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
            auto mcp = reinterpret_cast<MCP23017Resource*>(resource);
            _mcp23017.push_back(mcp);
            useI2C = true;
            if(mcp->UseEnable) {
                try
                {
                    auto enablePin = new LocalGpioPin(mcp->EnablePin, true, std::string ("none"));
                    enablePin->_isEnablePin = true;
                    _gpioPins.push_back(enablePin);
                }
                catch(const std::exception& e)
                {
                    LOG(ERROR) << e.what();
                    return false;
                }
            }
        } else if(resource->Type == ResourceType::GPIOPin) {
            auto currentPin = reinterpret_cast<GPIOPinResource*>(resource);
            try
            {
                auto enablePin = new LocalGpioPin(currentPin->Address, currentPin->Output, currentPin->MqttBaseName);
                _gpioPins.push_back(enablePin);
            }
            catch(const std::exception& e)
            {
                LOG(ERROR) << e.what();
                return false;
            }
        }
    }

    for(auto pin : _gpioPins ) {
        if(pin->GetDirection() == pin_direction::out) {
            *pin << pin_value::off;
        }
    }   
    
    //Todo Check I²C Pins are Free (GPIOPin Config)
    if(useI2C) {
        LOG(DEBUG) << "Starting I²C Bus";

        try
        {
           _i2cBus = new I2CBus("/dev/i2c-1");
        }
        catch(const std::exception& e)
        {
            LOG(ERROR) << e.what();
            return false;
        }
    }

    for(auto pin : _gpioPins ) {
        if(pin->_isEnablePin) {
            *pin << pin_value::on;
        }
    }

    return true;
}

void GPIOManager::Deinit()
{
    for(auto pin : _gpioPins ) {
        if(pin->GetDirection() == pin_direction::out) {
            *pin << pin_value::off;
        }
        delete pin;
    }

    _gpioPins.clear();

    if(_i2cBus != nullptr) {
        delete _i2cBus;
    }
}
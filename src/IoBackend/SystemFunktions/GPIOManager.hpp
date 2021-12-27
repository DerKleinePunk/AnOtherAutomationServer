#pragma once

#include "../Config.hpp"
#include "../../../modules/PiGPIOHelper/src/GPIOHelper/GpioPin.hpp"
#include "../../../modules/PiGPIOHelper/src/GPIOHelper/I2CBus.hpp"
#include "../../../modules/PiGPIOHelper/src/GPIOHelper/MCP23017.hpp"
#include "../ServiceEvents/ServiceEventManager.hpp"

class LocalGpioPin : public GpioPin 
{
private:
    std::string _mqttName;

public:
    LocalGpioPin(std::uint8_t port, bool output, const std::string& mqttName);
    bool _isEnablePin;
};

class GPIOManager
{
private:
    Config* _config;
    I2CBus* _i2cBus;
    std::vector<MCP23017Resource*> _mcp23017;
    std::vector<LocalGpioPin*> _gpioPins;
    std::vector<MCP23017*> _mcp23017Impl;
    ServiceEventManager* _serviceEventManager;

    void EventCallback(const std::string& name, const std::string& parameter);
public:
    GPIOManager(Config* config, ServiceEventManager* serviceEventManager);
    ~GPIOManager();

    bool Init();
    void Deinit();
};



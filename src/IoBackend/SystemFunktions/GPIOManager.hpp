#pragma once

#include "../Config.hpp"
#include "../../../modules/PiGPIOHelper/src/GPIOHelper/GpioPin.hpp"
#include "../../../modules/PiGPIOHelper/src/GPIOHelper/I2CBus.hpp"
#include "../../../modules/PiGPIOHelper/src/GPIOHelper/MCP23017.hpp"
#include "../GlobalFunctions.hpp"

class LocalGpioPin : public GpioPin 
{
private:
    std::string _varName;

public:
    LocalGpioPin(std::uint8_t port, bool output, const std::string& varName);
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
    GlobalFunctions* _globalFunctions;

    void EventCallback(const SystemEvent event, const std::string& parameter);
public:
    GPIOManager(Config* config, GlobalFunctions* globalFunctions);
    GPIOManager(const GPIOManager& orig) = delete;
	GPIOManager(GPIOManager&& other) = delete;
	GPIOManager& operator=(const GPIOManager& other) = delete;
	GPIOManager& operator=(GPIOManager&& other) = delete;
    ~GPIOManager();

    bool Init();
    void Deinit();
};



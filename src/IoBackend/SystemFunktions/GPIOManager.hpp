#pragma once

#include "../Config.hpp"
#include "../../../modules/PiGPIOHelper/src/GPIOHelper/I2CBus.hpp"

class GPIOManager
{
private:
    Config* _config;
    I2CBus* _i2cBus;
    std::vector<MCP23017Resource*> _mcp23017;
    std::vector<GPIOPinResource*> _gpioPins;
public:
    GPIOManager(Config* config);
    ~GPIOManager();

    bool Init();
    void Deinit();
};



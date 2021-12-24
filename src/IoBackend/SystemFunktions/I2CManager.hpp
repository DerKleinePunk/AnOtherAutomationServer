#pragma once

#include "../../../modules/PiGPIOHelper/src/GPIOHelper/I2CBus.hpp"

class I2CManager
{
private:
    I2CBus* _i2cBus;
public:
    I2CManager(/* args */);
    ~I2CManager();

    bool Init();
    void Deinit();
};



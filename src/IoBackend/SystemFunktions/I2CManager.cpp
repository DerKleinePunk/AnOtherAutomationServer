#include "I2CManager.hpp"

I2CManager::I2CManager(/* args */)
{
    _i2cBus = nullptr;
}

I2CManager::~I2CManager()
{
}

bool I2CManager::Init()
{
    _i2cBus = new I2CBus("/dev/i2c-1");

    return false;
}

void I2CManager::Deinit()
{
    if(_i2cBus != nullptr) {
        delete _i2cBus;
    }
}
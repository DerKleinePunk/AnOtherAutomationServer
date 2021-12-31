#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "GPIOManager"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../common/easylogging/easylogging++.h"
#include "GPIOManager.hpp"
#include "../../common/json/json.hpp"

using json = nlohmann::json;

LocalGpioPin::LocalGpioPin(std::uint8_t port, bool output, const std::string& varName):
    GpioPin(port, output)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _varName = varName;
    _isEnablePin = false;
}

void GPIOManager::EventCallback(const std::string& name, const std::string& parameter)
{
    if(name == "ChangeValue") {
        auto jsonText = json::parse(parameter);

        std::string name("");
        std::string value("");

        auto it_value = jsonText.find("name");
        if(it_value != jsonText.end()) {
            name = jsonText.at("name").get<std::string>();
        }

        it_value = jsonText.find("value");
        if(it_value != jsonText.end()) {
            value = jsonText.at("value").get<std::string>();
        }

        if(name.empty() || value.empty()) {
            LOG(ERROR) << "Missing data";

            return;
        }

        for(auto mcp : _mcp23017) {
            LOG(DEBUG) << "Check " << std::to_string(mcp->Address);
            for(int i = 0; i < 16; i++) {
                if(name == mcp->VarName[i]) {
                    if(value == "on") {
                        _mcp23017Impl[i]->SetPin(0, pin_value::on);
                        _globalFunctions->SetInternalVariable(name, "on");
                    } else if(value == "off") {
                        _mcp23017Impl[i]->SetPin(0, pin_value::off);
                        _globalFunctions->SetInternalVariable(name, "off");
                    } else {
                        LOG(WARNING) << value << " is not know want to do";
                    }
                }
            }
        }
    }
}

/**
 * @brief Construct a new GPIOManager::GPIOManager object
 * 
 * @param config 
 * 
 * @param globalFunctions
 */
GPIOManager::GPIOManager(Config* config, GlobalFunctions* globalFunctions)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _config = config;
    _globalFunctions = globalFunctions;
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
    LOG(DEBUG) << "Init ->";

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
                auto enablePin = new LocalGpioPin(currentPin->Address, currentPin->Output, currentPin->VarName);
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

    for(auto mcp : _mcp23017) {
        auto mcpImpl = new MCP23017(_i2cBus, mcp->Address);
        _mcp23017Impl.push_back(mcpImpl);
        uint8_t pos = 0;
        for(auto zeichen : mcp->OutputMap) {
            if(zeichen == '1') {
                mcpImpl->ConfigPin(pos, pin_direction::out);
            } else {
                mcpImpl->ConfigPin(pos, pin_direction::in);
            }
            pos++;
        }
    }

    auto callback = std::bind(&GPIOManager::EventCallback, this, std::placeholders::_1, std::placeholders::_2);
    _globalFunctions->RegisterForEvent(std::string("ChangeValue"), callback);

    LOG(DEBUG) << "Init <- success";
    return true;
}

void GPIOManager::Deinit()
{
    //Do not delete the Config Pointer
    /*for(auto mcp : _mcp23017) {
        delete mcp;
    }*/
    _mcp23017.clear();

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
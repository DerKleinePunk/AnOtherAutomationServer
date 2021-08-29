#pragma once
#include <mosquitto.h>
#include "../Config.hpp"

class MqttConnector
{
private:
    mosquitto* _mosq;
    const Config* _config;
public:
    MqttConnector(const Config* config);
    ~MqttConnector();

    bool Init();
    void Deinit();
};


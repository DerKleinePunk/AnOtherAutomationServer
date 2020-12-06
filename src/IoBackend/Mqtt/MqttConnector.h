#pragma once
#include <mosquitto.h>

class MqttConnector
{
private:
    mosquitto* _mosq;
public:
    MqttConnector();
    ~MqttConnector();

    bool Init();
    void Deinit();
};


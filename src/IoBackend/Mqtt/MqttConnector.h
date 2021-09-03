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
    void Connected();
    void OnMessage(const struct mosquitto_message *message);
};


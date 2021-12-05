#pragma once
#include <string>

#include <mosquitto.h>
#include "../Config.hpp"
#include "../ServiceEvents/ServiceEventManager.hpp"

class MqttConnector
{
private:
    mosquitto* _mosq;
    const Config* _config;
    bool _initOk;
    ServiceEventManager* _serviceEventManager;
    std::vector<std::string> _topics;

    void LogError(int errorCode);
    void EventCallback(const std::string& name, const std::string& parameter);
    void HandleMqttInternals(const std::string& name, const std::string& value);
public:
    MqttConnector(const Config* config, ServiceEventManager* serviceEventManager);
    ~MqttConnector();

    bool Init();
    void Deinit();
    void Connected();
    bool Publish(std::string topic, std::string value);

    //Internal Use Only
    void OnMessage(const struct mosquitto_message *message);
    void OnPublishWasSend(int mid);
   
};


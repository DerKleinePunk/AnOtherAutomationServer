#pragma once
#include <string>

enum class SystemEvent {
    MqttValue,
    ChangeValue,
    ValueChanged,
    Idle,
    PublishMqtt
};

std::ostream& operator<<(std::ostream& os, const SystemEvent type);
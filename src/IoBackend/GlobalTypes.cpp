#include "../common/json/json.hpp"
using json = nlohmann::json;
#include "GlobalTypes.hpp"

std::ostream& operator<<(std::ostream& os, const SystemEvent type)
{
    switch(type) {
        case SystemEvent::MqttValue:
            os << "MqttValue";
            break;
        case SystemEvent::ChangeValue:
            os << "ChangeValue";
            break;
        case SystemEvent::ValueChanged:
            os << "ValueChanged";
            break;
        case SystemEvent::Idle:
            os << "Idle";
            break;
        case SystemEvent::PublishMqtt:
            os << "PublishMqtt";
            break;
        case SystemEvent::Action:
            os << "Action";
            break;
        default:
            os.setstate(std::ios_base::failbit);
    }
    return os;
}
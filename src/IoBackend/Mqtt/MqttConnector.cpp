#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "MqttConnector"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "MqttConnector.h"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/json/json.hpp"

using json = nlohmann::json;

// https://www.systutorials.com/docs/linux/man/3-libmosquitto/
// https://stackoverflow.com/questions/19057835/how-to-find-connected-mqtt-client-details

static int mid_sent = 0;
static int qos = 0;
static int retain = 0;

void my_log_callback(struct mosquitto* mosq, void* userdata, int level, const char* str)
{
    if(level == MOSQ_LOG_WARNING) {
        LOG(WARNING) << str;
    } else if(level == MOSQ_LOG_ERR) {
        LOG(ERROR) << str;
    } else {
        LOG(DEBUG) << str;
    }
}

void my_connect_callback(struct mosquitto* mosq, void* userdata, int result)
{
    if(!result) {
        /* Subscribe to broker information topics on successful connect. */
        if(mosquitto_subscribe(mosq, NULL, "$SYS/#", 2) != MOSQ_ERR_SUCCESS) {
            LOG(ERROR) << "subscribe sys failed failed";
        } else {
            const auto connector = (MqttConnector*)userdata;
            connector->Connected();
            /*if(mosquitto_subscribe(mosq, NULL, "clients/#", 2) != MOSQ_ERR_SUCCESS) {
                LOG(ERROR) << "subscribe clients failed failed";
            } This is not Working Finde other way*/
           
        }

    } else {
        LOG(ERROR) << "Connect failed";
    }
}

void my_message_callback(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message)
{
    el::Helpers::setThreadName("MqttConnector WorkerThread");
    const auto connector = (MqttConnector*)userdata;
    connector->OnMessage(message);
}

void my_publish_callback(struct mosquitto* mosq, void* userdata, int mid)
{
    const auto connector = (MqttConnector*)userdata;
    connector->OnPublishWasSend(mid);
}

void MqttConnector::LogError(int errorCode)
{
    LOG(ERROR) << mosquitto_strerror(errorCode);
}

void MqttConnector::EventCallback(const std::string& name, const std::string& parameter)
{
    auto jsonText = json::parse(parameter);

    std::string topic("");
    std::string value("");

    auto it_value = jsonText.find("topic");
    if(it_value != jsonText.end()) {
        topic = jsonText.at("topic").get<std::string>();
    }

    it_value = jsonText.find("value");
    if(it_value != jsonText.end()) {
        value = jsonText.at("value").get<std::string>();
    }

    if(topic.empty() || value.empty()) {
        LOG(ERROR) << "Missing data";

        return;
    }

    Publish(topic, value);

}

MqttConnector::MqttConnector(const Config* config, ServiceEventManager* serviceEventManager) : _mosq(nullptr)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _config = config;
    _initOk = false;
    _serviceEventManager = serviceEventManager;
}

MqttConnector::~MqttConnector()
{
}

bool MqttConnector::Init()
{
    const auto host = _config->GetMqttServer();
    int port = 1883;
    int keepalive = 60;
    bool clean_session = true;

    mosquitto_lib_init();

    int major, minor, revision = 0;
    mosquitto_lib_version(&major, &minor, &revision);
    LOG(DEBUG) << "MOSQUITTO LIB Version " << major << "." << minor << "." << revision;

    _mosq = mosquitto_new("MiNeSimpleIoBackend", clean_session, this);
    if(_mosq == nullptr) {
        LOG(ERROR) << "mosquitto_new return null";
        return false;
    }

    mosquitto_log_callback_set(_mosq, my_log_callback);
    mosquitto_connect_callback_set(_mosq, my_connect_callback);
    mosquitto_message_callback_set(_mosq, my_message_callback);
    mosquitto_publish_callback_set(_mosq, my_publish_callback);

    auto result = mosquitto_connect(_mosq, host.c_str(), port, keepalive);
    if(result != MOSQ_ERR_SUCCESS) {
        LOG(ERROR) << "Unable to connect ErrorCode " << std::to_string(result);
        return false;
    }

    result = mosquitto_loop_start(_mosq);
    if(result != MOSQ_ERR_SUCCESS) {
        LOG(ERROR) << "Unable to start loop ErrorCode " << std::to_string(result);
        return false;
    }

    return true;
}

void MqttConnector::Deinit()
{
    _initOk = false;

    if(_mosq == nullptr) return;

    auto result = mosquitto_disconnect(_mosq);
    if(result != MOSQ_ERR_SUCCESS) {
        LOG(ERROR) << "Unable to disconnect ErrorCode " << std::to_string(result);
    }

    result = mosquitto_loop_stop(_mosq, false);
    if(result != MOSQ_ERR_SUCCESS) {
        LOG(ERROR) << "Unable to stop loop " << std::to_string(result);
        LogError(result);
    }

    mosquitto_destroy(_mosq);
    mosquitto_lib_cleanup();
}

void MqttConnector::Connected()
{
    LOG(INFO) << "Mqtt Conneted";
    _topics = _config->GetWatchTopics();
    for( const auto &topic : _topics)
    {
        if(mosquitto_subscribe(_mosq, NULL, topic.c_str(), 2) != MOSQ_ERR_SUCCESS) {
            LOG(ERROR) << topic << " subscribe failed";
        } else {
            LOG(DEBUG) << topic << " subscribed";
        }
    }

    _initOk = true;

    auto callback = std::bind(&MqttConnector::EventCallback, this, std::placeholders::_1, std::placeholders::_2);
    _serviceEventManager->RegisterMe(std::string("PublishMqtt"), callback);
}

bool MqttConnector::Publish(std::string topic, std::string value)
{
    if(!_initOk) {
        return false;
    }

    auto rc = mosquitto_publish(_mosq, &mid_sent, topic.c_str(), value.size(), value.c_str(), qos, retain);
    if(rc != MOSQ_ERR_SUCCESS) {
        LogError(rc);
        return false;
    }

    LOG(DEBUG) << "Send with Message Id " << mid_sent;

    return true;
}

void MqttConnector::OnMessage(const struct mosquitto_message* message)
{
    bool match = 0;
    printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*)message->payload, message->topic);

    mosquitto_topic_matches_sub("/devices/wb-adc/controls/+", message->topic, &match);
    if(match) {
        printf("got message for ADC topic\n");
    }

    for( const auto &topic : _topics) {
        mosquitto_topic_matches_sub(topic.c_str(), message->topic, &match);
        if(match) {
            json j;
            j["topic"] = message->topic;
            j["value"] = (char*)message->payload;
            _serviceEventManager->FireNewEvent("MqttValue", j.dump());
        }
    }
}

void MqttConnector::OnPublishWasSend(int mid)
{
    LOG(DEBUG) << "Sended with Message Id " << mid;
}
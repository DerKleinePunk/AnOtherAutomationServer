#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "MqttConnector"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "MqttConnector.h"
#include "../../common/easylogging/easylogging++.h"

// https://www.systutorials.com/docs/linux/man/3-libmosquitto/

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

MqttConnector::MqttConnector(const Config* config) : _mosq(nullptr)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _config = config;
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
    if(_mosq == nullptr) return;

    auto result = mosquitto_disconnect(_mosq);
    if(result != MOSQ_ERR_SUCCESS) {
        LOG(ERROR) << "Unable to disconnect ErrorCode " << std::to_string(result);
    }

    result = mosquitto_loop_stop(_mosq, false);
    if(result != MOSQ_ERR_SUCCESS) {
        LOG(ERROR) << "Unable to stop loop " << std::to_string(result);
    }

    mosquitto_destroy(_mosq);
    mosquitto_lib_cleanup();
}

void MqttConnector::Connected()
{
    LOG(INFO) << "Mqtt Conneted";
    const auto topics = _config->GetWatchTopics();
    for( const auto &topic : topics)
    {
        if(mosquitto_subscribe(_mosq, NULL, topic.c_str(), 2) != MOSQ_ERR_SUCCESS) {
            LOG(ERROR) << topic << " subscribe failed";
        } else {
            LOG(DEBUG) << topic << " subscribed";
        }
    }
}

void MqttConnector::OnMessage(const struct mosquitto_message* message)
{
    bool match = 0;
    printf("got message '%.*s' for topic '%s'\n", message->payloadlen, (char*)message->payload, message->topic);

    mosquitto_topic_matches_sub("/devices/wb-adc/controls/+", message->topic, &match);
    if(match) {
        printf("got message for ADC topic\n");
    }
}
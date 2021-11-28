#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "ServiceEventManager"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "ServiceEventManager.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"

void ServiceEventManager::WorkerLoop()
{
    el::Helpers::setThreadName("ServiceEventManager WorkerThread");

    while (_run)
    {
        InternalEvent event;
        if(_eventQueue.tryRemove(event, std::chrono::milliseconds(1000))) {
            for( auto consumer : _eventConsumer) {
                if(consumer.EventFilter.empty() || utils::hasBegining(event.Name, consumer.EventFilter)) {
                    LOG(DEBUG) << "Fire Event " << event.Name << " " << event.Parameter;
                    consumer.EventConsumer(event.Name, event.Parameter);
                }
            }
        } else {
            LOG(DEBUG) << "Event Idle";
        }
    }
    
}

ServiceEventManager::ServiceEventManager(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _run = true;
}

ServiceEventManager::~ServiceEventManager()
{
}

bool ServiceEventManager::Init()
{
    _eventConsumerThread = std::thread(&ServiceEventManager::WorkerLoop, this);

    return true;
}
    
void ServiceEventManager::Deinit()
{
    _run = false;

    if(_eventConsumerThread.joinable()) {
        _eventConsumerThread.join();
    }
}

/**
 * @brief Fire New System Event
 * 
 * @param name 
 * @param parameter 
 */
void ServiceEventManager::FireNewEvent(const std::string& name, const std::string& parameter)
{
    if(_eventConsumer.size() == 0) {
        LOG(WARNING) << "No Consumers no Event";
        return;
    }

    InternalEvent event;
    event.Name = name;
    event.Parameter = parameter;

    _eventQueue.add(event);
}

/**
 * @brief Register Callback for System Events
 * 
 * @param eventFilter the filer string
 * @param function the callback function
 */
void ServiceEventManager::RegisterMe(const std::string& eventFilter, EventDelegate function)
{
    EventConsumers consumer;
    consumer.EventFilter = eventFilter;
    consumer.EventConsumer = function;

    _eventConsumer.push_back(consumer);
}
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
                if(consumer.EventFilter.size() == 0 || 
                    std::find(consumer.EventFilter.begin(), consumer.EventFilter.end(), event.Event) != consumer.EventFilter.end()) {
                    LOG(DEBUG) << "send to " << consumer.Name << " Event " << event.Event << " " << event.Parameter;
                    consumer.EventConsumer(event.Event, event.Parameter);
                } else {
                    LOG(DEBUG) << "EventFiltered for " << consumer.Name;
                }
            }
        } else {
            LOG(DEBUG) << "Event Idle";
            if(_eventConsumer.size() > 0) {
                for( auto consumer : _eventConsumer) {
                    if(consumer.EventFilter.size() == 0 || 
                        std::find(consumer.EventFilter.begin(), consumer.EventFilter.end(), SystemEvent::Idle) != consumer.EventFilter.end()) {
                        consumer.EventConsumer(SystemEvent::Idle, "");
                    } else {
                        LOG(DEBUG) << "EventFiltered for " << consumer.Name;
                    }
                }
            }
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
void ServiceEventManager::FireNewEvent(const SystemEvent event, const std::string& parameter)
{
    if(_eventConsumer.size() == 0) {
        LOG(WARNING) << "No Consumers no event fired";
        return;
    }

    InternalEvent intEvent;
    intEvent.Event = event;
    intEvent.Parameter = parameter;

    _eventQueue.add(intEvent);
}

/**
 * @brief Register Callback for System Events
 * 
 * @param eventFilter array of the Filter for EvemtTypes
 * @param function the callback function
 * @param Name the Name Consumer (Loging)
 */
void ServiceEventManager::RegisterMe(const std::vector<SystemEvent>& eventFilter, EventDelegate function, const std::string& name)
{
    EventConsumers consumer;
    consumer.EventFilter = eventFilter;
    consumer.EventConsumer = function;
    consumer.Name = name;

    _eventConsumer.push_back(consumer);
}

/**
 * @brief Register Callback for System Events
 * 
 * @param eventFilter the Filter for EvemtTypes
 * @param function the callback function
 * @param Name the Name Consumer (Loging)
 */
void ServiceEventManager::RegisterMe(const SystemEvent eventFilter, EventDelegate function, const std::string& name)
{
    std::vector<SystemEvent> myEvents;
    myEvents.push_back(eventFilter);

    RegisterMe(myEvents, function, name);
}
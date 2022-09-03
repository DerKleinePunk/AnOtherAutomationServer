#pragma once
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include "../../common/utils/waitingqueue.h"
#include "../GlobalTypes.hpp"

typedef std::function<void(const SystemEvent event, const std::string& parameter)> EventDelegate;

struct EventConsumers
{
    std::vector<SystemEvent> EventFilter;
    EventDelegate EventConsumer;
    std::string Name;
};

struct InternalEvent
{
    SystemEvent Event;
    std::string Parameter;
};

class ServiceEventManager
{
private:
    std::vector<EventConsumers> _eventConsumer;
    WaitingQueue<InternalEvent> _eventQueue;
    std::thread _eventConsumerThread;
    bool _run;
    
    void WorkerLoop();
public:
    ServiceEventManager(/* args */);
    ~ServiceEventManager();

    bool Init();
    void Deinit();

    void FireNewEvent(const SystemEvent event, const std::string& parameter);
    void RegisterMe(const std::vector<SystemEvent>& eventFilter, EventDelegate function, const std::string& name);
    void RegisterMe(const SystemEvent eventFilter, EventDelegate function, const std::string& name);
};

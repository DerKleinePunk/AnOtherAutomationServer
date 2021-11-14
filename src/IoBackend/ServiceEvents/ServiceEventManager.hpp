#pragma once
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include "../../common/utils/waitingqueue.h"

typedef std::function<void(const std::string& name, const std::string& parameter)> EventCallbackFunction;

struct EventConsumers
{
    std::string EventFilter;
    EventCallbackFunction EventConsumer;
};

struct InternalEvent
{
    std::string Name;
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

    void FireNewEvent(const std::string& name, const std::string& parameter);
    void RegisterMe(const std::string& eventFilter, EventCallbackFunction function);
};

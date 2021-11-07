#pragma once
#include <string>

class ServiceEventManager
{
private:
    /* data */
public:
    ServiceEventManager(/* args */);
    ~ServiceEventManager();

    bool Init();
    void Deinit();

    void FireNewEvent(const std::string& name, const std::string& parameter);
};

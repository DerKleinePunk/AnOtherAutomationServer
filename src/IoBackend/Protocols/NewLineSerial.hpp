#pragma once
#include <functional>
#include "SerialBase.hpp"

typedef std::function<void(const std::string& Message)> NewDataCallbackFunction;

class NewLineSerial : public SerialBase
{
    NewDataCallbackFunction _callbackFunction;
    void AnalyseBuffer(int count) override;

  public:
    NewLineSerial(const std::string& portname);
    ~NewLineSerial();

    int Start(NewDataCallbackFunction callbackFunction);
};
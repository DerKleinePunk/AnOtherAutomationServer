#pragma once

#include <string>
#include "PythonConnector.hpp"
#include "../GlobalFunctions.hpp"

class PythonRunner
{
private:
    PythonConnector* _connector;
    std::string _appName;
    GlobalFunctions* _globalFunctions;
    void PrintTotalRefCount();
    void WriteMqtt(const std::string& topic, const std::string& value);
    void ChangeValue(const std::string& name, const std::string& value);
public:
    PythonRunner(const std::string& appName, GlobalFunctions* globalFunctions);
    ~PythonRunner();

    bool Init();
    void Deinit();

    bool RunScript(const std::string& scriptFile, const std::string& functionToCall = "", const std::string& parameter = "");
};


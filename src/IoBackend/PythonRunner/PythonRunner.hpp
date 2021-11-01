#pragma once

#include <string>
#include "PythonConnector.hpp"

class PythonRunner
{
private:
    PythonConnector* _connector;
    std::string _appName;
    void PrintTotalRefCount();
public:
    PythonRunner(const std::string& appName);
    ~PythonRunner();

    bool Init();
    void DeInit();

    bool RunScript(const std::string& scriptFile, const std::string& functionToCall = "");
};


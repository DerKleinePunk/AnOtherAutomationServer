#pragma once

#include <string>
#include "PythonConnector.hpp"

class PythonRunner
{
private:
    PythonConnector* _connector;
public:
    PythonRunner(/* args */);
    ~PythonRunner();

    bool Init();
    void DeInit();

    bool RunScript(const std::string& scriptFile, const std::string& functionToCall = "");
};


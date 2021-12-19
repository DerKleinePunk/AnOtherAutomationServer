#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>
#include <functional>

//https://docs.python.org/3/c-api/bytes.html#c.PyBytes_FromString

typedef std::function<void(const std::string& name, const std::string& parameter)> WriteMqttDelegate;

class PythonConnector
{
private:
    /* data */
    PyObject* ModuleInit();
    WriteMqttDelegate _writeMqttEvent;

public:
    PythonConnector(WriteMqttDelegate writeMqttEvent);
    ~PythonConnector();

    bool Init();

    //Internal Use Only
    void LogIt(const std::string& level, const std::string& text);
    void WriteMqtt(const std::string& topic, const std::string& value);
};


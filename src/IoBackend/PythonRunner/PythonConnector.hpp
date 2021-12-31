#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>
#include <functional>

//https://docs.python.org/3/c-api/bytes.html#c.PyBytes_FromString

typedef std::function<void(const std::string& name, const std::string& parameter)> WriteMqttDelegate;
typedef std::function<void(const std::string& name, const std::string& value)> ChangeValueDelegate;

class PythonConnector
{
private:
    /* data */
    PyObject* ModuleInit();
    WriteMqttDelegate _writeMqttEvent;
    ChangeValueDelegate _changeValueDelegate;

public:
    PythonConnector(WriteMqttDelegate writeMqttEvent, ChangeValueDelegate changeValueDelegate);
    ~PythonConnector();

    bool Init();

    //Internal Use Only
    void LogIt(const std::string& level, const std::string& text);
    void WriteMqtt(const std::string& topic, const std::string& value);
    void ChangeValue(const std::string& name, const std::string& value);
};


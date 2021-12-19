#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>

//https://docs.python.org/3/c-api/bytes.html#c.PyBytes_FromString

class PythonConnector
{
private:
    /* data */
    PyObject* ModuleInit();

public:
    PythonConnector(/* args */);
    ~PythonConnector();

    bool Init();

    //Internal Use Only
    void LogIt(const std::string& level, const std::string& text);
};


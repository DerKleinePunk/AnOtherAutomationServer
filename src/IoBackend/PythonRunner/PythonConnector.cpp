#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "PythonConnector"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "PythonConnector.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"
#define PY_SSIZE_T_CLEAN
#include <Python.h>

typedef PyObject* (*module_init_function)();

#define CLASSKEY "ClassPointer"

static PyObject* ConnectorVer(PyObject *self, PyObject *args)
{
    //PyBytes_FromString
    return PyUnicode_FromString("0.0");
}

static PyObject* ConnectorLogEntry(PyObject *self, PyObject *args)
{
    const char* level;
    const char* text;

    //https://docstore.mik.ua/orelly/other/python/0596001886_pythonian-chp-24-sect-1.html PyArg_ParseTuple
    if (!PyArg_ParseTuple(args, "ss", &level, &text)) {
        Py_RETURN_NONE;
    }

    auto dictionary = PyModule_GetDict(self);
    auto classPointer = PyDict_GetItemString(dictionary, CLASSKEY);
    auto parent = (PythonConnector*)PyLong_AsVoidPtr(classPointer);
    parent->LogIt(std::string(level), std::string(text));

    Py_RETURN_NONE;
}

static PyObject* ConnectorWriteMqtt(PyObject *self, PyObject *args)
{
    const char* topic;
    const char* text;

    //https://docstore.mik.ua/orelly/other/python/0596001886_pythonian-chp-24-sect-1.html PyArg_ParseTuple
    if (!PyArg_ParseTuple(args, "ss", &topic, &text)) {
        Py_RETURN_NONE;
    }

    auto dictionary = PyModule_GetDict(self);
    auto classPointer = PyDict_GetItemString(dictionary, CLASSKEY);
    auto parent = (PythonConnector*)PyLong_AsVoidPtr(classPointer);
    parent->WriteMqtt(std::string(topic), std::string(text));

    Py_RETURN_NONE;
}

static PyObject* ConnectorChangeValue(PyObject *self, PyObject *args)
{
    const char* name;
    const char* text;

    //https://docstore.mik.ua/orelly/other/python/0596001886_pythonian-chp-24-sect-1.html PyArg_ParseTuple
    if (!PyArg_ParseTuple(args, "ss", &name, &text)) {
        Py_RETURN_NONE;
    }

     auto dictionary = PyModule_GetDict(self);
    auto classPointer = PyDict_GetItemString(dictionary, CLASSKEY);
    auto parent = (PythonConnector*)PyLong_AsVoidPtr(classPointer);
    parent->ChangeValue(std::string(name), std::string(text));

    Py_RETURN_NONE;
}

static PyObject* ConnectorAction(PyObject *self, PyObject *args)
{
    const char* name;
    const char* text;

    //https://docstore.mik.ua/orelly/other/python/0596001886_pythonian-chp-24-sect-1.html PyArg_ParseTuple
    if (!PyArg_ParseTuple(args, "ss", &name, &text)) {
        Py_RETURN_NONE;
    }

    auto dictionary = PyModule_GetDict(self);
    auto classPointer = PyDict_GetItemString(dictionary, CLASSKEY);
    auto parent = (PythonConnector*)PyLong_AsVoidPtr(classPointer);
    parent->Action(std::string(name), std::string(text));

    Py_RETURN_NONE;
}

static PyMethodDef ConnectorMethods[] = {
    {"Ver", ConnectorVer, METH_VARARGS, "Return the Version from Backend"},
    {"LogEntry", ConnectorLogEntry, METH_VARARGS, "Write Logfile Entry"},
    {"WriteMqtt", ConnectorWriteMqtt, METH_VARARGS, "Write Value to Mqtt Topic"},
    {"ChangeValue", ConnectorChangeValue, METH_VARARGS, "Write Value to Internal Variable"},
    {"Action", ConnectorAction, METH_VARARGS, "SendAction to System"},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef ConnectorModule = {
    PyModuleDef_HEAD_INIT, "connector", "The Backend Connector Interface", -1, ConnectorMethods,
    NULL, NULL, NULL, NULL
};

PyObject* PythonConnector::ModuleInit()
{
    auto module = PyModule_Create(&ConnectorModule);
    PyModule_AddObject(module, CLASSKEY, PyLong_FromVoidPtr(this));
    return module;
}

/**
 * @brief Construct a new Python Connector:: Python Connector object
 * 
 * @param writeMqttEvent 
 */
PythonConnector::PythonConnector(WriteMqttDelegate writeMqttEvent, ChangeValueDelegate changeValueDelegate, ActionDelegate actionDelegate)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _writeMqttEvent = writeMqttEvent;
    _changeValueDelegate = changeValueDelegate;
    _actionDelegate = actionDelegate;
}

PythonConnector::~PythonConnector()
{
    
}

bool PythonConnector::Init()
{
    utils::Callback<PyObject*()>::func = std::bind(&PythonConnector::ModuleInit, this);
    module_init_function func = static_cast<module_init_function>(utils::Callback<PyObject*()>::callback);

    if(PyImport_AppendInittab("connector", func) < 0)
    {
        return false;
    }

    return true;
}

void PythonConnector::LogIt(const std::string& level, const std::string& text)
{
    if(level == "INFO") {
        LOG(INFO) << text;
    } else if(level == "ERROR") {
        LOG(ERROR) << text;
    } else {
        LOG(DEBUG) << text;
    }
}

void PythonConnector::WriteMqtt(const std::string& topic, const std::string& value)
{
    if(_writeMqttEvent != nullptr) {
        _writeMqttEvent(topic, value);
    }
}

void PythonConnector::ChangeValue(const std::string& name, const std::string& value)
{
    if(_changeValueDelegate != nullptr) {
        _changeValueDelegate(name, value);
    } else {
        LOG(WARNING) << "changeValueDelegate is nullptr";
    }
}

void PythonConnector::Action(const std::string& what, const std::string& value)
{
    if(_actionDelegate != nullptr) {
        _actionDelegate(what, value);
    } else {
        LOG(WARNING) << "actionDelegate is nullptr";
    }
}
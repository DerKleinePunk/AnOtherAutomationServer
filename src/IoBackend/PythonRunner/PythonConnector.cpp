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

    //Todo write Log
    auto dictionary = PyModule_GetDict(self);
    auto classPointer = PyDict_GetItemString(dictionary, CLASSKEY);
    auto parent = (PythonConnector*)PyLong_AsVoidPtr(classPointer);
    parent->LogIt(std::string(level), std::string(text));

    Py_RETURN_NONE;
}


static PyMethodDef ConnectorMethods[] = {
    {"Ver", ConnectorVer, METH_VARARGS, "Return the Version from Backend"},
    {"LogEntry", ConnectorLogEntry, METH_VARARGS, "Write Logfile Entry"},
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

PythonConnector::PythonConnector(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
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
    } else {
        LOG(DEBUG) << text;
    }
}
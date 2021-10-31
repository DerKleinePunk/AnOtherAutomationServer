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

static PyObject* ConnectorVer(PyObject *self, PyObject *args)
{
    //PyBytes_FromString
    return PyUnicode_FromString("0.0");
}


static PyMethodDef ConnectorMethods[] = {
    {"ver", ConnectorVer, METH_VARARGS,
     "Return the Version from Backend"},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef ConnectorModule = {
    PyModuleDef_HEAD_INIT, "connector", NULL, -1, ConnectorMethods,
    NULL, NULL, NULL, NULL
};

static PyObject* PyInit_Connector(void)
{
    return PyModule_Create(&ConnectorModule);
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
    if(PyImport_AppendInittab("connector", &PyInit_Connector) < 0)
    {
        return false;
    }

    return true;
}


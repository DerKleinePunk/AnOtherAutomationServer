#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "PythonRunner"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "PythonRunner.hpp"
#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"
#define PY_SSIZE_T_CLEAN
#include <Python.h>

PythonRunner::PythonRunner(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

PythonRunner::~PythonRunner()
{
}

bool PythonRunner::Init()
{
    _connector = new PythonConnector();

    if(!_connector->Init()){
        return false;
    }
    
    setenv("PYTHONPATH",".",1);
    //static std::wstring pythonRunTimeHomeDir = L"/usr/lib/x86_64-linux-gnu";
    //Py_SetPythonHome(const_cast<wchar_t*>(pythonRunTimeHomeDir.data()));
    Py_Initialize();

    return true;
}

void PythonRunner::DeInit()
{
    if (Py_FinalizeEx() < 0) {
        LOG(WARNING) << "Py_FinalizeEx Failed";
    }
}

bool PythonRunner::RunScript(const std::string& scriptFile, const std::string& functionToCall)
{
    PyObject *pName, *pModule, *pFunc;

    //pName = PyUnicode_DecodeFSDefault(scriptFile.c_str());
    pName = PyUnicode_FromString(scriptFile.c_str());

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == nullptr) {
        PyErr_Print();
        return false;
    }

    if(functionToCall.size() > 0) {
        pFunc = PyObject_GetAttrString(pModule, functionToCall.c_str());
    }

    Py_DECREF(pModule);

    return true;
}
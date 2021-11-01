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

//Stolen from https://github.com/pybind/pybind11

struct wide_char_arg_deleter {
    void operator()(wchar_t *ptr) const {
#if PY_VERSION_HEX >= 0x030500f0
        // API docs: https://docs.python.org/3/c-api/sys.html#c.Py_DecodeLocale
        PyMem_RawFree(ptr);
#else
        delete[] ptr;
#endif
    }
};

inline wchar_t *widen_chars(const char *safe_arg) {
#if PY_VERSION_HEX >= 0x030500f0
    wchar_t *widened_arg = Py_DecodeLocale(safe_arg, nullptr);
#else
    wchar_t *widened_arg = nullptr;
#    if defined(HAVE_BROKEN_MBSTOWCS) && HAVE_BROKEN_MBSTOWCS
    size_t count = strlen(safe_arg);
#    else
    size_t count = mbstowcs(nullptr, safe_arg, 0);
#    endif
    if (count != static_cast<size_t>(-1)) {
        widened_arg = new wchar_t[count + 1];
        mbstowcs(widened_arg, safe_arg, count + 1);
    }
#endif
    return widened_arg;
}

//End Stolen

PythonRunner::PythonRunner(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

PythonRunner::~PythonRunner()
{
}

bool PythonRunner::Init()
{
    if(Py_IsInitialized() > 0)
    {
        LOG(ERROR) << "Init only once";
        return false;
    }

    _connector = new PythonConnector();
    bool init_signal_handlers = false;
    bool add_program_dir_to_path = true;
    int argc = 2;
    const char *const empty_argv[]{"/home/punky/develop/AnOtherAutomationServer/bin/Linux/SimpelIoBackend.bin", "Parameter1"};

    if(!_connector->Init()){
        return false;
    }
    
    //setenv("PYTHONPATH",".",1);
    /*static std::wstring pythonRunTimeHomeDir = L"/usr/lib/python3.7";
    Py_SetPythonHome(const_cast<wchar_t*>(pythonRunTimeHomeDir.data()));*/

    //static std::wstring pythonPrgName = L"/home/punky/develop/AnOtherAutomationServer/bin/Linux/SimpelIoBackend.bin";
    //static std::wstring pythonPrgName = L"/home/punky/develop/AnOtherAutomationServer/bin/Linux";
    //const auto prg = Py_DecodeLocale("/home/punky/develop/AnOtherAutomationServer/bin/Linux/SimpelIoBackend",nullptr);
    //const auto pythonHome = Py_DecodeLocale("/home/punky/.local/lib/python3.7",nullptr);
    //Py_SetProgramName(prg);

    //Py_SetPythonHome(pythonHome);
    Py_InitializeEx(init_signal_handlers ? 1 : 0);

    /*PyObject *sysmodule = PyImport_ImportModule("sys");
    PyObject *path = PyObject_GetAttrString(sysmodule, "path");
    PyList_Append(path, PyUnicode_FromString("/home/punky/develop/AnOtherAutomationServer/bin/Linux"));*/

    auto argv_size = static_cast<size_t>(argc);
    std::unique_ptr<wchar_t *[]> widened_argv(new wchar_t *[argv_size]);
    std::vector<std::unique_ptr<wchar_t[], wide_char_arg_deleter>> widened_argv_entries;
    widened_argv_entries.reserve(argv_size);

    for (size_t ii = 0; ii < argv_size; ++ii) {
        widened_argv_entries.emplace_back(widen_chars(empty_argv[ii]));
        if (!widened_argv_entries.back()) {
            // A null here indicates a character-encoding failure or the python
            // interpreter out of memory. Give up.
            return false;
        }
        widened_argv[ii] = widened_argv_entries.back().get();
    }

    auto pysys_argv = widened_argv.get();

    PySys_SetArgvEx(argc, pysys_argv, static_cast<int>(add_program_dir_to_path));

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

    pName = PyUnicode_DecodeFSDefault(scriptFile.c_str());
    //pName = PyUnicode_FromString(scriptFile.c_str());

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
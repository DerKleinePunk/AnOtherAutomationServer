#pragma once

//https://docs.python.org/3/c-api/bytes.html#c.PyBytes_FromString

class PythonConnector
{
private:
    /* data */
public:
    PythonConnector(/* args */);
    ~PythonConnector();

    bool Init();
};


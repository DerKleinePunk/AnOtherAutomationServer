#pragma once

#include "../GlobalFunctions.hpp"

class ControlerSerialProtocol
{
private:
    GlobalFunctions* _globalFunctions;
public:
    ControlerSerialProtocol(GlobalFunctions* globalFunctions);
    ~ControlerSerialProtocol();
};



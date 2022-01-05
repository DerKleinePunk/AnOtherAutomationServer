#pragma once

#include <fstream>
#include <functional>
#include <map>
#include <string>
#include "../NewLineSerial.hpp"
#include "NMEADecoderBase.hpp"

class NMEAReader
{
    NewLineSerial* _newLineSerial;
    std::string _portname;
    std::string _rawLogFileName;
    DecoderResult _lastResult;

    void IncomingMessage(const std::string& message);
    uint8_t CalculateChecksum(const std::string& text);
    std::string GetNMEAStringLatitude(double latitude);
    std::string GetNMEAStringLongitude(double longitude);

  public:
    NMEAReader(const std::string& portname);
    virtual ~NMEAReader();

    int Init(const std::string& speed);
    void SetRawLogFile(const std::string& name);
    /*void SendMOB(double latitude, double longitude);
    void SendRMB(double latitude, double longitude);
    void SendWPL(double latitude, double longitude);
    void SendAisRadio(unsigned int mmsi, double latitude, double longitude, const std::string& callSign, const std::string& shipName);*/
    void Deinit();
};
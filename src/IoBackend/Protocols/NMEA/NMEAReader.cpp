#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "NMEAReader"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../../common/easylogging/easylogging++.h"
#include "../../../common/utils/commonutils.h"
#include "../../../common/utils/Tokenizer.h"
#include "NMEAReader.hpp"
#include "GPRMCDecoder.hpp"

void NMEAReader::IncomingMessage(const std::string& message)
{
    auto messageIntern = message;
    utils::replaceAll(messageIntern, "\r", "");
    utils::replaceAll(messageIntern, "\n", "");
    if(messageIntern.empty()) {
        return;
    }

    LOG(DEBUG) << "Work on Message " << messageIntern;
    size_t dollar = messageIntern.find_last_of('$');
    if(dollar == std::string::npos) {
        dollar = messageIntern.find_last_of('!');
        if(dollar == std::string::npos) {
            LOG(ERROR) << "No Start Found";
            return;
        }
    }
    messageIntern = messageIntern.substr(dollar + 1);
    size_t checkstri = messageIntern.find_last_of('*');

    if(checkstri == std::string::npos) {
        LOG(WARNING) << "No Checksum Found";
    } else {
        const auto checksumGet = messageIntern.substr(checkstri + 1, 2);
        messageIntern = messageIntern.substr(0, checkstri);
        const auto checksumText = utils::uchar2hex(CalculateChecksum(messageIntern));
        if(checksumGet != checksumText) {
            LOG(ERROR) << checksumGet << " != " << checksumText << "checksum Error";
            return;
        }
    }

    std::vector<std::string> values;
    Tokenizer tokenizer(messageIntern, ",");
    while(tokenizer.NextToken()) {
        values.push_back(tokenizer.GetToken());
    }

    LOG(DEBUG) << "search Decoder for " << values[0];
    if(values[0] == "GPRMC") {
        GPRMCDecoder decoder;
        if(decoder.Decode(values)) {
            _lastResult = decoder.GetResult();
        }
    }
}

uint8_t NMEAReader::CalculateChecksum(const std::string& text)
{
    uint8_t checksum = 0;
    for(const char i : text) {
        checksum = checksum ^ i;
    }
    return checksum;
}

std::string NMEAReader::GetNMEAStringLatitude(double latitude)
{
    std::string result("");
    auto lata = std::abs(latitude);
    auto latd = (int)lata;
    result = utils::string_format("%2d", latd);
    auto latminutes = (lata - latd) * 60;
    result += utils::string_format("%2.2f", latminutes);
    if(latitude > 0) {
        result += ",N";
    } else {
        result += ",S";
    }

    return result;
}

std::string NMEAReader::GetNMEAStringLongitude(double longitude)
{
    std::string result("");
    auto lona = std::abs(longitude);
    auto lond = (int)lona;
    result = utils::string_format("%03d", lond);
    auto lonminutes = (lona - lond) * 60;
    result += utils::string_format("%2.2f", lonminutes);
    if(longitude > 0) {
        result += ",E";
    } else {
        result += ",W";
    }

    return result;
}

NMEAReader::NMEAReader(const std::string& portname)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _portname = portname;
    _newLineSerial = nullptr;
    _lastResult._posValid = false;
}

NMEAReader::~NMEAReader()
{
}

int NMEAReader::Init(const std::string& speed)
{
    _newLineSerial = new NewLineSerial(_portname);
    auto result = _newLineSerial->Open(speed);
    if(!result) {

        delete _newLineSerial;
        _newLineSerial = nullptr;

        return -1;
    }
    
    _newLineSerial->SetRawLogFile(_rawLogFileName);

    auto newDatadelegate = std::bind(&NMEAReader::IncomingMessage, this, std::placeholders::_1);
    _newLineSerial->Start(newDatadelegate);

    return 0;
}

void NMEAReader::SetRawLogFile(const std::string& name)
{
    _rawLogFileName = name;
    if(_newLineSerial != nullptr) {
        _newLineSerial->SetRawLogFile(_rawLogFileName);
    }
}

void NMEAReader::Deinit()
{
    if(_newLineSerial != nullptr) {
        _newLineSerial->Close();
        delete _newLineSerial;
        _newLineSerial = nullptr;
    }
}
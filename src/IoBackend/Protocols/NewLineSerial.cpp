#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "NewLineSerial"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"
#include "NewLineSerial.hpp"

NewLineSerial::NewLineSerial(const std::string& portname) : SerialBase(portname)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
}

NewLineSerial::~NewLineSerial()
{
}

void NewLineSerial::AnalyseBuffer(int count)
{
    currentReadPos_ += count;
    auto lastpos = 0;
    for(auto pos = 0; pos < currentReadPos_; pos++) {
        if(readBuffer_[pos] == '\n') { // New Line Found Text Message
            if(pos - 1 > 0) {
                std::string received(readBuffer_, lastpos, pos - 1 - lastpos);
                LOG(DEBUG) << utils::GetPrintAbleString(received) << " from serial";
                _callbackFunction(received);
            }
            lastpos = pos + 1;
        }
    }
    if(lastpos > 0) {
        if(lastpos == currentReadPos_) {
            memset(readBuffer_, 0, MAXBUFFERSIZE);
            currentReadPos_ = 0;
        } else {
            memcpy(&readBuffer_[0], &readBuffer_[lastpos], currentReadPos_ - lastpos);
            currentReadPos_ = currentReadPos_ - lastpos;
            memset(&readBuffer_[currentReadPos_], 0, MAXBUFFERSIZE - currentReadPos_);
            LOG(DEBUG) << currentReadPos_ << " Bytes in buffer";
        }
    }
}

int NewLineSerial::Start(NewDataCallbackFunction callbackFunction)
{
    _callbackFunction = callbackFunction;
    return SerialBase::Start();
}
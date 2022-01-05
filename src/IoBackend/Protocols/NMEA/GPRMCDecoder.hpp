#pragma once

#include "NMEADecoderBase.hpp"

class GPRMCDecoder : public NMEADecoderBase
{
private:
    /* data */
public:
    GPRMCDecoder(/* args */);
    ~GPRMCDecoder();

    bool Decode(std::vector<std::string> values);
};

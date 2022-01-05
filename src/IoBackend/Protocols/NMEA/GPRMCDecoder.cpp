#include "GPRMCDecoder.hpp"

GPRMCDecoder::GPRMCDecoder()
{
}

GPRMCDecoder::~GPRMCDecoder()
{
}

bool GPRMCDecoder::Decode(std::vector<std::string> values) {
    DecodeUtcTime(values[1]);
    if(values[2] == "A") {
        DecodeLat(values[4], values[3]);
		DecodeLon(values[6], values[5]);
        _decoderResult._posValid = true;
        if(values.size() >= 8) {
			if (values[7].length() > 0) {
				_decoderResult._speedValid = true;
				_decoderResult._speed = std::stof(values[7]) * KMPH; //convert Knoten to Km/h
			}
			if (values[8].length() > 0) {
				_decoderResult._compassValid = true;
				_decoderResult._compass = std::stof(values[8]); //Bewegungsrichtung in Grad
			}
        }
    } else {
        _decoderResult._posValid = false;
    }

    return _decoderResult._posValid;
}
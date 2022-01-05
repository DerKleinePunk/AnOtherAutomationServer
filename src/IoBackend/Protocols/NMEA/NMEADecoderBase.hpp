#pragma once
#include <string>
#include <time.h>
#include <vector>

#define KMPH    1.852       // kilometers-per-hour in one knot
#define MPH     1.1507794   // miles-per-hour in one knot

struct Geopos
{
    double Lat;
    double Lon;
};

struct DecoderResult {
    bool _posValid;
    bool _timestampValid;
    bool _speedValid;
    bool _compassValid;
    time_t _lastTime;
    Geopos _currentPos;
    double _speed;
    double _compass;
};

class NMEADecoderBase
{
protected:
    DecoderResult _decoderResult;

    void DecodeUtcTime(const std::string& time);
    void DecodeLat(const std::string& richtung, const std::string& value);
    void DecodeLon(const std::string& richtung, const std::string& value);
public:
    NMEADecoderBase();
    virtual ~NMEADecoderBase();

    virtual bool Decode(std::vector<std::string> values) = 0;
    DecoderResult GetResult() const;
};

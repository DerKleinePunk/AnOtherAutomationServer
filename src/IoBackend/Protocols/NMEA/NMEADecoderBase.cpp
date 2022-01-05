#ifndef ELPP_DEFAULT_LOGGER
#   define ELPP_DEFAULT_LOGGER "NMEADecoderBase"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#   define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../../common/easylogging/easylogging++.h"
#include "../../../common/utils/commonutils.h"
#include "NMEADecoderBase.hpp"

#include <iomanip>
#include <sstream>

void NMEADecoderBase::DecodeUtcTime(const std::string& time)
{
    //withou seperator vs2015 can't decode the Time MS Bug ?
	//Todo Corrent this we need two thinks an TimeStamp in local time and the di form last position update on linux and windows with micro sekonds
	static const std::string dateTimeFormat{ "%H:%M:%S" };
	auto timeLocal = time;
	auto j = timeLocal.find_first_of(".");
	if (std::string::npos != j)
	{
		timeLocal = timeLocal.substr(0,j);
	}
	timeLocal = timeLocal.substr(0, 2) + ":" + timeLocal.substr(2);
	timeLocal = timeLocal.substr(0, 5) + ":" + timeLocal.substr(5);
	std::tm dt;
	std::istringstream input(timeLocal);
	input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
	input >> std::get_time(&dt, dateTimeFormat.c_str());
	if(input.fail()) {
		LOG(WARNING) << "Date convert Failed";
		return;
	}
	
	_decoderResult._lastTime = mktime(&dt);
	_decoderResult._posValid = true;
}

void NMEADecoderBase::DecodeLat(const std::string& richtung, const std::string& value) {
	_decoderResult._currentPos.Lat = std::stoi(value.substr(0, 2));
	double nMinuten = std::stof(value.substr(2));
	nMinuten = nMinuten / 60;
	_decoderResult._currentPos.Lat += nMinuten;
	if(richtung == "S") {
		//Andere Seite WeltKugel
		_decoderResult._currentPos.Lat =-_decoderResult._currentPos.Lat;
	}
}

void NMEADecoderBase::DecodeLon(const std::string& richtung, const std::string& value) {
	_decoderResult._currentPos.Lon = std::stoi(value.substr(0, 3));
	double nMinuten = std::stof(value.substr(3));
	nMinuten = nMinuten / 60;
	_decoderResult._currentPos.Lon += nMinuten;
	if (richtung == "W") {
		//Andere Seite WeltKugel
		_decoderResult._currentPos.Lon = -_decoderResult._currentPos.Lon;
	}
}

NMEADecoderBase::NMEADecoderBase()
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _decoderResult._posValid = false;
    _decoderResult._timestampValid = false;
    _decoderResult._speedValid = false;
    _decoderResult._compassValid = false;
}

NMEADecoderBase::~NMEADecoderBase()
{
}

DecoderResult NMEADecoderBase::GetResult() const {
	return _decoderResult;
}
#pragma once

#include <string>
#include <map>
#include <vector>
#include <NetworkManager.h>
#include "../../common/json/json.hpp"

using json = nlohmann::json;

struct AccessPointInfo
{
    std::string Ssid;
    std::string Bssid;
    std::string Mode;
    std::string Freg;
    std::string Bitrate;
    uint8_t Strength;
};

void to_json(json& j, const AccessPointInfo& p);

struct DeviceInfo
{
    std::string ActiveSsid;
    std::string Iface;
    long Speed;
    std::vector<AccessPointInfo> AccessPoints;
};

void to_json(json& j, const DeviceInfo& p);

class NetworkManager
{
private:
    NMClient* _client;

    bool CreateClient();
    void BuildWifiDeviceInfo(NMDevice *device, std::vector<DeviceInfo>& resultInfo);
    void BuildAccessPointInfo(NMAccessPoint *ap, std::vector<AccessPointInfo>& accessPoints);
public:
    NetworkManager(/* args */);
    ~NetworkManager();

    std::vector<DeviceInfo> ScanAccessPoints(const std::string& interfaceName = "");
};

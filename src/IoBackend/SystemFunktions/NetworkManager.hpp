#pragma once

#include <string>
#include <map>
#include <vector>
#include <NetworkManager.h>
#include "../../common/json/json.hpp"
#include "../Protocols/ZeroConf/mDnsClient.hpp"

using json = nlohmann::json;

struct AccessPointInfo
{
    std::string Ssid;
    std::string Bssid;
    std::string Mode;
    std::string Freg;
    std::string Bitrate;
    uint8_t Strength;
    std::string Security;
    std::string DBusPath;
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
    mDnsClient* _mdnsClient;
    bool CreateClient();
    void BuildWifiDeviceInfo(NMDevice *device, std::vector<DeviceInfo>& resultInfo);
    void BuildAccessPointInfo(NMAccessPoint *ap, std::vector<AccessPointInfo>& accessPoints);
    void AddedCB(GObject *client, GAsyncResult *result, gpointer user_data);
    void ActivateConnectionCB(GObject *client, GAsyncResult *result, gpointer user_data);
public:
    NetworkManager(/* args */);
    NetworkManager(const NetworkManager& orig) = delete;
	NetworkManager(NetworkManager&& other) = delete;
	NetworkManager& operator=(const NetworkManager& other) = delete;
	NetworkManager& operator=(NetworkManager&& other) = delete;
    ~NetworkManager();

    std::vector<DeviceInfo> ScanAccessPoints(const std::string& interfaceName = "");

    bool ConnectAccessPoint(const std::string& connectionName, const std::string& password, const std::string& interfaceName = "");
    void ScanDevices(const std::string serviceType);
};

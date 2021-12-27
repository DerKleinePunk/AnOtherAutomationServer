#ifndef ELPP_DEFAULT_LOGGER
#define ELPP_DEFAULT_LOGGER "NetworkManager"
#endif
#ifndef ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID
#define ELPP_CURR_FILE_PERFORMANCE_LOGGER_ID ELPP_DEFAULT_LOGGER
#endif

#include "../../common/easylogging/easylogging++.h"
#include "../../common/utils/commonutils.h"
#include "NetworkManager.hpp"
#include <glib.h>

void to_json(json& j, const AccessPointInfo& p)
{
    j = json{ 
        { "Bitrate", p.Bitrate }, 
        { "Bssid", p.Bssid }, 
        { "Freg", p.Freg }, 
        { "Mode", p.Mode },
        { "Ssid", p.Ssid },
        { "Strength", p.Strength}
    };
}

void to_json(json& j, const DeviceInfo& p)
{
     j = json{ 
        { "ActiveSsid", p.ActiveSsid }, 
        { "Iface", p.Iface }, 
        { "Speed", p.Speed }, 
        { "AccessPoints", p.AccessPoints }
    };
}

/* Convert flags to string */
static char *
ap_wpa_rsn_flags_to_string(guint32 flags)
{
    char *flags_str[16]; /* Enough space for flags and terminating NULL */
    char *ret_str;
    int   i = 0;

    if (flags & NM_802_11_AP_SEC_PAIR_WEP40)
        flags_str[i++] = g_strdup("pair_wpe40");
    if (flags & NM_802_11_AP_SEC_PAIR_WEP104)
        flags_str[i++] = g_strdup("pair_wpe104");
    if (flags & NM_802_11_AP_SEC_PAIR_TKIP)
        flags_str[i++] = g_strdup("pair_tkip");
    if (flags & NM_802_11_AP_SEC_PAIR_CCMP)
        flags_str[i++] = g_strdup("pair_ccmp");
    if (flags & NM_802_11_AP_SEC_GROUP_WEP40)
        flags_str[i++] = g_strdup("group_wpe40");
    if (flags & NM_802_11_AP_SEC_GROUP_WEP104)
        flags_str[i++] = g_strdup("group_wpe104");
    if (flags & NM_802_11_AP_SEC_GROUP_TKIP)
        flags_str[i++] = g_strdup("group_tkip");
    if (flags & NM_802_11_AP_SEC_GROUP_CCMP)
        flags_str[i++] = g_strdup("group_ccmp");
    if (flags & NM_802_11_AP_SEC_KEY_MGMT_PSK)
        flags_str[i++] = g_strdup("psk");
    if (flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X)
        flags_str[i++] = g_strdup("802.1X");

    if (i == 0)
        flags_str[i++] = g_strdup("none");

    flags_str[i] = NULL;

    ret_str = g_strjoinv(" ", flags_str);

    i = 0;
    while (flags_str[i])
        g_free(flags_str[i++]);

    return ret_str;
}

bool NetworkManager::CreateClient()
{
    GError* error = nullptr;
    _client = nm_client_new(nullptr, &error);
    if (!_client) {
        LOG(ERROR) << error->message;
        g_error_free(error);
        return false;
    }

    return true;
}

void NetworkManager::BuildWifiDeviceInfo(NMDevice *device, std::vector<DeviceInfo>& resultInfo)
{
    NMAccessPoint *  active_ap = nullptr;
    const GPtrArray* aps;
    GBytes* active_ssid;
    char*   active_ssid_str = nullptr;

    /* Get active AP */
    if (nm_device_get_state(device) == NM_DEVICE_STATE_ACTIVATED) {
        if ((active_ap = nm_device_wifi_get_active_access_point(NM_DEVICE_WIFI(device)))) {
            active_ssid = nm_access_point_get_ssid(active_ap);
            if (active_ssid)
                active_ssid_str = nm_utils_ssid_to_utf8((guint8*)(g_bytes_get_data(active_ssid, NULL)), g_bytes_get_size(active_ssid));
            else
                active_ssid_str = g_strdup("--");
        }
    }
    
    DeviceInfo info;
    info.Iface = nm_device_get_iface(device);
    info.ActiveSsid = active_ssid_str ?: "none";
    info.Speed  = nm_device_wifi_get_bitrate(NM_DEVICE_WIFI(device));
    info.Speed /= 1000;

    g_free(active_ssid_str);

    /* Get all APs of the Wi-Fi device */
    aps = nm_device_wifi_get_access_points(NM_DEVICE_WIFI(device));

    /* Print AP details */
    for (guint i = 0; i < aps->len; i++) {
        NMAccessPoint* ap = reinterpret_cast<NMAccessPoint*>(g_ptr_array_index(aps, i));
        BuildAccessPointInfo(ap, info.AccessPoints);
    }

    resultInfo.push_back(info);
}

void NetworkManager::BuildAccessPointInfo(NMAccessPoint* ap, std::vector<AccessPointInfo>& accessPoints)
{
    AccessPointInfo info;
    guint32     flags, wpa_flags, rsn_flags, freq, bitrate;
    guint8      strength;
    GBytes *    ssid;
    const char *hwaddr;
    NM80211Mode mode;
    char *      freq_str, *ssid_str, *bitrate_str, *wpa_flags_str, *rsn_flags_str;
    GString *   security_str;

    /* Get AP properties */
    flags     = nm_access_point_get_flags(ap);
    wpa_flags = nm_access_point_get_wpa_flags(ap);
    rsn_flags = nm_access_point_get_rsn_flags(ap);
    ssid      = nm_access_point_get_ssid(ap);
    hwaddr    = nm_access_point_get_bssid(ap);
    freq      = nm_access_point_get_frequency(ap);
    mode      = nm_access_point_get_mode(ap);
    bitrate   = nm_access_point_get_max_bitrate(ap);
    strength  = nm_access_point_get_strength(ap);

    /* Convert to strings */
    if (ssid)
        ssid_str = nm_utils_ssid_to_utf8((guint8*)(g_bytes_get_data(ssid, NULL)), g_bytes_get_size(ssid));
    else
        ssid_str = g_strdup("--");

    freq_str      = g_strdup_printf("%u MHz", freq);
    bitrate_str   = g_strdup_printf("%u Mbit/s", bitrate / 1000);
    //strength_str  = g_strdup_printf("%u", strength);
    wpa_flags_str = ap_wpa_rsn_flags_to_string(wpa_flags);
    rsn_flags_str = ap_wpa_rsn_flags_to_string(rsn_flags);

    security_str = g_string_new(NULL);
    if (!(flags & NM_802_11_AP_FLAGS_PRIVACY) && (wpa_flags != NM_802_11_AP_SEC_NONE)
        && (rsn_flags != NM_802_11_AP_SEC_NONE))
        g_string_append(security_str, "Encrypted: ");

    if ((flags & NM_802_11_AP_FLAGS_PRIVACY) && (wpa_flags == NM_802_11_AP_SEC_NONE)
        && (rsn_flags == NM_802_11_AP_SEC_NONE))
        g_string_append(security_str, "WEP ");
    if (wpa_flags != NM_802_11_AP_SEC_NONE)
        g_string_append(security_str, "WPA ");
    if (rsn_flags != NM_802_11_AP_SEC_NONE)
        g_string_append(security_str, "WPA2 ");
    if ((wpa_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X)
        || (rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X))
        g_string_append(security_str, "Enterprise ");

    if (security_str->len > 0)
        g_string_truncate(security_str, security_str->len - 1); /* Chop off last space */

    info.Ssid = ssid_str;
    info.Bssid = hwaddr;
    info.Mode =  mode == NM_802_11_MODE_ADHOC   ? "Ad-Hoc"
                 : mode == NM_802_11_MODE_INFRA ? "Infrastructure"
                                                : "Unknown";
    info.Freg = freq_str;
    info.Bitrate = bitrate_str;
    info.Strength = strength;

    //printf("SSID:       %s\n", ssid_str);
    /*printf("BSSID:      %s\n", hwaddr);
    printf("Mode:       %s\n",
           mode == NM_802_11_MODE_ADHOC   ? "Ad-Hoc"
           : mode == NM_802_11_MODE_INFRA ? "Infrastructure"
                                          : "Unknown");*/
    //printf("Freq:       %s\n", freq_str);
    //printf("Bitrate:    %s\n", bitrate_str);
    //printf("Strength:   %s\n", strength_str);
    printf("Security:   %s\n", security_str->str);
    printf("WPA flags:  %s\n", wpa_flags_str);
    printf("RSN flags:  %s\n", rsn_flags_str);
    printf("D-Bus path: %s\n\n", nm_object_get_path(NM_OBJECT(ap)));

    g_free(ssid_str);
    g_free(freq_str);
    g_free(bitrate_str);
    //g_free(strength_str);
    g_free(wpa_flags_str);
    g_free(rsn_flags_str);
    g_string_free(security_str, TRUE);

    accessPoints.push_back(info);
}

NetworkManager::NetworkManager(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _client = nullptr;
}

NetworkManager::~NetworkManager()
{
    if(_client != nullptr) {
        g_object_unref(_client);
    }
}

std::vector<DeviceInfo> NetworkManager::ScanAccessPoints(const std::string& interfaceName)
{
    if(_client == nullptr) {
        if(!CreateClient()){
            LOG(ERROR) << "Error Creating NMClient";
            return {};
        }
    }

    //Save all Devices ?
    /* Get all devices managed by NetworkManager */
    const GPtrArray* devices = nm_client_get_devices(_client);
    LOG(DEBUG) << std::to_string(devices->len) << " devices get from nm-client";

    std::vector<DeviceInfo> resultInfo;
    /* Go through the array and process Wi-Fi devices */
    for (guint i = 0; i < devices->len; i++) {
        NMDevice *device = reinterpret_cast<NMDevice*>(g_ptr_array_index(devices, i));
        if (NM_IS_DEVICE_WIFI(device)) {
            if(interfaceName.size() == 0 || strcmp(interfaceName.c_str(), nm_device_get_iface(device)) == 0) {
                BuildWifiDeviceInfo(device, resultInfo);
            }
        } else {
            LOG(DEBUG) << nm_device_get_iface(device) << " is not an wifi device";
        }
    }

    return resultInfo;
}
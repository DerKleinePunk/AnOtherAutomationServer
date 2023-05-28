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

//https://gitlab.freedesktop.org/NetworkManager/NetworkManager/-/blob/main/examples/C/glib/get-ap-info-libnm.c
//https://github.com/liugang/NetworkManager/blob/master/wifi-scan.c

void to_json(json& j, const AccessPointInfo& p)
{
    j = json{ 
        { "Bitrate", p.Bitrate }, 
        { "Bssid", p.Bssid }, 
        { "Freg", p.Freg }, 
        { "Mode", p.Mode },
        { "Ssid", p.Ssid },
        { "Strength", p.Strength},
        { "Security", p.Security},
        { "DBusPath", p.DBusPath}
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
    if (flags & NM_802_11_AP_SEC_KEY_MGMT_SAE)
        flags_str[i++] = g_strdup("sae");
    if (flags & NM_802_11_AP_SEC_KEY_MGMT_OWE)
        flags_str[i++] = g_strdup("owe");
    if (flags & NM_802_11_AP_SEC_KEY_MGMT_OWE_TM)
        flags_str[i++] = g_strdup("owe_transition_mode");
    if (flags & NM_802_11_AP_SEC_KEY_MGMT_EAP_SUITE_B_192)
        flags_str[i++] = g_strdup("wpa-eap-suite-b-192");

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
    info.Security = security_str->str;
    //printf("WPA flags:  %s\n", wpa_flags_str);
    //printf("RSN flags:  %s\n", rsn_flags_str);
    info.DBusPath = nm_object_get_path(NM_OBJECT(ap));

    g_free(ssid_str);
    g_free(freq_str);
    g_free(bitrate_str);
    g_free(wpa_flags_str);
    g_free(rsn_flags_str);
    g_string_free(security_str, TRUE);

    accessPoints.push_back(info);
}

void NetworkManager::AddedCB(GObject *client, GAsyncResult *result, gpointer user_data)
{
    const auto loop = reinterpret_cast<GMainLoop*>(user_data);
    NMRemoteConnection *remote;
    GError             *error = NULL;

    /* NM responded to our request; either handle the resulting error or
     * print out the object path of the connection we just added.
     */
    remote = nm_client_add_connection_finish(NM_CLIENT(client), result, &error);

    if (error) {
        g_print("Error adding connection: %s", error->message);
        g_error_free(error);
    } else {
        g_print("Added: %s\n", nm_connection_get_path(NM_CONNECTION(remote)));
        g_object_unref(remote);
    }

    /* Tell the mainloop we're done and we can quit now */
    g_main_loop_quit(loop);

}

void NetworkManager::ActivateConnectionCB(GObject *client, GAsyncResult *result, gpointer user_data)
{
    const auto loop = reinterpret_cast<GMainLoop*>(user_data);
    GError *error = nullptr;

    NMActiveConnection *active;
    active = nm_client_activate_connection_finish(NM_CLIENT (client), result, &error);

    auto endLoop = true;
    if (error) {
		g_print("Error activation connection: %s", error->message);
        LOG(ERROR) << error->message;
		g_error_free (error);
        g_main_loop_quit(loop);
    } else {
        auto state = nm_active_connection_get_state(active);
        if (state == NM_ACTIVE_CONNECTION_STATE_ACTIVATED) {
            LOG(DEBUG) << "NM_ACTIVE_CONNECTION_STATE_ACTIVATED";

        } else if(state == NM_ACTIVE_CONNECTION_STATE_DEACTIVATED) {
            LOG(DEBUG) << "NM_ACTIVE_CONNECTION_STATE_DEACTIVATED";
        
        } else if(state == NM_ACTIVE_CONNECTION_STATE_UNKNOWN) {
            LOG(DEBUG) << "NM_ACTIVE_CONNECTION_STATE_UNKNOWN";
            
        } else {
            //g_signal_connect(active, "state-changed", G_CALLBACK (active_connection_state_cb), loop);
        }
    }

    if(endLoop) {
        /* Tell the mainloop we're done and we can quit now */
        g_main_loop_quit(loop);
    }
}

NetworkManager::NetworkManager(/* args */)
{
    el::Loggers::getLogger(ELPP_DEFAULT_LOGGER);
    _client = nullptr;
    _mdnsClient = nullptr;
}

NetworkManager::~NetworkManager()
{
    if(_client != nullptr) {
        g_object_unref(_client);
    }

    if(_mdnsClient != nullptr) {
        delete _mdnsClient;
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

bool NetworkManager::ConnectAccessPoint(const std::string& connectionName, const std::string& password, const std::string& interfaceName)
{
    if(_client == nullptr) {
        if(!CreateClient()){
            LOG(ERROR) << "Error Creating NMClient";
            return false;
        }
    }

    auto loop = g_main_loop_new(NULL, FALSE);

    /*NMConnection        *connection;
    NMSettingConnection *s_con;
    NMSettingWired      *s_wired;
    NMSettingIP4Config  *s_ip4;
    char                *uuid;

    // Create a new connection object 
    connection = nm_simple_connection_new();

    // Build up the 'connection' Setting 
    s_con = (NMSettingConnection *) nm_setting_connection_new();
    uuid  = nm_utils_uuid_generate();
    g_object_set(G_OBJECT(s_con),
                 NM_SETTING_CONNECTION_UUID,
                 uuid,
                 NM_SETTING_CONNECTION_ID,
                 connectionName.c_str(),
                 NM_SETTING_CONNECTION_TYPE,
                 "802-3-ethernet",
                 NULL);
    g_free(uuid);
    nm_connection_add_setting(connection, NM_SETTING(s_con));

    // Build up the 'wired' Setting
    s_wired = (NMSettingWired *) nm_setting_wired_new();
    nm_connection_add_setting(connection, NM_SETTING(s_wired));

    // Build up the 'ipv4' Setting 
    s_ip4 = (NMSettingIP4Config *) nm_setting_ip4_config_new();
    g_object_set(G_OBJECT(s_ip4),
                 NM_SETTING_IP_CONFIG_METHOD,
                 NM_SETTING_IP4_CONFIG_METHOD_AUTO,
                 NULL);
    nm_connection_add_setting(connection, NM_SETTING(s_ip4));

    utils::Callback<void(GObject* , GAsyncResult*, gpointer)>::func = std::bind(&NetworkManager::AddedCB, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    GAsyncReadyCallback added_cb = static_cast<GAsyncReadyCallback>(utils::Callback<void(GObject* , GAsyncResult*, gpointer)>::callback);

    //Ask the settings service to add the new connection; we'll quit the
    // mainloop and exit when the callback is called.
    
    nm_client_add_connection_async(_client, connection, FALSE, NULL, added_cb, loop);
    g_object_unref(connection);*/

    //device_found = find_device_for_connection (nmc, connection, ifname, ap, nsp, &device, &spec_object, &local);

    //nm_client_activate_connection_async (_client, connection, device, spec_object, NULL, callback, info);

    //utils::Callback<void(GObject* , GAsyncResult*, gpointer)>::func = std::bind(&NetworkManager::AddedCB, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    //GAsyncReadyCallback added_cb = static_cast<GAsyncReadyCallback>(utils::Callback<void(GObject* , GAsyncResult*, gpointer)>::callback);

    const char *spec_object;

    //https://cgit.freedesktop.org/NetworkManager/NetworkManager/tree/clients/cli/connections.c?id=9c3df9caa3296a55fe54e6aa02fe8a11b27deaba
    //https://stackoverflow.com/questions/57621775/libnm-specifying-password-when-connecting-to-wifi-from-c-program

    const GPtrArray* devices = nm_client_get_devices(_client);
    LOG(DEBUG) << std::to_string(devices->len) << " devices get from nm-client";

    NMDevice* found_device = nullptr;
    NM80211ApSecurityFlags ap_wpa_flags;
    NM80211ApSecurityFlags ap_rsn_flags;

    for (guint i = 0; i < devices->len; i++) {
        NMDevice *device = reinterpret_cast<NMDevice*>(g_ptr_array_index(devices, i));
        if (NM_IS_DEVICE_WIFI(device)) {
            if(interfaceName.size() == 0 || strcmp(interfaceName.c_str(), nm_device_get_iface(device)) == 0) {
                //char *bssid_up = g_ascii_strup (connectionName.c_str(), -1);
				const GPtrArray *aps = nm_device_wifi_get_access_points (NM_DEVICE_WIFI(device));      
                for (guint j = 0; j < aps->len; j++) {
					NMAccessPoint *candidate_ap = reinterpret_cast<NMAccessPoint*>(g_ptr_array_index(aps, j));
                    //Todo later find Networks without SSID Broadcast
					//const char *candidate_bssid = nm_access_point_get_ssid (candidate_ap);
                    const auto ssid = nm_access_point_get_ssid (candidate_ap);
                    if(ssid == nullptr) 
                    {
                        LOG(DEBUG) << "ssid is empty";
                        continue;
                    }

                    const auto ssid_str = nm_utils_ssid_to_utf8((guint8*)(g_bytes_get_data(ssid, NULL)), g_bytes_get_size(ssid));

					if (strcmp(connectionName.c_str(), ssid_str) == 0) {
                        ap_wpa_flags = nm_access_point_get_wpa_flags(candidate_ap);
                        ap_rsn_flags = nm_access_point_get_rsn_flags(candidate_ap);
						found_device = device;
						spec_object = nm_object_get_path (NM_OBJECT (candidate_ap));
						break;
					}
                    g_free(ssid_str);
				}
				//g_free (bssid_up);
            }
        } else {
            if(found_device == nullptr) {
                LOG(DEBUG) << nm_device_get_iface(device) << " is not an wifi device";
            } else {
                break;
            }
        }
    }
    
    utils::Callback<void(GObject* , GAsyncResult*, gpointer)>::func = std::bind(&NetworkManager::ActivateConnectionCB, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    GAsyncReadyCallback added_cb = static_cast<GAsyncReadyCallback>(utils::Callback<void(GObject* , GAsyncResult*, gpointer)>::callback);

    if(found_device != nullptr) {
        bool wep_passphrase = false; //Todo Understand
        //auto agent = nm_secret_agent_simple_new ("nmcli-connect");
        
        NMConnection* connection = nm_simple_connection_new();
        nm_connection_set_path(connection, "/");
        
        char* uuid;

        uuid  = nm_utils_uuid_generate();
        g_object_set(G_OBJECT(connection),
                 NM_SETTING_CONNECTION_UUID,
                 uuid,
                 NM_SETTING_CONNECTION_ID,
                 connectionName.c_str(),
                 NM_SETTING_CONNECTION_TYPE,
                 "802-3-ethernet",
                 NULL);

        auto s_wireless = ( NMSettingWireless*) nm_setting_wireless_new();
        nm_connection_add_setting (connection, NM_SETTING (s_wireless));
        
        auto s_wsec = (NMSettingWirelessSecurity *) nm_setting_wireless_security_new();
        nm_connection_add_setting (connection, NM_SETTING (s_wsec));
 
        if (ap_wpa_flags == NM_802_11_AP_SEC_NONE && ap_rsn_flags == NM_802_11_AP_SEC_NONE) {
            /* WEP */
            nm_setting_wireless_security_set_wep_key(s_wsec, 0, password.c_str());
            g_object_set(G_OBJECT(s_wsec),
                            NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE,
                            wep_passphrase ? NM_WEP_KEY_TYPE_PASSPHRASE : NM_WEP_KEY_TYPE_KEY,
                            NULL);
        } else if ((ap_wpa_flags & NM_802_11_AP_SEC_KEY_MGMT_PSK)
                    || (ap_rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_PSK)
                    || (ap_rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_SAE)) {
            /* WPA PSK */
            g_object_set(s_wsec, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, "wpa-psk", NULL);
            g_object_set(s_wsec, NM_SETTING_WIRELESS_SECURITY_AUTH_ALG, "open", NULL);
            g_object_set(s_wsec, NM_SETTING_WIRELESS_SECURITY_PSK, password.c_str(), NULL);
        }

        nm_connection_dump(connection);

        nm_client_activate_connection_async (_client, connection, found_device, spec_object, NULL, added_cb, loop);

         /* Wait for the connection to be added */
        g_main_loop_run(loop);
    }

    g_main_loop_unref(loop);
   

    return false;
}

void NetworkManager::ScanDevices(const std::string serviceType)
{
    if(_mdnsClient == nullptr) {
        _mdnsClient = new mDnsClient();
    }

    _mdnsClient->ScanNetwork(serviceType);
}
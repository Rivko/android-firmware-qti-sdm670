/*********************************************************************
 Copyright (c) 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**********************************************************************/
#ifndef _WigigSuppConnector_h
#define _WigigSuppConnector_h

#include "SuppConnector.h"
#include "libwpa_client/wpa_ctrl.h"
#include "cutils/properties.h"
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

// __PATH_MAX on linux is 4096 bytes but we don't need that long,
// since we use relatively short paths in android
#define __CONFIG_PATH_MAX      128
#define __SERVICE_NAME_MAX     32
#define __SSID_MAX             32
#define __PASSPHRASE_MAX       63
#define __IFNAME_MAX           32
#define __WOWLAN_TRIGGERS_MAX  128

/**
 * Defines a configuration for starting and connecting to 
 * supplicant/hostapd 
 */
typedef struct SuppConnectorConfig {
    // path to entropy file name for supplicant. If not set, will not be created
    char supp_entropy_file[__CONFIG_PATH_MAX];
    // init.svc property for checking if service is running/stopped
    char supp_prop_name[PROPERTY_VALUE_MAX];
    // path to supplicant sockets. Directory will be cleaned up
    // before starting and after stopping supplicant
    char supp_cli_sock_path[__CONFIG_PATH_MAX];
    // name of the supplicant/hostapd service to start/stop
    char supplicant_name[__SERVICE_NAME_MAX];
    // name or path of supplicant socket for sending
    // commands/receiving events
    char supp_socket_path[__CONFIG_PATH_MAX];
    // the primary interface the supplicant is serving (usually wigig0)
    char primary_iface[__IFNAME_MAX];
    // set to true if we need to receive events. Typically we want to receive
    // events from the supplicant but not hostapd (yet)
    bool receive_events;
} SuppConnectorConfig;

class WigigSuppConnector : public SuppConnector {
public:
    WigigSuppConnector();
    ~WigigSuppConnector();
    // post construction init - MUST be called immediately after constructor
    virtual void init();

    int startSupplicant();
    int killSupplicant();
    int sendCommand(const char *cmd, char *reply, size_t *replylen);
    int registerCallback(ISuppConnectorCallback *callback);
    virtual int setConfigOption(const char *name, const char *value);
protected:
    virtual int setup_config_file();
protected:
    SuppConnectorConfig mConfig;
private:
    int ensure_config_file_exists(const char *config_file, const char *template_file);
    int ensure_entropy_file_exists();
    void wpa_cleanup_internal(const char *cli_path);
    int start_supplicant_internal();
    int stop_supplicant_internal();
    int connect_on_socket_path(const char *path);
    int connect_to_supplicant();
    int send_command_internal(const char *cmd, char *reply, size_t *reply_len);
    int is_supplicant_connection_active();
    int wait_event_internal(char *reply, size_t *reply_len);
    int recv_from_supplicant_internal(char *buf, size_t buflen);
    void close_sockets();
    void close_supplicant_connection();
    int start_monitoring();
    int stop_monitoring();
    static void *do_monitor(void *arg);
    void notify_event(char *buf, size_t buflen);
private:
    struct wpa_ctrl *ctrl_conn;
    struct wpa_ctrl *monitor_conn;
    int exit_sockets[2];
    pthread_t monitor_thread;
    bool is_monitoring;
    ISuppConnectorCallback *mCallback;
    bool driver_load_only;
};

class WigigHostapdConnector : public WigigSuppConnector {
public:
    WigigHostapdConnector();
    ~WigigHostapdConnector();
    void init();
    int setConfigOption(const char *name, const char *value);

protected:
    int setup_config_file();
private:
    int generate_psk(const char *ssid, const char *passphrase, char *psk_str,
                     size_t psk_str_len);

private:
    // hostapd config options
    char ifname[__IFNAME_MAX + 1];
    char ssid[__SSID_MAX + 1];
    int channel;
    int hidden;
    int secured;
    char passphrase[__PASSPHRASE_MAX + 1];
    char wowlan_triggers[__WOWLAN_TRIGGERS_MAX + 1];
    int max_num_sta;
    char country_code[3];
};
#endif /* _WigigSuppConnector_h */

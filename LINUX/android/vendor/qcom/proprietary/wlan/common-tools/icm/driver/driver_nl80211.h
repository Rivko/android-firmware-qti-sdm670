/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * Notifications and licenses are retained for attribution purposes only

 * Driver interaction with Linux nl80211/cfg80211 - definitions
 * Copyright (c) 2002-2017, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2003-2004, Instant802 Networks, Inc.
 * Copyright (c) 2005-2006, Devicescape Software, Inc.
 * Copyright (c) 2007, Johannes Berg <johannes@sipsolutions.net>
 * Copyright (c) 2009-2010, Atheros Communications
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef DRIVER_NL80211_H
#define DRIVER_NL80211_H

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include "nl80211_copy.h"
#include "qca-vendor_copy.h"
#include "list.h"

#define nl_handle nl_sock
#define nl80211_handle_alloc nl_socket_alloc_cb
#define nl80211_handle_destroy nl_socket_free

struct nl80211_global {
    void *ctx;
    struct dl_list interfaces;
    int if_add_ifindex;
    u64 if_add_wdevid;
    int if_add_wdevid_set;
    struct netlink_data *netlink;
    struct nl_cb *nl_cb;
    struct nl_handle *nl;
    int nl80211_id;
    int ioctl_sock; /* socket for ioctl() use */

    struct nl_handle *nl_event;
};

struct nl80211_data {
    struct nl80211_global *global;
    int ifindex;
    void *ctx;

    /* Scan specific data */
#define MAX_REPORT_FREQS 50
    int external_scan;
    enum scan_states {
        NO_SCAN, SCAN_REQUESTED, SCAN_STARTED, SCAN_COMPLETED,
        SCAN_ABORTED, SCHED_SCAN_STARTED, SCHED_SCAN_STOPPED,
        SCHED_SCAN_RESULTS
    } scan_state;
    u64 vendor_scan_cookie;
    int aborted;
    int *freqs;
    size_t num_freqs;

    struct nl_handle *rtnl_sk; /* nl_sock for NETLINK_ROUTE */
};

#define WPA_SCAN_QUAL_INVALID           BIT(0)
#define WPA_SCAN_NOISE_INVALID          BIT(1)
#define WPA_SCAN_LEVEL_INVALID          BIT(2)
#define WPA_SCAN_LEVEL_DBM              BIT(3)
#define WPA_SCAN_ASSOCIATED             BIT(5)

/*
 * enum event_type - Event type for ICM calls based on nl80211 events
 */
enum event_type {
    EVENT_SCAN_RESULTS,
    EVENT_SCAN_STARTED,
    EVENT_START_ACS,
};

#define MAX_NL80211_NOISE_FREQS 50

struct nl80211_noise_info {
    u32 freq[MAX_NL80211_NOISE_FREQS];
    s8 noise[MAX_NL80211_NOISE_FREQS];
    u64 time[MAX_NL80211_NOISE_FREQS];
    u64 time_busy[MAX_NL80211_NOISE_FREQS];
    u64 time_tx[MAX_NL80211_NOISE_FREQS];
    unsigned int count;
};

/*
 * struct scan_res - Scan result for an BSS/IBSS
 * @flags: information flags about the BSS/IBSS (WPA_SCAN_*)
 * @bssid: BSSID
 * @freq: frequency of the channel in MHz (e.g., 2412 = channel 1)
 * @beacon_int: beacon interval in TUs (host byte order)
 * @caps: capability information field in host byte order
 * @qual: signal quality
 * @survey: survey info data per channel
 * @level: signal level
 * @tsf: Timestamp
 * @age: Age of the information in milliseconds (i.e., how many milliseconds
 * ago the last Beacon or Probe Response frame was received)
 * @est_throughput: Estimated throughput in kbps (this is calculated during
 * scan result processing if left zero by the driver wrapper)
 * @snr: Signal-to-noise ratio in dB (calculated during scan result processing)
 * @ie_len: length of the following IE field in octets
 * @beacon_ie_len: length of the following Beacon IE field in octets
 *
 * This structure is used as a generic format for scan results from the
 * driver. Each driver interface implementation is responsible for converting
 * the driver or OS specific scan results into this format.
 *
 * If the driver does not support reporting all IEs, the IE data structure is
 * constructed of the IEs that are available. This field will also need to
 * include SSID in IE format. All drivers are encouraged to be extended to
 * report all IEs to make it easier to support future additions.
 *
 * This structure data is followed by ie_len octets of IEs from Probe Response
 * frame (or if the driver does not indicate source of IEs, these may also be
 * from Beacon frame). After the first set of IEs, another set of IEs may follow
 * (with beacon_ie_len octets of data) if the driver provides both IE sets.
 */
struct scan_res {
    unsigned int flags;
    u8 bssid[ETH_ALEN];
    int freq;
    u16 beacon_int;
    u16 caps;
    int qual;
    int level;
    u64 tsf;
    unsigned int age;
    unsigned int est_throughput;
    int snr;
    size_t ie_len;
    size_t beacon_ie_len;
    /* Followed by ie_len + beacon_ie_len octets of IE data.
     * Can be fetched as following
     * pos = (u8 *) (scan_res +1);
     * ie = pos;
     * beacon_ie = pos + scan_res->ie_len; */
};

/*
 * struct scan_results - Scan results
 * @res: Array of pointers to allocated variable length scan result entries
 * @num: Number of entries in the scan result array
 * @fetch_time: Time when the results were fetched from the driver
 */
struct scan_results {
    struct scan_res **res;
    size_t num;
    struct nl80211_noise_info info;
    //struct os_reltime fetch_time;
};

struct nl80211_bss_info_arg {
    struct nl80211_data *drv;
    struct scan_results *res;
    unsigned int assoc_freq;
    unsigned int ibss_freq;
    u8 assoc_bssid[ETH_ALEN];
};


#if 1
/* Following structure needs to be inline with current ICM design.
 * Remove this once we have clear idea on which parameter maps
 * to which param in ICM
 */

struct nl80211_scan_config {

    /*
     * Not sure why this is need, could this be equivalent to scan_flags.
     * Right now not sending this to driver
     */
    u8 chan_info_flag;

    /*
     * scan_ies - Scan IE(s) to add into Probe Request or %NULL
     */
    const u8 *scan_ies;

    /*
     * scan_ies_len - Length of scan_ies in octets
     */
    size_t scan_ies_len;

    /*
     * freqs - Array of frequencies to scan or %NULL for all frequencies
     *
     * The frequency is set in MHz. The array is zero-terminated.
     * This is equivalent to ch_list mentioned in ICM LLD
     */
    int *freqs;
};

struct nl80211_chan_args {
    enum nl80211_band band; /* 2G or 5G */
    u8 pri_ch;  /* primary channel needed for all */
    u8 ht_sec_ch; // required only for 160 / 80 + 80
    u8 channel_width; /* needed for all */
    u8 vht_seg0_center_ch; /* needed only for vht */
    u8 vht_seg1_center_ch; /* needed only for vht */
};

struct nl80211_channel_config {
    char *ifname;
    u8 num_channel;
    enum qca_wlan_vendor_acs_select_reason reselect_reason;
    struct nl80211_chan_args *channel_list;
};

struct nl80211_chan_info_event {
    /* Error code */
    u32 err_code;
    /* Channel freq */
    u32 freq;
    /* Read flags */
    u32 cmd_flags;
    /* Noise Floor value */
    u32 noise_floor;
    /* rx clear count */
    u32 rx_clear_count;
    /* cycle count */
    u32 cycle_count;
    /* channel tx power per range in 0.5dBm steps */
    u32 chan_tx_pwr_range;
    /* channel tx power per throughput */
    u32 chan_tx_pwr_tp;
    /* rx frame count (cumulative) */
    u32 rx_frame_count;
    /* BSS rx cycle count */
    u32 my_bss_rx_cycle_count;
    /* b-mode data rx time (units are microseconds) */
    u32 rx_11b_mode_data_duration;
};

#endif /* parameters */

struct nl_msg;

void * nl80211_cmd(struct nl80211_global *global,
        struct nl_msg *msg, int flags, uint8_t cmd);
int send_and_recv_msgs(struct nl80211_global *global, struct nl_msg *msg,
        int (*valid_handler)(struct nl_msg *, void *),
        void *valid_data);
struct nl_msg * nl80211_drv_msg(struct nl80211_data *drv, int flags,
        uint8_t cmd);

void process_nl80211_events(void *ctx, enum event_type event, void *event_data);
int nl80211_handle_ifindx_change(void *ctx, struct nl80211_global *global);

/* ICM related driver exposed APIs */
int get_ifindex(char *ifname);
int linux_get_ifhwaddr(int sock, const char *ifname, u8 *addr);
const u8 * get_ie(const u8 *ies, size_t len, u8 eid);
void * zalloc(size_t size);
int init_driver_netlink(void *ctx);
int deinit_driver_netlink(void *ctx);
int driver_nl80211_event_receive(void *ctx, void *handle);
int driver_nl80211_abort_scan(void *data);
int driver_nl80211_vendor_scan(void *data, struct nl80211_scan_config *params);
struct scan_results * driver_nl80211_get_scan_results(void *ctx);
void scan_results_free(struct scan_results *res);
int driver_nl80211_set_channel(void *ctx, struct nl80211_channel_config *chan_config);
int driver_nl80211_get_country(void *ctx, char *alpha2);
int driver_nl80211_get_reg_domain(void *ctx, enum nl80211_dfs_regions *dfs_domain);
#endif /* DRIVER_NL80211_H */

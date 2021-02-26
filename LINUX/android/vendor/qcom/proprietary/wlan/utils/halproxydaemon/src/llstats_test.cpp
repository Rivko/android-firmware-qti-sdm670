/*
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*
*/

#include "llstats_test.hpp"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "link_layer_stats.h"

#define MAC_ADDR_ARRAY(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MAC_ADDR_STR "%02x:%02x:%02x:%02x:%02x:%02x"
wifi_interface_handle wifi_get_iface_handle(wifi_handle handle, char *name);
namespace LLStats
{
    /* CLI cmd strings */
    const char *LLStatsTestSuite::LL_CMD = "llstats";
    const char *LLStatsTestSuite::LL_SET = "set";
    const char *LLStatsTestSuite::LL_GET = "get";
    const char *LLStatsTestSuite::LL_CLEAR = "clear";


    wifi_error LLStatsTestSuite:: wifi_set_link_stats_test(
                                    wifi_interface_handle iface,
                                    u32 mpduSizeThreshold,
                                    u32 aggressiveStatisticsGathering)
    {
        wifi_link_layer_params params;
        params.mpdu_size_threshold = mpduSizeThreshold;
        params.aggressive_statistics_gathering = aggressiveStatisticsGathering;
        wifi_error status;
        status = wifi_set_link_stats(iface, params);
        fprintf(stderr, "wifi_set_link_stats_test: mpdu size threshold: %d, "
            "aggressive statistics gathering: %d\n",
            mpduSizeThreshold,
            aggressiveStatisticsGathering);
        if (!status) {
           printf(" Status Success: %d in %s:%d\n", status, __func__, __LINE__);
        } else {
           printf(" Status Failed : %d in %s:%d\n", status, __func__, __LINE__);
        }
        return status;
    }

    void link_stats_results_handler(wifi_request_id id,
                                    wifi_iface_stat *iface_stat,
                                    int num_radios,
                                    wifi_radio_stat *radio_stat)
    {
        int i = 0;
        u32 j = 0;
        u32 k = 0;
        u32 n = 0;
        wifi_radio_stat *stats = radio_stat;

        printf("Stats Received for Request Id : %d\n", id);
        if (stats) {
            for (i = 0; i < num_radios; i++) {
                printf("\n################ Radio Stats ########################\n");
                printf("radio :               %u\n", stats->radio);
                printf("on_time :             %u\n", stats->on_time);
                printf("tx_time :             %u\n", stats->tx_time);
                printf("rx_time :             %u\n", stats->rx_time);
                printf("on_time_scan :        %u\n", stats->on_time_scan);
                printf("on_time_nbd :         %u\n", stats->on_time_nbd);
                printf("on_time_gscan :       %u\n", stats->on_time_gscan);
                printf("on_time_roam_scan :   %u\n", stats->on_time_roam_scan);
                printf("on_time_pno_scan)     %u\n", stats->on_time_pno_scan);
                printf("on_time_hs20 :        %u\n", stats->on_time_hs20);
                printf("num_channels :        %u\n", stats->num_channels);
                printf("num_tx_levels :       %u\n", stats->num_tx_levels);

                for (j = 0; j < stats->num_channels; j++) {
                    printf("\n");
                    printf("center_freq  : %u\n", stats->channels[j].channel.center_freq);
                    printf("width        : %u\n", stats->channels[j].channel.width);
                    printf("center_freq0 : %u\n", stats->channels[j].channel.center_freq0);
                    printf("center_freq1 : %u\n", stats->channels[j].channel.center_freq1);
                    printf("on_time      : %u\n", stats->channels[j].on_time);
                    printf("cca_busy_time: %u\n", stats->channels[j].cca_busy_time);
                    printf("\n");
                }

                if (stats->tx_time_per_levels) {
                    printf("Time spent in each transmission power level: \n");
                    for (j = 0; j < stats->num_tx_levels; j++) {
                        printf("Power level : %u    tx_time : %u\n", j,
                                stats->tx_time_per_levels[j]);
                    }
                }
                printf("\n#####################################################\n");
                stats = (wifi_radio_stat *)((u8 *)stats
                         + sizeof(wifi_radio_stat)
                         + stats->num_channels* sizeof(wifi_channel_stat));
            }
        }
        if (iface_stat) {
            printf("\n################# Interface Stats ##################\n");
            if (iface_stat->info.mode == 0)
                printf("Interface Mode : %s\n","STA");
            else if (iface_stat->info.mode == 1)
                printf("Interface Mode : %s\n","SOFTAP");
            else if (iface_stat->info.mode == 2)
                printf("Interface Mode : %s\n","IBSS");
            else if (iface_stat->info.mode == 3)
                printf("Interface Mode : %s\n","P2P_CLIENT");
            else if (iface_stat->info.mode == 4)
                printf("Interface Mode : %s\n","P2P_GO");
            else if (iface_stat->info.mode == 5)
                printf("Interface Mode : %s\n","NAN");
            else if (iface_stat->info.mode == 6)
                printf("Interface Mode : %s\n","MESH");
            else
                printf("Interface Mode : %s\n","Unknown");

            printf("Interface Mac Address : %02x:%02x:%02x:%02x:%02x:%02x \n",
                    iface_stat->info.mac_addr[0], iface_stat->info.mac_addr[1],
                    iface_stat->info.mac_addr[2], iface_stat->info.mac_addr[3],
                    iface_stat->info.mac_addr[4], iface_stat->info.mac_addr[5]);

            if (iface_stat->info.state == 0)
                printf("Interface State : %s\n","DISCONNECTED");
            else if (iface_stat->info.state == 1)
                printf("Interface State : %s\n","AUTHENTICATING");
            else if (iface_stat->info.state == 2)
                printf("Interface State : %s\n","ASSOCIATING");
            else if (iface_stat->info.state == 3)
                printf("Interface State : %s\n","ASSOCIATED");

            printf("Roaming State  : %0d\n",iface_stat->info.roaming);
            printf("Interface Capabilities : %0x\n",
                    iface_stat->info.capabilities);
            printf("Interface SSID : %s\n",iface_stat->info.ssid);
            printf("Interface BSSID : %02x:%02x:%02x:%02x:%02x:%02x \n",
                    iface_stat->info.bssid[0], iface_stat->info.bssid[1],
                    iface_stat->info.bssid[2], iface_stat->info.bssid[3],
                    iface_stat->info.bssid[4], iface_stat->info.bssid[5]);
            printf("AP Country String : %c%c%c\n",
                    iface_stat->info.ap_country_str[0],
                    iface_stat->info.ap_country_str[1],
                    iface_stat->info.ap_country_str[2]);
            printf("Country String : %c%c%c\n",
                    iface_stat->info.country_str[0],
                    iface_stat->info.country_str[1],
                    iface_stat->info.country_str[2]);
            printf("Beacon rx : %u\n", iface_stat->beacon_rx);
            printf("average_tsf_offset: %" PRIu64 "\n",
                    iface_stat->average_tsf_offset);
            printf("leaky_ap_detected: %u\n", iface_stat->leaky_ap_detected);
            printf("leaky_ap_avg_num_frames_leaked: %u\n",
                    iface_stat->leaky_ap_avg_num_frames_leaked);
            printf("leaky_ap_guard_time: %u\n",
                    iface_stat->leaky_ap_guard_time);
            printf("mgmt rx : %u\n", iface_stat->mgmt_rx);
            printf("mgmt_action_rx : %u\n", iface_stat->mgmt_action_rx);
            printf("mgmt_action_tx : %u\n", iface_stat->mgmt_action_tx);
            printf("rssi_mgmt : %d\n", iface_stat->rssi_mgmt);
            printf("rssi_data : %d\n", iface_stat->rssi_data);
            printf("rssi_ack : %d\n", iface_stat->rssi_ack);
            printf("num peers : %u\n\n", iface_stat->num_peers);

            printf("############# WMM Stats: #################\n\n");

            for (k = 0; k < WIFI_AC_MAX; k++) {
                wifi_wmm_ac_stat *stat = &iface_stat->ac[k];
                printf("ac : %u \n", stat->ac);
                printf("txMpdu : %u \n", stat->tx_mpdu) ;
                printf("rxMpdu : %u \n", stat->rx_mpdu);
                printf("txMcast : %u \n", stat->tx_mcast);
                printf("rxMcast : %u \n", stat->rx_mcast);
                printf("rxAmpdu : %u \n", stat->rx_ampdu);
                printf("txAmpdu : %u \n", stat->tx_ampdu);
                printf("mpduLost : %u \n", stat->mpdu_lost);
                printf("retries : %u \n", stat->retries);
                printf("retriesShort : %u \n",
                        stat->retries_short);
                printf("retriesLong : %u \n",
                        stat->retries_long);
                printf("contentionTimeMin : %u \n",
                        stat->contention_time_min);
                printf("contentionTimeMax : %u \n",
                        stat->contention_time_max);
                printf("contentionTimeAvg : %u \n",
                        stat->contention_time_avg);
                printf("contentionNumSamples : %u \n\n",
                        stat->contention_num_samples);
            }

            printf("############# Peer stats: #################\n\n");
            for (k = 0; k < iface_stat->num_peers; k++) {
                switch (iface_stat->peer_info[k].type)
                {
                    case WIFI_PEER_STA:
                        printf("Peer Type : WIFI_PEER_STA\n");
                        break;
                    case WIFI_PEER_AP:
                        printf("Peer Type : WIFI_PEER_AP\n");
                        break;
                    case WIFI_PEER_P2P_GO:
                        printf("Peer Type : WIFI_PEER_P2P_GO\n");
                        break;
                    case WIFI_PEER_P2P_CLIENT:
                        printf("Peer Type : WIFI_PEER_P2P_CLIENT\n");
                        break;
                    case WIFI_PEER_NAN:
                        printf("Peer Type : WIFI_PEER_NAN\n");
                        break;
                    case WIFI_PEER_TDLS:
                        printf("Peer Type : WIFI_PEER_TDLS\n");
                        break;
                    case WIFI_PEER_INVALID:
                        printf("Peer Type : WIFI_PEER_INVALID\n");
                        break;
                    default:
                        printf("Invalid peer type value\n");
                }
                printf("peer mac address : %02x:%02x:%02x:%02x:%02x:%02x \n",
                        iface_stat->peer_info[k].peer_mac_address[0],
                        iface_stat->peer_info[k].peer_mac_address[1],
                        iface_stat->peer_info[k].peer_mac_address[2],
                        iface_stat->peer_info[k].peer_mac_address[3],
                        iface_stat->peer_info[k].peer_mac_address[4],
                        iface_stat->peer_info[k].peer_mac_address[5]);
                printf("capability : %u\n", iface_stat->peer_info[k].capabilities);
                printf("Number of rates : %u\n\n", iface_stat->peer_info[k].num_rate);
                n = iface_stat->peer_info[k].num_rate;
                for (j = 0; j < n ; j++) {
                    printf("Rate Set  = %u\n", j);
                    printf("preamble  %u \n", iface_stat->peer_info[k].rate_stats[j].rate.preamble);
                    printf("nss %u\n", iface_stat->peer_info[k].rate_stats[j].rate.nss);
                    printf("bw %u\n", iface_stat->peer_info[k].rate_stats[j].rate.bw);
                    printf("rateMcsIdx  %u\n", iface_stat->peer_info[k].rate_stats[j].rate.rateMcsIdx);
                    printf("reserved %u\n", iface_stat->peer_info[k].rate_stats[j].rate.reserved);
                    printf("bitrate %u\n", iface_stat->peer_info[k].rate_stats[j].rate.bitrate);
                    printf("txMpdu %u\n", iface_stat->peer_info[k].rate_stats[j].tx_mpdu);
                    printf("rxMpdu %u\n", iface_stat->peer_info[k].rate_stats[j].rx_mpdu);
                    printf("mpduLost %u\n", iface_stat->peer_info[k].rate_stats[j].mpdu_lost);
                    printf("retries %u\n", iface_stat->peer_info[k].rate_stats[j].retries);
                    printf("retriesShort %u\n\n", iface_stat->peer_info[k].rate_stats[j].retries_short);
                }
            }
            printf("Done \n");
        }
    }
        /* Helper routine to initiialize the Link Layer Stats */
    wifi_error LLStatsTestSuite:: wifi_get_link_stats_test(
                                    wifi_interface_handle iface)
    {
        wifi_error status;
        wifi_stats_result_handler handler;
        handler.on_link_stats_results = link_stats_results_handler;

        status = wifi_get_link_stats(1, iface, handler);
        if (!status) {
            printf(" Status Success: %d in %s:%d\n", status, __func__, __LINE__);
        } else {
            printf(" Status Failed : %d in %s:%d\n", status, __func__, __LINE__);
        }
        return status;
    }

    /*
     * WIFI_STATS_RADIO              0x00000001       all radio statistics
     * WIFI_STATS_RADIO_CCA          0x00000002       cca_busy_time (within radio statistics)
     * WIFI_STATS_RADIO_CHANNELS     0x00000004       all channel statistics (within radio statistics)
     * WIFI_STATS_RADIO_SCAN         0x00000008       all scan statistics (within radio statistics)
     * WIFI_STATS_IFACE              0x00000010       all interface statistics
     * WIFI_STATS_IFACE_TXRATE       0x00000020       all tx rate statistics (within interface statistics)
     * WIFI_STATS_IFACE_AC           0x00000040       all ac statistics (within interface statistics)
     * WIFI_STATS_IFACE_CONTENTION   0x00000080       all contention (min, max, avg) statistics (within ac statisctics)
     */
    /* Helper routine to initiialize the Link Layer statistics Handlers */
    wifi_error LLStatsTestSuite:: wifi_clr_link_stats_test (
                                    wifi_interface_handle iface,
                                    u32 stats_clear_req_mask,
                                    u8 stop_req)
    {
        u32 stats_clear_rsp_mask = 0;
        u8  stop_rsp = 0;
        wifi_error status;

        status = wifi_clear_link_stats(iface, stats_clear_req_mask,
                                &stats_clear_rsp_mask, stop_req, &stop_rsp);
        if (!status) {
           printf(" Status Success\n");
           printf(" stats_clear_req_mask : %d, stats_clear_rsp_mask : %d,"
                   "stop_req : %d, stop_rsp : %d\n", stats_clear_req_mask,
                   stats_clear_rsp_mask, stop_req, stop_rsp);
        } else
           printf(" Status Failed\n");
        return status;
    }
    /* Constructor */
    LLStatsTestSuite::LLStatsTestSuite(wifi_handle handle)
        :wifiHandle_(handle)
    {

    }

    /* Constructor */
    void LLStatsTestSuite::Usage()
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "$hal_proxy_daemon llstats iface_name set"
                " <mpdu_size_threshold> <aggressive_statistics_gathering>\n");
        fprintf(stderr, "Ex: $hal_proxy_daemon llstats wlan0 set 100 1\n\n");
        fprintf(stderr, "$hal_proxy_daemon llstats iface_name get\n");
        fprintf(stderr, "Ex: $hal_proxy_daemon llstats wlan0 get\n\n");
        fprintf(stderr, "$hal_proxy_daemon llstats iface_name clear"
                " <stats_clear_req_mask(in decimal)> <stop_req>\n");
        fprintf(stderr, "Ex: $hal_proxy_daemon llstats wlan0 clear 255 1\n\n");
    }

    /* process the command line args */
    wifi_error LLStatsTestSuite::processCmd(int argc, char **argv)
    {
        if (argc < 4) {
            Usage();
            fprintf(stderr, "%s: insufficient LL Stats args\n", argv[0]);
            return WIFI_ERROR_INVALID_ARGS;
        }

        if (strcasecmp(argv[3], LL_SET) == 0) {
            if (argc < 6) {
                fprintf(stderr, "%s: insufficient args for LL Stats set\n",
                        __func__);
                fprintf(stderr, "Usage : hal_proxy_daemon llstats iface_name"
                        " set <MPDU Size Threshold>"
                        " <Aggressive Statistics Gathering>\n");
                return WIFI_ERROR_INVALID_ARGS;
            }

            //TODO : Take the Interface name as an argument
            ifaceHandle = wifi_get_iface_handle(wifiHandle_, argv[2]);
            if(!ifaceHandle)
            {
                fprintf(stderr, "Interface %s is not up, exiting.\n", argv[2]);
                return WIFI_ERROR_INVALID_ARGS;
            }

            return  wifi_set_link_stats_test(ifaceHandle, atoi(argv[4]), atoi(argv[5]));
        }

        if (strcasecmp(argv[3], LL_GET) == 0) {

            //TODO : Take the Interface name as an argument
            ifaceHandle = wifi_get_iface_handle(wifiHandle_, argv[2]);
            if(!ifaceHandle)
            {
                fprintf(stderr, "Interface %s is not up, exiting.\n", argv[2]);
                return WIFI_ERROR_INVALID_ARGS;
            }
            return wifi_get_link_stats_test(ifaceHandle);
        }

        if (strcasecmp(argv[3], LL_CLEAR) == 0) {
            if (argc < 6) {
                fprintf(stderr, "%s: insufficient args for LL Stats clear\n",
                        __func__);
                fprintf(stderr, "Usage : hal_proxy_daemon llstats"
                        " iface_name clear clr_req_mask stop_req\n");
                return WIFI_ERROR_INVALID_ARGS;
            }

            ifaceHandle = wifi_get_iface_handle(wifiHandle_, argv[2]);
            if(!ifaceHandle)
            {
                fprintf(stderr, "Interface %s is not up, exiting.\n", argv[2]);
                return WIFI_ERROR_INVALID_ARGS;
            }

            fprintf(stderr, "%s: calling wifi_clr_link_stats_test with "
                "req_mask:0x%x and stop_req:%d\n", __func__,
                (u32)strtoul(argv[4], NULL, 0), (u8)atoi(argv[5]));
            return wifi_clr_link_stats_test(ifaceHandle,
                strtoul(argv[4], NULL, 0), (u8)atoi(argv[5]));
        }
        fprintf(stderr, "%s: unknown cmd %s\n", argv[0], argv[3]);
        fprintf(stderr,
            "USAGE: ex: \n%s llstats wlan0 set <MPDU Size Threshold>"
            " <Aggressive Statistics Gathering> \n", argv[0]);
        return WIFI_ERROR_NOT_SUPPORTED;
    }

}

#ifndef NETMGR_CONFIG_H
#define NETMGR_CONFIG_H
/*!
  @file
  netmgr_config.h

  @brief
  This file provides configuration required by netmgr module. It uses configdb
  library and XML configuration file ("netmgr_config.xml").
*/

/*===========================================================================
  Copyright (c) 2013-2017, 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#include "netmgr.h"
#include "netmgr_defs.h"

/*===========================================================================
                               MACRO DEFINITIONS
===========================================================================*/

#define NETMGR_PROPERTY_DATA_TARGET "persist.data.target"
#define NETMGR_PROPERTY_DATA_TARGET_VALUE_FUSION4_5_PCIE "fusion4_5_pcie"
#define NETMGR_PROPERTY_DATA_TARGET_VALUE_FUSION4_5_HSIC "fusion4_5_hsic"
#define NETMGR_PROPERTY_DATA_TARGET_VALUE_MSM8994        "msm8994"
#define NETMGR_PROPERTY_DATA_TARGET_VALUE_LE_9X45        "tesla"

#if defined (NETMGR_OFFTARGET)
#define NETMGR_CONFIG_FILE  "/mnt/host/vendor/qcom/proprietary/data/netmgr/src/netmgr_config.xml"
#elif defined (FEATURE_DS_LINUX_ANDROID)
#define NETMGR_CONFIG_FILE  "/system/vendor/etc/data/netmgr_config.xml"
#else
#define NETMGR_CONFIG_FILE  "/etc/data/netmgr_config.xml"
#endif /* FEATURE_DS_LINUX_ANDROID */

#define NETMGR_CFGDB_STRVAL_MAX 100
#define NETMGR_MAX_PATH_LEN NETMGR_CFGDB_STRVAL_MAX

/* Structure for QMI data control devices. It also contains a boolean
 * member to identify if the link is enabled or not. */
#define NETMGR_CFG_PARAM_LEN  20
#define NETMGR_CFG_CONNID_LEN 30

#define NETMGR_DEVICE_NAME_SIZE 100

/* Macros representing program run mode */
#define  NETMGR_MAIN_RUNMODE_DEFAULT     0x00  /* Default runmode          */
#define  NETMGR_MAIN_RUNMODE_BACK        0x01  /* Run as forked deamon     */
#define  NETMGR_MAIN_RUNMODE_ETHERNET    0x02  /* Use Ethernet framing     */
#define  NETMGR_MAIN_RUNMODE_QOSHDR      0x04  /* Use RmNET QoS header     */

/* Macros representing physical network devices */
#define NETMGR_PHYS_NET_DEV_RMNET       "rmnet0"
#define NETMGR_PHYS_NET_DEV_RMNET_IPA   "rmnet_ipa0"
#define NETMGR_PHYS_NET_DEV_RMNET_USB   "rmnet_usb0"
#define NETMGR_PHYS_NET_DEV_RMNET_MHI   "rmnet_mhi0"
#define NETMGR_PHYS_NET_DEV_RMNET_BAM   NETMGR_PHYS_NET_DEV_RMNET

#define NETMGR_INCR_PROCESS_READY_REQ()     \
  NETMGR_LOCK_MUTEX(netmgr_main_cfg_lock);  \
  ++netmgr_main_cfg.process_ready_reqs;     \
  netmgr_l2s_format_msg(netmgr_l2s_invalid_link,"incr process_ready_reqs %d",\
                        netmgr_main_cfg.process_ready_reqs); \
  NETMGR_UNLOCK_MUTEX(netmgr_main_cfg_lock);

#define NETMGR_DECR_PROCESS_READY_REQ()     \
  NETMGR_LOCK_MUTEX(netmgr_main_cfg_lock);  \
  --netmgr_main_cfg.process_ready_reqs;     \
  netmgr_l2s_format_msg(netmgr_l2s_invalid_link,"decr process_ready_reqs %d",\
                        netmgr_main_cfg.process_ready_reqs); \
  NETMGR_UNLOCK_MUTEX(netmgr_main_cfg_lock);

#define NETMGR_GET_PROCESS_READY_REQ()   \
  netmgr_main_cfg.process_ready_reqs

#define NETMGR_SET_MODEM_SSR_STATE(state)   \
  NETMGR_LOCK_MUTEX(netmgr_main_cfg_lock);  \
  netmgr_main_cfg.modem_ssr_state = state;  \
  netmgr_l2s_format_msg(netmgr_l2s_invalid_link,\
        "set SSR state to %d", state); \
  NETMGR_UNLOCK_MUTEX(netmgr_main_cfg_lock);

#define NETMGR_GET_MODEM_SSR_STATE()   \
  netmgr_main_cfg.modem_ssr_state


/*===========================================================================
                     DEFINITIONS AND DECLARATIONS
===========================================================================*/
typedef struct netmgr_data_format_s {
  int qos_format;
  int link_prot;
  int ul_data_aggregation_protocol;
  int dl_data_aggregation_protocol;
  int dl_gro_enabled;
  int dl_agg_size;
  int dl_agg_cnt;
  int dl_agg_pad;
  int num_mux_channels;
  int num_iwlan_channels;
  int qos_header_format;
  int qos_header_format_valid;
  int ul_agg_cnt;
  int ul_agg_size;
  int ul_gso_enabled;
} netmgr_data_format_t;

typedef enum iwlan_state_e
{
  IWLAN_DISABLE,
  IWLAN_ENABLE,
  NSWO_ONLY
}iwlan_state_t;

typedef enum netmgr_modem_state_e
{
  NETMGR_MODEM_NORMAL_STATE,
  NETMGR_MODEM_OOS_STATE,
  NETMGR_MODEM_IS_STATE
}netmgr_modem_state_t;

struct netmgr_cmd_exec_overrides_s
{
  unsigned int log_options;                  /* Choose from DS_EXEC_LOG* bitmasks */
  unsigned int cmd_debug;                    /* Choose from DS_EXEC_CMD* bitmasks */
  char outfile[NETMGR_MAX_PATH_LEN];         /* Output file name                  */
};

#define MAX_IKE_PORTS 5
#define NETMGR_MAIN_IKE_PORT_INVALID  (-1)

typedef struct
{
  int ike_port;
  char local_addr[NETMGR_MAX_STR_LENGTH];
  char dest_addr[NETMGR_MAX_STR_LENGTH];
} netmgr_ike_port_info_t;

/* Collection of program configuration info */
struct netmgr_main_cfg_s {
  int runmode;         /* Process run mode */
  int logmode;         /* Logging mode */
  int logthreshold;    /* Logging threshold */
  int nint;            /* Number of interfaces */
  char * iname;        /* Name of virtual ethernet interface */
  int skip;            /* Whether to skip driver module loading */
  char * dirpath;      /* Directory pathname to look for script files */
  char * modscr;       /* Name of script to use for loading module */
  boolean debug;       /* Verbose debug flag */
  boolean runtests;    /* Execute internal tests flag */
  boolean initialized; /* Program initialization completed */
  netmgr_link_id_t  def_link; /* Default link */
  iwlan_state_t iwlan_enabled;      /* iWLAN feature is enabled */
  boolean iwlan_ims_enabled;  /* iWLAN IMS feature is enabled */
  boolean iwlan_traffic_sel_enabled;   /* iWLAN traffic selector feature  is enabled */
  boolean iwlan_client_enable;         /* Based on modem capability we will enable or disable
                                          iwlan client inside netmgr */

  boolean rmnet_data_enabled; /* RmNET data enabled */
  char phys_net_dev[NETMGR_DEVICE_NAME_SIZE]; /* Physical network device name */

  int modem_enable[NETMGR_MAX_MODEMS]; /* Modems enabled status */

  boolean single_qmux_ch_enabled; /* Is single QMUX channel enabled */
  char qmux_ch_name[NETMGR_DEVICE_NAME_SIZE];/* Single QMUX channel name */
  char smd_ch_name[NETMGR_DEVICE_NAME_SIZE]; /* Single SMD control channel */
  uint32 epid;                  /* Data channel EPID associated with QMUX ch */
  uint32 ep_type;               /* EP_TYPE based on the underlying physical channel */
  uint32 consumer_pipe_num;     /* IPA consumer EP number for data channel */
  uint32 producer_pipe_num;     /* IPA producer EP number for data channel */

  boolean dpm_enabled;        /* Data port-mapper enabled */
  boolean use_qmuxd;          /* Force use qmuxd in presence of shim */
  int     dpm_retry_timeout;  /* Data port-mapper retry timeout */
  boolean wda_data_format;    /* Use WDA data format (default is CTL) */
  boolean tcp_ack_prio;       /* TCP Ack Prioritization is enabled */
  boolean ibfc;               /* In-band flow control */
  boolean qos_disabled;       /* QOS is disabled */
  struct netmgr_data_format_s data_format;
  netmgr_modem_state_t modem_ssr_state;        /* Modem SSR state */
  int pnd_rps_mask;           /* RPS mask on transport */
  int vnd_rps_mask;           /* RPS mask on virtual net devices */
  int clat_rps_mask;          /* RPS mask on clat device */
  int netdev_budget;          /* System-wide netdev budget */
  boolean low_latency_filters;  /* Use QMI DFS for low latency filters */
  boolean dropssdp;           /* Drop SSDP packets on WWAN */
  int process_ready_reqs; /* Should netmgr start responding to ready reqs */
  boolean hybrid_qos;         /* Hybrid qos */
  boolean df_configured;      /* Is data format configured on phy net dev */
  int netdev_max_backlog; /* System-wide netdev max backlog */
  int disable_tcp_hystart_detect; /* disable tcp hystart detect */
  int disable_hystart;        /* disable hystart */
  int initial_ssthresh;     /* intitial ssthresh */
  int frag_low_mem;           /* System-wide ipv4 frag low threshold */
  int frag_high_mem;          /* System-wide ipv4 frag high threshold */
  boolean qos_via_idl;        /* QoS service supported via IDL */
  unsigned long tc_ul_baserate; /* Maximum TC UL rate */
  unsigned long dynamic_tc_ul_baserate; /* Dynamic TC UL rate */
  unsigned long tc_ul_burst;    /*TC UL Burst */
  unsigned int num_data_ports;           /* Number of data ports */
  unsigned int num_rev_data_ports;       /* Number of reverse data ports */
  netmgr_ike_port_info_t ike_ports[MAX_IKE_PORTS];   /* IKE port to be used for iWLAN
                                                        TODO: Once the modules are converted
                                                        to their own libraries we
                                                        need to move this to the shared module
                                                        and avoid accessing netmgr core directly */
  struct netmgr_cmd_exec_overrides_s exec_opts;
  unsigned int skip_buffered_qos_modify; /*switch to enable/disable continuous qos modify events*/
};

extern struct netmgr_main_cfg_s netmgr_main_cfg;
extern pthread_mutex_t netmgr_main_cfg_lock;

typedef struct netmgr_ctl_port_config_s
{
  char              data_ctl_port[NETMGR_CFG_PARAM_LEN];
  char              qmi_conn_id[NETMGR_CFG_CONNID_LEN];
  netmgr_link_id_t  link_id;
  boolean           modem_wait;
  boolean           enabled;
  boolean           v4_qos_rules_enabled;
  boolean           v6_qos_rules_enabled;
}netmgr_ctl_port_config_type;

extern netmgr_ctl_port_config_type netmgr_ctl_port_array[NETMGR_MAX_LINK+1];

#ifdef FEATURE_DATA_IWLAN
  #define NETMGR_MAIN_GET_INST_MIN(MODEM)                         \
    netmgr_main_dev_prefix_tbl[MODEM][NETMGR_FWD_LINK].inst_min
  #define NETMGR_MAIN_GET_INST_MAX(MODEM)                         \
    netmgr_main_dev_prefix_tbl[MODEM][NETMGR_FWD_LINK].inst_max
  #define NETMGR_MAIN_GET_INST_MIN_REV(MODEM)                     \
    netmgr_main_dev_prefix_tbl[MODEM][NETMGR_REV_LINK].inst_min
  #define NETMGR_MAIN_GET_INST_MAX_REV(MODEM)                     \
    netmgr_main_dev_prefix_tbl[MODEM][NETMGR_REV_LINK].inst_max
  #define NETMGR_MAIN_GET_DEV_PREFIX(MODEM)                       \
    netmgr_main_dev_prefix_tbl[MODEM][NETMGR_FWD_LINK].prefix
  #define NETMGR_MAIN_GET_DEV_REV_PREFIX(MODEM)                   \
    netmgr_main_dev_prefix_tbl[MODEM][NETMGR_REV_LINK].prefix
#else
  #define NETMGR_MAIN_GET_INST_MIN(MODEM)                         \
    netmgr_main_dev_prefix_tbl[MODEM].inst_min
  #define NETMGR_MAIN_GET_INST_MAX(MODEM)                         \
    netmgr_main_dev_prefix_tbl[MODEM].inst_max
  #define NETMGR_MAIN_GET_DEV_PREFIX(MODEM)                       \
    netmgr_main_dev_prefix_tbl[MODEM].prefix
#endif /* FEATURE_DATA_IWLAN */

/*==========================================================================
                             PUBLIC FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION:  netmgr_config_init
===========================================================================*/
/*!
    @brief
    Initialize the configration framework. Things that don't belong in actual
    configuration loading.
*/
/*=========================================================================*/
void
netmgr_config_init();

/*===========================================================================
  FUNCTION:  netmgr_config_load
===========================================================================*/
/*!
    @brief
    This function loads the configuration based on the config file and
    "target" property specified.

    @param[in] xml_file: XML file location string.
    @param[in] target: configuration to be used within the XML file.

    @return  0 if configuration file is loaded properly.
    @return -1 if configuration file file load/parase fails.
*/
/*=========================================================================*/
int netmgr_config_load
(
  const char *xml_file,
  const char *target
);

/*===========================================================================
  FUNCTION:  netmgr_config_print
===========================================================================*/
/*!
    @brief
    This function prints the configuration currently loaded for netmgr.

    @param None.
    @return  None.
*/
/*=========================================================================*/
void netmgr_config_print();


/*===========================================================================
  FUNCTION:  int netmgr_main_cfg_update_ike_port
===========================================================================*/
/*!
    @brief
    Helper function to update a IKE port with addrs
*/
/*=========================================================================*/
int netmgr_main_cfg_update_ike_port(int port, char *local_addr, char *dest_addr);

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_get_ike_port
===========================================================================*/
/*!
    @brief
    Helper function to query the current IKE port being used
*/
/*=========================================================================*/
int netmgr_main_cfg_get_ike_port(char *local_addr, char *dest_addr);

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_set_ike_port
===========================================================================*/
/*!
    @brief
    Sets the IKE port which will be used for iWLAN call
*/
/*=========================================================================*/
void netmgr_main_cfg_set_ike_port(int port);

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_associate_local_addr_with_ike_ports
===========================================================================*/
/*!
@brief
  Update the local address for all valid IKE ports
*/
/*=========================================================================*/
void netmgr_main_cfg_associate_local_addr_with_ike_ports(char *local_addr);

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_cleanup_local_addr_for_ike_port
===========================================================================*/
/*!
@brief
  Cleanup the local address associated with all valid IKE ports
*/
/*=========================================================================*/
void netmgr_main_cfg_cleanup_local_addr_for_ike_ports(char *local_addr);

/*===========================================================================
  FUNCTION:  netmgr_main_cfg_cleanup_ike_port
===========================================================================*/
/*!
    @brief
    Cleanup entry corresponding to the IKE port
*/
/*=========================================================================*/
void netmgr_main_cfg_cleanup_ike_port(int port);

#ifdef __cplusplus
}
#endif

#endif /* NETMGR_CONFIG_H */


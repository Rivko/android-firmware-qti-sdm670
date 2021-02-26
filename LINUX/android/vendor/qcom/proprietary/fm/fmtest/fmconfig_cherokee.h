/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <pthread.h>
#include <sys/socket.h>
#include <errno.h>

#define FMCONFIG        ("wdssock")

/* Opcode OGF */
#define HCI_OGF_FM_RECV_CTRL_CMD_REQ            0x0013
#define HCI_OGF_FM_TRANS_CTRL_CMD_REQ           0x0014
#define HCI_OGF_FM_COMMON_CTRL_CMD_REQ          0x0015
#define HCI_OGF_FM_STATUS_PARAMETERS_CMD_REQ    0x0016
#define HCI_OGF_FM_TEST_CMD_REQ                 0x0017
#define HCI_OGF_FM_DIAGNOSTIC_CMD_REQ           0x003F

/* Opcode OCF */

/* HCI recv control commands opcode */
#define HCI_OCF_FM_ENABLE_RECV_REQ          0x0001
#define HCI_OCF_FM_DISABLE_RECV_REQ         0x0002
#define HCI_OCF_FM_GET_RECV_CONF_REQ        0x0003
#define HCI_OCF_FM_SET_RECV_CONF_REQ        0x0004
#define HCI_OCF_FM_SET_MUTE_MODE_REQ        0x0005
#define HCI_OCF_FM_SET_STEREO_MODE_REQ      0x0006
#define HCI_OCF_FM_SET_ANTENNA              0x0007
#define HCI_OCF_FM_SET_SIGNAL_THRESHOLD     0x0008
#define HCI_OCF_FM_GET_SIGNAL_THRESHOLD     0x0009
#define HCI_OCF_FM_GET_STATION_PARAM_REQ    0x000A
#define HCI_OCF_FM_GET_PROGRAM_SERVICE_REQ  0x000B
#define HCI_OCF_FM_GET_RADIO_TEXT_REQ       0x000C
#define HCI_OCF_FM_GET_AF_LIST_REQ          0x000D
#define HCI_OCF_FM_SEARCH_STATIONS          0x000E
#define HCI_OCF_FM_SEARCH_RDS_STATIONS      0x000F
#define HCI_OCF_FM_SEARCH_STATIONS_LIST     0x0010
#define HCI_OCF_FM_CANCEL_SEARCH            0x0011
#define HCI_OCF_FM_RDS_GRP                  0x0012
#define HCI_OCF_FM_RDS_GRP_PROCESS          0x0013
#define HCI_OCF_FM_EN_WAN_AVD_CTRL          0x0014
#define HCI_OCF_FM_EN_NOTCH_CTRL            0x0015
#define HCI_OCF_FM_SET_EVENT_MASK           0x0016
#define HCI_OCF_FM_SET_CH_DET_THRESHOLD     0x0017
#define HCI_OCF_FM_GET_CH_DET_THRESHOLD     0x0018
#define HCI_OCF_FM_SET_BLND_TBL             0x001B
#define HCI_OCF_FM_GET_BLND_TBL             0x001C
#define HCI_OCF_FM_LOW_PASS_FILTER_CTRL     0x001F

/* HCI trans control commans opcode*/
#define HCI_OCF_FM_ENABLE_TRANS_REQ         0x0001
#define HCI_OCF_FM_DISABLE_TRANS_REQ        0x0002
#define HCI_OCF_FM_GET_TRANS_CONF_REQ       0x0003
#define HCI_OCF_FM_SET_TRANS_CONF_REQ       0x0004
#define HCI_OCF_FM_RDS_RT_REQ               0x0008
#define HCI_OCF_FM_RDS_PS_REQ               0x0009

/* HCI common control commands opcode */
#define HCI_OCF_FM_TUNE_STATION_REQ         0x0001
#define HCI_OCF_FM_DEFAULT_DATA_READ        0x0002
#define HCI_OCF_FM_DEFAULT_DATA_WRITE       0x0003
#define HCI_OCF_FM_RESET                    0x0004
#define HCI_OCF_FM_GET_FEATURE_LIST         0x0005
#define HCI_OCF_FM_DO_CALIBRATION           0x0006
#define HCI_OCF_FM_SET_CALIBRATION          0x0007
#define HCI_OCF_FM_SET_SPUR_TABLE           0x0008
#define HCI_OCF_FM_GET_SPUR_TABLE           0x0009

/*HCI Status parameters commands*/
#define HCI_OCF_FM_READ_GRP_COUNTERS        0x0001

#define HCI_OCF_FM_READ_GRP_COUNTERS_EXT    0x0002

/*HCI Diagnostic commands*/
#define HCI_OCF_FM_PEEK_DATA                0x0002
#define HCI_OCF_FM_POKE_DATA                0x0003
#define HCI_OCF_FM_SSBI_PEEK_REG            0x0004
#define HCI_OCF_FM_SSBI_POKE_REG            0x0005
#define HCI_OCF_FM_STATION_DBG_PARAM        0x0007
#define HCI_FM_SET_INTERNAL_TONE_GENRATOR   0x0008
#define HCI_FM_SET_GET_RESET_AGC            0x000D

/* Command opcode pack/unpack */
#define hci_opcode_pack(ogf, ocf)    (uint16_t) (((ocf) & 0x03ff)|((ogf) << 10))
#define hci_opcode_ogf(op)    (op >> 10)
#define hci_opcode_ocf(op)    (op & 0x03ff)
#define hci_recv_ctrl_cmd_op_pack(ocf) \
         (uint16_t) hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ, ocf)
#define hci_trans_ctrl_cmd_op_pack(ocf) \
         (uint16_t) hci_opcode_pack(HCI_OGF_FM_TRANS_CTRL_CMD_REQ, ocf)
#define hci_common_cmd_op_pack(ocf) \
         (uint16_t) hci_opcode_pack(HCI_OGF_FM_COMMON_CTRL_CMD_REQ, ocf)
#define hci_status_param_op_pack(ocf)   \
         (uint16_t) hci_opcode_pack(HCI_OGF_FM_STATUS_PARAMETERS_CMD_REQ, ocf)
#define hci_diagnostic_cmd_op_pack(ocf) \
         (uint16_t) hci_opcode_pack(HCI_OGF_FM_DIAGNOSTIC_CMD_REQ, ocf)

struct fm_command_header_t {
    uint8_t  pi;                /* packet indicator */
    uint16_t opcode;
    uint8_t  len;
    uint8_t  params[];
}__attribute__((packed));

struct fm_event_header_t {
    uint8_t pi;                /* packet indicator */
    uint8_t evt_code;
    uint8_t evt_len;
    uint8_t params[];
}__attribute__((packed));

/* HCI data types */
#define RADIO_HCI_COMMAND_PKT   (0x11)
#define RADIO_HCI_EVENT_PKT     (0x14)

#define FM_COMMAND_TIMEOUT      (0x2)

struct fm_config {
    int sk_fd;
    pthread_t rd_thread;
    pthread_mutex_t evt_lock;
    pthread_cond_t evt_cond;
};

enum {
    RX_ERROR = -1,
    RX_FM_EVT_IND,
    RX_FM_HDR,
    RX_FM_DATA,
};

typedef void (*event_callback)(unsigned char *buf);

typedef enum {
    BT_SOC_DEFAULT = 0,
    BT_SOC_SMD = BT_SOC_DEFAULT,
    BT_SOC_AR3K,
    BT_SOC_ROME,
    BT_SOC_CHEROKEE,
    /* Add chipset type here */
    BT_SOC_RESERVED
} bt_soc_type;

struct fm_get_station_params {
    uint32_t station_freq;
    uint8_t serv_avble;
    uint8_t rssi;
    uint8_t stereo_prg;
    uint8_t rds_sync_status;
    uint8_t mute_mode;
    uint8_t sinr;
    uint8_t intf_det_th;
}__attribute__((packed)) ;

struct fm_tune_status {
    char subevent;
    struct fm_get_station_params stn_param;
} __attribute__ ((packed));

struct fm_search_station {
    uint32_t station_freq;
    uint8_t rds_cap;
    uint8_t pty;
    uint16_t picode;
} __attribute__ ((packed));

struct fm_srch_list_compl {
    uint8_t status;
    uint8_t num_stations_found;
    struct fm_search_station station[20];
} __attribute__ ((packed));

struct fm_radio_text {
    int16_t prgm_id;
    uint8_t prgm_type;
    uint8_t prgm_code_flag;
    uint8_t text_ab_flag;
    uint8_t radio_text[64];
} __attribute__ ((packed));

struct fm_af_list {
    uint32_t tuned_freq;
    uint16_t pi_code;
    uint8_t af_size;
    uint32_t af_list;
} __attribute__((packed));

struct fm_recv_conf_req {
    char  emphasis;
    char  ch_spacing;
    char  rds_std;
    char  hlsi;
    int   band_low_limit;
    int   band_high_limit;
} __attribute__ ((packed));

struct fm_rds_grp_req {
    int   rds_grp_enable_mask;
    int   rds_buf_size;
    char  en_rds_change_filter;
} __attribute__ ((packed));

struct fm_stereo_mode_req {
    char    stereo_mode;
    char    sig_blend;
    char    intf_blend;
    char    most_switch;
} __attribute__ ((packed)) ;

struct fm_evt_pkt {
    uint8_t PI; /* Packet Indicator*/
    uint8_t evt_code;
    uint8_t param_len;
    uint8_t param[256];
} __attribute__ ((packed));

struct fm_stereo_status {
    uint8_t stereo_prg;
} __attribute__ ((packed));

struct fm_rds_lock_status {
    uint8_t rds_capability;
} __attribute__ ((packed));

struct fm_serv_avl_status {
    uint8_t serv_avl;
    uint8_t sinr;
    uint8_t intf_det_th;
} __attribute__ ((packed));

struct fm_srch_in_prog {
    uint8_t srch_mode;
} __attribute__ ((packed));

struct fm_rds_srch_in_prog {
    uint8_t srch_rds_mode;
} __attribute__ ((packed));

struct fm_prgm_serv {
    uint16_t prgm_id;
    uint8_t prgm_type;
    uint8_t prgm_code_flag;
    uint8_t announcement_code_flag;
    uint8_t switch_code_flag;
    uint8_t ctrl_code_flag;
    uint8_t prgm_serv_num;
    uint8_t prgm_serv_name[116];
} __attribute__ ((packed));

struct fm_command_complete {
    uint8_t no_hci_cmd_pkt;
    uint16_t opcode;
    uint8_t param[256];
} __attribute__ ((packed));

struct fm_command_status {
    uint8_t status;
    uint8_t no_hci_cmd_pkt;
    uint16_t opcode;
} __attribute__ ((packed));

struct fm_srch_comp {
    uint8_t status;
} __attribute__ ((packed));

struct fm_rds_srch_comp {
    uint8_t status;
} __attribute__ ((packed));

struct fm_log_evt {
    uint8_t log_info;
    uint8_t log_seq_num;
    uint8_t log_time_stamp[7];
    uint8_t log_len;
    uint8_t log_string[235];
} __attribute__ ((packed));

struct fm_prog_type_name {
    uint16_t prgm_id;
    uint8_t prgm_type;
    uint8_t prgm_code_flag;
    uint8_t text_ab_flag;
    uint8_t prgm_type_name[8];
} __attribute__ ((packed));

struct fm_ext_cntry_code {
    uint16_t ecc_prgm_id;
    uint8_t ecc_prgm_type;
    uint8_t prgm_code_flag;
    uint8_t radio_paging_codes;
    uint8_t linkage_actuator;
    uint8_t variant_code;
    uint8_t paging;
    uint8_t ext_cntry_code;
    uint16_t prgm_item_num_code;
} __attribute__ ((packed));

struct fm_rt_plus_id {
    uint16_t prgm_id;
    uint8_t prgm_type;
    uint8_t prgm_code_flag;
    uint8_t grp;
    uint8_t rfu;
    uint8_t rt_flag;
    uint8_t cb_flag;
    uint8_t cb;
    uint8_t template_num;
} __attribute__ ((packed));

struct rt_plus_tag_info {
    uint8_t tag_cont_type;
    uint8_t tag_start_mark1;
    uint8_t tag_len_mark1;
} __attribute__ ((packed));

struct fm_rt_plus_tag {
    uint16_t prgm_id;
    uint8_t prgm_type;
    uint8_t prgm_code_flag;
    uint8_t grp;
    uint8_t item_toggle;
    uint8_t item_running;
    struct rt_plus_tag_info tag_info[2];
} __attribute__ ((packed));

struct fm_rds_rx_data {
    uint8_t num_rds_grps;
    uint8_t rds_grp_data[256];
} __attribute__ ((packed));

int cherokee_init(void);
void cherokee_shutdown(void);
int fm_enable_receiver(void);
int fm_disable_receiver(void);
int fm_srch_stations(struct fm_search_stations *srch);
int fm_search_list(struct fm_search_list_stations *s_list);
int fm_tune_station_req(int param);
int fm_set_config_receiver(struct fm_recv_conf_req *config);
int fm_set_rds_grp_process_req(int rds_grp);
int fm_rds_grp_mask_req(struct fm_rds_grp_req *rds_grp_msk);
int fm_set_antenna_req(char ant);
int fm_get_station_param_req(void);
int fm_set_stereo_mode_req(struct fm_stereo_mode_req *stereo_mode);

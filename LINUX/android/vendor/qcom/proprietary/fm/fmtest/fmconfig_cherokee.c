/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <pthread.h>
#include <cutils/sockets.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fmhalapis.h>
#include "fmconfig_cherokee.h"


struct fm_config fmconfig;
uint8_t cmd_status;
static void *reader_thread(void *arg);

static void event_notification(void) {
        pthread_mutex_lock(&fmconfig.evt_lock);
        pthread_cond_signal(&fmconfig.evt_cond);
        pthread_mutex_unlock(&fmconfig.evt_lock);
}

int connect_to_socket(char *name) {
    int sk = -1;

    if ((sk = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        printf("Socket create failed. err = %d(%s)", errno, strerror(errno));
    } else {
        if (socket_local_client_connect(sk, name,
                ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM) < 0) {
            printf("Socket failed to connect. err= %d(%s)", errno,
                    strerror(errno));
            close(sk);
            sk = -1;
        }
    }
    return sk;
}

void cherokee_shutdown(void)
{
    shutdown(fmconfig.sk_fd, SHUT_RDWR);
    if (pthread_join(fmconfig.rd_thread, NULL) < 0) {
        printf("thread join failed. err = %d(%s)", errno, strerror(errno));
    }
    pthread_mutex_destroy(&fmconfig.evt_lock);
    pthread_cond_destroy(&fmconfig.evt_cond);
}

int cherokee_init(void)
{
    pthread_mutex_init(&fmconfig.evt_lock, NULL);
    pthread_cond_init(&fmconfig.evt_cond, NULL);

    if ((fmconfig.sk_fd = connect_to_socket(FMCONFIG)) < 0)
        goto err_socket;

    if (pthread_create(&fmconfig.rd_thread, NULL, &reader_thread, NULL) < 0) {
        printf("Read Thread create failed. err =%d(%s)\n", errno,
                strerror(errno));
        goto err_thread;
    }

    return FM_SUCCESS;

err_thread:
    shutdown(fmconfig.sk_fd, SHUT_RDWR);
err_socket:
    pthread_mutex_destroy(&fmconfig.evt_lock);
    pthread_cond_destroy(&fmconfig.evt_cond);

    return FM_FAILURE;
}
static void evt_dummy_func(unsigned char *buf) {
    printf("%s: This print shouldn't come. Something is wrong\n", __func__);
}

static void evt_tune_status(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_tune_status *tune_status = NULL;

    tune_status = (struct fm_tune_status *)&evt->param ;
    printf("Tuned freq: %d\n", tune_status->stn_param.station_freq);
    event_notification();
}

static void evt_rds_lock_status(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_rds_lock_status *rds_lock =
                                (struct fm_rds_lock_status *)&evt->param;
    if (rds_lock->rds_capability == 0x00)
        printf("RDS decoder not synchronized\n");
    else
        printf("RDS decoder synchronized\n");
}

static void evt_stereo_status(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_stereo_status *stereo = (struct fm_stereo_status *)&evt->param;

    if (stereo->stereo_prg == 0x00)
        printf("Received Mono Mode\n");
    else
        printf("Received Stereo Mode\n");
}

static void evt_service_available(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_serv_avl_status *fm_serv =
                        (struct fm_serv_avl_status *)&evt->param;
}

static void evt_srch_in_prog(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_srch_in_prog *srch_prog =
                        (struct fm_srch_in_prog *)&evt->param;

    printf("Search in Progress. Mode = %d\n", srch_prog->srch_mode);
}
static void evt_srch_rds_in_prog(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_rds_srch_in_prog *rds_srch_prog =
                        (struct fm_rds_srch_in_prog *)&evt->param;

    printf("Search RDS in Progress. RDS Mode = %d\n",
                                    rds_srch_prog->srch_rds_mode);
}

static void evt_rds_rx_data(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_rds_rx_data *rds_data = (struct fm_rds_rx_data *)&evt->param;
}

static void evt_program_service(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_prgm_serv *prgm_serv = (struct fm_prgm_serv *)&evt->param;
    char serv_name[9] = {0};
    char *serv_ptr = NULL;
    int i;

    printf("Pid = %d\n", prgm_serv->prgm_id);
    printf("Ptype = %d\n", prgm_serv->prgm_type);
    printf("No. of Services = %d\n", prgm_serv->prgm_serv_num);

    serv_ptr = (char *)&prgm_serv->prgm_serv_name;
    for (i =0; i < prgm_serv->prgm_serv_num; i++) {
        strlcpy(serv_name, serv_ptr, sizeof(serv_name));
        printf("Service name is %s\n", serv_name);
        serv_ptr += 9; /*Each Program Service string will be 8 characters in length and will include NULL termination (9 characters total).*/
    }
}

static void evt_radio_text(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_radio_text *radio_text = (struct fm_radio_text *)&evt->param;
    char rt[64] = {0};
    uint8_t rt_len = 0;

    rt_len = evt->param_len - (sizeof(uint16_t) + 3 * sizeof(uint8_t));

    strlcpy(rt, (char *)radio_text, sizeof(rt));

    printf("Radio Text: %s\n", rt);
}

static void evt_af_list(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_af_list *af_list = (struct fm_af_list *)&evt->param;

    printf("AF tuned frequency= %d\n", af_list->tuned_freq);
}

static void evt_tx_rds_grp_aval(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
}

static void evt_tx_rds_grp_comp(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
}

static void evt_tx_rds_cont_grp_comp(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
}

static void evt_command_complete(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_command_complete *cmd_comp =
                            (struct fm_command_complete *)&evt->param;
    struct fm_get_station_params *params = NULL;

    if (!buf) {
        printf("%s, buffer is null\n",  __func__);
        return;
    }

    cmd_status = cmd_comp->param[0];
    switch (cmd_comp->opcode) {
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_ENABLE_RECV_REQ):
            if (cmd_status == FM_CMD_SUCCESS)
                printf("ENABLE RECIEVER SUCCESSFULL\n");
            else
                printf("Enable Receiver failed. Status= %d\n", cmd_status);
            break;
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_DISABLE_RECV_REQ):
            if (cmd_status == FM_CMD_SUCCESS)
                printf("DISABLE RECIEVER SUCCESSFULL\n");
            else
                printf("DISABLE Receiver failed. Status= %d\n", cmd_status);
            break;
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_GET_RECV_CONF_REQ):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_SET_RECV_CONF_REQ):
            break;
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_SET_MUTE_MODE_REQ):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_SET_STEREO_MODE_REQ):
            break;
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_SET_ANTENNA):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_SET_SIGNAL_THRESHOLD):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_CANCEL_SEARCH):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_RDS_GRP):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_RDS_GRP_PROCESS):
            break;
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_EN_WAN_AVD_CTRL):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_EN_NOTCH_CTRL):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_SET_CH_DET_THRESHOLD):
        case hci_common_cmd_op_pack(HCI_OCF_FM_RESET):
        case hci_diagnostic_cmd_op_pack(HCI_OCF_FM_SSBI_POKE_REG):
        case hci_diagnostic_cmd_op_pack(HCI_OCF_FM_POKE_DATA):
        case hci_diagnostic_cmd_op_pack(HCI_FM_SET_INTERNAL_TONE_GENRATOR):
        case hci_common_cmd_op_pack(HCI_OCF_FM_SET_CALIBRATION):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_SET_EVENT_MASK):
        case hci_common_cmd_op_pack(HCI_OCF_FM_SET_SPUR_TABLE):
        case hci_status_param_op_pack(HCI_OCF_FM_READ_GRP_COUNTERS):
        case hci_status_param_op_pack(HCI_OCF_FM_READ_GRP_COUNTERS_EXT):
        case hci_diagnostic_cmd_op_pack(HCI_OCF_FM_PEEK_DATA):
        case hci_diagnostic_cmd_op_pack(HCI_OCF_FM_SSBI_PEEK_REG):
        case hci_diagnostic_cmd_op_pack(HCI_FM_SET_GET_RESET_AGC):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_GET_CH_DET_THRESHOLD):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_GET_SIGNAL_THRESHOLD):
        case hci_common_cmd_op_pack(HCI_OCF_FM_DEFAULT_DATA_READ):
        case hci_common_cmd_op_pack(HCI_OCF_FM_DEFAULT_DATA_WRITE):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_GET_BLND_TBL):
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_SET_BLND_TBL):
            break;
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_GET_STATION_PARAM_REQ):
            if (cmd_status == FM_CMD_SUCCESS) {
                params = (struct fm_get_station_params *)&cmd_comp->param[1];
                printf("Current Parameters ....\n");
                printf("Station           : %d\n", params->station_freq);
                printf("Service Available : %d\n", params->serv_avble);
                printf("Rssi              : %d \n", params->rssi);
                printf("Audio mode        : %d\n",  params->stereo_prg);
                printf("Rds sync          : %d\n", params->rds_sync_status);
                printf("Mute status       : %d\n\n", params->mute_mode);
            }
            break;
        case hci_recv_ctrl_cmd_op_pack(HCI_OCF_FM_LOW_PASS_FILTER_CTRL):
        case hci_diagnostic_cmd_op_pack(HCI_OCF_FM_STATION_DBG_PARAM):
            break;
        default:
            break;
    }
    event_notification();
}

static void evt_command_status(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_command_status *cmd_status_evt =
                            (struct fm_command_status *)&evt->param;

    cmd_status = cmd_status_evt->status;
    event_notification();
}

static void evt_srch_comp(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_srch_comp *srch_comp = (struct fm_srch_comp *)&evt->param;

    cmd_status = srch_comp->status;
    printf("Search Complete.\n");
    event_notification();
}

static void evt_srch_rds_comp(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_rds_srch_comp *rds_srch_comp =
                            (struct fm_rds_srch_comp *)&evt->param;
    cmd_status = rds_srch_comp->status;
    printf("RDS Search Complete. Status = 0x%x", cmd_status);
    event_notification();
}

static void evt_srch_list_comp(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_srch_list_compl *srch_list =
                    (struct fm_srch_list_compl *)&evt->param;
    int i;

    cmd_status = srch_list->status;

    if (cmd_status == FM_CMD_SUCCESS) {
        printf("Received new search list\n");
        printf("Station_Num = %d\n", srch_list->num_stations_found);
        for (i = 0 ; i < srch_list->num_stations_found; i++) {
            printf("Freq = %d\n", srch_list->station[i].station_freq);
        }
    }
    event_notification();
}
static void evt_log(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_log_evt *log = (struct fm_log_evt *)&evt->param;
}

static void evt_prog_type_name(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_prog_type_name *prgm_type =
                            (struct fm_prog_type_name *)&evt->param;
    char pt_name[9] = {0};

    strlcpy(pt_name, (char *)prgm_type->prgm_type_name, sizeof(pt_name));

    printf("Program Type Name: %s\n", pt_name);
}

static void evt_ext_country_code(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_ext_cntry_code *ecc =
                            (struct fm_ext_cntry_code *)&evt->param;

    printf("Pid = %d\n", ecc->ecc_prgm_id);
    printf("Ptype = %d\n", ecc->ecc_prgm_type);
    printf("extended Country Code = %d\n", ecc->ext_cntry_code);
}

static void evt_rtplus_id(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_rt_plus_id *rt_plus_id =
                            (struct fm_rt_plus_id *)&evt->param;
}

static void evt_rtplus_tag(unsigned char *buf)
{
    struct fm_evt_pkt *evt = (struct fm_evt_pkt *)buf;
    struct fm_rt_plus_tag *rt_plus_tag =
                            (struct fm_rt_plus_tag *)&evt->param;
}

event_callback event_callback_func[] = {
    evt_dummy_func,             //no event at 0x00
    evt_tune_status,            //event = 0x01
    evt_rds_lock_status,
    evt_stereo_status,
    evt_service_available,
    evt_srch_in_prog,
    evt_srch_rds_in_prog,       //eventi = 0x06
    evt_dummy_func,             //no event at 0x07
    evt_rds_rx_data,
    evt_program_service,
    evt_radio_text,
    evt_af_list,                //event = 0x0B
    evt_tx_rds_grp_aval,
    evt_tx_rds_grp_comp,
    evt_tx_rds_cont_grp_comp,
    evt_command_complete,
    evt_command_status,         //event = 0x10
    evt_dummy_func,             //no event at 0x11
    evt_srch_comp,
    evt_srch_rds_comp,
    evt_srch_list_comp,
    evt_log,                    //event = 0x15
    evt_prog_type_name,
    evt_ext_country_code,
    evt_rtplus_id,
    evt_rtplus_tag
};


static void *reader_thread(void *arg) {
    fd_set rd_fds;
    int ret = FM_CMD_SUCCESS, state = RX_FM_EVT_IND;
    int sk_fd = fmconfig.sk_fd;
    unsigned char evt_buf[264] = {0};
    int len = 1, n_total = 0;

    do {
        FD_ZERO(&rd_fds);
        FD_SET(sk_fd, &rd_fds);
        ret = select(sk_fd + 1, &rd_fds, NULL, NULL, NULL);
        if (ret < 0) {
            printf("select failed. err; %d(%s)", errno, strerror(errno));
            break;
        }
        len = 1;
        n_total = 0;

        if (FD_ISSET(sk_fd, &rd_fds)) {
            while (len) {
                if ((ret = read(sk_fd, &evt_buf[n_total], len)) > 0) {
                    n_total += ret;
                    len -= ret;
                    if (len)
                        continue;
                    switch (state) {
                        case RX_FM_EVT_IND:
                            len = 2;
                            state = RX_FM_HDR;
                            break;
                        case RX_FM_HDR:
                            len = evt_buf[2];
                            state = RX_FM_DATA;
                            break;
                        case RX_FM_DATA:
                            state = RX_FM_EVT_IND;
                            len = 0;
                            ret = FM_CMD_SUCCESS;
                            break;
                        default:
                            state = RX_ERROR;
                            ret = FM_CMD_FAILURE;
                            break;
                    }
                } else {
                    ret = FM_CMD_FAILURE;
                    printf("read from socket failed\n");
                    break;
                }
            }
        }
        //TODO: parse data
        if (ret == FM_CMD_SUCCESS) {
            if (evt_buf[1] <
                (sizeof(event_callback_func)/sizeof(*event_callback_func))) {
                event_callback_func[evt_buf[1]](evt_buf);
            } else {
                printf("Wrong event received. evt = 0x%x\n", evt_buf[1]);
                break;
            }
        } else {
            printf("error while reading bytes\n");
            break;
        }
    } while(1);

    shutdown(fmconfig.sk_fd, SHUT_RDWR);
    return arg;
}


static int send_fm_cmd(uint16_t opcode, uint32_t len, void *param) {
    int p_len = 4 + len;
    int ret = FM_CMD_SUCCESS;
    int count = -1;
    struct timespec ts;

    struct fm_command_header_t *hdr =
                                (struct fm_command_header_t *) malloc(p_len);
    if (!hdr) {
        printf("hdr allocation failed\n");
        return FM_CMD_NO_RESOURCES;
    }

    hdr->pi = RADIO_HCI_COMMAND_PKT;
    hdr->opcode = opcode;
    hdr->len = len;

    if (len)
        memcpy(hdr->params, (uint8_t *)param, len);

    /* wait for event for n seconds*/
    pthread_mutex_lock(&fmconfig.evt_lock);

    cmd_status = FM_CMD_FAILURE;
    do {
        count = write(fmconfig.sk_fd, (void *)hdr, p_len);
        if (count < 0) {
            ret = FM_CMD_FAILURE;
            printf("Write failed. err= %d(%s)\n", errno, strerror(errno));
            pthread_mutex_unlock(&fmconfig.evt_lock);
            goto end;
        } else {
            p_len -= count;
        }
    } while(p_len);

    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        printf("clock_gettime error. err= %d(%s)\n", errno, strerror(errno));
        pthread_mutex_unlock(&fmconfig.evt_lock);
        ret = FM_CMD_FAILURE;
        goto end;
    }
    ts.tv_sec += FM_COMMAND_TIMEOUT;

    ret = pthread_cond_timedwait(&fmconfig.evt_cond, &fmconfig.evt_lock, &ts);

    if (ret != ETIMEDOUT)
        ret = cmd_status;

    pthread_mutex_unlock(&fmconfig.evt_lock);

end:
    free(hdr);
    return ret;
}

int fm_enable_receiver(void)
{
    uint16_t opcode = 0;

    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_ENABLE_RECV_REQ);
    return  send_fm_cmd(opcode, 0, NULL);
}

int fm_disable_receiver(void)
{
    uint16_t opcode = 0;

    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_DISABLE_RECV_REQ);
    return send_fm_cmd(opcode, 0, NULL);
}

int fm_srch_stations(struct fm_search_stations *srch)
{
    uint16_t opcode = 0;

    if (srch == NULL) {
        printf("search station param is null\n");
        return -FM_CMD_NO_RESOURCES;
    }
    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_SEARCH_STATIONS);
    return send_fm_cmd(opcode, sizeof((*srch)), srch);
}

int fm_search_list(struct fm_search_list_stations *s_list)
{
    uint16_t opcode = 0;

    if (s_list == NULL) {
        printf("%s: search list param is null\n", __func__);
        return -FM_CMD_NO_RESOURCES;
    }
    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_SEARCH_STATIONS_LIST);
    return send_fm_cmd(opcode, sizeof((*s_list)), s_list);
}

int fm_tune_station_req(int param)
{
    uint16_t opcode = 0;
    int tune_freq = param;

    printf("tune_freq: %d\n", tune_freq);
    opcode = hci_opcode_pack(HCI_OGF_FM_COMMON_CTRL_CMD_REQ,
            HCI_OCF_FM_TUNE_STATION_REQ);
    return send_fm_cmd(opcode, sizeof(tune_freq), &tune_freq);
}

int fm_set_config_receiver(struct fm_recv_conf_req *config)
{
    uint16_t opcode =0;

    if (!config) {
        printf("config param is NULL\n");
        return -FM_CMD_NO_RESOURCES;
    }

    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_SET_RECV_CONF_REQ);
    return send_fm_cmd(opcode, sizeof(*config), config);
}

int fm_set_rds_grp_process_req(int rds_grp)
{
    uint16_t opcode = 0;

    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_RDS_GRP_PROCESS);
    return send_fm_cmd(opcode, sizeof(rds_grp), &rds_grp);
}

int fm_rds_grp_mask_req(struct fm_rds_grp_req *rds_grp_msk)
{
    uint16_t opcode = 0;

    if (!rds_grp_msk) {
        printf("grp mask param is null\n");
        return -FM_CMD_NO_RESOURCES;
    }
    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_RDS_GRP);
    return send_fm_cmd(opcode, sizeof(*rds_grp_msk), rds_grp_msk);
}

int fm_set_antenna_req(char ant)
{
    uint16_t opcode = 0;

    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_SET_ANTENNA);
    return send_fm_cmd(opcode, sizeof(ant), &ant);
}

int fm_get_station_param_req(void)
{
    int opcode = 0;

    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_GET_STATION_PARAM_REQ);
    return send_fm_cmd(opcode, 0,  NULL);
}

int fm_set_stereo_mode_req(struct fm_stereo_mode_req *stereo_mode)
{
    uint16_t opcode;

    if (!stereo_mode) {
        printf("%s:param is null\n", __func__);
        return FM_CMD_NO_RESOURCES;
    }
    opcode = hci_opcode_pack(HCI_OGF_FM_RECV_CTRL_CMD_REQ,
            HCI_OCF_FM_SET_STEREO_MODE_REQ);
    return send_fm_cmd(opcode, sizeof(*stereo_mode), stereo_mode);
}

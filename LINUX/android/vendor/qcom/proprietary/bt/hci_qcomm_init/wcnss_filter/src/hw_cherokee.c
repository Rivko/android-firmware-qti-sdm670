/*
 *  Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
 *
 *  Copyright 2012 The Android Open Source Project
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you
 *  may not use this file except in compliance with the License. You may
 *  obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  permissions and limitations under the License.
 *
 */

/******************************************************************************
 *
 *  Filename:      hw_cherokee.c
 *
 *  Description:   Contains controller-specific functions, like
 *                      firmware patch download
 *                      low power mode operations
 *
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "WCNSS_FILTER"

#include <sys/socket.h>
#include <utils/Log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <stdbool.h>

#include "uart_utils.h"
#include "hw_cherokee.h"
#include "bt_qxdmlog.h"
#include "main.h"
#include "wcnss_ibs.h"
#include "bt_vendor_persist.h"

#define BLUETOOTH_MAC_ADDR_BOOT_PROPERTY "ro.boot.btmacaddr"
#define BT_VERSION_FILEPATH "/data/misc/bluedroid/bt_fw_version.txt"
#define BOARD_ID_LEN 0x5
#define MSB_NIBBLE_MASK 0xF0
#define LSB_NIBBLE_MASK 0x0F

#ifdef __cplusplus
}
#endif

#define RESERVED(p)  if(p) ALOGI( "%s: reserved param", __FUNCTION__);

int get_vs_hci_event(unsigned char *rsp);
int local_read_vs_hci_event(int fd, unsigned char* buf, int size);
int read_vs_hci_event(int fd, unsigned char* buf, int size);
#ifdef DEBUG_CRASH_SOC_WHILE_DLD
static int send_crash_command(int fd);
#endif //DEBUG_CRASH_SOC_WHILE_DLD
int get_boardid_req(int fd);
unsigned char convert2ascii(unsigned char temp);


/******************************************************************************
**  Variables
******************************************************************************/
FILE *file;
unsigned char *phdr_buffer;
unsigned char *pdata_buffer = NULL;
patch_info rampatch_patch_info;
int chipset_ver = 0;
unsigned char gTlv_type;
unsigned char gTlv_dwndCfg;
static unsigned int wipower_flag = 0;
static unsigned int wipower_handoff_ready = 0;
char *rampatch_file_path = NULL;
char *nvm_file_path = NULL;
char *fw_su_info = NULL;
unsigned short fw_su_offset =0;
unsigned char wait_vsc_evt = TRUE;
bool patch_dnld_pending = FALSE;
int dnld_fd = -1;
unsigned char board_id[BOARD_ID_LEN];
/******************************************************************************
**  Extern variables
******************************************************************************/
uint8_t vnd_local_bd_addr[6];


extern RING_BUF g_out_ringbuf;
extern RING_BUF g_out_ringbuf_swap;
extern RING_BUF g_in_ringbuf;
extern RING_BUF g_in_ringbuf_swap;

/*****************************************************************************
**   Functions
*****************************************************************************/
int do_write_with_logging (int fd, unsigned char *buf, int len)
{
    int ret = 0;
    int write_offset = 0;
    int write_len = len;
    do {
        ret = write(fd,buf+write_offset,write_len);
        if (ret < 0)
        {
            ALOGE("%s, write failed ret = %d err = %s",__func__,ret,strerror(errno));
            return -1;
        } else if (ret == 0) {
            ALOGE("%s, write failed with ret 0 err = %s",__func__,strerror(errno));
            return 0;
        } else {
            if (ret < write_len) {
                ALOGD("%s, Write pending,do write ret = %d err = %s",__func__,ret,
                       strerror(errno));
                write_len = write_len - ret;
                write_offset = ret;
            } else {
                ALOGV("Write successful");
#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
                if (is_diag_enabled() && is_snoop_log_enabled()) {
                    send_btlog_pkt(buf, ret, LOG_BT_HOST_TO_SOC);
                if (is_debug_out_ring_buf_enabled())
                    update_ring_buffer (&g_out_ringbuf, buf, ret, &g_out_ringbuf_swap, OUT_RING_BUF);
                }
#endif
#endif//DIAG_ENABLED
                break;
            }
        }
    } while(1);
    return len;
}

int read_an_hci_event(int fd, unsigned char* buf, int size) {
    int retval;
    unsigned char protocol_byte;
    unsigned char hdr[BT_EVT_HDR_SIZE];
    unsigned char packet_len;
    unsigned short tot_len;
    ALOGV("%s: >", __func__);

    if (size < 0) {
        ALOGE("%s: Invalid size: %d\n", __func__, size);
        return -1;
    }

    do {
        retval = do_read(fd, &protocol_byte, 1);
        if (retval < 0) {
           ALOGE("%s: read error", __func__);
           return -1;
        }

        if (protocol_byte == LOG_BT_EVT_PACKET_TYPE) {
            break;
        } else {
            ALOGI("%s: Got an invalid proto byte: %d", __func__, protocol_byte);
        }
    } while (1);

    retval = do_read(fd, hdr, BT_EVT_HDR_SIZE);
    if (retval < 0) {
         ALOGE("%s:error in reading hdr: %d", __func__, retval);
         return -1;
    }
    ALOGV("read scucesssssfully HDR");
    packet_len = hdr[BT_EVT_HDR_LEN_OFFSET];
    ALOGV("packet_len: %d\n", packet_len);

    buf[0] = protocol_byte;
    memcpy(buf+1, hdr, BT_EVT_HDR_SIZE);
    retval = do_read(fd, buf+BT_EVT_HDR_SIZE+1, packet_len);
    if (retval < 0) {
               ALOGE("%s:error in reading buf: %d", __func__, retval);
               retval = -1;
               return retval;
    }
    tot_len = packet_len+BT_EVT_HDR_SIZE+1;
    ALOGV("read scucesssssfully payload: tot_len: %d", tot_len);
    return tot_len;
}

int local_read_hci_event(int fd, unsigned char* buf, int size) {
    int  tot_len;

    tot_len = read_an_hci_event(fd, buf, size);
    if (tot_len < 0) {
        ALOGE("%s: Error while reading the hci event", __func__);
        return -1;
    }

    if (buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC) {
        if (buf[3] == LOG_BT_CONTROLLER_LOG) {
#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
           /* if it is RAMDUMP event OR
            * if normal logging event and diag is enabled
            */
            if (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP ||
                (is_diag_enabled() && is_snoop_log_enabled())) {
                send_btlog_pkt(buf, tot_len, LOG_BT_SOC_TO_HOST);

                if (is_debug_in_ring_buf_enabled() && buf[4] != LOG_BT_MESSAGE_TYPE_MEM_DUMP)
                    update_ring_buffer (&g_in_ringbuf, buf, tot_len, &g_in_ringbuf_swap, IN_RING_BUF);
            }

            if (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP) {
                ALOGV("It is RAMDUMP, keep looping to get more RAMDUMP events");
                //HAVE A TERMINATION
                local_read_vs_hci_event(fd, buf, size);
            } else {
                ALOGV("%s: It is an LOG packet, just dumped it to DIAG", __func__);
                return 0;
            }
#endif
#endif//DIAG_ENABLED
        }
    }
    else if (buf[1] == EVT_CMD_COMPLETE) {
            ALOGV("%s: Expected CC", __func__);
#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
            if (is_diag_enabled() && is_snoop_log_enabled()) {
                send_btlog_pkt(buf, tot_len, LOG_BT_SOC_TO_HOST);

                if (is_debug_in_ring_buf_enabled() && buf[4] != LOG_BT_MESSAGE_TYPE_MEM_DUMP)
                    update_ring_buffer (&g_in_ringbuf, buf, tot_len, &g_in_ringbuf_swap, IN_RING_BUF);
            }
#endif
#endif//DIAG_ENABLED
    } else {
        ALOGE("%s: Unexpected event : protocol byte: %d", __func__, buf[1]);
        tot_len = -1;
    }
    return tot_len;
}

int local_read_vs_hci_event(int fd, unsigned char* buf, int size) {
    int tot_len;
    bool collecting_ram_dump = false;

    do {
        tot_len = read_an_hci_event(fd, buf, size);
        if (tot_len < 0) {
            ALOGE("%s: Error while reading the hci event", __func__);
            return -1;
        }

        if (buf[1] == LOG_BT_EVT_VENDOR_SPECIFIC) {
#ifdef DIAG_ENABLED
#ifdef LOG_BT_ENABLE
            /* if it is RAMDUMP event OR
             * if normal logging event and diag is enabled
             */
            if (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP ||
                 (is_diag_enabled() && is_snoop_log_enabled()) ) {
                 send_btlog_pkt(buf, tot_len, LOG_BT_SOC_TO_HOST);

                 if (is_debug_in_ring_buf_enabled() && buf[4] != LOG_BT_MESSAGE_TYPE_MEM_DUMP)
                     update_ring_buffer (&g_in_ringbuf, buf, tot_len, &g_in_ringbuf_swap, IN_RING_BUF);
            }

            if (buf[3] == LOG_BT_CONTROLLER_LOG) {
                if (buf[4] == LOG_BT_MESSAGE_TYPE_MEM_DUMP) {
                    ALOGV("It is RAMDUMP, keep looping to get more RAMDUMP events");
                    //HAVE A TERMINATION
                    collecting_ram_dump = true;
                }
                else {
                    ALOGV("%s: It is an LOG packet, just dumped it to DIAG", __func__);
                    return 0;
                }
            }
            else
#endif
#endif//DIAG_ENABLED
            {
                ALOGV("VSC Event! good");
            }
        } else {
            ALOGE("Unexpected event! : opcode: %d", buf[1]);
            tot_len = -1;
        }

    } while (collecting_ram_dump);

    /* Check if the set patch command is successful or not */
    if(get_vs_hci_event(buf) != HCI_CMD_SUCCESS)
        return -1;

    return tot_len;
}

int get_vs_hci_event(unsigned char *rsp)
{
    int err = 0;
    unsigned char paramlen = 0;
    unsigned char EMBEDDED_MODE_CHECK = 0x02;
    FILE *btversionfile = 0;
    unsigned int soc_id = 0;
    unsigned int productid = 0;
    unsigned short patchversion = 0;
    char build_label[255];
    int build_lbl_len;
    unsigned short buildversion = 0;

    if( (rsp[EVENTCODE_OFFSET] == VSEVENT_CODE) || (rsp[EVENTCODE_OFFSET] == EVT_CMD_COMPLETE))
        ALOGI("%s: Received HCI-Vendor Specific event", __FUNCTION__);
    else {
        ALOGI("%s: Failed to receive HCI-Vendor Specific event", __FUNCTION__);
        err = -EIO;
        goto failed;
    }

    ALOGI("%s: Parameter Length: 0x%x", __FUNCTION__, paramlen = rsp[EVT_PLEN]);
    ALOGI("%s: Command response: 0x%x", __FUNCTION__, rsp[CMD_RSP_OFFSET]);
    ALOGI("%s: Response type   : 0x%x", __FUNCTION__, rsp[RSP_TYPE_OFFSET]);

    /* Check the status of the operation */
    switch ( rsp[CMD_RSP_OFFSET] )
    {
        case EDL_CMD_REQ_RES_EVT:
        ALOGI("%s: Command Request Response", __FUNCTION__);
        switch(rsp[RSP_TYPE_OFFSET])
        {
            case EDL_PATCH_VER_RES_EVT:
            case EDL_APP_VER_RES_EVT:
                ALOGI("\t Current Product ID\t\t: 0x%08x",
                    productid = (unsigned int)(rsp[PATCH_PROD_ID_OFFSET +3] << 24 |
                                        rsp[PATCH_PROD_ID_OFFSET+2] << 16 |
                                        rsp[PATCH_PROD_ID_OFFSET+1] << 8 |
                                        rsp[PATCH_PROD_ID_OFFSET]  ));

                /* Patch Version indicates FW patch version */
                ALOGI("\t Current Patch Version\t\t: 0x%04x",
                    (patchversion = (unsigned short)(rsp[PATCH_PATCH_VER_OFFSET + 1] << 8 |
                                            rsp[PATCH_PATCH_VER_OFFSET] )));

                /* ROM Build Version indicates ROM build version like 1.0/1.1/2.0 */
                ALOGI("\t Current ROM Build Version\t: 0x%04x", buildversion =
                    (int)(rsp[PATCH_ROM_BUILD_VER_OFFSET + 1] << 8 |
                                            rsp[PATCH_ROM_BUILD_VER_OFFSET] ));

                if (paramlen - 10)
                {
                    ALOGI("\t Current SOC Version\t\t: 0x%08x", soc_id =
                        (unsigned int)(rsp[PATCH_SOC_VER_OFFSET +3] << 24 |
                                                rsp[PATCH_SOC_VER_OFFSET+2] << 16 |
                                                rsp[PATCH_SOC_VER_OFFSET+1] << 8 |
                                                rsp[PATCH_SOC_VER_OFFSET]  ));
                }

                if (NULL != (btversionfile = fopen(BT_VERSION_FILEPATH, "wb"))) {
                    fprintf(btversionfile, "Bluetooth Controller Product ID    : 0x%08x\n", productid);
                    fprintf(btversionfile, "Bluetooth Controller Patch Version : 0x%04x\n", patchversion);
                    fprintf(btversionfile, "Bluetooth Controller Build Version : 0x%04x\n", buildversion);
                    fprintf(btversionfile, "Bluetooth Controller SOC Version   : 0x%08x\n", soc_id);
                    fclose(btversionfile);
                }else {
                    ALOGI("Failed to dump SOC version info. Errno:%d", errno);
                }

                chipset_ver = (productid << 24) | (buildversion ) |(soc_id );
                break;
            case EDL_TVL_DNLD_RES_EVT:
            case EDL_CMD_EXE_STATUS_EVT:
                switch (err = rsp[CMD_STATUS_OFFSET])
                    {
                    case HCI_CMD_SUCCESS:
                        ALOGI("%s: Download Packet successfully!", __FUNCTION__);
                        break;
                    case PATCH_LEN_ERROR:
                        ALOGI("%s: Invalid patch length argument passed for EDL PATCH "
                        "SET REQ cmd", __FUNCTION__);
                        break;
                    case PATCH_VER_ERROR:
                        ALOGI("%s: Invalid patch version argument passed for EDL PATCH "
                        "SET REQ cmd", __FUNCTION__);
                        break;
                    case PATCH_CRC_ERROR:
                        ALOGI("%s: CRC check of patch failed!!!", __FUNCTION__);
                        break;
                    case PATCH_NOT_FOUND:
                        ALOGI("%s: Invalid patch data!!!", __FUNCTION__);
                        break;
                    case TLV_TYPE_ERROR:
                        ALOGI("%s: TLV Type Error !!!", __FUNCTION__);
                        break;
                    default:
                        ALOGI("%s: Undefined error (0x%x)", __FUNCTION__, err);
                        break;
                    }
            break;
            case HCI_VS_GET_BUILD_VER_EVT:
                build_lbl_len = rsp[5];
                memcpy (build_label, &rsp[6], build_lbl_len);
                *(build_label+build_lbl_len) = '\0';

                ALOGI("BT SoC FW SU Build info: %s, %d", build_label, build_lbl_len);
                if (NULL != (btversionfile = fopen(BT_VERSION_FILEPATH, "a+b"))) {
                    fprintf(btversionfile, "Bluetooth Contoller SU Build info  : %s\n", build_label);
                    fclose(btversionfile);
                } else {
                    ALOGI("Failed to dump  FW SU build info. Errno:%d", errno);
                }
            break;
            case EDL_BOARD_ID_RESPONSE:
                ALOGI("%s: board id %x %x!!", __FUNCTION__, rsp[6], rsp[7]);
                if (rsp[6] <= 0x00) {
                    board_id[0] = convert2ascii ((rsp[7] & MSB_NIBBLE_MASK) >> 4);
                    board_id[1] = convert2ascii (rsp[7] & LSB_NIBBLE_MASK);
                    board_id[2] = '\0';
                } else {
                    board_id[0] = convert2ascii ((rsp[6] & MSB_NIBBLE_MASK) >> 4);
                    board_id[1] = convert2ascii (rsp[6] & LSB_NIBBLE_MASK);
                    board_id[2] = convert2ascii ((rsp[7] & MSB_NIBBLE_MASK) >> 4);
                    board_id[3] = convert2ascii (rsp[7] & LSB_NIBBLE_MASK);
                    board_id[4] = '\0';
                }
            break;
        }
        break;

        case NVM_ACCESS_CODE:
            ALOGI("%s: NVM Access Code!!!", __FUNCTION__);
            err = HCI_CMD_SUCCESS;
            break;
        case EDL_SET_BAUDRATE_RSP_EVT:
            if (rsp[BAUDRATE_RSP_STATUS_OFFSET] != BAUDRATE_CHANGE_SUCCESS)
            {
                ALOGE("%s: Set Baudrate request failed - 0x%x", __FUNCTION__,
                    rsp[CMD_STATUS_OFFSET]);
                err = -1;
            }
            break;
       case EDL_WIP_QUERY_CHARGING_STATUS_EVT:
            /* Query charging command has below return values
            0 - in embedded mode not charging
            1 - in embedded mode and charging
            2 - hadofff completed and in normal mode
            3 - no wipower supported on mtp. so irrepective of charging
            handoff command has to be sent if return values are 0 or 1.
            These change include logic to enable generic BT turn on sequence.*/
            if (rsp[4] < EMBEDDED_MODE_CHECK)
            {
               ALOGI("%s: WiPower Charging in Embedded Mode!!!", __FUNCTION__);
               wipower_handoff_ready = rsp[4];
               wipower_flag = 1;
            }
            break;
        case EDL_WIP_START_HANDOFF_TO_HOST_EVENT:
            /*TODO: rsp code 00 mean no charging
            this is going to change in FW soon*/
            if (rsp[4] == NON_WIPOWER_MODE)
            {
               ALOGE("%s: WiPower Charging hand off not ready!!!", __FUNCTION__);
            }
            break;
        case HCI_VS_GET_ADDON_FEATURES_EVENT:
            if ((rsp[4] & ADDON_FEATURES_EVT_WIPOWER_MASK))
            {
               ALOGD("%s: WiPower feature supported!!", __FUNCTION__);
               property_set("persist.bluetooth.a4wp", "true");
            }
            break;
        case HCI_VS_STRAY_EVT:
            /* WAR to handle stray Power Apply EVT during patch download */
            ALOGD("%s: Stray HCI VS EVENT", __FUNCTION__);
            if (patch_dnld_pending && dnld_fd != -1)
            {
                unsigned char rsp[HCI_MAX_EVENT_SIZE];
                memset(rsp, 0x00, HCI_MAX_EVENT_SIZE);
                local_read_vs_hci_event(dnld_fd, rsp, HCI_MAX_EVENT_SIZE);
            }
            else
            {
                ALOGE("%s: Not a valid status!!!", __FUNCTION__);
                err = -1;
            }
            break;
        default:
            ALOGE("%s: Not a valid status!!!", __FUNCTION__);
            err = -1;
            break;
    }

failed:
    return err;
}


/*
 * Read an VS HCI event from the given file descriptor.
 */
int read_vs_hci_event(int fd, unsigned char* buf, int size)
{
    int remain, r;
    int count = 0, i;

    if (size <= 0) {
        ALOGE("Invalid size arguement!");
        return -1;
    }

    ALOGI("%s: Wait for HCI-Vendor Specfic Event from SOC", __FUNCTION__);

    /* The first byte identifies the packet type. For HCI event packets, it
     * should be 0x04, so we read until we get to the 0x04. */
    /* It will keep reading until find 0x04 byte */
    while (1) {
            r = read(fd, buf, 1);
            if (r <= 0)
                    return -1;
            if (buf[0] == 0x04)
                    break;
    }
    count++;

    /* The next two bytes are the event code and parameter total length. */
    while (count < 3) {
            r = read(fd, buf + count, 3 - count);
            if ((r <= 0) || (buf[1] != 0xFF )) {
                ALOGE("It is not VS event !! ret: %d, EVT: %d", r, buf[1]);
                return -1;
            }
            count += r;
    }

    /* Now we read the parameters. */
    if (buf[2] < (size - 3))
            remain = buf[2];
    else
            remain = size - 3;

    while ((count - 3) < remain) {
            r = read(fd, buf + count, remain - (count - 3));
            if (r <= 0)
                    return -1;
            count += r;
    }

     /* Check if the set patch command is successful or not */
    if(get_vs_hci_event(buf) != HCI_CMD_SUCCESS)
        return -1;

    return count;
}

/*
 * For Hand-Off related Wipower commands, Command complete arrives first and
 * the followd with VS event
 *
 */
int hci_send_wipower_vs_cmd(int fd, unsigned char *cmd, unsigned char *rsp, int size)
{
    int ret = 0;
    int err = 0;

    /* Send the HCI command packet to UART for transmission */
    ret = do_write_with_logging(fd, cmd, size);
    if (ret != size) {
        ALOGE("%s: WP Send failed with ret value: %d", __FUNCTION__, ret);
        goto failed;
    }

    /* Wait for command complete event */
    err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to charging status cmd on Controller", __FUNCTION__);
        goto failed;
    }

    ALOGI("%s: WP Received HCI command complete Event from SOC", __FUNCTION__);
failed:
    return ret;
}


int hci_send_vs_cmd(int fd, unsigned char *cmd, unsigned char *rsp, int size)
{
    int ret = 0;

    /* Send the HCI command packet to UART for transmission */
    ret = do_write_with_logging(fd, cmd, size);
    if (ret != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, ret);
        goto failed;
    }

    if (wait_vsc_evt) {
        /* Check for response from the Controller */
        if (local_read_vs_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE) < 0) {
           ret = -ETIMEDOUT;
           ALOGI("%s: Failed to get HCI-VS Event from SOC", __FUNCTION__);
           goto failed;
        }
        ALOGI("%s: Received HCI-Vendor Specific Event from SOC", __FUNCTION__);
    }

failed:
    return ret;
}

void frame_hci_cmd_pkt(
    unsigned char *cmd,
    int edl_cmd, unsigned int p_base_addr,
    int segtNo, int size
    )
{
    int offset = 0;
    hci_command_hdr *cmd_hdr;

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);

    cmd[0]      = HCI_COMMAND_PKT;
    cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_PATCH_CMD_OCF);
    cmd_hdr->plen   = size;
    cmd[4]      = edl_cmd;

    switch (edl_cmd)
    {
        case EDL_PATCH_SET_REQ_CMD:
            /* Copy the patch header info as CMD params */
            memcpy(&cmd[5], phdr_buffer, PATCH_HDR_LEN);
            ALOGD("%s: Sending EDL_PATCH_SET_REQ_CMD", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
                segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_PATCH_DLD_REQ_CMD:
            offset = ((segtNo - 1) * MAX_DATA_PER_SEGMENT);
            p_base_addr += offset;
            cmd_hdr->plen   = (size + 6);
            cmd[5]  = (size + 4);
            cmd[6]  = EXTRACT_BYTE(p_base_addr, 0);
            cmd[7]  = EXTRACT_BYTE(p_base_addr, 1);
            cmd[8]  = EXTRACT_BYTE(p_base_addr, 2);
            cmd[9]  = EXTRACT_BYTE(p_base_addr, 3);
            memcpy(&cmd[10], (pdata_buffer + offset), size);

            ALOGV("%s: Sending EDL_PATCH_DLD_REQ_CMD: size: %d bytes",
                __FUNCTION__, size);
            ALOGV("HCI-CMD %d:\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t0x%x\t"
                "0x%x\t0x%x\t0x%x\t\n", segtNo, cmd[0], cmd[1], cmd[2],
                cmd[3], cmd[4], cmd[5], cmd[6], cmd[7], cmd[8], cmd[9]);
            break;
        case EDL_PATCH_ATCH_REQ_CMD:
            ALOGD("%s: Sending EDL_PATCH_ATTACH_REQ_CMD", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_PATCH_RST_REQ_CMD:
            ALOGD("%s: Sending EDL_PATCH_RESET_REQ_CMD", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_PATCH_VER_REQ_CMD:
            ALOGD("%s: Sending EDL_PATCH_VER_REQ_CMD", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_PATCH_TLV_REQ_CMD:
            ALOGV("%s: Sending EDL_PATCH_TLV_REQ_CMD", __FUNCTION__);
            /* Parameter Total Length */
            cmd[3] = size +2;

            /* TLV Segment Length */
            cmd[5] = size;
            ALOGV("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
            segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5]);
            offset = (segtNo * MAX_SIZE_PER_TLV_SEGMENT);
            memcpy(&cmd[6], (pdata_buffer + offset), size);
            break;
        case EDL_GET_BUILD_INFO:
            ALOGD("%s: Sending EDL_GET_BUILD_INFO", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
                segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        case EDL_GET_BOARD_ID:
            ALOGD("%s: Sending EDL_GET_BOARD_ID", __FUNCTION__);
            ALOGD("HCI-CMD %d:\t0x%x \t0x%x \t0x%x \t0x%x \t0x%x",
                segtNo, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
            break;
        default:
            ALOGE("%s: Unknown EDL CMD !!!", __FUNCTION__);
    }
}

void extract_patch_header_info(unsigned char *buf)
{
    int index;

    /* Extract patch id */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_id |=
            (LSH(buf[index + P_ID_OFFSET], (index * 8)));

    /* Extract (ROM and BUILD) version information */
    for (index = 0; index < 2; index++)
        rampatch_patch_info.patch_ver.rom_version |=
            (LSH(buf[index + P_CHIP_VER_OFFSET], (index * 8)));

    for (index = 0; index < 2; index++)
        rampatch_patch_info.patch_ver.build_version |=
            (LSH(buf[index + P_BUILD_VER_OFFSET], (index * 8)));

    /* Extract patch base and entry addresses */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_base_addr |=
            (LSH(buf[index + P_BASE_ADDR_OFFSET], (index * 8)));

    /* Patch BASE & ENTRY addresses are same */
    rampatch_patch_info.patch_entry_addr = rampatch_patch_info.patch_base_addr;

    /* Extract total length of the patch payload */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_length |=
            (LSH(buf[index + P_LEN_OFFSET], (index * 8)));

    /* Extract the CRC checksum of the patch payload */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_crc |=
            (LSH(buf[index + P_CRC_OFFSET], (index * 8)));

    /* Extract patch control value */
    for (index = 0; index < 4; index++)
        rampatch_patch_info.patch_ctrl |=
            (LSH(buf[index + P_CONTROL_OFFSET], (index * 8)));

    ALOGI("PATCH_ID\t : 0x%x", rampatch_patch_info.patch_id);
    ALOGI("ROM_VERSION\t : 0x%x", rampatch_patch_info.patch_ver.rom_version);
    ALOGI("BUILD_VERSION\t : 0x%x", rampatch_patch_info.patch_ver.build_version);
    ALOGI("PATCH_LENGTH\t : 0x%x", rampatch_patch_info.patch_length);
    ALOGI("PATCH_CRC\t : 0x%x", rampatch_patch_info.patch_crc);
    ALOGI("PATCH_CONTROL\t : 0x%x\n", rampatch_patch_info.patch_ctrl);
    ALOGI("PATCH_BASE_ADDR\t : 0x%x\n", rampatch_patch_info.patch_base_addr);

}

int edl_set_patch_request(int fd)
{
    int size, err;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    /* Frame the HCI CMD to be sent to the Controller */
    frame_hci_cmd_pkt(cmd, EDL_PATCH_SET_REQ_CMD, 0,
        -1, PATCH_HDR_LEN + 1);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

    /* Send HCI Command packet to Controller */
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to set the patch info to the Controller!");
        goto error;
    }

    err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to set patch info on Controller", __FUNCTION__);
        goto error;
    }
    ALOGI("%s: Successfully set patch info on the Controller", __FUNCTION__);
error:
    return err;
}

int edl_patch_download_request(int fd)
{
    int no_of_patch_segment;
    int index = 1, err = 0, size = 0;
    unsigned int p_base_addr;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    no_of_patch_segment = (rampatch_patch_info.patch_length /
        MAX_DATA_PER_SEGMENT);
    ALOGI("%s: %d patch segments to be d'loaded from patch base addr: 0x%x",
        __FUNCTION__, no_of_patch_segment,
    rampatch_patch_info.patch_base_addr);

    /* Initialize the patch base address from the one read from bin file */
    p_base_addr = rampatch_patch_info.patch_base_addr;

    /*
    * Depending upon size of the patch payload, download the patches in
    * segments with a max. size of 239 bytes
    */
    for (index = 1; index <= no_of_patch_segment; index++) {

        ALOGI("%s: Downloading patch segment: %d", __FUNCTION__, index);

        /* Frame the HCI CMD PKT to be sent to Controller*/
        frame_hci_cmd_pkt(cmd, EDL_PATCH_DLD_REQ_CMD, p_base_addr,
        index, MAX_DATA_PER_SEGMENT);

        /* Total length of the packet to be sent to the Controller */
        size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

        /* Initialize the RSP packet everytime to 0 */
        memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

        /* Send HCI Command packet to Controller */
        err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
        if ( err != size) {
            ALOGE("Failed to send the patch payload to the Controller!");
            goto error;
        }

        /* Read Command Complete Event */
        err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if ( err < 0) {
            ALOGE("%s: Failed to downlaod patch segment: %d!",
            __FUNCTION__, index);
            goto error;
        }
        ALOGI("%s: Successfully downloaded patch segment: %d",
        __FUNCTION__, index);
    }

    /* Check if any pending patch data to be sent */
    size = (rampatch_patch_info.patch_length < MAX_DATA_PER_SEGMENT) ?
        rampatch_patch_info.patch_length :
        (rampatch_patch_info.patch_length  % MAX_DATA_PER_SEGMENT);

    if (size)
    {
        /* Frame the HCI CMD PKT to be sent to Controller*/
        frame_hci_cmd_pkt(cmd, EDL_PATCH_DLD_REQ_CMD, p_base_addr, index, size);

        /* Initialize the RSP packet everytime to 0 */
        memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

        /* Total length of the packet to be sent to the Controller */
        size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

        /* Send HCI Command packet to Controller */
        err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
        if ( err != size) {
            ALOGE("Failed to send the patch payload to the Controller!");
            goto error;
        }

        /* Read Command Complete Event */
        err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if ( err < 0) {
            ALOGE("%s: Failed to downlaod patch segment: %d!",
                __FUNCTION__, index);
            goto error;
        }

        ALOGI("%s: Successfully downloaded patch segment: %d",
        __FUNCTION__, index);
    }

error:
    return err;
}

unsigned char is_sibs_enabled ()
{
   char value[PROPERTY_VALUE_MAX] = {'\0'};
   property_get("persist.service.bdroid.sibs", value, "true");
   return ((strcmp(value, "true") == 0)? TRUE: FALSE);
}

int get_tlv_file(char *file_path)
{
    FILE * pFile;
    long fileSize;
    int readSize, err = 0, total_segment, remain_size, nvm_length, nvm_tot_len, nvm_index, i;
    unsigned short nvm_tag_len;
    tlv_patch_info *ptlv_header;
    tlv_nvm_hdr *nvm_ptr;
    unsigned char data_buf[PRINT_BUF_SIZE]={0,};
    unsigned char *nvm_byte_ptr;

    ALOGI("File Open (%s)", file_path);
    pFile = fopen ( file_path , "r" );
    if (pFile==NULL) {;
        ALOGE("%s File Open Fail", file_path);
        return -1;
    }

    /* Get File Size */
    fseek (pFile , 0 , SEEK_END);
    fileSize = ftell (pFile);
    rewind (pFile);

    pdata_buffer = (unsigned char*) malloc (sizeof(char)*fileSize);
    if (pdata_buffer == NULL) {
        ALOGE("Allocated Memory failed");
        fclose (pFile);
        return -1;
    }

    /* Copy file into allocated buffer */
    readSize = fread (pdata_buffer,1,fileSize,pFile);

    /* File Close */
    fclose (pFile);

    if (readSize != fileSize) {
        ALOGE("Read file size(%d) not matched with actual file size (%ld bytes)",readSize,fileSize);
        return -1;
    }

    ptlv_header = (tlv_patch_info *) pdata_buffer;

    /* To handle different event between rampatch and NVM */
    gTlv_type = ptlv_header->tlv_type;
    gTlv_dwndCfg = ptlv_header->tlv.patch.dwnd_cfg;

    if(ptlv_header->tlv_type == TLV_TYPE_PATCH){
        ALOGI("====================================================");
        ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
        ALOGI("Length\t\t\t : %d bytes", (ptlv_header->tlv_length1) |
                                                    (ptlv_header->tlv_length2 << 8) |
                                                    (ptlv_header->tlv_length3 << 16));
        ALOGI("Total Length\t\t\t : %d bytes", ptlv_header->tlv.patch.tlv_data_len);
        ALOGI("Patch Data Length\t\t\t : %d bytes",ptlv_header->tlv.patch.tlv_patch_data_len);
        ALOGI("Signing Format Version\t : 0x%x", ptlv_header->tlv.patch.sign_ver);
        ALOGI("Signature Algorithm\t\t : 0x%x", ptlv_header->tlv.patch.sign_algorithm);
        ALOGI("Event Handling\t\t\t : 0x%x", ptlv_header->tlv.patch.dwnd_cfg);
        ALOGI("Reserved\t\t\t : 0x%x", ptlv_header->tlv.patch.reserved1);
        ALOGI("Product ID\t\t\t : 0x%04x\n", ptlv_header->tlv.patch.prod_id);
        ALOGI("Rom Build Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.build_ver);
        ALOGI("Patch Version\t\t : 0x%04x\n", ptlv_header->tlv.patch.patch_ver);
        ALOGI("Reserved\t\t\t : 0x%x\n", ptlv_header->tlv.patch.reserved2);
        ALOGI("Patch Entry Address\t\t : 0x%x\n", (ptlv_header->tlv.patch.patch_entry_addr));
        ALOGI("====================================================");

    }else if( (ptlv_header->tlv_type >= TLV_TYPE_BT_NVM) &&
            ((ptlv_header->tlv_type <= TLV_TYPE_BT_FM_NVM)) ){
        ALOGI("====================================================");
        ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
        ALOGI("Length\t\t\t : %d bytes",  nvm_tot_len = nvm_length  = (ptlv_header->tlv_length1) |
                                                    (ptlv_header->tlv_length2 << 8) |
                                                    (ptlv_header->tlv_length3 << 16));
        ALOGI("====================================================");

        if(nvm_tot_len <= 0)
            return readSize;

        if(ptlv_header->tlv_type == TLV_TYPE_BT_FM_NVM) {
             nvm_byte_ptr = ptlv_header;
             nvm_byte_ptr +=4;
             ptlv_header = nvm_byte_ptr;
             nvm_tot_len -=4;
             ALOGI("====================================================");
             ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
             ALOGI("Length\t\t\t : %d bytes",  nvm_length = (ptlv_header->tlv_length1) |
                                                         (ptlv_header->tlv_length2 << 8) |
                                                         (ptlv_header->tlv_length3 << 16));
             ALOGI("====================================================");
        }

multi_nvm:
       for(nvm_byte_ptr=(unsigned char *)(nvm_ptr = &(ptlv_header->tlv.nvm)), nvm_index=0;
             nvm_index < nvm_length ; nvm_ptr = (tlv_nvm_hdr *) nvm_byte_ptr)
       {
            ALOGI("TAG ID\t\t\t : %d", nvm_ptr->tag_id);
            ALOGI("TAG Length\t\t : %d", nvm_tag_len = nvm_ptr->tag_len);
            ALOGI("TAG Pointer\t\t : %d", nvm_ptr->tag_ptr);
            ALOGI("TAG Extended Flag\t : %d", nvm_ptr->tag_ex_flag);

            /* Increase nvm_index to NVM data */
            nvm_index+=sizeof(tlv_nvm_hdr);
            nvm_byte_ptr+=sizeof(tlv_nvm_hdr);
            if(ptlv_header->tlv_type == TLV_TYPE_BT_NVM) {
                /* Write BD Address */
                if(nvm_ptr->tag_id == TAG_NUM_2){
                    memcpy(nvm_byte_ptr, vnd_local_bd_addr, 6);
                    ALOGI("BD Address: %.02x:%.02x:%.02x:%.02x:%.02x:%.02x",
                        *nvm_byte_ptr, *(nvm_byte_ptr+1), *(nvm_byte_ptr+2),
                        *(nvm_byte_ptr+3), *(nvm_byte_ptr+4), *(nvm_byte_ptr+5));
                }

                /* Change SIBS setting */
                if(!is_sibs_enabled()) {
                    if(nvm_ptr->tag_id == 17){
                        *nvm_byte_ptr = ((*nvm_byte_ptr) & ~(0x80));
                    }

                    if(nvm_ptr->tag_id == 27){
                        *nvm_byte_ptr = ((*nvm_byte_ptr) & ~(0x01));
                    }
                }
                /* Change Baudrate setting based on Host setting */
                if(nvm_ptr->tag_id == 17) {
                    *(nvm_byte_ptr+2) = BT_BAUD_RATE;
                }

            }
            for(i =0;(i<nvm_ptr->tag_len && (i*3 + 2) <PRINT_BUF_SIZE);i++)
                snprintf((char *) data_buf, PRINT_BUF_SIZE, "%s%.02x ", (char *)data_buf, *(nvm_byte_ptr + i));

            ALOGI("TAG Data\t\t\t : %s", data_buf);

            /* Clear buffer */
            memset(data_buf, 0x0, PRINT_BUF_SIZE);

            /* increased by tag_len */
            nvm_index+=nvm_ptr->tag_len;
            nvm_byte_ptr +=nvm_ptr->tag_len;
        }

        nvm_tot_len -= nvm_index;

        if (nvm_tot_len > 4 ) {
            nvm_byte_ptr = ptlv_header;
            nvm_byte_ptr +=(4+nvm_index);
            ptlv_header = nvm_byte_ptr;
            nvm_tot_len -=4;
            ALOGI("====================================================");
            ALOGI("TLV Type\t\t\t : 0x%x", ptlv_header->tlv_type);
            ALOGI("Length\t\t\t : %d bytes",  nvm_length = (ptlv_header->tlv_length1) |
                                                        (ptlv_header->tlv_length2 << 8) |
                                                        (ptlv_header->tlv_length3 << 16));
            ALOGI("====================================================");
            goto multi_nvm;
        }

        ALOGI("====================================================");

    } else {
        ALOGI("TLV Header type is unknown (%d) ", ptlv_header->tlv_type);
    }

    return readSize;
}

int tlv_dnld_segment(int fd, int index, int seg_size, unsigned char wait_cc_evt)
{
    int size=0, err = -1;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    ALOGI("%s: Downloading TLV Patch segment no.%d, size:%d", __FUNCTION__, index, seg_size);

    /* Frame the HCI CMD PKT to be sent to Controller*/
    frame_hci_cmd_pkt(cmd, EDL_PATCH_TLV_REQ_CMD, 0, index, seg_size);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + cmd[PLEN]);

    /* Initialize the RSP packet everytime to 0 */
    memset(rsp, 0x0, HCI_MAX_EVENT_SIZE);

    /* Send HCI Command packet to Controller */
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to send the patch payload to the Controller! 0x%x", err);
        return err;
    }

    if(wait_cc_evt) {
        err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if ( err < 0) {
            ALOGE("%s: Failed to downlaod patch segment: %d!",  __FUNCTION__, index);
            return err;
        }
    }

    ALOGV("%s: Successfully downloaded patch segment: %d", __FUNCTION__, index);
    return err;
}

int tlv_dnld_req(int fd, int tlv_size)
{
    int  total_segment, remain_size, i, err = -1;
    unsigned char wait_cc_evt = TRUE;

    total_segment = tlv_size/MAX_SIZE_PER_TLV_SEGMENT;
    remain_size = (tlv_size < MAX_SIZE_PER_TLV_SEGMENT)?\
        tlv_size: (tlv_size%MAX_SIZE_PER_TLV_SEGMENT);

    ALOGI("%s: TLV size: %d, Total Seg num: %d, remain size: %d",
        __FUNCTION__,tlv_size, total_segment, remain_size);

    if (gTlv_type == TLV_TYPE_PATCH) {
       /* Prior to Rome version 3.2(including inital few rampatch release of Rome 3.2), the event
        * handling mechanism is SKIP_EVT_NONE. After few release of rampatch for Rome 3.2, the
        * mechamism is changed to SKIP_EVT_VSE_CC. Rest of the mechanism is not used for now
        */
       switch(gTlv_dwndCfg)
       {
           case SKIP_EVT_NONE:
              wait_vsc_evt = TRUE;
              wait_cc_evt = TRUE;
              ALOGI("Event handling type: SKIP_EVT_NONE");
              break;
           case SKIP_EVT_VSE_CC:
              wait_vsc_evt = FALSE;
              wait_cc_evt = FALSE;
              ALOGI("Event handling type: SKIP_EVT_VSE_CC");
              break;
           /* Not handled for now */
           case SKIP_EVT_VSE:
           case SKIP_EVT_CC:
           default:
              ALOGE("Unsupported Event handling: %d", gTlv_dwndCfg);
              break;
       }
    } else {
        wait_vsc_evt = TRUE;
        wait_cc_evt = TRUE;
    }

    for(i=0;i<total_segment ;i++){
        if ((i+1) == total_segment) {
             if ((gTlv_type == TLV_TYPE_PATCH)) {
                /*
                 * 1. None of the command segments receive CCE
                 * 2. No command segments receive VSE except the last one
                 * 3. If gTlv_dwndCfg is SKIP_EVT_NONE then wait for everything
                 */
                 if (gTlv_dwndCfg == SKIP_EVT_NONE) {
                    wait_cc_evt = !remain_size ? FALSE: TRUE;
                 } else if (gTlv_dwndCfg == SKIP_EVT_VSE_CC) {
                    wait_vsc_evt = !remain_size ? TRUE: FALSE;
                 }
             }
        }

        patch_dnld_pending = TRUE;
        if((err = tlv_dnld_segment(fd, i, MAX_SIZE_PER_TLV_SEGMENT, wait_cc_evt )) < 0)
            goto error;
        patch_dnld_pending = FALSE;
    }

    if ((gTlv_type == TLV_TYPE_PATCH)) {
        /*
         * 1. None of the command segments receive CCE
         * 2. No command segments receive VSE except the last one
         * 3. If gTlv_dwndCfg is SKIP_EVT_NONE then the logic is
         *    same as Rome 2.1, 2.2, 3.0
         */
        if (gTlv_dwndCfg == SKIP_EVT_NONE) {
           wait_cc_evt = remain_size ? FALSE: TRUE;
        } else if (gTlv_dwndCfg == SKIP_EVT_VSE_CC) {
           wait_vsc_evt = remain_size ? TRUE: FALSE;
        }
    }
    patch_dnld_pending = TRUE;
    if(remain_size) err =tlv_dnld_segment(fd, i, remain_size, wait_cc_evt);
    patch_dnld_pending = FALSE;
error:
    if(patch_dnld_pending) patch_dnld_pending = FALSE;
    return err;
}

int download_tlv_file(int fd)
{
    int tlv_size, err = -1;
    char nvm_file_path_bid[32] = { 0,};

    memcpy(nvm_file_path_bid, nvm_file_path, strlen(nvm_file_path) - 2);

    /* Rampatch TLV file Downloading */
    pdata_buffer = NULL;
    if((tlv_size = get_tlv_file(rampatch_file_path)) <= 0) {
        ALOGI("%s: rampatch file is not available", __FUNCTION__);
        goto nvm_download;
    }

    if((err = tlv_dnld_req(fd, tlv_size)) <0 )
        goto error;

    if (pdata_buffer != NULL){
        free (pdata_buffer);
        pdata_buffer = NULL;
    }

#ifdef DEBUG_CRASH_SOC_WHILE_DLD
    {   char trial_prop[PROPERTY_VALUE_MAX];
        int value = 0;
        property_get("persist.bluetooth.csoc.cnt", trial_prop, "0");
        value = atoi(trial_prop);
        ALOGV("%s: value : %d\n", __func__, value);
        if (value > 0) {
            sprintf(trial_prop, "%d", (value=value-1));
            property_set("persist.bluetooth.csoc.cnt", trial_prop);
            /*send crash command only for 1st time*/
            send_crash_command(fd);
        }
     }
#endif//DEBUG_CRASH_SOC_WHILE_DLD

nvm_download:
    if (get_boardid_req(fd) < 0) {
        ALOGE("%s: failed to get board id(0x%x)", __FUNCTION__, err);
        goto default_download;
    }

    strlcat(nvm_file_path_bid, (char*)board_id, sizeof(nvm_file_path_bid));
    if((tlv_size = get_tlv_file(nvm_file_path_bid)) < 0) {
        ALOGI("%s: %s: file doesn't exist, falling back to default file", __FUNCTION__, nvm_file_path_bid);
default_download:
        /* NVM TLV file Downloading */
        if((tlv_size = get_tlv_file(nvm_file_path)) < 0)
            goto error;
    }

    if((err = tlv_dnld_req(fd, tlv_size)) <0 )
        goto error;

error:
    if (pdata_buffer != NULL)
        free (pdata_buffer);

    return err;
}

int patch_ver_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    /* Frame the HCI CMD to be sent to the Controller */
    frame_hci_cmd_pkt(cmd, EDL_PATCH_VER_REQ_CMD, 0,
    -1, EDL_PATCH_CMD_LEN);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

    /* Send HCI Command packet to Controller */
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to attach the patch payload to the Controller!");
        goto error;
    }

    /* Read Command Complete Event*/
    err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to get patch version(s)", __FUNCTION__);
        goto error;
    }
error:
    return err;

}

int get_build_info_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    /* Frame the HCI CMD to be sent to the Controller */
    frame_hci_cmd_pkt(cmd, EDL_GET_BUILD_INFO, 0,
    -1, EDL_PATCH_CMD_LEN);

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

    /* Send HCI Command packet to Controller */
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to send get build info cmd to the SoC!");
        goto error;
    }

    err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to get build info", __FUNCTION__);
        goto error;
    }
error:
    return err;

}


int set_baudrate_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_SET_BAUDRATE_CMD_OCF);
    cmd_hdr->plen     = VSC_SET_BAUDRATE_REQ_LEN;
    cmd[4]  = BT_BAUD_RATE;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + VSC_SET_BAUDRATE_REQ_LEN);

    /* Flow off during baudrate change */
    if ((err = userial_vendor_ioctl(fd, USERIAL_OP_FLOW_OFF , &flags)) < 0)
    {
      ALOGE("%s: HW Flow-off error: 0x%x\n", __FUNCTION__, err);
      goto error;
    }

    /* Send the HCI command packet to UART for transmission */
    err = do_write_with_logging(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        goto error;
    }

    tcdrain(fd);
    /* Change Local UART baudrate to high speed UART */
    userial_vendor_set_baud(fd, BT_BAUD_RATE);

    /* Check current Baudrate */
    userial_vendor_get_baud(fd);

    /* Flow on after changing local uart baudrate */
    if ((err = userial_vendor_ioctl(fd, USERIAL_OP_FLOW_ON , &flags)) < 0)
    {
        ALOGE("%s: HW Flow-on error: 0x%x \n", __FUNCTION__, err);
        return err;
    }

    /* Check for response from the Controller */
    if ((err =local_read_vs_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE)) < 0) {
            ALOGE("%s: Failed to get HCI-VS Event from SOC", __FUNCTION__);
            goto error;
    }

    ALOGI("%s: Received HCI-Vendor Specific Event from SOC", __FUNCTION__);

    /* Wait for command complete event */
    err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to set patch info on Controller", __FUNCTION__);
        goto error;
    }

error:
    return err;

}

int hci_reset(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    ALOGI("%s: HCI RESET ", __FUNCTION__);

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = HCI_RESET;
    cmd_hdr->plen   = 0;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);
    err = do_write_with_logging(fd, cmd, size);
    if (err != size) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, err);
        err = -1;
        goto error;
    }

    /* Wait for command complete event */
    err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if ( err < 0) {
        ALOGE("%s: Failed to set patch info on Controller", __FUNCTION__);
        goto error;
    }

error:
    return err;

}

int wipower_current_charging_status_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_WIPOWER_VS_CMD_OCF);
    cmd_hdr->plen     = EDL_WIP_QUERY_CHARGING_STATUS_LEN;
    cmd[4]  = EDL_WIP_QUERY_CHARGING_STATUS_CMD;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_WIP_QUERY_CHARGING_STATUS_LEN);

    ALOGD("%s: Sending EDL_WIP_QUERY_CHARGING_STATUS_CMD", __FUNCTION__);
    ALOGD("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);

    err = hci_send_wipower_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to send EDL_WIP_QUERY_CHARGING_STATUS_CMD command!");
        goto error;
    }

    /* Check for response from the Controller */
    if (local_read_vs_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE) < 0) {
        err = -ETIMEDOUT;
        ALOGI("%s: WP Failed to get HCI-VS Event from SOC", __FUNCTION__);
        goto error;
    }

    /* Read Command Complete Event */
    if (rsp[4] > NON_WIPOWER_MODE) {
        err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if (err < 0) {
            ALOGE("%s: Failed to get charging status", __FUNCTION__);
            goto error;
        }
    }

error:
    return err;
}

int get_boardid_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    bool cmd_supported = TRUE;

    /* Frame the HCI CMD to be sent to the Controller */
    frame_hci_cmd_pkt(cmd, EDL_GET_BOARD_ID, 0,
    -1, EDL_PATCH_CMD_LEN);
    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_PATCH_CMD_LEN);

    ALOGI("%s: Sending EDL_GET_BOARD_ID", __FUNCTION__);
    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to send EDL_GET_BOARD_ID command!");
        cmd_supported = FALSE;
    }

    err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if (err < 0) {
        ALOGE("%s: Failed to get feature request", __FUNCTION__);
        goto error;
    }
error:
    return (cmd_supported == TRUE? err: -1);
}


int addon_feature_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, HCI_VS_GET_ADDON_FEATURES_SUPPORT);
    cmd_hdr->plen     = 0x00;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE);

    ALOGD("%s: Sending HCI_VS_GET_ADDON_FEATURES_SUPPORT", __FUNCTION__);
    ALOGD("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3]);

    err = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to send HCI_VS_GET_ADDON_FEATURES_SUPPORT command!");
        goto error;
    }

    err = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
    if (err < 0) {
        ALOGE("%s: Failed to get feature request", __FUNCTION__);
        goto error;
    }
error:
    return err;
}


int check_embedded_mode(int fd) {
    int err = 0;

    wipower_flag = 0;
    /* Get current wipower charging status */
    if ((err = wipower_current_charging_status_req(fd)) < 0)
    {
        ALOGI("%s: Wipower status req failed (0x%x)", __FUNCTION__, err);
    }
    usleep(500);

    ALOGE("%s: wipower_flag: %d", __FUNCTION__, wipower_flag);

    return wipower_flag;
}

int get_addon_feature_list(int fd) {
    int err = 0;

    /* Get addon features that are supported by FW */
    if ((err = addon_feature_req(fd)) < 0)
    {
        ALOGE("%s: failed (0x%x)", __FUNCTION__, err);
    }
    return err;
}

int wipower_forward_handoff_req(int fd)
{
    int size, err = 0;
    unsigned char cmd[HCI_MAX_CMD_SIZE];
    unsigned char rsp[HCI_MAX_EVENT_SIZE];
    hci_command_hdr *cmd_hdr;
    int flags;

    memset(cmd, 0x0, HCI_MAX_CMD_SIZE);

    cmd_hdr = (void *) (cmd + 1);
    cmd[0]  = HCI_COMMAND_PKT;
    cmd_hdr->opcode = cmd_opcode_pack(HCI_VENDOR_CMD_OGF, EDL_WIPOWER_VS_CMD_OCF);
    cmd_hdr->plen     = EDL_WIP_START_HANDOFF_TO_HOST_LEN;
    cmd[4]  = EDL_WIP_START_HANDOFF_TO_HOST_CMD;

    /* Total length of the packet to be sent to the Controller */
    size = (HCI_CMD_IND + HCI_COMMAND_HDR_SIZE + EDL_WIP_START_HANDOFF_TO_HOST_LEN);

    ALOGD("%s: Sending EDL_WIP_START_HANDOFF_TO_HOST_CMD", __FUNCTION__);
    ALOGD("HCI-CMD: \t0x%x \t0x%x \t0x%x \t0x%x \t0x%x", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4]);
    err = hci_send_wipower_vs_cmd(fd, (unsigned char *)cmd, rsp, size);
    if ( err != size) {
        ALOGE("Failed to send EDL_WIP_START_HANDOFF_TO_HOST_CMD command!");
        goto error;
    }

    /* Check for response from the Controller */
    if (local_read_vs_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE) < 0) {
        err = -ETIMEDOUT;
        ALOGI("%s: WP Failed to get HCI-VS Event from SOC", __FUNCTION__);
        goto error;
    }

error:
    return err;
}


void enable_controller_log (int fd)
{
   int ret = 0;
   /* VS command to enable/disable enhanced controller logging to the HOST */
   unsigned char cmd[6] = {0x01, 0x17, 0xFC, 0x02, 0x14, 0x00};
   unsigned char temp_wait_vsc_evt;
   unsigned char rsp[HCI_MAX_EVENT_SIZE];
   char value[PROPERTY_VALUE_MAX] = {'\0'};

   property_get("persist.service.bdroid.soclog", value, "false");

   // value at cmd[5]: 1 - to enable, 0 - to disable
   ret = (strcmp(value, "true") == 0) ? cmd[5] = 0x01: 0;
   ALOGI("%s: %d", __func__, ret);

   // Command status not expected for enhanced logging
   temp_wait_vsc_evt = wait_vsc_evt;
   wait_vsc_evt = false;

   ret = hci_send_vs_cmd(fd, (unsigned char *)cmd, rsp, 6);
   if (ret != 6) {
       ALOGE("%s: command failed", __func__);
   }
   wait_vsc_evt = temp_wait_vsc_evt;

   ret = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);

   if (ret < 0) {
       ALOGE("%s: Failed to get CC for enable enh SoC log", __FUNCTION__);
   }

   return;
}

bool is_extldo_enabled() {
    bool is_extldo = false;
    char extldo_prop[PROPERTY_VALUE_MAX];

    property_get("wc_transport.extldo", extldo_prop, "disabled");

    if (!strncmp(extldo_prop, "enabled",strlen(extldo_prop))) {
        is_extldo = true;
    }

    return is_extldo;
}

static int disable_internal_ldo(int fd)
{
    int ret = 0;
    if (is_extldo_enabled()) {
        unsigned char cmd[5] = {0x01, 0x0C, 0xFC, 0x01, 0x32};
        unsigned char rsp[HCI_MAX_EVENT_SIZE];

        ALOGI(" %s ", __FUNCTION__);
        ret = hci_send_vs_cmd (fd, cmd, rsp, 5);
        if (ret != 5) {
            ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, ret);
            ret = -1;
        } else {
            /* Wait for command complete event */
            ret = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
            if ( ret < 0) {
                ALOGE("%s: Failed to get response from controller", __FUNCTION__);
            }
        }
    }
    return ret;
}

void cherokee_shutdown_vs_cmd(int fd)
{
    int ret = 0;
    unsigned char cmd[4] = {0x01, 0x41, 0xFC, 0x00};

    ALOGI(" %s ", __FUNCTION__);
    ret = write(fd, cmd, 4);
    if (ret != 4) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, ret);
    }
}

#ifdef DEBUG_CRASH_SOC_WHILE_DLD
static int send_crash_command(int fd) {
    int ret = 0;
    unsigned char cmd[5] = {0x01, 0x0C, 0xFC, 0x01, 0x28};
    unsigned char rsp[HCI_MAX_EVENT_SIZE];

    ALOGI(" %s ", __FUNCTION__);
    ret = do_write_with_logging(fd, cmd, 5);
    if (ret != 5) {
        ALOGE("%s: Send failed with ret value: %d", __FUNCTION__, ret);
        ret = -1;
    } else {
        /* Wait for command complete event */
        ret = local_read_hci_event(fd, rsp, HCI_MAX_EVENT_SIZE);
        if ( ret < 0) {
            ALOGE("%s: Failed to get response from controller", __FUNCTION__);
        }
    }
    return ret;
}
#endif//DEBUG_CRASH_SOC_WHILE_DLD

int soc_init(int fd, uint8_t *bdaddr, bool is_emb_wp_mode)
{
    int err = -1, size = 0;
    dnld_fd = fd;
    ALOGI(" %s ", __FUNCTION__);
    RESERVED(bdaddr);

    /* If wipower charging is going on in embedded mode then start hand off req */
    if (wipower_flag == WIPOWER_IN_EMBEDDED_MODE && wipower_handoff_ready != NON_WIPOWER_MODE)
    {
        wipower_flag = 0;
        wipower_handoff_ready = 0;
        if ((err = wipower_forward_handoff_req(fd)) < 0)
        {
            ALOGI("%s: Wipower handoff failed (0x%x)", __FUNCTION__, err);
        }
    }

    /* Get Chipset version information */
    if((err = patch_ver_req(fd)) <0){
        ALOGI("%s: Fail to get chipset Version (0x%x)", __FUNCTION__, err);
        goto error;
    }

    ALOGI("%s: Chipset Version (0x%08x)", __FUNCTION__, chipset_ver);

    switch (chipset_ver){
        case CHEROKEE_VER_0_0:
        case CHEROKEE_VER_0_1:
        case CHEROKEE_VER_1_0:
            rampatch_file_path = CHEROKEE_RAMPATCH_TLV_1_0_PATH;
            nvm_file_path = CHEROKEE_NVM_TLV_1_0_PATH;
            goto download;
        case CHEROKEE_VER_1_1:
            rampatch_file_path = CHEROKEE_RAMPATCH_TLV_1_1_PATH;
            nvm_file_path = CHEROKEE_NVM_TLV_1_1_PATH;
            goto download;
        case CHEROKEE_VER_2_0:
        case CHEROKEE_VER_2_0_1:
            rampatch_file_path = CHEROKEE_RAMPATCH_TLV_2_0_PATH;
            nvm_file_path = CHEROKEE_NVM_TLV_2_0_PATH;
            goto download;
        case CHEROKEE_VER_2_1:
            rampatch_file_path = CHEROKEE_RAMPATCH_TLV_2_1_PATH;
            nvm_file_path = CHEROKEE_NVM_TLV_2_1_PATH;
            goto download;
        case CHEROKEE_VER_3_0:
            rampatch_file_path = CHEROKEE_RAMPATCH_TLV_3_0_PATH;
            nvm_file_path = CHEROKEE_NVM_TLV_3_0_PATH;
        case NAPIER_VER_1_0:
            rampatch_file_path = NAPIER_RAMPATCH_TLV_1_0_PATH;
            nvm_file_path = NAPIER_NVM_TLV_1_0_PATH;
            goto download;
download:
            /* Change baud rate 115.2 kbps to 3Mbps*/
            err = set_baudrate_req(fd);
            if (err < 0) {
                ALOGE("%s: Baud rate change failed!", __FUNCTION__);
                goto error;
            }
            ALOGI("%s: Baud rate changed successfully ", __FUNCTION__);
            /* Donwload TLV files (rampatch, NVM) */
            err = download_tlv_file(fd);
            if (err < 0) {
                ALOGE("%s: Download TLV file failed!", __FUNCTION__);
                goto error;
            }
            ALOGI("%s: Download TLV file successfully ", __FUNCTION__);
            /* Get SU FM label information */
            if((err = get_build_info_req(fd)) <0){
                ALOGI("%s: Fail to get FW SU Build info (0x%x)", __FUNCTION__, err);
                //Ignore the failure of FW SU label information
                err = 0;
            }

            /* Disable internal LDO to use external LDO instead*/
            err = disable_internal_ldo(fd);

            if(is_emb_wp_mode) {
                    /*  get chipset supported feature request */
                    ALOGV("%s: chipset_ver: %x08 Calling get addon feature", __FUNCTION__,chipset_ver);
                    get_addon_feature_list(fd);
            }

            enable_controller_log(fd);

            /* Send HCI Reset */
            err = hci_reset(fd);
            if ( err <0 ) {
                ALOGE("HCI Reset Failed !!");
                goto error;
            }

            ALOGI("HCI Reset is done\n");

            break;
        default:
            ALOGI("%s: Detected unknown SoC version: 0x%08x", __FUNCTION__, chipset_ver);
            err = -1;
            break;
    }

error:
    dnld_fd = -1;
    return err;
}

unsigned char convert2ascii(unsigned char temp)
{
    unsigned char n = temp;
    if ( n  >= 0 && n <= 9)
        n = n + 0x30;
    else
        n = n + 0x57;
    return n;
}

static bool validate_tok(char* bdaddr_tok) {
    int i = 0;
    bool ret;

    if (strlen(bdaddr_tok) != 2) {
        ret = FALSE;
        ALOGE("Invalid token length");
    } else {
        ret = TRUE;
        for (i=0; i<2; i++) {
            if ((bdaddr_tok[i] >= '0' && bdaddr_tok[i] <= '9') ||
                (bdaddr_tok[i] >= 'A' && bdaddr_tok[i] <= 'F') ||
                (bdaddr_tok[i] >= 'a' && bdaddr_tok[i] <= 'f')) {
                ret = TRUE;
                ALOGV("%s: tok %s @ %d is good", __func__, bdaddr_tok, i);
             } else {
                ret = FALSE;
                ALOGE("invalid character in tok: %s at ind: %d", bdaddr_tok, i);
                break;
             }
        }
    }
    return ret;
}

bool get_bdaddr_from_property(char *property, uint8_t res_bd_addr[6], bool is_reverse) {
    bool is_valid = FALSE;
    int i = 0;
    static char bd_addr[PROPERTY_VALUE_MAX];
    char* tok = NULL;
    char *tmp_token = NULL;

    if (property_get(property, bd_addr, NULL)) {
        ALOGV("BD address read from Boot property: %s\n", bd_addr);
        tok = strtok_r(bd_addr, ":", &tmp_token);
        while (tok != NULL) {
            ALOGV("bd add [%d]: %ld ", i, strtol(tok, NULL, 16));
            if (i>=6) {
                ALOGE("bd property of invalid length");
                is_valid = FALSE;
                break;
             }
             if (!validate_tok(tok)) {
                ALOGE("Invalid token in BD address");
                is_valid = FALSE;
                break;
             }
             if (is_reverse) {
                 res_bd_addr[5-i] = strtol(tok, NULL, 16);
             } else {
                 res_bd_addr[i] = strtol(tok, NULL, 16);
             }
             tok = strtok_r(NULL, ":", &tmp_token);
             i++;
        }
    }
    if (i == 6)
        is_valid = TRUE;

    return is_valid;
}


int perform_chip_initialization(int fd_transport) {
    int ret = 0;

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    char emb_wp_mode[PROPERTY_VALUE_MAX];
    bool is_emb_wp_mode = false;
    char wipower_status[PROPERTY_VALUE_MAX];
    uint8_t bd_addr[6], nv_bd_addr[6];
    bool ignore_boot_prop = TRUE;

    property_get("ro.bluetooth.emb_wp_mode", emb_wp_mode, false);
    if (strcmp(emb_wp_mode, "true") == 0) {
        ALOGV("%s: ro.bluetooth.emb_wp_mode is TRUE", __func__);
        is_emb_wp_mode = true;
    }

    ret  = get_bdaddr_from_property("wc_transport.stack_bdaddr", bd_addr, FALSE);
    if (!ret) {
        ALOGE("%s: Error while reading wc_transport.stack_bdaddr: %d", __func__, ret);
    } else {
        ALOGV("Valid BD address read from prop wc_transport.stack_bdaddr");
        memcpy(vnd_local_bd_addr, bd_addr, sizeof(vnd_local_bd_addr));
    }

    /* Clock on */
    //userial_clock_operation(fd_transport, USERIAL_OP_CLK_ON);
    //ALOGD("userial clock on");
    if(is_emb_wp_mode) {
        property_get("ro.bluetooth.wipower", wipower_status, false);
        if(strcmp(wipower_status, "true") == 0) {
            check_embedded_mode(fd_transport);
        } else {
            ALOGI("Wipower not enabled");
        }
    }

#ifdef READ_BT_ADDR_FROM_PROP
    ignore_boot_prop = FALSE;
    ret  = get_bdaddr_from_property(BLUETOOTH_MAC_ADDR_BOOT_PROPERTY, bd_addr, TRUE);
    if (!ret) {
        ALOGE("%s: Error while reading BLUETOOTH_MAC_ADDR_BOOT_PROPERTY: %d", __func__, ret);
        ignore_boot_prop = TRUE;
    } else {
        ALOGV("Valid BD address read from prop BLUETOOTH_MAC_ADDR_BOOT_PROPERTY");
        memcpy(vnd_local_bd_addr, bd_addr, sizeof(vnd_local_bd_addr));
    }
#endif//READ_BT_ADDR_FROM_PROP

#ifdef READ_BT_ADDR_FROM_NV
    if (ignore_boot_prop && bt_vendor_nv_read(1, nv_bd_addr)) {
        ALOGV("%s: nv bdaddr is %x:%x:%x:%x:%x%x\n", __func__, nv_bd_addr[0], nv_bd_addr[1],
            nv_bd_addr[2], nv_bd_addr[3], nv_bd_addr[4], nv_bd_addr[5]);
        memcpy(vnd_local_bd_addr, nv_bd_addr, sizeof(vnd_local_bd_addr));
    }
#endif//READ_BT_ADDR_FROM_NV

    ret = soc_init(fd_transport, vnd_local_bd_addr, is_emb_wp_mode);
    if (ret < 0) {
        ALOGE("%s: SoC initialization failed: %d\n", __func__, ret);
    }
    userial_clock_operation(fd_transport, USERIAL_OP_CLK_OFF);
    ALOGD("userial clock off");
#ifdef WAKE_LOCK_ENABLED
    wcnss_release_wakelock();
#endif

    return ret;
}

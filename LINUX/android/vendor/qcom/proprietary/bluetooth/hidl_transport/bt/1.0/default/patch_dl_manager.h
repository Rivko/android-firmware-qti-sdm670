/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
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

#pragma once

#include <hidl/HidlSupport.h>
#include <hci_uart_transport.h>
#include <logger.h>

/******************************************************************************
**  Constants & Macros
******************************************************************************/
#define HCI_MAX_CMD_SIZE                     260
#define HCI_MAX_EVENT_SIZE                   260
#define PRINT_BUF_SIZE                       ((HCI_MAX_CMD_SIZE * 3) + 2)

#define HCI_CHG_BAUD_CMD_OCF                 0x0C
#define HCI_VENDOR_CMD_OGF                   0x3F
#define WRITE_BDADDR_CMD_LEN                 14
#define WRITE_BAUD_CMD_LEN                   6
#define MAX_CMD_LEN                          WRITE_BDADDR_CMD_LEN
#define GET_VERSION_OCF                      0x1E

#define PS_HDR_LEN                           4
#define HCI_VENDOR_CMD_OGF                   0x3F
#define HCI_PS_CMD_OCF                       0x0B

#define HCI_COMMAND_HDR_SIZE                 3
#define EVT_CMD_COMPLETE_SIZE                3
#define EVT_CMD_STATUS                       0x0F
#define EVT_CMD_STATUS_SIZE                  4
#define HCI_EVENT_HDR_SIZE                   2
#define HCI_EV_SUCCESS                       0x00
/* HCI Socket options */
#define HCI_DATA_DIR                         1
#define HCI_FILTER                           2
#define HCI_TIME_STAMP                       3

#define P_ID_OFFSET                          (0)
#define HCI_CMD_IND                          (1)
#define EVENTCODE_OFFSET                     (1)
#define EVT_PLEN                             (2)
#define PLEN                                 (3)
#define CMD_RSP_OFFSET                       (3)
#define RSP_TYPE_OFFSET                      (4)
#define BAUDRATE_RSP_STATUS_OFFSET           (4)
#define CMD_STATUS_OFFSET                    (5)
#define P_CHIP_VER_OFFSET                    (4)
#define P_BUILD_VER_OFFSET                   (6)
#define P_BASE_ADDR_OFFSET                   (8)
#define P_ENTRY_ADDR_OFFSET                  (12)
#define P_LEN_OFFSET                         (16)
#define P_CRC_OFFSET                         (20)
#define P_CONTROL_OFFSET                     (24)
#define PATCH_HDR_LEN                        (28)
#define MAX_DATA_PER_SEGMENT                 (239)
#define VSEVENT_CODE                         (0xFF)
#define HC_VS_MAX_CMD_EVENT                  (0xFF)
#define PATCH_PROD_ID_OFFSET                 (5)
#define PATCH_PATCH_VER_OFFSET               (9)
#define PATCH_ROM_BUILD_VER_OFFSET           (11)
#define PATCH_SOC_VER_OFFSET                 (13)
#define MAX_SIZE_PER_TLV_SEGMENT             (243)
/* Unified VSC offset*/
#define PATCH_PROD_ID_OFFSET_UNIFIED         (9)
#define PATCH_PATCH_VER_OFFSET_UNIFIED       (13)
#define PATCH_ROM_BUILD_VER_OFFSET_UNIFIED   (15)
#define PATCH_SOC_VER_OFFSET_UNIFIED         (17)
#define UNIFIED_HCI_CODE                     (0x01)
#define UNIFIED_HCI_CC_MIN_LENGTH            (6)
#define HCI_VS_WIPOWER_CMD_OPCODE            (0xFC1F)
/* VS Opcode */
#define HCI_PATCH_CMD_OCF                    (0)
#define EDL_SET_BAUDRATE_CMD_OCF             (0x48)
#define EDL_WIPOWER_VS_CMD_OCF               (0x1f)
#define HCI_VS_GET_ADDON_FEATURES_SUPPORT    (0x1d)
#define EDL_GET_BOARD_ID                     (0x23)

/* VS Commands */
#define VSC_SET_BAUDRATE_REQ_LEN             (1)
#define EDL_PATCH_CMD_LEN                    (1)
#define EDL_PATCH_CMD_REQ_LEN                (1)
#define EDL_WIP_QUERY_CHARGING_STATUS_LEN    (0x01)
#define EDL_WIP_START_HANDOFF_TO_HOST_LEN    (0x01)
#define EDL_PATCH_DLD_REQ_CMD                (0x01)
#define EDL_PATCH_RST_REQ_CMD                (0x05)
#define EDL_PATCH_SET_REQ_CMD                (0x16)
#define EDL_PATCH_ATCH_REQ_CMD               (0x17)
#define EDL_PATCH_VER_REQ_CMD                (0x19)
#define EDL_GET_BUILD_INFO                   (0x20)
#define EDL_PATCH_TLV_REQ_CMD                (0x1E)
#define EDL_WIP_QUERY_CHARGING_STATUS_CMD    (0x1D)
#define EDL_WIP_START_HANDOFF_TO_HOST_CMD    (0x1E)
#define EDL_WIP_START_HANDOFF_TO_FW_CMD      (0x25)
/* VS Event */
#define EDL_CMD_REQ_RES_EVT                  (0x00)
#define EDL_CMD_EXE_STATUS_EVT               (0x00)
#define EDL_SET_BAUDRATE_RSP_EVT             (0x92)
#define EDL_PATCH_VER_RES_EVT                (0x19)
#define EDL_TVL_DNLD_RES_EVT                 (0x04)
#define EDL_APP_VER_RES_EVT                  (0x02)
#define EDL_WIP_QUERY_CHARGING_STATUS_EVT    (0x18)
#define EDL_WIP_START_HANDOFF_TO_HOST_EVENT  (0x19)
#define HCI_VS_GET_ADDON_FEATURES_EVENT      (0x1B)
#define EDL_BOARD_ID_RESPONSE                (0x23)
#define HCI_VS_GET_BUILD_VER_EVT             (0x05)
#define HCI_VS_STRAY_EVT                     (0x17)

/* Status Codes of HCI CMD execution*/
#define HCI_CMD_SUCCESS                      (0x0)
#define PATCH_LEN_ERROR                      (0x1)
#define PATCH_VER_ERROR                      (0x2)
#define PATCH_CRC_ERROR                      (0x3)
#define PATCH_NOT_FOUND                      (0x4)
#define TLV_TYPE_ERROR                       (0x10)
#define NVM_ACCESS_CODE                      (0x0B)
#define BAUDRATE_CHANGE_SUCCESS              (1)

/* Wipower status codes */
#define WIPOWER_IN_EMBEDDED_MODE             0x01
#define NON_WIPOWER_MODE                     0x02

/* mask to validate support for wipower */
#define ADDON_FEATURES_EVT_WIPOWER_MASK      (0x01)

/* TLV_TYPE */
#define TLV_TYPE_PATCH                       (1)
#define TLV_TYPE_BT_NVM                      (2)
#define TLV_TYPE_FM_NVM                      (3)
#define TLV_TYPE_BT_FM_NVM                   (4)

/* NVM */
#define MAX_TAG_CMD                          30
#define TAG_END                              0xFF
#define NVM_ACCESS_SET                       0x01
#define TAG_NUM_OFFSET                       5
#define TAG_NUM_2                            2
#define TAG_BDADDR_OFFSET                    7

#define TAG_NUM_17                           0x11  /* 17  : HCI UART Settings */

#define LSH(val, n)     ((uint32_t)(val) << (n))
#define EXTRACT_BYTE(val, pos)      (char)(((val) >> (8 * (pos))) & 0xFF)
#define CALC_SEG_SIZE(len, max)   ((plen) % (max)) ? ((plen / max) + 1) : ((plen) / (max))

/* NVM Tags specifically used for ROME 1.0 */
#define ROME_1_0_100022_1       0x101000221
#define ROME_1_0_100019         0x101000190
#define ROME_1_0_6002           0x100600200

/* Default NVM Version setting for ROME 1.0 */
#define NVM_VERSION                       ROME_1_0_100022_1
#define ROME_FW_PATH                      "/system/etc/firmware/rampatch.img"
#define ROME_RAMPATCH_TLV_PATH            "/system/etc/firmware/rampatch_tlv.img"
#define ROME_NVM_TLV_PATH                 "/system/etc/firmware/nvm_tlv.bin"
#define ROME_RAMPATCH_TLV_1_0_3_PATH      "/system/etc/firmware/rampatch_tlv_1.3.tlv"
#define ROME_NVM_TLV_1_0_3_PATH           "/system/etc/firmware/nvm_tlv_1.3.bin"
#define ROME_RAMPATCH_TLV_2_0_1_PATH      "/system/etc/firmware/rampatch_tlv_2.1.tlv"
#define ROME_NVM_TLV_2_0_1_PATH           "/system/etc/firmware/nvm_tlv_2.1.bin"
#define ROME_RAMPATCH_TLV_3_0_0_PATH      "/bt_firmware/image/btfw30.tlv"
#define ROME_NVM_TLV_3_0_0_PATH           "/bt_firmware/image/btnv30.bin"
#define ROME_RAMPATCH_TLV_3_0_2_PATH      "/bt_firmware/image/btfw32.tlv"
#define ROME_NVM_TLV_3_0_2_PATH           "/bt_firmware/image/btnv32.bin"

#define NAPLES_RAMPATCH_TLV_UART_1_0_PATH      "/firmware/image/btfwnpla.tlv"
#define NAPLES_NVM_TLV_UART_1_0_PATH         "/firmware/image/btnvnpla.bin"
#define NAPLES_RAMPATCH_TLV_UART_1_0_PATH_ALT    "/vendor/firmware/btfwnpla.tlv"
#define NAPLES_NVM_TLV_UART_1_0_PATH_ALT         "/vendor/firmware/btnvnpla.bin"

#define ROME_3_1_FW_SU  "bprm.cnss.3.1"
#define ROME_3_2_FW_SU  "btfwp.cnss.3.2"

#define CHEROKEE_RAMPATCH_TLV_1_0_PATH    "/bt_firmware/image/crbtfw10.tlv"
#define CHEROKEE_NVM_TLV_1_0_PATH         "/bt_firmware/image/crnv10.bin"
#define CHEROKEE_RAMPATCH_TLV_1_1_PATH    "/bt_firmware/image/crbtfw11.tlv"
#define CHEROKEE_NVM_TLV_1_1_PATH         "/bt_firmware/image/crnv11.bin"
#define CHEROKEE_RAMPATCH_TLV_2_0_PATH    "/bt_firmware/image/crbtfw20.tlv"
#define CHEROKEE_NVM_TLV_2_0_PATH         "/bt_firmware/image/crnv20.bin"
#define CHEROKEE_RAMPATCH_TLV_2_1_PATH    "/bt_firmware/image/crbtfw21.tlv"
#define CHEROKEE_NVM_TLV_2_1_PATH         "/bt_firmware/image/crnv21.bin"
#define CHEROKEE_RAMPATCH_TLV_3_0_PATH    "/bt_firmware/image/crbtfw30.tlv"
#define CHEROKEE_NVM_TLV_3_0_PATH         "/bt_firmware/image/crnv30.bin"

#define NAPIER_RAMPATCH_TLV_1_0_PATH      "/bt_firmware/image/nrbtfw10.tlv"
#define NAPIER_NVM_TLV_1_0_PATH           "/bt_firmware/image/nrnv10.bin"
#define NAPIER_RAMPATCH_TLV_2_0_PATH      "/bt_firmware/image/nrbtfw20.tlv"
#define NAPIER_NVM_TLV_2_0_PATH           "/bt_firmware/image/nrnv20.bin"
#define APACHE_RAMPATCH_TLV_0_0_5_PATH    "/bt_firmware/image/apbtfw05.tlv"
#define APACHE_NVM_TLV_0_0_5_PATH         "/bt_firmware/image/apnv05.bin"
#define APACHE_RAMPATCH_TLV_0_0_6_PATH    "/bt_firmware/image/apbtfw06.tlv"
#define APACHE_NVM_TLV_0_0_6_PATH         "/bt_firmware/image/apnv06.bin"
#define APACHE_RAMPATCH_TLV_1_0_0_PATH    "/bt_firmware/image/apbtfw10.tlv"
#define APACHE_NVM_TLV_1_0_0_PATH         "/bt_firmware/image/apnv10.bin"
#define APACHE_RAMPATCH_TLV_1_0_1_PATH    "/bt_firmware/image/apbtfw10.tlv"
#define APACHE_NVM_TLV_1_0_1_PATH         "/bt_firmware/image/apnv10.bin"
#define APACHE_RAMPATCH_TLV_1_0_2_PATH    "/bt_firmware/image/apbtfw10.tlv"
#define APACHE_NVM_TLV_1_0_2_PATH         "/bt_firmware/image/apnv10.bin"
#define APACHE_RAMPATCH_TLV_1_0_3_PATH    "/bt_firmware/image/apbtfw10.tlv"
#define APACHE_NVM_TLV_1_0_3_PATH         "/bt_firmware/image/apnv10.bin"
#define APACHE_RAMPATCH_TLV_1_1_0_PATH    "/bt_firmware/image/apbtfw11.tlv"
#define APACHE_NVM_TLV_1_1_0_PATH         "/bt_firmware/image/apnv11.bin"

#define COMANCHE_RAMPATCH_TLV_1_0_PATH    "/bt_firmware/image/cmbtfw10.tlv"
#define COMANCHE_NVM_TLV_1_0_PATH         "/bt_firmware/image/cmnv10.bin"
#define COMANCHE_RAMPATCH_TLV_1_0_1_PATH  "/bt_firmware/image/cmbtfw10.tlv"
#define COMANCHE_NVM_TLV_1_0_1_PATH       "/bt_firmware/image/cmnv10.bin"
#define COMANCHE_RAMPATCH_TLV_1_1_PATH    "/bt_firmware/image/cmbtfw11.tlv"
#define COMANCHE_NVM_TLV_1_1_PATH         "/bt_firmware/image/cmnv11.bin"

#define TEMP_RAMPATCH_TLV_0_0_0_PATH    "/bt_firmware/image/tpbtfw00.tlv"
#define TEMP_NVM_TLV_0_0_0_PATH         "/bt_firmware/image/tpnv00.bin"


#define ROME_RAMPATCH_TLV_3_0_0_PATH_ALT      "/vendor/firmware/btfw30.tlv"
#define ROME_NVM_TLV_3_0_0_PATH_ALT           "/vendor/firmware/btnv30.bin"
#define ROME_RAMPATCH_TLV_3_0_2_PATH_ALT      "/vendor/firmware/btfw32.tlv"
#define ROME_NVM_TLV_3_0_2_PATH_ALT           "/vendor/firmware/btnv32.bin"

#define CHEROKEE_RAMPATCH_TLV_1_0_PATH_ALT    "/vendor/firmware/crbtfw10.tlv"
#define CHEROKEE_NVM_TLV_1_0_PATH_ALT         "/vendor/firmware/crnv10.bin"
#define CHEROKEE_RAMPATCH_TLV_1_1_PATH_ALT    "/vendor/firmware/crbtfw11.tlv"
#define CHEROKEE_NVM_TLV_1_1_PATH_ALT         "/vendor/firmware/crnv11.bin"
#define CHEROKEE_RAMPATCH_TLV_2_0_PATH_ALT    "/vendor/firmware/crbtfw20.tlv"
#define CHEROKEE_NVM_TLV_2_0_PATH_ALT         "/vendor/firmware/crnv20.bin"
#define CHEROKEE_RAMPATCH_TLV_2_1_PATH_ALT    "/vendor/firmware/crbtfw21.tlv"
#define CHEROKEE_NVM_TLV_2_1_PATH_ALT         "/vendor/firmware/crnv21.bin"
#define CHEROKEE_RAMPATCH_TLV_3_0_PATH_ALT    "/vendor/firmware/crbtfw30.tlv"
#define CHEROKEE_NVM_TLV_3_0_PATH_ALT         "/vendor/firmware/crnv30.bin"

#define APACHE_RAMPATCH_TLV_0_0_5_PATH_ALT    "/vendor/firmware/apbtfw05.tlv"
#define APACHE_NVM_TLV_0_0_5_PATH_ALT         "/vendor/firmware/apnv05.bin"
#define APACHE_RAMPATCH_TLV_0_0_6_PATH_ALT    "/vendor/firmware/apbtfw06.tlv"
#define APACHE_NVM_TLV_0_0_6_PATH_ALT         "/vendor/firmware/apnv06.bin"
#define APACHE_RAMPATCH_TLV_1_0_0_PATH_ALT    "/vendor/firmware/apbtfw10.tlv"
#define APACHE_NVM_TLV_1_0_0_PATH_ALT         "/vendor/firmware/apnv10.bin"
#define APACHE_RAMPATCH_TLV_1_0_1_PATH_ALT    "/vendor/firmware/apbtfw10.tlv"
#define APACHE_NVM_TLV_1_0_1_PATH_ALT         "/vendor/firmware/apnv10.bin"
#define APACHE_RAMPATCH_TLV_1_0_2_PATH_ALT    "/vendor/firmware/apbtfw10.tlv"
#define APACHE_NVM_TLV_1_0_2_PATH_ALT         "/vendor/firmware/apnv10.bin"
#define APACHE_RAMPATCH_TLV_1_0_3_PATH_ALT    "/vendor/firmware/apbtfw10.tlv"
#define APACHE_NVM_TLV_1_0_3_PATH_ALT         "/vendor/firmware/apnv10.bin"
#define APACHE_RAMPATCH_TLV_1_1_0_PATH_ALT    "/vendor/firmware/apbtfw11.tlv"
#define APACHE_NVM_TLV_1_1_0_PATH_ALT         "/vendor/firmware/apnv11.bin"

#define NAPIER_RAMPATCH_TLV_1_0_PATH_ALT      "/vendor/firmware/nrbtfw10.tlv"
#define NAPIER_NVM_TLV_1_0_PATH_ALT           "/vendor/firmware/nrnv10.bin"
#define NAPIER_RAMPATCH_TLV_2_0_PATH_ALT      "/vendor/firmware/nrbtfw20.tlv"
#define NAPIER_NVM_TLV_2_0_PATH_ALT           "/vendor/firmware/nrnv20.bin"

#define COMANCHE_RAMPATCH_TLV_1_0_PATH_ALT   "/vendor/firmware/cmbtfw10.tlv"
#define COMANCHE_NVM_TLV_1_0_PATH_ALT        "/vendor/firmware/cmnv10.bin"
#define COMANCHE_RAMPATCH_TLV_1_0_1_PATH_ALT "/vendor/firmware/cmbtfw10.tlv"
#define COMANCHE_NVM_TLV_1_0_1_PATH_ALT      "/vendor/firmware/cmnv10.bin"
#define COMANCHE_RAMPATCH_TLV_1_1_PATH_ALT   "/vendor/firmware/cmbtfw11.tlv"
#define COMANCHE_NVM_TLV_1_1_PATH_ALT        "/vendor/firmware/cmnv11.bin"

#define TEMP_RAMPATCH_TLV_0_0_0_PATH_ALT    "/vendor/firmware/tpbtfw00.tlv"
#define TEMP_NVM_TLV_0_0_0_PATH_ALT         "/vendor/firmware/tpnv00.bin"

/* This header value in rampatch file decides event handling mechanism in the HOST */
#define SKIP_EVT_NONE     0x00
#define SKIP_EVT_VSE      0x01
#define SKIP_EVT_CC       0x02
#define SKIP_EVT_VSE_CC   0x03

/* Rome 3.1 FW SU release has been branched from rome 3.0 SU 224
   So, rome 3.1 formula is SU = patch version -(0xE0 = 224) - 0x111 -1
 */
#define ROME_3_1_FW_SW_OFFSET   0x01F2

/* Rome 3.2 FW SU formula is SU = patch version - 0x111 -1 */
#define ROME_3_2_FW_SW_OFFSET   0x0112

/* This header value in rampatch file decides event handling mechanism in the HOST */
#define ROME_SKIP_EVT_NONE     0x00
#define ROME_SKIP_EVT_VSE      0x01
#define ROME_SKIP_EVT_CC       0x02
#define ROME_SKIP_EVT_VSE_CC   0x03

#define BOARD_ID_LEN 0x5

/* (Product ID) << 20) + (Build version) << 4) + (SoC ID)) */
#define QCA_BT_VER(p, b, s) (((uint32_t)(p) << 20) + ((uint32_t)(b) << 4) + ((uint32_t)(s)))

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

/******************************************************************************
**  Local type definitions
******************************************************************************/
typedef struct {
  unsigned short rom_version;
  unsigned short build_version;
} __attribute__ ((packed)) patch_version;

typedef struct {
  uint16_t opcode;
  uint8_t plen;
} __attribute__ ((packed))  hci_command_hdr;

typedef struct {
  unsigned int patch_id;
  patch_version patch_ver;
  unsigned int patch_base_addr;
  unsigned int patch_entry_addr;
  unsigned short patch_length;
  int patch_crc;
  unsigned short patch_ctrl;
} __attribute__ ((packed)) patch_info;

typedef struct {
  unsigned int tlv_data_len;
  unsigned int tlv_patch_data_len;
  unsigned char sign_ver;
  unsigned char sign_algorithm;
  unsigned char dwnd_cfg;
  unsigned char reserved1;
  unsigned short prod_id;
  unsigned short build_ver;
  unsigned short patch_ver;
  unsigned short reserved2;
  unsigned int patch_entry_addr;
} __attribute__ ((packed)) tlv_patch_hdr;

typedef struct {
  unsigned short tag_id;
  unsigned short tag_len;
  unsigned int tag_ptr;
  unsigned int tag_ex_flag;
} __attribute__ ((packed)) tlv_nvm_hdr;

typedef struct {
  unsigned char tlv_type;
  unsigned char tlv_length1;
  unsigned char tlv_length2;
  unsigned char tlv_length3;

  union {
    tlv_patch_hdr patch;
    tlv_nvm_hdr nvm;
  }tlv;
} __attribute__ ((packed)) tlv_patch_info;

enum {
  PROD_ID_ROME = 0x08,
  PROD_ID_CHEROKEE = 0x0A,
  PROD_ID_NAPLES = 0x0C,
  PROD_ID_NAPIER = 0x0D,    /* Napier : Auto version of CHEROKEE */
  PROD_ID_COMANCHE = 0x0F,
  PROD_ID_APACHE = PROD_ID_CHEROKEE
};

enum {
  ROME_PATCH_VER_0100 = 0x0100,
  ROME_PATCH_VER_0101 = 0x0101,
  ROME_PATCH_VER_0200 = 0x0200,
  ROME_PATCH_VER_0300 = 0x0300,
  ROME_PATCH_VER_0302 = 0x0302
};

enum {
  ROME_SOC_ID_00 = 0x00000000,
  ROME_SOC_ID_11 = 0x00000011,
  ROME_SOC_ID_22 = 0x00000022,
  ROME_SOC_ID_44 = 0x00000044
};

enum {
  ROME_VER_UNKNOWN = 0,
  ROME_VER_1_0 = QCA_BT_VER(PROD_ID_ROME, ROME_PATCH_VER_0100, ROME_SOC_ID_00),
  ROME_VER_1_1 = QCA_BT_VER(PROD_ID_ROME, ROME_PATCH_VER_0101, ROME_SOC_ID_00),
  ROME_VER_1_3 = QCA_BT_VER(PROD_ID_ROME, ROME_PATCH_VER_0200, ROME_SOC_ID_00),
  ROME_VER_2_1 = QCA_BT_VER(PROD_ID_ROME, ROME_PATCH_VER_0200, ROME_SOC_ID_11),
  ROME_VER_3_0 = QCA_BT_VER(PROD_ID_ROME, ROME_PATCH_VER_0300, ROME_SOC_ID_22),
  ROME_VER_3_2 = QCA_BT_VER(PROD_ID_ROME, ROME_PATCH_VER_0302, ROME_SOC_ID_44),
};

enum {
  CHEROKEE_BUILD_VER_0000   = 0x1100,
  CHEROKEE_BUILD_VER_0100   = 0x0100,
  CHEROKEE_BUILD_VER_0101   = 0x0101,
  CHEROKEE_BUILD_VER_0200   = 0x0200,
  CHEROKEE_BUILD_VER_0201   = 0x0201,
  CHEROKEE_BUILD_VER_0300   = 0x0300,
};

enum {
  QCA_CHEROKEE_SOC_ID_0100  = 0x40010100,
  QCA_CHEROKEE_SOC_ID_0200  = 0x40010200,
  QCA_CHEROKEE_SOC_ID_0201  = 0x40010201,
  QCA_CHEROKEE_SOC_ID_0210  = 0x40010214,
  QCA_CHEROKEE_SOC_ID_0300  = 0x40010300,
};

enum {
  /* FPGA */
  CHEROKEE_VER_0_0 = QCA_BT_VER(PROD_ID_CHEROKEE, CHEROKEE_BUILD_VER_0000, 0),
  CHEROKEE_VER_0_1 = QCA_BT_VER(PROD_ID_CHEROKEE, CHEROKEE_BUILD_VER_0100, QCA_CHEROKEE_SOC_ID_0100),
  /* Silicon */
  CHEROKEE_VER_1_0 = QCA_BT_VER(PROD_ID_CHEROKEE, CHEROKEE_BUILD_VER_0100, 0),
  CHEROKEE_VER_1_1 = QCA_BT_VER(PROD_ID_CHEROKEE, CHEROKEE_BUILD_VER_0101, 0),
  CHEROKEE_VER_2_0 = QCA_BT_VER(PROD_ID_CHEROKEE, CHEROKEE_BUILD_VER_0200, QCA_CHEROKEE_SOC_ID_0200),
  CHEROKEE_VER_2_0_1 = QCA_BT_VER(PROD_ID_CHEROKEE, CHEROKEE_BUILD_VER_0200, QCA_CHEROKEE_SOC_ID_0201),
  CHEROKEE_VER_2_1 = QCA_BT_VER(PROD_ID_CHEROKEE, CHEROKEE_BUILD_VER_0201, QCA_CHEROKEE_SOC_ID_0210),
  CHEROKEE_VER_3_0 = QCA_BT_VER(PROD_ID_CHEROKEE, CHEROKEE_BUILD_VER_0300, QCA_CHEROKEE_SOC_ID_0300),
};


enum {
    NAPLES_BUILD_VER_0100 = 0x0100,
};

enum {
    NAPLES_SOC_ID_15 = 0x00000015,
};

enum {
    NAPLES_VER_1_0 =QCA_BT_VER(PROD_ID_NAPLES, NAPLES_BUILD_VER_0100, NAPLES_SOC_ID_15),
};


enum {
  NAPIER_BUILD_VER_0100     = 0x0100,
  NAPIER_BUILD_VER_0101     = 0x0101,
  NAPIER_BUILD_VER_0200     = 0x0200
};


enum{
    APACHE_BUILD_VER_0005   = 0x0100,
    APACHE_BUILD_VER_0006   = 0x0101,
    APACHE_BUILD_VER_0100   = 0x0200,
    APACHE_BUILD_VER_0101   = 0x0200,
    APACHE_BUILD_VER_0102   = 0x0200,
    APACHE_BUILD_VER_0103   = 0x0200,
    APACHE_BUILD_VER_0110   = 0x0201
};

enum{
    QCA_APACHE_SOC_ID_0005  = 0x40020100,
    QCA_APACHE_SOC_ID_0006  = 0x40020110,
    QCA_APACHE_SOC_ID_0100  = 0x40020120,
    QCA_APACHE_SOC_ID_0101  = 0x40020121,
    QCA_APACHE_SOC_ID_0102  = 0x40020122,
    QCA_APACHE_SOC_ID_0103  = 0x40020123,
    QCA_APACHE_SOC_ID_0110  = 0x40020130,
    QCA_APACHE_SOC_ID_0111  = 0x40020140,
};

enum {
  APACHE_VER_0_0_5 = QCA_BT_VER(PROD_ID_APACHE, APACHE_BUILD_VER_0005, QCA_APACHE_SOC_ID_0005),
  APACHE_VER_0_0_6 = QCA_BT_VER(PROD_ID_APACHE, APACHE_BUILD_VER_0006, QCA_APACHE_SOC_ID_0006),
  APACHE_VER_1_0_0 = QCA_BT_VER(PROD_ID_APACHE, APACHE_BUILD_VER_0100, QCA_APACHE_SOC_ID_0100),
  APACHE_VER_1_0_1 = QCA_BT_VER(PROD_ID_APACHE, APACHE_BUILD_VER_0101, QCA_APACHE_SOC_ID_0101),
  APACHE_VER_1_0_2 = QCA_BT_VER(PROD_ID_APACHE, APACHE_BUILD_VER_0102, QCA_APACHE_SOC_ID_0102),
  APACHE_VER_1_0_3 = QCA_BT_VER(PROD_ID_APACHE, APACHE_BUILD_VER_0103, QCA_APACHE_SOC_ID_0103),
  APACHE_VER_1_1_0 = QCA_BT_VER(PROD_ID_APACHE, APACHE_BUILD_VER_0110, QCA_APACHE_SOC_ID_0110),
  APACHE_VER_1_1_1 = QCA_BT_VER(PROD_ID_APACHE, APACHE_BUILD_VER_0110, QCA_APACHE_SOC_ID_0111),
};

enum {
    COMANCHE_BUILD_VER_0100   = 0X0100,
    COMANCHE_BUILD_VER_0101   = 0X0101,
};

enum {
    QCA_COMANCHE_SOC_ID_0100  = 0x40070100,
    QCA_COMANCHE_SOC_ID_0101  = 0x40070101,
    QCA_COMANCHE_SOC_ID_0110  = 0x40070110,
};

enum {
    COMANCHE_VER_1_0   = QCA_BT_VER(PROD_ID_COMANCHE, COMANCHE_BUILD_VER_0100, QCA_COMANCHE_SOC_ID_0100),
    COMANCHE_VER_1_0_1 = QCA_BT_VER(PROD_ID_COMANCHE, COMANCHE_BUILD_VER_0100, QCA_COMANCHE_SOC_ID_0101),
    COMANCHE_VER_1_1   = QCA_BT_VER(PROD_ID_COMANCHE, COMANCHE_BUILD_VER_0101, QCA_COMANCHE_SOC_ID_0110),
};


enum {
  QCA_NAPIER_SOC_ID_0100    = 0x40040100,
  QCA_NAPIER_SOC_ID_0101    = 0x40040101,
  QCA_NAPIER_SOC_ID_0200    = 0x40040200
};

enum {
  NAPIER_VER_1_0_0 = QCA_BT_VER(PROD_ID_NAPIER, NAPIER_BUILD_VER_0100, QCA_NAPIER_SOC_ID_0100),
  NAPIER_VER_1_0_1 = QCA_BT_VER(PROD_ID_NAPIER, NAPIER_BUILD_VER_0100, QCA_NAPIER_SOC_ID_0101),
  NAPIER_VER_1_1 = QCA_BT_VER(PROD_ID_NAPIER, NAPIER_BUILD_VER_0101, QCA_NAPIER_SOC_ID_0101),
  NAPIER_VER_2_0 = QCA_BT_VER(PROD_ID_NAPIER, NAPIER_BUILD_VER_0200, QCA_NAPIER_SOC_ID_0200)
};

#define IS_NAPIER(v)        (((v) == NAPIER_VER_1_0_0) || \
                             ((v) == NAPIER_VER_1_0_1) || \
                             ((v) == NAPIER_VER_1_1) || \
                             ((v) == NAPIER_VER_2_0))

class PatchDLManager {
 public:
  PatchDLManager(BluetoothSocType soc_type, HciUartTransport* transport);
  int PerformChipInit();
  int SetBaudRateReq();
  unsigned char IsSibsEnabled();
  ~PatchDLManager();

 protected:
  bool GetBdaddrFromProperty(const char *property, uint8_t res_bd_addr[6], bool is_reverse);
  int SocInit(uint8_t *bdaddr, bool is_emb_wp_mode);
  int DownloadTlvFile(const char *rampatch_file_path, const char *nvm_file_path, const char *rampatch_alt_file_path, const char *nvm_alt_file_path);
  int HciReset(void);
  int WipowerCurrentChargingStatusReq();
  int WipowerForwardHandoffReq();
  int HciSendWipowerVsCmd(unsigned char *cmd, unsigned char *rsp, int size);
  int PatchVerReq();
  void FrameHciCmdPkt(unsigned char *cmd, int edl_cmd, unsigned int p_base_addr,
                         int segtNo, int size);
  int CheckEmbeddedMode();
  int GetBuildInfoReq();
  bool IsExtldoEnabled();
  int HciSendVsCmd(unsigned char *cmd, unsigned char *rsp, int size);
  int ReadVsHciEvent(unsigned char* buf, int size);
  int ReadHciEvent(unsigned char* buf, int size);
  int GetVsHciEvent(unsigned char *rsp);
  int TlvDnldReq(int tlv_size);
  void FrameHciPkt(unsigned char *cmd, int edl_cmd, unsigned int p_base_addr,
                     int segtNo, int size);
  int GetBoardIdReq(void);
  int GetAddOnFeatureList();
  bool ValidateToken(char *bdaddr_tok);
  int GetTlvFile(const char *file_path, const char *alt_file_path);
  int TlvDnldSegment(int index, int seg_size, unsigned char wait_cc_evt);
  //for ROME_VER_1_0
  int RomeEdlPatchDownloadRequest();
  int RomeDownloadRampatch();
  int RomeAttachRampatch();
  int RomeRampatchReset();
  int RomeHciResetReq();
  int Rome10NvmTagDnld(uint8_t *bdaddr);
  void RomeExtractPatchHeaderInfo(unsigned char *buf);
  int RomeEdlSetPatchRequest(int fd);
  int DisableInternalLdo();
  void EnableControllerLog();
  void EnableEnhControllerLog();
  int ReadNewHciEvent(unsigned char* buf, int size);
  unsigned char Convert2Ascii(unsigned char temp);
  void UpdateNapierNvm(tlv_nvm_hdr *nvm);
  int GetUnifiedHciEvent(unsigned char* rsp);
#ifdef DEBUG_CRASH_SOC_WHILE_DLD
  int SendCrashCommand();
#endif

 private:
  BluetoothSocType soc_type_;
  HciUartTransport* uart_transport_;
  int fd_transport_;
  int tlv_type_;
  uint32_t chipset_ver_;
  Logger *bt_logger_;
  unsigned int wipower_flag_;
  unsigned int wipower_handoff_ready_;
  int dnld_fd_in_progress_;
  unsigned char *pdata_buffer_;
  unsigned char *phdr_buffer_;
  unsigned char tlv_dwn_cfg_;
  unsigned char wait_vsc_evt_;
  patch_info rampatch_patch_info_;
  bool patch_dnld_pending_;
  unsigned char board_id_[BOARD_ID_LEN];
  uint8_t vnd_local_bd_addr_[6];
  const char *fw_su_info_ = NULL;
  unsigned short fw_su_offset_ = 0;
  bool unified_hci;
};

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android

/*
 *  Copyright (c) 2016 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 *  Not a Contribution.
 *  Apache license notifications and license are retained
 *  for attribution purposes only.
 *
 *  Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *  Not a Contribution.
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <termios.h>
#include <utils/Log.h>
#include <fcntl.h>

#if 0
#include <cutils/log.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdint.h>
#include "uart_utils.h"
#include <errno.h>
#include <string.h>
#endif

#define TRUE           1
#define FALSE          0

/* Flow control */
#define USERIAL_CTSRTS               (1<<15)

#define BT_HS_UART_DEVICE "/dev/ttyHS0"

/* HCI Packet types */
#define HCI_COMMAND_PKT     0x01

#define EVT_CMD_COMPLETE       0x0E

#define BT_ACL_HDR_SIZE 4
#define BT_SCO_HDR_SIZE 3
#define BT_EVT_HDR_SIZE 2
#define BT_CMD_HDR_SIZE 3

#define BT_ACL_HDR_LEN_OFFSET 2
#define BT_SCO_HDR_LEN_OFFSET 2
#define BT_EVT_HDR_LEN_OFFSET 1
#define BT_CMD_HDR_LEN_OFFSET 2

/**** baud rates ****/
enum{
  BAUDRATE_115200     = 0x00,
  BAUDRATE_57600      = 0x01,
  BAUDRATE_38400      = 0x02,
  BAUDRATE_19200      = 0x03,
  BAUDRATE_9600       = 0x04,
  BAUDRATE_230400     = 0x05,
  BAUDRATE_250000     = 0x06,
  BAUDRATE_460800     = 0x07,
  BAUDRATE_500000     = 0x08,
  BAUDRATE_720000     = 0x09,
  BAUDRATE_921600     = 0x0A,
  BAUDRATE_1000000    = 0x0B,
  BAUDRATE_1250000    = 0x0C,
  BAUDRATE_2000000    = 0x0D,
  BAUDRATE_3000000    = 0x0E,
  BAUDRATE_4000000    = 0x0F,
  BAUDRATE_1600000    = 0x10,
  BAUDRATE_3200000    = 0x11,
  BAUDRATE_3500000    = 0x12,
  /* MSM Supported Baud rate */
  BAUDRATE_300        = 0x13,
  BAUDRATE_600        = 0x14,
  BAUDRATE_1200       = 0x15,
  BAUDRATE_2400       = 0x16,
  BAUDRATE_1500000    = 0x17,
  BAUDRATE_AUTO       = 0xFE,
  BAUDRATE_Reserved   = 0xFF
};

#define USERIAL_BAUD_300        BAUDRATE_300
#define USERIAL_BAUD_600        BAUDRATE_600
#define USERIAL_BAUD_1200       BAUDRATE_1200
#define USERIAL_BAUD_2400       BAUDRATE_2400
#define USERIAL_BAUD_9600       BAUDRATE_9600
#define USERIAL_BAUD_19200      BAUDRATE_19200
#define USERIAL_BAUD_57600      BAUDRATE_57600
#define USERIAL_BAUD_115200     BAUDRATE_115200
#define USERIAL_BAUD_230400     BAUDRATE_230400
#define USERIAL_BAUD_460800     BAUDRATE_460800
#define USERIAL_BAUD_921600     BAUDRATE_921600
#define USERIAL_BAUD_1M         BAUDRATE_1000000
#define USERIAL_BAUD_1_5M       BAUDRATE_1500000
#define USERIAL_BAUD_2M         BAUDRATE_2000000
#define USERIAL_BAUD_3M         BAUDRATE_3000000
#define USERIAL_BAUD_3_2M       BAUDRATE_3200000
#define USERIAL_BAUD_4M         BAUDRATE_4000000
#define USERIAL_BAUD_AUTO       BAUDRATE_AUTO

/* Stop Bits */
#define USERIAL_STOPBITS_1      1
#define USERIAL_STOPBITS_1_5    (1<<1)
#define USERIAL_STOPBITS_2      (1<<2)

/* Parity Bits */
#define USERIAL_PARITY_NONE     (1<<3)
#define USERIAL_PARITY_EVEN     (1<<4)
#define USERIAL_PARITY_ODD      (1<<5)

/* Data Bits */
#define USERIAL_DATABITS_5      (1<<6)
#define USERIAL_DATABITS_6      (1<<7)
#define USERIAL_DATABITS_7      (1<<8)
#define USERIAL_DATABITS_8      (1<<9)

#define USERIAL_OP_CLK_ON 0x5441
#define USERIAL_OP_CLK_OFF 0x5442
#define USERIAL_OP_CLK_STATE 0x5443

/* Structure used to configure serial port during open */
typedef struct {
  uint16_t fmt;       /* Data format */
  uint8_t  baud;      /* Baud rate */
} tUSERIAL_CFG;

typedef enum {
#if (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)
  USERIAL_OP_ASSERT_BT_WAKE,
  USERIAL_OP_DEASSERT_BT_WAKE,
  USERIAL_OP_GET_BT_WAKE_STATE,
#endif
  USERIAL_OP_FLOW_ON,
  USERIAL_OP_FLOW_OFF,
  USERIAL_OP_NOP,
} userial_vendor_ioctl_op_t;

/* Command opcode pack/unpack */
#define cmd_opcode_pack(ogf, ocf)   (uint16_t)((ocf & 0x03ff)|(ogf << 10))

/* Flow control */
#define USERIAL_CTSRTS               (1<<15)

/* HCI Command/Event Opcode */
#define HCI_RESET                    0x0C03

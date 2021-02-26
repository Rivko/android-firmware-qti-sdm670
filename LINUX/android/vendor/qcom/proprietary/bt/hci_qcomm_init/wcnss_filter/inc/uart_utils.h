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

#ifndef _UART_UTILS_H_
#define _UART_UTILS_H_

#include "hw_cherokee.h"

#define TRUE           1
#define FALSE          0

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

/****   BT Baudrate ****/
#ifdef UART_BAUDRATE_3_0_MBPS
#define BT_BAUD_RATE   USERIAL_BAUD_3M
#else
#define BT_BAUD_RATE   USERIAL_BAUD_3_2M
#endif

#define USERIAL_OP_CLK_ON 0x5441
#define USERIAL_OP_CLK_OFF 0x5442
#define USERIAL_OP_CLK_STATE 0x5443

typedef struct {
    uint16_t    opcode;
    uint8_t     plen;
} __attribute__ ((packed))  hci_command_hdr;

/* Structure used to configure serial port during open */
typedef struct
{
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
#define HCI_RESET                       0x0C03

/*******************************************************************************
**
** Function        userial_vendor_ioctl
**
** Description     ioctl inteface
**
** Returns         int error
**
*******************************************************************************/
int userial_vendor_ioctl(int fd, userial_vendor_ioctl_op_t op, int *p_data);

/*******************************************************************************
**
** Function        userial_vendor_set_baud
**
** Description     Set new baud rate
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_set_baud(int fd, uint8_t userial_baud);

/*******************************************************************************
**
** Function        userial_vendor_get_baud
**
** Description     Get current baud rate
**
** Returns         int
**
*******************************************************************************/
int userial_vendor_get_baud(int fd);

/*******************************************************************************
**
** Function        userial_to_tcio_baud
**
** Description     helper function converts USERIAL baud rates into TCIO
**                  conforming baud rates
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t userial_to_tcio_baud(uint8_t cfg_baud, uint32_t *baud);

/*******************************************************************************
**
** Function        userial_to_baud_tcio
**
** Description     helper function converts TCIO baud rate into integer
**
** Returns         uint32_t
**
*******************************************************************************/
int userial_tcio_baud_to_int(uint32_t baud);

int userial_clock_operation(int fd, int cmd);

int init_uart_transport(int *fd_transport, tUSERIAL_CFG *p_cfg);

void deinit_uart_transport(int *fd_transport);

int do_read(int fd, unsigned char* buf, size_t len);
#endif//_UART_UTILS_H_

/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <termios.h>
#include <cutils/log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include "uart_utils.h"
#include <errno.h>
#include <string.h>

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "WCNSS_FILTER/uart_utils.c"
#endif

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
uint8_t userial_to_tcio_baud(uint8_t cfg_baud, uint32_t *baud)
{
    if (cfg_baud == USERIAL_BAUD_115200)
        *baud = B115200;
    else if (cfg_baud == USERIAL_BAUD_4M)
        *baud = B4000000;
    else if (cfg_baud == USERIAL_BAUD_3M)
        *baud = B3000000;
    else if (cfg_baud == USERIAL_BAUD_3_2M)
#ifdef B3200000
        *baud = B3200000;
#else
        *baud = BOTHER;
#endif
    else if (cfg_baud == USERIAL_BAUD_2M)
        *baud = B2000000;
    else if (cfg_baud == USERIAL_BAUD_1M)
        *baud = B1000000;
    else if (cfg_baud == USERIAL_BAUD_921600)
        *baud = B921600;
    else if (cfg_baud == USERIAL_BAUD_460800)
        *baud = B460800;
    else if (cfg_baud == USERIAL_BAUD_230400)
        *baud = B230400;
    else if (cfg_baud == USERIAL_BAUD_57600)
        *baud = B57600;
    else if (cfg_baud == USERIAL_BAUD_19200)
        *baud = B19200;
    else if (cfg_baud == USERIAL_BAUD_9600)
        *baud = B9600;
    else if (cfg_baud == USERIAL_BAUD_2400)
        *baud = B2400;
    else if (cfg_baud == USERIAL_BAUD_1200)
        *baud = B1200;
    else if (cfg_baud == USERIAL_BAUD_600)
        *baud = B600;
    else if (cfg_baud == USERIAL_BAUD_300)
        *baud = B300;
    else
    {
        ALOGE( "userial vendor open: unsupported baud idx %i", cfg_baud);
        *baud = B115200;
        return FALSE;
    }
    return TRUE;
}

/*******************************************************************************
**
** Function        userial_vendor_ioctl
**
** Description     ioctl inteface
**
** Returns         None
**
*******************************************************************************/
int userial_vendor_ioctl(int fd, userial_vendor_ioctl_op_t op, int *p_data)
{
    int err = -1;

    switch(op)
    {
#if (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)
        case USERIAL_OP_ASSERT_BT_WAKE:
            ALOGI("## userial_vendor_ioctl: Asserting BT_Wake ##");
            err = ioctl(fd, USERIAL_IOCTL_BT_WAKE_ASSERT, NULL);
            break;

        case USERIAL_OP_DEASSERT_BT_WAKE:
            ALOGI("## userial_vendor_ioctl: De-asserting BT_Wake ##");
            err = ioctl(fd, USERIAL_IOCTL_BT_WAKE_DEASSERT, NULL);
            break;

        case USERIAL_OP_GET_BT_WAKE_STATE:
            err = ioctl(fd, USERIAL_IOCTL_BT_WAKE_GET_ST, p_data);
            break;
#endif  //  (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)
        case USERIAL_OP_FLOW_ON:
            ALOGI("## userial_vendor_ioctl: UART Flow On ");
            *p_data |=TIOCM_RTS;
            err = ioctl(fd, TIOCMSET, p_data);
            break;

        case USERIAL_OP_FLOW_OFF:
            ALOGI("## userial_vendor_ioctl: UART Flow Off ");
            ioctl(fd, TIOCMGET, p_data);
            *p_data &= ~TIOCM_RTS;
            err = ioctl(fd, TIOCMSET, p_data);
            break;

        default:
            break;
    }

    return err;
}

/*******************************************************************************
**
** Function        userial_vendor_set_baud
**
** Description     Set new baud rate
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_set_baud(int fd, uint8_t userial_baud)
{
    uint32_t tcio_baud;
    struct termios termios;

    ALOGI("## userial_vendor_set_baud: %d", userial_baud);

    userial_to_tcio_baud(userial_baud, &tcio_baud);
#ifndef B3200000
    if(tcio_baud == BOTHER)
    {
        struct termios2 term2;
        if (ioctl(fd, TCGETS2, &term2) == -1) {
            ALOGE("userial vendor open: TCGETS2 error");
            return;
        }

        term2.c_ospeed = term2.c_ispeed = (speed_t) userial_tcio_baud_to_int(tcio_baud);
        term2.c_cflag &= ~CBAUD;
        term2.c_cflag |= BOTHER;

        /* Set baud rate, but not changed until last write is done */
        if (ioctl(fd, TCSETSW2, &term2) == -1) {
            ALOGE("userial vendor open: TCSETSW2 error");
            return;
        }
        ALOGI("%s: in BOTHER", __func__);
    }
    else
#endif
    {
        tcgetattr(fd, &termios);

        cfsetospeed(&termios, tcio_baud);
        cfsetispeed(&termios, tcio_baud);
        tcsetattr(fd, TCSADRAIN, &termios); /* don't change speed until last write done */
    }
}

int userial_clock_operation(int fd, int cmd)
{
    int ret = 0;

    switch (cmd)
    {
        case USERIAL_OP_CLK_ON:
        case USERIAL_OP_CLK_OFF:
             ioctl(fd, cmd);
             break;
        case USERIAL_OP_CLK_STATE:
             ret = ioctl(fd, cmd);
             break;
    }

    return ret;
}

/*******************************************************************************
**
** Function        userial_to_baud_tcio
**
** Description     helper function converts TCIO baud rate into integer
**
** Returns         uint32_t
**
*******************************************************************************/
int userial_tcio_baud_to_int(uint32_t baud)
{
    int baud_rate =0;

    switch (baud)
    {
        case B300:
            baud_rate = 300;
            break;
        case B600:
            baud_rate = 600;
            break;
        case B1200:
            baud_rate = 1200;
            break;
        case B2400:
            baud_rate = 2400;
            break;
        case B9600:
            baud_rate = 9600;
            break;
        case B19200:
            baud_rate = 19200;
            break;
        case B57600:
            baud_rate = 57600;
            break;
        case B115200:
            baud_rate = 115200;
            break;
        case B230400:
            baud_rate = 230400;
            break;
        case B460800:
            baud_rate = 460800;
            break;
        case B921600:
            baud_rate = 921600;
            break;
        case B1000000:
            baud_rate = 1000000;
            break;
        case B2000000:
            baud_rate = 2000000;
            break;
        case B3000000:
            baud_rate = 3000000;
            break;
#ifdef B3200000
        case B3200000:
#else
        case BOTHER:
#endif
            baud_rate = 3200000;
            break;
        case B4000000:
            baud_rate = 4000000;
            break;
        default:
            ALOGE( "%s: unsupported baud %d", __FUNCTION__, baud);
            break;
    }
    return baud_rate;
}

/*******************************************************************************
**
** Function        userial_vendor_get_baud
**
** Description     Get current baud rate
**
** Returns         int
**
*******************************************************************************/
int userial_vendor_get_baud(int fd)
{
    int baud_rate;
    struct termios termios;

    ALOGV("%s: Entry", __func__);

    if (fd == -1)
    {
        ALOGE("%s: uart port(%s) has not been opened", __FUNCTION__, BT_HS_UART_DEVICE );
        return -1;
    }

    tcgetattr(fd, &termios);

    baud_rate = userial_tcio_baud_to_int(cfgetispeed(&termios));
    ALOGI( "%s: Current Baudrate = %d bps", __FUNCTION__, baud_rate);
    return baud_rate;
}

void deinit_uart_transport(int *fd_transport) {
    int ret;

    if (!fd_transport || *fd_transport == -1) {
        return;
    }

    if ((ret = close(*fd_transport)) < 0 ) {
        ALOGE("%s: Close returned Error: %d\n", __func__, ret);
    }

    *fd_transport = -1;
    ALOGI("%s: Transport is being closed!", __func__);
}

int init_uart_transport(int *fd_transport, tUSERIAL_CFG *p_cfg) {

    uint32_t baud;
    uint8_t data_bits;
    uint16_t parity;
    uint8_t stop_bits;
    struct termios termios;

    *fd_transport = -1;

    if (!userial_to_tcio_baud(p_cfg->baud, &baud))
    {
        ALOGE("%s:Invalid baud value", __func__);
        return -1;
    }

    if(p_cfg->fmt & USERIAL_DATABITS_8)
        data_bits = CS8;
    else if(p_cfg->fmt & USERIAL_DATABITS_7)
        data_bits = CS7;
    else if(p_cfg->fmt & USERIAL_DATABITS_6)
        data_bits = CS6;
    else if(p_cfg->fmt & USERIAL_DATABITS_5)
        data_bits = CS5;
    else
    {
        ALOGE("%s: unsupported data bits", __func__);
        return -1;
    }

    if(p_cfg->fmt & USERIAL_PARITY_NONE)
        parity = 0;
    else if(p_cfg->fmt & USERIAL_PARITY_EVEN)
        parity = PARENB;
    else if(p_cfg->fmt & USERIAL_PARITY_ODD)
        parity = (PARENB | PARODD);
    else
    {
        ALOGE("%s: unsupported parity bit mode", __func__);
        return -1;
    }

    if(p_cfg->fmt & USERIAL_STOPBITS_1)
        stop_bits = 0;
    else if(p_cfg->fmt & USERIAL_STOPBITS_2)
        stop_bits = CSTOPB;
    else
    {
        ALOGE("%s: unsupported stop bits", __func__);
        return -1;
    }

    ALOGI("%s: opening %s", __func__, BT_HS_UART_DEVICE);

    if ((*fd_transport = open(BT_HS_UART_DEVICE, O_RDWR|O_NOCTTY)) == -1)
    {
        ALOGE("%s: unable to open %s: %s(%d)",__func__, BT_HS_UART_DEVICE,
            strerror(errno), errno);
        return -1;
    }

    tcflush(*fd_transport, TCIOFLUSH);

    tcgetattr(*fd_transport, &termios);
    cfmakeraw(&termios);

    /* Set UART Control Modes */
    termios.c_cflag &= ~(CRTSCTS); /* Clear CTS/RTS flow control bit */
    termios.c_cflag |= stop_bits; /* Stop Bit */
    termios.c_cflag |= CLOCAL; /* Ignore modem control lines */
    if (p_cfg->fmt & USERIAL_CTSRTS) {
        ALOGI("%s: HW flow control enabled", __func__);
        termios.c_cflag |= (CRTSCTS); /* Enable CTS/RTS flow control */
    }

    tcsetattr(*fd_transport, TCSANOW, &termios);
    tcflush(*fd_transport, TCIOFLUSH);
    tcflush(*fd_transport, TCIOFLUSH);

#ifndef B3200000
    if(baud == BOTHER) {
        struct termios2 term2;
        if (ioctl(*fd_transport, TCGETS2, &term2) == -1) {
            ALOGE("%s: TCGETS2 error", __func__);
            return -1;
        }
        term2.c_ospeed = term2.c_ispeed = (speed_t) userial_tcio_baud_to_int(baud);
        term2.c_cflag &= ~CBAUD;
        term2.c_cflag |= BOTHER;
        /* Set baud rate immediately */
        if (ioctl(*fd_transport, TCSETS2, &term2) == -1) {
            ALOGE("%s: TCSETS2 error", __func__);
            return -1;
        }
        tcsetattr(*fd_transport, TCSANOW, (const struct termios *)&term2);
        ALOGI("%s: in BOTHER", __func__);
    }
    else
#endif
    {
        cfsetospeed(&termios, baud);
        cfsetispeed(&termios, baud);
        tcsetattr(*fd_transport, TCSANOW, &termios);
    }

    userial_vendor_get_baud(*fd_transport);

    ALOGI("%s: device fd = %d open @%d bps", __func__, *fd_transport,
         userial_tcio_baud_to_int(baud));

    return *fd_transport;
}

int do_read(int fd, unsigned char* buf, size_t len) {
   int bytes_left, bytes_read = 0, read_offset;

   if (!len) {
      ALOGV("%s: read returned with len 0.", __func__);
      return 0;
   }

   bytes_left = len;
   read_offset = 0;

   do {
       bytes_read = read(fd, buf+read_offset, bytes_left);
       if (bytes_read < 0) {
           ALOGE("%s: Read error: %d (%s)", __func__, bytes_left, strerror(errno));
           return -1;
       } else if (bytes_read == 0) {
            ALOGE("%s: read returned 0, err = %s, read bytes: %d, expected: %d",
                              __func__, strerror(errno),(unsigned int) (len-bytes_left),(unsigned int) len);
            return (len-bytes_left);
       }
       else {
           if (bytes_read < bytes_left) {
              ALOGV("Still there are %d bytes to read", bytes_left-bytes_read);
              bytes_left = bytes_left-bytes_read;
              read_offset = read_offset+bytes_read;
           } else {
              ALOGV("%s: done with read",__func__);
              break;
           }
       }
   }while(1);
   return len;
}

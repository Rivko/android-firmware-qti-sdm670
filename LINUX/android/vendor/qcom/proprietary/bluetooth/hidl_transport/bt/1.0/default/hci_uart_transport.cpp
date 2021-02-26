/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
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

#define LOG_TAG "vendor.qti.bluetooth@1.0-uart_transport"

#include "hci_uart_transport.h"
#include <utils/Log.h>

namespace {

static tUSERIAL_CFG bt_shutdown_cfg =
{
  (USERIAL_DATABITS_8 | USERIAL_PARITY_NONE | USERIAL_STOPBITS_1),
  USERIAL_BAUD_2400  /* bit = 0.417 ms */
};
static tUSERIAL_CFG userial_init_cfg =
{
  (USERIAL_DATABITS_8 | USERIAL_PARITY_NONE | USERIAL_STOPBITS_1 | USERIAL_CTSRTS),
  USERIAL_BAUD_115200 /* bit = 8.6 us */
};

static tUSERIAL_CFG rome_normal_cfg =
{
  (USERIAL_DATABITS_8 | USERIAL_PARITY_NONE | USERIAL_STOPBITS_1 | USERIAL_CTSRTS),
  USERIAL_BAUD_3M   /* bit = 8.6 us */
};


static tUSERIAL_CFG bt_reset_cfg =
{
  (USERIAL_DATABITS_8 | USERIAL_PARITY_NONE | USERIAL_STOPBITS_1),
  USERIAL_BAUD_115200  /* bit = 8.6 us*/
};

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
  else {
    ALOGE( "userial vendor open: unsupported baud idx %i", cfg_baud);
    *baud = B115200;
    return false;
  }
  return true;
}

int userial_tcio_baud_to_int(uint32_t baud)
{
  int baud_rate = 0;

  switch (baud) {
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
      ALOGE( "%s: unsupported baud %d", __func__, baud);
      break;
  }
  return baud_rate;
}
}

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

// private functions
bool HciUartTransport::InitTransport(tUSERIAL_CFG *p_cfg)
{
  uint32_t baud;
  uint8_t data_bits;
  uint16_t parity;
  uint8_t stop_bits;
  struct termios termios;

  ctrl_fd_ = -1;

  if (soc_type_ == BT_SOC_NAPIER) {

    ALOGD("%s open '%s', p_cfg: %p", __func__, BT_HS_UART_DEVICE, p_cfg);

    ctrl_fd_ = OpenUart(BT_HS_UART_DEVICE, p_cfg);

    ALOGD("%s napier, returns fd: %d", __func__, ctrl_fd_);

  } else if (soc_type_ == BT_SOC_ROME) {

    ctrl_fd_ = OpenUart(BT_HS_UART_DEVICE, p_cfg);

    ALOGD("%s rome, returns fd: %d", __func__, ctrl_fd_);
  } else if (soc_type_ == BT_SOC_NAPLES_UART) {

    ctrl_fd_ = OpenUart(BT_HS_UART_DEVICE, p_cfg);

    ALOGD("%s naples_uart, returns fd: %d", __func__, ctrl_fd_);

  } else if (soc_type_ == BT_SOC_CHEROKEE) {

    if (p_cfg == NULL)
        return false;

    if (!userial_to_tcio_baud(p_cfg->baud, &baud)) {
      ALOGE("%s:Invalid baud value", __func__);
      return false;
    }

    if (p_cfg->fmt & USERIAL_DATABITS_8)
      data_bits = CS8;
    else if (p_cfg->fmt & USERIAL_DATABITS_7)
      data_bits = CS7;
    else if (p_cfg->fmt & USERIAL_DATABITS_6)
      data_bits = CS6;
    else if (p_cfg->fmt & USERIAL_DATABITS_5)
      data_bits = CS5;
    else {
      ALOGE("%s: unsupported data bits", __func__);
      return false;
    }

    if (p_cfg->fmt & USERIAL_PARITY_NONE)
      parity = 0;
    else if (p_cfg->fmt & USERIAL_PARITY_EVEN)
      parity = PARENB;
    else if (p_cfg->fmt & USERIAL_PARITY_ODD)
      parity = (PARENB | PARODD);
    else {
      ALOGE("%s: unsupported parity bit mode", __func__);
      return false;
    }

    if (p_cfg->fmt & USERIAL_STOPBITS_1)
      stop_bits = 0;
    else if (p_cfg->fmt & USERIAL_STOPBITS_2)
      stop_bits = CSTOPB;
    else {
      ALOGE("%s: unsupported stop bits", __func__);
      return false;
    }

    ALOGI("%s: opening %s", __func__, BT_HS_UART_DEVICE);

    if ((ctrl_fd_ = open(BT_HS_UART_DEVICE, O_RDWR | O_NOCTTY)) == -1) {
      ALOGE("%s: unable to open %s: %s(%d)", __func__, BT_HS_UART_DEVICE,
            strerror(errno), errno);
      return false;
    }

    ClockOperation(USERIAL_OP_CLK_ON);
    ALOGD("userial clock on");

    tcflush(ctrl_fd_, TCIOFLUSH);

    tcgetattr(ctrl_fd_, &termios);
    cfmakeraw(&termios);

    /* Set UART Control Modes */
    termios.c_cflag &= ~(CRTSCTS); /* Clear CTS/RTS flow control bit */
    termios.c_cflag |= stop_bits;  /* Stop Bit */
    termios.c_cflag |= CLOCAL;     /* Ignore modem control lines */
    if (p_cfg->fmt & USERIAL_CTSRTS) {
      ALOGI("%s: HW flow control enabled", __func__);
      termios.c_cflag |= (CRTSCTS); /* Enable CTS/RTS flow control */
    }

    tcsetattr(ctrl_fd_, TCSANOW, &termios);
    tcflush(ctrl_fd_, TCIOFLUSH);
    tcflush(ctrl_fd_, TCIOFLUSH);

#ifndef B3200000
    if (baud == BOTHER) {
      struct termios2 term2;
      if (ioctl(ctrl_fd_, TCGETS2, &term2) == -1) {
        ALOGE("%s: TCGETS2 error", __func__);
        return false;
      }
      term2.c_ospeed = term2.c_ispeed = (speed_t)userial_tcio_baud_to_int(baud);
      term2.c_cflag &= ~CBAUD;
      term2.c_cflag |= BOTHER;
      /* Set baud rate immediately */
      if (ioctl(ctrl_fd_, TCSETS2, &term2) == -1) {
        ALOGE("%s: TCSETS2 error", __func__);
        return false;
      }
      tcsetattr(ctrl_fd_, TCSANOW, (const struct termios*)&term2);
      ALOGI("%s: in BOTHER", __func__);
    } else
#endif
    {
      cfsetospeed(&termios, baud);
      cfsetispeed(&termios, baud);
      tcsetattr(ctrl_fd_, TCSANOW, &termios);
    }

    GetBaudRate();

    ALOGI("%s: device fd = %d open @%d bps", __func__, ctrl_fd_,
          userial_tcio_baud_to_int(baud));

  }
  return true;
}

void HciUartTransport::DeInitTransport(void)
{
  int ret;

  ALOGV("%s: ctrl_fd_ = %d", __func__, ctrl_fd_);
  if (!ctrl_fd_ || ctrl_fd_ == -1) {
    return;
  }

  if (!foce_to_exit) {
    internal_mutex_.lock();
  }

  if(vote_counter) {
    ALOGE("%s: UART Clock not voted off, voting off now", __func__);
    ioctl(ctrl_fd_, USERIAL_OP_CLK_OFF);
  }
  if ((ret = close(ctrl_fd_)) < 0 ) {
    ALOGE("%s: Close returned Error: %d\n", __func__, ret);
  }

  ctrl_fd_ = -1;
  if (!foce_to_exit) {
    internal_mutex_.unlock();
  }

  ALOGI("%s: Transport is being closed!", __func__);
}

int HciUartTransport::OpenUart(const char *device, tUSERIAL_CFG *p_cfg)
{
  int fd = 0;

  if (!device || !p_cfg) {
    ALOGE("%s: Invalid device name or uart config", __func__);
    return -1;
  }

  fd = open(device, O_RDWR);

  if (fd < 0) {
    ALOGE("%s: Unable to open %s, %d (%s)", __func__, device, errno, strerror(errno));
    return -1;
  }

  if (!ConfigUart(fd, p_cfg)) {
    ALOGE("%s: Fail to config uart %d", __func__, fd);
    close(fd);
    return -1;
  }

  ALOGD("%s: succeed to open %s, fd: %d", __func__, device, fd);

  return fd;
}

bool HciUartTransport::ConfigUart(int fd, tUSERIAL_CFG *p_cfg)
{
  uint32_t baud = 0;
  uint16_t fmt = 0;
  uint8_t data_bits = 0;
  uint16_t parity = 0;
  uint8_t stop_bits = 0;
  struct termios termios;

  ALOGD("%s: fd: %d, p_cfg: %p", __func__, fd, p_cfg);

  userial_to_tcio_baud(p_cfg->baud, &baud);

  fmt = p_cfg->fmt;

  ALOGD("%s: baud {0x%x, 0x%x}, fmt: 0x%x", __func__, p_cfg->baud, baud, fmt);

  if (fmt & USERIAL_DATABITS_8) {
    data_bits = CS8;
  } else if (fmt & USERIAL_DATABITS_7) {
    data_bits = CS7;
  } else if (fmt & USERIAL_DATABITS_6) {
    data_bits = CS6;
  } else if (fmt & USERIAL_DATABITS_5) {
    data_bits = CS5;
  }

  if (fmt & USERIAL_PARITY_NONE) {
    parity = 0;
  } else if (fmt & USERIAL_PARITY_EVEN) {
    parity = PARENB;
  } else if (fmt & USERIAL_PARITY_ODD) {
    parity = (PARENB | PARODD);
  } else {
    ALOGE("%s: invalid uart parity", __func__);
    return false;
  }

  if (fmt & USERIAL_STOPBITS_1) {
    stop_bits = 0;
  } else if (fmt & USERIAL_STOPBITS_2) {
    stop_bits = CSTOPB;
  } else {
    ALOGE("%s: invalid uart stop bit", __func__);
    return false;
  }

  ALOGD("%s: data_bits: 0x%x, parity: 0x%x, stop_bits: 0x%x", __func__, data_bits, parity, stop_bits);

  tcflush(fd, TCIOFLUSH);

  tcgetattr(fd, &termios);
  cfmakeraw(&termios);

  /* Set uart control modes */
  termios.c_cflag &= ~(CRTSCTS);  /* Clear CTS/RTS flow control bit */
  termios.c_cflag |= stop_bits;   /* Stop Bit */
  termios.c_cflag |= CLOCAL;      /* Ignore modem control lines */

  if (fmt & USERIAL_CTSRTS) {
    ALOGI("%s: HW flow control enabled", __func__);
    termios.c_cflag |= (CRTSCTS); /* Enable CTS/RTS flow control */
  }

  tcsetattr(fd, TCSANOW, &termios);

  tcflush(fd, TCIOFLUSH);

  tcsetattr(fd, TCSANOW, &termios);

  tcflush(fd, TCIOFLUSH);

  /* Set input/output baudrate */
  cfsetospeed(&termios, baud);
  cfsetispeed(&termios, baud);
  tcsetattr(fd, TCSANOW, &termios);

  return true;
}

// public functions

int HciUartTransport::GetCtrlFd()
{
  return ctrl_fd_;
}
int HciUartTransport::GetDataFd()
{
  return ctrl_fd_;
}

int HciUartTransport::ClockOperation(int cmd)
{
  int ret = 0;

  switch (cmd) {
    case USERIAL_OP_CLK_ON:
      vote_counter++;
      if(vote_counter == 1)
        ret = ioctl(ctrl_fd_, cmd);
      break;
    case USERIAL_OP_CLK_OFF:
      vote_counter--;
      if(vote_counter < 0)
        vote_counter = 0;
      else if(vote_counter == 0)
        ret = ioctl(ctrl_fd_, cmd);
      break;
    case USERIAL_OP_CLK_STATE:
      ret = ioctl(ctrl_fd_, cmd);
      break;
  }
  return ret;
}

int HciUartTransport::Ioctl(userial_vendor_ioctl_op_t op, int *p_data)
{
  int err = -1;

  switch (op) {
#if (BT_WAKE_VIA_USERIAL_IOCTL == TRUE)
    case USERIAL_OP_ASSERT_BT_WAKE:
      ALOGI("## userial_vendor_ioctl: Asserting BT_Wake ##");
      err = ioctl(ctrl_fd_, USERIAL_IOCTL_BT_WAKE_ASSERT, NULL);
      break;

    case USERIAL_OP_DEASSERT_BT_WAKE:
      ALOGI("## userial_vendor_ioctl: De-asserting BT_Wake ##");
      err = ioctl(ctrl_fd_, USERIAL_IOCTL_BT_WAKE_DEASSERT, NULL);
      break;

    case USERIAL_OP_GET_BT_WAKE_STATE:
      err = ioctl(ctrl_fd_, USERIAL_IOCTL_BT_WAKE_GET_ST, p_data);
      break;
#endif        //  (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)
    case USERIAL_OP_FLOW_ON:
      ALOGI("## userial_vendor_ioctl: UART Flow On ");
      *p_data |= TIOCM_RTS;
      err = ioctl(ctrl_fd_, TIOCMSET, p_data);
      break;

    case USERIAL_OP_FLOW_OFF:
      ALOGI("## userial_vendor_ioctl: UART Flow Off ");
      ioctl(ctrl_fd_, TIOCMGET, p_data);
      *p_data &= ~TIOCM_RTS;
      err = ioctl(ctrl_fd_, TIOCMSET, p_data);
      break;

    default:
      break;
  }
  return err;
}

int HciUartTransport::Read(unsigned char* buf, size_t len)
{
  int bytes_left, bytes_read = 0, read_offset;

  if (!len) {
    ALOGV("%s: read returned with len 0.", __func__);
    return 0;
  }

  bytes_left = len;
  read_offset = 0;

  do {
    bytes_read = read(ctrl_fd_, buf + read_offset, bytes_left);
    if (bytes_read < 0) {
      ALOGE("%s: Read error: %d (%s)", __func__, bytes_left, strerror(errno));
      return -1;
    } else if (bytes_read == 0) {
      ALOGE("%s: read returned 0, err = %s, read bytes: %d, expected: %d",
            __func__, strerror(errno), (unsigned int)(len - bytes_left), (unsigned int)len);
      return (len - bytes_left);
    } else {
      if (bytes_read < bytes_left) {
        ALOGV("Still there are %d bytes to read", bytes_left - bytes_read);
        bytes_left = bytes_left - bytes_read;
        read_offset = read_offset + bytes_read;
      } else {
        ALOGV("%s: done with read", __func__);
        break;
      }
    }
  } while (1);
  return len;
}

int HciUartTransport::WriteSafely(const uint8_t *data, int length)
{
  size_t write_len = 0;

  while (length > 0) {
    ssize_t ret =
      TEMP_FAILURE_RETRY(write(ctrl_fd_, data + write_len, length));

    if (ret == -1) {
      if (errno == EAGAIN) continue;
      ALOGE("%s error writing to UART (%s)", __func__, strerror(errno));
      break;

    } else if (ret == 0) {
      // Nothing written :(
      ALOGE("%s zero bytes written - something went wrong...", __func__);
      break;
    }

    write_len += ret;
    length -= ret;
  }
  return write_len;
}

int HciUartTransport::UartWrite(const uint8_t *buf, int len)
{
  std::unique_lock<std::mutex> guard(internal_mutex_);
  return WriteSafely(buf, len);
}

int HciUartTransport::Write( HciPacketType type, const uint8_t *buf, int len)
{
  int ret = 0;

  std::unique_lock<std::mutex> guard(internal_mutex_);
  // write the type first
  if ((ret = write( ctrl_fd_, reinterpret_cast<unsigned char *> (&type), 1)) < 0) {
    ALOGE("HciUartTransport:: write() failed");
    return ret;
  }
  // write the data now
  return WriteSafely(buf, len);
}

/*******************************************************************************
**
** Function        SetBaudRate
**
** Description     Set new baud rate
**
** Returns         None
**
*******************************************************************************/
void HciUartTransport::SetBaudRate(uint8_t userial_baud)
{
  uint32_t tcio_baud;
  struct termios termios;

  ALOGI("## userial_vendor_set_baud: %d", userial_baud);

  userial_to_tcio_baud(userial_baud, &tcio_baud);
#ifndef B3200000
  if (tcio_baud == BOTHER) {
    struct termios2 term2;
    if (ioctl(ctrl_fd_, TCGETS2, &term2) == -1) {
      ALOGE("userial vendor open: TCGETS2 error");
      return;
    }

    term2.c_ospeed = term2.c_ispeed = (speed_t)userial_tcio_baud_to_int(tcio_baud);
    term2.c_cflag &= ~CBAUD;
    term2.c_cflag |= BOTHER;

    /* Set baud rate, but not changed until last write is done */
    if (ioctl(ctrl_fd_, TCSETSW2, &term2) == -1) {
      ALOGE("userial vendor open: TCSETSW2 error");
      return;
    }
    ALOGI("%s: in BOTHER", __func__);
  } else
#endif
  {
    tcgetattr(ctrl_fd_, &termios);

    cfsetospeed(&termios, tcio_baud);
    cfsetispeed(&termios, tcio_baud);
    tcsetattr(ctrl_fd_, TCSADRAIN, &termios); /* don't change speed until last write done */
  }
}

int HciUartTransport::GetBaudRate(void)
{
  int baud_rate;
  struct termios termios;

  ALOGV("%s: Entry", __func__);

  if (ctrl_fd_ == -1) {
    ALOGE("%s: uart port(%s) has not been opened", __func__, BT_HS_UART_DEVICE );
    return -1;
  }

  tcgetattr(ctrl_fd_, &termios);

  baud_rate = userial_tcio_baud_to_int(cfgetispeed(&termios));
  ALOGI( "%s: Current Baudrate = %d bps", __func__, baud_rate);
  return baud_rate;
}

uint8_t HciUartTransport::GetMaxBaudrate()
{
  switch (soc_type_) {
    case BT_SOC_CHEROKEE:
#ifdef UART_BAUDRATE_3_0_MBPS
      return (uint8_t) USERIAL_BAUD_3M;
#else
      return (uint8_t) USERIAL_BAUD_3_2M;
#endif
      break;

    case BT_SOC_NAPIER:
      return (uint8_t) USERIAL_BAUD_3_2M;
      break;

    case BT_SOC_ROME:
    case BT_SOC_NAPLES_UART:
    default:
      return (uint8_t) USERIAL_BAUD_3M;
  }
}

bool HciUartTransport::Init(BluetoothSocType soc_type)
{
  int len;
  bool status = false;
  char reset_val = 0xFC;
  vote_counter = 0;

  ALOGD("%s:> soc_type: %d", __func__, soc_type);

  this->foce_to_exit = false;
  this->soc_type_ = soc_type;

  switch (soc_type) {
    case BT_SOC_NAPIER:

      /* fall through */
    case BT_SOC_NAPLES_UART:
    case BT_SOC_ROME: {
      /* The initial baudrate to boot QCA BT should be 115200bps, instead of 3Mbps. */
      InitTransport(&userial_init_cfg);
    }
    break;

    case BT_SOC_CHEROKEE: {

      // TODO to remove it and proper fix
      CleanUp();

      if (!InitTransport(&bt_reset_cfg)) {
        break;
      }

#ifdef WAKE_LOCK_ENABLED
      Wakelock :: Acquire();
#endif

      /* Give some delay before clock and uart driver is ramping up and ready */
      usleep(200); /* 200 us delay */

      /* UART TxD control as BT Reset*/
      /* 0xFF = '0' start bit = 8.6 us */
      len = write(ctrl_fd_, &reset_val, 1);
      if (len != 1 ) {
        ALOGE("%s: Send failed with ret value: %d", __func__, len);
        /*CHECK: Does this need Hammer shutdown here?*/
        ClockOperation(USERIAL_OP_CLK_OFF);
        ALOGD("userial clock off");
        DeInitTransport();
        break;
      }
      tcdrain(ctrl_fd_);
      ALOGI("%s:send out poweron signal", __func__);
      /* Clock off */
      ClockOperation(USERIAL_OP_CLK_OFF);
      ALOGD("userial clock off");

#ifdef WAKE_LOCK_ENABLED
      Wakelock :: Release();
#endif

      /* Close uart port for the reset handler */
      DeInitTransport();
      if (!InitTransport((tUSERIAL_CFG*)&userial_init_cfg)) {
        ALOGE("InitTransport returns err");
        ClockOperation(USERIAL_OP_CLK_OFF);
        ALOGD("userial clock off");
        break;
      }

      ClockOperation(USERIAL_OP_CLK_OFF);
      ALOGD("userial clock off");
      /* For Cherokee, it need to wait for 100ms */
      usleep(100 * 1000);
      status = true;
    }
    break;

    default: {
      ALOGE("Unknown chip type: %d", soc_type);
    }
  }
  return status;
}

bool HciUartTransport::CleanUp()
{
  int fd, len, retval = 0;
  char shutdown_val = 0xC0;
  bool status = false;

  ALOGV("%s:> soc_type: %d", __func__, soc_type_);

  switch (soc_type_) {
    case BT_SOC_NAPIER:

      /* fall through */
    case BT_SOC_NAPLES_UART:
    case BT_SOC_ROME: {
      ClockOperation(USERIAL_OP_CLK_OFF);
      ALOGD("userial clock off");
      DeInitTransport();
    }
    break;

    case BT_SOC_CHEROKEE: {
      ClockOperation(USERIAL_OP_CLK_OFF);
      ALOGD("userial clock off");
      DeInitTransport();
      usleep(10 * 1000); /* 10 ms delay */

      /* Open at 2400 bps baud */
      if (!InitTransport(&bt_shutdown_cfg)) {
        ClockOperation(USERIAL_OP_CLK_OFF);
        ALOGD("userial clock off");
        ALOGE("%s: userial_vendor_open returns err: %d", __func__, ctrl_fd_);
        break;
      }

      /* Give some delay before clock and uart driver is ramping up and ready */
      usleep(20 * 1000); /* 20 ms delay */

      /* UART TxD control as BT Reset*/
      /* 0xF0 = 4 '0' single bit = 0.416 ms * 4 + start bit 0.416ms ~= 2.08 ms */
      len = write(ctrl_fd_, &shutdown_val, 1);
      if (len != 1) {
        ALOGE("%s: Send failed with ret value: %d", __func__, len);
      }
      tcdrain(ctrl_fd_);
      ALOGI("%s:send out shutdown signal", __func__);
      ClockOperation(USERIAL_OP_CLK_OFF);
      ALOGD("userial clock off");
      DeInitTransport();
      usleep(100 * 1000); /* 100 ms delay */
      status = true;
    }
    break;

    default: {
      ALOGE("Unknown chip type: %d", soc_type_);
    }
  }

  ALOGV("%s:< %d", __func__, retval);
  return status;
}

void HciUartTransport::Disconnect() {
  if (ctrl_fd_ > 0) {
    //make sure RTS is flowed on
    int flag = 0;
    ioctl(ctrl_fd_, TIOCMGET, &flag);
    flag |= TIOCM_RTS;
    ioctl(ctrl_fd_, TIOCMSET, &flag);

    ALOGD("%s: discard unsent data", __func__);
    tcflush(ctrl_fd_, TCIOFLUSH);
    foce_to_exit = true;
    CleanUp();
  }
}

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android


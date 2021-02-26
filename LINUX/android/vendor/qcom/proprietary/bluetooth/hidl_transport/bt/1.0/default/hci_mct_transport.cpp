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

#define LOG_TAG "vendor.qti.bluetooth@1.0-mct_transport"

#include "hci_mct_transport.h"
#include <cutils/properties.h>
#include <utils/Log.h>

namespace {

static const char *DevSmdNodes[] = {
    "/dev/smd3",
    "/dev/smd2"
};

int InitTransport (int channel_id )
{
  struct termios term;
  int fd = -1;
  int retry = 0;

  if(channel_id >= 2 || channel_id < 0)
     return -1;

  fd = open(DevSmdNodes[channel_id], (O_RDWR | O_NOCTTY));

  while ((-1 == fd) && (retry < 7)) {
    ALOGE("InitTransport: Cannot open %s: %s\n. Retry after 2 seconds",
        DevSmdNodes[channel_id], strerror(errno));
    usleep(2000000);
    fd = open(DevSmdNodes[channel_id], (O_RDWR | O_NOCTTY));
    retry++;
  }

  if (-1 == fd) {
    ALOGE("InitTransport: Cannot open %s: %s\n",
        DevSmdNodes[channel_id], strerror(errno));
    return -1;
  }

  if (tcflush(fd, TCIOFLUSH) < 0) {
    ALOGE("InitTransport: Cannot flush %s\n", DevSmdNodes[channel_id]);
    close(fd);
    return -1;
  }

  if (tcgetattr(fd, &term) < 0) {
    ALOGE("InitTransport: Error while getting attributes\n");
    close(fd);
    return -1;
  }

  cfmakeraw(&term);

  term.c_cflag |= (CRTSCTS | CLOCAL);

  if (tcsetattr(fd, TCSANOW, &term) < 0) {
    ALOGE("InitTransport: Error while getting attributes\n");
    close(fd);
    return -1;
  }
  return fd;
}

}

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

bool HciMctTransport::Init(BluetoothSocType soc_type)
{
  bool status = false;

  ALOGD("%s:> soc_type: %d", __func__, soc_type_);

  this->soc_type_ = soc_type;

  switch (soc_type) {
    case BT_SOC_SMD:
    {
      ctrl_fd_ = InitTransport(0);
      if(ctrl_fd_ > 0) {
        data_fd_ = InitTransport(1);

        if(data_fd_ < 0)
          close(ctrl_fd_);
      }
      if((ctrl_fd_ > 0) && (data_fd_ > 0)) {
          ALOGI("Done intiailizing Bluetooth SMD transport \n");
          status = true;
      }
    }
    break;
    default:
    {
      ALOGE("Unknown chip type: %d", soc_type);
    }
  }
  return status;
}

// public functions
int HciMctTransport::GetCtrlFd()
{
  return ctrl_fd_;
}

int HciMctTransport::GetDataFd()
{
  return data_fd_;
}

bool HciMctTransport::CleanUp()
{
  bool status = true;

  ALOGI("%s:> soc_type: %d", __func__, soc_type_);

  switch (soc_type_) {
    case BT_SOC_SMD:
    {
      if(ctrl_fd_ >= 0) close(ctrl_fd_);
      if(data_fd_ >= 0) close(data_fd_);
    }
    break;
    default:
    {
      ALOGE("Unknown chip type: %d", soc_type_);
    }
  }
  return status;
}

int HciMctTransport::Read(unsigned char* buf, size_t len)
{
  int bytes_left, bytes_read = 0, read_offset;

  if (!len) {
    ALOGE("%s: read returned with len 0.", __func__);
    return 0;
  }

  bytes_left = len;
  read_offset = 0;

  do {
    bytes_read = TEMP_FAILURE_RETRY(read(ctrl_fd_, buf + read_offset, bytes_left));
    if (bytes_read < 0) {
      ALOGE("%s: Read error: %d (%s)", __func__, bytes_left, strerror(errno));
      return -1;
    } else if (bytes_read == 0) {
      ALOGE("%s: read returned 0, err = %s, read bytes: %d, expected: %d",
            __func__, strerror(errno), (unsigned int)(len - bytes_left),
            (unsigned int)len);
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

int HciMctTransport::Write( HciPacketType type, const uint8_t *buf, int len)
{
  std::unique_lock<std::mutex> guard(internal_mutex_);

  // write the data now
  if(type == HCI_PACKET_TYPE_COMMAND) {
    if(ctrl_fd_ >= 0)
      return WriteSafely(ctrl_fd_, buf, len);
  } else if(type == HCI_PACKET_TYPE_ACL_DATA) {
    if(data_fd_ >= 0)
      return WriteSafely(data_fd_, buf, len);
  }
  return 0;
}

int HciMctTransport::WriteSafely(int fd, const uint8_t *data, int length)
{
  size_t write_len = 0;

  while (length > 0) {
    ssize_t ret =
      TEMP_FAILURE_RETRY(write(fd, data + write_len, length));

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

} // namespace implementation
} // namespace V1_0
} // namespace bluetooth
} // namespace hardware
} // namespace android


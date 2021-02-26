/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 */
//
// Copyright 2016 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <stdlib.h>

/* Vendor Specific Command */
#define BT_VS_CMD_OPCODE 0xFC
#define BT_VS_PRE_SHUTDOWN_EVT 0x08

// HCI UART transport packet types (Volume 4, Part A, 2)
enum HciPacketType {
  HCI_PACKET_TYPE_UNKNOWN = 0,
  HCI_PACKET_TYPE_COMMAND = 1,
  HCI_PACKET_TYPE_ACL_DATA = 2,
  HCI_PACKET_TYPE_SCO_DATA = 3,
  HCI_PACKET_TYPE_EVENT = 4,
  HCI_PACKET_TYPE_ANT_CTRL = 0x0c,
  HCI_PACKET_TYPE_ANT_DATA = 0x0e,
  HCI_PACKET_TYPE_FM_CMD = 0x11,
  HCI_PACKET_TYPE_FM_EVENT = 0x14
};

// 2 bytes for opcode, 1 byte for parameter length (Volume 2, Part E, 5.4.1)
const size_t HCI_COMMAND_PREAMBLE_SIZE = 3;
const size_t HCI_LENGTH_OFFSET_CMD = 2;

// 2 bytes for handle, 2 bytes for data length (Volume 2, Part E, 5.4.2)
const size_t HCI_ACL_PREAMBLE_SIZE = 4;
const size_t HCI_LENGTH_OFFSET_ACL = 2;

// 2 bytes for handle, 1 byte for data length (Volume 2, Part E, 5.4.3)
const size_t HCI_SCO_PREAMBLE_SIZE = 3;
const size_t HCI_LENGTH_OFFSET_SCO = 2;

// 1 byte for event code, 1 byte for parameter length (Volume 2, Part E, 5.4.4)
const size_t HCI_EVENT_PREAMBLE_SIZE = 2;
const size_t HCI_LENGTH_OFFSET_EVT = 1;
const int LENGTH_HW_ERROR_EVT = 3;

const size_t HCI_PREAMBLE_SIZE_MAX = HCI_ACL_PREAMBLE_SIZE;

// Event codes (Volume 2, Part E, 7.7.14)
const uint8_t HCI_COMMAND_COMPLETE_EVENT = 0x0E;

const size_t HCI_ANT_CMD_PREAMBLE_SIZE = 1;
const size_t HCI_LENGTH_OFFSET_ANT_CMD = 0;

const size_t HCI_FM_CMD_PREAMBLE_SIZE = 3;
const size_t HCI_LENGTH_OFFSET_FM_CMD = 2;

const size_t HCI_FM_EVENT_PREAMBLE_SIZE = 2;
const size_t HCI_LENGTH_OFFSET_FM_EVT = 1;
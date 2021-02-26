//-------------------------------------------------------------------------
// Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  ipebpsdefs.h
// @brief IPE BPS command & message definitions.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
//
// 11/Jan/2016       shirb        Initial version
//------------------------------------------------------------------------

#ifndef _IPE_BPS_DEFS_H_
#define _IPE_BPS_DEFS_H_

#include "hfidefs.h"
#include "ipe_data.h"
#include "bps_data.h"
#include "ipebps_data.h"

/**
   Level 2 Comment: "High-Level FW Driver Command Types"
   The section defines high-level driver control/data commands (HFI_CMD_IPEBPS_X).

   All the responses from FW are generated asynchronously. The responses are
   generated as messages and are defined as HFI_MSG_IPEBPS_X_ACK.
   */

/**
   This command creates an IPE/BPS handle.

   @return
   HFI_MSG_IPEBPS_CREATE_HANDLE_ACK

   @datatypes
   HFI_CMD_IPEBPS_CREATE_HANDLE_TYPE
*/
#define  HFI_CMD_IPEBPS_CREATE_HANDLE              (HFI_CMD_IPE_BPS_COMMON_START + 0x08)

/**
   This message is sent in response to the HFI_CMD_IPEBPS_CREATE_HANDLE command.

   @return
   NA

   @datatypes
   HFI_MSG_IPEBPS_CREATE_HANDLE_ACK_TYPE
*/
#define  HFI_MSG_IPEBPS_CREATE_HANDLE_ACK          (HFI_MSG_IPE_BPS_COMMON_START + 0x08)

/**
   This command enqueues an ASYNC operation.

   @return
   HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK

   @datatypes
   HFI_CMD_IPEBPS_ASYNC_COMMAND_DIRECT_TYPE
*/
#define  HFI_CMD_IPEBPS_ASYNC_COMMAND_DIRECT       (HFI_CMD_IPE_BPS_COMMON_START + 0x0a)

/**
   This message is sent in response to the HFI_CMD_IPEBPS_ASYNC_COMMAND command.

   @return
   NA

   @datatypes
   HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK_TYPE
*/
#define  HFI_MSG_IPEBPS_ASYNC_COMMAND_DIRECT_ACK   (HFI_MSG_IPE_BPS_COMMON_START + 0x0a)

/**
This command enqueues an ASYNC indirect operation.

@return
HFI_MSG_IPEBPS_ASYNC_COMMAND_INDIRECT_ACK

@datatypes
HFI_CMD_IPEBPS_ASYNC_COMMAND_INDIRECT_TYPE
*/
#define  HFI_CMD_IPEBPS_ASYNC_COMMAND_INDIRECT              (HFI_CMD_IPE_BPS_COMMON_START + 0x0e)


/**
This message is sent in response to the HFI_CMD_IPEBPS_ASYNC_COMMAND_INDIRECT command.

@return
NA

@datatypes
HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK_INDIRECT_TYPE
*/
#define  HFI_MSG_IPEBPS_ASYNC_COMMAND_INDIRECT_ACK          (HFI_MSG_IPE_BPS_COMMON_START + 0x0e)


/**
   Level 2 Comment: "High-Level FW Driver Asynchronous Command Opcodes"
*/

/**
   This command opcode configures BPS input and output buffers.

   @datatypes in cmdPayload in the HFI_CMD_IPEBPS_ASYNC_COMMAND
   BpsConfigIo (bps_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_BPS_CONFIG_IO         0x01

/**
   This command opcode provides all parameters needs for processing
   a BPS frame including input/output buffers and IQ settings.

   @datatypes in cmdPayload in the HFI_CMD_IPEBPS_ASYNC_COMMAND
   BpsFrameProcess (bps_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_BPS_FRAME_PROCESS     0x02

/**
   This command opcode aborts all outstanding BPS ProcessFrame
   requests.

   @datatypes in cmdPayload in the HFI_CMD_IPEBPS_ASYNC_COMMAND
   BpsAbort (bps_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_BPS_ABORT             0x03

/**
   This command opcode destroys a BPS instance.

   @datatypes in cmdPayload in the HFI_CMD_IPEBPS_ASYNC_COMMAND
   BpsDestroy (bps_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_BPS_DESTROY           0x04

/**
   This command opcode configures IPE input and output buffers.

   @datatypes in cmdPayload in the HFI_CMD_IPEBPS_ASYNC_COMMAND
   IpeConfigIo (ipe_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_IPE_CONFIG_IO         0x05

/**
   This command opcode provides all parameters needs for processing
   an IPE frame including input/output buffers and IQ settings.

   @datatypes in cmdPayload in the HFI_CMD_IPEBPS_ASYNC_COMMAND
   IpeFrameProcess (ipe_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_IPE_FRAME_PROCESS     0x06

/**
   This command opcode aborts all outstanding IPE ProcessFrame
   requests.

   @datatypes in cmdPayload in the HFI_CMD_IPEBPS_ASYNC_COMMAND
   IpeAbort (ipe_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_IPE_ABORT             0x07

/**
   This command opcode destroys an IPE instance.

   @datatypes in cmdPayload in the HFI_CMD_IPEBPS_ASYNC_COMMAND
   IpeDestroy (ipe_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_IPE_DESTROY           0x08

/**
   This command opcode requests for synchronization for BPS
   to wait for IPE (chaining).

   @return cmdPayload in HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK
   HandleWaitDone

   @datatypes in cmdPayload in the HFI_CMD_IPEIPE_ASYNC_COMMAND
   BpsWaitForIpe
*/
#define HFI_IPEBPS_CMD_OPCODE_BPS_WAIT_FOR_IPE      0x09

/**
   This command opcode requests for synchronization for BPS
   to wait for BPS (chaining).

   @return cmdPayload in HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK
   HandleWaitDone

   @datatypes in cmdPayload in the HFI_CMD_IPEIPE_ASYNC_COMMAND
   BpsWaitForBps
*/
#define HFI_IPEBPS_CMD_OPCODE_BPS_WAIT_FOR_BPS      0x0a

/**
   This command opcode requests for synchronization for IPE
   to wait for BPS (chaining).

   @return cmdPayload in HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK
   HandleWaitDone

   @datatypes in cmdPayload in the HFI_CMD_IPEIPE_ASYNC_COMMAND
   IpeWaitForBps
*/
#define HFI_IPEBPS_CMD_OPCODE_IPE_WAIT_FOR_BPS      0x0b

/**
   This command opcode requests for synchronization for IPE
   to wait for IPE (chaining).

   @return cmdPayload in HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK
   HandleWaitDone

   @datatypes in cmdPayload in the HFI_CMD_IPEIPE_ASYNC_COMMAND
   IpeWaitForIpe
*/
#define HFI_IPEBPS_CMD_OPCODE_IPE_WAIT_FOR_IPE      0x0c

/**
    This command opcode retrieves the cycle count from the
    performance counters

    @return cmdPayload in HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK
    IpeBpsPerfCountDone (ipebps_data.h)

    @datatypes in cmdPayload in the HFI_CMD_IPEIPE_ASYNC_COMMAND
    IpeBpsPerfCount (ipebps_data.h)
*/
#define HFI_IPEBPS_CMD_OPCODE_PERF_COUNT  0x0d


/**
   Level 2 Comment: "High-Level FW Driver Handle Types"
   */
#define HFI_IPEBPS_HANDLE_TYPE_BPS                  0x1
#define HFI_IPEBPS_HANDLE_TYPE_IPE_RT               0x2
#define HFI_IPEBPS_HANDLE_TYPE_IPE_NON_RT           0x3
#define HFI_IPEBPS_HANDLE_TYPE_IPE_SEMI_RT          0x4

#pragma pack(push)
#pragma pack(1)

/**
   Level 2 Comment: "High-Level FW Driver Packet Definitions"
   */
typedef struct HFI_CMD_IPEBPS_CREATE_HANDLE_TYPE
{
    uint32_t nSize;                                 /**< The packet size, in bytes. */
    uint32_t ePacketType;                           /**< HFI_CMD_IPEBPS_CREATE_HANDLE */
    uint32_t eHandleType;                           /**< HFI_IPEBPS_HANDLE_TYPE_X. */
    uint64_t nUserArg1;                             /**< Caller-provided argument #1. */
    uint64_t nUserArg2;                             /**< Caller-provided argument #2. */
} HFI_CMD_IPEBPS_CREATE_HANDLE_TYPE;

typedef struct HFI_MSG_IPEBPS_CREATE_HANDLE_ACK_TYPE
{
    uint32_t nSize;                                 /**< The packet size, in bytes. */
    uint32_t ePacketType;                           /**< HFI_MSG_IPEBPS_CREATE_HANDLE_ACK */
    uint32_t eErrorType;                            /**< CameraICPResult code. */
    uint32_t hHandle;                               /**< Output BPS/IPE handle. */
    uint64_t nUserArg1;                             /**< Caller-provided argument #1. */
    uint64_t nUserArg2;                             /**< Caller-provided argument #2. */
} HFI_MSG_IPEBPS_CREATE_HANDLE_ACK_TYPE;

typedef struct HFI_CMD_IPEBPS_ASYNC_COMMAND_TYPE
{
    uint32_t nSize;                                 /**< The packet size, in bytes. */
    uint32_t ePacketType;                           /**< HFI_CMD_IPEBPS_ASYNC_COMMAND_DIRECT */
    uint32_t eCmdOpcode;                            /**< HFI_IPEBPS_CMD_OPCODE_X*/
    uint64_t nUserArg1;                             /**< Caller-provided argument #1. */
    uint64_t nUserArg2;                             /**< Caller-provided argument #2. */
    uint32_t nNumHandles;                           /**< Number of BPS/IPE handles in array. */
    uint32_t hHandles[1];                           /**< BPS/IPE handle array. */
    union
    {
        uint32_t    direct[1];                      /**< BPS/IPE API struct by value. Refer in (ipe/bps)_data.h*/
        SMMU_ADDR   indirectAddr;                   /**< BPS/IPE indirect address. Refer in (ipe/bps)_data.h*/
    } payload;
} HFI_CMD_IPEBPS_ASYNC_COMMAND_TYPE;

typedef struct HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK_TYPE
{
    uint32_t nSize;                                 /**< The packet size, in bytes. */
    uint32_t ePacketType;                           /**< HFI_MSG_IPEBPS_ASYNC_COMMAND_DIRECT_ACK */
    uint32_t eCmdOpcode;                            /**< HFI_IPEBPS_CMD_OPCODE_X */
    uint64_t nUserArg1;                             /**< Caller-provided argument #1. */
    uint64_t nUserArg2;                             /**< Caller-provided argument #2. */
    uint32_t eErrorType;                            /**< CameraICPResult code. */
    uint32_t cmdPayload[1];                         /**< BPS/IPE DONE struct by value. Refer in (ipe/bps)_data.h*/
} HFI_MSG_IPEBPS_ASYNC_COMMAND_ACK_TYPE;

#pragma pack(pop)

#endif // _IPE_BPS_DEFS_H_

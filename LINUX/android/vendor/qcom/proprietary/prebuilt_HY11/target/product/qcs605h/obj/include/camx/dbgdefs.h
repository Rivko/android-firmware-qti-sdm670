//-------------------------------------------------------------------------
// Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @file  dbgdefs.h
// @brief Implementation of the DomainWrapper for the Debug domain.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
// 04/06/2016        vma           Initial version
//------------------------------------------------------------------------

#ifndef _DBG_DEFS_H_
#define _DBG_DEFS_H_

#include "hfidefs.h"

/**
   Level 2 Comment: "DBG Command Types"
   The section defines DBG specific control/data commands (HFI_CMD_DBG_X)
   DBG commands are private commands that support the DBG protocol

   All the responses from FW are generated asynchronously. The
   responses are generates as messages and are defined as HFI_MSG_DBG_X
   */

/**
This command sends a RegDump to the Firmware.

@datatypes
HFI_CMD_PRI_DBG_REG_DUMP_TYPE

@sa
HFI_EVENT_X
*/
#define  HFI_CMD_PRI_DBG_REG_DUMP                 (HFI_CMD_DBG_COMMON_START + 0x8)

/**
This command sends a trace buffer to the Firmware for profiling use.

@datatypes
HFI_CMD_PRI_DBG_TRACE_BUFFER_TYPE

@sa
HFI_EVENT_X
*/
#define  HFI_CMD_PRI_DBG_TRACE_BUFFER             (HFI_CMD_DBG_COMMON_START + 0x9)

/**
   Level 2 Comment: "Message Types"
   The section defines common control/data messages sent
   by FW in response to a submitted DBG command

   All the responses from FW are generated asynchronously.
   */

/**
   The DBG RegDump command packet definition

   @sa
   HFI_CMD_PRI_DBG_REG_DUMP
   */
typedef struct HFI_CMD_PRI_DBG_REG_DUMP_TYPE
{
    uint32_t nSize;             /**< The packet size, in bytes. */
    uint32_t ePacketType;       /**< Payload type HFI_CMD_PRI_DBG_REG_DUMP_TYPE. */
    uint32_t nRegDumpHolderPtr; /**< address of DBG RegDumpHolder (set to NULL to disable reg dumping)*/
} HFI_CMD_PRI_DBG_REG_DUMP_TYPE;

/**
The DBG TraceBuf command packet definition

@sa
HFI_CMD_PRI_DBG_TRACE_BUFFER
*/
typedef struct HFI_CMD_PRI_DBG_TRACE_BUFFER_TYPE
{
    uint32_t nSize;             /**< The packet size, in bytes. */
    uint32_t ePacketType;       /**< Payload type HFI_CMD_PRI_DBG_TRACE_BUFFER_TYPE. */
    uint32_t nTraceBufPtr;      /**< address of DBG trace buffer */
} HFI_CMD_PRI_DBG_TRACE_BUFFER_TYPE;

#endif // _DBG_DEFS_H_

#ifndef QUSB_COMMON_H
#define QUSB_COMMON_H
//============================================================================
/**
 * @file        qusb_common.h
 * @author      shreyasr
 * @date        23-April-2014
 *
 * @brief       Common QUSB defines.
 *
 *
 *              Copyright (c) 2014 QUALCOMM Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
 */
//============================================================================

// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when          who        what, where, why
// ----------   --------    --------------------------------------------------
// 2012-04-23   shreyasr    First Draft
// 
// ===========================================================================
#include "comdef.h"

#ifdef DDR_DEBUG_MODE_ENABLED
  #define __QUSB_DMA_ZI__  // Allow USB ZI to be placed differently or DDR debug tool
#else
  #define __QUSB_DMA_ZI__ __attribute__((section(".bss.BOOT_UNCACHED_ZI_ZONE")))
#endif

#define QUSB_MAX_CTRL_PKT_SIZE 512

//SBL driver needs to make a cognitive decision about the maximum speed for USB
//SBL can operate a maximum in High Speed Mode or in Super Speed. 
//If they need to operate in HS, they need to pass this in qusb_al_bulk_init()
typedef enum
{
  QUSB_MAX_SPEED_HIGH = 0x0, 
  QUSB_MAX_SPEED_SUPER
}qusb_max_speed_required_t;

typedef enum
{
  QUSB_COMPLETE_SHUTDOWN_MODE = 0x0,
  QUSB_SINGLE_ENUM_MODE,
} qusb_shutdown_mode_t;

#endif /*QUSB_COMMON_H*/

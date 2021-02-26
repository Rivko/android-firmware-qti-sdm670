#ifndef __PM_FG_SRAM_H__
#define __PM_FG_SRAM_H__

 /*! @file pm_fg_sram.h
 *
 * PMIC Battery Profile SRAM/OTP access functionalities 
 * FG OTP Starts at address 0x0.
 * 256x24 (3 banks of 256x8) byte addressable within a 32 bit word.
 * MSByte of each 32 bit word is invalid and will readback 0x00 and will not be programmed.
 * 
 * FG RAM Starts at address 0x400.
 * 128x32 byte addressable.
 *
 * Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*=============================================================================
                              EDIT HISTORY


 when            who     what, where, why
 --------------------------------------------------------------------------------------
 08/22/17   czq     Detect the bad conditions of FG SRAM (trim registers) and issue VDD hard reset on PM/PMi to recover
 07/10/17   pxm     Add function PmicFgSram_ImaWriteSingle
 09/22/14   aab     Updated to support FG SRAM configuration in SBL
 06/06/14   va      New file.
=============================================================================*/

/**
  PMIC Platform interface
 */

#include "com_dtypes.h"         /* Containse basic type definitions */
#include "pm_err_flags.h"     /* Containse the error definitions */
#include "pm_resources_and_types.h"



/*===========================================================================
                               MACROS
===========================================================================*/
#define FG_SRAM_START_ADDR     0x00
#define FG_SRAM_END_ADDR       0x80

#define FG_SRAM_RD_WR_BUS_WIDTH         4
#define FG_SRAM_RD_WR_OFFSET_WIDTH      3


/*=========================================================================
                            GLOBAL VARIABLES
===========================================================================*/
/**
  Sram State
*/
typedef enum _FgSramState_type{
  FG_SRAM_STATUS_INIT,
  FG_SRAM_STATUS_LOAD_PROFILE,
  FG_SRAM_STATUS_IN_USE,
  FG_SRAM_STATUS_POLLING,
  FG_SRAM_STATUS_AVAILABLE,
  FG_SRAM_STATUS_INVALID
}FgSramState_type;



typedef enum{
  FG_MEMORY_ACCESS_SLCT_DMA,
  FG_MEMORY_ACCESS_SLCT_IMA,
  FG_MEMORY_ACCESS_SLCT_TYPE_INVALID
}pm_fg_mem_if_memory_access_slct_type;


typedef enum{
  FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_SINGLE,
  FG_MEMORY_ACCESS_INTF_CTL_MEM_ACS_BURST,
  FG_MEMORY_ACCESS_INTF_CTL_BURST_INVALID
}pm_fg_mem_if_memory_access_intf_ctl_burst;



typedef enum{
  FG_MEMORY_ACCESS_INTF_CTL_READ_ACCESS,
  FG_MEMORY_ACCESS_INTF_CTL_WRITE_ACCESS,
  FG_MEMORY_ACCESS_INTF_CTL_INVALID
}pm_fg_mem_if_memory_access_intf_ctl_ima_wr_en;


typedef enum{
  FG_MEMORY_ACCESS_REQ_FALSE,
  FG_MEMORY_ACCESS_REQ_TRUE,
  FG_MEMORY_ACCESS_REQ_TYPE_INVALID
}pm_fg_mem_if_memory_access_req_type;






/*===========================================================================
                               FUNCTION DEFINITIONS
===========================================================================*/

pm_err_flag_type PmicFgSram_Init(uint32 PmicDeviceIndex);

pm_err_flag_type PmicFgSram_ImaRead(uint32 PmicDeviceIndex, uint16 ReadAddress, uint32 *fg_memif_data);

pm_err_flag_type PmicFgSram_ImaWriteSingle(uint32 PmicDeviceIndex, uint16 WriteAddress, uint32 fg_memif_data);

pm_err_flag_type PmicFgSram_ReadIntegrityBit(uint32 PmicDeviceIndex, boolean *bIntergrityBit);

 pm_err_flag_type pm_fg_sram_verify_content(uint32 pmic_index, boolean *unexpected_match);


#endif //__PM_FG_SRAM_H__



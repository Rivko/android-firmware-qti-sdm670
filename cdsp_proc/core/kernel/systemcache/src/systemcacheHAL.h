#ifndef SYSTEMCACHEHAL_H
#define SYSTEMCACHEHAL_H
/*=================================================================================================

                      S Y S T E M    C A C H E    INTERNAL    H E A D E R    F I L E

DESCRIPTION
   APIs for the System Cache Driver


Copyright (c) 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
=================================================================================================*/

/*=================================================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/kernel/systemcache/src/systemcacheHAL.h#1 $

=================================================================================================*/

/*=================================================================================================

                           INCLUDE FILES

=================================================================================================*/
#include "comdef.h"
#include "qurt_memory.h"

/*=================================================================================================

                            Enumeration DECLARATIONS

=================================================================================================*/

/** 
Definitions of system cache Register Offets
*/
#define LLCC_BROADCAST_LLCC_TRP_SCID_n_ACT_CTRL_OFFSET             0x200000
#define LLCC_BROADCAST_LLCC_TRP_SCID_n_ACT_CTRL_INCREMENT          0x1000
#define LLCC_ACTIVATION_VALUE                                      0x2
#define LLCC_DEACTIVATION_VALUE                                    0x4
#define LLCC_TRIGGER_VALUE                                         0x1


#define LLCC_i_LLCC_TRP_INCREMENT                                  0x80000

#define LLCC_i_LLCC_TRP_SCID_n_STATUS_OFFSET                       0x4
#define LLCC_i_LLCC_TRP_SCID_n_STATUS_INCREMENT                    0x1000
#define LLCC_CURR_CAP_MASK                                         0x3FFF0000
#define LLCC_CURR_CAP_SHIFT                                        16
#define LLCC_CHECK_ACTIVE_MASK                                     1
#define LLCC_CHECK_DEACTIVE_MASK                                   2

#define LLCC_i_LLCC_TRP_SCT_n_ATTRIBUTE1_CFG_OFFSET                0x021004
#define LLCC_i_LLCC_TRP_SCT_n_ATTRIBUTE1_CFG_INCREMENT             0x8
#define LLCC_MAX_CAP_MASK                                          0x3FFF0000
#define LLCC_MAX_CAP_SHIFT                                         16

                                            
/*==================================================================================================

                     FUNCTION DECLARATIONS

==================================================================================================*/
#ifdef __cplusplus
   extern "C"
   {
#endif

/*=================================================================================================
         SYSTEM CACHE HAL APIs
=================================================================================================*/

uint32 SysCacheHALGetSCIDMaxAlloc(uint32 SysCacheBaseAddress, uint32 scid_val);

uint32 SysCacheHALGetSCIDCurrAlloc(uint32 SysCacheBaseAddress, uint32 scid_val);

void   SysCacheHALActDeactSCID(uint32 SysCacheBaseAddress, uint32 scid, boolean activate);

uint32 SystemCacheHALGetNumLLCCBanks();

#ifdef __cplusplus
   }
#endif

#endif /* END SYSTEMCACHEHAL_H */

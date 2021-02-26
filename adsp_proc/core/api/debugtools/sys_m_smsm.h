#ifndef SYS_M_SMSM_H
#define SYS_M_SMSM_H
/*===========================================================================

           S Y S _ M _ S M S M. H

DESCRIPTION

Copyright (c) 2011-16 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/api/debugtools/sys_m_smsm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/
#include "comdef.h"

#define SYS_M_SMP2P_SUCCESS         0
#define SYS_M_SMP2P_NOT_READY       1
#define SYS_M_SMP2P_INVALID_PARAM   2

#define SYS_M_AP2SUBSYS_SMSM_ERRFATAL       0x1
#define SYS_M_AP2SUBSYS_SMSM_SHUT_DWN       0x2

// Force 32 bits enum
typedef enum
{
   SYS_M_SUBSYS2AP_SMSM_ERRFATAL = 0x00000001,
   SYS_M_SUBSYS2AP_SMSM_ERR_HDL_RDY = 0x00000002,
   SYS_M_SUBSYS2AP_SMSM_PWR_CLK_RDY = 0x00000004,
   SYS_M_SUBSYS2AP_SMSM_SHUT_DWN_ACK = 0x00000008,
   SYS_M_SUBSYS2AP_SMSM_SHUT_DWN = 0x00000010,
   SYS_M_SUBSYS2AP_SMSM_PWR_OFF = 0x00000020,
   SYS_M_SUBSYS2AP_SMSM_ERR_INITIATED = 0x00000040,
   SYS_M_SUBSYS2AP_QMI_REQ_ACK = 0x00000080,
   SYS_M_SUBSYS2AP_SMSM_RESERVED = 0x8000, /*Do not use it */
   SYS_M_SUBSYS2AP_FORCE_32BIT_ENUM = 0xFFFFFFFF    // Do not use, this is here only to force the enum to 32 bits
} SYS_M_SUBSYS2AP_SMSM_BITS;

/**
  Sets bit in sys_m point to point array to apps

  @param[in] bit       Bit to set
  
  @return
  SYS_M_SMP2P_SUCCESS – successful entry of the bit
  SYS_M_SMP2P_NOT_READY – SYS_M entry is not yet ready
  SYS_M_SMP2P_INVALID_PARAM – SYS_M transport detects invalid parameter

  
  @dependencies
  None.
*/
int32 sys_m_smsm_apps_set(uint32 bit);

/** 
  Single threaded mode version of above (no mutex operations) 
  Sets bit in sys_m point to point array to apps 
                        
  @return
  Error codes
  
  @dependencies
  None.
*/
int32 sys_m_smsm_apps_set_stm(uint32 bits);

/**
  Sets bit in sys_m point to point array to apps
  If sys_m_smsm_tx_context is not ready, then set sys_m_smp2p_cached_init_state flag

  @param[in] bit       Bit to set
  
  @return
   SYS_M_SMP2P_SUCCESS : successfully set or cached the bit

  @dependencies
  None.
*/
int32 sys_m_smsm_apps_set_cached(SYS_M_SUBSYS2AP_SMSM_BITS bits);

#endif  /* SYS_M_SMSM_H */

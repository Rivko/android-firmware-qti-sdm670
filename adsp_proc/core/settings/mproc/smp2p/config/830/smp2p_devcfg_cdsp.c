/*===========================================================================

                     SMP2P DEVICE CONFIGURATION

        This file provides the device configuration for SMP2P

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     Established settings interface
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/

#include "DDIIPCInt.h"
#include "smem_type.h"
#include "smp2p_settings.h"

/*===========================================================================
                    CONSTANT / MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                      GLOBAL DATA DEFINATION
===========================================================================*/
/** Local processor info */
const smp2p_proc_info_type smp2p_devcfg_proc_info = 
{ 
  /** This host name */
  "cdsp",
  
  /** This host SMEM ID */
  SMEM_CDSP,
  
  /** Feature flags supported */
  SMP2P_FFLAGS_SSR_HANDSHAKE,
  
  /** Max number of entries */
  16
};

/** Interrupt info for connected subsystems */
const smp2p_interrupt_info_type smp2p_devcfg_interrupt_info[] = 
{
  /** APPS */
  {
    /* destination */
    SMEM_APPS,
    
    /* destination host type */
    DALIPCINT_PROC_ACPU,
    
    /* outgoing interrupt register */
    DALIPCINT_GP_2,
    
    /* interrupt bits in irq_out register */
    0,
    
    /* incoming interrupt register */
    34,
  },
  /* TODO Enable if CDSP<->Modem is required */
#if 0
  /** MODEM */
  {
    /* destination */
    SMEM_MODEM,
    
    /* destination host type */
    DALIPCINT_PROC_MDSPSW,
    
    /* outgoing interrupt register */
    DALIPCINT_GP_2,
    
    /* interrupt bits in irq_out register */
    0,
    
    /* incoming interrupt register */
    48,
  },
#endif

  /* TODO Enable if CDSP<->SSC is required */
#if 0
  /** SSC */
  {
    /* destination */
    SMEM_SSC,
    
    /* destination host type */
    DALIPCINT_PROC_SPS,
    
    /* outgoing interrupt register */
    DALIPCINT_GP_2,
    
    /* interrupt bits in irq_out register */
    0,
    
    /* incoming interrupt register */
    145,
  },
#endif

  /* TODO Enable if CDSP<->ADSP is required */
#if 0
  /** SSC */
  {
    /* destination */
    SMEM_ADSP,
    
    /* destination host type */
    DALIPCINT_PROC_ADSP,
    
    /* outgoing interrupt register */
    DALIPCINT_GP_2,
    
    /* interrupt bits in irq_out register */
    0,
    
    /* incoming interrupt register */
    121,
  },
#endif
  /** END */
  {
    /* destination */
    SMEM_INVALID_HOST,
    
    /* destination host type */
    DALIPCINT_PROC_NULL,
    
    /* outgoing interrupt register */
    DALIPCINT_INT_32BITS,
    
    /* interrupt bits in irq_out register */
    0,
    
    /* incoming interrupt register */
    0,
  },
};

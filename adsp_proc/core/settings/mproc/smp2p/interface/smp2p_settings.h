/*------------------------------------------------------------------------------
   SMP2P_SETTINGS.H  - SMP2P Settings Interface
------------------------------------------------------------------------------*/
/*!
  @file
    smp2p_settings.h

  @brief
    This file contains the SMP2P settings interface structure definitions.
*/

/*------------------------------------------------------------------------------
     Copyright  2017 Qualcomm Technologies Incorporated.
     All rights reserved.
------------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/settings/mproc/smp2p/interface/smp2p_settings.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     Initial version; created settings interface for SMP2P
===========================================================================*/
#ifndef SMP2P_SETTINGS_H_
#define SMP2P_SETTINGS_H_

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_type.h"
#include "DDIIPCInt.h"

/*===========================================================================
                        DEFINITIONS
===========================================================================*/
/* Feature flags (smp2p_header_version_type.flags, 24 bits) */
#define SMP2P_FFLAGS_SSR_HANDSHAKE      0x000001

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
/** Defines the structure with information about this host */
typedef struct {
  /** This host name */
  const char     *name;
  /** This host SMEM ID */
  smem_host_type host;
  /** Feature flags supported */
  uint32         fflags;
  /** Max number of entries */
  uint16         max_entries;
} smp2p_proc_info_type;

/** Defines the structure with information on how to map, send and receive
 *  interrupts. */
typedef struct
{
  smem_host_type          dest;
  /** destination host type */
  DalIPCIntProcessorType  processor;
  /** outgoing interrupt register */
  DalIPCIntInterruptType  irq_out;
  /** interrupt bits in irq_out register */
  uint32                  irq_out_mask;
  /** incoming interrupt register */
  uint32                  irq_in;
} smp2p_interrupt_info_type;

#endif /* SMP2P_SETTINGS_H_ */

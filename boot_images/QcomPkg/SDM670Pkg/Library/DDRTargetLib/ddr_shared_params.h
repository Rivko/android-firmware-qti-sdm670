#ifndef __DDR_SHARED_PARAMS_H__
#define __DDR_SHARED_PARAMS_H__

/*=============================================================================
                              DDR HAL
                            Header File
GENERAL DESCRIPTION
This is the target header file for shared DDR params such as those shared with clock driver and AOP.

Copyright 2014-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/24/17   rp      Initial revision.
=============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"

typedef struct
{
  uint16 size;
  uint16 offset;
}shared_table_header_type;

typedef enum
{
  MC_STATES = 0,
  SHUB_STATES,
  DEFAULT_ALC_POLICY,
  ALC_TIMINGS,
  MAX_NUM_TABLES,
}smem_tables;

typedef struct
{
  uint16 major_version;
  uint16 minor_version;
  shared_table_header_type shared_table_header[MAX_NUM_TABLES];
}ddr_smem_info;

/*==============================================================================
                                  MACROS
==============================================================================*/
#define AOP_PARAMS_MAJOR_VERSION 0x1
#define AOP_PARAMS_MINOR_VERSION 0x0

/* With the new SMEM architecture, SMEM IDs need to be defined in individual driver files */
#define DDR_AOP_SMEM_ID 0x25C   //604

#endif /* __DDR_SHARED_PARAMS_H__ */

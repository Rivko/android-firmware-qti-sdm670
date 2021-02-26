/*=============================================================================
                        SYSDBG MAIN Header File 

GENERAL DESCRIPTION     
  This module defines functions for intializing SYSDBG functionalities.
  
 Copyright 2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who          what, where, why
--------   ---         --------------------------------------------------
09/14/2014  amcheriy   Refactored for HoneyBadger
01/14/2014 	davidai	   Ported SDI to Bear

===========================================================================*/


#ifndef _SYSDBG_MAIN_H
#define _SYSDBG_MAIN_H

#include "comdef.h"
#include "sysdbg.h"

#define CORE(x)     (1 << x)
#define CORE_NONE   (0)
#define CORE_ALL    (0xFFFFFFFFFFFFFFFF)

/* Sysdbg Function Table */
typedef boolean (*sysdbg_fcn_type)(uint32);
typedef struct
{
	uint64 cpu_affinity[SUBSYS_MAX];
	sysdbg_fcn_type sysdbg_fcn;
}sysdbg_info_type;
	
typedef struct
{
	sysdbg_info_type fcn_info[16];
}sysdbg_pass_sequence_type;

typedef struct
{
	sysdbg_pass_sequence_type pass_sequence;
}sysdbg_pass_data_type;

/* Memory Configuration structs */
typedef enum
{
    SHARED_IMEM_REGION1,
    SHARED_IMEM_REGION2,
    SHARED_IMEM_REGION3,
    SHARED_IMEM_REGION4,
    DUMP_TABLE_REGION,
    DEBUG_TABLE_REGION,
    MAX_REGIONS
}mem_region_type;


typedef struct
{
    uint64 base;
    uint32 offset;
    uint32 size;
    boolean zinit;
}mem_slice_type;

typedef struct
{
    mem_slice_type slices[8];
}mem_config_type;




#endif /* _SYSDBG_MAIN_H */
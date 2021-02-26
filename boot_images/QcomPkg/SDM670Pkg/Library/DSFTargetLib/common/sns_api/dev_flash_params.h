#ifndef __DEV_FLASH_PARAMS_H__
#define __DEV_FLASH_PARAMS_H__

/*=============================================================================

                                DDR device Flash PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the DDR parameters stored in DDR Flash partition

Copyright (c) 2014-2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/01/13   dp      Initial version.
==============================================================================*/
/*==========================================================================
                               INCLUDE FILES
===========================================================================*/
#include "HALcomdef.h"
#include "ddrss_training.h"

/*==============================================================================
                                  TYPES & DEFINES
==============================================================================*/

#define TRAINING_DATA_SIZE_IN_BYTES sizeof(struct training_results)

/******************************************************/
/*** DDR Parameter partition datastructure ***/
/******************************************************/

struct ddr_params_partition
{
  uint32 magic_number;
  uint32 version;
  uint32 training_data_size;
  uint32 training_log_size;
  uint32 checksum;
  /* PHY code is allowed to restructure needed fields within the training_data array, as required */
  uint8 training_data[TRAINING_DATA_SIZE_IN_BYTES];  
};
#endif /* __DEV_FLASH_PARAMS_H__ */

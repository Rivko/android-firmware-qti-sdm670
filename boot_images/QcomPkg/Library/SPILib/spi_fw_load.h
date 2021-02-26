/*==================================================================================================

FILE: spi_fw_load.h

DESCRIPTION: This module provides the firmware image for I2C Core.

Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   spi_fw_load

==================================================================================================*/
/*==================================================================================================
Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
01/07/19   bng     Initial revision

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "spi_config.h"
#include "SpiApi.h"

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
spi_status spi_fw_load(spi_plat_device_config *dcfg);

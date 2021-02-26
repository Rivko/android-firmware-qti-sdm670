/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/stub_src/lpi_gpio/afe_lpi_gpio_driver_stub.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     afe_lpi_gpio_driver_stub.cpp

DESCRIPTION: Stub for LPI GPIO configuration functions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI)
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/stub_src/lpi_gpio/afe_lpi_gpio_driver_stub.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/06/17    sudhir      Created file

============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/

#include "adsp_error_codes.h"
#include  "AFEDevCommon.h"


ADSPResult afe_device_lpi_gpio_init(gpio_int_properties_t *gpio_int_prop_ptr)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_device_lpi_gpio_deinit(gpio_int_properties_t *gpio_int_prop_ptr)
{
   return ADSP_EUNSUPPORTED;
}


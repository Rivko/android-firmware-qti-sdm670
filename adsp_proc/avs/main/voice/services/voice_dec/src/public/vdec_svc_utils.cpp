
/*========================================================================
vdec_svc_utils.cpp

This file contains utility function definition for voice decoder service.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/08/16   HB      Created file.

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "vdec_private.h"


ADSPResult vdec_beamr_set_param (vdec_t* vdec_ptr, int8_t *cal_payload, uint32_t pid, uint16 param_size)
{
    return ADSP_EOK;
}

ADSPResult vdec_beamr_get_param (vdec_t* vdec_ptr, int8_t *cal_payload, uint32_t pid, uint16 buffer_size, uint16_t *param_size)
{
    *param_size = 0;
    return ADSP_EOK;
}


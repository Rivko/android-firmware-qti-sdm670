/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifndef RBCPR_H
#define RBCPR_H

#include <stdbool.h>
#include <comdef.h>
#include "HAL_cpr.h"
#include "cpr_fuses.h"
#include "cpr_enablement.h"
#include "cpr_target_quotients.h"
#include "cpr_closed_loop.h"
#include "railway.h"

typedef struct cpr_image_closed_loop_rail_s
{
    HAL_cpr_dump        hal_core_dump;
    int8                corner_lookup_index[RAILWAY_CORNERS_COUNT];
} cpr_image_closed_loop_rail_s;

boolean rbcpr_internalize_state(void);

cpr_corner_params_t* cpr_corner_params(cpr_closed_loop_rail_t rail, railway_corner corner);
uint32 rbcpr_corner_index(cpr_closed_loop_rail_t rail, railway_corner corner);


#endif /* RBCPR_H */

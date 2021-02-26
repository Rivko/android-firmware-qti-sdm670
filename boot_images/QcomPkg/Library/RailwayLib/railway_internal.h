/*===========================================================================

  railway_internal.h - bits of state for railway, in a convenient global
                       wrapper for debug

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RAILWAY_INTERNAL_H
#define RAILWAY_INTERNAL_H

#include "railway.h"
#include "boot_error_if.h"

#define RAILWAY_MAX_CBS 6

typedef struct
{
    uint8          corners_count;
    railway_corner corners[RAILWAY_CORNERS_COUNT];
} railway_corners_t;

typedef enum
{
    RAILWAY_INTERNAL_PRECHANGE_CB = 0,
    RAILWAY_INTERNAL_POSTCHANGE_CB,
    RAILWAY_INTERNAL_NUM_CB_TYPES,
} railway_internal_cb_type;

typedef struct
{
    railway_cb      callback_fn;
    void*           callback_cookie;
} railway_cb_data;

typedef struct
{
    railway_settings    current_active;

    railway_corner      unconstrained_target;  //Should always be updated when the votes change, but isn't currently
    railway_settings    constrained_target;    //Re-calculated each time we transition rails.

    uint32              retention_voltage_uv;

    // Storage for callbacks.
    railway_cb_data     cbs[RAILWAY_INTERNAL_NUM_CB_TYPES][RAILWAY_MAX_CBS];

    railway_voter_t     voter_list_head;
} railway_rail_state_t;

typedef struct
{
    railway_rail_state_t *rail_state;
} railway_internals_t;


extern railway_internals_t railway;
extern int mx_rail_id;
extern int cx_rail_id;
extern int ebi_rail_id;
extern int wlan_rail_id;


//Returns the target voltage and corner for the named rail.
//rail is not currently used - all votes are for VddCx.
//If for_sleep is true then this will ignore any suppressible votes - this option would
//only be used for sleep to know if vdd_min is possible.
railway_corner railway_aggregated_voltage_target(int rail);

void railway_init(void);
void railway_transitioner_init(void);

//Any early target-specific init should be done in this function.
void railway_target_init(void);

//Get supported corners from CPR in addition to RETENTION and NO_REQUEST corners.
railway_corners_t* railway_get_supported_corners(int rail);

#endif // RAILWAY_INTERNAL_H


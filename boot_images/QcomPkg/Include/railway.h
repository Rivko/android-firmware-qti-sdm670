// railway.h - interface description of 'railway' driver
//
// Copyright 2011 - 2012 by Qualcomm Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header$
// $ Author$
// $ DateTime$


#ifndef RAILWAY_H
#define RAILWAY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "DALStdDef.h"

//Forward Declerations
typedef struct railway_voter_s* railway_voter_t;

#define RAIL_NOT_SUPPORTED_BY_RAILWAY -1

typedef enum
{
  RAILWAY_NO_REQUEST         = 0,
  RAILWAY_RETENTION          = 1,
  RAILWAY_SVS_MIN            = 2,
  RAILWAY_SVS_LOW            = 3,
  RAILWAY_SVS                = 4,
  RAILWAY_SVS_HIGH           = 5,
  RAILWAY_NOMINAL            = 6,
  RAILWAY_TURBO              = 7,
  RAILWAY_TURBO_L1           = 8,
  RAILWAY_SUPER_TURBO        = 9,
  RAILWAY_SUPER_TURBO_NO_CPR = 10,
  RAILWAY_CORNERS_COUNT,
} railway_corner;

typedef enum
{
  RAILWAY_PRECHANGE_CB = 0,
  RAILWAY_POSTCHANGE_CB,
  RAILWAY_LAST_PRECHANGE_CB,
  RAILWAY_FIRST_POSTCHANGE_CB,
  RAILWAY_NUM_CB_TYPES,
} railway_cb_type;

typedef enum
{
    RAILWAY_SVS_VOTER_ID = 100,
    RAILWAY_RPM_CX_VOTER_ID,   //The RPM itself makes a supressible vote for SVS on Cx.
    RAILWAY_RPM_MX_VOTER_ID,   //The RPM itself makes a supressible vote for SVS on Mx.
    RAILWAY_DDR_TRAINING_VOTER_ID,
    RAILWAY_RPM_BRINGUP_VOTER, //Voter to pin things for bringup.
    RAILWAY_RPM_INIT_VOTER, //Voter to pin Mx to super turbo.
    RAILWAY_CLOCK_DRIVER_VOTER_ID,
    RAILWAY_CPR_SETTLING_VOTER,
    RAILWAY_RPM_CX_SLEEP_VOTER_ID,
    RAILWAY_RPM_MX_SLEEP_VOTER_ID,
    RAILWAY_RPM_EBI_VOTER_ID,
    RAILWAY_RPM_EBI_SLEEP_VOTER_ID,
    RAILWAY_RPM_WLAN_VOTER_ID,
    RAILWAY_RPM_SSC_MX_VOTER_ID,
    RAILWAY_RPM_SSC_CX_VOTER_ID,
    RAILWAY_RPM_LPI_MX_VOTER_ID,
    RAILWAY_RPM_LPI_CX_VOTER_ID,
} railway_voter_id;

// railway_settings structure passed back to client of railway callbacks.
// mode       - the highest corner vote for this rail.
// microvolts - the voltage that the rail is set to.                
typedef struct
{
  railway_corner mode;
  uint32   microvolts;
} railway_settings;

typedef void (*railway_cb)(const railway_settings *settings, void* cookie);

/// Initialize the railway driver and all of its associated rail control.
void railway_init(void);

// Return the ID for the named rail on this target.
// The rail ID will be differnt from target to target depending on which rails railway is controlling.
// Returns RAIL_NOT_SUPPORTED_BY_RAILWAY if the rail isn't controlled by railway.
int rail_id(const char* rail);

// Sets a callback on a particular event (pre/post change) for a particular voltage rail.
// The cookie passed in will be passed back in the callback, can be NULL.
// This function asserts that there is no error and that the callback can be correctly added.
void railway_set_callback(int rail, railway_cb_type type, railway_cb cb, void* cookie);

// After changing votes, call this function to invoke a voltage change to the lowest acceptible
// level on all Railway-controlled rails.
void railway_transition_rails(void);

// Will assert that voter can be alloced - will not return NULL.
railway_voter_t railway_create_voter(int rail, int id);

// Update which corner this voter is voting for.
// Will NOT invoke the railway transitioner to transition the voltage - call railway_transition_rails
// to invoke the voltage change.
void railway_corner_vote(railway_voter_t voter, railway_corner corner);

//API used by CPR to get the value for a corner.
uint32 railway_get_corner_voltage(int rail, railway_corner corner);

//Returns a copy of the current settings of the specified rail.
void railway_get_current_settings(int rail, railway_settings* settings);

void railway_set_retention_voltage(int rail, uint32 microvolts);

void railway_synchronize_mx_and_ebi_corner_voltages(void);

void aop_mini_dump_init(void);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // RAILWAY_H


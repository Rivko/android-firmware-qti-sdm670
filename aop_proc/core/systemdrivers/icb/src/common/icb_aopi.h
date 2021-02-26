#ifndef ICB_AOPI_H
#define ICB_AOPI_H
/*
===========================================================================

FILE:         icb_rpmi.h

DESCRIPTION:  Internal header file for the ICB RPM library.

===========================================================================

                             Edit History

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/icb/src/common/icb_aopi.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2017/06/05   sds     Add corner matching framework
2016/11/08   sds     Initial revision.

===========================================================================
        Copyright (c) 2016-2017 QUALCOMM Technologies, Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include "icb_aop.h"
#include "icbid.h"
#include "pwr_utils_lvl.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define BCM_VOTETABLE_COMMIT_BMSK                               0x40000000
#define BCM_VOTETABLE_COMMIT_SHFT                                     0x1e
#define BCM_VOTETABLE_VOTE_VALID_BMSK                           0x20000000
#define BCM_VOTETABLE_VOTE_VALID_SHFT                                 0x1d
#define BCM_VOTETABLE_VOTE_X_BMSK                                0xfffc000
#define BCM_VOTETABLE_VOTE_X_SHFT                                      0xe
#define BCM_VOTETABLE_VOTE_X_MAX                                    0x3fff
#define BCM_VOTETABLE_VOTE_Y_BMSK                                   0x3fff
#define BCM_VOTETABLE_VOTE_Y_SHFT                                      0x0
#define BCM_VOTETABLE_VOTE_Y_MAX                                    0x3fff

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#define MAX_CPS 8

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
typedef struct icb_bcm_t
{
  struct icb_bcm_t *next;    /**< Link for request queue. */
  const char *      name;
  uint32_t          hw_id;
  uint32_t          clk_id;
  uint32_t          vote;
  uint32_t          ref_cnt;
} icb_bcm_t;

/* SoC data format for masters */
typedef struct icb_route_t
{
  ICBId_MasterType master;
  ICBId_SlaveType  slave;
  uint32_t         num_bcms;
  icb_bcm_t **     bcms;
} icb_route_t;

/* Internal client handle type */
typedef struct icb_client_t
{
  icb_route_t *  route;
  icb_callback_t callback;
  void *         callback_data;
} icb_client_t;

/* Completion queue type */
typedef struct icb_completion_t
{
  struct icb_completion_t *next;
  icb_client_t            *client;
  uint32_t                 req_id;
} icb_completion_t;

/* Rate matched VCD structure */
typedef struct
{
  uint32_t vcd;
  uint32_t num_hlvls;      /**< Number of populated levels */
  uint8_t  hlvls[MAX_CPS]; /**< Cached hlvls */
  uint32_t desired_cp;     /**< Desired SW_CP */
  uint32_t pending_cp;     /**< Pending SW_CP */
  bool     busy;           /**< In progress */
} icb_match_vcd_t;

/* SoC data format for system information */
typedef struct
{
  uint32_t         num_bcms;
  icb_bcm_t **     bcms;
  uint32_t         num_routes;
  icb_route_t **   routes;
  uint32_t         num_match_vcds;
  icb_match_vcd_t *match_vcds;
} icb_info_t;

/* -----------------------------------------------------------------------
** Function Declarations (to be implemented by target)
** ----------------------------------------------------------------------- */
/**
@brief Fetch the target specific data structure from device config.
*/
icb_info_t *icb_get_target_data( void );

/**
@brief Peform target specific initialization sequences and workarounds.

@param[in] info - The target info structure
*/
void icb_target_init( icb_info_t *info );

/**
@brief Peform target specific initialization sequences and workarounds in post init.
       Called after immediately after BCM hw is enabled.
*/
void icb_target_post_init( void );

/**
@brief Target specific interface for AOP services to notify ICB of any pending BCM services

@param[in] irq - The IRQ to handle

@returns True if successful, false otherwise.
*/
bool icb_target_service_ist( int irq );

#endif /* ICB_AOPI_H */

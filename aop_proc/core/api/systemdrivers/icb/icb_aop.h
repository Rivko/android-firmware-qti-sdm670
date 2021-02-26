#ifndef ICB_AOP_H
#define ICB_AOP_H
/*
===========================================================================

FILE:         icb_aop.h

DESCRIPTION:  Header file for the ICB-on-AOP library.

===========================================================================

                             Edit History

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/systemdrivers/icb/icb_aop.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2017/06/05   sds     Add corner matching framework
2016/12/22   sds     Add post init
2016/11/22   sds     Add async interface
2016/09/20   sds     Initial revision.

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
#include "icbid.h"

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
typedef struct icb_client_t * icb_handle_t;

/**
@param[in] callback_data - Client data passed as a part of client registration
@param[in] req_data      - RPMh request id associated with this callback

@note It is possible for the completion callback to come *before* the 
      request causing it has been returned to the client if the request
      completes immediately at the RPMh driver. Clients must handle this
      potential race condition.
*/
typedef void ( *icb_callback_t )( void *callback_data, uint32_t req_data );

/* -----------------------------------------------------------------------
** External interface
** ----------------------------------------------------------------------- */
/**
@brief This function initializes the ICB library.
*/
void icb_init( void );

/**
@brief This function performs any post initialization steps for the ICB library.
*/
void icb_post_init( void );

/**
@brief Get a handle for the requested route.

@param[in] master - The master endpoint
@param[in] slave  - The slave endpoint

@returns A handle to use if successful, NULL otherwise.
*/
icb_handle_t icb_get_handle( ICBId_MasterType master, ICBId_SlaveType slave );

/**
@brief Get a handle for the requested route.

@param[in] master        - The master endpoint
@param[in] slave         - The slave endpoint
@param[in] callback      - The callback function to call on action completion
                           If NULL, any actions will be performed synchronously
@param[in] callback_data - The callback function to call on action completion

@returns A handle to use if successful, NULL otherwise.

@note There are two conditions under which a completion callback will NOT occur:
      1.) A request issued with 'completion' set to false
      2.) The request returns a request id of zero.
          i.e. No change required to honor this request
*/
icb_handle_t icb_get_handle_ex( ICBId_MasterType master, 
                                ICBId_SlaveType  slave,
                                icb_callback_t   callback,
                                void *           callback_data );

/**
@brief Enable the specified route.

@param[in] handle -     The handle to enable
@param[in] completion - Is completion required?

@returns The RPMh request id for this request.
*/
uint32_t icb_enable_route( icb_handle_t handle,
                           bool         completion );

/**
@brief Disable the specified route.

@param[in] handle -     The handle to disable
@param[in] completion - Is completion required?

@returns The RPMh request id for this request.

@note The route is refcounted. If multiple enables were indicated for the same route,
      multiple disables will be required to fully disable the route.
*/
uint32_t icb_disable_route( icb_handle_t handle,
                            bool         completion );

/* -----------------------------------------------------------------------
** Notification interface
** ----------------------------------------------------------------------- */
/**
@brief Interface for DDR MGR to update BCM SW on Cx change due to MC/SHUB

@param[in] vlvl - The software voltage level of Cx
*/
void icb_update_ddr_cx_state( int vlvl );

/* -----------------------------------------------------------------------
** AOP services interface
** ----------------------------------------------------------------------- */
/**
@brief Interface for AOP services to notify ICB of any pending BCM services

@param[in] irq - The IRQ to handle

@returns True if successful, false otherwise.
*/
bool icb_handle_aop_service_req( int irq );

#endif /* ICB_AOP_H */


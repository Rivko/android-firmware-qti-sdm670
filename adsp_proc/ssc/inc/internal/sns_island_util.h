#pragma once
/*=============================================================================
  @file sns_island_util.h

  @brief
  Island control for Sensors.

  Copyright (c) 2016 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ===========================================================================*/

/*=============================================================================
  Includes
  ===========================================================================*/
#include <stdint.h>
#include "sns_osa_thread.h"
#include "sns_rc.h"

/*============================================================================
  Forward Declarations
  ===========================================================================*/

/* Start, end, and length of island memory */
extern uint32_t __sensors_island_start;
extern uint32_t __sensors_island_end;
extern uintptr_t sns_island_size;

/*=============================================================================
  TYPEDEFS
  ===========================================================================*/

/**
 * Island mode enable/disable request values
 */
typedef enum
{
  /* Island mode disable request */
  SNS_ISLAND_DISABLE = 0,
  /* Island mode enable request */
  SNS_ISLAND_ENABLE,
  /* Uninitialized state */
  SNS_ISLAND_UNKNOWN
}sns_island_vote;

/**
 * Island mode states
 */
typedef enum
{
  /* Island mode is disabled. It might be enabled later */
  SNS_ISLAND_STATE_ISLAND_DISABLED = 0,
  /* Island mode is available. */
  SNS_ISLAND_STATE_IN_ISLAND,
  /* Island mode is not available and will not be available in the future. */
  SNS_ISLAND_STATE_NOT_IN_ISLAND
} sns_island_state;

typedef void* sns_island_client_handle;

/**
 * Island Exit callback function type.
 */
typedef void (*sns_island_exit_cb)(intptr_t args);

/*=============================================================================
  Functions
  ===========================================================================*/
/*!
  @brief initializes the island manager's internal structures.

  @return
  SNS_RC_SUCCESS              init was successful
  SNS_RC_NOT_SUPPORTED        island mode not supported

  @note island mode is disabled by default at the end of sns_island_init
 */
sns_rc sns_island_init(void);

/*!
  @brief disables island mode.

  @return
  SNS_RC_SUCCESS              disable was successful
  SNS_RC_NOT_SUPPORTED        island mode not supported
*/
sns_rc sns_island_disable(void);

/*!
   @brief If Island mode can be enabled, then vote to enable it.

   @return true if island mode was enabled
           false otherwise
  */
bool sns_island_enable(void);

/*!
  @brief Configure island enable/disable based on registry setting.

  @param[i] island_enable     true if island is enabled
                              false if island is disabled
*/
void sns_island_configure_island_enable(bool island_enable);

/*!
  @brief Registers for a callback that is called every time the system exits island
    mode.

  @param[i] cb     Callback that will be called
  @param[i] args   arguments to the callback function

  @return
  SNS_RC_SUCCESS              signal was successfully registered
  SNS_RC_NOT_SUPPORTED        island mode not supported
*/
sns_rc sns_island_register_island_exit_cb(
  sns_island_exit_cb cb,
  intptr_t args);

/*!
  @brief Returns the current island mode state

  @return
  SNS_ISLAND_STATE_IN_ISLAND        When system is in island mode
  SNS_ISLAND_STATE_NOT_IN_ISLAND    When system is not in island mode
  SNS_ISLAND_STATE_ISLAND_DISABLED  When island mode is disabled
*/
sns_island_state sns_island_get_island_state(void);

/*!
  @brief Generates and commits an sns_island_state_log log packet.

  @param[i] req_id       Request ID to be used in log packet

  @return
  SNS_RC_SUCCESS              log packet operation was successful
  SNS_RC_NOT_SUPPORTED        island mode not supported
*/
sns_rc sns_island_generate_and_commit_state_log(uint64_t req_id);

/*!
  @brief Checks if a pointer can be safely dereferenced when in island mode

  @param[i] ptr        Pointer to be checked

  @return
  true                 Pointer can be safely dereferenced when in island mode
  false                Otherwise
*/

static inline bool sns_island_is_island_ptr(intptr_t ptr)
{
#ifdef SNS_NO_ISLAND_PTR_CHECKS
  return false;
#else
  return ((uintptr_t)ptr - (uintptr_t)&__sensors_island_start <= sns_island_size) ||
         ((intptr_t)NULL == ptr);
#endif
}

/*!
  @brief Register a client for the island vote aggregator

  @param[i] client_name     Name of the client that is registering

  @return
  client_handle             Client handle used to block or unblock
                            island mode
*/
sns_island_client_handle sns_island_aggregator_register_client(const char * client_name);

/*!
  @brief Deregister a client from the island vote aggregator

  @param[i] client_handle   Client handle to be deregistered

  */
void sns_island_aggregator_deregister_client(sns_island_client_handle client_handle);

/*!
  @brief Exit island mode and block re-entry to island mode

  @param[i] client_handle client_handle returned by sns_island_aggregator_register_client

  @return
  SNS_RC_SUCCESS        Operation successful
  SNS_RC_INVALID_TYPE   client_handle parameter is bad
  SNS_RC_NOT_SUPPORTED  Island mode is not supported
*/
sns_rc sns_island_block(sns_island_client_handle client_handle);

/*!
  @brief Unblock re-entry to island mode

  @param[i] client_handle client_handle returned by sns_island_aggregator_register_client

  @return
  SNS_RC_SUCCESS        Operation successful
  SNS_RC_INVALID_TYPE   client_handle parameter is bad
  SNS_RC_NOT_SUPPORTED  Island mode is not supported
*/
sns_rc sns_island_unblock(sns_island_client_handle client_handle);

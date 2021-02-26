#ifndef QDSS_CONTROL_H
#define QDSS_CONTROL_H

/*=============================================================================

FILE:         qdss_control.h

DESCRIPTION:  QDSS control API for debug agent

================================================================================
            Copyright (c) 2013 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "DALStdDef.h"

// Return error codes (non-App images)
#define QDSS_CONTROL_RVAL_SUCCESS         0
#define QDSS_CONTROL_RVAL_UNKNOWN_ERR     1
#define QDSS_CONTROL_RVAL_NOT_SUPPORTED   2
#define QDSS_CONTROL_RVAL_MALFORMED       3
#define QDSS_CONTROL_RVAL_INVALID_ARG     4
#define QDSS_CONTROL_RVAL_MISSING_ARG     5
#define QDSS_CONTROL_RVAL_ACCESS_DENIED   6



// Return error codes (App images)
#define QDSS_CONTROL_SUCCESS          0
#define QDSS_CONTROL_ERROR            1   // General error, failed.
#define QDSS_CONTROL_BAD_STATE        2   // Needed device handle is invalid.
#define QDSS_CONTROL_UNSUPPORTED      3   // command not supported.
#define QDSS_CONTROL_BAD_PARAM        4   // Bad "qdss_control_...()" parameter.
#define QDSS_CONTROL_FUSE_BLOWN       5   // QDSS fuse is blown.
#define QDSS_CONTROL_DEVICE_ERROR     6   // DAL, device, SW driver error,
#define QDSS_CONTROL_FORMAT_ERROR     7   // Request format in error.


#define QDSS_TRACESINK_CBUF    0
#define QDSS_TRACESINK_HWFIFO  2
#define QDSS_TRACESINK_AUTO    3

/*=========================================================================
  Trace Sink
 =========================================================================*/

/**
  @brief  Get the current trace sink

  @param [out] sinkid : Current sink id is returned here
                        See DDITMC.h for supported sinkids

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_sink(uint8 *sinkid);

/*-------------------------------------------------------------------------*/

/**
  @brief Set the trace sink

  @param [in ] sinkid : New trace sink
                        See DDITMC.h for supported sinkids

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_sink(uint8 sinkid);

/*-------------------------------------------------------------------------*/

/**
  @brief  Return a list of supported trace sink ids

  @param [out] sinkid_list : Pointer to a buffer where supported
                            sinkids will be listed
  @param [in ] list_size   : Size of the input buffer
  @param [out] filled      : Number of trace sink ids returned

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_sink_list(uint8 *sinkid_list, int list_size, int *filled);

/*=========================================================================
  ETM
 =========================================================================*/

/**
  @brief Get the current state of ETM on this core

  @param [out] state : ETM state 0 (disabled) or 1 (enabled)

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_etm(uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief  Enable or disable ETM on this CPU

  @param [in ] state : 1 (enable), 0 (disable)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_etm(uint8 state);

/*-------------------------------------------------------------------------*/

/**
  @brief Get the current state of ETM on RPM

  @param [out] state : ETM state: 0 (disabled) or 1 (enabled)

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_etm_rpm(uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief  Enable or disable ETM on RPM

  @param [in ] state : 1 (enable), 0 (disable)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_etm_rpm(uint8 state);

/*========================================================================
  Funnel
=========================================================================*/

/**
  @brief Get the state of a funnel

  @param [in ] name  : Trace funnel name
                       See qdss_tfunnel.h for supported names
  @param [out] state : Current state of the funnel is returned here

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_funnel(const char *name, int *state);

/*-------------------------------------------------------------------------*/

/**
  @brief  Set the state of a funnel

  @param [in ] name  : Trace funnel name
  @param [in ] state : Enable (1) or disable (0)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_funnel(const char *name, int state);

/*=========================================================================
  STM
 =========================================================================*/

/**
  @brief   Get the state of STM

  @param [in ] state : STM state is returned here

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_stm(uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief Set STM state

  @param [in ] state : 1 (enable) or 0 (disable)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_stm(uint8 state);

/*=========================================================================
  HWE
 =========================================================================*/

/**
  @brief Get the state of HW events

  @param [in ] state : HW events state is returned here.
                       1 (HW events enabled), 0 HW events disabled

  @return 0 if successful, error code otherwise
 */
int qdss_control_get_hwe(uint8 *state);

/*-------------------------------------------------------------------------*/

/**
  @brief Set the state of HW events

  @param [in ] state : 1 (enable) or 0 (disable)

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_hwe(uint8 state);

/*-------------------------------------------------------------------------*/

/**
  @brief Set a HW event register

  @param [in ] addr : address of register to set
  @param [in ] val  : value of register to set

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_hwe_reg(unsigned long addr, unsigned long val);

/*-------------------------------------------------------------------------*/
/**
  @brief Set mask for HW event detection

  @param [in ] mask : 32-bit mask to be set

  @return 0 if successful, error code otherwise
 */
int qdss_control_set_hwe_detect_mask(unsigned long mask);

/*-------------------------------------------------------------------------*/

/**
  @brief Set mask for HW event trigger

  @param [in ] mask : 32-bit mask to be set

  @return 0 if successful, error code otherwise
 */

int qdss_control_set_hwe_trigger_mask(unsigned long mask);

/*=========================================================================
  CTI
 =========================================================================*/

/**
  @brief Map a trigger to a channel.

  @param  [in] block   : CTI block name. Must be null terminated.
  @param  [in] type    : Indicates TRIGIN or TRIGOUT.
  @param  [in] trigger : Trigger number.
  @param  [in] channel : Channel number.

  @return 0 if successful, error code otherwise.
 */

int qdss_control_map_cti(const char *block,
                         uint8 type,
                         uint8 trigger,
                         uint8 channel);

/*-------------------------------------------------------------------------*/

/**
  @brief Unmap a trigger to a channel.

  @param [in] block   : CTI block name. Must be null terminated.
  @param [in] type    : Indicates TRIGIN or TRIGOUT.
  @param [in] trigger : Trigger number.
  @param [in] channel : Channel number.

  @return 0 if successful, error code otherwise.
 */

int qdss_control_unmap_cti(const char *block,
                           uint8 type,
                           uint8 trigger,
                           uint8 channel);

/*-------------------------------------------------------------------------*/

/**
  @brief Resets all mappings for the given block.

  @param [in] block : CTI block name. Must be null terminated.

  @return 0 if successful, error code otherwise.
 */

int qdss_control_reset_cti(const char *block);

/*-------------------------------------------------------------------------*/

/**
  @brief Query all mappings for the given block.

  @param [in ] block       : CTI block name. Must be null terminated.
  @param [in ] type        : Indicates TRIGIN or TRIGOUT.
  @param [in ] max_triggers: Allocated length of mappings[].
  @param [out] length      : Returned length of mappings[].
  @param [out] mappings    : Array of channel mappings.
                             Each index is a trigger.

  @return 0 if successful, error code otherwise.
 */

int qdss_control_query_cti(const char *block,
                           uint8 type,
                           uint8 max_triggers,
                           uint8 *length,
                           uint32 *mappings);

/*-------------------------------------------------------------------------*/

int qdss_stm_trace_enable(void);
int qdss_stm_trace_disable(void);
void ETBSwitchToHWFIFO(void);
void QDSS_OpenSSCFunnel(void);
void QDSS_CloseSSCFunnel(void);

#endif //QDSS_CONTROL_H

#pragma once
/*============================================================================
  @file sns_err.h

  @brief SEE and SDC error handling functions

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

/**
*****************************************************************************************
                        Public Function Declarations
*****************************************************************************************
*/

/**
 * Register for SDC wdog bite
 *
 * When SDC crashes, error handling service on SDC generates wdog.
 * The Q6 gets wdog interrupt and call err_fatal.
 *
 */
void sns_sdc_wdog_enable(void);

/**
 * Send ERR_IPC_IN_IRQ to SDC
 *
 * Q6 sends ERR_IPC_IN_IRQ when Q6 crashes.
 * SDC triggers err_fatal and generates a wdog_expired_irq.
 * Q6 polls for Wdog_expired_irq and collect SDC code/data dumps.
 *
 */
void sns_err_handler_enable(void);


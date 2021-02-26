#pragma once
/*=============================================================================
  @file sns_client_manager_qmi.h

  Incoming requests to the Framework will arrive via the QMI Client Manager.
  This file handles all qmi interactions from client manager's perspective.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Defines
  ===========================================================================*/
//The maximum number of QMI connections allowed on CM.
//TODO Rework the implementation with a better design.
#define SNS_CM_MAX_QMI_CONN_LIMIT 70

/*=============================================================================
  Include Files
  ===========================================================================*/
/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/*=============================================================================
  Function Definitions
  ===========================================================================*/
/**
 * Register a service with the QMI infrastructure.
 *
 * @return
 * SNS_RC_FAILED - QMI registration error
 * SNS_RC_SUCCESS
 */
sns_rc sns_cm_service_register(sns_sensor *sensor);

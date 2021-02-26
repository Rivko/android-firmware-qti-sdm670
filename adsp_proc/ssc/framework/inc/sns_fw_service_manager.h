#pragma once
/*=============================================================================
  @file sns_fw_service_manager.h

  Private state used by the Framework in conjunction with sns_service_manager.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_rc.h"
#include "sns_service.h"
#include "sns_service_manager.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_fw_service_manager
{
  sns_service_manager service;

  /* Array of all known services.  Populated at boot-up by the Framework. */
  sns_service *services[SNS_SERVICE_TYPE_LENGTH];
} sns_fw_service_manager;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Initialize the Service Manager, and populate the list of known services.
 */
sns_rc sns_service_manager_init(void);

/**
 * Return a pointer to the service manager.
 */
sns_fw_service_manager* sns_service_manager_ref(void);

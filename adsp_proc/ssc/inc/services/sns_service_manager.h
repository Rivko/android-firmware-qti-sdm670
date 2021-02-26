#pragma once
/*=============================================================================
  @file sns_service_manager.h

  Provides access to all service objects.  Sensors must query for the
  needed service each time they need it; the Framework reserves the right
  to move a service into/out of island mode, and pointers kept to it may
  break.

  It would be appropriate within sns_sensor::init to query for all required
  services, to ensure the Sensor is capable of operating.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "sns_service.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_service_manager
{
  uint32_t struct_len;

  /**
   * Query the Framework for a particular service.
   *
   * @param[io] this Reference to the Service Manager
   * @param[i] service Service for which to query
   *
   * @return Service reference, or NULL if not present; shall be casted
   *         by the client to the appropriate service object.
   */
  sns_service* (*get_service)(
    struct sns_service_manager *this,
    sns_service_type service);
} sns_service_manager;

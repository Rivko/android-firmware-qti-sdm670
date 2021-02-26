#pragma once
/*=============================================================================
  @file sns_service.h

  Synchronous services provided by the Framework.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/inc/sns_service.h#1 $
  $DateTime: 2018/02/01 03:15:18 $
  $Change: 15367748 $
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Enumeration of Services provided by the Framework.
 * Some of these are core services which will be provided in every
 * variation of the Framework.  Others are target or version specific.
 * Clients must query to determine if a particular service is available
 * on the system.
 */
typedef enum sns_service_type
{
  SNS_STREAM_SERVICE,
  SNS_ATTRIBUTE_SERVICE,
  SNS_DYNAMIC_BUFFER_SERVICE,
  SNS_EVENT_SERVICE,
  SNS_POWER_RAIL_SERVICE,
  SNS_DIAG_SERVICE,
  SNS_SYNC_COM_PORT_SERVICE,
  SNS_GPIO_SERVICE,
  SNS_ISLAND_SERVICE,
  SNS_SERVICE_TYPE_LENGTH
} sns_service_type;

/**
 * Service attributes.  New fields cannot be added without a Framework major
 * revision.
 */
typedef struct sns_service
{
  /* Version number of this service; incremented only when new functionality
   * is added to the service.  Backward compatibility shall be maintained
   * for all services. */
  uint16_t version;
  sns_service_type type;
} sns_service;


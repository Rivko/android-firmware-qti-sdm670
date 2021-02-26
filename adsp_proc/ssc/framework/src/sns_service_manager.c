/*=============================================================================
  @file sns_service_manager.c

  Provides access to all service objects.  Sensors must query for the
  needed service each time they need it; the Framework reserves the right
  to move a service into/out of island mode, and pointers kept to it may
  break.

  It would be appropriate within sns_sensor::init to query for all required
  services, to ensure the Sensor is capable of operating.

  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdlib.h>
#include "sns_fw_service_manager.h"
#include "sns_rc.h"
#include "sns_service.h"
#include "sns_service_manager.h"

#include "sns_fw_attribute_service.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_event_service.h"
#include "sns_fw_pwr_rail_service.h"
#include "sns_fw_stream_service.h"
#include "sns_fw_sync_com_port_service.h"
#include "sns_island.h"

#include "sns_fw_gpio_service.h"

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/

static sns_fw_service_manager service_manager SNS_SECTION(".data.sns");

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") static sns_service*
get_service(struct sns_service_manager *svc_mngr, sns_service_type service)
{
  sns_fw_service_manager *this = (sns_fw_service_manager*)svc_mngr;
  if(service >= SNS_SERVICE_TYPE_LENGTH)
    return NULL;

  if(!sns_island_is_island_ptr((intptr_t)this->services[service]))
  {
    SNS_ISLAND_EXIT();
  }

  return this->services[service];
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") sns_fw_service_manager*
sns_service_manager_ref(void)
{
  return &service_manager;
}

sns_rc
sns_service_manager_init(void)
{
  service_manager.service.struct_len = sizeof(service_manager.service);
  service_manager.service.get_service = &get_service;

  service_manager.services[SNS_STREAM_SERVICE] = (sns_service*)sns_stream_service_init();
  service_manager.services[SNS_ATTRIBUTE_SERVICE] = (sns_service*)sns_attr_svc_init();
  //service_manager.services[SNS_DYNAMIC_BUFFER_SERVICE] = ;
  service_manager.services[SNS_EVENT_SERVICE] = (sns_service*)sns_event_service_init();
  service_manager.services[SNS_DIAG_SERVICE] = (sns_service *)sns_diag_service_init();
  service_manager.services[SNS_POWER_RAIL_SERVICE] = (sns_service*)sns_pwr_rail_service_init();
  service_manager.services[SNS_SYNC_COM_PORT_SERVICE] = (sns_service *)sns_sync_com_port_service_init();
  service_manager.services[SNS_GPIO_SERVICE] = (sns_service *)sns_gpio_service_init();
  service_manager.services[SNS_ISLAND_SERVICE] = (sns_service *)sns_island_service_init();
  return SNS_RC_SUCCESS;
}

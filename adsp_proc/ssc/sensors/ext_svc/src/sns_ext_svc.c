/*=============================================================================
  @file sns_ext_svc.c

  External Service sensor implementation

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_assert.h"
#include "sns_ext_svc.h"
#include "sns_memmgr.h"
#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Each corresponds to an external service supported by this Sensor.
 */
typedef struct sns_ext_service
{
  sns_isafe_list_item list_entry;
  // Service ID, as listed in enum sns_ext_svc
  int32_t svc_id;
  // QMI Service object used to encode, decode, and connect
  qmi_idl_service_object_type svc_obj;
} sns_ext_service;

/*=============================================================================
  External Variables
  ===========================================================================*/

extern sns_sensor_instance_api sns_ext_svc_sensor_instance_api;
extern sns_sensor_api sns_ext_svc_sensor_api;

/*=============================================================================
  Function Definitions
  ===========================================================================*/

sns_rc
sns_ext_svc_register(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_ext_svc_sensor_state),
                            &sns_ext_svc_sensor_api,
                            &sns_ext_svc_sensor_instance_api);

  return SNS_RC_SUCCESS;
}

qmi_idl_service_object_type
sns_ext_svc_get(sns_isafe_list*services, uint32_t const svc_id)
{
  sns_isafe_list_iter iter;

  for(sns_isafe_list_iter_init(&iter, services, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_ext_service *curr = (sns_ext_service*)sns_isafe_list_iter_get_curr_data(&iter);
    if(svc_id == curr->svc_id)
      return curr->svc_obj;
  }

  return NULL;
}

void
sns_ext_svc_add(sns_isafe_list*services, uint32_t svc_id, qmi_idl_service_object_type svc_obj)
{
  sns_isafe_list_iter iter;
  sns_ext_service *svc;
  SNS_ASSERT(NULL == sns_ext_svc_get(services, svc_id));

  svc = sns_malloc(SNS_HEAP_MAIN, sizeof(*svc));
  SNS_ASSERT(NULL != svc);

  svc->svc_id = svc_id;
  svc->svc_obj = svc_obj;
  sns_isafe_list_item_init(&svc->list_entry, svc);
  sns_isafe_list_iter_init(&iter, services, false);
  sns_isafe_list_iter_insert(&iter, &svc->list_entry, true);
}

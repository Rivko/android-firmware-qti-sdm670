/*============================================================================
  @file sns_osa_sem.c

  @brief
  OS Abstraction layer for Sensors.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/3.2/utils/osa/x86/sns_osa_sem.c#2 $
  $DateTime: 2018/03/27 11:46:46 $
  $Change: 15784639 $
  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/
#include <semaphore.h>
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_sem.h"
#include "sns_printf_int.h"

/*============================================================================
  Typedefs
  ============================================================================*/
typedef sem_t sns_osa_x86_sem;

typedef struct
{
  sns_osa_mem_type mem_type;
  int32_t value;
} sns_osa_x86_sem_attr;

/*============================================================================
  Public   Functions
  ============================================================================*/

sns_rc
sns_osa_sem_attr_init(sns_osa_sem_attr *attrib)
{
  _Static_assert(sizeof(sns_osa_sem_attr) >= sizeof(sns_osa_x86_sem_attr),
    "__SIZEOF_ATTR_SEM is smaller than sizeof(sns_osa_x86_sem_attr)");
  sns_osa_x86_sem_attr *sem_attrib = (sns_osa_x86_sem_attr*)attrib;

  if(NULL == sem_attrib)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to init osa_sem attribute: NULL attrib");
    return SNS_RC_FAILED;
  }
  sem_attrib->value = 0;
  sem_attrib->mem_type = SNS_OSA_MEM_TYPE_NORMAL;
  return SNS_RC_SUCCESS;
}

sns_rc
sns_osa_sem_attr_set_memory_partition(
  sns_osa_sem_attr      *attrib,
  sns_osa_mem_type      mem_type)
{
  sns_osa_x86_sem_attr *sem_attrib = (sns_osa_x86_sem_attr*)attrib;
  if(NULL == sem_attrib)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set memory partition: NULL attrib");
    return SNS_RC_FAILED;
  }
  if(mem_type > SNS_OSA_MEM_TYPE_MAX)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set memory partition: type out of range");
    return SNS_RC_NOT_SUPPORTED;
  }
  sem_attrib->mem_type = mem_type;
  return SNS_RC_SUCCESS;
}

sns_rc
sns_osa_sem_attr_set_value(sns_osa_sem_attr *attrib, int32_t value)
{
  sns_osa_x86_sem_attr *sem_attrib = (sns_osa_x86_sem_attr*)attrib;
  if(NULL == sem_attrib)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set value: NULL attrib");
    return SNS_RC_FAILED;
  }
  sem_attrib->value = value;
  return SNS_RC_SUCCESS;
}

sns_rc
sns_osa_sem_create(const sns_osa_sem_attr *attrib, sns_osa_sem **sem)
{
  sns_osa_x86_sem_attr *sem_attrib = (sns_osa_x86_sem_attr*)attrib;
  sns_osa_x86_sem *sem_i;
  sns_mem_heap_id id;

  if(NULL == sem || NULL == sem_attrib)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to create semaphore: sem or attrib NULL");
    return SNS_RC_FAILED;
  }
  id = sem_attrib->mem_type == SNS_OSA_MEM_TYPE_NORMAL ?
    SNS_HEAP_MAIN : SNS_HEAP_ISLAND;
  sem_i = sns_malloc(id, sizeof(*sem_i));
  if(NULL == sem_i)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to create semaphore: malloc failure");
    return SNS_RC_FAILED;
  }
  sem_init(sem_i, 0, sem_attrib->value);
  *sem = (sns_osa_sem*)sem_i;
  return SNS_RC_SUCCESS;
}

sns_rc
sns_osa_sem_delete(sns_osa_sem *sem)
{
  sns_osa_x86_sem *sem_i= (sns_osa_x86_sem*)sem;
  if(NULL == sem_i)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to delete semaphore: NULL parameter");
    return SNS_RC_FAILED;
  }
  sem_destroy(sem_i);
  sns_free(sem_i);
  return SNS_RC_SUCCESS;
}

sns_rc
sns_osa_sem_wait(sns_osa_sem *sem)
{
  sns_osa_x86_sem *sem_i = (sns_osa_x86_sem*)sem;
  if(NULL == sem_i)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to wait on semaphore: NULL parameter");
    return SNS_RC_FAILED;
  }
  sem_wait(sem_i);
  return SNS_RC_SUCCESS;
}

sns_rc
sns_osa_sem_post(sns_osa_sem *sem)
{
  sns_osa_x86_sem *sem_i = (sns_osa_x86_sem*)sem;
  if(NULL == sem_i)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to post on semaphore: NULL parameter");
    return SNS_RC_FAILED;
  }
  sem_post(sem_i);
  return SNS_RC_SUCCESS;
}

int32_t
sns_osa_sem_get_value(sns_osa_sem *sem)
{
  sns_osa_x86_sem *sem_i = (sns_osa_x86_sem*)sem;
  int32_t value = -1;
  sem_getvalue(sem_i, &value);
  return value;
}

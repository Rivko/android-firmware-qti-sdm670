/**
 * sns_gpio_service.c
 *
 * The GPIO Service implementation
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/utils/gpio_service/hexagon/sns_gpio_service.c#1 $
 * $DateTime: 2018/02/01 03:15:18 $
 * $Change: 15367748 $
 *
 **/

#include <stdint.h>
#include <string.h>
#include "sns_assert.h"
#include "sns_fw_gpio_service.h"
#include "sns_gpio_service.h"
#include "sns_island.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_types.h"

#include "DDITlmm.h"
#include "DalDevice.h"
#include "uTlmm.h"

/** Version: */
static const uint16_t sns_gpio_service_version SNS_SECTION(".rodata.sns") = 1;

struct sns_fw_gpio_service
{
  sns_gpio_service service;

  DalDeviceHandle *dal_device_handle;
  /* Number of SNS GPIOs with active/sleeep available config info */
  uint32_t sns_num_gpios;
  /* Active/Sleep config for all SSC TLMM GPIOs */
  sns_gpio_config *gpio_state;
  
  sns_osa_lock *lock;
};

/**
 * Private state of the GPIO Service.
 */
static sns_fw_gpio_service gpio_service SNS_SECTION(".data.sns");

/** See sns_fw_gpio_service.h */
void sns_gpio_service_set_sns_gpio_config(bool sleep_config)
{
  uTlmmGpioSignalType slpi_gpio_config;
  uint32_t i = 0;
  
  if(sleep_config == true)
  {
    for(i = 0; i < gpio_service.sns_num_gpios; i++)
    {
      slpi_gpio_config = UTLMM_GPIO_CFG(gpio_service.gpio_state[i].gpio_num,
                           gpio_service.gpio_state[i].sleep_fs,
                           gpio_service.gpio_state[i].sleep_dir,
                           gpio_service.gpio_state[i].sleep_pull,
                           gpio_service.gpio_state[i].sleep_drive);
      if(!uTlmm_ConfigGpio(slpi_gpio_config, UTLMM_GPIO_ENABLE))
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Error when setting sleep config for sns gpio: %d", 
          gpio_service.gpio_state[i].gpio_num);
      }
      else
      {
        SNS_PRINTF(LOW, sns_fw_printf, "Setting Sleep config for GPIO:%d, Drive:%d",
                   gpio_service.gpio_state[i].gpio_num,
                   gpio_service.gpio_state[i].sleep_drive);
      }        
    }
  }
  else
  {
    for(i = 0; i < gpio_service.sns_num_gpios; i++)
    {
      slpi_gpio_config = UTLMM_GPIO_CFG(gpio_service.gpio_state[i].gpio_num,
                           gpio_service.gpio_state[i].active_fs,
                           gpio_service.gpio_state[i].active_dir,
                           gpio_service.gpio_state[i].active_pull,
                           gpio_service.gpio_state[i].active_drive);
      if(!uTlmm_ConfigGpio(slpi_gpio_config, UTLMM_GPIO_ENABLE))
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Error when setting active config for sns gpio: %d", 
          gpio_service.gpio_state[i].gpio_num);
      }        
      else
      {
        SNS_PRINTF(LOW, sns_fw_printf, "Setting Active config for GPIO:%d, Drive:%d",
                   gpio_service.gpio_state[i].gpio_num,
                   gpio_service.gpio_state[i].active_drive);
      }        
    }
  }
}

/** See sns_fw_gpio_service.h */
void sns_gpio_service_store_gpio_config(sns_gpio_config *gpio_config)
{
  uint32_t i;

  sns_osa_lock_acquire(gpio_service.lock);
  for(i = 0; i < gpio_service.sns_num_gpios && i < SNS_MAX_GPIO; i++)
  {
    if(gpio_service.gpio_state[i].gpio_num == gpio_config->gpio_num)
    {
      gpio_service.gpio_state[i].active_fs = gpio_config->active_fs;
      gpio_service.gpio_state[i].active_dir = gpio_config->active_dir;
      gpio_service.gpio_state[i].active_pull = gpio_config->active_pull;
      gpio_service.gpio_state[i].active_drive = gpio_config->active_drive;
      gpio_service.gpio_state[i].sleep_fs = gpio_config->sleep_fs;
      gpio_service.gpio_state[i].sleep_dir = gpio_config->sleep_dir;
      gpio_service.gpio_state[i].sleep_pull = gpio_config->sleep_pull;
      gpio_service.gpio_state[i].sleep_drive = gpio_config->sleep_drive;
      break;
    }
  }

  if(i >= gpio_service.sns_num_gpios && gpio_service.sns_num_gpios < SNS_MAX_GPIO)
  {
    sns_gpio_config *cfg = &gpio_service.gpio_state[gpio_service.sns_num_gpios++];
    cfg->gpio_num = gpio_config->gpio_num;
    cfg->active_fs = gpio_config->active_fs;
    cfg->active_dir = gpio_config->active_dir;
    cfg->active_pull = gpio_config->active_pull;
    cfg->active_drive = gpio_config->active_drive;
    cfg->sleep_fs = gpio_config->sleep_fs;
    cfg->sleep_dir = gpio_config->sleep_dir;
    cfg->sleep_pull = gpio_config->sleep_pull;
    cfg->sleep_drive = gpio_config->sleep_drive;

    SNS_PRINTF(LOW, sns_fw_printf, "Stored GPIO:%d, Num of GPIOs:%d", 
               gpio_config->gpio_num, gpio_service.sns_num_gpios);

    if(gpio_service.sns_num_gpios == SNS_MAX_GPIO)
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Excess number of SNS GPIOs registered with GPIO service");
    }
  }

  sns_osa_lock_release(gpio_service.lock);
}

/** See sns_gpio_service.h */
SNS_SECTION(".text.sns") sns_rc sns_read_gpio(uint32_t gpio,
                                              bool is_chip_pin,
                                              sns_gpio_state *state)
{
  if(is_chip_pin)
  {
    DALGpioSignalType gpio_config =
       DAL_GPIO_CFG(gpio, 0, DAL_GPIO_INPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA);
    DALGpioValueType gpio_val = DAL_GPIO_LOW_VALUE;

    sns_osa_lock_acquire(gpio_service.lock);

    /** DAL TLMM API are not available in island mode. Exit
     *  island mode. */
    SNS_ISLAND_EXIT();

    if(DAL_SUCCESS != DalTlmm_GpioIn(gpio_service.dal_device_handle,
                                     gpio_config,
                                     &gpio_val))
    {
      sns_osa_lock_release(gpio_service.lock);
      SNS_PRINTF(ERROR, sns_fw_printf, "DalTlmm_GpioIn gpio %d", gpio);
      return SNS_RC_NOT_SUPPORTED;
    }

    *state = (gpio_val == DAL_GPIO_LOW_VALUE) ? SNS_GPIO_STATE_LOW : SNS_GPIO_STATE_HIGH;

    //SNS_PRINTF(HIGH, sns_fw_printf, "read_gpio gpio %d gpio_val %d  state = %d", gpio, gpio_val, *state);

    sns_osa_lock_release(gpio_service.lock);
  }
  else
  {
    // TODO Get pin config from registry
    uTlmmGpioSignalType gpio_config =
       UTLMM_GPIO_CFG(gpio, 0, UTLMM_GPIO_INPUT, UTLMM_GPIO_NO_PULL, UTLMM_GPIO_2MA);
    uTlmmGpioValueType gpio_val = UTLMM_GPIO_LOW_VALUE;

    if(!uTlmm_GpioIn(gpio_config, &gpio_val))
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "uTlmm_GpioIn gpio %d", gpio);
      return SNS_RC_NOT_SUPPORTED;
    }

    *state = (gpio_val == UTLMM_GPIO_LOW_VALUE) ? SNS_GPIO_STATE_LOW : SNS_GPIO_STATE_HIGH;
  }

  return SNS_RC_SUCCESS;
}

/** See sns_gpio_service.h */
SNS_SECTION(".text.sns") sns_rc sns_write_gpio(uint32_t gpio,
                                               bool is_chip_pin,
                                               sns_gpio_drive_strength drive_strength,
                                               sns_gpio_pull_type pull,
                                               sns_gpio_state state)
{
  if(is_chip_pin)
  {
    DALGpioValueType out_val =
       (state == SNS_GPIO_STATE_LOW) ? DAL_GPIO_LOW_VALUE : DAL_GPIO_HIGH_VALUE;
    DALGpioSignalType gpio_config =
       DAL_GPIO_CFG_OUT(gpio, 0, DAL_GPIO_OUTPUT,
                       (DALGpioPullType)pull, (DALGpioDriveType)drive_strength, out_val);

    sns_osa_lock_acquire(gpio_service.lock);

    /** DAL TLMM APIs are not available in island mode. Exit island
     *  mode. */
    SNS_ISLAND_EXIT();

    //SNS_PRINTF(sns_fw_printf, (HIGH, 2, "write gpio %d out_val = %d", gpio, out_val);

    if(DAL_SUCCESS != DalTlmm_ConfigGpio(gpio_service.dal_device_handle, gpio_config, DAL_TLMM_GPIO_ENABLE))
    {
      sns_osa_lock_release(gpio_service.lock);
      SNS_PRINTF(ERROR, sns_fw_printf, "DalTlmm_ConfigGpio gpio %d", gpio);
      return SNS_RC_NOT_SUPPORTED;
    }
    sns_osa_lock_release(gpio_service.lock);
  }
  else
  {
    uTlmmGpioValueType out_val =
       (state == SNS_GPIO_STATE_LOW) ? UTLMM_GPIO_LOW_VALUE : UTLMM_GPIO_HIGH_VALUE;
    uTlmmGpioSignalType gpio_config =
       UTLMM_GPIO_CFG_OUT(gpio, 0, UTLMM_GPIO_OUTPUT, (uTlmmGpioPullType)pull, (uTlmmGpioDriveType)drive_strength, out_val);

    if(!uTlmm_ConfigGpio(gpio_config, UTLMM_GPIO_ENABLE))
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "uTlmm_ConfigGpio gpio %d", gpio);
      return SNS_RC_NOT_SUPPORTED;
    }
  }

  return SNS_RC_SUCCESS;
}

/**
 * GPIO Service API.
 */
static sns_gpio_service_api  gpio_service_api SNS_SECTION(".data.sns") =
{
  .struct_len = sizeof(sns_gpio_service_api),
  .read_gpio = &sns_read_gpio,
  .write_gpio = &sns_write_gpio
};

/**
 * See sns_fw_gpio_service.h
 */
sns_fw_gpio_service* sns_gpio_service_init(void)
{
  sns_osa_lock_attr   lock_attr;
  sns_rc return_code = SNS_RC_SUCCESS;

  gpio_service.service.api = &gpio_service_api;
  gpio_service.service.service.type = SNS_GPIO_SERVICE;
  gpio_service.service.service.version = sns_gpio_service_version;

  gpio_service.sns_num_gpios = 0;
  
  gpio_service.gpio_state = (sns_gpio_config *)sns_malloc(SNS_HEAP_MAIN, 
                               sizeof(sns_gpio_config) * SNS_MAX_GPIO);

  /** Initialize rail_mgr_lock. */
  sns_osa_lock_attr_init(&lock_attr);
  sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  return_code = sns_osa_lock_create(&lock_attr, &gpio_service.lock);
  SNS_ASSERT(SNS_RC_SUCCESS == return_code);

  if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_TLMM, &gpio_service.dal_device_handle))
  {
    SNS_ASSERT(0);
  }

  return &gpio_service;
}

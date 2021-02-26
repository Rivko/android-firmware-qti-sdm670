/**
 * sns_pwr_sleep_mgr.c
 *
 * The Sensors Power Sleep Manager implementation
 *
 * Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/utils/pwr_sleep_mgr/hexagon/sns_pwr_sleep_mgr.c#4 $
 * $DateTime: 2018/05/24 18:16:33 $
 * $Change: 16250947 $
 *
 **/

#include <stdint.h>
#include <string.h>
#include "sns_assert.h"
#include "sns_fw_sensor_instance.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_std_sensor.pb.h"
#include "sns_types.h"

#include "npa.h"
#include "npa_resource.h"

/**
 * Private instance wakeup rate list item.
 */
typedef struct sns_pwr_sleep_instance_item
{
  /* Sensors instance pointer */
  sns_sensor_instance *instance;
  /* This sensor instance's Q6 wakeup rate in Hz */
  float               q6_wakeup_rate;
  /* true for an SDC sensor routing data through the DAE
   * false otherwise
   */
  bool                dae_sensor;
  /* True if instance and its data streams are in island */
  bool                island_instance;
  /* Instance list entry */
  sns_isafe_list_item       list_entry;
}sns_pwr_sleep_instance_item;

/**
 * Private state definition.
 */
struct sns_fw_pwr_sleep_mgr
{
  /* NPA client handle to the Sleep driver max duration node */
  npa_client_handle max_duration_handle;
  /* NPA client handle to the Sleep driver normal latency node */
  npa_client_handle normal_latency_handle;
  /* UNPA client handle to control island sleep modes */
  int32_t           unpa_client;
  /* Current effective normal mode Q6 wakeup rate in Hz */
  float             normal_mode_wakeup_rate;
  /* Current effective island mode Q6 wakeup rate in Hz */
  float             island_mode_wakeup_rate;
  /* Current wakeup rate of sensors routing data through the DAE sensor */
  float             curr_dae_wakeup_rate;
  /* List of sensor instances aggregating to the effective Q6 wakeup rate */
  struct sns_isafe_list  instance_list;
  /* True if island mode is enabled, False otherwise */
  bool              island_enabled;
  /* Sensor power sleep manager mutex */
  sns_osa_lock*     sleep_mgr_lock;
} ;

/**
 * Private state of the Power Sleep Manager Service.
 */
static struct sns_fw_pwr_sleep_mgr sns_pwr_sleep_mgr SNS_SECTION(".data.sns");

/*=============================================================================
  Forward Declarations
  ===========================================================================*/
extern bool sns_instance_streams_in_island(sns_sensor_instance *instance);

extern void sns_pwr_sleep_mgr_disable_lpm_plus(int32_t unpa_client);

extern void sns_pwr_sleep_mgr_enable_lpm_plus(int32_t unpa_client);

extern int32_t sns_pwr_sleep_mgr_create_unpa_handle(void);

/**
 * Enable/disable island mode sleep modes based on Q6 wakeup rates
 * in island mode. Send the request to the sleep driver
 */
SNS_SECTION(".text.sns") static void
sns_pwr_sleep_mgr_set_island_mode_sleep_mode(float wakeup_rate)
{
  if(wakeup_rate > SNS_PWR_SLEEP_MGR_ISLAND_SLEEP_MODE_THRESHOLD_HZ)
  {
    sns_pwr_sleep_mgr_disable_lpm_plus(sns_pwr_sleep_mgr.unpa_client);
  }
  else
  {
    sns_pwr_sleep_mgr_enable_lpm_plus(sns_pwr_sleep_mgr.unpa_client);
  }
}

/**
 * Set the Q6 wakeup interval based on the Q6 wakeup rate and
 * send the request to the sleep driver NPA nodes
 */
SNS_SECTION(".text.sns") static void
sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(float wakeup_rate)
{
  uint32_t q6_wakeup_intvl_usec = 0;
  sns_isafe_list_iter  iter;

  SNS_ISLAND_EXIT();

  sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);

  /* Calcualte the wakeup interval in usecs from the effective Q6 wakeup rate */
  if (wakeup_rate > 0)
  {
    q6_wakeup_intvl_usec = (uint32_t)((USEC_PER_SEC) / wakeup_rate);
  }
  else
  {
    /* Wakeup interval of 0 with sensor instances in the list is a special case
     * that applies to motion gated or CCD gated sensor instances.
     * Set the wakeup interval to a max value.
     */
    if(sns_isafe_list_iter_len(&iter) > 1)
    {
      q6_wakeup_intvl_usec = SNS_PWR_SLEEP_MGR_MAX_WAKEUP_INTVL_USEC;
    }
    else
    {
      q6_wakeup_intvl_usec = 0;
    }
  }

  if(q6_wakeup_intvl_usec == 0)
  {
    /* If the Q6 wakeup interval is 0, no clients are present, hence
     * release the wakeup rate requirement to the sleep driver allowing for deep sleep
     */
    SNS_PRINTF(HIGH, sns_fw_printf, "Release the Max Duration request");
    npa_complete_request(sns_pwr_sleep_mgr.max_duration_handle);
  }
  else
  {
    /* Update the sleep driver with the new Q6 wakeup interval */
    SNS_PRINTF(HIGH, sns_fw_printf, "Set a new Q6 wakeup interval: %d", q6_wakeup_intvl_usec);
    npa_issue_required_request(sns_pwr_sleep_mgr.max_duration_handle, q6_wakeup_intvl_usec);
  }
  SNS_PROFILE(SNS_PWR_SLEEP_MGR_SET_Q6_WAKEUP_INTERVAL_USEC, 1, q6_wakeup_intvl_usec);
}

/**
 * Calculate the new effective Q6 wakeup rate based on the incoming
 * and outgoing wakeup rates
 *
 * @param[i] in_freq_hz    Q6 wakeup rate to be added to the aggregate
 * @param[i] out_freq_hz   Q6 wakeup rate to tbe removed from the aggregate
 */
SNS_SECTION(".text.sns") static void
sns_pwr_sleep_mgr_calc_eff_wakeup_rate(float in_freq_Hz, float out_freq_Hz, bool island_instance)
{
  float new_q6_wakeup_rate;

  if(true == island_instance)
  {
    new_q6_wakeup_rate = sns_pwr_sleep_mgr.island_mode_wakeup_rate;
  }
  else
  {
    new_q6_wakeup_rate = sns_pwr_sleep_mgr.normal_mode_wakeup_rate;
  }

  new_q6_wakeup_rate += (in_freq_Hz - out_freq_Hz);
  /* Correct for negative wakeup rate */
  if (new_q6_wakeup_rate < 0)
  {
    new_q6_wakeup_rate = 0;
  }

  if(true == island_instance)
  {
    sns_pwr_sleep_mgr.island_mode_wakeup_rate = new_q6_wakeup_rate;
  }
  else
  {
    sns_pwr_sleep_mgr.normal_mode_wakeup_rate = new_q6_wakeup_rate;
  }
  
  if((true == island_instance) && (true == sns_pwr_sleep_mgr.island_enabled))
  {
    sns_pwr_sleep_mgr_set_island_mode_sleep_mode(sns_pwr_sleep_mgr.island_mode_wakeup_rate);
  }
  else
  {
    sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(sns_pwr_sleep_mgr.normal_mode_wakeup_rate + 
      sns_pwr_sleep_mgr.island_mode_wakeup_rate);
  }
}

/**
 * Check to see if a sensor instance and all its data streams are in
 * island operation state
 *
 * @param[i] instance    Sensor instance pointer
 *
 * @return
 * true        If island and all its data streams are in island operation
 * false       Otherwise
 */
SNS_SECTION(".text.sns") static bool
sns_pwr_sleep_mgr_check_island_instance(sns_sensor_instance *instance)
{
  bool sns_rc = false;

  if(SNS_ISLAND_STATE_IN_ISLAND == ((sns_fw_sensor_instance *)instance)->island_operation)
  {
    sns_rc = sns_instance_streams_in_island(instance);
  }

  return sns_rc;
}

/**
 * Remove the sensor instance from the sleep manager instance list
 */
SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_sensor_instance_deinit(struct sns_fw_sensor_instance *instance)
{
  sns_isafe_list_iter iter;
  bool dae_sensor_instance =  false;
  sns_pwr_sleep_instance_item *instance_item = NULL;

  SNS_PROFILE(SNS_PWR_SLEEP_MGR_REMOVE_INSTANCE_START, 0);
  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);

  sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);

  /* Find the sensor instance in the list */
  for(;
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    instance_item = (sns_pwr_sleep_instance_item*)sns_isafe_list_iter_get_curr_data(&iter);

    if(instance == (struct sns_fw_sensor_instance*)instance_item->instance)
    {
      if(instance_item->dae_sensor)
      {
        /* DAE sensor instances being removed require special handling - see below*/
        dae_sensor_instance = true;
        break;
      }
      else
      {
        /* For non-DAE sensors update the effective Q6 wakeup rate and remove the instance */
        sns_pwr_sleep_mgr_calc_eff_wakeup_rate(0, instance_item->q6_wakeup_rate, instance_item->island_instance);
        sns_isafe_list_iter_remove(&iter);
        sns_free(instance_item);
        break;
      }
    }
  }

  if(dae_sensor_instance)
  {
    /* If the DAE sensor instance being removed is not the current Q6 DAE wakeup rate,
     * only the instance needs to be removed. No change needed to the Q6 wakeup rate
     */
    if(instance_item->q6_wakeup_rate != sns_pwr_sleep_mgr.curr_dae_wakeup_rate)
    {
      sns_isafe_list_iter_remove(&iter);
      sns_free(instance_item);
    }
    else
    {
      /* Remove the DAE sensor instance from the list and adjust the Q6 wakeup
       * rate accordingly
       */
      sns_pwr_sleep_mgr_calc_eff_wakeup_rate(0, instance_item->q6_wakeup_rate, instance_item->island_instance);
      sns_isafe_list_iter_remove(&iter);
      sns_free(instance_item);
      sns_pwr_sleep_mgr.curr_dae_wakeup_rate = 0;

      /* Look for the DAE sensor instance with the next highest Q6 wakeup rate */
      sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);
      for(;
          NULL != sns_isafe_list_iter_curr(&iter);
          sns_isafe_list_iter_advance(&iter))
      {
        instance_item = (sns_pwr_sleep_instance_item *)sns_isafe_list_iter_get_curr_data(&iter);
        if(instance_item->dae_sensor)
        {
          if(instance_item->q6_wakeup_rate > sns_pwr_sleep_mgr.curr_dae_wakeup_rate)
          {
            sns_pwr_sleep_mgr.curr_dae_wakeup_rate = instance_item->q6_wakeup_rate;
          }
        }
      }
      /* If a DAE sensor instance is found, update the effective Q6 wakeup rate */
      if(sns_pwr_sleep_mgr.curr_dae_wakeup_rate != 0)
      {
        sns_pwr_sleep_mgr_calc_eff_wakeup_rate(sns_pwr_sleep_mgr.curr_dae_wakeup_rate, 0, instance_item->island_instance);
      }
    }
  }
  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_PROFILE(SNS_PWR_SLEEP_MGR_REMOVE_INSTANCE_END, 0);
}

/**
 * Adds/updates a sensor power instance to the power manager sensor instance list
 * Calls the internal helper functions to aggregate the new Q6 wakeup rate and
 * communicate it to the sleep driver
 *
 * @param[i] instance           sensor instance to be added/updated
 * @param[i] phy_sensor_config  physical sensor config information
 */
SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_add_instance(sns_sensor_instance *instance,
                               sns_std_sensor_physical_config_event const*phy_sensor_config)
{
  sns_isafe_list_iter  iter;
  sns_pwr_sleep_instance_item *pwr_sleep_instance_item = NULL;
  float instance_wakeup_rate;
  bool island_instance = false;

  SNS_PROFILE(SNS_PWR_SLEEP_MGR_ADD_INSTANCE_START, 0);

  island_instance = sns_pwr_sleep_mgr_check_island_instance(instance);

  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);


  if(phy_sensor_config->has_sample_rate && phy_sensor_config->sample_rate != 0)
  {
    /* Use DAE/FIFO watermark to calculate the sensor instance Q6 wakeup rate
     * depending on what the sensor instance supports
     */
    if(phy_sensor_config->has_DAE_watermark == true && phy_sensor_config->DAE_watermark != 0)
    {
      instance_wakeup_rate = (phy_sensor_config->sample_rate / phy_sensor_config->DAE_watermark);
    }
    else if(phy_sensor_config->has_water_mark == true && phy_sensor_config->water_mark != 0)
    {
      instance_wakeup_rate = (phy_sensor_config->sample_rate / phy_sensor_config->water_mark);
    }
    else
    {
      instance_wakeup_rate = phy_sensor_config->sample_rate;
    }
  }
  else
  {
    instance_wakeup_rate = 0;
  }

  sns_isafe_list_iter_init(&iter, &sns_pwr_sleep_mgr.instance_list, true);

  /* Check if this sensor instance is already in the list */
  for(;
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_pwr_sleep_instance_item *instance_item =
      (sns_pwr_sleep_instance_item *)sns_isafe_list_iter_get_curr_data(&iter);

    if (instance == instance_item->instance)
    {
      pwr_sleep_instance_item = instance_item;
      break;
    }
  }

  /* Create a new instance item if its not already present on the list */
  if(NULL == pwr_sleep_instance_item)
  {
    pwr_sleep_instance_item = sns_malloc(SNS_HEAP_ISLAND, sizeof(*pwr_sleep_instance_item));
    if (NULL == pwr_sleep_instance_item)
    {
      SNS_ISLAND_EXIT();
      pwr_sleep_instance_item = sns_malloc(SNS_HEAP_MAIN, sizeof(*pwr_sleep_instance_item));
    }
    SNS_ASSERT(pwr_sleep_instance_item != NULL);

    pwr_sleep_instance_item->instance = instance;
    pwr_sleep_instance_item->q6_wakeup_rate = 0;
    pwr_sleep_instance_item->island_instance = island_instance;

    sns_isafe_list_item_init(&pwr_sleep_instance_item->list_entry, pwr_sleep_instance_item);
    sns_isafe_list_iter_insert(&iter, &pwr_sleep_instance_item->list_entry, true);
  }

  if(phy_sensor_config->has_DAE_watermark && phy_sensor_config->DAE_watermark != 0)
  {
    /* For sensors using DAE, update the effective Q6 wakeup rate only if the
     * new instance wakeup rate is > than the current DAE wakeup rate
     */
    pwr_sleep_instance_item->dae_sensor =  true;
    if(instance_wakeup_rate > sns_pwr_sleep_mgr.curr_dae_wakeup_rate)
    {
      /* If the instance has switched from island to non-island or vice versa update
         the wakeup rate for both sleep driver nodes. If not only update the wakeup
         rate for the respective node */
      if(pwr_sleep_instance_item->island_instance == island_instance)
      {
        sns_pwr_sleep_mgr_calc_eff_wakeup_rate(instance_wakeup_rate, sns_pwr_sleep_mgr.curr_dae_wakeup_rate, island_instance);
      }
      else
      {
        sns_pwr_sleep_mgr_calc_eff_wakeup_rate(instance_wakeup_rate, 0, island_instance);
        sns_pwr_sleep_mgr_calc_eff_wakeup_rate(0, sns_pwr_sleep_mgr.curr_dae_wakeup_rate, pwr_sleep_instance_item->island_instance);
      }
      sns_pwr_sleep_mgr.curr_dae_wakeup_rate = instance_wakeup_rate;
    }
  }
  else
  {
    /* For non DAE sensors calculate the new effective Q6 wakeup rate */
    /* If the instance has switched from island to non-island or vice versa update
       the wakeup rate for both sleep driver nodes. If not only update the wakeup rate
       for the respective node */
    if(pwr_sleep_instance_item->island_instance == island_instance)
    {
      sns_pwr_sleep_mgr_calc_eff_wakeup_rate(instance_wakeup_rate, pwr_sleep_instance_item->q6_wakeup_rate, island_instance);
    }
    else
    {
      sns_pwr_sleep_mgr_calc_eff_wakeup_rate(instance_wakeup_rate, 0, island_instance);
      sns_pwr_sleep_mgr_calc_eff_wakeup_rate(0, pwr_sleep_instance_item->q6_wakeup_rate, pwr_sleep_instance_item->island_instance);
    }
    pwr_sleep_instance_item->dae_sensor =  false;
  }
  pwr_sleep_instance_item->q6_wakeup_rate = instance_wakeup_rate;
  pwr_sleep_instance_item->island_instance = island_instance;

  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_PROFILE(SNS_PWR_SLEEP_MGR_ADD_INSTANCE_END, 0);
}

/**
 * Check for physical sensor config events.
 */
SNS_SECTION(".text.sns") sns_rc
sns_pwr_sleep_mgr_check_config_event(sns_sensor_instance *instance,
    sns_sensor_event const *event, bool is_physical_sensor)
{
  sns_std_sensor_physical_config_event phy_sensor_config =
      sns_std_sensor_physical_config_event_init_default;
  pb_istream_t stream;
  sns_rc rc = SNS_RC_SUCCESS;

  /* Only decode physical sensor config events */
  if ((SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT != event->message_id) ||
      !is_physical_sensor)
  {
    return rc;
  }

  stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

  if (!pb_decode(&stream, sns_std_sensor_physical_config_event_fields, &phy_sensor_config))
  {
    /* Decode failed */
    rc = SNS_RC_FAILED;
  }
  else
  {
    sns_pwr_sleep_mgr_add_instance(instance, &phy_sensor_config);
  }
  return rc;
}

/**
 * Initialize the sensors power sleep manager.
 */
sns_rc
sns_pwr_sleep_mgr_init(void)
{
  sns_osa_lock_attr   lock_attr;
  sns_rc rc = SNS_RC_SUCCESS;

  /* Initialize handles to the sleep driver NPA nodes */
  sns_pwr_sleep_mgr.max_duration_handle = npa_create_sync_client(
                            "/sleep/max_duration/usec",
                            "SNS_PM_MaxDuration",
                            NPA_CLIENT_REQUIRED);
  SNS_ASSERT(NULL != sns_pwr_sleep_mgr.max_duration_handle);

  sns_pwr_sleep_mgr.normal_latency_handle = npa_create_sync_client(
                          "/core/cpu/latency/usec",
                          "SNS_PM_NormalLatency",
                          NPA_CLIENT_REQUIRED);
  SNS_ASSERT(NULL != sns_pwr_sleep_mgr.normal_latency_handle);

  sns_pwr_sleep_mgr.unpa_client = sns_pwr_sleep_mgr_create_unpa_handle();

  /* Initialize list of sensor instances aggregated towards Q6 wakeup rate */
  sns_isafe_list_init(&sns_pwr_sleep_mgr.instance_list);

  /* Initialize current DAE wakeup rate to 0 */
  sns_pwr_sleep_mgr.curr_dae_wakeup_rate = 0;

  /* Initialize power sleep manager lock */
  sns_osa_lock_attr_init(&lock_attr);
  sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc = sns_osa_lock_create(&lock_attr, &sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  return rc;
}

/**
 * Set normal mode sleep latency.
 * Configure how fast the system must respond to a normal mode wakeup interrupt
 *
 * @param[i] latency_us   Allowed latency in microseconds
 */
SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_set_latency(uint32 latency_us)
{
  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);
  npa_issue_required_request(sns_pwr_sleep_mgr.normal_latency_handle, latency_us);
  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_PRINTF(HIGH, sns_fw_printf, "Set latency to (%u)us", latency_us);
}

/**
 * Unset normal mode sleep latency.
 */
SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_release_latency()
{
  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);
  npa_complete_request(sns_pwr_sleep_mgr.normal_latency_handle);
  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_PRINTF(HIGH, sns_fw_printf, "Release latency request");
}

/**
 * Set/reset island mode enabled flag in power sleep manager
 */
SNS_SECTION(".text.sns") void
sns_pwr_sleep_mgr_set_island_mode_enabled(bool island_enabled)
{
  sns_osa_lock_acquire(sns_pwr_sleep_mgr.sleep_mgr_lock);
  sns_pwr_sleep_mgr.island_enabled = island_enabled;
  if(true == island_enabled)
  {
    sns_pwr_sleep_mgr_set_island_mode_sleep_mode(sns_pwr_sleep_mgr.island_mode_wakeup_rate);
    sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(sns_pwr_sleep_mgr.normal_mode_wakeup_rate);    
  }
  else
  {
    sns_pwr_sleep_mgr_set_normal_mode_wakeup_interval(sns_pwr_sleep_mgr.normal_mode_wakeup_rate + 
    sns_pwr_sleep_mgr.island_mode_wakeup_rate);
  }
  sns_osa_lock_release(sns_pwr_sleep_mgr.sleep_mgr_lock);
  SNS_PRINTF(LOW, sns_fw_printf, "Sleep Manager: Island mode state set to: %d", island_enabled);
}


/**
 * sns_pwr_rail_manager.c
 *
 * The Power Rail Management Utility implementation
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/utils/pwr_rail_mgr/hexagon/sns_pwr_rail_service.c#4 $
 * $DateTime: 2018/06/04 12:04:04 $
 * $Change: 16318356 $
 *
 **/

#include <stdint.h>
#include <string.h>
#include "sns_assert.h"
#include "sns_fw_gpio_service.h"
#include "sns_fw_pwr_rail_service.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_pwr_rail_config.h"
#include "sns_pwr_rail_service.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_sensor.h"
#include "sns_time.h"
#include "sns_types.h"

#include "mmpm.h"
#include "npa.h"
#include "npa_resource.h"
#include "pmapp_npa.h"

/** Version:
 */
static const uint16_t sns_pwr_rail_service_version SNS_SECTION(".rodata.sns") = 1;

/**
 *  Maximum allowed rails for sensors. The PMIC NPA interface
 *  defines 4 strings that correspond to rails available to
 *  Sensors.
 */
#define RAIL_NUM_MAX              4

/**
 * Define a dummy rail for OEMs that use external rails for their sensors.
 * Dummy rail config is required so island mode block/unblock logic still
 * works for those OEMs.
 */
#define DUMMY_SENSOR_VDD "/pmic/client/dummy_vdd"

/**
 *  Single item that goes into client list maintained per
 *  rail.
 */
typedef struct sns_client_item
{
  const sns_sensor      *sensor;       /** Client sensor.*/
  sns_power_rail_state  vote;          /** Client sensor's rail vote.*/
  sns_isafe_list_item         list_entry;    /** List entry to add/remove from list.*/
} sns_client_item;

/**
 *  Rail information maintained by the power rail manager.
 */
typedef struct sns_rail
{
  sns_rail_name         name;             /** Rail name.*/
  sns_power_rail_state  state;            /** Current state of rail.*/
  sns_time              on_timestamp;     /** Timestamp in ticks when the
                                              rail state changed to a higher
                                              vote.
                                              Example: OFF->LPM, LPM->NPM. OFF->NPM.*/
  npa_client_handle     rail_npa_handle;  /** NPA handle*/
  struct sns_isafe_list      client_item_list; /** List of clients for the rail.*/
} sns_rail;

/**
 * Private state definition.
 */
struct sns_fw_pwr_rail_service
{
  sns_pwr_rail_service service;

  sns_rail                rails[RAIL_NUM_MAX];    /** List of initialized rails.*/
  uint8_t                 num_rails_initialized;  /** Number of rails initialized.*/
  sns_island_client_handle  island_client;           /** Island mode client*/
  bool                      sleep_set;               /** Sleep config set*/
  sns_osa_lock*           rail_mgr_lock;           /** Mutex.*/
  // Client for SCPM voting
  sns_scpm_mgr_client *pwr_client;
} ;

/**
 * Private state of the Power Rail Manager Service.
 */
static sns_fw_pwr_rail_service pwr_rail_service SNS_SECTION(".data.sns");

/**
 *  Check to see if there are any clients on any of the power rails.
 *  Disable island mode if no clients are present
 *  Enable island mode if the sensors subsystem transitioned from no
 *  client to client
 */
SNS_SECTION(".text.sns") static void sns_check_pwr_rail_client_count(void)
{
  int i, pwr_rail_client_count = 0;
  sns_isafe_list_iter     iter;

  for(i = 0; i < pwr_rail_service.num_rails_initialized; i++)
  {
    sns_isafe_list_iter_init(&iter, &pwr_rail_service.rails[i].client_item_list, true);
    pwr_rail_client_count += sns_isafe_list_iter_len(&iter);
  }

  if((pwr_rail_client_count == 0) && (pwr_rail_service.sleep_set == false))
  {
    sns_island_block(pwr_rail_service.island_client);
    if(NULL != pwr_rail_service.pwr_client)
    {
       // Vote for default value, when there is a transition from client to no client
       sns_pwr_scpm_mgr_absolute_mcps_vote(pwr_rail_service.pwr_client,SNS_Q6_MCPS_DEFAULT);
    }
    sns_gpio_service_set_sns_gpio_config(true);
    pwr_rail_service.sleep_set = true;
  }
  else if((pwr_rail_client_count > 0) && (pwr_rail_service.sleep_set == true))
  {
    // If running mcps voting is default and If there is a client to power rail service,
    // Then,vote for minimum mcps vote and worker threads takes care bumping up and bumping down based on utilization
    if(SNS_Q6_MCPS_DEFAULT == sns_pwr_scpm_mgr_get_current_mcps_vote())
    {
	  if(NULL != pwr_rail_service.pwr_client)
	  {
	    sns_pwr_scpm_mgr_absolute_mcps_vote(pwr_rail_service.pwr_client,SNS_Q6_MCPS_LOW_SVS);
	  }
    }
    /* Setting the active gpio config to be done by the individual bus drivers */
    sns_island_unblock(pwr_rail_service.island_client);
    pwr_rail_service.sleep_set = false;
  }
}

/**
 *  Checks if input rail is registered. Also returns index of
 *  the rail in internal state if the clients needs this
 *  informaiton.
 *
 *  @param[i] rail        rail to check
 *  @param[o] rail_index  index of this rail in internal state
 *
 *  @return
 *  true if rail is registered. false otherwise.
 */
SNS_SECTION(".text.sns") static bool sns_rail_is_registered(sns_rail_name const* rail, uint8_t* rail_index)
{
  bool rail_is_registered = false;
  int i;


  for(i = 0; i < pwr_rail_service.num_rails_initialized; i++)
  {
    if(0 == strncmp(pwr_rail_service.rails[i].name.name,
                    rail->name,
                    sizeof(pwr_rail_service.rails[i].name.name)))
    {
      rail_is_registered = true;
      if(rail_index != NULL)
      {
        *rail_index = (uint8_t)i;
      }
      break;
    }
  }

  return rail_is_registered;
}

/**
 *  Checks if input rail name is valid (supported by PMIC API).
 *
 *  @param[i] rail        rail to check
 *
 *  @return
 *  true if rail name is valis. false otherwise.
 */
SNS_SECTION(".text.sns") static bool sns_rail_name_is_valid(sns_rail_name const* rail)
{
  bool rail_is_valid = true;

  /* Rails defined in pmapp_npa.h:
    #define PMIC_NPA_GROUP_ID_SENSOR_VDD "/pmic/client/sensor_vdd"
    #define PMIC_NPA_GROUP_ID_SENSOR_VDDIO "/pmic/client/sensor_vddio"
    #define PMIC_NPA_GROUP_ID_SENSOR_VDD_2 "/pmic/client/sensor_vdd_2"
    #define PMIC_NPA_GROUP_ID_SENSOR_VDDIO_2 "/pmic/client/sensor_vddio_2"
  */

  /* Dummy rail for clients that use external rails
     #define DUMMY_SENSOR_VDD "/pmic/client/dummy_vdd"
  */
  if(   0 != strncmp(PMIC_NPA_GROUP_ID_SENSOR_VDD, rail->name, sizeof(PMIC_NPA_GROUP_ID_SENSOR_VDD))
     && 0 != strncmp(PMIC_NPA_GROUP_ID_SENSOR_VDDIO, rail->name, sizeof(PMIC_NPA_GROUP_ID_SENSOR_VDDIO))
     && 0 != strncmp(PMIC_NPA_GROUP_ID_SENSOR_VDD_2, rail->name, sizeof(PMIC_NPA_GROUP_ID_SENSOR_VDD_2))
     && 0 != strncmp(PMIC_NPA_GROUP_ID_SENSOR_VDDIO_2, rail->name, sizeof(PMIC_NPA_GROUP_ID_SENSOR_VDDIO_2))
     && 0 != strncmp(DUMMY_SENSOR_VDD, rail->name, sizeof(DUMMY_SENSOR_VDD))
        )
  {
     rail_is_valid = false;
  }

  return rail_is_valid;
}

/**
 *  Adds a client to the list of clients for the input rail.
 *
 *  @param[i] sensor        client for the rail
 *  @param[i] vote          client's vote for this rail
 *  @param[i] rail_index    input rail index
 */
SNS_SECTION(".text.sns") static void sns_add_pwr_rail_client(sns_sensor const* sensor,
                                    sns_power_rail_state vote,
                                    uint8_t rail_index)
{
  sns_isafe_list_iter     iter;
  sns_client_item   *client = NULL;
  bool client_found = false;

  sns_isafe_list_iter_init(&iter, &pwr_rail_service.rails[rail_index].client_item_list, true);

  for(;
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    client = (sns_client_item *)sns_isafe_list_iter_get_curr_data(&iter);
    if(sensor == client->sensor)
    {
      client_found = true;
      break;
    }
  }

  if(client_found == true)
  {
    client->vote = vote;
  }
  else
  {
    client = sns_malloc(SNS_HEAP_MAIN, sizeof(*client));

    SNS_ASSERT(client != NULL);
    client->sensor = sensor;
    client->vote = vote;

    sns_isafe_list_item_init(&client->list_entry, client);
    sns_isafe_list_iter_insert(&iter, &client->list_entry, true);

    sns_check_pwr_rail_client_count();
  }
}

/**
 *  Removes a client for the input rail.
 *
 *  @param[i] sensor        client for the rail
 *  @param[i] rail_index    input rail index
 */
SNS_SECTION(".text.sns") static void sns_remove_pwr_rail_client(sns_sensor const* sensor, uint8_t rail_index)
{
  sns_isafe_list_iter   iter;

  sns_isafe_list_iter_init(&iter, &pwr_rail_service.rails[rail_index].client_item_list, true);

  for(;
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_client_item *client =
       (sns_client_item*)sns_isafe_list_iter_get_curr_data(&iter);

    if(sensor == client->sensor)
    {
      sns_isafe_list_iter_remove(&iter);
      sns_free(client);
      break;
    }
  }
  sns_check_pwr_rail_client_count();
}

/**
 *  Parses client info for the rail and chooses best state.
 *
 *  @param[i] rail_index  input rail index
 */
SNS_SECTION(".text.sns") static void sns_update_power_rail_state(uint8_t rail_index)
{
  sns_power_rail_state new_state = SNS_RAIL_OFF;
  sns_isafe_list_iter iter;
  uint8_t pmic_rail_state;

  sns_isafe_list_iter_init(&iter, &pwr_rail_service.rails[rail_index].client_item_list, true);

  for(;
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    // Search for the highest vote in rail's client list.
    new_state = SNS_MAX(new_state, ((sns_client_item*)sns_isafe_list_iter_get_curr_data(&iter))->vote);
  }

  if(new_state != pwr_rail_service.rails[rail_index].state)
  {
    /* state        new_state                     action
     *  ----------------------------------------------------------------------
     *  OFF          LPM/NPM     change right away. Update on_timestamp.
     *  LPM          NPM         change right away. Update on_timestamp.
     *  NPM/LPM      OFF         change right away. Clear on_timestamp.
     *  NPM          LPM         change right away. No change to on_timestamp.
     */

    if (SNS_VDDIO_PWR_RAIL_ALWAYS_ON == 0x1)
    {
    	if ((new_state == SNS_RAIL_OFF)
        	&& (0 == strncmp(PMIC_NPA_GROUP_ID_SENSOR_VDDIO, pwr_rail_service.rails[rail_index].name.name, sizeof(PMIC_NPA_GROUP_ID_SENSOR_VDDIO))))
        {
        	return;
        }
    }

    pmic_rail_state = (new_state == SNS_RAIL_OFF) ?
                      PMIC_NPA_MODE_ID_SENSOR_POWER_OFF :
                      ((new_state == SNS_RAIL_ON_LPM) ?
                      PMIC_NPA_MODE_ID_SENSOR_LPM :
                      PMIC_NPA_MODE_ID_SENSOR_POWER_ON);

    npa_issue_required_request(pwr_rail_service.rails[rail_index].rail_npa_handle,
                               pmic_rail_state);

    if(new_state > pwr_rail_service.rails[rail_index].state)
    {
      pwr_rail_service.rails[rail_index].on_timestamp = sns_get_system_time();
    }
    else
    {
      if(new_state == SNS_RAIL_OFF)
      {
        pwr_rail_service.rails[rail_index].on_timestamp = 0;
      }
    }

    pwr_rail_service.rails[rail_index].state = new_state;
  }
}

/**
 * Return timestamp when rail state was changed to a higher
 * power state.
 *
 * @param[i] rail_index     rail_index.
 *
 * @return
 * Time tick with rail was turned ON. 0 if the rail is
 * OFF.
 */
SNS_SECTION(".text.sns") static sns_time sns_get_rail_on_timestamp(rail_index)
{
  sns_time on_timestamp = 0;

  if(pwr_rail_service.rails[rail_index].state != SNS_RAIL_OFF)
  {
    on_timestamp = pwr_rail_service.rails[rail_index].on_timestamp;
  }

  return on_timestamp;
}

/**
 * Initializes a power rail with PMIC driver and add this rail
 * to the list of rails maintined by the power rail manager.
 *
 * IMPORTANT NOTE:
 * Each rail maps to a specific client handle of PMIC.
 * Internally, PAM table will contain managing of these rails.
 * So, if any rail needs to be changed from what's provided,
 * please ensure that PAM table is appropriately updated to
 * reflect the particular rail.
 *
 * Rail names are defined in pmapp_npa.h.
 *
 * @param[i] rail      rail to be initialized
 *
 * @return none
 */
SNS_SECTION(".text.sns") static sns_rc sns_pwr_rail_init(sns_rail_name const* rail)
{

  if(!sns_rail_name_is_valid(rail))
  {
    return SNS_RC_INVALID_VALUE;
  }

  if(sns_rail_is_registered(rail, NULL))
  {
    return SNS_RC_SUCCESS;
  }

  SNS_ASSERT(pwr_rail_service.num_rails_initialized < RAIL_NUM_MAX);

  // Init default state for rail.
  pwr_rail_service.rails[pwr_rail_service.num_rails_initialized].state = SNS_RAIL_OFF;

  // Copy Rail name.
  sns_strlcpy(pwr_rail_service.rails[pwr_rail_service.num_rails_initialized].name.name,
         rail->name,
         RAIL_NAME_STRING_SIZE_MAX);

  // Init client list for rail.
  sns_isafe_list_init(&pwr_rail_service.rails[pwr_rail_service.num_rails_initialized].client_item_list);

  // Get NPA handle for rail. Not to be done for dummy rail
  if(0 != strncmp(DUMMY_SENSOR_VDD, pwr_rail_service.rails[pwr_rail_service.num_rails_initialized].name.name,
                    sizeof(DUMMY_SENSOR_VDD)))
  {
    pwr_rail_service.rails[pwr_rail_service.num_rails_initialized].rail_npa_handle =
        npa_create_sync_client(pwr_rail_service.rails[pwr_rail_service.num_rails_initialized].name.name,
                               "Sensors",
                               NPA_CLIENT_REQUIRED);
    SNS_ASSERT(NULL != pwr_rail_service.rails[pwr_rail_service.num_rails_initialized].rail_npa_handle);
  }
  else
  {
    pwr_rail_service.rails[pwr_rail_service.num_rails_initialized].rail_npa_handle = NULL;
  }

  // Increment number of initialized rails.
  pwr_rail_service.num_rails_initialized++;

  return SNS_RC_SUCCESS;
}

/**
 * See sns_pwr_rail_service.h
 *
 * TODO: The NPA services that we use for power rail voting do
 * not reside in island mode. Need wrapper API or island exit
 * calls at appropriate places in this manager.
 */
SNS_SECTION(".text.sns") sns_rc sns_vote_power_rail_update(sns_pwr_rail_service *this,
                                  struct sns_sensor const* sensor,
                                  sns_rail_config const* rails_config,
                                  sns_time* rail_on_timestamp)
{
  UNUSED_VAR(this);
  int i;
  uint8_t rail_index = 0;
  bool rail_is_registered = false;
  sns_time on_timestamp = 0;
  sns_rc rc = SNS_RC_SUCCESS;
  SNS_ISLAND_EXIT();

  sns_osa_lock_acquire(pwr_rail_service.rail_mgr_lock);

  for(i = 0; i < rails_config->num_of_rails; i++)
  {
     rail_is_registered = sns_rail_is_registered(&rails_config->rails[i], &rail_index);

     if(!rail_is_registered)
     {
       rc =  SNS_RC_NOT_SUPPORTED;
       break;
     }

     if(rails_config->rail_vote == SNS_RAIL_OFF)
     {
       sns_remove_pwr_rail_client(sensor, rail_index);
     }
     else
     {
       sns_add_pwr_rail_client(sensor, rails_config->rail_vote, rail_index);
     }

     // Update the power rail state. Not required for dummy rail
     if(0 != strncmp(DUMMY_SENSOR_VDD, pwr_rail_service.rails[rail_index].name.name, sizeof(DUMMY_SENSOR_VDD)))
     {
       sns_update_power_rail_state(rail_index);
     }

     /** There can be more than one rail that a client wants to
      *  turn ON. Using max operation to get timestamp of rail that
      *  was turned ON most recently so that the client can know
      *  exactly how long it needs until HW is available. */
     on_timestamp = SNS_MAX(on_timestamp, sns_get_rail_on_timestamp(rail_index));
  }

  if((SNS_RC_SUCCESS == rc) && (rail_on_timestamp != NULL))
  {
    *rail_on_timestamp = on_timestamp;
  }

  sns_osa_lock_release(pwr_rail_service.rail_mgr_lock);

  return rc;
}

/**
 * See sns_pwr_rail_service.h
 *
 * The sns_rail_config:rails list is used in this function to
 * register rail names with the power raill manager.
 */
SNS_SECTION(".text.sns") sns_rc sns_register_power_rails(sns_pwr_rail_service *this,
                                sns_rail_config const* rail_config)
{
  UNUSED_VAR(this);
  int i;
  sns_rc rc = SNS_RC_SUCCESS;
  SNS_ISLAND_EXIT();

  sns_osa_lock_acquire(pwr_rail_service.rail_mgr_lock);

  if(rail_config == NULL)
  {
    rc =  SNS_RC_INVALID_VALUE;
  }
  else
  {
    for(i = 0; i< rail_config->num_of_rails; i++)
    {
      rc = sns_pwr_rail_init(&rail_config->rails[i]);
      // if even a single rail init fails then return immediately.
      if(rc != SNS_RC_SUCCESS)
      {
        break;
      }
    }
  }

  sns_osa_lock_release(pwr_rail_service.rail_mgr_lock);
  return rc;
}

/**
 * Power rail manager API.
 */
static sns_pwr_rail_service_api  pwr_rail_service_api SNS_SECTION(".data.sns") =
{
  .struct_len = sizeof(sns_pwr_rail_service_api),
  .sns_register_power_rails = &sns_register_power_rails,
  .sns_vote_power_rail_update = &sns_vote_power_rail_update
};

/**
 * See sns_fw_pwr_rail_service.h
 */
sns_fw_pwr_rail_service* sns_pwr_rail_service_init(void)
{
  sns_osa_lock_attr   lock_attr;
  sns_rc return_code = SNS_RC_SUCCESS;

  pwr_rail_service.service.api = &pwr_rail_service_api;
  pwr_rail_service.service.service.type = SNS_POWER_RAIL_SERVICE;
  pwr_rail_service.service.service.version = sns_pwr_rail_service_version;
  pwr_rail_service.sleep_set = true;

  /** Initialize rail_mgr_lock. */
  sns_osa_lock_attr_init(&lock_attr);
  sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  return_code = sns_osa_lock_create(&lock_attr, &pwr_rail_service.rail_mgr_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == return_code);

  /** Disable island mode */
  pwr_rail_service.island_client = sns_island_aggregator_register_client("power rail service");
  sns_island_block(pwr_rail_service.island_client);

  /** Register power rail service with SCPM manager **/
  pwr_rail_service.pwr_client = sns_pwr_scpm_mgr_create_client();

  return &pwr_rail_service;
}


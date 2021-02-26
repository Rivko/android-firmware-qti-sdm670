/******************************************************************************
                          L Q E . C

******************************************************************************/

/******************************************************************************

  @file    lqe.c
  @brief   LTE downlink quality estimation

  DESCRIPTION
  Implementation of wrapper over a bunch of QMI APIs which
  provide LTE quality estimation data to its clients.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#include <stdlib.h>
#include <assert.h>
#include "lqe.h"
#include "lqei.h"
#include "lqe_cmdq.h"
#include "lqe_qmi_ott.h"
#include "lqe_qmi_wds.h"

#include "ds_util.h"
#include "qmi_client.h"

static boolean lqe_client_initialized = FALSE;
struct lqe_client_config lqe_config;
static lqe_qmi_service_type lqe_qmi_service = LQE_QMI_SERVICE_NONE;

#define LQE_LOCK_CLIENT()                                   \
        if( pthread_mutex_lock( &lqe_config.mutex ) < 0) {  \
          LQE_LOG_ERROR("lock mutex failed");               \
          goto bail;                                        \
        }

#define LQE_UNLOCK_CLIENT()                                  \
        if( pthread_mutex_unlock( &lqe_config.mutex ) < 0) { \
          LQE_LOG_ERROR("unlock mutex failed");              \
          goto bail;                                         \
        }

#define LQE_DEFAULT_MASK       0xFEEB0000
#define LQE_HANDLE_ID_MASK     0x0000FFFF
#define LQE_HANDLE_TOKEN_MASK  0xFFFF0000

#define LQE_MAKE_HNDL(id)                              \
        (LQE_DEFAULT_MASK | (id & LQE_HANDLE_ID_MASK))

#define CLIENT_GET_ID_FROM_HNDL(hndl)                  \
        (hndl & LQE_HANDLE_ID_MASK)

#define LQE_IS_INDEX_VALID(id)                          \
        (LQE_CLIENT_MAX > id &&                         \
         lqe_config.clients_info[id].is_valid)

#define LQE_IS_HANDLE_VALID(hndl)                                          \
        ((LQE_DEFAULT_MASK == (hndl & LQE_HANDLE_TOKEN_MASK)) &&       \
         (LQE_IS_INDEX_VALID(CLIENT_GET_ID_FROM_HNDL(hndl))))

typedef void (*lqe_qmi_release_client_fptr)(int index);
typedef int (*lqe_qmi_set_tput_report_period_fptr)(int lqe_client_handle, lqe_data_direction_e direction, int report_interval_timer);
typedef int (*lqe_qmi_toggle_tput_indications_fptr)(int lqe_client_handle, lqe_data_direction_e direction, lqe_indications_switch_e ind_switch);
typedef int (*lqe_qmi_get_downlink_throughput_report_status_fptr)(int lqe_client_handle, lqe_downlink_throughput_report_status *status);
typedef int (*lqe_qmi_get_downlink_throughput_info_fptr)(int lqe_client_handle, int *dl_allowed_rate, int *confidence_level);

typedef struct lqe_qmi_vtable_s
{
  lqe_qmi_release_client_fptr                         lqe_qmi_release_client;
  lqe_qmi_set_tput_report_period_fptr                 lqe_qmi_set_tput_report_period;
  lqe_qmi_toggle_tput_indications_fptr                lqe_qmi_toggle_tput_indications;
  lqe_qmi_get_downlink_throughput_report_status_fptr  lqe_qmi_get_dl_tput_report_status;
  lqe_qmi_get_downlink_throughput_info_fptr           lqe_qmi_get_dl_tput_info;
} lqe_qmi_vtable_t;

lqe_qmi_vtable_t lqe_qmi_vtbl =
{
  .lqe_qmi_release_client             = lqe_release_ott_client,
  .lqe_qmi_set_tput_report_period     = lqe_ott_set_throughput_report_period,
  .lqe_qmi_toggle_tput_indications    = lqe_ott_toggle_throughput_indications,
  .lqe_qmi_get_dl_tput_report_status  = lqe_ott_get_downlink_throughput_report_status,
  .lqe_qmi_get_dl_tput_info           = lqe_ott_get_downlink_throughput_info,
};

static boolean lqe_rave_enabled = FALSE;
#define LQE_GET_UL_TP_INFO            lqe_ott_get_uplink_throughput_info
#define LQE_GET_UL_TP_REPORT_STATUS   lqe_ott_get_uplink_throughput_report_status

/*===========================================================================
  FUNCTION  lqe_process_ott_ind_cb
===========================================================================*/
/*!
@brief
  Function called from lqe ott to report asynchronous indications

@return
  int - LQE_SUCCESS on successful operation, LQE_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static void lqe_process_ott_ind_cb
(
  int                        index,
  int                        event,
  lqe_ott_ind_event_info_t  *info
)
{
  lqe_client_t *client_ptr = NULL;
  lqe_ind_event_info_t ind_data;

  LQE_LOG_ENTRY;

  if(lqe_qmi_service != LQE_QMI_SERVICE_OTT)
  {
    LQE_LOG_ERROR("%s(): LQE is using [0x%x] QMI service. Dropping ind",
                  __func__, lqe_qmi_service);
    goto bail;
  }

  if (!info || !LQE_IS_INDEX_VALID(index))
  {
    LQE_LOG_ERROR("%s(): Invalid info received. info [%p], index [%d]",
                  __func__, info, index);
    goto bail;
  }

  memset(&ind_data, 0, sizeof(lqe_ind_event_info_t));

  LQE_LOCK_CLIENT();

  switch(event)
  {
    case LQE_DOWNLINK_THROUGHPUT_IND_EV:
      {
        LQE_LOG_DEBUG("%s(): process LQE_DOWNLINK_THROUGHPUT_IND_EV, index [%d]", __func__, index);

        ind_data.dl_throughput_info.downlink_allowed_rate_valid = \
                      info->dl_throughput_info.downlink_rate_valid;
        ind_data.dl_throughput_info.downlink_allowed_rate = \
                      info->dl_throughput_info.downlink_rate;

        ind_data.dl_throughput_info.confidence_level_valid = \
                      info->dl_throughput_info.confidence_level_valid;
        ind_data.dl_throughput_info.confidence_level = info->dl_throughput_info.confidence_level;

        ind_data.dl_throughput_info.is_suspended_valid = \
                      info->dl_throughput_info.is_suspended_valid;
        ind_data.dl_throughput_info.is_suspended = info->dl_throughput_info.is_suspended;

        if (lqe_config.clients_info[index].is_valid)
        {
          if (lqe_config.clients_info[index].cb_f)
          {
            lqe_config.clients_info[index].cb_f(LQE_MAKE_HNDL(index),
                                                event,
                                                &ind_data,
                                                lqe_config.clients_info[index].data);
          }
          else
          {
            LQE_LOG_DEBUG("%s(): There is no registered callback", __func__);
          }
        }
      }
      break;

    case LQE_UPLINK_THROUGHPUT_IND_EV:
      {
        LQE_LOG_DEBUG("%s(): process LQE_UPLINK_THROUGHPUT_IND_EV, index [%d]", __func__, index);

        ind_data.ul_throughput_info.uplink_allowed_rate_valid = \
                      info->ul_throughput_info.uplink_rate_valid;
        ind_data.ul_throughput_info.uplink_allowed_rate = \
                      info->ul_throughput_info.uplink_rate;

        ind_data.ul_throughput_info.confidence_level_valid = \
                      info->ul_throughput_info.confidence_level_valid;
        ind_data.ul_throughput_info.confidence_level = info->ul_throughput_info.confidence_level;

        if (lqe_config.clients_info[index].is_valid)
        {
          if (lqe_config.clients_info[index].cb_f)
          {
            lqe_config.clients_info[index].cb_f(LQE_MAKE_HNDL(index),
                                                event,
                                                &ind_data,
                                                lqe_config.clients_info[index].data);
          }
          else
          {
            LQE_LOG_DEBUG("%s(): There is no registered callback", __func__);
          }
        }
      }
      break;

    default:
      {
        LQE_LOG_DEBUG("%s(): Unkown event receieved [0x%X]", __func__, event);
      }
      break;
  }

  LQE_UNLOCK_CLIENT();

bail:

  LQE_LOG_EXIT;
}

/*===========================================================================
  FUNCTION  lqe_process_wds_ind_cb
===========================================================================*/
/*!
@brief
  Function called from lqe wds to report asynchronous indications

@return
  int - LQE_SUCCESS on successful operation, LQE_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static void lqe_process_wds_ind_cb
(
  int                          index,
  int                          event,
  lqe_wds_ind_event_info_t    *info
)
{
  lqe_client_t *client_ptr = NULL;
  lqe_ind_event_info_t ind_data;

  LQE_LOG_ENTRY;

  if(lqe_qmi_service != LQE_QMI_SERVICE_WDS)
  {
    LQE_LOG_ERROR("%s(): LQE is using [0x%x] QMI service. Dropping ind",
                  __func__, lqe_qmi_service);
    goto bail;
  }

  if (!info || !LQE_IS_INDEX_VALID(index))
  {
    LQE_LOG_ERROR("%s(): Invalid info received. info [%p], index [%d]",
                  __func__, info, index);
    goto bail;
  }

  memset(&ind_data, 0, sizeof(lqe_ind_event_info_t));

  LQE_LOCK_CLIENT();

  switch(event)
  {
    case LQE_DOWNLINK_THROUGHPUT_IND_EV:
      {
        LQE_LOG_DEBUG("%s(): process LQE_DOWNLINK_THROUGHPUT_IND_EV, index [%d]", __func__, index);

        ind_data.dl_throughput_info.downlink_allowed_rate_valid = \
                      info->dl_throughput_info.downlink_allowed_rate_valid;
        ind_data.dl_throughput_info.downlink_allowed_rate = \
                      info->dl_throughput_info.downlink_allowed_rate;

        ind_data.dl_throughput_info.confidence_level_valid = \
                      info->dl_throughput_info.confidence_level_valid;
        ind_data.dl_throughput_info.confidence_level = info->dl_throughput_info.confidence_level;

        ind_data.dl_throughput_info.is_suspended_valid = \
                      info->dl_throughput_info.is_suspended_valid;
        ind_data.dl_throughput_info.is_suspended = info->dl_throughput_info.is_suspended;

        if (lqe_config.clients_info[index].is_valid)
        {
          if (lqe_config.clients_info[index].cb_f)
          {
            lqe_config.clients_info[index].cb_f(LQE_MAKE_HNDL(index),
                                                event,
                                                &ind_data,
                                                lqe_config.clients_info[index].data);
          }
          else
          {
            LQE_LOG_DEBUG("%s(): There is no registered callback", __func__);
          }
        }
      }
      break;
    default:
      {
        LQE_LOG_DEBUG("%s(): Unkown event receieved [0x%X]", __func__, event);
      }
      break;
  }

  LQE_UNLOCK_CLIENT();

bail:

  LQE_LOG_EXIT;
}

/*===========================================================================
  FUNCTION  lqe_allocate_client_helper
===========================================================================*/
/*!
@brief
  Helper function to allocate clients of this library. This function should
  striclty called with this lock held LQE_LOCK_CLIENT()

@return
  int - LQE_SUCCESS on successful operation, LQE_FAILURE otherwise

@note
  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static int lqe_allocate_client_helper
(
  lqe_client_t       **client_pptr,
  unsigned int        *index_ptr
)
{
  unsigned int index = 0;

  *client_pptr = NULL;

  for( index=0; index < LQE_CLIENT_MAX; index++ )
  {
    if( !lqe_config.clients_info[index].is_valid ) {
      memset(&lqe_config.clients_info[index], 0, sizeof(lqe_client_t));
      lqe_config.clients_info[index].is_valid = TRUE;
      lqe_config.clients_info[index].lqe_params.is_valid = FALSE;
      *index_ptr = index;
      *client_pptr = &lqe_config.clients_info[index];
      break;
    }
  }

  return (*client_pptr)? LQE_SUCCESS : LQE_FAILURE;
}

/*===========================================================================
  FUNCTION  lqe_client_release_internal
===========================================================================*/
static inline void lqe_client_release_internal
(
  lqe_client_t *client_ptr
)
{
  memset( client_ptr, 0x0, sizeof(lqe_client_t));
}

/*===========================================================================
  FUNCTION  lqei_client_release
===========================================================================*/
/*!
@brief
  This function registers a client with the LQE library and returns a handle
  to client as a result. The client is supposed to use this handle with all
  the other APIs. Users can create multiple clients with a max of 5 clients.

@param client_hndl: A valid handle to the library on successful return

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_client_release
(
  lqe_client_hdl_t       client_handle
)
{
  unsigned int index = 0;
  lqe_client_t *client_ptr = NULL;
  int ret = LQE_FAILURE;

  LQE_LOG_ENTRY;

  if (!LQE_IS_HANDLE_VALID(client_handle))
  {
    LQE_LOG_ERROR("%s(): Invalid handle passed", __func__);
    goto bail;
  }

  LQE_LOCK_CLIENT();

  index =  CLIENT_GET_ID_FROM_HNDL(client_handle);

  client_ptr = &lqe_config.clients_info[index];

  LQE_LOG_INFO("%s(): releasing lqe client with index:%d", __func__, index);

  lqe_qmi_vtbl.lqe_qmi_release_client(index);

  lqe_client_release_internal(client_ptr);

  LQE_LOG_INFO("%s(): released lqe client with index:%d\n", __func__, index);

  LQE_UNLOCK_CLIENT();

  ret = LQE_SUCCESS;

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqei_qmi_client_init
===========================================================================*/
/*!
@brief
  This function does a fallback mechanism first to check if Modem supports
  OTT QMI Service and if it doesn't then WDS QMI service will be used. This
  happens only during the first LQE register per process and caches the QMI
  service for further lqe_client_init.

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

*/
/*=========================================================================*/
static int
lqei_qmi_client_init
(
  int index
)
{
  int ret = LQE_FAILURE;
  ds_target_t target;
  const char *target_str;

  target = ds_get_target();
  target_str = ds_get_target_str(target);

  LQE_LOG_DEBUG("%s(): target: [%d]: [%s]", __func__, target, target_str);

  if (target == DS_TARGET_MSM8998 ||
      target == DS_TARGET_STARLORD ||
      target == DS_TARGET_STARHAWK ||
      target == DS_TARGET_WARLOCK ||
      target == DS_TARGET_MSMNILE ||
      target == DS_TARGET_SDM845)
  {
    lqe_rave_enabled = TRUE;
  }

  if(lqe_qmi_service == LQE_QMI_SERVICE_NONE)
  {
    if (lqe_rave_enabled == TRUE)
    {
      /*  In lqe_init_ott_client, we try to check if OTT QMI service is
          present on the modem using qmi_client_get_service_instance.
          Unfortunately, this api will fail even if modem is not up and
          is not easy to differentiate between failure due to MDM not
          up and QMI service not present.

          As a workaround, we try to init wds client which is a blocking call
          until modem comes up and once it succeeds, we go ahead with ott_init.
          This dummy wds client is then released immediately. */

      lqe_int_wds_client(index);
      lqe_release_wds_client(index);

      if(LQE_SUCCESS == (ret = lqe_int_ott_client(index)))
      {
        lqe_qmi_service = LQE_QMI_SERVICE_OTT;
      }
    }
    else
    {
      if(LQE_SUCCESS == (ret = lqe_int_wds_client(index)))
      {
        lqe_qmi_service = LQE_QMI_SERVICE_WDS;

        /* Change function pointers in lqe_qmi_vtbl to point to WDS service funcs */
        lqe_qmi_vtbl.lqe_qmi_release_client             = lqe_release_wds_client;
        lqe_qmi_vtbl.lqe_qmi_set_tput_report_period     = lqe_wds_set_throughput_report_period;
        lqe_qmi_vtbl.lqe_qmi_toggle_tput_indications    = lqe_wds_toggle_throughput_indications;
        lqe_qmi_vtbl.lqe_qmi_get_dl_tput_report_status  = lqe_wds_get_downlink_throughput_report_status;
        lqe_qmi_vtbl.lqe_qmi_get_dl_tput_info           = lqe_wds_get_downlink_throughput_info;
      }
    }
  }
  else if(lqe_qmi_service == LQE_QMI_SERVICE_OTT)
  {
    ret = lqe_int_ott_client(index);
  }
  else
  {
    ret = lqe_int_wds_client(index);
  }

  return ret;
}

/*===========================================================================
  FUNCTION  lqei_client_register
===========================================================================*/
/*!
@brief
  This function registers a client with the LQE library and returns a handle
  to client as a result. The client is supposed to use this handle with all
  the other APIs. Users can create multiple clients with a max of 5 clients.

@param cb_f: client callback to receive indications from modem.
@param data: cookie data
@param client_hndl: A valid handle to the library on successful return

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_client_register
(
  lqei_ind_f              cb_f,
  void                   *data,
  lqe_client_hdl_t       *client_handle
)
{
  lqe_client_t *client_ptr = NULL;
  int ret = LQE_FAILURE;
  unsigned int index = 0;

  LQE_LOG_ENTRY;

  if (!cb_f || !client_handle)
  {
    LQE_LOG_ERROR("%s, BAD PARAMS received", __func__);
    goto bail;
  }

  LQE_LOCK_CLIENT();

  if (LQE_SUCCESS != lqe_allocate_client_helper(&client_ptr,
                                                &index))
  {
    LQE_LOG_ERROR("%s:failed to allocate a client object", __func__);
    LQE_UNLOCK_CLIENT();
    goto bail;
  }

  LQE_LOG_DEBUG("%s: using index:%d", __func__, index);

  if (client_ptr)
  {
    client_ptr->cb_f = cb_f;
    client_ptr->data = data;
  }

  if (LQE_SUCCESS != lqei_qmi_client_init(index))
  {
    LQE_LOG_ERROR("%s:Unable to allocate a QMI OTT client ", __func__);
    lqe_client_release_internal(client_ptr);
    LQE_UNLOCK_CLIENT();
    goto bail;
  }

  ret = LQE_SUCCESS;

  *client_handle = LQE_MAKE_HNDL(index);

  LQE_UNLOCK_CLIENT();

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqei_get_min_downlink_throughput_frequency
===========================================================================*/
/*!
@brief
 This function queries the modem for the minimum downlink interval timer.
 The returned value in @param min_dl_interval is in milliseconds. This
 value specifies the minimum interval in which the downlink throughput
 indications are generated by the modem. These indications are received
 via the client registered callback function.

@param client_handle: handle obtained from lqe_client_register()
@param min_dl_interval: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_get_min_downlink_throughput_frequency
(
  lqe_client_hdl_t                        client_handle,
  lqe_downlink_throughput_report_status  *status
)
{
  int ret = LQE_FAILURE;
  lqe_client_t *client_ptr = NULL;
  unsigned int index = 0;

  LQE_LOG_ENTRY;

  if (!status)
  {
    LQE_LOG_ERROR("%s(): bad params passed", __func__);
    goto bail;
  }

  if (!LQE_IS_HANDLE_VALID(client_handle))
  {
    LQE_LOG_ERROR("%s(): Invalid handle passed", __func__);
    goto bail;
  }

  index =  CLIENT_GET_ID_FROM_HNDL(client_handle);
  client_ptr = &lqe_config.clients_info[index];

  if (LQE_SUCCESS != lqe_qmi_vtbl.lqe_qmi_get_dl_tput_report_status(index, status))
  {
    LQE_LOG_ERROR("%s(): failed to get downlink throughput report status", __func__);
    goto bail;
  }

  if(status->reporting_status_valid)
  {
    LQE_LOG_INFO("%s(): reporting_status [%d]", __func__, status->reporting_status);
  }
  if(status->actual_interval_valid)
  {
    LQE_LOG_INFO("%s(): actual_interval [%u]", __func__, status->actual_interval);
  }

  ret = LQE_SUCCESS;

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqe_toggle_registration_downlink_throughput_indications
===========================================================================*/
static int
lqe_toggle_registration_downlink_throughput_indications
(
  lqe_client_hdl_t            client_handle,
  lqe_indications_switch_e    ind_switch
)
{
  lqe_client_t *client_ptr = NULL;
  unsigned int index = 0;

  if (!LQE_IS_HANDLE_VALID(client_handle))
  {
    LQE_LOG_ERROR("%s,Invalid handle passed", __func__);
    return LQE_FAILURE;
  }

  index =  CLIENT_GET_ID_FROM_HNDL(client_handle);
  client_ptr = &lqe_config.clients_info[index];

  if (LQE_SUCCESS == lqe_qmi_vtbl.lqe_qmi_toggle_tput_indications(index,
                                                                  LQE_DATA_DOWNLINK_DIRECTION,
                                                                   ind_switch))
  {
    return LQE_SUCCESS;
  }

  return LQE_FAILURE;
}

/*===========================================================================
  FUNCTION  lqei_set_downlink_throughput_indication_frequency
===========================================================================*/
/*!
@brief
  This function registers for downlink throughput indications from the modem.
  It sends a request to the modem to specify the interval at which the
  downlink throughput information is reported. This minimum timer
  interval can be queried using  lqei_get_min_downlink_throughput_frequency().
  The rate at which the indications are generated is an integral multiple
  of @param report_interval_timer and @param min_dl_interval returned in
  lqei_get_min_downlink_throughput_frequency().

  For Example if @param min_dl_interval returned 512 ms and user specifies
  @param report_interval_timer of 2, then the downlink throughput indications
  are generated every 2 * 512 = 1024 ms while the radio is connected.

  A @param indications value of 0 disables the timer on the modem
  and no  more indications are received. By default and in the absence of
  explicit request using this API, the indications are disabled. @param value
  of 1 will enable indications. Even if indications is 0, the current
  downlink rate and confidence level can be queried using
  lqei_get_downlink_throughput_info

  If multiple clients request for this indication using this API, then the
  minimum of the all the timers requested will be used by the modem to send
  the indications.

  A non zero frequency needs to be set to get the correct throughput information.
  An error is returned if the frequency is set to 0.

@param client_handle: handle obtained from lqe_client_register()
@param report_interval_timer: see description above
@param indications: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - This function triggers the modem to start generating indications
    as per user request.
*/
/*=========================================================================*/
int
lqei_set_downlink_throughput_indication_frequency
(
  lqe_client_hdl_t            client_handle,
  int                         report_interval_timer,
  lqe_indications_switch_e    indications
)
{
  lqe_client_t *client_ptr = NULL;
  unsigned int index = 0;
  int ret = LQE_FAILURE;

  LQE_LOG_ENTRY;

  if (!LQE_IS_HANDLE_VALID(client_handle))
  {
    LQE_LOG_ERROR("%s,Invalid handle passed", __func__);
    goto bail;
  }

  index =  CLIENT_GET_ID_FROM_HNDL(client_handle);
  client_ptr = &lqe_config.clients_info[index];

  if (LQE_SUCCESS == (ret = lqe_qmi_vtbl.lqe_qmi_set_tput_report_period(index,
                                                                        LQE_DATA_DOWNLINK_DIRECTION,
                                                                        report_interval_timer)))
  {
    if (indications == LQE_TURN_ON_INDICATIONS)
    {
      LQE_LOG_DEBUG("%s: turning on downlink throughput indications on "
                 "client handle:%d", __func__, index);
    }
    else
    {
      LQE_LOG_DEBUG("%s: turning off downlink throughput indications on "
                 "client handle:%d", __func__, index);
    }
    if (LQE_SUCCESS == (ret = lqe_toggle_registration_downlink_throughput_indications(client_handle,
                                                                                      indications)))
    {
      LQE_LOCK_CLIENT();

      /* Cache the lqe params to re-trigger after SSR */
      client_ptr->lqe_params.dl_report_interval = report_interval_timer;
      client_ptr->lqe_params.dl_ind_switch = indications;

      if(client_ptr->lqe_params.dl_report_interval <= 0
         && client_ptr->lqe_params.ul_report_interval <= 0)
      {
        /* report_interval_timer of <= 0 will STOP LQE */
        client_ptr->lqe_params.is_valid = FALSE;
      }
      else
      {
        client_ptr->lqe_params.is_valid = TRUE;
      }

      LQE_UNLOCK_CLIENT();
    }
  }

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqei_get_downlink_throughput_info
===========================================================================*/
/*!
@brief
 This function queries the modem for the current downlink rate and the
 confidence level. This is a synchronous call.
 The returned value in @param dl_rate is the current downlink rate in Kbps.
 The returned value in @param confidence_level is the current confidence
 level. It gives the degree of accuracy at which the current downlink
 rate is generated. The accuracy is on a scale of 0-7, 0 being the least
 accurate while 7 being the most accurate.

 The downlink throughput indication frequency needs to be set for this
 API to work. A non zero frequency needs to be set to get the correct
 throughput information. An error is returned if the frequency is set to 0.

@param client_handle: handle obtained from lqe_client_register()
@param dl_rate: see description above
@param confidence_level: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_get_downlink_throughput_info
(
  lqe_client_hdl_t       client_handle,
  int                    *dl_rate,
  int                    *confidence_level
)
{
  lqe_client_t *client_ptr = NULL;
  unsigned int index = 0;
  int ret = LQE_FAILURE;

  LQE_LOG_ENTRY;

  if ((!dl_rate) || (!confidence_level))
  {
    LQE_LOG_ERROR("%s(): bad params passed", __func__);
    goto bail;
  }

  if (!LQE_IS_HANDLE_VALID(client_handle))
  {
    LQE_LOG_ERROR("%s(): Invalid handle passed", __func__);
    goto bail;
  }

  index =  CLIENT_GET_ID_FROM_HNDL(client_handle);
  client_ptr = &lqe_config.clients_info[index];

  if (LQE_SUCCESS == (ret = lqe_qmi_vtbl.lqe_qmi_get_dl_tput_info(index,
                                                                  dl_rate,
                                                                  confidence_level)))
  {
    LQE_LOG_INFO("%s(): dl_rate is:%d confidence_level is %d",
               __func__,
               *dl_rate,
               *confidence_level);
  }

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqei_get_min_uplink_throughput_frequency
===========================================================================*/
/*!
@brief
 This function queries the modem for the minimum uplink interval timer.
 The returned value in @param min_ul_interval is in milliseconds. This
 value specifies the minimum interval in which the uplink throughput
 indications are generated by the modem. These indications are received
 via the client registered callback function.

@param client_hanule: hanule obtained from lqe_client_register()
@param min_ul_interval: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_get_min_uplink_throughput_frequency
(
  lqe_client_hdl_t                        client_hanule,
  lqe_uplink_throughput_report_status  *status
)
{
  int ret = LQE_FAILURE;
  lqe_client_t *client_ptr = NULL;
  unsigned int index = 0;

  LQE_LOG_ENTRY;

  if(lqe_qmi_service != LQE_QMI_SERVICE_OTT)
  {
    LQE_LOG_ERROR("%s(): Uplink not supported", __func__);
    goto bail;
  }

  if (!status)
  {
    LQE_LOG_ERROR("%s(): bad params passed", __func__);
    goto bail;
  }

  if (!LQE_IS_HANDLE_VALID(client_hanule))
  {
    LQE_LOG_ERROR("%s(): Invalid hanule passed", __func__);
    goto bail;
  }

  index =  CLIENT_GET_ID_FROM_HNDL(client_hanule);
  client_ptr = &lqe_config.clients_info[index];

  if (LQE_SUCCESS != (LQE_GET_UL_TP_REPORT_STATUS(index, status)))
  {
    LQE_LOG_ERROR("%s(): failed to get uplink throughput report status", __func__);
    goto bail;
  }

  if(status->reporting_status_valid)
  {
    LQE_LOG_INFO("%s(): reporting_status [%d]", __func__, status->reporting_status);
  }
  if(status->actual_interval_valid)
  {
    LQE_LOG_INFO("%s(): actual_interval [%u]", __func__, status->actual_interval);
  }

  ret = LQE_SUCCESS;

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqe_toggle_registration_uplink_throughput_indications
===========================================================================*/
static int
lqe_toggle_registration_uplink_throughput_indications
(
  lqe_client_hdl_t            client_hanule,
  lqe_indications_switch_e    ind_switch
)
{
  lqe_client_t *client_ptr = NULL;
  unsigned int index = 0;

  if (!LQE_IS_HANDLE_VALID(client_hanule))
  {
    LQE_LOG_ERROR("%s,Invalid hanule passed", __func__);
    return LQE_FAILURE;
  }

  index =  CLIENT_GET_ID_FROM_HNDL(client_hanule);
  client_ptr = &lqe_config.clients_info[index];

  if (LQE_SUCCESS == (lqe_qmi_vtbl.lqe_qmi_toggle_tput_indications(index,
                                                                    LQE_DATA_UPLINK_DIRECTION,
                                                                    ind_switch)))
  {
    return LQE_SUCCESS;
  }

  return LQE_FAILURE;
}

/*===========================================================================
  FUNCTION  lqei_set_uplink_throughput_indication_frequency
===========================================================================*/
/*!
@brief
  This function registers for uplink throughput indications from the modem.
  It sends a request to the modem to specify the interval at which the
  uplink throughput information is reported. This minimum timer
  interval can be queried using  lqei_get_min_uplink_throughput_frequency().
  The rate at which the indications are generated is an integral multiple
  of @param report_interval_timer and @param min_ul_interval returned in
  lqei_get_min_uplink_throughput_frequency().

  For Example if @param min_ul_interval returned 512 ms and user specifies
  @param report_interval_timer of 2, then the uplink throughput indications
  are generated every 2 * 512 = 1024 ms while the radio is connected.

  A @param indications value of 0 disables the timer on the modem
  and no  more indications are received. By default and in the absence of
  explicit request using this API, the indications are disabled. @param value
  of 1 will enable indications. Even if indications is 0, the current
  uplink rate and confidence level can be queried using
  lqei_get_uplink_throughput_info

  If multiple clients request for this indication using this API, then the
  minimum of the all the timers requested will be used by the modem to send
  the indications.

  A non zero frequency needs to be set to get the correct throughput information.
  An error is returned if the frequency is set to 0.

@param client_hanule: hanule obtained from lqe_client_register()
@param report_interval_timer: see description above
@param indications: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - This function triggers the modem to start generating indications
    as per user request.
*/
/*=========================================================================*/
int
lqei_set_uplink_throughput_indication_frequency
(
  lqe_client_hdl_t            client_hanule,
  int                         report_interval_timer,
  lqe_indications_switch_e    indications
)
{
  lqe_client_t *client_ptr = NULL;
  unsigned int index = 0;
  int ret = LQE_FAILURE;

  LQE_LOG_ENTRY;

  if(lqe_qmi_service != LQE_QMI_SERVICE_OTT)
  {
    LQE_LOG_ERROR("%s(): Uplink not supported", __func__);
    goto bail;
  }

  if (!LQE_IS_HANDLE_VALID(client_hanule))
  {
    LQE_LOG_ERROR("%s,Invalid hanule passed", __func__);
    goto bail;
  }

  index =  CLIENT_GET_ID_FROM_HNDL(client_hanule);
  client_ptr = &lqe_config.clients_info[index];

  if (LQE_SUCCESS == (ret = lqe_qmi_vtbl.lqe_qmi_set_tput_report_period(index,
                                                                        LQE_DATA_UPLINK_DIRECTION,
                                                                        report_interval_timer)))
  {
    if (indications == LQE_TURN_ON_INDICATIONS)
    {
      LQE_LOG_DEBUG("%s: turning on uplink throughput indications on "
                 "client hanule:%d", __func__, index);
    }
    else
    {
      LQE_LOG_DEBUG("%s: turning off uplink throughput indications on "
                 "client hanule:%d", __func__, index);
    }
    if (LQE_SUCCESS == (ret = lqe_toggle_registration_uplink_throughput_indications(client_hanule,
                                                                                      indications)))
    {
      LQE_LOCK_CLIENT();

      /* Cache the lqe params to re-trigger after SSR */
      client_ptr->lqe_params.ul_report_interval = report_interval_timer;
      client_ptr->lqe_params.ul_ind_switch = indications;

      if(client_ptr->lqe_params.dl_report_interval <= 0
         && client_ptr->lqe_params.ul_report_interval <= 0)
      {
        /* report_interval_timer of <= 0 will STOP LQE */
        client_ptr->lqe_params.is_valid = FALSE;
      }
      else
      {
        client_ptr->lqe_params.is_valid = TRUE;
      }

      LQE_UNLOCK_CLIENT();
    }
  }

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqei_get_uplink_throughput_info
===========================================================================*/
/*!
@brief
 This function queries the modem for the current uplink rate and the
 confidence level. This is a synchronous call.
 The returned value in @param ul_rate is the current uplink rate in Kbps.
 The returned value in @param confidence_level is the current confidence
 level. It gives the degree of accuracy at which the current uplink
 rate is generated. The accuracy is on a scale of 0-7, 0 being the least
 accurate while 7 being the most accurate.

 The uplink throughput indication frequency needs to be set for this
 API to work. A non zero frequency needs to be set to get the correct
 throughput information. An error is returned if the frequency is set to 0.

@param client_hanule: hanule obtained from lqe_client_register()
@param ul_rate: see description above
@param confidence_level: see description above

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_get_uplink_throughput_info
(
  lqe_client_hdl_t       client_hanule,
  int                    *ul_rate,
  int                    *confidence_level
)
{
  lqe_client_t *client_ptr = NULL;
  unsigned int index = 0;
  int ret = LQE_FAILURE;

  LQE_LOG_ENTRY;

  if(lqe_qmi_service != LQE_QMI_SERVICE_OTT)
  {
    LQE_LOG_ERROR("%s(): Uplink not supported", __func__);
    goto bail;
  }

  if ((!ul_rate) || (!confidence_level))
  {
    LQE_LOG_ERROR("%s(): bad params passed", __func__);
    goto bail;
  }

  if (!LQE_IS_HANDLE_VALID(client_hanule))
  {
    LQE_LOG_ERROR("%s(): Invalid hanule passed", __func__);
    goto bail;
  }

  index =  CLIENT_GET_ID_FROM_HNDL(client_hanule);
  client_ptr = &lqe_config.clients_info[index];

  if (LQE_SUCCESS == (ret = LQE_GET_UL_TP_INFO(index,
                                               ul_rate,
                                               confidence_level)))
  {
    LQE_LOG_INFO("%s(): ul_rate is:%d confidence_level is %d",
               __func__,
               *ul_rate,
               *confidence_level);
  }

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqei_init
===========================================================================*/
/*!
@brief
  Initialize and setup the library resources. Client process should only
  call this API once.

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_init
(
  void
)
{
  int ret = LQE_FAILURE;

  LQE_LOG_ENTRY;

  if (FALSE == lqe_client_initialized)
  {
#ifndef LQE_DATA_OFFTARGET
    Diag_LSM_Init(NULL);
#endif
    memset(&lqe_config, 0, sizeof(lqe_config));

    if( pthread_mutex_init(&lqe_config.mutex, NULL)  < 0 ) {
      LQE_LOG_ERROR("%s(): Mutex initialization failed", __func__);
      goto bail;
    }

    if (LQE_FAILURE == lqe_cmdq_init())
    {
      LQE_LOG_ERROR("%s(): Failed to initialize cmdq", __func__);
      goto bail;
    }

    lqe_client_initialized = TRUE;
  }
  else
  {
    LQE_LOG_ERROR("%s(): already initialized. Skipping", __func__);
  }

  ret = LQE_SUCCESS;

bail:
  LQE_LOG_EXIT_RET;
  return ret;
}

/*===========================================================================
  FUNCTION  lqei_release
===========================================================================*/
/*!
@brief
  De-Initialize and releases the library resources. Client process should
  only call this API once.

@return
  LQE_SUCCESS on successful operation
  LQE_FAILURE otherwise

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
int
lqei_release
(
  void
)
{
  int ret = LQE_FAILURE;

  LQE_LOG_ENTRY;

  if (TRUE == lqe_client_initialized)
  {
    memset(&lqe_config, 0, sizeof(lqe_config));

    if( pthread_mutex_destroy(&lqe_config.mutex)  < 0 ) {
      LQE_LOG_ERROR("%s(): Mutex destroy failed", __func__);
    }

    if (LQE_FAILURE == lqe_cmdq_deinit())
    {
      LQE_LOG_ERROR("%s(): Failed to de-initialize cmdq", __func__);
    }

#ifndef LQE_DATA_OFFTARGET
    LQE_LOG_ERROR("Diag LSM DeInit\n");
    Diag_LSM_DeInit();
#endif
    lqe_client_initialized = FALSE;
    ret = LQE_SUCCESS;
  }
  else
  {
    LQE_LOG_ERROR("%s(): never initialized. Skipping", __func__);
  }

  LQE_LOG_EXIT_RET;

  return ret;
}

/*===========================================================================
  FUNCTION  lqe_mdm_out_of_service_hdlr
===========================================================================*/
/*!
@brief
  Handler function for Modem OOS event. Releases all valid OTT clients

@return
  void

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
static void lqe_mdm_out_of_service_hdlr
(
  int index
)
{
  if(LQE_IS_INDEX_VALID(index))
  {
    LQE_LOG_INFO("%s(): releasing client at index [%d]", __func__, index);
    lqe_qmi_vtbl.lqe_qmi_release_client(index);
  }
  else
  {
    LQE_LOG_INFO("%s(): invalid index [%d], skipping", __func__, index);
  }
}

/*===========================================================================
  FUNCTION  lqe_mdm_in_service_hdlr
===========================================================================*/
/*!
@brief
  Handler function for Modem IS event. Re-init all valid OTT clients and
  re-issue the lqe set indication frequency from the cached params

@return
  void

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
static void lqe_mdm_in_service_hdlr
(
  int index
)
{
  if(LQE_IS_INDEX_VALID(index))
  {
    lqe_client_t *client_ptr = &lqe_config.clients_info[index];

    LQE_LOG_INFO("%s(): init client at index [%d]", __func__, index);
    lqei_qmi_client_init(index);

    if(client_ptr->lqe_params.is_valid)
    {
      LQE_LOG_INFO("%s(): Re-issue LQE after SSR", __func__);
      if(client_ptr->lqe_params.dl_report_interval > 0)
      {
        lqei_set_downlink_throughput_indication_frequency(LQE_MAKE_HNDL(index),
                                                          client_ptr->lqe_params.dl_report_interval,
                                                          client_ptr->lqe_params.dl_ind_switch);
      }
      if(client_ptr->lqe_params.ul_report_interval > 0)
      {
        lqei_set_downlink_throughput_indication_frequency(LQE_MAKE_HNDL(index),
                                                          client_ptr->lqe_params.ul_report_interval,
                                                          client_ptr->lqe_params.ul_ind_switch);
      }
    }
  }
  else
  {
    LQE_LOG_INFO("%s(): invalid index [%d], skipping", __func__, index);
  }
}

/*===========================================================================
  FUNCTION:  lqe_process_qmi_sys_ind
===========================================================================*/
/*!
@brief
  Processes QMI system indications

@return
  void

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
static void lqe_process_qmi_sys_ind
(
  lqe_qmi_sys_event_type event_id,
  void *user_data
)
{
  int index;

  (void *)user_data;

  switch(event_id)
  {
    case LQE_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND:

      if(lqe_config.modem_ssr_state == LQE_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND)
      {
        LQE_LOG_INFO("%s(): SSR OOS in progress... ignoring duplication call");
        return;
      }
      lqe_config.modem_ssr_state = LQE_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND;

      LQE_LOG_INFO("%s(): received LQE_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND", __func__);
      for( index=0; index < LQE_CLIENT_MAX; index++ )
      {
        lqe_mdm_out_of_service_hdlr(index);
      }
      break;

    case LQE_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND:
      LQE_LOG_INFO("%s(): received LQE_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND", __func__);

      /* Update the SSR state */
      lqe_config.modem_ssr_state = LQE_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND;

      for( index=0; index < LQE_CLIENT_MAX; index++ )
      {
        lqe_mdm_in_service_hdlr(index);
      }
      break;

    default:
      LQE_LOG_ERROR("%s(): unknown qmi sys event [%d]", __func__, event_id);
      break;
  }
}

/*===========================================================================
  FUNCTION:  lqe_process_cmd_cb
===========================================================================*/
/*!
@brief
  Processes cmdq events

@return
  void

@note
  - Side Effects
    - None
*/
/*=========================================================================*/
void lqe_process_cmd_cb
(
  struct ds_cmd_s *ds_cmd,
  void * data
)
{
  lqe_cmdq_cmd_type *cmd = (lqe_cmdq_cmd_type *)ds_cmd;

  (void)data;

  LQE_LOG_ENTRY;

  if (NULL == cmd)
  {
    LQE_LOG_ERROR("%s(): bad params\n", __func__);
    goto bail;
  }

  switch(cmd->event)
  {
    case LQE_CB_CMD_QMI_SYS_IND:
      lqe_process_qmi_sys_ind
        (
          cmd->cmd_data.qmi_sys_ind.event_id,
          cmd->cmd_data.qmi_sys_ind.user_data
        );
      break;

    case LQE_CB_CMD_QMI_OTT_IND:
      lqe_process_ott_ind_cb
        (
          cmd->lqe_handle,
          cmd->cmd_data.qmi_ott_ind.event_id,
          &cmd->cmd_data.qmi_ott_ind.ind_data
        );
      break;

    case LQE_CB_CMD_QMI_WDS_IND:
      lqe_process_wds_ind_cb
        (
          cmd->lqe_handle,
          cmd->cmd_data.qmi_wds_ind.event_id,
          &cmd->cmd_data.qmi_wds_ind.ind_data
        );
      break;

    default:
      LQE_LOG_INFO("%s(): Unknown LQE event [0x%X]", __func__, cmd->event);
      break;
  }

bail:
  LQE_LOG_EXIT;
}

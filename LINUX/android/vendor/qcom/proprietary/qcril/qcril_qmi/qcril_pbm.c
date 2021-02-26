/******************************************************************************
  @file    qcril_pbm.c
  @brief   qcril qmi - PB

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2009-2010 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <string.h>
#include <cutils/properties.h>
#include "qcril_log.h"
#include "qcril_pbm.h"
#include "qcril_cm_ss.h"
#include "qcril_qmi_nas.h"
#include "qcril_other.h"

#include "phonebook_manager_service_v01.h"
#include "qcril_qmi_client.h"
#include "qmi_ril_platform_dep.h"
#include "qcril_reqlist.h"

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
typedef enum
{
  QCRIL_PBM_STATUS_UNKNOWN                   = 0,
  QCRIL_PBM_STATUS_REGISTER_READY,
  QCRIL_PBM_STATUS_GET_CAPACITY_COMPLETE,
  QCRIL_PBM_STATUS_READING_RECORD,
  QCRIL_PBM_STATUS_READ_RECORD_COMPLETE
} qcril_pbm_status_e_type;

typedef struct {
  int                       max_adn_num;
  int                       valid_adn_num;
  int                       max_email_num;
  int                       valid_email_num;
  int                       max_ad_num;
  int                       valid_ad_num;
  int                       max_name_len;
  int                       max_number_len;
  int                       max_email_len;
  int                       max_anr_len;
} qcril_pbm_adn_count_info;

struct pbm_cache_info_type {
    // store emergency numbers reported by PBM IND
    char pbm_ind_num[PROPERTY_VALUE_MAX];
    int pbm_num_index;
    int pbm_ind_num_valid;

    // store ADN info
    qcril_pbm_status_e_type pbm_status;
    int pbm_num_session;
    int pbm_session_type[QMI_PBM_MAX_NUM_SESSIONS_V01];
    qcril_pbm_adn_count_info pbm_capacity[QMI_PBM_MAX_NUM_SESSIONS_V01];

    //Store reg mask info
    pbm_reg_mask_type_v01 pbm_reg_mask;

    // for protecting
    pthread_mutex_t     pbm_info_mutex;
    pthread_mutexattr_t pbm_info_mutex_attr;
};

static struct pbm_cache_info_type pbm_cache_info;

typedef struct
{
    pbm_session_type_enum_v01 session_type;
    int record_index;
} qcril_pbm_cb_struct_type;

#define PBM_CACHE_LOCK()    do {\
        QCRIL_LOG_INFO("LOCK PBM_CACHE_LOCK");\
        pthread_mutex_lock(&pbm_cache_info.pbm_info_mutex);\
    } while (0)

#define PBM_CACHE_UNLOCK()  do {\
        pthread_mutex_unlock(&pbm_cache_info.pbm_info_mutex);\
        QCRIL_LOG_INFO("UNLOCK PBM_CACHE_LOCK");\
    } while (0)

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
void get_ecc_property_name(char *ecc_prop_name);
void qcril_qmi_pbm_all_phonebook_init_done_ind_hdlr(void *ind_data_ptr, uint32 ind_data_len);
RIL_Errno qcril_qmi_pbm_get_records_count(void);
RIL_Errno qcril_qmi_pbm_get_all_of_records(int adn_count, int session_type);
void qcril_qmi_pbm_record_read_ext_ind_hdlr(void *ind_data_ptr,
                                            uint32 ind_data_len,
                                            qcril_instance_id_e_type instance_id);
RIL_Errno qcril_qmi_pbm_delete_adn_record(int record_id, int session_type);
RIL_Errno qcril_qmi_pbm_add_or_modify_adn_record(
        const RIL_AdnRecordInfo* record_info_ptr,
        int record_id,
        int session_type);

void qcril_qmi_pbm_init
(
  void
)
{
  memset(&pbm_cache_info, 0, sizeof(pbm_cache_info));
  pthread_mutexattr_init(&pbm_cache_info.pbm_info_mutex_attr);
  pthread_mutex_init(&pbm_cache_info.pbm_info_mutex, &pbm_cache_info.pbm_info_mutex_attr);
  qcril_qmi_pbm_enable_emergency_number_indications(TRUE);
  qcril_qmi_pbm_enable_phonebook_ready_indications(TRUE);
  qcril_qmi_nas_start_wait_for_pbm_ind_timer();
};

RIL_Errno qcril_qmi_pbm_enable_emergency_number_indications(int enable)
{

  RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
  qmi_client_error_type qmi_transport_error;

  pbm_indication_register_req_msg_v01  indication_req;
  pbm_indication_register_resp_msg_v01 indication_resp;

  QCRIL_LOG_INFO("entered %d",enable);

  memset(&indication_req, 0, sizeof(indication_req));
  memset(&indication_resp, 0, sizeof(indication_resp));

  PBM_CACHE_LOCK();
  if( TRUE == enable )
  {/* Register for Emergency list indications */
    pbm_cache_info.pbm_reg_mask |= PBM_REG_EMERGENCY_NUMBER_LIST_EVENTS_V01;
  }
  else
  {/* Suppress Emergency list indications */
    pbm_cache_info.pbm_reg_mask &= ~PBM_REG_EMERGENCY_NUMBER_LIST_EVENTS_V01;
  }
  indication_req.reg_mask = pbm_cache_info.pbm_reg_mask;
  PBM_CACHE_UNLOCK();

  qmi_transport_error = qmi_client_send_msg_sync_with_shm( qcril_qmi_client_get_user_handle ( QCRIL_QMI_CLIENT_PBM ),
                                                   QMI_PBM_INDICATION_REGISTER_REQ_V01,
                                                   &indication_req,
                                                   sizeof(indication_req),
                                                   &indication_resp,
                                                   sizeof(indication_resp),
                                                   QCRIL_QMI_SYNC_REQ_DEF_TIMEOUT );

  ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_transport_error, &indication_resp.resp );

  if ( RIL_E_SUCCESS == ril_req_res )
  {
    QCRIL_LOG_INFO("Indication register successful with mask %d", indication_resp.reg_mask);
  }
  else
  {
    QCRIL_LOG_INFO("Indication register failed, error %d", ril_req_res);
  }

  return ril_req_res;
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_emergency_list_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PBM_EMERGENCY_LIST_IND.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_emergency_list_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
  unsigned int i, k;
  int j, len = 0, cnt = 0 ;
  char prop_val[ PROPERTY_VALUE_MAX ];
  char ecc_prop_name[ PROPERTY_NAME_MAX ];
  boolean is_prop_val_full = FALSE;
  pbm_emergency_list_ind_msg_v01 *emerg_ind;
  char temp_emer_num[QMI_PBM_EMER_NUM_MAX_LENGTH_V01 + 1];

  QCRIL_LOG_FUNC_ENTRY();

  QCRIL_NOTUSED( ind_data_len );

  if (  ind_data_ptr != NULL )
  {
    // multi-ril support, determine which ril.ecclist property to manipulate
    get_ecc_property_name(ecc_prop_name);
    property_get( ecc_prop_name, prop_val, "" );
    QCRIL_LOG_INFO( "current value of  %s = %s", ecc_prop_name, prop_val );

    emerg_ind = (pbm_emergency_list_ind_msg_v01*)ind_data_ptr;

    /* initialize the property value buffer */
    memset( prop_val, '\0', PROPERTY_VALUE_MAX );

    /* OTA emergency numbers */
    if(emerg_ind->network_emer_nums_valid)
    {
      for (k=0; k<emerg_ind->network_emer_nums_len && !is_prop_val_full; k++)
      {
        for(i=0; i < emerg_ind->network_emer_nums[k].emer_nums_len && !is_prop_val_full; i++)
        {
          len = emerg_ind->network_emer_nums[k].emer_nums[i].emer_num_len;
          if ( ( cnt + 1 + len ) <= ( PROPERTY_VALUE_MAX - 1 ) )
          {
            memset(temp_emer_num, 0, sizeof(temp_emer_num));
            memcpy(temp_emer_num, emerg_ind->network_emer_nums[k].emer_nums[i].emer_num, len);
            if( qcril_other_is_number_found( temp_emer_num, prop_val ) )
            {
              QCRIL_LOG_INFO("network emergency number %s already present in the accumulated list %s", temp_emer_num, prop_val);
              continue;
            }

            if ( prop_val[ 0 ] != '\0' )
            {
              /* If this is not the first emergency number, add a comma as a delimiter */
              prop_val[ cnt++ ] = ',';
            }

            for ( j = 0; j < len; j++ )
            {
              prop_val[ cnt++ ] = emerg_ind->network_emer_nums[k].emer_nums[i].emer_num[j];
            }
          }
          else
          {
            /* We cannot fit in any more emergency numbers */
            is_prop_val_full = TRUE;
            QCRIL_LOG_ERROR("emergancy number list exceeds max property legnth, dropping numbers");
          }
        }
      }
      QCRIL_LOG_INFO("Updating emergency number list from network, new list: \"%s\"", prop_val);
    }

    /* Emergency numbers on Card  */
    if ( !is_prop_val_full && emerg_ind->card_emer_nums_valid )
    {
      for (k=0; k<emerg_ind->card_emer_nums_len && !is_prop_val_full; k++)
      {
        for ( i = 0; i < emerg_ind->card_emer_nums[k].emer_nums_len && !is_prop_val_full; i++ )
        {
          len = emerg_ind->card_emer_nums[k].emer_nums[i].emer_num_len;
          if ( ( cnt + 1 + len ) <= ( PROPERTY_VALUE_MAX - 1 ) )
          {
            memset(temp_emer_num, 0, sizeof(temp_emer_num));
            memcpy(temp_emer_num, emerg_ind->card_emer_nums[k].emer_nums[i].emer_num, len);
            if( qcril_other_is_number_found( temp_emer_num, prop_val ) )
            {
              QCRIL_LOG_INFO("card emergency number %s already present in the accumulated list %s", temp_emer_num, prop_val);
              continue;
            }

            if ( prop_val[ 0 ] != '\0' )
            {
              prop_val[ cnt++ ] = ',';
            }

            for ( j = 0; j < len; j++ )
            {
              prop_val[ cnt++ ] = emerg_ind->card_emer_nums[k].emer_nums[i].emer_num[j];
            }
          }
          else
          {
            is_prop_val_full = TRUE;
            QCRIL_LOG_ERROR("emergancy number list exceeds max property legnth, dropping numbers");
          }
        }
      }
      QCRIL_LOG_INFO("Updating emergency number list from sim card, new list: \"%s\"", prop_val);
    }

    /* Hardcoded emergency numbers */
    if ( !is_prop_val_full )
    {
      for(i = 0; i < emerg_ind->emer_nums_len && !is_prop_val_full; i++)
      {
        len = emerg_ind->emer_nums[i].emer_num_len;
        if ( ( cnt + 1 + len ) <= ( PROPERTY_VALUE_MAX - 1 ) )
        {
          memset(temp_emer_num, 0, sizeof(temp_emer_num));
          memcpy(temp_emer_num, emerg_ind->emer_nums[i].emer_num, len);
          if( qcril_other_is_number_found( temp_emer_num, prop_val ) )
          {
            QCRIL_LOG_INFO("hardcoded emergency number %s already present in the accumulated list %s", temp_emer_num, prop_val);
            continue;
          }

          if ( prop_val[ 0 ] != '\0' )
          {
            prop_val[ cnt++ ] = ',';
          }
          for ( j = 0; j < len; j++ )
          {
            prop_val[ cnt++ ] = emerg_ind->emer_nums[i].emer_num[j];
          }
        }
        else
        {
          is_prop_val_full = TRUE;
          QCRIL_LOG_ERROR("emergancy number list exceeds max property legnth, dropping numbers");
        }
      }
      QCRIL_LOG_INFO("Updating emergency number from hardcoded value, new list: \"%s\"", prop_val);
    }

    /* Extended hardcoded emergency numbers */
    if ( !is_prop_val_full && emerg_ind->emer_nums_extended_valid )
    {
      for(i = 0; i < emerg_ind->emer_nums_extended_len && !is_prop_val_full; i++)
      {
        len = emerg_ind->emer_nums_extended[i].emer_num_len;
        if ( ( cnt + 1 + len ) <= ( PROPERTY_VALUE_MAX - 1 ) )
        {
          memset(temp_emer_num, 0, sizeof(temp_emer_num));
          memcpy(temp_emer_num, emerg_ind->emer_nums_extended[i].emer_num, len);
          if( qcril_other_is_number_found( temp_emer_num, prop_val ) )
          {
            QCRIL_LOG_INFO("hardcoded emergency number %s already present in the accumulated list %s", temp_emer_num, prop_val);
            continue;
          }

          if ( prop_val[ 0 ] != '\0' )
          {
            prop_val[ cnt++ ] = ',';
          }
          for ( j = 0; j < len; j++ )
          {
            prop_val[ cnt++ ] = emerg_ind->emer_nums_extended[i].emer_num[j];
          }
        }
        else
        {
          is_prop_val_full = TRUE;
          QCRIL_LOG_ERROR("emergancy number list exceeds max property legnth, dropping numbers");
        }
      }
      QCRIL_LOG_INFO("Updating emergency number from extended hardcoded value, new list: \"%s\"", prop_val);
    }

    /* NV emergency numbers */
    if ( !is_prop_val_full && emerg_ind->nv_emer_nums_valid )
    {
      for(i = 0; i < emerg_ind->nv_emer_nums_len && !is_prop_val_full; i++)
      {
        len = emerg_ind->nv_emer_nums[i].emer_num_len;
        if ( ( cnt + 1 + len ) <= ( PROPERTY_VALUE_MAX - 1 ) )
        {
          memset(temp_emer_num, 0, sizeof(temp_emer_num));
          memcpy(temp_emer_num, emerg_ind->nv_emer_nums[i].emer_num, len);
          if( qcril_other_is_number_found( temp_emer_num, prop_val ) )
          {
            QCRIL_LOG_INFO("NV emergency number %s already present in the accumulated list %s", temp_emer_num, prop_val);
            continue;
          }

          if ( prop_val[ 0 ] != '\0' )
          {
            prop_val[ cnt++ ] = ',';
          }
          for ( j = 0; j < len; j++ )
          {
            prop_val[ cnt++ ] = emerg_ind->nv_emer_nums[i].emer_num[j];
          }
        }
        else
        {
          is_prop_val_full = TRUE;
          QCRIL_LOG_ERROR("emergancy number list exceeds max property legnth, dropping numbers");
        }
      }
      QCRIL_LOG_INFO("Updating emergency number list from NV, new list: \"%s\"", prop_val);
    }

    /* Store the numbers into PBM cache, and do no set prop at this time for two reasons:
     *   - If custom_ecc is enabled, set the prop later together to avoid ecc prop has incompleted
     * numbers in a short time window.
     *   - Later when card_status changed or voice_rte_confidence changed, RIL will combine the numbers
     * matched in database and that in PBM cache into ecc.list property, other than appending news numbers
     * into existed ecc.list prop, which is not right.
     */
    PBM_CACHE_LOCK();
    pbm_cache_info.pbm_ind_num_valid = TRUE;
    pbm_cache_info.pbm_num_index = cnt;
    memcpy(pbm_cache_info.pbm_ind_num, prop_val, PROPERTY_VALUE_MAX);
    PBM_CACHE_UNLOCK();

    qmi_ril_nwreg_designated_number_ensure_fresh_check_ncl();
    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                   QCRIL_DEFAULT_MODEM_ID,
                   QCRIL_DATA_ON_STACK,
                   QCRIL_EVT_QMI_RIL_ASSESS_EMRGENCY_NUMBER_LIST_DESIGNATED_COUNTRY,
                   NULL,
                   QMI_RIL_ZERO,
                   (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );

  }

  QCRIL_LOG_FUNC_RETURN();

}/* qcril_qmi_pbm_emergency_list_ind_hdlr */


/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_unsol_ind_cb

===========================================================================*/
/*!
    @brief
    Callback for QMI PBM indications

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
)
{
  qmi_ind_callback_type qmi_callback;

  QCRIL_LOG_FUNC_ENTRY();

  memset(&qmi_callback,0,sizeof(qmi_callback));
  qmi_callback.data_buf = qcril_malloc(ind_buf_len);

  if( qmi_callback.data_buf )
  {
    qmi_callback.user_handle = user_handle;
    qmi_callback.msg_id = msg_id;
    memcpy(qmi_callback.data_buf,ind_buf,ind_buf_len);
    qmi_callback.data_buf_len = ind_buf_len;
    qmi_callback.cb_data = ind_cb_data;

    qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                   QCRIL_DEFAULT_MODEM_ID,
                   QCRIL_DATA_ON_STACK,
                   QCRIL_EVT_QMI_PBM_HANDLE_INDICATIONS,
                   (void*) &qmi_callback,
                   sizeof(qmi_callback),
                   (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );
  }
  else
  {
    QCRIL_LOG_FATAL("malloc failed");
  }

  QCRIL_LOG_FUNC_RETURN();
}/* qcril_qmi_pbm_unsol_ind_cb */


/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_unsolicited_indication_cb_helper

===========================================================================*/
/*!
    @brief
    helper function for handling pbm indication

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_unsolicited_indication_cb_helper
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type *const ret_ptr
)
{
    void * decoded_payload = NULL;
    qmi_client_error_type qmi_err;
    uint32_t decoded_payload_len;

    QCRIL_NOTUSED(ret_ptr);
    qmi_ind_callback_type *qmi_callback = (qmi_ind_callback_type*) params_ptr->data;

    do {

       if( !qmi_callback )
       {
          QCRIL_LOG_ERROR("qmi_callback is NULL");
          QCRIL_ASSERT(0); // this is a noop in release build
          break;
       }

       QCRIL_LOG_INFO("invoked msg 0x%x", (int) qmi_callback->msg_id);

       qmi_err = qmi_idl_get_message_c_struct_len( pbm_get_service_object_v01(),
                                                   QMI_IDL_INDICATION,
                                                   qmi_callback->msg_id,
                                                   &decoded_payload_len);
       if ( qmi_err != QMI_NO_ERR )
       {
          QCRIL_LOG_ERROR("Failed to process qmi message w/%d", qmi_err);
          QCRIL_ASSERT(0); // this is a noop in release build
          break;
       }

       if( !decoded_payload_len )
       {
          // ok, this is a null payload - don't need to process it.
       }
       else
       {
          // process the payload
          decoded_payload = qcril_malloc( decoded_payload_len );
          if ( !decoded_payload )
          {
             QCRIL_LOG_ERROR("Failed to alloc memory for decoded payload");
             QCRIL_ASSERT(0); // this is a noop in release build
             break;
          }

          memset( decoded_payload, 0, decoded_payload_len );

          qmi_err = qmi_client_message_decode(
                        qcril_qmi_client_get_user_handle( QCRIL_QMI_CLIENT_PBM ),
                        QMI_IDL_INDICATION,
                        qmi_callback->msg_id,
                        qmi_callback->data_buf,
                        qmi_callback->data_buf_len,
                        decoded_payload,
                        (int)decoded_payload_len );

          if ( qmi_err != QMI_NO_ERR )
          {
             QCRIL_LOG_ERROR("Failed to process qmi message w/%d", qmi_err);
             QCRIL_ASSERT(0); // this is a noop in release build
             qcril_free(decoded_payload);
             break;
          }

          switch ( qmi_callback->msg_id )
          {
            case QMI_PBM_EMERGENCY_LIST_IND_V01:
              qcril_qmi_pbm_emergency_list_ind_hdlr(decoded_payload, decoded_payload_len);
              break;

            case QMI_PBM_ALL_PB_INIT_DONE_IND_V01:
              qcril_qmi_pbm_all_phonebook_init_done_ind_hdlr(decoded_payload, decoded_payload_len);
              break;

            case QMI_PBM_RECORD_READ_EXT_IND_V01:
              qcril_qmi_pbm_record_read_ext_ind_hdlr(decoded_payload,
                                                decoded_payload_len,
                                                (qcril_instance_id_e_type)params_ptr->instance_id);
              break;

            default:
              QCRIL_LOG_INFO("Unsupported QMI PBM indication %x hex", qmi_callback->msg_id);
              break;
          }
          qcril_free( decoded_payload );
       }

    } while(0);

    if( qmi_callback && qmi_callback->data_buf )
    {
      qcril_free(qmi_callback->data_buf);
    }
    QCRIL_LOG_FUNC_RETURN();
}//qcril_qmi_pbm_unsolicited_indication_cb_helper

//===========================================================================
//qmi_ril_phone_number_is_emergency
//===========================================================================
int qmi_ril_phone_number_is_emergency(char * number_to_check)
{
  int res = FALSE;

  char ecc_prop_val[ PROPERTY_VALUE_MAX ];
  char ecc_prop_name[ PROPERTY_NAME_MAX ];
  int  prop_req_res;

  QCRIL_LOG_FUNC_ENTRY();

  get_ecc_property_name(ecc_prop_name);
  prop_req_res = property_get( ecc_prop_name, ecc_prop_val, "" );
  QCRIL_LOG_INFO(" .. prop req with %d", (int) prop_req_res);
  if ( prop_req_res  > 0 )
  {
    QCRIL_LOG_ESSENTIAL( "property %s = \"%s\"", ecc_prop_name, ecc_prop_val );
    res = qcril_other_is_number_found( number_to_check, ecc_prop_val );
  }
  else
  {
    QCRIL_LOG_INFO("propperty_get %s returned w/%d", ecc_prop_name, (int) prop_req_res);
  }

  QCRIL_LOG_FUNC_RETURN_WITH_RET( res );
  return res;
} // qmi_ril_phone_number_is_emergency

//===========================================================================
//qmi_ril_phone_number_is_non_std_otasp
//===========================================================================
int qmi_ril_phone_number_is_non_std_otasp(const char * number_to_check)
{
  return strncmp(number_to_check, NON_STD_OTASP_NUMBER,strlen(NON_STD_OTASP_NUMBER)+1) ? 0 : 1;
} // qmi_ril_phone_number_is_non_std_otasp

//===========================================================================
//get_ecc_property_name
//Returns the ecc list property name in ecc_prop_name for this instance.
//For Default instance and in non multisim targets this is ril.ecclist.
//===========================================================================
void get_ecc_property_name(char *ecc_prop_name)
{
   qcril_instance_id_e_type instance_id = qmi_ril_get_process_instance_id();

   if ( ecc_prop_name == NULL)
   {
       QCRIL_LOG_ERROR("get_ecc_property_name: Invalid argument.");
   }
   else
   {
       if ( instance_id == QCRIL_DEFAULT_INSTANCE_ID )
       {
           snprintf( ecc_prop_name, PROPERTY_NAME_MAX, "%s",QCRIL_ECC_LIST);
       }
       else
       {
           snprintf( ecc_prop_name, PROPERTY_NAME_MAX, "%s%d", QCRIL_ECC_LIST, instance_id);
       }
       QCRIL_LOG_ERROR("ecc list name : %s.", ecc_prop_name);
   }
}

/*=========================================================================
  FUNCTION:  qmi_ril_set_ecc_property

===========================================================================*/
/*!
    @brief
    Combine param "ecc_bumbers" and "numbers in PBM cache" into ECC property

    @params
    - ecc_numbers: ecc numbers delimited with comma, ended with '\0'. It could
    be *NULL*.

    @return
    0 if success. Other value if failure
*/
/*=========================================================================*/

int qmi_ril_set_ecc_property(char* ecc_numbers)
{
    char ecc_prop_name[PROPERTY_NAME_MAX];
    char ecc_prop_val[PROPERTY_VALUE_MAX];
    size_t param_ecc_len, item_len, val_index;
    int ret, no_space = 0;
    const char delim[2] = {',',0};
    char *num_parsing_holder, *saveptr, *token;

    QCRIL_LOG_FUNC_ENTRY();

    /* get the right prop name for current RIL instance */
    get_ecc_property_name(ecc_prop_name);
    memset(ecc_prop_val, 0, PROPERTY_VALUE_MAX);

    /* copy numbers from PBM IND */
    PBM_CACHE_LOCK();
    if (pbm_cache_info.pbm_ind_num_valid)
    {
        if (strlcpy(ecc_prop_val, pbm_cache_info.pbm_ind_num, PROPERTY_VALUE_MAX) >= PROPERTY_VALUE_MAX)
        {
            /* should not happen since PBM IND handler already consider this */
            QCRIL_LOG_FATAL("PBM IND numbers exceeds the property length");
            no_space = 1;
        }
    }
    PBM_CACHE_UNLOCK();

    /* append the custom ecc numbers if have space */
    if (!no_space && ecc_numbers)
    {
        param_ecc_len = strlen(ecc_numbers);
        val_index = strlen(ecc_prop_val);
        num_parsing_holder = (char*)qcril_malloc(param_ecc_len + 1);
        if (num_parsing_holder)
        {
            /* not checking return values since there could not be truncation */
            strlcpy(num_parsing_holder, ecc_numbers, param_ecc_len + 1);
            token = strtok_r(num_parsing_holder, delim, &saveptr);
            while (token)
            {
                /* if it is not a duplicated number */
                if (!qcril_other_is_number_found(token, ecc_prop_val))
                {
                    /* check if we have enough space to append this number */
                    item_len = strlen(token) + ((!val_index)?0:1);
                    if ((item_len + 1) > (PROPERTY_VALUE_MAX - val_index))
                    {
                        no_space = 1;
                        break;
                    }
                    else
                    {
                        /* Already checked boundary */
                        if (val_index)
                            strlcat(ecc_prop_val, ",", PROPERTY_VALUE_MAX);
                        strlcat(ecc_prop_val, token, PROPERTY_VALUE_MAX);
                    }
                    val_index += item_len;

                }
                token = strtok_r(NULL, delim, &saveptr);
            }
            qcril_free(num_parsing_holder);
        }
    }

    if (no_space)
    {
        QCRIL_LOG_ERROR("The ecc numbers exceeds the property length, abandon some of them");
    }
    QCRIL_LOG_INFO("new saved %s: %s", ecc_prop_name, ecc_prop_val);

    /* set the property */
    if ((ret = property_set(ecc_prop_name, ecc_prop_val)) != E_SUCCESS)
    {
        QCRIL_LOG_FATAL("Failed to save %s to system property", ecc_prop_name);
    }

    QCRIL_LOG_FUNC_RETURN_WITH_RET(ret);

    return ret;
}

RIL_Errno qcril_qmi_pbm_enable_phonebook_ready_indications(int enable)
{
    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
    qmi_client_error_type qmi_transport_error;

    pbm_indication_register_req_msg_v01  indication_req;
    pbm_indication_register_resp_msg_v01 indication_resp;

    QCRIL_LOG_INFO("entered %d",enable);

    memset(&indication_req, 0, sizeof(indication_req));
    memset(&indication_resp, 0, sizeof(indication_resp));

    PBM_CACHE_LOCK();
    if( TRUE == enable )
    {/* Register for phonebook ready indications */
        pbm_cache_info.pbm_reg_mask |= PBM_REG_RECORD_UPDATE_EVENTS_V01;
        pbm_cache_info.pbm_reg_mask |= PBM_REG_PHONEBOOK_READY_EVENTS_V01;
    }
    else
    {/* Suppress phonebook ready indications */
        pbm_cache_info.pbm_reg_mask &= ~PBM_REG_RECORD_UPDATE_EVENTS_V01;
        pbm_cache_info.pbm_reg_mask &= ~PBM_REG_PHONEBOOK_READY_EVENTS_V01;
    }
    indication_req.reg_mask = pbm_cache_info.pbm_reg_mask;
    PBM_CACHE_UNLOCK();

    qmi_transport_error = qmi_client_send_msg_sync_with_shm(
                                        qcril_qmi_client_get_user_handle( QCRIL_QMI_CLIENT_PBM ),
                                        QMI_PBM_INDICATION_REGISTER_REQ_V01,
                                        &indication_req,
                                        sizeof(indication_req),
                                        &indication_resp,
                                        sizeof(indication_resp),
                                        QCRIL_QMI_SYNC_REQ_DEF_TIMEOUT );

    ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_transport_error,
                                                                        &indication_resp.resp );

    if ( RIL_E_SUCCESS == ril_req_res )
    {
        QCRIL_LOG_INFO("Indication register successful with mask %d", indication_resp.reg_mask);
    }
    else
    {
        QCRIL_LOG_INFO("Indication register failed, error %d", ril_req_res);
    }

    return ril_req_res;
}


/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_all_phonebook_init_done_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PBM_ALL_PB_INIT_DONE_IND_V01.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_all_phonebook_init_done_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len
)
{
    pbm_all_pb_init_done_ind_msg_v01 *all_pb_init_done_ind;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_NOTUSED( ind_data_len );

    if ( ind_data_ptr != NULL )
    {
        all_pb_init_done_ind = (pbm_all_pb_init_done_ind_msg_v01*)ind_data_ptr;

        uint16_t pb_type = all_pb_init_done_ind->phonebook_ready_info[0].pb_bit_mask;
        pbm_session_type_enum_v01 session_type =
                            all_pb_init_done_ind->phonebook_ready_info[0].session_type;

        if( all_pb_init_done_ind->phonebook_ready_info_len > 0 && pb_type > 0 ) {

            QCRIL_LOG_INFO("Pb type is %d, session type is %d", pb_type, session_type);

            /* ADN phone book */
            if(pb_type & PBM_PB_TYPE_ADN_V01)
            {
                QCRIL_LOG_INFO("Pb type is ADN");


                pbm_phonebook_info_type_v01 req;
                memset(&req, 0, sizeof(req));

                req.pb_type      = PBM_PB_TYPE_ADN_V01;
                req.session_type = session_type;

                (void)qcril_event_queue(QCRIL_DEFAULT_INSTANCE_ID,
                                    QCRIL_DEFAULT_MODEM_ID,
                                    QCRIL_DATA_ON_STACK,
                                    QCRIL_EVT_PBM_GET_PB_CAPACITY,
                                    (void *) &req,
                                    sizeof(req),
                                    (RIL_Token) QCRIL_TOKEN_ID_INTERNAL);
            }
        }
    }

    QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_get_pb_capacity_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_PBM_GET_PB_CAPACITY.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_get_pb_capacity_hndlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
)
{
    RIL_Errno result = RIL_E_GENERIC_FAILURE;
    pbm_phonebook_info_type_v01 *req;
    int session_idx = 0;

    QCRIL_NOTUSED(ret_ptr);
    QCRIL_LOG_FUNC_ENTRY();

    PBM_CACHE_LOCK();
    for(session_idx=0; session_idx<(int)pbm_cache_info.pbm_num_session; session_idx++) {
        pbm_cache_info.pbm_session_type[session_idx] = 0;
        memset(&pbm_cache_info.pbm_capacity[session_idx], 0,
                                    sizeof(qcril_pbm_adn_count_info));
    }
    pbm_cache_info.pbm_num_session = 0;
    pbm_cache_info.pbm_status = QCRIL_PBM_STATUS_REGISTER_READY;
    PBM_CACHE_UNLOCK();

    req = (pbm_phonebook_info_type_v01 *)params_ptr->data;

    if (req)
    {
        result = qcril_qmi_pbm_get_records_count();
    }

    QCRIL_LOG_DEBUG("result: %d", result);

    if(result == RIL_E_SUCCESS
        && pbm_cache_info.pbm_status == QCRIL_PBM_STATUS_GET_CAPACITY_COMPLETE)
    {
        /* Send unsolicited response to framework */
        qcril_hook_unsol_response((qcril_instance_id_e_type)params_ptr->instance_id,
                                    QCRIL_EVT_HOOK_UNSOL_ADN_INIT_DONE, NULL, 0);
    }
    QCRIL_LOG_FUNC_RETURN();
}

/* This function is to get the records count of the PB via QMI PBM */
RIL_Errno qcril_qmi_pbm_get_records_count(void)
{
    pbm_get_all_pb_capabilities_resp_msg_v01 pbm_info_resp;
    int session_idx, pbset_idx, caps_idx;
    qmi_client_error_type            qmi_error;
    RIL_Errno                        result = RIL_E_GENERIC_FAILURE;
    int max_records = 0, used_records = 0, max_email = 0,
        used_email = 0, max_ad_num = 0, used_ad_num = 0,
        max_name_len = 0, max_number_len = 0, max_email_len = 0,
        max_anr_len = 0;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&pbm_info_resp,0,sizeof(pbm_info_resp));

    qmi_error = qmi_client_send_msg_sync_with_shm(
                        qcril_qmi_client_get_user_handle(QCRIL_QMI_CLIENT_PBM),
                        QMI_PBM_GET_ALL_PB_CAPABILITIES_REQ_V01,
                        NULL,
                        NAS_NIL,
                        &pbm_info_resp,
                        sizeof(pbm_info_resp),
                        QCRIL_QMI_SYNC_REQ_DEF_TIMEOUT);

    result = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_error,
                                                            &pbm_info_resp.resp);

    if ( RIL_E_SUCCESS == result )
    {
        QCRIL_LOG_INFO("Get pb capabilities successfully");
        if (pbm_info_resp.pb_set_info_array_valid)
        {
          for(session_idx= 0; session_idx < (int)pbm_info_resp.pb_set_info_array_len; session_idx++)
          {
            int session_type = (int)pbm_info_resp.pb_set_info_array[session_idx].session_type;

            pbm_pbr_cap_info_type_v01 *pbset_info_ptr =
                                & pbm_info_resp.pb_set_info_array[session_idx].pb_set_info;
            max_records = used_records =  max_email = used_email = max_ad_num = used_ad_num = 0;
            max_name_len = max_number_len = max_email_len = max_anr_len = 0;
            for (pbset_idx= 0; pbset_idx < (int)pbset_info_ptr->num_of_pb_sets; pbset_idx++)
            {
                pbm_pb_set_cap_info_type_v01 * pbset_caps_ptr =
                                    & pbset_info_ptr->pbset_caps[pbset_idx];
                max_records += pbset_caps_ptr->adn_caps.max_adn_records;
                used_records += pbset_caps_ptr->adn_caps.used_adn_records;
                max_name_len = MAX(max_name_len, pbset_caps_ptr->adn_caps.max_adn_name_len);
                max_number_len = MAX(max_number_len, pbset_caps_ptr->adn_caps.max_adn_num_len);
                for (caps_idx= 0; caps_idx < (int)pbset_caps_ptr->email_caps_len; caps_idx++)
                {
                     max_email += pbset_caps_ptr->email_caps[caps_idx].record_caps.max_records;
                     used_email += pbset_caps_ptr->email_caps[caps_idx].record_caps.used_records;
                     max_email_len = MAX(max_email_len, pbset_caps_ptr->email_caps[caps_idx].record_caps.max_record_len);
                }
                for (caps_idx= 0; caps_idx < (int)pbset_caps_ptr->ad_num_caps_len; caps_idx++)
                {
                     max_ad_num += pbset_caps_ptr->ad_num_caps[caps_idx].record_caps.max_records;
                     used_ad_num += pbset_caps_ptr->ad_num_caps[caps_idx].record_caps.used_records;
                     max_anr_len = MAX(max_anr_len,  pbset_caps_ptr->ad_num_caps[caps_idx].record_caps.max_record_len);
                }
          }

          //int pb_type = (int) pbm_info_resp.capability_basic_info.pb_type;
          QCRIL_LOG_INFO("Session type: %d", session_type);

          QCRIL_LOG_INFO("max records: %d, valid records: %d", max_records, used_records);
          QCRIL_LOG_INFO("max email: %d, valid email: %d, max ad number: %d, valid ad number: %d",
                        max_email, used_email, max_ad_num, used_ad_num);
          QCRIL_LOG_INFO("max name len: %d, max number len: %d, max email len: %d, max anr len: %d",
                        max_name_len, max_number_len, max_email_len, max_anr_len);

          if(max_records > 0)
          {
            PBM_CACHE_LOCK();
            if(session_type >= PBM_SESSION_TYPE_GLOBAL_PB_SLOT1_V01) {
              pbm_cache_info.pbm_status = QCRIL_PBM_STATUS_GET_CAPACITY_COMPLETE;
            }
            pbm_cache_info.pbm_session_type[pbm_cache_info.pbm_num_session] = session_type;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].max_adn_num= max_records;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].valid_adn_num= used_records;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].max_email_num= max_email;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].valid_email_num= used_email;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].max_ad_num= max_ad_num;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].valid_ad_num= used_ad_num;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].max_name_len= max_name_len;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].max_number_len= max_number_len;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].max_email_len= max_email_len;
            pbm_cache_info.pbm_capacity[pbm_cache_info.pbm_num_session].max_anr_len= max_anr_len;
            pbm_cache_info.pbm_num_session ++;
            PBM_CACHE_UNLOCK();
          }
        }
      }
    }
    else
    {
        QCRIL_LOG_INFO("Get pb capabilities failed, error %d", result);
    }
    return result;

    QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_get_adn_record_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_HOOK_GET_ADN_RECORD.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_get_adn_record_hndlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
)
{
    RIL_Errno                             ril_req_res = RIL_E_SUCCESS;
    qcril_pbm_adn_count_info              adn_count_info;
    qcril_request_resp_params_type        resp;
    qcril_instance_id_e_type   instance_id;
    int i;

    QCRIL_NOTUSED( ret_ptr );

    QCRIL_LOG_FUNC_ENTRY();

    instance_id = params_ptr->instance_id;
    if (instance_id >= QCRIL_MAX_INSTANCE_ID)
    {
        QCRIL_LOG_ERROR("Invalid value, instance_id: 0x%x", instance_id);
        QCRIL_ASSERT(0);
        return;
    }

    memset(&resp, 0, sizeof(qcril_request_resp_params_type));
    memset(&adn_count_info, 0, sizeof(qcril_pbm_adn_count_info));

    for ( i = 0; i < pbm_cache_info.pbm_num_session; i++ )
    {
        int adn_count = pbm_cache_info.pbm_capacity[i].max_adn_num;
        if(adn_count > 0) {
            ril_req_res = qcril_qmi_pbm_get_all_of_records(adn_count,
                                                pbm_cache_info.pbm_session_type[i]);
        } else {
            QCRIL_LOG_INFO("The ADN file did not exist on this session: ",
                                                pbm_cache_info.pbm_session_type[i]);
        }
        adn_count_info.max_adn_num += pbm_cache_info.pbm_capacity[i].max_adn_num;
        adn_count_info.valid_adn_num += pbm_cache_info.pbm_capacity[i].valid_adn_num;
        adn_count_info.max_email_num += pbm_cache_info.pbm_capacity[i].max_email_num;
        adn_count_info.valid_email_num += pbm_cache_info.pbm_capacity[i].valid_email_num;
        adn_count_info.max_ad_num += pbm_cache_info.pbm_capacity[i].max_ad_num;
        adn_count_info.valid_ad_num += pbm_cache_info.pbm_capacity[i].valid_ad_num;
        adn_count_info.max_name_len = MAX(adn_count_info.max_name_len, pbm_cache_info.pbm_capacity[i].max_name_len);
        adn_count_info.max_number_len = MAX(adn_count_info.max_number_len, pbm_cache_info.pbm_capacity[i].max_number_len);
        adn_count_info.max_email_len = MAX(adn_count_info.max_email_len, pbm_cache_info.pbm_capacity[i].max_email_len);
        adn_count_info.max_anr_len = MAX(adn_count_info.max_anr_len, pbm_cache_info.pbm_capacity[i].max_anr_len);
    }

    QCRIL_LOG_INFO("For all active session, the max records is %d, the valid records is %d",
                                        adn_count_info.max_adn_num, adn_count_info.valid_adn_num);
    QCRIL_LOG_INFO("For all active session, the max email is %d, the max ad num is %d",
                                        adn_count_info.max_email_num, adn_count_info.max_ad_num);
    QCRIL_LOG_INFO("For all active session, the max name length is %d, the max number length is %d",
                                        adn_count_info.max_name_len, adn_count_info.max_number_len);
    QCRIL_LOG_INFO("For all active session, the max email length is %d, the max anr length is %d",
                                        adn_count_info.max_email_len, adn_count_info.max_anr_len);

    qcril_default_request_resp_params( instance_id,
                                       params_ptr->t,
                                       params_ptr->event_id,
                                       ril_req_res,
                                       &resp );

    resp.resp_pkt = &adn_count_info;
    resp.resp_len = sizeof(adn_count_info);
    qcril_send_request_response( &resp );

    QCRIL_LOG_FUNC_RETURN();
}

RIL_Errno qcril_qmi_pbm_get_all_of_records(int adn_count, int session_type)
{
    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
    qmi_client_error_type qmi_transport_error;

    pbm_read_records_ext_req_msg_v01  pbm_read_req;
    pbm_read_records_ext_resp_msg_v01 pbm_read_resp;
    int index;

    QCRIL_LOG_FUNC_ENTRY();

    memset(&pbm_read_req, 0, sizeof(pbm_read_req));
    memset(&pbm_read_resp, 0, sizeof(pbm_read_resp));

    pbm_read_req.record_info.pb_type = PBM_PB_TYPE_ADN_V01;
    pbm_read_req.record_info.session_type = (pbm_session_type_enum_v01)session_type;
    pbm_read_req.record_info.record_start_id = 1;
    pbm_read_req.record_info.record_end_id = adn_count;

    QCRIL_LOG_INFO("Start ID is %d, end ID is %d",
                pbm_read_req.record_info.record_start_id,  pbm_read_req.record_info.record_end_id);
    QCRIL_LOG_INFO("Pb type is %d, session type is %d",
                pbm_read_req.record_info.pb_type,  pbm_read_req.record_info.session_type);

    qmi_transport_error = qmi_client_send_msg_sync_with_shm(
                                        qcril_qmi_client_get_user_handle( QCRIL_QMI_CLIENT_PBM ),
                                        QMI_PBM_READ_RECORDS_EXT_REQ_V01,
                                        &pbm_read_req,
                                        sizeof(pbm_read_req),
                                        &pbm_read_resp,
                                        sizeof(pbm_read_resp),
                                        QCRIL_QMI_SYNC_REQ_DEF_TIMEOUT );

    ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result( qmi_transport_error,
                                                                        &pbm_read_resp.resp );

    if ( RIL_E_SUCCESS == ril_req_res && pbm_read_resp.num_of_recs_valid )
    {
        int used_records = pbm_read_resp.num_of_recs;
        if (used_records == 0)
        {
            QCRIL_LOG_INFO("There are no entries filled in ADN, nothing to read");
        }
        else
        {
            QCRIL_LOG_INFO("There are %d filled entries in ADN", used_records);
            for ( index = 0; index < pbm_cache_info.pbm_num_session; index++ )
            {
                if(session_type == pbm_cache_info.pbm_session_type[index]) {
                    PBM_CACHE_LOCK();
                    pbm_cache_info.pbm_status = QCRIL_PBM_STATUS_READING_RECORD;
                    pbm_cache_info.pbm_capacity[index].valid_adn_num = used_records;
                    PBM_CACHE_UNLOCK();
                    break;
                 }
            }
        }
    }
    else
    {
        QCRIL_LOG_INFO("Read request for ADN failed with error code = %d",
                                            (int)pbm_read_resp.resp.error);
    }

    return ril_req_res;
    QCRIL_LOG_FUNC_RETURN();
}

char* qcril_qmi_pbm_alloc_to_utf8
(
  const uint8*  buffer_ptr,
  uint16        buffer_size,
  boolean       is_ucs2
)
{
  char*  out_ptr    = NULL;
  uint16 string_len = 0;

  if(buffer_ptr == NULL || buffer_size == 0)
  {
    QCRIL_LOG_INFO("Original buffer is NULL pointer");
    return NULL;
  }

  if(is_ucs2)
  {
      buffer_size <<= 1;
  }

  string_len = (buffer_size << 1) + sizeof(char);

  out_ptr = qcril_malloc(string_len);
  if (out_ptr != NULL)
  {
    memset(out_ptr, 0, string_len);
    if(is_ucs2)
    {
        qcril_cm_ss_convert_ucs2_to_utf8((char*) buffer_ptr,
                                            buffer_size,
                                            out_ptr,
                                            string_len);
    }
    else
    {
        qcril_cm_ss_ascii_to_utf8((unsigned char*) buffer_ptr, buffer_size, out_ptr, string_len);
    }
  }
  else
  {
      QCRIL_LOG_INFO("Failed to allocate buffer for UTF-8 output");
  }

  return out_ptr;
}

void qcril_qmi_pbm_free_adn_record_info
(
  RIL_AdnRecord_v1*  adn_record_ptr
)
{
  int index, j = 0;

  if (adn_record_ptr == NULL)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot proceed");
    QCRIL_ASSERT(0);
    return;
  }

  QCRIL_LOG_INFO( "%s\n", "Freeing ADN record info buffers" );

  /* Loop through all the apps and free buffers allocated */
  for (index = 0; index < adn_record_ptr->record_elements; index++)
  {
    if(adn_record_ptr->adn_record_info[index].name != NULL)
    {
      qcril_free(adn_record_ptr->adn_record_info[index].name);
      adn_record_ptr->adn_record_info[index].name = NULL;
      QCRIL_LOG_DEBUG("Freed Name pointer, record[%d]", index);
    }

    if(adn_record_ptr->adn_record_info[index].number != NULL)
    {
      qcril_free(adn_record_ptr->adn_record_info[index].number);
      adn_record_ptr->adn_record_info[index].number = NULL;
      QCRIL_LOG_DEBUG("Freed Number pointer, record[%d]", index);
    }

    if(adn_record_ptr->adn_record_info[index].email_elements > 0)
    {
      for(j = 0; j < adn_record_ptr->adn_record_info[index].email_elements; j++)
      {
          qcril_free(adn_record_ptr->adn_record_info[index].email[j]);
          adn_record_ptr->adn_record_info[index].email[j] = NULL;
          QCRIL_LOG_DEBUG("Freed Email pointer, index[%d], record[%d]", j, index);
      }
    }

    if(adn_record_ptr->adn_record_info[index].anr_elements > 0)
    {
      for(j = 0; j < adn_record_ptr->adn_record_info[index].anr_elements; j++)
      {
          qcril_free(adn_record_ptr->adn_record_info[index].ad_number[j]);
          adn_record_ptr->adn_record_info[index].ad_number[j] = NULL;
          QCRIL_LOG_DEBUG("Freed AdNumber pointer, index[%d], record[%d]", j, index);
      }
    }
  }
}

void qcril_qmi_pbm_record_read_ext_hook_unsol_rsp
(
  qcril_instance_id_e_type   instance_id,
  RIL_AdnRecord_v1*  adn_record_ptr
)
{
    char* hook_response;
    uint16_t element_index, response_index, response_length = 0;
    uint16_t name_length, number_length, email_length, anr_length;
    uint16_t email_elements, email_index, anr_elements, anr_index;
    const uint16_t INT_SIZE = 2;

    QCRIL_LOG_FUNC_ENTRY();
    response_length = INT_SIZE;  //record_elements
    for(element_index = 0; element_index < adn_record_ptr->record_elements; element_index++)
    {
        RIL_AdnRecordInfo* adn_resp_ptr = & adn_record_ptr->adn_record_info[element_index];

        response_length += INT_SIZE;                      //record_id
        response_length += INT_SIZE;                      //name_length
        if(adn_resp_ptr->name != NULL) {
            response_length += strlen(adn_resp_ptr->name);    //name
        }
        response_length += INT_SIZE;                      //number_length
        if(adn_resp_ptr->number != NULL) {
            response_length += strlen(adn_resp_ptr->number);  //number
        }
        response_length += INT_SIZE;                      //email_elements
        for(email_index = 0; email_index < adn_resp_ptr->email_elements; email_index++)
        {
            response_length += INT_SIZE;  //email_length
            response_length += strlen(adn_resp_ptr->email[email_index]); //email
        }
        response_length += INT_SIZE;                      //anr_elements
        for(anr_index = 0; anr_index < adn_resp_ptr->anr_elements; anr_index++)
        {
            response_length += INT_SIZE;                  //anr_length
            response_length += strlen(adn_resp_ptr->ad_number[anr_index]); //anr
        }
        response_length ++;
    }

    QCRIL_LOG_INFO("Response length is: %d",response_length);

    hook_response = qcril_malloc(response_length);
    if ( !hook_response )
    {
        QCRIL_LOG_ERROR("Failed to alloc memory for oem hook payload");
        //result = RIL_E_GENERIC_FAILURE;
        return;
    }

    //record_elements
    response_index = 0;
    hook_response[response_index++] = adn_record_ptr->record_elements & 0x00FF;
    hook_response[response_index++] = (adn_record_ptr->record_elements & 0xFF00) >> 8;

    for(element_index = 0; element_index < adn_record_ptr->record_elements; element_index++)
    {
        RIL_AdnRecordInfo* adn_resp_ptr = & adn_record_ptr->adn_record_info[element_index];

        //record_id
        hook_response[response_index++] = adn_resp_ptr->record_id & 0x00FF;
        hook_response[response_index++] = (adn_resp_ptr->record_id & 0xFF00) >> 8;

        //name_length
        if(adn_resp_ptr->name != NULL) {
            name_length = strlen(adn_resp_ptr->name);
        }
        else
        {
            name_length = 0;
        }
        hook_response[response_index++] = name_length & 0x00FF;
        hook_response[response_index++] = (name_length & 0xFF00) >> 8;

        //name
        memcpy(hook_response + response_index, adn_resp_ptr->name, name_length);
        response_index += name_length;

        //number_length
        if(adn_resp_ptr->number!= NULL) {
            number_length = strlen(adn_resp_ptr->number);
        }
        else
        {
            number_length = 0;
        }
        hook_response[response_index++] = number_length & 0x00FF;
        hook_response[response_index++] = (number_length & 0xFF00) >> 8;

        //number
        memcpy(hook_response + response_index, adn_resp_ptr->number, number_length);
        response_index += number_length;

        //email_elements
        email_elements = adn_resp_ptr->email_elements;
        hook_response[response_index++] = email_elements & 0x00FF;
        hook_response[response_index++] = (email_elements & 0xFF00) >> 8;

        for(email_index = 0; email_index < email_elements; email_index++)
        {
            //email_length
            email_length = strlen(adn_resp_ptr->email[email_index]);
            hook_response[response_index++] = email_length & 0x00FF;
            hook_response[response_index++] = (email_length & 0xFF00) >> 8;

            //email
            memcpy(hook_response + response_index,
                        adn_resp_ptr->email[email_index], email_length);
            response_index += email_length;
        }

        //anr_elements
        anr_elements = adn_resp_ptr->anr_elements;
        hook_response[response_index++] = anr_elements & 0x00FF;
        hook_response[response_index++] = (anr_elements & 0xFF00) >> 8;

        for(anr_index = 0; anr_index < anr_elements; anr_index++)
        {
            //anr_length
            anr_length = strlen(adn_resp_ptr->ad_number[anr_index]);
            hook_response[response_index++] = anr_length & 0x00FF;
            hook_response[response_index++] = (anr_length & 0xFF00) >> 8;

            //email
            memcpy(hook_response + response_index,
                        adn_resp_ptr->ad_number[anr_index], anr_length);
            response_index += anr_length;
        }
    }

    hook_response[response_index] = '\0';

    qcril_hook_unsol_response(instance_id, QCRIL_EVT_HOOK_UNSOL_ADN_RECORDS_IND,
                     (void *)hook_response, response_length);
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_record_read_ext_ind_hdlr

===========================================================================*/
/*!
    @brief
    Handle QMI_PBM_RECORD_READ_EXT_IND_V01.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_record_read_ext_ind_hdlr
(
  void *ind_data_ptr,
  uint32 ind_data_len,
  qcril_instance_id_e_type   instance_id
)
{
    unsigned int array_index = 0;
    int m_element_index, element_index, session_start_offset = 0;

    pbm_record_read_ext_ind_msg_v01 *pbm_read_ind;
    RIL_AdnRecord_v1    adn_record_resp;

    QCRIL_LOG_FUNC_ENTRY();

    QCRIL_NOTUSED( ind_data_len );

    memset(&adn_record_resp, 0, sizeof(RIL_AdnRecord_v1));

    if ( ind_data_ptr != NULL )
    {
        pbm_read_ind = (pbm_record_read_ext_ind_msg_v01*)ind_data_ptr;

        //session type
        QCRIL_LOG_INFO("Session type of record in ADN is: %d",
                            (int)pbm_read_ind->basic_record_data.session_type);

        //record count
        adn_record_resp.record_elements = pbm_read_ind->basic_record_data.record_instances_len;
        QCRIL_LOG_INFO("Number of ADN record in the read indication: %d",
                                                adn_record_resp.record_elements);

        //support multi-session
        for ( element_index = 0; element_index < pbm_cache_info.pbm_num_session; element_index++ )
        {
            if(pbm_read_ind->basic_record_data.session_type == pbm_cache_info.pbm_session_type[element_index])  {
                for ( m_element_index = 0; element_index>0 && m_element_index < element_index; m_element_index++ ) {
                  session_start_offset += pbm_cache_info.pbm_capacity[m_element_index].max_adn_num;
                }
                break;
             }
        }

        QCRIL_LOG_INFO("The offset of ADN record for this session is: %d", session_start_offset);

        //basic adn record info
        for (element_index = 0; element_index < adn_record_resp.record_elements; element_index++)
        {
            RIL_AdnRecordInfo* adn_resp_ptr = & adn_record_resp.adn_record_info[element_index];
            pbm_record_instance_ext_type_v01 basic_record_info =
                                pbm_read_ind->basic_record_data.record_instances[element_index];
            memset(adn_resp_ptr, 0, sizeof(RIL_AdnRecordInfo));

            //record id
            adn_resp_ptr->record_id = basic_record_info.record_id + session_start_offset;
            QCRIL_LOG_INFO("Index of record in ADN: %d", adn_resp_ptr->record_id);

            //name
            adn_resp_ptr->name= qcril_qmi_pbm_alloc_to_utf8(
                                    (const uint8*) basic_record_info.name,
                                    basic_record_info.name_len,
                                    TRUE);

            QCRIL_LOG_INFO("Name in the record: %s", adn_resp_ptr->name);

            //number
            adn_resp_ptr->number = qcril_qmi_pbm_alloc_to_utf8(
                                    (const uint8*) basic_record_info.number,
                                    basic_record_info.number_len,
                                    FALSE);
            QCRIL_LOG_INFO("Number in the record: %s", adn_resp_ptr->number);

            QCRIL_LOG_INFO("Number type in the record: %d", (int)basic_record_info.num_type);
            QCRIL_LOG_INFO("Number plan in the record: %d", (int)basic_record_info.num_plan);
        }

        //email info
        if(pbm_read_ind->email_info_array_valid && pbm_read_ind->email_info_array_len > 0)
        {
            for (array_index = 0; array_index < pbm_read_ind->email_info_array_len; array_index++)
            {
                pbm_email_record_info_ext_type_v01* email_info_ptr =
                                                        & pbm_read_ind->email_info_array[array_index];
                QCRIL_LOG_INFO("email_info_array_len is : %d", pbm_read_ind->email_info_array_len);

                //match the record id
                for(element_index = 0; element_index < adn_record_resp.record_elements; element_index++)
                {
                    if(adn_record_resp.adn_record_info[element_index].record_id == email_info_ptr->record_id + session_start_offset)
                    {
                         //support multi emails
                        adn_record_resp.adn_record_info[element_index].email_elements =
                                                            email_info_ptr->email_len;
                        QCRIL_LOG_INFO("email record id is : %d", email_info_ptr->record_id);
                        QCRIL_LOG_INFO("email_elements is : %d",
                                                adn_record_resp.adn_record_info[element_index].email_elements);
                        for(m_element_index = 0; m_element_index < adn_record_resp.adn_record_info[element_index].email_elements; m_element_index++)
                        {
                            adn_record_resp.adn_record_info[element_index].email[m_element_index] =
                                            qcril_qmi_pbm_alloc_to_utf8(
                                            (const uint8*) email_info_ptr->email[m_element_index].email_address,
                                            email_info_ptr->email[m_element_index].email_address_len,
                                            TRUE);

                            QCRIL_LOG_INFO("Email in the record : %s",
                                                    email_info_ptr->email[m_element_index].email_address);
                        }
                        break;
                    }
                }
            }
        } else {
            QCRIL_LOG_INFO("This group of ADN records unsupport or exclude the email");
        }

        //additional number info
        if(pbm_read_ind->ad_num_info_array_valid && pbm_read_ind->ad_num_info_array_len > 0)
        {
            for (array_index = 0; array_index < pbm_read_ind->ad_num_info_array_len; array_index++)
            {
                pbm_ad_num_record_info_type_v01* ad_num_info_ptr = & pbm_read_ind->ad_num_info_array[array_index];
                QCRIL_LOG_INFO("ad_num_info_array_len is : %d", pbm_read_ind->ad_num_info_array_len);

                //match the record id
                for(element_index = 0; element_index < adn_record_resp.record_elements; element_index++)
                {
                    if(adn_record_resp.adn_record_info[element_index].record_id == ad_num_info_ptr->record_id + session_start_offset)
                    {
                         //support multi additional numbers
                        adn_record_resp.adn_record_info[element_index].anr_elements = ad_num_info_ptr->ad_nums_len;
                        QCRIL_LOG_INFO("anr record id is : %d", ad_num_info_ptr->record_id);
                        QCRIL_LOG_INFO("anr_elements is : %d",
                                                adn_record_resp.adn_record_info[element_index].anr_elements);
                        for(m_element_index = 0; m_element_index < adn_record_resp.adn_record_info[element_index].anr_elements; m_element_index++)
                        {
                            adn_record_resp.adn_record_info[element_index].ad_number[m_element_index] =
                                            qcril_qmi_pbm_alloc_to_utf8(
                                            (const uint8*) ad_num_info_ptr->ad_nums[m_element_index].ad_number,
                                            ad_num_info_ptr->ad_nums[m_element_index].ad_number_len,
                                            FALSE);

                            QCRIL_LOG_INFO("AdNumber in the record : %s",
                                                    ad_num_info_ptr->ad_nums[m_element_index].ad_number);
                            QCRIL_LOG_INFO("Number type of AdNumber in the record: %d", (int)ad_num_info_ptr->ad_nums[m_element_index].num_type);
                            QCRIL_LOG_INFO("Number plan of AdNumber in the record: %d", (int)ad_num_info_ptr->ad_nums[m_element_index].num_plan);
                        }
                        break;
                    }
                }
            }
        } else {
            QCRIL_LOG_INFO("This group of ADN records unsupport or exclude the additional number");
        }

        /* Send unsolicited response to framework */
        qcril_qmi_pbm_record_read_ext_hook_unsol_rsp(instance_id, &adn_record_resp);

        qcril_qmi_pbm_free_adn_record_info(&adn_record_resp);
    }

    QCRIL_LOG_FUNC_RETURN();
}

RIL_AdnRecordInfo* qcril_qmi_pbm_fillin_changed_adn_record
(
  uint8_t *data,
  uint32 data_len,
  RIL_AdnRecordInfo* record_data_ptr
)
{
    uint16_t data_index, email_index, anr_index, email_elements, anr_elements;
    uint16_t name_length, number_length, email_length, anr_length;

    if ((NULL != data) && (1 < data_len))
    {
      data_index = 0;

      record_data_ptr->record_id = data[data_index++];
      record_data_ptr->record_id += data[data_index++]<<8;
      QCRIL_LOG_INFO("record_id is %d", record_data_ptr->record_id);

      name_length = data[data_index++];
      name_length += data[data_index++]<<8;
      QCRIL_LOG_INFO("name_length is %d", name_length);

      if(name_length > 0)
      {
          record_data_ptr->name = (char *)data + data_index;
          name_length ++;
      }
      else
      {
          record_data_ptr->name = NULL;
      }
      data_index += name_length;
      QCRIL_LOG_INFO("name is %s", record_data_ptr->name);

      number_length = data[data_index++];
      number_length += data[data_index++]<<8;
      QCRIL_LOG_INFO("number_length is %d", number_length);

      if(number_length > 0)
      {
          record_data_ptr->number = (char *)data + data_index;
          number_length ++;
      }
      else
      {
          record_data_ptr->number = NULL;
      }
      data_index += number_length;
      QCRIL_LOG_INFO("number is %s", record_data_ptr->number);

      email_elements = data[data_index++];
      email_elements += data[data_index++]<<8;
      record_data_ptr->email_elements = email_elements;
      QCRIL_LOG_INFO("email_elements is %d", email_elements);
      for(email_index = 0; email_index < email_elements; email_index++)
      {
        email_length = data[data_index++];
        email_length += data[data_index++]<<8;

        record_data_ptr->email[email_index] = (char *)data + data_index;
        data_index += email_length + 1;
      }

      anr_elements = data[data_index++];
      anr_elements += data[data_index++]<<8;
      record_data_ptr->anr_elements = anr_elements;
      QCRIL_LOG_INFO("anr_elements is %d", anr_elements);
      for(anr_index = 0; anr_index < anr_elements; anr_index++)
      {
        anr_length = data[data_index++];
        anr_length += data[data_index++]<<8;

        record_data_ptr->ad_number[anr_index] = (char *)data + data_index;
        data_index += anr_length + 1;
      }

      return record_data_ptr;
    }
    else
    {
      QCRIL_LOG_ERROR("parameter error");
      return NULL;
    }
}
/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_update_adn_record_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_HOOK_UPDATE_ADN_RECORD.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_update_adn_record_hndlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
)
{
    RIL_Errno                             ril_req_res = RIL_E_GENERIC_FAILURE;
    qcril_instance_id_e_type        instance_id;
    const RIL_AdnRecordInfo     * in_ptr       = NULL;
    RIL_AdnRecordInfo     record_data;
    qcril_request_resp_params_type        resp;
    qcril_reqlist_public_type qcril_req_info;
    int              record_id = 0, index = 0, sum = 0;

    QCRIL_NOTUSED( ret_ptr );

    QCRIL_LOG_FUNC_ENTRY();

    instance_id = params_ptr->instance_id;
    if (instance_id >= QCRIL_MAX_INSTANCE_ID)
    {
        QCRIL_LOG_ERROR("Invalid value, instance_id: 0x%x", instance_id);
        QCRIL_ASSERT(0);
        return;
    }

    in_ptr = qcril_qmi_pbm_fillin_changed_adn_record(
                                    (uint8_t *) params_ptr->data,
                                    (uint32) params_ptr->datalen,
                                     &record_data);

    /* Return with error if data was not provided */
    if (in_ptr == NULL)
    {
      QCRIL_LOG_ERROR( "%s", " Invalid input for data \n");

      qcril_default_request_resp_params( instance_id,
                               params_ptr->t,
                               params_ptr->event_id,
                               ril_req_res,
                               &resp );

      resp.resp_pkt = &record_id;
      resp.resp_len = sizeof(record_id);
      qcril_send_request_response( &resp );
      return;
    }

    qcril_reqlist_default_entry( params_ptr->t,
                           params_ptr->event_id,
                           QCRIL_DEFAULT_MODEM_ID,
                           QCRIL_REQ_AWAITING_MORE_AMSS_EVENTS,
                           QCRIL_EVT_PBM_ASYNC_CALLBACK,
                           NULL,
                           &qcril_req_info );

    if ( qcril_reqlist_new( QCRIL_DEFAULT_INSTANCE_ID, &qcril_req_info ) == E_SUCCESS )
    {
        record_id = in_ptr->record_id;
        //support multi-session
        if(record_id == 0) {
            //select the correct session type to add ADN
            for ( index = 0; index < pbm_cache_info.pbm_num_session; index++ )
            {
                if(pbm_cache_info.pbm_capacity[index].valid_adn_num
                        < pbm_cache_info.pbm_capacity[index].max_adn_num)
                    break;
            }
        }
        else if (record_id > 0)
        {
            //select the correct session type to delete/modify ADN
            for ( index = 0; index < pbm_cache_info.pbm_num_session; index++ )
            {
                sum += pbm_cache_info.pbm_capacity[index].max_adn_num;

                QCRIL_LOG_INFO("Sum is %d, index is %d", sum, index);
                if(record_id <= sum){
                    record_id -=  sum - pbm_cache_info.pbm_capacity[index].max_adn_num;
                    break;
                }
            }
        }

        if(index < pbm_cache_info.pbm_num_session) {
            if((in_ptr->name == NULL || strlen(in_ptr->name) == 0)
            && (in_ptr->number == NULL || strlen(in_ptr->number) == 0)
            && in_ptr->email_elements == 0 && in_ptr->anr_elements == 0)
            {
                ril_req_res = qcril_qmi_pbm_delete_adn_record(record_id,
                                        pbm_cache_info.pbm_session_type[index]);
            }
            else
            {
                ril_req_res = qcril_qmi_pbm_add_or_modify_adn_record(in_ptr,
                                        record_id,
                                        pbm_cache_info.pbm_session_type[index]);
            }
        }
        else
        {
            QCRIL_LOG_INFO("Could not find the correct session type");
        }
    }
    else
    {
        QCRIL_LOG_ERROR("No memory to allocate reqlist");
    }

    if(RIL_E_SUCCESS != ril_req_res)
    {
       QCRIL_LOG_INFO("Update request for ADN failed");
        qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                           params_ptr->t,
                                           params_ptr->event_id,
                                           ril_req_res,
                                           &resp );

        resp.resp_pkt = &record_id;
        resp.resp_len = sizeof(record_id);
        qcril_send_request_response( &resp );
    }
    QCRIL_LOG_FUNC_RETURN();
}

/*=========================================================================
  FUNCTION:  qcril_qmi_pbm_async_cb_helper_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_PBM_ASYNC_CALLBACK.

    @return
    None.
*/
/*=========================================================================*/
void qcril_qmi_pbm_async_cb_helper_hndlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type *const ret_ptr
)
{

  RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
  qmi_resp_callback_type * qmi_resp_callback;
  qcril_request_resp_params_type resp;
  errno_enum_type found_qcril_request;
  qcril_reqlist_public_type qcril_req_info;
  unsigned int  msg_id;
  int session_type, record_id = 0;
  int j, k, session_start_offset = 0, index = 0;

  QCRIL_NOTUSED( ret_ptr );

  QCRIL_LOG_FUNC_ENTRY();

  qmi_resp_callback = (qmi_resp_callback_type *) params_ptr->data;

  if(qmi_resp_callback->data_buf == NULL)
  {
    QCRIL_LOG_ERROR("%s", "NULL pointer");
    QCRIL_ASSERT(0);
    return;
  }

  if(qmi_resp_callback->cb_data == NULL)
  {
    QCRIL_LOG_ERROR("%s: resp_cb_data was not valid", __FUNCTION__);
    return;
  }

  session_type = ((qcril_pbm_cb_struct_type*) qmi_resp_callback->cb_data)->session_type;
  record_id = ((qcril_pbm_cb_struct_type*) qmi_resp_callback->cb_data)->record_index;


  found_qcril_request = qcril_reqlist_query_by_event( QCRIL_DEFAULT_INSTANCE_ID,
                        QCRIL_DEFAULT_MODEM_ID,
                        QCRIL_EVT_PBM_ASYNC_CALLBACK,
                        &qcril_req_info );

  msg_id = qmi_resp_callback->msg_id;

  /* call the response conversion function based on message id */
  switch (msg_id)
  {
    case QMI_PBM_DELETE_RECORD_RESP_V01:
      if ( E_SUCCESS == found_qcril_request )
      {
        pbm_delete_record_resp_msg_v01* pbm_delete_resp_ptr =
                      (pbm_delete_record_resp_msg_v01 *) qmi_resp_callback->data_buf;

        ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                                        qmi_resp_callback->transp_err,
                                                        &pbm_delete_resp_ptr->resp );
        QCRIL_LOG_INFO("session_type is %d", (int)session_type);
        QCRIL_LOG_INFO("pbm_delete_resp.record_id_valid is %d",
                                          (int)pbm_delete_resp_ptr->record_id_valid);

        if ( RIL_E_SUCCESS == ril_req_res && pbm_delete_resp_ptr->record_id_valid
           && record_id == pbm_delete_resp_ptr->record_id)
        {
          QCRIL_LOG_INFO("The ADN record in %d has already been deleted", record_id);
          for ( index = 0; index < pbm_cache_info.pbm_num_session; index++ )
          {
            if(session_type == pbm_cache_info.pbm_session_type[index])
            {
               PBM_CACHE_LOCK();
               pbm_cache_info.pbm_capacity[index].valid_adn_num --;
               PBM_CACHE_UNLOCK();
               break;
            }
          }

        }
      }
      break;
    case QMI_PBM_WRITE_RECORD_EXT_RESP_V01:
        if ( E_SUCCESS == found_qcril_request )
        {
          pbm_write_record_ext_resp_msg_v01* pbm_write_resp_ptr =
                        (pbm_write_record_ext_resp_msg_v01 *) qmi_resp_callback->data_buf;

          ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(
                                                          qmi_resp_callback->transp_err,
                                                          &pbm_write_resp_ptr->resp );

          QCRIL_LOG_INFO("session_type is %d", (int)session_type);
          QCRIL_LOG_INFO("pbm_write_ext_resp.record_id_valid is %d",
                                            (int)pbm_write_resp_ptr->record_id_valid);

          if ( RIL_E_SUCCESS == ril_req_res && pbm_write_resp_ptr->record_id_valid )
          {
            if(record_id == 0) {
              record_id = pbm_write_resp_ptr->record_id;
              QCRIL_LOG_INFO("The ADN record in %d has already been added", record_id);
              for ( index = 0; index < pbm_cache_info.pbm_num_session; index++ )
              {
                if(session_type == pbm_cache_info.pbm_session_type[index])
                {
                  PBM_CACHE_LOCK();
                  pbm_cache_info.pbm_capacity[index].valid_adn_num ++;
                  PBM_CACHE_UNLOCK();
                  break;
                }
              }
            }
            else if(record_id == pbm_write_resp_ptr->record_id)
            {
                QCRIL_LOG_INFO("The ADN record in %d has already been modified", record_id);
            }
          }
        }
      break;
    default:
      QCRIL_LOG_ERROR("qmi_pbm_client_async_cb: unknown rsp ID = %x", (unsigned int)msg_id);
      break;
   }

  if (found_qcril_request == E_SUCCESS)
  {
    qcril_default_request_resp_params( QCRIL_DEFAULT_INSTANCE_ID,
                                       qcril_req_info.t,
                                       qcril_req_info.request,
                                       ril_req_res,
                                       &resp );

    //support multi-session
    for ( j = 0; j < pbm_cache_info.pbm_num_session; j++ )
    {
        if(session_type == pbm_cache_info.pbm_session_type[j])  {
            for ( k = 0; j>0 && k < j; k++ ) {
              session_start_offset += pbm_cache_info.pbm_capacity[k].max_adn_num;
            }
            break;
         }
    }
    QCRIL_LOG_INFO("The offset of ADN record for this session is: %d", session_start_offset);

    record_id += session_start_offset;

    resp.resp_pkt = &record_id;
    resp.resp_len = sizeof(record_id);
    qcril_send_request_response( &resp );
  } else {
     QCRIL_LOG_ERROR( "pending event not found %d", (int) found_qcril_request );
  }

  qcril_free(qmi_resp_callback->cb_data);
  qcril_free(qmi_resp_callback->data_buf);

  QCRIL_LOG_FUNC_RETURN();
}

void qcril_qmi_pbm_client_async_cb
(
  qmi_client_type              user_handle,
  unsigned int                 msg_id,
  void                         *resp_c_struct,
  unsigned int                 resp_c_struct_len,
  void                         *resp_cb_data,
  qmi_client_error_type        transp_err
)
{
  qmi_resp_callback_type qmi_resp_callback;

  QCRIL_LOG_FUNC_ENTRY();

  qmi_resp_callback.user_handle = user_handle;
  qmi_resp_callback.msg_id = msg_id;
  qmi_resp_callback.data_buf = (void*) resp_c_struct;
  qmi_resp_callback.data_buf_len = resp_c_struct_len;
  qmi_resp_callback.cb_data = resp_cb_data;
  qmi_resp_callback.transp_err = transp_err;

  qcril_event_queue( QCRIL_DEFAULT_INSTANCE_ID,
                 QCRIL_DEFAULT_MODEM_ID,
                 QCRIL_DATA_ON_STACK,
                 QCRIL_EVT_PBM_ASYNC_CALLBACK,
                 (void*) &qmi_resp_callback,
                 sizeof(qmi_resp_callback),
                 (RIL_Token) QCRIL_TOKEN_ID_INTERNAL );

  QCRIL_LOG_FUNC_RETURN();
}/* qcril_qmi_nas_perform_network_scan_command_cb */

RIL_Errno qcril_qmi_pbm_delete_adn_record(int record_id, int session_type)
{
    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
    qmi_client_error_type qmi_transport_error;

    pbm_delete_record_req_msg_v01 pbm_delete_req;
    pbm_delete_record_resp_msg_v01 * pbm_delete_resp_ptr = NULL;
    qcril_pbm_cb_struct_type *pbm_rep_helper = NULL;
    qmi_txn_handle txn_handle;

    QCRIL_LOG_FUNC_ENTRY();

    pbm_delete_resp_ptr = qcril_malloc( sizeof( *pbm_delete_resp_ptr ) );

    if ( pbm_delete_resp_ptr )
    {
      memset(&pbm_delete_req, 0, sizeof(pbm_delete_req));
      memset(pbm_delete_resp_ptr, 0, sizeof( *pbm_delete_resp_ptr ));

      pbm_rep_helper = qcril_malloc( sizeof( *pbm_rep_helper ) );
      if ( pbm_rep_helper )
      {
        pbm_delete_req.record_info.pb_type = PBM_PB_TYPE_ADN_V01;
        pbm_delete_req.record_info.session_type = (pbm_session_type_enum_v01)session_type;
        pbm_delete_req.record_info.record_id = record_id;

        pbm_rep_helper->session_type = session_type;
        pbm_rep_helper->record_index = record_id;

        QCRIL_LOG_INFO("Record id to delete is %d", pbm_delete_req.record_info.record_id);
        QCRIL_LOG_INFO("Pb type is %d, session type is %d", pbm_delete_req.record_info.pb_type,
                                                        pbm_delete_req.record_info.session_type);

        qmi_transport_error =  qmi_client_send_msg_async_with_shm(
                                        qcril_qmi_client_get_user_handle( QCRIL_QMI_CLIENT_PBM ),
                                        QMI_PBM_DELETE_RECORD_REQ_V01,
                                        &pbm_delete_req,
                                        sizeof(pbm_delete_req),
                                        pbm_delete_resp_ptr,
                                        sizeof(*pbm_delete_resp_ptr),
                                        qcril_qmi_pbm_client_async_cb,
                                        (void *)pbm_rep_helper,
                                        &txn_handle );


        ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_transport_error,
                                                                    &(pbm_delete_resp_ptr->resp));
      }
    }

    if ( RIL_E_SUCCESS != ril_req_res )
    {
        if(pbm_delete_resp_ptr != NULL)
        {
            QCRIL_LOG_INFO("Delete request for ADN failed with error code = %d",
                                            (int)pbm_delete_resp_ptr->resp.error);
            qcril_free( pbm_delete_resp_ptr );
        }

        if(pbm_rep_helper != NULL)
        {
            qcril_free( pbm_rep_helper );
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return ril_req_res;
}

RIL_Errno qcril_qmi_pbm_add_or_modify_adn_record
(
    const RIL_AdnRecordInfo* record_info_ptr,
    int record_id,
    int session_type
)
{
    RIL_Errno ril_req_res = RIL_E_GENERIC_FAILURE;
    qmi_client_error_type qmi_transport_error;
    pbm_write_record_ext_req_msg_v01  pbm_write_req;
    pbm_write_record_ext_resp_msg_v01 * pbm_write_resp_ptr = NULL;
    qcril_pbm_cb_struct_type * pbm_rep_helper = NULL;
    qmi_txn_handle txn_handle;
    unsigned int i;

    QCRIL_LOG_FUNC_ENTRY();

    pbm_write_resp_ptr = qcril_malloc( sizeof( *pbm_write_resp_ptr ) );

    if ( pbm_write_resp_ptr )
    {
      memset(&pbm_write_req, 0, sizeof(pbm_write_req));
      memset(pbm_write_resp_ptr, 0, sizeof( *pbm_write_resp_ptr ));

      pbm_rep_helper = qcril_malloc( sizeof( *pbm_rep_helper ) );
      if ( pbm_rep_helper )
      {
        pbm_write_req.record_info.pb_type = PBM_PB_TYPE_ADN_V01;
        pbm_write_req.record_info.session_type = (pbm_session_type_enum_v01)session_type;
        pbm_write_req.record_info.record_id = record_id;

        pbm_rep_helper->session_type = session_type;
        pbm_rep_helper->record_index = record_id;

        QCRIL_LOG_INFO("Record id to write is %d", pbm_write_req.record_info.record_id);
        QCRIL_LOG_INFO("Pb type is %d, session type is %d", pbm_write_req.record_info.pb_type,
                                                        pbm_write_req.record_info.session_type);

        if(record_info_ptr->name != NULL)
        {
            pbm_write_req.name_valid = TRUE;
            pbm_write_req.name_len = qcril_cm_ss_convert_utf8_to_ucs2(
                                        (char*) record_info_ptr->name,
                                        strlen(record_info_ptr->name),
                                        (char*) pbm_write_req.name,
                                        sizeof(pbm_write_req.name));
        }
        QCRIL_LOG_INFO("Name in the record: %s", pbm_write_req.name);

        if(record_info_ptr->number != NULL)
        {
            pbm_write_req.num_valid = TRUE;
            pbm_write_req.num.num_plan= PBM_NUM_PLAN_ISDN_V01;
            pbm_write_req.num.number_len = strlen(record_info_ptr->number);
            memcpy(pbm_write_req.num.number, record_info_ptr->number,
                                        pbm_write_req.num.number_len);
        }
        QCRIL_LOG_INFO("Number in the record: %s", pbm_write_req.num.number);

        if(record_info_ptr->email_elements > 0)
        {
            pbm_write_req.email_info_valid = TRUE;
            pbm_write_req.email_info_len = MIN((unsigned int) record_info_ptr->email_elements,
                                                sizeof(pbm_write_req.email_info));
            for (i = 0; (i < pbm_write_req.email_info_len &&
                  i < QMI_PBM_EMAIL_INFO_MAX_V01 && i < RIL_MAX_NUM_EMAIL_COUNT); i++)
            {
                pbm_write_req.email_info[i].email_address_len =
                                            qcril_cm_ss_convert_utf8_to_ucs2(
                                                record_info_ptr->email[i],
                                                strlen(record_info_ptr->email[i]),
                                                (char*) pbm_write_req.email_info[i].email_address,
                                                sizeof(pbm_write_req.email_info[i].email_address));

                QCRIL_LOG_INFO("Email in the record of index %d: %s", i, pbm_write_req.email_info[i].email_address);
            }
        }
        QCRIL_LOG_INFO("Email elements in the record: %d", pbm_write_req.email_info_len);

        if(record_info_ptr->anr_elements > 0)
        {
            pbm_write_req.ad_num_info_valid = TRUE;
            pbm_write_req.ad_num_info_len = MIN((unsigned int) record_info_ptr->anr_elements,
                                                sizeof(pbm_write_req.ad_num_info));
            for (i = 0; (i < pbm_write_req.ad_num_info_len && i < QMI_PBM_AD_NUM_MAX_V01 && i< RIL_MAX_NUM_AD_COUNT); i++)
            {
                pbm_write_req.ad_num_info[i].num_plan= PBM_NUM_PLAN_ISDN_V01;
                pbm_write_req.ad_num_info[i].ad_number_len =
                                                strlen(record_info_ptr->ad_number[i]);
                memcpy(pbm_write_req.ad_num_info[i].ad_number,
                        record_info_ptr->ad_number[i],
                        pbm_write_req.ad_num_info[i].ad_number_len);

                QCRIL_LOG_INFO("Anr length in the record of index %d: %d", i, pbm_write_req.ad_num_info[i].ad_number_len);
                QCRIL_LOG_INFO("Anr in the record of index %d: %s", i, pbm_write_req.ad_num_info[i].ad_number);
            }
        }
        QCRIL_LOG_INFO("Anr elements in the record: %d", pbm_write_req.ad_num_info_len);

        qmi_transport_error =  qmi_client_send_msg_async_with_shm(
                                        qcril_qmi_client_get_user_handle( QCRIL_QMI_CLIENT_PBM ),
                                        QMI_PBM_WRITE_RECORD_EXT_REQ_V01,
                                        &pbm_write_req,
                                        sizeof(pbm_write_req),
                                        pbm_write_resp_ptr,
                                        sizeof(*pbm_write_resp_ptr),
                                        qcril_qmi_pbm_client_async_cb,
                                        (void *)pbm_rep_helper,
                                        &txn_handle );


        ril_req_res = qcril_qmi_util_convert_qmi_response_codes_to_ril_result(qmi_transport_error, NULL);
      }
    }

    if ( RIL_E_SUCCESS != ril_req_res )
    {
         if(pbm_write_resp_ptr != NULL)
        {
            QCRIL_LOG_INFO("Write request for ADN failed with error code = %d",
                                            (int)pbm_write_resp_ptr->resp.error);
            qcril_free( pbm_write_resp_ptr );
        }

        if(pbm_rep_helper != NULL)
        {
            qcril_free( pbm_rep_helper );
        }
    }

    QCRIL_LOG_FUNC_RETURN();
    return ril_req_res;
}

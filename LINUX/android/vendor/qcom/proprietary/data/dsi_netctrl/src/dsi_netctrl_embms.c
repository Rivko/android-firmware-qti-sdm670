/*!
  @file
  dsi_netctrl_embms.c

  @brief
  This file provides the functionality to control the embedded EMBMS data
  call by talking to the modem via QMI service(s).

*/

/*===========================================================================

  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/11   sy      Initial Version

===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#ifdef DSI_NETCTRL_OFFTARGET
#include <sys/socket.h>
#include <string.h>
#include "ds_sl_list.h"
#else
#include <linux/socket.h>
#endif

#include "dsi_netctrl.h"
#include "dsi_netctrli.h" /* declarations shared internally */
#include "dsi_netctrl_cb_thrd.h"
#include "dsi_netctrl_mni.h"

/*---------------------------------------------------------------------------
                           DECLARATIONS
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION:  dsi_embms_enable
===========================================================================*/
/*!
    @brief
    This function can be used to start an EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_embms_enable
(
  dsi_hndl_t hndl
)
{
  dsi_store_t             *st_hndl = NULL;
  int                      ret = DSI_ERROR, i = 0;
  dsi_call_param_value_t   ext_tech_pref;

  DSI_LOG_DEBUG( "%s", "dsi_embms_enable: ENTRY" );

  /* set EMBMS parameter */
  ext_tech_pref.buf_val = NULL;
  ext_tech_pref.num_val = DSI_EXT_TECH_EMBMS;

  /* validation check is inside dsi_set_data_call_param,
     nothing done here */
  if ( ( dsi_set_data_call_param( hndl,
                                  DSI_CALL_INFO_EXT_TECH_PREF,
                                  &ext_tech_pref ) ) != DSI_SUCCESS )
  {
    DSI_LOG_ERROR("%s", "unable to set extended tech pref ");
    return DSI_ERROR;
  }

  DSI_GLOBAL_LOCK;

  /* this do..while loop decides the overall return value.
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    st_hndl = ( (dsi_store_t *) hndl );

    if (!dsi_inited)
    {
      DSI_LOG_ERROR("%s","dsi_embms_enable: dsi not inited");
      break;
    }

    if (!(DSI_IS_HNDL_VALID(st_hndl)))
    {
      DSI_LOG_ERROR( "dsi_embms_enable invalid arg, st_hndl [%p]",
                       (unsigned int*)st_hndl );
      break;
    }

    ret = dsi_mni_vtbl.mni_look_up_f(st_hndl, &i);
    if (DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("%s", "dsi_route_look_up failed");
      break;
    }

    if( !DSI_IS_WDS_HNDL_VALID(i) && ( !DSI_IS_QOS_HNDL_VALID(i) ))
    {
      dsi_mni_init_client(i);
    }

    /* at this point, we know which dsi iface (i.e. qmi port) - i
       to issue embms_indication_registration and wds_start_nw_iface */

    DSI_LOG_DEBUG("issuing register_embms_ind on dsi_iface [%d]", i);

    ret = dsi_mni_vtbl.mni_register_embms_ind_f(i, st_hndl);
    if (DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("%s", "dsi_register_embms_ind failed");
      break;
    }

    DSI_LOG_DEBUG("issuing start_nw_if on dsi_iface [%d]", i);

    ret = dsi_mni_vtbl.mni_start_f(i, st_hndl);
    if(DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("dsi_mni_vtbl.mni_start_f [%p] returned err",
                    (unsigned int*)dsi_mni_vtbl.mni_start_f);
      /*TODO: add details if there is requirement in the future
      DSI_LOG_ERROR("dsi call end reason code/type is [%d/%d]",
                    st_hndl->priv.ce_reason.reason_code,
                    st_hndl->priv.ce_reason.reason_type);
      */

      dsi_mni_release_client(i);

      break;
    }
    else
    {
      /*
         now that we have successfully issued start_nw_if()
         on this interface for the given st_hndl, attach
         this interface to the st_hndl and put it's state
         to IN_USE, so doesn't get stepped on inadvertently
         by another call (store handle)
      */
      DSI_LOCK_MUTEX(&(st_hndl->priv.mutex));
      dsi_attach_dsi_iface(i, st_hndl);
      DSI_UPDATE_CALL_STATE(st_hndl, DSI_STATE_CALL_CONNECTING);
      DSI_UNLOCK_MUTEX(&(st_hndl->priv.mutex));
    }

    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_enable: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_enable: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_embms_tmgi_activate
===========================================================================*/
/*!
    @brief
    This function can be used to activate TMGI on EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_embms_tmgi_activate
(
  dsi_hndl_t                          hndl,
  char                               *tmgi,
  unsigned int                       *earfcnlist,
  unsigned char                       earfcnlist_len,
  unsigned long                       preemption_priority,
  unsigned int                       *sailist,
  unsigned char                       sailist_len,
  int                                 dbg_trace_id
)
{
  dsi_store_t    *st_hndl = NULL;
  int             ret = DSI_ERROR;
  int             i;
  int             itr;
  unsigned char   temp_len;

  DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_activate: ENTRY" );

  DSI_GLOBAL_LOCK;

  st_hndl = ( (dsi_store_t *) hndl );

  /* this do..while loop decides the overall return value.
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (!dsi_inited)
    {
      DSI_LOG_ERROR("%s","dsi_embms_tmgi_activate: dsi not inited");
      break;
    }

    if (!(DSI_IS_HNDL_VALID(st_hndl)) || (NULL == tmgi) || (NULL == earfcnlist))
    {
      DSI_LOG_ERROR( "dsi_embms_activate_tmgi invalid arg, st_hndl [%p],"
                     " or tmgi NULL", (unsigned int*)st_hndl);
      break;
    }

    DSI_LOCK_MUTEX(&(st_hndl->priv.mutex))
    i = st_hndl->priv.dsi_iface_id;
    DSI_UNLOCK_MUTEX(&(st_hndl->priv.mutex))

    if(!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("dsi_embms_tmgi_activate: st_hndl contains" \
                    "invalid id [%d]", i);
      break;
    }

    DSI_LOG_DEBUG( "dsi_embms_tmgi_activate dbg_trace_id:[%d]", dbg_trace_id );

    memset(&st_hndl->priv.embms_tmgi_list_info, 0, sizeof(st_hndl->priv.embms_tmgi_list_info));

    /* copy tmgi_list */
    memcpy(&(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.tmgi.tmgi[0]),
           (void*) tmgi,
            DSI_SIZE_OF_TMGI);

    /* invalidate session_id */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.tmgi.session_id_valid = FALSE;

    /* validate earfcnlist length */
    temp_len = (unsigned char)((earfcnlist_len > DSI_SIZE_OF_EARFCNLIST)?
               DSI_SIZE_OF_EARFCNLIST: earfcnlist_len);

    /* copy earfcnlist length */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.earfcn_list_ex_valid = TRUE;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.earfcn_list_ex_len = temp_len;

    /* copy earfcnlist */
    for(itr = 0; itr < temp_len; itr++)
    {
      st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.earfcn_list_ex[itr] = earfcnlist[itr];
    }

    /* validate sailist length */
    temp_len = (unsigned char)((sailist_len > DSI_EMBMS_SAI_PER_FREQ_MAX_SIZE)?
               DSI_EMBMS_SAI_PER_FREQ_MAX_SIZE : sailist_len);

    /* copy sailist */
    if(0 != temp_len)
    {
      /* empty sai_list is OK to be passed to modem along with TMGI/EARFCNLIST,
         MODEM will treat it as single band request. Set the param_mask only
         when sai_list is not empty
       */
      st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.sai_list_valid = TRUE;
      /* copy sailist length */
      st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.sai_list_len = temp_len;

      memcpy(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.sai_list,
             sailist,
             temp_len * sizeof(unsigned int));
    }

    /* copy preemption priority */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.preempt_priority_valid = TRUE;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.preempt_priority = preemption_priority;

    /* copy debug trace id */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.tranx_id_valid = TRUE;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_req.tranx_id = dbg_trace_id;

    ret = dsi_mni_vtbl.mni_embms_tmgi_activate_f(i, st_hndl);

    if (DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("dsi_mni_vtbl.mni_embms_tmgi_activate_f [%p] returned err",
                    (unsigned int*)dsi_mni_vtbl.mni_embms_tmgi_activate_f);
      break;
    }

    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_activate: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_activate: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_embms_tmgi_deactivate
===========================================================================*/
/*!
    @brief
    This function can be used to deactivate TMGI on EMBMS data call.

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_embms_tmgi_deactivate
(
  dsi_hndl_t                           hndl,
  char                                *tmgi,
  int                                  dbg_trace_id
)
{
  dsi_store_t    *st_hndl = NULL;
  int             ret = DSI_ERROR;
  int             i;

  DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_deactivate: ENTRY" );

  DSI_GLOBAL_LOCK;

  st_hndl = ( (dsi_store_t *) hndl );

  /* this do..while loop decides the overall return value.
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (!dsi_inited)
    {
      DSI_LOG_ERROR("%s","dsi_embms_deactivate_tmgi: dsi not inited");
      break;
    }

    if (!(DSI_IS_HNDL_VALID(st_hndl)) || (NULL == tmgi))
    {
      DSI_LOG_ERROR( "dsi_embms_tmgi_deactivate invalid arg, st_hndl [%p],"
                     " or tmgi NULL", (unsigned int*)st_hndl);
      break;
    }

    DSI_LOCK_MUTEX(&(st_hndl->priv.mutex))
    i = st_hndl->priv.dsi_iface_id;
    DSI_UNLOCK_MUTEX(&(st_hndl->priv.mutex))

    if(!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("dsi_embms_tmgi_deactivate: st_hndl contains" \
                    "invalid id [%d]", i);
      break;
    }

    DSI_LOG_DEBUG( "dsi_embms_deactivate_tmgi dbg_trace_id:[%d]", dbg_trace_id );

    memset(&st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.deact_req, 0, sizeof(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.deact_req));

    memcpy(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.deact_req.tmgi.tmgi,
           (void*)tmgi,
           DSI_SIZE_OF_TMGI);

    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.deact_req.tmgi.session_id_valid = FALSE;

    /* copy debug trace id */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.deact_req.tranx_id_valid = TRUE;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.deact_req.tranx_id = dbg_trace_id;

    ret = dsi_mni_vtbl.mni_embms_tmgi_deactivate_f(i, st_hndl);

    if (DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("dsi_mni_vtbl.mni_embms_tmgi_deactivate_f [%p] returned err",
                    (unsigned int*)dsi_mni_vtbl.mni_embms_tmgi_deactivate_f);

      /* no deactivate failure case, so we don't update deactivate_reason */
      break;
    }

    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_deactivate: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_deactivate: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_embms_tmgi_activate_deactivate
===========================================================================*/
/*!
    @brief
    This function can be used to both activate TMGI and deactivate TMGI on
    EMBMS data call.

    TMGI to be activated must be different than TMGI to be deactivated

    @return
    DSI_ERROR
    DSI_SUCCESS
*/
/*=========================================================================*/
int dsi_embms_tmgi_activate_deactivate
(
  dsi_hndl_t                         hndl,
  char                              *activate_tmgi,
  unsigned int                      *earfcnlist,
  unsigned char                      earfcnlist_len,
  unsigned long                      preemption_priority,
  char                              *deactivate_tmgi,
  unsigned int                      *sailist,
  unsigned char                      sailist_len,
  int                                dbg_trace_id
)
{
  dsi_store_t    *st_hndl = NULL;
  int             ret = DSI_ERROR;
  int             i;
  int             itr;
  unsigned char   temp_len;

  DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_activate_deactivate: ENTRY" );

  DSI_GLOBAL_LOCK;

  st_hndl = ( (dsi_store_t *) hndl );

  /* this do..while loop decides the overall return value.
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (!dsi_inited)
    {
      DSI_LOG_ERROR("%s","dsi_embms_tmgi_activate_deactivate: dsi not inited");
      break;
    }

    if (!(DSI_IS_HNDL_VALID(st_hndl)) || (NULL == activate_tmgi)
        || (NULL == earfcnlist) || (NULL == deactivate_tmgi))
    {
      DSI_LOG_ERROR( "dsi_embms_tmgi_activate_deactivate invalid arg, st_hndl [%p],"
                     " or tmgi NULL", (unsigned int*)st_hndl);
      break;
    }

    /* activate_tmgi must be different than deactivate_tmgi */
    if( 0 == memcmp(activate_tmgi, deactivate_tmgi, DSI_SIZE_OF_TMGI))
    {
      DSI_LOG_ERROR("%s", "activate tmgi is same as deactivate tmgi, INCORRECT\n");
      break;
    }

    DSI_LOCK_MUTEX(&(st_hndl->priv.mutex))
    i = st_hndl->priv.dsi_iface_id;
    DSI_UNLOCK_MUTEX(&(st_hndl->priv.mutex))

    if(!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("dsi_embms_tmgi_activate_deactivate: st_hndl contains" \
                    "invalid id [%d]", i);
      break;
    }

    DSI_LOG_DEBUG( "dsi_embms_tmgi_activate_deactivate dbg_trace_id:[%d]", dbg_trace_id );

    memset(&st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req, 0, sizeof(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req));

    memcpy(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.act_tmgi.tmgi,
           (void*) activate_tmgi,
            DSI_SIZE_OF_TMGI);

    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.act_tmgi.session_id_valid = FALSE;

    memcpy(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.deact_tmgi.tmgi,
           (void*)deactivate_tmgi,
           DSI_SIZE_OF_TMGI);

    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.deact_tmgi.session_id_valid = FALSE;

    /* validate earfcnlist length */
    temp_len = (unsigned char)((earfcnlist_len > DSI_SIZE_OF_EARFCNLIST)?
               DSI_SIZE_OF_EARFCNLIST : earfcnlist_len);
    /* copy earfcnlist length */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.earfcn_list_ex_len = temp_len;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.earfcn_list_ex_valid = TRUE;

    /* copy earfcnlist */
    for(itr = 0; itr < temp_len; itr++)
    {
      st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.earfcn_list_ex[itr] = earfcnlist[itr];
    }

    /* validate sailist length */
    temp_len = (unsigned char)((sailist_len > DSI_EMBMS_SAI_PER_FREQ_MAX_SIZE)?
               DSI_EMBMS_SAI_PER_FREQ_MAX_SIZE : sailist_len);

    /* copy sailist */
    if ( 0 != temp_len)
    {
      /* empty sai_list is OK to be passed to modem along with TMGI/EARFCNLIST,
         MODEM will treat it as single band request. Set the param_mask only
         when sai_list is not empty
       */
      st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.sai_list_valid = TRUE;

      st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.sai_list_len = temp_len;

      memcpy(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.sai_list,
             sailist,
             temp_len * sizeof(unsigned int));
    }


    /* copy preemption priority */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.preempt_priority_valid = TRUE;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.preempt_priority = preemption_priority;

    /* copy debug trace id */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.tranx_id_valid = TRUE;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.act_deact_req.tranx_id = dbg_trace_id;

    ret = dsi_mni_vtbl.mni_embms_tmgi_act_deactivate_f(i, st_hndl);

    if (DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("dsi_mni_vtbl.mni_embms_tmgi_act_deactivate_f [%p] returned err",
                    (unsigned int*)dsi_mni_vtbl.mni_embms_tmgi_act_deactivate_f);
      break;
    }

    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_activate_deactivate: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_activate_deactivate: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;

  return ret;
}
/*===========================================================================
  FUNCTION:  dsi_get_embms_tmgi_deactivated_reason
===========================================================================*/
/*!
    @brief
    used to get tmgi deactivated reason.

    @return
    DSI_ERROR
    DSI_SUCCESS

*/
/*=========================================================================*/
int dsi_embms_get_tmgi_deactivate_reason
(
  dsi_hndl_t                               hndl,
  dsi_embms_tmgi_status_field_type         status_field,
  int                                     *dsi_tmgi_deactivate_reason
)
{

  dsi_store_t *st = (dsi_store_t *)hndl;

  DSI_LOG_DEBUG("%s","dsi_embms_get_tmgi_deactivate_reason ENTRY");
  DSI_GLOBAL_LOCK;

  if (DSI_IS_HNDL_VALID(st) &&
      NULL != dsi_tmgi_deactivate_reason)
  {
    switch(status_field)
    {
      case CHECK_ACTIVATE_STATUS:
      case CHECK_ACTIVATE_DEACTIVATE_STATUS:
        DSI_LOG_DEBUG("returning reason type [%d] on handle [%p]",
                      st->priv.embms_tmgi_list_info.activate_status, hndl);
        *dsi_tmgi_deactivate_reason =
          st->priv.embms_tmgi_list_info.activate_status;
        break;
      case CHECK_DEACTIVATE_STATUS:
        DSI_LOG_DEBUG("returning reason type [%d] on handle [%p]",
                      st->priv.embms_tmgi_list_info.deactivate_status, hndl);
        *dsi_tmgi_deactivate_reason =
          st->priv.embms_tmgi_list_info.deactivate_status;
        break;
      default:
        break;
    }

    DSI_GLOBAL_UNLOCK;
    DSI_LOG_DEBUG("%s","dsi_embms_get_tmgi_deactivate_reason EXIT success");
    return DSI_SUCCESS;
  }

  DSI_LOG_DEBUG("%s","dsi_embms_get_tmgi_deactivate_reason EXIT error");
  DSI_GLOBAL_UNLOCK;

  return DSI_ERROR;
}

/*===========================================================================
  FUNCTION:  dsi_embms_tmgi_list_query
===========================================================================*/
/*!
    @brief
    used to query TMGI list(active or available).

    @return
    DSI_ERROR
    DSI_SUCCESS

*/
/*=========================================================================*/
int dsi_embms_tmgi_list_query
(
  dsi_hndl_t                               hndl,
  wds_embms_tmgi_list_type_enum_v01        embms_tmgi_list_query,
  wds_embms_tmgi_list_query_resp_msg_v01  *list_info,
  int                                      dbg_trace_id
)
{
  dsi_store_t    *st_hndl = NULL;
  int             ret = DSI_ERROR;
  int             i, j;

  DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_list_query: ENTRY" );

  /* validate input parameter */
  if( NULL == list_info)
  {
    DSI_LOG_ERROR("%s", "invalid input parameter");
    return ret;
  }

  DSI_GLOBAL_LOCK;

  st_hndl = ( (dsi_store_t *) hndl );

  /* this do..while loop decides the overall return value.
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (!dsi_inited)
    {
      DSI_LOG_ERROR("%s","dsi_embms_tmgi_list_query: dsi not inited");
      break;
    }

    if (!(DSI_IS_HNDL_VALID(st_hndl)))
    {
      DSI_LOG_ERROR( "dsi_embms_tmgi_list_query invalid arg, st_hndl [%p]" ,
                     (unsigned int*)st_hndl);
      break;
    }

    DSI_LOCK_MUTEX(&(st_hndl->priv.mutex))
    i = st_hndl->priv.dsi_iface_id;
    DSI_UNLOCK_MUTEX(&(st_hndl->priv.mutex))

    if(!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("dsi_embms_tmgi_list_query: st_hndl contains" \
                    "invalid id [%d]", i);
      break;
    }

    DSI_LOG_DEBUG( "dsi_embms_tmgi_list_query type:[%d] dbg_trace_id:[%d]", embms_tmgi_list_query, dbg_trace_id );

    memset(&st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_query_req,
           0,
           sizeof(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_query_req));

    /* copy list type */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_query_req.list_type = embms_tmgi_list_query;

    /* copy debug trace id */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_query_req.tranx_id_valid = TRUE;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_query_req.tranx_id = dbg_trace_id;

    ret = dsi_mni_vtbl.mni_embms_tmgi_list_query_f(i, st_hndl);

    if (DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("dsi_mni_vtbl.mni_embms_tmgi_list_query_f [%p] returned err",
                    (unsigned int*)dsi_mni_vtbl.mni_embms_tmgi_list_query_f);
      break;
    }
    else
    {
      /* prepare copy for qcril_data */
      if(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_info.tmgi_list_valid)
      {
        *list_info = st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.list_info;
      }
    }
    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_list_query: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_tmgi_list_query: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;

  return ret;
}


/*===========================================================================
  FUNCTION:  dsi_embms_content_desc_update
===========================================================================*/
/*!
    @brief
    used to update embms content desc.

    @return
    DSI_ERROR
    DSI_SUCCESS

*/
/*=========================================================================*/
int dsi_embms_content_desc_update
(
  dsi_hndl_t                    hndl,
  char                         *content_desc_update_tmgi,
  unsigned char                 content_desc_valid,
  unsigned int                  content_desc_len,
  dsi_embms_content_desc_type  *content_desc,
  int                           dbg_trace_id
)
{
  dsi_store_t    *st_hndl = NULL;
  int             ret = DSI_ERROR;
  int             i, j;
  unsigned int new_content_desc_len = 0;

  DSI_LOG_DEBUG( "%s", "dsi_embms_content_desc_update: ENTRY" );

  /* validate input parameter */
  if( NULL == content_desc_update_tmgi)
  {
    DSI_LOG_ERROR("%s", "invalid input parameter");
    return ret;
  }

  DSI_GLOBAL_LOCK;

  st_hndl = ( (dsi_store_t *) hndl );

  /* this do..while loop decides the overall return value.
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (!dsi_inited)
    {
      DSI_LOG_ERROR("%s","dsi_embms_content_desc_update: dsi not inited");
      break;
    }

    if (!(DSI_IS_HNDL_VALID(st_hndl)))
    {
      DSI_LOG_ERROR( "dsi_embms_content_desc_update invalid arg, st_hndl [%p]" ,
                     (unsigned int*)st_hndl);
      break;
    }

    DSI_LOCK_MUTEX(&(st_hndl->priv.mutex))
    i = st_hndl->priv.dsi_iface_id;
    DSI_UNLOCK_MUTEX(&(st_hndl->priv.mutex))

    if(!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("dsi_embms_content_desc_update: st_hndl contains" \
                    "invalid id [%d]", i);
      break;
    }

    DSI_LOG_DEBUG( "dsi_embms_content_desc_update dbg_trace_id:[%d]", dbg_trace_id );

    memcpy(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.content_desc_info.tmgi_list.tmgi,
           (void*) content_desc_update_tmgi,
            DSI_SIZE_OF_TMGI);

    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.content_desc_info.tmgi_list.session_id_valid =
      DSI_EMBMS_TMGI_SESSION_ID_NOT_VALID;

    /* copy debug trace id */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.content_desc_info.dbg_trace_id = dbg_trace_id;


    /* copy content desc array if valid */
    if(content_desc_valid != 0 && content_desc != NULL)
    {
      new_content_desc_len = (unsigned char)
            ((content_desc_len > DSI_QMI_WDS_EMBMS_CONTENT_PARAM_NUM_SIZE)?
                            DSI_QMI_WDS_EMBMS_CONTENT_PARAM_NUM_SIZE : content_desc_len);

      if(NULL ==
          (st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.content_desc_info.content_desc_ptr =
              malloc(sizeof (dsi_embms_content_desc_type) * new_content_desc_len)))
      {
        DSI_LOG_ERROR("%s", "dsi_embms_content_desc_update: allocate memory for tmgi failed\n");
        break;
      }

      memcpy(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.content_desc_info.content_desc_ptr,
             (void *)content_desc,
             new_content_desc_len * sizeof (dsi_embms_content_desc_type));
    }

    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.content_desc_info.content_desc_len =
                                                                               new_content_desc_len;

    ret = dsi_mni_vtbl.mni_embms_tmgi_content_desc_update_f(i, st_hndl);

    if (DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("dsi_mni_vtbl.mni_embms_tmgi_content_desc_update_f [%p] returned err",
                    (unsigned int*)dsi_mni_vtbl.mni_embms_tmgi_content_desc_update_f);
      break;
    }
    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_content_desc_update: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_content_desc_update: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;

  return ret;
}

/*===========================================================================
  FUNCTION:  dsi_embms_svc_interest_list
===========================================================================*/
/*!
    @brief
    used to send service interest list

    @return
    DSI_ERROR
    DSI_SUCCESS

*/
/*=========================================================================*/
int dsi_embms_svc_interest_list
(
  dsi_hndl_t          hndl,
  char              **svc_interest_tmgi_list,
  unsigned int        svc_interest_tmgi_list_len,
  int                 dbg_trace_id
)
{
  dsi_store_t    *st_hndl = NULL;
  int             ret = DSI_ERROR;
  unsigned int    i, j;
  unsigned int new_svc_interest_len = 0;

  DSI_LOG_DEBUG( "%s", "dsi_embms_svc_interest_list: ENTRY" );

  DSI_GLOBAL_LOCK;

  st_hndl = ( (dsi_store_t *) hndl );

  /* this do..while loop decides the overall return value.
     set ret to ERROR at the beginning. set ret to SUCCESS
     at the end. If there was an error in the middle, we break out*/
  do
  {
    ret = DSI_ERROR;

    if (!dsi_inited)
    {
      DSI_LOG_ERROR("%s","dsi_embms_svc_interest_list: dsi not inited");
      break;
    }

    if (!(DSI_IS_HNDL_VALID(st_hndl)))
    {
      DSI_LOG_ERROR( "dsi_embms_svc_interest_list invalid arg, st_hndl [%p]" ,
                     (unsigned int*)st_hndl);
      break;
    }

    DSI_LOCK_MUTEX(&(st_hndl->priv.mutex))
    i = st_hndl->priv.dsi_iface_id;
    DSI_UNLOCK_MUTEX(&(st_hndl->priv.mutex))

    if(!DSI_IS_ID_VALID(i))
    {
      DSI_LOG_ERROR("dsi_embms_svc_interest_list: st_hndl contains" \
                    "invalid id [%d]", i);
      break;
    }

    DSI_LOG_DEBUG( "dsi_embms_svc_interest_list dbg_trace_id:[%d]", dbg_trace_id );

    if(svc_interest_tmgi_list != NULL)
    {
      /* copy svc interest tmgi list */
      new_svc_interest_len = (unsigned char)
        ((svc_interest_tmgi_list_len > QMI_WDS_EMBMS_SVC_INTEREST_MAX_V01)?
              QMI_WDS_EMBMS_SVC_INTEREST_MAX_V01 : svc_interest_tmgi_list_len);
    }

    for(j=0; j<new_svc_interest_len; j++)
    {
      memcpy(st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.svc_interest_info.tmgi_list[j].tmgi,
            (void*) svc_interest_tmgi_list[j],
            DSI_SIZE_OF_TMGI);

      st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.svc_interest_info.tmgi_list[j].session_id_valid =
        DSI_EMBMS_TMGI_SESSION_ID_NOT_VALID;

    }

    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.svc_interest_info.tmgi_list_len =
                                                                               new_svc_interest_len;

    /* copy debug trace id */
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.svc_interest_info.tranx_id_valid = TRUE;
    st_hndl->priv.embms_tmgi_list_info.embms_tmgi_params.svc_interest_info.tranx_id = dbg_trace_id;

    ret = dsi_mni_vtbl.mni_embms_tmgi_svc_interest_list_f(i, st_hndl);

    if (DSI_SUCCESS != ret)
    {
      DSI_LOG_ERROR("dsi_mni_vtbl.mni_embms_tmgi_svc_interest_list_f [%p] returned err",
                    (unsigned int*)dsi_mni_vtbl.mni_embms_tmgi_svc_interest_list_f);
      break;
    }
    ret = DSI_SUCCESS;
  } while(0);

  if (DSI_SUCCESS == ret)
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_svc_interest_list: EXIT with suc" );
  }
  else
  {
    DSI_LOG_DEBUG( "%s", "dsi_embms_svc_interest_list: EXIT with err" );
  }

  DSI_GLOBAL_UNLOCK;

  return ret;
}

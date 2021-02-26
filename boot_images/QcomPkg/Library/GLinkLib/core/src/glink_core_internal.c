/*===========================================================================

                 GLINK Internal Utility Functions


 Copyright (c) 2015,2017 by Qualcomm Technologies, Inc.
 All Rights Reserved.
===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/src/glink_core_internal.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/16   sm      Channel list integrity while notifying SSR disconnect.
01/21/16   sm      Avoid holding status lock during complete ssr processing.
01/07/16   ap      Add spss support
12/11/15   sm      Not processing any notification for closed channel.
11/30/15   ap      Handle cases when the channel is not found by rcid or lcid
09/02/15   pp      Disabled logging from RPM xport
08/31/15   an      Fix critical section initialization in add channel func
08/20/15   pp      Added APIs to acquire/release per edge tranport_q_cs locks
08/20/15   pp      Change tx_cs type to os_mutex_type
                   Added definition for glink_acquire/release_edge_lock 
08/17/15   bc      Remove duplicate init of critical section
08/14/15   pp      updated notify_discon to consider GLINK_LOCAL_CH_INIT state
07/10/15   rs      + Moved all statements after local declarations 
                   + Use OS defined macro for unused parameters
07/01/15   bc      Ensure close/reopen protocol
06/19/15   pp      Update to use Ulogs and support for stats and diag commands
06/02/15   bc      Initial version. 
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink_internal.h"

#ifdef FEATURE_TRACER_PACKET
#include "glink_tracer.h"
#endif 
/*===========================================================================
  MACRO DEFINITION
===========================================================================*/
/*===========================================================================
  GLOBAL DATA DECLARATIONS
===========================================================================*/
static os_cs_type glink_transport_q_cs[GLINK_NUM_HOSTS];

#if defined(GLINK_OS_DEFINED_LOGGING) || defined(GLINK_MEMORY_LOGGING)
/* glink_log_filter_status indicates if channel filtering is enabled or not */
glink_logging_filter_cfg_type log_filter_cfg = { 
    FALSE,      /* Filter Disabled */
    "",         /* Filter channel name */
    0x0,        /* Filter lcid - will be updated dynamically */
    0x0,        /* Filter rcid - will be updated dynamically */
    0xFFFFFFFF, /* remote host ID */
    0x0,        /* Channel Context */
    (uint32)~(1 << GLINK_XPORT_RPM_MASK_BIT), /* Enable all xports except RPM */
   };
#endif

/* Keep a list of registered transport for each edge allowed for this host 
 * ***IMPORTANT*** 
 * it should be safe to traverse this list without taking locks */
static smem_list_type glink_registered_transports[GLINK_NUM_HOSTS];

/* List of supported hosts */
const char* glink_hosts_supported[] = { "apss",
                                        "mpss",
                                        "lpass",
                                        "dsps",
                                        "wcnss",
                                        "tz",
                                        "rpm",
                                        "spss",
										"cdsp"
                                      };

#if defined(GLINK_MEMORY_LOGGING)
static glink_mem_log_entry_type glink_mem_log_arr[GLINK_MEM_LOG_SIZE];
static uint32 glink_mem_log_idx = 0;
static os_cs_type glink_mem_log_cs;
#endif

static smem_list_type  glink_link_notif_list;
static os_cs_type      glink_link_state_cs;

/*===========================================================================
  GLOBAL FUNCTION DECLARATION
===========================================================================*/
/*===========================================================================
  LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION      glink_clean_channel_ctx
===========================================================================*/
/**

  This is called when channel is fully closed.
  Clean up the context and redeem channel id if possible.

  @param[in]    xport_ctx    transport context
  @param[in]    channel_ctx  channel context


  @return        None.
  @sideeffects   None.
  @dependencies  This function needs to be called in safe context
                 which is critical sections locked - channel_q_cs
*/
/*=========================================================================*/
static void glink_clean_channel_ctx
(
  glink_core_xport_ctx_type *xport_ctx,
  glink_channel_ctx_type    *channel_ctx
)
{
  /* If logging was enabled for this channel reset the logging filter */
  glinki_update_logging_filter(channel_ctx, TRUE);
  
  smem_list_delete(&xport_ctx->open_list, channel_ctx);

  xport_ctx->channel_cleanup(channel_ctx);
}


/*===========================================================================
  FUNCTION      glinki_xport_priority_comp
===========================================================================*/
/** 
 * Helper function for glinki_get_xport_from_prio. This will be provided to
 * glinki_xports_find function.
 * Return true if given transport has same priority
 *
 * @param[in]    if_ptr     transport interface pointer
 * @param[in]    cond1      unused
 * @param[in]    priority   glink transport priority to find
 *
 * @return       TRUE   if if_ptr priority matches
 *               FALSE  otherwise
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static boolean glinki_xport_priority_comp
(
  glink_transport_if_type *if_ptr,
  void                    *cond1,
  uint32                   priority,
  void                   **out
)
{
  GLINK_OS_UNREFERENCED_PARAM( cond1 );
  GLINK_OS_UNREFERENCED_PARAM( out );

  return if_ptr->glink_priority == (glink_xport_priority)priority;
}

/*===========================================================================
  FUNCTION      glinki_client_requested_xport_check
===========================================================================*/
/**
 * check whether this is client requested xport or not
 *
 * @param[in]    if_ptr     transport interface pointer
 * @param[in]    xport_name transport name to find
 * @param[in]    cond2      unused
 * @param[in]    out        unused
 * 
 * @return       TRUE   if transport name
 *               FALSE  otherwise
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static boolean glinki_client_requested_xport_check
(
  glink_transport_if_type *if_ptr,
  void                    *xport_name,
  uint32                   cond2,
  void                   **out
)
{
  GLINK_OS_UNREFERENCED_PARAM( cond2 );
  GLINK_OS_UNREFERENCED_PARAM( out );
  
  if (!glinki_xport_linkup(if_ptr))
  {
    return FALSE;
  }
  
  return 0 == glink_os_string_compare(if_ptr->glink_core_priv->xport,
                                      (const char *)xport_name);
}

/*===========================================================================
  FUNCTION      glinki_update_best_xport
===========================================================================*/
/** 
 * Check whether given transport (if_ptr) has higher priority than
 * priority user wants to check and update if it is true.
 *
 * @param[in]    if_ptr          transport interface pointer
 * @param[in]    priority_param  xport priority to compare
 * @param[in]    param2          unused
 * @param[out]   best_xport      pointer to result xport
 * 
 * @return       FALSE  all the time since this needs to iterate all transports
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static void glinki_update_best_xport
(
  glink_transport_if_type  *if_ptr,
  void                     *priority_param,
  uint32                    param2,
  void                    **best_xport
)
{
  glink_xport_priority *priority;
  
  GLINK_OS_UNREFERENCED_PARAM( param2 );

  ASSERT(best_xport && priority_param);
  
  priority = (glink_xport_priority*)priority_param;
  
  if (!glinki_xport_linkup(if_ptr))
  {
    return;
  }
  
  /* Given xport is best one if comparing xport doesn't exist */ 
  if (if_ptr->glink_priority < *priority)
  {
    *best_xport = (void*)if_ptr;
    *priority = if_ptr->glink_priority;
  }
}

/*===========================================================================
  FUNCTION      glinki_notify_xport_ssr
===========================================================================*/
/** 
 * Notify this xport of ssr event and do ssr clean up
 *
 * @param[in]    if_ptr         transport interface pointer
 * @param[in]    priority       priority to check
 * @param[in]    cond2          unused
 * @param[out]   updated_xport  pointer to result xport
 * 
 * @return       FALSE  all the time since this needs to iterate all transports
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static void glinki_notify_xport_ssr
(
  glink_transport_if_type  *if_ptr,
  void                     *param1,
  uint32                    param2,
  void                    **out
)
{
  GLINK_OS_UNREFERENCED_PARAM( param1 );
  GLINK_OS_UNREFERENCED_PARAM( param2 );
  GLINK_OS_UNREFERENCED_PARAM( out );

  /* Let the xport know about ssr */
  if_ptr->ssr( if_ptr );

  if_ptr->glink_core_if_ptr->link_down(if_ptr);
}

/*===========================================================================
  FUNCTION      glinki_check_xport_and_notify
===========================================================================*/
/** 
 * Notify this xport for link state change if applicable
 *
 * @param[in]  if_ptr                 transport interface pointer
 * @param[in]  link_notif_data_param  parameter for link notif data
 * @param[in]  state                  link state to notify
 * @param[out] out                    unused
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static void glinki_check_xport_and_notify
(
  glink_transport_if_type  *if_ptr,
  void                     *link_notif_data_param,
  uint32                    state,
  void                    **out
)
{
  glink_core_xport_ctx_type  *xport_ctx;
  glink_link_notif_data_type *link_notif_data;
  glink_link_info_type        link_info;

  GLINK_OS_UNREFERENCED_PARAM( out );
  
  ASSERT(link_notif_data_param && if_ptr->glink_core_priv);
  
  link_notif_data = (glink_link_notif_data_type *)link_notif_data_param;
  xport_ctx = if_ptr->glink_core_priv;
  
  if (link_notif_data->xport == NULL ||
      0 == glink_os_string_compare(xport_ctx->xport, link_notif_data->xport))
  {
    /* xport not specified, or it is specified and matches the current xport */
    /* Invoke registered callback */
    link_info.xport = xport_ctx->xport;
    link_info.remote_ss = xport_ctx->remote_ss;
    link_info.link_state = (glink_link_state_type)state;
    
    link_notif_data->link_notifier(&link_info, link_notif_data->priv);
  }
}

/*===========================================================================
  FUNCTION      glinki_check_xport_link_up_and_notify
===========================================================================*/
/** 
 * Notify this xport for link state change if applicable
 *
 * @param[in]  if_ptr                 transport interface pointer
 * @param[in]  link_notif_data_param  parameter for link notif data
 * @param[in]  state                  link state to notify
 * @param[out] out                    unused. but just passed as parameter
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
static void glinki_check_xport_link_up_and_notify
(
  glink_transport_if_type  *if_ptr,
  void                     *link_notif_data_param,
  uint32                    state,
  void                    **out
)
{
  if (!glinki_xport_linkup(if_ptr))
  {
    return;
  }
  
  glinki_check_xport_and_notify(if_ptr, link_notif_data_param, state, out);
}

/*===========================================================================
  EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      glink_init

DESCRIPTION   Initializes the GLink core library.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_init(void)
{
  uint32 i;
  boolean cs_created = FALSE;

#if defined(GLINK_MEMORY_LOGGING)
  cs_created = glink_os_cs_init(&glink_mem_log_cs);
  ASSERT(cs_created);
#elif defined(GLINK_OS_DEFINED_LOGGING)
  OS_LOG_INIT();
#endif
  
  smem_list_init(&glink_link_notif_list);
  cs_created = glink_os_cs_init(&glink_link_state_cs);
  ASSERT(cs_created);
  
  /* Create/Initalize crtitical sections */
  for (i = 0; i < GLINK_NUM_HOSTS; ++i)
  {
    cs_created = glink_os_cs_init(&glink_transport_q_cs[i]);
    ASSERT(cs_created);
    smem_list_init(&glink_registered_transports[i]);
  }
}

/*===========================================================================
  FUNCTION      glinki_add_ch_to_xport
===========================================================================*/
/** 
 * Add remote/local channel context to xport open channel queue
 *
 * @param[in]    if_ptr            Pointer to xport if on which channel is to
 *                                 be opened
 * @param[in]    req_if_ptr        Pointer to xport if on which channel 
 *                                 actually wants to open
 * @param[in]    ch_ctx            channel context
 * @param[out]   allocated_ch_ctx  Pointer to channel context pointer
 * @param[in]    local_open        flag to determine if channel is opened 
 *                                 locally or remotely
 * @param[in]    migrated_ch_prio  negotiated xport priority
 *                                 (used to send priority via remote_open_ack to 
 *                                  remote side)
 *
 * @return       G-Link standard error type
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
glink_err_type glinki_add_ch_to_xport
(
  glink_transport_if_type  *if_ptr,
  glink_channel_ctx_type   *ch_ctx,
  glink_channel_ctx_type  **allocated_ch_ctx,
  unsigned int              local_open,
  glink_xport_priority      migrated_ch_prio
)
{
  glink_err_type              status;
  glink_channel_ctx_type     *open_ch_ctx;
  glink_core_xport_ctx_type  *xport_ctx;
  boolean                     valid_open_call;

  if ( if_ptr == NULL          ||
       ch_ctx->name[0] == '\0' ||
       allocated_ch_ctx == NULL )
  {
    return GLINK_STATUS_INVALID_PARAM;
  }
  
  xport_ctx = if_ptr->glink_core_priv;
  
  /* See if channel already exists in open_list */
  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  
  open_ch_ctx = glinki_find_ch_ctx_by_name(xport_ctx,
                                           ch_ctx->name,
                                           (boolean)local_open,
                                           &valid_open_call);
  
  if ( !valid_open_call )
  {
    glink_os_free(ch_ctx);
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    return GLINK_STATUS_FAILURE;
  }
  
  if (!open_ch_ctx)
  {
    /* check if a new channel can be added */
    if ((uint32)smem_list_count(&xport_ctx->open_list) >= xport_ctx->max_lcid)
    {
      glink_os_cs_release(&xport_ctx->channel_q_cs);
      glink_os_free(ch_ctx);
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }
    
    /* Channel not in the list - it was not previously opened */
    ch_ctx->if_ptr = if_ptr;
    *allocated_ch_ctx = ch_ctx;

    /* Initialize the reference for either the local or remote client */
    ch_ctx->ref_count = 1;

    /* Set channel state */
    if (local_open)
    {
      /* This is a local open */
      ch_ctx->local_state = GLINK_LOCAL_CH_OPENING;
    }
    else
    {
      ch_ctx->remote_state = GLINK_REMOTE_CH_OPENED;
    }

    /* make sure next LCID is not used in currently open channels */
    open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);

    while (open_ch_ctx)
    {
      if (open_ch_ctx->lcid == xport_ctx->free_lcid)
      {
        xport_ctx->free_lcid++;
        
        if (xport_ctx->free_lcid >= xport_ctx->max_lcid)
        {
          xport_ctx->free_lcid = 1;
        }

        open_ch_ctx = smem_list_first(&if_ptr->glink_core_priv->open_list);
        continue;
      }

      open_ch_ctx = smem_list_next(open_ch_ctx);
    }

    ch_ctx->lcid = xport_ctx->free_lcid++;
    if (xport_ctx->free_lcid >= xport_ctx->max_lcid)
    {
      xport_ctx->free_lcid = 1;
    }

    if(!local_open)
    {
      /* initialize channel resources */
      status = xport_ctx->channel_init(ch_ctx);
      if(status != GLINK_STATUS_SUCCESS)
      {
        glink_os_free(ch_ctx);
        glink_os_cs_release(&xport_ctx->channel_q_cs);
        return status;
      }
    }
    glink_os_mutex_init(&ch_ctx->tx_cs);
    glink_os_cs_init(&ch_ctx->qos_cs);

    /* Append the channel to the transport interface's open_list */
    smem_list_append(&if_ptr->glink_core_priv->open_list, ch_ctx);

    /* release lock before context switch otherwise it is causing deadlock */
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    
    /* Send the OPEN command to transport */
    if (local_open)
    {
      status = if_ptr->tx_cmd_ch_open(if_ptr, ch_ctx->lcid,
                                      ch_ctx->name,
                                      migrated_ch_prio);
    }
    else
    {
      if_ptr->tx_cmd_ch_remote_open_ack(if_ptr, ch_ctx->rcid, migrated_ch_prio);
      status = GLINK_STATUS_SUCCESS;
    }

    if (status != GLINK_STATUS_SUCCESS)
    {
      /* Release the reference (Cleanup will be done appropriately since
       * the channel has just the reference */
      glinki_channel_ref_release(ch_ctx);
    }
    else
    {
      //Update the Filter
      glinki_update_logging_filter(*allocated_ch_ctx, FALSE);
    }

    return status;
  }
  
  if (local_open)
  {
    /* LOCAL OPEN REQUEST */
    ch_ctx->rcid     = open_ch_ctx->rcid;
    ch_ctx->lcid     = open_ch_ctx->lcid;
    ch_ctx->pintents = open_ch_ctx->pintents;
    ch_ctx->if_ptr   = open_ch_ctx->if_ptr;
    /* Inherit the ref count of the remote context */
    ch_ctx->ref_count = open_ch_ctx->ref_count;
    /* Take a reference for the local context */
    ch_ctx->ref_count++;

    if (ch_ctx->pintents != NULL)
    {
      ch_ctx->pintents->ch_ctx = ch_ctx;
      open_ch_ctx->pintents = NULL;
    }

    /* New channel context will be added to the channel list 
     * so its critical sections need to be initialized */
    glink_os_mutex_init(&ch_ctx->tx_cs);
    glink_os_cs_init(&ch_ctx->qos_cs);

    ch_ctx->remote_state = open_ch_ctx->remote_state;
    ch_ctx->local_state  = GLINK_LOCAL_CH_OPENING;

    /* Replace channel context in the channel list */
    smem_list_append(&xport_ctx->open_list, ch_ctx);

    /* Release lock before context switch otherwise it is causing
     * deadlock */
    glink_os_cs_release(&xport_ctx->channel_q_cs);

    /* Release the reference of the remote context.
     * It should get destroyed in this call */
    glinki_channel_ref_release(open_ch_ctx);

    *allocated_ch_ctx = ch_ctx;
    
    /* Send open cmd to transport */
    status = if_ptr->tx_cmd_ch_open(if_ptr, 
                                    ch_ctx->lcid,
                                    ch_ctx->name,
                                    migrated_ch_prio);
  }
  else
  { 
    /* REMOTE OPEN REQUEST */
    open_ch_ctx->rcid = ch_ctx->rcid;
    /* Take a reference for the remote client */
    open_ch_ctx->ref_count++;
    *allocated_ch_ctx = open_ch_ctx;
    status = xport_ctx->channel_init(open_ch_ctx);

    if (status == GLINK_STATUS_SUCCESS)
    {
      open_ch_ctx->remote_state = GLINK_REMOTE_CH_OPENED;
    }
    
    /* release lock before context switch otherwise it is causing deadlock */
    glink_os_cs_release(&xport_ctx->channel_q_cs);
    
    /* Send ACK to transport */
    if_ptr->tx_cmd_ch_remote_open_ack(if_ptr,
                                      open_ch_ctx->rcid,
                                      migrated_ch_prio);
    
    if (status == GLINK_STATUS_SUCCESS)
    {
      /* Inform the client */
      if (open_ch_ctx->local_state == GLINK_LOCAL_CH_OPENED)
      {
        open_ch_ctx->notify_state(open_ch_ctx, 
                                  open_ch_ctx->priv, 
                                  GLINK_CONNECTED);
      }
    }

    glink_os_free(ch_ctx);
  } /* end If - else (local_open) */
  
  /* If the channel was added to be closed later by channel migration
     do not update the filter */
  if ( status == GLINK_STATUS_SUCCESS )
  {
    //Update the Filter - Reset Boolean=FALSE
    glinki_update_logging_filter( *allocated_ch_ctx, FALSE );
  }
  
  return status;
}

/*===========================================================================
  FUNCTION      _glinki_channel_fully_opened
===========================================================================*/
/** 
 * Check whether this channel is fully opened or not (local & remote)
 * This also checks transport status. The caller is expected to hold
 * the channel_q_cs.
 *
 * @param[in]  handle        glink channel handle
 *
 * @return     TRUE,  if channel is fully opened
 *             FASLE, otherwise
 *
 * @sideeffects  None
 */
/*=========================================================================*/
boolean _glinki_channel_fully_opened
(
  glink_handle_type          handle
)
{
  if(glinki_xport_linkup(handle->if_ptr) &&
      handle->local_state == GLINK_LOCAL_CH_OPENED && 
      handle->remote_state == GLINK_REMOTE_CH_OPENED)
  {
    return TRUE;
  }

  return FALSE;
}


/*===========================================================================
  FUNCTION      glinki_channel_fully_opened
===========================================================================*/
/** 
 * Check whether this channel is fully opened or not (local & remote)
 * This also checks transport status
 *
 * @param[in]  handle        glink channel handle
 *
 * @return     TRUE,  if channel is fully opened
 *             FASLE, otherwise
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
boolean glinki_channel_fully_opened
(
  glink_handle_type          handle
)
{
  boolean ch_fully_opened = FALSE;

  glink_os_cs_acquire( &handle->if_ptr->glink_core_priv->channel_q_cs );

  ch_fully_opened = _glinki_channel_fully_opened(handle);

  glink_os_cs_release( &handle->if_ptr->glink_core_priv->channel_q_cs );
  
  return ch_fully_opened;
}

/*===========================================================================
FUNCTION      glink_core_register_transport

DESCRIPTION   Transport calls this API to register its interface with GLINK 
              Core

ARGUMENTS   *if_ptr   Pointer to interface instance; must be unique
                      for each edge
                         
            *cfg      Pointer to transport configuration structure.

RETURN VALUE  Standard GLINK error codes.

SIDE EFFECTS  None
===========================================================================*/
glink_err_type glink_core_register_transport
(
  glink_transport_if_type       *if_ptr,
  glink_core_transport_cfg_type *cfg
)
{
  unsigned int remote_host = 0;
  glink_core_xport_ctx_type *xport_ctx;

  /* Param validation */
  if (if_ptr == NULL          ||
      cfg == NULL             ||
      cfg->name == NULL       ||
      cfg->remote_ss == NULL  ||
      cfg->version == NULL    ||
      cfg->version_count == 0 ||
      cfg->max_cid == 0)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, "", "", "", 
                           GLINK_STATUS_INVALID_PARAM );    
    return GLINK_STATUS_INVALID_PARAM;
  }

  if(if_ptr->tx_cmd_version == NULL             ||
     if_ptr->tx_cmd_version_ack == NULL         ||
     if_ptr->set_version == NULL                ||
     if_ptr->tx_cmd_ch_open == NULL             ||
     if_ptr->tx_cmd_ch_close == NULL            ||
     if_ptr->tx_cmd_ch_remote_open_ack == NULL  ||
     if_ptr->tx_cmd_ch_remote_close_ack == NULL ||
     if_ptr->ssr  == NULL)
  {
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, "", 
                           cfg->name, 
                           cfg->remote_ss, 
                           GLINK_STATUS_INVALID_PARAM );    
    return GLINK_STATUS_INVALID_PARAM;;
  }

  remote_host = glinki_find_remote_host(cfg->remote_ss);

  if(remote_host == GLINK_NUM_HOSTS )
  {
    /* Unknown transport name trying to register with GLink */
    GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, "", 
                           cfg->name, 
                           cfg->remote_ss, 
                           GLINK_STATUS_INVALID_PARAM );
    return GLINK_STATUS_INVALID_PARAM;
  }

  /* Set the glink_core_if_ptr to point to the default interface */
  if_ptr->glink_core_if_ptr = glink_core_get_default_interface();

  /* Allocate/fill out the GLink private context data */
  {
     xport_ctx = glink_os_calloc(sizeof(glink_core_xport_ctx_type));
    if(xport_ctx == NULL)
    {
      GLINK_LOG_ERROR_EVENT( GLINK_EVENT_REGISTER_XPORT, "", 
                             cfg->name, 
                             cfg->remote_ss, 
                             GLINK_STATUS_OUT_OF_RESOURCES );
    
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }

    xport_ctx->xport = cfg->name;
    xport_ctx->remote_ss = cfg->remote_ss;
    xport_ctx->free_lcid = 1; /* lcid 0 is reserved for invalid channel */
    xport_ctx->max_lcid = cfg->max_cid; /* Max channel ID supported by transport */
    xport_ctx->version_array = cfg->version;
    xport_ctx->version_indx = (int32)cfg->version_count - 1;

    glink_os_cs_init(&xport_ctx->channel_q_cs);
    glink_os_cs_init(&xport_ctx->liid_cs);

    glink_os_cs_acquire(&xport_ctx->channel_q_cs);
    smem_list_init(&xport_ctx->open_list);
    glink_os_cs_release(&xport_ctx->channel_q_cs);

    /* Set the glink_core_if_ptr to point to the allocated structure */
    if_ptr->glink_core_priv = xport_ctx;
    xport_ctx->status = GLINK_XPORT_LINK_DOWN;
  }
  
  /* Push the transport interface into appropriate queue */
  glinki_enqueue_item(&glink_registered_transports[remote_host],
                      if_ptr,
                      &glink_transport_q_cs[remote_host]);

  GLINK_LOG_EVENT_NO_FILTER( GLINK_EVENT_REGISTER_XPORT, "", 
                             xport_ctx->xport, 
                             xport_ctx->remote_ss, 
                             GLINK_STATUS_SUCCESS );

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION      glinki_register_link_notif_data
===========================================================================*/
/** 
 * Register link notification data
 *
 * @param[in]  link_notif_data  parameter for link notif data
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glinki_register_link_notif_data
(
  glink_link_notif_data_type *link_notif_data
)
{
  glink_os_cs_acquire(&glink_link_state_cs);
  
  smem_list_append(&glink_link_notif_list, (void *)link_notif_data);
  
  /* Scan the list of available transport to see if this link is already up */
  glinki_scan_xports_and_notify(link_notif_data);
  
  glink_os_cs_release(&glink_link_state_cs);
}

/*===========================================================================
  FUNCTION      glinki_deregister_link_notif_data
===========================================================================*/
/** 
 * Deregister link notification data
 *
 * @param[in]  link_notif_data  parameter for link notif data
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glinki_deregister_link_notif_data
(
  glink_link_notif_data_type *link_notif_data
)
{
  glinki_dequeue_item(&glink_link_notif_list,
                      (void *)link_notif_data,
                      &glink_link_state_cs);
}

/*===========================================================================
  FUNCTION      glinki_scan_xports_and_notify
===========================================================================*/
/** 
 * Scan xports and notify link up state event
 *
 * @param[in]  link_notif_data  parameter for link notif data
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glinki_scan_xports_and_notify
(
  glink_link_notif_data_type *link_notif_data
)
{
  unsigned int          remote_host;
  glink_link_state_type link_state = GLINK_LINK_STATE_UP;
  
  ASSERT(link_notif_data);
  
  /* Find matching subsystem */
  for (remote_host = 0; remote_host < GLINK_NUM_HOSTS; ++remote_host)
  {
    if (link_notif_data->remote_ss != NULL &&
        0 != glink_os_string_compare(glink_hosts_supported[remote_host],
                                     link_notif_data->remote_ss))
    {
      /* client is not interested in this remote SS */
      continue;
    }
    
    glinki_xports_for_each(glink_hosts_supported[remote_host],
                           glinki_check_xport_link_up_and_notify,
                           link_notif_data,
                           link_state,
                           NULL);
  }
}

/*===========================================================================
  FUNCTION      glinki_update_link_state_and_notify
===========================================================================*/
/** 
 * Updates Link state and also scans registered link notification list to
 * notify xport's state change
 *
 * @param[in]  if_ptr  pointer to xport interface
 * @param[in]  state   link state to notify
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glinki_update_link_state_and_notify
(
  glink_transport_if_type       *if_ptr,
  glink_transport_status_type    state
)
{
  glink_link_notif_data_type *link_notif_data;
  glink_link_state_type       link_state;

  glink_os_cs_acquire(&glink_link_state_cs);
  
  /* change the xport status according to state input */
  if_ptr->glink_core_priv->status = state;
  
  switch(state)
  {
    case GLINK_XPORT_LINK_DOWN:
      link_state = GLINK_LINK_STATE_DOWN;
    break;
    
    case GLINK_XPORT_LINK_UP:
      link_state = GLINK_LINK_STATE_UP;
    break;
    
    case GLINK_XPORT_NEGOTIATING:
    default:
      /* Nothing to do */
      glink_os_cs_release(&glink_link_state_cs);
      return;
    break;
  }
  
  /* Notify all clients who registered for link state change */
  for (link_notif_data = smem_list_first(&glink_link_notif_list);
       link_notif_data != NULL;
       link_notif_data = smem_list_next(link_notif_data))
  {
    if( link_notif_data->remote_ss == NULL || 
        0 == glink_os_string_compare( if_ptr->glink_core_priv->remote_ss,
                                      link_notif_data->remote_ss ) ) 
    {
      glinki_check_xport_and_notify(if_ptr,
                                    link_notif_data,
                                    (uint32)link_state,
                                    NULL);
    }
  }
  
  glink_os_cs_release(&glink_link_state_cs);
} /* glinki_update_link_state_and_notify */

/*===========================================================================
  FUNCTION      glinki_ssr
===========================================================================*/
/** 
 * Notify all the xports of ssr event in this edge
 *
 * @param[in]    remote_ss name of remote subsystem
 * 
 * @return       NONE
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
void glink_ssr(const char* remote_ss)
{
  glinki_xports_for_each(remote_ss,
                         glinki_notify_xport_ssr,
                         NULL,
                         0,
                         NULL);
}

/*===========================================================================
  FUNCTION      glinki_xport_linkup
===========================================================================*/
/** 
 * Check whether this transport is in linkup state or not
 *
 * @param[in]    if_ptr     transport interface pointer
 * 
 * @return       TRUE   if this xport is in link up state
 *               FALSE  otherwise
 * 
 * @sideeffects  NONE
 */
/*=========================================================================*/
boolean glinki_xport_linkup
(
  glink_transport_if_type *if_ptr
)
{
  return if_ptr->glink_core_priv->status == GLINK_XPORT_LINK_UP;
}

/*===========================================================================
  FUNCTION      glinki_xports_for_each
===========================================================================*/
/** 
 * Scan all the transports in given edge and perform client's function for each
 * transport
 *
 * @param[in]    remote_ss    name of remote subsystem, NULL string not accepted
 * @param[in]    client_ex_fn client function to perform on each xport
 * @param[in]    param1       first parameter to use in client_ex_fn
 * @param[in]    param2       second parameter to use in client_ex_fn
 * @param[out]   out          value to return in case client wants
 *
 * @return       None.
 *
 * @sideeffects  None.
 *
 * This routine ASSUMES it is safe to traverse the glink_registered_transports 
 * without a lock. This is only true if the following conditions are met
 * 1) Only append/traverse operations are allowed on this list (No insert, delete)
 * 2) Append should make sure at no instance of time we have an uninitialized 
 * pointer(next) along the list
 */
/*=========================================================================*/
void glinki_xports_for_each
(
  const char         *remote_ss,
  glink_client_ex_fn  client_ex_fn,
  void               *param1,
  uint32              param2,
  void              **out
)
{
  glink_transport_if_type *if_iter_ptr;
  
  uint32 remote_host = glinki_find_remote_host(remote_ss);
  
  ASSERT(remote_host < GLINK_NUM_HOSTS);
  

  for(if_iter_ptr = smem_list_first(&glink_registered_transports[remote_host]);
      if_iter_ptr != NULL;
      if_iter_ptr = smem_list_next(if_iter_ptr))
  {
    client_ex_fn(if_iter_ptr, param1, param2, out);
  }
}

/*===========================================================================
  FUNCTION      glinki_xports_find
===========================================================================*/
/** 
 * Scan all the transports in given edge and finds transport that satisfy
 * client's condition
 *
 * @param[in]    remote_ss      name of remote subsystem, NULL string not accepted
 * @param[in]    client_cond_fn client function to check if this transport is 
 *                              what client is looking for
 * @param[in]    cond1          first condition to use in client_ex_fn
 * @param[in]    cond2          second condition to use in client_ex_fn
 * @param[out]   out            value to return in case client wants
 *
 * @return       pointer to glink_transport_if_type struct
 *               NULL if there isn't any xport matches client's search condition
 *
 * @sideeffects  None.
 *
 * This routine ASSUMES it is safe to traverse the glink_registered_transports 
 * without a lock. This is only true if the following conditions are met
 * 1) Only append/traverse operations are allowed on this list (No insert/delete)
 * 2) Append should make sure at no instance of time we have an uninitialized 
 * pointer(next) along the list
 */
/*=========================================================================*/
glink_transport_if_type *glinki_xports_find
(
  const char           *remote_ss,
  glink_client_cond_fn  client_cond_fn,
  void                 *cond1,
  uint32                cond2,
  void                **out
)
{
  glink_transport_if_type *if_iter_ptr;
  
  uint32 remote_host = glinki_find_remote_host(remote_ss);
  
  ASSERT(remote_host < GLINK_NUM_HOSTS);
  
  for (if_iter_ptr = smem_list_first(&glink_registered_transports[remote_host]);
       if_iter_ptr != NULL;
       if_iter_ptr = smem_list_next(if_iter_ptr))
  {
    if(client_cond_fn(if_iter_ptr, cond1, cond2, out))
    {
      break;
    }
  }
  
  return if_iter_ptr;
}

/*===========================================================================
  FUNCTION      glinki_find_ch_ctx_by_lcid
===========================================================================*/
/** 
 * Find channel context by lcid
 *
 * @param[in]    xport_ctx  Pointer to transport private context
 * @param[in]    lcid       local channel ID
 *
 * @return       pointer to glink channel context
 *
 * @sideeffects  This function needs to be protected by channel_q_cs
 *               Caller is responsible grab/release mutex when calling this
 */
/*=========================================================================*/
glink_channel_ctx_type *glinki_find_ch_ctx_by_lcid
(
  glink_core_xport_ctx_type *xport_ctx,
  uint32                     lcid
)
{
  glink_channel_ctx_type *open_ch_ctx;
  
  for (open_ch_ctx = smem_list_first(&xport_ctx->open_list);
       open_ch_ctx;
       open_ch_ctx = smem_list_next(open_ch_ctx))
  {
    if (open_ch_ctx->lcid == lcid)
    {
      return open_ch_ctx;
    }
  }
  return NULL;
}

/*===========================================================================
  FUNCTION      glinki_find_ch_ctx_by_rcid
===========================================================================*/
/** 
 * Find channel context by rcid
 *
 * @param[in]    xport_ctx  Pointer to transport private context
 * @param[in]    rcid       remote channel ID
 *
 * @return       pointer to glink channel context
 *
 * @sideeffects  This function needs to be protected by channel_q_cs
 *               Caller is responsible grab/release mutex when calling this
 */
/*=========================================================================*/
glink_channel_ctx_type *glinki_find_ch_ctx_by_rcid
(
  glink_core_xport_ctx_type *xport_ctx,
  uint32                     rcid
)
{
  glink_channel_ctx_type *open_ch_ctx;
  
  for (open_ch_ctx = smem_list_first(&xport_ctx->open_list);
       open_ch_ctx;
       open_ch_ctx = smem_list_next(open_ch_ctx))
  {
    if (open_ch_ctx->rcid == rcid)
    {
      return open_ch_ctx;
    }
  }
  return NULL;
}

/*===========================================================================
  FUNCTION      glinki_channel_ref_acquire
===========================================================================*/
/** 
 * Takes a reference for the given channel context
 *
 * @param[in]    ch_ctx     Channel context
 * @param[in]  opened_only   If true a reference is taken if the channel
 *                           is fully opened. returns false otherwise.
 *
 * @return     TRUE,  if reference was successfully taken
 *             FASLE, otherwise
 *
 * @sideeffects  None
 */
/*=========================================================================*/
boolean glinki_channel_ref_acquire
(
  glink_channel_ctx_type *ch_ctx,
  boolean                opened_only
)
{
  boolean ch_opened = TRUE;

  if(!ch_ctx)
  {
    return FALSE;
  }

  glink_os_cs_acquire( &ch_ctx->if_ptr->glink_core_priv->channel_q_cs );

  if(opened_only)
  {
    ch_opened = _glinki_channel_fully_opened(ch_ctx);
  }

  if(ch_opened)
  {
    ch_ctx->ref_count++;
  }

  glink_os_cs_release( &ch_ctx->if_ptr->glink_core_priv->channel_q_cs );
  return ch_opened;
}

/*===========================================================================
  FUNCTION      glinki_channel_ref_release
===========================================================================*/
/** 
 * Returns a reference taken for a channel context
 *
 * @param[in]    ch_ctx     Channel context
 *
 * @return       None
 *
 * @sideeffects  If the reference goes to 0, the channel context is 
 *               released.
 */
/*=========================================================================*/
void glinki_channel_ref_release
(
  glink_channel_ctx_type    *ch_ctx
)
{
  glink_core_xport_ctx_type *xport_ctx = ch_ctx->if_ptr->glink_core_priv;

  glink_os_cs_acquire(&xport_ctx->channel_q_cs);
  ASSERT(ch_ctx->ref_count > 0);
  ch_ctx->ref_count--;
  if(ch_ctx->ref_count == 0)
  {
    glink_clean_channel_ctx(xport_ctx, ch_ctx);
    glink_os_mutex_deinit(&ch_ctx->tx_cs);
    glink_os_cs_deinit(&ch_ctx->qos_cs);
    glink_os_free(ch_ctx);
  }
  glink_os_cs_release(&xport_ctx->channel_q_cs);
}

/*===========================================================================
  FUNCTION      glinki_find_ch_ctx_by_name
===========================================================================*/
/** 
 * Find channel context by channel name, called by local/remote open function
 * This function will also indicate (valid_open_call) if this open call would
 * be valid or not
 *
 * @param[in]    xport_ctx        Pointer to transport private context
 * @param[in]    ch_name          channel name
 * @param[in]    local_open       flag to indicate this is local open call
 * @param[out]   valid_open_call  tell whether this open call would be valid
 *                                or not
 *
 * @return       pointer to glink channel context
 *               NULL if channel doesn't exist
 *
 * @sideeffects  This function needs to be protected by channel_q_cs
 *               Caller is responsible grab/release mutex when calling this
 */
/*=========================================================================*/
glink_channel_ctx_type *glinki_find_ch_ctx_by_name
(
  glink_core_xport_ctx_type *xport_ctx,
  const char                *ch_name,
  boolean                    local_open,
  boolean                   *valid_open_call
)
{
  glink_channel_ctx_type *open_ch_ctx;
  glink_channel_ctx_type *ch_ctx_found = NULL;
  glink_remote_state_type remote_state;
  glink_local_state_type  local_state;
  
  ASSERT( valid_open_call != NULL );
  *valid_open_call = TRUE;
  
  for ( open_ch_ctx = smem_list_first(&xport_ctx->open_list);
        open_ch_ctx != NULL;
        open_ch_ctx = smem_list_next(open_ch_ctx) )
  {
    remote_state = open_ch_ctx->remote_state;
    local_state = open_ch_ctx->local_state;
    
    if ( 0 != glink_os_string_compare(open_ch_ctx->name, ch_name) ||
         remote_state == GLINK_REMOTE_CH_SSR_RESET )
    {
      continue;
    }
    
    if ( ( local_open == TRUE && local_state == GLINK_LOCAL_CH_INIT ) || 
         ( local_open == FALSE && remote_state == GLINK_REMOTE_CH_INIT ) )
    {
      /* Local/Remote side already opened channel */
      ch_ctx_found = open_ch_ctx;
    }
    else if ( ( local_open == TRUE && local_state != GLINK_LOCAL_CH_CLOSED ) ||
              ( local_open == FALSE && remote_state != GLINK_REMOTE_CH_CLOSED ) )
    {
      /* Local/Remote side is trying to open channel without closing old one */
      *valid_open_call = FALSE;
      break;
    }
  }
  
  return ch_ctx_found;
}

/*===========================================================================
  FUNCTION      glinki_find_remote_host
===========================================================================*/
/** 
 * return remote subsystem ID based on remote subsystem name
 *
 * @param[in]    remote_ss  remote subsystem name
 *
 * @return       remote subsystem ID
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
uint32 glinki_find_remote_host
(
  const char *remote_ss
)
{
  uint32 remote_host;

  for(remote_host = 0; remote_host < GLINK_NUM_HOSTS; remote_host++) 
  {
    if( 0 == glink_os_string_compare(glink_hosts_supported[remote_host],
                                     remote_ss))
    {
      /* Match found, break out of loop */
      break;
    }
  }
  
  return remote_host;
}

/*===========================================================================
  FUNCTION      glinki_find_best_xport
===========================================================================*/
/** 
 * This function gives best available transport for give edge
 *
 * @param[in]    remote_host  Index into glink_registered_transports array of
 *                            registered transports list per edge
 * 
 * @return       pointer to glink_transport_if_type
 *               Null, if transport not found
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
glink_transport_if_type *glinki_find_best_xport
(
  const char *remote_ss
)
{
  glink_transport_if_type *best_if_ptr = NULL;
  glink_xport_priority priority = GLINK_MIN_PRIORITY;
  
  glinki_xports_for_each(remote_ss,
                         glinki_update_best_xport,
                         (void *)&priority,
                         0,
                         (void **)&best_if_ptr);
  
  return best_if_ptr;
}

/*===========================================================================
  FUNCTION      glinki_find_requested_xport
===========================================================================*/
/** 
 * Find requested or best transport depending on client's request
 *
 * @param[in]    xport_name         name of transport
 * @param[in]    remote_ss          remote subsystem name
 * @param[in]    open_ch_option     option client gave when called glink_open
 * @param[out]   suggested_priority best xport priority glink suggests
 *
 * @return       pointer to glink_transport_if_type struct
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
glink_transport_if_type *glinki_find_requested_xport
(
  const char           *xport_name,
  const char           *remote_ss,
  uint32                open_ch_option,
  glink_xport_priority *suggested_priority
)
{
  glink_transport_if_type *best_xport = glinki_find_best_xport(remote_ss);
  glink_transport_if_type *xport_found = NULL;
  
  *suggested_priority = GLINK_INVALID_PRIORITY;

  if (!xport_name)
  {
    if (best_xport)
    {
      *suggested_priority = best_xport->glink_priority;
    }
    
    return best_xport;
  }
  
  xport_found = glinki_xports_find(remote_ss,
                                   glinki_client_requested_xport_check,
                                   (void *)xport_name,
                                   0,
                                   NULL);
  
  if (!xport_found)
  {
    return NULL;
  }
  
  if ((open_ch_option & GLINK_OPT_INITIAL_XPORT) != 0)
  {
    *suggested_priority = best_xport->glink_priority;
  }
  else
  {
    /* Client is not willing to migrate to better transport */
    *suggested_priority = xport_found->glink_priority;
  }
  
  return xport_found;
}

/*===========================================================================
  FUNCTION      glinki_find_xport_by_priority
===========================================================================*/
/** 
 * This function returns glink_transport_if pointer based on transport priority
 *
 * @param[in]    prio        glink xport prio
 * @param[in]    remote_ss   remote subsytem name
 *
 * @return       pointer to glink_transport_if_type struct
 *               NULL, if it's not registered or transport not found with
 *                     the priority
 *
 * @sideeffects  NONE
 */
/*=========================================================================*/
glink_transport_if_type *glinki_find_xport_by_priority
(
  glink_xport_priority  prio,
  const char           *remote_ss
)
{
  return glinki_xports_find(remote_ss,
                            glinki_xport_priority_comp,
                            NULL,
                            (uint32)prio,
                            NULL);
}

/*===========================================================================
  FUNCTION      glinki_enqueue_item
===========================================================================*/
/** 
 * Enqueue item to smem list in protected context
 * 
 * @param[in]    smem_list  smem list to enqueue
 * @param[in]    item       item to queue
 * @param[in]    cs         mutex to protect the list
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glinki_enqueue_item
(
  smem_list_type *smem_list_ptr,
  void           *item,
  os_cs_type     *cs
)
{
  glink_os_cs_acquire(cs);
  smem_list_append(smem_list_ptr, item);
  glink_os_cs_release(cs);
}

/*===========================================================================
  FUNCTION      glinki_dequeue_item
===========================================================================*/
/** 
 *  Dequeue item from smem list in protected context
 * 
 * @param[in]    smem_list  smem list to dequeue from 
 * @param[in]    item       item to dequeue
 * @param[in]    cs         mutex to protect the list
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glinki_dequeue_item
(
  smem_list_type *smem_list_ptr,
  void           *item,
  os_cs_type     *cs
)
{
  glink_os_cs_acquire(cs);
  smem_list_delete(smem_list_ptr, item);
  glink_os_cs_release(cs);
}

/*===========================================================================
  FUNCTION      glinki_acquire_edge_lock
===========================================================================*/
/** 
 *  Acquires the transport_q_cs lock for the specified edge
 * 
 * @param[in]    remote_ss  Name of the remote sub system on the edge
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glinki_acquire_edge_lock(const char *ss_name)
{
    uint32 remote_host;
    remote_host = glinki_find_remote_host(ss_name);
    glink_os_cs_acquire(&glink_transport_q_cs[remote_host]);
}

/*===========================================================================
  FUNCTION      glinki_release_edge_lock
===========================================================================*/
/** 
 *  Releases the transport_q_cs lock for the specified edge
 * 
 * @param[in]    remote_ss  Name of the remote sub system on the edge
 *
 * @return       None.
 *
 * @sideeffects  None.
 */
/*=========================================================================*/
void glinki_release_edge_lock(const char *ss_name)
{
    uint32 remote_host;
    remote_host = glinki_find_remote_host(ss_name);
    glink_os_cs_release(&glink_transport_q_cs[remote_host]);
}

#if defined(GLINK_MEMORY_LOGGING)
/* ============ Internal Logging API ================ */
void glink_mem_log
(
  glink_log_event_type type,
  const char *msg,
  const char *xport,
  const char *remote_ss,
  uint32 param
)
{
  glink_os_cs_acquire(&glink_mem_log_cs);

  glink_mem_log_arr[glink_mem_log_idx].type = type;
  glink_mem_log_arr[glink_mem_log_idx].msg = msg;
  glink_mem_log_arr[glink_mem_log_idx].xport = xport;
  glink_mem_log_arr[glink_mem_log_idx].remote_ss = remote_ss;
  glink_mem_log_arr[glink_mem_log_idx].param = param;

  if (++glink_mem_log_idx >= GLINK_MEM_LOG_SIZE)
  {
    glink_mem_log_idx = 0;
  }

  glink_os_cs_release(&glink_mem_log_cs);
}
#endif

#ifdef FEATURE_TRACER_PACKET
/*===========================================================================
FUNCTION      glink_tracer_packet_log_pctx_pkt
===========================================================================*/
/** 
 * Log tracer packet event. Tracer packet is included in glink_core_tx_pkt
 * and needs to use vprovider to extract it
 * 
 * @param[in]  pctx     pointer to glink_core_tx_pkt_type
 * @param[in]  event_id event_id
 * 
 * @return         None
 *
 * @sideeffects    None
 */
/*=========================================================================*/
void glink_tracer_packet_log_pctx_pkt
(
  glink_core_tx_pkt_type  *pctx,
  uint32                   event_id
)
{
  tracer_pkt_result_type  tracer_pkt_log_result;
  void                   *tracer_pkt_data;
  size_t                  tracer_pkt_size;
  
  tracer_pkt_data = pctx->vprovider(pctx->iovec, 0, &tracer_pkt_size);
  
  if (tracer_pkt_size != pctx->size)
  {
    GLINK_LOG_EVENT_NO_FILTER( GLINK_EVENT_TXV_INVALID_BUFFER, "", "", "",
                               tracer_pkt_size );
  }

  tracer_pkt_log_result = tracer_packet_log_event(tracer_pkt_data, (uint16)event_id);
  if (tracer_pkt_log_result != TRACER_PKT_STATUS_SUCCESS)
  {
    GLINK_LOG_EVENT_NO_FILTER( GLINK_EVENT_TRACER_PKT_FAILURE, "", "", "",
                               tracer_pkt_log_result );
  }
}
#endif

/*===========================================================================
  FUNCTION      glinki_update_logging_filter
===========================================================================*/
/** 
 *  Update/Reset the logging filter if the name and remote host of the  
 *  logging filter matches to that of the passed channel context
 * 
 * @param[in]    chnl_ctx   Channel content to match/compare
 * @param[in]    reset      Indicate Update(FALSE) or Reset(TRUE)
 *
 * @return       None.
 */
/*=========================================================================*/
#if defined(GLINK_OS_DEFINED_LOGGING) || defined(GLINK_MEMORY_LOGGING)
void glinki_update_logging_filter
(
  glink_channel_ctx_type  *chnl_ctx, 
  boolean                 reset
)
{
  if ( ( log_filter_cfg.ch_filter_status == TRUE )  && 
       ( (chnl_ctx) != NULL )   && 
       ( glink_os_string_compare( (chnl_ctx)->name,
                                 log_filter_cfg.ch_name) == 0 ) &&
       ( log_filter_cfg.remote_host ==
         glinki_find_remote_host((chnl_ctx)->if_ptr->glink_core_priv->remote_ss) ) 
     )
  {
    if (reset == FALSE) /* Update the Filter */
    {
      log_filter_cfg.ch_ctx = (chnl_ctx);
      log_filter_cfg.ch_lcid = (chnl_ctx)->lcid;
      log_filter_cfg.ch_rcid = (chnl_ctx)->rcid;
    }
    else  /* Reset the Filter */
    {
      log_filter_cfg.ch_ctx = NULL;
      log_filter_cfg.ch_lcid = 0;
      log_filter_cfg.ch_rcid = 0;
      log_filter_cfg.remote_host = (uint32)-1;        
    }
  }
}

#else
void glinki_update_logging_filter(glink_channel_ctx_type *chnl_ctx, boolean reset)
{
    GLINK_OS_UNREFERENCED_PARAM( chnl_ctx );
    GLINK_OS_UNREFERENCED_PARAM( reset );
}
#endif

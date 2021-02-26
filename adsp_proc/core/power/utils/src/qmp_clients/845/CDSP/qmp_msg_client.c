/*===========================================================================
                              qmp_msg_client.c

DESCRIPTION:
  Implements middleware to send messages to AOP over QMP 

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "qmp_msg_client.h"
#include <stdlib.h>
#include "stringl/stringl.h"
#include "../../../mproc/glink/os/inc/glink_os_type.h"
#include "DALSys.h"
#include "CoreVerify.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
/* Glink event type */
typedef enum 
{
  UT_GLINK_CONNECTED = 0,
  UT_GLINK_L_DISCONNECTED,
  UT_GLINK_R_DISCONNECTED,
  UT_GLINK_TX_NOTIF,
  UT_GLINK_RX_NOTIF,
  UT_GLINK_LINK_UP,
  UT_GLINK_LINK_DOWN,
}glink_event_type;

/* Glink Event Pool */
typedef struct 
{
  DALSYSEventHandle   connect[GLINK_NUM_EVENTS];
  DALSYSEventHandle   l_disconnect[GLINK_NUM_EVENTS];
  DALSYSEventHandle   r_disconnect[GLINK_NUM_EVENTS];
  DALSYSEventHandle   tx_callback[GLINK_NUM_EVENTS];
  DALSYSEventHandle   rx_callback[GLINK_NUM_EVENTS];
  DALSYSEventHandle   link_up[GLINK_NUM_EVENTS];
  DALSYSEventHandle   link_down[GLINK_NUM_EVENTS];
}glink_event_pool;

/* Glink info*/
typedef struct 
{
  glink_handle_type       handle;
  glink_open_config_type  config;
  glink_event_pool        *testevent;
  glink_link_id_type	  link_id;
}glink_config;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

glink_config         glink_ctx;
DALSYSSyncHandle      qmp_lock;
glink_event_pool     event_pool;
DALSYSEventHandle hClientStartEvent;
DALSYSWorkLoopHandle hClientWorkLoop;
//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
glink_err_type qmp_msg_client_close(void);
void qmp_client_init(void);
//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================
#define QMP_AOP_ALIGN8(_size_) (((_size_) + 0x7) & (~0x7))
//===========================================================================
//                           Function Definitions
//===========================================================================

/**
 * <!-- glink_link_state_cb -->
 *
 * @brief Callback function registered for link state change.
 *        Differs than glink_state_cb which is a channel state
 *        cb function.
 *
 * @param link_info     : link structure
 * @param priv          : glink ctx
 */
void glink_link_state_cb(glink_link_info_type *link_info,
                            void* priv)
{

  glink_config *pObj = NULL;
  if(NULL == priv)
  {
    return;
  }
  pObj = (glink_config *)priv;

  /*Check for the link state*/
  if(link_info->link_state)
      DALSYS_EventCtrl(*(pObj->testevent->link_down), DALSYS_EVENT_CTRL_TRIGGER);
  else
      DALSYS_EventCtrl(*(pObj->testevent->link_up), DALSYS_EVENT_CTRL_TRIGGER);
}

/**
 * <!-- glink_rx_cb -->
 *
 * @brief Callback function registered for rx callback.
 *        Called when client recieves pkt over QMP.
 *
 * @param handle        : link structure
 * @param priv          : glink ctx
 * @param pkt_priv      : per packet priv context
 * @param ptr           : buffer pointer to read from
 * @param size          : size of the buffer
 * @param intent_used   : if the remote size used an rx intent
 *                      for this pkt
 */
void glink_rx_cb(glink_handle_type handle,
                    const void        *priv,
                    const void        *pkt_priv,
                    const void        *ptr,
                    size_t            size,
                    size_t            intent_used)
{
  glink_config *pObj = NULL;
  if(NULL == priv || NULL == ptr)
  {
    return;
  }
  pObj = (glink_config *)priv;
  
  glink_rx_done(handle, ptr, 1);
  DALSYS_EventCtrl(*(pObj->testevent->rx_callback), DALSYS_EVENT_CTRL_TRIGGER);
  
  return;
}


/**
 * <!-- glink_rxv_cb -->
 *
 * @brief Callback function registered for rx callback.
 *        Called when client recieves pkt over QMP.
 *
 * @param handle        : link structure
 * @param priv          : glink ctx
 * @param pkt_priv      : per packet priv context
 * @param ptr           : buffer pointer to read from
 * @param size          : size of the buffer
 * @param intent_used   : if the remote size used an rx intent
 *                      for this pkt
 */
#if 0
void glink_rxv_cb(glink_handle_type        handle, 
					const void               *priv,
					const void               *pkt_priv,
					void                     *iovec,
					size_t                   size,
					size_t                   intent_used,
					glink_buffer_provider_fn vprovider,
					glink_buffer_provider_fn pprovider) 
{  
  glink_config *pObj = (glink_config *)priv;
  glink_rx_done(handle, iovec, 1);
  DALSYS_EventCtrl(*(pObj->testevent->rx_callback), DALSYS_EVENT_CTRL_TRIGGER);
  return;
}
#endif
/**
 * <!-- glink_tx_cb -->
 *
 * @brief Callback function registered for tx callback.
 *        Called when remote acknowledges transmitted packet.
 *
 * @param handle        : link structure
 * @param priv          : glink ctx
 * @param pkt_priv      : per packet priv context
 * @param ptr           : buffer pointer to read from
 * @param size          : size of the buffer
 * 
 */
void glink_tx_cb(glink_handle_type handle,
                    const void        *priv,
                    const void        *pkt_priv,
                    const void        *ptr,
                    size_t            size)
{
  glink_config *pObj = NULL;
  if(NULL == priv)
  {
      return;
  }
  pObj = (glink_config *)priv;
  // free allocated buffer that was used while sending
  free ((char*)pkt_priv);
  DALSYS_EventCtrl(*(pObj->testevent->tx_callback), DALSYS_EVENT_CTRL_TRIGGER);
  return;
}

/**
 * <!-- glink_state_cb -->
 *
 * @brief Callback function registered for channel state change.
 *        Called when channel state changes from OPEN to L_DISC or R_DISC.
 *
 * @param handle        : link structure
 * @param priv          : glink ctx
 * @param pkt_priv      : per packet priv context
 * @param ptr           : buffer pointer to read from
 * @param size          : size of the buffer
 * 
 */
void glink_state_cb(glink_handle_type handle,
                        const void *priv,
                        glink_channel_event_type  testevent)
{

  glink_config *pObj = NULL;
  if(NULL == priv)
  {
      return;
  }

  pObj = (glink_config *)priv;

  if( UT_GLINK_CONNECTED == testevent)
  {
    DALSYS_EventCtrl(*(pObj->testevent->connect), DALSYS_EVENT_CTRL_TRIGGER);
  }
  else if (UT_GLINK_L_DISCONNECTED == testevent)
  {
    //should hit this when we see if R_DISCONNECTED, we do a L_DISCONNECTED
    
    qmp_client_init();
  }
  else if (UT_GLINK_R_DISCONNECTED == testevent)
  {
    // remote disconnected we close and try opening the channel
    qmp_msg_client_close();
  }
  else
  {
    return;
  }
}

/**
 * <!-- glink_tx_abort -->
 *
 * @brief Callback function registered for rx abort
 *
 * @param handle        : link structure
 * @param priv          : glink ctx
 * @param pkt_priv      : per pkt priv data
 * 
 */
void glink_tx_abort(glink_handle_type  handle,
                        const void         *priv,
                        const void         *pkt_priv)
{

  // tx abort notification means something down the line was hosed either AOP or qmp xport.
  free ((char*)pkt_priv);
  // This cb is an error condition. Something went wrong gravely
  CORE_VERIFY(0);
}

/**
 * <!-- glink_event_destroy -->
 *
 * @brief Deinit all the registered DAL events
 *
 * @param pEvent        : Event handle
 * @param nEvents       : Number of events
 * 
 */
int glink_event_destroy(DALSYSEventHandle *pEvent,
                            uint8          nEvents) 
{
  uint8   i = 0;

  if (NULL != pEvent) 
  {
    for (i = 0 ; i < nEvents; i++) 
    {
        DALSYS_DestroyObject(*(pEvent+i));
    }
    return 0;
  }
  else
  {
    return -1;
  }
}

/**
 * <!-- glink_event_create -->
 *
 * @brief Init all the registered DAL events
 *
 * @param pEvent        : Event handle
 * @param nEvents       : Number of events
 * 
 */
int glink_event_create(DALSYSEventHandle *pEvent,
                            uint32 *pAttb,
                            uint8   nEvents) 
{
  uint8  i = 0;
  DALResult dalResult = DAL_SUCCESS;

  if (NULL != pEvent && NULL != pAttb) 
  {
    for (i = 0 ; i < nEvents; i++) 
    {
      dalResult = DALSYS_EventCreate(*(pAttb+i), pEvent+i, NULL);
      if ( DAL_SUCCESS != dalResult ) 
      {
        /*Destroy the previous event created*/
        glink_event_destroy(pEvent, i);
        return -1;
      }
    }
    return 0;
  }
  else
  {
    return -2;
  }
}

/*===========================================================================
  FUNCTION  glink_event_reset
===========================================================================*/
int glink_event_reset(DALSYSEventHandle  *pEvent)
{
  if(NULL != pEvent){
      DALSYS_EventCtrl(*pEvent, DALSYS_EVENT_CTRL_RESET);
      return 0;
  }
  else{
      return -1;
  }
}

/*===========================================================================
  FUNCTION  glink_event_wait
===========================================================================*/
int glink_event_wait(glink_config   *pObj,
                        uint32              type)
{

  uint32     index = 0;
  DALResult  dalResult   = DAL_ERROR;

  if (NULL == pObj) 
  {
    return 2;
  }

  /* GLINK_CONNECTED */
  if (0 == type) 
  {
    dalResult = DALSYS_EventMultipleWait(  pObj->testevent->connect,
                                              GLINK_NUM_EVENTS,
                                              DALSYS_EVENT_TIMEOUT_INFINITE,
                                              &index);
    glink_event_reset(pObj->testevent->connect);
    if (dalResult != DAL_SUCCESS)
      return 1;
    else
      return 0;
  }
  /* GLINK_L_DISCONNECTED */
  else if (1 == type) 
  {
    dalResult = DALSYS_EventMultipleWait(  pObj->testevent->l_disconnect,
                                              GLINK_NUM_EVENTS,
                                              DALSYS_EVENT_TIMEOUT_INFINITE,
                                              &index);
    glink_event_reset(pObj->testevent->l_disconnect);
    if (dalResult != DAL_SUCCESS)
      return 1;
    else
     return 0;
  }
  /* GLINK_R_DISCONNECTED */
  else if (2 == type) 
  {
    dalResult = DALSYS_EventMultipleWait(  pObj->testevent->r_disconnect,
                                              GLINK_NUM_EVENTS,
                                              DALSYS_EVENT_TIMEOUT_INFINITE,
                                              &index);
    glink_event_reset(pObj->testevent->r_disconnect);
    if (dalResult != DAL_SUCCESS)
      return 1;
    else
      return 0;
  }
  /* GLINK_TX_NOTIF */
  else if (3 == type) 
  {
    dalResult = DALSYS_EventMultipleWait(  pObj->testevent->tx_callback,
                                              GLINK_NUM_EVENTS,
                                              DALSYS_EVENT_TIMEOUT_INFINITE,
                                              &index);
    glink_event_reset(pObj->testevent->tx_callback);
    if (dalResult != DAL_SUCCESS)
      return 1;
    else
      return 0;
  }
  /* GLINK_RX_NOTIF */
  else if (4 == type) 
  {
    dalResult = DALSYS_EventMultipleWait(  pObj->testevent->rx_callback,
                                              GLINK_NUM_EVENTS,
                                              DALSYS_EVENT_TIMEOUT_INFINITE,
                                              &index);
    glink_event_reset(pObj->testevent->rx_callback);
    if (dalResult != DAL_SUCCESS)
      return 1;
    else
      return 0;
  }
  /* GLINK_LINK_UP */
  else if (5 == type) 
  {
    dalResult = DALSYS_EventMultipleWait(  pObj->testevent->link_up,
                                              GLINK_NUM_EVENTS,
                                              DALSYS_EVENT_TIMEOUT_INFINITE,
                                              &index);
    glink_event_reset(pObj->testevent->link_up);
    if (dalResult != DAL_SUCCESS)
      return 1;
    else
      return 0;
  }
  /* GLINK_LINK_DOWN */
  else if (6 == type) 
  {
    dalResult = DALSYS_EventMultipleWait(  pObj->testevent->link_down,
                                              GLINK_NUM_EVENTS,
                                              DALSYS_EVENT_TIMEOUT_INFINITE,
                                              &index);
    glink_event_reset(pObj->testevent->link_down);
    if (dalResult != DAL_SUCCESS)
      return 1;
    else
      return 0;
  }
  else
  {
    return 3;
  }
}

/*===========================================================================
  FUNCTION  glink_events_init
===========================================================================*/
int glink_events_init(glink_event_pool *pEvents,
                            uint32           *pAttb)
{

  if(NULL != pEvents && NULL != pAttb)
  {
    /* Create connect event */
    if(glink_event_create(pEvents->connect, pAttb, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Create l_disconnect event */
    if(glink_event_create(pEvents->l_disconnect, pAttb, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Create r_disconnect event */
    if(glink_event_create(pEvents->r_disconnect, pAttb, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Create tx_callback event */
    if(glink_event_create(pEvents->tx_callback, pAttb, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Create rx_callback event */
    if(glink_event_create(pEvents->rx_callback, pAttb, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Create link_up event */
    if(glink_event_create(pEvents->link_up, pAttb, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Create link_down event */
    if(glink_event_create(pEvents->link_down, pAttb, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    return 0;
  }
  else
  {
    return -2;
  }
}

/*===========================================================================
  FUNCTION  glink_events_deinit
===========================================================================*/
int glink_events_deinit(glink_event_pool *pEvents)
{

  if(NULL != pEvents)
  {
    /* Destroy connect event */
    if(glink_event_destroy(pEvents->connect, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Destroy l_disconnect event */
    if(glink_event_destroy(pEvents->l_disconnect, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Destroy r_disconnect event */
    if(glink_event_destroy(pEvents->r_disconnect, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Destroy tx_callback event */
    if(glink_event_destroy(pEvents->tx_callback, GLINK_NUM_EVENTS))
    {
       return -1;
    }
    /* Destroy rx_callback event */
    if(glink_event_destroy(pEvents->rx_callback, GLINK_NUM_EVENTS))
    {
       return -1;
    }
    /* Destroy link_up event */
    if(glink_event_destroy(pEvents->link_up, GLINK_NUM_EVENTS))
    {
      return -1;
    }
    /* Destroy link_down event */
    if(glink_event_destroy(pEvents->link_down, GLINK_NUM_EVENTS))
    {
        return -1;
    }
    return 0;
  }
  else
  {
      return -2;
  }
}


/**
 * <!-- __qmp_client_init -->
 *
 * @brief inits events and registered with DAL
 *
 * @param handle : event handle
 * @param param : void ptr
 * 
 */
static DALResult __qmp_client_init( DALSYSEventHandle handle, void * param )
{

  glink_err_type                              ret_code;
  uint32  event_attb[GLINK_NUM_EVENTS] = {DALSYS_EVENT_ATTR_NORMAL,
                                              DALSYS_EVENT_ATTR_TIMEOUT_EVENT};
  
  /* Set the required configuration for the test */
  glink_ctx.config.name 				= "aop_cdsp";
  glink_ctx.config.transport            = "qmp";
  glink_ctx.config.remote_ss            = "rpm";
  glink_ctx.config.options              = 0;
  glink_ctx.config.priv 				= &glink_ctx;
  glink_ctx.config.notify_rx            = glink_rx_cb;
  glink_ctx.config.notify_rxv           = NULL;
  glink_ctx.config.notify_rx_abort      = NULL;
  glink_ctx.config.notify_rx_intent     = NULL;
  glink_ctx.config.notify_rx_intent_req = NULL;
  glink_ctx.config.notify_rx_sigs       = NULL;
  glink_ctx.config.notify_rx_tracer_pkt = NULL;
  glink_ctx.config.notify_state         = glink_state_cb;
  glink_ctx.config.notify_tx_abort      = glink_tx_abort;
  glink_ctx.config.notify_tx_done       = glink_tx_cb;
  glink_ctx.testevent                   = &event_pool;

  if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, &(qmp_lock), 0))
    ERR_FATAL("Couldn't create qmp lock.", 0, 0, 0);

  /* Set glink link configuration */
  glink_ctx.link_id.version             = GLINK_LINK_ID_VER;
  glink_ctx.link_id.xport               = glink_ctx.config.transport;
  glink_ctx.link_id.remote_ss           = glink_ctx.config.remote_ss;
  glink_ctx.link_id.link_notifier       = glink_link_state_cb;
  glink_ctx.link_id.handle              = NULL;

  /* Initialize glink event pool */
  if(glink_events_init(glink_ctx.testevent,event_attb))
  {
      return GLINK_STATUS_FAILURE;
  }

  /*Register link state callback*/
  ret_code = glink_register_link_state_cb(&glink_ctx.link_id, &glink_ctx);
  if(GLINK_STATUS_SUCCESS != ret_code)
  {
      glink_events_deinit(glink_ctx.testevent);
      return ret_code;
  }

  /* Wait for link up notification before proceeding */
  if(glink_event_wait(&glink_ctx, UT_GLINK_LINK_UP)){
      glink_deregister_link_state_cb(glink_ctx.link_id.handle);
      glink_events_deinit(glink_ctx.testevent);
      return GLINK_STATUS_FAILURE;
  }


  /* Open the glink channel */
  ret_code = glink_open(&(glink_ctx.config), &(glink_ctx.handle));
  if(GLINK_STATUS_SUCCESS != ret_code)
  {
      glink_deregister_link_state_cb(glink_ctx.link_id.handle);
      glink_events_deinit(glink_ctx.testevent);
      goto cleanup;
  }


  /*Wait for UT_GLINK_CONNECTED event*/
  if(glink_event_wait(&glink_ctx, UT_GLINK_CONNECTED)){
      return GLINK_STATUS_FAILURE;
  }

  /*Queue rx intent here*/
  ret_code = glink_queue_rx_intent(glink_ctx.handle, "ut", GLINK_DATA_SIZE);
  if(GLINK_STATUS_SUCCESS != ret_code){
      goto cleanup;
  }


cleanup:
    return ret_code;

}



/**
 * <!-- __qmp_client_init -->
 *
 * @brief deinits events and registered with DAL
 *
 * @param void : void
 * 
 */
void qmp_client_init(void)
{
  uint32 error_code = -1;
  
    /* Start workloop */
  error_code = DALSYS_EventCreate( DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                   &hClientStartEvent, NULL);
  if( DAL_SUCCESS == error_code )
  {
    error_code = DALSYS_RegisterWorkLoopEx( "QMP_PWR_CLIENT",
                                        1024,                 //stack size
                                        0,
                                        100,                  //max events
                                        &hClientWorkLoop,
                                        NULL );

    if( DAL_SUCCESS == error_code )
    {
      error_code = DALSYS_AddEventToWorkLoop( hClientWorkLoop,
                                              &__qmp_client_init,
                                              NULL,
                                              hClientStartEvent,
                                              NULL );
    }
  }

  error_code = DALSYS_EventCtrl( hClientStartEvent, DALSYS_EVENT_CTRL_TRIGGER );

  if( DAL_SUCCESS != error_code )
  {
    CORE_VERIFY_PTR(0);
  }


}




/**
 * <!-- send2aop -->
 *
 * @brief sends messages over qmp to aop. Since it is
 *        non-blocking, we make a copy of the message on the
 *        heap to avoid pointer corruption once stack returns.
 *        Free once we get corresponding callback for either
 *        tx_done or tx_abort
 *
 * @param json_str : char* to json message
 * @param size  : size of the message including the /0 at the
 *              end
 * @return glink_err_type: returns transparent values from
 *         glink_tx. Clients should try again, on return !=
 *         GLINK_STATUS_SUCCESS
 */
glink_err_type send2aop(char* json_str, size_t size)
{
  // pointer to the buffer gets corrupted beyond this point
  // allocate buffer and free when tx_done event is recieved for the packet
  size_t __str_size = QMP_AOP_ALIGN8(size);
  char* copy_json_str = (char*) malloc (__str_size);
  CORE_VERIFY_PTR(copy_json_str);

  memcpy(copy_json_str, json_str, __str_size);

  DALSYS_SyncEnter(qmp_lock);
  glink_err_type ret_code = glink_tx(glink_ctx.handle,
                            copy_json_str,
                            (void *)json_str,
                            size,
                            1);
  if (ret_code != GLINK_STATUS_SUCCESS)
  {
    free(copy_json_str);
  }
  DALSYS_SyncLeave(qmp_lock);

  return ret_code;
}

/**
 * <!-- qmp_msg_client_close -->
 *
 * @brief Wraps glink_close(), deregister link state callbacks
 *        and DAL events
 *       
 *
 * @param void    : void
 * @return glink_err_type  :
 *         GLINK_STATUS_FAILURE/GLINK_STATUS_SUCCESS
 * 
 */

glink_err_type qmp_msg_client_close(void)
{
  glink_err_type ret_code = glink_close(glink_ctx.handle);

  glink_deregister_link_state_cb(glink_ctx.link_id.handle);
  glink_events_deinit(glink_ctx.testevent);

  return ret_code;
}


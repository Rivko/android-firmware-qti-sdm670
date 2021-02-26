#ifndef GLINK_H
#define GLINK_H

/**
 * @file glink.h
 *
 * Public API for the GLink
 */

/** \defgroup glink GLink
 * \ingroup GLINK
 *
 * GLink reliable, in-order, datagram-based interprocessor communication
 * over a set of supported transport (MAILBOX)
 *
 * All ports preserve message boundaries across the interprocessor channel; one
 * write into the port exactly matches one read from the port.
 */
/*@{*/

/*==============================================================================
     Copyright (c) 2014-2017 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/mproc/glink.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/15   pp      Expanded list for err_type & changed to -ve error codes
04/15/15   sm      Added timeout error type.
04/02/15   an      Use com_dtypes instead of comdef due to wider availability
02/23/15   bc      Add rx tracer packet notification callback
12/19/14   bm      Add rx/tx abort notifiaction callbacks
11/20/14   bm      Added glink open config flag
07/04/14   bm      Updated API to v0.6.5 GLink spec
03/22/14   bm      Initial release of GLink API
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "stddef.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/
/** GLink status/return codes */
typedef enum {
  GLINK_STATUS_SUCCESS                 = 0,
  GLINK_STATUS_INVALID_PARAM             = -1,
  GLINK_STATUS_NOT_INIT                  = -2,
  GLINK_STATUS_OUT_OF_RESOURCES          = -3,
  /* Failure due to lack of transport */
  GLINK_STATUS_NO_TRANSPORT              = -4 ,
  /* TX failure when there is no remote intent queued */
  GLINK_STATUS_NO_REMOTE_INTENT_FOUND    = -5,
  /* Failure of a glink_* call when channel is not fully opened yet */
  GLINK_STATUS_CH_NOT_FULLY_OPENED       = -6,
  /* Failure due to closing the same channel twice */
  GLINK_STATUS_CH_ALREADY_CLOSED         = -7,
  /* Returned when intent APIs are used over an intent less xport */
  GLINK_STATUS_API_NOT_SUPPORTED         = -8,
  /* Failure specific to QoS algorithm/implementation */
  GLINK_STATUS_QOS_FAILURE               = -9,
  /* Failure due to tx_cmd* calls */
  GLINK_STATUS_TX_CMD_FAILURE            = -10,
  /* For other Failures not covered above */
  GLINK_STATUS_FAILURE                   = -11,
  /* Failures relating to GLink operation timeout */
  GLINK_STATUS_TIMEOUT                   = -12
}glink_err_type;

/** List of possible subsystems */
/**
  "apss"   Application Processor Subsystem
  "mpss"   Modem subsystem
  "lpass"  Low Power Audio Subsystem
  "dsps"   Sensors Processor
  "wcnss"  Wireless Connectivity Subsystem
  "rpm"    Resource Power Manager processor
*/

/** Max allowed channel name length */
#define GLINK_CH_NAME_LEN 32

/** Version number for the glink_link_id_type structure */
#define GLINK_LINK_ID_VER  0x00000001

/** Macro to initialize the link identifier structure with default values.
 * It memsets the header to 0 and initializes the header field */
#define GLINK_LINK_ID_STRUCT_INIT(link_id) \
                          (link_id).remote_ss = 0; \
                          (link_id).link_notifier = 0; \
                          (link_id).handle = 0; \
                          (link_id).version = GLINK_LINK_ID_VER;


/* GLink tx options */
/* Flag for no options */
#define GLINK_TX_NO_OPTIONS      ( 0 )

#define GLINK_MAX_HOST_NAME 10

typedef void (*glink_link_notify_cb_type)(void);

typedef struct 
{
  char remote_ss[GLINK_MAX_HOST_NAME];
  glink_link_notify_cb_type cb; 
}glink_link_notify_type;


/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
/** Enums to identify link state */
typedef enum {
 GLINK_LINK_STATE_UP,  /* Called after specified link is online */
 GLINK_LINK_STATE_DOWN /* Called after SSR (before REMOTE_DISCONNECT) */
} glink_link_state_type;

/** Data structure holding the link state information */
typedef struct          glink_link_info_t {
  const char*           xport;      /* Transport name */
  const char*           remote_ss;  /* Remote subsystem name */
  glink_link_state_type link_state; /* Link state */
}glink_link_info_type;

/* Callback function invoked when link goes up/down */
typedef void (*glink_link_state_notif_cb)
(
  glink_link_info_type *link_info, /* Ptr to link information strcuture */
  void* priv                       /* Client private data */
);

/** 
 * Opaque handle returned by glink_register_link_state_cb. Client uses this
 * handle to call glink_deregister_link_state_cb() to deregister their
 * callback
 *
 * Client must not modify or try to interpret this value 
 */
typedef struct glink_link_notif_data_type * glink_link_handle_type;

/** Data structure to provide link information for the link to monitor */
typedef struct glink_link_id_t {
  glink_link_state_notif_cb link_notifier; /* Notification callback */
  glink_link_handle_type    handle;  /* Set by glink_register_link_state_cb */
}glink_link_id_type;

/** 
 * Opaque handle returned by glink_open. Client uses this handle to call into
 * GLink API for any further activity related to the channel,
 *
 * Client must to modify or try to interpret this value 
 */
typedef void* glink_handle_type;

/** GLink logical link state notifications */
typedef enum {
  /** Channel is fully opened. Both local and remote end have
      opened channel from their respective ends, and data communication
      can now take place */
  GLINK_CONNECTED = 0,

  /** Local side has called glink_close() and remote side has acknowledged
      this end's close. Client may call glink_open() after this point */
  GLINK_LOCAL_DISCONNECTED,

  /** Remote side has called glink_close() to close the channel */
  GLINK_REMOTE_DISCONNECTED
}glink_channel_event_type;

/** Data receive notification callback type*/  
typedef void (*glink_rx_notification_cb)
(
  glink_handle_type handle,     /* handle for the glink channel */
  const void        *priv,      /* priv client data passed in glink_open */
  const void        *pkt_priv,  /* private client data assiciated with the
                                   rx intent that client queued earlier */
  const void        *ptr,       /* pointer to the received buffer */
  size_t            size,       /* size of the packet */
  size_t            intent_used /* size of the intent used for this packet */
);


/** Data transmit notification callback type*/  
typedef void (*glink_tx_notification_cb)
(
  glink_handle_type handle,    /* handle for the glink channel */
  const void        *priv,     /* priv client data passed in glink_open */
  const void        *pkt_priv, /* private client data assiciated with the
                                  tx pkt that client queued earlier */
  const void        *ptr,      /* pointer to the transmitted buffer */
  size_t            size       /* size of the packet */
);

/** GLink channel state change notification callback type*/  
typedef void (*glink_state_notification_cb)
(
  glink_handle_type         handle, /* handle for the glink channel */
  const void                *priv,  /* priv client data passed in glink_open */
  glink_channel_event_type  event   /* Notification event */
);

/** tx abort notification. This callback would be invoked if client 
 *   had queued a tx buffer with glink and it had not been transmitted i.e.
 *   tx_done callback has not been called for this buffer and remote side
 *   or local side closed the port*/
typedef void(*glink_notify_tx_abort_cb)
(
glink_handle_type  handle,   /* handle for the glink channel */
const void         *priv,    /* priv client data passed in glink_open  */
const void         *pkt_priv /* pkt specific private data */
);

/**
 * Data Structure for GLink logical channel open configuration
 *
 * This structure is used by the clients to open a GLink logical channel
 * when calling glink_open()
 */
typedef struct {

  /** string name for the remote subsystem to which the user wants to 
      connect */
  const char                          *remote_ss;

  /** string name for the channel */
  const char                          *name;

  /** Private data for client to maintain context. This data is passed back
      to client in the notification callbacks */
  const void                          *priv;

  /** Data receive notification callback. Optional if notify_rxv is provided */
  glink_rx_notification_cb            notify_rx;

  /** Data transmit notification callback */
  glink_tx_notification_cb            notify_tx_done;

  /** GLink channel state notification callback */
  glink_state_notification_cb         notify_state;

  /** tx abort notification. This callback would be invoked if client
  *   had queued a tx buffer with glink and it had not been transmitted i.e.
  *   tx_done callback has not been called for this buffer and remote side
  *   or local side closed the port. Optional */
  glink_notify_tx_abort_cb            notify_tx_abort;

}glink_open_config_type;

/*===========================================================================
                      GLINK PUBLIC API
===========================================================================*/
/** 
 * Opens a logical GLink based on the specified config params
 *
 * @param[in]    cfg_ptr  Pointer to the configuration structure for the
 *                        GLink. See glink.h
 * @param[out]   handle   GLink handle associated with the logical channel
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Allocates channel resources and informs remote host about
 *               channel open.
 */
glink_err_type glink_open
(
  const  glink_open_config_type *cfg_ptr,
  glink_handle_type      *handle
);

/** 
 * Closes the GLink logical channel specified by the handle.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Closes local end of the channel and informs remote host
 */
glink_err_type glink_close
(
  glink_handle_type handle
);

/** 
 * Transmit the provided buffer over GLink.
 *
 * @param[in]    handle    GLink handle associated with the logical channel
 *
 * @param[in]   *pkt_priv  Per packet private data
 *
 * @param[in]   *data      Pointer to the data buffer to be transmitted
 *
 * @param[in]   size       Size of buffer
 *
 * @param[in]   options    Flags specifying how transmission for this buffer 
 *                         would be handled. See GLINK_TX_* flag definitions.
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  Causes remote host to wake-up and process rx pkt
 */
glink_err_type glink_tx
(
  glink_handle_type handle,
  const void        *pkt_priv,
  const void        *data,
  size_t            size,
  uint32            options
);

/** 
 * Queue one or more Rx intent for the logical GPIC Link channel.
 *
 * @param[in]    handle   GLink handle associated with the logical channel
 *
 * @param[in]   *pkt_priv Per packet private data
 *
 * @param[in]   size      Size of buffer
 *
 * @return       Standard GLink error codes
 *
 * @sideeffects  GLink XAL allocates rx buffers for receiving packets
 */
glink_err_type glink_queue_rx_intent
(
  glink_handle_type handle,
  const void        *pkt_priv,
  size_t            size
);

/** 
 * Client uses this to signal to GLink layer that it is done with the received 
 * data buffer. This API should be called to free up the receive buffer, which,
 * in zero-copy mode is actually remote-side's transmit buffer.
 *
 * @param[in]   handle   GLink handle associated with the logical channel
 *
 * @param[in]   *ptr     Pointer to the received buffer
 *
 * @param[in]   reuse    Reuse intent. Ignored on micro-cores.
 *
 * @return      Standard GLink error codes
 *
 * @sideeffects  GLink XAL frees the Rx buffer
 */
glink_err_type glink_rx_done
(
  glink_handle_type handle,
  const void        *ptr,
  boolean           reuse
);


void glink_register_notify_clients
(
 glink_link_notify_type* fn
);

void glink_notify_clients
(
 void* ctx_ptr
);

#ifdef __cplusplus
}
#endif

#endif //GLINK_H

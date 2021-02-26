#ifndef DIAGCOMM_IO_H
#define DIAGCOMM_IO_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O

General Description

Copyright (c) 2010-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagcomm_io.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
10/03/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
04/25/16   as      Removed this file's definition of QSOCKET_DIAG_ID, 
                     which will be defined in qsocket_ids.h
05/12/16   ph      SMD references removal and code clean up.
09/01/15   xy      Removed global diag_prev_tx_pending, diag_tx_iovec and diag_resend_data
04/08/15   ps      Diag over sockets
01/21/15   sr      Added changes to remove DSM items
10/31/14   rs      Removed EQUIP_ID_MAX.
07/06/14   ph      Added support for Diag over STM
07/16/14   sr      Resolved KW errors
04/10/14   sr      Support for peripheral buffering mode
09/27/13   xy      Removed code under FEATURE_WINCE
02/11/13   sr      Mainling Diag central routing
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/15/12   is      Support for preset masks
05/11/12   hvm     Added new api declarations for Tx wmq
03/07/12   rh      Added DIAG_MIN_STREAM_ID to go with DIAG_MAX_STREAM_ID
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/09/11   wjg     Added data_pending flag to diagcomm_io_conn_type for use
                     in diagcomm_smdl_buffer_read
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/


#include "customer.h"
#include "feature.h"                          /* For DIAG_INT_FEATURE_MASK_LEN */
#include "msgcfg.h"                           /* MSG_MASK_TBL_CNT */
#include "diag_cfg.h"
#include "log_codes.h"
#include "osal.h"
#include "diag_param.h"

#include "osal_qurt_types.h"
#include "qsocket_ipcr.h"
#include "diag_mode_v.h"

typedef struct qsockaddr_ipcr ipcr_addr_t;

/* ------------------------------------------------------------------------
** Typedefs and defines
** ------------------------------------------------------------------------ */
/* Diag Error codes */
#define DIAG_EOK                              0  /**< Operation successfully performed. */
#define DIAG_EPARAM                          -1  /**< Wrong values for parameters.  */
#define DIAG_EMEM                            -2  /**< Not enough memory to perform operation.*/
#define DIAG_EINVALID                        -3  /**< Invalid argument(s) */
#define DIAG_EUNKNOWN                        -4  /**< Unknown error */
#define DIAG_EFAILED                         -5  /**< Operation failed. */
#define DIAG_ENOTALLOWED                     -6  /**< Operation not allowed. */
#define DIAG_EHEAP                           -7  /**< No heap space available. */
#define DIAG_EUNREACH                        -8  /**< Destination unreachable. */
#define DIAG_ERETRY                          -9  /**< Retry */
#define DIAG_ELIMIT                          -10 /**< Limit reached */

#define DIAGCOMM_IO_NO_STREAM_ID -1 /* SIO_NO_STREAM_ID - Returned when sio_open() fails */
#define NUM_IO_CHANNELS 2
#define NUM_REMOTE_PORTS 2             /* size of diagcomm_enum_port_type */
#define NUM_SIO_PORTS 1

#define NUM_PORT_TYPES 1        /* size of diagcomm_port_type */

/* For log/event/f3 mask streams */
#define DIAG_MIN_STREAM_ID          1
#define DIAG_MAX_STREAM_ID          2
#define DIAG_STREAM_ALL             0 /* For all streams */
#define DIAG_STREAM_1               1 /* QXDM stream */
#define DIAG_STREAM_2               2 /* DCI stream */

/* Alias used for STM Mode (indicates which stream is piped to STM, if Diag STM is active) */
#define DIAG_STREAM_STM DIAG_STREAM_1
/* 'Listener' stream ID to indicate packet is needed by a listener.
   Needed when in STM mode to detect when STM msgs should also be placed into diagbuf */
#define DIAG_STREAM_LISTENER 0x80

#define DIAG_PRESET_MASK_ALL        0 /* For all presets */
#define DIAG_MIN_PRESET_ID          1
#define DIAG_MAX_PRESET_ID          2
#define DIAG_PRESET_MASK_1          1
#define DIAG_PRESET_MASK_2          2
#define DIAG_DCI_MASK_1             1
#define DIAG_MIN_DCI_ID             1
#define DIAG_MAX_DCI_ID             1

#define DIAG_INDEX_TO_ID( index )          (index+1)
#define DIAG_ID_TO_INDEX( id )             (id-1)
#define DIAG_ID_TO_PORT( id )              (id-1)       /* DIAG_STREAM_1 (1) -> DIAGCOMM_PORT_1 (0), etc */
#define DIAG_PORT_TO_INDEX( port_index )   (port_index) /* DIAGCOMM_PORT_1 (0) -> DIAG_STREAM_1 (1) */


typedef void (*diagcomm_close_cb_fnc_ptr_type)( void );

typedef enum
{
  DIAGCOMM_PORT_REMOTE_PROC = 0,  /*Port with remote proc */
  DIAGCOMM_PORT_EXTERNAL = 1      /*External port with USB/PCIE*/
} diagcomm_port_type;

/*lint -save -e{641} */
typedef enum
{
  DIAGCOMM_PORT_1 = 0, /*Stream 1 Port */
  DIAGCOMM_PORT_2 = 1, /*Stream 2 Port */
  DIAGCOMM_PORT_NUM_LAST = 1
} diagcomm_enum_port_type;
/*lint -restore */

typedef enum
{
  DIAGCOMM_IO_DATA = 0,
  DIAGCOMM_IO_CTRL = 1,
  DIAGCOMM_IO_CMD = 2

} diagcomm_io_channel_type;

typedef enum
{
  DIAGCOMM_IO_TYPE_INVALID = -1,
  /*Reserved*/
  /*Reserved */
  DIAGCOMM_IO_TYPE_IPCR_UDP = 2
} diagcomm_io_type;


typedef struct
{
    uint32            len;
    void             *buffer;
    diag_tx_mode_type *tx_ptr;
} diagcomm_io_tx_params_type;


 dword mask_dci_update[DIAG_MAX_DCI_ID];

  boolean * msg_mask_update[DIAG_MAX_STREAM_ID];
  boolean msg_mask_preset_update[DIAG_MAX_PRESET_ID][MSG_MASK_TBL_CNT];     /* Keeps track of which F3 mask has been updated for Stream 1 */
  #if defined(DIAG_STREAM_2_MASK_CHECK)
    boolean msg_mask_dci_update[DIAG_MAX_DCI_ID][MSG_MASK_TBL_CNT];
#endif
  boolean * log_mask_update[DIAG_MAX_STREAM_ID];
  boolean log_mask_preset_update[DIAG_MAX_PRESET_ID][LOG_EQUIP_ID_MAX + 1];         /* Keeps track of which equip id log mask has been updated for Stream 1 */
  boolean log_mask_dci_update[DIAG_MAX_DCI_ID][LOG_EQUIP_ID_MAX + 1];

  byte feature_mask[DIAG_INT_FEATURE_MASK_LEN];                             /* Diag feature mask of remote proc */


typedef struct
{
  diagcomm_io_type io_type;                  /* IO_TYPE_SIO or IO_TYPE_UDP */
  diagcomm_enum_port_type port_num;          /* PORT_1, PORT_2, etc */
  diagcomm_port_type port_type;              /* PORT_SMD or PORT_SIO */
  diagcomm_io_channel_type channel_type;     /* IO_DATA or IO_CTRL */
  boolean connected;                         /* Port connection state */
  boolean channel_connected;                 /* Channel (both ports) connection state */
  boolean data_pending;                      /* Connection waiting to be read from */
  boolean allow_flow;
  byte feature_mask[DIAG_INT_FEATURE_MASK_LEN];                             /* Diag feature mask of remote proc */

  boolean diag_prev_tx_pending;
  boolean diag_resend_data;      /*Flag to track if we are trying to resend the data IPCRTR signalled us*/

  void *buf;
  uint32 buf_length;
  uint32 used;
  osal_mutex_arg_t buf_mutex;
  diagcomm_io_type change_to_type;

  boolean isServer;
  ipcr_addr_t addr;
  int sock_handle;
  int send_flags;
  int recv_flags;
  int last_error;
} diagcomm_io_conn_type;

/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */
void diagcomm_io_init( void );
boolean diagcomm_io_open( diagcomm_io_conn_type * conn );
void diagcomm_io_close( diagcomm_io_conn_type * conn );
int diagcomm_io_transmit( diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param );
boolean diagcomm_io_status( diagcomm_io_conn_type * conn );

void diagcomm_io_enable_flow( diagcomm_io_conn_type * conn );

void diagcomm_io_conn_changed( diagcomm_io_conn_type * conn, boolean connected );
boolean diagcomm_io_has_tx_data( diagcomm_io_conn_type * conn );
void diagcomm_io_notify_rx(diagcomm_io_conn_type *conn);
void diagcomm_io_change_connection(diagcomm_io_type io_type_to);

#endif //DIAGCOMM_IO_H

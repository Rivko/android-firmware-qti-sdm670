#ifndef CCID_UIM_DAEMON_H
#define CCID_UIM_DAEMON_H
/*===========================================================================

  Copyright (c) 2011-2012, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/12   at      Updated function ccid_uim_send_sap_request
10/20/11   at      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "qmi.h"
#include "qmi_client.h"
#include <pthread.h>
#include <sys/ioctl.h>

#define DATA_SIZE 512

#define CCID_NOTIFY_CARD    _IOW('C', 1, struct ccid_uim_notification)
#define CCID_READ_DTR       _IOR('C', 3, int)

/* The 'DWG Smart-Card Integrated Circuit(s) Card Interface Devices' protocol defined notification/bulk-in/bulk-out structures */

/* Structure that notifies card status to PCSC tool */
struct ccid_uim_notification {
unsigned char   buf[4];
} __packed;

/* BULKIN - Data from PCSC tool to Card Reader */
struct ccid_uim_bulk_in_header {
unsigned char   message_type;
unsigned int    length;
unsigned char   slot;
unsigned char   seq_number;
unsigned char   status;
unsigned char   error;
unsigned char   specific;
unsigned char   data[DATA_SIZE];
unsigned char   size_to_send;
} __packed;

/* BULKOUT - Data from Card Reader to PCSC tool */
struct ccid_uim_bulk_out_header {
unsigned char   message_type;
unsigned int    length;
unsigned char   slot;
unsigned char   seq_number;
unsigned char   specific_0;
unsigned char   specific_1;
unsigned char   specific_2;
unsigned char   apdu[DATA_SIZE];
} __packed;


/*===========================================================================

                           DEFINES

===========================================================================*/
#define CCID_UIM_BULKOUT_BUFFER_SIZE      sizeof(struct ccid_uim_bulk_out_header)
#define CCID_UIM_CTRL_BUFFER_SIZE         4

/*===========================================================================

                           TYPES

===========================================================================*/

/* -----------------------------------------------------------------------------
   ENUM:      CCID_UIM_TTY_DEVNODE_TYPE

   DESCRIPTION:
     Possible USB TTY device nodes.
-------------------------------------------------------------------------------*/
typedef enum
{
  CCID_UIM_TTY_DEVNODE_BULK            = 0,
  CCID_UIM_TTY_DEVNODE_CTRL            = 1,
  CCID_UIM_TTY_DEVNODE_MAX             = 2
} ccid_uim_tty_devnode_type;


/* -----------------------------------------------------------------------------
   ENUM:      CCID_UIM_USB_STATE_TYPE

   DESCRIPTION:
     Possible USB states.
-------------------------------------------------------------------------------*/
typedef enum
{
  CCID_UIM_USB_STATE_UNKNOWN          = 0,
  CCID_UIM_USB_STATE_DISCONNECTED,
  CCID_UIM_USB_STATE_CONNECTED,
  CCID_UIM_USB_STATE_ERROR
} ccid_uim_usb_state_type;


/* -----------------------------------------------------------------------------
   STRUCT:      CCID_UIM_CMD_INFO_TYPE

   DESCRIPTION:
     Details of a CCID command used in asynchronous QMI command
-------------------------------------------------------------------------------*/
typedef struct
{
  uint8               req_msg_type;
  uint8               slot;
  uint8               sequence;
} ccid_uim_cmd_info_type;


/* -----------------------------------------------------------------------------
   STRUCT:      CCID_UIM_CARD_INFO_TYPE

   DESCRIPTION:
     Minimum card information needed for CCID
-------------------------------------------------------------------------------*/
typedef struct
{
  uim_card_state_enum_v01               card_state;
  uim_card_error_code_enum_v01          card_error_code;
  uim_sap_status_enum_v01               sap_state;
} ccid_uim_card_info_type;


/* -----------------------------------------------------------------------------
   STRUCT:      CCID_UIM_CARD_INFO_TYPE

   DESCRIPTION:
     Minimum card information needed for CCID
-------------------------------------------------------------------------------*/
typedef struct
{
  int                               fd;
  pthread_t                         thread_id;
} ccid_uim_device_node_type;


/* -----------------------------------------------------------------------------
   STRUCT:      CCID_UIM_STRUCT_TYPE

   DESCRIPTION:
     Global data with UIM status
-------------------------------------------------------------------------------*/
typedef struct
{
  int                                   usb_event_sd;
  ccid_uim_usb_state_type               usb_state;
  ccid_uim_card_info_type               card_info[QMI_UIM_CARDS_MAX_V01];
  ccid_uim_device_node_type             dev_node[CCID_UIM_TTY_DEVNODE_MAX];
  qmi_client_type                       qmi_uim_svc_client_ptr;
} ccid_uim_struct_type;


extern ccid_uim_struct_type   ccid_uim;


/*=========================================================================

  FUNCTION:  ccid_uim_send_write

===========================================================================*/
/*!
    @brief
    Performs the write command on the specified device node.

    @return
    None.
*/
/*=========================================================================*/
void ccid_uim_send_write
(
  int                           fd,
  uint8                      *  tx_buffer,
  uint16                        tx_buffer_len
);


/*=========================================================================

  FUNCTION:  ccid_uim_send_sap_request

===========================================================================*/
/*!
  @brief
    Sends the SAP APDU QMI request.

  @return
    None
*/
/*=========================================================================*/
boolean ccid_uim_send_sap_request
(
  uim_sap_request_req_msg_v01   * sap_request_req_ptr,
  ccid_uim_cmd_info_type        * cmd_info_ptr,
  boolean                         is_async
);


#endif /* CCID_UIM_DAEMON_H */


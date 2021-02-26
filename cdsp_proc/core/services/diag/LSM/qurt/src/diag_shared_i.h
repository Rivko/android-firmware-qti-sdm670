#ifndef DIAG_SHARED_I_H
#define DIAG_SHARED_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                GENERAL DESCRIPTION

  Diag-internal declarations  and definitions common to API layer
  (Diag_LSM) and diag driver (windiag).

Copyright (c) 2012-2015, 2017 by QUALCOMM Technologies.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE
$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/diag_shared_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implement seperate queue for UserPD events and enable buffering
06/08/17   gn      Added support for diag services to uImage
05/30/17   gn      Changes to support command deregistration
06/18/15   sa      Added support for time sync feature.
02/20/15   rh      Added client handle to table registration.
11/14/14   jtm/vk  Multi-PD Optimizations 
10/09/14   sr      Added stress test in UserPD
07/30/14   is      Do cleanup to support PD Restart
07/06/14   ph      Added support for Diag over STM
05/09/14   rs      Moving delayed response out of diag buffer. 
10/10/12   sg      Added client_id arg to mask chnage req functions to
                   use qurt_qdi_copy_to_user() function
07/05/12   sg      Changes to bring up Diag MultiPD
10/12/10   mad     Modified diag_modality_ctrl_params structure
09/15/10   mad     New mode of on-device logging: log to named MsgQ
07/23/10   mad     Added DIAG_IOCTL_ONDEV_QUERY and DIAG_CFG_UPDATE_SYNC_EVT_NAME
06/20/10   sg      Moved event_q_alloc, event_q_put to eventi.h and removed 
                   event_q_pending as we donot need it 
05/27/10   JV      Added prototype for diagdiag_get_diag_task_pri()
03/17/10   mad     Included comdef.h and windows.h
02/02/10   mad     Added Registry entries for on-device logging
11/19/09   mad     Added IOCTL codes for on-device logging
04/24/09   mad     Added IOCTL code to read delayed response id from windiag.
04/23/09   mad     Added structures etc for delayed response
01/16/09   mad     Added constant values for Diag Registry entries.
01/15/09   mad     Removed declaration of g_rx_buffer. Added prefix names for
                   synch. events.
10/03/08   mad     Created file.
===========================================================================*/

#include "comdef.h"
#include "qurt_qdi.h"
#include "qurt_signal.h"
/*Diag 1.5 Stream-driver model:
Adding a uint32 to the beginning of the diag packet,
so windiag driver can identify what this is. This will be stripped out in the 
WDG_Write() function, and only the rest of the data will
be copied to diagbuf. */
typedef struct
{
   uint32 diag_data_type; /* This will be used to identify whether the data passed to DCM is an event, log, F3 or response.*/
   uint8 rest_of_data;
}
diag_data;
#define DIAG_REST_OF_DATA_POS (FPOS(diag_data, rest_of_data))

/* different values that go in for diag_data_type */
#define DIAG_DATA_TYPE_EVENT                           0
#define DIAG_DATA_TYPE_F3                              1
#define DIAG_DATA_TYPE_LOG                             2
#define DIAG_DATA_TYPE_RESPONSE                        3
#define DIAG_DATA_TYPE_DELAYED_RESPONSE                4
#define DIAG_DATA_TYPE_STM_EVENT                       5
#define DIAG_DATA_TYPE_STM_F3                          6
#define DIAG_DATA_TYPE_STM_LOG                         7


/* The various IOCTLs */
#define DIAG_IOCTL_ERROR                               0 /* To have a default value for IOCTL Code DWORD */
#define DIAG_IOCTL_COMMAND_REG                         1 /* IOCTL for packet registration.
                                                            Clients can use this to register to 
                                                            respond to packets from host tool */
#define DIAG_IOCTL_COMMAND_DEREG                       2 /* IOCTL for de-registration. 
                                                          Client process uses this to 
                                                          de-register itself, while 
                                                          unloading gracefully. */
#define DIAG_IOCTL_MASK_REG                            3 /* IOCTL for registration for mask-change */
#define DIAG_IOCTL_MASK_DEREG                          4
#define DIAG_IOCTL_GETEVENTMASK                        5 /* For Client process to get event mask from DCM */
#define DIAG_IOCTL_GETLOGMASK                          6
#define DIAG_IOCTL_GETMSGMASK                          7
#define DIAG_IOCTL_GET_DELAYED_RSP_ID                  8 /* Diag_LSM uses this IOCTL to get the next delayed response id
                                                            in the system. */

#define DIAG_IOCTL_ONDEVICE_PORT_OPEN                  9
#define DIAG_IOCTL_USB_PORT_OPEN                       10
#define DIAG_IOCTL_PORT_CLOSE                          11
#define DIAG_IOCTL_CFG_READ                            12  /* Read mask-configuration file for on-device */
#define DIAG_IOCTL_ONDEV_QUERY                         13  /* Query On-device info, e.g. current log file */
#define DIAG_IOCTL_CTRL_DIAG                           14  /* Control diag sleep-vote, buffering modality etc */
#define DIAG_IOCTL_GET_STM_MODE                        15  /* IOCTL to retrive the stm information*/
#define DIAG_IOCTL_GET_TIME_API                        16
#define DIAG_IOCTL_GET_DIAG_ID                         17


#define DIAG_QDI_OPEN                                 ( 0 + QDI_PRIVATE)
#define DIAG_QDI_READ                                 ( 1 + QDI_PRIVATE)
#define DIAG_QDI_WRITE                                ( 2 + QDI_PRIVATE)
#define DIAG_QDI_CLOSE                                ( 3 + QDI_PRIVATE)
#define DIAG_QDI_IOCTL                                ( 4 + QDI_PRIVATE)
#define DIAG_QDI_BLOCK_FOR_NOTIFICATION               ( 5 + QDI_PRIVATE)
#define DIAG_QDI_SEND_PKT_READ_COMPLETE_NOTIFICATION  ( 6 + QDI_PRIVATE)
#define DIAG_QDI_KILL_RX_THREAD                       ( 7 + QDI_PRIVATE)
#define DIAG_QDI_GET_DIAGBUF                          ( 8 + QDI_PRIVATE)
#define DIAG_QDI_TRIGGER_DRAIN                        ( 9 + QDI_PRIVATE)
#define DIAG_QDI_SET_DRAIN_TIMER                      (10 + QDI_PRIVATE)
#define DIAG_QDI_CLEAR_STRESS_TEST_Q_NOTIFICATION     (11 + QDI_PRIVATE)
#define DIAG_QDI_INIT_EVENT_QUEUE                     (12 + QDI_PRIVATE)
#define DIAG_QDI_GET_MICRO_DIAGBUF                    (13 + QDI_PRIVATE)
#define DIAG_QDI_REQUEST_DIAGID                       (14 + QDI_PRIVATE)

#define DIAG_EVENTSVC_MASK_CHANGE                      1
#define DIAG_LOGSVC_MASK_CHANGE                        2
#define DIAG_MSGSVC_MASK_CHANGE                        4
#define SYNC_EVENT_DIAG_LSM_PKT_IDX                    8
#define DIAG_KILL_RX_THREAD                            16
#define DIAG_STM_MODE_CHANGE                           32
#define DIAG_KILL_STRESS_TEST_THREAD                   64  /*Signal used to clean up stress test queue*/
#define DIAG_TIME_API_CHANGE                          128
#define DIAG_DIAGID_CHANGE                            256

/* How many concurrent requests diag can handle? */
#define DIAG_MAX_CONCURRENT_REQS                       1

/* Name of synch. event, to signal diag thread that master
processor has processed the packet currently in global Rx buffer*/
#define DIAG_PKT_MSTR_READ_EVT_NAME      TEXT("DIAG_PKT_MASTER_READ")

/* Prefixes for Synch. event names, to notify diag clients.
ClientID (Process ID) is appended later, to make these names 
unique in the system. */
#define DIAG_LSM_PKT_EVENT_PREFIX _T("DIAG_SYNC_EVENT_PKT")
#define DIAG_LSM_MASK_EVENT_PREFIX _T("DIAG_SYNC_EVENT_MSK")


/* 
Changing the structure, for WM6.
We cannot pass "embedded" pointers, in WM6.
count and sync_obj_name will be repeated for each entry,
but command registration is not a frequent occurrence, so we can take the hit.
*/
typedef struct
{
   uint32 count; /* Number of entries in this bind */
   uint16 cmd_code;
   uint16 subsys_id;
   uint16 cmd_code_lo;
   uint16 cmd_code_hi;
   uint16 proc_id;
   uint32 event_id;
   uint32 log_code;
   uint32 client_id;
   qurt_signal_t * signal;
}
bindpkt_params_per_process;

/* functions used to communicate between 
API layer and diag driver. */
boolean diagpkt_tbl_reg_dcm(int client_handle, const byte *tbl_ptr, unsigned int count, qurt_signal_t *signal, int qdi_handle);
boolean diagpkt_tbl_dereg_dcm(const byte *tbl_ptr, int qdi_handle);
boolean event_process_LSM_mask_req(int client_handle, unsigned char *mask, int maskLen, int *maskLenReq);
boolean log_process_LSM_mask_req(int client_handle, unsigned char *mask, int maskLen, int *maskLenReq);
boolean msg_process_LSM_mask_req(int client_handle, unsigned char *mask, int maskLen, int *maskLenReq);
boolean diagpkt_mask_tbl_reg(uint32 client_id, qurt_signal_t *mask, int qdi_handle);
boolean diagpkt_mask_tbl_dereg(int qdi_handle);

/* prototype for function used to obtain the priority of
diag task */
uint16 diagdiag_get_diag_task_pri( void );

#endif /* DIAG_SHARED_I_H */

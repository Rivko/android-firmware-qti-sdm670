#ifndef DIAG_LSM_QDI_WRAPPER_H
#define  DIAG_LSM_QDI_WRAPPER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    Diag QDI Wrapper Header

General Description

we need to implement inline wrapper functions for our qdi driver in order to
hide the details of our QDI implementation from the user.

Copyright (c) 2012, 2014-2017 Qualcomm Technologies Incorporated. 
All Rights Reserved. Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                          Edit History 
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implement seperate queue for UserPD events and enable buffering
06/08/17   gn      Added support for diag services to uImage
11/14/14   jtm/vk  Multi-PD Optimizations 
10/09/14   sr      Added stress test in UserPD
07/24/14   xy      Resolve 8994 Diag warnings with new LLVM 7.0.04 toolchain  
02/12/14   jb      Marked inline functions as extern for LLVM
07/05/12   sg      Created File 
===========================================================================*/
#include "comdef.h"
#include "diagbuf_mpd_type.h"
#ifdef DIAG_UIMAGE_MODE
#include "micro_diagbuf_mpd_type.h"
#endif
/*===========================================================================

FUNCTION  diag_lsm_read_data

DESCRIPTION
 This function reads the data from guest os diag driver

RETURN VALUE
  Returns number of bytes read.

===========================================================================*/
int diag_lsm_read_data(byte *buf, int length);

/*===========================================================================

FUNCTION diag_lsm_write_data

DESCRIPTION
  This function writes the data to guest os diag driver


RETURN VALUE
  Returns number of bytes written.

===========================================================================*/
int diag_lsm_write_data(byte *buf, int length);

/*===========================================================================

FUNCTION diag_lsm_ioctl

DESCRIPTION
 This function ioctls into guest os diag driver

RETURN VALUE
  Returns the bytes for the ioctl code.

===========================================================================*/
int diag_lsm_ioctl(int diag_qdi_ioctl, byte *inbuf, dword inbuflen, byte *outbuf, dword outbuflen, dword *out_buf_actual_bytes_ptr);

/*===========================================================================

FUNCTION diag_lsm_wait_for_event

DESCRIPTION
  This function waits on a set of signals to be set to the Diag LSM.
  Signals are set when there is a mask change or when a command is available for the client

RETURN VALUE
  Indicates which signal is set to LSM.

===========================================================================*/
int diag_lsm_wait_for_event(void);

/*===========================================================================

FUNCTION diag_lsm_send_pkt_read_complete_notify

DESCRIPTION
 This function indicates to the diag in guest os that the LSM client has 
 read the command completely

RETURN VALUE
  None.

===========================================================================*/
int diag_lsm_send_pkt_read_complete_notify(void);

/*===========================================================================

FUNCTION diag_lsm_get_diag_buf

DESCRIPTION
 This function gets diagbuf ptr from guest OS 

RETURN VALUE
  -1 if Error
   0 otherwise.

===========================================================================*/
int diag_lsm_get_diag_buf(diagbuf_mpd_type **ptr, int buf_len,int pid);

/*===========================================================================

FUNCTION diag_lsm_init_event_queue

DESCRIPTION
 This function initialises the event_struct for any userpd.
 Once the userpd calls diag_lsm_init, a QDI call is made to call this function.

RETURN VALUE
  -1 if Error
   0 otherwise.

===========================================================================*/
int diag_lsm_init_event_queue(diagbuf_mpd_type **ptr, int buf_len);

#ifdef DIAG_UIMAGE_MODE
/*===========================================================================

FUNCTION diag_lsm_get_diag_buf

DESCRIPTION
 This function gets diagbuf ptr from guest OS 

RETURN VALUE
  -1 if Error
   0 otherwise.

===========================================================================*/
int diag_lsm_get_micro_diag_buf(micro_diagbuf_mpd_type **ptr, int buf_len);

#endif

/*===========================================================================

FUNCTION diag_lsm_trigger_drain

DESCRIPTION
 This function causes diag drain in guest OS 

RETURN VALUE
  0 Always.

===========================================================================*/
int diag_lsm_trigger_drain(void);

/*===========================================================================

FUNCTION diag_lsm_set_drain_timer

DESCRIPTION
 This function causes diag buf drain timer to be set in guest OS

RETURN VALUE 
   0 Always.

===========================================================================*/
int diag_lsm_set_drain_timer(void);

/*===========================================================================

FUNCTION diag_lsm_clear_stress_Test_q

DESCRIPTION
 This function indicates to the diag in guest os that the stress test task
 has completed and the stress test queue can be cleaned

RETURN VALUE
  Indicates which signal is set to LSM.

===========================================================================*/
int diag_lsm_clear_stress_test_q(void);

/*===========================================================================

FUNCTION diag_lsm_request_diagID

DESCRIPTION
  Request a DiagID assignment for the user PD.
 
PARAMETERS 
  name   - Process name
  guid   - Qshrink4 GUID byte array, of size QSHRINK_4_DATABASE_GUID_LENGTH
           If Qshrink4 not supported, pass in NULL.
           
RETURN VALUE
  None.

===========================================================================*/
int diag_lsm_request_diagID(char* name, uint8 *guid);

#endif

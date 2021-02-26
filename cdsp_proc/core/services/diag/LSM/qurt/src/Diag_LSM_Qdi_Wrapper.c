
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
    Diag QDI Wrapper 

General Description

we need to implement inline wrapper functions for our qdi driver in order to
hide the details of our QDI implementation from the user.

Copyright (c) 2012,2014,2017 Qualcomm Technologies, Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                          Edit History 
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/08/17   gn      Added support for diag services to uImage
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implement seperate queue for UserPD events and enable buffering
11/14/14   jtm/vk  Multi-PD Optimizations 
10/09/14   sr      Added stress test in UserPD
07/24/14   xy      Resolve 8994 Diag warnings with new LLVM 7.0.04 toolchain   
02/12/14   jb      Removed inline value of functions, as header file has functions as extern inline
07/05/12   sg      Created File

===========================================================================*/
#include "Diag_LSMi.h"
#include "qurt_qdi.h"
#include "Diag_LSM_Qdi_wrapper.h"
#include "diag_shared_i.h"

inline int diag_lsm_read_data(byte *buf, int length)
{
   int return_value = 0;
   return_value = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_READ, buf, length);

   return (return_value);
}

inline int diag_lsm_write_data(byte *buf, int length)
{
   int return_val = 0;
   return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_WRITE, buf, length);

   return (return_val);
}

inline int diag_lsm_ioctl(int diag_qdi_ioctl, byte *inbuf, dword inbuflen, byte *outbuf, dword outbuflen, dword *out_buf_actual_bytes_ptr)
{
   int return_val = 0;
   return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_IOCTL, diag_qdi_ioctl, inbuf, inbuflen, outbuf, outbuflen, out_buf_actual_bytes_ptr);

   return (return_val);
}

inline int diag_lsm_wait_for_event(void)
{
   int return_val = 0;
   return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_BLOCK_FOR_NOTIFICATION);

   return (return_val);
}

inline int diag_lsm_send_pkt_read_complete_notify(void)
{
   int return_val = 0;
   return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_SEND_PKT_READ_COMPLETE_NOTIFICATION);

   return (return_val);
}

inline int diag_lsm_get_diag_buf( diagbuf_mpd_type **ptr, int buf_len,int pid)
{
   int return_val = 0;
  return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_GET_DIAGBUF, ptr, buf_len, pid);

   return (return_val);
}

inline int diag_lsm_init_event_queue( diagbuf_mpd_type **ptr, int buf_len)
{
  int return_val = 0;
  return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_INIT_EVENT_QUEUE, ptr, buf_len);

   return (return_val);
}

#ifdef DIAG_UIMAGE_MODE
inline int diag_lsm_get_micro_diag_buf(micro_diagbuf_mpd_type **ptr, int buf_len)
{
   int return_val = 0;
    /* Send DIAG_QDI_GET_MICRO_DIAGBUF IOCTL to Root PD to allocate and initialize a shared buffer*/
   return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_GET_MICRO_DIAGBUF, ptr, buf_len);

   return (return_val);
}
#endif
inline int diag_lsm_trigger_drain(void)
{
   int return_val = 0;
   return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_TRIGGER_DRAIN);

   return (return_val);
}

inline int diag_lsm_set_drain_timer(void)
{
   int return_val = 0;
   return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_SET_DRAIN_TIMER);

   return (return_val);
}

inline int diag_lsm_clear_stress_test_q(void)
{
   int return_val = 0;
   return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_CLEAR_STRESS_TEST_Q_NOTIFICATION);

  return return_val;
}
inline int diag_lsm_request_diagID(char* name, uint8 *guid)
{
  int return_val = 0;
  return_val = qurt_qdi_handle_invoke(diag_qdi_handle, DIAG_QDI_REQUEST_DIAGID, name, guid);
  
  return return_val;
}


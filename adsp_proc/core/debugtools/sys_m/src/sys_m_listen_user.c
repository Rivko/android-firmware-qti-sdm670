/** vi: tw=128 ts=3 sw=3 et
@file sys_m_listen_user.c
@brief This file contains the API for the System Monitor, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014-17 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/sys_m/src/sys_m_listen_user.c#2 $
$DateTime: 2017/09/01 01:33:34 $
$Change: 14246640 $
$Author: pwbldsvc $
===============================================================================*/

#include "rcecb.h"
#include "rcevt.h"
#include "sys_m_messages.h"
#include "sys_m_internal.h"
#include "sys_m_sizes.h"
#include "sys_m_driver_internal.h"
#include "rcinit_qurt.h"
#include "err.h"
#include "tms_dll_api.h"
#include "tms_utils_msg.h"

#define SYS_M_MAX_QURT_THREAD_PRIO 255

static struct
{
   qurt_thread_t tid;

} sys_m_listen_internal;

SYS_M_DEVICE_HANDLE sys_m_listen_init(char const* name)
{
   SYS_M_DEVICE_HANDLE rc = SYS_M_DEVICE_STATUS_ERROR;
   char device[sizeof(SYS_M_LISTEN_DEVICE_PATH) + SYS_M_DEVICE_NAME_LEN];

   if (SYS_M_NULL != name &&
       SYS_M_DEVICE_NAME_LEN > strnlen(name, SYS_M_DEVICE_NAME_LEN) &&
       sizeof(device) > strlcpy(device, SYS_M_LISTEN_DEVICE_PATH, sizeof(device)) &&
       sizeof(device) > strlcat(device, name, sizeof(device)))
   {
      rc = qurt_qdi_open(device, 0, 0);
   }

   return rc;
}

SYS_M_DEVICE_STATUS sys_m_listen_destroy(SYS_M_DEVICE_HANDLE handle)
{
   SYS_M_DEVICE_STATUS rc = SYS_M_DEVICE_STATUS_ERROR;

   if (0 <= qurt_qdi_close(handle))
   {
      rc = SYS_M_DEVICE_STATUS_SUCCESS;
   }

   return rc;
}

SYS_M_HANDLE sys_m_listen_func_register_name(SYS_M_NAME const name, SYS_M_FNSIG const func)
{
   SYS_M_HANDLE rc = SYS_M_NULL;

   if (SYS_M_NULL != name &&
       SYS_M_NULL != func &&
       SYS_M_NAME_MAX > strnlen(name, SYS_M_NAME_MAX))
   {
      rc = (SYS_M_HANDLE)rcecb_register_name(name, func);
   }

   return rc;
}

SYS_M_HANDLE sys_m_listen_func_unregister_name(SYS_M_NAME const name, SYS_M_FNSIG const func)
{
   SYS_M_HANDLE rc = SYS_M_NULL;

   if (SYS_M_NULL != name &&
       SYS_M_NULL != func &&
       SYS_M_NAME_MAX > strnlen(name, SYS_M_NAME_MAX))
   {
      rc = (SYS_M_HANDLE)rcecb_unregister_name(name, func);
   }

   return rc;
}

SYS_M_DEVICE_STATUS sys_m_listen_read(SYS_M_DEVICE_HANDLE handle, char* out_p, size_t out_sz)
{
   SYS_M_DEVICE_STATUS rc = SYS_M_DEVICE_STATUS_ERROR;

   if (SYS_M_NULL != out_p &&
       SYS_M_NAME_MAX >= out_sz &&
       0 <= qurt_qdi_handle_invoke(handle, SYS_M_LISTEN_METHOD_READ, out_p, out_sz))
   {
      rc = SYS_M_DEVICE_STATUS_SUCCESS;
   }

   return rc;
}

SYS_M_DEVICE_STATUS sys_m_listen_ack(SYS_M_DEVICE_HANDLE handle)
{
   SYS_M_DEVICE_STATUS rc = SYS_M_DEVICE_STATUS_ERROR;

   if (0 <= qurt_qdi_handle_invoke(handle, SYS_M_LISTEN_METHOD_ACK))
   {
      rc = SYS_M_DEVICE_STATUS_SUCCESS;
   }

   return rc;
}

static void SYS_M_LISTEN_WORKER(void *argv __attribute__((unused)))
{
   SYS_M_DEVICE_HANDLE handle;

   if (0 <= (handle = sys_m_listen_init("USER")))
   {
      do
      {
         char out[SYS_M_NAME_MAX];

         if (0 <= sys_m_listen_read(handle, out, sizeof(out)))       // blocking wait within qdi driver
         {
            TMS_SHUTDOWN_MSG_SPRINTF_HIGH_1("sys_m_listen_read event: %s", out);
            int ackcnt = rcevt_getcount_name(SYS_M_ACK);             // ack count
            int regcnt = rcevt_getregistrants_name(out);                // registrants

            if (rcecb_signal_name(out)){ }

            if(rcevt_signal_name(out)){ }

            if (rcevt_wait_count_name(SYS_M_ACK, ackcnt + regcnt))   // blocking wait
            {
            }
         }

         if (0 <= sys_m_listen_ack(handle))                          // ack to qdi driver
         {
         }

      } while (1);

      if (0 <= sys_m_listen_destroy(handle))
      {
      }

      qurt_thread_exit(0);                                           // thread exit
   }

   else
   {
      qurt_thread_exit(1);
   }

}

DLL_API_GLOBAL void sys_m_listen_user_init(void)
{
   qurt_thread_attr_t attr;
   static unsigned long stack[SYS_M_LISTEN_WORKER_STACK/sizeof(unsigned long)];
   RCINIT_INFO info_handle = NULL;
   RCINIT_PRIO prio = 0;
   unsigned long stksz = 0;

   info_handle = rcinit_lookup(SYS_M_LISTEN_WORKER_NAME);

   if (!info_handle) 
   {
      ERR_FATAL("smlworker task info not found",0 , 0, 0);
   }
   else
   {
      prio = rcinit_lookup_prio_info(info_handle);
      stksz = rcinit_lookup_stksz_info(info_handle);
      if ((prio > SYS_M_MAX_QURT_THREAD_PRIO) || (stksz == 0)) 
      {
         ERR_FATAL("Invalid Priority:%d or Stack Size: %d",prio, stksz, 0);
      }
      prio = SYS_M_MAX_QURT_THREAD_PRIO - prio; /*normalizing task prio as per QuRT task prio order*/
   }

   qurt_thread_attr_init(&attr);
   qurt_thread_attr_set_name(&attr, SYS_M_LISTEN_WORKER_NAME);
   qurt_thread_attr_set_stack_addr(&attr, stack);
   qurt_thread_attr_set_stack_size(&attr, stksz);
   qurt_thread_attr_set_priority(&attr, prio);

   qurt_thread_create(&sys_m_listen_internal.tid, &attr, SYS_M_LISTEN_WORKER, NULL);
}

void sys_m_listen_user_term(void)
{
   /* NULL */ /* DECISION TO NOT CLEANUP SERVICE FOR POST MORTEM REASONS */
}

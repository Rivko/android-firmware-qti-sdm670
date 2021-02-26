/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_pd_root.c
@brief This file contains the API for the Run Control Framework, API 3.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/rcinit/src/rcinit_drvr_root.c#2 $
$DateTime: 2017/09/01 01:33:34 $
$Change: 14246640 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)

#include "stdint.h"
#include "stdlib.h"
#include "stringl/stringl.h"

#include "tms_utils.h"
#include "tms_rcinit.h"
#include "rcinit_qurt.h"
#include "qurt_qdi_driver.h"

#if !defined(RCINIT_EXCLUDE_TRACER_SWEVT)
#include "tracer.h"
#include "rcinit_tracer_swe.h"
#endif

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
#include "tms_utils_msg.h"
#endif

struct rcinit_device_s
{
   qurt_qdi_obj_t qdiobj;                 /**< REQUIRED TO BE FIRST */

   char name[RCINIT_DEVICE_NAME_LEN];     /**< name */

   uint32_t hash;                         /**< name hash */

   qurt_sysenv_procname_t pn;             /**< client procname on open */

   struct rcinit_device_s* next;
};

static struct rcinit_device_internal_s
{
   qurt_mutex_t mutex;                    /**< driver serilization */

   struct rcinit_device_s* list;          /**< driver list */

} rcinit_device_internal;

/** forward references */
static void rcinit_device_release(qurt_qdi_obj_t *qdiobj);
static int rcinit_device_invocation(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9);
static int rcinit_device_open(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9);
static int rcinit_device_close(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9);

/** implementation */
static void rcinit_device_release(qurt_qdi_obj_t *qdiobj)
{
   free(qdiobj);
}

/** implementation */
static int rcinit_device_invocation(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{
   int rc = RCINIT_DEVICE_STATUS_ERROR; /**< failure */

   do
   {
      qurt_sysenv_procname_t pn = {0};
      struct rcinit_device_s* device_p = (struct rcinit_device_s*)obj;

      /* client_handle means that we're asking this particular client handle to answer the question about its process */
      /* QDI_OS_SYSENV is the standard method for requesting this type of information */
      /* QDI_HANDLE_LOCAL_CLIENT means that this request originated locally */
      /* QURT_SYSENV_PROCNAME_TYPE is the specific piece of data being requested -- to fill in a qurt_sysenv_procname_t structure */
      /* &pn is the structure we want filled in */

      if (RCINIT_NULL == device_p
          || 0 > qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &pn)
          || RCINIT_DEVICE_ASID_MAX >= pn.asid)
      {

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_STARTUP_MSG_HIGH_2("Cannot obtain client asid %d %d", pn.asid, rc);
#endif

         break;
      }

      /**
      API, rcinit_device_init
      @param[in] a1.ptr/const char* RCINIT_NULL Terminated Device Name
      @return
      RCINIT_DEVICE_HANDLE -- Opaque Device Handle
      */
      if (QDI_OPEN == method)
      {
         rc = rcinit_device_open(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9);

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_STARTUP_MSG_HIGH_2("QDI OPEN %d: %x", pn.asid, rc);
#endif

         break;
      }

      /**
      API, rcinit_device_destroy
      @param[in] RCINIT_DEVICE_HANDLE Opaque Device Handle
      @return
      RCINIT_DEVICE_STATUS -- Error Indication
      */
      else if (QDI_CLOSE == method)
      {
         rc = rcinit_device_close(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9);

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_SHUTDOWN_MSG_HIGH_2("QDI CLOSE %d: %x", pn.asid, rc);
#endif

         break;
      }

      /**
      INTERNAL, OS_DUMP_PROCESS_EXIT Handler
      a1.ptr is a pointer to a list of (address, size) pairs
      a2.num is the number of entries pointed to by a1.ptr
      a3.ptr is a pointer to a list of TCB addresses for the threads
      a4.num is the number of entries pointed to by a3.ptr
      a5.num is the size of each TCB
      */
      else if (QDI_OS_DUMP_PROCESS_EXIT == method)
      {
         rc = RCINIT_DEVICE_STATUS_SUCCESS;

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_STARTUP_MSG_HIGH_2("OS_DUMP_PROCESS_EXIT %d: %x", pn.asid, rc);
#endif

         break;
      }

      /**
      API, rcinit_device_method_0
      @param[in] RCINIT_DEVICE_HANDLE Opaque Device Handle
      @return
      RCINIT_DEVICE_STATUS -- Error Indication
      */
      else if (RCINIT_DEVICE_METHOD_0 == method)
      {
         rc = RCINIT_DEVICE_STATUS_SUCCESS;

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_STARTUP_MSG_HIGH_2("Method 0 %d: %x", pn.asid, rc);
#endif

         break;
      }

      /**
      API, rcinit_device_method_1
      @param[in] RCINIT_DEVICE_HANDLE Opaque Device Handle
      @return
      RCINIT_DEVICE_STATUS -- Error Indication
      */
      else if (RCINIT_DEVICE_METHOD_1 == method)
      {
         rc = RCINIT_DEVICE_STATUS_SUCCESS;

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_STARTUP_MSG_HIGH_2("Method 1 %d: %x", pn.asid, rc);
#endif

         break;
      }

      /**
      INTERNAL, Default Method Handler
      */
      else
      {
         rc = qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9);

#if !defined(RCINIT_EXCLUDE_MSG_SWEVT)
         TMS_STARTUP_MSG_HIGH_2("Method DEFAULT %d: %x", pn.asid, rc);
#endif

         break;
      }

   } while (0);

   return rc;
}

/** implementation */
static int rcinit_device_open(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{
   int rc = RCINIT_DEVICE_STATUS_ERROR;
   struct rcinit_device_s* device_p = (struct rcinit_device_s*)obj;

   qurt_pimutex_lock(&rcinit_device_internal.mutex);

   do
   {
      char const* name;
      int name_len;

      if (RCINIT_NULL == a1.ptr)
      {
         break;
      }

      name = tms_utils_chr_last(a1.ptr, RCINIT_DEVICE_PATH_LEN + RCINIT_DEVICE_NAME_LEN, '/');

      if (RCINIT_NULL == name)
      {
         break;
      }

      name_len = tms_utils_chr_length(name, sizeof(device_p->name));

      if (1 >= name_len)
      {
         break;
      }

      name++; name_len--; /** skips the trailing '/' component of the device path prefix */

      for (device_p = rcinit_device_internal.list; RCINIT_NULL != device_p; device_p = device_p->next)
      {
         if (0 == tms_utils_chr_compare(device_p->name, sizeof(device_p->name), name, sizeof(device_p->name)))
         {
            break;
         }
      }

      if (RCINIT_NULL == device_p)
      {
         /** operate on existing */

         rc = RCINIT_DEVICE_STATUS_SUCCESS;

         break;
      }

      else
      {
         /** operate on new */

         device_p = malloc(sizeof(*device_p));

         if (RCINIT_NULL == device_p)
         {
            break;
         }

         /** initialize new */

         secure_memset(device_p, 0, sizeof(*device_p));

         device_p->qdiobj.invoke = rcinit_device_invocation;
         device_p->qdiobj.refcnt = QDI_REFCNT_INIT;
         device_p->qdiobj.release = rcinit_device_release;

         /** device object init */

         memsmove(device_p->name, sizeof(device_p->name), name, name_len);

         device_p->hash = tms_utils_hash(device_p->name, sizeof(device_p->name));

         if (qurt_qdi_handle_invoke(client_handle, QDI_OS_SYSENV, QDI_HANDLE_LOCAL_CLIENT, QURT_SYSENV_PROCNAME_TYPE, &device_p->pn)) {;}

         /** link into device list */

         device_p->next = rcinit_device_internal.list;

         rcinit_device_internal.list = device_p;

         rc = RCINIT_DEVICE_STATUS_SUCCESS;

         break;
      }

   } while (0);

   qurt_pimutex_unlock(&rcinit_device_internal.mutex);

   return rc;
}

/** implementation */
static int rcinit_device_close(int client_handle, qurt_qdi_obj_t* obj, int method, qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3, qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6, qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{
   int rc = RCINIT_DEVICE_STATUS_ERROR;
   // struct rcinit_device_s* device_p = (struct rcinit_device_s*)obj; // unused variable warning fix

   qurt_pimutex_lock(&rcinit_device_internal.mutex);

   do
   {
      break;

   } while (0);

   qurt_pimutex_unlock(&rcinit_device_internal.mutex);

   rc = qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3, a4, a5, a6, a7, a8, a9);

   return rc;
}

/**
INTERNAL, QDI Opener Invocation Object
*/
static struct
{
   qurt_qdi_obj_t qdiobj;

} opener = { { rcinit_device_invocation, QDI_REFCNT_PERM, 0 } };

/**
API, Initialization of service prior to use
@return
None.
*/
RCINIT_BOOL rcinit_driver_init(void)
{
   qurt_pimutex_init(&rcinit_device_internal.mutex);

   qurt_pimutex_lock(&rcinit_device_internal.mutex);

   qurt_qdi_devname_register(RCINIT_DEVICE, &opener.qdiobj);

   qurt_pimutex_unlock(&rcinit_device_internal.mutex);

   return RCINIT_TRUE;
}

/**
Termination of service following use
@return
None.
*/
RCINIT_BOOL rcinit_driver_term(void)
{
   return RCINIT_TRUE;
}

#endif

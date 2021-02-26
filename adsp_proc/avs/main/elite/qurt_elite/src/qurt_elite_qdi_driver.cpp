/*========================================================================
qurt_elite_qdi_driver.cpp

This file contains a qdi driver implementation

Copyright (c) 2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_qdi_driver.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
6/13/2017   rbhatnk      Created file.
========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite_qdi_driver.h"


#ifndef SIM
//#define ENABLE_LOG
#endif

struct opener {
   qurt_qdi_obj_t qdiobj;
};

struct driver {
   qurt_qdi_obj_t qdiobj;
   struct opener* opener;
};

//called for any QDI operation such as add_remove_pages
static int driver_invoke(int client_handle,
                         qurt_qdi_obj_t *pobj,
                         int method,
                         qurt_qdi_arg_t a1,
                         qurt_qdi_arg_t a2,
                         qurt_qdi_arg_t a3,
                         qurt_qdi_arg_t a4,
                         qurt_qdi_arg_t a5,
                         qurt_qdi_arg_t a6,
                         qurt_qdi_arg_t a7,
                         qurt_qdi_arg_t a8,
                         qurt_qdi_arg_t a9)
{
   int rc=0;
   int nErr = 0;
   
   switch(method) {
   case QDI_QURT_ELITE_ADD_REMOVE_PAGE:
   {
#ifdef ENABLE_LOG
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi: add/remove in guestos");
#endif

      //qurt_qdi_handle_invoke(handle, QDI_QURT_ELITE_ADD_REMOVE_PAGE, pool, is_add, addr, num_pages);
      if (a2.num)
      {
         rc = qurt_mem_pool_add_pages(a1.num, a3.num, a4.num);
      }
      else
      {
         rc = qurt_mem_pool_remove_pages(a1.num, a3.num, a4.num, QURT_POOL_REMOVE_ALL_OR_NONE, 0, 0);
      }
      break;
   }
   default:
   {
#ifdef ENABLE_LOG      
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi : qurt_qdi_method_default in driver_invoke");
#endif      
      //release handle is called inside this
      nErr = qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
      if (nErr)
      {
#ifdef ENABLE_LOG
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_qdi : qurt_qdi_method_default failed driver_invoke");
#endif
      }
      break;
   }
   }
#ifdef ENABLE_LOG
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi: returning rc = %d for method %d", rc, method);
#endif
   

   return rc;
}

//called automatically when QDI_CLOSE is called which in turn is called when qurt_elite_qdi_close is called.
static void driver_release(qurt_qdi_obj_t *pobj) {
   struct driver *me = (struct driver *)pobj;
#ifdef ENABLE_LOG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi: driver release");
#endif
   if(me) {
      free(me);
   }
}

//called each time qurt_elite_qdi_open is called.
static int driver_open(struct opener* opener, int client_handle, struct opener* me, int *handle) {
   struct driver *driver = 0;
   int nErr = 0;

   driver = (struct driver*)malloc(sizeof(*driver));
   if (NULL == driver)
   {
#ifdef ENABLE_LOG
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_qdi : drive malloc failed");
#endif
      nErr = 1;
      goto bail;
   }

   memset(driver, 0, sizeof(struct driver));

   driver->qdiobj.invoke = driver_invoke;
   driver->qdiobj.refcnt = QDI_REFCNT_INIT;
   driver->qdiobj.release = driver_release;
   driver->opener = opener;

   *handle = qurt_qdi_new_handle_from_obj_t(client_handle, &driver->qdiobj);
   if (*handle == NULL)
   {
#ifdef ENABLE_LOG
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_qdi : new qdi handle call failed");
#endif
      nErr = 1;
      goto bail;
   }

#ifdef ENABLE_LOG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi: driver open");
#endif

bail:
   if(nErr && driver) {
      free(driver);
   }
   return nErr;
}

//called each time qurt_elite_qdi_open is called.
static int opener_invoke(int client_handle,
                         qurt_qdi_obj_t *pobj,
                         int method,
                         qurt_qdi_arg_t a1,
                         qurt_qdi_arg_t a2,
                         qurt_qdi_arg_t a3,
                         qurt_qdi_arg_t a4,
                         qurt_qdi_arg_t a5,
                         qurt_qdi_arg_t a6,
                         qurt_qdi_arg_t a7,
                         qurt_qdi_arg_t a8,
                         qurt_qdi_arg_t a9) {

   struct opener *me = (struct opener *)pobj;
   int nErr = 0;

   switch(method) {
   case QDI_OPEN: {
      int handle = -1;
      nErr = driver_open(me, client_handle, me, &handle);
      if (nErr)
      {
#ifdef ENABLE_LOG
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_qdi : driver_open failed");
#endif
      }
#ifdef ENABLE_LOG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi: driver_open called nErr = %d", nErr);
#endif
      return handle;
   }
   default:
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi : qurt_qdi_method_default in opener_invoke");
      nErr = qurt_qdi_method_default(client_handle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
      if (nErr)
      {
#ifdef ENABLE_LOG
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "qurt_elite_qdi : qurt_qdi_method_default failed opener_invoke");
#endif
      }
      break;
   }
//bail:
   return nErr;
}

//never called
void qurt_elite_qdi_deinit(void)
{
#ifdef ENABLE_LOG
   int nErr = 0;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi : deinit. error code %d", nErr);
#endif
}

//called at boot-up through scons mechanism
static struct opener  gopener;
int qurt_elite_qdi_init(void)
{
   int nErr = 0;
   struct opener* op = &gopener;

   op->qdiobj.invoke = opener_invoke;
   op->qdiobj.refcnt = QDI_REFCNT_PERM;
   op->qdiobj.release = 0;

   nErr = qurt_qdi_devname_register(QDI_QURT_ELITE_NAME, (qurt_qdi_obj_t *) op);

   if(nErr) {
      qurt_elite_qdi_deinit();
   }
#ifdef ENABLE_LOG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "qurt_elite_qdi : init done. error code %d", nErr);
#endif

   return nErr;
}


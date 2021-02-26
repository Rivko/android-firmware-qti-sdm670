
/*==============================================================================
  FILE:         pram_mgr_qdi.c
  
  OVERVIEW:     Pram mgr qdi service layer

  DEPENDENCIES: None

                Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/products/pram_mgr/src/pram_mgr_qdi.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
==============================================================================*/

#include "qurt.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "pram_mgr.h"
#include "pram_mgr_v.h"
#include "pram_mgr_qdi_v.h"


#define PRAM_MGR_NUM_USER_PD 3 
#define PRAM_MGR_MAX_CLIENTS PRAM_MGR_NUM_USER_PD  /* Pram manager QDI API not supported in rootPD */

typedef struct pram_mgr_qdi_opener_type {
   qurt_qdi_obj_t qdiobj;
   int client_handle;
   boolean opener_allocated;
} pram_mgr_qdi_opener;


pram_mgr_qdi_opener pram_mgr_qdi_clients[PRAM_MGR_MAX_CLIENTS];
uint32 pram_mgr_qdi_clients_cntr = 0;

qurt_mutex_t pram_mgr_qdi_mutex;
/*MACRO for locking/unlocking the mutex*/
#define PRAM_MGR_QDI_LOCK()  \
    qurt_rmutex_lock(&pram_mgr_qdi_mutex); \

#define PRAM_MGR_QDI_UNLOCK() \
    qurt_rmutex_unlock(&pram_mgr_qdi_mutex); \


int pram_mgr_qdi_invoke(int client_handle,
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
                        qurt_qdi_arg_t a9);
void pram_mgr_qdi_release(qurt_qdi_obj_t *obj);



int pram_mgr_qdi_open(int client_handle)
{
   pram_mgr_qdi_opener *clntobj;
   uint32 idx;
   
   PRAM_MGR_QDI_LOCK();

   /* Find a free slot */
   for(idx=0; idx<PRAM_MGR_MAX_CLIENTS && pram_mgr_qdi_clients[idx].opener_allocated == TRUE; idx++);

   if(idx == PRAM_MGR_MAX_CLIENTS) {
      PRAM_MGR_QDI_UNLOCK();
      return -1;
   }

   pram_mgr_qdi_clients_cntr++;
   
   clntobj = &pram_mgr_qdi_clients[idx];
   clntobj->qdiobj.invoke = pram_mgr_qdi_invoke;
   clntobj->qdiobj.refcnt = QDI_REFCNT_INIT;
   clntobj->qdiobj.release = pram_mgr_qdi_release;
   clntobj->opener_allocated = TRUE;
   clntobj->client_handle = client_handle;
   
   PRAM_MGR_QDI_UNLOCK();

   return qurt_qdi_handle_create_from_obj_t(client_handle, &clntobj->qdiobj);

}


void pram_mgr_qdi_release(qurt_qdi_obj_t *obj)
{
   pram_mgr_qdi_opener *clntobj = (pram_mgr_qdi_opener *)obj;
   
   PRAM_MGR_QDI_LOCK();

   pram_mgr_drv_deregister_partitions(clntobj->client_handle);

   /* Free the object related to this process */
   clntobj->qdiobj.invoke = NULL;
   clntobj->qdiobj.refcnt = 0;
   clntobj->qdiobj.release = NULL;
   clntobj->opener_allocated = FALSE;
   clntobj->client_handle = -1;
   pram_mgr_qdi_clients_cntr--;

   PRAM_MGR_QDI_UNLOCK();

   return;
}



int pram_mgr_qdi_invoke(int client_handle,
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
   //pram_mgr_qdi_opener *clntobj = (pram_mgr_qdi_opener *)pobj;
  
   switch(method)
   {
     case QDI_OPEN:
        return pram_mgr_qdi_open(client_handle);
     case PRAM_MGR_QDI_GET_PARTITION_INFO:
        return pram_mgr_drv_get_partition_info(client_handle, a1.ptr, a2.ptr, a3.ptr); 
     case PRAM_MGR_QDI_REGISTER_PARTITION: 
        return pram_mgr_drv_register_partition(client_handle, a1.ptr, a2.num);
     case QDI_ISLAND_CHECK_ALLOC:
        return 1;
     default:
        return qurt_qdi_method_default(client_handle, pobj, method,
                                    a1, a2, a3, a4, a5, a6, a7, a8, a9);
   }

}


qurt_qdi_obj_t pram_mgr_qdi_opener_obj;

void pram_mgr_clnt_init(void);

void pram_mgr_init(void)
{
    qurt_qdi_obj_t *p_opener = &pram_mgr_qdi_opener_obj;
    int i;

    /* Initialize the mutex for QDI code */
    qurt_rmutex_init(&pram_mgr_qdi_mutex);

    pram_mgr_drv_init();

    for(i=0; i<PRAM_MGR_MAX_CLIENTS; i++) {
       pram_mgr_qdi_clients[i].opener_allocated = FALSE;
       pram_mgr_qdi_clients[i].client_handle = -1;
    }

    p_opener->invoke = pram_mgr_qdi_invoke;
    p_opener->refcnt = QDI_REFCNT_INIT;
    p_opener->release = pram_mgr_qdi_release;
    
    qurt_qdi_register_devname(PRAM_MGR_DRIVER_NAME, p_opener);

    // NG: required for pram clients residing in rootPD - buses & GPI
    pram_mgr_clnt_init();
}


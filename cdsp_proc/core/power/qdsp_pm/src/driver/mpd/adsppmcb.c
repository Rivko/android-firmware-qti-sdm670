/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppmcb.c
@brief: Implementation of ADSPPM component that handles callbacks for clients
        in the user protection domain (user PD).

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/mpd/adsppmcb.c#1 $
*/

#include <stdlib.h>
#include "qurt_rmutex.h"
#include "adsppmcb.h"
#include "adsppm.h"
#include "mmpm.h"
#include "adsppm_qdi.h"


static qurt_mutex_t Adsppmcb_mutex_Srv;
static Adsppmcb_Elem_Type_Srv *pfnlist = NULL;


void Adsppmcb_Init_Server(void)
{
    qurt_rmutex_init(&Adsppmcb_mutex_Srv);
    pfnlist = NULL;
}


void Adsppmcb_register_callback_Srv( QDI_Adsppm_cbinfo_t *arg)
{
    Adsppmcb_Elem_Type_Srv *p;
    p = malloc(sizeof(*p));
    if(p)
    {
        memscpy((void*)&p->arg, sizeof(QDI_Adsppm_cbinfo_t), arg,
            sizeof(QDI_Adsppm_cbinfo_t));
        qurt_rmutex_lock(&Adsppmcb_mutex_Srv);
        p->next = pfnlist;
        pfnlist = p;
        qurt_rmutex_unlock(&Adsppmcb_mutex_Srv);
    }
}


void Adsppmcb_deRegister_callback_Srv(int clientId)
{
    Adsppmcb_Elem_Type_Srv *p;
    Adsppmcb_Elem_Type_Srv *preP = NULL;
    qurt_rmutex_lock(&Adsppmcb_mutex_Srv);
    for(p = pfnlist; p; p = p->next)
    {
        if(clientId == (p->arg.clientId))
        {
            if(preP)
            {
                preP->next = p->next;
            }
            else
            {
                // Delete first element in list
                // Note that first element could also be the last one
                // This operation works for both cases
                pfnlist = p->next;
            }
            free(p);
            break;
        }
        preP = p;
    }
    qurt_rmutex_unlock(&Adsppmcb_mutex_Srv);
}


uint32 Adsppmcb_notify_callbacks_Srv(MmpmCallbackParamType *pCallbackValue)
{
    Adsppmcb_Elem_Type_Srv *p;
    for(p = pfnlist; p; p = p->next)
    {
        if(pCallbackValue->clientId == p->arg.clientId)
        {
            QDI_Adsppm_do_callback(&p->arg, pCallbackValue, p->arg.pdNum);
            break;
        }
    }
    return 0;
}


void Adsppmcb_Deinit_Srv(void)
{
    Adsppmcb_Elem_Type_Srv *p;
    // Go through pfnlist, release mem
    if(pfnlist)
    {
        while(pfnlist->next)
        {
            for(p = pfnlist; p; p = p->next)
            {
                if(NULL != p->next)
                {
                    if(NULL == p->next->next)
                    {
                        free(p->next);
                        p->next = NULL;
                    }
                }
            }
        }
        free(pfnlist);
        pfnlist = NULL;
    }
    qurt_rmutex_destroy(&Adsppmcb_mutex_Srv);
}


#include "stdlib.h"
#include "qurt_elite.h"
#include "qurt.h"
#include "qurt_qdi_constants.h"
#include "qurt_qdi_driver.h"
#include "qurt_qdi.h"
#include "adspfatalerr_qdi.h"

QDI_Adspfatalerr_Ctx_t gAdsperr;

void QDI_Adspfatalerr_Release(qurt_qdi_obj_t *qdiobj)
{
    QDI_Adspfatalerr_Ctx_t *obj = (QDI_Adspfatalerr_Ctx_t *)qdiobj;
    if(obj != NULL)
        free(obj);
}


int QDI_Adspfatalerr_Invocation(int client_handle,
        qurt_qdi_obj_t *obj,
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
   
    int result = 0;
    char* fatal_error;
    QDI_Adspfatalerr_Ctx_t* Adsperrobj;
   
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPFATALERR server is up and running -- !");

    switch(method)
    {
    case QDI_OPEN:
       Adsperrobj = (QDI_Adspfatalerr_Ctx_t *)malloc(sizeof(QDI_Adspfatalerr_Ctx_t));
        if(Adsperrobj != NULL)
        {
           Adsperrobj->qdiobj.invoke = QDI_Adspfatalerr_Invocation;
           Adsperrobj->qdiobj.refcnt = QDI_REFCNT_INIT;
           Adsperrobj->qdiobj.release = QDI_Adspfatalerr_Release;
           result = qurt_qdi_new_handle_from_obj_t(client_handle,(qurt_qdi_obj_t *)Adsperrobj);
        }
        else
        {
           result = -1;
        }
        break;
    case ADSP_FATAL_ERROR:
        fatal_error = (char*)malloc(sizeof((a2.num)+1));
        result = qurt_qdi_copy_from_user(
                client_handle,
                (void *)fatal_error,
                (void *)a1.ptr,
                sizeof((a2.num)+1));

        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPFATALERR server is up and running -- method ADSP_FATAL_ERROR is hit !");

            if(0 > result)
            {
                result = -1;
            }
            else
            {
                MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fatal error crash cause: %s",fatal_error);
                #if defined(__qdsp6__) && !defined(SIM)   
                    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPFATALERR server is up and running -- method ADSP_FATAL_ERROR is hit - before exit  !");
                    //ERR_FATAL("FATAL_ERR: Force crash Q6 in ", 0, 0, 0);
                    exit(1);  
                #endif
            }
       break;
    default:
        result = qurt_qdi_method_default(
            client_handle, obj, method,
            a1, a2, a3, a4, a5, a6, a7, a8, a9);
        break;
    }
    return result;
}


const QDI_Adspfatalerr_Opener_t adspfatalerr_opener =
{
    {
        QDI_Adspfatalerr_Invocation,
        QDI_REFCNT_PERM,
        0
    } //0 as object is PERM, never released
};


/**
 * @fn QDI_adspfatalerr_init - registers driver with QDI framework
 * @brief This function is called by RCINIT.
 */
void QDI_adspfatalerr_init(void)
{
    qurt_qdi_devname_register(ADSP_FATALERR_DRV_NAME, (void *)&adspfatalerr_opener);
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSPFATALERR server is up and running -- after devname register !");
}


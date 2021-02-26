/**
 Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "vap_srm.h"
#ifdef SRM_HCP_EN
#include "vap_srm_hcp.h"
#endif//SRM_HCP_EN
#include "qurt_srm_driver.h"
#include "qurt_qdi.h"
#include "qurt.h"

/*===========================================================================
 Constants, macros
===========================================================================*/
#define ALLOC_UNIT ALLOC_UNIT_QDI
#define VAP_SRM_MAX_QDI_OBJS 16 //For HCP, and DMA clients

/*===========================================================================
 Type definitions
===========================================================================*/

/**
* @struct   t_StStatus
* @brief    Status and statistics.
*/
typedef struct StStatus{
    unsigned int u32ErrStatus;

    //Some stats
    unsigned int u32InvokeCalls;
    unsigned int u32HcpSidValBefore;
    unsigned int u32HcpSidValAfter;
}t_StStatus;

/**
* @struct   SRM_VAP
* @brief    VAP driver object.
*/
typedef struct SRM_VAP {
    qurt_qdi_obj_t qdiobj;
    int eOwner;
    struct SRM_VAP *pNext;
    t_StStatus stStat;
}t_StSRM_VAP;

/**
* @struct   stSrmVapCb
* @brief    Main Control Block.
*/
typedef struct StSrmVapCb {
    qurt_mutex_t lock;
    unsigned int u32NumClientsPerOwnerId[OWNER_ID_MAX];
    /* Ptr to last element in LIFO */
    t_StSRM_VAP *pStQdiObjLastIn;
    /* Array of QDI objects to be given to QDI clients
    (instead of allocating when the request is made)*/
    t_StSRM_VAP stVapDrvObjArr[VAP_SRM_MAX_QDI_OBJS];
}t_StSrmVapCb;

/*===========================================================================
 Local Variables
===========================================================================*/
static const unsigned int u32DefaultNumClientsPerOwnerId[OWNER_ID_MAX] =
        {VAP_SRM_MAX_QDI_OBJS,/* How many objects can a client with ID OWNER_ID_DEFAULT open */
        1,/* How many objects can a client with ID OWNER_ID_HCP open */
        1/* How many objects can a client with ID OWNER_ID_DMA open */
        };

/* Control Block */
static t_StSrmVapCb stSrmVapCb;

/*===========================================================================
 Local function prototypes
===========================================================================*/
static t_StSRM_VAP* vap_srmPopQdiObj(void);
static int vap_srmPushQdiObj(t_StSRM_VAP *pVapDrvObj);
static int vap_srmHandleInit(int client_handle);
static int vap_srmHandleOpen(int client_handle);
static int vap_srmHandleOwnerSet(struct SRM_VAP *pVapDrvObj,
                                int eNewOwner);
static int vap_srmHandleOwnerRelease(struct SRM_VAP *pVapDrvObj);
static void vap_srmRelease(qurt_qdi_obj_t *obj);
static int vap_srmInvocation(int client_handle, qurt_qdi_obj_t *obj, int method,
                         qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3,
                         qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                         qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9);

/*===========================================================================
 Function definitions
===========================================================================*/

/*!
* @brief  Pre-allocate driver objects, initialize SRM resources.
*
* @return: success=0.
*/
static int vap_srmHandleInit(int client_handle)
{
    int status = SRM_ERR_SUCCESS;
    struct SRM_VAP *pVapDrvObj = 0;

    if (client_handle != QDI_HANDLE_LOCAL_CLIENT)
        return -1;

    stSrmVapCb.pStQdiObjLastIn = 0;

    /* Pre-allcoate driver objects */
    for(int i=0; i<sizeof(stSrmVapCb.stVapDrvObjArr)/sizeof(struct SRM_VAP); i++)
    {
        pVapDrvObj = &stSrmVapCb.stVapDrvObjArr[i];

        pVapDrvObj->qdiobj.invoke = vap_srmInvocation;
        pVapDrvObj->qdiobj.refcnt = QDI_REFCNT_INIT;
        pVapDrvObj->qdiobj.release = vap_srmRelease;

        pVapDrvObj->eOwner = OWNER_ID_DEFAULT;

        pVapDrvObj->stStat.u32ErrStatus = 0;
        pVapDrvObj->stStat.u32InvokeCalls = 0;

        if(vap_srmPushQdiObj(pVapDrvObj))
        {
            status = SRM_ERR_FAIL;
            goto exit_point;
        }
    }

    qurt_mutex_init(&stSrmVapCb.lock);

    memcpy(stSrmVapCb.u32NumClientsPerOwnerId,
            u32DefaultNumClientsPerOwnerId,
            sizeof(stSrmVapCb.u32NumClientsPerOwnerId));
#ifdef SRM_HCP_EN
    /* Initialize HCP Resources */
    status = vap_srmHcpResourcesInit(client_handle);
    if(status)
    {
        qurt_mutex_destroy(&stSrmVapCb.lock);
        goto exit_point;
    }
#endif// SRM_HCP_EN
exit_point:

   return status;
}

/*!
* @brief  Pop drvr object from LIFO.
*
* @return: Ptr to driver object.
*/
static t_StSRM_VAP* vap_srmPopQdiObj(void)
{
    int status = SRM_ERR_SUCCESS;
    t_StSRM_VAP *pRet = 0;

    pRet = stSrmVapCb.pStQdiObjLastIn;
    if(pRet)
        stSrmVapCb.pStQdiObjLastIn = pRet->pNext;

    return pRet;
}

/*!
* @brief  Push drvr object to LIFO.
*
* @return: success=0.
*/
static int vap_srmPushQdiObj(t_StSRM_VAP *pVapDrvObj)
{
    int status = SRM_ERR_SUCCESS;

    if(!pVapDrvObj)
    {
        return SRM_ERR_NULLPTR;
    }

    pVapDrvObj->pNext = stSrmVapCb.pStQdiObjLastIn;
    stSrmVapCb.pStQdiObjLastIn = pVapDrvObj;

    return status;
}

/*!
* @brief  Create handle to driver object.
*
* @return: success=0.
*/
static int vap_srmHandleOpen(int client_handle)
{
    int h = -1;

    qurt_mutex_lock(&stSrmVapCb.lock);

    struct SRM_VAP *pVapDrvObj = vap_srmPopQdiObj();
    if(!pVapDrvObj)
    {
        h = -1;
        goto exit_point;
    }

    if(pVapDrvObj)
    {
        h = qurt_qdi_new_handle_from_obj_t(client_handle, &pVapDrvObj->qdiobj);
        if (h < 0)
        {
            vap_srmPushQdiObj(pVapDrvObj);
            goto exit_point;
        }
    }

exit_point:

    qurt_mutex_unlock(&stSrmVapCb.lock);

    return h;
}

/*!
* @brief Set object ownership.
*
* @description An object can only have its ownership changed once from OWNER_ID_DEFAULT.
*
* @return: success=0.
*/
static int vap_srmHandleOwnerSet(struct SRM_VAP *pVapDrvObj, int eNewOwner)
{
    int status = SRM_ERR_SUCCESS;

    if(!pVapDrvObj)
    {
        return SRM_ERR_NULLPTR;
    }

    qurt_mutex_lock(&stSrmVapCb.lock);

    if((OWNER_ID_DEFAULT == pVapDrvObj->eOwner) &&
       (eNewOwner < OWNER_ID_MAX) &&
       (stSrmVapCb.u32NumClientsPerOwnerId[eNewOwner]))
    {
        --stSrmVapCb.u32NumClientsPerOwnerId[eNewOwner];
        pVapDrvObj->eOwner = eNewOwner;
    }
    else
    {
        status = SRM_ERR_FAIL;
    }

    qurt_mutex_unlock(&stSrmVapCb.lock);

    return status;
}

/*!
* @brief Release object ownership.
*
* @description Assuming that the object is not shared, only the present owner of the object can call this.
*
* @return: success=0.
*/
static int vap_srmHandleOwnerRelease(struct SRM_VAP *pVapDrvObj)
{
    int status = SRM_ERR_SUCCESS;

    if(!pVapDrvObj)
    {
        return SRM_ERR_NULLPTR;
    }

    qurt_mutex_lock(&stSrmVapCb.lock);

    if(stSrmVapCb.u32NumClientsPerOwnerId[pVapDrvObj->eOwner] < u32DefaultNumClientsPerOwnerId[pVapDrvObj->eOwner])
    {
        pVapDrvObj->eOwner = OWNER_ID_DEFAULT;
        ++stSrmVapCb.u32NumClientsPerOwnerId[pVapDrvObj->eOwner];
    }
    else
    {
        status = SRM_ERR_FAIL;
    }

    qurt_mutex_unlock(&stSrmVapCb.lock);

    return status;
}

/*!
* @brief Release object handle.
*
* @return: Nothing.
*/
static void vap_srmRelease(qurt_qdi_obj_t *obj)
{
    struct SRM_VAP *pVapDrvObj = (struct SRM_VAP *)obj;

    if(!pVapDrvObj)
    {
        return;
    }

    vap_srmHandleOwnerRelease(pVapDrvObj);

    qurt_mutex_lock(&stSrmVapCb.lock);

    vap_srmPushQdiObj(pVapDrvObj);

    qurt_mutex_unlock(&stSrmVapCb.lock);
}

/*!
* @brief Entry point
*
* @return: success=0;
*/
static int vap_srmInvocation(int client_handle, qurt_qdi_obj_t *obj, int method,
                         qurt_qdi_arg_t a1, qurt_qdi_arg_t a2, qurt_qdi_arg_t a3,
                         qurt_qdi_arg_t a4, qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                         qurt_qdi_arg_t a7, qurt_qdi_arg_t a8, qurt_qdi_arg_t a9)
{
    int status = SRM_ERR_SUCCESS;
    unsigned int u32Pid = -1;
    unsigned int nInputFrameSid = -1;
    unsigned int nOutputFrameSid = -1;
    unsigned int nPixBufSid = -1;
    unsigned int nNonPixBufSid = -1;
    unsigned int nRegVal = 0;

    struct SRM_VAP *pVapDrvObj = 0;

    switch (method) {
    case QDI_OPEN:
        return vap_srmHandleOpen(client_handle);
    case QDI_SRM_INIT:/*called on the opener object during system boot*/
        return vap_srmHandleInit(client_handle);
#if 0
    case QDI_SRM_ENTER_SLEEP:/*called before entering power collapse*/
        break;
    case QDI_SRM_EXIT_SLEEP:/*called after exiting power collapse*/
        break;
    case QDI_SRM_ENTER_ISLAND:/*called before entering micro-image*/
        break;
    case QDI_SRM_EXIT_ISLAND:/*called after exiting micro-image*/
        break;
    case QDI_SRM_PROCESS_START:/*called when a new process is created*/
        break;
    case QDI_SRM_PROCESS_STOP:/*called when a process exits or is killed*/
        break;
    case QDI_SRM_THREAD_STOP:/*called when a thread stops -- but only if SRM has set the appropriate bit in the TCB*/
        break;
    case QDI_SRM_EXIT:/*called when the system is going down -- prepare for a crash dump -- might have to clear certain memory if that memory contains sensitive or protected content*/
        break;
#endif
    case VAP_SRM_OWNER_SET:
        pVapDrvObj = (struct SRM_VAP *)obj;
        return vap_srmHandleOwnerSet(pVapDrvObj,
                                    a1.num);
        break;
#ifdef SRM_HCP_EN
    case VAP_SRM_HCP_SID_REG_GET:
        pVapDrvObj = (struct SRM_VAP *)obj;
        if(!pVapDrvObj) return SRM_ERR_NO_RESOURCE;

        pVapDrvObj->stStat.u32InvokeCalls++;
        if(pVapDrvObj->stStat.u32ErrStatus)
            return SRM_ERR_INVALID_STATE;
#ifdef EN_STATS
        qurt_qdi_copy_from_user(client_handle,
                &u32Pid, a1.ptr, sizeof(u32Pid));

        /* Check ownership */
        if(OWNER_ID_HCP != pVapDrvObj->eOwner)
        {
            pVapDrvObj->stStat.u32ErrStatus |= (1<<ERR_FLAGSHIFT_HCP_UNAUTHORIZED_ACCESS);//TODO: clear later
            return SRM_ERR_FAIL;
        }
        status = vap_srmHandleHcpSidRegGet(
                u32Pid,
                &nRegVal);

        qurt_qdi_copy_to_user(client_handle,
                a2.ptr, &nRegVal, sizeof(nRegVal));
#endif//EN_STATS
        break;
    case VAP_SRM_HCP_SID_REG_SET:
        pVapDrvObj = (struct SRM_VAP *)obj;
        if(!pVapDrvObj) return SRM_ERR_NO_RESOURCE;

        pVapDrvObj->stStat.u32InvokeCalls++;
        if(pVapDrvObj->stStat.u32ErrStatus)
            return SRM_ERR_INVALID_STATE;

        qurt_mutex_lock(&stSrmVapCb.lock);

        /* Check ownership */
        if(OWNER_ID_HCP != pVapDrvObj->eOwner)
        {
            pVapDrvObj->stStat.u32ErrStatus |= (1<<ERR_FLAGSHIFT_HCP_UNAUTHORIZED_ACCESS);//TODO: clear later
            qurt_mutex_unlock(&stSrmVapCb.lock);
            return SRM_ERR_FAIL;
        }
        status = vap_srmHandleHcpSidRegSet(
                a1.num,
                a2.num,
                &pVapDrvObj->stStat.u32ErrStatus,
                &pVapDrvObj->stStat.u32HcpSidValBefore,
                &pVapDrvObj->stStat.u32HcpSidValAfter);

        qurt_mutex_unlock(&stSrmVapCb.lock);

        break;
    case VAP_SRM_GET_HCP_SID_INFO:
#ifdef EN_STATS
        pVapDrvObj = (struct SRM_VAP *)obj;
        if(!pVapDrvObj) return SRM_ERR_NO_RESOURCE;

        pVapDrvObj->stStat.u32InvokeCalls++;
        if(pVapDrvObj->stStat.u32ErrStatus)
            return SRM_ERR_INVALID_STATE;

        t_StMapNode *pHcpSidRegMap = vap_srm_hcpGetMapEntries();

        qurt_qdi_copy_to_user(client_handle,
                a1.ptr, &pVapDrvObj->stStat.u32ErrStatus, sizeof(pVapDrvObj->stStat.u32ErrStatus));
        qurt_qdi_copy_to_user(client_handle,
                a2.ptr, &pHcpSidRegMap->pageno_virt, sizeof(pHcpSidRegMap->pageno_virt));
        qurt_qdi_copy_to_user(client_handle,
                a3.ptr, &pHcpSidRegMap->pageno_phys, sizeof(pHcpSidRegMap->pageno_phys));
        qurt_qdi_copy_to_user(client_handle,
                a4.ptr, &pVapDrvObj->stStat.u32InvokeCalls, sizeof(pVapDrvObj->stStat.u32InvokeCalls));
        qurt_qdi_copy_to_user(client_handle,
                a5.ptr, &pHcpSidRegMap->start_vaddr_h, sizeof(pHcpSidRegMap->start_vaddr_h));
        qurt_qdi_copy_to_user(client_handle,
                a6.ptr, &pHcpSidRegMap->start_vaddr_l, sizeof(pHcpSidRegMap->start_vaddr_l));
        qurt_qdi_copy_to_user(client_handle,
                a7.ptr, &pVapDrvObj->stStat.u32HcpSidValBefore, sizeof(pVapDrvObj->stStat.u32HcpSidValBefore));
        qurt_qdi_copy_to_user(client_handle,
                a8.ptr, &pVapDrvObj->stStat.u32HcpSidValAfter, sizeof(pVapDrvObj->stStat.u32HcpSidValAfter));
#endif //EN_STATS
        break;
#endif// SRM_HCP_EN
    default:
        return qurt_qdi_method_default(client_handle, obj, method, a1, a2, a3,
                a4, a5, a6, a7, a8, a9);
    }

    return status;
}

int vap_srmInit(void)
{
    int status = SRM_ERR_SUCCESS;

    return status;
}

/**** Driver registration ******/

static struct {
    qurt_qdi_obj_t obj;
} opener = {
    .obj = {
            .invoke = vap_srmInvocation,
            .refcnt = QDI_REFCNT_PERM
    }
};

QURT_SRM_DECLARE_DRIVER(VAP_SRM_driver_id, VAP_SRM_DRIVER_NAME, &opener.obj);

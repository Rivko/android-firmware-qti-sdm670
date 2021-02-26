/**
Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

  @file   audio_ftm_ext_loopback.c
  @brief  AUDIO FTM Tone play driver
==================================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

#include "audio_ftm_ext_loopback.h"
#include "audio_ftm_util_fifo.h"
#include "audio_ftm_hw_drv.h"

/*==================================================================================================
                                     LOCAL CONSTANTS
==================================================================================================*/

#define MAX_AUD_FTM_CLIENT   1
#define EXT_LOOP_TYPE 2

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

struct Aud_FTM_ClientCtxt_S;

typedef struct Aud_FTM_DrvCtxt_S{
    Aud_FTM_DevCtxt_T           *pDevCtxt;
    struct Aud_FTM_ClientCtxt_S *apClientCtxt[MAX_AUD_FTM_CLIENT];
    DALBOOL                     bOpened;
    DAL_ATOMIC                  dwOpenCount;
    DALSYSSyncHandle            hClientSync;

    AUD_FTM_EXT_LP_PARAM_T      client_param;       /* parameters passed from client */
} Aud_FTM_DrvCtxt_T, *pAud_FTM_DrvCtxt_T;

/* Structure which describes the client context */
typedef struct Aud_FTM_ClientCtxt_S{
    Aud_FTM_DrvCtxt_T *pDrvCtxt;
    uint32 dwAccessCode;/*< access mode for the opened device */
    uint32 dwShareMode; /*< share mode for the opened device */
    uint32 dwClientID;  /* client sequence number of LPA driver */
} Aud_FTM_ClientCtxt_T, *pAud_FTM_ClientCtxt_T;

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/



/*==================================================================================================
                                      LOCAL VARIABLES
==================================================================================================*/

static DALBOOL    g_bDriverInitialized=FALSE;   //  whether it has been inited or not

/*==================================================================================================
                                     GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                 LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                     LOCAL FUNCTIONS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
  @brief FTM Driver Attach API

  This API can be used to attach the FTM driver.

  @param  pParam:  Input - pointer to parameter
          pDriverHdl   : Output - a driver handle will be returned if attach success

  @return return code, AUDIO_FTM_SUCCESS on successful completion, error code otherwise
==================================================================================================*/

AUDIO_FTM_STS_T
audio_ftm_ext_lp_attach
(
    void *pParam,                                 /* Input: parameters  */
    AUDIO_FTM_DRIVER_HANDLE_T *pDriverHdl        /* Output: driver handle */
)
{

    /************* Context creation ***********/

    Aud_FTM_DevCtxt_T *pDeviceCtxt;
    Aud_FTM_DrvCtxt_T *pDriverCtxt;
    AUD_FTM_EXT_LP_PARAM_T     *ext_lp_param;

    Aud_FTM_HW_INIT_PARAM_T hw_param;
    AUDIO_FTM_STS_T sts;

    if (g_bDriverInitialized)
    {
        return  AUDIO_FTM_ERROR;    //  already inited,
    }

    /****************** create context object ******************/

    if(DAL_SUCCESS != DALSYS_Malloc(sizeof(Aud_FTM_DevCtxt_T), (void **)&pDeviceCtxt))
    {
        return AUDIO_FTM_ERR_MEM_ALLOC_FAIL;
    }

    if (pDeviceCtxt == NULL)  return AUDIO_FTM_ERR_MEM_ALLOC_FAIL;
    DALSYS_memset(pDeviceCtxt,0,sizeof(Aud_FTM_DevCtxt_T));

    if(DAL_SUCCESS != DALSYS_Malloc(sizeof(Aud_FTM_DrvCtxt_T), (void **)&pDriverCtxt))
    {
        DALSYS_Free(pDeviceCtxt);
        return AUDIO_FTM_ERR_MEM_ALLOC_FAIL;
    }

    if (pDriverCtxt == NULL)
    {
        DALSYS_Free(pDeviceCtxt);
        return AUDIO_FTM_ERR_MEM_ALLOC_FAIL;
    }

    DALSYS_memset(pDriverCtxt,0,sizeof(Aud_FTM_DrvCtxt_T));
    DALSYS_atomic_init(&pDriverCtxt->dwOpenCount);
    pDriverCtxt->pDevCtxt = pDeviceCtxt;
    pDeviceCtxt->pDrvCtxt = pDriverCtxt;
    ext_lp_param = (AUD_FTM_EXT_LP_PARAM_T *)pParam;
    printf("\n%s,freq=%d\n", __func__, ext_lp_param->freq);
    pDeviceCtxt->capturedevice = ext_lp_param->capturedevice;
    pDeviceCtxt->playbackdevice = ext_lp_param->playbackdevice;

    /************* Create Critical Section ***********/
    if(DAL_SUCCESS != DALSYS_SyncCreate(
        &(pDriverCtxt->hClientSync)))
    {
        return AUDIO_FTM_ERROR;
    }

    /*************  hardware initialization ***********/

    DALSYS_memcpy((uint8*)&(pDriverCtxt->client_param),
                  (uint8*)pParam, sizeof(AUD_FTM_EXT_LP_PARAM_T));

    hw_param.inpath = pDriverCtxt->client_param.path.inpath;
    hw_param.outpath = pDriverCtxt->client_param.path.outpath;
    hw_param.rate = AUDIO_FTM_PCM_RATE_8K;
    hw_param.width = AUDIO_FTM_BIT_WIDTH_16;
    hw_param.channel = AUDIO_FTM_CHN_1;
    hw_param.gain = pDriverCtxt->client_param.gain;
    hw_param.bLoopbackCase = TRUE;
    hw_param.m_loopback_type = AUDIO_FTM_EXT_LOOPBACK;

   sts = aud_ftm_hw_init(pDeviceCtxt, &hw_param);
   if(sts != AUDIO_FTM_SUCCESS)
        return AUDIO_FTM_ERROR;

    /***** end of Init *****/

    pDriverCtxt->bOpened=FALSE;
    g_bDriverInitialized=TRUE;
    *pDriverHdl = (AUDIO_FTM_DRIVER_HANDLE_T)pDriverCtxt;

    return AUDIO_FTM_SUCCESS;
}

/*==================================================================================================
  @brief FTM driver open

  This API can be used to open the FTM driver.

  @param DriverHdl:   Input -   handle created in Audio_FTM_Attach
         AccessMode:  Input -   Read/Write/RD_WR
         ShareMode:   Input -   Exclusive or Share
         pClientHdl:  Output -  point to the returned Open handle

  @return return code, AUDIO_FTM_SUCCESS on successful completion, error code otherwise
==================================================================================================*/
AUDIO_FTM_STS_T
audio_ftm_ext_lp_open
(
    AUDIO_FTM_DRIVER_HANDLE_T pDriverContext ,  /* Input: driver handle */
    AUD_FTM_ACCESS_MODE_T AccessMode,           /* Input: Access mode */
    AUD_FTM_SHARE_MODE_T  ShareMode,            /* Input: Share mode */
    AUDIO_FTM_CLIENT_HANDLE_T * pClientContext  /* Output: client handle */
)
{
    Aud_FTM_DrvCtxt_T * pDrvCxt = (Aud_FTM_DrvCtxt_T *)pDriverContext;
    Aud_FTM_ClientCtxt_T * pOpenContext = NULL;
    AUDIO_FTM_STS_T res;

    if (pDrvCxt == NULL)
    {
        return AUDIO_FTM_ERR_INVALID_PARAM;
    }

    DALSYS_SyncEnter(pDrvCxt->hClientSync);

    if((ShareMode == AUD_FTM_SHAREMODE_EXCLU ) && (DALSYS_atomic_read(&pDrvCxt->dwOpenCount) != 0))
        /* ShareMode = 0: exclusive mode; =FILE_SHARE_READ: read only; =FILE_SHARE_WRITE: write only */
        /* LPA driver only works at Exclusive Mode */
    {
      DALSYS_SyncLeave(pDrvCxt->hClientSync);
        return AUDIO_FTM_ERROR;
    }

    if(DAL_SUCCESS != DALSYS_Malloc(sizeof(Aud_FTM_ClientCtxt_T), (void **)&pOpenContext))
    {
        DALSYS_SyncLeave(pDrvCxt->hClientSync);
        return AUDIO_FTM_ERR_MEM_ALLOC_FAIL;
    }

    if (pOpenContext == NULL)
    {
      DALSYS_SyncLeave(pDrvCxt->hClientSync);
      return AUDIO_FTM_ERR_MEM_ALLOC_FAIL;
    }
    DALSYS_memset(pOpenContext,0,sizeof(Aud_FTM_ClientCtxt_T));

    // Store device settings for future use
    pOpenContext->pDrvCtxt = pDrvCxt;
    pOpenContext->dwAccessCode = AccessMode;
    pOpenContext->dwShareMode = ShareMode;

    pOpenContext->dwClientID = DALSYS_atomic_read(&pDrvCxt->dwOpenCount);

    pDrvCxt->apClientCtxt[pOpenContext->dwClientID] = pOpenContext;

    /***** open HW ****/
    res = audio_ftm_hw_open(pDrvCxt->pDevCtxt);
    if(res != AUDIO_FTM_SUCCESS)
    {
        DALSYS_SyncLeave(pDrvCxt->hClientSync);
        return AUDIO_FTM_ERROR;
    }

    // Increase opened device counter
    AddRef(&pDrvCxt->dwOpenCount);
    pDrvCxt->bOpened = TRUE;

    DALSYS_SyncLeave(pDrvCxt->hClientSync);

    *pClientContext = (AUDIO_FTM_CLIENT_HANDLE_T)pOpenContext;
    return AUDIO_FTM_SUCCESS;
}

/*==================================================================================================
  @brief FTM driver Close

  This API can be used to Close the FTM driver.

  @param OpenHdl:   Input -   handle created in Audio_FTM_Open

  @return return code, AUDIO_FTM_SUCCESS on successful completion, error code otherwise
==================================================================================================*/
AUDIO_FTM_STS_T
audio_ftm_ext_lp_close
(
    AUDIO_FTM_CLIENT_HANDLE_T  pOpenContext       /* Input: client handle */
)
{
    Aud_FTM_ClientCtxt_T   *pExplicitOpenContext;
    Aud_FTM_DrvCtxt_T      *pDrvCtx;
    uint32                  nClient_id;
    int32                   i;
    AUDIO_FTM_STS_T res;

    if (pOpenContext == NULL)
    {
        return AUDIO_FTM_ERR_INVALID_PARAM;
    }

    pExplicitOpenContext = (Aud_FTM_ClientCtxt_T *)pOpenContext;

    pDrvCtx = pExplicitOpenContext->pDrvCtxt;

    res = audio_ftm_hw_close(pDrvCtx->pDevCtxt);
    if(res != AUDIO_FTM_SUCCESS)
        return AUDIO_FTM_ERROR;

    nClient_id = pExplicitOpenContext->dwClientID;

    DALSYS_Free(pExplicitOpenContext);
    pExplicitOpenContext=NULL;

    DALSYS_SyncEnter(pDrvCtx->hClientSync);

    for(i=nClient_id; i<DALSYS_atomic_read(&pDrvCtx->dwOpenCount)-1; i++)
    {
        pDrvCtx->apClientCtxt[i] = pDrvCtx->apClientCtxt[i+1];
        pDrvCtx->apClientCtxt[i]->dwClientID--;
    }
    pDrvCtx->apClientCtxt[i] = NULL;

    // Decrease opened device counter
    Release(&pDrvCtx->dwOpenCount);
    if(DALSYS_atomic_read(&pDrvCtx->dwOpenCount) == 0)
        pDrvCtx->bOpened = FALSE;

    DALSYS_SyncLeave(pDrvCtx->hClientSync);

    return AUDIO_FTM_SUCCESS;

}

/*==================================================================================================
  @brief FTM driver Read

  This API can be used to Read data from FTM driver

  @param OpenHdl:     Input -   handle created in Audio_FTM_Open
         pBuf         Input -   point to buffer which stores data read from driver
         nBufSize     Input -   Read buffer size
         pCount:      Output -  return the actual read bytes

  @return return code, AUDIO_FTM_SUCCESS on successful completion, error code otherwise
==================================================================================================*/
AUDIO_FTM_STS_T
audio_ftm_ext_lp_read()
{
    return AUDIO_FTM_SUCCESS;
}

/*==================================================================================================
  @brief FTM driver Write

  This API can be used to Write data to FTM driver

  @param OpenHdl:     Input -   handle created in Audio_FTM_Open
         pBuf         Input -   point to data buffer which will be written to driver
         nCount:      Input -   Bytes count

  @return return code, AUDIO_FTM_SUCCESS on successful completion, error code otherwise
==================================================================================================*/
AUDIO_FTM_STS_T
audio_ftm_ext_lp_write()
{
    return AUDIO_FTM_SUCCESS;
}

/*==================================================================================================
  @brief FTM driver IOControl

  This API can be used to set/read parameters or commands to/from FTM driver

  @param OpenHdl:     Input -   handle created in Audio_FTM_Open
         dwIOCode     Input -   IOControl command
         pBufIn       Input -   buffer pointer for input parameters
         dwLenIn      Input -   Input parameter length
         pBufOut      Input -   buffer pointer for outputs
         dwLenOut     Input -   expected output length
         pActualOutLen Output - actual output length

   @return return code, AUDIO_FTM_SUCCESS on successful completion, error code otherwise
==================================================================================================*/
AUDIO_FTM_STS_T
audio_ftm_ext_lp_iocontrol
(
    AUDIO_FTM_CLIENT_HANDLE_T pOpenContext,   /* Input: client handle */
    uint32 dwCode,                            /* Input: IOControl command */
    uint8 * pBufIn                            /* Input: buffer pointer for input parameters */
)
{
    Aud_FTM_DrvCtxt_T * pDrvCtxt;
    Aud_FTM_DevCtxt_T * pDevCtxt;
    AUDIO_FTM_STS_T ret;

    /* The device has to be open */
    if (pOpenContext == NULL)
        return AUDIO_FTM_ERR_INVALID_PARAM;

    pDrvCtxt = ((Aud_FTM_ClientCtxt_T *)pOpenContext)->pDrvCtxt;
    pDevCtxt = pDrvCtxt->pDevCtxt;

    if( NULL == pDevCtxt)
        return AUDIO_FTM_ERR_INVALID_PARAM;

    ret = AUDIO_FTM_SUCCESS;

    switch(dwCode)
    {

    case IOCTL_AUDIO_FTM_CHG_VOL:
    {
        uint16 vol;
        if (pBufIn == NULL)
        {
            ret = AUDIO_FTM_ERR_INVALID_PARAM;
            break;
        }
        vol = *(uint16 *)pBufIn;
        audio_ftm_hw_iocontrol(pDevCtxt, IOCTL_AUDIO_FTM_CHG_VOL,(uint8 *) &vol, NULL);
    }
    break;

    case IOCTL_AUDIO_FTM_START:
    {
        AFEDevAudIfDirType dir;
        dir=AUDIO_IF_SOURCE;
        audio_ftm_hw_iocontrol(pDevCtxt, IOCTL_AUDIO_FTM_START, (uint8 *)&dir, NULL);
        audio_ftm_ext_loopback_en(1, EXT_LOOP_TYPE,
                                pDevCtxt->pDrvCtxt->client_param.freq,
                                pDevCtxt->pDrvCtxt->client_param.filename,
                                pDevCtxt->pDrvCtxt->client_param.tone_analysis,
                                pDevCtxt->pDrvCtxt->client_param.freq_tolerance);
    }
    break;

    case IOCTL_AUDIO_FTM_STOP:
    {
        /* pcm_close is called at each thread respectively. */
        audio_ftm_ext_loopback_en(0, EXT_LOOP_TYPE,
                                pDevCtxt->pDrvCtxt->client_param.freq,
                                pDevCtxt->pDrvCtxt->client_param.filename,
                                pDevCtxt->pDrvCtxt->client_param.tone_analysis,
                                pDevCtxt->pDrvCtxt->client_param.freq_tolerance);
    }
    break;

    default:
        ret = AUDIO_FTM_ERR_INVALID_PARAM;
    }

    return ret;
}

/*==================================================================================================
  @brief FTM driver Detach

  This API can be used to Detach FTM driver, it will release all resournces

  @param DriverHdl:     Input -   handle created in Audio_FTM_Attach

  @return return code, AUDIO_FTM_SUCCESS on successful completion, error code otherwise
==================================================================================================*/
AUDIO_FTM_STS_T
audio_ftm_ext_lp_detach
(
    AUDIO_FTM_DRIVER_HANDLE_T pDriverContext
)
{

    Aud_FTM_DevCtxt_T* pDevContext;
    Aud_FTM_DrvCtxt_T* pDrvContext;

    pDrvContext = (Aud_FTM_DrvCtxt_T *)pDriverContext;

    if(pDrvContext == NULL)
    {
        return AUDIO_FTM_ERROR;
    }
    pDevContext = pDrvContext->pDevCtxt;
    if(pDevContext == NULL)
    {
        return AUDIO_FTM_ERROR;
    }

    aud_ftm_hw_deinit(pDevContext);

    DALSYS_DestroySyncObject(pDrvContext->hClientSync);

    DALSYS_Free(pDevContext); pDevContext = NULL;
    DALSYS_Free(pDrvContext); pDrvContext = NULL;

    g_bDriverInitialized = FALSE;

    return AUDIO_FTM_SUCCESS;

}

/*================================================================================================*/


#ifdef __cplusplus
}
#endif

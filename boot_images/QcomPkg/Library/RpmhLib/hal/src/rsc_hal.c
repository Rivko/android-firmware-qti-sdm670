/*===========================================================================
                                RSC_HAL.c
DESCRIPTION: RSC Hal functions definition

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

#include "rsc_hal.h"
#include "rsc_hal_image_config.h"
#include "rsc_hal_internal.h"
#include "rpmh_os.h"


//ULogHandle rpm_log;



struct RscHalCtxt GlobalRscHalCtxt;

int32 isContextInitialized (RscHalCtxt *ctxt,RSCSW_DRV_MAPPING drvMapId)
{
  int32 status = HAL_STATUS_SUCCESS;
  
  if ((status = IS_DRV_VALID (drvMapId)) < HAL_STATUS_SUCCESS)
  {
    RPMH_LOG_1 ("\tInvalid DrvMapId, %d",drvMapId);
    return status;
  }

  if(ctxt != NULL && 
      ctxt->HalInitialized == TRUE && 
      ctxt->DrvConfigArray[drvMapId].DrvInitialized == TRUE)
  {
    return HAL_STATUS_SUCCESS;
  }
  else
  {
    return HAL_INVALID_CTXT_NOT_INITIALIZED;
  }
}

/*
* Helper function to get RSC Context Handle
* Input: NULL
* Return: Pointer to global RSC Hal Contxt
*/
RscHalCtxt * rscHalGetContext ()
{
  return &GlobalRscHalCtxt;
}

/*
* Helper function to get number of TCSs for a DRV
* Input: Drv Base Address
*        Drv Id
         Number Of TCS
* Return: HAL_STATUS_SUCCESS if success or error code
*/

int32 rscHalGetNumTcs (uint32 baseAddress, uint8 drvId, uint32 *numTcs)
{
  int32 status = HAL_STATUS_SUCCESS;
  
  if (baseAddress == 0 || numTcs == NULL)
  {
    RPMH_LOG_1 ("\tInvalid BaseAdresss/numtcs for drv, %d, passed to rscHalGetNUm",drvId);
    status = HAL_INVALID_INPUT_PTR; 
    return status;
  }

  switch (drvId)
  {
    case DRV_ID_0:
      *numTcs =   RSC_HAL_HWIO_INId_READ_MSK(PLATFORM,
                                             _RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd,
                                             baseAddress,
                                             drvId, 
                                             _NUM_TCS_DRV0_BMSK);
      
      *numTcs >>= RSC_HAL_BIT_SHFT (PLATFORM,
                                    _RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd,                               
                                    _NUM_TCS_DRV0_SHFT );
      
      break;
#ifdef DRV_1_VALID	
    case DRV_ID_1:
      *numTcs =   RSC_HAL_HWIO_INId_READ_MSK (PLATFORM,
                                              _RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd,
                                              baseAddress,
                                              drvId, 
                                              _NUM_TCS_DRV1_BMSK);
        
      *numTcs >>= RSC_HAL_BIT_SHFT (PLATFORM,
                                    _RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd,                               
                                    _NUM_TCS_DRV1_SHFT);
      break;
#endif	
#ifdef DRV_2_VALID	
    case DRV_ID_2:
      *numTcs =   RSC_HAL_HWIO_INId_READ_MSK (PLATFORM,
                                              _RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd,
                                              baseAddress,
                                              drvId, 
                                              _NUM_TCS_DRV2_BMSK);
        
      *numTcs >>= RSC_HAL_BIT_SHFT (PLATFORM,
                                    _RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd,                               
                                    _NUM_TCS_DRV2_SHFT);

      break;
#endif
    default:
      RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to rscHalGetNUm",drvId);  
      status = HAL_INVALID_DRV_ID; 
      break;
  }
  
  return status;
}

/*
* Helper function to get number of commands per TCSs for a DRV
* Input: Drv Base Address
*        Drv Id
*        Num Cmds Per TCS
* Return: HAL_STATUS_SUCCESS if success or error code
*/

int32 rscHalGetNumCmdsPerTcs (uint32 baseAddress, uint8 drvId, uint32 *numCmdsPerTcs) 
{
  int32 status = HAL_STATUS_SUCCESS;
  
  if (baseAddress == 0 || numCmdsPerTcs == NULL)
  {
    RPMH_LOG_1 ("\tInvalid BaseAdresss/numcmdspertcs for drv, %d, passed to rscHalGetNumCmdsPerTcs",drvId);
    status = HAL_INVALID_INPUT_PTR; 
    return status;
  }
  
  switch (drvId)
  {
    case DRV_ID_0:
    case DRV_ID_1:
    case DRV_ID_2:
      *numCmdsPerTcs  =   RSC_HAL_HWIO_INId_READ_MSK (PLATFORM,
                                                      _RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd,
                                                      baseAddress,
                                                      drvId, 
                                                      _NUM_CMDS_PER_TCS_BMSK);
      *numCmdsPerTcs >>= RSC_HAL_BIT_SHFT(PLATFORM,
                                          _RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd,                               
                                          _NUM_CMDS_PER_TCS_SHFT );
      break;

    default:
      RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to rscHalGetNumCmdsPerTcs",drvId);  
      status = HAL_INVALID_DRV_ID;
      break;
  }  

  return status;
}

/*
* Helper function to fill-up DRV-specific information 
* Input: POinter to Global Hal Struct
*        Pointer to Mapping enty in config table for subsystem
* Return: HAL_STATUS_SUCCESS if success or error code
*/

int32 rscHalGetDrvInfo (RscHalCtxt *pCtxt)
{
  DrvBaseAddressMappingEntry *pMappingEntry=NULL;
  uint32 numTcs = 0;
  uint32 numCmdsPerTcs = 0;  
  uint32 cnt = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
  if (pCtxt == NULL || pCtxt->NumDrvs == 0)
  {
    RPMH_LOG_0 ("\tInvalid Context Ptr/Mapping Entry in rscHalGetDrvInfo ");    
    return HAL_INVALID_INPUT_PTR;
  }

  for (cnt = 0; cnt < pCtxt->NumDrvs; cnt++)
  {
    pMappingEntry = &DrvBaseAddressMappingTable_subsystem.Prows[cnt];
    pCtxt->DrvConfigArray[pMappingEntry->RscSwDrvMapId].RscSwDrvMapId =  pMappingEntry->RscSwDrvMapId;
    pCtxt->DrvConfigArray[pMappingEntry->RscSwDrvMapId].BaseAddress   =  pMappingEntry->BaseAddress;
    pCtxt->DrvConfigArray[pMappingEntry->RscSwDrvMapId].DrvId         =  pMappingEntry->DrvId;
    if ((status = rscHalGetNumTcs (pMappingEntry->BaseAddress, pMappingEntry->DrvId, &numTcs)) == HAL_STATUS_SUCCESS)
    {
      pCtxt->DrvConfigArray[pMappingEntry->RscSwDrvMapId].NumTcs = numTcs; 
    }
    else
    {
      RPMH_LOG_0 ("\trscHalGetNumTcs failed in rscHalGetDrvInfo ");  
      status = HAL_INVALID_DRV_ID;
      break;
    }
    if ((status = rscHalGetNumCmdsPerTcs (pMappingEntry->BaseAddress, pMappingEntry->DrvId, &numCmdsPerTcs)) == HAL_STATUS_SUCCESS)
    {
      pCtxt->DrvConfigArray[pMappingEntry->RscSwDrvMapId].NumCmdsPerTcs =  numCmdsPerTcs;
    }
    else
    {
      RPMH_LOG_0 ("\trscHalGetNumCmdsPerTcs failed in rscHalGetDrvInfo ");  
      status = HAL_INVALID_DRV_ID;      
      break;
    }
    
  }
    
  return status;
    
}

/*
* Helper function to map DRV-specific information 
* Input: POinter to Global Hal Struct
*        Pointer to Mapping enty in config table for subsystem
* Return: HAL_STATUS_SUCCESS if success or error code
*/

int32 rscHalGetChipsetMappingTable (RscHalCtxt *pCtxt)
{
  if (pCtxt != NULL)
  {
    pCtxt->NumDrvs  =  DrvBaseAddressMappingTable_subsystem.Nrows;

  }
  else
  {
    RPMH_LOG_0 ("\tInvalid Context Ptr in rscHalGetChipsetMappingTable");    
    return HAL_INVALID_CTXT_NOT_INITIALIZED;
  }

  return HAL_STATUS_SUCCESS;
}

/**
* Function Name: RSCHalRegisterDrv 
* Description:   It is the first call made by RPMh driver to register/init a new DRV
* Input:         RSCSW_DRV_MAPPING DrvMapId
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalRegisterDrv (RSCSW_DRV_MAPPING rscDrvMapId)
{
  int32 status = HAL_STATUS_SUCCESS;
  RscHalCtxt *ctxt;
  
  if ((status = IS_DRV_VALID (rscDrvMapId)) < HAL_STATUS_SUCCESS)
  {
    RPMH_LOG_1 ("\tInvalid DrvMapId, %d, in RscHalRegisterDrv", rscDrvMapId);    
    return status;
  }
    
  ctxt = rscHalGetContext ();

  if (ctxt->HalInitialized != TRUE)
  {
    if ((status = rscHalGetChipsetMappingTable (ctxt)) == HAL_STATUS_SUCCESS)
    {
      if ((status = rscHalGetDrvInfo (ctxt)) < HAL_STATUS_SUCCESS)
      {
        return status;
      }
      ctxt->HalInitialized = TRUE;
    }
    else
    {
      //Log Error
      RPMH_LOG_0 ("\tMapping Table not mapped yet in RscHalRegisterDrv");
      return status;

    }
  }

  if (ctxt->DrvConfigArray[rscDrvMapId].BaseAddress != 0)
  {
    ctxt->DrvConfigArray[rscDrvMapId].DrvInitialized = TRUE;
  }
  else
  {
    RPMH_LOG_1 ("\tDrvMapId, %d, BaseAddress is not set in RscHalRegisterDrv", rscDrvMapId);
    status = HAL_INVALID_DRV_MAP_ID;
  }
    
  return status;

}

/**
* Function Name: RscHalVersionID 
* Description:   It returns RSC HW version Id
* Input:         RSCSW_DRV_MAPPING Id
*                uint32 *version: It returns RSC HW version Id
*                RSC HW Revision ID format:
*                23:16 Major Version
*                15:8  Minor Version
*                7:0   Step Version
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalVersionID (RSCSW_DRV_MAPPING drvMapId, uint32 *pVersion) 
{
  int32 status = HAL_STATUS_SUCCESS;
  RscHalCtxt *ctxt = rscHalGetContext ();

  if (pVersion == NULL)
  {
    RPMH_LOG_0 ( "\tpVersion passed in RscHalVersionID is NULL");
    return HAL_INVALID_INPUT_PTR;
  }
    
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    *pVersion = RSC_HAL_HWIO_INId_READ (PLATFORM,
                                        _RSCC_RSC_ID_DRVd,
                                        ctxt->DrvConfigArray[drvMapId].BaseAddress,
                                        ctxt->DrvConfigArray[drvMapId].DrvId);
  }
  else
  {
    RPMH_LOG_0 ( "\tContext not initialized in RscHalVersionID");
  }
    
  return status;
}

/**
* Function Name: RscHalReadConfig 
* Description:   It returns DRV-specific information such as Number of TCSs supported and Number of commands per TCS
* Input:         RSCSW_DRV_MAPPING Id
*                uint32 *NumTCS: Returns number of TCS supported 
*                uint32 *NumCmdsPerTCS: Returns number of commands per TCS
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalReadConfig (RSCSW_DRV_MAPPING drvMapId, uint32 *pNumTCS, uint32 *pNumCmdsPerTCS)
{
    
  int32 status = HAL_STATUS_SUCCESS;
  
  struct RscHalCtxt *ctxt =  rscHalGetContext ();

  if (pNumTCS == NULL || 
      pNumCmdsPerTCS == NULL)
  {
    RPMH_LOG_0 ( "\tInvalid Input PTR in RscHalReadConfig");
    return HAL_INVALID_INPUT_PTR;
  }
    
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    *pNumTCS        =  ctxt->DrvConfigArray[drvMapId].NumTcs;
    *pNumCmdsPerTCS =  ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs;
  }
  else
  {
    RPMH_LOG_0 ( "\tContext not initialized in RscHalReadConfig");
  }


  return status;

}

/**
* Function Name: RSCHalIsePCBTimedOut 
* Description:   It returns if ePCB has timedout. ePCB can only time out if PDC is hung which is an error fatal state
* Input:         RSCSW_DRV_MAPPING DrvMapId
*                bool *ePCBStatus to return TRUE/FALSE
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalIsePCBTimedOut (RSCSW_DRV_MAPPING drvMapId, boolean *pePCBStatus)
{
  int32 status = HAL_STATUS_SUCCESS;
  
  struct RscHalCtxt *ctxt =  rscHalGetContext ();

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    if (pePCBStatus != NULL && 
        ctxt->DrvConfigArray[drvMapId].DrvId == 0)
    {
      *pePCBStatus = RSC_HAL_HWIO_IN_READ(PLATFORM,
                                          _RSCC_RSC_ERROR_IRQ_STATUS_DRV0,
                                          ctxt->DrvConfigArray[drvMapId].BaseAddress);
    }
#ifdef DRV_1_VALID
    else if (pePCBStatus != NULL && 
             ctxt->DrvConfigArray[drvMapId].DrvId > 0)
    {
      *pePCBStatus = RSC_HAL_HWIO_INId_READ (PLATFORM,
                                             _RSCC_RSC_ERROR_IRQ_STATUS_DRVd,
                                             ctxt->DrvConfigArray[drvMapId].BaseAddress,
                                             ctxt->DrvConfigArray[drvMapId].DrvId);
    }
#endif
#ifdef DRV_2_VALID
    else if (pePCBStatus != NULL && 
             ctxt->DrvConfigArray[drvMapId].DrvId > 0)
    {
      *pePCBStatus = RSC_HAL_HWIO_INId_READ (PLATFORM,
                                             _RSCC_RSC_ERROR_IRQ_STATUS_DRVd,
                                             ctxt->DrvConfigArray[drvMapId].BaseAddress,
                                             ctxt->DrvConfigArray[drvMapId].DrvId);
    }
#endif
    else
    {
      RPMH_LOG_0 ( "\tInvalid Input PTR in RscHalIsePCBTimedOut");
      status = HAL_INVALID_INPUT_PTR;
    }
            
  }
  else
  {
    RPMH_LOG_0 ( "\tContext not initialized in RscHalIsePCBTimedOut");
  }
        
  return status;
}

/**
* Function Name: HalRscClearePCBTimedOut 
* Description:   Clear ePCBTimedout IRQ. This can only be done by DRV0
* Input:         RSCSW_DRV_MAPPING DrvMapId
* Return:        HAL_STATUS_SUCCESS if success or error code
**/

int32 RscHalClearePCBTimedOut (RSCSW_DRV_MAPPING drvMapId)
{
  int32 status = HAL_STATUS_SUCCESS;
  uint32 value = 0;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
      
     if( ctxt->DrvConfigArray[drvMapId].DrvId == 0)
     {
        value =   DRV0_CLEAR_IRQ_STATUS;
        value <<= RSC_HAL_BIT_SHFT (PLATFORM,_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0,_CLR_EPCB_TIMEOUT_IRQ_SHFT);
        value &=  RSC_HAL_BIT_MSK (PLATFORM,_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0, _CLR_EPCB_TIMEOUT_IRQ_BMSK);
    
        RSC_HAL_HWIO_OUT_WRITE (PLATFORM,
                                _RSCC_RSC_ERROR_IRQ_CLEAR_DRV0,
                                ctxt->DrvConfigArray[drvMapId].BaseAddress, 
                                value);
     }
     else 
     {
        //Log Error
        RPMH_LOG_1 ("\tInvalid Operation for DrvId, %d, in RscHalClearePCBTimedOut",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_OP_FOR_DRV;
     }
  }
  else
  {
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }
    
  return status;

}

/**
* Function Name:    RscHalToggleePCBTimeOut
* Description:      RscHalToggleePCBTimeOut enables/disables ePCB time out interrupt
*                   Only DRV0 can perform this operation (Other DRVs are read only)
* Input:            RSCSW_DRV_MAPPING   DrvMapId
*                   Status to be ENABLE/DISABLE
* Return:           TRUE if success ow error code 
**/

int32 RscHalToggleePCBTimeOut (RSCSW_DRV_MAPPING drvMapId, boolean status)
{
  uint32 baseAddress = 0;
  int32 retstatus = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
   

  if ((retstatus = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    if(ctxt->DrvConfigArray[drvMapId].DrvId == 0)
    {
      baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
                                           
      RSC_HAL_HWIO_OUT_WRITE_MSK (PLATFORM,
                                  _RSCC_RSC_ERROR_IRQ_ENABLE_DRV0,
                                  baseAddress,
                                  RSC_HAL_BIT_MSK(PLATFORM,_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0, _EPCB_TIMEOUT_IRQ_EN_BMSK),
                                  status << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0, _EPCB_TIMEOUT_IRQ_EN_SHFT));
                                  
    }
    else 
    {
      //Log Error
      RPMH_LOG_1 ("\tInvalid Operation for DrvId, %d, in RscHalToggleePCBTimeOut",ctxt->DrvConfigArray[drvMapId].DrvId);
      retstatus = HAL_INVALID_OP_FOR_DRV;    
    }
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }

  return retstatus;
}

/**
* Function Name:    RscHalUpdateePCBTimeOutThreshold
* Description:      RscHalUpdateePCBTimeOutThreshold updates threshold value for ePCB time out.
*                   The unit is rsc_clk clock cycle which driven by CXO; Only DRV0 can perform this operation 
*                   (Other DRVs read only)
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   Threshold value
* Return:           TRUE if success ow error code 
**/

int32 RscHalUpdateePCBTimeOutThreshold (RSCSW_DRV_MAPPING drvMapId, uint16 threshold)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    if(ctxt->DrvConfigArray[drvMapId].DrvId == 0)
    {
      baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
      RSC_HAL_HWIO_OUT_WRITE_MSK (PLATFORM,
                                  _RSCC_RSC_ERROR_IRQ_ENABLE_DRV0,
                                  baseAddress,
                                  RSC_HAL_BIT_MSK(PLATFORM,_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0, _EPCB_TIMEOUT_THRESHOLD_BMSK),
                                  threshold << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0, _EPCB_TIMEOUT_THRESHOLD_SHFT));      
    }
    else 
    {
      //Log Error
      RPMH_LOG_1 ("\tInvalid Operation for DrvId, %d, in RscHalUpdateePCBTimeOutThreshold",ctxt->DrvConfigArray[drvMapId].DrvId);
      status = HAL_INVALID_OP_FOR_DRV;
    }
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }

  return status;

}

/**
* Function Name:    RscHalTriggerTCS 
* Description:      RscHalTriggerTCS triggers controller in AMC mode 
* Input:            RSCSW_DRV_MAPPING 
*                   TCSId to be triggered
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalTriggerTCS (RSCSW_DRV_MAPPING drvMapId, uint8 tcsId)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
  boolean IsTCSIdle = FALSE;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalTriggerTCS", tcsId);
    status = HAL_INVALID_TCS_ID; 
    return status;
  }

  if ((status = RscHalIsTCSIdle(drvMapId,tcsId, &IsTCSIdle)) == HAL_STATUS_SUCCESS)
  {
    if(IsTCSIdle ==  FALSE)
    {
      RPMH_LOG_1 ("\tTCS Id %d is not Idle\n", tcsId);
      status = HAL_TCS_NOT_IDLE;
      return status;
    }
  }
  else
  {
    RPMH_LOG_0 ( "\tError in TCSIdle call in RscHalTriggerTCS");
    return status;
  }
    
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV0_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV0_CONTROL, _AMC_MODE_TRIGGER_BMSK), 
                                        AMC_CLEAR_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV0_CONTROL, _AMC_MODE_TRIGGER_SHFT));
        
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV0_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV0_CONTROL, _AMC_MODE_TRIGGER_BMSK), 
                                        AMC_SET_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV0_CONTROL, _AMC_MODE_TRIGGER_SHFT));

        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV1_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV1_CONTROL, _AMC_MODE_TRIGGER_BMSK), 
                                        AMC_CLEAR_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV1_CONTROL, _AMC_MODE_TRIGGER_SHFT));
        
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV1_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV1_CONTROL, _AMC_MODE_TRIGGER_BMSK), 
                                        AMC_SET_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV1_CONTROL, _AMC_MODE_TRIGGER_SHFT));
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV2_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV2_CONTROL, _AMC_MODE_TRIGGER_BMSK), 
                                        AMC_CLEAR_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV2_CONTROL, _AMC_MODE_TRIGGER_SHFT));
        
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV2_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV2_CONTROL, _AMC_MODE_TRIGGER_BMSK), 
                                        AMC_SET_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV2_CONTROL, _AMC_MODE_TRIGGER_SHFT));
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalTriggerTCS",ctxt->DrvConfigArray[drvMapId].DrvId);  
        status = HAL_INVALID_DRV_ID;
        break;

    }
  }
  else
  {
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }
    
  return status;
}

/**
* Function Name:    RscHalConvertTCStoAMC 
* Description:      RscHalConvertTCStoAMC converts a TCS into AMC. 
*                   RPMh driver should confirm if TCS is free before converting it to AMC 
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId to be triggered
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalConvertTCStoAMC (RSCSW_DRV_MAPPING drvMapId, uint8 tcsId)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
  boolean IsTCSIdle = FALSE;


  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalConvertTCStoAMC", tcsId);
    status = HAL_INVALID_TCS_ID; 
    return status;
  }
  if ((status = RscHalIsTCSIdle(drvMapId,tcsId, &IsTCSIdle)) == HAL_STATUS_SUCCESS)
  {
    if(IsTCSIdle ==  FALSE)
    {
      RPMH_LOG_1 ("\tTCS Id %d is not Idle\n", tcsId);
      status = HAL_TCS_NOT_IDLE;
      return status;
    }
  }
  else
  {
    RPMH_LOG_0 ( "\tError in TCSIdle call in RscHalConvertTCStoAMC");
    return status;
  }
    
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV0_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV0_CONTROL, _AMC_MODE_EN_BMSK), 
                                        AMC_SET_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV0_CONTROL, _AMC_MODE_EN_SHFT));
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV1_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV1_CONTROL, _AMC_MODE_EN_BMSK), 
                                        AMC_SET_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV1_CONTROL, _AMC_MODE_EN_SHFT));
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV2_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV2_CONTROL, _AMC_MODE_EN_BMSK), 
                                        AMC_SET_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV2_CONTROL, _AMC_MODE_EN_SHFT));
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalConvertTCStoAMC", ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID;
        break;
     }
   }
   else
   {
     //Log Error
     RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
   }
        
  return status;

}

/**
* Function Name:    RscHalConvertAMCtoTCS 
* Description:      RscHalConvertAMCtoTCS converts a AMC back into TCS. 
*                   RPMh driver should confirm if AMC is free before converting it to TCS 
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId to be triggered
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalConvertAMCtoTCS (RSCSW_DRV_MAPPING drvMapId, uint8 tcsId)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
  boolean IsTCSIdle = FALSE;

  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalConvertAMCtoTCS",tcsId);
    status = HAL_INVALID_TCS_ID; 
    return status;
  }
  if ((status = RscHalIsTCSIdle(drvMapId,tcsId, &IsTCSIdle)) == HAL_STATUS_SUCCESS)
  {
    if(IsTCSIdle ==  FALSE)
    {
      RPMH_LOG_1 ("\tTCS Id %d is not Idle\n", tcsId);
      status = HAL_TCS_NOT_IDLE;
      return status;
    }
  }
  else
  {
    RPMH_LOG_0 ( "\tError in TCSIdle call in RscHalConvertAMCtoTCS");
    return status;
  }

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK  (PLATFORM,
                                         _RSCC_TCSm_DRV0_CONTROL,
                                         baseAddress,
                                         tcsId,
                                         RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV0_CONTROL, _AMC_MODE_EN_BMSK), 
                                         AMC_CLEAR_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV0_CONTROL, _AMC_MODE_EN_SHFT));
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV1_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV1_CONTROL, _AMC_MODE_EN_BMSK), 
                                        AMC_CLEAR_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV1_CONTROL, _AMC_MODE_EN_SHFT));
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV2_CONTROL,
                                        baseAddress,
                                        tcsId,
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV2_CONTROL, _AMC_MODE_EN_BMSK), 
                                        AMC_CLEAR_BIT << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV2_CONTROL, _AMC_MODE_EN_SHFT));
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalConvertAMCtoTCS",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID;
        break;
    }
                        
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }
    
  return status;
}

/**
* Function Name:    RscHalIsTCSIdle
* Description:      RscHalIsTCSIdle returns if a TCS is Idle. This is a polling call
* Input:            RSCSW_DRV_MAPPING  DrvId 
*                   TCSId 
*                   bool * to get the status, TRUE/FALSE
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalIsTCSIdle(RSCSW_DRV_MAPPING drvMapId,uint8 tcsId, boolean *pIsTCSIdle)
{
    
  uint32 baseAddress = 0;
  int32 status = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalIsTCSIdle",tcsId);
    status = HAL_INVALID_TCS_ID; 
    return status;
  }
    
  if (pIsTCSIdle == NULL)
  {
    status = HAL_INVALID_INPUT_PTR;
    return status;
  }
    
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        *pIsTCSIdle = RSC_HAL_HWIO_INId_READ_MSK (PLATFORM,
                                                  _RSCC_TCSm_DRV0_STATUS,
                                                  baseAddress,
                                                  tcsId, 
                                                  _CONTROLLER_IDLE_BMSK);
        *pIsTCSIdle >>= RSC_HAL_BIT_SHFT (PLATFORM,
                                          _RSCC_TCSm_DRV0_STATUS, 
                                          _CONTROLLER_IDLE_SHFT);
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        *pIsTCSIdle = RSC_HAL_HWIO_INId_READ_MSK (PLATFORM,
                                                  _RSCC_TCSm_DRV1_STATUS,
                                                  baseAddress,
                                                  tcsId, 
                                                  _CONTROLLER_IDLE_BMSK);
        
        *pIsTCSIdle >>= RSC_HAL_BIT_SHFT (PLATFORM,
                                         _RSCC_TCSm_DRV1_STATUS, 
                                         _CONTROLLER_IDLE_SHFT);

        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        *pIsTCSIdle = RSC_HAL_HWIO_INId_READ_MSK (PLATFORM,
                                                  _RSCC_TCSm_DRV2_STATUS,
                                                  baseAddress,
                                                  tcsId, 
                                                  _CONTROLLER_IDLE_BMSK);
        
        *pIsTCSIdle >>= RSC_HAL_BIT_SHFT (PLATFORM,
                                         _RSCC_TCSm_DRV2_STATUS, 
                                         _CONTROLLER_IDLE_SHFT);

        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalIsTCSIdle",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID; 
        break;
     }
   }
   else
   {
     //Log Error
     RPMH_LOG_0 ( "\tContext Not Initialized yet"); 

   }
    
   return status;
}

/**
* Function Name:    RscHalToggleTCSCmd
* Description:      RscHalToggleTCSCmd can be used to enable/disable specific commands in a TCS
* Input:            RSCSW_DRV_MAPPING  DrvId 
*                   TCSId
*                   CmdIdMask: 32-bit CommandMask to identify commands to be enabled/disabled.One bit per command
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalToggleTCSCmd (RSCSW_DRV_MAPPING drvMapId, uint8 tcsId, uint32 cmdIdMask)
{
  uint32 baseAddress = 0;
  int32 status = HAL_STATUS_SUCCESS;
  boolean IsTCSIdle = FALSE;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalToggleTCSCmd", tcsId);
    status = HAL_INVALID_TCS_ID; 
    return status;
  }
    
  if ((status = RscHalIsTCSIdle(drvMapId,tcsId, &IsTCSIdle)) == HAL_STATUS_SUCCESS)
  {
    if(IsTCSIdle ==  FALSE)
    {
      RPMH_LOG_1 ("\tTCS Id %d is not Idle\n", tcsId);
      status = HAL_TCS_NOT_IDLE;
      return status;
    }
  }
  else
  {
    RPMH_LOG_0 ( "\tError in TCSIdle call in RscHalConvertAMCtoTCS");
    return status;
  }    
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV0_CMD_ENABLE,
                                        baseAddress,
                                        tcsId, 
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV0_CMD_ENABLE, _INDIVIDUAL_CMD_ENABLE_MASK_BMSK), 
                                        cmdIdMask << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV0_CMD_ENABLE,_INDIVIDUAL_CMD_ENABLE_MASK_SHFT));
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV1_CMD_ENABLE,
                                        baseAddress,
                                        tcsId, 
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV1_CMD_ENABLE, _INDIVIDUAL_CMD_ENABLE_MASK_BMSK), 
                                        cmdIdMask << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV1_CMD_ENABLE,_INDIVIDUAL_CMD_ENABLE_MASK_SHFT));
        break;
#endif	
#ifdef DRV_2_VALID
      case DRV_ID_2:
        RSC_HAL_HWIO_TCS_OUT_WRITE_MSK (PLATFORM,
                                        _RSCC_TCSm_DRV2_CMD_ENABLE,
                                        baseAddress,
                                        tcsId, 
                                        RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_DRV2_CMD_ENABLE, _INDIVIDUAL_CMD_ENABLE_MASK_BMSK), 
                                        cmdIdMask << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_DRV2_CMD_ENABLE,_INDIVIDUAL_CMD_ENABLE_MASK_SHFT));
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalToggleTCSCmd",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID; 
        break;
                        
    }
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 

  }

  return status;

}

/**
* Function Name:    RscHalEnableAMCFinishedIRQ
* Description:      RscHalEnableAMCFinishedIRQ is used to enable completion/finish interrupt on a  TCS/AMC.
*                   Interrupt is generated once per TCS/AMC—e.g. if there are 3 AMCs triggered and all three have completion interrupt enabled, 
*                   there would be 3 interrupts
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalEnableAMCFinishedIRQ (RSCSW_DRV_MAPPING drvMapId, uint32 tcsId)
{
    
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
   struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalEnableAMCFinishedIRQ",tcsId);
    status = HAL_INVALID_TCS_ID; 
    return status;
  }
   
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        RSC_HAL_HWIO_OUT_WRITE_MSK (PLATFORM,
                                    _RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV0,
                                    baseAddress,
                                    AMC_ENABLE_FINISH_IRQ << tcsId, 
                                    AMC_ENABLE_FINISH_IRQ << tcsId);
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        RSC_HAL_HWIO_OUT_WRITE_MSK (PLATFORM,
                                    _RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV1,
                                    baseAddress,
                                    AMC_ENABLE_FINISH_IRQ << tcsId, 
                                    AMC_ENABLE_FINISH_IRQ << tcsId);
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        RSC_HAL_HWIO_OUT_WRITE_MSK (PLATFORM,
                                    _RSCC_TCS_AMC_MODE_IRQ_ENABLE_DRV2,
                                    baseAddress,
                                    AMC_ENABLE_FINISH_IRQ << tcsId, 
                                    AMC_ENABLE_FINISH_IRQ << tcsId);
        break;	
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalEnableAMCFinishedIRQ",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID;
        break;
     }                  
                                                            
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 

  }
    
  return status;

}

/**
* Function Name:    RscHalCheckAMCFinishedIRQ 
* Description:      RscHalCheckAMCFinishedIRQ will return TCS/AMC Completion IRQ status
*                   Status will provide 32 bit value for all the completed TCSs. 1-bit per TCS
* Input:            RSCSW_DRV_MAPPING
*                   uint *Status will return completed TCSs
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalCheckAMCFinishedIRQ (RSCSW_DRV_MAPPING drvMapId, uint32 *pStatus)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
  if (pStatus == NULL)
  {
    return HAL_INVALID_INPUT_PTR;
  }

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
       *pStatus  = RSC_HAL_HWIO_INM_READ (PLATFORM,
                                          _RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0,
                                          baseAddress,
                                          _AMC_COMPLETION_IRQ_STATUS_BMSK);
            
       *pStatus  = (*pStatus  >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                    _RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV0, 
                                                    _AMC_COMPLETION_IRQ_STATUS_SHFT));
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        *pStatus  = RSC_HAL_HWIO_INM_READ (PLATFORM,
                                           _RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1,
                                           baseAddress,
                                           _AMC_COMPLETION_IRQ_STATUS_BMSK);
        
        *pStatus  = (*pStatus  >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                     _RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV1, 
                                                     _AMC_COMPLETION_IRQ_STATUS_SHFT));
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        *pStatus  = RSC_HAL_HWIO_INM_READ (PLATFORM,
                                           _RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2,
                                           baseAddress,
                                           _AMC_COMPLETION_IRQ_STATUS_BMSK);
        
        *pStatus  = (*pStatus  >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                     _RSCC_TCS_AMC_MODE_IRQ_STATUS_DRV2, 
                                                     _AMC_COMPLETION_IRQ_STATUS_SHFT));
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalCheckAMCFinishedIRQ",ctxt->DrvConfigArray[drvMapId].DrvId);  
        status = HAL_INVALID_DRV_ID;
        break;
    }

  }
  else
  {
     //Log Error
     RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }


  return status;

}

/**
* Function Name:    RscHalClearAMCFinishedIRQ 
* Description:      RscHalClearAMCFinishedIRQ will clear TCS/AMC Completion IRQ status
*                   The IRQ is cleared per AMC.So, IRQ will stay pending if only one AMC is cleared and interrupt is pending for multiple AMCs. 
*                   Because of this, SW does not need to handle all completed AMCs in a single IRQ handler invocation
* Input:            RSCSW_DRV_MAPPING
*                   TCS  Id. 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/


int32 RscHalClearAMCFinishedIRQ (RSCSW_DRV_MAPPING drvMapId,uint32 tcsId)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();

  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalEnableAMCFinishedIRQ",tcsId);
    status = HAL_INVALID_TCS_ID; 
    return status;
  }

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        RSC_HAL_HWIO_OUT_WRITE (PLATFORM,
                                _RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV0,
                                baseAddress, 
                                AMC_SET_BIT << tcsId);
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        RSC_HAL_HWIO_OUT_WRITE (PLATFORM,
                                _RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV1,
                                baseAddress, 
                                AMC_SET_BIT << tcsId);
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        RSC_HAL_HWIO_OUT_WRITE (PLATFORM,
                                _RSCC_TCS_AMC_MODE_IRQ_CLEAR_DRV2,
                                baseAddress, 
                                AMC_SET_BIT << tcsId);
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalClearAMCFinishedIRQ",ctxt->DrvConfigArray[drvMapId].DrvId);  
        status = HAL_INVALID_DRV_ID;
        break;
         
    }
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }

  return status;
}

/**
* Function Name:    RscHalCheckTCSCmdTriggerStatus 
* Description:      RscHalCheckTCSCmdTriggerStatus is used to check if a command in TCS was successfully triggered.
*                   This operation can be done once completion IRQ for TCS/AMC has arrived
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId
*                   Command Id
*                   bool *Status to get TRUE/FALSE 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalCheckTCSCmdTriggerStatus (RSCSW_DRV_MAPPING drvMapId, uint8 tcsId, uint8 cmdId, boolean *pStatus)
{
    
  uint32 baseAddress = 0;
  int32 status = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalCheckTCSCmdTriggerStatus",tcsId);
    status = HAL_INVALID_TCS_ID;
    return status;
  }

  if ((ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs == 0) || 
        (IS_CMD_VALID (cmdId,ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid CMD Id, %d, in RscHalCheckTCSCmdTriggerStatus",cmdId);
    status = HAL_INVALID_CMD_ID;
    return status;
  }

  if (pStatus == NULL)
  {
    return HAL_INVALID_INPUT_PTR;    
  }

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                    _RSCC_TCSm_CMDn_DRV0_STATUS,
                                                    baseAddress,
                                                    tcsId,
                                                    cmdId, 
                                                    _TRIGGERED_BMSK);
        *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                   _RSCC_TCSm_CMDn_DRV0_STATUS,
                                                   _TRIGGERED_SHFT));        
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                    _RSCC_TCSm_CMDn_DRV1_STATUS,
                                                    baseAddress,
                                                    tcsId,
                                                    cmdId, 
                                                    _TRIGGERED_BMSK);
        *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                   _RSCC_TCSm_CMDn_DRV1_STATUS,
                                                   _TRIGGERED_SHFT));                    
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                    _RSCC_TCSm_CMDn_DRV2_STATUS,
                                                    baseAddress,
                                                    tcsId,
                                                    cmdId, 
                                                    _TRIGGERED_BMSK);
        *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                   _RSCC_TCSm_CMDn_DRV2_STATUS,
                                                   _TRIGGERED_SHFT));                    
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalCheckTCSCmdTriggerStatus",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID;
        break;
                        
    }                     
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }

  return status;

}

/**
* Function Name:    RscHalCheckTCSCmdIssueStatus   
* Description:      RscHalCheckTCSCmdIssueStatus is used to check if a command in TCS was successfully Issued.
*                   This operation can be done once completion IRQ for TCS/AMC has arrived
* Input:            RSCSW_DRV_MAPPING DrvId 
*                   TCSId
*                   Command Id
*                   bool *Status to get TRUE/FALSE 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalCheckTCSCmdIssueStatus (RSCSW_DRV_MAPPING drvMapId, uint8 tcsId, uint8 cmdId, boolean *pStatus)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
   struct RscHalCtxt *ctxt =  rscHalGetContext();

    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalCheckTCSCmdIssueStatus",tcsId);
    return HAL_INVALID_TCS_ID;
  }

  if ((ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs == 0) || 
        (IS_CMD_VALID (cmdId,ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid CMD Id, %d, in RscHalCheckTCSCmdIssueStatus",cmdId);
    return HAL_INVALID_CMD_ID;
  }

  if (pStatus == NULL)
  {
    return HAL_INVALID_INPUT_PTR;
  }

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                    _RSCC_TCSm_CMDn_DRV0_STATUS,
                                                    baseAddress,
                                                    tcsId,
                                                    cmdId, 
                                                    _ISSUED_BMSK);
        *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                  _RSCC_TCSm_CMDn_DRV0_STATUS,
                                                  _ISSUED_SHFT));
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                    _RSCC_TCSm_CMDn_DRV1_STATUS,
                                                    baseAddress,
                                                    tcsId,
                                                    cmdId, 
                                                    _ISSUED_BMSK);
        *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                   _RSCC_TCSm_CMDn_DRV1_STATUS,
                                                   _ISSUED_SHFT));
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                    _RSCC_TCSm_CMDn_DRV2_STATUS,
                                                    baseAddress,
                                                    tcsId,
                                                    cmdId, 
                                                    _ISSUED_BMSK);
        *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                   _RSCC_TCSm_CMDn_DRV2_STATUS,
                                                   _ISSUED_SHFT));
        break;	
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalCheckTCSCmdIssueStatus",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID;
        break;                
    }
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }
    
  return status;

}

/**
* Function Name:    RscHalCheckTCSCmdCompletionStatus
* Description:      RscHalCheckTCSCmdCompletionStatus is used to check if a command in TCS was successfully completed.
*                   This can not be done for fire-n-forget requests and can only be done for completion requests.
*                   This operation can be done once completion IRQ for TCS/AMC has arrived
* Input:            RSCSW_DRV_MAPPING  DrvId 
*                   TCSId
*                   Command Id
*                   bool *Status to get TRUE/FALSE 
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalCheckTCSCmdCompletionStatus (RSCSW_DRV_MAPPING drvMapId, uint8 tcsId, uint8 cmdId, boolean *pStatus)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < 0))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalCheckTCSCmdCompletionStatus",tcsId);
    return HAL_INVALID_TCS_ID;
  }

  if ((ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs == 0) || 
       (IS_CMD_VALID (cmdId,ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs) < 0))
  {
    RPMH_LOG_1 ("\tInvalid CMD Id, %d, in RscHalCheckTCSCmdCompletionStatus",cmdId);
    return HAL_INVALID_CMD_ID;
  }

  if (pStatus == NULL)
  {
    return HAL_INVALID_INPUT_PTR;
  }

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
      baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
      switch (ctxt->DrvConfigArray[drvMapId].DrvId)
      {
        case DRV_ID_0:
          *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                      _RSCC_TCSm_CMDn_DRV0_STATUS,
                                                      baseAddress,
                                                      tcsId,
                                                      cmdId, 
                                                      _COMPLETED_BMSK);
           *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                      _RSCC_TCSm_CMDn_DRV0_STATUS,
                                                      _COMPLETED_SHFT));
          break;
#ifdef DRV_1_VALID
        case DRV_ID_1:
          *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                      _RSCC_TCSm_CMDn_DRV1_STATUS,
                                                      baseAddress,
                                                      tcsId,
                                                      cmdId, 
                                                      _COMPLETED_BMSK);
          *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                     _RSCC_TCSm_CMDn_DRV1_STATUS,
                                                     _COMPLETED_SHFT));
          break;
#endif	
#ifdef DRV_2_VALID	  
        case DRV_ID_2:
          *pStatus = RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                      _RSCC_TCSm_CMDn_DRV2_STATUS,
                                                      baseAddress,
                                                      tcsId,
                                                      cmdId, 
                                                      _COMPLETED_BMSK);
          *pStatus = (*pStatus >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                     _RSCC_TCSm_CMDn_DRV2_STATUS,
                                                     _COMPLETED_SHFT));
          break;
#endif
        default:
          RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalCheckTCSCmdCompletionStatus", ctxt->DrvConfigArray[drvMapId].DrvId);  
          status = HAL_INVALID_DRV_ID;
          break;
      }
                    
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }
    
  return status;

}

/**
* Function Name:    RscHalReadResponseData 
* Description:      RscHalReadResponseData is used to read response data from RPMh HW for a read command sent by RPMh driver
*                   This operation can be done once completion IRQ for TCS/AMC has arrived
* Input:            RSCSW_DRV_MAPPING  DrvId 
*                   TCSId 
*                   Command Id
*                   uint32 * to be filled with returned data from RPMh HW
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalReadResponseData (RSCSW_DRV_MAPPING drvMapId, uint8 tcsId, uint8 cmdId, uint32 *pDataReturned)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalReadResponseData",tcsId);
    return HAL_INVALID_TCS_ID;
  }

  if ((ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs == 0) || 
       (IS_CMD_VALID (cmdId,ctxt->DrvConfigArray[drvMapId].NumCmdsPerTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid CMD Id, %d, in RscHalReadResponseData",cmdId);
    return HAL_INVALID_CMD_ID;
  }

  if (pDataReturned == NULL)
  {
    return HAL_INVALID_INPUT_PTR;
  }

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
     baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
     switch (ctxt->DrvConfigArray[drvMapId].DrvId)
     {
       case DRV_ID_0:
         *pDataReturned =   RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                             _RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA,
                                                             baseAddress,
                                                             tcsId,
                                                             cmdId, 
                                                             _READ_RESPONSE_DATA_BMSK);
            
         *pDataReturned  = (*pDataReturned >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                                 _RSCC_TCSm_CMDn_DRV0_READ_RESPONSE_DATA,
                                                                 _READ_RESPONSE_DATA_SHFT));
         break;
#ifdef DRV_1_VALID
       case DRV_ID_1:
         *pDataReturned =   RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                             _RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA,
                                                             baseAddress,
                                                             tcsId,
                                                             cmdId, 
                                                             _READ_RESPONSE_DATA_BMSK);           

         *pDataReturned  = (*pDataReturned >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                                 _RSCC_TCSm_CMDn_DRV1_READ_RESPONSE_DATA,
                                                                 _READ_RESPONSE_DATA_SHFT));
           
         break;
#endif	
#ifdef DRV_2_VALID	 
       case DRV_ID_2:
         *pDataReturned =   RSC_HAL_HWIO_CMD_INId_READ_MASK (PLATFORM,
                                                             _RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA,
                                                             baseAddress,
                                                             tcsId,
                                                             cmdId, 
                                                             _READ_RESPONSE_DATA_BMSK);           

         *pDataReturned  = (*pDataReturned >>  RSC_HAL_BIT_SHFT (PLATFORM,
                                                                 _RSCC_TCSm_CMDn_DRV2_READ_RESPONSE_DATA,
                                                                 _READ_RESPONSE_DATA_SHFT));
           
         break;	 
#endif
       default:
         RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalReadResponseData",ctxt->DrvConfigArray[drvMapId].DrvId);  
         status = HAL_INVALID_DRV_ID;
         break;
     }

  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
    status = HAL_INVALID_CTXT_NOT_INITIALIZED;
  }
    
  return status;

}


/**
* Function Name:    RscHalConfigureCmdCompletion 
* Description:      RscHalConfigureCmdCompletion will make TCS/AMC HW wait untill a specific command is "completed"
* Input:            RSCSW_DRV_MAPPING  DrvId
*                   TCS Id
*                   CmdMaskId, 32 bit value, to be completed before moving to next command
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/


int32 RscHalConfigureCmdCompletion (RSCSW_DRV_MAPPING drvMapId,uint8 tcsId, uint32 cmdMaskId)
{
  uint32 baseAddress = 0;
  int32  status = HAL_STATUS_SUCCESS;
    
  struct RscHalCtxt *ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (tcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalConfigureCmdCompletion",tcsId);
    return HAL_INVALID_TCS_ID;
  }

  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        cmdMaskId &=  RSC_HAL_BIT_MSK (PLATFORM,_RSCC_TCSm_DRV0,_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK);
        cmdMaskId >>= RSC_HAL_BIT_SHFT (PLATFORM,_RSCC_TCSm_DRV0,_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT);
        RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV0_CMD_WAIT_FOR_CMPL,
                                    baseAddress,
                                    tcsId,
                                    cmdMaskId);
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        //cmdMaskId &=  HWIO_MSS_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK;
        //cmdMaskId >>= HWIO_MSS_RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT;
        cmdMaskId &=  RSC_HAL_BIT_MSK (PLATFORM,_RSCC_TCSm_DRV1,_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK);
        cmdMaskId >>= RSC_HAL_BIT_SHFT (PLATFORM,_RSCC_TCSm_DRV1,_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT);
        RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV1_CMD_WAIT_FOR_CMPL,
                                    baseAddress,
                                    tcsId,
                                    cmdMaskId);            
        break;
#endif	
#ifdef DRV_2_VALID	
      case DRV_ID_2:
        cmdMaskId &=  RSC_HAL_BIT_MSK (PLATFORM,_RSCC_TCSm_DRV2,_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_BMSK);
        cmdMaskId >>= RSC_HAL_BIT_SHFT (PLATFORM,_RSCC_TCSm_DRV2,_CMD_WAIT_FOR_CMPL_CMD_WAIT_FOR_CMPL_SHFT);
        RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV2_CMD_WAIT_FOR_CMPL,
                                    baseAddress,
                                    tcsId,
                                    cmdMaskId);            
        break;	
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalConfigureCmdCompletion",ctxt->DrvConfigArray[drvMapId].DrvId); 
        status = HAL_INVALID_DRV_ID;
        break;
     }
  }
  else
  {
    //Log Error
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
  }    

  return status;
}

/*
* Helper function to setup TCSs for DRV0
* Imput: Base Adddress
*        Tcs Id
*        Pointer to CommandInfo
* Output: NULL
*/
//#define PLATFORM        #HWIO_MSS
int32 rscHalSetupDrv0Tcs (uint32 baseAddress, uint8 tcsId, RscHalMessageBody *pCommandInfo)
{

  if(baseAddress == 0)
  {
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
    return HAL_INVALID_CTXT_NOT_INITIALIZED;
  }


  if(pCommandInfo == NULL)
  {
    RPMH_LOG_0 ( "\tInvalid Input Ptr pCommandInfo in rscHalSetupDrv1Tcs"); 
    return HAL_INVALID_INPUT_PTR;
  }

  //Slave Id Programming 
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE ( PLATFORM,
                                _RSCC_TCSm_CMDn_DRV0_ADDR,
                                baseAddress,
                                tcsId,
                                pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV0_ADDR,_SLV_ID_BMSK),
                                 pCommandInfo->SlaveId << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV0_ADDR,_SLV_ID_SHFT));
  //Offset Programming

  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV0_ADDR,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV0_ADDR,_OFFSET_BMSK),
                                 pCommandInfo->Offset << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV0_ADDR,_OFFSET_SHFT));

  //Request Type

  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV0_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV0_MSGID,_READ_OR_WRITE_BMSK),
                                 GET_REQUEST_TYPE(pCommandInfo->RequestType) << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV0_MSGID,_READ_OR_WRITE_SHFT));



  //Response Type
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV0_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV0_MSGID,_RES_REQ_BMSK),
                                 GET_RESPONSE_TYPE(pCommandInfo->RequestType) << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV0_MSGID,_RES_REQ_SHFT));



  //Message Length
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV0_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV0_MSGID,_MSG_LENGTH_BMSK),
                                 pCommandInfo->DataSize << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV0_MSGID,_MSG_LENGTH_SHFT));
  //Message 
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV0_DATA,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV0_DATA,_DATA_BMSK),
                                 pCommandInfo->Data << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV0_DATA,_DATA_SHFT));

  return HAL_STATUS_SUCCESS;
}

/*
* Helper function to setup TCSs for DRV1
* Imput: Base Adddress
*        Tcs Id
*        Pointer to CommandInfo
* Output: Hal_Status_Success ow Invalid id
*/
#ifdef DRV_1_VALID

int32 rscHalSetupDrv1Tcs (uint32 baseAddress, uint8 tcsId, RscHalMessageBody *pCommandInfo)
{

  if(baseAddress == 0)
  {
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
    return HAL_INVALID_CTXT_NOT_INITIALIZED;
  }


  if(pCommandInfo == NULL)
  {
    RPMH_LOG_0 ( "\tInvalid Input Ptr pCommandInfo in rscHalSetupDrv1Tcs"); 
    return HAL_INVALID_INPUT_PTR;
  }

  //Slave Id Programming    
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV1_ADDR,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV1_ADDR,_SLV_ID_BMSK),
                                 pCommandInfo->SlaveId << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV1_ADDR,_SLV_ID_SHFT));  
  //Offset Programming
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV1_ADDR,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV1_ADDR,_OFFSET_BMSK),
                                 pCommandInfo->Offset << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV1_ADDR,_OFFSET_SHFT));
  
  //Request Type
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV1_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV1_MSGID,_READ_OR_WRITE_BMSK),
                                 GET_REQUEST_TYPE(pCommandInfo->RequestType) << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV1_MSGID,_READ_OR_WRITE_SHFT));
  
  //Response Type
   RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV1_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV1_MSGID,_RES_REQ_BMSK),
                                 GET_RESPONSE_TYPE(pCommandInfo->RequestType) << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV1_MSGID,_RES_REQ_SHFT));
   
  //Message Length
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV1_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV1_MSGID,_MSG_LENGTH_BMSK),
                                 pCommandInfo->DataSize << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV1_MSGID,_MSG_LENGTH_SHFT));  
  //Message 
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV1_DATA,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV1_DATA,_DATA_BMSK),
                                 pCommandInfo->Data << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV1_DATA,_DATA_SHFT));  

  return HAL_STATUS_SUCCESS;
  
}
#endif
#ifdef DRV_2_VALID	
int32 rscHalSetupDrv2Tcs (uint32 baseAddress, uint8 tcsId, RscHalMessageBody *pCommandInfo)
{

  if(baseAddress == 0)
  {
    RPMH_LOG_0 ( "\tContext Not Initialized yet"); 
    return HAL_INVALID_CTXT_NOT_INITIALIZED;
  }


  if(pCommandInfo == NULL)
  {
    RPMH_LOG_0 ( "\tInvalid Input Ptr pCommandInfo in rscHalSetupDrv2Tcs"); 
    return HAL_INVALID_INPUT_PTR;
  }

  //Slave Id Programming    
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV2_ADDR,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV1_ADDR,_SLV_ID_BMSK),
                                 pCommandInfo->SlaveId << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV2_ADDR,_SLV_ID_SHFT));  
  //Offset Programming
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV2_ADDR,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV2_ADDR,_OFFSET_BMSK),
                                 pCommandInfo->Offset << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV2_ADDR,_OFFSET_SHFT));
  
  //Request Type
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV2_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV2_MSGID,_READ_OR_WRITE_BMSK),
                                 GET_REQUEST_TYPE(pCommandInfo->RequestType) << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV2_MSGID,_READ_OR_WRITE_SHFT));
  
  //Response Type
   RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV2_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV2_MSGID,_RES_REQ_BMSK),
                                 GET_RESPONSE_TYPE(pCommandInfo->RequestType) << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV2_MSGID,_RES_REQ_SHFT));
   
  //Message Length
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV2_MSGID,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV2_MSGID,_MSG_LENGTH_BMSK),
                                 pCommandInfo->DataSize << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV2_MSGID,_MSG_LENGTH_SHFT));  
  //Message 
  RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(PLATFORM,
                                 _RSCC_TCSm_CMDn_DRV2_DATA,
                                 baseAddress,
                                 tcsId,
                                 pCommandInfo->CmdId,
                                 RSC_HAL_BIT_MSK(PLATFORM,_RSCC_TCSm_CMDn_DRV2_DATA,_DATA_BMSK),
                                 pCommandInfo->Data << RSC_HAL_BIT_SHFT(PLATFORM,_RSCC_TCSm_CMDn_DRV2_DATA,_DATA_SHFT));  

  return HAL_STATUS_SUCCESS;
  
}
#endif

/**
* Function Name:    RscHalSetupTCS
* Description:      RscHalSetupTCS is used to setup commands on any given TCS
* Input:            RscHalMessage pointer
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalSetupTCS (RscHalMessage *pMsg)
{
  RSCSW_DRV_MAPPING drvMapId;
  RscHalMessageBody *commandInfo = NULL;
  uint32 baseAddress = 0;
  int32 status =  HAL_STATUS_SUCCESS;
  boolean IsTCSIdle = FALSE;
  RscHalCtxt *ctxt;
  uint32 cntCmds =0;
  
  if (pMsg == NULL)
  {
    RPMH_LOG_0 ( "\tInvalid Input Ptr pMsg in RscHalSetupTCS"); 
    return HAL_INVALID_INPUT_PTR;
  }
  
  drvMapId = pMsg->RscSwDrvMapId;  
  
  if ((status = IS_DRV_VALID (drvMapId)) < HAL_STATUS_SUCCESS)
  {
    RPMH_LOG_1 ("\tInvalid DrvMapId, %d, in RscHalSetupTCS", drvMapId);
    status = HAL_INVALID_DRV_MAP_ID;
    return status;
  }
    
  ctxt =  rscHalGetContext();
    
  if ((ctxt->DrvConfigArray[drvMapId].NumTcs ==  0) || 
       (IS_TCS_VALID (pMsg->TcsId,ctxt->DrvConfigArray[drvMapId].NumTcs) < HAL_STATUS_SUCCESS))
  {
    RPMH_LOG_1 ("\tInvalid TCS Id, %d, in RscHalSetupTCS", pMsg->TcsId);
    return HAL_INVALID_TCS_ID;
  }

  if ((status = RscHalIsTCSIdle(drvMapId,pMsg->TcsId, &IsTCSIdle)) == HAL_STATUS_SUCCESS)
  {
    if(IsTCSIdle ==  FALSE)
    {
      //RPMH_LOG_1 ("\tTCS Id %d is not Idle\n", pMsg->TcsId);
      RPMH_LOG_1 ("\tTCS Id %d is not Idle\n", pMsg->TcsId);
      status = HAL_TCS_NOT_IDLE;
      return status;
    }
  }
  else
  {
    RPMH_LOG_0 ( "\tError in TCSIdle call in RscHalSetupTCS");
    return status;
  }

  commandInfo = pMsg->Start;
  baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    
  if (commandInfo != NULL && pMsg->TotalCommands > 0)
  {
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
        for (cntCmds =0; cntCmds < pMsg->TotalCommands; cntCmds++)
        {
          if ((status = rscHalSetupDrv0Tcs (baseAddress,pMsg->TcsId, commandInfo)) == HAL_STATUS_SUCCESS)
          {
            commandInfo++;  
          }
          else
          {
            return status;
          }
          
        }
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
        for (cntCmds =0; cntCmds < pMsg->TotalCommands; cntCmds++)
        {
          if ((status = rscHalSetupDrv1Tcs (baseAddress,pMsg->TcsId, commandInfo)) == HAL_STATUS_SUCCESS)
          {
            commandInfo++;  
          }
          else
          {
            return status;
          }
        }
        break;                    
#endif	
#ifdef DRV_2_VALID
      case DRV_ID_2:
        for (cntCmds =0; cntCmds < pMsg->TotalCommands; cntCmds++)
        {
          if ((status = rscHalSetupDrv2Tcs (baseAddress,pMsg->TcsId, commandInfo)) == HAL_STATUS_SUCCESS)
          {
            commandInfo++;  
          }
          else
          {
            return status;
          }
        }
        break;                    
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalSetupTCS",ctxt->DrvConfigArray[drvMapId].DrvId); 
        status = HAL_INVALID_DRV_ID;
        break;
    }
  }
  else
  {
    //Log Error
    status = HAL_INVALID_INPUT_PTR;
  }    

  return status;

}

/**
* Function Name:    RscHalClearTcsControl
* Description:      RscHalClearTcsControl is used to Clear given TCS
* Input:            drvMapId for respective DRV, tcsID for given TCS
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/

int32 RscHalClearTcsControl(RSCSW_DRV_MAPPING drvMapId, uint8 tcsId)
{
   uint32 baseAddress = 0;
   int32  status = HAL_STATUS_SUCCESS;
   
   struct RscHalCtxt *ctxt =  rscHalGetContext();
  
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
		RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV0_CONTROL,
                                    baseAddress,
                                    tcsId,
                                    0);
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
		RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV1_CONTROL,
                                    baseAddress,
                                    tcsId,
                                    0);
        break;

#endif
#ifdef DRV_2_VALID
      case DRV_ID_2:
		RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV2_CONTROL,
                                    baseAddress,
                                    tcsId,
                                    0);
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalClearTcsControl",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID;
        break;

    }
  }
  else
  {
    RPMH_LOG_0 ( "\tContext Not Initialized yet");
  }
  
  return status;
}

/**
* Function Name:    RscHalClearTcsCmdEnable
* Description:      RscHalClearTcsCmdEnable is used to Clear/Disable all commands on any given TCS
* Input:            drvMapId for respective DRV, tcsID for given TCS
* Return:           HAL_STATUS_SUCCESS if success ow error code 
**/


int32 RscHalClearTcsCmdEnable(RSCSW_DRV_MAPPING drvMapId, uint8 tcsId)
{
   uint32 baseAddress = 0;
   int32  status = HAL_STATUS_SUCCESS;
   
   struct RscHalCtxt *ctxt =  rscHalGetContext();
  
  if ((status = isContextInitialized(ctxt, drvMapId)) == HAL_STATUS_SUCCESS)
  {
    baseAddress = ctxt->DrvConfigArray[drvMapId].BaseAddress;
    switch (ctxt->DrvConfigArray[drvMapId].DrvId)
    {
      case DRV_ID_0:
		RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV0_CMD_ENABLE,
                                    baseAddress,
                                    tcsId,
                                    0);
        break;
#ifdef DRV_1_VALID
      case DRV_ID_1:
		RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV1_CMD_ENABLE,
                                    baseAddress,
                                    tcsId,
                                    0);
        break;

#endif
#ifdef DRV_2_VALID
      case DRV_ID_2:
		RSC_HAL_HWIO_TCS_OUT_WRITE (PLATFORM,
                                    _RSCC_TCSm_DRV2_CMD_ENABLE,
                                    baseAddress,
                                    tcsId,
                                    0);
        break;
#endif
      default:
        RPMH_LOG_1 ("\tInvalid DRVId, %d, passed to RscHalClearTcsCmdEnable",ctxt->DrvConfigArray[drvMapId].DrvId);
        status = HAL_INVALID_DRV_ID;
        break;

    }
  }
  else
  {
    RPMH_LOG_0 ( "\tContext Not Initialized yet");
  }
  
  return status;
}




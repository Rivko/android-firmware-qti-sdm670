/*! \file */

/*
===========================================================================

FILE:        hal_mdp_traffic_ctrl.c

DESCRIPTION: Manages traffic control of memory transactions between MDP 
             and VBIF that interfaces to system memory.
  
===========================================================================
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/* -----------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------- */

#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------
 * Local defines 
 * ------------------------------------------------------------------------------------ */



#define HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN            0
#define HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX            HWIO_MDP_VP_0_VIG_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_WATERMARK_BMSK
                                                                    /* width of the field is 16-bit */

#define HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MIN                  1
#define HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MAX                  HWIO_MDP_TRAFFIC_SHAPER_RD_CLIENT_0_BYTES_PER_CLOCK_BMSK
                                                                    /* width of the field is 8-bit */

/* QoS watermark level refers to the number of 128-bit (16 bytes) entries in SMP,
   hence its step size is 16 bytes.                                              */
#define HAL_MDP_WATERMARK_STEP_SIZE                                 16

/*------------------------------------------------------------------------
 * Global Data Definitions
 *------------------------------------------------------------------------ */

/* Watermark table with default values */
const uint32 gMDP_WatermarkLvlTable_1_0_0[HAL_MDP_TRFCTRL_LATENCY_MAX][HAL_MDP_QOS_REQPRI_WATERMARK_DEFAULT_REGS_COUNT] = 
  {
    // Default values for HAL_MDP_TRFCTRL_LATENCY_BEST_EFFORT
    {HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX, HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX, HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX},
    // Default values for HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME
    {HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN, HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX, HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX},
    // Default values for HAL_MDP_TRFCTRL_LATENCY_REALTIME
    {HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN, HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN, HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MAX},
    // Default values for HAL_MDP_TRFCTRL_LATENCY_ULTRA_LOW_LATENCY
    {HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN, HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN, HAL_MDP_QOS_REQPRIORITY_FIFO_WATERMARK_LEVEL_MIN}
  };
/* -----------------------------------------------------------------------
 * Static data declarations and functions
 * ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_SetClientWatermarkLvl()
*/
/*!
* \brief
*     Sets a read client's watermark levels based on client's latency need.
*
* \param [in] eLatency       - latency setting for the client; 
* \param [in] uTotalSMPBytes - total number of bytes in SMP allocated for the client;
* \param [in] uFlags               - If configured surface is MacroTiled;
* \param [in] uRegAddr       - address of watermark level-0 register;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_SetClientWatermarkLvl (
                              HAL_MDP_TrafficCtrl_LatencyType  eLatency,
                              uint32                           uTotalSMPBytes,
                              uint32                           uFlags,
                              uintPtr                          uRegAddr )
{
   HAL_MDSS_ErrorType    eStatus          = HAL_MDSS_STATUS_SUCCESS;

   if (NULL != pgsMdpHwInfo->sIpFxnTbl.WatermarkLvlConfig)
   {
     eStatus = pgsMdpHwInfo->sIpFxnTbl.WatermarkLvlConfig(eLatency, uTotalSMPBytes, uFlags, uRegAddr);
   }
   else
   {
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
   }

   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_SetTrfShaper()
*/
/*!
* \brief
*     Configures traffice shaper.
*
* \param [in] bEnTrfShaper - enable/disable the traffic shaper;
* \param [in] uBytesPerClk - Bytes per clock as required by traffic shaper; 
* \param [in] uRegAddr     - address of the traffic shaper register;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_SetTrfShaper ( 
                              bool32  bEnTrfShaper,
                              uint32  uBytesPerClk,
                              uintPtr uRegAddr )
{
   HAL_MDSS_ErrorType    eStatus      = HAL_MDSS_STATUS_SUCCESS;
   uint32                uRegValue    = 0;

   if (TRUE == bEnTrfShaper)
   {  
      uBytesPerClk = (uBytesPerClk > HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MAX) ?
                      HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MAX : uBytesPerClk;

      if (uBytesPerClk < HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MIN)
      {
         uBytesPerClk = HAL_MDP_TRAFFIC_SHAPER_BYTES_PER_CLOCK_MIN;
      }
   }
   else
   {
      uBytesPerClk = 0;
   }

   uRegValue = HWIO_OUT_FLD(uRegValue,
                            MDP_TRAFFIC_SHAPER_RD_CLIENT_0,
                            EN,
                            bEnTrfShaper);
   uRegValue = HWIO_OUT_FLD(uRegValue,
                            MDP_TRAFFIC_SHAPER_RD_CLIENT_0,
                            BYTES_PER_CLOCK,
                            uBytesPerClk);
   out_dword(uRegAddr, uRegValue);

   return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_InitClients()
*/
/*!
* \brief
*     Initializes settings of traffic control clients with consecutive IDs.
*
* \param [in] eClientId     - ID of the first client; IDs of the following  
*                             clients must be consecutive to this;
* \param [in] uNumOfClients - number of clients with consecutive IDs;
*
* \retval None
*
****************************************************************************/
static void HAL_MDP_TrafficCtrl_InitClients (
                HAL_MDP_TrafficCtrl_ClientIDType   eClientId,
                uint32                             uNumOfClients )
{
  HAL_MDP_PerfCtrlParamsType         sSetting[HAL_MDP_TRFCTRL_CLIENT_MAX]  = {{0}};
  HAL_MDP_PerfCtrlParamsType        *psSetting                             = &sSetting[0];
  HAL_MDP_FifoWatermark_ConfigType   sFifoWatermarkSetting                 = {0};
  HAL_MDP_TraficSharper_ConfigType   sTrfShaperSetting                     = {0};
  uint32                             uIndex                                = 0;
  
  if (0 != uNumOfClients)   /* check this condition to avoid unnecessary function calls */
  {
  
     // Set traffic control for the layer
     sTrfShaperSetting.bEnableTrfShaper   = FALSE;
     sTrfShaperSetting.uBytesPerClk       = 0;
  
     // Fifo Watermark setting, set default for Non Realtime client
     sFifoWatermarkSetting.uTotalSMPBytes = 0; // configure default watemark
     
     for (uIndex = 0; uIndex < uNumOfClients; uIndex++)
     {
        psSetting->eClientId               = eClientId;
        psSetting->eReadClientLatency      = (eClientId <= HAL_MDP_TRFCTRL_CLIENT_LAYER4_CURSOR) ?
                                                           HAL_MDP_TRFCTRL_LATENCY_REALTIME : HAL_MDP_TRFCTRL_LATENCY_BEST_EFFORT;
        psSetting->psTrfShaperSetting      = &sTrfShaperSetting;
        psSetting->psFifoWatermarkSetting  = &sFifoWatermarkSetting;
  
        
        eClientId++;
        psSetting++;
     }
  
     if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_TrafficCtrl_Setup(&sSetting[0], uNumOfClients, 0, 0))
     {
        HAL_MDP_ASSERT();
     }
  }

}


/* -------------------------------------------------------------------------------------
 * Public functions
 * ------------------------------------------------------------------------------------ */


/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_Setup()
*/
/*!
* \brief
*     Configures the traffic control of memory transactions between MDP and VBIF
*     that interfaces to system memory.
*
* \param [in] psTrfCtrlSetting - traffic control settings;
* \param [in] uNumOfSettings   - number of settings;
* \param [in] uMdpCoreClkFreq  - MDP core clock working frequency in Hz;
* \param [in] uFlags           - reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_Setup (HAL_MDP_PerfCtrlParamsType *psPerfCtrlSetting,
                                              uint32                      uNumOfSettings,
                                              uint32                      uMdpCoreClkFreq,
                                              uint32                      uFlags )
  {
     HAL_MDSS_ErrorType          eStatus     = HAL_MDSS_STATUS_SUCCESS;
     HAL_MDP_PerfCtrlParamsType *psSetting   = psPerfCtrlSetting; 
     uint32                      uIndex      = 0;
     uintPtr                     uRegAddr    = 0;
     uintPtr                     uRegAddrInc = 0;
  
     for(uIndex = 0; uIndex < uNumOfSettings; uIndex++)
     {
        switch (psSetting->eClientId)
        {
        case HAL_MDP_TRFCTRL_CLIENT_VIG0:
        case HAL_MDP_TRFCTRL_CLIENT_VIG1:
        case HAL_MDP_TRFCTRL_CLIENT_VIG2:
        case HAL_MDP_TRFCTRL_CLIENT_VIG3:
           /* sets watermark levels */
           uRegAddrInc = HWIO_MDP_VP_0_VIG_1_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR - HWIO_MDP_VP_0_VIG_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR;
           uRegAddr    = HWIO_MDP_VP_0_VIG_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR +
                         (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_VIG0) * uRegAddrInc;
           if(NULL != psSetting->psFifoWatermarkSetting)
           {
              eStatus = HAL_MDP_TrafficCtrl_SetClientWatermarkLvl(psSetting->eReadClientLatency, psSetting->psFifoWatermarkSetting->uTotalSMPBytes, psSetting->uSrcSurfFlags, uRegAddr);
           }
  
           if (HAL_MDSS_STATUS_SUCCESS == eStatus)
           {
             if(NULL!= psSetting->psTrfShaperSetting)
             {
               uRegAddrInc =  HWIO_MDP_VP_0_VIG_1_SSPP_TRAFFIC_SHAPER_ADDR  - HWIO_MDP_VP_0_VIG_0_SSPP_TRAFFIC_SHAPER_ADDR;
               uRegAddr    =  HWIO_MDP_VP_0_VIG_0_SSPP_TRAFFIC_SHAPER_ADDR  + (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_VIG0) * uRegAddrInc;
  
               HAL_MDP_TrafficCtrl_SetTrfShaper(psSetting->psTrfShaperSetting->bEnableTrfShaper, psSetting->psTrfShaperSetting->uBytesPerClk, uRegAddr);
             }
           }
  
           break;
  
        case HAL_MDP_TRFCTRL_CLIENT_RGB0:
        case HAL_MDP_TRFCTRL_CLIENT_RGB1:
        case HAL_MDP_TRFCTRL_CLIENT_RGB2:
        case HAL_MDP_TRFCTRL_CLIENT_RGB3:
           /* sets watermark levels */
           uRegAddrInc = HWIO_MDP_VP_0_RGB_1_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR - HWIO_MDP_VP_0_RGB_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR;
           uRegAddr    = HWIO_MDP_VP_0_RGB_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR + 
                         (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_RGB0) * uRegAddrInc;
           if(NULL != psSetting->psFifoWatermarkSetting)
           {
              eStatus = HAL_MDP_TrafficCtrl_SetClientWatermarkLvl(psSetting->eReadClientLatency, psSetting->psFifoWatermarkSetting->uTotalSMPBytes, psSetting->uSrcSurfFlags, uRegAddr);
           }
           
           if (HAL_MDSS_STATUS_SUCCESS == eStatus)
           {
             if(NULL!= psSetting->psTrfShaperSetting)
             { 
               uRegAddrInc =  HWIO_MDP_VP_0_RGB_1_SSPP_TRAFFIC_SHAPER_ADDR  - HWIO_MDP_VP_0_RGB_0_SSPP_TRAFFIC_SHAPER_ADDR;
               uRegAddr    =  HWIO_MDP_VP_0_RGB_0_SSPP_TRAFFIC_SHAPER_ADDR  + (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_RGB0) * uRegAddrInc;
  
               HAL_MDP_TrafficCtrl_SetTrfShaper(psSetting->psTrfShaperSetting->bEnableTrfShaper, psSetting->psTrfShaperSetting->uBytesPerClk, uRegAddr);
             }
           }
  
           break;
  
        case HAL_MDP_TRFCTRL_CLIENT_DMA0:
        case HAL_MDP_TRFCTRL_CLIENT_DMA1:
           /* sets watermark levels */
           uRegAddrInc = HWIO_MDP_VP_0_DMA_1_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR - HWIO_MDP_VP_0_DMA_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR;
           uRegAddr    = HWIO_MDP_VP_0_DMA_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0_ADDR +
                         (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_DMA0) * uRegAddrInc;
           if(NULL != psSetting->psFifoWatermarkSetting)
           {
              eStatus = HAL_MDP_TrafficCtrl_SetClientWatermarkLvl(psSetting->eReadClientLatency, psSetting->psFifoWatermarkSetting->uTotalSMPBytes, psSetting->uSrcSurfFlags, uRegAddr);
           }
           
           if (HAL_MDSS_STATUS_SUCCESS == eStatus)
           {
              if(NULL!= psSetting->psTrfShaperSetting)
              { 
                uRegAddrInc =  HWIO_MDP_VP_0_DMA_1_SSPP_TRAFFIC_SHAPER_ADDR  - HWIO_MDP_VP_0_DMA_0_SSPP_TRAFFIC_SHAPER_ADDR;
                uRegAddr    =  HWIO_MDP_VP_0_DMA_0_SSPP_TRAFFIC_SHAPER_ADDR  + (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_DMA0) * uRegAddrInc;
  
                HAL_MDP_TrafficCtrl_SetTrfShaper(psSetting->psTrfShaperSetting->bEnableTrfShaper, psSetting->psTrfShaperSetting->uBytesPerClk, uRegAddr);
              }
           }
  
           break;
  
        case HAL_MDP_TRFCTRL_CLIENT_WB0:
        case HAL_MDP_TRFCTRL_CLIENT_WB1:
        case HAL_MDP_TRFCTRL_CLIENT_WB2:
           /* configures the traffice shaper for the client */
           uRegAddrInc = HWIO_MDP_WB_1_TRAFFIC_SHAPER_WR_CLIENT_ADDR - HWIO_MDP_WB_0_TRAFFIC_SHAPER_WR_CLIENT_ADDR;                
           uRegAddr    = HWIO_MDP_WB_0_TRAFFIC_SHAPER_WR_CLIENT_ADDR + (psSetting->eClientId - HAL_MDP_TRFCTRL_CLIENT_WB0) * uRegAddrInc;
  
           if(NULL!= psSetting->psTrfShaperSetting)
           {
             HAL_MDP_TrafficCtrl_SetTrfShaper(psSetting->psTrfShaperSetting->bEnableTrfShaper, psSetting->psTrfShaperSetting->uBytesPerClk, uRegAddr);
           }
  
           break;
  
        case HAL_MDP_TRFCTRL_CLIENT_NONE:
        case HAL_MDP_TRFCTRL_CLIENT_MAX:
        case HAL_MDP_TRFCTRL_CLIENT_FORCE_32BIT:
        default:
           eStatus = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
           break;         
        }
  
        
        if (HAL_MDSS_STATUS_SUCCESS == eStatus)
        {
           psSetting++;
        }
        else
        {
           break;          /* Exit on ERROR! */
        }
     }
  
     return eStatus;

}

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_Init()
*/
/*!
* \brief
*     Initializes QoS priority re-mapping and watermark level settings for  
*     read clients, the output of QoS priority level for write paths, and 
*     the traffic shaper. 
*
* \param [in] uFlags - reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_Init(HAL_MDP_InitConfigType *pMdpInitConfig, uint32  uFlags)
{
  HAL_MDSS_ErrorType               eStatus     = HAL_MDSS_STATUS_SUCCESS;
  uint32                           uIndex      = 0;
  uintPtr                          uRegAddr    = 0;
  uint32                           uRegValue   = 0;
  HAL_MDP_GpuMemoryBankConfigType  sGpuMemoryBankConfig;
  uint32                           uGpuHighestMemoryBankBit = 0;
  uint32                           uGpuMacroTileMode        = 0;
  
  /* get the GPU highest memory bank bit for DDR configuration */
  if (NULL != pMdpInitConfig)
  {
      sGpuMemoryBankConfig.uGpuHighestMemoryBankBit = pMdpInitConfig->uGpuHighestMemoryBankBit;
      uGpuHighestMemoryBankBit = HAL_MDP_Get_GpuMemoryBankBitConfig(&sGpuMemoryBankConfig, 0x0);
      uGpuMacroTileMode        = pMdpInitConfig->uGpuMacroTileMode;
  }
  
  /* init QoS priority re-mapping for all read clients */
  /* For each read client, 3 watermark settings are used by HW to check the status of buffer level 
     in SMP and then generate QoS priority signals. By using the above API, HAL_MDP_TrafficCtrl_Setup,
     to program watermark registers, 1 out of 4 QoS priority levels can be dynamically assigned for 
     each read client. Because of this, the single buffered registers of QoS re-mapper are set once 
     here during initialization with a fixed mapping. */
  out_dword(HWIO_MDP_QOS_REMAPPER_CLASS_0_ADDR, pgsMdpHwInfo->uQosRemapperRealTimeClients);
  out_dword(HWIO_MDP_QOS_REMAPPER_CLASS_1_ADDR, pgsMdpHwInfo->uQosRemapperNonRealTimeClients);
  
  /* init QoS output level for all write paths */
  /* For each write back path, QoS priority is purely controlled by a field of a single buffered
     register, dynamic altering is not allowed. Hence, they are all set here once at the 
     initialization stage. */
  for (uIndex = 0; uIndex < HAL_MDP_GET_DATA_PATHS_COUNT(); uIndex++)
  {
     uRegAddr  = HWIO_MDP_WB_0_DST_WRITE_CONFIG_ADDR + uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_0 + uIndex];
  
     uRegValue = in_dword(uRegAddr);
     uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_WRITE_CONFIG, HIGHEST_BANK_BIT, uGpuHighestMemoryBankBit);
     uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_WRITE_CONFIG, ADDR_GEN_MODE,    uGpuMacroTileMode);
     if (NULL != pgsMdpHwInfo->pQosWBPathsPriority)
     {
        uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_WRITE_CONFIG, PRIORITY_LEVEL, *(pgsMdpHwInfo->pQosWBPathsPriority + uIndex));
        uRegValue = HWIO_OUT_FLD(uRegValue, MDP_WB_0_DST_WRITE_CONFIG, REQ_PRIORITY,   *(pgsMdpHwInfo->pQosWBPathsPriority + uIndex));
     }
     out_dword(uRegAddr, uRegValue);
  }
  
  /* init all watermark registers and traffic shapers */
  HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_VIG0, pgsMdpHwInfo->sHwInfo.uNumOfVIGLayers);
  HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_RGB0, pgsMdpHwInfo->sHwInfo.uNumOfRGBLayers);
  HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_DMA0, pgsMdpHwInfo->sHwInfo.uNumOfDMALayers);
  HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_LAYER0_CURSOR, pgsMdpHwInfo->sHwInfo.uNumOfCursorLayers);
  HAL_MDP_TrafficCtrl_InitClients(HAL_MDP_TRFCTRL_CLIENT_WB0, pgsMdpHwInfo->sHwInfo.uNumOfBltEngines);


   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_SetClientWatermarkLvl_4_0_0()
*/
/*!
* \brief
*     Sets a read client's watermark levels based on client's latency need.
*
* \param [in] eLatency             - latency setting for the client; 
* \param [in] uTotalSMPBytes       - total number of bytes in SMP allocated for the client;
* \param [in] uFlags               - If configured surface is MacroTiled;
* \param [in] uRegAddr             - address of watermark level-0 register;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_SetClientWatermarkLvl_4_0_0 (
                              HAL_MDP_TrafficCtrl_LatencyType  eLatency,
                              uint32                           uTotalSMPBytes,
                              uint32                           uFlags,
                              uintPtr                          uRegAddr )
{
   HAL_MDSS_ErrorType    eStatus          = HAL_MDSS_STATUS_SUCCESS;
   uint32                uIndex           = 0;
   uint32                uRegValue        = 0;
   uint32                uWatermarkLvl[]  = {0, 0, 0};

   /* in the current HW, 3 watermark registers are used by HW to check the status
    * of buffer level in SMP and then generate 1 of 4 QoS priority levels.  
    *
    * if the number of watermark registers in HW is changed and not 3, the code
    * must be revisited in order to accommodate the new configurations in HW.
    */
   if (HAL_MDP_QOS_REQPRI_WATERMARK_DEFAULT_REGS_COUNT == pgsMdpHwInfo->uQosReqPriWatermarkRegsCount)
   {
      if (0 != uTotalSMPBytes)
      {       
         /* uTotalSMPBytes should be aligned to HAL_MDP_WATERMARK_STEP_SIZE */
         uTotalSMPBytes = HAL_ALIGN_DATA(uTotalSMPBytes, HAL_MDP_WATERMARK_STEP_SIZE);

         /* set watermark levels to 1/4, 1/2 and 3/4 of the total SMP space allocated */
         uWatermarkLvl[0] = (uint32)(uTotalSMPBytes/(HAL_MDP_WATERMARK_STEP_SIZE*4));
         uWatermarkLvl[1] = (uint32)((uTotalSMPBytes*2)/(HAL_MDP_WATERMARK_STEP_SIZE*4));
         uWatermarkLvl[2] = (uint32)((uTotalSMPBytes*3)/(HAL_MDP_WATERMARK_STEP_SIZE*4));
      }
      else
      {
        if(eLatency < HAL_MDP_TRFCTRL_LATENCY_MAX)
        {
          uWatermarkLvl[0] = gMDP_WatermarkLvlTable_1_0_0[eLatency][0];
          uWatermarkLvl[1] = gMDP_WatermarkLvlTable_1_0_0[eLatency][1];
          uWatermarkLvl[2] = gMDP_WatermarkLvlTable_1_0_0[eLatency][2];
        }
        else
        {
          eStatus = HAL_MDSS_STATUS_FAILED_INVALID_INPUT_PARAMETER;
        }
      }

      if (HAL_MDSS_STATUS_SUCCESS == eStatus)
      {
         for (uIndex = 0; uIndex < pgsMdpHwInfo->uQosReqPriWatermarkRegsCount; uIndex++)
         {
            uRegValue = 0;
            uRegValue = HWIO_OUT_FLD(uRegValue,
                                     MDP_VP_0_VIG_0_SSPP_REQPRIORITY_FIFO_WATERMARK_0,
                                     WATERMARK,
                                     uWatermarkLvl[uIndex]);
            out_dword(uRegAddr, uRegValue);
            uRegAddr += 4;      /* move to next register address */
         }
      }
   }
   else
   {
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
   }

   return eStatus;
}

#ifdef __cplusplus
}
#endif

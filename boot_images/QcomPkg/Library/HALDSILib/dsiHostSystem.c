/*=============================================================================

  File: dsiHostSystem.c
  
     Copyright (c) 2010-2018 Qualcomm Technologies, Inc.
     All Rights Reserved.
     Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "dsiHostShared.h"
#include "dsiHostSystem.h"
#include "HALdsi_Phy.h"
#include "MDPLib_i.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

#ifdef __cplusplus
extern "C" {
#endif

/* DSI Clock Divider Entry
 * 
 * 
 * DSI Programming ensures all clocks out from the DSI PLL are at BitClock frequency, therefore all MMSS CC calculations can be based
 * on the DSI BitClock Frequency.
 *
 *      BitClock = (8 * total_horizontal_pixels * total_vertical_lines * refresh_rate * bytes_per_pixel) / number_of_dsi_lanes
 *
 * Remaining clocks (DSI Pixel, DSI Byte) are all derived from the bitclock
 *
 *      DSIByte  = BitClock/8
 * 
 *      DSIPixel = (DSIByte * number_of_dsi_lanes) / bytes_per_pixel
 *               = [(BitClock/8) * number_of_dsi_lanes)] / bytes_per_pxiel    ; Using simple substitution of DSIByte
 *               = (BitClock * number_of_dsi_lanes) / (8 * bytes_per_pixel)   ; Rearranging equation.
 *
 *
 * Expanding DSIPixel (Which is based on the bytes_per_pixel and number_of_dsi_lanes)
 *
 * For 24bpp (or 3 bytes per pixel)
 *
 * Lanes | Equation
 * --------------------
 *   1   | BitClock / 24
 *   2   | BitClock / 12
 *   3   | BitClock / 8
 *   4   | BitClock / 6
 *
 *
 * For 18bpp (or 2.25 bytes per pixel)
 *
 * Lanes | Equation
 * --------------------
 *   1   | BitClock / 18
 *   2   | BitClock / 9
 *   3   | BitClock / 6
 *   4   | BitClock*4 / 18
 * 
 *
 * For 16bpp (or 2 bytes per pixel)
 *
 * Lanes | Equation
 * --------------------
 *   1   | BitClock / 16
 *   2   | BitClock / 8
 *   3   | (BitClock*3) / 16
 *   4   | BitClock / 4
 *
 *
 */

/* 24bpp Table
 */
static const DSI_ClockDividerEntry    asDSIDividers24bpp[] = 
{
  {{0, 0, 0, 0, 0},   // 0 Lanes (not supported)
   {0, 0, 0, 0, 0},   // 0 Lanes (not supported)
   {0, 0, 0, 0, 0}},  // 0 Lanes (not supported)

  {{0, 8,  0, 0,   0},  // 1 Lanes Byte : BitClock / 8
   {0, 0,  1, 24, 12},  // 1 Lanes Pixel: BitClock / 24 ; Predividers are only up to 16, therefore we switch to MND
   {0, 8,  0, 0,   0}}, // 1 Lanes Core : BitClock / 8
  
  {{0, 8,  0, 0, 0},  // 2 Lanes Byte : BitClock / 8
   {0, 12, 0, 0, 0},  // 2 Lanes Pixel: BitClock / 12
   {0, 4,  0, 0, 0}}, // 2 Lanes Core : BitClock / 4

  {{0, 8, 0, 0, 0},   // 3 Lanes Byte : BitClock / 8
   {0, 8, 0, 0, 0},   // 3 Lanes Pixel: BitClock / 8
   {0, 0, 3, 8, 4}},  // 3 Lanes Corel: (BitClock*3) / 8
  
  {{0, 8, 0, 0, 0},   // 4 Lanes Byte : BitClock / 8
   {0, 6, 0, 0, 0},   // 4 Lanes Pixel: BitClock / 6
   {0, 2, 0, 0, 0}}   // 4 Lanes Core : BitClock / 2
};

/* 18bpp Table
 */
static const DSI_ClockDividerEntry    asDSIDividers18bpp[] = 
{
  {{0, 0, 0, 0, 0},   // 0 Lanes (not supported)
   {0, 0, 0, 0, 0},   // 0 Lanes (not supported)
   {0, 0, 0, 0, 0}},  // 0 Lanes (not supported)

  {{0, 8,  0, 0,  0},  // 1 Lanes Byte : BitClock / 8
   {0, 0,  1, 18, 9},  // 1 Lanes Pixel: BitClock / 18 ; Predividers are only up to 16, therefore we switch to MND
   {0, 8,  0, 0,  0}}, // 1 Lanes Core : BitClock / 8
  
  {{0, 8,  0, 0, 0},  // 2 Lanes Byte : BitClock / 8
   {0, 9,  0, 0, 0},  // 2 Lanes Pixel: BitClock / 9
   {0, 4,  0, 0, 0}}, // 2 Lanes Core : BitClock / 4

  {{0, 8, 0, 0, 0},   // 3 Lanes Byte : BitClock / 8
   {0, 6, 0, 0, 0},   // 3 Lanes Pixel: BitClock / 6
   {0, 0, 3, 8, 4}},  // 3 Lanes Corel: (BitClock*3) / 8
  
  {{0, 8, 0, 0, 0},   // 4 Lanes Byte : BitClock / 8
   {0, 0, 4,18, 9},   // 4 Lanes Pixel: (BitClock*4) / 18
   {0, 2, 0, 0, 0}}   // 4 Lanes Core : BitClock / 2
};


/* 16bpp Table
 */
static const DSI_ClockDividerEntry    asDSIDividers16bpp[] = 
{
  {{0, 0, 0, 0, 0},  // 0 Lanes (not supported)
   {0, 0, 0, 0, 0},  // 0 Lanes (not supported)
   {0, 0, 0, 0, 0}}, // 0 Lanes (not supported)

  {{0, 8,  0, 0, 0}, // 1 Lanes Byte : BitClock / 8 
   {0, 16, 0, 0, 0}, // 1 Lanes Pixel: BitClock / 16
   {0, 8,  0, 0, 0}},// 1 Lanes Core : BitClock / 8
  
  {{0, 8,  0, 0, 0}, // 2 Lanes Byte : BitClock / 8 
   {0, 8,  0, 0, 0}, // 2 Lanes Pixel: BitClock / 8
   {0, 4,  0, 0, 0}},// 2 Lanes Core : BitClock / 4

  {{0, 8, 0, 0, 0},  // 3 Lanes Byte : BitClock / 8
   {0, 0, 3,16, 8},  // 3 Lanes Pixel: (BitClock*3) / 16
   {0, 0, 3, 8, 4}}, // 3 Lanes Corel: (BitClock*3) / 8
  
  {{0, 8, 0, 0, 0},  // 4 Lanes Byte : BitClock / 8
   {0, 4, 0, 0, 0},  // 4 Lanes Pixel: BitClock / 4
   {0, 2, 0, 0, 0}}  // 4 Lanes Core : BitClock / 2
};

/*------------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------------*/

/* Clear memory
*/
void DSI_OSAL_MemZero(void *pBuffer, uint32 uSizeInBytes)
{
  MDP_OSAL_MEMZERO(pBuffer, uSizeInBytes);
}


/* Copy memory
*/
void DSI_OSAL_MemCpy(void *pDst, void *pSrc, uint32 uSizeInBytes)
{
  MDP_OSAL_MEMCPY(pDst, pSrc, uSizeInBytes);
}


/* Sleep/halt processor for amount of time in milliseconds
*/
void DSI_OSAL_SleepMs(uint32 ms)
{
  MDP_OSAL_DELAYMS(ms);
}


/* Sleep/halt processor for amount of time in milliseconds
*/
void DSI_OSAL_SleepUs(uint32 us)
{
  MDP_OSAL_DELAYUS(us);
}

//-------------------------------------------------------------------------------------------------
//  dsiSystem_FrequencyCalibration
//
//  @brief
//      dynamic refresh calibration
//
//  @params
//      [IN] hDevice
//          DSI device handle.
//      [IN] psDisplayConfig
//          pointer to display config
//
//  @return
//      None.
//-------------------------------------------------------------------------------------------------
DSI_StatusType dsiSystem_FrequencyCalibration(DSI_HandleType         hDevice,
                                         DSI_DisplayConfigType  *psDisplayConfig)
{
    DSI_StatusType            eStatus          = DSI_STATUS_SUCCESS;
    DSI_Device_IDType         eDeviceId        = dsiShared_DeviceHandle2Id(hDevice);
    uint32                    uBitsPerPixel    = 0;
    uint32                    uNumLanes        = psDisplayConfig->sDisplayInfo.uNumLanes;
    HAL_DSI_PhyConfigType     sDsiPhyConfig;

    /* Calculate the bits per pixel */ 
    uBitsPerPixel = dsiShared_GetBitsPerPixel(psDisplayConfig->sDisplayInfo.eColorFormat);

    /* Setup the PhyStructure */
    DSI_OSAL_MemZero(&sDsiPhyConfig, sizeof(HAL_DSI_PhyConfigType));

    sDsiPhyConfig.eDeviceId          = eDeviceId;
    sDsiPhyConfig.uBitsPerPixel      = uBitsPerPixel;
    sDsiPhyConfig.uNumOfDataLanes    = uNumLanes;
    sDsiPhyConfig.uConfigFlags       = 0;
    sDsiPhyConfig.uDesiredBitClkFreq = dsiShared_CalculateDesiredBitClk(&psDisplayConfig->sDisplayInfo, uBitsPerPixel);  /* desired DSI PLL bit clk freq in Hz */
    sDsiPhyConfig.uEscapeFreq        = psDisplayConfig->sDisplayInfo.uEscapeFreq;
    sDsiPhyConfig.pPreDefinedTimings = &psDisplayConfig->sTimingOverride;
    
    //sDsiPhyConfig.bCPhyMode          = psDisplayConfig->sDisplayInfo.bCPhyMode;
    
    if (DSI_PLL_CONFIG_SPLIT_SOURCE == psDisplayConfig->ePLLSourceType)
    {
      sDsiPhyConfig.ePLLConfigSource = HAL_DSI_PLL_CONFIG_SPLIT_SOURCE;
    }
    else
    {
      sDsiPhyConfig.ePLLConfigSource = HAL_DSI_PLL_CONFIG_INDEPENDENT_SOURCE;
    }
    
    eStatus = dsiShared_FrequencyCalibration(&sDsiPhyConfig, psDisplayConfig);

    return eStatus;
}

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiSystem_ConfigDevicePll()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiSystem_ConfigDevicePll(DSI_HandleType         hDevice,
                                         DSI_DisplayConfigType  *psDisplayConfig)
{
  DSI_StatusType            eStatus          = DSI_STATUS_SUCCESS;
  DSI_Device_IDType         eDeviceId        = dsiShared_DeviceHandle2Id(hDevice);
  DSI_ClockDividerEntry    *pDividerEntry    = NULL;
  uint32                    uBitsPerPixel    = 0;
  MDP_ExtClockResourceList *pExtClockResList = MDPGetExternalClockList();  

  /* Calculate the bits per pixel */ 
  uBitsPerPixel = dsiShared_GetBitsPerPixel(psDisplayConfig->sDisplayInfo.eColorFormat);
    
  // Determine which divider table to use
  switch (uBitsPerPixel)
  {
    case 16:
        pDividerEntry = (DSI_ClockDividerEntry*)&asDSIDividers16bpp;
        break;
    case 18:
        pDividerEntry = (DSI_ClockDividerEntry*)&asDSIDividers18bpp;
        break;
    case 24:
        pDividerEntry = (DSI_ClockDividerEntry*)&asDSIDividers24bpp;
        break;
    default:
        pDividerEntry = NULL;
        break;
  }

  // Ensure that we have a dividers table for this configuration and there is an external clock list
  if ((NULL == pDividerEntry) &&
      (NULL == pExtClockResList))
  {
    eStatus = DSI_STATUS_FAILED_NOT_SUPPORTED;
  }
  else
  {
    /* Calculate clock configuration */
    HAL_DSI_PhyConfigType            sDsiPhyConfig;
    HAL_DSI_PhyConfigInfoType        sDsiPhyConfigInfo;
    MDPClockTypes                    eDSIClockId;
    MDPExternalClockEntry           *pDSIExtClock        = NULL;
    uint32                           uNumLanes           = psDisplayConfig->sDisplayInfo.uNumLanes;

    /* Setup the PhyStructure */
    DSI_OSAL_MemZero(&sDsiPhyConfig, sizeof(HAL_DSI_PhyConfigType));
    DSI_OSAL_MemZero(&sDsiPhyConfigInfo, sizeof(HAL_DSI_PhyConfigInfoType));

    /* Configure DSI PLL, PLL should be enabled prior to configuring MND to ensure a glitch free transition.
    *
    * HAL_DSI_PhySetup() returns PClk MND divider ratio for Clock Control block (CC) in the form of numerator and denominator;
    * also returned are frequencies of PLL output clocks, which can be used by upper layer to decided MND values.
    */
    sDsiPhyConfig.eDeviceId          = eDeviceId;
    sDsiPhyConfig.uBitsPerPixel      = uBitsPerPixel;
    sDsiPhyConfig.uNumOfDataLanes    = uNumLanes;
    sDsiPhyConfig.uConfigFlags       = 0;
    sDsiPhyConfig.uDesiredBitClkFreq = dsiShared_CalculateDesiredBitClk(&psDisplayConfig->sDisplayInfo, uBitsPerPixel);  /* desired DSI PLL bit clk freq in Hz */
    sDsiPhyConfig.uEscapeFreq        = psDisplayConfig->sDisplayInfo.uEscapeFreq;
    sDsiPhyConfig.pPreDefinedTimings = &psDisplayConfig->sTimingOverride;
    sDsiPhyConfig.bCPhyMode          = psDisplayConfig->sDisplayInfo.bCPhyMode;

    if (DSI_PLL_CONFIG_SPLIT_SOURCE == psDisplayConfig->ePLLSourceType)
    {
      sDsiPhyConfig.ePLLConfigSource = HAL_DSI_PLL_CONFIG_SPLIT_SOURCE;
    }
    else
    {
      sDsiPhyConfig.ePLLConfigSource = HAL_DSI_PLL_CONFIG_INDEPENDENT_SOURCE;
    }

    if (HAL_MDSS_STATUS_SUCCESS != HAL_DSI_PhyPllSetup(&sDsiPhyConfig, &sDsiPhyConfigInfo))
    {
      DSI_Log_Message(DSI_LOGLEVEL_ERROR, "DSIDriver: DSI%d PLL cannot be locked (Freq:%dHz)!\n", eDeviceId, sDsiPhyConfig.uDesiredBitClkFreq);
      eStatus = DSI_STATUS_FAILED_RESOURCE_NOT_READY;
    }
    else
    {
      // If the PLL does not lock the clock driver will wait indefinitely to change clock sources on the new source.
      // Only execute this path if the PLL has locked.
      HAL_DSI_TimingSettingType        sDsiTimingParams;

      /* Setup DSI Phy timing */
      sDsiTimingParams.eDeviceId          = eDeviceId;
      sDsiTimingParams.uBitclock          = sDsiPhyConfigInfo.uBitClkFreq;
      sDsiTimingParams.uEscapeFreq        = psDisplayConfig->sDisplayInfo.uEscapeFreq;
      sDsiTimingParams.pPreDefinedTimings = &psDisplayConfig->sTimingOverride;
      sDsiTimingParams.bCPhyMode          = psDisplayConfig->sDisplayInfo.bCPhyMode;
      if (HAL_MDSS_STATUS_SUCCESS != HAL_DSI_PhySetupTimingParams(&sDsiTimingParams))
      {
        DSI_Log_Message(DSI_LOGLEVEL_WARN, "DSI Phy timing setup failed.\n");
      }

      // Select the correct DSI core
      if (DSI_DeviceID_0 == eDeviceId)
      {
        pDSIExtClock = (MDPExternalClockEntry*)pExtClockResList->pDSI0ExtClocks;
        eDSIClockId  = MDP_CLOCKTYPE_DSI0;
      }
      else
      {
        if (DSI_PLL_CONFIG_SPLIT_SOURCE == psDisplayConfig->ePLLSourceType)
        {
          pDSIExtClock = (MDPExternalClockEntry*)pExtClockResList->pSharedSourceExtClocks;
        }
        else
        {
          pDSIExtClock = (MDPExternalClockEntry*)pExtClockResList->pDSI1ExtClocks;
        }
        eDSIClockId  = MDP_CLOCKTYPE_DSI1;
      }

      /* Setup clock Dividers for mdss_pclk#_clk */
      if (sDsiPhyConfigInfo.uPClkDivNumerator != sDsiPhyConfigInfo.uPClkDivDenominator)
      {
        pDSIExtClock[1].uClkPLL_M  = sDsiPhyConfigInfo.uPClkDivNumerator;
        pDSIExtClock[1].uClkPLL_N  = sDsiPhyConfigInfo.uPClkDivDenominator;
        pDSIExtClock[1].uClkPLL_2D = sDsiPhyConfigInfo.uPClkDivDenominator*2; 
      }

      /* Setup clock dividers for mdss_esc#_clk, keep default of CXO if we don't want a divider */
      if (psDisplayConfig->sDisplayInfo.uEscapeDivisor > 0)
      {
        // Source from DSI PLL0  and setup the divider
        //By default it is sourced from primary source (XO). for escape clock if divisor is greater than 0 then source it from byte clock.
        //Byte clock index may be different for different platforms so that index must be filled properly in platform file by referring to MSM SWI.
        //As clk source is used so assign the secondary source value to the clk source here.
        pDSIExtClock[0].uClkSource = pDSIExtClock[0].uClkSecondarySource; 
        pDSIExtClock[0].nClkDiv    = psDisplayConfig->sDisplayInfo.uEscapeDivisor;
      }

      /* Setup MMSS_MDSS_BYTEx_INTF_DIV clk source divider */
      if (sDsiTimingParams.bCPhyMode)
      {
        // byte_intf_div clk divided by 1 for CPhy
        pDSIExtClock[3].nSourceDiv    = 1; /* divided by 1 */

      }
      else
      {
        // byte_intf_div clk divided by 2 for DPhy
        pDSIExtClock[3].nSourceDiv    = 2; /* divided by 2 */

      }

      if (MDP_STATUS_OK != MDPSetupClocks(eDSIClockId, pDSIExtClock))
      {
        DSI_Log_Message(DSI_LOGLEVEL_ERROR, "DSIDriver: DSI%d clock source cannot be configured.\n", eDeviceId);
        eStatus = DSI_STATUS_FAILED_RESOURCE_NOT_READY;
      }
    }
  }

  return eStatus;
}

//-------------------------------------------------------------------------------------------------
//  dsiSystem_DisableClock
//
//  @brief
//      Vote for DSI clock off.
//
//  @params
//      [IN] hDevice
//          DSI device handle.
//
//  @return
//      None.
//-------------------------------------------------------------------------------------------------
//
DSI_StatusType dsiSystem_DisableClock(DSI_HandleType          hDevice)
{
  DSI_StatusType         eStatus        = DSI_STATUS_SUCCESS;
  DSI_Device_IDType      eDeviceID      = dsiShared_DeviceHandle2Id(hDevice);

  if( MDP_STATUS_OK != MDPDisableClocks((DSI_DeviceID_0 == eDeviceID)?MDP_CLOCKTYPE_DSI0: MDP_CLOCKTYPE_DSI1))
  {
    DSI_Log_Message(DSI_LOGLEVEL_WARN, "DSIDriver: DSI%d clocks cannot be disabled.\n" );
    eStatus = DSI_STATUS_FAILED_RESOURCE_NOT_READY;
  }

  return eStatus;
}

#ifdef __cplusplus
}
#endif

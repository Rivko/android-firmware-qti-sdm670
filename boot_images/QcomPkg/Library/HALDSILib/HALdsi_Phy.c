/*=============================================================================

  File: HALdsi_Phy.c
  

  Copyright (c) 2010-2016, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif 

#include "HALdsi.h"
#include "HALdsi_Phy.h"
#include "dsiHostSystem.h"

/* -----------------------------------------------------------------------
** Local Data Types
** ----------------------------------------------------------------------- */

HAL_DSI_PhyFunctionTable    gDsiPhyFxnTbl;

/* -----------------------------------------------------------------------
** Local functions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Public functions
** ----------------------------------------------------------------------- */

/****************************************************************************
 *
 ** FUNCTION: HAL_DSI_PhyFxnsInit()
 */
/*!
 * \brief
 *     Initialize DSI function table.
 *
 * \retval None
 *
 ****************************************************************************/
void HAL_DSI_PhyFxnsInit(void)
{
    HAL_DSI_VersionType           sDSIVersionInfo;

    HAL_DSI_ReadCoreVersionInfo(&sDSIVersionInfo);


    /* Napali */
    if ((0x02 == sDSIVersionInfo.uMajorVersion) &&
        (0x02 == sDSIVersionInfo.uMinorVersion))
    {
        gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize     = HAL_DSI_2_0_0_PhyPllInitialize;
        gDsiPhyFxnTbl.HAL_DSI_PhyDisable           = HAL_DSI_2_0_0_PhyDisable;
        gDsiPhyFxnTbl.HAL_DSI_Phy_PllEnable        = HAL_DSI_2_0_0_PhyPllEnable;
        gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl      = HAL_DSI_2_0_0_PhyPllPowerCtrl;
        gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup          = HAL_DSI_2_0_0_PhyPllSetup;
        gDsiPhyFxnTbl.HAL_DSI_PhyPllReConfigure    = HAL_DSI_2_0_0_PhyPllReConfigure;
        gDsiPhyFxnTbl.HAL_DSI_PhySetup             = HAL_DSI_2_0_0_PhySetup;
        gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams = HAL_DSI_2_0_0_PhySetupTimingParams;
        gDsiPhyFxnTbl.HAL_DSI_PhySetProperty       = HAL_DSI_2_0_0_PhySetProperty;
        gDsiPhyFxnTbl.HAL_DSI_PhyGetProperty       = HAL_DSI_2_0_0_PhyGetProperty;
        gDsiPhyFxnTbl.HAL_DSI_PhyGetPllCodes       = HAL_DSI_2_0_0_PhyGetPllCodes;
    }
    else
    {
      // Clear the table for unsupported targets
      DSI_OSAL_MemZero(&gDsiPhyFxnTbl, sizeof(gDsiPhyFxnTbl));
      DSI_Log_Message(DSI_LOGLEVEL_ERROR, "HAL_DSI_PhyFxnsInit, Wrong version, maj=%d minor=%d\n", sDSIVersionInfo.uMajorVersion, sDSIVersionInfo.uMinorVersion);
    }
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyDisable()
*/
/*!
* \brief
*     Disables DSI Phy.
*
* \param [in]   eDeviceId   - DSI core ID
*
* \retval None
*
****************************************************************************/
void HAL_DSI_PhyDisable( DSI_Device_IDType   eDeviceId )
{
    if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyDisable)
    {
        gDsiPhyFxnTbl.HAL_DSI_PhyDisable(eDeviceId);
    }
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyPllPowerCtrl()
*/
/*!
* \brief
*     Power up/down PLL, LDO and powergen.
*
*
* \param [in]  eDeviceId    - DSI core ID
* \param [in]  bPllPowerUp  - TRUE: power up, FALSE: power down;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyPllPowerCtrl( DSI_Device_IDType   eDeviceId,
                                            bool32              bPllPowerUp )
{
   HAL_MDSS_ErrorType   eStatus = HAL_MDSS_STATUS_SUCCESS;

   if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl)
   {
       eStatus = gDsiPhyFxnTbl.HAL_DSI_PhyPllPowerCtrl(eDeviceId, bPllPowerUp);
   }
   return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetup()
*/
/*!
* \brief
*     Set up DSI Phy alone.
*
* \param [in]  eDeviceId       - DSI core ID
* \param [IN]  psDsiPhyConfig     - Phy configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetup(DSI_Device_IDType          eDeviceId, 
                                    HAL_DSI_PhyConfigType     *psDsiPhyConfig)
{
    HAL_MDSS_ErrorType        eStatus = HAL_MDSS_STATUS_SUCCESS;

    if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhySetup)
    {
        eStatus = gDsiPhyFxnTbl.HAL_DSI_PhySetup(eDeviceId, psDsiPhyConfig);
    }  
    return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyPllSetup()
*/
/*!
* \brief
*     Set up DSI PLL, pass back some config parameters, such as VCO output frequency, 
*     PCLK divider ratio for CC in the form of numerator and denominator, etc.
*
* \param [in]   psDsiPhyConfig     - Phy config info
* \param [out]  psDsiPhyConfigInfo - Phy & PLL config pass back info
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyPllSetup(HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                       HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo)
{
    HAL_MDSS_ErrorType        eStatus = HAL_MDSS_STATUS_SUCCESS;

    if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup)
    {
        eStatus = gDsiPhyFxnTbl.HAL_DSI_PhyPllSetup(psDsiPhyConfig, psDsiPhyConfigInfo);
    }
    return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyPllEnable()
*/
/*!
* \brief
*     Enable or disable PLL
*
* \param [in]   eDeviceId     - DSI core ID
* \param [out]  bEnable       - true to enable PLL and false to disable pll
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyPllEnable(DSI_Device_IDType eDeviceId, bool32 bEnable)
{
    HAL_MDSS_ErrorType        eStatus = HAL_MDSS_STATUS_SUCCESS;

    if (NULL != gDsiPhyFxnTbl.HAL_DSI_Phy_PllEnable)
    {
        gDsiPhyFxnTbl.HAL_DSI_Phy_PllEnable(eDeviceId, bEnable);
    }
    return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetupTimingParams()
*/
/*!
* \brief
*     Calculate PHY timing parameters.
*
* \param [in]  pTimingParameters - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetupTimingParams(HAL_DSI_TimingSettingType        *pTimingParameters)
{
    HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

    if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams)
    {
        eStatus = gDsiPhyFxnTbl.HAL_DSI_PhySetupTimingParams(pTimingParameters);
    }

    return eStatus;
}

/****************************************************************************

HAL_DSI_PhyPllInitalize()

Description:
This function initializes the entire DSI core (Phy/PLL) to a default state.
This function should be applied only once after the core has come out of reset.

****************************************************************************/
void HAL_DSI_PhyPllInitialize(void)
{
    if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize)
    {
        gDsiPhyFxnTbl.HAL_DSI_PhyPllInitialize();
    }
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetProperty()
*/
/*!
* \brief
*   Configure DSI Phy properties
*
* \param [in] eDeviceId     - DSI core ID
* \param [in] eProperty     - Property ID that requires update
* \param [in] pPropertyData - Contains the new parameters of the property 
*                             that will be applied.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetProperty(DSI_Device_IDType              eDeviceId,
                                          HAL_DSI_PhyPropertyType        eProperty,
                                          HAL_DSI_PhyPropertyParamsType* pPropertyData)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhySetProperty)
  {
    eStatus = gDsiPhyFxnTbl.HAL_DSI_PhySetProperty(eDeviceId, eProperty, pPropertyData);
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyGetProperty()
*/
/*!
* \brief
*   Get DSI Phy properties
*
* \param [in]  pTimingParameters - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyGetProperty(DSI_Device_IDType              eDeviceId,
                                          HAL_DSI_PhyPropertyType        eProperty,
                                          HAL_DSI_PhyPropertyParamsType* pPropertyData)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyGetProperty)
  {
    eStatus = gDsiPhyFxnTbl.HAL_DSI_PhyGetProperty(eDeviceId, eProperty, pPropertyData);
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyGetPllCodes()
*/
/*!
* \brief
*   Get PLL codes
*
* \param [in]  eDeviceId     - DSI core ID
* \param [out] psPllCodes    - Pointer to buffer that stores pll codes
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyGetPllCodes(DSI_Device_IDType              eDeviceId,
                                              HAL_DSI_PHY_Dynamic_Cal_Result *psPllCodes)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

  if (NULL != gDsiPhyFxnTbl.HAL_DSI_PhyGetPllCodes)
  {
    eStatus = gDsiPhyFxnTbl.HAL_DSI_PhyGetPllCodes(eDeviceId, psPllCodes);
  }

  return eStatus;
}

#ifdef __cplusplus
}
#endif

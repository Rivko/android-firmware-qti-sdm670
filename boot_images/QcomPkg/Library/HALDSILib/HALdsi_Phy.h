#ifndef HALDSI_PHY_H
#define HALDSI_PHY_H
/*=============================================================================

  File: HALdsi_Phy.h
  

  Copyright (c) 2010-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"
#include "MDPLib_i.h"

/*------------------------------------------------------------------------
   Data Type Defines
-------------------------------------------------------------------------- */
#define  DSIV2_MAJOR_VERSION  0x02
#define  DSIV2_MINOR_VERSION  0x00

/* ------------------------------------------------------------------------
Enumerated types
---------------------------------------------------------------------------*/

typedef enum _HAL_DSI_BitDepthType
{
  HAL_DSI_BitDepth_RGB565  = 0x0,
  HAL_DSI_BitDepth_RGB666P = 0x1,
  HAL_DSI_BitDepth_RGB666  = 0x2,
  HAL_DSI_BitDepth_RGB888  = 0x3,
} HAL_DSI_BitDepthType;

typedef enum _HAL_DSI_PLLConfigSourceType
{
  HAL_DSI_PLL_CONFIG_INDEPENDENT_SOURCE,          /**< PLL0 and PLL1 run independently         */
  HAL_DSI_PLL_CONFIG_SPLIT_SOURCE,                /**< PLL0 is used to drive DSI0 and DSI1     */
  HAL_DSI_PLL_CONFIG_EXTERNAL_SOURCE,             /**< PLLs are used to drive external device  */
} HAL_DSI_PLLConfigSourceType;


/*!
 * \b HAL_DSI_PhyPropertyType
 *
 * Defines list of DSI Phy properties that can be modified.
 */
typedef enum
{
  HAL_DSI_PHY_PROPERTYID_NONE = 0,
  HAL_DSI_PHY_PROPERTYID_LP11,                          /**< Stop transmission, but controller back into LP11 */
  HAL_DSI_PHY_PROPERTYID_LANECONFIG,                    /**< Configure the lane state (Clock & data lanes) */
  HAL_DSI_PHY_PROPERTYID_LANESTATUS,                    /**< Reports the lane status */
  HAL_DSI_PHY_PROPERTYID_PLL_CTRL,                      /**< Enable/disable DSI PLL  */
  HAL_DSI_PHY_PROPERTYID_LANEMAP,                       /**< DSI lane map */
  HAL_DSI_PHY_PROPERTYID_MAX,
  HAL_DSI_PHY_PROPERTYID_FORCE_32BIT = 0x7FFFFFFF
}HAL_DSI_PhyPropertyType;


/*------------------------------------------------------------------------
Data structure definitions
-------------------------------------------------------------------------- */


/*!
 * \struct _HAL_DSI_PhyPllConfigType
 *
 * Config different features of Phy and PLL.
 *
 */
typedef struct _HAL_DSI_PhyConfigType
{
   DSI_Device_IDType            eDeviceId;              /* DSI Core ID */
   uint32                       uDesiredBitClkFreq;     /* desired bit clock frequency in Hz */
   uint32                       uBitsPerPixel;          /* number of bits per pixel */
   uint32                       uNumOfDataLanes;        /* number of DSI data lanes */
   uint32                       uConfigFlags;           /* PLL config options, use flags defined in the above enum _HAL_DSI_PhyConfigFlagType.     */
   bool32                       bDCDCMode;              /* Regulator mode, TRUE=DCDC ,FALSE=LDO */
   uint32                       uClockStrengthHS;       /* Clock lane strength in HS */
   uint32                       uDataStrengthHS;        /* Data lane strength in HS */
   uint32                       uDataStrengthLP;        /* Data lane strength in LP */
   HAL_DSI_PLLConfigSourceType  ePLLConfigSource;       /* PLL Source */
   uint32                       uEscapeFreq;            /* Escape clk freq in Hz */
   DSI_TimingOverrideType       *pPreDefinedTimings;    /* Timing override */
   bool32                       bCPhyMode;              /* C-Phy */
   uint32                       uDsiClkSel;             /* pixel clk source select */
   uint32                       uPhyPostDiv;
   uint32                       uPllPostDiv;
   
} HAL_DSI_PhyConfigType;


/*!
 * \struct HAL_DSI_PhyConfigInfoType 
 * 
 * Defines the clock divider ratio for DSI clocks. DSI clock tree follows this path
 *
 * Direct path mode (when the required bit clock rate > 680 MHz)      TODO: clarify the different edge frequency mentioned in 2 HW documents 
 *
 *                 +-------------------------------------------------------------+
 *                 |                                                             |-->[  Esc Clk PreDivider & MND ]---EscClkFreq--->[  DSI Controller & Phy ]
 *                 |                                                        +----+
 *                 |                                                        |
 * [TCXO(19.2Mhz)]-+->[DSI PLL (VCO)]-+->[  Constant div 4 ]---ByteClkFreq--+------->[  Pass-through without MND ]---ByteClkFreq-->[  DSI Controller & Phy ]
 *                                    |
 *                                    +->[ Digital Postdiv ]---DsiClkFreq----------->[ DSI PCLK PreDivider & MND ]---PClkFreq----->[ DSI Controller & MDP5 ]
 *                                                                              
 *                                                                              
 *
 * |---Reference---|--------------------------DSI PLL-----------------------|-------------------MMSS CC Dividers-----------------|
 *
 *
 *
 * Non-direct path mode  (when the required bit clock rate < 680 MHz) 
 *
 *                 +-------------------------------------------------------------+
 *                 |                                                             |-->[  Esc Clk PreDivider & MND ]---EscClkFreq--->[  DSI Controller & Phy ]
 *                 |                                                        +----+
 *                 |                                                        |
 * [TCXO(19.2Mhz)]-+->[DSI PLL (VCO)]-+->[ 8*Analog Postdiv]---ByteClkFreq--+------->[  Pass-through without MND ]---ByteClkFreq-->[  DSI Controller & Phy ]
 *                                    |
 *                                    +->[ Digital Postdiv ]---DsiClkFreq----------->[ DSI PCLK PreDivider & MND ]---PClkFreq----->[ DSI Controller & MDP5 ]
 *                                                                              
 *                                                                              
 *
 * |---Reference---|---------------------------DSI PLL-----------------------|-------------------MMSS CC Dividers-----------------|
 *
 *
 * Based on the diagram above:
 *
 * direct path:
 *                  ByteClk = VcoCLk/4 (Analog Postdiv bypassed)
 *                  PClk    = VcoClk/Digital Postdiv / MND Divider = DsiClk/MND Divider
 * non-direct path: 
 *                  ByteClk = VcoCLk/(8*Analog Postdiv)
 *                  PClk    = VcoClk/Digital Postdiv / MND Divider = DsiClk/MND Divider
 *
 */
typedef struct _HAL_DSI_PhyConfigInfoType
{
    uint32  uPllVcoOutputFreq;     /* PLL VCO output frequency in Hz */
    uint32  uBitClkFreq;           /* bit clock frequency in Hz */
    uint32  uByteClkFreq;          /* byte clock frequency in Hz */    
    uint32  uDsiClkFreq;           /* DSI clock frequency in Hz; this is the source of PClk (see the diagram above); */
    uint32  uPclkFreq;             /* PClk frequency in Hz */
    uint32  uPClkDivNumerator;     /* numerator of PClk divider ratio in CC */
    uint32  uPClkDivDenominator;   /* denominator of PClk divider ratio in CC */
                                   /*TODO: add output info on Esc clock once getting clarification from HW team */
} HAL_DSI_PhyConfigInfoType;

/*!
 * \b HAL_DSI_PhyPropertyParamsType
 *
 * Config parameters required for HAL DSI Phy set property.
 */
typedef union
{
  bool32                   bEnablePLL;                /**< HAL_DSI_PHY_PROPERTYID_PLL_CTRL          */
  uint32                   uLaneStatus;               /**< HAL_DSI_PHY_PROPERTYID_LANESTATUS        */
  DSI_LaneMapType          aLaneMap[DSI_LaneMap_MAX]; /**< HAL_DSI_PHY_PROPERTYID_LANEMAP           */
  HAL_DSI_LanePropertyType sLaneConfig;               /**< HAL_DSI_PHY_PROPERTYID_LANECONFIG        */
}HAL_DSI_PhyPropertyParamsType;


/*!
 * \struct _HAL_DSI_PllSettingType
 *
 * DSI Timing parameters
 */
typedef struct _HAL_DSI_TimingSettingType
{
  DSI_Device_IDType         eDeviceId;             /* DSI Core ID */
  bool32                    bCPhyMode;             /* C-Phy  mode */
  uint32                    uBitclock;
  uint32                    uEscapeFreq;
  DSI_TimingOverrideType   *pPreDefinedTimings;
} HAL_DSI_TimingSettingType;


/*!
* \struct HAL_DSI_PhyFunctionTable
*
* DSI PHY/PLL function table
*/
typedef struct
{
  /* Initialize DSI PHY/PLL */
  void (*HAL_DSI_PhyPllInitialize) (void);

  /* Disable DSI PHY */
  void (*HAL_DSI_PhyDisable) (DSI_Device_IDType eDeviceId);

  /* Enable/Disable PLL */
  void (*HAL_DSI_Phy_PllEnable) (DSI_Device_IDType eDeviceId, bool32 bEnable);

  /* Power up/down PLL, LDO and powergen */
  HAL_MDSS_ErrorType (*HAL_DSI_PhyPllPowerCtrl) (DSI_Device_IDType eDeviceId, bool32 bPllPowerUp);

  /* Setup DSI PHY */
  HAL_MDSS_ErrorType (*HAL_DSI_PhySetup) (DSI_Device_IDType eDeviceId, HAL_DSI_PhyConfigType *psDsiPhyConfig);

  /* Setup DSI PLL */
  HAL_MDSS_ErrorType (*HAL_DSI_PhyPllSetup) (HAL_DSI_PhyConfigType *psDsiPhyConfig, HAL_DSI_PhyConfigInfoType *psDsiPhyConfigInfo);

  /* Calculate PHY timing parameters */
  HAL_MDSS_ErrorType (*HAL_DSI_PhySetupTimingParams) (HAL_DSI_TimingSettingType *pTimingParameters);

    /* Reconfigures PHY PLL to a different refresh rate */
    HAL_MDSS_ErrorType (*HAL_DSI_PhyPllReConfigure) (HAL_DSI_PhyConfigType *psDsiPhyConfig, HAL_DSI_PhyConfigInfoType *psDsiPhyPLLConfigInfo);

  /* Set parmaeters on Phy */
  HAL_MDSS_ErrorType (*HAL_DSI_PhySetProperty) (DSI_Device_IDType              eDeviceId,
                                                HAL_DSI_PhyPropertyType        eProperty,
                                                HAL_DSI_PhyPropertyParamsType* pPropertyData);

  /* Get parmaeters on Phy */
  HAL_MDSS_ErrorType (*HAL_DSI_PhyGetProperty) (DSI_Device_IDType              eDeviceId,
                                                HAL_DSI_PhyPropertyType        eProperty,
                                                HAL_DSI_PhyPropertyParamsType* pPropertyData);

  /* Get pll codes */
  HAL_MDSS_ErrorType (*HAL_DSI_PhyGetPllCodes) (DSI_Device_IDType              eDeviceId,
                                                HAL_DSI_PHY_Dynamic_Cal_Result* psPllCodes);


} HAL_DSI_PhyFunctionTable;


/*------------------------------------------------------------------------
Function declarations
-------------------------------------------------------------------------- */
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
void HAL_DSI_PhyFxnsInit(void);

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
void HAL_DSI_PhyDisable( DSI_Device_IDType   eDeviceId );



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
                                            bool32              bPllPowerUp );


/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetup()
*/
/*!
* \brief
*     Set up DSI Phy alone.
*
* \param [in]  eDeviceId       - DSI core ID
* \param [IN]   psDsiPhyConfig     - Phy configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetup(DSI_Device_IDType          eDeviceId, 
                                    HAL_DSI_PhyConfigType     *psDsiPhyConfig);


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
HAL_MDSS_ErrorType HAL_DSI_PhyPllSetup( HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                        HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo);


/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyPllEnable()
*/
/*!
* \brief
*     Enable or disable Pll.
*
* \param [in]  eDeviceId - DSI id
* \param [in]  bEnable   - true to enable and false to disable pll
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyPllEnable(DSI_Device_IDType eDeviceId, bool32 bEnable);


/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetupTimingParams()
*/
/*!
* \brief
*     Calculate PHY timing parameters.
*
* \param [in]  pTimingParameters - Phy timing parameters
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetupTimingParams(HAL_DSI_TimingSettingType  *pTimingParameters);


/****************************************************************************
*
** FUNCTION: HAL_DSI_PhySetProperty
*/
/*!
* \brief
*   Configure DSI Phy properties
*
* \param [in] eDeviceId     - DSI core ID
* \param [in] eProperty     - Property ID that requires update
* \param [in] pPropertyData - Contains the new parameters of the property 
*                             that will be applied.
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhySetProperty(DSI_Device_IDType              eDeviceId,
                                          HAL_DSI_PhyPropertyType        eProperty,
                                          HAL_DSI_PhyPropertyParamsType* pPropertyData);

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyGetProperty
*/
/*!
* \brief
*   Get DSI Phy properties
*
* \param [in] eDeviceId     - DSI core ID
* \param [in] eProperty     - Property ID that is being queried
* \param [in] pPropertyData - Contains the parameters of the property 
*                             that are being queried
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyGetProperty(DSI_Device_IDType              eDeviceId,
                                          HAL_DSI_PhyPropertyType        eProperty,
                                          HAL_DSI_PhyPropertyParamsType* pPropertyData);


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
                                           HAL_DSI_PHY_Dynamic_Cal_Result *psPllCodes);


/* Nazgul */
void               HAL_DSI_2_0_0_PhyPllInitialize(void);
void               HAL_DSI_2_0_0_PhyDisable( DSI_Device_IDType   eDeviceId );
void               HAL_DSI_2_0_0_PhyPllEnable(DSI_Device_IDType eDeviceId, bool32 bEnable);
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyPllPowerCtrl(DSI_Device_IDType eDeviceId, bool32 bPllPowerUp);
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhySetup(DSI_Device_IDType eDeviceId, HAL_DSI_PhyConfigType *psDsiPhyConfig);
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyPllSetup(HAL_DSI_PhyConfigType *psDsiPhyConfig, HAL_DSI_PhyConfigInfoType *psDsiPhyConfigInfo);
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhySetupTimingParams(HAL_DSI_TimingSettingType *pTimingParameters);
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyPllReConfigure(HAL_DSI_PhyConfigType *psDsiPhyConfig, HAL_DSI_PhyConfigInfoType *psDsiPhyPLLConfigInfo);

HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhySetProperty(DSI_Device_IDType              eDeviceId,
                                                HAL_DSI_PhyPropertyType        eProperty,
                                                HAL_DSI_PhyPropertyParamsType* pPropertyData);

HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyGetProperty(DSI_Device_IDType              eDeviceId,
                                                HAL_DSI_PhyPropertyType        eProperty,
                                                HAL_DSI_PhyPropertyParamsType* pPropertyData);

HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyGetPllCodes(DSI_Device_IDType               eDeviceId,
                                                HAL_DSI_PHY_Dynamic_Cal_Result *psPllCodes);


#ifdef __cplusplus
}
#endif

#endif  /* #define HALDSI_PHY_H */

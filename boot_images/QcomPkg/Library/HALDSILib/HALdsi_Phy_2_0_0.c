/*=============================================================================

  File: HALdsi_Phy_2_0_0.c
  
  Description:
  HALdsi_Phy_2_0_0.c supports PHY/PLL operations for Nazgul(8998) and Napali(845).

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
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
#include "HALdsi_Pll.h"
#include "dsiHostSystem.h"
#include "HALdsi_Phy_2_0_0.h"


/* -----------------------------------------------------------------------
** Local defines
** ----------------------------------------------------------------------- */
#define HAL_DSI_PHY_REFGEN_TIMEOUT                  150           /* ~15 ms */
#define HAL_DSI_PHY_PLL_READY_TIMEOUT               150           /* ~15 ms */

#define HAL_DSI_PHY_PLL_PRE_EMPHASIS_BOTTOM_DEFAULT 0x0
#define HAL_DSI_PHY_PLL_PRE_EMPHASIS_TOP_DEFAULT    0x0

/* Generic parameters that applies to all parameter calculations */
#define HAL_DSI_PHY_TIPX_NUMERATOR                  1000000000.0f /* numerator for the TIPX formula */
#define HAL_DSI_PHY_PERCENT_DENOMENATOR             100.0f
#define HAL_DSI_PHY_TREOT                           20            /* t_reot    */

/* Range factor applied */
#define HAL_DSI_PHY_RANGEFACTOR_1                   1.0f
#define HAL_DSI_PHY_RANGEFACTOR_5                   5.0f

#define HAL_DSI_PHY_RANGEFACTOR_TCLK_PREPARE        50.0f
#define HAL_DSI_PHY_RANGEFACTOR_THS_PREPARE         50.0f
#define HAL_DSI_PHY_RANGEFACTOR_THS_ZERO            10.0f
#define HAL_DSI_PHY_RANGEFACTOR_NOMAX               10.0f
#define HAL_DSI_PHY_RANGEFACTOR_TRAIL               30.0f

#define HAL_DSI_PHY_TREOT                           20

/* Recommended values for TCLK_PREPARE formula */
#define HAL_DSI_PHY_TCLK_PREPARE_PHY_MIN            38.0f
#define HAL_DSI_PHY_TCLK_PREPARE_PHY_MAX            95.0f
#define HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM1         0x04
#define HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM2         0x08
#define HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM3         0x04

/* Recommended Values for TCLK_ZERO formula */
#define HAL_DSI_PHY_TCLK_ZERO_PARAM1                300.0f
#define HAL_DSI_PHY_TCLK_ZERO_PARAM3                8.0f
#define HAL_DSI_PHY_TCLK_ZERO_PARAM4                0x01
#define HAL_DSI_PHY_TCLK_ZERO_PARAM5                255
#define HAL_DSI_PHY_TCLK_ZERO_PARAM6                511
#define HAL_DSI_PHY_TCLK_ZERO_PARAM7                0x02

/* Recommended Values for TCLK_TRAIL formula */
#define HAL_DSI_PHY_TCLK_TRAIL_MIN                  60.0f

/* Recommended Values for T_HS_PREPARE formula */
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM1             0x28
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM2             0x04
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM3             0x55
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM4             0x06
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM5             0x04
#define HAL_DSI_PHY_T_HS_PREPARE_PARAM6             0x08

/* Recommended Values for T_HS_ZERO formula */
#define HAL_DSI_PHY_T_HS_ZERO_PARAM1                0x91
#define HAL_DSI_PHY_T_HS_ZERO_PARAM2                0x0A
#define HAL_DSI_PHY_T_HS_ZERO_PARAM3                0x0B
#define HAL_DSI_PHY_T_HS_ZERO_PARAM4                0x08
#define HAL_DSI_PHY_T_HS_ZERO_PARAM5                0x01
#define HAL_DSI_PHY_T_HS_ZERO_PARAM6                0xFF

/* Recommended Values for T_XX_TRAIL formula */
#define HAL_DSI_PHY_T_T_TRAIL_PARAM1                0x3C
#define HAL_DSI_PHY_T_T_TRAIL_PARAM2                0x04
#define HAL_DSI_PHY_T_T_TRAIL_PARAM3                0x03
#define HAL_DSI_PHY_T_T_TRAIL_PARAM4                0x08
#define HAL_DSI_PHY_T_T_TRAIL_PARAM5                0x01

/* Recommended Values for TEOT formula */
#define HAL_DSI_PHY_TEOT_PARAM1                     0x69
#define HAL_DSI_PHY_TEOT_PARAM2                     0x0C

/* Recommended Values for T_HS_EXIT formula */
#define HAL_DSI_PHY_T_HS_EXIT_MIN                   100.0f
#define HAL_DSI_PHY_T_HS_EXIT_MAX                   255
#define HAL_DSI_PHY_T_HS_EXIT_PARAM1                0x08
#define HAL_DSI_PHY_T_HS_EXIT_PARAM2                0x01

/* Recommended Values for T_HS_RQST formula */
#define HAL_DSI_PHY_T_HS_RQST_PARAM1                50
#define HAL_DSI_PHY_T_HS_RQST_PARAM2                4
#define HAL_DSI_PHY_T_HS_RQST_PARAM3                8

/* Recommended Values for T_TA_GO formula */
#define HAL_DSI_PHY_T_TA_GO_PARAM1                  0x03

/* Recommended Values for T_TA_SURE formula */
#define HAL_DSI_PHY_T_TA_SURE_PARAM1                0x00

/* Recommended Values for T_TA_GET formula */
#define HAL_DSI_PHY_T_TA_GET_PARAM1                 0x04

/* Recommended Values for T_CLK_PRE formula */
#define HAL_DSI_PHY_T_CLK_PRE_PARAM1                0x08
#define HAL_DSI_PHY_T_CLK_PRE_PARAM2                0x08
#define HAL_DSI_PHY_T_CLK_PRE_PARAM3                0x02
#define HAL_DSI_PHY_T_CLK_PRE_PARAM4                0x01
#define HAL_DSI_PHY_T_CLK_PRE_MAX                   0x3F

/* Recommended Values for T_CLK_POST formula */
#define HAL_DSI_PHY_T_CLK_POST_PARAM1               0x3C
#define HAL_DSI_PHY_T_CLK_POST_PARAM2               0x34
#define HAL_DSI_PHY_T_CLK_POST_PARAM3               0x2B
#define HAL_DSI_PHY_T_CLK_POST_PARAM4               0x08
#define HAL_DSI_PHY_T_CLK_POST_PARAM5               0x01
#define HAL_DSI_PHY_T_CLK_POST_MAX                  0x3F

/* Recommended Values for PLL */
#define HAL_DSI_PLL_REF_FREQ_MHZ                    19.2f         /* 19.2 Mhz Reference Clock */
#define HAL_DSI_PLL_PRESCALE_FACTOR                 2.0f          /* Prescaler */
#define HAL_DSI_PLL_SSC_FREQ_KHZ                    31.5f         /* 31.5 Khz */
#define HAL_DSI_PLL_SSC_OFFSET_PPM                  4800          /* 4800 ppm */
#define HAL_DSI_PLL_SSC_ADJ_PER                     2             /* Reference cycles per SSC step */
#define HAL_DSI_PLL_LOCK_TIMER                      0x40          /* 0-255 for Qlink */
#define HAL_DSI_PLL_FRAC_BITS                       18            /* Number of bits for fraction Qlink-18bits */
#define HAL_DSI_PLL_THRESHOLD_CYCLES                32            /* 0 - 255 */
#define HAL_DSI_PLL_REFCLK_CYCLES                   256           /* 1 - 1024 */
#define HAL_DSI_PLL_SYSTEM_MUXES                    0xD0
#define HAL_DSI_PLL_PLL_LOCK_DELAY                  0x06

#define HAL_DSI_PLL_PARAM_16                        16
#define HAL_DSI_PLL_PARAM_10                        10
#define HAL_DSI_PLL_PARAM_8                         8
#define HAL_DSI_PLL_PARAM_2                         2
#define HAL_DSI_PLL_PARAM_0_2F                      0.2f
#define HAL_DSI_PLL_PARAM_1                         1
#define HAL_DSI_PLL_PARAM_1100F                     1100.0f
#define HAL_DSI_PLL_PARAM_1900F                     1900.0f
#define HAL_DSI_PLL_PARAM_3000F                     3000.0f
#define HAL_DSI_PLL_PARAM_15                        15
#define HAL_DSI_PLL_PARAM_1M                        1000000
#define HAL_DSI_PLL_PARAM_500F                      500.0f
#define HAL_DSI_PLL_OUTDIV_LUT_COUNT                19
#define HAL_DSI_PLL_VCO_MIN_MHZ                     1500.0f
#define HAL_DSI_PLL_VCO_MIN_MHZ_2_2_0               1000.0f        /* Minimum VCO for Napali */

#define HAL_DSI_LANE_TX_DCTRL_1                     0x00           /* TX Dctrl for Lanes 0, 1 & 2 */
#define HAL_DSI_LANE_TX_DCTRL_2                     0x02           /* TX Dctrl for Lane 3 */
#define HAL_DSI_LANE_TX_DCTRL_3                     0x01           /* TX Dctrl for CLK Lane */


/* C-Phy Parameters */
#define HAL_DSI_CPHY_HALFBYTECLK_EN_THRESHOLD       80000000
#define HAL_DSI_CPHY_T_HS_EXIT_MIN                  100.0f
#define HAL_DSI_CPHY_T_HS_EXIT_MAX                  255.0f
#define HAL_DSI_CPHY_T3_PREPARE_MIN                 38
#define HAL_DSI_CPHY_T3_PREPARE_MAX                 95
#define HAL_DSI_CPHY_T3_PREBEGIN_CNT_MIN            7
#define HAL_DSI_CPHY_T3_PREBEGIN_CNT_MAX            448
#define HAL_DSI_CPHY_T3_POST_CNT_MIN                7
#define HAL_DSI_CPHY_T3_POST_CNT_MAX                224
#define HAL_DSI_CPHY_T_HS_RQST_MIN                  50.0f
#define HAL_DSI_CPHY_BITS_SYMBOL                    3
#define HAL_DSI_CPHY_BITS_BYTE                      7
#define HAL_DSI_CPHY_RANGEFACTOR_0                  0.0f

/*!
 * \struct _HAL_DSI_PHY_LaneConfigType_
 *
 * PHY configuration parameters
 */
typedef struct _HAL_DSI_PHY_LaneConfigType_
{
  uint32           uDataStrengthLP_P;         /* Data strength LP_P */
  uint32           uDataStrengthLP_N;         /* Data strength LP_N */
  uint32           uDataStrengthHS;           /* Data strength HS */
  uint32           uClockStrengthHS;          /* Clock Strength HS */
  uint32           uPemphBottom;              /* Determines how many pre-emphasis branches for bottom termination */
  uint32           uPemphTop;                 /* Determines how many pre-emphasis branches for top termination */
  bool32           bPemphEnable;              /* Enable driver pre-emphasis */
  bool32           bStrengthOverride;         /* Strength override to use DSIPHY_LNn_TEST_STR */
} HAL_DSI_PHY_LaneConfigType;


/*!
 * \struct _HAL_DSI_PHY_TimingConfigType_
 *
 * PHY timing parameters
 */
typedef struct _HAL_DSI_PHY_TimingConfigType_
{
  bool32           bHSTX_HalfByteClk_En;      /* DSI timing parameter: HSTX_HALFBYTECLK_EN */
  uint32           uT_CLK_ZERO;               /* DSI timing parameter: t_CLK_ZERO */
  uint32           uT_CLK_PREPARE;            /* DSI timing parameter: t_CLK_PREPARE */
  uint32           uT_CLK_TRAIL;              /* DSI timing parameter: t_CLK_TRAIL */
  uint32           uT_HS_EXIT;                /* DSI timing parameter: t_HS_EXIT */
  uint32           uT_HS_ZERO;                /* DSI timing parameter: t_HS_ZERO */
  uint32           uT_HS_PREPARE;             /* DSI timing parameter: t_HS_PREPARE */
  uint32           uT_HS_TRAIL;               /* DSI timing parameter: t_HS_TRAIL */
  uint32           uT_HS_RQST;                /* DSI timing parameter: t_HS_RQST */
  uint32           uT_TA_SURE;                /* DSI timing parameter: t_TA_SURE */
  uint32           uT_TA_GO;                  /* DSI timing parameter: t_TA_GO */
  uint32           uT_TA_GET;                 /* DSI timing parameter: t_TA_GET */
  uint32           uT_CLK_PRE;                /* DSI timing parameter: t_CLK_PRE */
  uint32           uT_CLK_POST;               /* DSI timing parameter: t_CLK_POST */
  bool32           bT_CLK_PRE_INC_BY_2;       /* DSI timing parameter: t_CLK_PRE_INC_BY_2 */
} HAL_DSI_PHY_TimingConfigType;

/*!
 * \struct _HAL_DSI_PHY_PhySettingsType
 *
 * PHY configuration parameters
 */
typedef struct _HAL_DSI_PHY_PhySettingsType
{
  uint32                       uDivCtrl;
  uint32                       uPhyPostDiv;
  bool32                       bSlave;
  HAL_DSI_PHY_TimingConfigType sTimingConfig;
  HAL_DSI_PHY_LaneConfigType   sLaneConfig;
  bool32                       bCPhyMode;
  bool32                       bDualDSI;
} HAL_DSI_PHY_PhySettingsType;

/*!
 * \struct _HAL_DSI_PHY_PllConfigType
 *
 * PLL configuration parameters
 */
typedef struct _HAL_DSI_PHY_PllConfigType
{
  uint32                       uDecDivStart;   /* Freq divider Decimal    value */
  uint32                       uFracDivLow;    /* Freq divider Fractional part  bits 7:0   */
  uint32                       uFracDivMid;    /* Freq divider Fractional part  bits 15:8  */
  uint32                       uFracDivHigh;   /* Freq divider Fractional part  bits 17:16 */
  uint32                       uPllPostDiv;    /* PLL Post divider ratio */
  uint32                       uPropGainRate;
  uint32                       uLockDetRate;
  uint32                       uFreqDetThresh;
  uint32                       uFreqDetRefclk;
  uint32                       uFreqDetPllclk;
  uint32                       uSSCPER;
  uint32                       uSSCAdjPER;
  uint32                       uSSCStepSize;
  uint32                       uLockDelay;
  uint32                       uClockInverters;
  float                        fPllErrorPPM;
} HAL_DSI_PHY_PllConfigType;

/*!
 * \struct _HAL_DSI_PHY_PLL_KVCO_RANGE_TYPE_
 *
 * DSI KVCO range settings
 */
typedef struct _HAL_DSI_PHY_PLL_KVCO_RANGE_TYPE_
{
  uint32           uLow;                      /* KVCO lower bound  */
  uint32           uHigh;                     /* KVCO higher bound */
  uint32           uKvcoSlope;                /* KVCO slope        */
  uint32           uKvcoCode;                 /* KVCO code         */
} HAL_DSI_PHY_PLL_KVCORangeType;

/*!
 * \struct HAL_DSI_PHY_DividerLUTEntryType
 *
 * Divider LUT entry
 */
typedef struct _HAL_DSI_PHY_DividerLUTEntryType_
{
  uint16        uPllPostDiv;
  uint16        uPhyPostDiv;
} HAL_DSI_PHY_DividerLUTEntryType;

/*!
 * \struct HAL_DSI_PHY_VersionType
 *
 * HAL DSI PHY hardware version info
 */
typedef struct _HAL_DSI_PHY_VersionType_
{
  uint16        uMajorVersion;
  uint16        uMinorVersion;
} HAL_DSI_PHY_VersionType;

/* PLL divider LUTs */
static HAL_DSI_PHY_DividerLUTEntryType asPllDividerLUT_DPhy[] = {
  { 2, 11},
  { 4, 5 },
  { 2, 9 },
  { 8, 2 },
  { 1, 15},
  { 2, 7 },
  { 1, 13},
  { 4, 3 },
  { 1, 11},
  { 2, 5 },
  { 1, 9 },
  { 8, 1 },
  { 1, 7 },
  { 2, 3 },
  { 1, 5 },
  { 4, 1 },
  { 1, 3 },
  { 2, 1 },
  { 1, 1 }
};

static HAL_DSI_PHY_DividerLUTEntryType asPllDividerLUT_CPhy[] = {
  { 4, 5 },     /* 20 */
  { 8, 2 },     /* 16 */
  { 4, 3 },     /* 12 */
  { 2, 5 },     /* 10 */
  { 8, 1 },     /* 8 */
  { 2, 3 },     /* 6 */
  { 1, 5 },     /* 5 */
  { 4, 1 },     /* 4 */
  { 1, 3 },     /* 3 */
  { 2, 1 },     /* 2 */
  { 1, 1 }      /* 1 */
};

/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_GetVersionInfo()
*/
/*!
* \brief
*     Gets DSI PHY hardware info.
*
*
* \param [out]  HAL_DSI_PHY_VersionType;
*
* \retval
*
****************************************************************************/
static void HAL_DSI_Phy_GetVersionInfo(HAL_DSI_PHY_VersionType *pVersionInfo)
{
  uint32 uVersionInfo = in_dword(HWIO_DSI_0_HW_VERSION_ADDR);

  if (NULL != pVersionInfo)
  {
    pVersionInfo->uMajorVersion = HWIO_GETVAL(DSI_0_HW_VERSION, uVersionInfo, MAJOR);
    pVersionInfo->uMinorVersion = HWIO_GETVAL(DSI_0_HW_VERSION, uVersionInfo, MINOR);
  }
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_GetLaneOffset()
*/
/*!
* \brief
*     Gets register offset for the specific Phy Lane.
*
*
* \param [in]  eLainId - Lane id;
*
* \retval  Register offset value for the specified lane;
*
****************************************************************************/
static uintPtr HAL_DSI_Phy_GetLaneOffset(DSI_LaneIDType eLaneId)
{
  uintPtr uLaneBlockOffset[DSI_LaneID_Max];
  uintPtr uOffset                           = 0;

  uLaneBlockOffset[DSI_LaneID_0]   = 0;
  uLaneBlockOffset[DSI_LaneID_1]   = HWIO_DSI_0_PHY_DSIPHY_DLN1_CFG0_ADDR - HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG0_ADDR;
  uLaneBlockOffset[DSI_LaneID_2]   = HWIO_DSI_0_PHY_DSIPHY_DLN2_CFG0_ADDR - HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG0_ADDR;
  uLaneBlockOffset[DSI_LaneID_3]   = HWIO_DSI_0_PHY_DSIPHY_DLN3_CFG0_ADDR - HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG0_ADDR;
  uLaneBlockOffset[DSI_LaneID_CLK] = HWIO_DSI_0_PHY_DSIPHY_CKLN_CFG0_ADDR - HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG0_ADDR;

  if (eLaneId < DSI_LaneID_Max)
  {
    uOffset = uLaneBlockOffset[eLaneId];
  }
  return uOffset;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_GetTxDctrl()
*/
/*!
* \brief
*     Gets Tx DCTRL value for the specific Phy Lane.
*
*
* \param [in]  eLainId - Lane id;
*
* \retval  Tx Dctrl value
*
****************************************************************************/
static uint32 HAL_DSI_Phy_GetTxDctrl(DSI_LaneIDType eLaneId)
{
  uint32 uTxDctrl = 0;
  HAL_DSI_PHY_VersionType   sVersionInfo;

  DSI_OSAL_MemZero(&sVersionInfo, sizeof(HAL_DSI_PHY_VersionType));

  /* Get the DSI PHY version info */
  HAL_DSI_Phy_GetVersionInfo(&sVersionInfo);

  switch (eLaneId)
  {
  case DSI_LaneID_0:
  case DSI_LaneID_1:
  case DSI_LaneID_2:
    uTxDctrl = HAL_DSI_LANE_TX_DCTRL_1;
    break;
  case DSI_LaneID_3:
    if (0x02 <= sVersionInfo.uMinorVersion)
    {
      /* Napali */
      uTxDctrl = HAL_DSI_LANE_TX_DCTRL_1;
    }
    else
    {
      /* Nazgul */
      uTxDctrl = HAL_DSI_LANE_TX_DCTRL_2;
    }
    break;
  case DSI_LaneID_CLK:
    uTxDctrl = HAL_DSI_LANE_TX_DCTRL_3;
    break;
  default:
    break;
  }
  return uTxDctrl;

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CheckNegative()
*/
/*!
* \brief
*     If the integer is negative, set it to zero
*
*
* \param [in]  iVal - integer value to be processed;
*
* \retval  0 or positive value as integer;
*
****************************************************************************/
static int32 HAL_DSI_Phy_CheckNegative(int32 iVal)
{
  if (iVal < 0)
  {
    iVal = 0;
  }
  return iVal;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Round()
*/
/*!
* \brief
*     Rounds to the nearest integer.
*
*
* \param [in]  fFloatVal - float number to be processed;
*
* \retval  rounded integer;
*
****************************************************************************/
static int32 HAL_DSI_Phy_Round(float fFloatVal)
{
  int32  iRoundupVal = (int32)fFloatVal;
  if (fFloatVal - iRoundupVal > 0.5) iRoundupVal++;

  return iRoundupVal;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Roundup()
*/
/*!
* \brief
*     Rounds up to the nearest integer.
*
*
* \param [in]  fFloatVal - float number to be processed;
*
* \retval  rounded up integer;
*
****************************************************************************/
static int32 HAL_DSI_Phy_Roundup(float fFloatVal)
{
  int32  iRoundupVal = (int32)fFloatVal;

  if (fFloatVal - iRoundupVal > 0) iRoundupVal++;
  else if (fFloatVal - iRoundupVal < 0) iRoundupVal--;

  return iRoundupVal;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Rounddown()
*/
/*!
* \brief
*     Rounds down to the nearest integer for positive number, 0 if it is negative number.
*
*
* \param [in]  fFloatVal - float number to be processed;
*
* \retval  rounded down integer;
*
****************************************************************************/
static uint32 HAL_DSI_Phy_Rounddown(double pFloatValue)
{
  uint32     roundupValue;

  if (pFloatValue > 0)
  {
    roundupValue = (uint32)pFloatValue;
  }
  else
  {
    roundupValue = 0;
  }

  return roundupValue;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_PllEnable()
*/
/*!
* \brief
*     Enable/Disable Phy PLL
*
* \param [in]  eDeviceId         - DSI core ID
* \param [in]  bEnable           - Enable/disable PLL
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_PllEnable(DSI_Device_IDType eDeviceId, bool32 bEnable)
{
  HAL_MDSS_ErrorType   eStatus = HAL_MDSS_STATUS_SUCCESS;
  uintPtr              uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);
  uint32               uRegVal = 0;

  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_PLL_CNTRL, PLL_START, bEnable);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_PLL_CNTRL_ADDR, uRegVal);

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_PllBiasEnable()
*/
/*!
* \brief
*     Enable/Disable PLL Bias
*
* \param [in]  eDeviceId         - DSI core ID
* \param [in]  bEnable           - Enable/disable PLL Bias
*
* \retval void
*
****************************************************************************/
static void HAL_DSI_Phy_PllBiasEnable(DSI_Device_IDType eDeviceId, bool32 bEnable)
{
  uintPtr            uOffset    = HAL_DSI_GetRegBaseOffset(eDeviceId);
  uint32             uRegVal    = 0;

  if (bEnable)
  {
    /* Set BIAS_EN_MUX, BIAS_EN */
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_PLL_SYSTEM_MUXES, CM_BIAS_EN_MUX, 1);
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_PLL_SYSTEM_MUXES, CM_BIAS_EN, 1);
  }
  
  /* pll system muxes */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_SYSTEM_MUXES_ADDR, uRegVal);
  
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_PllGlobalClockEnable()
*/
/*!
* \brief
*     Enable/Disable PLL clock

*
* \param [in]  eDeviceId         - DSI core ID
* \param [in]  bEnable           - Enable/disable Global Clk
*
* \retval void
*
****************************************************************************/
static void HAL_DSI_Phy_PllGlobalClockEnable(DSI_Device_IDType eDeviceId, bool32 bEnable)
{
  uintPtr            uOffset    = HAL_DSI_GetRegBaseOffset(eDeviceId);
  uint32             uClkCfg    = in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CLK_CFG1_ADDR);
  uint32             uClkEnable = 0;

  /* Set CLK_EN */
  if (bEnable)
  {
    uClkEnable = 1;
  }
  
  uClkCfg = HWIO_OUT_FLD(uClkCfg, DSI_0_PHY_DSIPHY_CMN_CLK_CFG1, CLK_EN, uClkEnable);

  /* clk cfg1 */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CLK_CFG1_ADDR, uClkCfg);
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_PllSetSource()
*/
/*!
* \brief
*     Set byte clock source for PLL
*
* \param [in]  psDsiPhyConfig         - PHY Configruation 
*
* \retval void
*
****************************************************************************/
static void HAL_DSI_Phy_PllSetSource(HAL_DSI_PhyConfigType       *psDsiPhyConfig )
{
  uintPtr            uOffset    = HAL_DSI_GetRegBaseOffset(psDsiPhyConfig->eDeviceId);
  uint32             uClkCfg    = in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CLK_CFG1_ADDR);

  if ((HAL_DSI_PLL_CONFIG_SPLIT_SOURCE == psDsiPhyConfig->ePLLConfigSource) &&
      (DSI_DeviceID_1 == psDsiPhyConfig->eDeviceId))
  {
    uClkCfg = HWIO_OUT_FLD(uClkCfg, DSI_0_PHY_DSIPHY_CMN_CLK_CFG1, BITCLK_SEL, 1);
  }
  else
  {
    uClkCfg = HWIO_OUT_FLD(uClkCfg, DSI_0_PHY_DSIPHY_CMN_CLK_CFG1, DSICLK_SEL, psDsiPhyConfig->uDsiClkSel);
  }

  /* clk cfg1 */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CLK_CFG1_ADDR, uClkCfg);

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_WaitForRefgen()
*/
/*!
* \brief
*     Check Phy status and wait for refgen
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_WaitForRefgen(DSI_Device_IDType eDeviceId)
{
  HAL_MDSS_ErrorType eStatus     = HAL_MDSS_STATUS_SUCCESS;
  uintPtr            uOffset     = HAL_DSI_GetRegBaseOffset(eDeviceId);
  uint32             uTimeout    = HAL_DSI_PHY_REFGEN_TIMEOUT;
  uint32             uRefgen     = 0;

  while (0 == uRefgen)
  {
    uRefgen = (in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_PHY_STATUS_ADDR) &
                  HWIO_DSI_0_PHY_DSIPHY_CMN_PHY_STATUS_REFGEN_READY_BMSK);
    if (0 == uRefgen)
    {
      DSI_OSAL_SleepUs(100);  /*delay ~100us*/
      uTimeout--;
      if (0 == uTimeout)
      {
        /* timeout while polling the lock status */
        eStatus = HAL_MDSS_DSI_FAILED_UNABLE_TO_INIT_HW;
        DSI_Log_Message(DSI_LOGLEVEL_WARN, "Wait for Refgen timed out on DSI %d after %d microsecs\n", eDeviceId,
               HAL_DSI_PHY_REFGEN_TIMEOUT * 100);
        break;
      }
    }
  }
  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_PowerDown()
*/
/*!
* \brief
*     Power down the PHY
*
* \param [in]  eDeviceId - DSI core ID
* \param [in]  pSettings - pointer to phy settings structure
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_PowerDown(DSI_Device_IDType eDeviceId,
                                  HAL_DSI_PHY_PhySettingsType  *pSettings)
{
  uintPtr            uOffset     = HAL_DSI_GetRegBaseOffset(eDeviceId);
  uint32             uRegVal     = 0;

  /* power up DIGTOP & PLL */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CTRL_0_ADDR, 0x00000060);

  /* Disable PLL */
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_PLL_CNTRL, PLL_START, 0);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_PLL_CNTRL_ADDR, uRegVal);

  /* Resync re-time FIFO OFF*/
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_RBUF_CTRL_ADDR, 0x0);
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_ResyncFIFO()
*/
/*!
* \brief
*     Resync FIFO on DSI Phy
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_ResyncFIFO(DSI_Device_IDType eDeviceId)
{
  uintPtr            uOffset     = HAL_DSI_GetRegBaseOffset(eDeviceId);

  /* Resync FIFO*/
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_RBUF_CTRL_ADDR, 0x00000001);
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_PllLockDetect()
*/
/*!
* \brief
*     Wait and check if PLL locked.
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_PllLockDetect(DSI_Device_IDType eDeviceId)
{
  HAL_MDSS_ErrorType eStatus     = HAL_MDSS_STATUS_SUCCESS;
  uintPtr            uOffset     = HAL_DSI_GetRegBaseOffset(eDeviceId);
  uint32             uRegVal     = 0;
  uint32             uTimeout    = HAL_DSI_PHY_PLL_READY_TIMEOUT;
  uint32             uPllStatus;

  /* Enable PLL */
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_PLL_CNTRL, PLL_START, 1);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_PLL_CNTRL_ADDR, uRegVal);


  /* Wait for Lock */
  uPllStatus = (in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_COMMON_STATUS_ONE_ADDR) &
                   HWIO_DSI_0_PHY_DSIPHY_PLL_COMMON_STATUS_ONE_CORE_READY_STATUS_BMSK);

  while ((0 == uPllStatus) && (uTimeout))
  {
    DSI_OSAL_SleepUs(100);  /*delay ~100us*/
    uPllStatus = (in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_COMMON_STATUS_ONE_ADDR) &
                     HWIO_DSI_0_PHY_DSIPHY_PLL_COMMON_STATUS_ONE_CORE_READY_STATUS_BMSK);
    uTimeout--;
  }

  if (0 == (uPllStatus & HWIO_DSI_0_PHY_DSIPHY_PLL_COMMON_STATUS_ONE_CORE_READY_STATUS_BMSK))
  {
    /* timeout while polling the lock status */
    eStatus = HAL_MDSS_DSI_FAILED_UNABLE_TO_INIT_HW;
  }
  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CalculateTimingParams()
*/
/*!
* \brief
*     Calculate timing parameters
*
* \param [in]  pTimingParameters - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_CalculateTimingParams(HAL_DSI_TimingSettingType *pTimingParameters,
                                                            HAL_DSI_PHY_TimingConfigType *psTimingConfig)
{
  HAL_MDSS_ErrorType     eStatus                = HAL_MDSS_STATUS_SUCCESS;
  DSI_TimingOverrideType *pPreDefinedTimings    = pTimingParameters->pPreDefinedTimings;
  float                  fMin;
  float                  fMax;
  float                  fT_clk_prepare_actual;
  float                  fT_clk_zero_actual;
  float                  fT_hs_prepare_actual;
  float                  fT_hs_rqst_clk_actual;
  float                  fTEOT_data;
  float                  fTEOT_clock;
  float                  fUiBitclk              = HAL_DSI_PHY_TIPX_NUMERATOR / (float)pTimingParameters->uBitclock;
  float                  fVal;
  int32                  iMin;
  int32                  iMax;
  int32                  iVal;
  uint32                 uT_clk_prepare;
  uint32                 uT_clk_zero;
  uint32                 uT_clk_trail;
  uint32                 uT_hs_prepare;
  uint32                 uT_hs_zero;
  uint32                 uT_hs_trail;
  uint32                 uT_hs_rqst;
  uint32                 uHalfByteClkEn;
  uint32                 uT_hs_exit;
  uint32                 uT_ta_go;
  uint32                 uT_ta_sure;
  uint32                 uT_ta_get;
  uint32                 uT_clk_post;
  uint32                 uT_clk_pre;
  bool32                 bT_clk_pre_inc_by_2    = FALSE;

  fTEOT_data  = HAL_DSI_PHY_TEOT_PARAM1 + HAL_DSI_PHY_TEOT_PARAM2 * fUiBitclk;
  fTEOT_clock = HAL_DSI_PHY_TEOT_PARAM1 + HAL_DSI_PHY_TEOT_PARAM2 * fUiBitclk;

  /**********************************************************************************/
  /* HSTX_HALFBYTECLK_EN calculation */
  uHalfByteClkEn = 0;

  /**********************************************************************************/
  /* T_CLK_PREPARE calculation */
  if (pPreDefinedTimings->bTimingCLKPrepareOverride)
  {
    uT_clk_prepare = pPreDefinedTimings->uTimingCLKPrepareValue;
  }
  else
  {
    fMin = 0;
    fMax = 0;

    /* calculate the min and max in float */
    fVal = (HAL_DSI_PHY_TCLK_PREPARE_PHY_MIN / fUiBitclk) - (HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM1 * uHalfByteClkEn);
    iMin = HAL_DSI_Phy_Roundup(fVal / HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM2);
    fMin = (float)HAL_DSI_Phy_CheckNegative(iMin);

    fVal = (HAL_DSI_PHY_TCLK_PREPARE_PHY_MAX / fUiBitclk) - (HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM1 * uHalfByteClkEn);
    iMax = HAL_DSI_Phy_Rounddown(fVal / HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM2);
    fMax = (float)HAL_DSI_Phy_CheckNegative(iMax);

    /* interpolate the value at 10% between min and max */
    iVal = HAL_DSI_Phy_Roundup((fMax - fMin) * (HAL_DSI_PHY_RANGEFACTOR_TCLK_PREPARE / HAL_DSI_PHY_PERCENT_DENOMENATOR) + fMin);
    uT_clk_prepare = HAL_DSI_Phy_CheckNegative(iVal);
  }

  fT_clk_prepare_actual = ((uT_clk_prepare * HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM2) +
                           (uHalfByteClkEn * HAL_DSI_PHY_TCLK_PREPARE_PHY_PARAM3)) * fUiBitclk;

  /**********************************************************************************/
  /* T_CLK_ZERO calculation */
  if (pPreDefinedTimings->bTimingCLKZeroOverride)
  {
    uT_clk_zero = pPreDefinedTimings->uTimingCLKZeroValue;
  }
  else
  {
    fMin = HAL_DSI_PHY_TCLK_ZERO_PARAM1 - fT_clk_prepare_actual;
    iMin = HAL_DSI_Phy_Roundup((fMin / fUiBitclk) / HAL_DSI_PHY_TCLK_ZERO_PARAM3) - 1;
    iMax = (iMin > HAL_DSI_PHY_TCLK_ZERO_PARAM5) ? HAL_DSI_PHY_TCLK_ZERO_PARAM6 : HAL_DSI_PHY_TCLK_ZERO_PARAM5;
    iVal = HAL_DSI_Phy_Roundup((iMax - iMin) * (HAL_DSI_PHY_TCLK_ZERO_PARAM7 / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMin);
    uT_clk_zero = HAL_DSI_Phy_CheckNegative(iVal);
  }

  fT_clk_zero_actual = ((uT_clk_zero + HAL_DSI_PHY_TCLK_ZERO_PARAM4) * HAL_DSI_PHY_TCLK_ZERO_PARAM3) * fUiBitclk;

  /**********************************************************************************/
  /* T_CLK_TRAIL calculation */
  if (pPreDefinedTimings->bTimingCLKTrailOverride)
  {
    uT_clk_trail = pPreDefinedTimings->uTimingCLKTrailValue;
  }
  else
  {
    fMin = HAL_DSI_PHY_TCLK_TRAIL_MIN;
    fMax = fTEOT_clock - HAL_DSI_PHY_TREOT;
    iMin = HAL_DSI_Phy_Roundup((fMin / fUiBitclk) / HAL_DSI_PHY_T_T_TRAIL_PARAM4) - HAL_DSI_PHY_T_T_TRAIL_PARAM5;
    iMax = HAL_DSI_Phy_Rounddown((fMax / fUiBitclk) / HAL_DSI_PHY_T_T_TRAIL_PARAM4) - HAL_DSI_PHY_T_T_TRAIL_PARAM5;
    iVal = HAL_DSI_Phy_Roundup((iMax - iMin) * (HAL_DSI_PHY_RANGEFACTOR_TRAIL / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMin);
    uT_clk_trail = HAL_DSI_Phy_CheckNegative(iVal);
  }
  /* fT_clk_trail_actual = ((uT_clk_trail + HAL_DSI_PHY_T_T_TRAIL_PARAM3) * HAL_DSI_PHY_T_T_TRAIL_PARAM4) * fUiBitclk; */

  /**********************************************************************************/
  /* T_HS_PREPARE calculation */
  if (pPreDefinedTimings->bTimingHSPrepareOverride)
  {
    uT_hs_prepare = pPreDefinedTimings->uTimingHSPrepareValue;
  }
  else
  {
    fMin = HAL_DSI_PHY_T_HS_PREPARE_PARAM1 + HAL_DSI_PHY_T_HS_PREPARE_PARAM2 * fUiBitclk;
    fMax = HAL_DSI_PHY_T_HS_PREPARE_PARAM3 + HAL_DSI_PHY_T_HS_PREPARE_PARAM4 * fUiBitclk;

    fVal = ((fMin / fUiBitclk) - HAL_DSI_PHY_T_HS_PREPARE_PARAM5 * uHalfByteClkEn);
    iVal = HAL_DSI_Phy_Roundup(fVal / HAL_DSI_PHY_T_HS_PREPARE_PARAM6);
    iMin = HAL_DSI_Phy_CheckNegative(iVal);

    fVal = ((fMax / fUiBitclk) - HAL_DSI_PHY_T_HS_PREPARE_PARAM5 * uHalfByteClkEn);
    iVal = HAL_DSI_Phy_Rounddown(fVal / HAL_DSI_PHY_T_HS_PREPARE_PARAM6);
    iMax = HAL_DSI_Phy_CheckNegative(iVal);

    iVal = HAL_DSI_Phy_Roundup((iMax - iMin) * (HAL_DSI_PHY_RANGEFACTOR_THS_PREPARE / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMin);
    uT_hs_prepare = HAL_DSI_Phy_CheckNegative(iVal);
  }

  fVal = (uT_hs_prepare * HAL_DSI_PHY_T_HS_PREPARE_PARAM6) + (uHalfByteClkEn * HAL_DSI_PHY_T_HS_PREPARE_PARAM5);
  fT_hs_prepare_actual = fVal * fUiBitclk;

  /**********************************************************************************/
  /* T_HS_ZERO calculation */
  if (pPreDefinedTimings->bTimingHSZeroOverride)
  {
    uT_hs_zero = pPreDefinedTimings->uTimingHSZeroValue;
  }
  else
  {
    fMin = HAL_DSI_PHY_T_HS_ZERO_PARAM1 + HAL_DSI_PHY_T_HS_ZERO_PARAM2 * fUiBitclk - fT_hs_prepare_actual;
    iMin = HAL_DSI_Phy_Roundup((fMin / fUiBitclk) / HAL_DSI_PHY_T_HS_ZERO_PARAM4) - HAL_DSI_PHY_T_HS_ZERO_PARAM5;
    iMax = HAL_DSI_PHY_T_HS_ZERO_PARAM6;
    iVal = HAL_DSI_Phy_Roundup((iMax - iMin) * (HAL_DSI_PHY_RANGEFACTOR_THS_ZERO / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMin);
    uT_hs_zero = HAL_DSI_Phy_CheckNegative(iVal);
  }

  /**********************************************************************************/
  /* T_HS_TRAIL calculation */
  if (pPreDefinedTimings->bTimingHSTrailOverride)
  {
    uT_hs_trail = pPreDefinedTimings->uTimingHSTrailValue;
  }
  else
  {
    fMin = HAL_DSI_PHY_T_T_TRAIL_PARAM1 + (HAL_DSI_PHY_T_T_TRAIL_PARAM2 * fUiBitclk);
    fMax = fTEOT_data - HAL_DSI_PHY_TREOT;
    iMin = HAL_DSI_Phy_Roundup((fMin / fUiBitclk) / HAL_DSI_PHY_T_T_TRAIL_PARAM4) - HAL_DSI_PHY_T_T_TRAIL_PARAM5;
    iMax = HAL_DSI_Phy_Rounddown((fMax / fUiBitclk) / HAL_DSI_PHY_T_T_TRAIL_PARAM4) - HAL_DSI_PHY_T_T_TRAIL_PARAM5;
    iVal = HAL_DSI_Phy_Roundup((iMax - iMin) * (HAL_DSI_PHY_RANGEFACTOR_TRAIL / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMin);
    uT_hs_trail = HAL_DSI_Phy_CheckNegative(iVal);
  }

  /**********************************************************************************/
  /* T_HS_RQST is user entry based on UI/ESCCLK */
  uT_hs_rqst = 0;
  if (pPreDefinedTimings->bTimingHSRequestOverride)
  {
    uT_hs_rqst = pPreDefinedTimings->uTimingHSRequestValue;
  }
  else
  {
    fVal = (HAL_DSI_PHY_T_HS_RQST_PARAM1 / fUiBitclk) + (HAL_DSI_PHY_T_HS_RQST_PARAM2 * uHalfByteClkEn);
    iVal = HAL_DSI_Phy_Roundup((fVal - HAL_DSI_PHY_T_HS_RQST_PARAM3) / HAL_DSI_PHY_T_HS_RQST_PARAM3);
    uT_hs_rqst = HAL_DSI_Phy_CheckNegative(iVal);
  }

  fVal = (uT_hs_rqst * HAL_DSI_PHY_T_HS_RQST_PARAM3) + HAL_DSI_PHY_T_HS_RQST_PARAM3;
  fVal = fVal - (HAL_DSI_PHY_T_HS_RQST_PARAM2 * uHalfByteClkEn);
  fT_hs_rqst_clk_actual = fVal * fUiBitclk;

  /**********************************************************************************/
  /* T_HS_EXIT calculation */
  if (pPreDefinedTimings->bTimingHSExitOverride)
  {
    uT_hs_exit = pPreDefinedTimings->uTimingHSExitValue;
  }
  else
  {
    fVal = HAL_DSI_PHY_T_HS_EXIT_MIN / fUiBitclk / HAL_DSI_PHY_T_HS_EXIT_PARAM1;
    iMin = HAL_DSI_Phy_Roundup(fVal) - HAL_DSI_PHY_T_HS_EXIT_PARAM2;
    iMax = HAL_DSI_PHY_T_HS_EXIT_MAX;
    iVal = HAL_DSI_Phy_Roundup((iMax - iMin) * (HAL_DSI_PHY_RANGEFACTOR_NOMAX / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMin);
    uT_hs_exit = HAL_DSI_Phy_CheckNegative(iVal);
  }

  /**********************************************************************************/
  /* T_TA_GO calculation (Hard coded value) */
  uT_ta_go = HAL_DSI_PHY_T_TA_GO_PARAM1;
  if (pPreDefinedTimings->bTimingTAGoOverride)
  {
    uT_ta_go = pPreDefinedTimings->uTimingTAGoValue;
  }

  /**********************************************************************************/
  /* T_TA_SURE calculation (Hard coded value) */
  uT_ta_sure = HAL_DSI_PHY_T_TA_SURE_PARAM1;
  if (pPreDefinedTimings->bTimingTASureOverride)
  {
    uT_ta_sure = pPreDefinedTimings->uTimingTASureValue;
  }

  /**********************************************************************************/
  /* T_TA_GET calculation (Hard coded value) */
  uT_ta_get = HAL_DSI_PHY_T_TA_GET_PARAM1;
  if (pPreDefinedTimings->bTimingTAGetOverride)
  {
    uT_ta_get = pPreDefinedTimings->uTimingTAGetValue;
  }

  /**********************************************************************************/
  /* T_CLK_POST calculation */
  if (pPreDefinedTimings->bTimingCLKPostOverride)
  {
    uT_clk_post = pPreDefinedTimings->uTimingCLKPostValue;
  }
  else
  {
    fMin = HAL_DSI_PHY_T_CLK_POST_PARAM1 + HAL_DSI_PHY_T_CLK_POST_PARAM2 * fUiBitclk;
    fVal = (fMin - HAL_DSI_PHY_T_CLK_POST_PARAM3 * fUiBitclk) / (HAL_DSI_PHY_T_CLK_POST_PARAM4 * fUiBitclk);
    iMin = HAL_DSI_Phy_Roundup(fVal) - HAL_DSI_PHY_T_CLK_POST_PARAM5;

    iMax = HAL_DSI_PHY_T_CLK_POST_MAX;
    iVal = HAL_DSI_Phy_Roundup((iMax - iMin) * (HAL_DSI_PHY_RANGEFACTOR_NOMAX / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMin);
    uT_clk_post = (iVal > 0) ? iVal : 0;
  }


  /**********************************************************************************/
  /* T_CLK_PRE calculation */
  if (pPreDefinedTimings->bTimingCLKPreOverride)
  {
    uT_clk_pre = pPreDefinedTimings->uTimingCLKPreValue;
  }
  else
  {
    fMin = HAL_DSI_PHY_T_CLK_PRE_PARAM1 * fUiBitclk;
    fVal = (fMin + fT_clk_prepare_actual + fT_clk_zero_actual + fT_hs_rqst_clk_actual);
    iMin = HAL_DSI_Phy_Roundup(fVal / (HAL_DSI_PHY_T_CLK_PRE_PARAM2 * fUiBitclk)) - HAL_DSI_PHY_T_CLK_PRE_PARAM4;
    iMax = HAL_DSI_PHY_T_CLK_PRE_MAX;
    if (iMin > iMax)
    {
      fVal = ((iMax * HAL_DSI_PHY_T_CLK_PRE_PARAM3) - iMin) * (HAL_DSI_PHY_RANGEFACTOR_NOMAX / HAL_DSI_PHY_PERCENT_DENOMENATOR);
      iVal = HAL_DSI_Phy_Roundup(fVal + iMin);
      iVal = HAL_DSI_Phy_Rounddown(iVal / HAL_DSI_PHY_T_CLK_PRE_PARAM3);
      bT_clk_pre_inc_by_2 = TRUE;
    }
    else
    {
      iVal = HAL_DSI_Phy_Roundup((iMax - iMin) * (HAL_DSI_PHY_RANGEFACTOR_NOMAX / HAL_DSI_PHY_PERCENT_DENOMENATOR) + iMin);
      bT_clk_pre_inc_by_2 = FALSE;
    }
    uT_clk_pre = (iVal < 0) ? 0 : iVal;
  }

  /**********************************************************************************/
  /*   Up to here, all DSI timing parameters have been calculated                   */
  /*   now, assign the parameters and program the HW                                */
  /**********************************************************************************/

  /* Assign data lane timing parameters */
  psTimingConfig->bHSTX_HalfByteClk_En = (uHalfByteClkEn > 0) ? TRUE : FALSE;
  psTimingConfig->uT_HS_PREPARE        = uT_hs_prepare;
  psTimingConfig->uT_HS_EXIT           = uT_hs_exit;
  psTimingConfig->uT_HS_RQST           = uT_hs_rqst;
  psTimingConfig->uT_HS_TRAIL          = uT_hs_trail;
  psTimingConfig->uT_HS_ZERO           = uT_hs_zero;
  psTimingConfig->uT_TA_GET            = uT_ta_get;
  psTimingConfig->uT_TA_GO             = uT_ta_go;
  psTimingConfig->uT_TA_SURE           = uT_ta_sure;
  psTimingConfig->uT_CLK_PREPARE       = uT_clk_prepare;
  psTimingConfig->uT_CLK_ZERO          = uT_clk_zero;
  psTimingConfig->uT_CLK_TRAIL         = uT_clk_trail;
  psTimingConfig->uT_CLK_PRE           = uT_clk_pre;
  psTimingConfig->uT_CLK_POST          = uT_clk_post;
  psTimingConfig->bT_CLK_PRE_INC_BY_2  = bT_clk_pre_inc_by_2;

  return eStatus;

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CalculateTimingParams_CPhy()
*/
/*!
* \brief
*     Calculate C-Phy timing parameters
*
* \param [in]  pTimingParameters - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_CalculateTimingParams_CPhy(HAL_DSI_TimingSettingType *pTimingParameters,
                                                                 HAL_DSI_PHY_TimingConfigType *psTimingConfig)
{
  HAL_MDSS_ErrorType     eStatus                = HAL_MDSS_STATUS_SUCCESS;
  DSI_TimingOverrideType *pPreDefinedTimings    = pTimingParameters->pPreDefinedTimings;
  float                  fMin;
  float                  fMax;
  float                  fUiBitclk;
  float                  fVal;
  int32                  iMin;
  int32                  iMax;
  int32                  iVal;
  uint32                 uHalfByteClkEn;
  uint32                 uT_hs_exit;
  uint32                 uT_prebegin;
  uint32                 uT_prepare;
  uint32                 uT_post;
  uint32                 uT_hs_rqst;
  uint32                 uT_ta_go;
  uint32                 uT_ta_sure;
  uint32                 uT_ta_get;

  if (0 == pTimingParameters->uBitclock)
  {
    eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
  }
  else
  {
     fUiBitclk              = HAL_DSI_PHY_TIPX_NUMERATOR / (float)pTimingParameters->uBitclock;

     /**********************************************************************************/
     /* TIMING_CTRL_0, HSTX_HALFBYTECLK_EN calculation */
     if (HAL_DSI_CPHY_HALFBYTECLK_EN_THRESHOLD > pTimingParameters->uBitclock)
     {
       uHalfByteClkEn = 1;
     }
     else
     {
       uHalfByteClkEn = 0;
     }

     /* TIMING_CTRL_4, T3_HS_EXIT */
     if (pPreDefinedTimings->bTimingHSExitOverride)
     {
       uT_hs_exit = pPreDefinedTimings->uTimingHSExitValue;
     }
     else
     {
       fVal = HAL_DSI_CPHY_T_HS_EXIT_MIN / fUiBitclk;
       iMin = HAL_DSI_Phy_Roundup(fVal / HAL_DSI_CPHY_BITS_BYTE);
       iMin -= 1;
       fMin = (float)HAL_DSI_Phy_CheckNegative(iMin);
       fMax = HAL_DSI_CPHY_T_HS_EXIT_MAX;
       iVal = HAL_DSI_Phy_Roundup((fMax - fMin) * (HAL_DSI_CPHY_RANGEFACTOR_0 / HAL_DSI_PHY_PERCENT_DENOMENATOR) + fMin);
       uT_hs_exit = HAL_DSI_Phy_CheckNegative(iVal);
     }

	
     /* TIMING_CTRL_5, T_HS_ZERO/T3_PREBEGIN */
     if (pPreDefinedTimings->bTimingHSZeroOverride)
     {
       uT_prebegin = pPreDefinedTimings->uTimingHSZeroValue;
     }
     else
     {
       fVal = HAL_DSI_CPHY_T3_PREBEGIN_CNT_MIN * fUiBitclk;
       fVal = 1.0f;
       iMin = HAL_DSI_Phy_Roundup((fVal / fUiBitclk) / HAL_DSI_CPHY_BITS_BYTE);
       iMin -= 1;
       fMin = (float)HAL_DSI_Phy_CheckNegative(iMin);
       fVal = HAL_DSI_CPHY_T3_PREBEGIN_CNT_MAX * fUiBitclk;
       iMax = HAL_DSI_Phy_Rounddown((fVal / fUiBitclk) / HAL_DSI_CPHY_BITS_BYTE);
       iMax -= 1;
       fMax = (float)HAL_DSI_Phy_CheckNegative(iMax);
       iVal = HAL_DSI_Phy_Roundup((fMax - fMin) * (HAL_DSI_CPHY_RANGEFACTOR_0 / HAL_DSI_PHY_PERCENT_DENOMENATOR) + fMin);
       uT_prebegin = HAL_DSI_Phy_CheckNegative(iVal);
     }


     /* TIMING_CTRL_6, T3_PREPARE/T_HS_PREPARE */
     if (pPreDefinedTimings->bTimingHSPrepareOverride)
     {
       uT_prepare = pPreDefinedTimings->uTimingHSPrepareValue;
     }
     else
     {
       /* calculate the min and max in float */
       fVal = ((float)HAL_DSI_CPHY_T3_PREPARE_MIN / fUiBitclk) - (HAL_DSI_CPHY_BITS_SYMBOL * uHalfByteClkEn);
       iMin = HAL_DSI_Phy_Roundup(fVal / HAL_DSI_CPHY_BITS_BYTE);
       fMin = (float)HAL_DSI_Phy_CheckNegative(iMin);
       fVal = ((float)HAL_DSI_CPHY_T3_PREPARE_MAX / fUiBitclk) - (HAL_DSI_CPHY_BITS_SYMBOL * uHalfByteClkEn);
       iMax = HAL_DSI_Phy_Rounddown(fVal / HAL_DSI_CPHY_BITS_BYTE);
       fMax = (float)HAL_DSI_Phy_CheckNegative(iMax);
       iVal = HAL_DSI_Phy_Roundup((fMax - fMin) * (HAL_DSI_CPHY_RANGEFACTOR_0 / HAL_DSI_PHY_PERCENT_DENOMENATOR) + fMin);
       uT_prepare = HAL_DSI_Phy_CheckNegative(iVal);
     }


     /* TIMING_CTRL_7, T3_POST/T_HS_TRAIL */
     if (pPreDefinedTimings->bTimingHSTrailOverride)
     {
       uT_post = pPreDefinedTimings->uTimingHSTrailValue;
     }
     else
     {
       fVal = HAL_DSI_CPHY_T3_POST_CNT_MIN * fUiBitclk;
       fVal = 1.0f;
       iMin = HAL_DSI_Phy_Roundup((fVal/ fUiBitclk) / HAL_DSI_CPHY_BITS_BYTE);
       iMin -= 1;
       fMin = (float)HAL_DSI_Phy_CheckNegative(iMin);
       fVal = HAL_DSI_CPHY_T3_POST_CNT_MAX * fUiBitclk;
       iMax = HAL_DSI_Phy_Roundup((fVal / fUiBitclk) / HAL_DSI_CPHY_BITS_BYTE);
       iMax -= 1;
       fMax = (float)HAL_DSI_Phy_CheckNegative(iMax);
       iVal = HAL_DSI_Phy_Roundup((fMax - fMin) * (HAL_DSI_CPHY_RANGEFACTOR_0 / HAL_DSI_PHY_PERCENT_DENOMENATOR) + fMin);
       uT_post = HAL_DSI_Phy_CheckNegative(iVal);
     }


     /* TIMING_CTRL_8, T_HS_RQST */
     uT_hs_rqst = 0;
     if (pPreDefinedTimings->bTimingHSRequestOverride)
     {
       uT_hs_rqst = pPreDefinedTimings->uTimingHSRequestValue;
     }
     else
     {
       fVal = (HAL_DSI_CPHY_T_HS_RQST_MIN / fUiBitclk) + (4 * uHalfByteClkEn) - HAL_DSI_CPHY_BITS_BYTE; 
       uT_hs_rqst = HAL_DSI_Phy_Roundup(fVal / HAL_DSI_CPHY_BITS_BYTE);
     }

     /* T_TA_GO calculation (Hard coded value) */
     uT_ta_go = HAL_DSI_PHY_T_TA_GO_PARAM1;
     if (pPreDefinedTimings->bTimingTAGoOverride)
     {
       uT_ta_go = pPreDefinedTimings->uTimingTAGoValue;
     }

     /* T_TA_SURE calculation (Hard coded value) */
     uT_ta_sure = HAL_DSI_PHY_T_TA_SURE_PARAM1;
     if (pPreDefinedTimings->bTimingTASureOverride)
     {
       uT_ta_sure = pPreDefinedTimings->uTimingTASureValue;
     }

     /* T_TA_GET calculation (Hard coded value) */
     uT_ta_get = HAL_DSI_PHY_T_TA_GET_PARAM1;
     if (pPreDefinedTimings->bTimingTAGetOverride)
     {
       uT_ta_get = pPreDefinedTimings->uTimingTAGetValue;
     }

     /**********************************************************************************/
     /*   Up to here, all DSI CPhy timing parameters have been calculated                   */
     /*   now, assign the parameters and program the HW                                */
     /**********************************************************************************/

     /* Assign data lane timing parameters */
     psTimingConfig->bHSTX_HalfByteClk_En = (uHalfByteClkEn > 0) ? TRUE : FALSE;
     psTimingConfig->uT_HS_EXIT           = uT_hs_exit;
     psTimingConfig->uT_HS_ZERO           = uT_prebegin;
     psTimingConfig->uT_HS_PREPARE        = uT_prepare;
     psTimingConfig->uT_HS_TRAIL          = uT_post;
     psTimingConfig->uT_HS_RQST           = uT_hs_rqst;
     psTimingConfig->uT_TA_SURE           = uT_ta_sure;
     psTimingConfig->uT_TA_GO             = uT_ta_go;
     psTimingConfig->uT_TA_GET            = uT_ta_get;
   }
   return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CommitTimingParams()
*/
/*!
* \brief
*     Config lane timing parameters
*
* \param [in]  eDeviceId     - DSI core ID
* \param [in]  pTimingConfig - pointer to lane timing parameter structure
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_CommitTimingParams(DSI_Device_IDType eDeviceId, HAL_DSI_PHY_TimingConfigType *pTimingConfig)
{
  uint32               uRegVal = 0;
  uintPtr              uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

  // Step 4 Common block including GlobalTiming Parameters
  /* BYTECLK_SEL */
  uRegVal = 0;
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_GLBL_CTRL, BYTECLK_SEL, 2);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_GLBL_CTRL_ADDR, uRegVal);

  /* VREG_CTRL */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_VREG_CTRL_ADDR, 0x00000059);

  /*HALFBYTECLK_EN*/
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_0_ADDR, pTimingConfig->bHSTX_HalfByteClk_En);

  /* T_CLK_ZERO */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_1_ADDR, pTimingConfig->uT_CLK_ZERO);
  /* T_CLK_PREPARE */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_2_ADDR, pTimingConfig->uT_CLK_PREPARE);
  /* T_CLK_TRAIL */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_3_ADDR, pTimingConfig->uT_CLK_TRAIL);
  /* T_HS_EXIT */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_4_ADDR, pTimingConfig->uT_HS_EXIT);
  /* T_HS_ZERO */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_5_ADDR, pTimingConfig->uT_HS_ZERO);
  /* T_HS_PREPARE */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_6_ADDR, pTimingConfig->uT_HS_PREPARE);
  /* T_HS_TRAIL */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_7_ADDR, pTimingConfig->uT_HS_TRAIL);
  /* T_HS_RQST */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_8_ADDR, pTimingConfig->uT_HS_RQST);
  /* T_TA_GO & T_TA_SURE */
  uRegVal = HWIO_OUT_FLD(0, DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_9, DSIPHY_T_TA_SURE, pTimingConfig->uT_TA_SURE);
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_9, DSIPHY_T_TA_GO, pTimingConfig->uT_TA_GO);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_9_ADDR, uRegVal);
  /* T_TA_GET */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_10_ADDR, pTimingConfig->uT_TA_GET);

  /*DSIPHY_TRIG3_CMD*/
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_11_ADDR, 0x00000000);

  /* DSI clock out timing ctrl T_CLK_PRE & T_CLK_POST*/
  uRegVal = 0;
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_CLKOUT_TIMING_CTRL, T_CLK_POST, pTimingConfig->uT_CLK_POST);
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_CLKOUT_TIMING_CTRL, T_CLK_PRE, pTimingConfig->uT_CLK_PRE);
  out_dword(uOffset + HWIO_DSI_0_CLKOUT_TIMING_CTRL_ADDR, uRegVal);

  /* T_CLK_PRE counter increment control */
  uRegVal = (pTimingConfig->bT_CLK_PRE_INC_BY_2) ? 1 : 0;
  out_dword(uOffset + HWIO_DSI_0_TEST_PATTERN_GEN_VIDEO_ENABLE_ADDR, uRegVal);

  /* DCTRL */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CTRL_2_ADDR, 0x00000040);

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CommitTimingParams_CPhy()
*/
/*!
* \brief
*     Config C-PHY lane timing parameters
*
* \param [in]  eDeviceId     - DSI core ID
* \param [in]  pTimingConfig - pointer to lane timing parameter structure
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_CommitTimingParams_CPhy(DSI_Device_IDType eDeviceId, HAL_DSI_PHY_TimingConfigType *pTimingConfig)
{
  uint32               uRegVal = 0;
  uintPtr              uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

  /* PHY MODE */
  uRegVal = 0;
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_GLBL_CTRL, PHY_MODE, 1);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_GLBL_CTRL_ADDR, uRegVal);

  /* VREG_CTRL */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_VREG_CTRL_ADDR, 0x00000059);

  /*HALFBYTECLK_EN*/
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_0_ADDR, pTimingConfig->bHSTX_HalfByteClk_En);

  /* T_HS_EXIT */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_4_ADDR, pTimingConfig->uT_HS_EXIT);
  /* T_HS_ZERO */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_5_ADDR, pTimingConfig->uT_HS_ZERO);
  /* T_HS_PREPARE */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_6_ADDR, pTimingConfig->uT_HS_PREPARE);
  /* T_HS_TRAIL */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_7_ADDR, pTimingConfig->uT_HS_TRAIL);
  /* T_HS_RQST */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_8_ADDR, pTimingConfig->uT_HS_RQST);
  /* T_TA_GO & T_TA_SURE */
  uRegVal = HWIO_OUT_FLD(0, DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_9, DSIPHY_T_TA_SURE, pTimingConfig->uT_TA_SURE);
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_9, DSIPHY_T_TA_GO, pTimingConfig->uT_TA_GO);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_9_ADDR, uRegVal);
  /* T_TA_GET */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_10_ADDR, pTimingConfig->uT_TA_GET);

  /*DSIPHY_TRIG3_CMD*/
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_TIMING_CTRL_11_ADDR, 0x00000000);

  /* DCTRL */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CTRL_2_ADDR, 0x00000040);

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_SetupLanePhy()
*/
/*!
* \brief
*     Setup DSI PHY
*
* \param [in]  eDeviceId     - DSI core ID
* \param [in]  eLaneId       - DSI lane ID
* \param [in]  pConfig       - pointer to PHY config data structure
* \param [in]  bDualDSI      - Indicates whether this is dual/single DSI usecase
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_SetupLanePhy(DSI_Device_IDType           eDeviceId,
                                                   DSI_LaneIDType              eLaneId,
                                                   HAL_DSI_PHY_LaneConfigType *pConfig,
                                                   bool32                      bDualDSI)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

  if(NULL == pConfig)
  {
    eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    uint32                  uRegVal       = 0;
    uintPtr                 uOffset       = HAL_DSI_GetRegBaseOffset(eDeviceId) + HAL_DSI_Phy_GetLaneOffset(eLaneId);
    uint32                  uLPRXCtrl     = 0;
    uint32                  uHSTXStrength = 0;
    uint32                  uClkLane      = 0;
    HAL_DSI_PHY_VersionType sVersionInfo;

    DSI_OSAL_MemZero(&sVersionInfo, sizeof(HAL_DSI_PHY_VersionType));

    // Get the DSI PHY version info
    HAL_DSI_Phy_GetVersionInfo(&sVersionInfo);

    //Step 5 for DLn0/1/2/3 and ClkLn
    if (DSI_LaneID_CLK == eLaneId)
    {
      uClkLane      = 1;
      uHSTXStrength = pConfig->uClockStrengthHS;
    }
    else
    {
      uClkLane      = 0;
      uHSTXStrength = pConfig->uDataStrengthHS;
    }

    if (DSI_LaneID_0 == eLaneId)
    {
      uLPRXCtrl = 3;
    }

    // DSIPHY_STR_LP_N
    // DSIPHY_STR_LP_P
    uRegVal = 0;
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_LPTX_STR_CTRL, DSIPHY_STR_LP_N, pConfig->uDataStrengthLP_N);
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_LPTX_STR_CTRL, DSIPHY_STR_LP_P, pConfig->uDataStrengthLP_P);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_LPTX_STR_CTRL_ADDR, uRegVal);

    /* DSIPHY_LPRX_EN */
    /* DSIPHY_CDRX_EN */
    /* Transition from 0 to 1 for DLN0-3 CLKLN stays 0 */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_LPRX_CTRL_ADDR, 0x00000000);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_LPRX_CTRL_ADDR, uLPRXCtrl);

    /* Pin Swap */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_PIN_SWAP_ADDR, 0x00000000);

    /* DSIPHY_HSTX_STR_HSTOP */
    /* DSIPHY_HSTX_STR_HSBOT */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_HSTX_STR_CTRL_ADDR, uHSTXStrength);

    /* PGM Delay */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG0_ADDR, 0x00000000);

    /* DLN0_CFG1 */
    uRegVal = 0;
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_CFG1, DSIPHY_STR_VALUE_OVERRIDE, pConfig->bStrengthOverride);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG1_ADDR, uRegVal);

    /* DLN0_CFG2 */
    uRegVal = 0;
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_CFG2, DSIPHY_PEMPH_STRBOT, pConfig->uPemphBottom);
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_CFG2, DSIPHY_PEMPH_STRTOP, pConfig->uPemphTop);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG2_ADDR, uRegVal);

    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_OFFSET_TOP_CTRL_ADDR, 0x00000000);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_OFFSET_BOT_CTRL_ADDR, 0x00000000);

    /* DSIPHY_LPRX_DLY */
    /* IS_CKLANE */
    uRegVal = 0;
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_CFG3, IS_CKLANE, uClkLane);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG3_ADDR, uRegVal);

    uRegVal = HAL_DSI_Phy_GetTxDctrl(eLaneId);

    if (sVersionInfo.uMinorVersion >= 0x2)
    {
      /* Split display enabled */
      if ((DSI_LaneID_3 == eLaneId) &&
          (TRUE == bDualDSI))
      {
        uRegVal |= 0x04;   /* bit 2, set up correct pll source  */
      }
    }

    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_TX_DCTRL_ADDR, uRegVal);
  }

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_Toogle_Dln3_Tx_Dctrl()
*/
/*!
* \brief
*     Toogle Data lane 3's Tx_Dctrl value for the specific Phy Device.
*
*
* \param [in]  eLainId - Lane id;
*
* \retval  Tx Dctrl value
*
****************************************************************************/
static void HAL_DSI_Phy_Toogle_Dln3_Tx_Dctrl(DSI_Device_IDType eDeviceId)
{
  uintPtr  uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId) + HAL_DSI_Phy_GetLaneOffset(DSI_LaneID_3);
  uint32   uRegVal = 0;

  uRegVal = in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_TX_DCTRL_ADDR);
  uRegVal &= ~0x01; /* clear bit 0 and keep all other bits including bit 2 */
  /* toggle bit 0 */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_TX_DCTRL_ADDR, (0x01 | uRegVal));
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_TX_DCTRL_ADDR, uRegVal);
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_SetupLanPhy_CPhy()
*/
/*!
* \brief
*     Setup DSI C-PHY Lane
*
* \param [in]  eDeviceId     - DSI core ID
* \param [in]  eLaneId       - DSI lane ID
* \param [in]  pConfig       - pointer to PHY config data structure
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_SetupLanePhy_CPhy(DSI_Device_IDType eDeviceId, DSI_LaneIDType eLaneId, HAL_DSI_PHY_LaneConfigType *pConfig)
{
  HAL_MDSS_ErrorType   eStatus       = HAL_MDSS_STATUS_SUCCESS;
  uint32               uRegVal       = 0;
  uintPtr              uOffset       = HAL_DSI_GetRegBaseOffset(eDeviceId) + HAL_DSI_Phy_GetLaneOffset(eLaneId);
  uint32               uLPRXCtrl     = 0;

  if ((DSI_LaneID_CLK == eLaneId) ||
      (DSI_LaneID_3 == eLaneId))
  {
    eStatus = HAL_MDSS_DSI_FAILED_PARAMETER_OUT_OF_RANGE;
  }
  else
  {
    if (DSI_LaneID_0 == eLaneId)
    {
      uLPRXCtrl = 3;
    }

    // DSIPHY_STR_LP_N
    // DSIPHY_STR_LP_P
    uRegVal = 0;
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_LPTX_STR_CTRL, DSIPHY_STR_LP_N, pConfig->uDataStrengthLP_N);
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_LPTX_STR_CTRL, DSIPHY_STR_LP_P, pConfig->uDataStrengthLP_P);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_LPTX_STR_CTRL_ADDR, uRegVal);

    /* DSIPHY_LPRX_EN */
    /* DSIPHY_CDRX_EN */
    /* Transition from 0 to 1 for DLN0, others stays 0 */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_LPRX_CTRL_ADDR, 0x00000000);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_LPRX_CTRL_ADDR, uLPRXCtrl);
  
    /* Pin Swap */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_PIN_SWAP_ADDR, 0x00000000);

    /* DSIPHY_HSTX_STR_HSTOP */
    /* DSIPHY_HSTX_STR_HSBOT */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_HSTX_STR_CTRL_ADDR, pConfig->uDataStrengthHS);

    /* PGM Delay */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG0_ADDR, 0x00000000);

    /* DLN0_CFG1 */
    uRegVal = 0;
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_CFG1, DSIPHY_STR_VALUE_OVERRIDE, pConfig->bStrengthOverride);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG1_ADDR, uRegVal);

    /* DLN0_CFG2 */
    uRegVal = 0;
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_CFG2, DSIPHY_PEMPH_STRBOT, pConfig->uPemphBottom);
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_DLN0_CFG2, DSIPHY_PEMPH_STRTOP, pConfig->uPemphTop);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG2_ADDR, uRegVal);


    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_OFFSET_TOP_CTRL_ADDR, 0x00000000);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_OFFSET_BOT_CTRL_ADDR, 0x00000000);

    /* longer delay to fix BTA problem */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_DLN0_CFG3_ADDR, 0x0000000f);
  }

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CommitPhy()
*/
/*!
* \brief
*     Configures the PHY hardware registers.
*
*
* \param [in]  eDeviceId     - DSI core ID
* \param [in]  pSettings     - pointer to phy settings structure
*
* \retval None
*
****************************************************************************/
static HAL_MDSS_ErrorType HAL_DSI_Phy_CommitPhy(DSI_Device_IDType             eDeviceId,
                                                HAL_DSI_PHY_PhySettingsType  *pSettings)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;
  uintPtr            uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

  /* Step 1 Wait for refgen */
  eStatus = HAL_DSI_Phy_WaitForRefgen(eDeviceId);

  if (HAL_MDSS_STATUS_SUCCESS == eStatus)
  {
    /*Step 2: Power down */
    HAL_DSI_Phy_PowerDown(eDeviceId, pSettings);

    /* Remove PLL, DIG and all lanes from pwrdn */
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CTRL_0_ADDR, 0x0000007F);

    /* STEP5: Lane Settings */
    if (pSettings->bCPhyMode)
    {
      /* Lane enable */
      out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL0_ADDR, 0x00000007);
      HAL_DSI_Phy_SetupLanePhy_CPhy(eDeviceId, DSI_LaneID_0,   &pSettings->sLaneConfig);
      HAL_DSI_Phy_SetupLanePhy_CPhy(eDeviceId, DSI_LaneID_1,   &pSettings->sLaneConfig);
      HAL_DSI_Phy_SetupLanePhy_CPhy(eDeviceId, DSI_LaneID_2,   &pSettings->sLaneConfig);
    }
    else
    {
      /* Lane enable */
      out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL0_ADDR, 0x0000001F);
      HAL_DSI_Phy_SetupLanePhy(eDeviceId, DSI_LaneID_0,   &pSettings->sLaneConfig, pSettings->bDualDSI);
      HAL_DSI_Phy_SetupLanePhy(eDeviceId, DSI_LaneID_1,   &pSettings->sLaneConfig, pSettings->bDualDSI);
      HAL_DSI_Phy_SetupLanePhy(eDeviceId, DSI_LaneID_2,   &pSettings->sLaneConfig, pSettings->bDualDSI);
      HAL_DSI_Phy_SetupLanePhy(eDeviceId, DSI_LaneID_3,   &pSettings->sLaneConfig, pSettings->bDualDSI);
      HAL_DSI_Phy_SetupLanePhy(eDeviceId, DSI_LaneID_CLK, &pSettings->sLaneConfig, pSettings->bDualDSI);
    }
  }
  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CommitPll()
*/
/*!
* \brief
*     Setup DSI PLL
*
* \param [in]  eDeviceId         - DSI core ID
* \param [in]  ePLLConfigSource  - PLL source
* \param [in]  pPllConfig        - pointer to PLL config data structure
*
* \retval None
*
****************************************************************************/
static void HAL_DSI_Phy_CommitPll(DSI_Device_IDType            eDeviceId,
                                  HAL_DSI_PLLConfigSourceType  ePLLConfigSource,
                                  HAL_DSI_PHY_PllConfigType   *pPllConfig)
{
  uintPtr uOffset     = HAL_DSI_GetRegBaseOffset(eDeviceId);
  uint32  uRegVal     = 0;
  uint32  uPllPostDiv = 0;
  uint32  i           = 0;

  /* Step 3: PLL programming */

  /* PLL core input override */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_CORE_INPUT_OVERRIDE_ADDR, 0x12);

  /* Dec div start */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_DECIMAL_DIV_START_1_ADDR, pPllConfig->uDecDivStart);

  /* Frac div low part */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FRAC_DIV_START_LOW_1_ADDR, pPllConfig->uFracDivLow);

  /* Frac div mid part */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FRAC_DIV_START_MID_1_ADDR, pPllConfig->uFracDivMid);

  /* Frac div high part */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FRAC_DIV_START_HIGH_1_ADDR, pPllConfig->uFracDivHigh);

  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PLL_PROP_GAIN_RATE_1_ADDR, pPllConfig->uPropGainRate);

  /* Set BIAS_EN_MUX, BIAS_EN */
  uRegVal = 0;
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_PLL_SYSTEM_MUXES, CM_BIAS_EN_MUX, 1);
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_PLL_SYSTEM_MUXES, CM_BIAS_EN, 1);
  /* pll system muxes */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_SYSTEM_MUXES_ADDR, uRegVal);

  /* lockdet */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PLL_LOCKDET_RATE_1_ADDR, pPllConfig->uLockDetRate);

  /* Output divider */
  uPllPostDiv = 0;
  for (i = 0; i < 4; i++)
  {
    if (pPllConfig->uPllPostDiv & (1 << i))
    {
      uPllPostDiv = i;
      break;
    }
  }

  uRegVal = 0;
  uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_PLL_PLL_OUTDIV_RATE,  PLL_OUTDIV_1, uPllPostDiv);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PLL_OUTDIV_RATE_ADDR, uRegVal);

  /* Pll lock delay */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PLL_LOCK_DELAY_ADDR, pPllConfig->uLockDelay);

  /* clock inverters */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_CLOCK_INVERTERS_ADDR, pPllConfig->uClockInverters);

  //Step 3B
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_CMODE_ADDR,                        0x00000010);

  /* Frequency independent registers */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_ANALOG_CONTROLS_ONE_ADDR,          0x00000080);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_ANALOG_CONTROLS_TWO_ADDR,          0x00000003);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_ANALOG_CONTROLS_THREE_ADDR,        0x00000000);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_DSM_DIVIDER_ADDR,                  0x00000000);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FEEDBACK_DIVIDER_ADDR,             0x0000004E);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_CALIBRATION_SETTINGS_ADDR,         0x00000040);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_BAND_SEL_CAL_SETTINGS_THREE_ADDR,  0x000000BA);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DETECT_SETTINGS_ONE_ADDR,     0x0000000C);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_OUTDIV_ADDR,                       0x00000000);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_CORE_OVERRIDE_ADDR,                0x00000000);
  out_dword(uOffset + HWIO_DSI_1_PHY_DSIPHY_PLL_PLL_DIGITAL_TIMERS_TWO_ADDR,       0x00000008);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PLL_BAND_SEL_RATE_1_ADDR,          0x000000C0);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PLL_INT_GAIN_IFILT_BAND_1_ADDR,    0x000000FA);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PLL_FL_INT_GAIN_PFILT_BAND_1_ADDR, 0x0000004C);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PLL_LOCK_OVERRIDE_ADDR,            0x00000080);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_PFILT_ADDR,                        0x00000029);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_IFILT_ADDR,                        0x0000003F);

  /* #TODO# Placeholder for SSC - to be enabled later*/
#if 0
  /* SSC Div PER */
  uRegVal = pPllConfig->uSSCPER & HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_DIV_PER_LOW_RMSK;
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_DIV_PER_LOW_ADDR, uRegVal);

  uRegVal = (pPllConfig->uSSCPER >> 8) & HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_DIV_PER_HIGH_RMSK;
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_DIV_PER_HIGH_ADDR, uRegVal);

  /* SSC Step Size */
  uRegVal = pPllConfig->uSSCStepSize & HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_STEPSIZE_LOW_RMSK;
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_STEPSIZE_LOW_ADDR, uRegVal);

  uRegVal = (pPllConfig->uSSCStepSize >> 8) & HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_STEPSIZE_HIGH_RMSK;
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_STEPSIZE_HIGH_ADDR, uRegVal);

  /* SSC Adj PER */
  uRegVal = pPllConfig->uSSCAdjPER & HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_ADJPER_LOW_RMSK;
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_ADJPER_LOW_ADDR, uRegVal);

  uRegVal = (pPllConfig->uSSCAdjPER >> 8) & HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_ADJPER_HIGH_RMSK;
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_SSC_ADJPER_HIGH_ADDR, uRegVal);

  /* Freq detect */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DETECT_THRESH_ADDR, pPllConfig->uFreqDetThresh);

  /* Freq det refclk high */
  uRegVal = ( pPllConfig->uFreqDetRefclk >> 8 ) & HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DET_REFCLK_HIGH_RMSK;
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DET_REFCLK_HIGH_ADDR, uRegVal);

  /* Freq det refclk low */
  uRegVal = ( pPllConfig->uFreqDetRefclk & HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DET_REFCLK_LOW_RMSK);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DET_REFCLK_LOW_ADDR, uRegVal);

  /* Freq det PLL clk high */
  uRegVal = ( pPllConfig->uFreqDetPllclk >> 8 ) & HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DET_PLLCLK_HIGH_RMSK;
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DET_PLLCLK_HIGH_ADDR, uRegVal);

  /* Freq det PLL clk low */
  uRegVal = ( pPllConfig->uFreqDetPllclk & HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DET_PLLCLK_LOW_RMSK);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_FREQ_DET_PLLCLK_LOW_ADDR, uRegVal);
#endif

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_ResetPhy()
*/
/*!
* \brief
* 
*
* \param [in]  eDeviceId - DSI core ID
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
static void HAL_DSI_Phy_ResetPhy(DSI_Device_IDType eDeviceId)
{
  uintPtr uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CTRL_1_ADDR, HWIO_DSI_0_PHY_DSIPHY_CMN_CTRL_1_DSIPHY_SW_RESET_BMSK);
  DSI_OSAL_SleepUs(1000);          /* delay ~1ms */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CTRL_1_ADDR, 0x0);

  return;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_LaneMapConfig()
*/
/*!
* \brief
*     Configure the Lane Map
*
* \param [in]  eDeviceId     - DSI core ID
* \param [in]  paDisplayLane - pointer to lane map array
*
* \retval None
*
****************************************************************************/
static HAL_MDSS_ErrorType  HAL_DSI_Phy_LaneMapConfig(DSI_Device_IDType  eDeviceID,
                                                     DSI_LaneMapType    *paDisplayLane)
{
  HAL_MDSS_ErrorType  eStatus           = HAL_MDSS_STATUS_SUCCESS;
  uintPtr             uOffset           = HAL_DSI_GetRegBaseOffset(eDeviceID);
  uint32              uLaneSwapCtrlReg  = 0;
  uint32              uLaneMask[4];
  uint32              i;

  /* 
  * Convert lane numbers to bit masks 
  * # -> Mask(binary)
  * 1 -> 0001             
  * 2 -> 0010
  * 3 -> 0010
  * 4 -> 1000
  */
  for (i = 0; i < 4; i++)
  {
    if ((DSI_LaneMap_NONE < paDisplayLane[i]) && (DSI_LaneMap_MAX > paDisplayLane[i]))
    {
      uLaneMask[i] = 1 << (paDisplayLane[i] - 1);
    }
    else
    {
      uLaneMask[i] = 0;
    }
  }

  /* Lane cfg0 */
  uLaneSwapCtrlReg = HWIO_OUT_FLD(uLaneSwapCtrlReg, DSI_0_PHY_DSIPHY_CMN_LANE_CFG0, LOGICAL_LANE0_SEL, uLaneMask[0]);
  uLaneSwapCtrlReg = HWIO_OUT_FLD(uLaneSwapCtrlReg, DSI_0_PHY_DSIPHY_CMN_LANE_CFG0, LOGICAL_LANE1_SEL, uLaneMask[1]);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_LANE_CFG0_ADDR, uLaneSwapCtrlReg);

  /* Lane cfg1 */
  uLaneSwapCtrlReg = 0;
  uLaneSwapCtrlReg = HWIO_OUT_FLD(uLaneSwapCtrlReg, DSI_0_PHY_DSIPHY_CMN_LANE_CFG1, LOGICAL_LANE2_SEL, uLaneMask[2]);
  uLaneSwapCtrlReg = HWIO_OUT_FLD(uLaneSwapCtrlReg, DSI_0_PHY_DSIPHY_CMN_LANE_CFG1, LOGICAL_LANE3_SEL, uLaneMask[3]);
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_LANE_CFG1_ADDR, uLaneSwapCtrlReg);

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_LaneConfig()
*/
/*!
* \brief
*     Configure the Lane Map
*
* \param [in]  eDeviceId     - DSI core ID
* \param [in]  paDisplayLane - pointer to lane map array
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_Phy_LaneConfig(DSI_Device_IDType           eDeviceID,
                                          HAL_DSI_LanePropertyType   *psLaneProperty)
{
  HAL_MDSS_ErrorType  eStatus    = HAL_MDSS_STATUS_SUCCESS;
  uint32              uLaneCtrl1 = 0;
  uint32              uLaneCtrl2 = 0;
  uint32              uLaneCtrl3 = 0;
  uintPtr             uOffset    = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Check input Parameters */
  if (NULL == psLaneProperty)
  {
    eStatus = HAL_MDSS_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    uLaneCtrl1 = in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1_ADDR);
    uLaneCtrl2 = in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2_ADDR);
    uLaneCtrl3 = in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3_ADDR);

    /* Program clock lane */
    switch (psLaneProperty->eClockLaneOp)
    {
      case DSI_LANE_OP_ULPS_REQUEST:
      {
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, CLKLN_ULPS_REQUEST, 1);
      }
        break;

      case DSI_LANE_OP_ULPS_REQUEST_CLEAR:
      {
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, CLKLN_ULPS_REQUEST, 0);
      }
        break;

      case DSI_LANE_OP_ULPS_EXIT:
      {
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, CLKLN_ULPS_EXIT, 1);
      }
        break;

      case DSI_LANE_OP_ULPS_EXIT_CLEAR:
      {
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, CLKLN_ULPS_EXIT, 0);
      }
        break;

      case DSI_LANE_OP_FORCE_TX_STOP:
      {
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, CLKLN_FORCE_TX_STOP, 1);
      }
        break;

      case DSI_LANE_OP_FORCE_TX_STOP_CLEAR:
      {
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, CLKLN_FORCE_TX_STOP, 0);
      }
        break;

      default:
        break;
    }

    /* Program data lane */
    switch (psLaneProperty->eDataLaneOp)
    {
      case DSI_LANE_OP_ULPS_REQUEST:
      {
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, DLN0_ULPS_REQUEST,  1);
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, DLN1_ULPS_REQUEST,  1);
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, DLN2_ULPS_REQUEST,  1);
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, DLN3_ULPS_REQUEST,  1);
      }
        break;

      case DSI_LANE_OP_ULPS_REQUEST_CLEAR:
      {
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, DLN0_ULPS_REQUEST,  0);
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, DLN1_ULPS_REQUEST,  0);
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, DLN2_ULPS_REQUEST,  0);
        uLaneCtrl1 = HWIO_OUT_FLD(uLaneCtrl1, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1, DLN3_ULPS_REQUEST,  0);
      }
        break;

      case DSI_LANE_OP_ULPS_EXIT:
      {
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, DLN0_ULPS_EXIT,     1);
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, DLN1_ULPS_EXIT,     1);
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, DLN2_ULPS_EXIT,     1);
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, DLN3_ULPS_EXIT,     1);
      }
        break;

      case DSI_LANE_OP_ULPS_EXIT_CLEAR:
      {
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, DLN0_ULPS_EXIT,     0);
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, DLN1_ULPS_EXIT,     0);
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, DLN2_ULPS_EXIT,     0);
        uLaneCtrl2 = HWIO_OUT_FLD(uLaneCtrl2, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2, DLN3_ULPS_EXIT,     0);
      }
        break;

      case DSI_LANE_OP_FORCE_TX_STOP:
      {
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, DLN0_FORCE_TX_STOP, 1);
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, DLN1_FORCE_TX_STOP, 1);
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, DLN2_FORCE_TX_STOP, 1);
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, DLN3_FORCE_TX_STOP, 1);
      }
        break;

      case DSI_LANE_OP_FORCE_TX_STOP_CLEAR:
      {
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, DLN0_FORCE_TX_STOP, 0);
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, DLN1_FORCE_TX_STOP, 0);
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, DLN2_FORCE_TX_STOP, 0);
        uLaneCtrl3 = HWIO_OUT_FLD(uLaneCtrl3, DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3, DLN3_FORCE_TX_STOP, 0);
      }
        break;

      default:
        break;
    }
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL1_ADDR, uLaneCtrl1);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL2_ADDR, uLaneCtrl2);
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_DSI_LANE_CTRL3_ADDR, uLaneCtrl3);
  }
  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyGetLaneStatus_2_0_0()
*/
/*!
* \brief
*    Query the lane status information for a particular DSI device.
*
* \param [in]  eDeviceID    - DSI core device ID 
* \param [out] puDsiStatus  - Device lane status information is returned
*
* \retval void
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_PhyGetLaneStatus_2_0_0(DSI_Device_IDType   eDeviceID,
                                                  uint32             *puDsiStatus)
{
  HAL_MDSS_ErrorType  eStatus      = HAL_MDSS_STATUS_SUCCESS;
  uintPtr             uOffset      = HAL_DSI_GetRegBaseOffset(eDeviceID);

  /* Valid parameters */
  if (NULL == puDsiStatus)
  {
    eStatus = HAL_MDSS_DSI_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    uint32 uLaneStatus0Reg  = 0;
    uint32 uLaneStatus1Reg  = 0;

    *puDsiStatus   =  0;

    /* Get the HW generated DSI Status. */
    uLaneStatus0Reg = in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_LANE_STATUS0_ADDR);
    uLaneStatus1Reg = in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_LANE_STATUS1_ADDR);

    /* Data lane 0 ULPS state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS0, uLaneStatus0Reg, DLN0_ULPS_ACTIVE_NOT))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN0_ULPS_NOT_ACTIVE;
    }

    /* Data lane 1 ULPS state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS0, uLaneStatus0Reg, DLN1_ULPS_ACTIVE_NOT))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN1_ULPS_NOT_ACTIVE;
    }

    /* Data lane 2 ULPS state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS0, uLaneStatus0Reg, DLN2_ULPS_ACTIVE_NOT))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN2_ULPS_NOT_ACTIVE;
    }

    /* Data lane 3 ULPS state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS0, uLaneStatus0Reg, DLN3_ULPS_ACTIVE_NOT))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN3_ULPS_NOT_ACTIVE;
    }

    /* Clock lane ULPS state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS0, uLaneStatus0Reg, CLKLN_ULPS_ACTIVE_NOT))
    {
      *puDsiStatus |= DSI_LANE_STATUS_CLKLN_ULPS_NOT_ACTIVE;
    }

    /* Data lane 0 state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS1, uLaneStatus1Reg, DLN0_STOPSTATE))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN0_STOPSTATE;
    }

    /* Data lane 1 state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS1, uLaneStatus1Reg, DLN1_STOPSTATE))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN1_STOPSTATE;
    }

    /* Data lane 2 state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS1, uLaneStatus1Reg, DLN2_STOPSTATE))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN2_STOPSTATE;
    }

    /* Data lane 3 state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS1, uLaneStatus1Reg, DLN3_STOPSTATE))
    {
      *puDsiStatus |= DSI_LANE_STATUS_DLN3_STOPSTATE;
    }

    /* Clock lane state */
    if (0 != HWIO_GETVAL(DSI_0_PHY_DSIPHY_CMN_LANE_STATUS1, uLaneStatus1Reg, CLKLN_STOPSTATE))
    {
      *puDsiStatus |= DSI_LANE_STATUS_CLKLN_STOPSTATE;
    }
  }
  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_yValue()
*/
/*!
* \brief
*	this is for C-Phy only
*
* \retval float
*
****************************************************************************/
float  HAL_DSI_Phy_yValue(HAL_DSI_PhyConfigType *psDsiPhyConfig)
{
  float yVal = 0.0f;

  if (psDsiPhyConfig->uDsiClkSel == 3)
  {
     yVal = psDsiPhyConfig->uPhyPostDiv / 3.5f;
  }
  else if (psDsiPhyConfig->uDsiClkSel == 2)
  {
     yVal = psDsiPhyConfig->uPhyPostDiv;
  }
  else if (psDsiPhyConfig->uDsiClkSel == 1)
  {
     yVal = 0.5f;
  }
  else if (psDsiPhyConfig->uDsiClkSel == 0)
  {
     yVal = 1.0f;
  }

  return yVal; 
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_DsiClk_Divider()
*/
/*!
* \brief
*     Calculate Pixel Clock's Source Clock Divider (Div_ctrl_7_4).
*
* \param [in]  psDsiPhyConfig       - PHY config info
* \param [out] psDsiPhyConfigInfo   - Phy & PLL config pass back info
*
* \retval uint32
*
****************************************************************************/
uint32  HAL_DSI_Phy_DsiClk_Divider(HAL_DSI_PhyConfigType       *psDsiPhyConfig, 
                                   HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo)
{
  uint32 uM_Val = psDsiPhyConfigInfo->uPClkDivNumerator;
  uint32 uN_Val = psDsiPhyConfigInfo->uPClkDivDenominator;
  uint32 uDivCtrl;
  uint32 bpp = psDsiPhyConfig->uBitsPerPixel / 8;
  float  yVal;
  float  fVal;


  if (psDsiPhyConfig->bCPhyMode) /* C-phy */
  {
    yVal = HAL_DSI_Phy_yValue(psDsiPhyConfig);
    fVal = (((((float)uM_Val / uN_Val) * 7 * bpp) / 
			(psDsiPhyConfig->uNumOfDataLanes * 2)) * yVal);

    uDivCtrl = HAL_DSI_Phy_Rounddown(fVal);
    if (uDivCtrl > 15)
    {
      uDivCtrl = 15;
    }
    else if (uDivCtrl < 1)
    {
      uDivCtrl = 1;
    }
  }
  else
  {
    fVal = (float)(uM_Val * psDsiPhyConfig->uBitsPerPixel) / (float)(uN_Val * psDsiPhyConfig->uNumOfDataLanes * 2);
    if (psDsiPhyConfig->uBitsPerPixel >= 16)
    {
      uDivCtrl = (uint32)fVal;
    }
    else
    {
      uDivCtrl = HAL_DSI_Phy_Rounddown(fVal);
    }

  }

  return uDivCtrl;

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_DsiCLkSel()
*/
/*!
* \brief
*     Select Pixel Clock Source Clock.
*
* \param [in]  psDsiPhyConfig       - PHY config info
* \param [out] psDsiPhyConfigInfo   - Phy & PLL config pass back info
*
*
****************************************************************************/
void  HAL_DSI_Phy_DsiClk_Sel(HAL_DSI_PhyConfigType       *psDsiPhyConfig, 
                              HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo)
{     
  uint32 dsiClkSel = 0;

  if (psDsiPhyConfig->bCPhyMode) /* C-phy */
  {
    if (psDsiPhyConfig->uBitsPerPixel == 24)
    {
       dsiClkSel = 3;
    }
    else if ((psDsiPhyConfig->uBitsPerPixel == 16) &&
             (psDsiPhyConfig->uNumOfDataLanes == 2))
    {
	dsiClkSel = 3;
    }
  }
  else   /* D-Phy */
  {
	dsiClkSel = 1;
  }

  psDsiPhyConfig->uDsiClkSel = dsiClkSel;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_MND_Divider()
*/
/*!
* \brief
*     Calculate Pixel Clock MND Divider.
*
* \param [in]  psDsiPhyConfig       - PHY config info
* \param [out] psDsiPhyConfigInfo   - Phy & PLL config pass back info
*
* \retval uint32
*
****************************************************************************/
void  HAL_DSI_Phy_MND_Divider(HAL_DSI_PhyConfigType       *psDsiPhyConfig, 
                              HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo)
{     
  uint32    uM_Val              = 1;
  uint32    uN_Val              = 1;

  if (psDsiPhyConfig->bCPhyMode) /* C-phy */
  {
    if (psDsiPhyConfig->uBitsPerPixel == 18)
    {
      switch (psDsiPhyConfig->uNumOfDataLanes)
      {
      case 1:
        uM_Val = 8;
        uN_Val = 63;
        break;
      case 2:
        uM_Val = 16;
        uN_Val = 63;
        break;
      case 3:
        uM_Val = 8;
        uN_Val = 21;
        break;
      default:
        break;
      }
    }
    else if ((psDsiPhyConfig->uBitsPerPixel == 16) &&
             (psDsiPhyConfig->uNumOfDataLanes == 3))
    {
      uM_Val = 3;
      uN_Val = 7;
    }
    else if ((psDsiPhyConfig->uBitsPerPixel == 24) &&
             (psDsiPhyConfig->uNumOfDataLanes == 2))
    {
      uM_Val = 2;
      uN_Val = 3;
    }
    else if (psDsiPhyConfig->uBitsPerPixel == 30)
    {
      switch (psDsiPhyConfig->uNumOfDataLanes)
      {
      case 1:
        uM_Val = 4;
        uN_Val = 15;
        break;
      case 2:
      case 3:
        uM_Val = 16;
        uN_Val = 35;
        break;
      default:
        break;
      }
    }
  }
  else      /* D-Phy */
  {
    if (psDsiPhyConfig->uBitsPerPixel == 18)
    {
      switch (psDsiPhyConfig->uNumOfDataLanes)
      {
      case 1:
      case 2:
        uM_Val = 2;
        uN_Val = 9;
        break;
      case 4:
        uM_Val = 4;
        uN_Val = 9;
        break;

      default:
        break;
      }
    }
    else if ((psDsiPhyConfig->uBitsPerPixel == 16) &&
             (psDsiPhyConfig->uNumOfDataLanes == 3))
    {
      uM_Val = 3;
      uN_Val = 8;
    }
    else if ((psDsiPhyConfig->uBitsPerPixel == 30) &&
             (psDsiPhyConfig->uNumOfDataLanes == 4))
    {
       uM_Val = 2;
       uN_Val = 3;
    }
  }

  /*Save M/N info */
  psDsiPhyConfigInfo->uPClkDivNumerator   = uM_Val;
  psDsiPhyConfigInfo->uPClkDivDenominator = uN_Val;

}


/****************************************************************************
*
** FUNCTION: HAL_DSI_Phy_CalcPixelClockDivider()
*/
/*!
* \brief
*     Calculate Pixel Clock divider.
*
* \param [in]  psDsiPhyConfig       - PHY config info
* \param [in]  psPllConfig          - PLL config info
* \param [out] psDsiPhyConfigInfo   - Phy & PLL config pass back info
*
* \retval uint32
*
****************************************************************************/
float  HAL_DSI_Phy_CalcClockDivider(HAL_DSI_PhyConfigType       *psDsiPhyConfig, 
                                    HAL_DSI_PHY_PllConfigType   *psPllConfig,
                                    HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo)
{     
  /***************************************************************************************
  * Notes:
  *    MMSS DSI PCLK MND
  *         = 2/9, if RGB666 packed and 2 lanes
  *         = 3/8, if RGB565 and 3 lanes
  *         = 4/9, if RGB666 packed and 4 lanes
  *         = 1/1, for other cases
  *   Bitclk_divider
  *         - Clock division ratio required for the divider controlled by DIV_CTRL_3_0
  *   VCO clock = full-rate-bitclk * Bitclk_divider
  *   Dsiclk_sel
  *         = 0, if RGB111 and 3/4 lanes
  *         = 1, for other cases
  *   Dsiclk_divider
  *         - Clock division ratio required for the divider controlled by DIV_CTRL_7_4
  *         - (MND * 8 * byte_per_pixel/lane/(dsiclk_sel+1)), if byte_per_pixel >= 2
  *         - rounddown(MND * 8 * byte_per_pixel/lane/(dsiclk_sel+1)), for other cases
  ***************************************************************************************/
  /* init numerator and denominator of divider ratio in CC to 1 */
  int32     i;
  uintPtr   uOffset              = HAL_DSI_GetRegBaseOffset(psDsiPhyConfig->eDeviceId);
  uint32    uM_Val               = 1;
  uint32    uN_Val               = 1;
  uint32    uDivCtrl             = 0;
  uint32    uRegVal              = 0;
  uint32    uPllPostDiv          = 0;
  uint32    uPhyPostDiv          = 0;
  bool32    bDivFound            = FALSE;
  float     fVCOFreqMHz          = 0.0f;
  float     fVal                 = 0.0f;
  float     fPllOutputFreqMHz;
  float     fDesiredBitclockMHz;
  float     fMinVCOFreqMHz       = 0.0f;
  HAL_DSI_PHY_VersionType          sVersionInfo;
  uint32    lut_max;
  HAL_DSI_PHY_DividerLUTEntryType *lut;

  DSI_OSAL_MemZero(&sVersionInfo, sizeof(HAL_DSI_PHY_VersionType));

  /* Get the DSI PHY version info */
  HAL_DSI_Phy_GetVersionInfo(&sVersionInfo);

  if (0x02 <= sVersionInfo.uMinorVersion)
  {
    /* Napali, use 1000Mhz */
    fMinVCOFreqMHz = HAL_DSI_PLL_VCO_MIN_MHZ_2_2_0;
  }
  else
  {
    /* Nazgul, use 1500Mhz */
    fMinVCOFreqMHz = HAL_DSI_PLL_VCO_MIN_MHZ;
  }

  HAL_DSI_Phy_MND_Divider(psDsiPhyConfig, psDsiPhyConfigInfo);

  uM_Val = psDsiPhyConfigInfo->uPClkDivNumerator;
  uN_Val = psDsiPhyConfigInfo->uPClkDivDenominator;

  /* Desired clock in MHz */
  fDesiredBitclockMHz = (float)psDsiPhyConfig->uDesiredBitClkFreq / 1000000.0f;

  if (psDsiPhyConfig->bCPhyMode)
  {
    /* C Phy */
    /* 7 symbols per 16 bits*/
    fDesiredBitclockMHz *= 7;
    fDesiredBitclockMHz /= 16;
    lut = asPllDividerLUT_CPhy;
    lut_max = sizeof(asPllDividerLUT_CPhy) / sizeof(*lut);
    lut += (lut_max - 1);
  }
  else 
  {
    /* D Phy */
    lut = asPllDividerLUT_DPhy;
    lut_max = sizeof(asPllDividerLUT_DPhy) / sizeof(*lut);
    lut += (lut_max - 1);
  }

  /*
  * PLL Post Div - from LUT
  * Check the LUT in reverse order
  */
  for (i = lut_max - 1; i >= 0; i--, lut--)
  {
    fVal = lut->uPhyPostDiv * lut->uPllPostDiv;

    if (fVal)
    {
      fVal = fMinVCOFreqMHz / fVal;

      if (fDesiredBitclockMHz > fVal)
      {
        /* Range found */
        uPllPostDiv = lut->uPllPostDiv;
        uPhyPostDiv = lut->uPhyPostDiv;
        bDivFound = TRUE;
        break;
      }
    }
  }

  if (bDivFound)
  {
    psPllConfig->uPllPostDiv = uPllPostDiv;
    psDsiPhyConfig->uPllPostDiv = uPllPostDiv;
    psDsiPhyConfig->uPhyPostDiv = uPhyPostDiv;

    /* Div Ctrl*/
    HAL_DSI_Phy_DsiClk_Sel(psDsiPhyConfig, psDsiPhyConfigInfo);
    uDivCtrl = HAL_DSI_Phy_DsiClk_Divider(psDsiPhyConfig, psDsiPhyConfigInfo); /*div_ctrl_7_4 */

    /* DIV_CTRL_7_4 DIV_CTRL_3_0  (DIV_CTRL_3_0 = PHY post divider ratio) */
    uRegVal = 0;
    uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_CLK_CFG0, DIV_CTRL_7_4, uDivCtrl);

    if ((HAL_DSI_PLL_CONFIG_SPLIT_SOURCE == psDsiPhyConfig->ePLLConfigSource) &&
      (DSI_DeviceID_1 == psDsiPhyConfig->eDeviceId))
    {
      /* For slave node PHY post div is forced to 1 */
      uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_CLK_CFG0, DIV_CTRL_3_0, 1);
    }
    else
    {
      uRegVal = HWIO_OUT_FLD(uRegVal, DSI_0_PHY_DSIPHY_CMN_CLK_CFG0, DIV_CTRL_3_0, uPhyPostDiv);
    }
    out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CLK_CFG0_ADDR, uRegVal);


    /* PLL output frequency = fDesiredBitclockMHz * uPhyPostDiv */
    fPllOutputFreqMHz = fDesiredBitclockMHz * uPhyPostDiv;

    /* VCO output freq*/
    fVCOFreqMHz       = fPllOutputFreqMHz * uPllPostDiv;
    /* Save vco output frequency */
    psDsiPhyConfigInfo->uPllVcoOutputFreq = psDsiPhyConfig->uDesiredBitClkFreq * uPhyPostDiv * uPllPostDiv;
  }

  return fVCOFreqMHz;
}


/* -----------------------------------------------------------------------
** Public functions
** ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhyDisable()
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
void HAL_DSI_2_0_0_PhyDisable(DSI_Device_IDType   eDeviceId)
{
  uintPtr uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);

  /* Disable PLL */
  HAL_DSI_Phy_PllEnable(eDeviceId, FALSE);

  /* PLL block power down */
  out_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_CMN_CTRL_0_ADDR, 0);
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhyPllEnable()
*/
/*!
* \brief
*     Enable or Disable Pll
*
* \param [in]   eDeviceId   - DSI core ID
* \param [in]   bEnable     - true to enable pll and false to disable pll

*
* \retval None
*
****************************************************************************/
void HAL_DSI_2_0_0_PhyPllEnable(DSI_Device_IDType eDeviceId, bool32 bEnable)
{
  HAL_DSI_Phy_PllEnable(eDeviceId, bEnable);
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhyPllPowerCtrl()
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
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyPllPowerCtrl(DSI_Device_IDType   eDeviceId,
                                                 bool32              bPllPowerUp)
{
  HAL_MDSS_ErrorType   eStatus     = HAL_MDSS_STATUS_SUCCESS;
  /*
  * Moved the implementation to HAL_DSI_2_0_0_PhyPllSetup to enable programming PHY and PLL
  * together
  */

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhySetup()
*/
/*!
* \brief
*     Set up DSI Phy alone.
*
* \param [in]  eDeviceId          - DSI core ID
* \param [IN]  psDsiPhyConfig     - Phy configuration
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhySetup(DSI_Device_IDType          eDeviceId,
                                          HAL_DSI_PhyConfigType     *psDsiPhyConfig)
{
  HAL_MDSS_ErrorType          eStatus         = HAL_MDSS_STATUS_SUCCESS;
  HAL_DSI_PHY_PhySettingsType sPhySettings;


  DSI_OSAL_MemZero(&sPhySettings, sizeof(HAL_DSI_PHY_PhySettingsType));

  /* Lane Settings */
  sPhySettings.bCPhyMode                     = psDsiPhyConfig->bCPhyMode;
  sPhySettings.sLaneConfig.uDataStrengthLP_N = 0x5;
  sPhySettings.sLaneConfig.uDataStrengthLP_P = 0x5;
  sPhySettings.sLaneConfig.uDataStrengthHS   = 0x88;
  sPhySettings.sLaneConfig.uClockStrengthHS  = 0x88;
  sPhySettings.sLaneConfig.uPemphBottom      = HAL_DSI_PHY_PLL_PRE_EMPHASIS_BOTTOM_DEFAULT;
  sPhySettings.sLaneConfig.uPemphTop         = HAL_DSI_PHY_PLL_PRE_EMPHASIS_TOP_DEFAULT;
  sPhySettings.sLaneConfig.bPemphEnable      = FALSE;
  sPhySettings.sLaneConfig.bStrengthOverride = 0;

  // Indicate whether config is dual or single DSI
  if (HAL_DSI_PLL_CONFIG_SPLIT_SOURCE == psDsiPhyConfig->ePLLConfigSource)
  {
    sPhySettings.bDualDSI = TRUE;
  }

  /*
  * DSI PHY Reset:
  * According to HPG, In the case of certain host errors,  it's required to do DSI PHY reset along with 
  * DSI controller reset to recover display panel successfully.
  */
  HAL_DSI_Phy_ResetPhy(eDeviceId);

  /* Program HW */
  eStatus = HAL_DSI_Phy_CommitPhy(eDeviceId, &sPhySettings);

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhyPllSetup()
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
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyPllSetup(HAL_DSI_PhyConfigType       *psDsiPhyConfig,
                                             HAL_DSI_PhyConfigInfoType   *psDsiPhyConfigInfo)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

  if((NULL == psDsiPhyConfig)||
     (NULL == psDsiPhyConfigInfo))
  {
    eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  /* validate input parameters */
  else if ((psDsiPhyConfig->eDeviceId != DSI_DeviceID_0) &&
           (psDsiPhyConfig->eDeviceId != DSI_DeviceID_1))
  {
    eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else if ((psDsiPhyConfig->uBitsPerPixel != 16) &&
           (psDsiPhyConfig->uBitsPerPixel != 18) &&
           (psDsiPhyConfig->uBitsPerPixel != 24))
  {
    /* Unsupported pixel bit depth */
    eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else if ((psDsiPhyConfig->uNumOfDataLanes == 0) ||
           (psDsiPhyConfig->uNumOfDataLanes > 4))
  {
    /* Illegal number of DSI data lanes */
    eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    float                       fActualFreqMHz;
    float                       fVCOFreqMHz;
    float                       fPllClkTarget;
    float                       fRefFreqMHz          = HAL_DSI_PLL_REF_FREQ_MHZ;
    float                       fDecFrac             = 0.0f;
    float                       fFrac                = 0.0f;
    float                       fVal                 = 0.0f;
    uint32                      uFrac                = 0;
    uint32                      uDec                 = 0;
    uint32                      uPropGain            = 0;
    uint32                      uSSCPERStart;
    uint32                      uSSCMod;
    uint32                      uSSCPER;
    HAL_DSI_PHY_PllConfigType   sPllConfig;
    DSI_Device_IDType           eDeviceId            = psDsiPhyConfig->eDeviceId;
    HAL_DSI_PHY_VersionType     sVersionInfo;

    DSI_OSAL_MemZero(&sPllConfig,   sizeof(HAL_DSI_PHY_PllConfigType));
    DSI_OSAL_MemZero(&sVersionInfo, sizeof(HAL_DSI_PHY_VersionType));

    /* Get the DSI PHY version info */
    HAL_DSI_Phy_GetVersionInfo(&sVersionInfo);

    /* Save Clock info */
    psDsiPhyConfigInfo->uBitClkFreq = psDsiPhyConfig->uDesiredBitClkFreq;

    /* Ref clk after prescaler */
    fRefFreqMHz = fRefFreqMHz * HAL_DSI_PLL_PRESCALE_FACTOR;

    fVCOFreqMHz = HAL_DSI_Phy_CalcClockDivider(psDsiPhyConfig, &sPllConfig, psDsiPhyConfigInfo);

    if (0.0f == fVCOFreqMHz)
    {
      /* bitclock too low  - unsupported */
      eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
    }
    else
    {
      /* Enable PLL bias */
      HAL_DSI_Phy_PllBiasEnable(psDsiPhyConfig->eDeviceId, TRUE);

      /* Set byte clk source */
      HAL_DSI_Phy_PllSetSource(psDsiPhyConfig);

      if ((HAL_DSI_PLL_CONFIG_SPLIT_SOURCE == psDsiPhyConfig->ePLLConfigSource) &&
          (DSI_DeviceID_1 == psDsiPhyConfig->eDeviceId))
      {
        /* Global clock enable */
        HAL_DSI_Phy_PllGlobalClockEnable(psDsiPhyConfig->eDeviceId, TRUE);
        
        /* Resync FIFOs */
        HAL_DSI_Phy_ResyncFIFO(eDeviceId);
      }
      else
      {
        /* Prop Gain*/
        if (HAL_DSI_PLL_PARAM_1900F >= fVCOFreqMHz)
        {
          uPropGain = 8;
        }
        else if (HAL_DSI_PLL_PARAM_3000F >= fVCOFreqMHz)
        {
          uPropGain = 10;
        }
        else
        {
          uPropGain = 12;
        }

        if (HAL_DSI_PLL_PARAM_1100F >= fVCOFreqMHz)
        {
          sPllConfig.uClockInverters = 8;
        }
        else
        {
          sPllConfig.uClockInverters = 0;
        }

        sPllConfig.uPropGainRate = uPropGain;

        /* Clock multiplier ratio */
        fDecFrac = fVCOFreqMHz / fRefFreqMHz;

        /* Decimal part */
        uDec = HAL_DSI_Phy_Rounddown(fDecFrac);
        sPllConfig.uDecDivStart = uDec;

        /* Fractional part - 18 bits */
        fFrac = (fDecFrac - uDec) * (1 << HAL_DSI_PLL_FRAC_BITS);
        uFrac = HAL_DSI_Phy_Round(fFrac);

        /* Frac Low : bits 0-3 */
        sPllConfig.uFracDivLow = (uFrac & 0xF);

        /* Frac Mid : bits 8-15 */
        sPllConfig.uFracDivMid = (uFrac >> 8) & 0xFF;

        /* Frac High - bits 16-17 */
        sPllConfig.uFracDivHigh = (uFrac >> 16) & 0x3;

        /* Actual Frequency */
        fVal = uDec + ((float)uFrac) / (1 << HAL_DSI_PLL_FRAC_BITS);
        fActualFreqMHz = fVal * fRefFreqMHz;

        /* SSC Starting PER */
        uSSCPERStart = HAL_DSI_PLL_REF_FREQ_MHZ * 1000 / (2 * HAL_DSI_PLL_SSC_FREQ_KHZ);

        /* SSC MOD */
        uSSCMod = (uSSCPERStart + 1) % (HAL_DSI_PLL_SSC_ADJ_PER + 1);

        /* SSC PER */
        uSSCPER = uSSCPERStart - uSSCMod;
        sPllConfig.uSSCPER = uSSCPER;

        /* SSC Adj PER */
        sPllConfig.uSSCAdjPER = HAL_DSI_PLL_SSC_ADJ_PER;

        /* SSC Step Size */
        fVal = (uDec * (1 << HAL_DSI_PLL_FRAC_BITS)) + uFrac;
        fVal = fVal * HAL_DSI_PLL_SSC_OFFSET_PPM / HAL_DSI_PLL_PARAM_1M;
        fVal = fVal * (HAL_DSI_PLL_SSC_ADJ_PER + 1) / (uSSCPER + 1);
        sPllConfig.uSSCStepSize = HAL_DSI_Phy_Round(fVal);

        /* Freq detect threshold */
        sPllConfig.uFreqDetThresh = HAL_DSI_PLL_THRESHOLD_CYCLES;

        /* Refclk cycles */
        sPllConfig.uFreqDetRefclk = HAL_DSI_PLL_REFCLK_CYCLES - 1;

        /* PLL Clk Target */
        fPllClkTarget = (fActualFreqMHz * HAL_DSI_PLL_REFCLK_CYCLES) / (8 * fRefFreqMHz);
        sPllConfig.uFreqDetRefclk = (HAL_DSI_Phy_Round(fPllClkTarget) - 1);

        /* lockdet */
        sPllConfig.uLockDetRate = HAL_DSI_PLL_LOCK_TIMER;

        /* Lock delay */
        sPllConfig.uLockDelay = HAL_DSI_PLL_PLL_LOCK_DELAY;

        /* clock inverters */
        if (1100.0f > fVCOFreqMHz)
          sPllConfig.uClockInverters = 1;

        /* Error */
        sPllConfig.fPllErrorPPM = (1 + HAL_DSI_PLL_THRESHOLD_CYCLES) * HAL_DSI_PLL_PARAM_1M / fPllClkTarget;

        /* Save clock info */
        if (psDsiPhyConfig->bCPhyMode)
        {
          fVal  = psDsiPhyConfig->uDesiredBitClkFreq;
          fVal *= 7;
          fVal /= 16;
          psDsiPhyConfigInfo->uBitClkFreq  = (uint32)fVal;
          psDsiPhyConfigInfo->uByteClkFreq =  (uint32)fVal / 7;
          fVal = (float)psDsiPhyConfigInfo->uByteClkFreq * (float)psDsiPhyConfig->uNumOfDataLanes * 2 / (float)(psDsiPhyConfig->uBitsPerPixel / 8);
        }
        else
        {
          psDsiPhyConfigInfo->uBitClkFreq = psDsiPhyConfig->uDesiredBitClkFreq;
          psDsiPhyConfigInfo->uByteClkFreq = psDsiPhyConfig->uDesiredBitClkFreq / 8;

          fVal = (float)psDsiPhyConfig->uDesiredBitClkFreq * (float)psDsiPhyConfig->uNumOfDataLanes / (float)psDsiPhyConfig->uBitsPerPixel;
        }
        psDsiPhyConfigInfo->uPclkFreq = (uint32)fVal;

        /* Step 3 only for Master */
        if (HAL_MDSS_STATUS_SUCCESS == eStatus)
        {
          HAL_DSI_Phy_CommitPll(eDeviceId, psDsiPhyConfig->ePLLConfigSource, &sPllConfig);
        }

        if (0x02 <= sVersionInfo.uMinorVersion)
        {
          HAL_DSI_Phy_Toogle_Dln3_Tx_Dctrl(eDeviceId);
        }

        /* Steps 6,7 Start PLL & Lock */
        if (HAL_MDSS_STATUS_SUCCESS == eStatus)
        {
          eStatus = HAL_DSI_Phy_PllLockDetect(eDeviceId);
        }

        /* Step 8 - Resync Data Paths */
        if (HAL_MDSS_STATUS_SUCCESS == eStatus)
        {
          /* Global clock enable */
          HAL_DSI_Phy_PllGlobalClockEnable(psDsiPhyConfig->eDeviceId, TRUE);

          /* Resync FIFOs */
          HAL_DSI_Phy_ResyncFIFO(eDeviceId);
        }
      }
      /* Phy Programming complete */
    }
  }

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhySetupTimingParams()
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
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhySetupTimingParams(HAL_DSI_TimingSettingType *pTimingParameters)
{
  HAL_MDSS_ErrorType           eStatus             = HAL_MDSS_STATUS_SUCCESS;
  HAL_DSI_PHY_TimingConfigType sTimingConfig;

  /* validate input parameters */
  if ((pTimingParameters->eDeviceId != DSI_DeviceID_0) &&
      (pTimingParameters->eDeviceId != DSI_DeviceID_1))
  {
    eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    DSI_OSAL_MemZero(&sTimingConfig, sizeof(HAL_DSI_PHY_TimingConfigType));

    if (pTimingParameters->bCPhyMode)
    {
      /* Calculate timing parameters */
      HAL_DSI_Phy_CalculateTimingParams_CPhy(pTimingParameters, &sTimingConfig);

      /* config timing parameters */
      HAL_DSI_Phy_CommitTimingParams_CPhy(pTimingParameters->eDeviceId, &sTimingConfig);

    }
    else
    {
      /* Calculate timing parameters */
      HAL_DSI_Phy_CalculateTimingParams(pTimingParameters, &sTimingConfig);

      /* config timing parameters */
      HAL_DSI_Phy_CommitTimingParams(pTimingParameters->eDeviceId, &sTimingConfig);
    }

  }

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhyPllReConfigure()
*/
/*!
* \brief
*     Sets up the DSI PLL for dynamic refresh rate reconfiguration and passes
*     back the calculated DSI Phy and PLL information to the caller for the
*     requested refresh rate. Also allows the caller to poll the DSI PHY PLL
*     to ensure that the PLL is locked to the new refresh rate.
*
* \param [in]      psDsiPhyConfig        - Phy configuration information
* \param [in/out]  psDsiPhyPLLConfigInfo - Phy PLL configuration information
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyPllReConfigure(HAL_DSI_PhyConfigType	   *psDsiPhyConfig,
                                                   HAL_DSI_PhyConfigInfoType   *psDsiPhyPLLConfigInfo) 
{
  // Reconfiguring PLL for new refresh rate - To be Implemented
  return HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhyPllInitialize()
*/
/*!
* \brief
*     Initialize the phy in to a power-on reset state.
*
* \param 
*
* \retval 
*
****************************************************************************/
void HAL_DSI_2_0_0_PhyPllInitialize(void)
{
  uint32 uControllerIndex;

  /* Inialize all phys */
  for (uControllerIndex = 0; uControllerIndex < DSI_DeviceID_MAX; uControllerIndex++)
  {
    HAL_DSI_2_0_0_PhyPllPowerCtrl((DSI_Device_IDType)uControllerIndex, TRUE);
  }
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhySetProperty()
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
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhySetProperty(DSI_Device_IDType              eDeviceId,
                                                HAL_DSI_PhyPropertyType        eProperty,
                                                HAL_DSI_PhyPropertyParamsType *pPropertyData)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

  switch (eProperty)
  {
    case HAL_DSI_PHY_PROPERTYID_LANEMAP:
      eStatus = HAL_DSI_Phy_LaneMapConfig(eDeviceId, &pPropertyData->aLaneMap[0]);
      break;

    case HAL_DSI_PHY_PROPERTYID_LANECONFIG:
      eStatus = HAL_DSI_Phy_LaneConfig(eDeviceId, &pPropertyData->sLaneConfig);
      break;

    default:
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
      break;
  }
  return eStatus;
}


/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhyGetProperty
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
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyGetProperty(DSI_Device_IDType              eDeviceId,
                                                HAL_DSI_PhyPropertyType        eProperty,
                                                HAL_DSI_PhyPropertyParamsType *pPropertyData)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;

  switch (eProperty)
  {
    case HAL_DSI_PHY_PROPERTYID_LANESTATUS:
      eStatus = HAL_DSI_PhyGetLaneStatus_2_0_0(eDeviceId, &pPropertyData->uLaneStatus);
      break;

    default:
      eStatus = HAL_MDSS_STATUS_FAILED_NOT_SUPPORTED;
      break;
  }
  return eStatus;
}

/****************************************************************************
*
** FUNCTION: HAL_DSI_2_0_0_PhyGetPllCodes
*/
/*!
* \brief
*   Get PLL codes
*
* \param [in] eDeviceId     - DSI core ID
* \param [in] pBuffer       - Pointer to buffer that stores pll codes
* \param [in] uSize         - Size of the buffer that stores pll codes 
*                             
****************************************************************************/
HAL_MDSS_ErrorType HAL_DSI_2_0_0_PhyGetPllCodes(DSI_Device_IDType               eDeviceId,
                                                HAL_DSI_PHY_Dynamic_Cal_Result *psPllCodes)
{
  HAL_MDSS_ErrorType eStatus = HAL_MDSS_STATUS_SUCCESS;
  uintPtr            uOffset = HAL_DSI_GetRegBaseOffset(eDeviceId);  

  if (NULL == psPllCodes)
  {
    eStatus = HAL_MDSS_DSI_FAILED_INVALID_INPUT_PARAMETER;
  }
  else
  {
    psPllCodes->uPllCodes[0] =in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_ICODE_ACCUM_STATUS_HIGH_ADDR);
    psPllCodes->uPllCodes[1] =in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_ICODE_ACCUM_STATUS_LOW_ADDR);
    psPllCodes->uPllCodes[2] =in_dword(uOffset + HWIO_DSI_0_PHY_DSIPHY_PLL_BAND_SEL_CAL_ADDR);
  }
  return eStatus;
}

#ifdef __cplusplus
}
#endif

/*============================================================================
  FILE:         FgAdcHal.c

  OVERVIEW:     Implementation of device HAL for FG ADC

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/fgadc/src/FgAdcHal.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2017-03-13  jjo  Forced batt id workaround.
  2016-12-07  jjo  Add SW request API.
  2016-04-22  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "FgAdcHal.h"
#include "FgAdcHalPmio.h"
#include "pm_pmio.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

#define FGADC_CTRL_ENABLE_MASK     0x1
#define FGADC_CTRL_ENABLE_SHFT     0x0

#define FGADC_STS_CHANNEL_STS_BMSK 0x2
#define FGADC_STS_READING_STS_BMSK 0x1

#define FGADC_MSB_ADC_READ_BMSK    0x7

#define FGADC_HAL_MAX_NUM_READ_TRIES 3

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static const uint8 gaucRegDumpAddresses[FGADC_HAL_NUM_REGISTERS_TO_DUMP] =
{
   PMIO_ADDRX(0, PERPH_TYPE),
   PMIO_ADDRX(0, PERPH_SUBTYPE),
   PMIO_ADDRX(0, STATUS1),
   PMIO_ADDRX(0, CHG_PWR_STS),
   PMIO_ADDRX(0, REF_STS),
   PMIO_ADDRX(0, INT_RT_STS),
   PMIO_ADDRX(0, INT_SET_TYPE),
   PMIO_ADDRX(0, INT_POLARITY_HIGH),
   PMIO_ADDRX(0, INT_POLARITY_LOW),
   PMIO_ADDRX(0, INT_EN_SET),
   PMIO_ADDRX(0, INT_EN_CLR),
   PMIO_ADDRX(0, INT_LATCHED_STS),
   PMIO_ADDRX(0, INT_PENDING_STS),
   PMIO_ADDRX(0, INT_MID_SEL),
   PMIO_ADDRX(0, INT_PRIORITY),
   PMIO_ADDRX(0, EN_CTL),
   PMIO_ADDRX(0, SKIN_TEMPERATURE_LSB),
   PMIO_ADDRX(0, SKIN_TEMPERATURE_MSB),
   PMIO_ADDRX(0, RR_ADC_CTL),
   PMIO_ADDRX(0, RR_ADC_LOG),
   PMIO_ADDRX(0, REF_CFG),
   PMIO_ADDRX(0, RBIAS_CNLS),
   PMIO_ADDRX(0, FAKE_BATT_LOW_LSB),
   PMIO_ADDRX(0, FAKE_BATT_LOW_MSB),
   PMIO_ADDRX(0, FAKE_BATT_HIGH_LSB),
   PMIO_ADDRX(0, FAKE_BATT_HIGH_MSB),
   PMIO_ADDRX(0, BATT_ID_CTRL),
   PMIO_ADDRX(0, BATT_ID_TRIGGER),
   PMIO_ADDRX(0, BATT_ID_STS),
   PMIO_ADDRX(0, BATT_ID_CFG),
   PMIO_ADDRX(0, BATT_ID_5_LSB),
   PMIO_ADDRX(0, BATT_ID_5_MSB),
   PMIO_ADDRX(0, BATT_ID_15_LSB),
   PMIO_ADDRX(0, BATT_ID_15_MSB),
   PMIO_ADDRX(0, BATT_ID_150_LSB),
   PMIO_ADDRX(0, BATT_ID_150_MSB),
   PMIO_ADDRX(0, BATT_THERM_CTRL),
   PMIO_ADDRX(0, BATT_THERM_TRIGGER),
   PMIO_ADDRX(0, BATT_THERM_STS),
   PMIO_ADDRX(0, BATT_THERM_CFG),
   PMIO_ADDRX(0, BATT_THERM_LSB),
   PMIO_ADDRX(0, BATT_THERM_MSB),
   PMIO_ADDRX(0, BATT_THERM_FREQ),
   PMIO_ADDRX(0, AUX_THERM_CTRL),
   PMIO_ADDRX(0, AUX_THERM_TRIGGER),
   PMIO_ADDRX(0, AUX_THERM_STS),
   PMIO_ADDRX(0, AUX_THERM_CFG),
   PMIO_ADDRX(0, AUX_THERM_LSB),
   PMIO_ADDRX(0, AUX_THERM_MSB),
   PMIO_ADDRX(0, SKIN_HOT),
   PMIO_ADDRX(0, SKIN_TOO_HOT),
   PMIO_ADDRX(0, AUX_THERM_C1),
   PMIO_ADDRX(0, AUX_THERM_C2),
   PMIO_ADDRX(0, AUX_THERM_C3),
   PMIO_ADDRX(0, AUX_THERM_HALF_RANGE),
   PMIO_ADDRX(0, USB_IN_V_CTRL),
   PMIO_ADDRX(0, USB_IN_V_TRIGGER),
   PMIO_ADDRX(0, USB_IN_V_STS),
   PMIO_ADDRX(0, USB_IN_V_LSB),
   PMIO_ADDRX(0, USB_IN_V_MSB),
   PMIO_ADDRX(0, USB_IN_I_CTRL),
   PMIO_ADDRX(0, USB_IN_I_TRIGGER),
   PMIO_ADDRX(0, USB_IN_I_STS),
   PMIO_ADDRX(0, USB_IN_I_LSB),
   PMIO_ADDRX(0, USB_IN_I_MSB),
   PMIO_ADDRX(0, DC_IN_V_CTRL),
   PMIO_ADDRX(0, DC_IN_V_TRIGGER),
   PMIO_ADDRX(0, DC_IN_V_STS),
   PMIO_ADDRX(0, DC_IN_V_LSB),
   PMIO_ADDRX(0, DC_IN_V_MSB),
   PMIO_ADDRX(0, DC_IN_I_CTRL),
   PMIO_ADDRX(0, DC_IN_I_TRIGGER),
   PMIO_ADDRX(0, DC_IN_I_STS),
   PMIO_ADDRX(0, DC_IN_I_LSB),
   PMIO_ADDRX(0, DC_IN_I_MSB),
   PMIO_ADDRX(0, PMI_DIE_TEMP_CTRL),
   PMIO_ADDRX(0, PMI_DIE_TEMP_TRIGGER),
   PMIO_ADDRX(0, PMI_DIE_TEMP_STS),
   PMIO_ADDRX(0, PMI_DIE_TEMP_CFG),
   PMIO_ADDRX(0, PMI_DIE_TEMP_LSB),
   PMIO_ADDRX(0, PMI_DIE_TEMP_MSB),
   PMIO_ADDRX(0, CHARGER_TEMP_CTRL),
   PMIO_ADDRX(0, CHARGER_TEMP_TRIGGER),
   PMIO_ADDRX(0, CHARGER_TEMP_STS),
   PMIO_ADDRX(0, CHARGER_TEMP_CFG),
   PMIO_ADDRX(0, CHARGER_TEMP_LSB),
   PMIO_ADDRX(0, CHARGER_TEMP_MSB),
   PMIO_ADDRX(0, CHARGER_HOT),
   PMIO_ADDRX(0, CHARGER_TOO_HOT),
   PMIO_ADDRX(0, GPIO_CTRL),
   PMIO_ADDRX(0, GPIO_TRIGGER),
   PMIO_ADDRX(0, GPIO_STS),
   PMIO_ADDRX(0, GPIO_LSB),
   PMIO_ADDRX(0, GPIO_MSB),
   PMIO_ADDRX(0, ATEST_CTRL),
   PMIO_ADDRX(0, ATEST_TRIGGER),
   PMIO_ADDRX(0, ATEST_STS),
   PMIO_ADDRX(0, ATEST_LSB),
   PMIO_ADDRX(0, ATEST_MSB),
   PMIO_ADDRX(0, SEC_ACCESS),
   PMIO_ADDRX(0, PERPH_RESET_CTL2),
   PMIO_ADDRX(0, PERPH_RESET_CTL3),
   PMIO_ADDRX(0, PERPH_RESET_CTL4),
   PMIO_ADDRX(0, INT_TEST1),
   PMIO_ADDRX(0, INT_TEST_VAL),
   PMIO_ADDRX(0, TM_TRIGGER_CTRLS),
   PMIO_ADDRX(0, TM_ADC_CTRLS),
   PMIO_ADDRX(0, TM_CNL_CTRL),
   PMIO_ADDRX(0, TM_BATT_ID_CTRL),
   PMIO_ADDRX(0, TM_THERM_CTRL),
   PMIO_ADDRX(0, TM_CONV_STS),
   PMIO_ADDRX(0, TM_ADC_READ_LSB),
   PMIO_ADDRX(0, TM_ADC_READ_MSB),
   PMIO_ADDRX(0, TM_ATEST_MUX_1),
   PMIO_ADDRX(0, TM_ATEST_MUX_2),
   PMIO_ADDRX(0, TM_REFERENCES),
   PMIO_ADDRX(0, TM_MISC_CTL),
   PMIO_ADDRX(0, TM_RR_CTRL),
   PMIO_ADDRX(0, TRIM_NUM),
   PMIO_ADDRX(0, THERM_GAIN),
   PMIO_ADDRX(0, THERM_OFFSET),
   PMIO_ADDRX(0, DIG_5V_GAIN),
   PMIO_ADDRX(0, DIG_5V_OFFSET),
   PMIO_ADDRX(0, USBIN_I_GAIN),
   PMIO_ADDRX(0, USBIN_I_OFFSET),
   PMIO_ADDRX(0, DCIN_I_GAIN),
   PMIO_ADDRX(0, DCIN_I_OFFSET),
   PMIO_ADDRX(0, DIG_2P5_V_GAIN),
   PMIO_ADDRX(0, DIG_2P5_V_OFFSET),
   PMIO_ADDRX(0, TMP_GAIN),
   PMIO_ADDRX(0, TMP_OFFSET),
   PMIO_ADDRX(0, MISC_TRIM),
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
FgAdcHalResultType FgAdcHalGetRevisionInfo(FgAdcHalInterfaceType *piFgAdcHal, FgAdcRevisionInfoType *pRevisionInfo)
{
   FgAdcHalResultType result;
   uint8 aucData[2];

   result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                     PMIO_ADDRX(0, PERPH_TYPE),
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != FGADC_HAL_SUCCESS)
   {
      return result;
   }

   pRevisionInfo->ucType = aucData[0];
   pRevisionInfo->ucSubType = aucData[1];

   return result;
}

FgAdcHalResultType FgAdcHalSetEnable(FgAdcHalInterfaceType *piFgAdcHal, FgAdcEnableType eEnable)
{
   uint8 aucData[1];

   if (eEnable == FGADC_DISABLE)
   {
      aucData[0] = PMIO_FVAL(EN_CTL, RR_ADC_EN, 0);
   }
   else
   {
      aucData[0] = PMIO_FVAL(EN_CTL, RR_ADC_EN, 1);
   }

   return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                    PMIO_ADDRX(0, EN_CTL),
                                    aucData,
                                    ARRAY_LENGTH(aucData));
}

FgAdcHalResultType FgAdcHalSetShutdown(FgAdcHalInterfaceType *piFgAdcHal, FgAdcShdnType eShutdown)
{
   FgAdcHalResultType result;
   uint8 aucData[1];
   uint8 ucOffset = PMIO_ADDRX(0, RR_ADC_CTL);

   result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                     ucOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != FGADC_HAL_SUCCESS)
   {
      return result;
   }

   aucData[0] &= ~PMIO_FMSK(RR_ADC_CTL, RR_SHDN);
   if (eShutdown == FGADC_SHDN_REQUESTED)
   {
      aucData[0] |= PMIO_FVAL(RR_ADC_CTL, RR_SHDN, 1);
   }

   return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                    ucOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData));
}

FgAdcHalResultType FgAdcHalSetChanEn(FgAdcHalInterfaceType *piFgAdcHal, FcAdcChannelType eChannel, FgAdcEnableType eEnable)
{
   FgAdcHalResultType result;
   uint8 aucData[1];
   uint8 ucOffset;

   switch (eChannel)
   {
      case FGADC_CHAN_SKIN_TEMP:
         /* Fall through */
      case FGADC_CHAN_AUX_THERM:
         ucOffset = PMIO_ADDRX(0, AUX_THERM_CTRL);
         break;
      case FGADC_CHAN_BATT_ID:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_FRESH:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_5:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_15:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_150:
         ucOffset = PMIO_ADDRX(0, BATT_ID_CTRL);
         break;
      case FGADC_CHAN_BATT_THERM:
         ucOffset = PMIO_ADDRX(0, BATT_THERM_CTRL);
         break;
      case FGADC_CHAN_USB_IN_V:
         ucOffset = PMIO_ADDRX(0, USB_IN_V_CTRL);
         break;
      case FGADC_CHAN_USB_IN_I:
         ucOffset = PMIO_ADDRX(0, USB_IN_I_CTRL);
         break;
      case FGADC_CHAN_DC_IN_V:
         ucOffset = PMIO_ADDRX(0, DC_IN_V_CTRL);
         break;
      case FGADC_CHAN_DC_IN_I:
         ucOffset = PMIO_ADDRX(0, DC_IN_I_CTRL);
         break;
      case FGADC_CHAN_DIE_TEMP:
         ucOffset = PMIO_ADDRX(0, PMI_DIE_TEMP_CTRL);
         break;
      case FGADC_CHAN_CHARGER_TEMP:
         ucOffset = PMIO_ADDRX(0, CHARGER_TEMP_CTRL);
         break;
      case FGADC_CHAN_GPIO:
         ucOffset = PMIO_ADDRX(0, GPIO_CTRL);
         break;
      default:
         return FGADC_HAL_ERROR;
   }

   result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                     ucOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != FGADC_HAL_SUCCESS)
   {
      return result;
   }

   aucData[0] &= ~FGADC_CTRL_ENABLE_MASK;
   if (eEnable == FGADC_ENABLE)
   {
      aucData[0] |= (1 << FGADC_CTRL_ENABLE_SHFT) & FGADC_CTRL_ENABLE_MASK;
   }

   return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                    ucOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData));
}

FgAdcHalResultType FgAdcHalMakeSwReq(FgAdcHalInterfaceType *piFgAdcHal)
{
   FgAdcHalResultType result;
   uint8 aucData[1];
   uint8 ucOffset = PMIO_ADDRX(0, RR_ADC_CTL);

   result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                     ucOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != FGADC_HAL_SUCCESS)
   {
      return result;
   }

   aucData[0] ^= PMIO_FMSK(RR_ADC_CTL, SW_SEQUENCE_INITIATE);

   return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                    ucOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData));
}

FgAdcHalResultType FgAdcHalGetChanTrigs(FgAdcHalInterfaceType *piFgAdcHal, FcAdcChannelType eChannel, uint8 *pucTriggers)
{
   FgAdcHalResultType result;
   uint8 aucData[1];
   uint8 ucOffset;

   switch (eChannel)
   {
      case FGADC_CHAN_SKIN_TEMP:
         /* Fall through */
      case FGADC_CHAN_AUX_THERM:
         ucOffset = PMIO_ADDRX(0, AUX_THERM_TRIGGER);
         break;
      case FGADC_CHAN_BATT_ID:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_FRESH:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_5:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_15:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_150:
         ucOffset = PMIO_ADDRX(0, BATT_ID_TRIGGER);
         break;
      case FGADC_CHAN_BATT_THERM:
         ucOffset = PMIO_ADDRX(0, BATT_THERM_TRIGGER);
         break;
      case FGADC_CHAN_USB_IN_V:
         ucOffset = PMIO_ADDRX(0, USB_IN_V_TRIGGER);
         break;
      case FGADC_CHAN_USB_IN_I:
         ucOffset = PMIO_ADDRX(0, USB_IN_I_TRIGGER);
         break;
      case FGADC_CHAN_DC_IN_V:
         ucOffset = PMIO_ADDRX(0, DC_IN_V_TRIGGER);
         break;
      case FGADC_CHAN_DC_IN_I:
         ucOffset = PMIO_ADDRX(0, DC_IN_I_TRIGGER);
         break;
      case FGADC_CHAN_DIE_TEMP:
         ucOffset = PMIO_ADDRX(0, PMI_DIE_TEMP_TRIGGER);
         break;
      case FGADC_CHAN_CHARGER_TEMP:
         ucOffset = PMIO_ADDRX(0, CHARGER_TEMP_TRIGGER);
         break;
      case FGADC_CHAN_GPIO:
         ucOffset = PMIO_ADDRX(0, GPIO_TRIGGER);
         break;
      default:
         return FGADC_HAL_ERROR;
   }

   result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                     ucOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != FGADC_HAL_SUCCESS)
   {
      return result;
   }

   *pucTriggers = aucData[0];

   return result;
}

FgAdcHalResultType FgAdcHalSetChanTrigs(FgAdcHalInterfaceType *piFgAdcHal, FcAdcChannelType eChannel, uint8 ucTriggers)
{
   uint8 aucData[1];
   uint8 ucOffset;

   switch (eChannel)
   {
      case FGADC_CHAN_SKIN_TEMP:
         /* Fall through */
      case FGADC_CHAN_AUX_THERM:
         ucOffset = PMIO_ADDRX(0, AUX_THERM_TRIGGER);
         break;
      case FGADC_CHAN_BATT_ID:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_FRESH:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_5:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_15:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_150:
         ucOffset = PMIO_ADDRX(0, BATT_ID_TRIGGER);
         break;
      case FGADC_CHAN_BATT_THERM:
         ucOffset = PMIO_ADDRX(0, BATT_THERM_TRIGGER);
         break;
      case FGADC_CHAN_USB_IN_V:
         ucOffset = PMIO_ADDRX(0, USB_IN_V_TRIGGER);
         break;
      case FGADC_CHAN_USB_IN_I:
         ucOffset = PMIO_ADDRX(0, USB_IN_I_TRIGGER);
         break;
      case FGADC_CHAN_DC_IN_V:
         ucOffset = PMIO_ADDRX(0, DC_IN_V_TRIGGER);
         break;
      case FGADC_CHAN_DC_IN_I:
         ucOffset = PMIO_ADDRX(0, DC_IN_I_TRIGGER);
         break;
      case FGADC_CHAN_DIE_TEMP:
         ucOffset = PMIO_ADDRX(0, PMI_DIE_TEMP_TRIGGER);
         break;
      case FGADC_CHAN_CHARGER_TEMP:
         ucOffset = PMIO_ADDRX(0, CHARGER_TEMP_TRIGGER);
         break;
      case FGADC_CHAN_GPIO:
         ucOffset = PMIO_ADDRX(0, GPIO_TRIGGER);
         break;
      default:
         return FGADC_HAL_ERROR;
   }

   aucData[0] = ucTriggers;

   return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                    ucOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData));
}

FgAdcHalResultType FgAdcHalGetChanStatus(FgAdcHalInterfaceType *piFgAdcHal, FcAdcChannelType eChannel, FgAdcChanStatusType *pStatus)
{
   FgAdcHalResultType result;
   uint8 aucData[1];
   uint8 ucOffset;

   switch (eChannel)
   {
      case FGADC_CHAN_SKIN_TEMP:
         /* Fall through */
      case FGADC_CHAN_AUX_THERM:
         ucOffset = PMIO_ADDRX(0, AUX_THERM_STS);
         break;
      case FGADC_CHAN_BATT_ID:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_FRESH:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_5:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_15:
         /* Fall through */
      case FGADC_CHAN_BATT_ID_150:
         ucOffset = PMIO_ADDRX(0, BATT_ID_STS);
         break;
      case FGADC_CHAN_BATT_THERM:
         ucOffset = PMIO_ADDRX(0, BATT_THERM_STS);
         break;
      case FGADC_CHAN_USB_IN_V:
         ucOffset = PMIO_ADDRX(0, USB_IN_V_STS);
         break;
      case FGADC_CHAN_USB_IN_I:
         ucOffset = PMIO_ADDRX(0, USB_IN_I_STS);
         break;
      case FGADC_CHAN_DC_IN_V:
         ucOffset = PMIO_ADDRX(0, DC_IN_V_STS);
         break;
      case FGADC_CHAN_DC_IN_I:
         ucOffset = PMIO_ADDRX(0, DC_IN_I_STS);
         break;
      case FGADC_CHAN_DIE_TEMP:
         ucOffset = PMIO_ADDRX(0, PMI_DIE_TEMP_STS);
         break;
      case FGADC_CHAN_CHARGER_TEMP:
         ucOffset = PMIO_ADDRX(0, CHARGER_TEMP_STS);
         break;
      case FGADC_CHAN_GPIO:
         ucOffset = PMIO_ADDRX(0, GPIO_STS);
         break;
      default:
         return FGADC_HAL_ERROR;
   }

   result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                     ucOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != FGADC_HAL_SUCCESS)
   {
      return result;
   }

   if (aucData[0] & FGADC_STS_CHANNEL_STS_BMSK)
   {
      pStatus->bReadingAvailable = TRUE;
   }
   else
   {
      pStatus->bReadingAvailable = FALSE;
   }

   if (aucData[0] & FGADC_STS_READING_STS_BMSK)
   {
      pStatus->bReadingFresh = TRUE;
   }
   else
   {
      pStatus->bReadingFresh = FALSE;
   }

   /* CHANNEL_STS may not get set for BATT_ID - need to check
      the RT interrupt status bit instead */
   if ((eChannel == FGADC_CHAN_BATT_ID ||
        eChannel == FGADC_CHAN_BATT_ID_FRESH ||
        eChannel == FGADC_CHAN_BATT_ID_5 ||
        eChannel == FGADC_CHAN_BATT_ID_15 ||
        eChannel == FGADC_CHAN_BATT_ID_150) &&
       !pStatus->bReadingAvailable)
   {
      ucOffset = PMIO_ADDRX(0, INT_RT_STS);

      result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                        ucOffset,
                                        aucData,
                                        ARRAY_LENGTH(aucData));
      if (result != FGADC_HAL_SUCCESS)
      {
         return result;
      }

      if (aucData[0] & PMIO_FMSK(INT_RT_STS, BT_ID))
      {
         pStatus->bReadingAvailable = TRUE;
      }
      else
      {
         pStatus->bReadingAvailable = FALSE;
      }
   }

   return result;
}

FgAdcHalResultType FgAdcHalGetConversionCode(FgAdcHalInterfaceType *piFgAdcHal, FcAdcChannelType eChannel, uint32 *puCode)
{
   FgAdcHalResultType result;
   uint8 aucData[2];
   uint8 ucOffset;
   uint32 uCode1;
   uint32 uCode2 = 0;
   uint8 i;

   switch (eChannel)
   {
      case FGADC_CHAN_SKIN_TEMP:
         ucOffset = PMIO_ADDRX(0, SKIN_TEMPERATURE_LSB);
         break;
      case FGADC_CHAN_BATT_ID_5:
         ucOffset = PMIO_ADDRX(0, BATT_ID_5_LSB);
         break;
      case FGADC_CHAN_BATT_ID_15:
         ucOffset = PMIO_ADDRX(0, BATT_ID_15_LSB);
         break;
      case FGADC_CHAN_BATT_ID_150:
         ucOffset = PMIO_ADDRX(0, BATT_ID_150_LSB);
         break;
      case FGADC_CHAN_BATT_THERM:
         ucOffset = PMIO_ADDRX(0, BATT_THERM_LSB);
         break;
      case FGADC_CHAN_AUX_THERM:
         ucOffset = PMIO_ADDRX(0, SKIN_TEMPERATURE_LSB);
         break;
      case FGADC_CHAN_USB_IN_V:
         ucOffset = PMIO_ADDRX(0, USB_IN_V_LSB);
         break;
      case FGADC_CHAN_USB_IN_I:
         ucOffset = PMIO_ADDRX(0, USB_IN_I_LSB);
         break;
      case FGADC_CHAN_DC_IN_V:
         ucOffset = PMIO_ADDRX(0, DC_IN_V_LSB);
         break;
      case FGADC_CHAN_DC_IN_I:
         ucOffset = PMIO_ADDRX(0, DC_IN_I_LSB);
         break;
      case FGADC_CHAN_DIE_TEMP:
         ucOffset = PMIO_ADDRX(0, PMI_DIE_TEMP_LSB);
         break;
      case FGADC_CHAN_CHARGER_TEMP:
         ucOffset = PMIO_ADDRX(0, CHARGER_TEMP_LSB);
         break;
      case FGADC_CHAN_GPIO:
         ucOffset = PMIO_ADDRX(0, GPIO_LSB);
         break;
      default:
         return FGADC_HAL_ERROR;
   }

   for (i = 0; i < FGADC_HAL_MAX_NUM_READ_TRIES; i++)
   {
      result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                        ucOffset,
                                        aucData,
                                        ARRAY_LENGTH(aucData));
      if (result != FGADC_HAL_SUCCESS)
      {
         return result;
      }

      uCode1 = ((aucData[1] & FGADC_MSB_ADC_READ_BMSK) << 8) | aucData[0];

      result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                        ucOffset,
                                        aucData,
                                        ARRAY_LENGTH(aucData));
      if (result != FGADC_HAL_SUCCESS)
      {
         return result;
      }

      uCode2 = ((aucData[1] & FGADC_MSB_ADC_READ_BMSK) << 8) | aucData[0];

      if (uCode1 == uCode2)
      {
         break;
      }
   }

   *puCode = uCode2;

   return FGADC_HAL_SUCCESS;
}

FgAdcHalResultType FgAdcHalSetThreshold(FgAdcHalInterfaceType *piFgAdcHal, FgAdcThresholdType eThresh, uint32 uCode)
{
   uint8 aucData[1];
   uint8 ucOffset;

   switch (eThresh)
   {
      case FGADC_THRESH_SKIN_HOT:
         ucOffset = PMIO_ADDRX(0, SKIN_HOT);
         break;
      case FGADC_THRESH_SKIN_TOO_HOT:
         ucOffset = PMIO_ADDRX(0, SKIN_TOO_HOT);
         break;
      case FGADC_THRESH_CHARGER_HOT:
         ucOffset = PMIO_ADDRX(0, CHARGER_HOT);
         break;
      case FGADC_THRESH_CHARGER_TOO_HOT:
         ucOffset = PMIO_ADDRX(0, CHARGER_TOO_HOT);
         break;
      default:
         return FGADC_HAL_ERROR;
   }

   aucData[0] = (uint8)uCode;

   return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                    ucOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData));
}

FgAdcHalResultType FgAdcHalClearThresholdComparison(FgAdcHalInterfaceType *piFgAdcHal, FgAdcThresholdType eThresh)
{
   FgAdcHalResultType result;
   uint8 aucData[1];
   uint8 ucOffset;
   uint8 ucOrigVal;

   if (eThresh == FGADC_THRESH_SKIN_HOT ||
       eThresh == FGADC_THRESH_SKIN_TOO_HOT)
   {
      ucOffset = PMIO_ADDRX(0, AUX_THERM_CTRL);

      result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                        ucOffset,
                                        aucData,
                                        ARRAY_LENGTH(aucData));
      if (result != FGADC_HAL_SUCCESS)
      {
         return result;
      }

      ucOrigVal = aucData[0];

      aucData[0] &= ~PMIO_FMSK(AUX_THERM_CTRL, AUX_CMP_CLEAR);
      aucData[0] |= PMIO_FVAL(AUX_THERM_CTRL, AUX_CMP_CLEAR, 1);

      result = piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                         ucOffset,
                                         aucData,
                                         ARRAY_LENGTH(aucData));
      if (result != FGADC_HAL_SUCCESS)
      {
         return result;
      }

      aucData[0] = ucOrigVal;

      return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                       ucOffset,
                                       aucData,
                                       ARRAY_LENGTH(aucData));
   }
   else
   {
      ucOffset = PMIO_ADDRX(0, CHARGER_TEMP_CFG);

      result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                        ucOffset,
                                        aucData,
                                        ARRAY_LENGTH(aucData));
      if (result != FGADC_HAL_SUCCESS)
      {
         return result;
      }

      ucOrigVal = aucData[0];

      aucData[0] &= ~PMIO_FMSK(CHARGER_TEMP_CFG, TEMP_CMP_CLEAR);
      aucData[0] |= PMIO_FVAL(CHARGER_TEMP_CFG, TEMP_CMP_CLEAR, 1);

      result = piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                         ucOffset,
                                         aucData,
                                         ARRAY_LENGTH(aucData));
      if (result != FGADC_HAL_SUCCESS)
      {
         return result;
      }

      aucData[0] = ucOrigVal;

      return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                       ucOffset,
                                       aucData,
                                       ARRAY_LENGTH(aucData));
   }
}

FgAdcHalResultType FgAdcHalDumpRegisters(FgAdcHalInterfaceType *piFgAdcHal, FgAdcHalRegDumpType *pFgAdcRegDump)
{
   FgAdcHalResultType result;
   uint8 aucData[1];
   uint8 ucOffset;
   uint32 uReg;

   for (uReg = 0; uReg < FGADC_HAL_NUM_REGISTERS_TO_DUMP; uReg++)
   {
      ucOffset = gaucRegDumpAddresses[uReg];

      result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                        ucOffset,
                                        aucData,
                                        ARRAY_LENGTH(aucData));
      if (result != FGADC_HAL_SUCCESS)
      {
         return result;
      }

      pFgAdcRegDump->aFgAdcReg[uReg].ucOffset = ucOffset;
      pFgAdcRegDump->aFgAdcReg[uReg].ucVal = aucData[0];
   }

   return FGADC_HAL_SUCCESS;
}

FgAdcHalResultType FgAdcHalSetContModeEn(FgAdcHalInterfaceType *piFgAdcHal, FgAdcEnableType eEnable)
{
   FgAdcHalResultType result;
   uint8 aucData[1];
   uint8 ucOffset = PMIO_ADDRX(0, RR_ADC_CTL);

   result = piFgAdcHal->pfnReadBytes(piFgAdcHal->pCtxt,
                                     ucOffset,
                                     aucData,
                                     ARRAY_LENGTH(aucData));
   if (result != FGADC_HAL_SUCCESS)
   {
      return result;
   }

   aucData[0] &= ~PMIO_FMSK(RR_ADC_CTL, CONTINOUS_SEQUENCE_INITIATE);
   if (eEnable)
   {
      aucData[0] |= PMIO_FVAL(RR_ADC_CTL, CONTINOUS_SEQUENCE_INITIATE, 1);
   }

   return piFgAdcHal->pfnWriteBytes(piFgAdcHal->pCtxt,
                                    ucOffset,
                                    aucData,
                                    ARRAY_LENGTH(aucData));
}


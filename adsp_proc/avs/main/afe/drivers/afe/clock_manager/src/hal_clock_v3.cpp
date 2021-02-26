/*========================================================================
   This file contains audio clock specific configuration.

  Copyright (c) 2011-2017 Qualcomm Technologies Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/hal_clock_v3.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "Elite.h"
#include "common.h"
#include "adsp_afe_service_commands.h"
#include "hal_clock_i.h"
#include "LPASS_ADDRESS_FILE_TCSR_HAL_V3.h"
#include "clock_debug_i.h"

/*==========================================================================
  Function Prototypes
========================================================================== */

/*==========================================================================
  Globals
========================================================================== */

/*==========================================================================
  Function implementations
========================================================================== */
// Calls the mux_sel function to set the default bitclocks for the chipset
void HAL_clock_mux_sel_default(uint32_t virbase)
{
  HAL_clock_mux_sel(AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT,virbase);
  HAL_clock_mux_sel(AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT,virbase);
  HAL_clock_mux_sel(AFE_CLOCK_SET_CLOCK_ID_TER_PCM_EBIT,virbase);
  HAL_clock_mux_sel(AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_EBIT,virbase);
  HAL_clock_mux_sel(AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_EBIT,virbase);
}

ADSPResult HAL_clock_locate_clkid_pair(uint32_t clkid,
  uint32_t *dividend_clkid_ptr, uint32_t *divisor_clkid_ptr)
{
  ADSPResult result = ADSP_EOK;

  // Locate (dividend, divisor) paired clk-id based on input clk-id
  switch(clkid)
  {
    case AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_OSR:
    case AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_IBIT:
      *dividend_clkid_ptr = AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_OSR;
      *divisor_clkid_ptr = AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_IBIT;
      break;

    case AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_OSR:
    case AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT:
      *dividend_clkid_ptr = AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_OSR;
      *divisor_clkid_ptr = AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT;
      break;

    case AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_OSR:
    case AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_IBIT:
      *dividend_clkid_ptr = AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_OSR;
      *divisor_clkid_ptr = AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_IBIT;
      break;

    default:
      *dividend_clkid_ptr = 0;
      *divisor_clkid_ptr = 0;
      result = ADSP_EFAILED;
      break;
  }
  return result;
}

void HAL_clock_mux_sel(uint32_t clkid, uint32_t virbase)
{
  uint32_t Reg;
  uint32_t mask;
  uint32_t fieldValue;

  switch(clkid)
  {
    // Primary MI2S/PCM internal clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_IBIT:
    case AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_IBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL___M;
      fieldValue = (0 << LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL___S);
      break;
    // Primary MI2S/PCM external clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_PRI_MI2S_EBIT:
    case AFE_CLOCK_SET_CLOCK_ID_PRI_PCM_EBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL___M;
      fieldValue = (1 << LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL___S);
      break;
    // Secondary MI2S/PCM internal clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_IBIT:
    case AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_IBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL___M;
      fieldValue = (0 << LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL___S);
      break;
    // Secondary MI2S/PCM external clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_SEC_MI2S_EBIT:
    case AFE_CLOCK_SET_CLOCK_ID_SEC_PCM_EBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL___M;
      fieldValue = (1 << LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL___S);
      break;
    // Tertiary MI2S/PCM internal clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_IBIT:
    case AFE_CLOCK_SET_CLOCK_ID_TER_PCM_IBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL___M;
      fieldValue = (0 << LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL___S);
      break;
    // Tertiary MI2S/PCM external clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_TER_MI2S_EBIT:
    case AFE_CLOCK_SET_CLOCK_ID_TER_PCM_EBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL___M;
      fieldValue = (1 << LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL___S);
      break;
    // Quantery MI2S/PCM internal clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_IBIT:
    case AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_IBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL___M;
      fieldValue = (0 << LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL___S);
      break;
    // Quantery MI2S/PCM external clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_QUAD_MI2S_EBIT:
    case AFE_CLOCK_SET_CLOCK_ID_QUAD_PCM_EBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL___M;
      fieldValue = (1 << LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL___S);
      break;

      // SPEAKER MI2S internal clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_IBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL___M;
      fieldValue = (0 << LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL___S);
      break;

      // SPEAKER I2S external clock MUX select
    case AFE_CLOCK_SET_CLOCK_ID_SPEAKER_I2S_EBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL___M;
      fieldValue = (1 << LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL___S);
      break;

   // Quinary MI2S/PCM internal clock MUX select
   case AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_IBIT:
   case AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_IBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL___M;
      fieldValue = (0 << LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL___S);
      break;

   // Quinary MI2S/PCM external clock MUX select
   case AFE_CLOCK_SET_CLOCK_ID_QUI_MI2S_EBIT:
   case AFE_CLOCK_SET_CLOCK_ID_QUI_PCM_EBIT:
      Reg = LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL;
      mask = LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL___M;
      fieldValue = (1 << LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL___S);
      break;
    // No MUX config
    default:
      Reg = 0;
      mask = 0;
      fieldValue = 0;
      break;
  }
#ifdef SIM
  if(0 != Reg)
  {
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
      "MUX select tcsr addr: 0x%lx mask: 0x%lx val: 0x%lx", Reg, mask, fieldValue);
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MUX select none..");
  }
#else
  if(0 != Reg)
  {
#ifdef DEBUG_DUMP_MUX_CLK
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
      "TCSR addr: 0x%lx mask: 0x%lx val: 0x%lx", Reg, mask, fieldValue);
#endif
    // Figure out the offset to update the register
    int32_t tcsrVirOffset = (virbase - LPASS_LPASS_TCSR_BASE) >> 2;
    UpdateRegister1(Reg, tcsrVirOffset, mask, fieldValue, 0);
}
#endif

}

#ifdef DEBUG_DUMP_MUX_CLK
void HAL_clock_dump_mux(uint32_t virbase)
{
  int32_t tcsrVirOffset;
  uint32_t regVal;

  tcsrVirOffset = (virbase - LPASS_LPASS_TCSR_BASE) >> 2;

  ReadRegister1(LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL, tcsrVirOffset, &regVal);
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
    "==== LPASS_AUDIO_CORE_LPAIF_PRI_MODE_MUXSEL = 0x%x ", regVal);

  ReadRegister1(LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL, tcsrVirOffset, &regVal);
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
    "==== LPASS_AUDIO_CORE_LPAIF_SEC_MODE_MUXSEL = 0x%x ", regVal);

  ReadRegister1(LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL, tcsrVirOffset, &regVal);
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
    "==== LPASS_AUDIO_CORE_LPAIF_TER_MODE_MUXSEL = 0x%x ", regVal);

  ReadRegister1(LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL, tcsrVirOffset, &regVal);
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
    "==== LPASS_AUDIO_CORE_LPAIF_QUAD_MODE_MUXSEL = 0x%x ", regVal);

  ReadRegister1(LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL, tcsrVirOffset, &regVal);
  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
    "==== LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_MODE_MUXSEL = 0x%x ", regVal);

}
#endif

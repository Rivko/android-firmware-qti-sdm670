/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "anc_map"
/*#define LOG_NDEBUG 0*/
#define LOG_NDDEBUG 0

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <linux/mfd/wcd9xxx/wcd9320_registers.h>
/*#include <linux/mfd/wcd9xxx/wcd9335_registers.h>*/

#include "anc_map_api.h"
#include "acdb-anc-general.h"
#include "anc_bitmasks.h"
#include "wcd9335_anc_registers.h"
#include "wcd9340_anc_registers.h"

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

#ifdef _ANDROID_
#include <cutils/properties.h>
#include <utils/Log.h>
#include "common_log.h"
#else
#define LOGI(...)      fprintf(stdout,__VA_ARGS__)
#define LOGE(...)      fprintf(stderr,__VA_ARGS__)
#define LOGV(...)      fprintf(stderr,__VA_ARGS__)
#define LOGD(...)      fprintf(stderr,__VA_ARGS__)
#endif

#define INVALID_DATA     -1

#define COEFF_PRECISION (1 << 13)

#define UNITY_GAIN (0x80) //Q8 format
#define WCD9335_CHECK_SEQUENCE_ENTRIES (2*10)
#define WCD9335_CHECK_SEQUENCE_ENTRIES_PER_CH (WCD9335_CHECK_SEQUENCE_ENTRIES/2)
#define NUM_ANC_CH (2)
#define TEMP_CAL_BUFSZ 1024 /* 1K should be plenty */
#define MAX_COL_SIZE   324

#define WCD9335_NUM_ANC_CH0_REG (24)
#define WCD9340_NUM_ANC_CH0_REG (24)

#define round(val) ((val > 0) ? (val + 0.5) : (val - 0.5))

typedef enum anc_ch_t
{
   ANC0 = 0, ANC1, ANC_CH_END
}anc_ch_t;

static uint8_t         *virt_cal_buffer = NULL;

#define ABS(x) (((x) < 0) ? (-1 * (x)) : (x))

static codec_version_t snd_card_type = TAIKO;

int32_t FP_mult(int32_t val1, int32_t val2)
{
     int32_t prod = 0;

     if ((val1 > 0 && val2 > 0) || (val1 < 0 && val2 < 0)) {
        if (ABS(val1) > (int32_t) (MAX_INT / ABS(val2)))
            prod = MAX_INT;
     } else if ((val1 > 0 && val2 < 0) || (val1 < 0 && val2 > 0)) {
                 if (ABS(val1) > (int32_t) (MAX_INT / ABS(val2)))
                     prod = -(int32_t) MAX_INT;
     }

     if (0 == prod)
         prod = val1 * val2;

     return prod;
}

int32_t FP_shift(int32_t val, int32_t shift)
{
     int32_t rnd = 1 << (ABS(shift) - 1);
     int32_t val_s = val;

     if (shift < 0) {
         /* underflow -> rounding errors */
         val_s = ABS(val_s) + rnd;
         val_s = val_s >> ABS(shift);
         val_s = (val > 0) ? val_s : -val_s;
     } else if (shift > 0) {
         /* overflow -> saturation */
         if (ABS(val) > (int32_t) ((MAX_INT >> ABS(shift)))) {
             if (val < 0)
                 val_s = -(int32_t) MAX_INT;
             else
                 val_s = (int32_t) MAX_INT;
         } else
             val_s = val << ABS(shift);
     }

     return val_s;
}

uint16_t twosComp(int16_t val, int16_t bits)
{
    uint16_t res = SUCCESS;
    uint32_t width = bits + 1;

    if (val >= 0)
        res = (uint16_t) val;
    else
        res = -((-val) - (1 << width));

    return res;
}

int32_t FP_format(int32_t val, int32_t intb, int32_t fracb, int32_t max_val)
{
    val = FP_shift(val, -(ANC_COEFF_FRAC_BITS - fracb));

    /* Check for saturation */
    if (val > max_val)
        val = max_val;
    else if (val < -max_val)
        val = -max_val;

    /* convert to 2s compl */
    val = twosComp((uint16_t) val, (uint16_t) (intb + fracb));

    return val;
}

// Tasha and forward, coeffs writing
// %
//  Register Address:	0xA0C / 0xA24
//  CDC_ANCn_IIR_COEFF_1_CTL
//  Default:	0x00
//  bit7 : Setting this bit to 1, will enable autoincrement.
//        0 will make coeff pointed by pointer field to be updated
//  bit 6:0 : COEF_PTR This register points to a bank of 45x12 Coeffs
//            used for the FFa, FFb and FB Main IIR Filters.
//
//  One ANC Contains 45 x 12-Bit Width Coeffs used for the FFa, FFb and FB Main IIR
//  Filter.
//  The 12-Bit Coeff can be read/written to via ANCn_IIR_COEFF. SInce, ANCn_IIR_COEFF is 8-
//  bits width. 2 Writes is needed to form the 12-Bit Coeff. The first write required is for the upper 4-
//  Bits and the 2nd write is for the lower 8-bits. ANCn_IIR_COEFF is autoincremented.
//
//  Denominator FFa Filter
//  PTR[0] = LSB A(1), PTR[1] = MSB A(1)....
//  PTR[12] = LSB A(7) , PTR[13] = MSB A(7)
//
//  Numerator FFa Filter
//  PTR[14] = LSB B(1), PTR[15]=MSB B(1)....
//  PTR[28] = LSB B(8) , PTR[29] = MSB B(8)
//
//  Denominator FFb Filter
//  PTR[30] = LSB C(1), PTR[1] = MSB C(31)....
//  PTR[42] = LSB C(7) , PTR[43] = MSB C(7)
//
//  Numerator FFb Filter
//  PTR[44] = LSB D(1), PTR[45]=MSB D(1)....
//  PTR[58] = LSB D(8) , PTR[59] = MSB D(8)
//
//  Denominator FB Filter
//  PTR[60] = LSB V(1), PTR[1] = MSB V(61)....
//  PTR[72] = LSB V(7) , PTR[73] = MSB V(7)
//
//  Numerator FB Filter
//  PTR[74] = LSB W(1), PTR[75]=MSB W(1)....
//  PTR[88] = LSB W(8) , PTR[89] = MSB W(8)
//
//  Register Address:				0xA0D / 0xA25
//  DC_ANC0_IIR_COEFF_2_CTL
//  Default:	0x00
//  COEF_LSB must be written first and COEF_MSB written second.
//  COEFF are 12-Bit Signed Values from -2047 -> +2047
//  --> Writes COEFF MSB or LSB (bit 7: 0)


return_code_t Setwcd9xxxANC_HW5_IIRCoeffs(uint32_t *anc_config, uint32_t *anc_index,
      int8_t *anc_acdb_ptr, uint32_t ancCh, codec_version_t codec_version __unused)
{
   return_code_t res = SUCCESS;
   int32_t coeff = 0;
   uint32_t u_coeff = 0;
   uint8_t valMSBs = 0;
   uint8_t valLSBs = 0;
   uint32_t iter = 0;
   uint8_t iir_index=0;
   double cal_gain = 0;
   int32_t temp_int = 0;

   anc_wcd_hw5_acdb_cfg_t *pANCCfg = (anc_wcd_hw5_acdb_cfg_t *)anc_acdb_ptr;
   uint32_t offset = ancCh * WCD9335_NUM_ANC_CH0_REG;

   /* Divide by 2^13 */

   // SW fix for HW bug fix, only auto increment mode works, hence setting the pointer right now to 0
   // write address in manual mode
   anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + offset), (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR | WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC), 0x0);
   // Write address in auto mode
   anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + offset), (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR | WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC), (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC | 0x0));

   /* Write ff_a_num and ff_a_den coeffs */
   for (iter = 0; iter < ((sizeof(pANCCfg->anc_ff_a_num_coeff) +sizeof(pANCCfg->anc_ff_a_den_coeff))/sizeof(int32_t)); iter++)
   {
      if (iter < (sizeof(pANCCfg->anc_ff_a_den_coeff)/sizeof(int32_t)))
      {
         coeff = pANCCfg[ancCh].anc_ff_a_den_coeff[iter];
         temp_int = (int32_t)round((double)(coeff)/16); // --> divide by 16 becuase codec registers of bitwidth 12 and we want to support values +3.98 to -4.0
         u_coeff = (uint32_t)temp_int;
      }
      else
      {
         coeff = pANCCfg[ancCh].anc_ff_a_num_coeff[iter-(sizeof(pANCCfg->anc_ff_a_den_coeff)/sizeof(int32_t))];
         cal_gain = ((double)pANCCfg[ancCh].anc_ff_a_gain_scale)/COEFF_PRECISION;
         temp_int = (int32_t)round((((double)coeff * cal_gain) / 16)); // --> divide by 16 becuase codec registers of bitwidth 12 and we want to support values +3.98 to -4.0
         u_coeff = (uint32_t)temp_int;
      }
      valMSBs = (uint8_t) (0x0F & (u_coeff >> REGISTER_DEPTH));
      valLSBs = (uint8_t) (0xFF & u_coeff);
      anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset), WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valLSBs);
      iir_index++; // just for debugging
      anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset), WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valMSBs);
      iir_index++; // just for debugging
   }

   /* Write ff_b_num and ff_b_den coeffs */
   for (iter = 0; iter < ((sizeof(pANCCfg->anc_ff_b_num_coeff) +sizeof(pANCCfg->anc_ff_b_den_coeff))/sizeof(int32_t)); iter++)
   {
      if (iter < (sizeof(pANCCfg->anc_ff_b_den_coeff)/sizeof(int32_t)))
      {
         coeff = pANCCfg[ancCh].anc_ff_b_den_coeff[iter];
         temp_int = (int32_t)round((double)(coeff)/16); // --> divide by 16 becuase codec registers of bitwidth 12 and we want to support values +3.98 to -4.0
         u_coeff = (uint32_t)temp_int;
      }
      else
      {
         cal_gain = ((double)pANCCfg[ancCh].anc_ff_b_gain_scale)/COEFF_PRECISION;
         coeff = pANCCfg[ancCh].anc_ff_b_num_coeff[iter-(sizeof(pANCCfg->anc_ff_b_den_coeff)/sizeof(int32_t))];
         temp_int = (int32_t)round((((double)coeff * cal_gain) / 16)); // --> divide by 16 becuase codec registers of bitwidth 12 and we want to support values +3.98 to -4.0
         u_coeff = (uint32_t)temp_int;
      }
      valMSBs = (uint8_t) (0x0F & (u_coeff >> REGISTER_DEPTH));
      valLSBs = (uint8_t) (0xFF & u_coeff);
      anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset), WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valLSBs);
      iir_index++; // just for debugging
      anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset), WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valMSBs);
      iir_index++; // just for debugging
   }

   /* Write fb_num and fb_den coeffs */
   for (iter = 0; iter < ((sizeof(pANCCfg->anc_fb_num_coeff) +sizeof(pANCCfg->anc_fb_den_coeff))/sizeof(int32_t)); iter++)
   {
      cal_gain = ((double)pANCCfg[ancCh].anc_fb_gain_scale)/COEFF_PRECISION;
      if (iter < (sizeof(pANCCfg->anc_fb_den_coeff)/sizeof(int32_t)))
      {
         coeff = pANCCfg[ancCh].anc_fb_den_coeff[iter];
         temp_int = (int32_t)round((double)(coeff)/16); // --> divide by 16 becuase codec registers of bitwidth 12 and we want to support values +3.98 to -4.0
         u_coeff = (uint32_t)temp_int;
      }
      else
      {
         coeff = pANCCfg[ancCh].anc_fb_num_coeff[iter-(sizeof(pANCCfg->anc_fb_den_coeff)/sizeof(int32_t))];
         temp_int = (int32_t)round((((double)coeff * cal_gain) / 16)); // --> divide by 16 becuase codec registers of bitwidth 12 and we want to support values +3.98 to -4.0
         u_coeff = (uint32_t)temp_int;
      }
      valMSBs = (uint8_t) (0x0F & (u_coeff >> REGISTER_DEPTH));
      valLSBs = (uint8_t) (0xFF & u_coeff);
      anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset), WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valLSBs);
      iir_index++; // just for debugging
      anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset), WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valMSBs);
      iir_index++; // just for debugging
   }

   return res;
}

//return_code_t Setwcd9xxxANC_IIRCoeffs(uint32_t *anc_config, uint32_t *anc_index,
//	struct anc_taiko_acdb_cfg_t *pANCCfg, uint32_t ancCh, codec_version_t codec_version)
return_code_t Setwcd9xxxANC_IIRCoeffs(uint32_t *anc_config, uint32_t *anc_index,
    int8_t *anc_acdb_ptr, uint32_t ancCh, codec_version_t codec_version)
{
    return_code_t res = SUCCESS;
    int32_t coeff = 0;
    uint32_t u_coeff = 0;
    uint8_t valMSBs = 0;
    uint8_t valLSBs = 0;
    uint32_t iter = 0;
    uint32_t offset = 0;
    uint8_t iir_index=0;
    double cal_gain = 0;
    int32_t temp_int = 0;

    LOGD("%s:enter", __func__);

    switch (codec_version) {
    case TAVIL:
        res = Setwcd9xxxANC_HW5_IIRCoeffs(anc_config, anc_index, anc_acdb_ptr,
                                          ancCh, codec_version);
        break;
    case TASHA:
         {
             anc_tasha_acdb_cfg_t *pANCCfg =
                                     (anc_tasha_acdb_cfg_t *)anc_acdb_ptr;
             offset = ancCh * WCD9335_NUM_ANC_CH0_REG;

            /* Write ff_a_num and ff_a_den coeffs */
            for (iter = 0; iter < ((sizeof(pANCCfg->anc_ff_a_num_coeff) +
                           sizeof(pANCCfg->anc_ff_a_den_coeff)) / sizeof(int32_t));
                           iter++) {
                if (iter < (sizeof(pANCCfg->anc_ff_a_den_coeff) / sizeof(int32_t))) {
                    coeff = pANCCfg[ancCh].anc_ff_a_den_coeff[iter];
                    /*
                     * divide by 16 becuase codec registers of bitwidth 12
                     * and we want to support values +3.98 to -4.0
                     */
                    temp_int = (int32_t)round((double)(coeff) / 16);
                    u_coeff = (uint32_t)temp_int;
                } else {
                    coeff = pANCCfg[ancCh].anc_ff_a_num_coeff[iter -
                                (sizeof(pANCCfg->anc_ff_a_den_coeff) / sizeof(int32_t))];
                    cal_gain = ((double)pANCCfg[ancCh].anc_ff_a_gain_scale) / COEFF_PRECISION;
                    /*
                     * divide by 16 becuase codec registers of bitwidth 12
                     * and we want to support values +3.98 to -4.0
                    */
                    temp_int = (int32_t)round((((double)coeff * cal_gain) / 16));
                    u_coeff = (uint32_t)temp_int;
               }
               valMSBs = (uint8_t) (0x0F & (u_coeff >> REGISTER_DEPTH));
               valLSBs = (uint8_t) (0xFF & u_coeff);

               anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((
                         WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + offset),
                         (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR |
                         WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC),
                         iir_index++);

               anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((
                         WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset),
                         WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valLSBs);

               anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((
                         WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + offset),
                         (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR |
                         WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC),
                         iir_index++);

               anc_config[(*anc_index)++] = REG_CODEC_PACK_ENTRY((
                         WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset),
                         WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valMSBs);
          }

      /* Write ff_b_num and ff_b_den coeffs */
      for (iter = 0; iter < ((sizeof(pANCCfg->anc_ff_b_num_coeff) +
           sizeof(pANCCfg->anc_ff_b_den_coeff)) / sizeof(int32_t)); iter++) {
         if (iter < (sizeof(pANCCfg->anc_ff_b_den_coeff) / sizeof(int32_t))) {
            coeff = pANCCfg[ancCh].anc_ff_b_den_coeff[iter];
            /*
             * divide by 16 becuase codec registers of bitwidth 12
             * and we want to support values +3.98 to -4.0
             */
            temp_int = (int32_t)round((double)(coeff) / 16);
            u_coeff = (uint32_t)temp_int;
         } else {
            cal_gain = ((double)pANCCfg[ancCh].anc_ff_b_gain_scale) / COEFF_PRECISION;
            coeff = pANCCfg[ancCh].anc_ff_b_num_coeff[iter -
                      (sizeof(pANCCfg->anc_ff_b_den_coeff) / sizeof(int32_t))];
            /*
             * divide by 16 becuase codec registers of bitwidth 12 and
             * we want to support values +3.98 to -4.0
             */
            temp_int = (int32_t)round((((double)coeff * cal_gain) / 16));
            u_coeff = (uint32_t)temp_int;
         }

         valMSBs = (uint8_t) (0x0F & (u_coeff >> REGISTER_DEPTH));
         valLSBs = (uint8_t) (0xFF & u_coeff);

         anc_config[(*anc_index)++] =
                REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + offset),
                (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR |
                 WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC), iir_index++);

         anc_config[(*anc_index)++] =
                REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset),
                WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valLSBs);

         anc_config[(*anc_index)++] =
                REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + offset),
                (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR |
                 WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC), iir_index++);

         anc_config[(*anc_index)++] =
                REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset),
                WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valMSBs);
      }

      /* Write fb_num and fb_den coeffs */
      for (iter = 0; iter < ((sizeof(pANCCfg->anc_fb_num_coeff) +
           sizeof(pANCCfg->anc_fb_den_coeff)) / sizeof(int32_t)); iter++) {
           cal_gain = ((double)pANCCfg[ancCh].anc_fb_gain_scale) / COEFF_PRECISION;

           if (iter < (sizeof(pANCCfg->anc_fb_den_coeff) / sizeof(int32_t))) {
               coeff = pANCCfg[ancCh].anc_fb_den_coeff[iter];
               temp_int = (int32_t)round((double)(coeff) / 16);
               /*
                * divide by 16 becuase codec registers of bitwidth 12
                * and we want to support values +3.98 to -4.0
                */
               u_coeff = (uint32_t)temp_int;
           } else {
             coeff = pANCCfg[ancCh].anc_fb_num_coeff[iter -
                    (sizeof(pANCCfg->anc_fb_den_coeff) / sizeof(int32_t))];
             /*
              * divide by 16 becuase codec registers of bitwidth 12
              * and we want to support values +3.98 to -4.0
              */
             temp_int = (int32_t)round((((double)coeff * cal_gain) / 16));
             u_coeff = (uint32_t)temp_int;
          }

          valMSBs = (uint8_t) (0x0F & (u_coeff >> REGISTER_DEPTH));
          valLSBs = (uint8_t) (0xFF & u_coeff);

          anc_config[(*anc_index)++] =
             REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + offset),
             (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR |
              WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC), iir_index++);

         anc_config[(*anc_index)++] =
             REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset),
             WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valLSBs);

         anc_config[(*anc_index)++] =
             REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + offset),
             (WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR |
              WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC), iir_index++);

         anc_config[(*anc_index)++] =
             REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL + offset),
             WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF, valMSBs);
      }
      break;
   }
   case TAIKO:
   default:
   {
         anc_taiko_acdb_cfg_t *pANCCfg = (anc_taiko_acdb_cfg_t *)anc_acdb_ptr;
         offset = ancCh * 128;

         /* Divide by 2^13 */
         cal_gain = ((double)pANCCfg[ancCh].anc_gain) / COEFF_PRECISION;

         /* Write FF coeffs */
         for (iter = 0; iter < TAIKO_ANC_NUM_IIR_FF_A_COEFFS +
                        TAIKO_ANC_NUM_IIR_FF_B_COEFFS; iter++) {
              coeff = pANCCfg[ancCh].anc_ff_coeff[iter];

              if (iter < TAIKO_ANC_NUM_IIR_FF_A_COEFFS) {
                  temp_int = (int32_t)round((double)(coeff) / 16);
                  u_coeff = (uint32_t)temp_int;
              } else {
                  temp_int = (int32_t)round((((double)coeff * cal_gain) / 16));
                  u_coeff = (uint32_t)temp_int;
              }

              valMSBs = (uint8_t) (0x0F & (u_coeff >> REGISTER_DEPTH));
              valLSBs = (uint8_t) (0xFF & u_coeff);
              anc_config[(*anc_index)++] =
                        REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B2_CTL +
                        offset), 0xFF, iir_index++);

              anc_config[(*anc_index)++] =
                        REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B3_CTL +
                        offset), 0xFF, valLSBs);

             anc_config[(*anc_index)++] =
                        REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B2_CTL +
                        offset), 0xFF, iir_index++);

             anc_config[(*anc_index)++] =
                        REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B3_CTL +
                        offset), 0xFF, valMSBs);
       }

        /* Write FB coeff */
        for (iter = 0; iter < TAIKO_ANC_NUM_IIR_FB_A_COEFFS +
                       TAIKO_ANC_NUM_IIR_FB_B_COEFFS; iter++) {
             coeff = pANCCfg[ancCh].anc_fb_coeff[iter];
             temp_int = (int32_t)round((double)(coeff) / 16);
             u_coeff = (uint32_t)temp_int;
             valMSBs = (uint8_t) (0x0F & (u_coeff >> REGISTER_DEPTH));
             valLSBs = (uint8_t) (0xFF & u_coeff);
             anc_config[(*anc_index)++] =
                       REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B2_CTL +
                       offset), 0xFF, iir_index++);
             anc_config[(*anc_index)++] =
                       REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B3_CTL +
                       offset), 0xFF, valLSBs);
             anc_config[(*anc_index)++] =
                       REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B2_CTL +
                       offset), 0xFF, iir_index++);
             anc_config[(*anc_index)++] =
                       REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B3_CTL +
                       offset), 0xFF, valMSBs);
           }
       }
       break;
     }

     LOGD("%s:leave", __func__);
     return res;
}


return_code_t Setwcd9xxxANC_LPFShift(uint32_t *anc_config, uint32_t *anc_index,
        struct anc_taiko_acdb_cfg_t *pANCCfg, uint32_t ancCh)
{
        return_code_t res = SUCCESS;
        int32_t coeff = 0;
        uint32_t u_coeff = 0;
        uint8_t valMSBs = 0;
        uint8_t valLSBs = 0;
        uint32_t iter = 0;
        uint32_t offset = ancCh * 128;
        uint8_t value = 0;

        LOGD("%s:enter", __func__);
        /* FF */
        value |= pANCCfg[ancCh].anc_ff_lpf_shift[0];
        value |= pANCCfg[ancCh].anc_ff_lpf_shift[1] << 4;
        anc_config[(*anc_index)++] =
                  TAIKO_CODEC_PACK_ENTRY(
                  (TAIKO_A_CDC_ANC1_LPF_B1_CTL + offset), 0xFF, value);

        /* FB */
        value = 0;
        value |= pANCCfg[ancCh].anc_fb_lpf_shift[0];
        value |= pANCCfg[ancCh].anc_fb_lpf_shift[1] << 4;
        anc_config[(*anc_index)++] =
                  TAIKO_CODEC_PACK_ENTRY(
                 (TAIKO_A_CDC_ANC1_LPF_B2_CTL + offset), 0xFF, value);

        LOGD("%s:leave", __func__);
        return res;
}

return_code_t convert_anc_hw5_hw4_acdb_to_acdb(int8_t *anc_acdb_out_ptr, uint32_t *filled_output_size __unused,
                                               uint32_t output_size, int32_t  out_struct_id __unused,
                                               int8_t *anc_acdb_in_ptr, uint32_t input_size,
                                               int32_t  in_struct_id)
{
   return_code_t res = SUCCESS;
   int32_t num_anc_ch = 1, i = 0, j = 0;
   switch (in_struct_id)
   {
      case ANC_HW5:
         {
            anc_tasha_acdb_cfg_t *anc_wcd9335_acdb_cfg_ptr = (anc_tasha_acdb_cfg_t *) anc_acdb_out_ptr;
            anc_wcd_hw5_acdb_cfg_t *anc_wcd_hw5_acdb_cfg_ptr = (anc_wcd_hw5_acdb_cfg_t *) anc_acdb_in_ptr;

            if ((sizeof(anc_tasha_acdb_cfg_t) > output_size) || (sizeof(anc_wcd_hw5_acdb_cfg_t) > input_size))
            {
               res = NOT_ENOUGH_MEMORY;
            }
            if ((input_size >= 2*sizeof(anc_wcd_hw5_acdb_cfg_t)) && (output_size >= 2*sizeof(anc_tasha_acdb_cfg_t)))
            { // input in two structures
               num_anc_ch = 2;
            }
            memset(anc_wcd9335_acdb_cfg_ptr,0,num_anc_ch*sizeof(anc_tasha_acdb_cfg_t));


            for (i = 0; i < num_anc_ch; i++)
            {
               if ((anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_input_type == SMTLNQ_ANC_MIC) || (anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_input_type == SMTLNQ_ANC_MIC))
               { // There is not Smart LnQ in WCD9335, hence unsupported
                  return NOT_SUPPORTED;
               }
               else
               {
                  anc_wcd9335_acdb_cfg_ptr[i].ff_a_input_device = (anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_input_type == ADC_ANC_MIC) ? ADIE_MAP_ADC0 : ADIE_MAP_DMIC0 ;
                  anc_wcd9335_acdb_cfg_ptr[i].ff_b_input_device = (anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_input_type == ADC_ANC_MIC) ? ADIE_MAP_ADC0 : ADIE_MAP_DMIC0 ;
               }
               anc_wcd9335_acdb_cfg_ptr[i].ff_a_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_b_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].fb_enable = anc_wcd_hw5_acdb_cfg_ptr[i].fb_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_a_in_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_in_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_b_in_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_in_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_out_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_out_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].smlpf_enable = anc_wcd_hw5_acdb_cfg_ptr[i].smlpf_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_flex_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_flex_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_a_gain_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_gain_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_b_gain_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_gain_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].fb_gain_enable = anc_wcd_hw5_acdb_cfg_ptr[i].fb_gain_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_a_dcflt_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_dcflt_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_b_dcflt_enable = anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_dcflt_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].dmic_x2_ff_a_enable = anc_wcd_hw5_acdb_cfg_ptr[i].dmic_x2_ff_a_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].dmic_x2_ff_b_enable = anc_wcd_hw5_acdb_cfg_ptr[i].dmic_x2_ff_b_enable ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_shift = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_shift ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_shift = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_shift ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_fb_shift = anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_shift ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_gain_scale = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_gain_scale ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_gain_scale_default = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_gain_scale_default ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_gain = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_gain ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_gain_scale = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_gain_scale ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_gain_scale_default = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_gain_scale_default ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_gain = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_gain ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_fb_gain_scale = anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_gain_scale ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_fb_gain_scale_default = anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_gain_scale_default ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_fb_gain = anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_gain ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[0] = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[0] ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[1] = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[1] ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_lpf_shift[0] = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_lpf_shift[0] ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_lpf_shift[1] = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_lpf_shift[1] ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_fb_lpf_shift[0] = anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_lpf_shift[0] ;
               anc_wcd9335_acdb_cfg_ptr[i].anc_fb_lpf_shift[1] = anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_lpf_shift[1] ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_a_dcflt_shift = anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_dcflt_shift ;
               anc_wcd9335_acdb_cfg_ptr[i].ff_b_dcflt_shift = anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_dcflt_shift ;
               anc_wcd9335_acdb_cfg_ptr[i].smlpf_shift = anc_wcd_hw5_acdb_cfg_ptr[i].smlpf_shift ;
               // filter coefficients conversion
               for (j = 0; j< 7;j++) // ANC HW3 (Taiko/TomTom) supports 7 denominator coeffs
               {  // using for loop instead of memcpy for the security concerns
                  anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_den_coeff[j]  = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_den_coeff[j] ;
                  anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_den_coeff[j]  = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_den_coeff[j] ;
                  anc_wcd9335_acdb_cfg_ptr[i].anc_fb_den_coeff[j]  = anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_den_coeff[j] ;
               }
               // filter coefficients conversion
               for (j = 0; j< 8;j++) // ANC HW3 (Taiko/TomTom) supports 8 numerator coeffs
               {
                  anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_num_coeff[j]  = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_num_coeff[j] ;
                  anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_num_coeff[j]  = anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_num_coeff[j] ;
                  anc_wcd9335_acdb_cfg_ptr[i].anc_fb_num_coeff[j]  = anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_num_coeff[j] ;
               }
            }
            break;
         }
      case ANC_HW4:
         {
			anc_tasha_acdb_cfg_t *anc_wcd9335_acdb_cfg_ptr = (anc_tasha_acdb_cfg_t *) anc_acdb_in_ptr;
            anc_wcd_hw5_acdb_cfg_t *anc_wcd_hw5_acdb_cfg_ptr = (anc_wcd_hw5_acdb_cfg_t *) anc_acdb_out_ptr;
            if ((sizeof(anc_tasha_acdb_cfg_t) > input_size) || (sizeof(anc_wcd_hw5_acdb_cfg_t) > output_size))
            {
               res = NOT_ENOUGH_MEMORY;
            }
            if ((output_size >= 2*sizeof(anc_wcd_hw5_acdb_cfg_t)) && (input_size >= 2*sizeof(anc_tasha_acdb_cfg_t)))
            { // input in two structures
               num_anc_ch = 2;
            }
            memset(anc_wcd_hw5_acdb_cfg_ptr,0,num_anc_ch*sizeof(anc_wcd_hw5_acdb_cfg_t));

            for (i = 0; i < num_anc_ch; i++)
            {
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_a_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_b_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].fb_enable = anc_wcd9335_acdb_cfg_ptr[i].fb_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_in_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_a_in_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_in_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_b_in_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_input_type = (anc_wcd9335_acdb_cfg_ptr[i].ff_a_input_device>ADIE_MAP_ADC5)?DMIC_ANC_MIC:ADC_ANC_MIC;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_input_type = (anc_wcd9335_acdb_cfg_ptr[i].ff_b_input_device>ADIE_MAP_ADC5)?DMIC_ANC_MIC:ADC_ANC_MIC;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_out_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_out_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].fb_mon = 0; //Ffa path IIR input goes to FB mon, default
               anc_wcd_hw5_acdb_cfg_ptr[i].smlpf_enable = anc_wcd9335_acdb_cfg_ptr[i].smlpf_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_flex_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_flex_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_gain_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_a_gain_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_gain_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_b_gain_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].fb_gain_enable = anc_wcd9335_acdb_cfg_ptr[i].fb_gain_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_dcflt_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_a_dcflt_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_dcflt_enable = anc_wcd9335_acdb_cfg_ptr[i].ff_b_dcflt_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].dmic_x2_ff_a_enable = anc_wcd9335_acdb_cfg_ptr[i].dmic_x2_ff_a_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].dmic_x2_ff_b_enable = anc_wcd9335_acdb_cfg_ptr[i].dmic_x2_ff_b_enable;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_shift = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_shift;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_shift = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_shift;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_shift = anc_wcd9335_acdb_cfg_ptr[i].anc_fb_shift;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_gain_scale = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_gain_scale;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_gain_scale_default = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_gain_scale_default;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_gain = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_gain;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_gain_scale = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_gain_scale;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_gain_scale_default = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_gain_scale_default;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_gain = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_gain;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_gain_scale = anc_wcd9335_acdb_cfg_ptr[i].anc_fb_gain_scale;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_gain_scale_default = anc_wcd9335_acdb_cfg_ptr[i].anc_fb_gain_scale_default;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_gain = anc_wcd9335_acdb_cfg_ptr[i].anc_fb_gain;
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[0] = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[0];
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[1] = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[1];
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_lpf_shift[0] = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_lpf_shift[0];
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_lpf_shift[1] = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_lpf_shift[1];
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_lpf_shift[0] = anc_wcd9335_acdb_cfg_ptr[i].anc_fb_lpf_shift[0];
               anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_lpf_shift[1] = anc_wcd9335_acdb_cfg_ptr[i].anc_fb_lpf_shift[1];
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_a_dcflt_shift = anc_wcd9335_acdb_cfg_ptr[i].ff_a_dcflt_shift;
               anc_wcd_hw5_acdb_cfg_ptr[i].ff_b_dcflt_shift = anc_wcd9335_acdb_cfg_ptr[i].ff_b_dcflt_shift;
               anc_wcd_hw5_acdb_cfg_ptr[i].smlpf_shift = anc_wcd9335_acdb_cfg_ptr[i].smlpf_shift;
               // filter coefficients conversion
               for (j=0; j< 7;j++) // ANC HW3 (Taiko/TomTom) supports 7 denominator coeffs
               {  // using for loop instead of memcpy for the security concerns
                  anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_den_coeff[j] = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_den_coeff[j];
                  anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_den_coeff[j] = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_den_coeff[j];
                  anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_den_coeff[j] = anc_wcd9335_acdb_cfg_ptr[i].anc_fb_den_coeff[j];
               }
               // filter coefficients conversion
               for (j=0; j< 8;j++) // ANC HW3 (Taiko/TomTom) supports 8 numerator coeffs
               {
                  anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_a_num_coeff[j] = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_a_num_coeff[j];
                  anc_wcd_hw5_acdb_cfg_ptr[i].anc_ff_b_num_coeff[j] = anc_wcd9335_acdb_cfg_ptr[i].anc_ff_b_num_coeff[j];
                  anc_wcd_hw5_acdb_cfg_ptr[i].anc_fb_num_coeff[j] = anc_wcd9335_acdb_cfg_ptr[i].anc_fb_num_coeff[j];
               }
            }
            break;
         }
   } //end switch (in_struct_id)
   return res;
}

return_code_t convert_anc_acdb_to_hw5_reg(struct anc_reg_t *anc_reg_config_ptr, int8_t *anc_acdb_ptr, uint32_t input_size, codec_version_t codec_version)
{
   anc_wcd_hw5_acdb_cfg_t *pANCCfg = (anc_wcd_hw5_acdb_cfg_t *)anc_acdb_ptr;
   uint32_t temp_ctl_reg_val;
   uint32_t temp_ctl_reg_mask;
   uint32_t anc_index = 0;
   return_code_t res = SUCCESS;

   uint32_t offset;
   uint32_t ancCh;

   if (TAVIL == codec_version)
   {
      for(ancCh = 0; ancCh < input_size; ancCh++) {
         offset = ancCh * WCD9340_NUM_ANC_CH0_REG;

         // apply reset before beggining to program
         temp_ctl_reg_val = 0;
         temp_ctl_reg_mask = 0;
         temp_ctl_reg_val |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_RESET;
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_RESET;
         temp_ctl_reg_val |= !WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN;
         temp_ctl_reg_mask |= !WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN;
         temp_ctl_reg_val |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_RESET;
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_RESET;
         temp_ctl_reg_val |= !WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN;
         temp_ctl_reg_mask |= !WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN;
         temp_ctl_reg_val |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_RESET;
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_RESET;
         temp_ctl_reg_val |= !WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN;
         temp_ctl_reg_mask |= !WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN;
         temp_ctl_reg_val |= ((pANCCfg[ancCh].ff_a_input_type) == SMTLNQ_ANC_MIC) ? WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_A_EN : 0; // UI input 1 == Enable SMTLnQ, 0 = Disable SMTLnQ
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_A_EN ;
         temp_ctl_reg_val |= ((pANCCfg[ancCh].ff_b_input_type) == SMTLNQ_ANC_MIC) ? WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_B_EN : 0; // UI input 1 == Enable SMTLnQ, 0 = Disable SMTLnQ
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_B_EN ;
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY(WCD9340_CDC_ANC0_CLK_RESET_CTL+offset, temp_ctl_reg_mask,  temp_ctl_reg_val);

         // Program register WCD9340_CDC_ANC0_MODE_1_CTL+offset
         temp_ctl_reg_val = 0;
         temp_ctl_reg_mask = 0;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_a_in_enable ? WCD9340_CDC_ANC0_MODE_1_CTL___FFA_IN_EN : 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_1_CTL___FFA_IN_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_b_in_enable ? WCD9340_CDC_ANC0_MODE_1_CTL___FFB_IN_EN : 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_1_CTL___FFB_IN_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_out_enable ? WCD9340_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN : 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].smlpf_enable ? WCD9340_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN : 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN;
         temp_ctl_reg_val |= ((pANCCfg[ancCh].ff_a_input_type) == DMIC_ANC_MIC) ? WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL : 0; // UI input 1 == Digital, 0 = Analog
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL;
         temp_ctl_reg_val |= ((pANCCfg[ancCh].ff_b_input_type ) >= DMIC_ANC_MIC) ? WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL : 0; // UI input 1 == Digital, 0 = Analog
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL;
         temp_ctl_reg_val |= (pANCCfg[ancCh].fb_mon) ? WCD9340_CDC_ANC0_MODE_1_CTL___FB_ON_FBMON_IS_TRUE: 0; // - 0 (legacy) : Ffa path IIR input goes to FB mon , 1 (Adaptive Signal out) : FB path IIR input goes to FB mon
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_1_CTL___FB_ON_FBMON_IS_TRUE;

         //TODO: define controls for WCD9340_CDC_ANC0_MODE_1_CTL___FB_ON_FBMON_IS_TRUE & WCD9340_CDC_ANC0_MODE_1_CTL___ANC_OUTMIX_EN
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY(WCD9340_CDC_ANC0_MODE_1_CTL+offset, temp_ctl_reg_mask, temp_ctl_reg_val );

         // Program register WCD9340_CDC_ANC0_MODE_2_CTL+offset
         temp_ctl_reg_val = 0;
         temp_ctl_reg_mask = 0;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_flex_enable ? WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN : 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_a_gain_enable ? WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN : 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_b_gain_enable ? WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN : 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_a_dcflt_enable ? WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN: 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_b_dcflt_enable ? WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN: 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].dmic_x2_ff_a_enable ? WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL: 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL;
         temp_ctl_reg_val |= (pANCCfg[ancCh].dmic_x2_ff_b_enable ? WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL: 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL;
         temp_ctl_reg_val |= (pANCCfg[ancCh].fb_gain_enable ? WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN : 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN;
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY(WCD9340_CDC_ANC0_MODE_2_CTL+offset, temp_ctl_reg_mask, temp_ctl_reg_val );

         // Program register WCD9340_CDC_ANC0_FF_SHIFT+offset
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FF_SHIFT+offset), (WCD9340_CDC_ANC0_FF_SHIFT___FFA_SHIFT | WCD9340_CDC_ANC0_FF_SHIFT___FFB_SHIFT), ((pANCCfg[ancCh].anc_ff_a_shift << 4) | pANCCfg[ancCh].anc_ff_b_shift ));

         // Program register WCD9340_CDC_ANC0_FB_SHIFT+offset
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FB_SHIFT+offset), WCD9340_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT, (pANCCfg[ancCh].anc_fb_shift));

         // Program register WCD9340_CDC_ANC0_LPF_FF_A_CTL+offset
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_LPF_FF_A_CTL+offset), (WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2 | WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1), (pANCCfg[ancCh].anc_ff_a_lpf_shift[1] << 4)|(pANCCfg[ancCh].anc_ff_a_lpf_shift[0]));

         // Program register WCD9340_CDC_ANC0_LPF_FF_B_CTL+offset
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_LPF_FF_B_CTL+offset), (WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2 | WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1), (pANCCfg[ancCh].anc_ff_b_lpf_shift[1] << 4)|(pANCCfg[ancCh].anc_ff_b_lpf_shift[0]));

         // Program register WCD9340_CDC_ANC0_LPF_FB_CTL+offset
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_LPF_FB_CTL+offset), (WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2 | WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1), (pANCCfg[ancCh].anc_fb_lpf_shift[1] << 4)|(pANCCfg[ancCh].anc_fb_lpf_shift[0]));

         // Program register WCD9340_CDC_ANC0_SMLPF_CTL+offset
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_SMLPF_CTL+offset), WCD9340_CDC_ANC0_SMLPF_CTL___SHIFT, (pANCCfg[ancCh].smlpf_shift << WCD9340_CDC_ANC0_SMLPF_CTL___SHIFT_INDEX));

         // Program register WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL+offset
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL+offset),
               (WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT|WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT),
               (pANCCfg[ancCh].ff_b_dcflt_shift << WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT_INDEX) | (pANCCfg[ancCh].ff_a_dcflt_shift << WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT_INDEX));

         Setwcd9xxxANC_IIRCoeffs((uint32_t *)anc_reg_config_ptr->writes, &anc_index, (int8_t*)pANCCfg, ancCh,codec_version);

         // Program FF_A Adaptive ANC gain
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FF_A_GAIN_CTL+offset),
               (WCD9340_CDC_ANC0_FF_A_GAIN_CTL___GAIN),
               (pANCCfg[ancCh].anc_ff_a_gain & WCD9340_CDC_ANC0_FF_A_GAIN_CTL___GAIN));

         // Program FF_B Adaptive ANC gain
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FF_B_GAIN_CTL+offset),
               (WCD9340_CDC_ANC0_FF_B_GAIN_CTL___GAIN),
               (pANCCfg[ancCh].anc_ff_b_gain & WCD9340_CDC_ANC0_FF_B_GAIN_CTL___GAIN));

         // Program FB Adaptive ANC gain
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FB_GAIN_CTL+offset),
               (WCD9340_CDC_ANC0_FB_GAIN_CTL___GAIN),
               (pANCCfg[ancCh].anc_fb_gain & WCD9340_CDC_ANC0_FB_GAIN_CTL___GAIN));

         // remove the reset after programming
         temp_ctl_reg_val = 0;
         temp_ctl_reg_mask = 0;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_a_enable ? 0:WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_RESET);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_RESET;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_a_enable ? WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN: 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_b_enable ? 0: WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_RESET);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_RESET;
         temp_ctl_reg_val |= (pANCCfg[ancCh].ff_b_enable ? WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN: 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN;
         temp_ctl_reg_val |= (pANCCfg[ancCh].fb_enable ? 0 :WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_RESET);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_RESET;
         temp_ctl_reg_val |= (pANCCfg[ancCh].fb_enable ? WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN: 0);
         temp_ctl_reg_mask |= WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN;
         temp_ctl_reg_val |= ((pANCCfg[ancCh].ff_a_input_type) == SMTLNQ_ANC_MIC) ? WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_A_EN : 0; // UI input 1 == Enable SMTLnQ, 0 = Disable SMTLnQ
         temp_ctl_reg_val |= ((pANCCfg[ancCh].ff_b_input_type) == SMTLNQ_ANC_MIC) ? WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_B_EN : 0; // UI input 1 == Enable SMTLnQ, 0 = Disable SMTLnQ
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY(WCD9340_CDC_ANC0_CLK_RESET_CTL+offset, temp_ctl_reg_mask,  temp_ctl_reg_val);
      } // end of channels' for loop
   }
   else
   {
      return NOT_SUPPORTED;
   }
   anc_reg_config_ptr->size = anc_index;
   return res;
}


return_code_t convert_anc_hw5_reg_to_acdb(int8_t *anc_acdb_ptr, int32_t anc_acdb_size, struct anc_reg_t *anc_reg_config_ptr, codec_version_t codec_version)
{
   return_code_t res = SUCCESS;
   uint32_t index;
   uint32_t reg, mask, val;
   uint32_t anc_index = 0;
   uint32_t ancCh = 0;
   uint32_t iir_msb = 0;
   int32_t iir_index = 0;
   int32_t inc = 0;

   anc_wcd_hw5_acdb_cfg_t *pANCCfg = (anc_wcd_hw5_acdb_cfg_t *)anc_acdb_ptr;

   const uint32_t offset = WCD9340_CDC_ANC1_OFFSET;


   if ( TAVIL == codec_version)
   {
      // ensure that the input buffer is big enough
      if ((uint32_t)anc_acdb_size < sizeof(anc_wcd_hw5_acdb_cfg_t))
      {
         res = NOT_ENOUGH_MEMORY;
		 return res;
      }
      for(index = 0; index < anc_reg_config_ptr->size; index++)
      {
         REG_CODEC_UNPACK_ENTRY((anc_reg_config_ptr->writes[index]),reg,mask,val);

         if ((reg > WCD9340_CDC_ANC0_FB_GAIN_CTL) && (reg <= WCD9340_CDC_ANC0_FB_GAIN_CTL+offset))
         {
            if ((uint32_t)anc_acdb_size >= 2*sizeof(anc_wcd_hw5_acdb_cfg_t)) // check if the memory for the structure is enough for channel 1 as well
            {
               if (!ancCh)
               {
                  iir_index = 0;
                  iir_msb = 0;
                  inc = 0;
               }
               ancCh = 1;

            }
            else
            {
               continue;
            }
         }
         pANCCfg[ancCh].anc_ff_a_gain_scale = COEFF_PRECISION; // unity gain in 2^13 format
         pANCCfg[ancCh].anc_ff_a_gain_scale_default = COEFF_PRECISION; // unity gain in 2^13 format
         pANCCfg[ancCh].anc_ff_b_gain_scale = COEFF_PRECISION; // unity gain in 2^13 format
         pANCCfg[ancCh].anc_ff_b_gain_scale_default = COEFF_PRECISION; // unity gain in 2^13 format
         pANCCfg[ancCh].anc_fb_gain_scale = COEFF_PRECISION; // unity gain in 2^13 format
         pANCCfg[ancCh].anc_fb_gain_scale_default = COEFF_PRECISION; // unity gain in 2^13 format

         switch (reg)
         {
            case WCD9340_CDC_ANC0_CLK_RESET_CTL:
            case (WCD9340_CDC_ANC0_CLK_RESET_CTL+WCD9340_CDC_ANC1_OFFSET):
               {
                  pANCCfg[ancCh].ff_a_enable = (!(val & WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_RESET) && (val & WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN)) ? 1: 0;
                  pANCCfg[ancCh].ff_b_enable = (!(val & WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_RESET) && (val & WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN)) ? 1: 0;
                  pANCCfg[ancCh].fb_enable = (!(val & WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_RESET) && (val & WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN)) ? 1: 0;
                  pANCCfg[ancCh].ff_a_input_type = (val & WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_A_EN) ? SMTLNQ_ANC_MIC: pANCCfg[ancCh].ff_a_input_type;
                  pANCCfg[ancCh].ff_b_input_type = (val & WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_B_EN) ? SMTLNQ_ANC_MIC: pANCCfg[ancCh].ff_b_input_type;
                  break;
               }

            case WCD9340_CDC_ANC0_MODE_1_CTL:
            case (WCD9340_CDC_ANC0_MODE_1_CTL+WCD9340_CDC_ANC1_OFFSET):
               {
                  pANCCfg[ancCh].ff_out_enable= (val & WCD9340_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN) ? 1: 0;
                  if (!pANCCfg[ancCh].ff_a_input_type)
                  {
                     pANCCfg[ancCh].ff_a_input_type = (val & WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL) ? 1: 0;
                  }
                  if (!pANCCfg[ancCh].ff_b_input_type)
                  {
                     pANCCfg[ancCh].ff_b_input_type = (val & WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL) ? 1: 0;
                  }
                  pANCCfg[ancCh].ff_b_in_enable = (val & WCD9340_CDC_ANC0_MODE_1_CTL___FFB_IN_EN) ? 1: 0;
                  pANCCfg[ancCh].ff_a_in_enable = (val & WCD9340_CDC_ANC0_MODE_1_CTL___FFA_IN_EN) ? 1: 0;
                  pANCCfg[ancCh].smlpf_enable = (val & WCD9340_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN) ? 1: 0;
                  pANCCfg[ancCh].fb_mon = (val & WCD9340_CDC_ANC0_MODE_1_CTL___FB_ON_FBMON_IS_TRUE) ? 1 : 0;
                  break;
               }
            case WCD9340_CDC_ANC0_MODE_2_CTL:
            case WCD9340_CDC_ANC0_MODE_2_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].dmic_x2_ff_a_enable = (val & WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL) ? 1: 0;
                  pANCCfg[ancCh].dmic_x2_ff_b_enable = (val & WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL) ? 1: 0;
                  pANCCfg[ancCh].ff_a_dcflt_enable = (val & WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN) ? 1: 0;
                  pANCCfg[ancCh].ff_b_dcflt_enable = (val & WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN) ? 1: 0;
                  pANCCfg[ancCh].ff_flex_enable = (val & WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN) ? 1: 0;
                  pANCCfg[ancCh].ff_a_gain_enable = (val & WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN) ? 1: 0;
                  pANCCfg[ancCh].ff_b_gain_enable = (val & WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN) ? 1: 0;
                  pANCCfg[ancCh].fb_gain_enable = (val & WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN) ? 1: 0;
                  break;
               }
            case WCD9340_CDC_ANC0_FF_SHIFT:
            case WCD9340_CDC_ANC0_FF_SHIFT+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].anc_ff_a_shift = (val & WCD9340_CDC_ANC0_FF_SHIFT___FFA_SHIFT) >> WCD9340_CDC_ANC0_FF_SHIFT___FFA_SHIFT_INDEX;
                  pANCCfg[ancCh].anc_ff_b_shift = (val & WCD9340_CDC_ANC0_FF_SHIFT___FFB_SHIFT) >> WCD9340_CDC_ANC0_FF_SHIFT___FFB_SHIFT_INDEX;
                  break;
               }
            case WCD9340_CDC_ANC0_FB_SHIFT:
            case WCD9340_CDC_ANC0_FB_SHIFT+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].anc_fb_shift = (val & WCD9340_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT) >> WCD9340_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT_INDEX;
                  break;
               }
            case WCD9340_CDC_ANC0_LPF_FF_A_CTL:
            case WCD9340_CDC_ANC0_LPF_FF_A_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].anc_ff_a_lpf_shift[1] = (val & WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2) >> WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2_INDEX;
                  pANCCfg[ancCh].anc_ff_a_lpf_shift[0] = (val & WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1) >> WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1_INDEX;
                  break;
               }
            case WCD9340_CDC_ANC0_LPF_FF_B_CTL:
            case WCD9340_CDC_ANC0_LPF_FF_B_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].anc_ff_b_lpf_shift[1] = (val & WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2) >> WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2_INDEX;
                  pANCCfg[ancCh].anc_ff_b_lpf_shift[0] = (val & WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1) >> WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1_INDEX;
                  break;
               }
            case WCD9340_CDC_ANC0_LPF_FB_CTL:
            case WCD9340_CDC_ANC0_LPF_FB_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].anc_fb_lpf_shift[1] = (val & WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2) >> WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2_INDEX;
                  pANCCfg[ancCh].anc_fb_lpf_shift[0] = (val & WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1) >> WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1_INDEX;
                  break;
               }
            case WCD9340_CDC_ANC0_SMLPF_CTL:
            case WCD9340_CDC_ANC0_SMLPF_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].smlpf_shift = (val & WCD9340_CDC_ANC0_SMLPF_CTL___SHIFT) >> WCD9340_CDC_ANC0_SMLPF_CTL___SHIFT_INDEX;
                  break;
               }
            case WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL:
            case WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].ff_b_dcflt_shift = (val & WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT) >> WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT_INDEX;
                  pANCCfg[ancCh].ff_a_dcflt_shift = (val & WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT) >> WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT_INDEX;
                  break;
               }
            case WCD9340_CDC_ANC0_IIR_COEFF_1_CTL:
            case WCD9340_CDC_ANC0_IIR_COEFF_1_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  inc = val & WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC;
                  val = val & WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR;

                  if (0x00 != val)
                  {
                     // Given the HW bug, SW read sequence has only one address written in auto increment mode - zero
                     return INVALID_SEQ;
                  }
                  break;
               }
            case WCD9340_CDC_ANC0_IIR_COEFF_2_CTL:
            case WCD9340_CDC_ANC0_IIR_COEFF_2_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  val = val & WCD9340_CDC_ANC0_IIR_COEFF_2_CTL___COEF;

                  if (0==inc)
                  {
                     // error out, auto increment should be enabled given the HW bug,
                     // SW read sequence has only one address written in auto increment mode - zero
                     return INVALID_SEQ;
                  }


                  if (iir_index >= 0)
                  {
                     if (iir_msb)
                     {
                        if (iir_index < 7) // FF_A Denominator Index Limit
                        {
                           pANCCfg[ancCh].anc_ff_a_den_coeff[iir_index] |= ((val * 16) << REGISTER_DEPTH) ;
                           // sign extend in case of -ve number
                           if (pANCCfg[ancCh].anc_ff_a_den_coeff[iir_index] & 0x8000)
                           {
                              pANCCfg[ancCh].anc_ff_a_den_coeff[iir_index] |= 0xFFFF0000;
                           }
                        }
                        else if (iir_index < 15) // FF_A Numerator Index Limit
                        {
                           pANCCfg[ancCh].anc_ff_a_num_coeff[iir_index-7] |= ((val * 16) << REGISTER_DEPTH) ;
                           // sign extend in case of -ve number
                           if (pANCCfg[ancCh].anc_ff_a_num_coeff[iir_index-7] & 0x8000)
                           {
                              pANCCfg[ancCh].anc_ff_a_num_coeff[iir_index-7] |= 0xFFFF0000;
                           }
                        }
                        else if (iir_index < 22) // FF_B Denominator Index Limit
                        {
                           pANCCfg[ancCh].anc_ff_b_den_coeff[iir_index-15] |= ((val * 16) << REGISTER_DEPTH) ;
                           // sign extend in case of -ve number
                           if (pANCCfg[ancCh].anc_ff_b_den_coeff[iir_index-15] & 0x8000)
                           {
                              pANCCfg[ancCh].anc_ff_b_den_coeff[iir_index-15] |= 0xFFFF0000;
                           }
                        }
                        else if (iir_index < 30) // FF_B Numerator Index Limit
                        {
                           pANCCfg[ancCh].anc_ff_b_num_coeff[iir_index-22] |= ((val * 16) << REGISTER_DEPTH) ;
                           // sign extend in case of -ve number
                           if (pANCCfg[ancCh].anc_ff_b_num_coeff[iir_index-22] & 0x8000)
                           {
                              pANCCfg[ancCh].anc_ff_b_num_coeff[iir_index-22] |= 0xFFFF0000;
                           }
                        }
                        else if (iir_index < 37) // FB Denominator Index Limit
                        {
                           pANCCfg[ancCh].anc_fb_den_coeff[iir_index-30] |= ((val * 16) << REGISTER_DEPTH) ;
                           // sign extend in case of -ve number
                           if (pANCCfg[ancCh].anc_fb_den_coeff[iir_index-30] & 0x8000)
                           {
                              pANCCfg[ancCh].anc_fb_den_coeff[iir_index-30] |= 0xFFFF0000;
                           }
                        }
                        else if (iir_index < 45) // FB Numerator Index Limit
                        {
                           pANCCfg[ancCh].anc_fb_num_coeff[iir_index-37] |= ((val * 16) << REGISTER_DEPTH) ;
                           // sign extend in case of -ve number
                           if (pANCCfg[ancCh].anc_fb_num_coeff[iir_index-37] & 0x8000)
                           {
                              pANCCfg[ancCh].anc_fb_num_coeff[iir_index-37] |= 0xFFFF0000;
                           }
                        }
                        else
                        {
                           // error condition
                           res = FAIL;
                        }
                     }
                     else // if lsb term
                     {
                        if (iir_index < 7)
                        {
                           pANCCfg[ancCh].anc_ff_a_den_coeff[iir_index] |= (val * 16);
                        }
                        else if (iir_index < 15)
                        {
                           pANCCfg[ancCh].anc_ff_a_num_coeff[iir_index-7] |= (val * 16);
                        }
                        else if (iir_index < 22)
                        {
                           pANCCfg[ancCh].anc_ff_b_den_coeff[iir_index-15] |= (val * 16);
                        }
                        else if (iir_index < 30)
                        {
                           pANCCfg[ancCh].anc_ff_b_num_coeff[iir_index-22] |= (val * 16);
                        }
                        else if (iir_index < 37)
                        {
                           pANCCfg[ancCh].anc_fb_den_coeff[iir_index-30] |= (val * 16);
                        }
                        else if (iir_index < 45)
                        {
                           pANCCfg[ancCh].anc_fb_num_coeff[iir_index-37] |= (val * 16);
                        }
                        else
                        {
                           res = FAIL;
                           // error condition
                        }
                     }
                  }
                  if(iir_msb)
                  {
                     iir_index++;
                  }

                  if (iir_msb == 0)
                  {
                     iir_msb++;
                  }
                  else
                  {
                     iir_msb = 0;
                  }

                  break;
               }
            case WCD9340_CDC_ANC0_FF_A_GAIN_CTL:
            case WCD9340_CDC_ANC0_FF_A_GAIN_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].anc_ff_a_gain = (val & WCD9340_CDC_ANC0_FF_A_GAIN_CTL___GAIN);
                  break;
               }
            case WCD9340_CDC_ANC0_FF_B_GAIN_CTL:
            case WCD9340_CDC_ANC0_FF_B_GAIN_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].anc_ff_b_gain = (val & WCD9340_CDC_ANC0_FF_B_GAIN_CTL___GAIN);
                  break;
               }
            case WCD9340_CDC_ANC0_FB_GAIN_CTL:
            case WCD9340_CDC_ANC0_FB_GAIN_CTL+WCD9340_CDC_ANC1_OFFSET:
               {
                  pANCCfg[ancCh].anc_fb_gain = (val & WCD9340_CDC_ANC0_FB_GAIN_CTL___GAIN);
                  break;
               }
            default:
               {
                  break;
               }
         }// end of switch case
      }// end of for loop
   } // if (WCD9340 == codec_version)
   else
   {
      return NOT_SUPPORTED;
   }

   return res;
}

return_code_t get_anc_hw5_reg_read_sequence(struct anc_reg_t *anc_reg_config_ptr, uint32_t size __unused, codec_version_t codec_version)
{
   return_code_t res = SUCCESS;
   uint32_t offset = 0 ;
   uint32_t ancCh = 0;
   uint32_t anc_index = 0, i = 0;
   if (TAVIL == codec_version)
   {
      for (ancCh = 0; ancCh < 2; ancCh++)
      {
         offset = ancCh * WCD9340_NUM_ANC_CH0_REG;
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_CLK_RESET_CTL+offset), READ, 0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_MODE_1_CTL+offset), READ, 0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_MODE_2_CTL+offset), READ, 0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FF_SHIFT+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FB_SHIFT+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_LPF_FF_A_CTL+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_LPF_FF_B_CTL+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_LPF_FB_CTL+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_SMLPF_CTL+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL+offset),READ,0x0);
         // SW fix for HW bug fix, only auto increment mode works, hence setting the pointer right now to 0
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_IIR_COEFF_1_CTL+offset), WRITE, (0x0)); // write address 0x0 with manual mode
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_IIR_COEFF_1_CTL+offset), WRITE, (WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC|0x0)); // write address with Auto mode
         ///
         for (i = 0; i < 90; i++)
         {
            anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_IIR_COEFF_2_CTL+offset), READ, 0x0);
         }
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FF_A_GAIN_CTL+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FF_B_GAIN_CTL+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FB_GAIN_CTL+offset),READ,0x0);
         anc_reg_config_ptr->writes[anc_index++] = REG_CODEC_PACK_ENTRY((WCD9340_CDC_ANC0_FB_GAIN_CTL+offset),READ,0x0); // extra to make even number of total reads
      } // end of channels for loop
   }
   else
   {
      return NOT_SUPPORTED;
   }
   anc_reg_config_ptr->size = anc_index;
   return res;
}

return_code_t convert_anc_acdb_to_reg(struct anc_reg_t *anc_reg_config_ptr,
       int8_t *anc_acdb_ptr, uint32_t input_size, codec_version_t codec_version)
{
        return_code_t res = SUCCESS;
        //uint32_t index;
        //uint32_t reg, mask, val;
        uint32_t temp_ctl_reg_val;
        uint32_t temp_ctl_reg_mask;
        uint32_t anc_index = 0;
        uint32_t offset;
        uint32_t ancCh;
        //boolean  ancDMICselect;

        if ((NULL == anc_reg_config_ptr) || (NULL == anc_acdb_ptr))
        {
             return INVALID_PTR;
        }

        memset(anc_reg_config_ptr,0,sizeof(anc_reg_t));
        switch (codec_version) {
        case TAVIL:
              res = convert_anc_acdb_to_hw5_reg(anc_reg_config_ptr, anc_acdb_ptr, input_size, codec_version);
              break;
        case TASHA:
        {
             anc_tasha_acdb_cfg_t *pANCCfg =
                      (anc_tasha_acdb_cfg_t *)anc_acdb_ptr;
             for(ancCh = 0; ancCh < input_size; ancCh++) {
                 offset = ancCh * WCD9335_NUM_ANC_CH0_REG;

                 // apply reset before beggining to program
                 temp_ctl_reg_val = 0;
                 temp_ctl_reg_mask = 0;
                 temp_ctl_reg_val |= WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_RESET;
                 temp_ctl_reg_mask |= WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_RESET;
                 temp_ctl_reg_val |= !WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN;
                 temp_ctl_reg_mask |= !WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN;
                 temp_ctl_reg_val |= WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_RESET;
                 temp_ctl_reg_mask |= WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_RESET;
                 temp_ctl_reg_val |= !WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN;
                 temp_ctl_reg_mask |= !WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN;
                 temp_ctl_reg_val |= WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_RESET;
                 temp_ctl_reg_mask |= WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_RESET;
                 temp_ctl_reg_val |= !WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN;
                 temp_ctl_reg_mask |= !WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN;
                 anc_reg_config_ptr->writes[anc_index++] =
                             REG_CODEC_PACK_ENTRY(WCD9335_CDC_ANC0_CLK_RESET_CTL+offset,
                                            temp_ctl_reg_mask, temp_ctl_reg_val);

                 /* Program register WCD9335_CDC_ANC0_MODE_1_CTL+offset */
                 temp_ctl_reg_val = 0;
                 temp_ctl_reg_mask = 0;
                 temp_ctl_reg_val |=
                             (pANCCfg[ancCh].ff_a_in_enable ?
                              WCD9335_CDC_ANC0_MODE_1_CTL___FFA_IN_EN : 0);
                 temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_1_CTL___FFA_IN_EN;
                 temp_ctl_reg_val |= (pANCCfg[ancCh].ff_b_in_enable ?
                              WCD9335_CDC_ANC0_MODE_1_CTL___FFB_IN_EN : 0);
                 temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_1_CTL___FFB_IN_EN;
                 temp_ctl_reg_val |= (pANCCfg[ancCh].ff_out_enable ?
                              WCD9335_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN : 0);
                 temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN;
                 temp_ctl_reg_val |= (pANCCfg[ancCh].smlpf_enable ?
                              WCD9335_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN : 0);
                 temp_ctl_reg_mask |=
                              WCD9335_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN;
                 temp_ctl_reg_val |=
                      (pANCCfg[ancCh].ff_a_input_device >= ADIE_MAP_DMIC0) ?
                        WCD9335_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL : 0;
                 /* UI input 1 == Digital, 0 = Analog */
                 temp_ctl_reg_mask |=
                        WCD9335_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL;
                 temp_ctl_reg_val |=
                      (pANCCfg[ancCh].ff_b_input_device >= ADIE_MAP_DMIC0) ?
                        WCD9335_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL : 0;
                 temp_ctl_reg_mask |=
                         WCD9335_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL;
                 /*
                  * TODO: define controls for
                  *  WCD9335_CDC_ANC0_MODE_1_CTL___FB_ON_FBMON_IS_TRUE &
                  * WCD9335_CDC_ANC0_MODE_1_CTL___ANC_OUTMIX_EN
                  */
                 anc_reg_config_ptr->writes[anc_index++] =
                    REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_MODE_1_CTL + offset),
                    temp_ctl_reg_mask, temp_ctl_reg_val);

                /* Program register WCD9335_CDC_ANC0_MODE_2_CTL+offset */
                temp_ctl_reg_val = 0;
                temp_ctl_reg_mask = 0;
                temp_ctl_reg_val |= (pANCCfg[ancCh].ff_flex_enable ?
                     WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN : 0);
                temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN;
		temp_ctl_reg_val |= (pANCCfg[ancCh].ff_a_gain_enable ?
					WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN : 0);
		temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN;
		temp_ctl_reg_val |= (pANCCfg[ancCh].ff_b_gain_enable ?
					WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN : 0);
		temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN;
		temp_ctl_reg_val |= (pANCCfg[ancCh].ff_a_dcflt_enable ?
                     WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN : 0);
                temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN;
                temp_ctl_reg_val |= (pANCCfg[ancCh].ff_b_dcflt_enable ?
                     WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN : 0);
                temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN;
                temp_ctl_reg_val |= (pANCCfg[ancCh].dmic_x2_ff_a_enable ?
                     WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL : 0);
                temp_ctl_reg_mask |=
                     WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL;
                temp_ctl_reg_val |= (pANCCfg[ancCh].dmic_x2_ff_b_enable ?
                     WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL : 0);

		temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL;
		temp_ctl_reg_val |= (pANCCfg[ancCh].fb_gain_enable ?
					 WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN : 0);
		temp_ctl_reg_mask |= WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN;
                /*
                 * Note: Bits controlled by DSP FW are
                 * WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN &
                 * WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN &
                 * WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN
                 */
                anc_reg_config_ptr->writes[anc_index++] =
                 REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_MODE_2_CTL + offset),
                    temp_ctl_reg_mask, temp_ctl_reg_val);

                /* Program register WCD9335_CDC_ANC0_FF_SHIFT+offset */
                anc_reg_config_ptr->writes[anc_index++] =
                 REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FF_SHIFT + offset),
                    (WCD9335_CDC_ANC0_FF_SHIFT___FFA_SHIFT |
                     WCD9335_CDC_ANC0_FF_SHIFT___FFB_SHIFT),
                    ((pANCCfg[ancCh].anc_ff_a_shift << 4) |
                     pANCCfg[ancCh].anc_ff_b_shift ));

                /* Program register WCD9335_CDC_ANC0_FB_SHIFT+offset */
                anc_reg_config_ptr->writes[anc_index++] =
                 REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FB_SHIFT + offset),
                    WCD9335_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT,
                    (pANCCfg[ancCh].anc_fb_shift));

                /* Program register WCD9335_CDC_ANC0_LPF_FF_A_CTL+offset */
                anc_reg_config_ptr->writes[anc_index++] =
                 REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_LPF_FF_A_CTL + offset),
                    (WCD9335_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2 |
                     WCD9335_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1),
                     ((pANCCfg[ancCh].anc_ff_a_lpf_shift[1] << 4) |
                     (pANCCfg[ancCh].anc_ff_a_lpf_shift[0])));

                /* Program register WCD9335_CDC_ANC0_LPF_FF_B_CTL+offset */
                anc_reg_config_ptr->writes[anc_index++] =
                 REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_LPF_FF_B_CTL + offset),
                    (WCD9335_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2 |
                            WCD9335_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1),
                    ((pANCCfg[ancCh].anc_ff_b_lpf_shift[1] << 4) |
                            (pANCCfg[ancCh].anc_ff_b_lpf_shift[0])));

                /* Program register WCD9335_CDC_ANC0_LPF_FB_CTL+offset */
                anc_reg_config_ptr->writes[anc_index++] =
                 REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_LPF_FB_CTL + offset),
                    (WCD9335_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2 |
                             WCD9335_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1),
                    ((pANCCfg[ancCh].anc_fb_lpf_shift[1] << 4) |
                            (pANCCfg[ancCh].anc_fb_lpf_shift[0])));

                /* Program register WCD9335_CDC_ANC0_SMLPF_CTL+offset */
                anc_reg_config_ptr->writes[anc_index++] =
                REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_SMLPF_CTL + offset),
                    WCD9335_CDC_ANC0_SMLPF_CTL___SHIFT,
                    (pANCCfg[ancCh].smlpf_shift <<
                            WCD9335_CDC_ANC0_SMLPF_CTL___SHIFT_INDEX));

                /* Program register WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL+offset */
                anc_reg_config_ptr->writes[anc_index++] =
                 REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL + offset),
                 (WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT |
                        WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT),
                 ((pANCCfg[ancCh].ff_b_dcflt_shift <<
                        WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT_INDEX) |
                 (pANCCfg[ancCh].ff_a_dcflt_shift <<
                        WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT_INDEX)));

                Setwcd9xxxANC_IIRCoeffs((uint32_t *)anc_reg_config_ptr->writes,
                         &anc_index, (int8_t*)pANCCfg, ancCh, codec_version);
		// Program FF_A Adaptive ANC gain
		anc_reg_config_ptr->writes[anc_index++] =
				REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FF_A_GAIN_CTL+offset),
				(WCD9335_CDC_ANC0_FF_A_GAIN_CTL___GAIN),
				(pANCCfg[ancCh].anc_ff_a_gain & WCD9335_CDC_ANC0_FF_A_GAIN_CTL___GAIN));

		// Program FF_B Adaptive ANC gain
		anc_reg_config_ptr->writes[anc_index++] =
				REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FF_B_GAIN_CTL+offset),
				(WCD9335_CDC_ANC0_FF_B_GAIN_CTL___GAIN),
				(pANCCfg[ancCh].anc_ff_b_gain & WCD9335_CDC_ANC0_FF_B_GAIN_CTL___GAIN));

		// Program FB Adaptive ANC gain
		anc_reg_config_ptr->writes[anc_index++] =
				REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FB_GAIN_CTL+offset),
				(WCD9335_CDC_ANC0_FB_GAIN_CTL___GAIN),
				(pANCCfg[ancCh].anc_fb_gain & WCD9335_CDC_ANC0_FB_GAIN_CTL___GAIN));

                // remove the reset after programming
                temp_ctl_reg_val = 0;
                temp_ctl_reg_mask = 0;
                temp_ctl_reg_val |=
                     (pANCCfg[ancCh].ff_a_enable ? 0:WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_RESET);
                temp_ctl_reg_mask |=
                     WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_RESET;
                temp_ctl_reg_val |=
                     (pANCCfg[ancCh].ff_a_enable ? WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN: 0);
                temp_ctl_reg_mask |=
                     WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN;
                temp_ctl_reg_val |=
                     (pANCCfg[ancCh].ff_b_enable ? 0: WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_RESET);
                temp_ctl_reg_mask |=
                     WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_RESET;
                temp_ctl_reg_val |=
                     (pANCCfg[ancCh].ff_b_enable ? WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN: 0);
                temp_ctl_reg_mask |=
                     WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN;
                temp_ctl_reg_val |=
                     (pANCCfg[ancCh].fb_enable ? 0 :WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_RESET);
                temp_ctl_reg_mask |=
                     WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_RESET;
                temp_ctl_reg_val |=
                     (pANCCfg[ancCh].fb_enable ? WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN: 0);
                temp_ctl_reg_mask |=
                     WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN;
                anc_reg_config_ptr->writes[anc_index++] =
                          REG_CODEC_PACK_ENTRY(WCD9335_CDC_ANC0_CLK_RESET_CTL + offset,
                              temp_ctl_reg_mask,  temp_ctl_reg_val);
         }
         anc_reg_config_ptr->size = anc_index;
         break;
      }
   default:
   case TAIKO:
      {
            anc_taiko_acdb_cfg_t *pANCCfg = (anc_taiko_acdb_cfg_t *)anc_acdb_ptr;
            int8_t enable_val = 0;

            anc_reg_config_ptr->writes[anc_index++] =
                    TAIKO_CODEC_PACK_ENTRY(TAIKO_A_CDC_CLK_ANC_RESET_CTL, 0xff, 0xff);
            anc_reg_config_ptr->writes[anc_index++] =
                    TAIKO_CODEC_PACK_ENTRY(TAIKO_A_CDC_CLK_ANC_CLK_EN_CTL, 0xff, 0x05);
            for(ancCh = 0; ancCh < input_size; ancCh++) {
                LOGD("%s:enter, ancch  %d, input device   %d", __func__,
                                ancCh, pANCCfg[ancCh].input_device);
                if (!pANCCfg[ancCh].input_device) {
                         continue;
                }
                offset = ancCh * 128;

                temp_ctl_reg_val = 0;
                if (pANCCfg[ancCh].ff_out_enable)
                        temp_ctl_reg_val |= 0x1;
                if ((pANCCfg[ancCh].input_device & 0xF) >= ADIE_CODEC_DMIC1)
                        temp_ctl_reg_val |= 0x2;
                if (pANCCfg[ancCh].anc_lr_mix_enable)
                        temp_ctl_reg_val |= 0x4;
                if (pANCCfg[ancCh].hybrid_enable)
                        temp_ctl_reg_val |= 0x8;
                if (pANCCfg[ancCh].ff_in_enable)
                        temp_ctl_reg_val |= 0x10;
                if (pANCCfg[ancCh].dcflt_enable)
                        temp_ctl_reg_val |= 0x20;
                if (pANCCfg[ancCh].smlpf_enable)
                        temp_ctl_reg_val |= 0x40;
                if (pANCCfg[ancCh].adaptive_gain_enable)
                        temp_ctl_reg_val |= 0x80;

                anc_reg_config_ptr->writes[anc_index++] =
                    REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_B1_CTL + offset),
                    0xFF, temp_ctl_reg_val);
                anc_reg_config_ptr->writes[anc_index++] =
                    REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_SHIFT + offset),
                    0xFF, ((pANCCfg[ancCh].anc_ff_shift << 4) |
                            pANCCfg[ancCh].anc_fb_shift));
                /* IIR COEFFS */
                Setwcd9xxxANC_IIRCoeffs((uint32_t *)anc_reg_config_ptr->writes,
                              &anc_index, (int8_t *)pANCCfg, ancCh, codec_version);

               /*
                * Decprecated/Reserved for future use, this register is
                * controlled completely by AFE code in DSP
                */
               /* Sequence follwed by AFE is as follows
                * 0x202 --> 0x4 (0x0100)
                * 0x20C --> Gain value
                * 0x202 --> 0xC (0x1100)
                * anc_reg_config_ptr->writes[anc_index++] =
                * TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B1_CTL +
                * offset), 0x08, pANCCfg[ancCh].adaptive ? 0x08 : 0);
                */

                /* LPF COEFFS */
                Setwcd9xxxANC_LPFShift((uint32_t *)anc_reg_config_ptr->writes,
                                       &anc_index, pANCCfg, ancCh);

                /* ANC SMLPF CTL */
                anc_reg_config_ptr->writes[anc_index++] =
                    REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_SMLPF_CTL + offset),
                                            0xFF, pANCCfg[ancCh].smlpf_shift);
                /* ANC DCFLT CTL */
                anc_reg_config_ptr->writes[anc_index++] =
                    REG_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_DCFLT_CTL + offset),
                                            0xFF, pANCCfg[ancCh].dcflt_shift);

                anc_reg_config_ptr->writes[anc_index++] =
                    TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_GAIN_CTL + offset),
                                            0xFF, pANCCfg[ancCh].adaptive_gain);

                enable_val |= (pANCCfg[ancCh].default_enable |
                              (1 << pANCCfg[ancCh].anc_feedback_enable)) << ( ancCh * 2);

         } // End of For loop for channels
         anc_reg_config_ptr->writes[anc_index++] =
                        TAIKO_CODEC_PACK_ENTRY(TAIKO_A_CDC_CLK_ANC_CLK_EN_CTL, 0xF, enable_val);
         anc_reg_config_ptr->writes[anc_index++] =
                        TAIKO_CODEC_PACK_ENTRY(TAIKO_A_CDC_CLK_ANC_RESET_CTL, 0xF, ~(enable_val));
         anc_reg_config_ptr->size = anc_index;
         break;
      } // End of TAIKO/default case statement
   } // End of Switch case for codec

    /*
     * write (file_descriptor, &anc_reg_config_ptr->size,
     * sizeof(anc_reg_config_ptr->size));
     * write (file_descriptor, anc_reg_config_ptr->writes,
     * anc_reg_config_ptr->size * CODEC_PACKED_REG_SIZE);
     */
    LOGD("%s:leave", __func__);
    return res;
}

return_code_t convert_anc_reg_to_acdb(int8_t *anc_acdb_ptr,
      uint32_t anc_acdb_size, struct anc_reg_t *anc_reg_config_ptr,
      codec_version_t codec_version)
{
   return_code_t res = SUCCESS;
   uint32_t index;
   uint32_t reg, mask, val;
//   uint32_t temp_ctl_reg_val;
   uint32_t anc_index = 0;
   uint32_t offset = 0;
   uint32_t ancCh = 0;
   uint32_t iir_msb = 0;
   int32_t iir_index = 0;

   if ((NULL == anc_reg_config_ptr) || (NULL == anc_acdb_ptr))
   {
      return INVALID_PTR;
   }

   memset(anc_acdb_ptr,0,anc_acdb_size);

   switch (codec_version) {
   case TAVIL:
         res = convert_anc_hw5_reg_to_acdb(anc_acdb_ptr, anc_acdb_size, anc_reg_config_ptr, codec_version);
         break;
   case TASHA:
   {
         anc_tasha_acdb_cfg_t *pANCCfg = (anc_tasha_acdb_cfg_t *)anc_acdb_ptr;
         uint32_t ancCh = 0;
         offset = WCD9335_CDC_ANC1_OFFSET;

         /* ensure that the input buffer is big enough */
         if (anc_acdb_size < sizeof(anc_tasha_acdb_cfg_t)) {
            LOGE("%s: input buffer %d is not big enough for TASHA", __func__, anc_acdb_size);
            res = NOT_ENOUGH_MEMORY;
            break;
         }

         for(index = 0; index < anc_reg_config_ptr->size; index++) {
            REG_CODEC_UNPACK_ENTRY(
                (anc_reg_config_ptr->writes[index]),reg,mask,val);

            if ((reg > WCD9335_CDC_ANC0_FB_GAIN_CTL) &&
                (reg <= WCD9335_CDC_ANC0_FB_GAIN_CTL + offset)) {
               if (anc_acdb_size >= 2 * sizeof(anc_tasha_acdb_cfg_t)) {
                /*
                 * check if the memory for the structure is enough
                 * for channel 1 as well
                 */
                  ancCh = 1;
               } else {
                  continue;
               }
            }
            pANCCfg[ancCh].anc_ff_a_gain_scale = COEFF_PRECISION; // unity gain in 2^13 format
            pANCCfg[ancCh].anc_ff_a_gain_scale_default = COEFF_PRECISION; // unity gain in 2^13 format
            pANCCfg[ancCh].anc_ff_b_gain_scale = COEFF_PRECISION; // unity gain in 2^13 format
            pANCCfg[ancCh].anc_ff_b_gain_scale_default = COEFF_PRECISION; // unity gain in 2^13 format
            pANCCfg[ancCh].anc_fb_gain_scale = COEFF_PRECISION; // unity gain in 2^13 format
            pANCCfg[ancCh].anc_fb_gain_scale_default = COEFF_PRECISION; // unity gain in 2^13 format

            switch (reg) {
                case WCD9335_CDC_ANC0_CLK_RESET_CTL:
                case (WCD9335_CDC_ANC0_CLK_RESET_CTL + WCD9335_CDC_ANC1_OFFSET):
                      pANCCfg[ancCh].ff_a_enable =
                                (!(val & WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_RESET) &&
                                (val & WCD9335_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN)) ? 1: 0;
                      pANCCfg[ancCh].ff_b_enable =
                                (!(val & WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_RESET) &&
                                (val & WCD9335_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN)) ? 1: 0;
                      pANCCfg[ancCh].fb_enable =
                                (!(val & WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_RESET) &&
                                (val & WCD9335_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN)) ? 1: 0;
                      break;
               case WCD9335_CDC_ANC0_MODE_1_CTL:
               case (WCD9335_CDC_ANC0_MODE_1_CTL + WCD9335_CDC_ANC1_OFFSET):
                     pANCCfg[ancCh].ff_out_enable =
                       (val & WCD9335_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN) ? 1: 0;
                     pANCCfg[ancCh].ff_a_input_device =
                       (val & WCD9335_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL) ? 1: 0;
                     pANCCfg[ancCh].ff_b_input_device =
                       (val & WCD9335_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL) ? 1: 0;
                     pANCCfg[ancCh].ff_b_in_enable =
                       (val & WCD9335_CDC_ANC0_MODE_1_CTL___FFB_IN_EN) ? 1: 0;
                     pANCCfg[ancCh].ff_a_in_enable =
                       (val & WCD9335_CDC_ANC0_MODE_1_CTL___FFA_IN_EN) ? 1: 0;
                     pANCCfg[ancCh].smlpf_enable =
                       (val & WCD9335_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN) ? 1: 0;
                     break;
               case WCD9335_CDC_ANC0_MODE_2_CTL:
               case WCD9335_CDC_ANC0_MODE_2_CTL + WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].dmic_x2_ff_a_enable =
                       (val & WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL) ? 1: 0;
                     pANCCfg[ancCh].dmic_x2_ff_b_enable =
                       (val & WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL) ? 1: 0;
                     pANCCfg[ancCh].ff_a_dcflt_enable =
                       (val & WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN) ? 1: 0;
                     pANCCfg[ancCh].ff_b_dcflt_enable =
                       (val & WCD9335_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN) ? 1: 0;
                     pANCCfg[ancCh].ff_flex_enable =
                       (val & WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN) ? 1 : 0;
			pANCCfg[ancCh].ff_a_gain_enable =
					(val & WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN) ? 1: 0;
                     pANCCfg[ancCh].ff_b_gain_enable =
					(val & WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN) ? 1: 0;
                     pANCCfg[ancCh].fb_gain_enable =
					(val & WCD9335_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN) ? 1: 0;
                     break;
               case WCD9335_CDC_ANC0_FF_SHIFT:
               case WCD9335_CDC_ANC0_FF_SHIFT + WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].anc_ff_a_shift =
                       (val & WCD9335_CDC_ANC0_FF_SHIFT___FFA_SHIFT) >>
                              WCD9335_CDC_ANC0_FF_SHIFT___FFA_SHIFT_INDEX;
                     pANCCfg[ancCh].anc_ff_b_shift =
                       (val & WCD9335_CDC_ANC0_FF_SHIFT___FFB_SHIFT) >>
                              WCD9335_CDC_ANC0_FF_SHIFT___FFB_SHIFT_INDEX;
                     break;
               case WCD9335_CDC_ANC0_FB_SHIFT:
               case WCD9335_CDC_ANC0_FB_SHIFT + WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].anc_fb_shift =
                      (val & WCD9335_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT) >>
                             WCD9335_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT_INDEX;
                     break;
               case WCD9335_CDC_ANC0_LPF_FF_A_CTL:
               case WCD9335_CDC_ANC0_LPF_FF_A_CTL + WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].anc_ff_a_lpf_shift[1] =
                       (val & WCD9335_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2) >>
                        WCD9335_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2_INDEX;
                     pANCCfg[ancCh].anc_ff_a_lpf_shift[0] =
                       (val & WCD9335_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1) >>
                        WCD9335_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1_INDEX;
                     break;
               case WCD9335_CDC_ANC0_LPF_FF_B_CTL:
               case WCD9335_CDC_ANC0_LPF_FF_B_CTL + WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].anc_ff_b_lpf_shift[0] =
                       (val & WCD9335_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2) >>
                              WCD9335_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2_INDEX;
                     pANCCfg[ancCh].anc_ff_b_lpf_shift[1] =
                       (val & WCD9335_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1) >>
                              WCD9335_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1_INDEX;
                     break;
               case WCD9335_CDC_ANC0_LPF_FB_CTL:
               case WCD9335_CDC_ANC0_LPF_FB_CTL + WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].anc_fb_lpf_shift[0] =
                       (val & WCD9335_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2) >>
                              WCD9335_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2_INDEX;
                     pANCCfg[ancCh].anc_fb_lpf_shift[1] =
                       (val & WCD9335_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1) >>
                              WCD9335_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1_INDEX;
                     break;
               case WCD9335_CDC_ANC0_SMLPF_CTL:
               case WCD9335_CDC_ANC0_SMLPF_CTL + WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].smlpf_shift =
                       (val & WCD9335_CDC_ANC0_SMLPF_CTL___SHIFT) >>
                              WCD9335_CDC_ANC0_SMLPF_CTL___SHIFT_INDEX;
                     break;
               case WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL:
               case WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL + WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].ff_b_dcflt_shift =
                       (val & WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT) >>
                              WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT_INDEX;
                     pANCCfg[ancCh].ff_a_dcflt_shift =
                       (val & WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT) >>
                              WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT_INDEX;
                     break;
               case WCD9335_CDC_ANC0_IIR_COEFF_1_CTL:
               case WCD9335_CDC_ANC0_IIR_COEFF_1_CTL + WCD9335_CDC_ANC1_OFFSET:
                     if (val < 90) {
                         /*
                          * 90 is the total number of register addresses in
                          * Tasha for iir coefficients
                          */
                        val = val & WCD9335_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR;
                        iir_index = val / 2;
                        iir_msb = 0;
                        if (val % 2) {
                           iir_index = (val - 1) / 2;
                           iir_msb = 1;
                        }
                     } else {
                        iir_index = -1;
                        iir_msb = 0;
                     }
                     break;
               case WCD9335_CDC_ANC0_IIR_COEFF_2_CTL:
               case WCD9335_CDC_ANC0_IIR_COEFF_2_CTL+WCD9335_CDC_ANC1_OFFSET:
                     val = val & WCD9335_CDC_ANC0_IIR_COEFF_2_CTL___COEF;

                     if (iir_index >= 0) {
                        if (iir_msb) {
                           if (iir_index < 7) {
                              pANCCfg[ancCh].anc_ff_a_den_coeff[iir_index]
                                     |= ((val * 16) << REGISTER_DEPTH) ;
                              /* sign extend in case of -ve number */
                              if (pANCCfg[ancCh].anc_ff_a_den_coeff[iir_index] & 0x8000) {
                                  pANCCfg[ancCh].anc_ff_a_den_coeff[iir_index] |= 0xFFFF0000;
                              }
                           } else if (iir_index < 15) {
                            pANCCfg[ancCh].anc_ff_a_num_coeff[iir_index - 7] |=
                                     ((val * 16) << REGISTER_DEPTH) ;
                              /* sign extend in case of -ve number */
                            if (pANCCfg[ancCh].anc_ff_a_num_coeff[iir_index - 7] & 0x8000) {
                                 pANCCfg[ancCh].anc_ff_a_num_coeff[iir_index - 7] |= 0xFFFF0000;
                              }
                           } else if (iir_index < 22) {
                              pANCCfg[ancCh].anc_ff_b_den_coeff[iir_index - 15] |=
                                     ((val * 16) << REGISTER_DEPTH) ;
                              /* sign extend in case of -ve number */
                              if (pANCCfg[ancCh].anc_ff_b_den_coeff[iir_index - 15] & 0x8000) {
                                 pANCCfg[ancCh].anc_ff_b_den_coeff[iir_index - 15] |= 0xFFFF0000;
                              }
                           } else if (iir_index < 30) {
                              pANCCfg[ancCh].anc_ff_b_num_coeff[iir_index - 22] |=
                                     ((val * 16) << REGISTER_DEPTH) ;
                              /* sign extend in case of -ve number */
                              if (pANCCfg[ancCh].anc_ff_b_num_coeff[iir_index - 22] & 0x8000) {
                                 pANCCfg[ancCh].anc_ff_b_num_coeff[iir_index - 22] |= 0xFFFF0000;
                              }
                           } else if (iir_index < 37) {
                              pANCCfg[ancCh].anc_fb_den_coeff[iir_index - 30] |=
                                     ((val * 16) << REGISTER_DEPTH) ;
                              /* sign extend in case of -ve number */
                              if (pANCCfg[ancCh].anc_fb_den_coeff[iir_index - 30] & 0x8000) {
                                  pANCCfg[ancCh].anc_fb_den_coeff[iir_index - 30] |= 0xFFFF0000;
                              }
                           } else if (iir_index < 45) {
                              pANCCfg[ancCh].anc_fb_num_coeff[iir_index - 37] |=
                                    ((val * 16) << REGISTER_DEPTH) ;
                              /* sign extend in case of -ve number */
                              if (pANCCfg[ancCh].anc_fb_num_coeff[iir_index - 37] & 0x8000) {
                                 pANCCfg[ancCh].anc_fb_num_coeff[iir_index - 37] |= 0xFFFF0000;
                              }
                           } else {
                              res = FAIL;
                           }
                        } else {
                           if (iir_index < 7) {
                              pANCCfg[ancCh].anc_ff_a_den_coeff[iir_index] |= (val * 16);
                           } else if (iir_index < 15) {
                              pANCCfg[ancCh].anc_ff_a_num_coeff[iir_index - 7] |= (val * 16);
                           } else if (iir_index < 22) {
                              pANCCfg[ancCh].anc_ff_b_den_coeff[iir_index - 15] |= (val * 16);
                           } else if (iir_index < 30) {
                              pANCCfg[ancCh].anc_ff_b_num_coeff[iir_index - 22] |= (val * 16);
                           } else if (iir_index < 37) {
                              pANCCfg[ancCh].anc_fb_den_coeff[iir_index - 30] |= (val * 16);
                           } else if (iir_index < 45) {
                              pANCCfg[ancCh].anc_fb_num_coeff[iir_index - 37] |= (val * 16);
                           } else {
                              res = FAIL;
                           }
                        }
                     }
                     break;
				               case WCD9335_CDC_ANC0_FF_A_GAIN_CTL:
               case WCD9335_CDC_ANC0_FF_A_GAIN_CTL+WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].anc_ff_a_gain =
						(val & WCD9335_CDC_ANC0_FF_A_GAIN_CTL___GAIN);
                     break;
               case WCD9335_CDC_ANC0_FF_B_GAIN_CTL:
               case WCD9335_CDC_ANC0_FF_B_GAIN_CTL+WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].anc_ff_b_gain =
						(val & WCD9335_CDC_ANC0_FF_B_GAIN_CTL___GAIN);
                     break;
               case WCD9335_CDC_ANC0_FB_GAIN_CTL:
               case WCD9335_CDC_ANC0_FB_GAIN_CTL+WCD9335_CDC_ANC1_OFFSET:
                     pANCCfg[ancCh].anc_fb_gain =
						(val & WCD9335_CDC_ANC0_FB_GAIN_CTL___GAIN);
                     break;
               default:
                     break;
            }
         }
         break;
   }
   default:
   case(TAIKO):
      {
         anc_taiko_acdb_cfg_t *pANCCfg = (anc_taiko_acdb_cfg_t *)anc_acdb_ptr;
         offset = 0;
         uint32_t reset_val  = 0;
         uint32_t clk_val  = 0;

         memset(anc_acdb_ptr,0,anc_acdb_size);
         for(index = 0; index < anc_reg_config_ptr->size; index++) {
             TAIKO_CODEC_UNPACK_ENTRY(
                  (anc_reg_config_ptr->writes[index]),reg,mask,val);

             ancCh = 0;
             /* ensure that the input buffer is big enough */
             if (anc_acdb_size < 2 * sizeof(anc_taiko_acdb_cfg_t)) {
                 LOGE("%s: input buffer %d is not big enough for TAIKO", __func__, anc_acdb_size);
                 res = NOT_ENOUGH_MEMORY;
                 break;
             }

             if ((reg >= TAIKO_A_CDC_ANC2_B1_CTL) &&
                 (reg < TAIKO_A_CDC_CLK_ANC_RESET_CTL)) {
                 if (anc_acdb_size > sizeof(anc_taiko_acdb_cfg_t)) {
                     ancCh = 1;
                     pANCCfg[1].anc_gain = COEFF_PRECISION; /* unity gain in 2^13 format */
                 } else {
                     ancCh = 0;
                     continue;
                 }
             }
             pANCCfg[ancCh].anc_gain = COEFF_PRECISION; /* unity gain in 2^13 format */
             pANCCfg[ancCh].anc_gain_default = COEFF_PRECISION; // unity gain in 2^13 format
             pANCCfg[ancCh].adaptive_gain = UNITY_GAIN; // unity gain in Q8
             pANCCfg[ancCh].flex_enable = 0;
             pANCCfg[ancCh].anc_lr_mix_output_channel = 0;

             switch (reg) {
             case TAIKO_A_CDC_CLK_ANC_CLK_EN_CTL:
                  clk_val = val & 0xf;
                  break;
             case TAIKO_A_CDC_CLK_ANC_RESET_CTL:
                  reset_val = (~val) & 0xf;
                  break;
             case TAIKO_A_CDC_ANC1_B1_CTL:
             case TAIKO_A_CDC_ANC2_B1_CTL:
                  pANCCfg[ancCh].ff_out_enable = (val & FF_OUT_EN) ? 1 : 0;
                  // make this based on the noise mic section
                  //pANCCfg[ancCh].input_device = (val & ADC_DMIC_SEL) ? 1 : 0;
                  pANCCfg[ancCh].anc_lr_mix_enable = (val & LR_EN) ? 1 : 0;
                  pANCCfg[ancCh].hybrid_enable = (val & HYBRID_EN) ? 1 : 0;
                  pANCCfg[ancCh].ff_in_enable = (val & FF_IN_EN) ? 1 : 0;
                  pANCCfg[ancCh].dcflt_enable = (val & DCFLT_EN) ? 1 : 0;
                  pANCCfg[ancCh].smlpf_enable = (val & SMLPF_EN) ? 1 : 0;
                  pANCCfg[ancCh].adaptive_gain_enable = (val & FFGAIN_EN) ? 1 : 0;
                  break;
             case TAIKO_A_CDC_ANC1_B2_CTL:
                  pANCCfg[0].dmic_x2_enable = (val & DMIC_X2_SEL);
                  break;
             case TAIKO_A_CDC_ANC2_B2_CTL:
                  pANCCfg[1].dmic_x2_enable = (val & DMIC_X2_SEL);
                  break;
             case TAIKO_A_CDC_ANC1_SHIFT:
             case TAIKO_A_CDC_ANC2_SHIFT:
                  pANCCfg[ancCh].anc_ff_shift = ((val >> 4 ) & 0xF);
                  pANCCfg[ancCh].anc_fb_shift = (val & 0xF);
                  break;
             case TAIKO_A_CDC_ANC1_IIR_B1_CTL:
             case TAIKO_A_CDC_ANC2_IIR_B1_CTL:
                  pANCCfg[ancCh].adaptive = (val & 0x08);
                  break;
             case TAIKO_A_CDC_ANC1_SMLPF_CTL:
             case TAIKO_A_CDC_ANC2_SMLPF_CTL:
                  pANCCfg[ancCh].smlpf_shift = (val & 0xFF);
                  break;
             case TAIKO_A_CDC_ANC1_DCFLT_CTL:
             case TAIKO_A_CDC_ANC2_DCFLT_CTL:
                  pANCCfg[ancCh].dcflt_shift = (val & 0xFF);
                  break;
       /*
             case TAIKO_A_CDC_CLK_ANC_CLK_EN_CTL:
                  if (ancCh > 0) {
                      pANCCfg[ancCh].anc_feedback_enable = (val & 0x2);
                  } else {
                      pANCCfg[ancCh].anc_feedback_enable = (val & 0x8);
                  }
                  break;
       */
             case TAIKO_A_CDC_ANC1_LPF_B1_CTL:
             case TAIKO_A_CDC_ANC2_LPF_B1_CTL:
                  pANCCfg[ancCh].anc_ff_lpf_shift[0] = (val & 0xF);
                  pANCCfg[ancCh].anc_ff_lpf_shift[1] = ((val >> 4 ) & 0xF);
                  break;
             case TAIKO_A_CDC_ANC1_LPF_B2_CTL:
             case TAIKO_A_CDC_ANC2_LPF_B2_CTL:
                  pANCCfg[ancCh].anc_fb_lpf_shift[0] = (val & 0xF);
                  pANCCfg[ancCh].anc_fb_lpf_shift[1] = ((val >> 4 ) & 0xF);
                  break;
             case TAIKO_A_CDC_CONN_ANC_B1_CTL:
                  pANCCfg[0].input_device = (val & 0xF);
                  if (anc_acdb_size > sizeof(anc_taiko_acdb_cfg_t)) {
                     pANCCfg[1].input_device = ((val >> 4) & 0xF);
                  }
                  break;
             case TAIKO_A_CDC_ANC1_IIR_B2_CTL:
             case TAIKO_A_CDC_ANC2_IIR_B2_CTL:
                  if (val < 56) {
                  /* 56 is the total number of register addresses in Taiko for iir coefficients */
                      iir_index = val / 2;
                      iir_msb = 0;
                      if (val % 2) {
                         iir_index = (val - 1) / 2;
                         iir_msb = 1;
                      }
                  } else {
                      iir_index = -1;
                      iir_msb = 0;
                  }
                  break;
             case TAIKO_A_CDC_ANC1_IIR_B3_CTL:
             case TAIKO_A_CDC_ANC2_IIR_B3_CTL:
                  if (iir_index >= 0) {
                      if (iir_msb) {
                          if (iir_index >= 15) {
                              pANCCfg[ancCh].anc_fb_coeff[iir_index - 15] |=
                                            ((val * 16) << REGISTER_DEPTH) ;
                              // sign extend in case of -ve number
                              if (pANCCfg[ancCh].anc_fb_coeff[iir_index - 15] & 0x8000) {
                                  pANCCfg[ancCh].anc_fb_coeff[iir_index - 15] |= 0xFFFF0000;
                              }
                          } else {
                            pANCCfg[ancCh].anc_ff_coeff[iir_index] |= (val * 16) << REGISTER_DEPTH ;
                            // sign extend in case of -ve number
                            if (pANCCfg[ancCh].anc_ff_coeff[iir_index] & 0x8000) {
                                pANCCfg[ancCh].anc_ff_coeff[iir_index] |= 0xFFFF0000;
                            }
                         }
                      } else {
                        if (iir_index >= 15) {
                            pANCCfg[ancCh].anc_fb_coeff[iir_index - 15] |= (val * 16);
                        } else {
                            pANCCfg[ancCh].anc_ff_coeff[iir_index] |= (val * 16);
                        }
                     }
                 }
                 break;
            default:
               break;
            } // End of switch case
          }// End of for loop
         // set the enable fields
         pANCCfg[0].default_enable = ((reset_val & clk_val) & (1 << 0))? 1 : 0;
         pANCCfg[1].default_enable = ((reset_val & clk_val) & (1 << 2)) ? 1 : 0;
         pANCCfg[0].anc_feedback_enable = ((reset_val & clk_val) & (1 << 1)) ? 1 : 0;
         pANCCfg[1].anc_feedback_enable = ((reset_val & clk_val) & (1 << 3)) ? 1 : 0;
         break;
         }
        } // End of for switch

        return res;
}

return_code_t get_anc_reg_read_sequence(struct anc_reg_t *anc_reg_config_ptr, uint32_t size, codec_version_t codec_version)
{
        return_code_t res = SUCCESS;
        int32_t i = 0, ancCh = 0;
        uint32_t anc_index = 0;

        if ((NULL == anc_reg_config_ptr))
        {
            return INVALID_PTR;
        }
        memset(anc_reg_config_ptr, 0, sizeof(anc_reg_t));

        switch (codec_version) {
        case TAVIL:
            res = get_anc_hw5_reg_read_sequence(anc_reg_config_ptr, size, codec_version);
            break;
        case TASHA:
        {
            uint32_t offset = 0 ;
            for (ancCh = 0; ancCh < 2; ancCh++) {
               offset = ancCh * WCD9335_NUM_ANC_CH0_REG;
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_CLK_RESET_CTL+offset), READ, 0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_MODE_1_CTL+offset), READ, 0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_MODE_2_CTL+offset), READ, 0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FF_SHIFT+offset),READ,0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FB_SHIFT+offset),READ,0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_LPF_FF_A_CTL+offset),READ,0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_LPF_FF_B_CTL+offset),READ,0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_LPF_FB_CTL+offset),READ,0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_SMLPF_CTL+offset),READ,0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_DCFLT_SHIFT_CTL+offset),READ,0x0);
               for (i = 0; i < 90; i++)
               {
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_1_CTL+offset),WRITE,i);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_IIR_COEFF_2_CTL+offset),READ,0x0);
               }
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FF_A_GAIN_CTL+offset),READ,0x0);
               anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FF_B_GAIN_CTL+offset),READ,0x0);
                  anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FB_GAIN_CTL+offset),READ,0x0);
               /* extra to make even number of total reads */
                  anc_reg_config_ptr->writes[anc_index++] =
                       REG_CODEC_PACK_ENTRY((WCD9335_CDC_ANC0_FB_GAIN_CTL+offset),READ,0x0);
          }
          anc_reg_config_ptr->size = anc_index;
          break;
      }
      case TAIKO:
      default:
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY(TAIKO_A_CDC_CLK_ANC_CLK_EN_CTL, READ,0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY(TAIKO_A_CDC_CLK_ANC_RESET_CTL, READ, 0x0);
             // for channel 0
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_B1_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_B2_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_SHIFT), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B1_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_SMLPF_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_DCFLT_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_CLK_ANC_CLK_EN_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_LPF_B1_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_LPF_B2_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_CONN_ANC_B1_CTL), READ, 0x0);

             for (i = 0; i < 56; i++) {
                anc_reg_config_ptr->writes[anc_index++] =
                        TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B2_CTL), WRITE, i);
                anc_reg_config_ptr->writes[anc_index++] =
                        TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC1_IIR_B3_CTL), READ, 0x0);
             }

       // for channel 1
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_B1_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_B2_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_SHIFT), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_IIR_B1_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_SMLPF_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_DCFLT_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_CLK_ANC_CLK_EN_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_LPF_B1_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_LPF_B2_CTL), READ, 0x0);
             anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_CONN_ANC_B1_CTL), READ, 0x0);
             for (i = 0; i < 56; i++) {
                anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_IIR_B2_CTL), WRITE, i);
                anc_reg_config_ptr->writes[anc_index++] =
                     TAIKO_CODEC_PACK_ENTRY((TAIKO_A_CDC_ANC2_IIR_B3_CTL), READ, 0x0);
             }
          anc_reg_config_ptr->size = anc_index;
          break;
    }

    // check for memory corruption
    if (anc_reg_config_ptr->size > size) {
        res = FAIL;
    }

    return res;
}

return_code_t convert_anc_acdb_to_acdb(int8_t *anc_acdb_out_ptr,
      uint32_t *filled_output_size, uint32_t output_size,
      int32_t out_struct_id, int8_t *anc_acdb_in_ptr, uint32_t input_size,
      int32_t in_struct_id)
{
   return_code_t res = SUCCESS;
   int32_t num_anc_ch = 1, i = 0, j = 0;
   uint32_t ff_a_input_device = 0;
   uint32_t input_device = 0;

   if ((NULL == anc_acdb_out_ptr) || (NULL == filled_output_size) ||
       (NULL == anc_acdb_in_ptr)) {
      return INVALID_PTR;
   }

   *filled_output_size = 0;

   if (in_struct_id == out_struct_id) {
      return INVALID_ID; // only supported conversion
   }

   switch (in_struct_id) {
      case ANC_HW3:
         {
            anc_taiko_acdb_cfg_t *anc_taiko_acdb_cfg_ptr =
                       (anc_taiko_acdb_cfg_t *) anc_acdb_in_ptr;
            anc_tasha_acdb_cfg_t *anc_tasha_acdb_cfg_ptr =
                       (anc_tasha_acdb_cfg_t *) anc_acdb_out_ptr;

            if ((sizeof(anc_tasha_acdb_cfg_t) > output_size) ||
                (sizeof(anc_taiko_acdb_cfg_t) > input_size)) {
               res = NOT_ENOUGH_MEMORY;
            }
            if ((input_size >= 2 * sizeof(anc_taiko_acdb_cfg_t)) &&
                (output_size >= 2 * sizeof(anc_tasha_acdb_cfg_t))) {
               num_anc_ch = 2;
            }

            memset(anc_tasha_acdb_cfg_ptr, 0, num_anc_ch * sizeof(anc_tasha_acdb_cfg_t));
            for (i = 0; i < num_anc_ch; i++) {
               anc_tasha_acdb_cfg_ptr[i].ff_a_enable =
                         anc_taiko_acdb_cfg_ptr[i].default_enable;
               anc_tasha_acdb_cfg_ptr[i].fb_enable =
                         anc_taiko_acdb_cfg_ptr[i].anc_feedback_enable;
               anc_tasha_acdb_cfg_ptr[i].smlpf_enable =
                         anc_taiko_acdb_cfg_ptr[i].smlpf_enable;
               anc_tasha_acdb_cfg_ptr[i].ff_a_in_enable =
                         anc_taiko_acdb_cfg_ptr[i].ff_in_enable;
               anc_tasha_acdb_cfg_ptr[i].ff_out_enable =
                         anc_taiko_acdb_cfg_ptr[i].ff_out_enable;
               anc_tasha_acdb_cfg_ptr[i].ff_a_dcflt_enable =
                         anc_taiko_acdb_cfg_ptr[i].dcflt_enable;
		anc_tasha_acdb_cfg_ptr[i].ff_a_gain_enable =
			anc_taiko_acdb_cfg_ptr[i].adaptive_gain_enable;
               anc_tasha_acdb_cfg_ptr[i].ff_b_gain_enable = 0; //default since this field is not present in HW3
               anc_tasha_acdb_cfg_ptr[i].fb_gain_enable = 0; //default since this field is not present in HW3
               anc_tasha_acdb_cfg_ptr[i].ff_flex_enable =
                         anc_taiko_acdb_cfg_ptr[i].flex_enable;
               anc_tasha_acdb_cfg_ptr[i].dmic_x2_ff_a_enable =
                         anc_taiko_acdb_cfg_ptr[i].dmic_x2_enable;
               anc_tasha_acdb_cfg_ptr[i].anc_ff_a_shift =
                         anc_taiko_acdb_cfg_ptr[i].anc_ff_shift;
               anc_tasha_acdb_cfg_ptr[i].anc_fb_shift =
                         anc_taiko_acdb_cfg_ptr[i].anc_fb_shift;
               for (j = 0; j < 7; j++) {
                  anc_tasha_acdb_cfg_ptr[i].anc_ff_a_den_coeff[j] =
                            anc_taiko_acdb_cfg_ptr[i].anc_ff_coeff[j];
               }
               for (j = 0; j < 8; j++) {
                  anc_tasha_acdb_cfg_ptr[i].anc_ff_a_num_coeff[j] =
                            anc_taiko_acdb_cfg_ptr[i].anc_ff_coeff[7 + j];
               }
               anc_tasha_acdb_cfg_ptr[i].anc_ff_a_gain_scale =
                         anc_taiko_acdb_cfg_ptr[i].anc_gain;
               anc_tasha_acdb_cfg_ptr[i].anc_fb_gain_scale   =
                         anc_taiko_acdb_cfg_ptr[i].anc_gain;
               anc_tasha_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[0] =
                         anc_taiko_acdb_cfg_ptr[i].anc_ff_lpf_shift[0];
               anc_tasha_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[1] =
                         anc_taiko_acdb_cfg_ptr[i].anc_ff_lpf_shift[1];
               for (j = 0; j < 6; j++) {
                  anc_tasha_acdb_cfg_ptr[i].anc_fb_den_coeff[j] =
                            anc_taiko_acdb_cfg_ptr[i].anc_fb_coeff[j];
               }
               for (j = 0; j < 7; j++) {
                  anc_tasha_acdb_cfg_ptr[i].anc_fb_num_coeff[j] =
                            anc_taiko_acdb_cfg_ptr[i].anc_fb_coeff[6 + j];
               }
               anc_tasha_acdb_cfg_ptr[i].anc_ff_a_gain_scale_default =
                         anc_taiko_acdb_cfg_ptr[i].anc_gain_default;
               anc_tasha_acdb_cfg_ptr[i].anc_fb_gain_scale_default =
                         anc_taiko_acdb_cfg_ptr[i].anc_gain_default;
               anc_tasha_acdb_cfg_ptr[i].anc_fb_lpf_shift[0] =
                         anc_taiko_acdb_cfg_ptr[i].anc_fb_lpf_shift[0];
               anc_tasha_acdb_cfg_ptr[i].anc_fb_lpf_shift[1] =
                         anc_taiko_acdb_cfg_ptr[i].anc_fb_lpf_shift[1];
               switch(anc_taiko_acdb_cfg_ptr[i].input_device) {
                  case ADIE_CODEC_ADC1:
                        ff_a_input_device = (ADIE_MAP_ADC0);
                        break;
                  case ADIE_CODEC_ADC2:
                        ff_a_input_device = (ADIE_MAP_ADC1);
                        break;
                  case ADIE_CODEC_ADC3:
                        ff_a_input_device = (ADIE_MAP_ADC2);
                        break;
                  case ADIE_CODEC_ADC4:
                        ff_a_input_device = (ADIE_MAP_ADC3);
                        break;
                  case ADIE_CODEC_ADC5:
                        ff_a_input_device = (ADIE_MAP_ADC4);
                        break;
                  case ADIE_CODEC_ADC6:
                        ff_a_input_device = (ADIE_MAP_ADC5);
                        break;
                  case ADIE_CODEC_DMIC1:
                        ff_a_input_device = (ADIE_MAP_DMIC0);
                        break;
                  case ADIE_CODEC_DMIC2:
                        ff_a_input_device = (ADIE_MAP_DMIC1);
                        break;
                  case ADIE_CODEC_DMIC3:
                        ff_a_input_device = (ADIE_MAP_DMIC2);
                        break;
                  case ADIE_CODEC_DMIC4:
                        ff_a_input_device = (ADIE_MAP_DMIC3);
                        break;
                  case ADIE_CODEC_DMIC5:
                        ff_a_input_device = (ADIE_MAP_DMIC4);
                        break;
                  case ADIE_CODEC_DMIC6:
                        ff_a_input_device = (ADIE_MAP_DMIC5);
                        break;
                  case ADIE_CODEC_ZERO:
                  default:
                        ff_a_input_device = (ADIE_MAP_ZERO);
                        break;
               }
               anc_tasha_acdb_cfg_ptr[i].ff_a_input_device = ff_a_input_device;
               anc_tasha_acdb_cfg_ptr[i].smlpf_shift =
                         anc_taiko_acdb_cfg_ptr[i].smlpf_shift;
               anc_tasha_acdb_cfg_ptr[i].ff_a_dcflt_shift =
                         anc_taiko_acdb_cfg_ptr[i].dcflt_shift;
		anc_tasha_acdb_cfg_ptr[i].anc_ff_a_gain =
			 anc_taiko_acdb_cfg_ptr[i].adaptive_gain;
               anc_tasha_acdb_cfg_ptr[i].anc_ff_b_gain =
					 UNITY_GAIN; // Unity default since this field is not present in HW3
               anc_tasha_acdb_cfg_ptr[i].anc_fb_gain =
					 UNITY_GAIN; // Unity default since this field is not present in HW3
               *filled_output_size += sizeof(anc_tasha_acdb_cfg_t);
            } // end of for loop
            break;
         }
   case ANC_HW4:
        if (ANC_HW5 == out_struct_id) {
            res = convert_anc_hw5_hw4_acdb_to_acdb(anc_acdb_out_ptr, filled_output_size, output_size,
                                                 out_struct_id, anc_acdb_in_ptr, input_size, in_struct_id);
        } else if (ANC_HW3 == out_struct_id) {
            anc_tasha_acdb_cfg_t *anc_tasha_acdb_cfg_ptr =
                                 (anc_tasha_acdb_cfg_t *) anc_acdb_in_ptr;
            anc_taiko_acdb_cfg_t *anc_taiko_acdb_cfg_ptr =
                                 (anc_taiko_acdb_cfg_t *) anc_acdb_out_ptr;

            if ((sizeof(anc_tasha_acdb_cfg_t) > input_size) ||
                (sizeof(anc_taiko_acdb_cfg_t) > output_size)){
               res = NOT_ENOUGH_MEMORY;
            }
            if ((output_size >= 2 * sizeof(anc_taiko_acdb_cfg_t)) &&
                (input_size >= 2 * sizeof(anc_tasha_acdb_cfg_t))){
               num_anc_ch = 2;
            }
            memset(anc_taiko_acdb_cfg_ptr, 0, num_anc_ch * sizeof(anc_taiko_acdb_cfg_t));

            for (i = 0; i < num_anc_ch; i++)
            {
               anc_taiko_acdb_cfg_ptr[i].default_enable =
                                        (uint8_t)anc_tasha_acdb_cfg_ptr[i].ff_a_enable;
               anc_taiko_acdb_cfg_ptr[i].anc_feedback_enable =
                                        (uint8_t)anc_tasha_acdb_cfg_ptr[i].fb_enable;
               anc_taiko_acdb_cfg_ptr[i].anc_lr_mix_enable = 0;
               anc_taiko_acdb_cfg_ptr[i].smlpf_enable =
                                        (uint8_t)anc_tasha_acdb_cfg_ptr[i].smlpf_enable;
               anc_taiko_acdb_cfg_ptr[i].ff_in_enable =
                                        (uint8_t)anc_tasha_acdb_cfg_ptr[i].ff_a_in_enable;
               anc_taiko_acdb_cfg_ptr[i].hybrid_enable = 0;
               anc_taiko_acdb_cfg_ptr[i].ff_out_enable =
                                        (uint8_t)anc_tasha_acdb_cfg_ptr[i].ff_out_enable;
               anc_taiko_acdb_cfg_ptr[i].dcflt_enable =
                                        (uint8_t)anc_tasha_acdb_cfg_ptr[i].ff_a_dcflt_enable;
               anc_taiko_acdb_cfg_ptr[i].adaptive = 0;
		anc_taiko_acdb_cfg_ptr[i].adaptive_gain_enable =
					(uint8_t)anc_tasha_acdb_cfg_ptr[i].ff_a_gain_enable;
               anc_taiko_acdb_cfg_ptr[i].flex_enable = 0; // hard coding to 0
               anc_taiko_acdb_cfg_ptr[i].dmic_x2_enable =
                                        (uint8_t)anc_tasha_acdb_cfg_ptr[i].dmic_x2_ff_a_enable;
               anc_taiko_acdb_cfg_ptr[i].anc_lr_mix_output_channel = 0;
               anc_taiko_acdb_cfg_ptr[i].anc_ff_shift =
                                        (int8_t)anc_tasha_acdb_cfg_ptr[i].anc_ff_a_shift;
               anc_taiko_acdb_cfg_ptr[i].anc_fb_shift =
                                        (int8_t)anc_tasha_acdb_cfg_ptr[i].anc_fb_shift;

               for (j = 0; j < 7; j++) {
                  anc_taiko_acdb_cfg_ptr[i].anc_ff_coeff[j] =
                            anc_tasha_acdb_cfg_ptr[i].anc_ff_a_den_coeff[j];
               }
               for (j = 0; j < 8; j++) {
                  anc_taiko_acdb_cfg_ptr[i].anc_ff_coeff[7 + j] =
                            anc_tasha_acdb_cfg_ptr[i].anc_ff_a_num_coeff[j];
               }
               anc_taiko_acdb_cfg_ptr[i].anc_gain =
                              anc_tasha_acdb_cfg_ptr[i].anc_ff_a_gain_scale;
               anc_taiko_acdb_cfg_ptr[i].anc_ff_lpf_shift[0] =
                              anc_tasha_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[0];
               anc_taiko_acdb_cfg_ptr[i].anc_ff_lpf_shift[1] =
                              anc_tasha_acdb_cfg_ptr[i].anc_ff_a_lpf_shift[1];

               if ((anc_tasha_acdb_cfg_ptr[i].anc_fb_den_coeff[6] != 0) ||
                   (anc_tasha_acdb_cfg_ptr[i].anc_fb_num_coeff[7] != 0)) {
                  for (j = 0; j < 6; j++) {
                     anc_taiko_acdb_cfg_ptr[i].anc_fb_coeff[j] = 0;
                  }
                  for (j = 0; j < 7; j++) {
                     anc_taiko_acdb_cfg_ptr[i].anc_fb_coeff[6 + j] = 0;
                  }
               } else {
                  for (j = 0; j < 6; j++) {
                     anc_taiko_acdb_cfg_ptr[i].anc_fb_coeff[j] =
                               anc_tasha_acdb_cfg_ptr[i].anc_fb_den_coeff[j];
                  }
                  for (j = 0; j < 7; j++) {
                     anc_taiko_acdb_cfg_ptr[i].anc_fb_coeff[6 + j] =
                               anc_tasha_acdb_cfg_ptr[i].anc_fb_num_coeff[j];
                  }
               }
               anc_taiko_acdb_cfg_ptr[i].anc_gain_default =
                         anc_tasha_acdb_cfg_ptr[i].anc_ff_a_gain_scale_default;
               anc_taiko_acdb_cfg_ptr[i].anc_fb_lpf_shift[0] =
                         anc_tasha_acdb_cfg_ptr[i].anc_fb_lpf_shift[0];
               anc_taiko_acdb_cfg_ptr[i].anc_fb_lpf_shift[1] =
                         anc_tasha_acdb_cfg_ptr[i].anc_fb_lpf_shift[1];

               switch(anc_tasha_acdb_cfg_ptr[i].ff_a_input_device) {
                  case ADIE_MAP_ADC0:
                        input_device = (ADIE_CODEC_ADC1);
                        break;
                  case ADIE_MAP_ADC1:
                        input_device = (ADIE_CODEC_ADC2);
                        break;
                  case ADIE_MAP_ADC2:
                        input_device = (ADIE_CODEC_ADC3);
                        break;
                  case ADIE_MAP_ADC3:
                        input_device = (ADIE_CODEC_ADC4);
                        break;
                  case ADIE_MAP_ADC4:
                        input_device = (ADIE_CODEC_ADC5);
                        break;
                  case ADIE_MAP_ADC5:
                        input_device = (ADIE_CODEC_ADC6);
                        break;
                  case ADIE_MAP_DMIC0:
                        input_device = (ADIE_CODEC_DMIC1);
                        break;
                  case ADIE_MAP_DMIC1:
                        input_device = (ADIE_CODEC_DMIC2);
                        break;
                  case ADIE_MAP_DMIC2:
                        input_device = (ADIE_CODEC_DMIC3);
                        break;
                  case ADIE_MAP_DMIC3:
                        input_device = (ADIE_CODEC_DMIC4);
                        break;
                  case ADIE_MAP_DMIC4:
                        input_device = (ADIE_CODEC_DMIC5);
                        break;
                  case ADIE_MAP_DMIC5:
                        input_device = (ADIE_CODEC_DMIC6);
                        break;
                  case ADIE_MAP_ZERO:
                  default:
                        input_device = (ADIE_CODEC_ZERO);
                        break;
               }
		anc_taiko_acdb_cfg_ptr[i].input_device  = input_device;
		anc_taiko_acdb_cfg_ptr[i].smlpf_shift = anc_tasha_acdb_cfg_ptr[i].smlpf_shift ;
		anc_taiko_acdb_cfg_ptr[i].dcflt_shift = anc_tasha_acdb_cfg_ptr[i].ff_a_dcflt_shift ;
		anc_taiko_acdb_cfg_ptr[i].adaptive_gain = anc_tasha_acdb_cfg_ptr[i].anc_ff_a_gain;
               *filled_output_size += sizeof(anc_taiko_acdb_cfg_t);
            } // end of for loop
            break;
         }
      case ANC_HW5:
      {
         if (ANC_HW4 == out_struct_id)
            res = convert_anc_hw5_hw4_acdb_to_acdb(anc_acdb_out_ptr, filled_output_size,
                                                   output_size, out_struct_id, anc_acdb_in_ptr,
                                                   input_size, in_struct_id);
         else
            return INVALID_ID; // no version higher than HW5 right now, so it is not valid and HW5 to HW3 is not supported as well

         break;
      }
   } //end switch (in_struct_id)

   return res;
}

void parse_codec_type(char *snd_card_name)
{
	if (snd_card_name != NULL){
		if (strstr(snd_card_name, "tasha"))
			snd_card_type = TASHA;
		else if (strstr(snd_card_name, "tavil"))
			snd_card_type = TAVIL;
		else
			snd_card_type = TAIKO;
	}
}

return_code_t anc_conversion(anc_reg_t *anc_reg_config_ptr,
	int *pANCCfg, int anc_version)
{
	return_code_t result;

	if(snd_card_type ==  TASHA && anc_version == ANC_V0) {
		anc_tasha_acdb_cfg_t pANCCfg_tasha[2];
		uint32_t filled_output_size;

		convert_anc_acdb_to_acdb((int8_t *)(pANCCfg_tasha), &filled_output_size,
			NUM_ANC_COMPONENTS * sizeof(anc_tasha_acdb_cfg_t), ANC_HW4,
			(int8_t *)(pANCCfg), NUM_ANC_COMPONENTS * sizeof(anc_taiko_acdb_cfg_t), ANC_HW3);

		result = convert_anc_acdb_to_reg(anc_reg_config_ptr, (int8_t *)pANCCfg_tasha,
			NUM_ANC_COMPONENTS, TASHA);
	} else	if(snd_card_type == TAVIL && anc_version == ANC_V1) {
		anc_tasha_acdb_cfg_t pANCCfg_tavil[2];
		uint32_t filled_output_size;

		convert_anc_acdb_to_acdb((int8_t *)(pANCCfg_tavil), &filled_output_size,
			NUM_ANC_COMPONENTS * sizeof(anc_wcd_hw5_acdb_cfg_t), ANC_HW5,
			(int8_t *)(pANCCfg), NUM_ANC_COMPONENTS * sizeof(anc_tasha_acdb_cfg_t), ANC_HW4);

		result = convert_anc_acdb_to_reg(anc_reg_config_ptr, (int8_t *)pANCCfg_tavil,
			NUM_ANC_COMPONENTS, snd_card_type);
	} else {
		result = convert_anc_acdb_to_reg(anc_reg_config_ptr, (int8_t *)pANCCfg,
				NUM_ANC_COMPONENTS, snd_card_type);
	}

	return result;
}

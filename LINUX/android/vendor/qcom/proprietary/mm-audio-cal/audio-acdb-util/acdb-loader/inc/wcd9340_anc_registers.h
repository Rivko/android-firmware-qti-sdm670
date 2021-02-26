/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef WCD9340_ANC_REGISTERS_H
#define WCD9340_ANC_REGISTERS_H

// Register
#define WCD9340_CDC_ANC0_CLK_RESET_CTL                      (0xA01)
//Bitmasks
#define WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_A_EN      (1 << 7)
#define WCD9340_CDC_ANC0_CLK_RESET_CTL___SMARTLNQ_B_EN      (1 << 6)
#define WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_RESET           (1 << 5)
#define WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_RESET          (1 << 4)
#define WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_RESET          (1 << 3)
#define WCD9340_CDC_ANC0_CLK_RESET_CTL___FB_CLK_EN          (1 << 2)
#define WCD9340_CDC_ANC0_CLK_RESET_CTL___FFB_CLK_EN         (1 << 1)
#define WCD9340_CDC_ANC0_CLK_RESET_CTL___FFA_CLK_EN         (1 << 0)

// Register
#define WCD9340_CDC_ANC0_MODE_1_CTL                         (0xA02)
//Bitmasks
#define WCD9340_CDC_ANC0_MODE_1_CTL___FB_ON_FBMON_IS_TRUE   (1 << 7)
#define WCD9340_CDC_ANC0_MODE_1_CTL___ANC_SMLPF_EN          (1 << 6)
#define WCD9340_CDC_ANC0_MODE_1_CTL___FFA_IN_EN             (1 << 5)
#define WCD9340_CDC_ANC0_MODE_1_CTL___FFB_IN_EN             (1 << 4)
#define WCD9340_CDC_ANC0_MODE_1_CTL___ANC_OUTMIX_EN         (1 << 3)
#define WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_B_SEL    (1 << 2)
#define WCD9340_CDC_ANC0_MODE_1_CTL___ANC_ADC_DMIC_A_SEL    (1 << 1)
#define WCD9340_CDC_ANC0_MODE_1_CTL___ANC_OUT_EN            (1 << 0)

//Register
#define WCD9340_CDC_ANC0_MODE_2_CTL                         (0xA03)
//Bitmasks
#define WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FBGAIN_EN         (1 << 7)
#define WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_A_SEL     (1 << 6)
#define WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DMIC_X2_B_SEL     (1 << 5)
#define WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_A_EN       (1 << 4)
#define WCD9340_CDC_ANC0_MODE_2_CTL___ANC_DCFILT_B_EN       (1 << 3)
#define WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_A_EN       (1 << 2)
#define WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFGAIN_B_EN       (1 << 1)
#define WCD9340_CDC_ANC0_MODE_2_CTL___ANC_FFLE_EN           (1 << 0)

//Register
#define WCD9340_CDC_ANC0_FF_SHIFT                           (0xA04)
//Bitmasks
#define WCD9340_CDC_ANC0_FF_SHIFT___FFA_SHIFT_INDEX         (4)
#define WCD9340_CDC_ANC0_FF_SHIFT___FFA_SHIFT               (0xf << WCD9340_CDC_ANC0_FF_SHIFT___FFA_SHIFT_INDEX)
#define WCD9340_CDC_ANC0_FF_SHIFT___FFB_SHIFT_INDEX         (0)
#define WCD9340_CDC_ANC0_FF_SHIFT___FFB_SHIFT               (0xf << WCD9340_CDC_ANC0_FF_SHIFT___FFB_SHIFT_INDEX)

//Register
#define WCD9340_CDC_ANC0_FB_SHIFT                           (0xA05)
//Bitmasks
#define WCD9340_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT_INDEX      (0)
#define WCD9340_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT            (0xf << WCD9340_CDC_ANC0_FB_SHIFT___FB_LPF_SHIFT_INDEX)

//Register
#define WCD9340_CDC_ANC0_LPF_FF_A_CTL                       (0xA06)
//Bitmasks
#define WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2_INDEX    (4)
#define WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2          (0xf << WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S2_INDEX)
#define WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1_INDEX    (0)
#define WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1          (0xf << WCD9340_CDC_ANC0_LPF_FF_A_CTL___LPF_FFA_S1_INDEX)

//Register
#define WCD9340_CDC_ANC0_LPF_FF_B_CTL                       (0xA07)
//Bitmasks
#define WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2_INDEX    (4)
#define WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2          (0xf << WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S2_INDEX)
#define WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1_INDEX    (0)
#define WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1          (0xf << WCD9340_CDC_ANC0_LPF_FF_B_CTL___LPF_FFB_S1_INDEX)

//Register
#define WCD9340_CDC_ANC0_LPF_FB_CTL                         (0xA08)
//Bitmasks
#define WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2_INDEX       (4)
#define WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2             (0xf << WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S2_INDEX)
#define WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1_INDEX       (0)
#define WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1             (0xf << WCD9340_CDC_ANC0_LPF_FB_CTL___LPF_FB_S1_INDEX)

//Register
#define WCD9340_CDC_ANC0_SMLPF_CTL                          (0xA09)
//Bitmasks
#define WCD9340_CDC_ANC0_SMLPF_CTL___SHIFT_INDEX            (0)
#define WCD9340_CDC_ANC0_SMLPF_CTL___SHIFT                  (0x7 << WCD9340_CDC_ANC0_SMLPF_CTL___SHIFT_INDEX)

//Register
#define WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL                    (0xA0A)
//Bitmasks
#define WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT_INDEX    (3)
#define WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT          (0x7 << WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___B_SHIFT_INDEX)
#define WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT_INDEX    (0)
#define WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT          (0x7 << WCD9340_CDC_ANC0_DCFLT_SHIFT_CTL___A_SHIFT_INDEX)

//Register
#define WCD9340_CDC_ANC0_IIR_ADAPT_CTL                      (0xA0B)
//Bitmasks
#define WCD9340_CDC_ANC0_IIR_ADAPT_CTL___FFGAIN_UPDATE_EN   (1 << 3)
#define WCD9340_CDC_ANC0_IIR_ADAPT_CTL___FFGAIN_ADAPTIVE    (1 << 2)
#define WCD9340_CDC_ANC0_IIR_ADAPT_CTL___COEF_UPDATE_EN     (1 << 1)
#define WCD9340_CDC_ANC0_IIR_ADAPT_CTL___COEF_ADAPTIVE      (1 << 0)


//Register
#define WCD9340_CDC_ANC0_IIR_COEFF_1_CTL                    (0xA0C)
//Bitmasks
#define WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___AUTO_INC         (1 << 7)
#define WCD9340_CDC_ANC0_IIR_COEFF_1_CTL___COEF_PTR         (0x7f << 0)


//Register
#define WCD9340_CDC_ANC0_IIR_COEFF_2_CTL                    (0xA0D)
//Bitmasks
#define WCD9340_CDC_ANC0_IIR_COEFF_2_CTL___COEF             (0xff << 0)

//Register
#define WCD9340_CDC_ANC0_FF_A_GAIN_CTL                      (0xA0E)
//Bitmasks
#define WCD9340_CDC_ANC0_FF_A_GAIN_CTL___GAIN               (0xff << 0)

//Register
#define WCD9340_CDC_ANC0_FF_B_GAIN_CTL                      (0xA0F)
//Bitmasks
#define WCD9340_CDC_ANC0_FF_B_GAIN_CTL___GAIN               (0xff << 0)


//Register
#define WCD9340_CDC_ANC0_FB_GAIN_CTL                        (0xA10)
//Bitmasks
#define WCD9340_CDC_ANC0_FB_GAIN_CTL___GAIN                 (0xff << 0)

// ANC1 register addresses are at the following offset from the ANC0 registers
#define WCD9340_CDC_ANC1_OFFSET                             (24)

#endif//#ifndef WCD9340_ANC_REGISTERS_H


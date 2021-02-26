/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ANC_TAIKO_BITMASKS_H
#define ANC_TAIKO_BITMASKS_H

// Register CDC_ANCn_B1_CTL bitmasks
#define FFGAIN_EN  (1 << 7)
//0x0: DISABLE
//0x1: ENABLE

#define SMLPF_EN  (1 << 6)
//0x0: DISABLE
//0x1: ENABLE

#define DCFLT_EN  (1 << 5)
//0x0: DISABLE
//0x1: ENABLE

#define FF_IN_EN  (1 << 4)
//Specifies ANC Enable
//0x0: DISABLE
//0x1: ENABLE

#define HYBRID_EN  (1 << 3)
//Specifies ANC FB MIxing
//0x0: DISABLE
//0x1: ENABLE

#define LR_EN  (1 << 2)
//Specifies ANC L/R MIxing
//0x0: DISABLE
//0x1: ENABLE

#define ADC_DMIC_SEL  (1 << 1)
//Specifies ANC Input Source
//0x0: ADC
//0x1: DMIC

#define FF_OUT_EN (1 << 0)
//Specifies ANC FF Output
//0x0: DISABLE
//0x1: ENABLE



// Register TAIKO_A_CDC_ANCn_B2_CTL
#define FFLE_EN (1 << 1)
//0x0: DISABLE
//0x1: ENABLE

#define DMIC_X2_SEL (1 << 0)
//0x0: DISABLE
//0x1: ENABLE


#endif

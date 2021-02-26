#ifndef DALSBIPROPS_H
#define DALSBIPROPS_H
/*
===========================================================================

FILE:         DalSBIProps.h

DESCRIPTION:  This file contains enumeration values that can be used in
              DAL SBI property files.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
04/27/11   MJS     Add general-purpose HW ports.
10/22/09   MJS     Added PMIC Arbiter API extension.
04/21/09   MJS     Initial revision.

===========================================================================
             Copyright (c) 2009, 2011 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* The possible sampling delay values for SSBI and SSBI FTM mode buses */

#define  SBI_SAMP_DEL_0_12_NS 0  /* Delay in range of 0 - 12 ns (SSBI FTM only) */
#define  SBI_SAMP_DEL_0_24_NS 1  /* Delay in range of 0 - 24 ns (SSBI FTM only) */
#define  SBI_SAMP_DEL_0_37_NS 2  /* Delay in range of 0 - 37 ns (SSBI only) */
#define  SBI_SAMP_DEL_12_49_NS 3  /* Delay in range of 12 - 49 ns */
#define  SBI_SAMP_DEL_25_62_NS 4  /* Delay in range of 25 - 62 ns */
#define  SBI_SAMP_DEL_37_74_NS 5  /* Delay in range of 37 - 74 ns (SSBI FTM only) */
#define  SBI_SAMP_DEL_50_87_NS 6  /* Delay in range of 50 - 87 ns (SSBI only) */
#define  SBI_SAMP_DEL_62_99_NS 7  /* Delay in range of 62 - 99 ns */
#define  SBI_SAMP_DEL_75_112_NS 8  /* Delay in range of 75 - 112 ns */
#define  SBI_SAMP_DEL_87_124_NS 9  /* Delay in range of 87 - 124 ns (SSBI FTM only) */
#define  SBI_SAMP_DEL_100_137_NS 10  /* Delay in range of 100 - 137 ns (SSBI only) */
#define  SBI_SAMP_DEL_112_149_NS 11  /* Delay in range of 112 - 149 ns */
#define  SBI_SAMP_DEL_125_162_NS 12  /* Delay in range of 125 - 162 ns */
#define  SBI_SAMP_DEL_150_187_NS 13  /* Delay in range of 150 - 187 ns (SSBI only) */
#define  SBI_SAMP_DEL_162_199_NS 14  /* Delay in range of 162 - 199 ns (SSBI only) */

/* Defines for SSBI hardware request ports */
#define SBI_PROP_HW_PORT_RXFRONT_0       0x00a00001
#define SBI_PROP_HW_PORT_RXFRONT_1       0x00a00002
#define SBI_PROP_HW_PORT_AGC_0           0x00a00003
#define SBI_PROP_HW_PORT_AGC_1           0x00a00004
#define SBI_PROP_HW_PORT_MICRO           0x00a00005
#define SBI_PROP_HW_PORT_MDSP            0x00a00006
#define SBI_PROP_HW_PORT_ADSP            0x00a00007
#define SBI_PROP_HW_PORT_TSADC           0x00a00008
#define SBI_PROP_HW_PORT_SVS             0x00a00009
#define SBI_PROP_HW_PORT_UNUSED_0        0x00a0000a
#define SBI_PROP_HW_PORT_GP_1            0x00a0000a
#define SBI_PROP_HW_PORT_UNUSED_1        0x00a0000b
#define SBI_PROP_HW_PORT_GP_2            0x00a0000b
#define SBI_PROP_HW_PORT_UNUSED_2        0x00a0000c
#define SBI_PROP_HW_PORT_GP_3            0x00a0000c
#define SBI_PROP_HW_PORT_LPASS_Q6_SAW    0x00a0000d
#define SBI_PROP_HW_PORT_SMPSS_CPU0_SAW  0x00a0000e
#define SBI_PROP_HW_PORT_SMPSS_CPU1_SAW  0x00a0000f


#endif /* DALSBIPROPS_H */


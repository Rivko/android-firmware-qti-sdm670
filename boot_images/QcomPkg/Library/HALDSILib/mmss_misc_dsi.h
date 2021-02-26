#ifndef _MMSS_MISC_DSI_REG_H_
#define _MMSS_MISC_DSI_REG_H_
/*=================================================================================================

  File: Mmss_misc_dsi.h
  

     Copyright (c) 2008-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=================================================================================================*/

/* Extern MMSS_MISC block base address */
#define MMSS_MISC_BASE_PHY                                                                      0xFD800000

#define MMSS_MISC_BASE                                                                          (MMSS_MISC_BASE_PHY)
#define MMSS_MISC_REG_BASE                                                                      (MMSS_MISC_BASE + 0x00000000)


#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_ADDR                                                       (MMSS_MISC_REG_BASE + 0x00000014)
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_RMSK                                                       0x83ff83ff
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_IN          \
        in_dword_masked(HWIO_MMSS_DSI_ULP_CLAMP_CTRL_ADDR, HWIO_MMSS_DSI_ULP_CLAMP_CTRL_RMSK)
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_INM(m)      \
        in_dword_masked(HWIO_MMSS_DSI_ULP_CLAMP_CTRL_ADDR, m)
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_OUT(v)      \
        out_dword(HWIO_MMSS_DSI_ULP_CLAMP_CTRL_ADDR,v)
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MMSS_DSI_ULP_CLAMP_CTRL_ADDR,m,v,HWIO_MMSS_DSI_ULP_CLAMP_CTRL_IN)
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLAMP_EN_BMSK                                         0x80000000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLAMP_EN_SHFT                                               0x1f
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLKLN_EN_BMSK                                          0x2000000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLKLN_EN_SHFT                                               0x19
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLKLN_ULPS_REQUEST_BMSK                                0x1000000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_CLKLN_ULPS_REQUEST_SHFT                                     0x18
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN0_EN_BMSK                                            0x800000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN0_EN_SHFT                                                0x17
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN0_ULPS_REQUEST_BMSK                                  0x400000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN0_ULPS_REQUEST_SHFT                                      0x16
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN1_EN_BMSK                                            0x200000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN1_EN_SHFT                                                0x15
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN1_ULPS_REQUEST_BMSK                                  0x100000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN1_ULPS_REQUEST_SHFT                                      0x14
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN2_EN_BMSK                                             0x80000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN2_EN_SHFT                                                0x13
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN2_ULPS_REQUEST_BMSK                                   0x40000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN2_ULPS_REQUEST_SHFT                                      0x12
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN3_EN_BMSK                                             0x20000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN3_EN_SHFT                                                0x11
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN3_ULPS_REQUEST_BMSK                                   0x10000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI1_DLN3_ULPS_REQUEST_SHFT                                      0x10
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLAMP_EN_BMSK                                             0x8000
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLAMP_EN_SHFT                                                0xf
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_EN_BMSK                                              0x200
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_EN_SHFT                                                0x9
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_ULPS_REQUEST_BMSK                                    0x100
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_CLKLN_ULPS_REQUEST_SHFT                                      0x8
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_EN_BMSK                                                0x80
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_EN_SHFT                                                 0x7
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_ULPS_REQUEST_BMSK                                      0x40
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN0_ULPS_REQUEST_SHFT                                       0x6
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_EN_BMSK                                                0x20
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_EN_SHFT                                                 0x5
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_ULPS_REQUEST_BMSK                                      0x10
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN1_ULPS_REQUEST_SHFT                                       0x4
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_EN_BMSK                                                 0x8
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_EN_SHFT                                                 0x3
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_ULPS_REQUEST_BMSK                                       0x4
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN2_ULPS_REQUEST_SHFT                                       0x2
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_EN_BMSK                                                 0x2
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_EN_SHFT                                                 0x1
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_ULPS_REQUEST_BMSK                                       0x1
#define HWIO_MMSS_DSI_ULP_CLAMP_CTRL_DSI0_DLN3_ULPS_REQUEST_SHFT                                       0x0


#endif /* _MMSS_MISC_DSI_REG_H_ */


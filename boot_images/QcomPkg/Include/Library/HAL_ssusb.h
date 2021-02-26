#ifndef HAL_SSUSB
#define HAL_SSUSB
/*=============================================================================

       Hardware Abstraction Layer Super-Speed USB

=============================================================================*/
/**
@file HAL_ssusb.h

Hardware Abstraction Layer (HAL) external interface for the SS-USB core.

This file contains the definitions of the SS-USB identification,
capability, and operational registers.

*/
/*=============================================================================
Copyright (c) 2011 - 2013, QUALCOMM Technologies Inc. All rights reserved.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.
=============================================================================*/

/*=============================================================================

EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when      who      what, where, why
--------  ---      ---------------------------------------------------------
02/13/13  amitg  Initial draft for SSUSB core
=============================================================================*/


/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "com_dtypes.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*----------------------------------------------------------------------------
 * MODULE: USB30_QSCRATCH
 *--------------------------------------------------------------------------*/

#define USB30_QSCRATCH_REG_BASE_OFFS                                      0x000f8800

#define HAL_SSUSB_PHY_PARAM_CTRL_1_ADDR(x)                                  ((x) + 0x00000034)
#define HAL_SSUSB_PHY_PARAM_CTRL_1_PHYS(x)                                  ((x) + 0x00000034)
#define HAL_SSUSB_PHY_PARAM_CTRL_1_OFFS                                     (0x00000034)
#define HAL_SSUSB_PHY_PARAM_CTRL_1_RMSK                                     0xffffffff
#define HAL_SSUSB_PHY_PARAM_CTRL_1_POR                                      0x0718154a
#define HAL_SSUSB_PHY_PARAM_CTRL_1_IN(x)      \
        in_dword_masked(HAL_SSUSB_PHY_PARAM_CTRL_1_ADDR(x), HAL_SSUSB_PHY_PARAM_CTRL_1_RMSK)
#define HAL_SSUSB_PHY_PARAM_CTRL_1_INM(x, m)      \
        in_dword_masked(HAL_SSUSB_PHY_PARAM_CTRL_1_ADDR(x), m)
#define HAL_SSUSB_PHY_PARAM_CTRL_1_OUT(x, v)      \
        out_dword(HAL_SSUSB_PHY_PARAM_CTRL_1_ADDR(x),v)
#define HAL_SSUSB_PHY_PARAM_CTRL_1_OUTM(x,m,v) \
        out_dword_masked_ns(HAL_SSUSB_PHY_PARAM_CTRL_1_ADDR(x),m,v,HAL_SSUSB_PHY_PARAM_CTRL_1_IN(x))
#define HAL_SSUSB_PHY_PARAM_CTRL_1_LANE0_TX_TERM_OFFSET_BMSK                0xf8000000
#define HAL_SSUSB_PHY_PARAM_CTRL_1_LANE0_TX_TERM_OFFSET_SHFT                      0x1b
#define HAL_SSUSB_PHY_PARAM_CTRL_1_TX_SWING_FULL_BMSK                        0x7f00000
#define HAL_SSUSB_PHY_PARAM_CTRL_1_TX_SWING_FULL_SHFT                             0x14
#define HAL_SSUSB_PHY_PARAM_CTRL_1_TX_DEEMPH_6DB_BMSK                          0xfc000
#define HAL_SSUSB_PHY_PARAM_CTRL_1_TX_DEEMPH_6DB_SHFT                              0xe
#define HAL_SSUSB_PHY_PARAM_CTRL_1_TX_DEEMPH_3_5DB_BMSK                         0x3f00
#define HAL_SSUSB_PHY_PARAM_CTRL_1_TX_DEEMPH_3_5DB_SHFT                            0x8
#define HAL_SSUSB_PHY_PARAM_CTRL_1_LOS_LEVEL_BMSK                                 0xf8
#define HAL_SSUSB_PHY_PARAM_CTRL_1_LOS_LEVEL_SHFT                                  0x3
#define HAL_SSUSB_PHY_PARAM_CTRL_1_LOS_BIAS_BMSK                                   0x7
#define HAL_SSUSB_PHY_PARAM_CTRL_1_LOS_BIAS_SHFT                                   0x0

#define HAL_SSUSB_PHY_PARAM_CTRL_2_ADDR(x)                                  ((x) + 0x00000038)
#define HAL_SSUSB_PHY_PARAM_CTRL_2_PHYS(x)                                  ((x) + 0x00000038)
#define HAL_SSUSB_PHY_PARAM_CTRL_2_OFFS                                     (0x00000038)
#define HAL_SSUSB_PHY_PARAM_CTRL_2_RMSK                                           0x37
#define HAL_SSUSB_PHY_PARAM_CTRL_2_POR                                      0x00000004
#define HAL_SSUSB_PHY_PARAM_CTRL_2_IN(x)      \
        in_dword_masked(HAL_SSUSB_PHY_PARAM_CTRL_2_ADDR(x), HAL_SSUSB_PHY_PARAM_CTRL_2_RMSK)
#define HAL_SSUSB_PHY_PARAM_CTRL_2_INM(x, m)      \
        in_dword_masked(HAL_SSUSB_PHY_PARAM_CTRL_2_ADDR(x), m)
#define HAL_SSUSB_PHY_PARAM_CTRL_2_OUT(x, v)      \
        out_dword(HAL_SSUSB_PHY_PARAM_CTRL_2_ADDR(x),v)
#define HAL_SSUSB_PHY_PARAM_CTRL_2_OUTM(x,m,v) \
        out_dword_masked_ns(HAL_SSUSB_PHY_PARAM_CTRL_2_ADDR(x),m,v,HAL_SSUSB_PHY_PARAM_CTRL_2_IN(x))
#define HAL_SSUSB_PHY_PARAM_CTRL_2_LANE0_TX2RX_LOOPBACK_BMSK                      0x20
#define HAL_SSUSB_PHY_PARAM_CTRL_2_LANE0_TX2RX_LOOPBACK_SHFT                       0x5
#define HAL_SSUSB_PHY_PARAM_CTRL_2_LANE0_EXT_PCLK_REQ_BMSK                        0x10
#define HAL_SSUSB_PHY_PARAM_CTRL_2_LANE0_EXT_PCLK_REQ_SHFT                         0x4
#define HAL_SSUSB_PHY_PARAM_CTRL_2_TX_VBOOST_LEVEL_BMSK                            0x7
#define HAL_SSUSB_PHY_PARAM_CTRL_2_TX_VBOOST_LEVEL_SHFT                            0x0

#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_ADDR(x)                               ((x) + 0x0000003c)
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_PHYS(x)                               ((x) + 0x0000003c)
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_OFFS                                  (0x0000003c)
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_RMSK                                      0xffff
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_POR                                   0x00000000
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_IN(x)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_DATA_IN_ADDR(x), HAL_SSUSB_CR_PROTOCOL_DATA_IN_RMSK)
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_INM(x, m)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_DATA_IN_ADDR(x), m)
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_OUT(x, v)      \
        out_dword(HAL_SSUSB_CR_PROTOCOL_DATA_IN_ADDR(x),v)
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_OUTM(x,m,v) \
        out_dword_masked_ns(HAL_SSUSB_CR_PROTOCOL_DATA_IN_ADDR(x),m,v,HAL_SSUSB_CR_PROTOCOL_DATA_IN_IN(x))
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_SS_CR_DATA_IN_REG_BMSK                    0xffff
#define HAL_SSUSB_CR_PROTOCOL_DATA_IN_SS_CR_DATA_IN_REG_SHFT                       0x0

#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_ADDR(x)                              ((x) + 0x00000040)
#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_PHYS(x)                              ((x) + 0x00000040)
#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_OFFS                                 (0x00000040)
#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_RMSK                                     0xffff
#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_POR                                  0x00000000
#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_IN(x)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_DATA_OUT_ADDR(x), HAL_SSUSB_CR_PROTOCOL_DATA_OUT_RMSK)
#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_INM(x, m)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_DATA_OUT_ADDR(x), m)
#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_SS_CR_DATA_OUT_REG_BMSK                  0xffff
#define HAL_SSUSB_CR_PROTOCOL_DATA_OUT_SS_CR_DATA_OUT_REG_SHFT                     0x0

#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(x)                              ((x) + 0x00000044)
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_PHYS(x)                              ((x) + 0x00000044)
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_OFFS                                 (0x00000044)
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_RMSK                                        0x1
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_POR                                  0x00000000
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_IN(x)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(x), HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_RMSK)
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_INM(x, m)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(x), m)
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_OUT(x, v)      \
        out_dword(HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(x),v)
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_OUTM(x,m,v) \
        out_dword_masked_ns(HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_ADDR(x),m,v,HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_IN(x))
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_SS_CR_CAP_ADDR_REG_BMSK                     0x1
#define HAL_SSUSB_CR_PROTOCOL_CAP_ADDR_SS_CR_CAP_ADDR_REG_SHFT                     0x0

#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_ADDR(x)                              ((x) + 0x00000048)
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_PHYS(x)                              ((x) + 0x00000048)
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_OFFS                                 (0x00000048)
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_RMSK                                        0x1
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_POR                                  0x00000000
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_IN(x)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_CAP_DATA_ADDR(x), HAL_SSUSB_CR_PROTOCOL_CAP_DATA_RMSK)
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_INM(x, m)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_CAP_DATA_ADDR(x), m)
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_OUT(x, v)      \
        out_dword(HAL_SSUSB_CR_PROTOCOL_CAP_DATA_ADDR(x),v)
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_OUTM(x,m,v) \
        out_dword_masked_ns(HAL_SSUSB_CR_PROTOCOL_CAP_DATA_ADDR(x),m,v,HAL_SSUSB_CR_PROTOCOL_CAP_DATA_IN(x))
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_SS_CR_CAP_DATA_REG_BMSK                     0x1
#define HAL_SSUSB_CR_PROTOCOL_CAP_DATA_SS_CR_CAP_DATA_REG_SHFT                     0x0

#define HAL_SSUSB_CR_PROTOCOL_READ_ADDR(x)                                  ((x) + 0x0000004c)
#define HAL_SSUSB_CR_PROTOCOL_READ_PHYS(x)                                  ((x) + 0x0000004c)
#define HAL_SSUSB_CR_PROTOCOL_READ_OFFS                                     (0x0000004c)
#define HAL_SSUSB_CR_PROTOCOL_READ_RMSK                                            0x1
#define HAL_SSUSB_CR_PROTOCOL_READ_POR                                      0x00000000
#define HAL_SSUSB_CR_PROTOCOL_READ_IN(x)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_READ_ADDR(x), HAL_SSUSB_CR_PROTOCOL_READ_RMSK)
#define HAL_SSUSB_CR_PROTOCOL_READ_INM(x, m)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_READ_ADDR(x), m)
#define HAL_SSUSB_CR_PROTOCOL_READ_OUT(x, v)      \
        out_dword(HAL_SSUSB_CR_PROTOCOL_READ_ADDR(x),v)
#define HAL_SSUSB_CR_PROTOCOL_READ_OUTM(x,m,v) \
        out_dword_masked_ns(HAL_SSUSB_CR_PROTOCOL_READ_ADDR(x),m,v,HAL_SSUSB_CR_PROTOCOL_READ_IN(x))
#define HAL_SSUSB_CR_PROTOCOL_READ_SS_CR_READ_REG_BMSK                             0x1
#define HAL_SSUSB_CR_PROTOCOL_READ_SS_CR_READ_REG_SHFT                             0x0

#define HAL_SSUSB_CR_PROTOCOL_WRITE_ADDR(x)                                 ((x) + 0x00000050)
#define HAL_SSUSB_CR_PROTOCOL_WRITE_PHYS(x)                                 ((x) + 0x00000050)
#define HAL_SSUSB_CR_PROTOCOL_WRITE_OFFS                                    (0x00000050)
#define HAL_SSUSB_CR_PROTOCOL_WRITE_RMSK                                           0x1
#define HAL_SSUSB_CR_PROTOCOL_WRITE_POR                                     0x00000000
#define HAL_SSUSB_CR_PROTOCOL_WRITE_IN(x)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_WRITE_ADDR(x), HAL_SSUSB_CR_PROTOCOL_WRITE_RMSK)
#define HAL_SSUSB_CR_PROTOCOL_WRITE_INM(x, m)      \
        in_dword_masked(HAL_SSUSB_CR_PROTOCOL_WRITE_ADDR(x), m)
#define HAL_SSUSB_CR_PROTOCOL_WRITE_OUT(x, v)      \
        out_dword(HAL_SSUSB_CR_PROTOCOL_WRITE_ADDR(x),v)
#define HAL_SSUSB_CR_PROTOCOL_WRITE_OUTM(x,m,v) \
        out_dword_masked_ns(HAL_SSUSB_CR_PROTOCOL_WRITE_ADDR(x),m,v,HAL_SSUSB_CR_PROTOCOL_WRITE_IN(x))
#define HAL_SSUSB_CR_PROTOCOL_WRITE_SS_CR_WRITE_REG_BMSK                           0x1
#define HAL_SSUSB_CR_PROTOCOL_WRITE_SS_CR_WRITE_REG_SHFT                           0x0

#endif /* HAL_SSUSB */

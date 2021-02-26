#ifndef __USLEEP_HALHWIO_H__
#define __USLEEP_HALHWIO_H__
/*
===========================================================================
*/
/**
  @file uSleep_HALhwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p2q1r89]
 
  This file contains HWIO register definitions for the following modules:
    SSC_QDSP6V65SS_CSR
    SSC_QDSP6V65SS_PUB
    SCC_MCC_SSC_REG

  'Include' filters applied: 
  'Exclude' filters applied: 
*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/hwio/670/uSleep_HALhwio.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6V65SS_CSR
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6V65SS_CSR_REG_BASE                                                      (LPASS_BASE      + 0x00480000)

#define HWIO_SSC_QDSP6SS_INSTANCE_ID_ADDR                                                (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00000000)
#define HWIO_SSC_QDSP6SS_INSTANCE_ID_RMSK                                                       0x7
#define HWIO_SSC_QDSP6SS_INSTANCE_ID_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INSTANCE_ID_ADDR, HWIO_SSC_QDSP6SS_INSTANCE_ID_RMSK)
#define HWIO_SSC_QDSP6SS_INSTANCE_ID_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INSTANCE_ID_ADDR, m)
#define HWIO_SSC_QDSP6SS_INSTANCE_ID_ID_BMSK                                                    0x7
#define HWIO_SSC_QDSP6SS_INSTANCE_ID_ID_SHFT                                                    0x0

#define HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_ADDR                                            (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00000100)
#define HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_RMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_ADDR, HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_HVX_EN_BMSK                                            0x1
#define HWIO_SSC_QDSP6SS_CP_EFUSE_STATUS_HVX_EN_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_WDOG_RESET_ADDR                                                 (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00004000)
#define HWIO_SSC_QDSP6SS_WDOG_RESET_RMSK                                                        0x1
#define HWIO_SSC_QDSP6SS_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WDOG_RESET_ADDR,v)
#define HWIO_SSC_QDSP6SS_WDOG_RESET_WDOG_RESET_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_WDOG_CTL_ADDR                                                   (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00004004)
#define HWIO_SSC_QDSP6SS_WDOG_CTL_RMSK                                                          0x7
#define HWIO_SSC_QDSP6SS_WDOG_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_CTL_ADDR, HWIO_SSC_QDSP6SS_WDOG_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_WDOG_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WDOG_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_WDOG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_WDOG_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_WDOG_CTL_IN)
#define HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK                                           0x4
#define HWIO_SSC_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT                                           0x2
#define HWIO_SSC_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_BMSK                                          0x2
#define HWIO_SSC_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_SHFT                                          0x1
#define HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_BMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_WDOG_CTL_ENABLE_SHFT                                                   0x0

#define HWIO_SSC_QDSP6SS_WDOG_STATUS_ADDR                                                (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00004008)
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_RMSK                                                0xf81fffff
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_STATUS_ADDR, HWIO_SSC_QDSP6SS_WDOG_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_BMSK                              0x80000000
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_SHFT                                    0x1f
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_BMSK                          0x40000000
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_SHFT                                0x1e
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_BMSK                          0x20000000
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_SHFT                                0x1d
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_BMSK                           0x10000000
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_SHFT                                 0x1c
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_BMSK                           0x8000000
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_SHFT                                0x1b
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_WDOG_COUNT_BMSK                                       0x1ffffe
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_WDOG_COUNT_SHFT                                            0x1
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_ADDR                                             (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000400c)
#define HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_RMSK                                                0xfffff
#define HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_ADDR, HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_RMSK)
#define HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_ADDR, m)
#define HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_ADDR,v)
#define HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_ADDR,m,v,HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_IN)
#define HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK                                           0xfffff
#define HWIO_SSC_QDSP6SS_WDOG_BARK_TIME_DATA_SHFT                                               0x0

#define HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_ADDR                                             (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00004010)
#define HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_RMSK                                                0xfffff
#define HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_ADDR, HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_RMSK)
#define HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_ADDR, m)
#define HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_ADDR,v)
#define HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_ADDR,m,v,HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_IN)
#define HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_DATA_BMSK                                           0xfffff
#define HWIO_SSC_QDSP6SS_WDOG_BITE_TIME_DATA_SHFT                                               0x0

#define HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_ADDR                                              (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00004014)
#define HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_RMSK                                                 0xfffff
#define HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_ADDR, HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_RMSK)
#define HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_ADDR, m)
#define HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_ADDR,v)
#define HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_ADDR,m,v,HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_IN)
#define HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_DATA_BMSK                                            0xfffff
#define HWIO_SSC_QDSP6SS_WDOG_NMI_TIME_DATA_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_WDOG_TEST_LOAD_ADDR                                             (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00004018)
#define HWIO_SSC_QDSP6SS_WDOG_TEST_LOAD_RMSK                                                    0x1
#define HWIO_SSC_QDSP6SS_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_SSC_QDSP6SS_WDOG_TEST_LOAD_LOAD_BMSK                                               0x1
#define HWIO_SSC_QDSP6SS_WDOG_TEST_LOAD_LOAD_SHFT                                               0x0

#define HWIO_SSC_QDSP6SS_WDOG_TEST_ADDR                                                  (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000401c)
#define HWIO_SSC_QDSP6SS_WDOG_TEST_RMSK                                                    0x1fffff
#define HWIO_SSC_QDSP6SS_WDOG_TEST_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_TEST_ADDR, HWIO_SSC_QDSP6SS_WDOG_TEST_RMSK)
#define HWIO_SSC_QDSP6SS_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_WDOG_TEST_ADDR, m)
#define HWIO_SSC_QDSP6SS_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WDOG_TEST_ADDR,v)
#define HWIO_SSC_QDSP6SS_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_WDOG_TEST_ADDR,m,v,HWIO_SSC_QDSP6SS_WDOG_TEST_IN)
#define HWIO_SSC_QDSP6SS_WDOG_TEST_SYNC_STATUS_BMSK                                        0x100000
#define HWIO_SSC_QDSP6SS_WDOG_TEST_SYNC_STATUS_SHFT                                            0x14
#define HWIO_SSC_QDSP6SS_WDOG_TEST_LOAD_VALUE_BMSK                                          0xfffff
#define HWIO_SSC_QDSP6SS_WDOG_TEST_LOAD_VALUE_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_ADDR                                            (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00004100)
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_RMSK                                                   0xf
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_ADDR, HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_IN)
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_MAX_COUNT_BMSK                                         0xe
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_MAX_COUNT_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_DISABLE_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_GLBCNT_TRIG_CFG_DISABLE_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_TS_m_EN_ADDR(m)                                                 (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00005000 + 0x10 * (m))
#define HWIO_SSC_QDSP6SS_TS_m_EN_RMSK                                                           0x7
#define HWIO_SSC_QDSP6SS_TS_m_EN_MAXm                                                             3
#define HWIO_SSC_QDSP6SS_TS_m_EN_INI(m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_m_EN_ADDR(m), HWIO_SSC_QDSP6SS_TS_m_EN_RMSK)
#define HWIO_SSC_QDSP6SS_TS_m_EN_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_m_EN_ADDR(m), mask)
#define HWIO_SSC_QDSP6SS_TS_m_EN_OUTI(m,val)    \
        out_dword(HWIO_SSC_QDSP6SS_TS_m_EN_ADDR(m),val)
#define HWIO_SSC_QDSP6SS_TS_m_EN_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_TS_m_EN_ADDR(m),mask,val,HWIO_SSC_QDSP6SS_TS_m_EN_INI(m))
#define HWIO_SSC_QDSP6SS_TS_m_EN_MODE_BMSK                                                      0x4
#define HWIO_SSC_QDSP6SS_TS_m_EN_MODE_SHFT                                                      0x2
#define HWIO_SSC_QDSP6SS_TS_m_EN_CLEAR_BMSK                                                     0x2
#define HWIO_SSC_QDSP6SS_TS_m_EN_CLEAR_SHFT                                                     0x1
#define HWIO_SSC_QDSP6SS_TS_m_EN_EN_BMSK                                                        0x1
#define HWIO_SSC_QDSP6SS_TS_m_EN_EN_SHFT                                                        0x0

#define HWIO_SSC_QDSP6SS_TS_m_STATUS_ADDR(m)                                             (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00005004 + 0x10 * (m))
#define HWIO_SSC_QDSP6SS_TS_m_STATUS_RMSK                                                     0x101
#define HWIO_SSC_QDSP6SS_TS_m_STATUS_MAXm                                                         3
#define HWIO_SSC_QDSP6SS_TS_m_STATUS_INI(m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_m_STATUS_ADDR(m), HWIO_SSC_QDSP6SS_TS_m_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_TS_m_STATUS_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_m_STATUS_ADDR(m), mask)
#define HWIO_SSC_QDSP6SS_TS_m_STATUS_OVERFLOW_BMSK                                            0x100
#define HWIO_SSC_QDSP6SS_TS_m_STATUS_OVERFLOW_SHFT                                              0x8
#define HWIO_SSC_QDSP6SS_TS_m_STATUS_VALID_BMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_TS_m_STATUS_VALID_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m)                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00005008 + 0x10 * (m))
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_RMSK                                           0xffffffff
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_MAXm                                                    3
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_INI(m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m), HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_RMSK)
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m), mask)
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_DATA_L_BMSK                                    0xffffffff
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_L_DATA_L_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m)                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000500c + 0x10 * (m))
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_RMSK                                             0xffffff
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_MAXm                                                    3
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_INI(m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m), HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_RMSK)
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m), mask)
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_DATA_H_BMSK                                      0xffffff
#define HWIO_SSC_QDSP6SS_TS_m_TIMESTAMP_H_DATA_H_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_TS_HWE_SEL_ADDR                                                 (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00005100)
#define HWIO_SSC_QDSP6SS_TS_HWE_SEL_RMSK                                                       0x1f
#define HWIO_SSC_QDSP6SS_TS_HWE_SEL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_HWE_SEL_ADDR, HWIO_SSC_QDSP6SS_TS_HWE_SEL_RMSK)
#define HWIO_SSC_QDSP6SS_TS_HWE_SEL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TS_HWE_SEL_ADDR, m)
#define HWIO_SSC_QDSP6SS_TS_HWE_SEL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_TS_HWE_SEL_ADDR,v)
#define HWIO_SSC_QDSP6SS_TS_HWE_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_TS_HWE_SEL_ADDR,m,v,HWIO_SSC_QDSP6SS_TS_HWE_SEL_IN)
#define HWIO_SSC_QDSP6SS_TS_HWE_SEL_MUX_SEL_BMSK                                               0x1f
#define HWIO_SSC_QDSP6SS_TS_HWE_SEL_MUX_SEL_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_IPC_ADDR                                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00008000)
#define HWIO_SSC_QDSP6SS_IPC_RMSK                                                        0xffffffff
#define HWIO_SSC_QDSP6SS_IPC_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_IPC_ADDR,v)
#define HWIO_SSC_QDSP6SS_IPC_INTR_BMSK                                                   0xffffffff
#define HWIO_SSC_QDSP6SS_IPC_INTR_SHFT                                                          0x0

#define HWIO_SSC_QDSP6SS_IPC1_ADDR                                                       (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x00008004)
#define HWIO_SSC_QDSP6SS_IPC1_RMSK                                                       0xffffffff
#define HWIO_SSC_QDSP6SS_IPC1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_IPC1_ADDR,v)
#define HWIO_SSC_QDSP6SS_IPC1_INTR_BMSK                                                  0xffffffff
#define HWIO_SSC_QDSP6SS_IPC1_INTR_SHFT                                                         0x0

#define HWIO_SSC_QDSP6SS_BLOCK_INTR_ADDR                                                 (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c008)
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_RMSK                                                        0x7
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_BLOCK_INTR_ADDR, HWIO_SSC_QDSP6SS_BLOCK_INTR_RMSK)
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_BLOCK_INTR_ADDR, m)
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_BLOCK_INTR_ADDR,v)
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_BLOCK_INTR_ADDR,m,v,HWIO_SSC_QDSP6SS_BLOCK_INTR_IN)
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_CLEAR_BLOCK_BMSK                                            0x4
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_CLEAR_BLOCK_SHFT                                            0x2
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_BLOCK_STATUS_BMSK                                           0x2
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_BLOCK_STATUS_SHFT                                           0x1
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_BLOCK_INTR_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_BLOCK_INTR_BLOCK_INTR_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_SLPC_CFG_ADDR                                                   (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c01c)
#define HWIO_SSC_QDSP6SS_SLPC_CFG_RMSK                                                       0xffa0
#define HWIO_SSC_QDSP6SS_SLPC_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_SLPC_CFG_ADDR, HWIO_SSC_QDSP6SS_SLPC_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_SLPC_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_SLPC_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_SLPC_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_SLPC_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_SLPC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_SLPC_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_SLPC_CFG_IN)
#define HWIO_SSC_QDSP6SS_SLPC_CFG_STAGGER_LKAD_DIS_BMSK                                      0x8000
#define HWIO_SSC_QDSP6SS_SLPC_CFG_STAGGER_LKAD_DIS_SHFT                                         0xf
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_FAST_STAGGER_BMSK                                      0x4000
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_FAST_STAGGER_SHFT                                         0xe
#define HWIO_SSC_QDSP6SS_SLPC_CFG_IDLE_CORE_CLK_EN_BMSK                                      0x2000
#define HWIO_SSC_QDSP6SS_SLPC_CFG_IDLE_CORE_CLK_EN_SHFT                                         0xd
#define HWIO_SSC_QDSP6SS_SLPC_CFG_EXTHW_WAKE_EN_BMSK                                         0x1000
#define HWIO_SSC_QDSP6SS_SLPC_CFG_EXTHW_WAKE_EN_SHFT                                            0xc
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_PU_PERI_STAGGER_BMSK                                    0x800
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_PU_PERI_STAGGER_SHFT                                      0xb
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_PD_PERI_STAGGER_BMSK                                    0x400
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_PD_PERI_STAGGER_SHFT                                      0xa
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_PU_ARRY_STAGGER_BMSK                                    0x200
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_PU_ARRY_STAGGER_SHFT                                      0x9
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_PD_ARRY_STAGGER_BMSK                                    0x100
#define HWIO_SSC_QDSP6SS_SLPC_CFG_MEM_PD_ARRY_STAGGER_SHFT                                      0x8
#define HWIO_SSC_QDSP6SS_SLPC_CFG_WAKEUP_IN_EN_BMSK                                            0x80
#define HWIO_SSC_QDSP6SS_SLPC_CFG_WAKEUP_IN_EN_SHFT                                             0x7
#define HWIO_SSC_QDSP6SS_SLPC_CFG_CLK_GATING_MODE_BMSK                                         0x20
#define HWIO_SSC_QDSP6SS_SLPC_CFG_CLK_GATING_MODE_SHFT                                          0x5

#define HWIO_SSC_QDSP6SS_RSCCTL_BR_ADDR                                                  (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c020)
#define HWIO_SSC_QDSP6SS_RSCCTL_BR_RMSK                                                         0xe
#define HWIO_SSC_QDSP6SS_RSCCTL_BR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_BR_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_BR_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_BR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_BR_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_BR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_BR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_BR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_BR_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_BR_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_BR_EVENT_BMSK                                                   0xe
#define HWIO_SSC_QDSP6SS_RSCCTL_BR_EVENT_SHFT                                                   0x1

#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_ADDR                                                 (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c024)
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_RMSK                                                        0x3
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_WAKEUP_CTL_ADDR, HWIO_SSC_QDSP6SS_WAKEUP_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_WAKEUP_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WAKEUP_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_WAKEUP_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_WAKEUP_CTL_IN)
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_NMI_WAKE_EN_BMSK                                            0x2
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_NMI_WAKE_EN_SHFT                                            0x1
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_L2VIC_WAKE_EN_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_WAKEUP_CTL_L2VIC_WAKE_EN_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_ADDR                                            (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c028)
#define HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_RMSK                                                  0x1c
#define HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_ADDR, HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_RMSK)
#define HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_ADDR, m)
#define HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_ADDR,v)
#define HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_ADDR,m,v,HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_IN)
#define HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_WAKEUP_EN_BMSK                                        0x1c
#define HWIO_SSC_QDSP6SS_L2VIC_WAKEUP_EN_WAKEUP_EN_SHFT                                         0x2

#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_ADDR                                              (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c02c)
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_RMSK                                                    0x1f
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_WAKEUP_STATUS_ADDR, HWIO_SSC_QDSP6SS_WAKEUP_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_WAKEUP_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_WAKEUP_STATUS_ADDR,v)
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_WAKEUP_STATUS_ADDR,m,v,HWIO_SSC_QDSP6SS_WAKEUP_STATUS_IN)
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_STICKY_BMSK                                 0x10
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_STICKY_SHFT                                  0x4
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_BMSK                                         0x8
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_SHFT                                         0x3
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_DBG_NMI_BMSK                                             0x4
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_DBG_NMI_SHFT                                             0x2
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_EXT_TRIGGER_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_EXT_TRIGGER_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_L2VIC_WAKEUP_BMSK                                        0x1
#define HWIO_SSC_QDSP6SS_WAKEUP_STATUS_L2VIC_WAKEUP_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR                                      (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c054)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_RMSK                                             0x7
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR, HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_RMSK)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR,m,v,HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_IN)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM2_HALTACK_OVRRD_BMSK                         0x4
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM2_HALTACK_OVRRD_SHFT                         0x2
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM_HALTACK_OVRRD_BMSK                          0x2
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM_HALTACK_OVRRD_SHFT                          0x1
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ALL_HALTACK_OVRRD_BMSK                           0x1
#define HWIO_SSC_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ALL_HALTACK_OVRRD_SHFT                           0x0

#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR                                         (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c058)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_RMSK                                             0x3fff
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR, HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_RMSK)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR,m,v,HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_IN)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_SPARE_OVRRD_BMSK                                 0x3c00
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_SPARE_OVRRD_SHFT                                    0xa
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_TCS_HW_ACK_OVRRD_BMSK                             0x3fc
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_TCS_HW_ACK_OVRRD_SHFT                               0x2
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_BRINGUP_ACK_OVRRD_BMSK                              0x2
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_BRINGUP_ACK_OVRRD_SHFT                              0x1
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_SHUTDOWN_ACK_OVRRD_BMSK                             0x1
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PRSC_OVR_SHUTDOWN_ACK_OVRRD_SHFT                             0x0

#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c05c)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_RMSK                                                 0x3
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR, HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_RMSK)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR,m,v,HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_IN)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_FINE_OVRRD_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_FINE_OVRRD_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_OVRRD_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_OVRRD_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c060)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_RMSK                                                0x1f
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR, HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_RMSK)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR,m,v,HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_IN)
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_REFRESH_BMSK                                        0x10
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_REFRESH_SHFT                                         0x4
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_PD_BMSK                                       0x8
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_PD_SHFT                                       0x3
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_READY_BMSK                                    0x4
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_READY_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_PD_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_PD_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_READY_BMSK                                      0x1
#define HWIO_SSC_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_READY_SHFT                                      0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ADDR                                             (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c100)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_RMSK                                                 0x1fff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_APB_BMSK                                             0x1000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_APB_SHFT                                                0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AXIS2_BMSK                                            0x800
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AXIS2_SHFT                                              0xb
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CSI1_BMSK                                             0x400
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CSI1_SHFT                                               0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CSI0_BMSK                                             0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CSI0_SHFT                                               0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ARCH_BMSK                                             0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ARCH_SHFT                                               0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CORE_BMSK                                              0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CORE_SHFT                                               0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AXIM2_BMSK                                             0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AXIM2_SHFT                                              0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AXIS_BMSK                                              0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AXIS_SHFT                                               0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ATBM_BMSK                                              0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ATBM_SHFT                                               0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AHBM_BMSK                                               0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AHBM_SHFT                                               0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AXIM_BMSK                                               0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_AXIM_SHFT                                               0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ISDB_ETM_BMSK                                           0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_ISDB_ETM_SHFT                                           0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_RET_BMSK                                                0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_RET_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_ADDR                                            (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c104)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_RMSK                                                   0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_MSS2CX_BMSK                                      0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_MSS2CX_SHFT                                      0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_IO_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_IO_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_ADDR                                              (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c108)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_RMSK                                                   0x3ff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AXIS2_BMSK                                             0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AXIS2_SHFT                                               0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CSI1_BMSK                                              0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CSI1_SHFT                                                0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CSI0_BMSK                                               0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CSI0_SHFT                                                0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AXIM2_BMSK                                              0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AXIM2_SHFT                                               0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AXIS_BMSK                                               0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AXIS_SHFT                                                0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_ATBM_BMSK                                               0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_ATBM_SHFT                                                0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AHBM_BMSK                                                0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AHBM_SHFT                                                0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AXIM_BMSK                                                0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_AXIM_SHFT                                                0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_APB_BMSK                                                 0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_APB_SHFT                                                 0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CORE_BMSK                                                0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CORE_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c10c)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_RMSK                                             0xfffff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L2CLADE_BMSK                                     0xc0000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L2CLADE_SHFT                                        0x12
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L2TAG_BMSK                                       0x3c000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L2TAG_SHFT                                           0xe
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_BMSK                                         0x3000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_SHFT                                            0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BMSK                                          0xc00
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_SHFT                                            0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_TAG_BMSK                                      0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_TAG_SHFT                                        0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_JU_BMSK                                            0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_JU_SHFT                                              0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L2PLRU_BMSK                                         0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L2PLRU_SHFT                                          0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_ETB_BMSK                                            0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_ETB_SHFT                                             0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L2STBUF_BMSK                                        0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L2STBUF_SHFT                                         0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BTB_BMSK                                       0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BTB_SHFT                                        0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_TAG_BMSK                                        0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_TAG_SHFT                                        0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_PDEC_BMSK                                       0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_PDEC_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c110)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_RMSK                                                 0xe
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_QMC_MEM_BMSK                                   0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_QMC_MEM_SHFT                                   0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_WL_BMSK                                        0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_WL_SHFT                                        0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_MEM_PERIPH_BMSK                                      0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_MEM_PERIPH_SHFT                                      0x1

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_ADDR                                              (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c114)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_RMSK                                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_BHS_BMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_BHS_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_ADDR                                              (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c118)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_RMSK                                                    0x7f
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_BG_OFF_BMSK                                             0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_BG_OFF_SHFT                                              0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_FORCEOFF_BMSK                                           0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_FORCEOFF_SHFT                                            0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SPARE_BITS4_2_BMSK                                      0x1c
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SPARE_BITS4_2_SHFT                                       0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_VREF_SEL_UPDATE_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_VREF_SEL_UPDATE_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_BYPASS_OPEN_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_BYPASS_OPEN_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_ADDR                                              (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c11c)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_RMSK                                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_XO_ENABLE_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_XO_ENABLE_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_ADDR                                              (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c120)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_RMSK                                                    0x17
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_PLL_STBY_BMSK                                           0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_PLL_STBY_SHFT                                            0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_PLL_OUTDIS_BMSK                                          0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_PLL_OUTDIS_SHFT                                          0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_PLL_OPMODE1_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_PLL_OPMODE1_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_PLL_RESET_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_PLL_RESET_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_ADDR                                         (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c124)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_RMSK                                                0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_WAKE_IRQ_BMSK                                       0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_WAKE_IRQ_SHFT                                       0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_RESTORE_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_RESTORE_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SAVE_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SAVE_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR                                      (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c128)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_RMSK                                            0x3d
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_FIFO_ARESET_BMSK                          0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_FIFO_ARESET_SHFT                           0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_HALTREQ_BMSK                              0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_HALTREQ_SHFT                               0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_FIFO_ARESET_BMSK                            0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_FIFO_ARESET_SHFT                            0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_HALTREQ_BMSK                                0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_HALTREQ_SHFT                                0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_BUSM_HALTREQ_BMSK                                0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_BUSM_HALTREQ_SHFT                                0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR                                    (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c12c)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_RMSK                                          0xff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_CLEAR_BMSK                      0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_CLEAR_SHFT                       0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_SET_BMSK                        0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_SET_SHFT                         0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_CLEAR_BMSK                      0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_CLEAR_SHFT                       0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_SET_BMSK                        0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_SET_SHFT                         0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_CLEAR_BMSK                        0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_CLEAR_SHFT                        0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_SET_BMSK                          0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_SET_SHFT                          0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_CLEAR_BMSK                        0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_CLEAR_SHFT                        0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_SET_BMSK                          0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_SET_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR                                      (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c130)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_RMSK                                             0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM2_BMSK                                       0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM2_SHFT                                       0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CORE_BMSK                                        0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CORE_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c134)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_RMSK                                               0xf
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_L2DATA_BMSK                                        0xf
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_L2DATA_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_ADDR                                             (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c138)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_RMSK                                                 0x3fff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_ADDR, HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_RMSK)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_IN)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SPARE_BMSK                                           0x3c00
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SPARE_SHFT                                              0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_TCS_HW_TRIGGER_BMSK                                   0x3fc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_TCS_HW_TRIGGER_SHFT                                     0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_BRINGUP_REQ_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_BRINGUP_REQ_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SHUTDOWN_REQ_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SHUTDOWN_REQ_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_ADDR                                         (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c180)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_RMSK                                             0x1fff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_APB_BMSK                                         0x1000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_APB_SHFT                                            0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS2_BMSK                                        0x800
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS2_SHFT                                          0xb
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_CSI1_BMSK                                         0x400
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_CSI1_SHFT                                           0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_CSI0_BMSK                                         0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_CSI0_SHFT                                           0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_ARCH_BMSK                                         0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_ARCH_SHFT                                           0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_CORE_BMSK                                          0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_CORE_SHFT                                           0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM2_BMSK                                         0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM2_SHFT                                          0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS_BMSK                                          0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS_SHFT                                           0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_ATBM_BMSK                                          0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_ATBM_SHFT                                           0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AHBM_BMSK                                           0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AHBM_SHFT                                           0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM_BMSK                                           0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM_SHFT                                           0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_ISDB_ETM_BMSK                                       0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_ISDB_ETM_SHFT                                       0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_RET_BMSK                                            0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_SET_RET_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_SET_ADDR                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c184)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_SET_RMSK                                               0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_MSS2CX_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_MSS2CX_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_IO_BMSK                                      0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_IO_SHFT                                      0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c188)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_RMSK                                               0x3ff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS2_BMSK                                         0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS2_SHFT                                           0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_CSI1_BMSK                                          0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_CSI1_SHFT                                            0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_CSI0_BMSK                                           0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_CSI0_SHFT                                            0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM2_BMSK                                          0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM2_SHFT                                           0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS_BMSK                                           0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS_SHFT                                            0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_ATBM_BMSK                                           0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_ATBM_SHFT                                            0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AHBM_BMSK                                            0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AHBM_SHFT                                            0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM_BMSK                                            0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM_SHFT                                            0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_APB_BMSK                                             0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_APB_SHFT                                             0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_CORE_BMSK                                            0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_SET_CORE_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ADDR                                      (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c18c)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_RMSK                                         0xfffff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2CLADE_BMSK                                 0xc0000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2CLADE_SHFT                                    0x12
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2TAG_BMSK                                   0x3c000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2TAG_SHFT                                       0xe
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_BMSK                                     0x3000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_SHFT                                        0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BMSK                                      0xc00
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_SHFT                                        0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_TAG_BMSK                                  0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_TAG_SHFT                                    0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_BMSK                                        0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_SHFT                                          0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2PLRU_BMSK                                     0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2PLRU_SHFT                                      0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ETB_BMSK                                        0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ETB_SHFT                                         0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2STBUF_BMSK                                    0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2STBUF_SHFT                                     0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BTB_BMSK                                   0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BTB_SHFT                                    0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_TAG_BMSK                                    0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_TAG_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_PDEC_BMSK                                   0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_PDEC_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_ADDR                                      (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c190)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_RMSK                                             0xe
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_QMC_MEM_BMSK                               0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_QMC_MEM_SHFT                               0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_WL_BMSK                                    0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_WL_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_MEM_PERIPH_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_MEM_PERIPH_SHFT                                  0x1

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_SET_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c194)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_SET_RMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_SET_BHS_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_SET_BHS_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c198)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_RMSK                                                0x7f
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_BG_OFF_BMSK                                         0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_BG_OFF_SHFT                                          0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_FORCEOFF_BMSK                                       0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_FORCEOFF_SHFT                                        0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_SPARE_BITS4_2_BMSK                                  0x1c
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_SPARE_BITS4_2_SHFT                                   0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_UPDATE_BMSK                                          0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_UPDATE_SHFT                                          0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_BYPASS_OPEN_BMSK                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_SET_BYPASS_OPEN_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_SET_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c19c)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_SET_RMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_SET_XO_ENABLE_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_SET_XO_ENABLE_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_ADDR                                          (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1a0)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_RMSK                                                0x17
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_STBY_BMSK                                       0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_STBY_SHFT                                        0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OUTDIS_BMSK                                      0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OUTDIS_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OPMODE1_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OPMODE1_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_RESET_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_RESET_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_ADDR                                     (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1a4)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_RMSK                                            0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_WAKE_IRQ_BMSK                                   0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_WAKE_IRQ_SHFT                                   0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_RESTORE_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_RESTORE_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_SAVE_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_SET_SAVE_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_ADDR                                  (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1a8)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_RMSK                                        0x3d
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_FIFO_ARESET_BMSK                      0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_FIFO_ARESET_SHFT                       0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_HALTREQ_BMSK                          0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_HALTREQ_SHFT                           0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_FIFO_ARESET_BMSK                        0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_FIFO_ARESET_SHFT                        0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_HALTREQ_BMSK                            0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_HALTREQ_SHFT                            0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_BUSM_HALTREQ_BMSK                            0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_BUSM_HALTREQ_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_ADDR                                (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1ac)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_RMSK                                      0xff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_CLEAR_BMSK                  0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_CLEAR_SHFT                   0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_SET_BMSK                    0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_SET_SHFT                     0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_CLEAR_BMSK                  0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_CLEAR_SHFT                   0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_SET_BMSK                    0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_SET_SHFT                     0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_CLEAR_BMSK                    0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_CLEAR_SHFT                    0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_SET_BMSK                      0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_SET_SHFT                      0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_CLEAR_BMSK                    0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_CLEAR_SHFT                    0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_SET_BMSK                      0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_SET_SHFT                      0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_ADDR                                  (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1b0)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_RMSK                                         0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM2_BMSK                                   0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM2_SHFT                                   0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_CORE_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_CORE_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_ADDR                                    (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1b4)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_RMSK                                           0xf
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_L2DATA_BMSK                                    0xf
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_L2DATA_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_ADDR                                         (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1b8)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_RMSK                                             0x3fff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_SPARE_BMSK                                       0x3c00
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_SPARE_SHFT                                          0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_TCS_HW_TRIGGER_BMSK                               0x3fc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_TCS_HW_TRIGGER_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_BRINGUP_REQ_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_BRINGUP_REQ_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_SHUTDOWN_REQ_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_SET_SHUTDOWN_REQ_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ADDR                                       (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c200)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RMSK                                           0x1fff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_APB_BMSK                                       0x1000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_APB_SHFT                                          0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS2_BMSK                                      0x800
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS2_SHFT                                        0xb
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI1_BMSK                                       0x400
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI1_SHFT                                         0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI0_BMSK                                       0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI0_SHFT                                         0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ARCH_BMSK                                       0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ARCH_SHFT                                         0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CORE_BMSK                                        0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CORE_SHFT                                         0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM2_BMSK                                       0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM2_SHFT                                        0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS_BMSK                                        0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS_SHFT                                         0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ATBM_BMSK                                        0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ATBM_SHFT                                         0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AHBM_BMSK                                         0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AHBM_SHFT                                         0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM_BMSK                                         0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM_SHFT                                         0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ISDB_ETM_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ISDB_ETM_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RET_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RET_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_ADDR                                      (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c204)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_RMSK                                             0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_MSS2CX_BMSK                                0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_MSS2CX_SHFT                                0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_IO_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_IO_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ADDR                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c208)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_RMSK                                             0x3ff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS2_BMSK                                       0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS2_SHFT                                         0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI1_BMSK                                        0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI1_SHFT                                          0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI0_BMSK                                         0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI0_SHFT                                          0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM2_BMSK                                        0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM2_SHFT                                         0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS_BMSK                                         0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS_SHFT                                          0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ATBM_BMSK                                         0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ATBM_SHFT                                          0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AHBM_BMSK                                          0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AHBM_SHFT                                          0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM_BMSK                                          0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM_SHFT                                          0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_APB_BMSK                                           0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_APB_SHFT                                           0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CORE_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CORE_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ADDR                                    (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c20c)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_RMSK                                       0xfffff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2CLADE_BMSK                               0xc0000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2CLADE_SHFT                                  0x12
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2TAG_BMSK                                 0x3c000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2TAG_SHFT                                     0xe
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_BMSK                                   0x3000
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_SHFT                                      0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BMSK                                    0xc00
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_SHFT                                      0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_TAG_BMSK                                0x200
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_TAG_SHFT                                  0x9
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_BMSK                                      0x100
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_SHFT                                        0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2PLRU_BMSK                                   0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2PLRU_SHFT                                    0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ETB_BMSK                                      0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ETB_SHFT                                       0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2STBUF_BMSK                                  0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2STBUF_SHFT                                   0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BTB_BMSK                                 0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BTB_SHFT                                  0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_TAG_BMSK                                  0xc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_TAG_SHFT                                  0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_PDEC_BMSK                                 0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_PDEC_SHFT                                 0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_ADDR                                    (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c210)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_RMSK                                           0xe
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_QMC_MEM_BMSK                             0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_QMC_MEM_SHFT                             0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_WL_BMSK                                  0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_WL_SHFT                                  0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_MEM_PERIPH_BMSK                                0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_MEM_PERIPH_SHFT                                0x1

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_CLEAR_ADDR                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c214)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_CLEAR_RMSK                                               0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_CLEAR_BHS_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_BHS_CLEAR_BHS_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_ADDR                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c218)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_RMSK                                              0x7f
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BG_OFF_BMSK                                       0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BG_OFF_SHFT                                        0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_FORCEOFF_BMSK                                     0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_FORCEOFF_SHFT                                      0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_SPARE_BITS4_2_BMSK                                0x1c
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_SPARE_BITS4_2_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_UPDATE_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_UPDATE_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BYPASS_OPEN_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BYPASS_OPEN_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_CLEAR_ADDR                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c21c)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_CLEAR_RMSK                                               0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_CLEAR_XO_ENABLE_BMSK                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_EXT_CLEAR_XO_ENABLE_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_ADDR                                        (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c220)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_RMSK                                              0x17
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_STBY_BMSK                                     0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_STBY_SHFT                                      0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OUTDIS_BMSK                                    0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OUTDIS_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OPMODE1_BMSK                                   0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OPMODE1_SHFT                                   0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_RESET_BMSK                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_RESET_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_ADDR                                   (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c224)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RMSK                                          0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_WAKE_IRQ_BMSK                                 0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_WAKE_IRQ_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RESTORE_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RESTORE_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_SAVE_BMSK                                     0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_SAVE_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_ADDR                                (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c228)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_RMSK                                      0x3d
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_FIFO_ARESET_BMSK                    0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_FIFO_ARESET_SHFT                     0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_HALTREQ_BMSK                        0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_HALTREQ_SHFT                         0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_FIFO_ARESET_BMSK                      0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_FIFO_ARESET_SHFT                      0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_HALTREQ_BMSK                          0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_HALTREQ_SHFT                          0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_BUSM_HALTREQ_BMSK                          0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_BUSM_HALTREQ_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_ADDR                              (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c22c)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_RMSK                                    0xff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_CLEAR_BMSK                0x80
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_CLEAR_SHFT                 0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_SET_BMSK                  0x40
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_SET_SHFT                   0x6
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_CLEAR_BMSK                0x20
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_CLEAR_SHFT                 0x5
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_SET_BMSK                  0x10
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_SET_SHFT                   0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_CLEAR_BMSK                  0x8
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_CLEAR_SHFT                  0x3
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_SET_BMSK                    0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_SET_SHFT                    0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_CLEAR_BMSK                  0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_CLEAR_SHFT                  0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_SET_BMSK                    0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_SET_SHFT                    0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_ADDR                                (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c230)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_RMSK                                       0x7
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM2_BMSK                                 0x4
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM2_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_CORE_BMSK                                  0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_CORE_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_ADDR                                  (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c234)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_RMSK                                         0xf
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_L2DATA_BMSK                                  0xf
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_L2DATA_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_ADDR                                       (SSC_QDSP6V65SS_CSR_REG_BASE      + 0x0000c238)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_RMSK                                           0x3fff
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SPARE_BMSK                                     0x3c00
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SPARE_SHFT                                        0xa
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_TCS_HW_TRIGGER_BMSK                             0x3fc
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_TCS_HW_TRIGGER_SHFT                               0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_BRINGUP_REQ_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_BRINGUP_REQ_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SHUTDOWN_REQ_BMSK                                 0x1
#define HWIO_SSC_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SHUTDOWN_REQ_SHFT                                 0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6V65SS_PUB
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6V65SS_PUB_REG_BASE                                                 (LPASS_BASE      + 0x00400000)

#define HWIO_SSC_QDSP6SS_VERSION_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000000)
#define HWIO_SSC_QDSP6SS_VERSION_RMSK                                               0xffffffff
#define HWIO_SSC_QDSP6SS_VERSION_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_VERSION_ADDR, HWIO_SSC_QDSP6SS_VERSION_RMSK)
#define HWIO_SSC_QDSP6SS_VERSION_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_VERSION_ADDR, m)
#define HWIO_SSC_QDSP6SS_VERSION_MAJOR_BMSK                                         0xf0000000
#define HWIO_SSC_QDSP6SS_VERSION_MAJOR_SHFT                                               0x1c
#define HWIO_SSC_QDSP6SS_VERSION_MINOR_BMSK                                          0xfff0000
#define HWIO_SSC_QDSP6SS_VERSION_MINOR_SHFT                                               0x10
#define HWIO_SSC_QDSP6SS_VERSION_STEP_BMSK                                              0xffff
#define HWIO_SSC_QDSP6SS_VERSION_STEP_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_CONFIG_ID_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000004)
#define HWIO_SSC_QDSP6SS_CONFIG_ID_RMSK                                             0xfff1ff3f
#define HWIO_SSC_QDSP6SS_CONFIG_ID_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CONFIG_ID_ADDR, HWIO_SSC_QDSP6SS_CONFIG_ID_RMSK)
#define HWIO_SSC_QDSP6SS_CONFIG_ID_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CONFIG_ID_ADDR, m)
#define HWIO_SSC_QDSP6SS_CONFIG_ID_NUM_L2VIC_SRC_BMSK                               0xfff00000
#define HWIO_SSC_QDSP6SS_CONFIG_ID_NUM_L2VIC_SRC_SHFT                                     0x14
#define HWIO_SSC_QDSP6SS_CONFIG_ID_ZMEAS_PRESENT_BMSK                                  0x10000
#define HWIO_SSC_QDSP6SS_CONFIG_ID_ZMEAS_PRESENT_SHFT                                     0x10
#define HWIO_SSC_QDSP6SS_CONFIG_ID_PMIC_IF_PRESENT_BMSK                                 0x8000
#define HWIO_SSC_QDSP6SS_CONFIG_ID_PMIC_IF_PRESENT_SHFT                                    0xf
#define HWIO_SSC_QDSP6SS_CONFIG_ID_IN_SUBSYSTEM_BMSK                                    0x7000
#define HWIO_SSC_QDSP6SS_CONFIG_ID_IN_SUBSYSTEM_SHFT                                       0xc
#define HWIO_SSC_QDSP6SS_CONFIG_ID_LDO_PRESENT_BMSK                                      0x800
#define HWIO_SSC_QDSP6SS_CONFIG_ID_LDO_PRESENT_SHFT                                        0xb
#define HWIO_SSC_QDSP6SS_CONFIG_ID_BHS_PRESENT_BMSK                                      0x400
#define HWIO_SSC_QDSP6SS_CONFIG_ID_BHS_PRESENT_SHFT                                        0xa
#define HWIO_SSC_QDSP6SS_CONFIG_ID_PLL_VOTE_BMSK                                         0x200
#define HWIO_SSC_QDSP6SS_CONFIG_ID_PLL_VOTE_SHFT                                           0x9
#define HWIO_SSC_QDSP6SS_CONFIG_ID_PLL_PRESENT_BMSK                                      0x100
#define HWIO_SSC_QDSP6SS_CONFIG_ID_PLL_PRESENT_SHFT                                        0x8
#define HWIO_SSC_QDSP6SS_CONFIG_ID_L2_SIZE_BMSK                                           0x3f
#define HWIO_SSC_QDSP6SS_CONFIG_ID_L2_SIZE_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_RST_EVB_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000010)
#define HWIO_SSC_QDSP6SS_RST_EVB_RMSK                                                0xffffff0
#define HWIO_SSC_QDSP6SS_RST_EVB_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RST_EVB_ADDR, HWIO_SSC_QDSP6SS_RST_EVB_RMSK)
#define HWIO_SSC_QDSP6SS_RST_EVB_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RST_EVB_ADDR, m)
#define HWIO_SSC_QDSP6SS_RST_EVB_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RST_EVB_ADDR,v)
#define HWIO_SSC_QDSP6SS_RST_EVB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RST_EVB_ADDR,m,v,HWIO_SSC_QDSP6SS_RST_EVB_IN)
#define HWIO_SSC_QDSP6SS_RST_EVB_EVB_BMSK                                            0xffffff0
#define HWIO_SSC_QDSP6SS_RST_EVB_EVB_SHFT                                                  0x4

#define HWIO_SSC_QDSP6SS_DBG_CFG_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000018)
#define HWIO_SSC_QDSP6SS_DBG_CFG_RMSK                                               0xffff0002
#define HWIO_SSC_QDSP6SS_DBG_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_CFG_ADDR, HWIO_SSC_QDSP6SS_DBG_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_DBG_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_DBG_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_DBG_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_DBG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_DBG_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_DBG_CFG_IN)
#define HWIO_SSC_QDSP6SS_DBG_CFG_DBG_SW_REG_BMSK                                    0xff000000
#define HWIO_SSC_QDSP6SS_DBG_CFG_DBG_SW_REG_SHFT                                          0x18
#define HWIO_SSC_QDSP6SS_DBG_CFG_DBG_SPARE_BMSK                                       0xff0000
#define HWIO_SSC_QDSP6SS_DBG_CFG_DBG_SPARE_SHFT                                           0x10
#define HWIO_SSC_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_SHFT                                         0x1

#define HWIO_SSC_QDSP6SS_RET_CFG_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000001c)
#define HWIO_SSC_QDSP6SS_RET_CFG_RMSK                                                      0x1
#define HWIO_SSC_QDSP6SS_RET_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RET_CFG_ADDR, HWIO_SSC_QDSP6SS_RET_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_RET_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RET_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_RET_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RET_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_RET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RET_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_RET_CFG_IN)
#define HWIO_SSC_QDSP6SS_RET_CFG_RET_ARES_ENA_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_RET_CFG_RET_ARES_ENA_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000020)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_RMSK                                             0x80000001
#define HWIO_SSC_QDSP6SS_CORE_CBCR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR, HWIO_SSC_QDSP6SS_CORE_CBCR_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_CBCR_IN)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_SSC_QDSP6SS_CORE_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_SSC_QDSP6SS_CORE_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_SSC_QDSP6SS_CORE_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR                                         (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000028)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_RMSK                                         0x80000013
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR, HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_IN)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_BMSK                                0x80000000
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_SHFT                                      0x1f
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                0x10
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                 0x4
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_UPDATE_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR                                         (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000002c)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_RMSK                                           0x11071f
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR, HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_IN)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_HW_CLK_CONTROL_BMSK                            0x100000
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_HW_CLK_CONTROL_SHFT                                0x14
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_RCGLITE_DISABLE_BMSK                            0x10000
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_RCGLITE_DISABLE_SHFT                               0x10
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_BMSK                                      0x700
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT                                        0x8
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_BMSK                                       0x1f
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000048)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_RMSK                                               0x1
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR, HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_RMSK)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_IN)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_Q6_MON_CLKEN_BMSK                                  0x1
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_Q6_MON_CLKEN_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_XO_CBCR_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000038)
#define HWIO_SSC_QDSP6SS_XO_CBCR_RMSK                                               0x80000001
#define HWIO_SSC_QDSP6SS_XO_CBCR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_XO_CBCR_ADDR, HWIO_SSC_QDSP6SS_XO_CBCR_RMSK)
#define HWIO_SSC_QDSP6SS_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_XO_CBCR_ADDR, m)
#define HWIO_SSC_QDSP6SS_XO_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_XO_CBCR_ADDR,v)
#define HWIO_SSC_QDSP6SS_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_XO_CBCR_ADDR,m,v,HWIO_SSC_QDSP6SS_XO_CBCR_IN)
#define HWIO_SSC_QDSP6SS_XO_CBCR_CLKOFF_BMSK                                        0x80000000
#define HWIO_SSC_QDSP6SS_XO_CBCR_CLKOFF_SHFT                                              0x1f
#define HWIO_SSC_QDSP6SS_XO_CBCR_CLKEN_BMSK                                                0x1
#define HWIO_SSC_QDSP6SS_XO_CBCR_CLKEN_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000003c)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_RMSK                                            0x80000001
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR, HWIO_SSC_QDSP6SS_SLEEP_CBCR_RMSK)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR, m)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR,v)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR,m,v,HWIO_SSC_QDSP6SS_SLEEP_CBCR_IN)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_NMI_ADDR                                                   (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000040)
#define HWIO_SSC_QDSP6SS_NMI_RMSK                                                          0x3
#define HWIO_SSC_QDSP6SS_NMI_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_NMI_ADDR,v)
#define HWIO_SSC_QDSP6SS_NMI_CLEAR_STATUS_BMSK                                             0x2
#define HWIO_SSC_QDSP6SS_NMI_CLEAR_STATUS_SHFT                                             0x1
#define HWIO_SSC_QDSP6SS_NMI_SET_NMI_BMSK                                                  0x1
#define HWIO_SSC_QDSP6SS_NMI_SET_NMI_SHFT                                                  0x0

#define HWIO_SSC_QDSP6SS_NMI_STATUS_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000044)
#define HWIO_SSC_QDSP6SS_NMI_STATUS_RMSK                                                   0x7
#define HWIO_SSC_QDSP6SS_NMI_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_NMI_STATUS_ADDR, HWIO_SSC_QDSP6SS_NMI_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_NMI_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_NMI_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_NMI_STATUS_EXT_DBG_TRIG_BMSK                                      0x4
#define HWIO_SSC_QDSP6SS_NMI_STATUS_EXT_DBG_TRIG_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_NMI_STATUS_WDOG_TRIG_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_NMI_STATUS_WDOG_TRIG_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000088)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_RMSK                                                 0x7
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR, HWIO_SSC_QDSP6SS_INTF_HALTREQ_RMSK)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR, m)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR,v)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR,m,v,HWIO_SSC_QDSP6SS_INTF_HALTREQ_IN)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_AXIM2_BMSK                                           0x4
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_AXIM2_SHFT                                           0x2
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_AXIM_BMSK                                            0x2
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_AXIM_SHFT                                            0x1
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_ALL_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_ALL_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_INTF_HALTACK_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000008c)
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_RMSK                                                 0x7
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_HALTACK_ADDR, HWIO_SSC_QDSP6SS_INTF_HALTACK_RMSK)
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_HALTACK_ADDR, m)
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_AXIM2_BMSK                                           0x4
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_AXIM2_SHFT                                           0x2
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_AXIM_BMSK                                            0x2
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_AXIM_SHFT                                            0x1
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_ALL_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_ALL_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_ADDR                                         (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000090)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_RMSK                                                0xf
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_INTFCLAMP_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_E_BMSK                                  0x8
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_E_SHFT                                  0x3
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_L_BMSK                                  0x4
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_L_SHFT                                  0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_E_BMSK                                   0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_E_SHFT                                   0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_L_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_L_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_ADDR                                       (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000094)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_RMSK                                              0xf
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_E_BMSK                                0x8
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_E_SHFT                                0x3
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_L_BMSK                                0x4
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_L_SHFT                                0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_E_BMSK                                 0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_E_SHFT                                 0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_L_BMSK                                 0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_L_SHFT                                 0x0

#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_ADDR                                      (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000098)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_RMSK                                             0xf
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_ADDR, HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_E_BMSK                               0x8
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_E_SHFT                               0x3
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_L_BMSK                               0x4
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_L_SHFT                               0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_E_BMSK                                0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_E_SHFT                                0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_L_BMSK                                0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_L_SHFT                                0x0

#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR                                       (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000009c)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_RMSK                                              0x3
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR, HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_RMSK)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR, m)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR,v)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR,m,v,HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_IN)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_AXIM2_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_AXIM2_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_AXIM_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_AXIM_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x000000a0)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_RMSK                                                 0xf
#define HWIO_SSC_QDSP6SS_STATERET_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR, HWIO_SSC_QDSP6SS_STATERET_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_STATERET_CTL_IN)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_WAKEUP_IN_BMSK                                       0x8
#define HWIO_SSC_QDSP6SS_STATERET_CTL_WAKEUP_IN_SHFT                                       0x3
#define HWIO_SSC_QDSP6SS_STATERET_CTL_WAKE_IRQ_BMSK                                        0x4
#define HWIO_SSC_QDSP6SS_STATERET_CTL_WAKE_IRQ_SHFT                                        0x2
#define HWIO_SSC_QDSP6SS_STATERET_CTL_RESTORE_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_STATERET_CTL_RESTORE_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_STATERET_CTL_SAVE_BMSK                                            0x1
#define HWIO_SSC_QDSP6SS_STATERET_CTL_SAVE_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x000000b0)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_RMSK                                              0xffff
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR, HWIO_SSC_QDSP6SS_CGC_OVERRIDE_RMSK)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR, m)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR,v)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR,m,v,HWIO_SSC_QDSP6SS_CGC_OVERRIDE_IN)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CP0_CLK_EN_BMSK                                   0x8000
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CP0_CLK_EN_SHFT                                      0xf
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT14_BMSK                                  0x4000
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT14_SHFT                                     0xe
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_BMSK                                  0x2000
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_SHFT                                     0xd
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT12_BMSK                                  0x1000
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT12_SHFT                                     0xc
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_BMSK                                   0x800
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_SHFT                                     0xb
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_BMSK                                   0x400
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_SHFT                                     0xa
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_BMSK                              0x300
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_SHFT                                0x8
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT7_BMSK                                     0x80
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT7_SHFT                                      0x7
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_BMSK                              0x60
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_SHFT                               0x5
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_BMSK                               0x10
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_SHFT                                0x4
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_BMSK                                 0x8
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_SHFT                                 0x3
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_BMSK                                     0x4
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_SHFT                                     0x2
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_BMSK                                     0x1
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x000000c0)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_RMSK                                                 0xf
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR, HWIO_SSC_QDSP6SS_CORE_BHS_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_BHS_CTL_IN)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_DRIVE_SEL_BMSK                                       0xf
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_DRIVE_SEL_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_CP_BHS_CTL_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x000000c4)
#define HWIO_SSC_QDSP6SS_CP_BHS_CTL_RMSK                                                   0xf
#define HWIO_SSC_QDSP6SS_CP_BHS_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_BHS_CTL_ADDR, HWIO_SSC_QDSP6SS_CP_BHS_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_CP_BHS_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_BHS_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_CP_BHS_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CP_BHS_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_CP_BHS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CP_BHS_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_CP_BHS_CTL_IN)
#define HWIO_SSC_QDSP6SS_CP_BHS_CTL_DRIVE_SEL_BMSK                                         0xf
#define HWIO_SSC_QDSP6SS_CP_BHS_CTL_DRIVE_SEL_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x000000d0)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_RMSK                                                 0x3
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR, HWIO_SSC_QDSP6SS_CPR_OVERRIDE_RMSK)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR, m)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR,v)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR,m,v,HWIO_SSC_QDSP6SS_CPR_OVERRIDE_IN)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_EXT_ABYP_DIS_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_EXT_ABYP_DIS_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_LOCAL_BYP_DIS_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_LOCAL_BYP_DIS_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000100)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_RMSK                                             0xfffc0000
#define HWIO_SSC_QDSP6SS_STRAP_TCM_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR, HWIO_SSC_QDSP6SS_STRAP_TCM_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_TCM_IN)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_BASE_ADDR_BMSK                                   0xfffc0000
#define HWIO_SSC_QDSP6SS_STRAP_TCM_BASE_ADDR_SHFT                                         0x12

#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000104)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_RMSK                                        0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR, HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_IN)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_BMSK                             0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_SHFT                                    0xe

#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000108)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_RMSK                                        0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR, HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_IN)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_BMSK                             0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_SHFT                                    0xe

#define HWIO_SSC_QDSP6SS_STRAP_ACC_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000110)
#define HWIO_SSC_QDSP6SS_STRAP_ACC_RMSK                                             0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_ACC_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_ACC_ADDR, HWIO_SSC_QDSP6SS_STRAP_ACC_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_ACC_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_ACC_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_ACC_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_ACC_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_ACC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_ACC_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_ACC_IN)
#define HWIO_SSC_QDSP6SS_STRAP_ACC_DATA_BMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_ACC_DATA_SHFT                                               0x0

#define HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR                                              (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000118)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_RMSK                                              0xffffffff
#define HWIO_SSC_QDSP6SS_DCC_CTRL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR, HWIO_SSC_QDSP6SS_DCC_CTRL_RMSK)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR, m)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR,v)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR,m,v,HWIO_SSC_QDSP6SS_DCC_CTRL_IN)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_CTRL_BMSK                                         0xffffffff
#define HWIO_SSC_QDSP6SS_DCC_CTRL_CTRL_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR                                      (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000011c)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_RMSK                                      0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR, HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_IN)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_UPPER_ADDR_BMSK                           0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_UPPER_ADDR_SHFT                                  0xe

#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR                                      (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000120)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_RMSK                                      0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR, HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_IN)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_LOWER_ADDR_BMSK                           0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_LOWER_ADDR_SHFT                                  0xe

#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR                                     (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000124)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_RMSK                                            0x1
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR, HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_IN)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_STRAP_QMC_ACC_SEL_BMSK                          0x1
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_STRAP_QMC_ACC_SEL_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR                                         (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000128)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_RMSK                                         0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR, HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_IN)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_STRAP_QMC_ACC_BMSK                           0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_STRAP_QMC_ACC_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR                                       (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000012c)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_RMSK                                       0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR, HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_IN)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_STRAP_QMC_ACC_U_BMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_STRAP_QMC_ACC_U_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR                                           (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000130)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_RMSK                                                  0x3
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR, HWIO_SSC_QDSP6SS_QMC_SVS_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_QMC_SVS_CTL_IN)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_BMSK                                      0x1
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SHFT                                      0x0

#define HWIO_SSC_QDSP6SS_PLL_MODE_ADDR                                              (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000200)
#define HWIO_SSC_QDSP6SS_PLL_MODE_RMSK                                              0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_MODE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_MODE_ADDR, HWIO_SSC_QDSP6SS_PLL_MODE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_MODE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_MODE_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_MODE_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_MODE_IN)
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_BMSK                                 0x80000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_SHFT                                       0x1f
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                              0x40000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                    0x1e
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_BMSK                                0x20000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_SHFT                                      0x1d
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_FINE_BMSK                            0x10000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_FINE_SHFT                                  0x1c
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BITS27_24_BMSK                             0xf000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BITS27_24_SHFT                                  0x18
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                     0x800000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                         0x17
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_UPDATE_BMSK                                     0x400000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_UPDATE_SHFT                                         0x16
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                             0x200000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                 0x15
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                               0x100000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                   0x14
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_BMSK                                  0xfc000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_SHFT                                      0xe
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_BMSK                                   0x3f00
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_SHFT                                      0x8
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BITS7_3_BMSK                                    0xf8
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BITS7_3_SHFT                                     0x3
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_RESET_N_BMSK                                         0x4
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_RESET_N_SHFT                                         0x2
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BIT1_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BIT1_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_OUTCTRL_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000204)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_RMSK                                                 0xffff
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR, HWIO_SSC_QDSP6SS_PLL_L_VAL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_L_VAL_IN)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_PLL_L_BMSK                                           0xffff
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_PLL_L_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR                                         (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000208)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_RMSK                                             0xffff
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR, HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_IN)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_PLL_CAL_L_BMSK                                   0xffff
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_PLL_CAL_L_SHFT                                      0x0

#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000020c)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR, HWIO_SSC_QDSP6SS_PLL_USER_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_USER_CTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_19_BMSK                        0xfff80000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_19_SHFT                              0x13
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                               0x70000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                  0x10
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_ODD_BMSK                           0xf000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_ODD_SHFT                              0xc
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                           0xf00
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                             0x8
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                               0x80
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                0x7
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                                0x60
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                                 0x5
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_TEST_BMSK                                    0x10
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_TEST_SHFT                                     0x4
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BIT3_BMSK                                    0x8
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BIT3_SHFT                                    0x3
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_ODD_BMSK                                      0x4
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_ODD_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_EVEN_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_EVEN_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_MAIN_BMSK                                     0x1
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_MAIN_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000210)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR, HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_IN)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS31_19_BMSK                      0xfff80000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS31_19_SHFT                            0x13
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                       0x40000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                          0x12
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                       0x20000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                          0x11
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_BMSK                        0x10000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_SHFT                           0x10
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_FRAC_FORMAT_SEL_BMSK                            0x8000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_FRAC_FORMAT_SEL_SHFT                               0xf
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                         0x4000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                            0xe
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_REF_CLK_AT_OUT_BMSK                             0x2000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                0xd
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_BMSK                      0x1800
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_SHFT                         0xb
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                      0x400
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                        0xa
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                             0x3e0
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                               0x5
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_EN_BMSK                               0x10
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_EN_SHFT                                0x4
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_CALIB_CTRL_BMSK                                    0xe
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_CALIB_CTRL_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_BMSK                        0x1
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_SHFT                        0x0

#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000214)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR, HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_BMSK                     0xfc000000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_SHFT                           0x1a
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_BMSK                    0x3c00000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_SHFT                         0x16
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_BMSK            0x3c0000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_SHFT                0x12
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_BMSK             0x3c000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_SHFT                 0xe
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                    0x3800
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                       0xb
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                   0x700
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                     0x8
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_BMSK                         0xf0
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_SHFT                          0x4
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                  0xf
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR                                      (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000218)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RMSK                                      0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR, HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_IN)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_BIST_CFG_BMSK                             0xfff00000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_BIST_CFG_SHFT                                   0x14
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BITS19_12_BMSK                       0xff000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BITS19_12_SHFT                           0xc
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_BMSK                       0xc00
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_SHFT                         0xa
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_BMSK                           0x380
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_SHFT                             0x7
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_BMSK                0x40
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                 0x6
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_BMSK                       0x30
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_SHFT                        0x4
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_BMSK                       0x8
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_SHFT                       0x3
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_BMSK                            0x6
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_SHFT                            0x1
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_BMSK                     0x1
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000021c)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR, HWIO_SSC_QDSP6SS_PLL_TEST_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_TEST_CTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_BMSK                  0xc0000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_SHFT                        0x1e
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK              0x30000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                    0x1c
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                     0x8000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                          0x1b
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_FINE_FCW_BMSK                                  0x7e00000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_FINE_FCW_SHFT                                       0x15
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                          0x100000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                              0x14
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_BMSK                                  0xff000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_SHFT                                      0xc
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_BMSK                           0x800
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_SHFT                             0xb
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_PROCESS_CALIB_WORD_BMSK                            0x700
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_PROCESS_CALIB_WORD_SHFT                              0x8
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_BMSK                            0x80
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_SHFT                             0x7
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                   0x40
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                    0x6
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST1_SEL_BMSK                                     0x30
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST1_SEL_SHFT                                      0x4
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST0_SEL_BMSK                                      0xc
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST0_SEL_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_BMSK                                       0x2
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_SHFT                                       0x1
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000220)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR, HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_IN)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_BMSK                    0x80000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_SHFT                          0x1f
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_BMSK                       0x40000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_SHFT                             0x1e
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_BIAS_DET_BMSK                        0x20000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_BIAS_DET_SHFT                              0x1d
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_BMSK                       0x10000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_SHFT                             0x1c
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_BMSK                  0x8000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_SHFT                       0x1b
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_BMSK               0x6000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_SHFT                    0x19
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_BMSK                    0x1c00000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_SHFT                         0x16
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_BMSK                     0x200000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_SHFT                         0x15
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DISABLE_CLAMP_BMSK                            0x100000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DISABLE_CLAMP_SHFT                                0x14
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_BMSK                       0x80000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_SHFT                          0x13
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_BMSK                   0x40000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_SHFT                      0x12
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BIAS_ADJUST_BMSK                               0x30000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BIAS_ADJUST_SHFT                                  0x10
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DIV2_NMO_EN_BMSK                                0x8000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DIV2_NMO_EN_SHFT                                   0xf
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DIS_LEAK_CMP_BMSK                               0x4000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DIS_LEAK_CMP_SHFT                                  0xe
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SINGLE_DMET_EN_BMSK                             0x2000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SINGLE_DMET_EN_SHFT                                0xd
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_BMSK                           0x1000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_SHFT                              0xc
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                                 0xc00
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                   0xa
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                   0x380
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                     0x7
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_EN_BMSK                                 0x40
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_EN_SHFT                                  0x6
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                 0x20
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                  0x5
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                             0x18
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                              0x3
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_STATUS_REG_EN_BMSK                                 0x4
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_STATUS_REG_EN_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SKIP_FINE_TUNE_BMSK                                0x2
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SKIP_FINE_TUNE_SHFT                                0x1
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SKIP_FAST_LOCK_BMSK                                0x1
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SKIP_FAST_LOCK_SHFT                                0x0

#define HWIO_SSC_QDSP6SS_PLL_STATUS_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000224)
#define HWIO_SSC_QDSP6SS_PLL_STATUS_RMSK                                            0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_STATUS_ADDR, HWIO_SSC_QDSP6SS_PLL_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_STATUS_STATUS_31_0_BMSK                                0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_STATUS_STATUS_31_0_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000228)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR, HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000022c)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_RMSK                                                   0x7
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR, HWIO_SSC_QDSP6SS_PLL_OPMODE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_OPMODE_IN)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_PLL_OPMODE_BMSK                                        0x7
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_PLL_OPMODE_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_PLL_STATE_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000230)
#define HWIO_SSC_QDSP6SS_PLL_STATE_RMSK                                                    0x7
#define HWIO_SSC_QDSP6SS_PLL_STATE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_STATE_ADDR, HWIO_SSC_QDSP6SS_PLL_STATE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_STATE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_STATE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_STATE_PLL_STATE_BMSK                                          0x7
#define HWIO_SSC_QDSP6SS_PLL_STATE_PLL_STATE_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_PLL_DROOP_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000234)
#define HWIO_SSC_QDSP6SS_PLL_DROOP_RMSK                                                   0xff
#define HWIO_SSC_QDSP6SS_PLL_DROOP_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_DROOP_ADDR, HWIO_SSC_QDSP6SS_PLL_DROOP_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_DROOP_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_DROOP_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_DROOP_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_DROOP_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_DROOP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_DROOP_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_DROOP_IN)
#define HWIO_SSC_QDSP6SS_PLL_DROOP_DROOP_CODE_BMSK                                        0xfe
#define HWIO_SSC_QDSP6SS_PLL_DROOP_DROOP_CODE_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_PLL_DROOP_DROOP_EN_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_PLL_DROOP_DROOP_EN_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000238)
#define HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_RMSK                                              0xffff
#define HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_ADDR, HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_IN)
#define HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_PLL_FRAC_VAL_BMSK                                 0xffff
#define HWIO_SSC_QDSP6SS_PLL_FRAC_VAL_PLL_FRAC_VAL_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000023c)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_RMSK                                                   0xff
#define HWIO_SSC_QDSP6SS_PLL_SPARE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR, HWIO_SSC_QDSP6SS_PLL_SPARE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_SPARE_IN)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_SPARE_OUTPUTS_BMSK                                     0xf0
#define HWIO_SSC_QDSP6SS_PLL_SPARE_SPARE_OUTPUTS_SHFT                                      0x4
#define HWIO_SSC_QDSP6SS_PLL_SPARE_SPARE_INPUTS_BMSK                                       0xf
#define HWIO_SSC_QDSP6SS_PLL_SPARE_SPARE_INPUTS_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR                                   (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000240)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_RMSK                                   0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR, HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_IN)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_RESERVE_BITS31_8_BMSK                  0xffffff00
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_RESERVE_BITS31_8_SHFT                         0x8
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_BMSK                         0xff
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR                                   (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000244)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_RMSK                                   0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR, HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_IN)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_RESERVE_BITS31_4_BMSK                  0xfffffff0
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_RESERVE_BITS31_4_SHFT                         0x4
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_BMSK                          0xf
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR                                     (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000248)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_RMSK                                     0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR, HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_IN)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_RESERVE_BITS31_4_BMSK                    0xfffffff0
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_RESERVE_BITS31_4_SHFT                           0x4
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_BMSK                              0xf
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR                                 (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000250)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_RMSK                                        0x3
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR, HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_RCG2PLL_UPD_STATUS_BMSK                     0x2
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_RCG2PLL_UPD_STATUS_SHFT                     0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_PLL2RCG_UPD_STATUS_BMSK                     0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_PLL2RCG_UPD_STATUS_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR                                    (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000254)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_RMSK                                           0x3
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR, HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_IN)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_RCG2PLL_UPD_EN_BMSK                            0x2
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_RCG2PLL_UPD_EN_SHFT                            0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_PLL2RCG_UPD_EN_BMSK                            0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_PLL2RCG_UPD_EN_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR                                 (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000258)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_RMSK                                       0x1f
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR, HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_MATCH_VALUE_BMSK                           0x1f
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_MATCH_VALUE_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_ADDR                                    (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000025c)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_RMSK                                           0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_CMD_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_CMD_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR                                           (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000300)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_RMSK                                                  0xf
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR, HWIO_SSC_QDSP6SS_DBG_NMI_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_DBG_NMI_CFG_IN)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_APCR_BMSK                               0x8
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_APCR_SHFT                               0x3
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_PWR_COLLAPSE_BMSK                       0x4
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_PWR_COLLAPSE_SHFT                       0x2
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_DISABLE_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_DISABLE_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_WAKEUP_DISABLE_BMSK                           0x1
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_WAKEUP_DISABLE_SHFT                           0x0

#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR                                    (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000304)
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_RMSK                                           0x3
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR, HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_PWR_COLLAPSE_BMSK                        0x2
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_PWR_COLLAPSE_SHFT                        0x1
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_IN_SLEEP_BMSK                            0x1
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_IN_SLEEP_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR                                       (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000400)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_RMSK                                              0x1
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR, HWIO_SSC_QDSP6SS_BOOT_CORE_START_RMSK)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR, m)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR,v)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR,m,v,HWIO_SSC_QDSP6SS_BOOT_CORE_START_IN)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_START_BMSK                                        0x1
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_START_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_BOOT_CMD_ADDR                                              (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000404)
#define HWIO_SSC_QDSP6SS_BOOT_CMD_RMSK                                                     0x1
#define HWIO_SSC_QDSP6SS_BOOT_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_BOOT_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_BOOT_CMD_CMD_BMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_BOOT_CMD_CMD_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_BOOT_STATUS_ADDR                                           (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000408)
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_RMSK                                                  0x1
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_BOOT_STATUS_ADDR, HWIO_SSC_QDSP6SS_BOOT_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_BOOT_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_STATUS_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_STATUS_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000410)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_RMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR, HWIO_SSC_QDSP6SS_CORE_BHS_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_BHS_CFG_IN)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_BHS_ON_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_BHS_ON_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000414)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_RMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_BHS_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_UPDATE_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_UPDATE_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_ADDR                                       (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000418)
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_RMSK                                              0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_ADDR, HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_BHS_ON_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_BHS_ON_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR                                           (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000420)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_RMSK                                                  0x7
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR, HWIO_SSC_QDSP6SS_LDO_CTL_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_LDO_CTL_CFG_IN)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BG_BMSK                                    0x4
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BG_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BMSK                                       0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_SHFT                                       0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_BYP_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_BYP_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_ADDR                                           (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000424)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_RMSK                                                  0x7
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_LDO_CTL_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_BG_UPDATE_BMSK                             0x4
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_BG_UPDATE_SHFT                             0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_UPDATE_BMSK                                0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_UPDATE_SHFT                                0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_BYP_UPDATE_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_BYP_UPDATE_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000428)
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_RMSK                                               0x7
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_ADDR, HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BG_BMSK                                 0x4
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BG_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_BYP_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_BYP_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_MEM_CFG_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000430)
#define HWIO_SSC_QDSP6SS_MEM_CFG_RMSK                                                 0x1f001f
#define HWIO_SSC_QDSP6SS_MEM_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_CFG_ADDR, HWIO_SSC_QDSP6SS_MEM_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_MEM_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_MEM_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_MEM_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_MEM_CFG_IN)
#define HWIO_SSC_QDSP6SS_MEM_CFG_L2MEM_SLP_RET_N_BMSK                                 0x1e0000
#define HWIO_SSC_QDSP6SS_MEM_CFG_L2MEM_SLP_RET_N_SHFT                                     0x11
#define HWIO_SSC_QDSP6SS_MEM_CFG_MEM_SLP_RET_N_BMSK                                    0x10000
#define HWIO_SSC_QDSP6SS_MEM_CFG_MEM_SLP_RET_N_SHFT                                       0x10
#define HWIO_SSC_QDSP6SS_MEM_CFG_L2MEM_SLP_NRET_N_BMSK                                    0x1e
#define HWIO_SSC_QDSP6SS_MEM_CFG_L2MEM_SLP_NRET_N_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_MEM_CFG_MEM_SLP_NRET_N_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_MEM_CFG_MEM_SLP_NRET_N_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_MEM_CMD_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000434)
#define HWIO_SSC_QDSP6SS_MEM_CMD_RMSK                                                      0x3
#define HWIO_SSC_QDSP6SS_MEM_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_CMD_UPDATE_SLP_RET_N_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_MEM_CMD_UPDATE_SLP_RET_N_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_MEM_CMD_UPDATE_SLP_NRET_N_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_MEM_CMD_UPDATE_SLP_NRET_N_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_MEM_STATUS_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000438)
#define HWIO_SSC_QDSP6SS_MEM_STATUS_RMSK                                              0x1f001f
#define HWIO_SSC_QDSP6SS_MEM_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_STATUS_ADDR, HWIO_SSC_QDSP6SS_MEM_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_MEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_MEM_STATUS_L2MEM_SLP_RET_N_BMSK                              0x1e0000
#define HWIO_SSC_QDSP6SS_MEM_STATUS_L2MEM_SLP_RET_N_SHFT                                  0x11
#define HWIO_SSC_QDSP6SS_MEM_STATUS_MEM_SLP_RET_N_BMSK                                 0x10000
#define HWIO_SSC_QDSP6SS_MEM_STATUS_MEM_SLP_RET_N_SHFT                                    0x10
#define HWIO_SSC_QDSP6SS_MEM_STATUS_L2MEM_SLP_NRET_N_BMSK                                 0x1e
#define HWIO_SSC_QDSP6SS_MEM_STATUS_L2MEM_SLP_NRET_N_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_MEM_STATUS_MEM_SLP_NRET_N_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_MEM_STATUS_MEM_SLP_NRET_N_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_RESET_CFG_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000440)
#define HWIO_SSC_QDSP6SS_RESET_CFG_RMSK                                                 0x1fff
#define HWIO_SSC_QDSP6SS_RESET_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RESET_CFG_ADDR, HWIO_SSC_QDSP6SS_RESET_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_RESET_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RESET_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_RESET_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RESET_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_RESET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RESET_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_RESET_CFG_IN)
#define HWIO_SSC_QDSP6SS_RESET_CFG_APB_BMSK                                             0x1000
#define HWIO_SSC_QDSP6SS_RESET_CFG_APB_SHFT                                                0xc
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIS2_BMSK                                            0x800
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIS2_SHFT                                              0xb
#define HWIO_SSC_QDSP6SS_RESET_CFG_CSI1_BMSK                                             0x400
#define HWIO_SSC_QDSP6SS_RESET_CFG_CSI1_SHFT                                               0xa
#define HWIO_SSC_QDSP6SS_RESET_CFG_CSI0_BMSK                                             0x200
#define HWIO_SSC_QDSP6SS_RESET_CFG_CSI0_SHFT                                               0x9
#define HWIO_SSC_QDSP6SS_RESET_CFG_ARCH_BMSK                                             0x100
#define HWIO_SSC_QDSP6SS_RESET_CFG_ARCH_SHFT                                               0x8
#define HWIO_SSC_QDSP6SS_RESET_CFG_CORE_BMSK                                              0x80
#define HWIO_SSC_QDSP6SS_RESET_CFG_CORE_SHFT                                               0x7
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIM2_BMSK                                             0x40
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIM2_SHFT                                              0x6
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIS_BMSK                                              0x20
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIS_SHFT                                               0x5
#define HWIO_SSC_QDSP6SS_RESET_CFG_ATBM_BMSK                                              0x10
#define HWIO_SSC_QDSP6SS_RESET_CFG_ATBM_SHFT                                               0x4
#define HWIO_SSC_QDSP6SS_RESET_CFG_AHBM_BMSK                                               0x8
#define HWIO_SSC_QDSP6SS_RESET_CFG_AHBM_SHFT                                               0x3
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIM_BMSK                                               0x4
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIM_SHFT                                               0x2
#define HWIO_SSC_QDSP6SS_RESET_CFG_ISDB_ETM_BMSK                                           0x2
#define HWIO_SSC_QDSP6SS_RESET_CFG_ISDB_ETM_SHFT                                           0x1
#define HWIO_SSC_QDSP6SS_RESET_CFG_RET_BMSK                                                0x1
#define HWIO_SSC_QDSP6SS_RESET_CFG_RET_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_RESET_CMD_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000444)
#define HWIO_SSC_QDSP6SS_RESET_CMD_RMSK                                                    0x1
#define HWIO_SSC_QDSP6SS_RESET_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RESET_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_RESET_CMD_UPDATE_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_RESET_CMD_UPDATE_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_RESET_STATUS_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000448)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_RMSK                                              0x1fff
#define HWIO_SSC_QDSP6SS_RESET_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RESET_STATUS_ADDR, HWIO_SSC_QDSP6SS_RESET_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RESET_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RESET_STATUS_ADDR,v)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RESET_STATUS_ADDR,m,v,HWIO_SSC_QDSP6SS_RESET_STATUS_IN)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_APB_BMSK                                          0x1000
#define HWIO_SSC_QDSP6SS_RESET_STATUS_APB_SHFT                                             0xc
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIS2_BMSK                                         0x800
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIS2_SHFT                                           0xb
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CSI1_BMSK                                          0x400
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CSI1_SHFT                                            0xa
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CSI0_BMSK                                          0x200
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CSI0_SHFT                                            0x9
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ARCH_BMSK                                          0x100
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ARCH_SHFT                                            0x8
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CORE_BMSK                                           0x80
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CORE_SHFT                                            0x7
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIM2_BMSK                                          0x40
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIM2_SHFT                                           0x6
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIS_BMSK                                           0x20
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIS_SHFT                                            0x5
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ATBM_BMSK                                           0x10
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ATBM_SHFT                                            0x4
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AHBM_BMSK                                            0x8
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AHBM_SHFT                                            0x3
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIM_BMSK                                            0x4
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIM_SHFT                                            0x2
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ISDB_ETM_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ISDB_ETM_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_RESET_STATUS_RET_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_RESET_STATUS_RET_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000450)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_RMSK                                                    0xf
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR, HWIO_SSC_QDSP6SS_CLAMP_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CLAMP_CFG_IN)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_MSS2CX_BMSK                                       0x8
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_MSS2CX_SHFT                                       0x3
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_QMC_MEM_BMSK                                      0x4
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_QMC_MEM_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_WL_BMSK                                           0x2
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_WL_SHFT                                           0x1
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_IO_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_IO_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_CLAMP_CMD_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000454)
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_RMSK                                                    0xf
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLAMP_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_MSS2CX_BMSK                                0x8
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_MSS2CX_SHFT                                0x3
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_QMC_MEM_BMSK                               0x4
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_QMC_MEM_SHFT                               0x2
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_WL_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_WL_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_IO_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_IO_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000458)
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_RMSK                                                 0xf
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLAMP_STATUS_ADDR, HWIO_SSC_QDSP6SS_CLAMP_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLAMP_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_MSS2CX_BMSK                                    0x8
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_MSS2CX_SHFT                                    0x3
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_QMC_MEM_BMSK                                   0x4
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_QMC_MEM_SHFT                                   0x2
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_WL_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_WL_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_IO_BMSK                                        0x1
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_IO_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_CLK_CFG_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000460)
#define HWIO_SSC_QDSP6SS_CLK_CFG_RMSK                                                    0x3ff
#define HWIO_SSC_QDSP6SS_CLK_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLK_CFG_ADDR, HWIO_SSC_QDSP6SS_CLK_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CLK_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLK_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLK_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLK_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLK_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CLK_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CLK_CFG_IN)
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIS2_BMSK                                              0x200
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIS2_SHFT                                                0x9
#define HWIO_SSC_QDSP6SS_CLK_CFG_CSI1_BMSK                                               0x100
#define HWIO_SSC_QDSP6SS_CLK_CFG_CSI1_SHFT                                                 0x8
#define HWIO_SSC_QDSP6SS_CLK_CFG_CSI0_BMSK                                                0x80
#define HWIO_SSC_QDSP6SS_CLK_CFG_CSI0_SHFT                                                 0x7
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIM2_BMSK                                               0x40
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIM2_SHFT                                                0x6
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIS_BMSK                                                0x20
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIS_SHFT                                                 0x5
#define HWIO_SSC_QDSP6SS_CLK_CFG_ATBM_BMSK                                                0x10
#define HWIO_SSC_QDSP6SS_CLK_CFG_ATBM_SHFT                                                 0x4
#define HWIO_SSC_QDSP6SS_CLK_CFG_AHBM_BMSK                                                 0x8
#define HWIO_SSC_QDSP6SS_CLK_CFG_AHBM_SHFT                                                 0x3
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIM_BMSK                                                 0x4
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIM_SHFT                                                 0x2
#define HWIO_SSC_QDSP6SS_CLK_CFG_APB_BMSK                                                  0x2
#define HWIO_SSC_QDSP6SS_CLK_CFG_APB_SHFT                                                  0x1
#define HWIO_SSC_QDSP6SS_CLK_CFG_CORE_BMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_CLK_CFG_CORE_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_CLK_CMD_ADDR                                               (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000464)
#define HWIO_SSC_QDSP6SS_CLK_CMD_RMSK                                                      0x1
#define HWIO_SSC_QDSP6SS_CLK_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLK_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLK_CMD_UPDATE_CLK_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_CLK_CMD_UPDATE_CLK_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_CLK_STATUS_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000468)
#define HWIO_SSC_QDSP6SS_CLK_STATUS_RMSK                                                 0x3ff
#define HWIO_SSC_QDSP6SS_CLK_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLK_STATUS_ADDR, HWIO_SSC_QDSP6SS_CLK_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CLK_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLK_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIS2_BMSK                                           0x200
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIS2_SHFT                                             0x9
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CSI1_BMSK                                            0x100
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CSI1_SHFT                                              0x8
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CSI0_BMSK                                             0x80
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CSI0_SHFT                                              0x7
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIM2_BMSK                                            0x40
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIM2_SHFT                                             0x6
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIS_BMSK                                             0x20
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIS_SHFT                                              0x5
#define HWIO_SSC_QDSP6SS_CLK_STATUS_ATBM_BMSK                                             0x10
#define HWIO_SSC_QDSP6SS_CLK_STATUS_ATBM_SHFT                                              0x4
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AHBM_BMSK                                              0x8
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AHBM_SHFT                                              0x3
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIM_BMSK                                              0x4
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIM_SHFT                                              0x2
#define HWIO_SSC_QDSP6SS_CLK_STATUS_APB_BMSK                                               0x2
#define HWIO_SSC_QDSP6SS_CLK_STATUS_APB_SHFT                                               0x1
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CORE_BMSK                                              0x1
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CORE_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000500)
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_RMSK                                                  0x1d
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_PWR_CTL_ADDR, HWIO_SSC_QDSP6SS_CP_PWR_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_PWR_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CP_PWR_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CP_PWR_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_CP_PWR_CTL_IN)
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_CLAMP_QMC_MEM_BMSK                                    0x10
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_CLAMP_QMC_MEM_SHFT                                     0x4
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_LDO_2PASS_ON_BMSK                                      0x8
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_LDO_2PASS_ON_SHFT                                      0x3
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_SHUNT_BHS_ON_BMSK                                      0x4
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_SHUNT_BHS_ON_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_CLAMP_IO_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_CP_PWR_CTL_CLAMP_IO_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_CP_RESET_ADDR                                              (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000504)
#define HWIO_SSC_QDSP6SS_CP_RESET_RMSK                                                     0x1
#define HWIO_SSC_QDSP6SS_CP_RESET_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_RESET_ADDR, HWIO_SSC_QDSP6SS_CP_RESET_RMSK)
#define HWIO_SSC_QDSP6SS_CP_RESET_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_RESET_ADDR, m)
#define HWIO_SSC_QDSP6SS_CP_RESET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CP_RESET_ADDR,v)
#define HWIO_SSC_QDSP6SS_CP_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CP_RESET_ADDR,m,v,HWIO_SSC_QDSP6SS_CP_RESET_IN)
#define HWIO_SSC_QDSP6SS_CP_RESET_ARES_BMSK                                                0x1
#define HWIO_SSC_QDSP6SS_CP_RESET_ARES_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000508)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_RMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR, HWIO_SSC_QDSP6SS_CP_CLK_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_CP_CLK_CTL_IN)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_CLK_ENA_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_CLK_ENA_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_CP_BHS_CFG_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000510)
#define HWIO_SSC_QDSP6SS_CP_BHS_CFG_RMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_CP_BHS_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_BHS_CFG_ADDR, HWIO_SSC_QDSP6SS_CP_BHS_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CP_BHS_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_BHS_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CP_BHS_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CP_BHS_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CP_BHS_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CP_BHS_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CP_BHS_CFG_IN)
#define HWIO_SSC_QDSP6SS_CP_BHS_CFG_BHS_ON_BMSK                                            0x1
#define HWIO_SSC_QDSP6SS_CP_BHS_CFG_BHS_ON_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_CP_BHS_CMD_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000514)
#define HWIO_SSC_QDSP6SS_CP_BHS_CMD_RMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_CP_BHS_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CP_BHS_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CP_BHS_CMD_UPDATE_BMSK                                            0x1
#define HWIO_SSC_QDSP6SS_CP_BHS_CMD_UPDATE_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_CP_BHS_STATUS_ADDR                                         (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000518)
#define HWIO_SSC_QDSP6SS_CP_BHS_STATUS_RMSK                                                0x1
#define HWIO_SSC_QDSP6SS_CP_BHS_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_BHS_STATUS_ADDR, HWIO_SSC_QDSP6SS_CP_BHS_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CP_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_BHS_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CP_BHS_STATUS_BHS_ON_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_CP_BHS_STATUS_BHS_ON_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000520)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_RMSK                                                    0x3
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR, HWIO_SSC_QDSP6SS_CPMEM_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CPMEM_CFG_IN)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_VTCM_SLP_RET_N_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_VTCM_SLP_RET_N_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_VTCM_SLP_NRET_N_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_VTCM_SLP_NRET_N_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_CPMEM_CMD_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000524)
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_RMSK                                                    0x3
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CPMEM_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_RET_N_BMSK                              0x2
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_RET_N_SHFT                              0x1
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_NRET_N_BMSK                             0x1
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_NRET_N_SHFT                             0x0

#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00000528)
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_RMSK                                                 0x3
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_STATUS_ADDR, HWIO_SSC_QDSP6SS_CPMEM_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_VTCM_SLP_RET_N_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_VTCM_SLP_RET_N_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_VTCM_SLP_NRET_N_BMSK                                 0x1
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_VTCM_SLP_NRET_N_SHFT                                 0x0

#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002000)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR, HWIO_SSC_QDSP6SS_TEST_BUS_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_TEST_BUS_CTL_IN)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_BMSK                                0xff000000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_SHFT                                      0x18
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_HWE_INV_BMSK                                    0x800000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_HWE_INV_SHFT                                        0x17
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_HWE_SEL_BMSK                                    0x7c0000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_HWE_SEL_SHFT                                        0x12
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_BMSK                                  0x20000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_SHFT                                     0x11
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_Q6_SEL_BMSK                                      0x10000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_Q6_SEL_SHFT                                         0x10
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_SEL_BMSK                                          0xffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_SEL_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002004)
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_ADDR, HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_RMSK)
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_ADDR, m)
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_VALUE_BMSK                                  0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_VALUE_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR                                        (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002008)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR, HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_RMSK)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR, m)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR,v)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR,m,v,HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_IN)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_VALUE_BMSK                                  0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_VALUE_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR                                            (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000200c)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_RMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR, HWIO_SSC_QDSP6SS_PWRDBG_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PWRDBG_CTL_IN)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_ENA_BMSK                                               0x1
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_ENA_SHFT                                               0x0

#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002010)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_RMSK                                                0x7f
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR, HWIO_SSC_QDSP6SS_TEST_CLK_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_TEST_CLK_CTL_IN)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_BMSK                                  0x40
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_SHFT                                   0x6
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_BMSK                                 0x20
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_SHFT                                  0x5
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_BMSK                                    0x1f
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_BHS_TEST_ADDR                                              (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002014)
#define HWIO_SSC_QDSP6SS_BHS_TEST_RMSK                                                     0x1
#define HWIO_SSC_QDSP6SS_BHS_TEST_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_BHS_TEST_ADDR, HWIO_SSC_QDSP6SS_BHS_TEST_RMSK)
#define HWIO_SSC_QDSP6SS_BHS_TEST_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_BHS_TEST_ADDR, m)
#define HWIO_SSC_QDSP6SS_BHS_TEST_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_BHS_TEST_ADDR,v)
#define HWIO_SSC_QDSP6SS_BHS_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_BHS_TEST_ADDR,m,v,HWIO_SSC_QDSP6SS_BHS_TEST_IN)
#define HWIO_SSC_QDSP6SS_BHS_TEST_INSTANT_ON_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_BHS_TEST_INSTANT_ON_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002018)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_RMSK                                            0xffffff
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR, HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_RMSK)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR, m)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR,v)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR,m,v,HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_IN)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_BMSK                                0xffffe0
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_SHFT                                     0x5
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_BMSK                                        0x10
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_SHFT                                         0x4
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_BMSK                                 0x8
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_SHFT                                 0x3
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_BMSK                                    0x4
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_BMSK                                  0x1
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_CORE_STATUS_ADDR                                           (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002028)
#define HWIO_SSC_QDSP6SS_CORE_STATUS_RMSK                                                  0x7
#define HWIO_SSC_QDSP6SS_CORE_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_STATUS_ADDR, HWIO_SSC_QDSP6SS_CORE_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CP0_IDLE_BMSK                                         0x4
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CP0_IDLE_SHFT                                         0x2
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CORE_IN_SLEEP_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CORE_IN_SLEEP_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CORE_STATE_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CORE_STATE_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR                                             (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002030)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_RMSK                                             0xffffffff
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR, HWIO_SSC_QDSP6SS_MEM_DEBUG_RMSK)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR, m)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR,m,v,HWIO_SSC_QDSP6SS_MEM_DEBUG_IN)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OFF_BMSK                                         0xffffffff
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OFF_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002034)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR, HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_RMSK)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR, m)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR,v)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR,m,v,HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_IN)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OFF_BMSK                                      0xffffffff
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OFF_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR                                          (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x00002038)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR, HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_RMSK)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR, m)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR,v)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR,m,v,HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_IN)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OFF_BMSK                                      0xffffffff
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OFF_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR                                           (SSC_QDSP6V65SS_PUB_REG_BASE      + 0x0000203c)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_RMSK                                           0xffffffff
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR, HWIO_SSC_QDSP6SS_CPMEM_DEBUG_RMSK)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR,m,v,HWIO_SSC_QDSP6SS_CPMEM_DEBUG_IN)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OFF_BMSK                                       0xffffffff
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OFF_SHFT                                              0x0

/*----------------------------------------------------------------------------
 * MODULE: SCC_MCC_SSC_REG
 *--------------------------------------------------------------------------*/

#define SCC_MCC_SSC_REG_REG_BASE                                                 (LPASS_BASE      + 0x00b00000)

#define HWIO_SCC_SSCAON_SPARE_ADDR                                               (SCC_MCC_SSC_REG_REG_BASE      + 0x00000000)
#define HWIO_SCC_SSCAON_SPARE_RMSK                                               0xffffffff
#define HWIO_SCC_SSCAON_SPARE_IN          \
        in_dword_masked(HWIO_SCC_SSCAON_SPARE_ADDR, HWIO_SCC_SSCAON_SPARE_RMSK)
#define HWIO_SCC_SSCAON_SPARE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSCAON_SPARE_ADDR, m)
#define HWIO_SCC_SSCAON_SPARE_OUT(v)      \
        out_dword(HWIO_SCC_SSCAON_SPARE_ADDR,v)
#define HWIO_SCC_SSCAON_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSCAON_SPARE_ADDR,m,v,HWIO_SCC_SSCAON_SPARE_IN)
#define HWIO_SCC_SSCAON_SPARE_SSCAON_SSC_SPARE_IN_BMSK                           0xffff0000
#define HWIO_SCC_SSCAON_SPARE_SSCAON_SSC_SPARE_IN_SHFT                                 0x10
#define HWIO_SCC_SSCAON_SPARE_SSCAON_SSC_SPARE_OUT_BMSK                              0xffff
#define HWIO_SCC_SSCAON_SPARE_SSCAON_SSC_SPARE_OUT_SHFT                                 0x0

#define HWIO_SCC_SSCAON_SM_CSR1_ADDR                                             (SCC_MCC_SSC_REG_REG_BASE      + 0x00000004)
#define HWIO_SCC_SSCAON_SM_CSR1_RMSK                                                  0x1ff
#define HWIO_SCC_SSCAON_SM_CSR1_IN          \
        in_dword_masked(HWIO_SCC_SSCAON_SM_CSR1_ADDR, HWIO_SCC_SSCAON_SM_CSR1_RMSK)
#define HWIO_SCC_SSCAON_SM_CSR1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSCAON_SM_CSR1_ADDR, m)
#define HWIO_SCC_SSCAON_SM_CSR1_OUT(v)      \
        out_dword(HWIO_SCC_SSCAON_SM_CSR1_ADDR,v)
#define HWIO_SCC_SSCAON_SM_CSR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSCAON_SM_CSR1_ADDR,m,v,HWIO_SCC_SSCAON_SM_CSR1_IN)
#define HWIO_SCC_SSCAON_SM_CSR1_PLL_TMR_EN_BMSK                                       0x100
#define HWIO_SCC_SSCAON_SM_CSR1_PLL_TMR_EN_SHFT                                         0x8
#define HWIO_SCC_SSCAON_SM_CSR1_PLL_LOCKDLY_WR_BMSK                                    0x80
#define HWIO_SCC_SSCAON_SM_CSR1_PLL_LOCKDLY_WR_SHFT                                     0x7
#define HWIO_SCC_SSCAON_SM_CSR1_PLL_LOCKDLY_BMSK                                       0x7f
#define HWIO_SCC_SSCAON_SM_CSR1_PLL_LOCKDLY_SHFT                                        0x0

#define HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_ADDR                                 (SCC_MCC_SSC_REG_REG_BASE      + 0x00000008)
#define HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_RMSK                                        0x3
#define HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_IN          \
        in_dword_masked(HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_ADDR, HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_RMSK)
#define HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_ADDR, m)
#define HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_OUT(v)      \
        out_dword(HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_ADDR,v)
#define HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_ADDR,m,v,HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_IN)
#define HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_VAL_BMSK                                    0x3
#define HWIO_SCC_SSC_RSC_TS_PROFILING_EVENT_VAL_SHFT                                    0x0

#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR                                   (SCC_MCC_SSC_REG_REG_BASE      + 0x0000000c)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_RMSK                                     0x1fffff
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_IN          \
        in_dword_masked(HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR, HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_RMSK)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR, m)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR,v)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR,m,v,HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_IN)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_VAL_BMSK                                 0x1fffff
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_VAL_VAL_SHFT                                      0x0

#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR                                  (SCC_MCC_SSC_REG_REG_BASE      + 0x00000010)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_RMSK                                    0x1fffff
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_IN          \
        in_dword_masked(HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR, HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_RMSK)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR, m)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_OUT(v)      \
        out_dword(HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR,v)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR,m,v,HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_IN)
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_MASK_VAL_BMSK                           0x1fffff
#define HWIO_SCC_SSC_RSC_PWR_CTL_OVRD_MASK_MASK_VAL_SHFT                                0x0

#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR                                (SCC_MCC_SSC_REG_REG_BASE      + 0x00000014)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_RMSK                                  0x1fffff
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_IN          \
        in_dword_masked(HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR, HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_RMSK)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR, m)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR,v)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR,m,v,HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_IN)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_VAL_BMSK                              0x1fffff
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_VAL_VAL_SHFT                                   0x0

#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR                               (SCC_MCC_SSC_REG_REG_BASE      + 0x00000018)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_RMSK                                 0x1fffff
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_IN          \
        in_dword_masked(HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR, HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_RMSK)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR, m)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_OUT(v)      \
        out_dword(HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR,v)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR,m,v,HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_IN)
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_MASK_VAL_BMSK                        0x1fffff
#define HWIO_SCC_SSC_RSC_WAIT_EVENT_OVRD_MASK_MASK_VAL_SHFT                             0x0

#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR                                  (SCC_MCC_SSC_REG_REG_BASE      + 0x0000001c)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_RMSK                                         0xf
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_IN          \
        in_dword_masked(HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR, HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_RMSK)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR, m)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR,v)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR,m,v,HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_IN)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_VAL_BMSK                                     0xf
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_VAL_VAL_SHFT                                     0x0

#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR                                 (SCC_MCC_SSC_REG_REG_BASE      + 0x00000020)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_RMSK                                        0xf
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_IN          \
        in_dword_masked(HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR, HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_RMSK)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR, m)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_OUT(v)      \
        out_dword(HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR,v)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR,m,v,HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_IN)
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_MASK_VAL_BMSK                               0xf
#define HWIO_SCC_SSC_RSC_BR_EVENT_OVRD_MASK_MASK_VAL_SHFT                               0x0

#define HWIO_SCC_SSC_CCD_TESTBUS_SEL_ADDR                                        (SCC_MCC_SSC_REG_REG_BASE      + 0x00000024)
#define HWIO_SCC_SSC_CCD_TESTBUS_SEL_RMSK                                               0x3
#define HWIO_SCC_SSC_CCD_TESTBUS_SEL_IN          \
        in_dword_masked(HWIO_SCC_SSC_CCD_TESTBUS_SEL_ADDR, HWIO_SCC_SSC_CCD_TESTBUS_SEL_RMSK)
#define HWIO_SCC_SSC_CCD_TESTBUS_SEL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_CCD_TESTBUS_SEL_ADDR, m)
#define HWIO_SCC_SSC_CCD_TESTBUS_SEL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_CCD_TESTBUS_SEL_ADDR,v)
#define HWIO_SCC_SSC_CCD_TESTBUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_CCD_TESTBUS_SEL_ADDR,m,v,HWIO_SCC_SSC_CCD_TESTBUS_SEL_IN)
#define HWIO_SCC_SSC_CCD_TESTBUS_SEL_SEL_BMSK                                           0x3
#define HWIO_SCC_SSC_CCD_TESTBUS_SEL_SEL_SHFT                                           0x0

#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR                                    (SCC_MCC_SSC_REG_REG_BASE      + 0x00000028)
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_RMSK                                           0x3
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_IN          \
        in_dword_masked(HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR, HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_RMSK)
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR, m)
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_OUT(v)      \
        out_dword(HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR,v)
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR,m,v,HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_IN)
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_ISLAND_MODE_BMSK                               0x2
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_ISLAND_MODE_SHFT                               0x1
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_DISABLE_GCC_TO_SSC_CLKS_BMSK                   0x1
#define HWIO_SCC_SSC_GCC_TO_SSC_CLKS_OFF_DISABLE_GCC_TO_SSC_CLKS_SHFT                   0x0

#define HWIO_SCC_SSC_TEST_BUS_SEL_CFG_ADDR                                       (SCC_MCC_SSC_REG_REG_BASE      + 0x00001000)
#define HWIO_SCC_SSC_TEST_BUS_SEL_CFG_RMSK                                              0x1
#define HWIO_SCC_SSC_TEST_BUS_SEL_CFG_IN          \
        in_dword_masked(HWIO_SCC_SSC_TEST_BUS_SEL_CFG_ADDR, HWIO_SCC_SSC_TEST_BUS_SEL_CFG_RMSK)
#define HWIO_SCC_SSC_TEST_BUS_SEL_CFG_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_TEST_BUS_SEL_CFG_ADDR, m)
#define HWIO_SCC_SSC_TEST_BUS_SEL_CFG_OUT(v)      \
        out_dword(HWIO_SCC_SSC_TEST_BUS_SEL_CFG_ADDR,v)
#define HWIO_SCC_SSC_TEST_BUS_SEL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_TEST_BUS_SEL_CFG_ADDR,m,v,HWIO_SCC_SSC_TEST_BUS_SEL_CFG_IN)
#define HWIO_SCC_SSC_TEST_BUS_SEL_CFG_SSC_TEST_BUS_SEL_BMSK                             0x1
#define HWIO_SCC_SSC_TEST_BUS_SEL_CFG_SSC_TEST_BUS_SEL_SHFT                             0x0

#define HWIO_SCC_SSC_HW_VERSION_ADDR                                             (SCC_MCC_SSC_REG_REG_BASE      + 0x00001004)
#define HWIO_SCC_SSC_HW_VERSION_RMSK                                             0xffffffff
#define HWIO_SCC_SSC_HW_VERSION_IN          \
        in_dword_masked(HWIO_SCC_SSC_HW_VERSION_ADDR, HWIO_SCC_SSC_HW_VERSION_RMSK)
#define HWIO_SCC_SSC_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_HW_VERSION_ADDR, m)
#define HWIO_SCC_SSC_HW_VERSION_MAJOR_BMSK                                       0xf0000000
#define HWIO_SCC_SSC_HW_VERSION_MAJOR_SHFT                                             0x1c
#define HWIO_SCC_SSC_HW_VERSION_MINOR_BMSK                                        0xfff0000
#define HWIO_SCC_SSC_HW_VERSION_MINOR_SHFT                                             0x10
#define HWIO_SCC_SSC_HW_VERSION_STEP_BMSK                                            0xffff
#define HWIO_SCC_SSC_HW_VERSION_STEP_SHFT                                               0x0

#define HWIO_SCC_SSC_XO_CLK_GATE_ADDR                                            (SCC_MCC_SSC_REG_REG_BASE      + 0x00001008)
#define HWIO_SCC_SSC_XO_CLK_GATE_RMSK                                                   0x1
#define HWIO_SCC_SSC_XO_CLK_GATE_IN          \
        in_dword_masked(HWIO_SCC_SSC_XO_CLK_GATE_ADDR, HWIO_SCC_SSC_XO_CLK_GATE_RMSK)
#define HWIO_SCC_SSC_XO_CLK_GATE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_XO_CLK_GATE_ADDR, m)
#define HWIO_SCC_SSC_XO_CLK_GATE_OUT(v)      \
        out_dword(HWIO_SCC_SSC_XO_CLK_GATE_ADDR,v)
#define HWIO_SCC_SSC_XO_CLK_GATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_XO_CLK_GATE_ADDR,m,v,HWIO_SCC_SSC_XO_CLK_GATE_IN)
#define HWIO_SCC_SSC_XO_CLK_GATE_SSC_XO_CLK_GATE_EN_BMSK                                0x1
#define HWIO_SCC_SSC_XO_CLK_GATE_SSC_XO_CLK_GATE_EN_SHFT                                0x0

#define HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_ADDR                                 (SCC_MCC_SSC_REG_REG_BASE      + 0x0000100c)
#define HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_RMSK                                        0x1
#define HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_IN          \
        in_dword_masked(HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_ADDR, HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_RMSK)
#define HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_ADDR, m)
#define HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_DISABLE_BMSK                                0x1
#define HWIO_SCC_SSC_SW_ISLAND_MODE_DISABLE_DISABLE_SHFT                                0x0

#define HWIO_SCC_SSC_I3C0_CFG_ADDR                                               (SCC_MCC_SSC_REG_REG_BASE      + 0x00001010)
#define HWIO_SCC_SSC_I3C0_CFG_RMSK                                                     0x3f
#define HWIO_SCC_SSC_I3C0_CFG_IN          \
        in_dword_masked(HWIO_SCC_SSC_I3C0_CFG_ADDR, HWIO_SCC_SSC_I3C0_CFG_RMSK)
#define HWIO_SCC_SSC_I3C0_CFG_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_I3C0_CFG_ADDR, m)
#define HWIO_SCC_SSC_I3C0_CFG_OUT(v)      \
        out_dword(HWIO_SCC_SSC_I3C0_CFG_ADDR,v)
#define HWIO_SCC_SSC_I3C0_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_I3C0_CFG_ADDR,m,v,HWIO_SCC_SSC_I3C0_CFG_IN)
#define HWIO_SCC_SSC_I3C0_CFG_SDA_I3C0_CFG_MODE_BMSK                                   0x38
#define HWIO_SCC_SSC_I3C0_CFG_SDA_I3C0_CFG_MODE_SHFT                                    0x3
#define HWIO_SCC_SSC_I3C0_CFG_SCL_I3C0_CFG_MODE_BMSK                                    0x7
#define HWIO_SCC_SSC_I3C0_CFG_SCL_I3C0_CFG_MODE_SHFT                                    0x0

#define HWIO_SCC_SSC_I3C1_CFG_ADDR                                               (SCC_MCC_SSC_REG_REG_BASE      + 0x00001014)
#define HWIO_SCC_SSC_I3C1_CFG_RMSK                                                     0x3f
#define HWIO_SCC_SSC_I3C1_CFG_IN          \
        in_dword_masked(HWIO_SCC_SSC_I3C1_CFG_ADDR, HWIO_SCC_SSC_I3C1_CFG_RMSK)
#define HWIO_SCC_SSC_I3C1_CFG_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_I3C1_CFG_ADDR, m)
#define HWIO_SCC_SSC_I3C1_CFG_OUT(v)      \
        out_dword(HWIO_SCC_SSC_I3C1_CFG_ADDR,v)
#define HWIO_SCC_SSC_I3C1_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_I3C1_CFG_ADDR,m,v,HWIO_SCC_SSC_I3C1_CFG_IN)
#define HWIO_SCC_SSC_I3C1_CFG_SDA_I3C1_CFG_MODE_BMSK                                   0x38
#define HWIO_SCC_SSC_I3C1_CFG_SDA_I3C1_CFG_MODE_SHFT                                    0x3
#define HWIO_SCC_SSC_I3C1_CFG_SCL_I3C1_CFG_MODE_BMSK                                    0x7
#define HWIO_SCC_SSC_I3C1_CFG_SCL_I3C1_CFG_MODE_SHFT                                    0x0

#define HWIO_SCC_SSC_IEC_CTL_ADDR                                                (SCC_MCC_SSC_REG_REG_BASE      + 0x00002000)
#define HWIO_SCC_SSC_IEC_CTL_RMSK                                                      0x7f
#define HWIO_SCC_SSC_IEC_CTL_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_CTL_ADDR, HWIO_SCC_SSC_IEC_CTL_RMSK)
#define HWIO_SCC_SSC_IEC_CTL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_CTL_ADDR, m)
#define HWIO_SCC_SSC_IEC_CTL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_CTL_ADDR,v)
#define HWIO_SCC_SSC_IEC_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_CTL_ADDR,m,v,HWIO_SCC_SSC_IEC_CTL_IN)
#define HWIO_SCC_SSC_IEC_CTL_BUS_ISO_ENTER_ISLAND_DIS_BMSK                             0x40
#define HWIO_SCC_SSC_IEC_CTL_BUS_ISO_ENTER_ISLAND_DIS_SHFT                              0x6
#define HWIO_SCC_SSC_IEC_CTL_L2_MISS_ISLAND_EXIT_EN_BMSK                               0x20
#define HWIO_SCC_SSC_IEC_CTL_L2_MISS_ISLAND_EXIT_EN_SHFT                                0x5
#define HWIO_SCC_SSC_IEC_CTL_ISO_AHB_MASTER_BYPASS_BMSK                                0x10
#define HWIO_SCC_SSC_IEC_CTL_ISO_AHB_MASTER_BYPASS_SHFT                                 0x4
#define HWIO_SCC_SSC_IEC_CTL_ISO_AHB_SLAVE_BYPASS_BMSK                                  0x8
#define HWIO_SCC_SSC_IEC_CTL_ISO_AHB_SLAVE_BYPASS_SHFT                                  0x3
#define HWIO_SCC_SSC_IEC_CTL_AHB_REQPEND_CLR_BMSK                                       0x4
#define HWIO_SCC_SSC_IEC_CTL_AHB_REQPEND_CLR_SHFT                                       0x2
#define HWIO_SCC_SSC_IEC_CTL_ENTER_ISLAND_MODE_IRQ_CLR_BMSK                             0x2
#define HWIO_SCC_SSC_IEC_CTL_ENTER_ISLAND_MODE_IRQ_CLR_SHFT                             0x1
#define HWIO_SCC_SSC_IEC_CTL_EXIT_ISLAND_MODE_TRIG_BMSK                                 0x1
#define HWIO_SCC_SSC_IEC_CTL_EXIT_ISLAND_MODE_TRIG_SHFT                                 0x0

#define HWIO_SCC_SSC_IEC_STAT_ADDR                                               (SCC_MCC_SSC_REG_REG_BASE      + 0x00002004)
#define HWIO_SCC_SSC_IEC_STAT_RMSK                                                      0x7
#define HWIO_SCC_SSC_IEC_STAT_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_STAT_ADDR, HWIO_SCC_SSC_IEC_STAT_RMSK)
#define HWIO_SCC_SSC_IEC_STAT_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_STAT_ADDR, m)
#define HWIO_SCC_SSC_IEC_STAT_AHB_REQPEND_STATUS_SDC_BMSK                               0x4
#define HWIO_SCC_SSC_IEC_STAT_AHB_REQPEND_STATUS_SDC_SHFT                               0x2
#define HWIO_SCC_SSC_IEC_STAT_AHB_REQPEND_STATUS_QUP_BMSK                               0x2
#define HWIO_SCC_SSC_IEC_STAT_AHB_REQPEND_STATUS_QUP_SHFT                               0x1
#define HWIO_SCC_SSC_IEC_STAT_ISLAND_MODE_BMSK                                          0x1
#define HWIO_SCC_SSC_IEC_STAT_ISLAND_MODE_SHFT                                          0x0

#define HWIO_SCC_SSC_IEC_BSY_WAIT_ADDR                                           (SCC_MCC_SSC_REG_REG_BASE      + 0x00002008)
#define HWIO_SCC_SSC_IEC_BSY_WAIT_RMSK                                                  0x3
#define HWIO_SCC_SSC_IEC_BSY_WAIT_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_BSY_WAIT_ADDR, HWIO_SCC_SSC_IEC_BSY_WAIT_RMSK)
#define HWIO_SCC_SSC_IEC_BSY_WAIT_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_BSY_WAIT_ADDR, m)
#define HWIO_SCC_SSC_IEC_BSY_WAIT_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_BSY_WAIT_ADDR,v)
#define HWIO_SCC_SSC_IEC_BSY_WAIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_BSY_WAIT_ADDR,m,v,HWIO_SCC_SSC_IEC_BSY_WAIT_IN)
#define HWIO_SCC_SSC_IEC_BSY_WAIT_BUSY_WAIT_EN_BMSK                                     0x2
#define HWIO_SCC_SSC_IEC_BSY_WAIT_BUSY_WAIT_EN_SHFT                                     0x1
#define HWIO_SCC_SSC_IEC_BSY_WAIT_BUSY_WAIT_RST_BMSK                                    0x1
#define HWIO_SCC_SSC_IEC_BSY_WAIT_BUSY_WAIT_RST_SHFT                                    0x0

#define HWIO_SCC_SSC_IEC_SQCNTR_VAL0_ADDR                                        (SCC_MCC_SSC_REG_REG_BASE      + 0x0000200c)
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL0_RMSK                                        0xffffffff
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL0_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_SQCNTR_VAL0_ADDR, HWIO_SCC_SSC_IEC_SQCNTR_VAL0_RMSK)
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL0_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_SQCNTR_VAL0_ADDR, m)
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL0_SQCNTR_VAL0_BMSK                            0xffffffff
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL0_SQCNTR_VAL0_SHFT                                   0x0

#define HWIO_SCC_SSC_IEC_SQCNTR_VAL1_ADDR                                        (SCC_MCC_SSC_REG_REG_BASE      + 0x00002010)
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL1_RMSK                                          0xffffff
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL1_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_SQCNTR_VAL1_ADDR, HWIO_SCC_SSC_IEC_SQCNTR_VAL1_RMSK)
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_SQCNTR_VAL1_ADDR, m)
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL1_SQCNTR_VAL1_BMSK                              0xffffff
#define HWIO_SCC_SSC_IEC_SQCNTR_VAL1_SQCNTR_VAL1_SHFT                                   0x0

#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_ADDR                                (SCC_MCC_SSC_REG_REG_BASE      + 0x00002014)
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_RMSK                                0xffffffff
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_ADDR, HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_RMSK)
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_ADDR, m)
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_BUSY_WAIT_CNTR_VAL0_BMSK            0xffffffff
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_BUSY_WAIT_CNTR_VAL0_SHFT                   0x0

#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_ADDR                                (SCC_MCC_SSC_REG_REG_BASE      + 0x00002018)
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_RMSK                                  0xffffff
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_ADDR, HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_RMSK)
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_ADDR, m)
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_BUSY_WAIT_CNTR_VAL1_BMSK              0xffffff
#define HWIO_SCC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_BUSY_WAIT_CNTR_VAL1_SHFT                   0x0

#define HWIO_SCC_SSC_IEC_HADDR_VAL0_ADDR                                         (SCC_MCC_SSC_REG_REG_BASE      + 0x0000201c)
#define HWIO_SCC_SSC_IEC_HADDR_VAL0_RMSK                                         0xffffffff
#define HWIO_SCC_SSC_IEC_HADDR_VAL0_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HADDR_VAL0_ADDR, HWIO_SCC_SSC_IEC_HADDR_VAL0_RMSK)
#define HWIO_SCC_SSC_IEC_HADDR_VAL0_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HADDR_VAL0_ADDR, m)
#define HWIO_SCC_SSC_IEC_HADDR_VAL0_HADDR_VAL0_BMSK                              0xffffffff
#define HWIO_SCC_SSC_IEC_HADDR_VAL0_HADDR_VAL0_SHFT                                     0x0

#define HWIO_SCC_SSC_IEC_HADDR_VAL1_ADDR                                         (SCC_MCC_SSC_REG_REG_BASE      + 0x00002020)
#define HWIO_SCC_SSC_IEC_HADDR_VAL1_RMSK                                                0xf
#define HWIO_SCC_SSC_IEC_HADDR_VAL1_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HADDR_VAL1_ADDR, HWIO_SCC_SSC_IEC_HADDR_VAL1_RMSK)
#define HWIO_SCC_SSC_IEC_HADDR_VAL1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HADDR_VAL1_ADDR, m)
#define HWIO_SCC_SSC_IEC_HADDR_VAL1_HADDR_VAL1_BMSK                                     0xf
#define HWIO_SCC_SSC_IEC_HADDR_VAL1_HADDR_VAL1_SHFT                                     0x0

#define HWIO_SCC_SSC_IEC_HMID_VAL_ADDR                                           (SCC_MCC_SSC_REG_REG_BASE      + 0x00002024)
#define HWIO_SCC_SSC_IEC_HMID_VAL_RMSK                                                 0xff
#define HWIO_SCC_SSC_IEC_HMID_VAL_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HMID_VAL_ADDR, HWIO_SCC_SSC_IEC_HMID_VAL_RMSK)
#define HWIO_SCC_SSC_IEC_HMID_VAL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HMID_VAL_ADDR, m)
#define HWIO_SCC_SSC_IEC_HMID_VAL_SSC_IEC_HMID_VAL_BMSK                                0xff
#define HWIO_SCC_SSC_IEC_HMID_VAL_SSC_IEC_HMID_VAL_SHFT                                 0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x00002028)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL0_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x0000202c)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL1_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x00002030)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL2_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x00002034)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL3_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x00002038)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL4_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x0000203c)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL5_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x00002040)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL6_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x00002044)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL7_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x00002048)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL8_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x0000204c)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_RMSK                                      0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL9_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002050)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL10_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002054)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL11_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002058)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL12_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x0000205c)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL13_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002060)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL14_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002064)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL15_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002068)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL16_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x0000206c)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL17_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002070)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL18_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002074)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL19_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002078)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL20_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x0000207c)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL21_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002080)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL22_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002084)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL23_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002088)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL24_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x0000208c)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL25_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002090)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL26_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002094)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL27_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002098)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL28_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x0000209c)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL29_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002100)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL30_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002104)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_SEL31_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SCC_SSC_IEC_HWEVENTS_EN_ADDR                                        (SCC_MCC_SSC_REG_REG_BASE      + 0x00002108)
#define HWIO_SCC_SSC_IEC_HWEVENTS_EN_RMSK                                        0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_EN_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_EN_ADDR, HWIO_SCC_SSC_IEC_HWEVENTS_EN_RMSK)
#define HWIO_SCC_SSC_IEC_HWEVENTS_EN_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HWEVENTS_EN_ADDR, m)
#define HWIO_SCC_SSC_IEC_HWEVENTS_EN_OUT(v)      \
        out_dword(HWIO_SCC_SSC_IEC_HWEVENTS_EN_ADDR,v)
#define HWIO_SCC_SSC_IEC_HWEVENTS_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_IEC_HWEVENTS_EN_ADDR,m,v,HWIO_SCC_SSC_IEC_HWEVENTS_EN_IN)
#define HWIO_SCC_SSC_IEC_HWEVENTS_EN_HWEVENTS_EN_BMSK                            0xffffffff
#define HWIO_SCC_SSC_IEC_HWEVENTS_EN_HWEVENTS_EN_SHFT                                   0x0

#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x0000210c)
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_RMSK                                     0xffffffff
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_ADDR, HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_RMSK)
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_ADDR, m)
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_HADDR_VAL0_BMSK                          0xffffffff
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL0_HADDR_VAL0_SHFT                                 0x0

#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x00002110)
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_RMSK                                            0xf
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_ADDR, HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_RMSK)
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_ADDR, m)
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_HADDR_VAL1_BMSK                                 0xf
#define HWIO_SCC_SSC_IEC_HADDR_SDC_VAL1_HADDR_VAL1_SHFT                                 0x0

#define HWIO_SCC_SSC_IEC_HMID_SDC_VAL_ADDR                                       (SCC_MCC_SSC_REG_REG_BASE      + 0x00002114)
#define HWIO_SCC_SSC_IEC_HMID_SDC_VAL_RMSK                                             0xff
#define HWIO_SCC_SSC_IEC_HMID_SDC_VAL_IN          \
        in_dword_masked(HWIO_SCC_SSC_IEC_HMID_SDC_VAL_ADDR, HWIO_SCC_SSC_IEC_HMID_SDC_VAL_RMSK)
#define HWIO_SCC_SSC_IEC_HMID_SDC_VAL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_IEC_HMID_SDC_VAL_ADDR, m)
#define HWIO_SCC_SSC_IEC_HMID_SDC_VAL_SSC_IEC_HMID_VAL_BMSK                            0xff
#define HWIO_SCC_SSC_IEC_HMID_SDC_VAL_SSC_IEC_HMID_VAL_SHFT                             0x0

#define HWIO_SCC_SSC_DATA_BUS_CONFIG_ADDR                                        (SCC_MCC_SSC_REG_REG_BASE      + 0x00003000)
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_RMSK                                             0xfff
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_IN          \
        in_dword_masked(HWIO_SCC_SSC_DATA_BUS_CONFIG_ADDR, HWIO_SCC_SSC_DATA_BUS_CONFIG_RMSK)
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_DATA_BUS_CONFIG_ADDR, m)
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_OUT(v)      \
        out_dword(HWIO_SCC_SSC_DATA_BUS_CONFIG_ADDR,v)
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_DATA_BUS_CONFIG_ADDR,m,v,HWIO_SCC_SSC_DATA_BUS_CONFIG_IN)
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_AHBE_GLOBAL_EN_BMSK                              0x800
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_AHBE_GLOBAL_EN_SHFT                                0xb
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_AHBE_TIMEBASE_BMSK                               0x400
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_AHBE_TIMEBASE_SHFT                                 0xa
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_LOCK_EN_ARRAY_BMSK                               0x380
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_LOCK_EN_ARRAY_SHFT                                 0x7
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_PRIORITY_ARRAY_BMSK                               0x7e
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_PRIORITY_ARRAY_SHFT                                0x1
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_ARB_SCHEME_SEL_BMSK                                0x1
#define HWIO_SCC_SSC_DATA_BUS_CONFIG_ARB_SCHEME_SEL_SHFT                                0x0

#define HWIO_SCC_SSC_DATA_BUS_STATUS_ADDR                                        (SCC_MCC_SSC_REG_REG_BASE      + 0x00003004)
#define HWIO_SCC_SSC_DATA_BUS_STATUS_RMSK                                               0x7
#define HWIO_SCC_SSC_DATA_BUS_STATUS_IN          \
        in_dword_masked(HWIO_SCC_SSC_DATA_BUS_STATUS_ADDR, HWIO_SCC_SSC_DATA_BUS_STATUS_RMSK)
#define HWIO_SCC_SSC_DATA_BUS_STATUS_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_DATA_BUS_STATUS_ADDR, m)
#define HWIO_SCC_SSC_DATA_BUS_STATUS_AHBE_STATUS_BMSK                                   0x7
#define HWIO_SCC_SSC_DATA_BUS_STATUS_AHBE_STATUS_SHFT                                   0x0

#define HWIO_SCC_SSC_CFG_BUS_CONFIG_ADDR                                         (SCC_MCC_SSC_REG_REG_BASE      + 0x00004000)
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_RMSK                                              0xfff
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_IN          \
        in_dword_masked(HWIO_SCC_SSC_CFG_BUS_CONFIG_ADDR, HWIO_SCC_SSC_CFG_BUS_CONFIG_RMSK)
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_CFG_BUS_CONFIG_ADDR, m)
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_OUT(v)      \
        out_dword(HWIO_SCC_SSC_CFG_BUS_CONFIG_ADDR,v)
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_CFG_BUS_CONFIG_ADDR,m,v,HWIO_SCC_SSC_CFG_BUS_CONFIG_IN)
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_AHBE_GLOBAL_EN_BMSK                               0x800
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_AHBE_GLOBAL_EN_SHFT                                 0xb
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_AHBE_TIMEBASE_BMSK                                0x400
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_AHBE_TIMEBASE_SHFT                                  0xa
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_LOCK_EN_ARRAY_BMSK                                0x380
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_LOCK_EN_ARRAY_SHFT                                  0x7
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_PRIORITY_ARRAY_BMSK                                0x7e
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_PRIORITY_ARRAY_SHFT                                 0x1
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_ARB_SCHEME_SEL_BMSK                                 0x1
#define HWIO_SCC_SSC_CFG_BUS_CONFIG_ARB_SCHEME_SEL_SHFT                                 0x0

#define HWIO_SCC_SSC_CFG_BUS_STATUS_ADDR                                         (SCC_MCC_SSC_REG_REG_BASE      + 0x00004004)
#define HWIO_SCC_SSC_CFG_BUS_STATUS_RMSK                                                0x7
#define HWIO_SCC_SSC_CFG_BUS_STATUS_IN          \
        in_dword_masked(HWIO_SCC_SSC_CFG_BUS_STATUS_ADDR, HWIO_SCC_SSC_CFG_BUS_STATUS_RMSK)
#define HWIO_SCC_SSC_CFG_BUS_STATUS_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_CFG_BUS_STATUS_ADDR, m)
#define HWIO_SCC_SSC_CFG_BUS_STATUS_AHBE_STATUS_BMSK                                    0x7
#define HWIO_SCC_SSC_CFG_BUS_STATUS_AHBE_STATUS_SHFT                                    0x0

#define HWIO_SCC_SSC_TSGEN_PRELOAD_LO_ADDR                                       (SCC_MCC_SSC_REG_REG_BASE      + 0x00005000)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_LO_RMSK                                       0xffffffff
#define HWIO_SCC_SSC_TSGEN_PRELOAD_LO_IN          \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_PRELOAD_LO_ADDR, HWIO_SCC_SSC_TSGEN_PRELOAD_LO_RMSK)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_LO_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_PRELOAD_LO_ADDR, m)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_LO_OUT(v)      \
        out_dword(HWIO_SCC_SSC_TSGEN_PRELOAD_LO_ADDR,v)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_TSGEN_PRELOAD_LO_ADDR,m,v,HWIO_SCC_SSC_TSGEN_PRELOAD_LO_IN)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_LO_TSGEN_PRELOAD_LO_BMSK                      0xffffffff
#define HWIO_SCC_SSC_TSGEN_PRELOAD_LO_TSGEN_PRELOAD_LO_SHFT                             0x0

#define HWIO_SCC_SSC_TSGEN_PRELOAD_HI_ADDR                                       (SCC_MCC_SSC_REG_REG_BASE      + 0x00005004)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_HI_RMSK                                       0xffffffff
#define HWIO_SCC_SSC_TSGEN_PRELOAD_HI_IN          \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_PRELOAD_HI_ADDR, HWIO_SCC_SSC_TSGEN_PRELOAD_HI_RMSK)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_HI_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_PRELOAD_HI_ADDR, m)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_HI_OUT(v)      \
        out_dword(HWIO_SCC_SSC_TSGEN_PRELOAD_HI_ADDR,v)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_TSGEN_PRELOAD_HI_ADDR,m,v,HWIO_SCC_SSC_TSGEN_PRELOAD_HI_IN)
#define HWIO_SCC_SSC_TSGEN_PRELOAD_HI_TSGEN_PRELOAD_HI_BMSK                      0xffffffff
#define HWIO_SCC_SSC_TSGEN_PRELOAD_HI_TSGEN_PRELOAD_HI_SHFT                             0x0

#define HWIO_SCC_SSC_TSGEN_CTR_LO_ADDR                                           (SCC_MCC_SSC_REG_REG_BASE      + 0x00005008)
#define HWIO_SCC_SSC_TSGEN_CTR_LO_RMSK                                           0xffffffff
#define HWIO_SCC_SSC_TSGEN_CTR_LO_IN          \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_CTR_LO_ADDR, HWIO_SCC_SSC_TSGEN_CTR_LO_RMSK)
#define HWIO_SCC_SSC_TSGEN_CTR_LO_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_CTR_LO_ADDR, m)
#define HWIO_SCC_SSC_TSGEN_CTR_LO_TSGEN_CTR_LO_BMSK                              0xffffffff
#define HWIO_SCC_SSC_TSGEN_CTR_LO_TSGEN_CTR_LO_SHFT                                     0x0

#define HWIO_SCC_SSC_TSGEN_CTR_HI_ADDR                                           (SCC_MCC_SSC_REG_REG_BASE      + 0x0000500c)
#define HWIO_SCC_SSC_TSGEN_CTR_HI_RMSK                                           0xffffffff
#define HWIO_SCC_SSC_TSGEN_CTR_HI_IN          \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_CTR_HI_ADDR, HWIO_SCC_SSC_TSGEN_CTR_HI_RMSK)
#define HWIO_SCC_SSC_TSGEN_CTR_HI_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_CTR_HI_ADDR, m)
#define HWIO_SCC_SSC_TSGEN_CTR_HI_TSGEN_CTR_HI_BMSK                              0xffffffff
#define HWIO_SCC_SSC_TSGEN_CTR_HI_TSGEN_CTR_HI_SHFT                                     0x0

#define HWIO_SCC_SSC_TSGEN_CTL_ADDR                                              (SCC_MCC_SSC_REG_REG_BASE      + 0x00005010)
#define HWIO_SCC_SSC_TSGEN_CTL_RMSK                                                     0x3
#define HWIO_SCC_SSC_TSGEN_CTL_IN          \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_CTL_ADDR, HWIO_SCC_SSC_TSGEN_CTL_RMSK)
#define HWIO_SCC_SSC_TSGEN_CTL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_TSGEN_CTL_ADDR, m)
#define HWIO_SCC_SSC_TSGEN_CTL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_TSGEN_CTL_ADDR,v)
#define HWIO_SCC_SSC_TSGEN_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_TSGEN_CTL_ADDR,m,v,HWIO_SCC_SSC_TSGEN_CTL_IN)
#define HWIO_SCC_SSC_TSGEN_CTL_TSGEN_PRELOAD_EN_BMSK                                    0x2
#define HWIO_SCC_SSC_TSGEN_CTL_TSGEN_PRELOAD_EN_SHFT                                    0x1
#define HWIO_SCC_SSC_TSGEN_CTL_TSGEN_DISABLE_CNT_BMSK                                   0x1
#define HWIO_SCC_SSC_TSGEN_CTL_TSGEN_DISABLE_CNT_SHFT                                   0x0

#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR                                (SCC_MCC_SSC_REG_REG_BASE      + 0x00005014)
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR, HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SCC_SSC_ATB_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR                                (SCC_MCC_SSC_REG_REG_BASE      + 0x00005018)
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR, HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SCC_SSC_APB_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR                                (SCC_MCC_SSC_REG_REG_BASE      + 0x0000501c)
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR, HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SCC_SSC_DAP_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR                                (SCC_MCC_SSC_REG_REG_BASE      + 0x00005020)
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR, HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SCC_SSC_NTS_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ADDR                           (SCC_MCC_SSC_REG_REG_BASE      + 0x00005024)
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_RMSK                                  0x7
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ADDR, HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_REQUEST_BMSK                          0x4
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_REQUEST_SHFT                          0x2
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                           0x2
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                           0x1
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ACK_BMSK                              0x1
#define HWIO_SCC_SSC_ATB2QDSS_LOW_POWER_HANDSHAKE_ACK_SHFT                              0x0

#define HWIO_SCC_SSC_HM_TESTBUS_SEL_ADDR                                         (SCC_MCC_SSC_REG_REG_BASE      + 0x00006000)
#define HWIO_SCC_SSC_HM_TESTBUS_SEL_RMSK                                          0xfffffff
#define HWIO_SCC_SSC_HM_TESTBUS_SEL_IN          \
        in_dword_masked(HWIO_SCC_SSC_HM_TESTBUS_SEL_ADDR, HWIO_SCC_SSC_HM_TESTBUS_SEL_RMSK)
#define HWIO_SCC_SSC_HM_TESTBUS_SEL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_HM_TESTBUS_SEL_ADDR, m)
#define HWIO_SCC_SSC_HM_TESTBUS_SEL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_HM_TESTBUS_SEL_ADDR,v)
#define HWIO_SCC_SSC_HM_TESTBUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_HM_TESTBUS_SEL_ADDR,m,v,HWIO_SCC_SSC_HM_TESTBUS_SEL_IN)
#define HWIO_SCC_SSC_HM_TESTBUS_SEL_I_TESTBUS_SEL_BMSK                            0xfffffff
#define HWIO_SCC_SSC_HM_TESTBUS_SEL_I_TESTBUS_SEL_SHFT                                  0x0

#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR                          (SCC_MCC_SSC_REG_REG_BASE      + 0x00007000)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RMSK                          0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_IN          \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR, HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RMSK)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR, m)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR,v)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR,m,v,HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_IN)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RFU_BMSK                      0xfffffffc
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RFU_SHFT                             0x2
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_BMSK                 0x2
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_SHFT                 0x1
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_CAPTURE_BMSK                         0x1
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_CAPTURE_SHFT                         0x0

#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR                             (SCC_MCC_SSC_REG_REG_BASE      + 0x00007004)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_RMSK                             0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_IN          \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR, HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_RMSK)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR, m)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_TIMEVAL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_TIMEVAL_SHFT                            0x0

#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR                             (SCC_MCC_SSC_REG_REG_BASE      + 0x00007008)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_RMSK                             0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_IN          \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR, HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_RMSK)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR, m)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_TIMEVAL_BMSK                     0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_TIMEVAL_SHFT                            0x0

#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR                           (SCC_MCC_SSC_REG_REG_BASE      + 0x0000700c)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_RMSK                           0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_IN          \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR, HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_RMSK)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR, m)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_TIMEVAL_BMSK                   0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_TIMEVAL_SHFT                          0x0

#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR                           (SCC_MCC_SSC_REG_REG_BASE      + 0x00007010)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_RMSK                           0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_IN          \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR, HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_RMSK)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR, m)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_TIMEVAL_BMSK                   0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_TIMEVAL_SHFT                          0x0

#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR                          (SCC_MCC_SSC_REG_REG_BASE      + 0x00007014)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_RMSK                          0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_IN          \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR, HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_RMSK)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR, m)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OUT(v)      \
        out_dword(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR,v)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR,m,v,HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_IN)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_TIMELOAD_BMSK                 0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_TIMELOAD_SHFT                        0x0

#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR                          (SCC_MCC_SSC_REG_REG_BASE      + 0x00007018)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_RMSK                          0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_IN          \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR, HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_RMSK)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR, m)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OUT(v)      \
        out_dword(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR,v)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR,m,v,HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_IN)
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_TIMELOAD_BMSK                 0xffffffff
#define HWIO_SCC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_TIMELOAD_SHFT                        0x0

#define HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x00008000)
#define HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_RMSK                                           0x1ff
#define HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_IN          \
        in_dword_masked(HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_ADDR, HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_RMSK)
#define HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_ADDR, m)
#define HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_OUT(v)      \
        out_dword(HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_ADDR,v)
#define HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_ADDR,m,v,HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_IN)
#define HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_WAKEUP_IE_BMSK                                 0x1ff
#define HWIO_SCC_SSC_WAKEUP_PRIMARY_IE_WAKEUP_IE_SHFT                                   0x0

#define HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_ADDR                                    (SCC_MCC_SSC_REG_REG_BASE      + 0x00009000)
#define HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_RMSK                                         0x1ff
#define HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_IN          \
        in_dword_masked(HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_ADDR, HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_RMSK)
#define HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_ADDR, m)
#define HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_OUT(v)      \
        out_dword(HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_ADDR,v)
#define HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_ADDR,m,v,HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_IN)
#define HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_WAKEUP_IE_BMSK                               0x1ff
#define HWIO_SCC_SSC_WAKEUP_SECONDARY_IE_WAKEUP_IE_SHFT                                 0x0

#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR                                     (SCC_MCC_SSC_REG_REG_BASE      + 0x0000b000)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_RMSK                                            0x1
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_IN          \
        in_dword_masked(HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR, HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_RMSK)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR, m)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_OUT(v)      \
        out_dword(HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR,v)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR,m,v,HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_IN)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_SEL_BMSK                                        0x1
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_SEL_SEL_SHFT                                        0x0

#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR                                    (SCC_MCC_SSC_REG_REG_BASE      + 0x0000b004)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_RMSK                                     0xfffffff
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_IN          \
        in_dword_masked(HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR, HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_RMSK)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR, m)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_OUT(v)      \
        out_dword(HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR,v)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR,m,v,HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_IN)
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR_BMSK                                0xffffff0
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR_SHFT                                      0x4
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_RSVD_BIT3_0_BMSK                               0xf
#define HWIO_SCC_SSC_EFUSE_Q6SS_EVB_ADDR_RSVD_BIT3_0_SHFT                               0x0

#define HWIO_SCC_SSC_ALT_RESET_Q6SS_ADDR                                         (SCC_MCC_SSC_REG_REG_BASE      + 0x0000b008)
#define HWIO_SCC_SSC_ALT_RESET_Q6SS_RMSK                                                0x1
#define HWIO_SCC_SSC_ALT_RESET_Q6SS_IN          \
        in_dword_masked(HWIO_SCC_SSC_ALT_RESET_Q6SS_ADDR, HWIO_SCC_SSC_ALT_RESET_Q6SS_RMSK)
#define HWIO_SCC_SSC_ALT_RESET_Q6SS_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_ALT_RESET_Q6SS_ADDR, m)
#define HWIO_SCC_SSC_ALT_RESET_Q6SS_OUT(v)      \
        out_dword(HWIO_SCC_SSC_ALT_RESET_Q6SS_ADDR,v)
#define HWIO_SCC_SSC_ALT_RESET_Q6SS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_ALT_RESET_Q6SS_ADDR,m,v,HWIO_SCC_SSC_ALT_RESET_Q6SS_IN)
#define HWIO_SCC_SSC_ALT_RESET_Q6SS_SSC_ALT_ARES_ENABLE_BMSK                            0x1
#define HWIO_SCC_SSC_ALT_RESET_Q6SS_SSC_ALT_ARES_ENABLE_SHFT                            0x0

#define HWIO_SCC_SSC_SPARE_1_ADDR                                                (SCC_MCC_SSC_REG_REG_BASE      + 0x0000c000)
#define HWIO_SCC_SSC_SPARE_1_RMSK                                                0xffffffff
#define HWIO_SCC_SSC_SPARE_1_IN          \
        in_dword_masked(HWIO_SCC_SSC_SPARE_1_ADDR, HWIO_SCC_SSC_SPARE_1_RMSK)
#define HWIO_SCC_SSC_SPARE_1_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_SPARE_1_ADDR, m)
#define HWIO_SCC_SSC_SPARE_1_OUT(v)      \
        out_dword(HWIO_SCC_SSC_SPARE_1_ADDR,v)
#define HWIO_SCC_SSC_SPARE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_SPARE_1_ADDR,m,v,HWIO_SCC_SSC_SPARE_1_IN)
#define HWIO_SCC_SSC_SPARE_1_SPARE_IN_BMSK                                       0xffff0000
#define HWIO_SCC_SSC_SPARE_1_SPARE_IN_SHFT                                             0x10
#define HWIO_SCC_SSC_SPARE_1_SPARE_OUT_BMSK                                          0xffff
#define HWIO_SCC_SSC_SPARE_1_SPARE_OUT_SHFT                                             0x0

#define HWIO_SCC_SSC_SPARE_2_ADDR                                                (SCC_MCC_SSC_REG_REG_BASE      + 0x0000d000)
#define HWIO_SCC_SSC_SPARE_2_RMSK                                                0xffffffff
#define HWIO_SCC_SSC_SPARE_2_IN          \
        in_dword_masked(HWIO_SCC_SSC_SPARE_2_ADDR, HWIO_SCC_SSC_SPARE_2_RMSK)
#define HWIO_SCC_SSC_SPARE_2_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_SPARE_2_ADDR, m)
#define HWIO_SCC_SSC_SPARE_2_OUT(v)      \
        out_dword(HWIO_SCC_SSC_SPARE_2_ADDR,v)
#define HWIO_SCC_SSC_SPARE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_SPARE_2_ADDR,m,v,HWIO_SCC_SSC_SPARE_2_IN)
#define HWIO_SCC_SSC_SPARE_2_SPARE_IN_BMSK                                       0xffff0000
#define HWIO_SCC_SSC_SPARE_2_SPARE_IN_SHFT                                             0x10
#define HWIO_SCC_SSC_SPARE_2_SPARE_OUT_BMSK                                          0xffff
#define HWIO_SCC_SSC_SPARE_2_SPARE_OUT_SHFT                                             0x0

#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_ADDR                                      (SCC_MCC_SSC_REG_REG_BASE      + 0x0000e000)
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_RMSK                                             0xf
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_IN          \
        in_dword_masked(HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_ADDR, HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_RMSK)
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_INM(m)      \
        in_dword_masked(HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_ADDR, m)
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_OUT(v)      \
        out_dword(HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_ADDR,v)
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_ADDR,m,v,HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_IN)
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_OP_MODE_OVRD_BMSK                                0x8
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_OP_MODE_OVRD_SHFT                                0x3
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_OPMODE_OVRD_VAL_BMSK                             0x4
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_OPMODE_OVRD_VAL_SHFT                             0x2
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_CBC_EN_OVRD_BMSK                                 0x2
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_CBC_EN_OVRD_SHFT                                 0x1
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_CBC_EN_OVRD_VAL_BMSK                             0x1
#define HWIO_SCC_SSC_RSC_PWR_CTRL_OVRD_CBC_EN_OVRD_VAL_SHFT                             0x0


#endif /* __USLEEP_HALHWIO_H__ */

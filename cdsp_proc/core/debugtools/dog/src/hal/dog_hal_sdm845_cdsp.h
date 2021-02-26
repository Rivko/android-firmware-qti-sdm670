#ifndef __DOG_HAL_SDM845_CDSP_H__
#define __DOG_HAL_SDM845_CDSP_H__
/*
===========================================================================
*/
/**
  @file dog_hal_sdm845_cdsp.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p1q0r21.2.0]
 
  This file contains HWIO register definitions for the following modules:
    TURING_QDSP6V65SS_CSR

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtools/dog/src/hal/dog_hal_sdm845_cdsp.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: TURING_QDSP6V65SS_CSR
 *--------------------------------------------------------------------------*/

#define TURING_QDSP6V65SS_CSR_REG_BASE                                                      (TURING_BASE      + 0x00380000)

#define HWIO_TURING_QDSP6SS_INSTANCE_ID_ADDR                                                (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00000000)
#define HWIO_TURING_QDSP6SS_INSTANCE_ID_RMSK                                                       0x7
#define HWIO_TURING_QDSP6SS_INSTANCE_ID_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_INSTANCE_ID_ADDR, HWIO_TURING_QDSP6SS_INSTANCE_ID_RMSK)
#define HWIO_TURING_QDSP6SS_INSTANCE_ID_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_INSTANCE_ID_ADDR, m)
#define HWIO_TURING_QDSP6SS_INSTANCE_ID_ID_BMSK                                                    0x7
#define HWIO_TURING_QDSP6SS_INSTANCE_ID_ID_SHFT                                                    0x0

#define HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_ADDR                                            (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00000100)
#define HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_RMSK                                                   0x1
#define HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_ADDR, HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_RMSK)
#define HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_ADDR, m)
#define HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_HVX_EN_BMSK                                            0x1
#define HWIO_TURING_QDSP6SS_CP_EFUSE_STATUS_HVX_EN_SHFT                                            0x0

#define HWIO_TURING_QDSP6SS_WDOG_RESET_ADDR                                                 (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00004000)
#define HWIO_TURING_QDSP6SS_WDOG_RESET_RMSK                                                        0x1
#define HWIO_TURING_QDSP6SS_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WDOG_RESET_ADDR,v)
#define HWIO_TURING_QDSP6SS_WDOG_RESET_WDOG_RESET_BMSK                                             0x1
#define HWIO_TURING_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT                                             0x0

#define HWIO_TURING_QDSP6SS_WDOG_CTL_ADDR                                                   (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00004004)
#define HWIO_TURING_QDSP6SS_WDOG_CTL_RMSK                                                          0x7
#define HWIO_TURING_QDSP6SS_WDOG_CTL_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_CTL_ADDR, HWIO_TURING_QDSP6SS_WDOG_CTL_RMSK)
#define HWIO_TURING_QDSP6SS_WDOG_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_CTL_ADDR, m)
#define HWIO_TURING_QDSP6SS_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WDOG_CTL_ADDR,v)
#define HWIO_TURING_QDSP6SS_WDOG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_WDOG_CTL_ADDR,m,v,HWIO_TURING_QDSP6SS_WDOG_CTL_IN)
#define HWIO_TURING_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK                                           0x4
#define HWIO_TURING_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT                                           0x2
#define HWIO_TURING_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_BMSK                                          0x2
#define HWIO_TURING_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_SHFT                                          0x1
#define HWIO_TURING_QDSP6SS_WDOG_CTL_ENABLE_BMSK                                                   0x1
#define HWIO_TURING_QDSP6SS_WDOG_CTL_ENABLE_SHFT                                                   0x0

#define HWIO_TURING_QDSP6SS_WDOG_STATUS_ADDR                                                (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00004008)
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_RMSK                                                0xf81fffff
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_STATUS_ADDR, HWIO_TURING_QDSP6SS_WDOG_STATUS_RMSK)
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_STATUS_ADDR, m)
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_BMSK                              0x80000000
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_SHFT                                    0x1f
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_BMSK                          0x40000000
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_SHFT                                0x1e
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_BMSK                          0x20000000
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_SHFT                                0x1d
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_BMSK                           0x10000000
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_SHFT                                 0x1c
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_BMSK                           0x8000000
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_SHFT                                0x1b
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_WDOG_COUNT_BMSK                                       0x1ffffe
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_WDOG_COUNT_SHFT                                            0x1
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_BMSK                                   0x1
#define HWIO_TURING_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_SHFT                                   0x0

#define HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_ADDR                                             (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000400c)
#define HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_RMSK                                                0xfffff
#define HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_ADDR, HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_RMSK)
#define HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_ADDR, m)
#define HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_ADDR,v)
#define HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_ADDR,m,v,HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_IN)
#define HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK                                           0xfffff
#define HWIO_TURING_QDSP6SS_WDOG_BARK_TIME_DATA_SHFT                                               0x0

#define HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_ADDR                                             (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00004010)
#define HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_RMSK                                                0xfffff
#define HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_ADDR, HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_RMSK)
#define HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_ADDR, m)
#define HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_ADDR,v)
#define HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_ADDR,m,v,HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_IN)
#define HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_DATA_BMSK                                           0xfffff
#define HWIO_TURING_QDSP6SS_WDOG_BITE_TIME_DATA_SHFT                                               0x0

#define HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_ADDR                                              (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00004014)
#define HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_RMSK                                                 0xfffff
#define HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_ADDR, HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_RMSK)
#define HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_ADDR, m)
#define HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_ADDR,v)
#define HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_ADDR,m,v,HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_IN)
#define HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_DATA_BMSK                                            0xfffff
#define HWIO_TURING_QDSP6SS_WDOG_NMI_TIME_DATA_SHFT                                                0x0

#define HWIO_TURING_QDSP6SS_WDOG_TEST_LOAD_ADDR                                             (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00004018)
#define HWIO_TURING_QDSP6SS_WDOG_TEST_LOAD_RMSK                                                    0x1
#define HWIO_TURING_QDSP6SS_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_TURING_QDSP6SS_WDOG_TEST_LOAD_LOAD_BMSK                                               0x1
#define HWIO_TURING_QDSP6SS_WDOG_TEST_LOAD_LOAD_SHFT                                               0x0

#define HWIO_TURING_QDSP6SS_WDOG_TEST_ADDR                                                  (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000401c)
#define HWIO_TURING_QDSP6SS_WDOG_TEST_RMSK                                                    0x1fffff
#define HWIO_TURING_QDSP6SS_WDOG_TEST_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_TEST_ADDR, HWIO_TURING_QDSP6SS_WDOG_TEST_RMSK)
#define HWIO_TURING_QDSP6SS_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_WDOG_TEST_ADDR, m)
#define HWIO_TURING_QDSP6SS_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WDOG_TEST_ADDR,v)
#define HWIO_TURING_QDSP6SS_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_WDOG_TEST_ADDR,m,v,HWIO_TURING_QDSP6SS_WDOG_TEST_IN)
#define HWIO_TURING_QDSP6SS_WDOG_TEST_SYNC_STATUS_BMSK                                        0x100000
#define HWIO_TURING_QDSP6SS_WDOG_TEST_SYNC_STATUS_SHFT                                            0x14
#define HWIO_TURING_QDSP6SS_WDOG_TEST_LOAD_VALUE_BMSK                                          0xfffff
#define HWIO_TURING_QDSP6SS_WDOG_TEST_LOAD_VALUE_SHFT                                              0x0

#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_ADDR                                            (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00004100)
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_RMSK                                                   0xf
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_ADDR, HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_RMSK)
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_ADDR, m)
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_ADDR,v)
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_ADDR,m,v,HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_IN)
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_MAX_COUNT_BMSK                                         0xe
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_MAX_COUNT_SHFT                                         0x1
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_DISABLE_BMSK                                           0x1
#define HWIO_TURING_QDSP6SS_GLBCNT_TRIG_CFG_DISABLE_SHFT                                           0x0

#define HWIO_TURING_QDSP6SS_TS_m_EN_ADDR(m)                                                 (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00005000 + 0x10 * (m))
#define HWIO_TURING_QDSP6SS_TS_m_EN_RMSK                                                           0x7
#define HWIO_TURING_QDSP6SS_TS_m_EN_MAXm                                                             3
#define HWIO_TURING_QDSP6SS_TS_m_EN_INI(m)        \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_m_EN_ADDR(m), HWIO_TURING_QDSP6SS_TS_m_EN_RMSK)
#define HWIO_TURING_QDSP6SS_TS_m_EN_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_m_EN_ADDR(m), mask)
#define HWIO_TURING_QDSP6SS_TS_m_EN_OUTI(m,val)    \
        out_dword(HWIO_TURING_QDSP6SS_TS_m_EN_ADDR(m),val)
#define HWIO_TURING_QDSP6SS_TS_m_EN_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_TS_m_EN_ADDR(m),mask,val,HWIO_TURING_QDSP6SS_TS_m_EN_INI(m))
#define HWIO_TURING_QDSP6SS_TS_m_EN_MODE_BMSK                                                      0x4
#define HWIO_TURING_QDSP6SS_TS_m_EN_MODE_SHFT                                                      0x2
#define HWIO_TURING_QDSP6SS_TS_m_EN_CLEAR_BMSK                                                     0x2
#define HWIO_TURING_QDSP6SS_TS_m_EN_CLEAR_SHFT                                                     0x1
#define HWIO_TURING_QDSP6SS_TS_m_EN_EN_BMSK                                                        0x1
#define HWIO_TURING_QDSP6SS_TS_m_EN_EN_SHFT                                                        0x0

#define HWIO_TURING_QDSP6SS_TS_m_STATUS_ADDR(m)                                             (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00005004 + 0x10 * (m))
#define HWIO_TURING_QDSP6SS_TS_m_STATUS_RMSK                                                     0x101
#define HWIO_TURING_QDSP6SS_TS_m_STATUS_MAXm                                                         3
#define HWIO_TURING_QDSP6SS_TS_m_STATUS_INI(m)        \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_m_STATUS_ADDR(m), HWIO_TURING_QDSP6SS_TS_m_STATUS_RMSK)
#define HWIO_TURING_QDSP6SS_TS_m_STATUS_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_m_STATUS_ADDR(m), mask)
#define HWIO_TURING_QDSP6SS_TS_m_STATUS_OVERFLOW_BMSK                                            0x100
#define HWIO_TURING_QDSP6SS_TS_m_STATUS_OVERFLOW_SHFT                                              0x8
#define HWIO_TURING_QDSP6SS_TS_m_STATUS_VALID_BMSK                                                 0x1
#define HWIO_TURING_QDSP6SS_TS_m_STATUS_VALID_SHFT                                                 0x0

#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m)                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00005008 + 0x10 * (m))
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_RMSK                                           0xffffffff
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_MAXm                                                    3
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_INI(m)        \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m), HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_RMSK)
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m), mask)
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_DATA_L_BMSK                                    0xffffffff
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_L_DATA_L_SHFT                                           0x0

#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m)                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000500c + 0x10 * (m))
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_RMSK                                             0xffffff
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_MAXm                                                    3
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_INI(m)        \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m), HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_RMSK)
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_INMI(m,mask)    \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m), mask)
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_DATA_H_BMSK                                      0xffffff
#define HWIO_TURING_QDSP6SS_TS_m_TIMESTAMP_H_DATA_H_SHFT                                           0x0

#define HWIO_TURING_QDSP6SS_TS_HWE_SEL_ADDR                                                 (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00005100)
#define HWIO_TURING_QDSP6SS_TS_HWE_SEL_RMSK                                                       0x1f
#define HWIO_TURING_QDSP6SS_TS_HWE_SEL_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_HWE_SEL_ADDR, HWIO_TURING_QDSP6SS_TS_HWE_SEL_RMSK)
#define HWIO_TURING_QDSP6SS_TS_HWE_SEL_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_TS_HWE_SEL_ADDR, m)
#define HWIO_TURING_QDSP6SS_TS_HWE_SEL_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_TS_HWE_SEL_ADDR,v)
#define HWIO_TURING_QDSP6SS_TS_HWE_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_TS_HWE_SEL_ADDR,m,v,HWIO_TURING_QDSP6SS_TS_HWE_SEL_IN)
#define HWIO_TURING_QDSP6SS_TS_HWE_SEL_MUX_SEL_BMSK                                               0x1f
#define HWIO_TURING_QDSP6SS_TS_HWE_SEL_MUX_SEL_SHFT                                                0x0

#define HWIO_TURING_QDSP6SS_IPC_ADDR                                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00008000)
#define HWIO_TURING_QDSP6SS_IPC_RMSK                                                        0xffffffff
#define HWIO_TURING_QDSP6SS_IPC_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_IPC_ADDR,v)
#define HWIO_TURING_QDSP6SS_IPC_INTR_BMSK                                                   0xffffffff
#define HWIO_TURING_QDSP6SS_IPC_INTR_SHFT                                                          0x0

#define HWIO_TURING_QDSP6SS_IPC1_ADDR                                                       (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x00008004)
#define HWIO_TURING_QDSP6SS_IPC1_RMSK                                                       0xffffffff
#define HWIO_TURING_QDSP6SS_IPC1_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_IPC1_ADDR,v)
#define HWIO_TURING_QDSP6SS_IPC1_INTR_BMSK                                                  0xffffffff
#define HWIO_TURING_QDSP6SS_IPC1_INTR_SHFT                                                         0x0

#define HWIO_TURING_QDSP6SS_BLOCK_INTR_ADDR                                                 (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c008)
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_RMSK                                                        0x7
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_BLOCK_INTR_ADDR, HWIO_TURING_QDSP6SS_BLOCK_INTR_RMSK)
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_BLOCK_INTR_ADDR, m)
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_BLOCK_INTR_ADDR,v)
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_BLOCK_INTR_ADDR,m,v,HWIO_TURING_QDSP6SS_BLOCK_INTR_IN)
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_CLEAR_BLOCK_BMSK                                            0x4
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_CLEAR_BLOCK_SHFT                                            0x2
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_BLOCK_STATUS_BMSK                                           0x2
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_BLOCK_STATUS_SHFT                                           0x1
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_BLOCK_INTR_BMSK                                             0x1
#define HWIO_TURING_QDSP6SS_BLOCK_INTR_BLOCK_INTR_SHFT                                             0x0

#define HWIO_TURING_QDSP6SS_SLPC_CFG_ADDR                                                   (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c01c)
#define HWIO_TURING_QDSP6SS_SLPC_CFG_RMSK                                                       0xffa0
#define HWIO_TURING_QDSP6SS_SLPC_CFG_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_SLPC_CFG_ADDR, HWIO_TURING_QDSP6SS_SLPC_CFG_RMSK)
#define HWIO_TURING_QDSP6SS_SLPC_CFG_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_SLPC_CFG_ADDR, m)
#define HWIO_TURING_QDSP6SS_SLPC_CFG_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_SLPC_CFG_ADDR,v)
#define HWIO_TURING_QDSP6SS_SLPC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_SLPC_CFG_ADDR,m,v,HWIO_TURING_QDSP6SS_SLPC_CFG_IN)
#define HWIO_TURING_QDSP6SS_SLPC_CFG_STAGGER_LKAD_DIS_BMSK                                      0x8000
#define HWIO_TURING_QDSP6SS_SLPC_CFG_STAGGER_LKAD_DIS_SHFT                                         0xf
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_FAST_STAGGER_BMSK                                      0x4000
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_FAST_STAGGER_SHFT                                         0xe
#define HWIO_TURING_QDSP6SS_SLPC_CFG_IDLE_CORE_CLK_EN_BMSK                                      0x2000
#define HWIO_TURING_QDSP6SS_SLPC_CFG_IDLE_CORE_CLK_EN_SHFT                                         0xd
#define HWIO_TURING_QDSP6SS_SLPC_CFG_EXTHW_WAKE_EN_BMSK                                         0x1000
#define HWIO_TURING_QDSP6SS_SLPC_CFG_EXTHW_WAKE_EN_SHFT                                            0xc
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_PU_PERI_STAGGER_BMSK                                    0x800
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_PU_PERI_STAGGER_SHFT                                      0xb
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_PD_PERI_STAGGER_BMSK                                    0x400
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_PD_PERI_STAGGER_SHFT                                      0xa
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_PU_ARRY_STAGGER_BMSK                                    0x200
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_PU_ARRY_STAGGER_SHFT                                      0x9
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_PD_ARRY_STAGGER_BMSK                                    0x100
#define HWIO_TURING_QDSP6SS_SLPC_CFG_MEM_PD_ARRY_STAGGER_SHFT                                      0x8
#define HWIO_TURING_QDSP6SS_SLPC_CFG_WAKEUP_IN_EN_BMSK                                            0x80
#define HWIO_TURING_QDSP6SS_SLPC_CFG_WAKEUP_IN_EN_SHFT                                             0x7
#define HWIO_TURING_QDSP6SS_SLPC_CFG_CLK_GATING_MODE_BMSK                                         0x20
#define HWIO_TURING_QDSP6SS_SLPC_CFG_CLK_GATING_MODE_SHFT                                          0x5

#define HWIO_TURING_QDSP6SS_RSCCTL_BR_ADDR                                                  (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c020)
#define HWIO_TURING_QDSP6SS_RSCCTL_BR_RMSK                                                         0xe
#define HWIO_TURING_QDSP6SS_RSCCTL_BR_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_BR_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_BR_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_BR_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_BR_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_BR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_BR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_BR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_BR_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_BR_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_BR_EVENT_BMSK                                                   0xe
#define HWIO_TURING_QDSP6SS_RSCCTL_BR_EVENT_SHFT                                                   0x1

#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_ADDR                                                 (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c024)
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_RMSK                                                        0x3
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_WAKEUP_CTL_ADDR, HWIO_TURING_QDSP6SS_WAKEUP_CTL_RMSK)
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_WAKEUP_CTL_ADDR, m)
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WAKEUP_CTL_ADDR,v)
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_WAKEUP_CTL_ADDR,m,v,HWIO_TURING_QDSP6SS_WAKEUP_CTL_IN)
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_NMI_WAKE_EN_BMSK                                            0x2
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_NMI_WAKE_EN_SHFT                                            0x1
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_L2VIC_WAKE_EN_BMSK                                          0x1
#define HWIO_TURING_QDSP6SS_WAKEUP_CTL_L2VIC_WAKE_EN_SHFT                                          0x0

#define HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_ADDR                                            (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c028)
#define HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_RMSK                                                  0x1c
#define HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_ADDR, HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_RMSK)
#define HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_ADDR, m)
#define HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_ADDR,v)
#define HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_ADDR,m,v,HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_IN)
#define HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_WAKEUP_EN_BMSK                                        0x1c
#define HWIO_TURING_QDSP6SS_L2VIC_WAKEUP_EN_WAKEUP_EN_SHFT                                         0x2

#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_ADDR                                              (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c02c)
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_RMSK                                                    0x1f
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_WAKEUP_STATUS_ADDR, HWIO_TURING_QDSP6SS_WAKEUP_STATUS_RMSK)
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_WAKEUP_STATUS_ADDR, m)
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_WAKEUP_STATUS_ADDR,v)
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_WAKEUP_STATUS_ADDR,m,v,HWIO_TURING_QDSP6SS_WAKEUP_STATUS_IN)
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_STICKY_BMSK                                 0x10
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_STICKY_SHFT                                  0x4
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_BMSK                                         0x8
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_SHFT                                         0x3
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_DBG_NMI_BMSK                                             0x4
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_DBG_NMI_SHFT                                             0x2
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_EXT_TRIGGER_BMSK                                         0x2
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_EXT_TRIGGER_SHFT                                         0x1
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_L2VIC_WAKEUP_BMSK                                        0x1
#define HWIO_TURING_QDSP6SS_WAKEUP_STATUS_L2VIC_WAKEUP_SHFT                                        0x0

#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR                                      (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c054)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_RMSK                                             0x7
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR, HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_RMSK)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR,m,v,HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_IN)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM2_HALTACK_OVRRD_BMSK                         0x4
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM2_HALTACK_OVRRD_SHFT                         0x2
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM_HALTACK_OVRRD_BMSK                          0x2
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM_HALTACK_OVRRD_SHFT                          0x1
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ALL_HALTACK_OVRRD_BMSK                           0x1
#define HWIO_TURING_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ALL_HALTACK_OVRRD_SHFT                           0x0

#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR                                         (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c058)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_RMSK                                             0x3fff
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR, HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_RMSK)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR,m,v,HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_IN)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_SPARE_OVRRD_BMSK                                 0x3c00
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_SPARE_OVRRD_SHFT                                    0xa
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_TCS_HW_ACK_OVRRD_BMSK                             0x3fc
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_TCS_HW_ACK_OVRRD_SHFT                               0x2
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_BRINGUP_ACK_OVRRD_BMSK                              0x2
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_BRINGUP_ACK_OVRRD_SHFT                              0x1
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_SHUTDOWN_ACK_OVRRD_BMSK                             0x1
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PRSC_OVR_SHUTDOWN_ACK_OVRRD_SHFT                             0x0

#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c05c)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_RMSK                                                 0x3
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR, HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_RMSK)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR,m,v,HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_IN)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_FINE_OVRRD_BMSK                                  0x2
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_FINE_OVRRD_SHFT                                  0x1
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_OVRRD_BMSK                                       0x1
#define HWIO_TURING_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_OVRRD_SHFT                                       0x0

#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c060)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_RMSK                                                0x1f
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR, HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_RMSK)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR,m,v,HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_IN)
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_REFRESH_BMSK                                        0x10
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_REFRESH_SHFT                                         0x4
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_PD_BMSK                                       0x8
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_PD_SHFT                                       0x3
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_READY_BMSK                                    0x4
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_READY_SHFT                                    0x2
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_PD_BMSK                                         0x2
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_PD_SHFT                                         0x1
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_READY_BMSK                                      0x1
#define HWIO_TURING_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_READY_SHFT                                      0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ADDR                                             (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c100)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_RMSK                                                 0x1fff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_APB_BMSK                                             0x1000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_APB_SHFT                                                0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AXIS2_BMSK                                            0x800
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AXIS2_SHFT                                              0xb
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CSI1_BMSK                                             0x400
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CSI1_SHFT                                               0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CSI0_BMSK                                             0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CSI0_SHFT                                               0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ARCH_BMSK                                             0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ARCH_SHFT                                               0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CORE_BMSK                                              0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CORE_SHFT                                               0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AXIM2_BMSK                                             0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AXIM2_SHFT                                              0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AXIS_BMSK                                              0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AXIS_SHFT                                               0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ATBM_BMSK                                              0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ATBM_SHFT                                               0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AHBM_BMSK                                               0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AHBM_SHFT                                               0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AXIM_BMSK                                               0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_AXIM_SHFT                                               0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ISDB_ETM_BMSK                                           0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_ISDB_ETM_SHFT                                           0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_RET_BMSK                                                0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_RET_SHFT                                                0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_ADDR                                            (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c104)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_RMSK                                                   0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_MSS2CX_BMSK                                      0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_MSS2CX_SHFT                                      0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_IO_BMSK                                          0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_IO_SHFT                                          0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_ADDR                                              (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c108)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_RMSK                                                   0x3ff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AXIS2_BMSK                                             0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AXIS2_SHFT                                               0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CSI1_BMSK                                              0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CSI1_SHFT                                                0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CSI0_BMSK                                               0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CSI0_SHFT                                                0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AXIM2_BMSK                                              0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AXIM2_SHFT                                               0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AXIS_BMSK                                               0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AXIS_SHFT                                                0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_ATBM_BMSK                                               0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_ATBM_SHFT                                                0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AHBM_BMSK                                                0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AHBM_SHFT                                                0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AXIM_BMSK                                                0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_AXIM_SHFT                                                0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_APB_BMSK                                                 0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_APB_SHFT                                                 0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CORE_BMSK                                                0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CORE_SHFT                                                0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c10c)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_RMSK                                             0xfffff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L2CLADE_BMSK                                     0xc0000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L2CLADE_SHFT                                        0x12
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L2TAG_BMSK                                       0x3c000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L2TAG_SHFT                                           0xe
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_BMSK                                         0x3000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_SHFT                                            0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BMSK                                          0xc00
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_SHFT                                            0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_TAG_BMSK                                      0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_TAG_SHFT                                        0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_JU_BMSK                                            0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_JU_SHFT                                              0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L2PLRU_BMSK                                         0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L2PLRU_SHFT                                          0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_ETB_BMSK                                            0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_ETB_SHFT                                             0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L2STBUF_BMSK                                        0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L2STBUF_SHFT                                         0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BTB_BMSK                                       0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BTB_SHFT                                        0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_TAG_BMSK                                        0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_TAG_SHFT                                        0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_PDEC_BMSK                                       0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_PDEC_SHFT                                       0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c110)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_RMSK                                                 0xe
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_QMC_MEM_BMSK                                   0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_QMC_MEM_SHFT                                   0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_WL_BMSK                                        0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_WL_SHFT                                        0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_MEM_PERIPH_BMSK                                      0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_MEM_PERIPH_SHFT                                      0x1

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_ADDR                                              (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c114)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_RMSK                                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_BHS_BMSK                                                 0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_BHS_SHFT                                                 0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_ADDR                                              (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c118)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_RMSK                                                    0x7f
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_BG_OFF_BMSK                                             0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_BG_OFF_SHFT                                              0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_FORCEOFF_BMSK                                           0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_FORCEOFF_SHFT                                            0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SPARE_BITS4_2_BMSK                                      0x1c
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SPARE_BITS4_2_SHFT                                       0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_VREF_SEL_UPDATE_BMSK                                     0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_VREF_SEL_UPDATE_SHFT                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_BYPASS_OPEN_BMSK                                         0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_BYPASS_OPEN_SHFT                                         0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_ADDR                                              (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c11c)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_RMSK                                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_XO_ENABLE_BMSK                                           0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_XO_ENABLE_SHFT                                           0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_ADDR                                              (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c120)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_RMSK                                                    0x17
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_PLL_STBY_BMSK                                           0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_PLL_STBY_SHFT                                            0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_PLL_OUTDIS_BMSK                                          0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_PLL_OUTDIS_SHFT                                          0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_PLL_OPMODE1_BMSK                                         0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_PLL_OPMODE1_SHFT                                         0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_PLL_RESET_BMSK                                           0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_PLL_RESET_SHFT                                           0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_ADDR                                         (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c124)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_RMSK                                                0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_WAKE_IRQ_BMSK                                       0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_WAKE_IRQ_SHFT                                       0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_RESTORE_BMSK                                        0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_RESTORE_SHFT                                        0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SAVE_BMSK                                           0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SAVE_SHFT                                           0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR                                      (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c128)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_RMSK                                            0x3d
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_FIFO_ARESET_BMSK                          0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_FIFO_ARESET_SHFT                           0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_HALTREQ_BMSK                              0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_HALTREQ_SHFT                               0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_FIFO_ARESET_BMSK                            0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_FIFO_ARESET_SHFT                            0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_HALTREQ_BMSK                                0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_HALTREQ_SHFT                                0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_BUSM_HALTREQ_BMSK                                0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_BUSM_HALTREQ_SHFT                                0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR                                    (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c12c)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_RMSK                                          0xff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_CLEAR_BMSK                      0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_CLEAR_SHFT                       0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_SET_BMSK                        0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_SET_SHFT                         0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_CLEAR_BMSK                      0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_CLEAR_SHFT                       0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_SET_BMSK                        0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_SET_SHFT                         0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_CLEAR_BMSK                        0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_CLEAR_SHFT                        0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_SET_BMSK                          0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_SET_SHFT                          0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_CLEAR_BMSK                        0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_CLEAR_SHFT                        0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_SET_BMSK                          0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_SET_SHFT                          0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR                                      (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c130)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_RMSK                                             0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM2_BMSK                                       0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM2_SHFT                                       0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM_BMSK                                        0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM_SHFT                                        0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CORE_BMSK                                        0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CORE_SHFT                                        0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c134)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_RMSK                                               0xf
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_L2DATA_BMSK                                        0xf
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_L2DATA_SHFT                                        0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_ADDR                                             (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c138)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_RMSK                                                 0x3fff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_IN          \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_ADDR, HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_RMSK)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_INM(m)      \
        in_dword_masked(HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_ADDR, m)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_ADDR,m,v,HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_IN)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SPARE_BMSK                                           0x3c00
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SPARE_SHFT                                              0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_TCS_HW_TRIGGER_BMSK                                   0x3fc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_TCS_HW_TRIGGER_SHFT                                     0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_BRINGUP_REQ_BMSK                                        0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_BRINGUP_REQ_SHFT                                        0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SHUTDOWN_REQ_BMSK                                       0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SHUTDOWN_REQ_SHFT                                       0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_ADDR                                         (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c180)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_RMSK                                             0x1fff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_APB_BMSK                                         0x1000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_APB_SHFT                                            0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS2_BMSK                                        0x800
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS2_SHFT                                          0xb
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_CSI1_BMSK                                         0x400
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_CSI1_SHFT                                           0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_CSI0_BMSK                                         0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_CSI0_SHFT                                           0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_ARCH_BMSK                                         0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_ARCH_SHFT                                           0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_CORE_BMSK                                          0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_CORE_SHFT                                           0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM2_BMSK                                         0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM2_SHFT                                          0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS_BMSK                                          0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS_SHFT                                           0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_ATBM_BMSK                                          0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_ATBM_SHFT                                           0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AHBM_BMSK                                           0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AHBM_SHFT                                           0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM_BMSK                                           0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM_SHFT                                           0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_ISDB_ETM_BMSK                                       0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_ISDB_ETM_SHFT                                       0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_RET_BMSK                                            0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_SET_RET_SHFT                                            0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_SET_ADDR                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c184)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_SET_RMSK                                               0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_MSS2CX_BMSK                                  0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_MSS2CX_SHFT                                  0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_IO_BMSK                                      0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_IO_SHFT                                      0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c188)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_RMSK                                               0x3ff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS2_BMSK                                         0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS2_SHFT                                           0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_CSI1_BMSK                                          0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_CSI1_SHFT                                            0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_CSI0_BMSK                                           0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_CSI0_SHFT                                            0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM2_BMSK                                          0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM2_SHFT                                           0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS_BMSK                                           0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS_SHFT                                            0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_ATBM_BMSK                                           0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_ATBM_SHFT                                            0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AHBM_BMSK                                            0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AHBM_SHFT                                            0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM_BMSK                                            0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM_SHFT                                            0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_APB_BMSK                                             0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_APB_SHFT                                             0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_CORE_BMSK                                            0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_SET_CORE_SHFT                                            0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ADDR                                      (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c18c)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_RMSK                                         0xfffff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2CLADE_BMSK                                 0xc0000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2CLADE_SHFT                                    0x12
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2TAG_BMSK                                   0x3c000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2TAG_SHFT                                       0xe
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_BMSK                                     0x3000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_SHFT                                        0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BMSK                                      0xc00
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_SHFT                                        0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_TAG_BMSK                                  0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_TAG_SHFT                                    0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_BMSK                                        0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_SHFT                                          0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2PLRU_BMSK                                     0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2PLRU_SHFT                                      0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ETB_BMSK                                        0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ETB_SHFT                                         0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2STBUF_BMSK                                    0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2STBUF_SHFT                                     0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BTB_BMSK                                   0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BTB_SHFT                                    0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_TAG_BMSK                                    0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_TAG_SHFT                                    0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_PDEC_BMSK                                   0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_PDEC_SHFT                                   0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_ADDR                                      (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c190)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_RMSK                                             0xe
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_QMC_MEM_BMSK                               0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_QMC_MEM_SHFT                               0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_WL_BMSK                                    0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_WL_SHFT                                    0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_MEM_PERIPH_BMSK                                  0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_MEM_PERIPH_SHFT                                  0x1

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_SET_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c194)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_SET_RMSK                                                 0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_SET_BHS_BMSK                                             0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_SET_BHS_SHFT                                             0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c198)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_RMSK                                                0x7f
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_BG_OFF_BMSK                                         0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_BG_OFF_SHFT                                          0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_FORCEOFF_BMSK                                       0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_FORCEOFF_SHFT                                        0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_SPARE_BITS4_2_BMSK                                  0x1c
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_SPARE_BITS4_2_SHFT                                   0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_UPDATE_BMSK                                          0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_UPDATE_SHFT                                          0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_BYPASS_OPEN_BMSK                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_SET_BYPASS_OPEN_SHFT                                     0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_SET_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c19c)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_SET_RMSK                                                 0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_SET_XO_ENABLE_BMSK                                       0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_SET_XO_ENABLE_SHFT                                       0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_ADDR                                          (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1a0)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_RMSK                                                0x17
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_STBY_BMSK                                       0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_STBY_SHFT                                        0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OUTDIS_BMSK                                      0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OUTDIS_SHFT                                      0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OPMODE1_BMSK                                     0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OPMODE1_SHFT                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_RESET_BMSK                                       0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_RESET_SHFT                                       0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_ADDR                                     (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1a4)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_RMSK                                            0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_WAKE_IRQ_BMSK                                   0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_WAKE_IRQ_SHFT                                   0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_RESTORE_BMSK                                    0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_RESTORE_SHFT                                    0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_SAVE_BMSK                                       0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_SET_SAVE_SHFT                                       0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_ADDR                                  (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1a8)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_RMSK                                        0x3d
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_FIFO_ARESET_BMSK                      0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_FIFO_ARESET_SHFT                       0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_HALTREQ_BMSK                          0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_HALTREQ_SHFT                           0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_FIFO_ARESET_BMSK                        0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_FIFO_ARESET_SHFT                        0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_HALTREQ_BMSK                            0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_HALTREQ_SHFT                            0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_BUSM_HALTREQ_BMSK                            0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_BUSM_HALTREQ_SHFT                            0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_ADDR                                (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1ac)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_RMSK                                      0xff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_CLEAR_BMSK                  0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_CLEAR_SHFT                   0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_SET_BMSK                    0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_SET_SHFT                     0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_CLEAR_BMSK                  0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_CLEAR_SHFT                   0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_SET_BMSK                    0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_SET_SHFT                     0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_CLEAR_BMSK                    0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_CLEAR_SHFT                    0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_SET_BMSK                      0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_SET_SHFT                      0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_CLEAR_BMSK                    0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_CLEAR_SHFT                    0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_SET_BMSK                      0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_SET_SHFT                      0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_ADDR                                  (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1b0)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_RMSK                                         0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM2_BMSK                                   0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM2_SHFT                                   0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM_BMSK                                    0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM_SHFT                                    0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_CORE_BMSK                                    0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_CORE_SHFT                                    0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_ADDR                                    (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1b4)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_RMSK                                           0xf
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_L2DATA_BMSK                                    0xf
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_L2DATA_SHFT                                    0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_ADDR                                         (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c1b8)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_RMSK                                             0x3fff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_SPARE_BMSK                                       0x3c00
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_SPARE_SHFT                                          0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_TCS_HW_TRIGGER_BMSK                               0x3fc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_TCS_HW_TRIGGER_SHFT                                 0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_BRINGUP_REQ_BMSK                                    0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_BRINGUP_REQ_SHFT                                    0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_SHUTDOWN_REQ_BMSK                                   0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_SET_SHUTDOWN_REQ_SHFT                                   0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ADDR                                       (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c200)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RMSK                                           0x1fff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_APB_BMSK                                       0x1000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_APB_SHFT                                          0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS2_BMSK                                      0x800
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS2_SHFT                                        0xb
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI1_BMSK                                       0x400
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI1_SHFT                                         0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI0_BMSK                                       0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI0_SHFT                                         0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ARCH_BMSK                                       0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ARCH_SHFT                                         0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CORE_BMSK                                        0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CORE_SHFT                                         0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM2_BMSK                                       0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM2_SHFT                                        0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS_BMSK                                        0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS_SHFT                                         0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ATBM_BMSK                                        0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ATBM_SHFT                                         0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AHBM_BMSK                                         0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AHBM_SHFT                                         0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM_BMSK                                         0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM_SHFT                                         0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ISDB_ETM_BMSK                                     0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ISDB_ETM_SHFT                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RET_BMSK                                          0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RET_SHFT                                          0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_ADDR                                      (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c204)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_RMSK                                             0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_MSS2CX_BMSK                                0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_MSS2CX_SHFT                                0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_IO_BMSK                                    0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_IO_SHFT                                    0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ADDR                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c208)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_RMSK                                             0x3ff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS2_BMSK                                       0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS2_SHFT                                         0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI1_BMSK                                        0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI1_SHFT                                          0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI0_BMSK                                         0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI0_SHFT                                          0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM2_BMSK                                        0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM2_SHFT                                         0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS_BMSK                                         0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS_SHFT                                          0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ATBM_BMSK                                         0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ATBM_SHFT                                          0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AHBM_BMSK                                          0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AHBM_SHFT                                          0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM_BMSK                                          0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM_SHFT                                          0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_APB_BMSK                                           0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_APB_SHFT                                           0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CORE_BMSK                                          0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CORE_SHFT                                          0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ADDR                                    (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c20c)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_RMSK                                       0xfffff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2CLADE_BMSK                               0xc0000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2CLADE_SHFT                                  0x12
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2TAG_BMSK                                 0x3c000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2TAG_SHFT                                     0xe
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_BMSK                                   0x3000
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_SHFT                                      0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BMSK                                    0xc00
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_SHFT                                      0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_TAG_BMSK                                0x200
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_TAG_SHFT                                  0x9
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_BMSK                                      0x100
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_SHFT                                        0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2PLRU_BMSK                                   0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2PLRU_SHFT                                    0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ETB_BMSK                                      0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ETB_SHFT                                       0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2STBUF_BMSK                                  0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2STBUF_SHFT                                   0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BTB_BMSK                                 0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BTB_SHFT                                  0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_TAG_BMSK                                  0xc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_TAG_SHFT                                  0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_PDEC_BMSK                                 0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_PDEC_SHFT                                 0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_ADDR                                    (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c210)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_RMSK                                           0xe
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_QMC_MEM_BMSK                             0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_QMC_MEM_SHFT                             0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_WL_BMSK                                  0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_WL_SHFT                                  0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_MEM_PERIPH_BMSK                                0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_MEM_PERIPH_SHFT                                0x1

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_CLEAR_ADDR                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c214)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_CLEAR_RMSK                                               0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_CLEAR_BHS_BMSK                                           0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_BHS_CLEAR_BHS_SHFT                                           0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_ADDR                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c218)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_RMSK                                              0x7f
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BG_OFF_BMSK                                       0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BG_OFF_SHFT                                        0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_FORCEOFF_BMSK                                     0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_FORCEOFF_SHFT                                      0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_SPARE_BITS4_2_BMSK                                0x1c
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_SPARE_BITS4_2_SHFT                                 0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_UPDATE_BMSK                                        0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_UPDATE_SHFT                                        0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BYPASS_OPEN_BMSK                                   0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BYPASS_OPEN_SHFT                                   0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_CLEAR_ADDR                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c21c)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_CLEAR_RMSK                                               0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_CLEAR_XO_ENABLE_BMSK                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_EXT_CLEAR_XO_ENABLE_SHFT                                     0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_ADDR                                        (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c220)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_RMSK                                              0x17
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_STBY_BMSK                                     0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_STBY_SHFT                                      0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OUTDIS_BMSK                                    0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OUTDIS_SHFT                                    0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OPMODE1_BMSK                                   0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OPMODE1_SHFT                                   0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_RESET_BMSK                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_RESET_SHFT                                     0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_ADDR                                   (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c224)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RMSK                                          0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_WAKE_IRQ_BMSK                                 0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_WAKE_IRQ_SHFT                                 0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RESTORE_BMSK                                  0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RESTORE_SHFT                                  0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_SAVE_BMSK                                     0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_SAVE_SHFT                                     0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_ADDR                                (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c228)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_RMSK                                      0x3d
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_FIFO_ARESET_BMSK                    0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_FIFO_ARESET_SHFT                     0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_HALTREQ_BMSK                        0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_HALTREQ_SHFT                         0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_FIFO_ARESET_BMSK                      0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_FIFO_ARESET_SHFT                      0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_HALTREQ_BMSK                          0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_HALTREQ_SHFT                          0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_BUSM_HALTREQ_BMSK                          0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_BUSM_HALTREQ_SHFT                          0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_ADDR                              (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c22c)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_RMSK                                    0xff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_CLEAR_BMSK                0x80
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_CLEAR_SHFT                 0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_SET_BMSK                  0x40
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_SET_SHFT                   0x6
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_CLEAR_BMSK                0x20
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_CLEAR_SHFT                 0x5
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_SET_BMSK                  0x10
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_SET_SHFT                   0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_CLEAR_BMSK                  0x8
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_CLEAR_SHFT                  0x3
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_SET_BMSK                    0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_SET_SHFT                    0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_CLEAR_BMSK                  0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_CLEAR_SHFT                  0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_SET_BMSK                    0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_SET_SHFT                    0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_ADDR                                (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c230)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_RMSK                                       0x7
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM2_BMSK                                 0x4
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM2_SHFT                                 0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM_BMSK                                  0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM_SHFT                                  0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_CORE_BMSK                                  0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_CORE_SHFT                                  0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_ADDR                                  (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c234)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_RMSK                                         0xf
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_L2DATA_BMSK                                  0xf
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_L2DATA_SHFT                                  0x0

#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_ADDR                                       (TURING_QDSP6V65SS_CSR_REG_BASE      + 0x0000c238)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_RMSK                                           0x3fff
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_OUT(v)      \
        out_dword(HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_ADDR,v)
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SPARE_BMSK                                     0x3c00
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SPARE_SHFT                                        0xa
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_TCS_HW_TRIGGER_BMSK                             0x3fc
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_TCS_HW_TRIGGER_SHFT                               0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_BRINGUP_REQ_BMSK                                  0x2
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_BRINGUP_REQ_SHFT                                  0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SHUTDOWN_REQ_BMSK                                 0x1
#define HWIO_TURING_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SHUTDOWN_REQ_SHFT                                 0x0


#endif /* __DOG_HAL_SDM845_CDSP_H__ */

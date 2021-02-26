#ifndef __HWIO_DDRAUX_TCS__
#define __HWIO_DDRAUX_TCS__
/*
===========================================================================
*/
/**
  @file hwio_ddraux_tcs
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p1q0r28.2.0]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_DDRAUX_DDR_AUX_TCS_CFG

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_ddraux_tcs.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: DDR_AUX_TCS_CFG
 *--------------------------------------------------------------------------*/

#define DDR_AUX_TCS_CFG_REG_BASE                                     (AOSS_BASE      + 0x00c00000)

#define HWIO_RSI_TCSm_CFG_ADDR(m)                                    (DDR_AUX_TCS_CFG_REG_BASE      + 0x00000000 + 0x4 * (m))
#define HWIO_RSI_TCSm_CFG_RMSK                                             0xff
#define HWIO_RSI_TCSm_CFG_MAXm                                               13
#define HWIO_RSI_TCSm_CFG_ATTR                                              0x3
#define HWIO_RSI_TCSm_CFG_INI(m)        \
        in_dword_masked(HWIO_RSI_TCSm_CFG_ADDR(m), HWIO_RSI_TCSm_CFG_RMSK)
#define HWIO_RSI_TCSm_CFG_INMI(m,mask)    \
        in_dword_masked(HWIO_RSI_TCSm_CFG_ADDR(m), mask)
#define HWIO_RSI_TCSm_CFG_OUTI(m,val)    \
        out_dword(HWIO_RSI_TCSm_CFG_ADDR(m),val)
#define HWIO_RSI_TCSm_CFG_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RSI_TCSm_CFG_ADDR(m),mask,val,HWIO_RSI_TCSm_CFG_INI(m))
#define HWIO_RSI_TCSm_CFG_CMD_WAIT_BMSK                                    0xf0
#define HWIO_RSI_TCSm_CFG_CMD_WAIT_SHFT                                     0x4
#define HWIO_RSI_TCSm_CFG_CMD_EN_BMSK                                       0xf
#define HWIO_RSI_TCSm_CFG_CMD_EN_SHFT                                       0x0

#define HWIO_RSI_TCSm_STAT_ADDR(m)                                   (DDR_AUX_TCS_CFG_REG_BASE      + 0x00000040 + 0x4 * (m))
#define HWIO_RSI_TCSm_STAT_RMSK                                      0x80000fff
#define HWIO_RSI_TCSm_STAT_MAXm                                              13
#define HWIO_RSI_TCSm_STAT_ATTR                                             0x1
#define HWIO_RSI_TCSm_STAT_INI(m)        \
        in_dword_masked(HWIO_RSI_TCSm_STAT_ADDR(m), HWIO_RSI_TCSm_STAT_RMSK)
#define HWIO_RSI_TCSm_STAT_INMI(m,mask)    \
        in_dword_masked(HWIO_RSI_TCSm_STAT_ADDR(m), mask)
#define HWIO_RSI_TCSm_STAT_TCS_BUSY_BMSK                             0x80000000
#define HWIO_RSI_TCSm_STAT_TCS_BUSY_SHFT                                   0x1f
#define HWIO_RSI_TCSm_STAT_CMD_CMPL_STAT_BMSK                             0xf00
#define HWIO_RSI_TCSm_STAT_CMD_CMPL_STAT_SHFT                               0x8
#define HWIO_RSI_TCSm_STAT_CMD_SEND_STAT_BMSK                              0xf0
#define HWIO_RSI_TCSm_STAT_CMD_SEND_STAT_SHFT                               0x4
#define HWIO_RSI_TCSm_STAT_CMD_ISSUE_STAT_BMSK                              0xf
#define HWIO_RSI_TCSm_STAT_CMD_ISSUE_STAT_SHFT                              0x0

#define HWIO_RSI_TCSm_CMDt_ADDR(m,t)                                 (DDR_AUX_TCS_CFG_REG_BASE      + 0x00000080 + 0x10 * (m) + 0x4 * (t))
#define HWIO_RSI_TCSm_CMDt_RMSK                                      0xffffffff
#define HWIO_RSI_TCSm_CMDt_MAXm                                              13
#define HWIO_RSI_TCSm_CMDt_MAXt                                               3
#define HWIO_RSI_TCSm_CMDt_ATTR                                             0x3
#define HWIO_RSI_TCSm_CMDt_INI2(m,t)        \
        in_dword_masked(HWIO_RSI_TCSm_CMDt_ADDR(m,t), HWIO_RSI_TCSm_CMDt_RMSK)
#define HWIO_RSI_TCSm_CMDt_INMI2(m,t,mask)    \
        in_dword_masked(HWIO_RSI_TCSm_CMDt_ADDR(m,t), mask)
#define HWIO_RSI_TCSm_CMDt_OUTI2(m,t,val)    \
        out_dword(HWIO_RSI_TCSm_CMDt_ADDR(m,t),val)
#define HWIO_RSI_TCSm_CMDt_OUTMI2(m,t,mask,val) \
        out_dword_masked_ns(HWIO_RSI_TCSm_CMDt_ADDR(m,t),mask,val,HWIO_RSI_TCSm_CMDt_INI2(m,t))
#define HWIO_RSI_TCSm_CMDt_CTRL_BMSK                                 0xff000000
#define HWIO_RSI_TCSm_CMDt_CTRL_SHFT                                       0x18
#define HWIO_RSI_TCSm_CMDt_ADDR_BMSK                                   0xff0000
#define HWIO_RSI_TCSm_CMDt_ADDR_SHFT                                       0x10
#define HWIO_RSI_TCSm_CMDt_DATA_BMSK                                     0xffff
#define HWIO_RSI_TCSm_CMDt_DATA_SHFT                                        0x0

#define HWIO_RSI_TCSm_AMC_MODE_ADDR(m)                               (DDR_AUX_TCS_CFG_REG_BASE      + 0x00000180 + 0x4 * (m))
#define HWIO_RSI_TCSm_AMC_MODE_RMSK                                         0x3
#define HWIO_RSI_TCSm_AMC_MODE_MAXm                                          13
#define HWIO_RSI_TCSm_AMC_MODE_ATTR                                         0x0
#define HWIO_RSI_TCSm_AMC_MODE_INI(m)        \
        in_dword_masked(HWIO_RSI_TCSm_AMC_MODE_ADDR(m), HWIO_RSI_TCSm_AMC_MODE_RMSK)
#define HWIO_RSI_TCSm_AMC_MODE_INMI(m,mask)    \
        in_dword_masked(HWIO_RSI_TCSm_AMC_MODE_ADDR(m), mask)
#define HWIO_RSI_TCSm_AMC_MODE_OUTI(m,val)    \
        out_dword(HWIO_RSI_TCSm_AMC_MODE_ADDR(m),val)
#define HWIO_RSI_TCSm_AMC_MODE_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_RSI_TCSm_AMC_MODE_ADDR(m),mask,val,HWIO_RSI_TCSm_AMC_MODE_INI(m))
#define HWIO_RSI_TCSm_AMC_MODE_TCS_START_BMSK                               0x2
#define HWIO_RSI_TCSm_AMC_MODE_TCS_START_SHFT                               0x1
#define HWIO_RSI_TCSm_AMC_MODE_AMC_ENABLE_BMSK                              0x1
#define HWIO_RSI_TCSm_AMC_MODE_AMC_ENABLE_SHFT                              0x0

#define HWIO_RSI_TCS0_RESP_DATAm_ADDR(m)                             (DDR_AUX_TCS_CFG_REG_BASE      + 0x000001c0 + 0x4 * (m))
#define HWIO_RSI_TCS0_RESP_DATAm_RMSK                                    0xffff
#define HWIO_RSI_TCS0_RESP_DATAm_MAXm                                         3
#define HWIO_RSI_TCS0_RESP_DATAm_ATTR                                       0x1
#define HWIO_RSI_TCS0_RESP_DATAm_INI(m)        \
        in_dword_masked(HWIO_RSI_TCS0_RESP_DATAm_ADDR(m), HWIO_RSI_TCS0_RESP_DATAm_RMSK)
#define HWIO_RSI_TCS0_RESP_DATAm_INMI(m,mask)    \
        in_dword_masked(HWIO_RSI_TCS0_RESP_DATAm_ADDR(m), mask)
#define HWIO_RSI_TCS0_RESP_DATAm_RESP_DATA_BMSK                          0xffff
#define HWIO_RSI_TCS0_RESP_DATAm_RESP_DATA_SHFT                             0x0

#define HWIO_RSI_TCS_BUSY_SUMMARY_ADDR                               (DDR_AUX_TCS_CFG_REG_BASE      + 0x000001d0)
#define HWIO_RSI_TCS_BUSY_SUMMARY_RMSK                                   0x3fff
#define HWIO_RSI_TCS_BUSY_SUMMARY_ATTR                                      0x1
#define HWIO_RSI_TCS_BUSY_SUMMARY_IN          \
        in_dword_masked(HWIO_RSI_TCS_BUSY_SUMMARY_ADDR, HWIO_RSI_TCS_BUSY_SUMMARY_RMSK)
#define HWIO_RSI_TCS_BUSY_SUMMARY_INM(m)      \
        in_dword_masked(HWIO_RSI_TCS_BUSY_SUMMARY_ADDR, m)
#define HWIO_RSI_TCS_BUSY_SUMMARY_TCS_BUSY_BMSK                          0x3fff
#define HWIO_RSI_TCS_BUSY_SUMMARY_TCS_BUSY_SHFT                             0x0


#endif /* __HWIO_DDRAUX_TCS__ */

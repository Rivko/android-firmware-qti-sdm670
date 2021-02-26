#ifndef __APSS_HANG_STATUS_HWIO_H__
#define __APSS_HANG_STATUS_HWIO_H__
/*
===========================================================================
*/
/**
  @file apss_hang_status_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r47.0.0_MTO]
 
  This file contains HWIO register definitions for the following modules:
    APSS_SHARED_APSS_SHARED
    APSS_WDT_TMR1_APSS_WDT
    APSS_ALIAS_0_APSS_CPU_IPM
    APSS_ALIAS_1_APSS_CPU_IPM
    APSS_ALIAS_2_APSS_CPU_IPM
    APSS_ALIAS_3_APSS_CPU_IPM
    APSS_ALIAS_4_APSS_CPU_IPM
    APSS_ALIAS_5_APSS_CPU_IPM
    APSS_ALIAS_6_APSS_CPU_IPM
    APSS_ALIAS_7_APSS_CPU_IPM

  'Include' filters applied: 
  'Exclude' filters applied: 
*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/target/670/apss_hang_status_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#define APSS_HM_BASE  0x77800000

/*----------------------------------------------------------------------------
 * MODULE: APSS_SHARED_APSS_SHARED
 *--------------------------------------------------------------------------*/

#define APSS_SHARED_APSS_SHARED_REG_BASE                                          (APSS_HM_BASE      + 0x00190000)

#define HWIO_APSS_SHARED_SHR_SECURE_ADDR                                          (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000004)
#define HWIO_APSS_SHARED_SHR_SECURE_RMSK                                                 0x3
#define HWIO_APSS_SHARED_SHR_SECURE_IN          \
        in_dword_masked(HWIO_APSS_SHARED_SHR_SECURE_ADDR, HWIO_APSS_SHARED_SHR_SECURE_RMSK)
#define HWIO_APSS_SHARED_SHR_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_SHR_SECURE_ADDR, m)
#define HWIO_APSS_SHARED_SHR_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_SHR_SECURE_ADDR,v)
#define HWIO_APSS_SHARED_SHR_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_SHR_SECURE_ADDR,m,v,HWIO_APSS_SHARED_SHR_SECURE_IN)
#define HWIO_APSS_SHARED_SHR_SECURE_INTR_BMSK                                            0x2
#define HWIO_APSS_SHARED_SHR_SECURE_INTR_SHFT                                            0x1
#define HWIO_APSS_SHARED_SHR_SECURE_CTL_SEC_BMSK                                         0x1
#define HWIO_APSS_SHARED_SHR_SECURE_CTL_SEC_SHFT                                         0x0

#define HWIO_APSS_SHARED_TZ_IPC_INTERRUPT_ADDR                                    (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000008)
#define HWIO_APSS_SHARED_TZ_IPC_INTERRUPT_RMSK                                      0xf00007
#define HWIO_APSS_SHARED_TZ_IPC_INTERRUPT_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_TZ_IPC_INTERRUPT_ADDR,v)
#define HWIO_APSS_SHARED_TZ_IPC_INTERRUPT_TZ_SPARE_IPC_BMSK                         0xf00000
#define HWIO_APSS_SHARED_TZ_IPC_INTERRUPT_TZ_SPARE_IPC_SHFT                             0x14
#define HWIO_APSS_SHARED_TZ_IPC_INTERRUPT_RPM_TZ_IPC_BMSK                                0x7
#define HWIO_APSS_SHARED_TZ_IPC_INTERRUPT_RPM_TZ_IPC_SHFT                                0x0

#define HWIO_APSS_SHARED_IPC_INTERRUPT_ADDR                                       (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000000c)
#define HWIO_APSS_SHARED_IPC_INTERRUPT_RMSK                                        0xffffff7
#define HWIO_APSS_SHARED_IPC_INTERRUPT_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_IPC_INTERRUPT_ADDR,v)
#define HWIO_APSS_SHARED_IPC_INTERRUPT_SSC_IPC_BMSK                                0xf000000
#define HWIO_APSS_SHARED_IPC_INTERRUPT_SSC_IPC_SHFT                                     0x18
#define HWIO_APSS_SHARED_IPC_INTERRUPT_SPARE_IPC_BMSK                               0xf00000
#define HWIO_APSS_SHARED_IPC_INTERRUPT_SPARE_IPC_SHFT                                   0x14
#define HWIO_APSS_SHARED_IPC_INTERRUPT_WCN_IPC_BMSK                                  0xf0000
#define HWIO_APSS_SHARED_IPC_INTERRUPT_WCN_IPC_SHFT                                     0x10
#define HWIO_APSS_SHARED_IPC_INTERRUPT_MSS_IPC_BMSK                                   0xf000
#define HWIO_APSS_SHARED_IPC_INTERRUPT_MSS_IPC_SHFT                                      0xc
#define HWIO_APSS_SHARED_IPC_INTERRUPT_ADSP_IPC_BMSK                                   0xf00
#define HWIO_APSS_SHARED_IPC_INTERRUPT_ADSP_IPC_SHFT                                     0x8
#define HWIO_APSS_SHARED_IPC_INTERRUPT_CDSP_IPC_BMSK                                    0xf0
#define HWIO_APSS_SHARED_IPC_INTERRUPT_CDSP_IPC_SHFT                                     0x4
#define HWIO_APSS_SHARED_IPC_INTERRUPT_RPM_HLOS_IPC_BMSK                                 0x7
#define HWIO_APSS_SHARED_IPC_INTERRUPT_RPM_HLOS_IPC_SHFT                                 0x0

#define HWIO_APSS_SHARED_KRYO_CFG_SIG_ADDR                                        (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000010)
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_RMSK                                        0xffff007c
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_IN          \
        in_dword_masked(HWIO_APSS_SHARED_KRYO_CFG_SIG_ADDR, HWIO_APSS_SHARED_KRYO_CFG_SIG_RMSK)
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_KRYO_CFG_SIG_ADDR, m)
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_KRYO_CFG_SIG_ADDR,v)
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_KRYO_CFG_SIG_ADDR,m,v,HWIO_APSS_SHARED_KRYO_CFG_SIG_IN)
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_CLUSTERIDAFF3_BMSK                          0xff000000
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_CLUSTERIDAFF3_SHFT                                0x18
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_CLUSTERIDAFF2_BMSK                            0xff0000
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_CLUSTERIDAFF2_SHFT                                0x10
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTPERSIST_BMSK                             0x40
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTPERSIST_SHFT                              0x6
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTINNER_BMSK                               0x20
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTINNER_SHFT                                0x5
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTOUTER_BMSK                               0x10
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTOUTER_SHFT                                0x4
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTCACHEMAINT_BMSK                           0x8
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTCACHEMAINT_SHFT                           0x3
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTCACHEMAINTPOU_BMSK                        0x4
#define HWIO_APSS_SHARED_KRYO_CFG_SIG_BROADCASTCACHEMAINTPOU_SHFT                        0x2

#define HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_ADDR                                   (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000014)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_RMSK                                   0xffffffff
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_IN          \
        in_dword_masked(HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_ADDR, HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_RMSK)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_ADDR, m)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_ADDR,v)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_ADDR,m,v,HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_IN)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_RVBARADDR_LO_REG_BMSK                  0xffffffff
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_LO_RVBARADDR_LO_REG_SHFT                         0x0

#define HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_ADDR                                   (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000018)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_RMSK                                   0xffffffff
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_IN          \
        in_dword_masked(HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_ADDR, HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_RMSK)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_ADDR, m)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_ADDR,v)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_ADDR,m,v,HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_IN)
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_RVBARADDR_HI_REG_BMSK                  0xffffffff
#define HWIO_APSS_SHARED_KRYO_RVBARADDR_HI_RVBARADDR_HI_REG_SHFT                         0x0

#define HWIO_APSS_SHARED_MISC_SIG_CTL_ADDR                                        (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000001c)
#define HWIO_APSS_SHARED_MISC_SIG_CTL_RMSK                                            0x1fff
#define HWIO_APSS_SHARED_MISC_SIG_CTL_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MISC_SIG_CTL_ADDR, HWIO_APSS_SHARED_MISC_SIG_CTL_RMSK)
#define HWIO_APSS_SHARED_MISC_SIG_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MISC_SIG_CTL_ADDR, m)
#define HWIO_APSS_SHARED_MISC_SIG_CTL_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MISC_SIG_CTL_ADDR,v)
#define HWIO_APSS_SHARED_MISC_SIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MISC_SIG_CTL_ADDR,m,v,HWIO_APSS_SHARED_MISC_SIG_CTL_IN)
#define HWIO_APSS_SHARED_MISC_SIG_CTL_LMH_CURR_CTRL_TRIM_BMSK                         0x1000
#define HWIO_APSS_SHARED_MISC_SIG_CTL_LMH_CURR_CTRL_TRIM_SHFT                            0xc
#define HWIO_APSS_SHARED_MISC_SIG_CTL_GOLD_LATE_CANCEL_BMSK                            0x800
#define HWIO_APSS_SHARED_MISC_SIG_CTL_GOLD_LATE_CANCEL_SHFT                              0xb
#define HWIO_APSS_SHARED_MISC_SIG_CTL_GIC_SLP_RET_BMSK                                 0x600
#define HWIO_APSS_SHARED_MISC_SIG_CTL_GIC_SLP_RET_SHFT                                   0x9
#define HWIO_APSS_SHARED_MISC_SIG_CTL_GIC_SLP_NRET_BMSK                                0x180
#define HWIO_APSS_SHARED_MISC_SIG_CTL_GIC_SLP_NRET_SHFT                                  0x7
#define HWIO_APSS_SHARED_MISC_SIG_CTL_OSM_SLP_RET_BMSK                                  0x40
#define HWIO_APSS_SHARED_MISC_SIG_CTL_OSM_SLP_RET_SHFT                                   0x6
#define HWIO_APSS_SHARED_MISC_SIG_CTL_OSM_SLP_NRET_BMSK                                 0x20
#define HWIO_APSS_SHARED_MISC_SIG_CTL_OSM_SLP_NRET_SHFT                                  0x5
#define HWIO_APSS_SHARED_MISC_SIG_CTL_CLREXMONREQ_VAL_BMSK                              0x10
#define HWIO_APSS_SHARED_MISC_SIG_CTL_CLREXMONREQ_VAL_SHFT                               0x4
#define HWIO_APSS_SHARED_MISC_SIG_CTL_CLREXMONREQ_EN_BMSK                                0x8
#define HWIO_APSS_SHARED_MISC_SIG_CTL_CLREXMONREQ_EN_SHFT                                0x3
#define HWIO_APSS_SHARED_MISC_SIG_CTL_CLREXMONACK_VAL_BMSK                               0x4
#define HWIO_APSS_SHARED_MISC_SIG_CTL_CLREXMONACK_VAL_SHFT                               0x2
#define HWIO_APSS_SHARED_MISC_SIG_CTL_CLREXMONACK_EN_BMSK                                0x2
#define HWIO_APSS_SHARED_MISC_SIG_CTL_CLREXMONACK_EN_SHFT                                0x1
#define HWIO_APSS_SHARED_MISC_SIG_CTL_DBG_CTL_SEC_UP_REQ_BMSK                            0x1
#define HWIO_APSS_SHARED_MISC_SIG_CTL_DBG_CTL_SEC_UP_REQ_SHFT                            0x0

#define HWIO_APSS_SHARED_MISC_SIG_STS_ADDR                                        (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000020)
#define HWIO_APSS_SHARED_MISC_SIG_STS_RMSK                                               0xf
#define HWIO_APSS_SHARED_MISC_SIG_STS_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MISC_SIG_STS_ADDR, HWIO_APSS_SHARED_MISC_SIG_STS_RMSK)
#define HWIO_APSS_SHARED_MISC_SIG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MISC_SIG_STS_ADDR, m)
#define HWIO_APSS_SHARED_MISC_SIG_STS_GOLD_ISENSECTL_TRIM_BUSY_BMSK                      0x8
#define HWIO_APSS_SHARED_MISC_SIG_STS_GOLD_ISENSECTL_TRIM_BUSY_SHFT                      0x3
#define HWIO_APSS_SHARED_MISC_SIG_STS_HW_CLREXMONACK_BMSK                                0x4
#define HWIO_APSS_SHARED_MISC_SIG_STS_HW_CLREXMONACK_SHFT                                0x2
#define HWIO_APSS_SHARED_MISC_SIG_STS_HW_CLREXMONREQ_BMSK                                0x2
#define HWIO_APSS_SHARED_MISC_SIG_STS_HW_CLREXMONREQ_SHFT                                0x1
#define HWIO_APSS_SHARED_MISC_SIG_STS_HW_DBG_CTL_SEC_UP_REQ_BMSK                         0x1
#define HWIO_APSS_SHARED_MISC_SIG_STS_HW_DBG_CTL_SEC_UP_REQ_SHFT                         0x0

#define HWIO_APSS_SHARED_CPU_PRESENT_ADDR                                         (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000024)
#define HWIO_APSS_SHARED_CPU_PRESENT_RMSK                                               0xff
#define HWIO_APSS_SHARED_CPU_PRESENT_IN          \
        in_dword_masked(HWIO_APSS_SHARED_CPU_PRESENT_ADDR, HWIO_APSS_SHARED_CPU_PRESENT_RMSK)
#define HWIO_APSS_SHARED_CPU_PRESENT_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_CPU_PRESENT_ADDR, m)
#define HWIO_APSS_SHARED_CPU_PRESENT_CORE_PRESENT_BMSK                                  0xff
#define HWIO_APSS_SHARED_CPU_PRESENT_CORE_PRESENT_SHFT                                   0x0

#define HWIO_APSS_SHARED_GNOC_CFG_SIG_ADDR                                        (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000028)
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_RMSK                                              0xa0
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_CFG_SIG_ADDR, HWIO_APSS_SHARED_GNOC_CFG_SIG_RMSK)
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_CFG_SIG_ADDR, m)
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_GNOC_CFG_SIG_ADDR,v)
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_GNOC_CFG_SIG_ADDR,m,v,HWIO_APSS_SHARED_GNOC_CFG_SIG_IN)
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_GNOC_TSREADY_FORCE_DISABLE_BMSK                   0x80
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_GNOC_TSREADY_FORCE_DISABLE_SHFT                    0x7
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_PROTNSS2_BMSK                                     0x20
#define HWIO_APSS_SHARED_GNOC_CFG_SIG_PROTNSS2_SHFT                                      0x5

#define HWIO_APSS_SHARED_QOS_ACE_ADDR                                             (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000002c)
#define HWIO_APSS_SHARED_QOS_ACE_RMSK                                                    0xf
#define HWIO_APSS_SHARED_QOS_ACE_IN          \
        in_dword_masked(HWIO_APSS_SHARED_QOS_ACE_ADDR, HWIO_APSS_SHARED_QOS_ACE_RMSK)
#define HWIO_APSS_SHARED_QOS_ACE_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_QOS_ACE_ADDR, m)
#define HWIO_APSS_SHARED_QOS_ACE_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_QOS_ACE_ADDR,v)
#define HWIO_APSS_SHARED_QOS_ACE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_QOS_ACE_ADDR,m,v,HWIO_APSS_SHARED_QOS_ACE_IN)
#define HWIO_APSS_SHARED_QOS_ACE_AWQOS_BMSK                                              0xc
#define HWIO_APSS_SHARED_QOS_ACE_AWQOS_SHFT                                              0x2
#define HWIO_APSS_SHARED_QOS_ACE_ARQOS_BMSK                                              0x3
#define HWIO_APSS_SHARED_QOS_ACE_ARQOS_SHFT                                              0x0

#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_ADDR                                  (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000034)
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_RMSK                                  0x80001fff
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_ADDR, HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_RMSK)
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_ADDR, m)
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_ADDR,v)
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_ADDR,m,v,HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_IN)
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_TESTBUS_MUX_EN_BMSK                   0x80000000
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_TESTBUS_MUX_EN_SHFT                         0x1f
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_TESTBUS_LVL2_OLC_SEL_BMSK                 0x1e00
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_TESTBUS_LVL2_OLC_SEL_SHFT                    0x9
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_TESTBUS_LVL2_SEL_BMSK                      0x1f0
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_TESTBUS_LVL2_SEL_SHFT                        0x4
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_HWEVENT_SEL_BMSK                             0x8
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_HWEVENT_SEL_SHFT                             0x3
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_TESTBUS_LVL1_SEL_BMSK                        0x7
#define HWIO_APSS_SHARED_TESTBUS_MUX_CONFIG_TESTBUS_LVL1_SEL_SHFT                        0x0

#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_ADDR                               (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000038)
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_RMSK                                     0x1f
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_ADDR, HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_RMSK)
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_ADDR, m)
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_ADDR,v)
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_ADDR,m,v,HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_IN)
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_GIC_FAULT_INT_SEL_BMSK                   0x10
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_GIC_FAULT_INT_SEL_SHFT                    0x4
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_GIC_ERR_INT_SEL_BMSK                      0x8
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_GIC_ERR_INT_SEL_SHFT                      0x3
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_GNOC_HANG_STS_SEL_BMSK                    0x4
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_GNOC_HANG_STS_SEL_SHFT                    0x2
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_CORE_HANG_STS_SEL_BMSK                    0x2
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_CORE_HANG_STS_SEL_SHFT                    0x1
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_WDOG_BITE_INT_SEL_BMSK                    0x1
#define HWIO_APSS_SHARED_WDOG_BITE_INT0_CONFIG_WDOG_BITE_INT_SEL_SHFT                    0x0

#define HWIO_APSS_SHARED_FIRST_CORE_HANG_ADDR                                     (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000003c)
#define HWIO_APSS_SHARED_FIRST_CORE_HANG_RMSK                                           0xff
#define HWIO_APSS_SHARED_FIRST_CORE_HANG_IN          \
        in_dword_masked(HWIO_APSS_SHARED_FIRST_CORE_HANG_ADDR, HWIO_APSS_SHARED_FIRST_CORE_HANG_RMSK)
#define HWIO_APSS_SHARED_FIRST_CORE_HANG_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_FIRST_CORE_HANG_ADDR, m)
#define HWIO_APSS_SHARED_FIRST_CORE_HANG_CORE_NUMBER_BMSK                               0xff
#define HWIO_APSS_SHARED_FIRST_CORE_HANG_CORE_NUMBER_SHFT                                0x0

#define HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_ADDR                                     (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000040)
#define HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_RMSK                                            0x1
#define HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_ADDR, HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_RMSK)
#define HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_ADDR, m)
#define HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_ADDR,v)
#define HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_ADDR,m,v,HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_IN)
#define HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_MEMTYPE_OVERRIDE_BMSK                           0x1
#define HWIO_APSS_SHARED_GIC_NOC_MEMTYPE_MEMTYPE_OVERRIDE_SHFT                           0x0

#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_ADDR                                    (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000044)
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_RMSK                                       0x3ffff
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_IN          \
        in_dword_masked(HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_ADDR, HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_RMSK)
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_ADDR, m)
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_ADDR,v)
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_ADDR,m,v,HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_IN)
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_SYSCO_ACK_OVRRIDE_EN_BMSK                  0x20000
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_SYSCO_ACK_OVRRIDE_EN_SHFT                     0x11
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_SYSCO_ACK_OVRRIDE_VAL_BMSK                 0x10000
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_SYSCO_ACK_OVRRIDE_VAL_SHFT                    0x10
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_SYSCO_HS_BYP_STS_BMSK                       0xffff
#define HWIO_APSS_SHARED_SYSCO_HS_BYP_CSR_SYSCO_HS_BYP_STS_SHFT                          0x0

#define HWIO_APSS_SHARED_APSS_ID_ADDR                                             (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000050)
#define HWIO_APSS_SHARED_APSS_ID_RMSK                                             0xffffffff
#define HWIO_APSS_SHARED_APSS_ID_IN          \
        in_dword_masked(HWIO_APSS_SHARED_APSS_ID_ADDR, HWIO_APSS_SHARED_APSS_ID_RMSK)
#define HWIO_APSS_SHARED_APSS_ID_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_APSS_ID_ADDR, m)
#define HWIO_APSS_SHARED_APSS_ID_SPARE_BMSK                                       0xffff0000
#define HWIO_APSS_SHARED_APSS_ID_SPARE_SHFT                                             0x10
#define HWIO_APSS_SHARED_APSS_ID_ARCH_FAMILY_BMSK                                     0xf000
#define HWIO_APSS_SHARED_APSS_ID_ARCH_FAMILY_SHFT                                        0xc
#define HWIO_APSS_SHARED_APSS_ID_CONFIGURATION_BMSK                                    0xff0
#define HWIO_APSS_SHARED_APSS_ID_CONFIGURATION_SHFT                                      0x4
#define HWIO_APSS_SHARED_APSS_ID_REVISION_BMSK                                           0xf
#define HWIO_APSS_SHARED_APSS_ID_REVISION_SHFT                                           0x0

#define HWIO_APSS_SHARED_APSS_ECO_ADDR                                            (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000054)
#define HWIO_APSS_SHARED_APSS_ECO_RMSK                                            0xffffffff
#define HWIO_APSS_SHARED_APSS_ECO_IN          \
        in_dword_masked(HWIO_APSS_SHARED_APSS_ECO_ADDR, HWIO_APSS_SHARED_APSS_ECO_RMSK)
#define HWIO_APSS_SHARED_APSS_ECO_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_APSS_ECO_ADDR, m)
#define HWIO_APSS_SHARED_APSS_ECO_ECO_ID_BMSK                                     0xffffffff
#define HWIO_APSS_SHARED_APSS_ECO_ECO_ID_SHFT                                            0x0

#define HWIO_APSS_SHARED_APSS_SKU_ADDR                                            (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000058)
#define HWIO_APSS_SHARED_APSS_SKU_RMSK                                            0xffffffff
#define HWIO_APSS_SHARED_APSS_SKU_IN          \
        in_dword_masked(HWIO_APSS_SHARED_APSS_SKU_ADDR, HWIO_APSS_SHARED_APSS_SKU_RMSK)
#define HWIO_APSS_SHARED_APSS_SKU_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_APSS_SKU_ADDR, m)
#define HWIO_APSS_SHARED_APSS_SKU_SPARE_BMSK                                      0xfffff000
#define HWIO_APSS_SHARED_APSS_SKU_SPARE_SHFT                                             0xc
#define HWIO_APSS_SHARED_APSS_SKU_FREQ_LMT_BMSK                                        0xc00
#define HWIO_APSS_SHARED_APSS_SKU_FREQ_LMT_SHFT                                          0xa
#define HWIO_APSS_SHARED_APSS_SKU_L3_SIZE_BMSK                                         0x300
#define HWIO_APSS_SHARED_APSS_SKU_L3_SIZE_SHFT                                           0x8
#define HWIO_APSS_SHARED_APSS_SKU_AG_COUNT_BMSK                                         0xf0
#define HWIO_APSS_SHARED_APSS_SKU_AG_COUNT_SHFT                                          0x4
#define HWIO_APSS_SHARED_APSS_SKU_AU_COUNT_BMSK                                          0xf
#define HWIO_APSS_SHARED_APSS_SKU_AU_COUNT_SHFT                                          0x0

#define HWIO_APSS_SHARED_SPI_CONFIG_n_ADDR(n)                                     (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x000000f0 + 0x4 * (n))
#define HWIO_APSS_SHARED_SPI_CONFIG_n_RMSK                                        0xffffffff
#define HWIO_APSS_SHARED_SPI_CONFIG_n_MAXn                                                20
#define HWIO_APSS_SHARED_SPI_CONFIG_n_INI(n)        \
        in_dword_masked(HWIO_APSS_SHARED_SPI_CONFIG_n_ADDR(n), HWIO_APSS_SHARED_SPI_CONFIG_n_RMSK)
#define HWIO_APSS_SHARED_SPI_CONFIG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_SHARED_SPI_CONFIG_n_ADDR(n), mask)
#define HWIO_APSS_SHARED_SPI_CONFIG_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_SHARED_SPI_CONFIG_n_ADDR(n),val)
#define HWIO_APSS_SHARED_SPI_CONFIG_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_SHARED_SPI_CONFIG_n_ADDR(n),mask,val,HWIO_APSS_SHARED_SPI_CONFIG_n_INI(n))
#define HWIO_APSS_SHARED_SPI_CONFIG_n_SPI_NUMBER_BMSK                             0xffffffff
#define HWIO_APSS_SHARED_SPI_CONFIG_n_SPI_NUMBER_SHFT                                    0x0

#define HWIO_APSS_SHARED_PPI_CONFIG_n_ADDR(n)                                     (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000150 + 0x4 * (n))
#define HWIO_APSS_SHARED_PPI_CONFIG_n_RMSK                                        0xffffffff
#define HWIO_APSS_SHARED_PPI_CONFIG_n_MAXn                                                 3
#define HWIO_APSS_SHARED_PPI_CONFIG_n_INI(n)        \
        in_dword_masked(HWIO_APSS_SHARED_PPI_CONFIG_n_ADDR(n), HWIO_APSS_SHARED_PPI_CONFIG_n_RMSK)
#define HWIO_APSS_SHARED_PPI_CONFIG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_SHARED_PPI_CONFIG_n_ADDR(n), mask)
#define HWIO_APSS_SHARED_PPI_CONFIG_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_SHARED_PPI_CONFIG_n_ADDR(n),val)
#define HWIO_APSS_SHARED_PPI_CONFIG_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_SHARED_PPI_CONFIG_n_ADDR(n),mask,val,HWIO_APSS_SHARED_PPI_CONFIG_n_INI(n))
#define HWIO_APSS_SHARED_PPI_CONFIG_n_PPI_NUMBER_BMSK                             0xffffffff
#define HWIO_APSS_SHARED_PPI_CONFIG_n_PPI_NUMBER_SHFT                                    0x0

#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_ADDR                                    (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000160)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_RMSK                                    0xffffffff
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_SILVER_0_ADDR, HWIO_APSS_SHARED_MEM_ACC_SILVER_0_RMSK)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_SILVER_0_ADDR, m)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MEM_ACC_SILVER_0_ADDR,v)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_ACC_SILVER_0_ADDR,m,v,HWIO_APSS_SHARED_MEM_ACC_SILVER_0_IN)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_MEM_ACC_TYPE4_BMSK                      0xff000000
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_MEM_ACC_TYPE4_SHFT                            0x18
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_MEM_ACC_TYPE3_BMSK                        0xff0000
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_MEM_ACC_TYPE3_SHFT                            0x10
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_MEM_ACC_TYPE2_BMSK                          0xff00
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_MEM_ACC_TYPE2_SHFT                             0x8
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_MEM_ACC_TYPE1_BMSK                            0xff
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_0_MEM_ACC_TYPE1_SHFT                             0x0

#define HWIO_APSS_SHARED_MEM_ACC_SILVER_1_ADDR                                    (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000164)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_1_RMSK                                          0xff
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_1_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_SILVER_1_ADDR, HWIO_APSS_SHARED_MEM_ACC_SILVER_1_RMSK)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_1_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_SILVER_1_ADDR, m)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_1_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MEM_ACC_SILVER_1_ADDR,v)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_ACC_SILVER_1_ADDR,m,v,HWIO_APSS_SHARED_MEM_ACC_SILVER_1_IN)
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_1_MEM_ACC_TYPE5_BMSK                            0xff
#define HWIO_APSS_SHARED_MEM_ACC_SILVER_1_MEM_ACC_TYPE5_SHFT                             0x0

#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_ADDR                                      (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000168)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_RMSK                                      0xffffffff
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_GOLD_0_ADDR, HWIO_APSS_SHARED_MEM_ACC_GOLD_0_RMSK)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_GOLD_0_ADDR, m)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MEM_ACC_GOLD_0_ADDR,v)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_ACC_GOLD_0_ADDR,m,v,HWIO_APSS_SHARED_MEM_ACC_GOLD_0_IN)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_MEM_ACC_TYPE4_BMSK                        0xff000000
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_MEM_ACC_TYPE4_SHFT                              0x18
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_MEM_ACC_TYPE3_BMSK                          0xff0000
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_MEM_ACC_TYPE3_SHFT                              0x10
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_MEM_ACC_TYPE2_BMSK                            0xff00
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_MEM_ACC_TYPE2_SHFT                               0x8
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_MEM_ACC_TYPE1_BMSK                              0xff
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_0_MEM_ACC_TYPE1_SHFT                               0x0

#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_ADDR                                      (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000016c)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_RMSK                                        0xffffff
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_GOLD_1_ADDR, HWIO_APSS_SHARED_MEM_ACC_GOLD_1_RMSK)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_GOLD_1_ADDR, m)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MEM_ACC_GOLD_1_ADDR,v)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_ACC_GOLD_1_ADDR,m,v,HWIO_APSS_SHARED_MEM_ACC_GOLD_1_IN)
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_MEM_ACC_TYPE7_BMSK                          0xff0000
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_MEM_ACC_TYPE7_SHFT                              0x10
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_MEM_ACC_TYPE6_BMSK                            0xff00
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_MEM_ACC_TYPE6_SHFT                               0x8
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_MEM_ACC_TYPE5_BMSK                              0xff
#define HWIO_APSS_SHARED_MEM_ACC_GOLD_1_MEM_ACC_TYPE5_SHFT                               0x0

#define HWIO_APSS_SHARED_MEM_ACC_L3_ADDR                                          (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000170)
#define HWIO_APSS_SHARED_MEM_ACC_L3_RMSK                                              0xffff
#define HWIO_APSS_SHARED_MEM_ACC_L3_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_L3_ADDR, HWIO_APSS_SHARED_MEM_ACC_L3_RMSK)
#define HWIO_APSS_SHARED_MEM_ACC_L3_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_L3_ADDR, m)
#define HWIO_APSS_SHARED_MEM_ACC_L3_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MEM_ACC_L3_ADDR,v)
#define HWIO_APSS_SHARED_MEM_ACC_L3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_ACC_L3_ADDR,m,v,HWIO_APSS_SHARED_MEM_ACC_L3_IN)
#define HWIO_APSS_SHARED_MEM_ACC_L3_MEM_ACC_TYPE2_BMSK                                0xff00
#define HWIO_APSS_SHARED_MEM_ACC_L3_MEM_ACC_TYPE2_SHFT                                   0x8
#define HWIO_APSS_SHARED_MEM_ACC_L3_MEM_ACC_TYPE1_BMSK                                  0xff
#define HWIO_APSS_SHARED_MEM_ACC_L3_MEM_ACC_TYPE1_SHFT                                   0x0

#define HWIO_APSS_SHARED_MEM_SVS_CX_ADDR                                          (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000174)
#define HWIO_APSS_SHARED_MEM_SVS_CX_RMSK                                                 0x1
#define HWIO_APSS_SHARED_MEM_SVS_CX_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MEM_SVS_CX_ADDR, HWIO_APSS_SHARED_MEM_SVS_CX_RMSK)
#define HWIO_APSS_SHARED_MEM_SVS_CX_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MEM_SVS_CX_ADDR, m)
#define HWIO_APSS_SHARED_MEM_SVS_CX_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MEM_SVS_CX_ADDR,v)
#define HWIO_APSS_SHARED_MEM_SVS_CX_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_SVS_CX_ADDR,m,v,HWIO_APSS_SHARED_MEM_SVS_CX_IN)
#define HWIO_APSS_SHARED_MEM_SVS_CX_MEM_SVS_BMSK                                         0x1
#define HWIO_APSS_SHARED_MEM_SVS_CX_MEM_SVS_SHFT                                         0x0

#define HWIO_APSS_SHARED_MEM_SVS_APC0_ADDR                                        (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000178)
#define HWIO_APSS_SHARED_MEM_SVS_APC0_RMSK                                               0x1
#define HWIO_APSS_SHARED_MEM_SVS_APC0_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MEM_SVS_APC0_ADDR, HWIO_APSS_SHARED_MEM_SVS_APC0_RMSK)
#define HWIO_APSS_SHARED_MEM_SVS_APC0_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MEM_SVS_APC0_ADDR, m)
#define HWIO_APSS_SHARED_MEM_SVS_APC0_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MEM_SVS_APC0_ADDR,v)
#define HWIO_APSS_SHARED_MEM_SVS_APC0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_SVS_APC0_ADDR,m,v,HWIO_APSS_SHARED_MEM_SVS_APC0_IN)
#define HWIO_APSS_SHARED_MEM_SVS_APC0_MEM_SVS_BMSK                                       0x1
#define HWIO_APSS_SHARED_MEM_SVS_APC0_MEM_SVS_SHFT                                       0x0

#define HWIO_APSS_SHARED_MEM_SVS_APC1_ADDR                                        (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000017c)
#define HWIO_APSS_SHARED_MEM_SVS_APC1_RMSK                                               0x1
#define HWIO_APSS_SHARED_MEM_SVS_APC1_IN          \
        in_dword_masked(HWIO_APSS_SHARED_MEM_SVS_APC1_ADDR, HWIO_APSS_SHARED_MEM_SVS_APC1_RMSK)
#define HWIO_APSS_SHARED_MEM_SVS_APC1_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_MEM_SVS_APC1_ADDR, m)
#define HWIO_APSS_SHARED_MEM_SVS_APC1_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_MEM_SVS_APC1_ADDR,v)
#define HWIO_APSS_SHARED_MEM_SVS_APC1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_SVS_APC1_ADDR,m,v,HWIO_APSS_SHARED_MEM_SVS_APC1_IN)
#define HWIO_APSS_SHARED_MEM_SVS_APC1_MEM_SVS_BMSK                                       0x1
#define HWIO_APSS_SHARED_MEM_SVS_APC1_MEM_SVS_SHFT                                       0x0

#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_ADDR(n)                                   (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000180 + 0x4 * (n))
#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_RMSK                                            0xff
#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_MAXn                                               7
#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_INI(n)        \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_COMP_n_ADDR(n), HWIO_APSS_SHARED_MEM_ACC_COMP_n_RMSK)
#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_SHARED_MEM_ACC_COMP_n_ADDR(n), mask)
#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_SHARED_MEM_ACC_COMP_n_ADDR(n),val)
#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_SHARED_MEM_ACC_COMP_n_ADDR(n),mask,val,HWIO_APSS_SHARED_MEM_ACC_COMP_n_INI(n))
#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_MEM_ACC_COMP_BMSK                               0xff
#define HWIO_APSS_SHARED_MEM_ACC_COMP_n_MEM_ACC_COMP_SHFT                                0x0

#define HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_ADDR                                     (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000204)
#define HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_RMSK                                            0x1
#define HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_IN          \
        in_dword_masked(HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_ADDR, HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_RMSK)
#define HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_ADDR, m)
#define HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_ADDR,v)
#define HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_ADDR,m,v,HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_IN)
#define HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_ACC_MEM_SVS_SEL_BMSK                            0x1
#define HWIO_APSS_SHARED_ACC_MEM_SVS_SEL_ACC_MEM_SVS_SEL_SHFT                            0x0

#define HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_ADDR(n)                              (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000404 + 0x4 * (n))
#define HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_RMSK                                 0xffffffff
#define HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_MAXn                                          1
#define HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_INI(n)        \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_ADDR(n), HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_RMSK)
#define HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_ADDR(n), mask)
#define HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_GNOC_HANG_VALUE_BMSK                 0xffffffff
#define HWIO_APSS_SHARED_GNOC_HANG_COUNTER_n_GNOC_HANG_VALUE_SHFT                        0x0

#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_ADDR(n)                            (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000041c + 0x4 * (n))
#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_RMSK                               0xffffffff
#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_MAXn                                        1
#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_INI(n)        \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_ADDR(n), HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_RMSK)
#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_ADDR(n), mask)
#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_ADDR(n),val)
#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_ADDR(n),mask,val,HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_INI(n))
#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_THRESHOLD_VALUE_BMSK               0xffffffff
#define HWIO_APSS_SHARED_GNOC_HANG_THRESHOLD_n_THRESHOLD_VALUE_SHFT                      0x0

#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ADDR                                    (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000434)
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_RMSK                                         0x303
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ADDR, HWIO_APSS_SHARED_GNOC_HANG_CONFIG_RMSK)
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ADDR,m,v,HWIO_APSS_SHARED_GNOC_HANG_CONFIG_IN)
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_IO_COUNTER_STS_BMSK                          0x200
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_IO_COUNTER_STS_SHFT                            0x9
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ACE_COUNTER_STS_BMSK                         0x100
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ACE_COUNTER_STS_SHFT                           0x8
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_IO_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_IO_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ACE_COUNTER_EN_BMSK                            0x1
#define HWIO_APSS_SHARED_GNOC_HANG_CONFIG_ACE_COUNTER_EN_SHFT                            0x0

#define HWIO_APSS_SHARED_GIC_ERR_IPC_ADDR                                         (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000438)
#define HWIO_APSS_SHARED_GIC_ERR_IPC_RMSK                                                0xf
#define HWIO_APSS_SHARED_GIC_ERR_IPC_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GIC_ERR_IPC_ADDR, HWIO_APSS_SHARED_GIC_ERR_IPC_RMSK)
#define HWIO_APSS_SHARED_GIC_ERR_IPC_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GIC_ERR_IPC_ADDR, m)
#define HWIO_APSS_SHARED_GIC_ERR_IPC_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_GIC_ERR_IPC_ADDR,v)
#define HWIO_APSS_SHARED_GIC_ERR_IPC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_GIC_ERR_IPC_ADDR,m,v,HWIO_APSS_SHARED_GIC_ERR_IPC_IN)
#define HWIO_APSS_SHARED_GIC_ERR_IPC_ERR_INT_CLR_BMSK                                    0x8
#define HWIO_APSS_SHARED_GIC_ERR_IPC_ERR_INT_CLR_SHFT                                    0x3
#define HWIO_APSS_SHARED_GIC_ERR_IPC_ERR_INT_BMSK                                        0x4
#define HWIO_APSS_SHARED_GIC_ERR_IPC_ERR_INT_SHFT                                        0x2
#define HWIO_APSS_SHARED_GIC_ERR_IPC_FAULT_INT_CLR_BMSK                                  0x2
#define HWIO_APSS_SHARED_GIC_ERR_IPC_FAULT_INT_CLR_SHFT                                  0x1
#define HWIO_APSS_SHARED_GIC_ERR_IPC_FAULT_INT_BMSK                                      0x1
#define HWIO_APSS_SHARED_GIC_ERR_IPC_FAULT_INT_SHFT                                      0x0

#define HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_ADDR                             (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000043c)
#define HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_RMSK                                    0x3
#define HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_ADDR, HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_RMSK)
#define HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_ADDR, m)
#define HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_IO_CHANNEL_BMSK                         0x2
#define HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_IO_CHANNEL_SHFT                         0x1
#define HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_ACE_CHANNEL_BMSK                        0x1
#define HWIO_APSS_SHARED_GNOC_FIRST_CHANNEL_HANG_ACE_CHANNEL_SHFT                        0x0

#define HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_ADDR                                 (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000440)
#define HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_RMSK                                 0xffffffff
#define HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_ADDR, HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_RMSK)
#define HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_ADDR, m)
#define HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_GNOC_HANG_DBG_STS_31_0_BMSK          0xffffffff
#define HWIO_APSS_SHARED_GNOC_HANG_DBG_STS_0_GNOC_HANG_DBG_STS_31_0_SHFT                 0x0

#define HWIO_APSS_SHARED_GIC_MEM_MAP_ADDR                                         (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000444)
#define HWIO_APSS_SHARED_GIC_MEM_MAP_RMSK                                         0xffffffff
#define HWIO_APSS_SHARED_GIC_MEM_MAP_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GIC_MEM_MAP_ADDR, HWIO_APSS_SHARED_GIC_MEM_MAP_RMSK)
#define HWIO_APSS_SHARED_GIC_MEM_MAP_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GIC_MEM_MAP_ADDR, m)
#define HWIO_APSS_SHARED_GIC_MEM_MAP_OUT(v)      \
        out_dword(HWIO_APSS_SHARED_GIC_MEM_MAP_ADDR,v)
#define HWIO_APSS_SHARED_GIC_MEM_MAP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_SHARED_GIC_MEM_MAP_ADDR,m,v,HWIO_APSS_SHARED_GIC_MEM_MAP_IN)
#define HWIO_APSS_SHARED_GIC_MEM_MAP_GICD_PAGE_OFFSET_BMSK                        0xffff0000
#define HWIO_APSS_SHARED_GIC_MEM_MAP_GICD_PAGE_OFFSET_SHFT                              0x10
#define HWIO_APSS_SHARED_GIC_MEM_MAP_ITS_TRANSR_PAGE_OFFSET_BMSK                      0xffff
#define HWIO_APSS_SHARED_GIC_MEM_MAP_ITS_TRANSR_PAGE_OFFSET_SHFT                         0x0

#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_ADDR                                (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000448)
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RMSK                                     0x1ff
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_ADDR, HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RMSK)
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_ADDR, m)
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RSP_WAIT_BMSK                            0x100
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RSP_WAIT_SHFT                              0x8
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RSP_PBUSY_BMSK                            0x80
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RSP_PBUSY_SHFT                             0x7
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RSP_IDLE_BMSK                             0x40
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RSP_IDLE_SHFT                              0x6
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RSP_BUSY_BMSK                             0x20
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_RSP_BUSY_SHFT                              0x5
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_REQ_WAIT_BMSK                             0x10
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_REQ_WAIT_SHFT                              0x4
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_REQ_PBUSY_BMSK                             0x8
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_REQ_PBUSY_SHFT                             0x3
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_REQ_IDLE_BMSK                              0x4
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_REQ_IDLE_SHFT                              0x2
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_REQ_BUSY_BMSK                              0x2
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_REQ_BUSY_SHFT                              0x1
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_TRPEND_BMSK                                0x1
#define HWIO_APSS_SHARED_GIC_NOC_APSS_AHB_STS_TRPEND_SHFT                                0x0

#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_ADDR                                   (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x0000044c)
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RMSK                                        0x1ff
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_ADDR, HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RMSK)
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_ADDR, m)
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RSP_WAIT_BMSK                               0x100
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RSP_WAIT_SHFT                                 0x8
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RSP_PBUSY_BMSK                               0x80
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RSP_PBUSY_SHFT                                0x7
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RSP_IDLE_BMSK                                0x40
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RSP_IDLE_SHFT                                 0x6
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RSP_BUSY_BMSK                                0x20
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_RSP_BUSY_SHFT                                 0x5
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_REQ_WAIT_BMSK                                0x10
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_REQ_WAIT_SHFT                                 0x4
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_REQ_PBUSY_BMSK                                0x8
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_REQ_PBUSY_SHFT                                0x3
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_REQ_IDLE_BMSK                                 0x4
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_REQ_IDLE_SHFT                                 0x2
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_REQ_BUSY_BMSK                                 0x2
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_REQ_BUSY_SHFT                                 0x1
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_TRPEND_BMSK                                   0x1
#define HWIO_APSS_SHARED_GIC_NOC_GIC_S_STS_TRPEND_SHFT                                   0x0

#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_ADDR                                   (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000450)
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RMSK                                        0x1ff
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_ADDR, HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RMSK)
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_ADDR, m)
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RSP_WAIT_BMSK                               0x100
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RSP_WAIT_SHFT                                 0x8
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RSP_PBUSY_BMSK                               0x80
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RSP_PBUSY_SHFT                                0x7
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RSP_IDLE_BMSK                                0x40
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RSP_IDLE_SHFT                                 0x6
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RSP_BUSY_BMSK                                0x20
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_RSP_BUSY_SHFT                                 0x5
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_REQ_WAIT_BMSK                                0x10
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_REQ_WAIT_SHFT                                 0x4
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_REQ_PBUSY_BMSK                                0x8
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_REQ_PBUSY_SHFT                                0x3
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_REQ_IDLE_BMSK                                 0x4
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_REQ_IDLE_SHFT                                 0x2
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_REQ_BUSY_BMSK                                 0x2
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_REQ_BUSY_SHFT                                 0x1
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_TRPEND_BMSK                                   0x1
#define HWIO_APSS_SHARED_GIC_NOC_OSM_M_STS_TRPEND_SHFT                                   0x0

#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_ADDR                                     (APSS_SHARED_APSS_SHARED_REG_BASE      + 0x00000454)
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RMSK                                          0x1ff
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_IN          \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_AHB_STS_ADDR, HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RMSK)
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_SHARED_GIC_NOC_AHB_STS_ADDR, m)
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RSP_WAIT_BMSK                                 0x100
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RSP_WAIT_SHFT                                   0x8
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RSP_PBUSY_BMSK                                 0x80
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RSP_PBUSY_SHFT                                  0x7
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RSP_IDLE_BMSK                                  0x40
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RSP_IDLE_SHFT                                   0x6
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RSP_BUSY_BMSK                                  0x20
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_RSP_BUSY_SHFT                                   0x5
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_REQ_WAIT_BMSK                                  0x10
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_REQ_WAIT_SHFT                                   0x4
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_REQ_PBUSY_BMSK                                  0x8
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_REQ_PBUSY_SHFT                                  0x3
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_REQ_IDLE_BMSK                                   0x4
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_REQ_IDLE_SHFT                                   0x2
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_REQ_BUSY_BMSK                                   0x2
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_REQ_BUSY_SHFT                                   0x1
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_TRPEND_BMSK                                     0x1
#define HWIO_APSS_SHARED_GIC_NOC_AHB_STS_TRPEND_SHFT                                     0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_WDT_TMR1_APSS_WDT
 *--------------------------------------------------------------------------*/

#define APSS_WDT_TMR1_APSS_WDT_REG_BASE                                    (APSS_HM_BASE      + 0x00180000)

#define HWIO_APSS_WDT_TMR1_WDOG_SECURE_ADDR                                (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000000)
#define HWIO_APSS_WDT_TMR1_WDOG_SECURE_RMSK                                       0x1
#define HWIO_APSS_WDT_TMR1_WDOG_SECURE_IN          \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_SECURE_ADDR, HWIO_APSS_WDT_TMR1_WDOG_SECURE_RMSK)
#define HWIO_APSS_WDT_TMR1_WDOG_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_SECURE_ADDR, m)
#define HWIO_APSS_WDT_TMR1_WDOG_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_SECURE_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_WDT_TMR1_WDOG_SECURE_ADDR,m,v,HWIO_APSS_WDT_TMR1_WDOG_SECURE_IN)
#define HWIO_APSS_WDT_TMR1_WDOG_SECURE_WDT_BMSK                                   0x1
#define HWIO_APSS_WDT_TMR1_WDOG_SECURE_WDT_SHFT                                   0x0

#define HWIO_APSS_WDT_TMR1_WDOG_RESET_ADDR                                 (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000004)
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_RMSK                                        0x1
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_RESET_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_RESET_BMSK                                  0x1
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_RESET_SHFT                                  0x0

#define HWIO_APSS_WDT_TMR1_WDOG_CTL_ADDR                                   (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000008)
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_RMSK                                          0x3
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_CTL_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_UNMASKED_INT_ENABLE_BMSK                      0x2
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_UNMASKED_INT_ENABLE_SHFT                      0x1
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_ENABLE_BMSK                                   0x1
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_ENABLE_SHFT                                   0x0

#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_ADDR                                (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x0000000c)
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_RMSK                                0x801fffff
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_STATUS_ADDR, HWIO_APSS_WDT_TMR1_WDOG_STATUS_RMSK)
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_STATUS_ADDR, m)
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_FROZEN_BMSK                         0x80000000
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_FROZEN_SHFT                               0x1f
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_COUNT_BMSK                            0x1ffffe
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_COUNT_SHFT                                 0x1
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_EXPIRED_STATUS_BMSK                        0x1
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_EXPIRED_STATUS_SHFT                        0x0

#define HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_ADDR                             (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000010)
#define HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_RMSK                                0xfffff
#define HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_ADDR, HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_RMSK)
#define HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_ADDR, m)
#define HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_ADDR,m,v,HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_IN)
#define HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_DATA_BMSK                           0xfffff
#define HWIO_APSS_WDT_TMR1_WDOG_BARK_TIME_DATA_SHFT                               0x0

#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR                             (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000014)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_RMSK                                0xfffff
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR, HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_RMSK)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR, m)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR,m,v,HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_IN)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_DATA_BMSK                           0xfffff
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_DATA_SHFT                               0x0

#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_ADDR                      (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000018)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_RMSK                             0x1
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_IN          \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_ADDR, HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_RMSK)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_ADDR, m)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_SYNC_STATUS_BMSK                 0x1
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_STATUS_SYNC_STATUS_SHFT                 0x0

#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_ADDR                             (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x0000001c)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_RMSK                                    0x1
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_LOAD_BMSK                               0x1
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_LOAD_SHFT                               0x0

#define HWIO_APSS_WDT_TMR1_WDOG_TEST_ADDR                                  (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000020)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_RMSK                                     0xfffff
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_IN          \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_TEST_ADDR, HWIO_APSS_WDT_TMR1_WDOG_TEST_RMSK)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_TEST_ADDR, m)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_TEST_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_WDT_TMR1_WDOG_TEST_ADDR,m,v,HWIO_APSS_WDT_TMR1_WDOG_TEST_IN)
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_VALUE_BMSK                          0xfffff
#define HWIO_APSS_WDT_TMR1_WDOG_TEST_LOAD_VALUE_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_ALIAS_0_APSS_CPU_IPM
 *--------------------------------------------------------------------------*/

#define APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE                                                (APSS_HM_BASE      + 0x00600000)

#define HWIO_APSS_ALIAS_0_APC_SECURE_ADDR                                                 (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000000)
#define HWIO_APSS_ALIAS_0_APC_SECURE_RMSK                                                        0x1
#define HWIO_APSS_ALIAS_0_APC_SECURE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_APC_SECURE_ADDR, HWIO_APSS_ALIAS_0_APC_SECURE_RMSK)
#define HWIO_APSS_ALIAS_0_APC_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_APC_SECURE_ADDR, m)
#define HWIO_APSS_ALIAS_0_APC_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_APC_SECURE_ADDR,v)
#define HWIO_APSS_ALIAS_0_APC_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_APC_SECURE_ADDR,m,v,HWIO_APSS_ALIAS_0_APC_SECURE_IN)
#define HWIO_APSS_ALIAS_0_APC_SECURE_PWR_CTL_SEC_BMSK                                            0x1
#define HWIO_APSS_ALIAS_0_APC_SECURE_PWR_CTL_SEC_SHFT                                            0x0

#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_ADDR                                        (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000008)
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_RMSK                                             0x7ff
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_ADDR, HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_IN)
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_BMSK                                  0x400
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_SHFT                                    0xa
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_HS_CNT_BMSK                                      0x3ff
#define HWIO_APSS_ALIAS_0_CPU_HEAD_SWITCH_CTL_HS_CNT_SHFT                                        0x0

#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_ADDR                                            (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x0000000c)
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_RMSK                                               0x7ffff
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_ADDR, HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_IN)
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_BMSK                         0x70000
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_SHFT                            0x10
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_BMSK                         0xff00
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_SHFT                            0x8
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_BMSK                             0xff
#define HWIO_APSS_ALIAS_0_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_SHFT                              0x0

#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_ADDR                                        (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000010)
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_RMSK                                              0x7f
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_ADDR, HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_BMSK                               0x40
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_SHFT                                0x6
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_BMSK                             0x20
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_SHFT                              0x5
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_BMSK                                  0x10
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_SHFT                                   0x4
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_SLP_NRET_N_BMSK                                    0x8
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_SLP_NRET_N_SHFT                                    0x3
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_SLP_RET_N_BMSK                                     0x4
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_SLP_RET_N_SHFT                                     0x2
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_EN_REST_BMSK                                       0x2
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_EN_REST_SHFT                                       0x1
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_EN_FEW_BMSK                                        0x1
#define HWIO_APSS_ALIAS_0_CPU_PWR_GATE_STATUS_EN_FEW_SHFT                                        0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_ADDR                                     (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000014)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_RMSK                                        0xfffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_IN)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_BMSK                        0xf0000
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_SHFT                           0x10
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_BMSK                           0xffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_SHFT                              0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_ADDR                                    (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000018)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_RMSK                                       0xfffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_IN)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_BMSK                      0xf0000
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_SHFT                         0x10
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_BMSK                         0xffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR                                   (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x0000001c)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK                                     0xffffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_IN)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_EN_BMSK                                  0xffffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_EN_EN_SHFT                                       0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR                                  (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000020)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK                                  0x80ffffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_IN)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_BMSK                       0x80000000
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_SHFT                             0x1f
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_BMSK                        0xffffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_SHFT                             0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_ADDR                                                (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000024)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_RMSK                                                 0x1ff0f07
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_IN)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_START_ADDR_BMSK                                  0x1ff0000
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_START_ADDR_SHFT                                       0x10
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_BMSK                                   0xf00
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_SHFT                                     0x8
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_SLEEP_REQ_BMSK                                         0x4
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_SLEEP_REQ_SHFT                                         0x2
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_BMSK                                        0x2
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_SHFT                                        0x1
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_EN_BMSK                                                0x1
#define HWIO_APSS_ALIAS_0_CPU_SEQ_CTL_SEQ_EN_SHFT                                                0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n)                             (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_RMSK                                     0x1ff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_MAXn                                         3
#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), mask)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),val)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),mask,val,HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_INI(n))
#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_BMSK                   0x1ff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_SHFT                     0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_ADDR                                               (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000038)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_RMSK                                                   0xffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_SEQ_WAIT_EVENT_BMSK                                    0xffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS1_SEQ_WAIT_EVENT_SHFT                                       0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_ADDR                                               (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x0000003c)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_RMSK                                               0xffffffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_SEQ_PWR_CTL_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS2_SEQ_PWR_CTL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_ADDR                                               (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000040)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_RMSK                                                   0x3fff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_BMSK                                  0x3c00
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_SHFT                                     0xa
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_SEQ_BUSY_BMSK                                           0x200
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_SEQ_BUSY_SHFT                                             0x9
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_SEQ_CMD_ADDR_BMSK                                       0x1ff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_STS3_SEQ_CMD_ADDR_SHFT                                         0x0

#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_ADDR                                       (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000044)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_RMSK                                        0xfff63ff
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_ADDR, HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_IN)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_BMSK              0xf000000
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_SHFT                   0x18
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_BMSK                    0xff0000
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_SHFT                        0x10
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_BMSK                  0x4000
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_SHFT                     0xe
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_BMSK                      0x2000
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_SHFT                         0xd
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_BMSK                        0x3f8
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_SHFT                          0x3
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_BMSK                            0x4
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_SHFT                            0x2
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_BMSK                       0x2
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_SHFT                       0x1
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_BMSK                         0x1
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_SHFT                         0x0

#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_ADDR                                    (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000048)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_RMSK                                    0xffffffff
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_ADDR, HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_STATUS_BMSK                             0xffffffff
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_FSM_STATUS_STATUS_SHFT                                    0x0

#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_ADDR                                  (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x0000004c)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_RMSK                                  0xffffffff
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_ADDR, HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_STATUS_BMSK                           0xffffffff
#define HWIO_APSS_ALIAS_0_CPU_PCHANNEL_CTLER_STATUS_STATUS_SHFT                                  0x0

#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_ADDR                                    (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000054)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_RMSK                                         0xfff
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_ADDR, HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_IN)
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_BMSK                 0x800
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_SHFT                   0xb
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_BMSK                 0x400
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_SHFT                   0xa
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_BMSK                0x200
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_SHFT                  0x9
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_BMSK                 0x100
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_SHFT                   0x8
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_BMSK                 0x80
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_SHFT                  0x7
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_BMSK                 0x40
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_SHFT                  0x6
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_BMSK                0x20
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_SHFT                 0x5
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_BMSK                 0x10
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_SHFT                  0x4
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_BMSK                     0x8
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_SHFT                     0x3
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_BMSK                     0x4
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_SHFT                     0x2
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_BMSK                    0x2
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_SHFT                    0x1
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_BMSK                     0x1
#define HWIO_APSS_ALIAS_0_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_SHFT                     0x0

#define HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_ADDR                                        (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000058)
#define HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_ADDR, HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_RMSK)
#define HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_ADDR, m)
#define HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_ADDR,v)
#define HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_ADDR,m,v,HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_IN)
#define HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_BMSK              0xffffffff
#define HWIO_APSS_ALIAS_0_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_SHFT                     0x0

#define HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_ADDR                                            (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x0000005c)
#define HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_RMSK                                            0xffffffff
#define HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_ADDR, HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_RMSK)
#define HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_ADDR, m)
#define HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_VALUE_WHEN_HUNG_BMSK                            0xffffffff
#define HWIO_APSS_ALIAS_0_CORE_HANG_VALUE_VALUE_WHEN_HUNG_SHFT                                   0x0

#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_ADDR                                           (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000060)
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_RMSK                                                0x3fb
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_ADDR, HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_RMSK)
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_ADDR,m,v,HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_IN)
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_BMSK                        0x200
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_SHFT                          0x9
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_PMUEVENT_SEL_BMSK                                   0x1f0
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_PMUEVENT_SEL_SHFT                                     0x4
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_CORE_HANG_RESET_BMSK                                  0x8
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_CORE_HANG_RESET_SHFT                                  0x3
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_CORE_HANG_STATUS_BMSK                                 0x1
#define HWIO_APSS_ALIAS_0_CORE_HANG_CONFIG_CORE_HANG_STATUS_SHFT                                 0x0

#define HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_ADDR                                          (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000064)
#define HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_RMSK                                          0xffffffff
#define HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_ADDR, HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_RMSK)
#define HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_ADDR, m)
#define HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_STATUS_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_0_CORE_HANG_DBG_STS_STATUS_SHFT                                          0x0

#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_ADDR                                            (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x00000068)
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_RMSK                                                 0x3ff
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_ADDR, HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_RMSK)
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_ADDR, m)
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_ADDR,v)
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_ADDR,m,v,HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_IN)
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_EVENT_SEL_BMSK                                       0x3e0
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_EVENT_SEL_SHFT                                         0x5
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_PWR_CTL_SEL_BMSK                                      0x1f
#define HWIO_APSS_ALIAS_0_CPU_HWEVENT_SEL_PWR_CTL_SEL_SHFT                                       0x0

#define HWIO_APSS_ALIAS_0_SPARE_ADDR                                                      (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x000000f0)
#define HWIO_APSS_ALIAS_0_SPARE_RMSK                                                            0xff
#define HWIO_APSS_ALIAS_0_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_SPARE_ADDR, HWIO_APSS_ALIAS_0_SPARE_RMSK)
#define HWIO_APSS_ALIAS_0_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_0_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_0_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_0_SPARE_IN)
#define HWIO_APSS_ALIAS_0_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_APSS_ALIAS_0_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_APSS_ALIAS_0_SPARE_TZ_ADDR                                                   (APSS_ALIAS_0_APSS_CPU_IPM_REG_BASE      + 0x000000f4)
#define HWIO_APSS_ALIAS_0_SPARE_TZ_RMSK                                                         0xff
#define HWIO_APSS_ALIAS_0_SPARE_TZ_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_0_SPARE_TZ_ADDR, HWIO_APSS_ALIAS_0_SPARE_TZ_RMSK)
#define HWIO_APSS_ALIAS_0_SPARE_TZ_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_0_SPARE_TZ_ADDR, m)
#define HWIO_APSS_ALIAS_0_SPARE_TZ_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_0_SPARE_TZ_ADDR,v)
#define HWIO_APSS_ALIAS_0_SPARE_TZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_0_SPARE_TZ_ADDR,m,v,HWIO_APSS_ALIAS_0_SPARE_TZ_IN)
#define HWIO_APSS_ALIAS_0_SPARE_TZ_SPARE_BMSK                                                   0xff
#define HWIO_APSS_ALIAS_0_SPARE_TZ_SPARE_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_ALIAS_1_APSS_CPU_IPM
 *--------------------------------------------------------------------------*/

#define APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE                                                (APSS_HM_BASE      + 0x00610000)

#define HWIO_APSS_ALIAS_1_APC_SECURE_ADDR                                                 (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000000)
#define HWIO_APSS_ALIAS_1_APC_SECURE_RMSK                                                        0x1
#define HWIO_APSS_ALIAS_1_APC_SECURE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_APC_SECURE_ADDR, HWIO_APSS_ALIAS_1_APC_SECURE_RMSK)
#define HWIO_APSS_ALIAS_1_APC_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_APC_SECURE_ADDR, m)
#define HWIO_APSS_ALIAS_1_APC_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_APC_SECURE_ADDR,v)
#define HWIO_APSS_ALIAS_1_APC_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_APC_SECURE_ADDR,m,v,HWIO_APSS_ALIAS_1_APC_SECURE_IN)
#define HWIO_APSS_ALIAS_1_APC_SECURE_PWR_CTL_SEC_BMSK                                            0x1
#define HWIO_APSS_ALIAS_1_APC_SECURE_PWR_CTL_SEC_SHFT                                            0x0

#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_ADDR                                        (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000008)
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_RMSK                                             0x7ff
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_ADDR, HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_IN)
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_BMSK                                  0x400
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_SHFT                                    0xa
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_HS_CNT_BMSK                                      0x3ff
#define HWIO_APSS_ALIAS_1_CPU_HEAD_SWITCH_CTL_HS_CNT_SHFT                                        0x0

#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_ADDR                                            (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x0000000c)
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_RMSK                                               0x7ffff
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_ADDR, HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_IN)
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_BMSK                         0x70000
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_SHFT                            0x10
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_BMSK                         0xff00
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_SHFT                            0x8
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_BMSK                             0xff
#define HWIO_APSS_ALIAS_1_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_SHFT                              0x0

#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_ADDR                                        (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000010)
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_RMSK                                              0x7f
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_ADDR, HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_BMSK                               0x40
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_SHFT                                0x6
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_BMSK                             0x20
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_SHFT                              0x5
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_BMSK                                  0x10
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_SHFT                                   0x4
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_SLP_NRET_N_BMSK                                    0x8
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_SLP_NRET_N_SHFT                                    0x3
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_SLP_RET_N_BMSK                                     0x4
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_SLP_RET_N_SHFT                                     0x2
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_EN_REST_BMSK                                       0x2
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_EN_REST_SHFT                                       0x1
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_EN_FEW_BMSK                                        0x1
#define HWIO_APSS_ALIAS_1_CPU_PWR_GATE_STATUS_EN_FEW_SHFT                                        0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_ADDR                                     (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000014)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_RMSK                                        0xfffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_IN)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_BMSK                        0xf0000
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_SHFT                           0x10
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_BMSK                           0xffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_SHFT                              0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_ADDR                                    (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000018)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_RMSK                                       0xfffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_IN)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_BMSK                      0xf0000
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_SHFT                         0x10
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_BMSK                         0xffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR                                   (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x0000001c)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK                                     0xffffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_IN)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_EN_BMSK                                  0xffffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_EN_EN_SHFT                                       0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR                                  (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000020)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK                                  0x80ffffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_IN)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_BMSK                       0x80000000
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_SHFT                             0x1f
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_BMSK                        0xffffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_SHFT                             0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_ADDR                                                (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000024)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_RMSK                                                 0x1ff0f07
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_IN)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_START_ADDR_BMSK                                  0x1ff0000
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_START_ADDR_SHFT                                       0x10
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_BMSK                                   0xf00
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_SHFT                                     0x8
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_SLEEP_REQ_BMSK                                         0x4
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_SLEEP_REQ_SHFT                                         0x2
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_BMSK                                        0x2
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_SHFT                                        0x1
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_EN_BMSK                                                0x1
#define HWIO_APSS_ALIAS_1_CPU_SEQ_CTL_SEQ_EN_SHFT                                                0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n)                             (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_RMSK                                     0x1ff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_MAXn                                         3
#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), mask)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),val)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),mask,val,HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_INI(n))
#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_BMSK                   0x1ff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_SHFT                     0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_ADDR                                               (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000038)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_RMSK                                                   0xffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_SEQ_WAIT_EVENT_BMSK                                    0xffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS1_SEQ_WAIT_EVENT_SHFT                                       0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_ADDR                                               (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x0000003c)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_RMSK                                               0xffffffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_SEQ_PWR_CTL_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS2_SEQ_PWR_CTL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_ADDR                                               (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000040)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_RMSK                                                   0x3fff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_BMSK                                  0x3c00
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_SHFT                                     0xa
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_SEQ_BUSY_BMSK                                           0x200
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_SEQ_BUSY_SHFT                                             0x9
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_SEQ_CMD_ADDR_BMSK                                       0x1ff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_STS3_SEQ_CMD_ADDR_SHFT                                         0x0

#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_ADDR                                       (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000044)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_RMSK                                        0xfff63ff
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_ADDR, HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_IN)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_BMSK              0xf000000
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_SHFT                   0x18
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_BMSK                    0xff0000
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_SHFT                        0x10
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_BMSK                  0x4000
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_SHFT                     0xe
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_BMSK                      0x2000
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_SHFT                         0xd
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_BMSK                        0x3f8
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_SHFT                          0x3
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_BMSK                            0x4
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_SHFT                            0x2
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_BMSK                       0x2
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_SHFT                       0x1
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_BMSK                         0x1
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_SHFT                         0x0

#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_ADDR                                    (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000048)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_RMSK                                    0xffffffff
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_ADDR, HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_STATUS_BMSK                             0xffffffff
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_FSM_STATUS_STATUS_SHFT                                    0x0

#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_ADDR                                  (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x0000004c)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_RMSK                                  0xffffffff
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_ADDR, HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_STATUS_BMSK                           0xffffffff
#define HWIO_APSS_ALIAS_1_CPU_PCHANNEL_CTLER_STATUS_STATUS_SHFT                                  0x0

#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_ADDR                                    (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000054)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_RMSK                                         0xfff
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_ADDR, HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_IN)
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_BMSK                 0x800
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_SHFT                   0xb
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_BMSK                 0x400
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_SHFT                   0xa
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_BMSK                0x200
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_SHFT                  0x9
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_BMSK                 0x100
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_SHFT                   0x8
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_BMSK                 0x80
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_SHFT                  0x7
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_BMSK                 0x40
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_SHFT                  0x6
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_BMSK                0x20
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_SHFT                 0x5
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_BMSK                 0x10
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_SHFT                  0x4
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_BMSK                     0x8
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_SHFT                     0x3
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_BMSK                     0x4
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_SHFT                     0x2
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_BMSK                    0x2
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_SHFT                    0x1
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_BMSK                     0x1
#define HWIO_APSS_ALIAS_1_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_SHFT                     0x0

#define HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_ADDR                                        (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000058)
#define HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_ADDR, HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_RMSK)
#define HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_ADDR, m)
#define HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_ADDR,v)
#define HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_ADDR,m,v,HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_IN)
#define HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_BMSK              0xffffffff
#define HWIO_APSS_ALIAS_1_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_SHFT                     0x0

#define HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_ADDR                                            (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x0000005c)
#define HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_RMSK                                            0xffffffff
#define HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_ADDR, HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_RMSK)
#define HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_ADDR, m)
#define HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_VALUE_WHEN_HUNG_BMSK                            0xffffffff
#define HWIO_APSS_ALIAS_1_CORE_HANG_VALUE_VALUE_WHEN_HUNG_SHFT                                   0x0

#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_ADDR                                           (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000060)
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_RMSK                                                0x3fb
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_ADDR, HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_RMSK)
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_ADDR,m,v,HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_IN)
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_BMSK                        0x200
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_SHFT                          0x9
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_PMUEVENT_SEL_BMSK                                   0x1f0
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_PMUEVENT_SEL_SHFT                                     0x4
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_CORE_HANG_RESET_BMSK                                  0x8
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_CORE_HANG_RESET_SHFT                                  0x3
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_CORE_HANG_STATUS_BMSK                                 0x1
#define HWIO_APSS_ALIAS_1_CORE_HANG_CONFIG_CORE_HANG_STATUS_SHFT                                 0x0

#define HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_ADDR                                          (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000064)
#define HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_RMSK                                          0xffffffff
#define HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_ADDR, HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_RMSK)
#define HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_ADDR, m)
#define HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_STATUS_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_1_CORE_HANG_DBG_STS_STATUS_SHFT                                          0x0

#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_ADDR                                            (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x00000068)
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_RMSK                                                 0x3ff
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_ADDR, HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_RMSK)
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_ADDR, m)
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_ADDR,v)
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_ADDR,m,v,HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_IN)
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_EVENT_SEL_BMSK                                       0x3e0
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_EVENT_SEL_SHFT                                         0x5
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_PWR_CTL_SEL_BMSK                                      0x1f
#define HWIO_APSS_ALIAS_1_CPU_HWEVENT_SEL_PWR_CTL_SEL_SHFT                                       0x0

#define HWIO_APSS_ALIAS_1_SPARE_ADDR                                                      (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x000000f0)
#define HWIO_APSS_ALIAS_1_SPARE_RMSK                                                            0xff
#define HWIO_APSS_ALIAS_1_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_SPARE_ADDR, HWIO_APSS_ALIAS_1_SPARE_RMSK)
#define HWIO_APSS_ALIAS_1_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_1_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_1_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_1_SPARE_IN)
#define HWIO_APSS_ALIAS_1_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_APSS_ALIAS_1_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_APSS_ALIAS_1_SPARE_TZ_ADDR                                                   (APSS_ALIAS_1_APSS_CPU_IPM_REG_BASE      + 0x000000f4)
#define HWIO_APSS_ALIAS_1_SPARE_TZ_RMSK                                                         0xff
#define HWIO_APSS_ALIAS_1_SPARE_TZ_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_1_SPARE_TZ_ADDR, HWIO_APSS_ALIAS_1_SPARE_TZ_RMSK)
#define HWIO_APSS_ALIAS_1_SPARE_TZ_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_1_SPARE_TZ_ADDR, m)
#define HWIO_APSS_ALIAS_1_SPARE_TZ_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_1_SPARE_TZ_ADDR,v)
#define HWIO_APSS_ALIAS_1_SPARE_TZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_1_SPARE_TZ_ADDR,m,v,HWIO_APSS_ALIAS_1_SPARE_TZ_IN)
#define HWIO_APSS_ALIAS_1_SPARE_TZ_SPARE_BMSK                                                   0xff
#define HWIO_APSS_ALIAS_1_SPARE_TZ_SPARE_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_ALIAS_2_APSS_CPU_IPM
 *--------------------------------------------------------------------------*/

#define APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE                                                (APSS_HM_BASE      + 0x00620000)

#define HWIO_APSS_ALIAS_2_APC_SECURE_ADDR                                                 (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000000)
#define HWIO_APSS_ALIAS_2_APC_SECURE_RMSK                                                        0x1
#define HWIO_APSS_ALIAS_2_APC_SECURE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_APC_SECURE_ADDR, HWIO_APSS_ALIAS_2_APC_SECURE_RMSK)
#define HWIO_APSS_ALIAS_2_APC_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_APC_SECURE_ADDR, m)
#define HWIO_APSS_ALIAS_2_APC_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_APC_SECURE_ADDR,v)
#define HWIO_APSS_ALIAS_2_APC_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_APC_SECURE_ADDR,m,v,HWIO_APSS_ALIAS_2_APC_SECURE_IN)
#define HWIO_APSS_ALIAS_2_APC_SECURE_PWR_CTL_SEC_BMSK                                            0x1
#define HWIO_APSS_ALIAS_2_APC_SECURE_PWR_CTL_SEC_SHFT                                            0x0

#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_ADDR                                        (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000008)
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_RMSK                                             0x7ff
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_ADDR, HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_IN)
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_BMSK                                  0x400
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_SHFT                                    0xa
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_HS_CNT_BMSK                                      0x3ff
#define HWIO_APSS_ALIAS_2_CPU_HEAD_SWITCH_CTL_HS_CNT_SHFT                                        0x0

#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_ADDR                                            (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x0000000c)
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_RMSK                                               0x7ffff
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_ADDR, HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_IN)
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_BMSK                         0x70000
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_SHFT                            0x10
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_BMSK                         0xff00
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_SHFT                            0x8
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_BMSK                             0xff
#define HWIO_APSS_ALIAS_2_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_SHFT                              0x0

#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_ADDR                                        (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000010)
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_RMSK                                              0x7f
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_ADDR, HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_BMSK                               0x40
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_SHFT                                0x6
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_BMSK                             0x20
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_SHFT                              0x5
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_BMSK                                  0x10
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_SHFT                                   0x4
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_SLP_NRET_N_BMSK                                    0x8
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_SLP_NRET_N_SHFT                                    0x3
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_SLP_RET_N_BMSK                                     0x4
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_SLP_RET_N_SHFT                                     0x2
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_EN_REST_BMSK                                       0x2
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_EN_REST_SHFT                                       0x1
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_EN_FEW_BMSK                                        0x1
#define HWIO_APSS_ALIAS_2_CPU_PWR_GATE_STATUS_EN_FEW_SHFT                                        0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_ADDR                                     (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000014)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_RMSK                                        0xfffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_IN)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_BMSK                        0xf0000
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_SHFT                           0x10
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_BMSK                           0xffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_SHFT                              0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_ADDR                                    (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000018)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_RMSK                                       0xfffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_IN)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_BMSK                      0xf0000
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_SHFT                         0x10
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_BMSK                         0xffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR                                   (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x0000001c)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK                                     0xffffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_IN)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_EN_BMSK                                  0xffffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_EN_EN_SHFT                                       0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR                                  (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000020)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK                                  0x80ffffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_IN)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_BMSK                       0x80000000
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_SHFT                             0x1f
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_BMSK                        0xffffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_SHFT                             0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_ADDR                                                (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000024)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_RMSK                                                 0x1ff0f07
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_IN)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_START_ADDR_BMSK                                  0x1ff0000
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_START_ADDR_SHFT                                       0x10
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_BMSK                                   0xf00
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_SHFT                                     0x8
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_SLEEP_REQ_BMSK                                         0x4
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_SLEEP_REQ_SHFT                                         0x2
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_BMSK                                        0x2
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_SHFT                                        0x1
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_EN_BMSK                                                0x1
#define HWIO_APSS_ALIAS_2_CPU_SEQ_CTL_SEQ_EN_SHFT                                                0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n)                             (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_RMSK                                     0x1ff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_MAXn                                         3
#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), mask)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),val)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),mask,val,HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_INI(n))
#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_BMSK                   0x1ff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_SHFT                     0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_ADDR                                               (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000038)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_RMSK                                                   0xffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_SEQ_WAIT_EVENT_BMSK                                    0xffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS1_SEQ_WAIT_EVENT_SHFT                                       0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_ADDR                                               (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x0000003c)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_RMSK                                               0xffffffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_SEQ_PWR_CTL_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS2_SEQ_PWR_CTL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_ADDR                                               (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000040)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_RMSK                                                   0x3fff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_BMSK                                  0x3c00
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_SHFT                                     0xa
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_SEQ_BUSY_BMSK                                           0x200
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_SEQ_BUSY_SHFT                                             0x9
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_SEQ_CMD_ADDR_BMSK                                       0x1ff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_STS3_SEQ_CMD_ADDR_SHFT                                         0x0

#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_ADDR                                       (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000044)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_RMSK                                        0xfff63ff
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_ADDR, HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_IN)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_BMSK              0xf000000
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_SHFT                   0x18
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_BMSK                    0xff0000
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_SHFT                        0x10
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_BMSK                  0x4000
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_SHFT                     0xe
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_BMSK                      0x2000
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_SHFT                         0xd
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_BMSK                        0x3f8
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_SHFT                          0x3
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_BMSK                            0x4
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_SHFT                            0x2
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_BMSK                       0x2
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_SHFT                       0x1
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_BMSK                         0x1
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_SHFT                         0x0

#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_ADDR                                    (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000048)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_RMSK                                    0xffffffff
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_ADDR, HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_STATUS_BMSK                             0xffffffff
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_FSM_STATUS_STATUS_SHFT                                    0x0

#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_ADDR                                  (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x0000004c)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_RMSK                                  0xffffffff
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_ADDR, HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_STATUS_BMSK                           0xffffffff
#define HWIO_APSS_ALIAS_2_CPU_PCHANNEL_CTLER_STATUS_STATUS_SHFT                                  0x0

#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_ADDR                                    (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000054)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_RMSK                                         0xfff
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_ADDR, HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_IN)
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_BMSK                 0x800
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_SHFT                   0xb
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_BMSK                 0x400
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_SHFT                   0xa
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_BMSK                0x200
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_SHFT                  0x9
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_BMSK                 0x100
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_SHFT                   0x8
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_BMSK                 0x80
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_SHFT                  0x7
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_BMSK                 0x40
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_SHFT                  0x6
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_BMSK                0x20
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_SHFT                 0x5
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_BMSK                 0x10
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_SHFT                  0x4
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_BMSK                     0x8
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_SHFT                     0x3
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_BMSK                     0x4
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_SHFT                     0x2
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_BMSK                    0x2
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_SHFT                    0x1
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_BMSK                     0x1
#define HWIO_APSS_ALIAS_2_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_SHFT                     0x0

#define HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_ADDR                                        (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000058)
#define HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_ADDR, HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_RMSK)
#define HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_ADDR, m)
#define HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_ADDR,v)
#define HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_ADDR,m,v,HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_IN)
#define HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_BMSK              0xffffffff
#define HWIO_APSS_ALIAS_2_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_SHFT                     0x0

#define HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_ADDR                                            (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x0000005c)
#define HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_RMSK                                            0xffffffff
#define HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_ADDR, HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_RMSK)
#define HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_ADDR, m)
#define HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_VALUE_WHEN_HUNG_BMSK                            0xffffffff
#define HWIO_APSS_ALIAS_2_CORE_HANG_VALUE_VALUE_WHEN_HUNG_SHFT                                   0x0

#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_ADDR                                           (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000060)
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_RMSK                                                0x3fb
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_ADDR, HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_RMSK)
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_ADDR,m,v,HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_IN)
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_BMSK                        0x200
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_SHFT                          0x9
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_PMUEVENT_SEL_BMSK                                   0x1f0
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_PMUEVENT_SEL_SHFT                                     0x4
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_CORE_HANG_RESET_BMSK                                  0x8
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_CORE_HANG_RESET_SHFT                                  0x3
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_CORE_HANG_STATUS_BMSK                                 0x1
#define HWIO_APSS_ALIAS_2_CORE_HANG_CONFIG_CORE_HANG_STATUS_SHFT                                 0x0

#define HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_ADDR                                          (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000064)
#define HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_RMSK                                          0xffffffff
#define HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_ADDR, HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_RMSK)
#define HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_ADDR, m)
#define HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_STATUS_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_2_CORE_HANG_DBG_STS_STATUS_SHFT                                          0x0

#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_ADDR                                            (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x00000068)
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_RMSK                                                 0x3ff
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_ADDR, HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_RMSK)
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_ADDR, m)
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_ADDR,v)
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_ADDR,m,v,HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_IN)
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_EVENT_SEL_BMSK                                       0x3e0
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_EVENT_SEL_SHFT                                         0x5
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_PWR_CTL_SEL_BMSK                                      0x1f
#define HWIO_APSS_ALIAS_2_CPU_HWEVENT_SEL_PWR_CTL_SEL_SHFT                                       0x0

#define HWIO_APSS_ALIAS_2_SPARE_ADDR                                                      (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x000000f0)
#define HWIO_APSS_ALIAS_2_SPARE_RMSK                                                            0xff
#define HWIO_APSS_ALIAS_2_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_SPARE_ADDR, HWIO_APSS_ALIAS_2_SPARE_RMSK)
#define HWIO_APSS_ALIAS_2_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_2_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_2_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_2_SPARE_IN)
#define HWIO_APSS_ALIAS_2_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_APSS_ALIAS_2_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_APSS_ALIAS_2_SPARE_TZ_ADDR                                                   (APSS_ALIAS_2_APSS_CPU_IPM_REG_BASE      + 0x000000f4)
#define HWIO_APSS_ALIAS_2_SPARE_TZ_RMSK                                                         0xff
#define HWIO_APSS_ALIAS_2_SPARE_TZ_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_2_SPARE_TZ_ADDR, HWIO_APSS_ALIAS_2_SPARE_TZ_RMSK)
#define HWIO_APSS_ALIAS_2_SPARE_TZ_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_2_SPARE_TZ_ADDR, m)
#define HWIO_APSS_ALIAS_2_SPARE_TZ_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_2_SPARE_TZ_ADDR,v)
#define HWIO_APSS_ALIAS_2_SPARE_TZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_2_SPARE_TZ_ADDR,m,v,HWIO_APSS_ALIAS_2_SPARE_TZ_IN)
#define HWIO_APSS_ALIAS_2_SPARE_TZ_SPARE_BMSK                                                   0xff
#define HWIO_APSS_ALIAS_2_SPARE_TZ_SPARE_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_ALIAS_3_APSS_CPU_IPM
 *--------------------------------------------------------------------------*/

#define APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE                                                (APSS_HM_BASE      + 0x00630000)

#define HWIO_APSS_ALIAS_3_APC_SECURE_ADDR                                                 (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000000)
#define HWIO_APSS_ALIAS_3_APC_SECURE_RMSK                                                        0x1
#define HWIO_APSS_ALIAS_3_APC_SECURE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_APC_SECURE_ADDR, HWIO_APSS_ALIAS_3_APC_SECURE_RMSK)
#define HWIO_APSS_ALIAS_3_APC_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_APC_SECURE_ADDR, m)
#define HWIO_APSS_ALIAS_3_APC_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_APC_SECURE_ADDR,v)
#define HWIO_APSS_ALIAS_3_APC_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_APC_SECURE_ADDR,m,v,HWIO_APSS_ALIAS_3_APC_SECURE_IN)
#define HWIO_APSS_ALIAS_3_APC_SECURE_PWR_CTL_SEC_BMSK                                            0x1
#define HWIO_APSS_ALIAS_3_APC_SECURE_PWR_CTL_SEC_SHFT                                            0x0

#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_ADDR                                        (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000008)
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_RMSK                                             0x7ff
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_ADDR, HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_IN)
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_BMSK                                  0x400
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_SHFT                                    0xa
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_HS_CNT_BMSK                                      0x3ff
#define HWIO_APSS_ALIAS_3_CPU_HEAD_SWITCH_CTL_HS_CNT_SHFT                                        0x0

#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_ADDR                                            (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x0000000c)
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_RMSK                                               0x7ffff
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_ADDR, HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_IN)
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_BMSK                         0x70000
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_SHFT                            0x10
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_BMSK                         0xff00
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_SHFT                            0x8
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_BMSK                             0xff
#define HWIO_APSS_ALIAS_3_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_SHFT                              0x0

#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_ADDR                                        (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000010)
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_RMSK                                              0x7f
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_ADDR, HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_BMSK                               0x40
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_SHFT                                0x6
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_BMSK                             0x20
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_SHFT                              0x5
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_BMSK                                  0x10
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_SHFT                                   0x4
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_SLP_NRET_N_BMSK                                    0x8
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_SLP_NRET_N_SHFT                                    0x3
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_SLP_RET_N_BMSK                                     0x4
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_SLP_RET_N_SHFT                                     0x2
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_EN_REST_BMSK                                       0x2
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_EN_REST_SHFT                                       0x1
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_EN_FEW_BMSK                                        0x1
#define HWIO_APSS_ALIAS_3_CPU_PWR_GATE_STATUS_EN_FEW_SHFT                                        0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_ADDR                                     (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000014)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_RMSK                                        0xfffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_IN)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_BMSK                        0xf0000
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_SHFT                           0x10
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_BMSK                           0xffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_SHFT                              0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_ADDR                                    (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000018)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_RMSK                                       0xfffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_IN)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_BMSK                      0xf0000
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_SHFT                         0x10
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_BMSK                         0xffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR                                   (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x0000001c)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK                                     0xffffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_IN)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_EN_BMSK                                  0xffffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_EN_EN_SHFT                                       0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR                                  (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000020)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK                                  0x80ffffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_IN)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_BMSK                       0x80000000
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_SHFT                             0x1f
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_BMSK                        0xffffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_SHFT                             0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_ADDR                                                (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000024)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_RMSK                                                 0x1ff0f07
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_IN)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_START_ADDR_BMSK                                  0x1ff0000
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_START_ADDR_SHFT                                       0x10
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_BMSK                                   0xf00
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_SHFT                                     0x8
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_SLEEP_REQ_BMSK                                         0x4
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_SLEEP_REQ_SHFT                                         0x2
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_BMSK                                        0x2
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_SHFT                                        0x1
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_EN_BMSK                                                0x1
#define HWIO_APSS_ALIAS_3_CPU_SEQ_CTL_SEQ_EN_SHFT                                                0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n)                             (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_RMSK                                     0x1ff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_MAXn                                         3
#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), mask)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),val)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),mask,val,HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_INI(n))
#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_BMSK                   0x1ff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_SHFT                     0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_ADDR                                               (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000038)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_RMSK                                                   0xffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_SEQ_WAIT_EVENT_BMSK                                    0xffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS1_SEQ_WAIT_EVENT_SHFT                                       0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_ADDR                                               (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x0000003c)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_RMSK                                               0xffffffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_SEQ_PWR_CTL_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS2_SEQ_PWR_CTL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_ADDR                                               (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000040)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_RMSK                                                   0x3fff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_BMSK                                  0x3c00
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_SHFT                                     0xa
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_SEQ_BUSY_BMSK                                           0x200
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_SEQ_BUSY_SHFT                                             0x9
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_SEQ_CMD_ADDR_BMSK                                       0x1ff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_STS3_SEQ_CMD_ADDR_SHFT                                         0x0

#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_ADDR                                       (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000044)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_RMSK                                        0xfff63ff
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_ADDR, HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_IN)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_BMSK              0xf000000
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_SHFT                   0x18
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_BMSK                    0xff0000
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_SHFT                        0x10
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_BMSK                  0x4000
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_SHFT                     0xe
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_BMSK                      0x2000
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_SHFT                         0xd
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_BMSK                        0x3f8
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_SHFT                          0x3
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_BMSK                            0x4
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_SHFT                            0x2
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_BMSK                       0x2
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_SHFT                       0x1
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_BMSK                         0x1
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_SHFT                         0x0

#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_ADDR                                    (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000048)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_RMSK                                    0xffffffff
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_ADDR, HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_STATUS_BMSK                             0xffffffff
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_FSM_STATUS_STATUS_SHFT                                    0x0

#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_ADDR                                  (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x0000004c)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_RMSK                                  0xffffffff
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_ADDR, HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_STATUS_BMSK                           0xffffffff
#define HWIO_APSS_ALIAS_3_CPU_PCHANNEL_CTLER_STATUS_STATUS_SHFT                                  0x0

#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_ADDR                                    (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000054)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_RMSK                                         0xfff
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_ADDR, HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_IN)
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_BMSK                 0x800
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_SHFT                   0xb
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_BMSK                 0x400
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_SHFT                   0xa
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_BMSK                0x200
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_SHFT                  0x9
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_BMSK                 0x100
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_SHFT                   0x8
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_BMSK                 0x80
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_SHFT                  0x7
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_BMSK                 0x40
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_SHFT                  0x6
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_BMSK                0x20
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_SHFT                 0x5
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_BMSK                 0x10
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_SHFT                  0x4
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_BMSK                     0x8
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_SHFT                     0x3
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_BMSK                     0x4
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_SHFT                     0x2
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_BMSK                    0x2
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_SHFT                    0x1
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_BMSK                     0x1
#define HWIO_APSS_ALIAS_3_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_SHFT                     0x0

#define HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_ADDR                                        (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000058)
#define HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_ADDR, HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_RMSK)
#define HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_ADDR, m)
#define HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_ADDR,v)
#define HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_ADDR,m,v,HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_IN)
#define HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_BMSK              0xffffffff
#define HWIO_APSS_ALIAS_3_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_SHFT                     0x0

#define HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_ADDR                                            (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x0000005c)
#define HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_RMSK                                            0xffffffff
#define HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_ADDR, HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_RMSK)
#define HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_ADDR, m)
#define HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_VALUE_WHEN_HUNG_BMSK                            0xffffffff
#define HWIO_APSS_ALIAS_3_CORE_HANG_VALUE_VALUE_WHEN_HUNG_SHFT                                   0x0

#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_ADDR                                           (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000060)
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_RMSK                                                0x3fb
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_ADDR, HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_RMSK)
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_ADDR,m,v,HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_IN)
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_BMSK                        0x200
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_SHFT                          0x9
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_PMUEVENT_SEL_BMSK                                   0x1f0
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_PMUEVENT_SEL_SHFT                                     0x4
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_CORE_HANG_RESET_BMSK                                  0x8
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_CORE_HANG_RESET_SHFT                                  0x3
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_CORE_HANG_STATUS_BMSK                                 0x1
#define HWIO_APSS_ALIAS_3_CORE_HANG_CONFIG_CORE_HANG_STATUS_SHFT                                 0x0

#define HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_ADDR                                          (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000064)
#define HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_RMSK                                          0xffffffff
#define HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_ADDR, HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_RMSK)
#define HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_ADDR, m)
#define HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_STATUS_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_3_CORE_HANG_DBG_STS_STATUS_SHFT                                          0x0

#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_ADDR                                            (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x00000068)
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_RMSK                                                 0x3ff
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_ADDR, HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_RMSK)
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_ADDR, m)
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_ADDR,v)
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_ADDR,m,v,HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_IN)
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_EVENT_SEL_BMSK                                       0x3e0
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_EVENT_SEL_SHFT                                         0x5
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_PWR_CTL_SEL_BMSK                                      0x1f
#define HWIO_APSS_ALIAS_3_CPU_HWEVENT_SEL_PWR_CTL_SEL_SHFT                                       0x0

#define HWIO_APSS_ALIAS_3_SPARE_ADDR                                                      (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x000000f0)
#define HWIO_APSS_ALIAS_3_SPARE_RMSK                                                            0xff
#define HWIO_APSS_ALIAS_3_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_SPARE_ADDR, HWIO_APSS_ALIAS_3_SPARE_RMSK)
#define HWIO_APSS_ALIAS_3_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_3_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_3_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_3_SPARE_IN)
#define HWIO_APSS_ALIAS_3_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_APSS_ALIAS_3_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_APSS_ALIAS_3_SPARE_TZ_ADDR                                                   (APSS_ALIAS_3_APSS_CPU_IPM_REG_BASE      + 0x000000f4)
#define HWIO_APSS_ALIAS_3_SPARE_TZ_RMSK                                                         0xff
#define HWIO_APSS_ALIAS_3_SPARE_TZ_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_3_SPARE_TZ_ADDR, HWIO_APSS_ALIAS_3_SPARE_TZ_RMSK)
#define HWIO_APSS_ALIAS_3_SPARE_TZ_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_3_SPARE_TZ_ADDR, m)
#define HWIO_APSS_ALIAS_3_SPARE_TZ_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_3_SPARE_TZ_ADDR,v)
#define HWIO_APSS_ALIAS_3_SPARE_TZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_3_SPARE_TZ_ADDR,m,v,HWIO_APSS_ALIAS_3_SPARE_TZ_IN)
#define HWIO_APSS_ALIAS_3_SPARE_TZ_SPARE_BMSK                                                   0xff
#define HWIO_APSS_ALIAS_3_SPARE_TZ_SPARE_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_ALIAS_4_APSS_CPU_IPM
 *--------------------------------------------------------------------------*/

#define APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE                                                (APSS_HM_BASE      + 0x00640000)

#define HWIO_APSS_ALIAS_4_APC_SECURE_ADDR                                                 (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000000)
#define HWIO_APSS_ALIAS_4_APC_SECURE_RMSK                                                        0x1
#define HWIO_APSS_ALIAS_4_APC_SECURE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_APC_SECURE_ADDR, HWIO_APSS_ALIAS_4_APC_SECURE_RMSK)
#define HWIO_APSS_ALIAS_4_APC_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_APC_SECURE_ADDR, m)
#define HWIO_APSS_ALIAS_4_APC_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_APC_SECURE_ADDR,v)
#define HWIO_APSS_ALIAS_4_APC_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_APC_SECURE_ADDR,m,v,HWIO_APSS_ALIAS_4_APC_SECURE_IN)
#define HWIO_APSS_ALIAS_4_APC_SECURE_PWR_CTL_SEC_BMSK                                            0x1
#define HWIO_APSS_ALIAS_4_APC_SECURE_PWR_CTL_SEC_SHFT                                            0x0

#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_ADDR                                        (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000008)
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_RMSK                                             0x7ff
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_ADDR, HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_IN)
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_BMSK                                  0x400
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_SHFT                                    0xa
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_HS_CNT_BMSK                                      0x3ff
#define HWIO_APSS_ALIAS_4_CPU_HEAD_SWITCH_CTL_HS_CNT_SHFT                                        0x0

#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_ADDR                                            (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x0000000c)
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_RMSK                                               0x7ffff
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_ADDR, HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_IN)
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_BMSK                         0x70000
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_SHFT                            0x10
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_BMSK                         0xff00
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_SHFT                            0x8
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_BMSK                             0xff
#define HWIO_APSS_ALIAS_4_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_SHFT                              0x0

#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_ADDR                                        (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000010)
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_RMSK                                              0x7f
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_ADDR, HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_BMSK                               0x40
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_SHFT                                0x6
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_BMSK                             0x20
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_SHFT                              0x5
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_BMSK                                  0x10
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_SHFT                                   0x4
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_SLP_NRET_N_BMSK                                    0x8
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_SLP_NRET_N_SHFT                                    0x3
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_SLP_RET_N_BMSK                                     0x4
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_SLP_RET_N_SHFT                                     0x2
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_EN_REST_BMSK                                       0x2
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_EN_REST_SHFT                                       0x1
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_EN_FEW_BMSK                                        0x1
#define HWIO_APSS_ALIAS_4_CPU_PWR_GATE_STATUS_EN_FEW_SHFT                                        0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_ADDR                                     (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000014)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_RMSK                                        0xfffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_IN)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_BMSK                        0xf0000
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_SHFT                           0x10
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_BMSK                           0xffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_SHFT                              0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_ADDR                                    (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000018)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_RMSK                                       0xfffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_IN)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_BMSK                      0xf0000
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_SHFT                         0x10
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_BMSK                         0xffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR                                   (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x0000001c)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK                                     0xffffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_IN)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_EN_BMSK                                  0xffffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_EN_EN_SHFT                                       0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR                                  (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000020)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK                                  0x80ffffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_IN)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_BMSK                       0x80000000
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_SHFT                             0x1f
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_BMSK                        0xffffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_SHFT                             0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_ADDR                                                (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000024)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_RMSK                                                 0x1ff0f07
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_IN)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_START_ADDR_BMSK                                  0x1ff0000
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_START_ADDR_SHFT                                       0x10
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_BMSK                                   0xf00
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_SHFT                                     0x8
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_SLEEP_REQ_BMSK                                         0x4
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_SLEEP_REQ_SHFT                                         0x2
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_BMSK                                        0x2
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_SHFT                                        0x1
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_EN_BMSK                                                0x1
#define HWIO_APSS_ALIAS_4_CPU_SEQ_CTL_SEQ_EN_SHFT                                                0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n)                             (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_RMSK                                     0x1ff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_MAXn                                         3
#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), mask)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),val)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),mask,val,HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_INI(n))
#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_BMSK                   0x1ff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_SHFT                     0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_ADDR                                               (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000038)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_RMSK                                                   0xffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_SEQ_WAIT_EVENT_BMSK                                    0xffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS1_SEQ_WAIT_EVENT_SHFT                                       0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_ADDR                                               (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x0000003c)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_RMSK                                               0xffffffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_SEQ_PWR_CTL_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS2_SEQ_PWR_CTL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_ADDR                                               (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000040)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_RMSK                                                   0x3fff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_BMSK                                  0x3c00
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_SHFT                                     0xa
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_SEQ_BUSY_BMSK                                           0x200
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_SEQ_BUSY_SHFT                                             0x9
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_SEQ_CMD_ADDR_BMSK                                       0x1ff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_STS3_SEQ_CMD_ADDR_SHFT                                         0x0

#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_ADDR                                       (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000044)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_RMSK                                        0xfff63ff
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_ADDR, HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_IN)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_BMSK              0xf000000
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_SHFT                   0x18
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_BMSK                    0xff0000
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_SHFT                        0x10
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_BMSK                  0x4000
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_SHFT                     0xe
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_BMSK                      0x2000
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_SHFT                         0xd
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_BMSK                        0x3f8
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_SHFT                          0x3
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_BMSK                            0x4
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_SHFT                            0x2
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_BMSK                       0x2
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_SHFT                       0x1
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_BMSK                         0x1
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_SHFT                         0x0

#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_ADDR                                    (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000048)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_RMSK                                    0xffffffff
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_ADDR, HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_STATUS_BMSK                             0xffffffff
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_FSM_STATUS_STATUS_SHFT                                    0x0

#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_ADDR                                  (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x0000004c)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_RMSK                                  0xffffffff
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_ADDR, HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_STATUS_BMSK                           0xffffffff
#define HWIO_APSS_ALIAS_4_CPU_PCHANNEL_CTLER_STATUS_STATUS_SHFT                                  0x0

#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_ADDR                                    (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000054)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_RMSK                                         0xfff
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_ADDR, HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_IN)
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_BMSK                 0x800
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_SHFT                   0xb
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_BMSK                 0x400
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_SHFT                   0xa
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_BMSK                0x200
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_SHFT                  0x9
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_BMSK                 0x100
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_SHFT                   0x8
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_BMSK                 0x80
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_SHFT                  0x7
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_BMSK                 0x40
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_SHFT                  0x6
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_BMSK                0x20
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_SHFT                 0x5
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_BMSK                 0x10
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_SHFT                  0x4
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_BMSK                     0x8
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_SHFT                     0x3
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_BMSK                     0x4
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_SHFT                     0x2
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_BMSK                    0x2
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_SHFT                    0x1
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_BMSK                     0x1
#define HWIO_APSS_ALIAS_4_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_SHFT                     0x0

#define HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_ADDR                                        (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000058)
#define HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_ADDR, HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_RMSK)
#define HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_ADDR, m)
#define HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_ADDR,v)
#define HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_ADDR,m,v,HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_IN)
#define HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_BMSK              0xffffffff
#define HWIO_APSS_ALIAS_4_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_SHFT                     0x0

#define HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_ADDR                                            (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x0000005c)
#define HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_RMSK                                            0xffffffff
#define HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_ADDR, HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_RMSK)
#define HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_ADDR, m)
#define HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_VALUE_WHEN_HUNG_BMSK                            0xffffffff
#define HWIO_APSS_ALIAS_4_CORE_HANG_VALUE_VALUE_WHEN_HUNG_SHFT                                   0x0

#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_ADDR                                           (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000060)
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_RMSK                                                0x3fb
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_ADDR, HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_RMSK)
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_ADDR,m,v,HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_IN)
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_BMSK                        0x200
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_SHFT                          0x9
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_PMUEVENT_SEL_BMSK                                   0x1f0
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_PMUEVENT_SEL_SHFT                                     0x4
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_CORE_HANG_RESET_BMSK                                  0x8
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_CORE_HANG_RESET_SHFT                                  0x3
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_CORE_HANG_STATUS_BMSK                                 0x1
#define HWIO_APSS_ALIAS_4_CORE_HANG_CONFIG_CORE_HANG_STATUS_SHFT                                 0x0

#define HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_ADDR                                          (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000064)
#define HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_RMSK                                          0xffffffff
#define HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_ADDR, HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_RMSK)
#define HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_ADDR, m)
#define HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_STATUS_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_4_CORE_HANG_DBG_STS_STATUS_SHFT                                          0x0

#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_ADDR                                            (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00000068)
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_RMSK                                                 0x3ff
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_ADDR, HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_RMSK)
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_ADDR, m)
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_ADDR,v)
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_ADDR,m,v,HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_IN)
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_EVENT_SEL_BMSK                                       0x3e0
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_EVENT_SEL_SHFT                                         0x5
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_PWR_CTL_SEL_BMSK                                      0x1f
#define HWIO_APSS_ALIAS_4_CPU_HWEVENT_SEL_PWR_CTL_SEL_SHFT                                       0x0

#define HWIO_APSS_ALIAS_4_SPARE_ADDR                                                      (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x000000f0)
#define HWIO_APSS_ALIAS_4_SPARE_RMSK                                                            0xff
#define HWIO_APSS_ALIAS_4_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_SPARE_ADDR, HWIO_APSS_ALIAS_4_SPARE_RMSK)
#define HWIO_APSS_ALIAS_4_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_4_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_4_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_4_SPARE_IN)
#define HWIO_APSS_ALIAS_4_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_APSS_ALIAS_4_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_APSS_ALIAS_4_SPARE_TZ_ADDR                                                   (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x000000f4)
#define HWIO_APSS_ALIAS_4_SPARE_TZ_RMSK                                                         0xff
#define HWIO_APSS_ALIAS_4_SPARE_TZ_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_SPARE_TZ_ADDR, HWIO_APSS_ALIAS_4_SPARE_TZ_RMSK)
#define HWIO_APSS_ALIAS_4_SPARE_TZ_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_SPARE_TZ_ADDR, m)
#define HWIO_APSS_ALIAS_4_SPARE_TZ_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_SPARE_TZ_ADDR,v)
#define HWIO_APSS_ALIAS_4_SPARE_TZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_SPARE_TZ_ADDR,m,v,HWIO_APSS_ALIAS_4_SPARE_TZ_IN)
#define HWIO_APSS_ALIAS_4_SPARE_TZ_SPARE_BMSK                                                   0xff
#define HWIO_APSS_ALIAS_4_SPARE_TZ_SPARE_SHFT                                                    0x0

#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_ADDR                                                 (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00008000)
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_RMSK                                                       0x7b
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_SEQCTL_BHS_ADDR, HWIO_APSS_ALIAS_4_SEQCTL_BHS_RMSK)
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_SEQCTL_BHS_ADDR, m)
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_SEQCTL_BHS_ADDR,v)
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_SEQCTL_BHS_ADDR,m,v,HWIO_APSS_ALIAS_4_SEQCTL_BHS_IN)
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_ENR_BRANCH_DIS_BMSK                                        0x78
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_ENR_BRANCH_DIS_SHFT                                         0x3
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_BHS_REST_DLY_SEL_BMSK                                       0x2
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_BHS_REST_DLY_SEL_SHFT                                       0x1
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_BHS_FEW_DLY_SEL_BMSK                                        0x1
#define HWIO_APSS_ALIAS_4_SEQCTL_BHS_BHS_FEW_DLY_SEL_SHFT                                        0x0

#define HWIO_APSS_ALIAS_4_BHS_STATUS_ADDR                                                 (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00008004)
#define HWIO_APSS_ALIAS_4_BHS_STATUS_RMSK                                                        0x3
#define HWIO_APSS_ALIAS_4_BHS_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_BHS_STATUS_ADDR, HWIO_APSS_ALIAS_4_BHS_STATUS_RMSK)
#define HWIO_APSS_ALIAS_4_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_BHS_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_4_BHS_STATUS_BHS_EN_REST_ACK_BMSK                                        0x2
#define HWIO_APSS_ALIAS_4_BHS_STATUS_BHS_EN_REST_ACK_SHFT                                        0x1
#define HWIO_APSS_ALIAS_4_BHS_STATUS_BHS_EN_FEW_ACK_BMSK                                         0x1
#define HWIO_APSS_ALIAS_4_BHS_STATUS_BHS_EN_FEW_ACK_SHFT                                         0x0

#define HWIO_APSS_ALIAS_4_FFWD_CNTL_ADDR                                                  (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00008008)
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_RMSK                                                      0xffff
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_FFWD_CNTL_ADDR, HWIO_APSS_ALIAS_4_FFWD_CNTL_RMSK)
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_FFWD_CNTL_ADDR, m)
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_FFWD_CNTL_ADDR,v)
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_FFWD_CNTL_ADDR,m,v,HWIO_APSS_ALIAS_4_FFWD_CNTL_IN)
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_FFWD_CNTL_EN_BMSK                                         0x8000
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_FFWD_CNTL_EN_SHFT                                            0xf
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_FFWD_CNTL_DELAY_BMSK                                      0x7f80
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_FFWD_CNTL_DELAY_SHFT                                         0x7
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_FFWD_CNTL_CLK_DIVIDE_BY_BMSK                                0x60
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_FFWD_CNTL_CLK_DIVIDE_BY_SHFT                                 0x5
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_FFWD_CNTL_OFFSET_HYST_CONFIG_BMSK                           0x1f
#define HWIO_APSS_ALIAS_4_FFWD_CNTL_FFWD_CNTL_OFFSET_HYST_CONFIG_SHFT                            0x0

#define HWIO_APSS_ALIAS_4_FFWD_STATUS_ADDR                                                (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x0000800c)
#define HWIO_APSS_ALIAS_4_FFWD_STATUS_RMSK                                                      0xff
#define HWIO_APSS_ALIAS_4_FFWD_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_FFWD_STATUS_ADDR, HWIO_APSS_ALIAS_4_FFWD_STATUS_RMSK)
#define HWIO_APSS_ALIAS_4_FFWD_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_FFWD_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_4_FFWD_STATUS_FFWD_CNT_VAL_BMSK                                         0xff
#define HWIO_APSS_ALIAS_4_FFWD_STATUS_FFWD_CNT_VAL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_4_BHS_SPARE_ADDR                                                  (APSS_ALIAS_4_APSS_CPU_IPM_REG_BASE      + 0x00008010)
#define HWIO_APSS_ALIAS_4_BHS_SPARE_RMSK                                                  0xffffffff
#define HWIO_APSS_ALIAS_4_BHS_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_4_BHS_SPARE_ADDR, HWIO_APSS_ALIAS_4_BHS_SPARE_RMSK)
#define HWIO_APSS_ALIAS_4_BHS_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_4_BHS_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_4_BHS_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_4_BHS_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_4_BHS_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_4_BHS_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_4_BHS_SPARE_IN)
#define HWIO_APSS_ALIAS_4_BHS_SPARE_SPARE_BMSK                                            0xffffffc0
#define HWIO_APSS_ALIAS_4_BHS_SPARE_SPARE_SHFT                                                   0x6
#define HWIO_APSS_ALIAS_4_BHS_SPARE_BHS_STRENGTH_BMSK                                           0x3f
#define HWIO_APSS_ALIAS_4_BHS_SPARE_BHS_STRENGTH_SHFT                                            0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_ALIAS_5_APSS_CPU_IPM
 *--------------------------------------------------------------------------*/

#define APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE                                                (APSS_HM_BASE      + 0x00650000)

#define HWIO_APSS_ALIAS_5_APC_SECURE_ADDR                                                 (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000000)
#define HWIO_APSS_ALIAS_5_APC_SECURE_RMSK                                                        0x1
#define HWIO_APSS_ALIAS_5_APC_SECURE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_APC_SECURE_ADDR, HWIO_APSS_ALIAS_5_APC_SECURE_RMSK)
#define HWIO_APSS_ALIAS_5_APC_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_APC_SECURE_ADDR, m)
#define HWIO_APSS_ALIAS_5_APC_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_APC_SECURE_ADDR,v)
#define HWIO_APSS_ALIAS_5_APC_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_APC_SECURE_ADDR,m,v,HWIO_APSS_ALIAS_5_APC_SECURE_IN)
#define HWIO_APSS_ALIAS_5_APC_SECURE_PWR_CTL_SEC_BMSK                                            0x1
#define HWIO_APSS_ALIAS_5_APC_SECURE_PWR_CTL_SEC_SHFT                                            0x0

#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_ADDR                                        (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000008)
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_RMSK                                             0x7ff
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_ADDR, HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_IN)
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_BMSK                                  0x400
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_SHFT                                    0xa
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_HS_CNT_BMSK                                      0x3ff
#define HWIO_APSS_ALIAS_5_CPU_HEAD_SWITCH_CTL_HS_CNT_SHFT                                        0x0

#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_ADDR                                            (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x0000000c)
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_RMSK                                               0x7ffff
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_ADDR, HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_IN)
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_BMSK                         0x70000
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_SHFT                            0x10
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_BMSK                         0xff00
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_SHFT                            0x8
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_BMSK                             0xff
#define HWIO_APSS_ALIAS_5_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_SHFT                              0x0

#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_ADDR                                        (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000010)
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_RMSK                                              0x7f
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_ADDR, HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_BMSK                               0x40
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_SHFT                                0x6
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_BMSK                             0x20
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_SHFT                              0x5
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_BMSK                                  0x10
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_SHFT                                   0x4
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_SLP_NRET_N_BMSK                                    0x8
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_SLP_NRET_N_SHFT                                    0x3
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_SLP_RET_N_BMSK                                     0x4
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_SLP_RET_N_SHFT                                     0x2
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_EN_REST_BMSK                                       0x2
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_EN_REST_SHFT                                       0x1
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_EN_FEW_BMSK                                        0x1
#define HWIO_APSS_ALIAS_5_CPU_PWR_GATE_STATUS_EN_FEW_SHFT                                        0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_ADDR                                     (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000014)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_RMSK                                        0xfffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_IN)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_BMSK                        0xf0000
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_SHFT                           0x10
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_BMSK                           0xffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_SHFT                              0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_ADDR                                    (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000018)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_RMSK                                       0xfffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_IN)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_BMSK                      0xf0000
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_SHFT                         0x10
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_BMSK                         0xffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR                                   (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x0000001c)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK                                     0xffffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_IN)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_EN_BMSK                                  0xffffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_EN_EN_SHFT                                       0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR                                  (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000020)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK                                  0x80ffffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_IN)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_BMSK                       0x80000000
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_SHFT                             0x1f
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_BMSK                        0xffffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_SHFT                             0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_ADDR                                                (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000024)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_RMSK                                                 0x1ff0f07
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_IN)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_START_ADDR_BMSK                                  0x1ff0000
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_START_ADDR_SHFT                                       0x10
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_BMSK                                   0xf00
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_SHFT                                     0x8
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_SLEEP_REQ_BMSK                                         0x4
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_SLEEP_REQ_SHFT                                         0x2
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_BMSK                                        0x2
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_SHFT                                        0x1
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_EN_BMSK                                                0x1
#define HWIO_APSS_ALIAS_5_CPU_SEQ_CTL_SEQ_EN_SHFT                                                0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n)                             (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_RMSK                                     0x1ff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_MAXn                                         3
#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), mask)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),val)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),mask,val,HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_INI(n))
#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_BMSK                   0x1ff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_SHFT                     0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_ADDR                                               (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000038)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_RMSK                                                   0xffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_SEQ_WAIT_EVENT_BMSK                                    0xffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS1_SEQ_WAIT_EVENT_SHFT                                       0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_ADDR                                               (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x0000003c)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_RMSK                                               0xffffffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_SEQ_PWR_CTL_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS2_SEQ_PWR_CTL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_ADDR                                               (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000040)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_RMSK                                                   0x3fff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_BMSK                                  0x3c00
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_SHFT                                     0xa
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_SEQ_BUSY_BMSK                                           0x200
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_SEQ_BUSY_SHFT                                             0x9
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_SEQ_CMD_ADDR_BMSK                                       0x1ff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_STS3_SEQ_CMD_ADDR_SHFT                                         0x0

#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_ADDR                                       (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000044)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_RMSK                                        0xfff63ff
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_ADDR, HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_IN)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_BMSK              0xf000000
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_SHFT                   0x18
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_BMSK                    0xff0000
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_SHFT                        0x10
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_BMSK                  0x4000
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_SHFT                     0xe
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_BMSK                      0x2000
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_SHFT                         0xd
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_BMSK                        0x3f8
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_SHFT                          0x3
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_BMSK                            0x4
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_SHFT                            0x2
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_BMSK                       0x2
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_SHFT                       0x1
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_BMSK                         0x1
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_SHFT                         0x0

#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_ADDR                                    (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000048)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_RMSK                                    0xffffffff
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_ADDR, HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_STATUS_BMSK                             0xffffffff
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_FSM_STATUS_STATUS_SHFT                                    0x0

#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_ADDR                                  (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x0000004c)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_RMSK                                  0xffffffff
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_ADDR, HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_STATUS_BMSK                           0xffffffff
#define HWIO_APSS_ALIAS_5_CPU_PCHANNEL_CTLER_STATUS_STATUS_SHFT                                  0x0

#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_ADDR                                    (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000054)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_RMSK                                         0xfff
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_ADDR, HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_IN)
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_BMSK                 0x800
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_SHFT                   0xb
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_BMSK                 0x400
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_SHFT                   0xa
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_BMSK                0x200
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_SHFT                  0x9
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_BMSK                 0x100
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_SHFT                   0x8
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_BMSK                 0x80
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_SHFT                  0x7
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_BMSK                 0x40
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_SHFT                  0x6
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_BMSK                0x20
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_SHFT                 0x5
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_BMSK                 0x10
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_SHFT                  0x4
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_BMSK                     0x8
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_SHFT                     0x3
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_BMSK                     0x4
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_SHFT                     0x2
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_BMSK                    0x2
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_SHFT                    0x1
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_BMSK                     0x1
#define HWIO_APSS_ALIAS_5_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_SHFT                     0x0

#define HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_ADDR                                        (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000058)
#define HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_ADDR, HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_RMSK)
#define HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_ADDR, m)
#define HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_ADDR,v)
#define HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_ADDR,m,v,HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_IN)
#define HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_BMSK              0xffffffff
#define HWIO_APSS_ALIAS_5_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_SHFT                     0x0

#define HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_ADDR                                            (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x0000005c)
#define HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_RMSK                                            0xffffffff
#define HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_ADDR, HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_RMSK)
#define HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_ADDR, m)
#define HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_VALUE_WHEN_HUNG_BMSK                            0xffffffff
#define HWIO_APSS_ALIAS_5_CORE_HANG_VALUE_VALUE_WHEN_HUNG_SHFT                                   0x0

#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_ADDR                                           (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000060)
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_RMSK                                                0x3fb
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_ADDR, HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_RMSK)
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_ADDR,m,v,HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_IN)
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_BMSK                        0x200
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_SHFT                          0x9
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_PMUEVENT_SEL_BMSK                                   0x1f0
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_PMUEVENT_SEL_SHFT                                     0x4
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_CORE_HANG_RESET_BMSK                                  0x8
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_CORE_HANG_RESET_SHFT                                  0x3
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_CORE_HANG_STATUS_BMSK                                 0x1
#define HWIO_APSS_ALIAS_5_CORE_HANG_CONFIG_CORE_HANG_STATUS_SHFT                                 0x0

#define HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_ADDR                                          (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000064)
#define HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_RMSK                                          0xffffffff
#define HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_ADDR, HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_RMSK)
#define HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_ADDR, m)
#define HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_STATUS_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_5_CORE_HANG_DBG_STS_STATUS_SHFT                                          0x0

#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_ADDR                                            (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00000068)
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_RMSK                                                 0x3ff
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_ADDR, HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_RMSK)
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_ADDR, m)
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_ADDR,v)
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_ADDR,m,v,HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_IN)
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_EVENT_SEL_BMSK                                       0x3e0
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_EVENT_SEL_SHFT                                         0x5
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_PWR_CTL_SEL_BMSK                                      0x1f
#define HWIO_APSS_ALIAS_5_CPU_HWEVENT_SEL_PWR_CTL_SEL_SHFT                                       0x0

#define HWIO_APSS_ALIAS_5_SPARE_ADDR                                                      (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x000000f0)
#define HWIO_APSS_ALIAS_5_SPARE_RMSK                                                            0xff
#define HWIO_APSS_ALIAS_5_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_SPARE_ADDR, HWIO_APSS_ALIAS_5_SPARE_RMSK)
#define HWIO_APSS_ALIAS_5_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_5_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_5_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_5_SPARE_IN)
#define HWIO_APSS_ALIAS_5_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_APSS_ALIAS_5_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_APSS_ALIAS_5_SPARE_TZ_ADDR                                                   (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x000000f4)
#define HWIO_APSS_ALIAS_5_SPARE_TZ_RMSK                                                         0xff
#define HWIO_APSS_ALIAS_5_SPARE_TZ_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_SPARE_TZ_ADDR, HWIO_APSS_ALIAS_5_SPARE_TZ_RMSK)
#define HWIO_APSS_ALIAS_5_SPARE_TZ_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_SPARE_TZ_ADDR, m)
#define HWIO_APSS_ALIAS_5_SPARE_TZ_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_SPARE_TZ_ADDR,v)
#define HWIO_APSS_ALIAS_5_SPARE_TZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_SPARE_TZ_ADDR,m,v,HWIO_APSS_ALIAS_5_SPARE_TZ_IN)
#define HWIO_APSS_ALIAS_5_SPARE_TZ_SPARE_BMSK                                                   0xff
#define HWIO_APSS_ALIAS_5_SPARE_TZ_SPARE_SHFT                                                    0x0

#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_ADDR                                                 (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00008000)
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_RMSK                                                       0x7b
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_SEQCTL_BHS_ADDR, HWIO_APSS_ALIAS_5_SEQCTL_BHS_RMSK)
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_SEQCTL_BHS_ADDR, m)
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_SEQCTL_BHS_ADDR,v)
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_SEQCTL_BHS_ADDR,m,v,HWIO_APSS_ALIAS_5_SEQCTL_BHS_IN)
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_ENR_BRANCH_DIS_BMSK                                        0x78
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_ENR_BRANCH_DIS_SHFT                                         0x3
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_BHS_REST_DLY_SEL_BMSK                                       0x2
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_BHS_REST_DLY_SEL_SHFT                                       0x1
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_BHS_FEW_DLY_SEL_BMSK                                        0x1
#define HWIO_APSS_ALIAS_5_SEQCTL_BHS_BHS_FEW_DLY_SEL_SHFT                                        0x0

#define HWIO_APSS_ALIAS_5_BHS_STATUS_ADDR                                                 (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00008004)
#define HWIO_APSS_ALIAS_5_BHS_STATUS_RMSK                                                        0x3
#define HWIO_APSS_ALIAS_5_BHS_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_BHS_STATUS_ADDR, HWIO_APSS_ALIAS_5_BHS_STATUS_RMSK)
#define HWIO_APSS_ALIAS_5_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_BHS_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_5_BHS_STATUS_BHS_EN_REST_ACK_BMSK                                        0x2
#define HWIO_APSS_ALIAS_5_BHS_STATUS_BHS_EN_REST_ACK_SHFT                                        0x1
#define HWIO_APSS_ALIAS_5_BHS_STATUS_BHS_EN_FEW_ACK_BMSK                                         0x1
#define HWIO_APSS_ALIAS_5_BHS_STATUS_BHS_EN_FEW_ACK_SHFT                                         0x0

#define HWIO_APSS_ALIAS_5_FFWD_CNTL_ADDR                                                  (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00008008)
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_RMSK                                                      0xffff
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_FFWD_CNTL_ADDR, HWIO_APSS_ALIAS_5_FFWD_CNTL_RMSK)
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_FFWD_CNTL_ADDR, m)
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_FFWD_CNTL_ADDR,v)
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_FFWD_CNTL_ADDR,m,v,HWIO_APSS_ALIAS_5_FFWD_CNTL_IN)
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_FFWD_CNTL_EN_BMSK                                         0x8000
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_FFWD_CNTL_EN_SHFT                                            0xf
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_FFWD_CNTL_DELAY_BMSK                                      0x7f80
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_FFWD_CNTL_DELAY_SHFT                                         0x7
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_FFWD_CNTL_CLK_DIVIDE_BY_BMSK                                0x60
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_FFWD_CNTL_CLK_DIVIDE_BY_SHFT                                 0x5
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_FFWD_CNTL_OFFSET_HYST_CONFIG_BMSK                           0x1f
#define HWIO_APSS_ALIAS_5_FFWD_CNTL_FFWD_CNTL_OFFSET_HYST_CONFIG_SHFT                            0x0

#define HWIO_APSS_ALIAS_5_FFWD_STATUS_ADDR                                                (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x0000800c)
#define HWIO_APSS_ALIAS_5_FFWD_STATUS_RMSK                                                      0xff
#define HWIO_APSS_ALIAS_5_FFWD_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_FFWD_STATUS_ADDR, HWIO_APSS_ALIAS_5_FFWD_STATUS_RMSK)
#define HWIO_APSS_ALIAS_5_FFWD_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_FFWD_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_5_FFWD_STATUS_FFWD_CNT_VAL_BMSK                                         0xff
#define HWIO_APSS_ALIAS_5_FFWD_STATUS_FFWD_CNT_VAL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_5_BHS_SPARE_ADDR                                                  (APSS_ALIAS_5_APSS_CPU_IPM_REG_BASE      + 0x00008010)
#define HWIO_APSS_ALIAS_5_BHS_SPARE_RMSK                                                  0xffffffff
#define HWIO_APSS_ALIAS_5_BHS_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_5_BHS_SPARE_ADDR, HWIO_APSS_ALIAS_5_BHS_SPARE_RMSK)
#define HWIO_APSS_ALIAS_5_BHS_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_5_BHS_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_5_BHS_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_5_BHS_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_5_BHS_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_5_BHS_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_5_BHS_SPARE_IN)
#define HWIO_APSS_ALIAS_5_BHS_SPARE_SPARE_BMSK                                            0xffffffc0
#define HWIO_APSS_ALIAS_5_BHS_SPARE_SPARE_SHFT                                                   0x6
#define HWIO_APSS_ALIAS_5_BHS_SPARE_BHS_STRENGTH_BMSK                                           0x3f
#define HWIO_APSS_ALIAS_5_BHS_SPARE_BHS_STRENGTH_SHFT                                            0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_ALIAS_6_APSS_CPU_IPM
 *--------------------------------------------------------------------------*/

#define APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE                                                (APSS_HM_BASE      + 0x00660000)

#define HWIO_APSS_ALIAS_6_APC_SECURE_ADDR                                                 (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000000)
#define HWIO_APSS_ALIAS_6_APC_SECURE_RMSK                                                        0x1
#define HWIO_APSS_ALIAS_6_APC_SECURE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_APC_SECURE_ADDR, HWIO_APSS_ALIAS_6_APC_SECURE_RMSK)
#define HWIO_APSS_ALIAS_6_APC_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_APC_SECURE_ADDR, m)
#define HWIO_APSS_ALIAS_6_APC_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_APC_SECURE_ADDR,v)
#define HWIO_APSS_ALIAS_6_APC_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_APC_SECURE_ADDR,m,v,HWIO_APSS_ALIAS_6_APC_SECURE_IN)
#define HWIO_APSS_ALIAS_6_APC_SECURE_PWR_CTL_SEC_BMSK                                            0x1
#define HWIO_APSS_ALIAS_6_APC_SECURE_PWR_CTL_SEC_SHFT                                            0x0

#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_ADDR                                        (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000008)
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_RMSK                                             0x7ff
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_ADDR, HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_IN)
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_BMSK                                  0x400
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_SHFT                                    0xa
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_HS_CNT_BMSK                                      0x3ff
#define HWIO_APSS_ALIAS_6_CPU_HEAD_SWITCH_CTL_HS_CNT_SHFT                                        0x0

#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_ADDR                                            (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x0000000c)
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_RMSK                                               0x7ffff
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_ADDR, HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_IN)
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_BMSK                         0x70000
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_SHFT                            0x10
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_BMSK                         0xff00
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_SHFT                            0x8
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_BMSK                             0xff
#define HWIO_APSS_ALIAS_6_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_SHFT                              0x0

#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_ADDR                                        (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000010)
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_RMSK                                              0x7f
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_ADDR, HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_BMSK                               0x40
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_SHFT                                0x6
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_BMSK                             0x20
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_SHFT                              0x5
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_BMSK                                  0x10
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_SHFT                                   0x4
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_SLP_NRET_N_BMSK                                    0x8
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_SLP_NRET_N_SHFT                                    0x3
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_SLP_RET_N_BMSK                                     0x4
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_SLP_RET_N_SHFT                                     0x2
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_EN_REST_BMSK                                       0x2
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_EN_REST_SHFT                                       0x1
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_EN_FEW_BMSK                                        0x1
#define HWIO_APSS_ALIAS_6_CPU_PWR_GATE_STATUS_EN_FEW_SHFT                                        0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_ADDR                                     (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000014)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_RMSK                                        0xfffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_IN)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_BMSK                        0xf0000
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_SHFT                           0x10
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_BMSK                           0xffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_SHFT                              0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_ADDR                                    (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000018)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_RMSK                                       0xfffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_IN)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_BMSK                      0xf0000
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_SHFT                         0x10
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_BMSK                         0xffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR                                   (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x0000001c)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK                                     0xffffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_IN)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_EN_BMSK                                  0xffffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_EN_EN_SHFT                                       0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR                                  (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000020)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK                                  0x80ffffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_IN)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_BMSK                       0x80000000
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_SHFT                             0x1f
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_BMSK                        0xffffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_SHFT                             0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_ADDR                                                (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000024)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_RMSK                                                 0x1ff0f07
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_IN)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_START_ADDR_BMSK                                  0x1ff0000
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_START_ADDR_SHFT                                       0x10
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_BMSK                                   0xf00
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_SHFT                                     0x8
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_SLEEP_REQ_BMSK                                         0x4
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_SLEEP_REQ_SHFT                                         0x2
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_BMSK                                        0x2
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_SHFT                                        0x1
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_EN_BMSK                                                0x1
#define HWIO_APSS_ALIAS_6_CPU_SEQ_CTL_SEQ_EN_SHFT                                                0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n)                             (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_RMSK                                     0x1ff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_MAXn                                         3
#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), mask)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),val)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),mask,val,HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_INI(n))
#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_BMSK                   0x1ff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_SHFT                     0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_ADDR                                               (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000038)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_RMSK                                                   0xffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_SEQ_WAIT_EVENT_BMSK                                    0xffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS1_SEQ_WAIT_EVENT_SHFT                                       0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_ADDR                                               (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x0000003c)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_RMSK                                               0xffffffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_SEQ_PWR_CTL_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS2_SEQ_PWR_CTL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_ADDR                                               (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000040)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_RMSK                                                   0x3fff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_BMSK                                  0x3c00
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_SHFT                                     0xa
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_SEQ_BUSY_BMSK                                           0x200
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_SEQ_BUSY_SHFT                                             0x9
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_SEQ_CMD_ADDR_BMSK                                       0x1ff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_STS3_SEQ_CMD_ADDR_SHFT                                         0x0

#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_ADDR                                       (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000044)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_RMSK                                        0xfff63ff
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_ADDR, HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_IN)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_BMSK              0xf000000
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_SHFT                   0x18
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_BMSK                    0xff0000
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_SHFT                        0x10
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_BMSK                  0x4000
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_SHFT                     0xe
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_BMSK                      0x2000
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_SHFT                         0xd
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_BMSK                        0x3f8
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_SHFT                          0x3
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_BMSK                            0x4
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_SHFT                            0x2
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_BMSK                       0x2
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_SHFT                       0x1
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_BMSK                         0x1
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_SHFT                         0x0

#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_ADDR                                    (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000048)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_RMSK                                    0xffffffff
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_ADDR, HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_STATUS_BMSK                             0xffffffff
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_FSM_STATUS_STATUS_SHFT                                    0x0

#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_ADDR                                  (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x0000004c)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_RMSK                                  0xffffffff
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_ADDR, HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_STATUS_BMSK                           0xffffffff
#define HWIO_APSS_ALIAS_6_CPU_PCHANNEL_CTLER_STATUS_STATUS_SHFT                                  0x0

#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_ADDR                                    (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000054)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_RMSK                                         0xfff
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_ADDR, HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_IN)
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_BMSK                 0x800
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_SHFT                   0xb
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_BMSK                 0x400
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_SHFT                   0xa
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_BMSK                0x200
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_SHFT                  0x9
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_BMSK                 0x100
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_SHFT                   0x8
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_BMSK                 0x80
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_SHFT                  0x7
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_BMSK                 0x40
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_SHFT                  0x6
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_BMSK                0x20
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_SHFT                 0x5
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_BMSK                 0x10
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_SHFT                  0x4
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_BMSK                     0x8
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_SHFT                     0x3
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_BMSK                     0x4
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_SHFT                     0x2
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_BMSK                    0x2
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_SHFT                    0x1
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_BMSK                     0x1
#define HWIO_APSS_ALIAS_6_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_SHFT                     0x0

#define HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_ADDR                                        (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000058)
#define HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_ADDR, HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_RMSK)
#define HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_ADDR, m)
#define HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_ADDR,v)
#define HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_ADDR,m,v,HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_IN)
#define HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_BMSK              0xffffffff
#define HWIO_APSS_ALIAS_6_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_SHFT                     0x0

#define HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_ADDR                                            (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x0000005c)
#define HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_RMSK                                            0xffffffff
#define HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_ADDR, HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_RMSK)
#define HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_ADDR, m)
#define HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_VALUE_WHEN_HUNG_BMSK                            0xffffffff
#define HWIO_APSS_ALIAS_6_CORE_HANG_VALUE_VALUE_WHEN_HUNG_SHFT                                   0x0

#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_ADDR                                           (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000060)
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_RMSK                                                0x3fb
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_ADDR, HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_RMSK)
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_ADDR,m,v,HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_IN)
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_BMSK                        0x200
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_SHFT                          0x9
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_PMUEVENT_SEL_BMSK                                   0x1f0
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_PMUEVENT_SEL_SHFT                                     0x4
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_CORE_HANG_RESET_BMSK                                  0x8
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_CORE_HANG_RESET_SHFT                                  0x3
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_CORE_HANG_STATUS_BMSK                                 0x1
#define HWIO_APSS_ALIAS_6_CORE_HANG_CONFIG_CORE_HANG_STATUS_SHFT                                 0x0

#define HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_ADDR                                          (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000064)
#define HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_RMSK                                          0xffffffff
#define HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_ADDR, HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_RMSK)
#define HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_ADDR, m)
#define HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_STATUS_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_6_CORE_HANG_DBG_STS_STATUS_SHFT                                          0x0

#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_ADDR                                            (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00000068)
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_RMSK                                                 0x3ff
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_ADDR, HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_RMSK)
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_ADDR, m)
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_ADDR,v)
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_ADDR,m,v,HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_IN)
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_EVENT_SEL_BMSK                                       0x3e0
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_EVENT_SEL_SHFT                                         0x5
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_PWR_CTL_SEL_BMSK                                      0x1f
#define HWIO_APSS_ALIAS_6_CPU_HWEVENT_SEL_PWR_CTL_SEL_SHFT                                       0x0

#define HWIO_APSS_ALIAS_6_SPARE_ADDR                                                      (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x000000f0)
#define HWIO_APSS_ALIAS_6_SPARE_RMSK                                                            0xff
#define HWIO_APSS_ALIAS_6_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_SPARE_ADDR, HWIO_APSS_ALIAS_6_SPARE_RMSK)
#define HWIO_APSS_ALIAS_6_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_6_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_6_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_6_SPARE_IN)
#define HWIO_APSS_ALIAS_6_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_APSS_ALIAS_6_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_APSS_ALIAS_6_SPARE_TZ_ADDR                                                   (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x000000f4)
#define HWIO_APSS_ALIAS_6_SPARE_TZ_RMSK                                                         0xff
#define HWIO_APSS_ALIAS_6_SPARE_TZ_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_SPARE_TZ_ADDR, HWIO_APSS_ALIAS_6_SPARE_TZ_RMSK)
#define HWIO_APSS_ALIAS_6_SPARE_TZ_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_SPARE_TZ_ADDR, m)
#define HWIO_APSS_ALIAS_6_SPARE_TZ_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_SPARE_TZ_ADDR,v)
#define HWIO_APSS_ALIAS_6_SPARE_TZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_SPARE_TZ_ADDR,m,v,HWIO_APSS_ALIAS_6_SPARE_TZ_IN)
#define HWIO_APSS_ALIAS_6_SPARE_TZ_SPARE_BMSK                                                   0xff
#define HWIO_APSS_ALIAS_6_SPARE_TZ_SPARE_SHFT                                                    0x0

#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_ADDR                                                 (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00008000)
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_RMSK                                                       0x7b
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_SEQCTL_BHS_ADDR, HWIO_APSS_ALIAS_6_SEQCTL_BHS_RMSK)
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_SEQCTL_BHS_ADDR, m)
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_SEQCTL_BHS_ADDR,v)
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_SEQCTL_BHS_ADDR,m,v,HWIO_APSS_ALIAS_6_SEQCTL_BHS_IN)
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_ENR_BRANCH_DIS_BMSK                                        0x78
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_ENR_BRANCH_DIS_SHFT                                         0x3
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_BHS_REST_DLY_SEL_BMSK                                       0x2
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_BHS_REST_DLY_SEL_SHFT                                       0x1
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_BHS_FEW_DLY_SEL_BMSK                                        0x1
#define HWIO_APSS_ALIAS_6_SEQCTL_BHS_BHS_FEW_DLY_SEL_SHFT                                        0x0

#define HWIO_APSS_ALIAS_6_BHS_STATUS_ADDR                                                 (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00008004)
#define HWIO_APSS_ALIAS_6_BHS_STATUS_RMSK                                                        0x3
#define HWIO_APSS_ALIAS_6_BHS_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_BHS_STATUS_ADDR, HWIO_APSS_ALIAS_6_BHS_STATUS_RMSK)
#define HWIO_APSS_ALIAS_6_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_BHS_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_6_BHS_STATUS_BHS_EN_REST_ACK_BMSK                                        0x2
#define HWIO_APSS_ALIAS_6_BHS_STATUS_BHS_EN_REST_ACK_SHFT                                        0x1
#define HWIO_APSS_ALIAS_6_BHS_STATUS_BHS_EN_FEW_ACK_BMSK                                         0x1
#define HWIO_APSS_ALIAS_6_BHS_STATUS_BHS_EN_FEW_ACK_SHFT                                         0x0

#define HWIO_APSS_ALIAS_6_FFWD_CNTL_ADDR                                                  (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00008008)
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_RMSK                                                      0xffff
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_FFWD_CNTL_ADDR, HWIO_APSS_ALIAS_6_FFWD_CNTL_RMSK)
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_FFWD_CNTL_ADDR, m)
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_FFWD_CNTL_ADDR,v)
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_FFWD_CNTL_ADDR,m,v,HWIO_APSS_ALIAS_6_FFWD_CNTL_IN)
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_FFWD_CNTL_EN_BMSK                                         0x8000
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_FFWD_CNTL_EN_SHFT                                            0xf
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_FFWD_CNTL_DELAY_BMSK                                      0x7f80
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_FFWD_CNTL_DELAY_SHFT                                         0x7
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_FFWD_CNTL_CLK_DIVIDE_BY_BMSK                                0x60
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_FFWD_CNTL_CLK_DIVIDE_BY_SHFT                                 0x5
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_FFWD_CNTL_OFFSET_HYST_CONFIG_BMSK                           0x1f
#define HWIO_APSS_ALIAS_6_FFWD_CNTL_FFWD_CNTL_OFFSET_HYST_CONFIG_SHFT                            0x0

#define HWIO_APSS_ALIAS_6_FFWD_STATUS_ADDR                                                (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x0000800c)
#define HWIO_APSS_ALIAS_6_FFWD_STATUS_RMSK                                                      0xff
#define HWIO_APSS_ALIAS_6_FFWD_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_FFWD_STATUS_ADDR, HWIO_APSS_ALIAS_6_FFWD_STATUS_RMSK)
#define HWIO_APSS_ALIAS_6_FFWD_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_FFWD_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_6_FFWD_STATUS_FFWD_CNT_VAL_BMSK                                         0xff
#define HWIO_APSS_ALIAS_6_FFWD_STATUS_FFWD_CNT_VAL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_6_BHS_SPARE_ADDR                                                  (APSS_ALIAS_6_APSS_CPU_IPM_REG_BASE      + 0x00008010)
#define HWIO_APSS_ALIAS_6_BHS_SPARE_RMSK                                                  0xffffffff
#define HWIO_APSS_ALIAS_6_BHS_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_6_BHS_SPARE_ADDR, HWIO_APSS_ALIAS_6_BHS_SPARE_RMSK)
#define HWIO_APSS_ALIAS_6_BHS_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_6_BHS_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_6_BHS_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_6_BHS_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_6_BHS_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_6_BHS_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_6_BHS_SPARE_IN)
#define HWIO_APSS_ALIAS_6_BHS_SPARE_SPARE_BMSK                                            0xffffffc0
#define HWIO_APSS_ALIAS_6_BHS_SPARE_SPARE_SHFT                                                   0x6
#define HWIO_APSS_ALIAS_6_BHS_SPARE_BHS_STRENGTH_BMSK                                           0x3f
#define HWIO_APSS_ALIAS_6_BHS_SPARE_BHS_STRENGTH_SHFT                                            0x0

/*----------------------------------------------------------------------------
 * MODULE: APSS_ALIAS_7_APSS_CPU_IPM
 *--------------------------------------------------------------------------*/

#define APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE                                                (APSS_HM_BASE      + 0x00670000)

#define HWIO_APSS_ALIAS_7_APC_SECURE_ADDR                                                 (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000000)
#define HWIO_APSS_ALIAS_7_APC_SECURE_RMSK                                                        0x1
#define HWIO_APSS_ALIAS_7_APC_SECURE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_APC_SECURE_ADDR, HWIO_APSS_ALIAS_7_APC_SECURE_RMSK)
#define HWIO_APSS_ALIAS_7_APC_SECURE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_APC_SECURE_ADDR, m)
#define HWIO_APSS_ALIAS_7_APC_SECURE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_APC_SECURE_ADDR,v)
#define HWIO_APSS_ALIAS_7_APC_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_APC_SECURE_ADDR,m,v,HWIO_APSS_ALIAS_7_APC_SECURE_IN)
#define HWIO_APSS_ALIAS_7_APC_SECURE_PWR_CTL_SEC_BMSK                                            0x1
#define HWIO_APSS_ALIAS_7_APC_SECURE_PWR_CTL_SEC_SHFT                                            0x0

#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_ADDR                                        (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000008)
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_RMSK                                             0x7ff
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_ADDR, HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_IN)
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_BMSK                                  0x400
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_CPU_PWRDUP_SHFT                                    0xa
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_HS_CNT_BMSK                                      0x3ff
#define HWIO_APSS_ALIAS_7_CPU_HEAD_SWITCH_CTL_HS_CNT_SHFT                                        0x0

#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_ADDR                                            (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x0000000c)
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_RMSK                                               0x7ffff
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_ADDR, HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_IN)
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_BMSK                         0x70000
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_MEM_SEQ_CLK_DIV_RATIO_SHFT                            0x10
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_BMSK                         0xff00
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_MEM_WL_EN_SEQ_TERM_VAL_SHFT                            0x8
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_BMSK                             0xff
#define HWIO_APSS_ALIAS_7_CPU_MEM_SLP_CTL_MEM_SLP_SEQ_TERM_VAL_SHFT                              0x0

#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_ADDR                                        (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000010)
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_RMSK                                              0x7f
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_ADDR, HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_BMSK                               0x40
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_IDLE_WL_EN_SEQ_SHFT                                0x6
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_BMSK                             0x20
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_IDLE_MEM_SLP_SEQ_SHFT                              0x5
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_BMSK                                  0x10
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_IDLE_HS_SEQ_SHFT                                   0x4
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_SLP_NRET_N_BMSK                                    0x8
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_SLP_NRET_N_SHFT                                    0x3
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_SLP_RET_N_BMSK                                     0x4
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_SLP_RET_N_SHFT                                     0x2
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_EN_REST_BMSK                                       0x2
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_EN_REST_SHFT                                       0x1
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_EN_FEW_BMSK                                        0x1
#define HWIO_APSS_ALIAS_7_CPU_PWR_GATE_STATUS_EN_FEW_SHFT                                        0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_ADDR                                     (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000014)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_RMSK                                        0xfffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_IN)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_BMSK                        0xf0000
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_BRANCH_EVENT_EN_SHFT                           0x10
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_BMSK                           0xffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_EN_WAIT_EVENT_EN_SHFT                              0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_ADDR                                    (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000018)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_RMSK                                       0xfffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_IN)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_BMSK                      0xf0000
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_BRANCH_EVENT_VAL_SHFT                         0x10
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_BMSK                         0xffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_EVENT_VAL_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR                                   (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x0000001c)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK                                     0xffffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_IN)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_EN_BMSK                                  0xffffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_EN_EN_SHFT                                       0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR                                  (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000020)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK                                  0x80ffffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_IN)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_BMSK                       0x80000000
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_OVRRIDE_EN_SHFT                             0x1f
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_BMSK                        0xffffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_FORCE_PWR_CTL_VAL_PWR_CTL_VAL_SHFT                             0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_ADDR                                                (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000024)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_RMSK                                                 0x1ff0f07
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_IN)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_START_ADDR_BMSK                                  0x1ff0000
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_START_ADDR_SHFT                                       0x10
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_BMSK                                   0xf00
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_PWR_STATE_IDX_SHFT                                     0x8
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_SLEEP_REQ_BMSK                                         0x4
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_SLEEP_REQ_SHFT                                         0x2
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_BMSK                                        0x2
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_WAKEUP_REQ_SHFT                                        0x1
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_EN_BMSK                                                0x1
#define HWIO_APSS_ALIAS_7_CPU_SEQ_CTL_SEQ_EN_SHFT                                                0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n)                             (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000028 + 0x4 * (n))
#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_RMSK                                     0x1ff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_MAXn                                         3
#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_INI(n)        \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n), mask)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_OUTI(n,val)    \
        out_dword(HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),val)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_ADDR(n),mask,val,HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_INI(n))
#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_BMSK                   0x1ff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_BRANCH_START_ADDR_n_BRANCH_START_ADDR_SHFT                     0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_ADDR                                               (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000038)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_RMSK                                                   0xffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_SEQ_WAIT_EVENT_BMSK                                    0xffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS1_SEQ_WAIT_EVENT_SHFT                                       0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_ADDR                                               (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x0000003c)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_RMSK                                               0xffffffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_SEQ_PWR_CTL_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS2_SEQ_PWR_CTL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_ADDR                                               (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000040)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_RMSK                                                   0x3fff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_BMSK                                  0x3c00
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_SEQ_BRANCH_EVENT_SHFT                                     0xa
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_SEQ_BUSY_BMSK                                           0x200
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_SEQ_BUSY_SHFT                                             0x9
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_SEQ_CMD_ADDR_BMSK                                       0x1ff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_STS3_SEQ_CMD_ADDR_SHFT                                         0x0

#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_ADDR                                       (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000044)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_RMSK                                        0xfff63ff
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_ADDR, HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_IN)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_BMSK              0xf000000
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PACTIVE_SYNC_TMR_SHFT                   0x18
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_BMSK                    0xff0000
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_PSTABLE_TMR_SHFT                        0x10
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_BMSK                  0x4000
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_WO_MEM_RST_SHFT                     0xe
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_BMSK                      0x2000
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_MASK_WAKEUP_SHFT                         0xd
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_BMSK                        0x3f8
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PSTATE_SHFT                          0x3
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_BMSK                            0x4
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_PREQ_SHFT                            0x2
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_BMSK                       0x2
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_FSM_SW_CTL_EN_SHFT                       0x1
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_BMSK                         0x1
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_CTL_PCHANNEL_BOOT_SW_REQ_SHFT                         0x0

#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_ADDR                                    (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000048)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_RMSK                                    0xffffffff
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_ADDR, HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_STATUS_BMSK                             0xffffffff
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_FSM_STATUS_STATUS_SHFT                                    0x0

#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_ADDR                                  (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x0000004c)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_RMSK                                  0xffffffff
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_ADDR, HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_STATUS_BMSK                           0xffffffff
#define HWIO_APSS_ALIAS_7_CPU_PCHANNEL_CTLER_STATUS_STATUS_SHFT                                  0x0

#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_ADDR                                    (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000054)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_RMSK                                         0xfff
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_ADDR, HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_IN)
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_BMSK                 0x800
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C4_SHFT                   0xb
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_BMSK                 0x400
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C3_SHFT                   0xa
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_BMSK                0x200
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C2D_SHFT                  0x9
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_BMSK                 0x100
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_MODE_C1_SHFT                   0x8
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_BMSK                 0x80
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C4_SHFT                  0x7
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_BMSK                 0x40
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C3_SHFT                  0x6
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_BMSK                0x20
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C2D_SHFT                 0x5
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_BMSK                 0x10
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_RESET_C1_SHFT                  0x4
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_BMSK                     0x8
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C4_SHFT                     0x3
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_BMSK                     0x4
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C3_SHFT                     0x2
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_BMSK                    0x2
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C2D_SHFT                    0x1
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_BMSK                     0x1
#define HWIO_APSS_ALIAS_7_CPU_SEQ_LPM_COUNTER_CFG_SEQ_LPM_COUNTER_EN_C1_SHFT                     0x0

#define HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_ADDR                                        (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000058)
#define HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_ADDR, HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_RMSK)
#define HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_ADDR, m)
#define HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_ADDR,v)
#define HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_ADDR,m,v,HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_IN)
#define HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_BMSK              0xffffffff
#define HWIO_APSS_ALIAS_7_CORE_HANG_THRESHOLD_CORE_HANG_THRESHOLD_VALUE_SHFT                     0x0

#define HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_ADDR                                            (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x0000005c)
#define HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_RMSK                                            0xffffffff
#define HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_ADDR, HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_RMSK)
#define HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_ADDR, m)
#define HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_VALUE_WHEN_HUNG_BMSK                            0xffffffff
#define HWIO_APSS_ALIAS_7_CORE_HANG_VALUE_VALUE_WHEN_HUNG_SHFT                                   0x0

#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_ADDR                                           (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000060)
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_RMSK                                                0x3fb
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_ADDR, HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_RMSK)
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_ADDR, m)
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_ADDR,v)
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_ADDR,m,v,HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_IN)
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_BMSK                        0x200
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_PMUEVENT_LEGACY_IMP_SEL_SHFT                          0x9
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_PMUEVENT_SEL_BMSK                                   0x1f0
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_PMUEVENT_SEL_SHFT                                     0x4
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_CORE_HANG_RESET_BMSK                                  0x8
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_CORE_HANG_RESET_SHFT                                  0x3
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_BMSK                             0x2
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_CORE_HANG_COUNTER_EN_SHFT                             0x1
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_CORE_HANG_STATUS_BMSK                                 0x1
#define HWIO_APSS_ALIAS_7_CORE_HANG_CONFIG_CORE_HANG_STATUS_SHFT                                 0x0

#define HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_ADDR                                          (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000064)
#define HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_RMSK                                          0xffffffff
#define HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_ADDR, HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_RMSK)
#define HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_ADDR, m)
#define HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_STATUS_BMSK                                   0xffffffff
#define HWIO_APSS_ALIAS_7_CORE_HANG_DBG_STS_STATUS_SHFT                                          0x0

#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_ADDR                                            (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00000068)
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_RMSK                                                 0x3ff
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_ADDR, HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_RMSK)
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_ADDR, m)
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_ADDR,v)
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_ADDR,m,v,HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_IN)
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_EVENT_SEL_BMSK                                       0x3e0
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_EVENT_SEL_SHFT                                         0x5
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_PWR_CTL_SEL_BMSK                                      0x1f
#define HWIO_APSS_ALIAS_7_CPU_HWEVENT_SEL_PWR_CTL_SEL_SHFT                                       0x0

#define HWIO_APSS_ALIAS_7_SPARE_ADDR                                                      (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x000000f0)
#define HWIO_APSS_ALIAS_7_SPARE_RMSK                                                            0xff
#define HWIO_APSS_ALIAS_7_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_SPARE_ADDR, HWIO_APSS_ALIAS_7_SPARE_RMSK)
#define HWIO_APSS_ALIAS_7_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_7_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_7_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_7_SPARE_IN)
#define HWIO_APSS_ALIAS_7_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_APSS_ALIAS_7_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_APSS_ALIAS_7_SPARE_TZ_ADDR                                                   (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x000000f4)
#define HWIO_APSS_ALIAS_7_SPARE_TZ_RMSK                                                         0xff
#define HWIO_APSS_ALIAS_7_SPARE_TZ_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_SPARE_TZ_ADDR, HWIO_APSS_ALIAS_7_SPARE_TZ_RMSK)
#define HWIO_APSS_ALIAS_7_SPARE_TZ_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_SPARE_TZ_ADDR, m)
#define HWIO_APSS_ALIAS_7_SPARE_TZ_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_SPARE_TZ_ADDR,v)
#define HWIO_APSS_ALIAS_7_SPARE_TZ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_SPARE_TZ_ADDR,m,v,HWIO_APSS_ALIAS_7_SPARE_TZ_IN)
#define HWIO_APSS_ALIAS_7_SPARE_TZ_SPARE_BMSK                                                   0xff
#define HWIO_APSS_ALIAS_7_SPARE_TZ_SPARE_SHFT                                                    0x0

#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_ADDR                                                 (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00008000)
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_RMSK                                                       0x7b
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_SEQCTL_BHS_ADDR, HWIO_APSS_ALIAS_7_SEQCTL_BHS_RMSK)
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_SEQCTL_BHS_ADDR, m)
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_SEQCTL_BHS_ADDR,v)
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_SEQCTL_BHS_ADDR,m,v,HWIO_APSS_ALIAS_7_SEQCTL_BHS_IN)
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_ENR_BRANCH_DIS_BMSK                                        0x78
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_ENR_BRANCH_DIS_SHFT                                         0x3
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_BHS_REST_DLY_SEL_BMSK                                       0x2
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_BHS_REST_DLY_SEL_SHFT                                       0x1
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_BHS_FEW_DLY_SEL_BMSK                                        0x1
#define HWIO_APSS_ALIAS_7_SEQCTL_BHS_BHS_FEW_DLY_SEL_SHFT                                        0x0

#define HWIO_APSS_ALIAS_7_BHS_STATUS_ADDR                                                 (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00008004)
#define HWIO_APSS_ALIAS_7_BHS_STATUS_RMSK                                                        0x3
#define HWIO_APSS_ALIAS_7_BHS_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_BHS_STATUS_ADDR, HWIO_APSS_ALIAS_7_BHS_STATUS_RMSK)
#define HWIO_APSS_ALIAS_7_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_BHS_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_7_BHS_STATUS_BHS_EN_REST_ACK_BMSK                                        0x2
#define HWIO_APSS_ALIAS_7_BHS_STATUS_BHS_EN_REST_ACK_SHFT                                        0x1
#define HWIO_APSS_ALIAS_7_BHS_STATUS_BHS_EN_FEW_ACK_BMSK                                         0x1
#define HWIO_APSS_ALIAS_7_BHS_STATUS_BHS_EN_FEW_ACK_SHFT                                         0x0

#define HWIO_APSS_ALIAS_7_FFWD_CNTL_ADDR                                                  (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00008008)
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_RMSK                                                      0xffff
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_FFWD_CNTL_ADDR, HWIO_APSS_ALIAS_7_FFWD_CNTL_RMSK)
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_FFWD_CNTL_ADDR, m)
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_FFWD_CNTL_ADDR,v)
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_FFWD_CNTL_ADDR,m,v,HWIO_APSS_ALIAS_7_FFWD_CNTL_IN)
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_FFWD_CNTL_EN_BMSK                                         0x8000
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_FFWD_CNTL_EN_SHFT                                            0xf
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_FFWD_CNTL_DELAY_BMSK                                      0x7f80
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_FFWD_CNTL_DELAY_SHFT                                         0x7
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_FFWD_CNTL_CLK_DIVIDE_BY_BMSK                                0x60
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_FFWD_CNTL_CLK_DIVIDE_BY_SHFT                                 0x5
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_FFWD_CNTL_OFFSET_HYST_CONFIG_BMSK                           0x1f
#define HWIO_APSS_ALIAS_7_FFWD_CNTL_FFWD_CNTL_OFFSET_HYST_CONFIG_SHFT                            0x0

#define HWIO_APSS_ALIAS_7_FFWD_STATUS_ADDR                                                (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x0000800c)
#define HWIO_APSS_ALIAS_7_FFWD_STATUS_RMSK                                                      0xff
#define HWIO_APSS_ALIAS_7_FFWD_STATUS_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_FFWD_STATUS_ADDR, HWIO_APSS_ALIAS_7_FFWD_STATUS_RMSK)
#define HWIO_APSS_ALIAS_7_FFWD_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_FFWD_STATUS_ADDR, m)
#define HWIO_APSS_ALIAS_7_FFWD_STATUS_FFWD_CNT_VAL_BMSK                                         0xff
#define HWIO_APSS_ALIAS_7_FFWD_STATUS_FFWD_CNT_VAL_SHFT                                          0x0

#define HWIO_APSS_ALIAS_7_BHS_SPARE_ADDR                                                  (APSS_ALIAS_7_APSS_CPU_IPM_REG_BASE      + 0x00008010)
#define HWIO_APSS_ALIAS_7_BHS_SPARE_RMSK                                                  0xffffffff
#define HWIO_APSS_ALIAS_7_BHS_SPARE_IN          \
        in_dword_masked(HWIO_APSS_ALIAS_7_BHS_SPARE_ADDR, HWIO_APSS_ALIAS_7_BHS_SPARE_RMSK)
#define HWIO_APSS_ALIAS_7_BHS_SPARE_INM(m)      \
        in_dword_masked(HWIO_APSS_ALIAS_7_BHS_SPARE_ADDR, m)
#define HWIO_APSS_ALIAS_7_BHS_SPARE_OUT(v)      \
        out_dword(HWIO_APSS_ALIAS_7_BHS_SPARE_ADDR,v)
#define HWIO_APSS_ALIAS_7_BHS_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_ALIAS_7_BHS_SPARE_ADDR,m,v,HWIO_APSS_ALIAS_7_BHS_SPARE_IN)
#define HWIO_APSS_ALIAS_7_BHS_SPARE_SPARE_BMSK                                            0xffffffc0
#define HWIO_APSS_ALIAS_7_BHS_SPARE_SPARE_SHFT                                                   0x6
#define HWIO_APSS_ALIAS_7_BHS_SPARE_BHS_STRENGTH_BMSK                                           0x3f
#define HWIO_APSS_ALIAS_7_BHS_SPARE_BHS_STRENGTH_SHFT                                            0x0


#endif /* __APSS_HANG_STATUS_HWIO_H__ */

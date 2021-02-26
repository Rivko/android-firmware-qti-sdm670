#ifndef __PMICARBCOREHWIO_H__
#define __PMICARBCOREHWIO_H__
/*
===========================================================================
*/
/**
  @file PmicArbCoreHwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r47.0.0_MTO]
 
  This file contains HWIO register definitions for the following modules:
    PMIC_ARB_CORE
    PMIC_ARB_CORE_REGISTERS
    PMIC_ARB_CORE_REGISTERS_OBS
    PMIC_ARB_MGPI
    QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG

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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/core/hal/hoya/hw/v5/PmicArbCoreHwio.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: PMIC_ARB_CORE
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_CORE_REG_BASE_OFFS                                      0x00040000

#define HWIO_PMIC_ARB_HW_VERSION_ADDR(x)                                 ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000000)
#define HWIO_PMIC_ARB_HW_VERSION_OFFS                                    (0x00000000)
#define HWIO_PMIC_ARB_HW_VERSION_RMSK                                    0xffffffff
#define HWIO_PMIC_ARB_HW_VERSION_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_HW_VERSION_ADDR(x), HWIO_PMIC_ARB_HW_VERSION_RMSK)
#define HWIO_PMIC_ARB_HW_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_HW_VERSION_ADDR(x), m)
#define HWIO_PMIC_ARB_HW_VERSION_MAJOR_BMSK                              0xf0000000
#define HWIO_PMIC_ARB_HW_VERSION_MAJOR_SHFT                                    0x1c
#define HWIO_PMIC_ARB_HW_VERSION_MINOR_BMSK                               0xfff0000
#define HWIO_PMIC_ARB_HW_VERSION_MINOR_SHFT                                    0x10
#define HWIO_PMIC_ARB_HW_VERSION_STEP_BMSK                                   0xffff
#define HWIO_PMIC_ARB_HW_VERSION_STEP_SHFT                                      0x0

#define HWIO_PMIC_ARB_FEATURES_ADDR(x)                                   ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000004)
#define HWIO_PMIC_ARB_FEATURES_OFFS                                      (0x00000004)
#define HWIO_PMIC_ARB_FEATURES_RMSK                                      0xffff03ff
#define HWIO_PMIC_ARB_FEATURES_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_FEATURES_ADDR(x), HWIO_PMIC_ARB_FEATURES_RMSK)
#define HWIO_PMIC_ARB_FEATURES_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_FEATURES_ADDR(x), m)
#define HWIO_PMIC_ARB_FEATURES_NUMBER_OF_PIC_ACC_ENABLES_BMSK            0xffc00000
#define HWIO_PMIC_ARB_FEATURES_NUMBER_OF_PIC_ACC_ENABLES_SHFT                  0x16
#define HWIO_PMIC_ARB_FEATURES_NUMBER_OF_PVC_PORTS_BMSK                    0x3f0000
#define HWIO_PMIC_ARB_FEATURES_NUMBER_OF_PVC_PORTS_SHFT                        0x10
#define HWIO_PMIC_ARB_FEATURES_NUMBER_OF_PMIC_PERIPHERALS_BMSK                0x3ff
#define HWIO_PMIC_ARB_FEATURES_NUMBER_OF_PMIC_PERIPHERALS_SHFT                  0x0

#define HWIO_PMIC_ARB_MISC_CONFIG_ADDR(x)                                ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000010)
#define HWIO_PMIC_ARB_MISC_CONFIG_OFFS                                   (0x00000010)
#define HWIO_PMIC_ARB_MISC_CONFIG_RMSK                                          0x7
#define HWIO_PMIC_ARB_MISC_CONFIG_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_MISC_CONFIG_ADDR(x), HWIO_PMIC_ARB_MISC_CONFIG_RMSK)
#define HWIO_PMIC_ARB_MISC_CONFIG_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_MISC_CONFIG_ADDR(x), m)
#define HWIO_PMIC_ARB_MISC_CONFIG_OUT(x, v)      \
        out_dword(HWIO_PMIC_ARB_MISC_CONFIG_ADDR(x),v)
#define HWIO_PMIC_ARB_MISC_CONFIG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_MISC_CONFIG_ADDR(x),m,v,HWIO_PMIC_ARB_MISC_CONFIG_IN(x))
#define HWIO_PMIC_ARB_MISC_CONFIG_TEST_BUS_EN_BMSK                              0x4
#define HWIO_PMIC_ARB_MISC_CONFIG_TEST_BUS_EN_SHFT                              0x2
#define HWIO_PMIC_ARB_MISC_CONFIG_CLK_GATE_EN_BMSK                              0x2
#define HWIO_PMIC_ARB_MISC_CONFIG_CLK_GATE_EN_SHFT                              0x1
#define HWIO_PMIC_ARB_MISC_CONFIG_DONE_IRQ_EN_BMSK                              0x1
#define HWIO_PMIC_ARB_MISC_CONFIG_DONE_IRQ_EN_SHFT                              0x0

#define HWIO_PMIC_ARB_PRIORITIESn_ADDR(base,n)                           ((base) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000100 + 0x4 * (n))
#define HWIO_PMIC_ARB_PRIORITIESn_OFFS(base,n)                           (0x00000100 + 0x4 * (n))
#define HWIO_PMIC_ARB_PRIORITIESn_RMSK                                          0xf
#define HWIO_PMIC_ARB_PRIORITIESn_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_PRIORITIESn_ADDR(base,n), HWIO_PMIC_ARB_PRIORITIESn_RMSK)
#define HWIO_PMIC_ARB_PRIORITIESn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_PRIORITIESn_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_PRIORITIESn_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_ARB_PRIORITIESn_ADDR(base,n),val)
#define HWIO_PMIC_ARB_PRIORITIESn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_PRIORITIESn_ADDR(base,n),mask,val,HWIO_PMIC_ARB_PRIORITIESn_INI(base,n))
#define HWIO_PMIC_ARB_PRIORITIESn_PORT_PRIORITY_BMSK                            0xf
#define HWIO_PMIC_ARB_PRIORITIESn_PORT_PRIORITY_SHFT                            0x0

#define HWIO_PMIC_ARB_PVC_INTF_CTL_ADDR(x)                               ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000200)
#define HWIO_PMIC_ARB_PVC_INTF_CTL_OFFS                                  (0x00000200)
#define HWIO_PMIC_ARB_PVC_INTF_CTL_RMSK                                         0x1
#define HWIO_PMIC_ARB_PVC_INTF_CTL_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_PVC_INTF_CTL_ADDR(x), HWIO_PMIC_ARB_PVC_INTF_CTL_RMSK)
#define HWIO_PMIC_ARB_PVC_INTF_CTL_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_PVC_INTF_CTL_ADDR(x), m)
#define HWIO_PMIC_ARB_PVC_INTF_CTL_OUT(x, v)      \
        out_dword(HWIO_PMIC_ARB_PVC_INTF_CTL_ADDR(x),v)
#define HWIO_PMIC_ARB_PVC_INTF_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_PVC_INTF_CTL_ADDR(x),m,v,HWIO_PMIC_ARB_PVC_INTF_CTL_IN(x))
#define HWIO_PMIC_ARB_PVC_INTF_CTL_PVC_INTF_EN_BMSK                             0x1
#define HWIO_PMIC_ARB_PVC_INTF_CTL_PVC_INTF_EN_SHFT                             0x0

#define HWIO_PMIC_ARB_PVC_INTF_STATUS_ADDR(x)                            ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000204)
#define HWIO_PMIC_ARB_PVC_INTF_STATUS_OFFS                               (0x00000204)
#define HWIO_PMIC_ARB_PVC_INTF_STATUS_RMSK                                      0x1
#define HWIO_PMIC_ARB_PVC_INTF_STATUS_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_PVC_INTF_STATUS_ADDR(x), HWIO_PMIC_ARB_PVC_INTF_STATUS_RMSK)
#define HWIO_PMIC_ARB_PVC_INTF_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_PVC_INTF_STATUS_ADDR(x), m)
#define HWIO_PMIC_ARB_PVC_INTF_STATUS_PVC_INTF_BUSY_BMSK                        0x1
#define HWIO_PMIC_ARB_PVC_INTF_STATUS_PVC_INTF_BUSY_SHFT                        0x0

#define HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR(base,n)                         ((base) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000208 + 0x4 * (n))
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_OFFS(base,n)                         (0x00000208 + 0x4 * (n))
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_RMSK                                        0x3
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR(base,n), HWIO_PMIC_ARB_PVC_PORTn_CTL_RMSK)
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR(base,n),val)
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR(base,n),mask,val,HWIO_PMIC_ARB_PVC_PORTn_CTL_INI(base,n))
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_SPMI_PRIORITY_BMSK                          0x2
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_SPMI_PRIORITY_SHFT                          0x1
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_PVC_PORT_EN_BMSK                            0x1
#define HWIO_PMIC_ARB_PVC_PORTn_CTL_PVC_PORT_EN_SHFT                            0x0

#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_ADDR(base,n)                      ((base) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000288 + 0x4 * (n))
#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_OFFS(base,n)                      (0x00000288 + 0x4 * (n))
#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_RMSK                                     0x3
#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_PVC_PORTn_STATUS_ADDR(base,n), HWIO_PMIC_ARB_PVC_PORTn_STATUS_RMSK)
#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_PVC_PORTn_STATUS_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_PVC_PORT_FAILURE_BMSK                    0x2
#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_PVC_PORT_FAILURE_SHFT                    0x1
#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_PVC_PORT_BUSY_BMSK                       0x1
#define HWIO_PMIC_ARB_PVC_PORTn_STATUS_PVC_PORT_BUSY_SHFT                       0x0

#define HWIO_PMIC_ARB_PVCn_ADDRm_ADDR(base,n,m)                          ((base) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000400 + 0x20 * (n) + 0x4 * (m))
#define HWIO_PMIC_ARB_PVCn_ADDRm_OFFS(base,n,m)                          (0x00000400 + 0x20 * (n) + 0x4 * (m))
#define HWIO_PMIC_ARB_PVCn_ADDRm_RMSK                                       0xfffff
#define HWIO_PMIC_ARB_PVCn_ADDRm_INI2(base,n,m)        \
        in_dword_masked(HWIO_PMIC_ARB_PVCn_ADDRm_ADDR(base,n,m), HWIO_PMIC_ARB_PVCn_ADDRm_RMSK)
#define HWIO_PMIC_ARB_PVCn_ADDRm_INMI2(base,n,m,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_PVCn_ADDRm_ADDR(base,n,m), mask)
#define HWIO_PMIC_ARB_PVCn_ADDRm_OUTI2(base,n,m,val)    \
        out_dword(HWIO_PMIC_ARB_PVCn_ADDRm_ADDR(base,n,m),val)
#define HWIO_PMIC_ARB_PVCn_ADDRm_OUTMI2(base,n,m,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_PVCn_ADDRm_ADDR(base,n,m),mask,val,HWIO_PMIC_ARB_PVCn_ADDRm_INI2(base,n,m))
#define HWIO_PMIC_ARB_PVCn_ADDRm_SID_BMSK                                   0xf0000
#define HWIO_PMIC_ARB_PVCn_ADDRm_SID_SHFT                                      0x10
#define HWIO_PMIC_ARB_PVCn_ADDRm_ADDRESS_BMSK                                0xffff
#define HWIO_PMIC_ARB_PVCn_ADDRm_ADDRESS_SHFT                                   0x0

#define HWIO_PMIC_ARB_REG_ADDRp_ADDR(base,p)                             ((base) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00000900 + 0x4 * (p))
#define HWIO_PMIC_ARB_REG_ADDRp_OFFS(base,p)                             (0x00000900 + 0x4 * (p))
#define HWIO_PMIC_ARB_REG_ADDRp_RMSK                                      0x10fff00
#define HWIO_PMIC_ARB_REG_ADDRp_INI(base,p)        \
        in_dword_masked(HWIO_PMIC_ARB_REG_ADDRp_ADDR(base,p), HWIO_PMIC_ARB_REG_ADDRp_RMSK)
#define HWIO_PMIC_ARB_REG_ADDRp_INMI(base,p,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_REG_ADDRp_ADDR(base,p), mask)
#define HWIO_PMIC_ARB_REG_ADDRp_OUTI(base,p,val)    \
        out_dword(HWIO_PMIC_ARB_REG_ADDRp_ADDR(base,p),val)
#define HWIO_PMIC_ARB_REG_ADDRp_OUTMI(base,p,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_REG_ADDRp_ADDR(base,p),mask,val,HWIO_PMIC_ARB_REG_ADDRp_INI(base,p))
#define HWIO_PMIC_ARB_REG_ADDRp_IRQ_OWN_BMSK                              0x1000000
#define HWIO_PMIC_ARB_REG_ADDRp_IRQ_OWN_SHFT                                   0x18
#define HWIO_PMIC_ARB_REG_ADDRp_SID_BMSK                                    0xf0000
#define HWIO_PMIC_ARB_REG_ADDRp_SID_SHFT                                       0x10
#define HWIO_PMIC_ARB_REG_ADDRp_ADDRESS_BMSK                                 0xff00
#define HWIO_PMIC_ARB_REG_ADDRp_ADDRESS_SHFT                                    0x8

#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_ADDR(x)                             ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00002200)
#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_OFFS                                (0x00002200)
#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_RMSK                                       0x1
#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_INTF_CTL_ADDR(x), HWIO_PMIC_ARB_VRM_S_INTF_CTL_RMSK)
#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_INTF_CTL_ADDR(x), m)
#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_OUT(x, v)      \
        out_dword(HWIO_PMIC_ARB_VRM_S_INTF_CTL_ADDR(x),v)
#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_VRM_S_INTF_CTL_ADDR(x),m,v,HWIO_PMIC_ARB_VRM_S_INTF_CTL_IN(x))
#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_VRM_S_INTF_EN_BMSK                         0x1
#define HWIO_PMIC_ARB_VRM_S_INTF_CTL_VRM_S_INTF_EN_SHFT                         0x0

#define HWIO_PMIC_ARB_VRM_S_INTF_STATUS_ADDR(x)                          ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00002204)
#define HWIO_PMIC_ARB_VRM_S_INTF_STATUS_OFFS                             (0x00002204)
#define HWIO_PMIC_ARB_VRM_S_INTF_STATUS_RMSK                                    0x1
#define HWIO_PMIC_ARB_VRM_S_INTF_STATUS_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_INTF_STATUS_ADDR(x), HWIO_PMIC_ARB_VRM_S_INTF_STATUS_RMSK)
#define HWIO_PMIC_ARB_VRM_S_INTF_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_INTF_STATUS_ADDR(x), m)
#define HWIO_PMIC_ARB_VRM_S_INTF_STATUS_VRM_S_INTF_BUSY_BMSK                    0x1
#define HWIO_PMIC_ARB_VRM_S_INTF_STATUS_VRM_S_INTF_BUSY_SHFT                    0x0

#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_ADDR(x)                             ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00002208)
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_OFFS                                (0x00002208)
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_RMSK                                       0x3
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_PORT_CTL_ADDR(x), HWIO_PMIC_ARB_VRM_S_PORT_CTL_RMSK)
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_PORT_CTL_ADDR(x), m)
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_OUT(x, v)      \
        out_dword(HWIO_PMIC_ARB_VRM_S_PORT_CTL_ADDR(x),v)
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_ARB_VRM_S_PORT_CTL_ADDR(x),m,v,HWIO_PMIC_ARB_VRM_S_PORT_CTL_IN(x))
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_SPMI_PRIORITY_BMSK                         0x2
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_SPMI_PRIORITY_SHFT                         0x1
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_VRM_S_PORT_EN_BMSK                         0x1
#define HWIO_PMIC_ARB_VRM_S_PORT_CTL_VRM_S_PORT_EN_SHFT                         0x0

#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_ADDR(x)                          ((x) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x0000220c)
#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_OFFS                             (0x0000220c)
#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_RMSK                                    0x3
#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_IN(x)      \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_PORT_STATUS_ADDR(x), HWIO_PMIC_ARB_VRM_S_PORT_STATUS_RMSK)
#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_PORT_STATUS_ADDR(x), m)
#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_VRM_S_PORT_FAILURE_BMSK                 0x2
#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_VRM_S_PORT_FAILURE_SHFT                 0x1
#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_VRM_S_PORT_BUSY_BMSK                    0x1
#define HWIO_PMIC_ARB_VRM_S_PORT_STATUS_VRM_S_PORT_BUSY_SHFT                    0x0

#define HWIO_PMIC_ARB_VRM_S_ADDRw_ADDR(base,w)                           ((base) + PMIC_ARB_CORE_REG_BASE_OFFS + 0x00002300 + 0x4 * (w))
#define HWIO_PMIC_ARB_VRM_S_ADDRw_OFFS(base,w)                           (0x00002300 + 0x4 * (w))
#define HWIO_PMIC_ARB_VRM_S_ADDRw_RMSK                                      0xfff00
#define HWIO_PMIC_ARB_VRM_S_ADDRw_INI(base,w)        \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_ADDRw_ADDR(base,w), HWIO_PMIC_ARB_VRM_S_ADDRw_RMSK)
#define HWIO_PMIC_ARB_VRM_S_ADDRw_INMI(base,w,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_VRM_S_ADDRw_ADDR(base,w), mask)
#define HWIO_PMIC_ARB_VRM_S_ADDRw_OUTI(base,w,val)    \
        out_dword(HWIO_PMIC_ARB_VRM_S_ADDRw_ADDR(base,w),val)
#define HWIO_PMIC_ARB_VRM_S_ADDRw_OUTMI(base,w,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_VRM_S_ADDRw_ADDR(base,w),mask,val,HWIO_PMIC_ARB_VRM_S_ADDRw_INI(base,w))
#define HWIO_PMIC_ARB_VRM_S_ADDRw_SID_BMSK                                  0xf0000
#define HWIO_PMIC_ARB_VRM_S_ADDRw_SID_SHFT                                     0x10
#define HWIO_PMIC_ARB_VRM_S_ADDRw_PPID_BMSK                                  0xff00
#define HWIO_PMIC_ARB_VRM_S_ADDRw_PPID_SHFT                                     0x8

/*----------------------------------------------------------------------------
 * MODULE: PMIC_ARB_CORE_REGISTERS
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS                                          0x00200000

#define HWIO_PMIC_ARB_CHNLn_CMD_ADDR(base,n)                                           ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000000 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_CMD_OFFS(base,n)                                           (0x00000000 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_CMD_RMSK                                                   0xfc000ff7
#define HWIO_PMIC_ARB_CHNLn_CMD_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_CMD_ADDR(base,n), HWIO_PMIC_ARB_CHNLn_CMD_RMSK)
#define HWIO_PMIC_ARB_CHNLn_CMD_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_CMD_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_CHNLn_CMD_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_ARB_CHNLn_CMD_ADDR(base,n),val)
#define HWIO_PMIC_ARB_CHNLn_CMD_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_CHNLn_CMD_ADDR(base,n),mask,val,HWIO_PMIC_ARB_CHNLn_CMD_INI(base,n))
#define HWIO_PMIC_ARB_CHNLn_CMD_OPCODE_BMSK                                            0xf8000000
#define HWIO_PMIC_ARB_CHNLn_CMD_OPCODE_SHFT                                                  0x1b
#define HWIO_PMIC_ARB_CHNLn_CMD_PRIORITY_BMSK                                           0x4000000
#define HWIO_PMIC_ARB_CHNLn_CMD_PRIORITY_SHFT                                                0x1a
#define HWIO_PMIC_ARB_CHNLn_CMD_ADDRESS_OFFSET_BMSK                                         0xff0
#define HWIO_PMIC_ARB_CHNLn_CMD_ADDRESS_OFFSET_SHFT                                           0x4
#define HWIO_PMIC_ARB_CHNLn_CMD_BYTE_CNT_BMSK                                                 0x7
#define HWIO_PMIC_ARB_CHNLn_CMD_BYTE_CNT_SHFT                                                 0x0

#define HWIO_PMIC_ARB_CHNLn_CONFIG_ADDR(base,n)                                        ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000004 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_CONFIG_OFFS(base,n)                                        (0x00000004 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_CONFIG_RMSK                                                       0x1
#define HWIO_PMIC_ARB_CHNLn_CONFIG_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_CONFIG_ADDR(base,n), HWIO_PMIC_ARB_CHNLn_CONFIG_RMSK)
#define HWIO_PMIC_ARB_CHNLn_CONFIG_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_CONFIG_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_CHNLn_CONFIG_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_ARB_CHNLn_CONFIG_ADDR(base,n),val)
#define HWIO_PMIC_ARB_CHNLn_CONFIG_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_CHNLn_CONFIG_ADDR(base,n),mask,val,HWIO_PMIC_ARB_CHNLn_CONFIG_INI(base,n))
#define HWIO_PMIC_ARB_CHNLn_CONFIG_IRQ_EN_BMSK                                                0x1
#define HWIO_PMIC_ARB_CHNLn_CONFIG_IRQ_EN_SHFT                                                0x0

#define HWIO_PMIC_ARB_CHNLn_STATUS_ADDR(base,n)                                        ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000008 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_STATUS_OFFS(base,n)                                        (0x00000008 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_STATUS_RMSK                                                       0xf
#define HWIO_PMIC_ARB_CHNLn_STATUS_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_STATUS_ADDR(base,n), HWIO_PMIC_ARB_CHNLn_STATUS_RMSK)
#define HWIO_PMIC_ARB_CHNLn_STATUS_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_STATUS_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_CHNLn_STATUS_DROPPED_BMSK                                               0x8
#define HWIO_PMIC_ARB_CHNLn_STATUS_DROPPED_SHFT                                               0x3
#define HWIO_PMIC_ARB_CHNLn_STATUS_DENIED_BMSK                                                0x4
#define HWIO_PMIC_ARB_CHNLn_STATUS_DENIED_SHFT                                                0x2
#define HWIO_PMIC_ARB_CHNLn_STATUS_FAILURE_BMSK                                               0x2
#define HWIO_PMIC_ARB_CHNLn_STATUS_FAILURE_SHFT                                               0x1
#define HWIO_PMIC_ARB_CHNLn_STATUS_DONE_BMSK                                                  0x1
#define HWIO_PMIC_ARB_CHNLn_STATUS_DONE_SHFT                                                  0x0

#define HWIO_PMIC_ARB_CHNLn_WDATA0_ADDR(base,n)                                        ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000010 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_WDATA0_OFFS(base,n)                                        (0x00000010 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_WDATA0_RMSK                                                0xffffffff
#define HWIO_PMIC_ARB_CHNLn_WDATA0_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_WDATA0_ADDR(base,n), HWIO_PMIC_ARB_CHNLn_WDATA0_RMSK)
#define HWIO_PMIC_ARB_CHNLn_WDATA0_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_WDATA0_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_CHNLn_WDATA0_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_ARB_CHNLn_WDATA0_ADDR(base,n),val)
#define HWIO_PMIC_ARB_CHNLn_WDATA0_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_CHNLn_WDATA0_ADDR(base,n),mask,val,HWIO_PMIC_ARB_CHNLn_WDATA0_INI(base,n))
#define HWIO_PMIC_ARB_CHNLn_WDATA0_DATA_BMSK                                           0xffffffff
#define HWIO_PMIC_ARB_CHNLn_WDATA0_DATA_SHFT                                                  0x0

#define HWIO_PMIC_ARB_CHNLn_WDATA1_ADDR(base,n)                                        ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000014 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_WDATA1_OFFS(base,n)                                        (0x00000014 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_WDATA1_RMSK                                                0xffffffff
#define HWIO_PMIC_ARB_CHNLn_WDATA1_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_WDATA1_ADDR(base,n), HWIO_PMIC_ARB_CHNLn_WDATA1_RMSK)
#define HWIO_PMIC_ARB_CHNLn_WDATA1_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_WDATA1_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_CHNLn_WDATA1_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_ARB_CHNLn_WDATA1_ADDR(base,n),val)
#define HWIO_PMIC_ARB_CHNLn_WDATA1_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_CHNLn_WDATA1_ADDR(base,n),mask,val,HWIO_PMIC_ARB_CHNLn_WDATA1_INI(base,n))
#define HWIO_PMIC_ARB_CHNLn_WDATA1_DATA_BMSK                                           0xffffffff
#define HWIO_PMIC_ARB_CHNLn_WDATA1_DATA_SHFT                                                  0x0

#define HWIO_PMIC_ARB_CHNLn_RDATA0_ADDR(base,n)                                        ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000018 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_RDATA0_OFFS(base,n)                                        (0x00000018 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_RDATA0_RMSK                                                0xffffffff
#define HWIO_PMIC_ARB_CHNLn_RDATA0_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_RDATA0_ADDR(base,n), HWIO_PMIC_ARB_CHNLn_RDATA0_RMSK)
#define HWIO_PMIC_ARB_CHNLn_RDATA0_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_RDATA0_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_CHNLn_RDATA0_DATA_BMSK                                           0xffffffff
#define HWIO_PMIC_ARB_CHNLn_RDATA0_DATA_SHFT                                                  0x0

#define HWIO_PMIC_ARB_CHNLn_RDATA1_ADDR(base,n)                                        ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x0000001c + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_RDATA1_OFFS(base,n)                                        (0x0000001c + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_RDATA1_RMSK                                                0xffffffff
#define HWIO_PMIC_ARB_CHNLn_RDATA1_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_RDATA1_ADDR(base,n), HWIO_PMIC_ARB_CHNLn_RDATA1_RMSK)
#define HWIO_PMIC_ARB_CHNLn_RDATA1_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_RDATA1_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_CHNLn_RDATA1_DATA_BMSK                                           0xffffffff
#define HWIO_PMIC_ARB_CHNLn_RDATA1_DATA_SHFT                                                  0x0

#define HWIO_PMIC_ARB_CHNLn_RDATA2_ADDR(base,n)                                        ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000020 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_RDATA2_OFFS(base,n)                                        (0x00000020 + 0x10000 * (n))
#define HWIO_PMIC_ARB_CHNLn_RDATA2_RMSK                                                    0xffff
#define HWIO_PMIC_ARB_CHNLn_RDATA2_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_RDATA2_ADDR(base,n), HWIO_PMIC_ARB_CHNLn_RDATA2_RMSK)
#define HWIO_PMIC_ARB_CHNLn_RDATA2_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_CHNLn_RDATA2_ADDR(base,n), mask)
#define HWIO_PMIC_ARB_CHNLn_RDATA2_DATA_BMSK                                               0xffff
#define HWIO_PMIC_ARB_CHNLn_RDATA2_DATA_SHFT                                                  0x0

#define HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(base,n)                                         ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000100 + 0x10000 * (n))
#define HWIO_SPMI_PIC_ACC_ENABLEn_OFFS(base,n)                                         (0x00000100 + 0x10000 * (n))
#define HWIO_SPMI_PIC_ACC_ENABLEn_RMSK                                                        0x1
#define HWIO_SPMI_PIC_ACC_ENABLEn_INI(base,n)        \
        in_dword_masked(HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(base,n), HWIO_SPMI_PIC_ACC_ENABLEn_RMSK)
#define HWIO_SPMI_PIC_ACC_ENABLEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(base,n), mask)
#define HWIO_SPMI_PIC_ACC_ENABLEn_OUTI(base,n,val)    \
        out_dword(HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(base,n),val)
#define HWIO_SPMI_PIC_ACC_ENABLEn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_SPMI_PIC_ACC_ENABLEn_ADDR(base,n),mask,val,HWIO_SPMI_PIC_ACC_ENABLEn_INI(base,n))
#define HWIO_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE_BMSK                                         0x1
#define HWIO_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE_SHFT                                         0x0

#define HWIO_SPMI_PIC_IRQ_STATUSn_ADDR(base,n)                                         ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000104 + 0x10000 * (n))
#define HWIO_SPMI_PIC_IRQ_STATUSn_OFFS(base,n)                                         (0x00000104 + 0x10000 * (n))
#define HWIO_SPMI_PIC_IRQ_STATUSn_RMSK                                                       0xff
#define HWIO_SPMI_PIC_IRQ_STATUSn_INI(base,n)        \
        in_dword_masked(HWIO_SPMI_PIC_IRQ_STATUSn_ADDR(base,n), HWIO_SPMI_PIC_IRQ_STATUSn_RMSK)
#define HWIO_SPMI_PIC_IRQ_STATUSn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SPMI_PIC_IRQ_STATUSn_ADDR(base,n), mask)
#define HWIO_SPMI_PIC_IRQ_STATUSn_INT_STATUS_BMSK                                            0xff
#define HWIO_SPMI_PIC_IRQ_STATUSn_INT_STATUS_SHFT                                             0x0

#define HWIO_SPMI_PIC_IRQ_CLEARn_ADDR(base,n)                                          ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x00000108 + 0x10000 * (n))
#define HWIO_SPMI_PIC_IRQ_CLEARn_OFFS(base,n)                                          (0x00000108 + 0x10000 * (n))
#define HWIO_SPMI_PIC_IRQ_CLEARn_RMSK                                                        0xff
#define HWIO_SPMI_PIC_IRQ_CLEARn_OUTI(base,n,val)    \
        out_dword(HWIO_SPMI_PIC_IRQ_CLEARn_ADDR(base,n),val)
#define HWIO_SPMI_PIC_IRQ_CLEARn_INT_CLEAR_BMSK                                              0xff
#define HWIO_SPMI_PIC_IRQ_CLEARn_INT_CLEAR_SHFT                                               0x0

#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_ADDR(base,n)                                  ((base) + PMIC_ARB_CORE_REGISTERS_REG_BASE_OFFS + 0x0000010c + 0x10000 * (n))
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_OFFS(base,n)                                  (0x0000010c + 0x10000 * (n))
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_RMSK                                               0xfff
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_INI(base,n)        \
        in_dword_masked(HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_ADDR(base,n), HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_RMSK)
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_ADDR(base,n), mask)
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_ARBITER_DISCONNECTED_BMSK                          0x800
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_ARBITER_DISCONNECTED_SHFT                            0xb
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_ARBITER_CONNECTED_BMSK                             0x400
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_ARBITER_CONNECTED_SHFT                               0xa
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_PERIPH_IRQ_LOST_BMSK                               0x200
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_PERIPH_IRQ_LOST_SHFT                                 0x9
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_UNEXPECTED_SSC_BMSK                                0x100
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_UNEXPECTED_SSC_SHFT                                  0x8
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                0x80
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                 0x7
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                 0x40
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                  0x6
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_FALSE_MASTER_ARBITRATION_WIN_BMSK                   0x20
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_FALSE_MASTER_ARBITRATION_WIN_SHFT                    0x5
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_FALSE_BUS_REQ_BMSK                                  0x10
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_FALSE_BUS_REQ_SHFT                                   0x4
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_UNSUPPORTED_COMMAND_BMSK                             0x8
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_UNSUPPORTED_COMMAND_SHFT                             0x3
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                    0x4
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                    0x2
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                    0x2
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                    0x1
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                   0x1
#define HWIO_SWCHAN_PROTOCOL_IRQ_STATUSn_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                   0x0

/*----------------------------------------------------------------------------
 * MODULE: PMIC_ARB_CORE_REGISTERS_OBS
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_CORE_REGISTERS_OBS_REG_BASE_OFFS                                        0x02200000

#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDR(base,q,n)                                      ((base) + PMIC_ARB_CORE_REGISTERS_OBS_REG_BASE_OFFS + 0x00000000 + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_OFFS(base,q,n)                                      (0x00000000 + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_RMSK                                                0xfc000ff7
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_INI2(base,q,n)        \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDR(base,q,n), HWIO_PMIC_ARB_OBSq_CHNLn_CMD_RMSK)
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_INMI2(base,q,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDR(base,q,n), mask)
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_OUTI2(base,q,n,val)    \
        out_dword(HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDR(base,q,n),val)
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_OUTMI2(base,q,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDR(base,q,n),mask,val,HWIO_PMIC_ARB_OBSq_CHNLn_CMD_INI2(base,q,n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_OPCODE_BMSK                                         0xf8000000
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_OPCODE_SHFT                                               0x1b
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_PRIORITY_BMSK                                        0x4000000
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_PRIORITY_SHFT                                             0x1a
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDRESS_BMSK                                             0xff0
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDRESS_SHFT                                               0x4
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_BYTE_CNT_BMSK                                              0x7
#define HWIO_PMIC_ARB_OBSq_CHNLn_CMD_BYTE_CNT_SHFT                                              0x0

#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_ADDR(base,q,n)                                   ((base) + PMIC_ARB_CORE_REGISTERS_OBS_REG_BASE_OFFS + 0x00000004 + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_OFFS(base,q,n)                                   (0x00000004 + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_RMSK                                                    0x1
#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_INI2(base,q,n)        \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_ADDR(base,q,n), HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_RMSK)
#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_INMI2(base,q,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_ADDR(base,q,n), mask)
#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_OUTI2(base,q,n,val)    \
        out_dword(HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_ADDR(base,q,n),val)
#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_OUTMI2(base,q,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_ADDR(base,q,n),mask,val,HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_INI2(base,q,n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_IRQ_EN_BMSK                                             0x1
#define HWIO_PMIC_ARB_OBSq_CHNLn_CONFIG_IRQ_EN_SHFT                                             0x0

#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR(base,q,n)                                   ((base) + PMIC_ARB_CORE_REGISTERS_OBS_REG_BASE_OFFS + 0x00000008 + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_OFFS(base,q,n)                                   (0x00000008 + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_RMSK                                                    0xf
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_INI2(base,q,n)        \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR(base,q,n), HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_RMSK)
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_INMI2(base,q,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR(base,q,n), mask)
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DROPPED_BMSK                                            0x8
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DROPPED_SHFT                                            0x3
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DENIED_BMSK                                             0x4
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DENIED_SHFT                                             0x2
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_FAILURE_BMSK                                            0x2
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_FAILURE_SHFT                                            0x1
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DONE_BMSK                                               0x1
#define HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DONE_SHFT                                               0x0

#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_ADDR(base,q,n)                                   ((base) + PMIC_ARB_CORE_REGISTERS_OBS_REG_BASE_OFFS + 0x00000018 + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_OFFS(base,q,n)                                   (0x00000018 + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_RMSK                                             0xffffffff
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_INI2(base,q,n)        \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_ADDR(base,q,n), HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_RMSK)
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_INMI2(base,q,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_ADDR(base,q,n), mask)
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_DATA_BMSK                                        0xffffffff
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_DATA_SHFT                                               0x0

#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_ADDR(base,q,n)                                   ((base) + PMIC_ARB_CORE_REGISTERS_OBS_REG_BASE_OFFS + 0x0000001c + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_OFFS(base,q,n)                                   (0x0000001c + 0x10000 * (q) + 0x80 * (n))
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_RMSK                                             0xffffffff
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_INI2(base,q,n)        \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_ADDR(base,q,n), HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_RMSK)
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_INMI2(base,q,n,mask)    \
        in_dword_masked(HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_ADDR(base,q,n), mask)
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_DATA_BMSK                                        0xffffffff
#define HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_DATA_SHFT                                               0x0

#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_ADDR(base,q,n)                                ((base) + PMIC_ARB_CORE_REGISTERS_OBS_REG_BASE_OFFS + 0x0000000c + 0x10000 * (q) + 0x80 * (n))
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_OFFS(base,q,n)                                (0x0000000c + 0x10000 * (q) + 0x80 * (n))
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_RMSK                                               0xfff
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_INI2(base,q,n)        \
        in_dword_masked(HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_ADDR(base,q,n), HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_RMSK)
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_INMI2(base,q,n,mask)    \
        in_dword_masked(HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_ADDR(base,q,n), mask)
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_ARBITER_DISCONNECTED_BMSK                          0x800
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_ARBITER_DISCONNECTED_SHFT                            0xb
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_ARBITER_CONNECTED_BMSK                             0x400
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_ARBITER_CONNECTED_SHFT                               0xa
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_PERIPH_IRQ_LOST_BMSK                               0x200
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_PERIPH_IRQ_LOST_SHFT                                 0x9
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_UNEXPECTED_SSC_BMSK                                0x100
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_UNEXPECTED_SSC_SHFT                                  0x8
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_NO_RESPONSE_DATA_FRAME_DETECTED_BMSK                0x80
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_NO_RESPONSE_DATA_FRAME_DETECTED_SHFT                 0x7
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_NO_RESPONSE_CMD_FRAME_DETECTED_BMSK                 0x40
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_NO_RESPONSE_CMD_FRAME_DETECTED_SHFT                  0x6
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_FALSE_MASTER_ARBITRATION_WIN_BMSK                   0x20
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_FALSE_MASTER_ARBITRATION_WIN_SHFT                    0x5
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_FALSE_BUS_REQ_BMSK                                  0x10
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_FALSE_BUS_REQ_SHFT                                   0x4
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_UNSUPPORTED_COMMAND_BMSK                             0x8
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_UNSUPPORTED_COMMAND_SHFT                             0x3
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_DATA_ADDR_FRAME_PARITY_ERROR_BMSK                    0x4
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_DATA_ADDR_FRAME_PARITY_ERROR_SHFT                    0x2
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_SLAVE_CMD_FRAME_PARITY_ERROR_BMSK                    0x2
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_SLAVE_CMD_FRAME_PARITY_ERROR_SHFT                    0x1
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_MASTER_CMD_FRAME_PARITY_ERROR_BMSK                   0x1
#define HWIO_OBSCHANq_PROTOCOL_IRQ_STATUSn_MASTER_CMD_FRAME_PARITY_ERROR_SHFT                   0x0

/*----------------------------------------------------------------------------
 * MODULE: PMIC_ARB_MGPI
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_MGPI_REG_BASE_OFFS                                   0x00060000

#define HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR(base,n)                      ((base) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x00002000 + 0x8 * (n))
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_OFFS(base,n)                      (0x00002000 + 0x8 * (n))
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_RMSK                               0xfff0fff
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR(base,n), HWIO_PMIC_MGPI_TRIG_CONFIGn_RMSK)
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR(base,n), mask)
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR(base,n),val)
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR(base,n),mask,val,HWIO_PMIC_MGPI_TRIG_CONFIGn_INI(base,n))
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR_IDX_NEG_BMSK                  0xf000000
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR_IDX_NEG_SHFT                       0x18
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_DATA_NEG_BMSK                       0xff0000
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_DATA_NEG_SHFT                           0x10
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR_IDX_POS_BMSK                      0xf00
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_ADDR_IDX_POS_SHFT                        0x8
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_DATA_POS_BMSK                           0xff
#define HWIO_PMIC_MGPI_TRIG_CONFIGn_DATA_POS_SHFT                            0x0

#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_ADDR(base,n)                      ((base) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x00002004 + 0x8 * (n))
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_OFFS(base,n)                      (0x00002004 + 0x8 * (n))
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_RMSK                               0x1f33
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_ADDR(base,n), HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_RMSK)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_ADDR(base,n), mask)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_ADDR(base,n),val)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_ADDR(base,n),mask,val,HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_INI(base,n))
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_NEXT_TRIGGER_BMSK                  0x1f00
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_NEXT_TRIGGER_SHFT                     0x8
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_INTERRUPT_EN_BMSK                    0x20
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_INTERRUPT_EN_SHFT                     0x5
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_DUAL_EDGE_ERR_EN_BMSK                0x10
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_DUAL_EDGE_ERR_EN_SHFT                 0x4
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_MGPI_NEG_EN_BMSK                      0x2
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_MGPI_NEG_EN_SHFT                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_MGPI_POS_EN_BMSK                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_ENn_MGPI_POS_EN_SHFT                      0x0

#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_ADDR(x)                        ((x) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x00002004)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_OFFS                           (0x00002004)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_RMSK                               0x1f33
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_IN(x)      \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_ADDR(x), HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_RMSK)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_ADDR(x), m)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_OUT(x, v)      \
        out_dword(HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_ADDR(x),v)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_ADDR(x),m,v,HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_IN(x))
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_NEXT_TRIGGER_BMSK                  0x1f00
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_NEXT_TRIGGER_SHFT                     0x8
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_INTERRUPT_EN_BMSK                    0x20
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_INTERRUPT_EN_SHFT                     0x5
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_DUAL_EDGE_ERR_EN_BMSK                0x10
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_DUAL_EDGE_ERR_EN_SHFT                 0x4
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_MGPI_NEG_EN_BMSK                      0x2
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_MGPI_NEG_EN_SHFT                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_MGPI_POS_EN_BMSK                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN0_MGPI_POS_EN_SHFT                      0x0

#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_ADDR(x)                        ((x) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x0000200c)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_OFFS                           (0x0000200c)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_RMSK                               0x1f33
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_IN(x)      \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_ADDR(x), HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_RMSK)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_ADDR(x), m)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_OUT(x, v)      \
        out_dword(HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_ADDR(x),v)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_ADDR(x),m,v,HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_IN(x))
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_NEXT_TRIGGER_BMSK                  0x1f00
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_NEXT_TRIGGER_SHFT                     0x8
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_INTERRUPT_EN_BMSK                    0x20
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_INTERRUPT_EN_SHFT                     0x5
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_DUAL_EDGE_ERR_EN_BMSK                0x10
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_DUAL_EDGE_ERR_EN_SHFT                 0x4
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_MGPI_NEG_EN_BMSK                      0x2
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_MGPI_NEG_EN_SHFT                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_MGPI_POS_EN_BMSK                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN1_MGPI_POS_EN_SHFT                      0x0

#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_ADDR(x)                        ((x) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x00002014)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_OFFS                           (0x00002014)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_RMSK                               0x1f33
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_IN(x)      \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_ADDR(x), HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_RMSK)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_ADDR(x), m)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_OUT(x, v)      \
        out_dword(HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_ADDR(x),v)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_ADDR(x),m,v,HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_IN(x))
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_NEXT_TRIGGER_BMSK                  0x1f00
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_NEXT_TRIGGER_SHFT                     0x8
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_INTERRUPT_EN_BMSK                    0x20
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_INTERRUPT_EN_SHFT                     0x5
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_DUAL_EDGE_ERR_EN_BMSK                0x10
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_DUAL_EDGE_ERR_EN_SHFT                 0x4
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_MGPI_NEG_EN_BMSK                      0x2
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_MGPI_NEG_EN_SHFT                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_MGPI_POS_EN_BMSK                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN2_MGPI_POS_EN_SHFT                      0x0

#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_ADDR(x)                        ((x) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x0000201c)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_OFFS                           (0x0000201c)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_RMSK                               0x1f33
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_IN(x)      \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_ADDR(x), HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_RMSK)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_ADDR(x), m)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_OUT(x, v)      \
        out_dword(HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_ADDR(x),v)
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_ADDR(x),m,v,HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_IN(x))
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_NEXT_TRIGGER_BMSK                  0x1f00
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_NEXT_TRIGGER_SHFT                     0x8
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_INTERRUPT_EN_BMSK                    0x20
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_INTERRUPT_EN_SHFT                     0x5
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_DUAL_EDGE_ERR_EN_BMSK                0x10
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_DUAL_EDGE_ERR_EN_SHFT                 0x4
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_MGPI_NEG_EN_BMSK                      0x2
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_MGPI_NEG_EN_SHFT                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_MGPI_POS_EN_BMSK                      0x1
#define HWIO_PMIC_MGPI_TRIG_CONFIG_EN3_MGPI_POS_EN_SHFT                      0x0

#define HWIO_PMIC_MGPI_MASTER_CSR_ADDR(x)                             ((x) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x00002100)
#define HWIO_PMIC_MGPI_MASTER_CSR_OFFS                                (0x00002100)
#define HWIO_PMIC_MGPI_MASTER_CSR_RMSK                                  0x1f1f73
#define HWIO_PMIC_MGPI_MASTER_CSR_IN(x)      \
        in_dword_masked(HWIO_PMIC_MGPI_MASTER_CSR_ADDR(x), HWIO_PMIC_MGPI_MASTER_CSR_RMSK)
#define HWIO_PMIC_MGPI_MASTER_CSR_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_MGPI_MASTER_CSR_ADDR(x), m)
#define HWIO_PMIC_MGPI_MASTER_CSR_OUT(x, v)      \
        out_dword(HWIO_PMIC_MGPI_MASTER_CSR_ADDR(x),v)
#define HWIO_PMIC_MGPI_MASTER_CSR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_MASTER_CSR_ADDR(x),m,v,HWIO_PMIC_MGPI_MASTER_CSR_IN(x))
#define HWIO_PMIC_MGPI_MASTER_CSR_TRIG_CUR_BMSK                         0x1f0000
#define HWIO_PMIC_MGPI_MASTER_CSR_TRIG_CUR_SHFT                             0x10
#define HWIO_PMIC_MGPI_MASTER_CSR_TRIG_START_BMSK                         0x1f00
#define HWIO_PMIC_MGPI_MASTER_CSR_TRIG_START_SHFT                            0x8
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_IRQ_BMSK                             0x40
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_IRQ_SHFT                              0x6
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_ERROR_BMSK                           0x20
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_ERROR_SHFT                            0x5
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_ACTIVE_BMSK                          0x10
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_ACTIVE_SHFT                           0x4
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_RESET_BMSK                            0x2
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_RESET_SHFT                            0x1
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_ENABLE_BMSK                           0x1
#define HWIO_PMIC_MGPI_MASTER_CSR_MGPI_ENABLE_SHFT                           0x0

#define HWIO_PMIC_MGPI_HW_VERSION_ADDR(x)                             ((x) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x00002104)
#define HWIO_PMIC_MGPI_HW_VERSION_OFFS                                (0x00002104)
#define HWIO_PMIC_MGPI_HW_VERSION_RMSK                                0xffffffff
#define HWIO_PMIC_MGPI_HW_VERSION_IN(x)      \
        in_dword_masked(HWIO_PMIC_MGPI_HW_VERSION_ADDR(x), HWIO_PMIC_MGPI_HW_VERSION_RMSK)
#define HWIO_PMIC_MGPI_HW_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_MGPI_HW_VERSION_ADDR(x), m)
#define HWIO_PMIC_MGPI_HW_VERSION_MAJOR_BMSK                          0xf0000000
#define HWIO_PMIC_MGPI_HW_VERSION_MAJOR_SHFT                                0x1c
#define HWIO_PMIC_MGPI_HW_VERSION_MINOR_BMSK                           0xfff0000
#define HWIO_PMIC_MGPI_HW_VERSION_MINOR_SHFT                                0x10
#define HWIO_PMIC_MGPI_HW_VERSION_STEP_BMSK                               0xffff
#define HWIO_PMIC_MGPI_HW_VERSION_STEP_SHFT                                  0x0

#define HWIO_PMIC_MGPI_RESERVED_ADDR(x)                               ((x) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x00002108)
#define HWIO_PMIC_MGPI_RESERVED_OFFS                                  (0x00002108)
#define HWIO_PMIC_MGPI_RESERVED_RMSK                                  0xffffffff
#define HWIO_PMIC_MGPI_RESERVED_IN(x)      \
        in_dword_masked(HWIO_PMIC_MGPI_RESERVED_ADDR(x), HWIO_PMIC_MGPI_RESERVED_RMSK)
#define HWIO_PMIC_MGPI_RESERVED_INM(x, m)      \
        in_dword_masked(HWIO_PMIC_MGPI_RESERVED_ADDR(x), m)
#define HWIO_PMIC_MGPI_RESERVED_OUT(x, v)      \
        out_dword(HWIO_PMIC_MGPI_RESERVED_ADDR(x),v)
#define HWIO_PMIC_MGPI_RESERVED_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_RESERVED_ADDR(x),m,v,HWIO_PMIC_MGPI_RESERVED_IN(x))
#define HWIO_PMIC_MGPI_RESERVED_SPARE_31_0_BMSK                       0xffffffff
#define HWIO_PMIC_MGPI_RESERVED_SPARE_31_0_SHFT                              0x0

#define HWIO_PMIC_MGPI_TRIG_STATUSn_ADDR(base,n)                      ((base) + PMIC_ARB_MGPI_REG_BASE_OFFS + 0x00010000 + 0x10000 * (n))
#define HWIO_PMIC_MGPI_TRIG_STATUSn_OFFS(base,n)                      (0x00010000 + 0x10000 * (n))
#define HWIO_PMIC_MGPI_TRIG_STATUSn_RMSK                               0x3fb0000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_INI(base,n)        \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_STATUSn_ADDR(base,n), HWIO_PMIC_MGPI_TRIG_STATUSn_RMSK)
#define HWIO_PMIC_MGPI_TRIG_STATUSn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_PMIC_MGPI_TRIG_STATUSn_ADDR(base,n), mask)
#define HWIO_PMIC_MGPI_TRIG_STATUSn_OUTI(base,n,val)    \
        out_dword(HWIO_PMIC_MGPI_TRIG_STATUSn_ADDR(base,n),val)
#define HWIO_PMIC_MGPI_TRIG_STATUSn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_PMIC_MGPI_TRIG_STATUSn_ADDR(base,n),mask,val,HWIO_PMIC_MGPI_TRIG_STATUSn_INI(base,n))
#define HWIO_PMIC_MGPI_TRIG_STATUSn_SUCCESS_NEG_BMSK                   0x2000000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_SUCCESS_NEG_SHFT                        0x19
#define HWIO_PMIC_MGPI_TRIG_STATUSn_SUCCESS_POS_BMSK                   0x1000000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_SUCCESS_POS_SHFT                        0x18
#define HWIO_PMIC_MGPI_TRIG_STATUSn_ERROR_WRT_BMSK                      0x800000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_ERROR_WRT_SHFT                          0x17
#define HWIO_PMIC_MGPI_TRIG_STATUSn_OVERFLOW_ERROR_NEG_BMSK             0x400000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_OVERFLOW_ERROR_NEG_SHFT                 0x16
#define HWIO_PMIC_MGPI_TRIG_STATUSn_OVERFLOW_ERROR_POS_BMSK             0x200000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_OVERFLOW_ERROR_POS_SHFT                 0x15
#define HWIO_PMIC_MGPI_TRIG_STATUSn_DUAL_EDGE_ERROR_BMSK                0x100000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_DUAL_EDGE_ERROR_SHFT                    0x14
#define HWIO_PMIC_MGPI_TRIG_STATUSn_EVENT_WRT_BMSK                       0x80000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_EVENT_WRT_SHFT                          0x13
#define HWIO_PMIC_MGPI_TRIG_STATUSn_NEG_EVENT_FLAG_BMSK                  0x20000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_NEG_EVENT_FLAG_SHFT                     0x11
#define HWIO_PMIC_MGPI_TRIG_STATUSn_POS_EVENT_FLAG_BMSK                  0x10000
#define HWIO_PMIC_MGPI_TRIG_STATUSn_POS_EVENT_FLAG_SHFT                     0x10

/*----------------------------------------------------------------------------
 * MODULE: QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG
 *--------------------------------------------------------------------------*/

#define QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS                      0x00000000

#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_ADDR(x)                 ((x) + QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS + 0x00000000)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_OFFS                    (0x00000000)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_RMSK                          0x3f
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_IN(x)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_ADDR(x), HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_RMSK)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_INM(x, m)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_ADDR(x), m)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_OUT(x, v)      \
        out_dword(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_ADDR(x),v)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_ADDR(x),m,v,HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_IN(x))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_OPCODE_BMSK                   0x3e
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_OPCODE_SHFT                    0x1
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_PRIORITY_BMSK                  0x1
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD0_PRIORITY_SHFT                  0x0

#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_ADDR(x)                 ((x) + QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS + 0x00000004)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_OFFS                    (0x00000004)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_RMSK                          0x7f
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_IN(x)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_ADDR(x), HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_RMSK)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_INM(x, m)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_ADDR(x), m)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_OUT(x, v)      \
        out_dword(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_ADDR(x),v)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_ADDR(x),m,v,HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_IN(x))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_BYTE_CNT_BMSK                 0x70
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_BYTE_CNT_SHFT                  0x4
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_SID_BMSK                       0xf
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD1_SID_SHFT                       0x0

#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_ADDR(x)                 ((x) + QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS + 0x00000008)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_OFFS                    (0x00000008)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_RMSK                          0xff
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_IN(x)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_ADDR(x), HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_RMSK)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_INM(x, m)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_ADDR(x), m)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_OUT(x, v)      \
        out_dword(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_ADDR(x),v)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_ADDR(x),m,v,HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_IN(x))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_PPID_BMSK                     0xff
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD2_PPID_SHFT                      0x0

#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDR(x)                 ((x) + QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS + 0x0000000c)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_OFFS                    (0x0000000c)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_RMSK                          0xff
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_IN(x)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDR(x), HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_RMSK)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_INM(x, m)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDR(x), m)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_OUT(x, v)      \
        out_dword(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDR(x),v)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDR(x),m,v,HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_IN(x))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDRESS_BMSK                  0xff
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CMD3_ADDRESS_SHFT                   0x0

#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_ADDR(x)               ((x) + QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS + 0x00000010)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_OFFS                  (0x00000010)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_RMSK                         0x1
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_IN(x)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_ADDR(x), HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_RMSK)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_INM(x, m)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_ADDR(x), m)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_OUT(x, v)      \
        out_dword(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_ADDR(x),v)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_ADDR(x),m,v,HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_IN(x))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_IRQ_EN_BMSK                  0x1
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_CONFIG_IRQ_EN_SHFT                  0x0

#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_ADDR(x)               ((x) + QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS + 0x00000014)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_OFFS                  (0x00000014)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_RMSK                         0xf
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_IN(x)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_ADDR(x), HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_RMSK)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_ADDR(x), m)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DROPPED_BMSK                 0x8
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DROPPED_SHFT                 0x3
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DENIED_BMSK                  0x4
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DENIED_SHFT                  0x2
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_FAILURE_BMSK                 0x2
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_FAILURE_SHFT                 0x1
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DONE_BMSK                    0x1
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_STATUS_DONE_SHFT                    0x0

#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_ADDR(base,g)          ((base) + QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS + 0x00000018 + 0x4 * (g))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_OFFS(base,g)          (0x00000018 + 0x4 * (g))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_RMSK                        0xff
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_INI(base,g)        \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_ADDR(base,g), HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_RMSK)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_INMI(base,g,mask)    \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_ADDR(base,g), mask)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_OUTI(base,g,val)    \
        out_dword(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_ADDR(base,g),val)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_OUTMI(base,g,mask,val) \
        out_dword_masked_ns(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_ADDR(base,g),mask,val,HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_INI(base,g))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_DATA_BMSK                   0xff
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_WDATAg_DATA_SHFT                    0x0

#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_ADDR(base,h)          ((base) + QDSS_PMIC_ARB_PMIC_ARB_PMIC_ARB_DEBUG_REG_BASE_OFFS + 0x00000038 + 0x4 * (h))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_OFFS(base,h)          (0x00000038 + 0x4 * (h))
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_RMSK                        0xff
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_INI(base,h)        \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_ADDR(base,h), HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_RMSK)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_INMI(base,h,mask)    \
        in_dword_masked(HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_ADDR(base,h), mask)
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_DATA_BMSK                   0xff
#define HWIO_QDSS_PMIC_ARB_PMIC_ARB_DEBUG_SPMI_CHNL_RDATAh_DATA_SHFT                    0x0


#endif /* __PMICARBCOREHWIO_H__ */

#ifndef __HALHWIOSECCTRL_H__
#define __HALHWIOSECCTRL_H__
/*
===========================================================================
*/
/**
  @file HALhwioSecCtrl.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================


  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                                             (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_PHYS                                                                        (SECURITY_CONTROL_BASE_PHYS + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                                        0x00000000

#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n)                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000000 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_PHYS(n)                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000000 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_OFFS(n)                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000000 + 0x4 * (n))
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK                                                                          0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_MAXn                                                                                  39
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_RAW_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                                              0xffffffff
#define HWIO_QFPROM_RAW_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                                                     0x0

#define HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a0)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_PHYS                                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000a0)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_OFFS                                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000a0)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_RMSK                                                                            0xfffffff
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR, HWIO_QFPROM_RAW_JTAG_ID_LSB_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_LSB_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_LSB_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_FEATURE_ID_BMSK                                                                 0xff00000
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_FEATURE_ID_SHFT                                                                      0x14
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_JTAG_ID_BMSK                                                                      0xfffff
#define HWIO_QFPROM_RAW_JTAG_ID_LSB_JTAG_ID_SHFT                                                                          0x0

#define HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a4)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_PHYS                                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000a4)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_OFFS                                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000a4)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR, HWIO_QFPROM_RAW_JTAG_ID_MSB_RMSK)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_JTAG_ID_MSB_ADDR,m,v,HWIO_QFPROM_RAW_JTAG_ID_MSB_IN)
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_QFPROM_RAW_JTAG_ID_MSB_BMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_JTAG_ID_MSB_QFPROM_RAW_JTAG_ID_MSB_SHFT                                                           0x0

#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000a8)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000a8)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000a8)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY1_BMSK                                                         0x80000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY1_SHFT                                                               0x1f
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY1_ALLOW_READ_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY1_DISABLE_READ_FVAL                                                   0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY0_BMSK                                                         0x40000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY0_SHFT                                                               0x1e
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY0_ALLOW_READ_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_PRIV_KEY0_DISABLE_READ_FVAL                                                   0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_HDCP_KEY_BMSK                                                               0x20000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_HDCP_KEY_SHFT                                                                     0x1d
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_HDCP_KEY_ALLOW_READ_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_HDCP_KEY_DISABLE_READ_FVAL                                                         0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE28_BMSK                                                                0x10000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE28_SHFT                                                                      0x1c
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE28_ALLOW_READ_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE28_DISABLE_READ_FVAL                                                          0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE27_BMSK                                                                 0x8000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE27_SHFT                                                                      0x1b
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE27_ALLOW_READ_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE27_DISABLE_READ_FVAL                                                          0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_USB_VID_PID_BMSK                                                             0x4000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_USB_VID_PID_SHFT                                                                  0x1a
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_USB_VID_PID_ALLOW_READ_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_USB_VID_PID_DISABLE_READ_FVAL                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                                             0x2000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                                                  0x19
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_ALLOW_READ_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_QC_SEC_BOOT_DISABLE_READ_FVAL                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                                            0x1000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_ALLOW_READ_FVAL                                                       0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_SEC_BOOT_DISABLE_READ_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_PK_HASH_BMSK                                                              0x800000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_PK_HASH_SHFT                                                                  0x17
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_PK_HASH_ALLOW_READ_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_PK_HASH_DISABLE_READ_FVAL                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                                   0x400000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                                       0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                                             0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                                           0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                                   0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                                       0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                                             0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                                           0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                                           0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                                               0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_ALLOW_READ_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_BOOT_ROM_PATCH_DISABLE_READ_FVAL                                                   0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_BMSK                                                                   0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_SHFT                                                                      0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_ALLOW_READ_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SPARE19_DISABLE_READ_FVAL                                                          0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SERIAL_NUM_BMSK                                                                0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SERIAL_NUM_SHFT                                                                   0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SERIAL_NUM_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_SERIAL_NUM_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_KEY_BMSK                                                                  0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_KEY_SHFT                                                                     0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_KEY_ALLOW_READ_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CUST_KEY_DISABLE_READ_FVAL                                                         0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_BMSK                                                          0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_SHFT                                                             0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_ALLOW_READ_FVAL                                                   0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_DISABLE_READ_FVAL                                                 0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_BMSK                                                                     0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_SHFT                                                                        0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_ALLOW_READ_FVAL                                                             0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEC_EN_DISABLE_READ_FVAL                                                           0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_BMSK                                                                      0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_SHFT                                                                         0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_ALLOW_READ_FVAL                                                              0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_CALIB_DISABLE_READ_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_ACCEL_BMSK                                                                  0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_ACCEL_SHFT                                                                     0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_ACCEL_ALLOW_READ_FVAL                                                          0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_ACCEL_DISABLE_READ_FVAL                                                        0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_DEBUG_KEY_BMSK                                                                  0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_DEBUG_KEY_SHFT                                                                     0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_DEBUG_KEY_ALLOW_READ_FVAL                                                          0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_DEBUG_KEY_DISABLE_READ_FVAL                                                        0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_REDUN_BMSK                                                                   0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_REDUN_SHFT                                                                     0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_REDUN_ALLOW_READ_FVAL                                                          0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MEM_REDUN_DISABLE_READ_FVAL                                                        0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                                                 0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                                   0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_ALLOW_READ_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_FEAT_CONFIG_DISABLE_READ_FVAL                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                                                  0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                                                    0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_OEM_CONFIG_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN2_BMSK                                                                   0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN2_SHFT                                                                     0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN2_ALLOW_READ_FVAL                                                          0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN2_DISABLE_READ_FVAL                                                        0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN1_BMSK                                                                    0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN1_SHFT                                                                     0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN1_ALLOW_READ_FVAL                                                          0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN1_DISABLE_READ_FVAL                                                        0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN0_BMSK                                                                    0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN0_SHFT                                                                     0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN0_ALLOW_READ_FVAL                                                          0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_IMEI_ESN0_DISABLE_READ_FVAL                                                        0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_BMSK                                                            0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_SHFT                                                             0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_ALLOW_READ_FVAL                                                  0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_DISABLE_READ_FVAL                                                0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_BMSK                                                             0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_SHFT                                                              0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_ALLOW_READ_FVAL                                                   0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_DISABLE_READ_FVAL                                                 0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_BMSK                                                                           0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_SHFT                                                                           0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_ALLOW_READ_FVAL                                                                0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_PTE_DISABLE_READ_FVAL                                                              0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                                                    0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                                                    0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_RD_WR_PERM_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_JTAG_ID_BMSK                                                                       0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_JTAG_ID_SHFT                                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_JTAG_ID_ALLOW_READ_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_JTAG_ID_DISABLE_READ_FVAL                                                          0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ACC_PRIVATE_BMSK                                                                   0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ACC_PRIVATE_SHFT                                                                   0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ACC_PRIVATE_ALLOW_READ_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_LSB_ACC_PRIVATE_DISABLE_READ_FVAL                                                      0x1

#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ac)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000ac)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000ac)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY1_BMSK                                                         0x80000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY1_SHFT                                                               0x1f
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY1_ALLOW_WRITE_FVAL                                                    0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY1_DISABLE_WRITE_FVAL                                                  0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY0_BMSK                                                         0x40000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY0_SHFT                                                               0x1e
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY0_ALLOW_WRITE_FVAL                                                    0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_PRIV_KEY0_DISABLE_WRITE_FVAL                                                  0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_HDCP_KEY_BMSK                                                               0x20000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_HDCP_KEY_SHFT                                                                     0x1d
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_HDCP_KEY_ALLOW_WRITE_FVAL                                                          0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_HDCP_KEY_DISABLE_WRITE_FVAL                                                        0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE28_BMSK                                                                0x10000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE28_SHFT                                                                      0x1c
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE28_ALLOW_WRITE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE28_DISABLE_WRITE_FVAL                                                         0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE27_BMSK                                                                 0x8000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE27_SHFT                                                                      0x1b
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE27_ALLOW_WRITE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE27_DISABLE_WRITE_FVAL                                                         0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_USB_VID_PID_BMSK                                                             0x4000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_USB_VID_PID_SHFT                                                                  0x1a
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_USB_VID_PID_ALLOW_WRITE_FVAL                                                       0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_USB_VID_PID_DISABLE_WRITE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                                             0x2000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                                                  0x19
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_ALLOW_WRITE_FVAL                                                       0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_QC_SEC_BOOT_DISABLE_WRITE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                                            0x1000000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_ALLOW_WRITE_FVAL                                                      0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_SEC_BOOT_DISABLE_WRITE_FVAL                                                    0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_PK_HASH_BMSK                                                              0x800000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_PK_HASH_SHFT                                                                  0x17
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_PK_HASH_ALLOW_WRITE_FVAL                                                       0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_PK_HASH_DISABLE_WRITE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                                   0x400000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                                       0x16
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                                   0x200000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                                       0x15
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                                           0x100000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                                               0x14
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_ALLOW_WRITE_FVAL                                                    0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_BOOT_ROM_PATCH_DISABLE_WRITE_FVAL                                                  0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_BMSK                                                                   0x80000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_SHFT                                                                      0x13
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_ALLOW_WRITE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SPARE19_DISABLE_WRITE_FVAL                                                         0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SERIAL_NUM_BMSK                                                                0x40000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SERIAL_NUM_SHFT                                                                   0x12
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SERIAL_NUM_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_SERIAL_NUM_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_KEY_BMSK                                                                  0x20000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_KEY_SHFT                                                                     0x11
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_KEY_ALLOW_WRITE_FVAL                                                          0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CUST_KEY_DISABLE_WRITE_FVAL                                                        0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_BMSK                                                          0x10000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_SHFT                                                             0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_ALLOW_WRITE_FVAL                                                  0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_DISABLE_WRITE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_BMSK                                                                     0x8000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_SHFT                                                                        0xf
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_ALLOW_WRITE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEC_EN_DISABLE_WRITE_FVAL                                                          0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_BMSK                                                                      0x4000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_SHFT                                                                         0xe
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_ALLOW_WRITE_FVAL                                                             0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_CALIB_DISABLE_WRITE_FVAL                                                           0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_ACCEL_BMSK                                                                  0x2000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_ACCEL_SHFT                                                                     0xd
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_ACCEL_ALLOW_WRITE_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_ACCEL_DISABLE_WRITE_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_DEBUG_KEY_BMSK                                                                  0x1000
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_DEBUG_KEY_SHFT                                                                     0xc
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_DEBUG_KEY_ALLOW_WRITE_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_DEBUG_KEY_DISABLE_WRITE_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_REDUN_BMSK                                                                   0x800
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_REDUN_SHFT                                                                     0xb
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_REDUN_ALLOW_WRITE_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MEM_REDUN_DISABLE_WRITE_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                                                 0x400
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                                                   0xa
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_ALLOW_WRITE_FVAL                                                       0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_FEAT_CONFIG_DISABLE_WRITE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                                                  0x200
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                                                    0x9
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_OEM_CONFIG_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN2_BMSK                                                                   0x100
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN2_SHFT                                                                     0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN2_ALLOW_WRITE_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN2_DISABLE_WRITE_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN1_BMSK                                                                    0x80
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN1_SHFT                                                                     0x7
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN1_ALLOW_WRITE_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN1_DISABLE_WRITE_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN0_BMSK                                                                    0x40
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN0_SHFT                                                                     0x6
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN0_ALLOW_WRITE_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_IMEI_ESN0_DISABLE_WRITE_FVAL                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_BMSK                                                            0x20
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_SHFT                                                             0x5
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_ALLOW_WRITE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_DISABLE_WRITE_FVAL                                               0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_BMSK                                                             0x10
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_SHFT                                                              0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_ALLOW_WRITE_FVAL                                                  0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_DISABLE_WRITE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_BMSK                                                                           0x8
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_SHFT                                                                           0x3
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_ALLOW_WRITE_FVAL                                                               0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_PTE_DISABLE_WRITE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                                                    0x4
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                                                    0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_RD_WR_PERM_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_JTAG_ID_BMSK                                                                       0x2
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_JTAG_ID_SHFT                                                                       0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_JTAG_ID_ALLOW_WRITE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_JTAG_ID_DISABLE_WRITE_FVAL                                                         0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ACC_PRIVATE_BMSK                                                                   0x1
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ACC_PRIVATE_SHFT                                                                   0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ACC_PRIVATE_ALLOW_WRITE_FVAL                                                       0x0
#define HWIO_QFPROM_RAW_RD_WR_PERM_MSB_ACC_PRIVATE_DISABLE_WRITE_FVAL                                                     0x1

#define HWIO_QFPROM_RAW_PTE_LSB_ADDR                                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000b0)
#define HWIO_QFPROM_RAW_PTE_LSB_PHYS                                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000b0)
#define HWIO_QFPROM_RAW_PTE_LSB_OFFS                                                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000b0)
#define HWIO_QFPROM_RAW_PTE_LSB_RMSK                                                                               0xffffffff
#define HWIO_QFPROM_RAW_PTE_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_LSB_ADDR, HWIO_QFPROM_RAW_PTE_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_LSB_PTE_DATA0_BMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_LSB_PTE_DATA0_SHFT                                                                            0x0

#define HWIO_QFPROM_RAW_PTE_MSB_ADDR                                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000b4)
#define HWIO_QFPROM_RAW_PTE_MSB_PHYS                                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000b4)
#define HWIO_QFPROM_RAW_PTE_MSB_OFFS                                                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000b4)
#define HWIO_QFPROM_RAW_PTE_MSB_RMSK                                                                               0xffffffff
#define HWIO_QFPROM_RAW_PTE_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_MSB_ADDR, HWIO_QFPROM_RAW_PTE_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_MSB_PTE_DATA1_BMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_PTE_MSB_PTE_DATA1_SHFT                                                                            0x0

#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000b8)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000b8)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000b8)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_TZ_BMSK                                                          0xffff0000
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_TZ_SHFT                                                                0x10
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_SBL1_BMSK                                                            0xffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_LSB_SBL1_SHFT                                                               0x0

#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000bc)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000bc)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000bc)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_PIL_SUBSYSTEM_31_0_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW0_MSB_PIL_SUBSYSTEM_31_0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c0)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000c0)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000c0)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_RMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_APPSBL0_BMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_LSB_APPSBL0_SHFT                                                            0x0

#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c4)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_PHYS                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000c4)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_OFFS                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000c4)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_RMSK                                                              0x3ffffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_RPM_BMSK                                                          0x3fc0000
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_RPM_SHFT                                                               0x12
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_APPSBL1_BMSK                                                        0x3ffff
#define HWIO_QFPROM_RAW_AP_ANTI_ROLLBACK_ROW1_MSB_APPSBL1_SHFT                                                            0x0

#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000c8)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000c8)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000c8)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR, HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_RMSK)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_IN)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_MSS_BMSK                                                             0xffff0000
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_MSS_SHFT                                                                   0x10
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_MBA_BMSK                                                                 0xffff
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_LSB_MBA_SHFT                                                                    0x0

#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000cc)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000cc)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000cc)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR, HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_RMSK)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_IN)
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_BMSK                                0xffffffff
#define HWIO_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_QFPROM_RAW_MSA_ANTI_ROLLBACK_MSB_SHFT                                       0x0

#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d0)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000d0)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000d0)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN0_LSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN0_LSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS3_POINT1_BMSK                                                           0xfc000000
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS3_POINT1_SHFT                                                                 0x1a
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS2_POINT1_BMSK                                                            0x3f00000
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS2_POINT1_SHFT                                                                 0x14
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS1_POINT1_BMSK                                                              0xfc000
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS1_POINT1_SHFT                                                                  0xe
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS0_POINT1_BMSK                                                               0x3f00
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS0_POINT1_SHFT                                                                  0x8
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS_BASE0_BMSK                                                                   0xff
#define HWIO_QFPROM_RAW_IMEI_ESN0_LSB_TSENS_BASE0_SHFT                                                                    0x0

#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d4)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000d4)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000d4)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN0_MSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN0_MSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS_CAL_SEL_1_0_BMSK                                                       0xc0000000
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS_CAL_SEL_1_0_SHFT                                                             0x1e
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS8_POINT1_BMSK                                                           0x3f000000
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS8_POINT1_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS7_POINT1_BMSK                                                             0xfc0000
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS7_POINT1_SHFT                                                                 0x12
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS6_POINT1_BMSK                                                              0x3f000
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS6_POINT1_SHFT                                                                  0xc
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS5_POINT1_BMSK                                                                0xfc0
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS5_POINT1_SHFT                                                                  0x6
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS4_POINT1_BMSK                                                                 0x3f
#define HWIO_QFPROM_RAW_IMEI_ESN0_MSB_TSENS4_POINT1_SHFT                                                                  0x0

#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000d8)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000d8)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000d8)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN1_LSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN1_LSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS1_POINT2_BMSK                                                           0xfc000000
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS1_POINT2_SHFT                                                                 0x1a
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS0_POINT2_BMSK                                                            0x3f00000
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS0_POINT2_SHFT                                                                 0x14
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS_BASE1_BMSK                                                                0xff000
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS_BASE1_SHFT                                                                    0xc
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS10_POINT1_BMSK                                                               0xfc0
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS10_POINT1_SHFT                                                                 0x6
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS9_POINT1_BMSK                                                                 0x3f
#define HWIO_QFPROM_RAW_IMEI_ESN1_LSB_TSENS9_POINT1_SHFT                                                                  0x0

#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000dc)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000dc)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000dc)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN1_MSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN1_MSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_SW_CAL_REDUN_0_BMSK                                                          0x80000000
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_SW_CAL_REDUN_0_SHFT                                                                0x1f
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENSE_CAL_SEL_2_BMSK                                                        0x40000000
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENSE_CAL_SEL_2_SHFT                                                              0x1e
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS6_POINT2_BMSK                                                           0x3f000000
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS6_POINT2_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS5_POINT2_BMSK                                                             0xfc0000
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS5_POINT2_SHFT                                                                 0x12
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS4_POINT2_BMSK                                                              0x3f000
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS4_POINT2_SHFT                                                                  0xc
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS3_POINT2_BMSK                                                                0xfc0
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS3_POINT2_SHFT                                                                  0x6
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS2_POINT2_BMSK                                                                 0x3f
#define HWIO_QFPROM_RAW_IMEI_ESN1_MSB_TSENS2_POINT2_SHFT                                                                  0x0

#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e0)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000e0)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000e0)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN2_LSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN2_LSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_SW_CAL_REDUN_8_1_BMSK                                                        0xff000000
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_SW_CAL_REDUN_8_1_SHFT                                                              0x18
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_TSENS10_POINT2_BMSK                                                            0xfc0000
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_TSENS10_POINT2_SHFT                                                                0x12
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_TSENS9_POINT2_BMSK                                                              0x3f000
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_TSENS9_POINT2_SHFT                                                                  0xc
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_TSENS8_POINT2_BMSK                                                                0xfc0
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_TSENS8_POINT2_SHFT                                                                  0x6
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_TSENS7_POINT2_BMSK                                                                 0x3f
#define HWIO_QFPROM_RAW_IMEI_ESN2_LSB_TSENS7_POINT2_SHFT                                                                  0x0

#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e4)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000e4)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000e4)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR, HWIO_QFPROM_RAW_IMEI_ESN2_MSB_RMSK)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_IMEI_ESN2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_IMEI_ESN2_MSB_IN)
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_SW_CAL_REDUN_40_9_BMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_IMEI_ESN2_MSB_SW_CAL_REDUN_40_9_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000e8)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000e8)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000e8)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_BMSK                                                    0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_SHFT                                                          0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_BMSK                                                    0x40000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_SHFT                                                          0x1e
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_BMSK                                             0x20000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_SHFT                                                   0x1d
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_BMSK                                                 0x10000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_SHFT                                                       0x1c
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                       0x8000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                            0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                     0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_BMSK                                                  0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_SHFT                                                       0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_BMSK                                                 0x2000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_SHFT                                                      0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_BMSK                                                0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_BMSK                                                   0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_SHFT                                                       0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_BMSK                                                   0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_SHFT                                                       0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                                                   0x200000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                                       0x15
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_BMSK                                                              0x1e0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE1_SHFT                                                                  0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                        0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                           0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                                                   0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                                      0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                                             0x7c00
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                                                0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_DEFAULT_FVAL                                                        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SD_SDC2_EMMC_SDC1_FVAL                                              0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_EMMC_SDC1_FVAL                                                      0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_USB_FVAL                                                            0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_HSIC_FVAL                                                           0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_NAND_FVAL                                                           0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SPI_ON_BLSP1_QUP1_FVAL                                              0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SPI_ON_BLSP1_QUP2_FVAL                                              0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_BMSK                                                          0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_SHFT                                                            0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_FLCB_ROM_WILL_NOT_SKIP_SDP_ENUMERATION_FVAL                     0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_FORCE_THE_FLCB_ROM_TO_SKIP_SDP_ENUMERATION_AND_FALLBACK_TO_ATC_FVAL        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                                    0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                                      0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_ENUM_19_2_MHZ_FVAL                                        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_ENUM_25_MHZ_FVAL                                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_BMSK                                                     0xc0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_SHFT                                                      0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_XO_FVAL                                                   0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_ENUM_384_MHZ_FVAL                                         0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_ENUM_614_4_MHZ_FVAL                                       0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_ENUM_998_4_MHZ_FVAL                                       0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                                                  0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                                                   0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                                                  0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                                                   0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_BMSK                                                  0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_SHFT                                                  0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_DO_NOT_SHUT_DOWN_PHONE_SW_KEPT_IN_FOREVER_LOOP_FVAL        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_QC_PMIC_POWER_DOWN_THE_PHONE_FVAL                     0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_BMSK                                                         0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_SHFT                                                         0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_TIMEOUT_DISABLED_FVAL                                        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                                     0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_BMSK                                                            0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_SHFT                                                            0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_TIMEOUT_DISABLED_FVAL                                           0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                                          0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_DOWNLOADER_ENABLED_FVAL                                       0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_DOWNLOADER_DISABLED_FVAL                                      0x1

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000ec)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000ec)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000ec)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WDOG_EN_BMSK                                                           0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WDOG_EN_SHFT                                                                 0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WDOG_EN_USE_GPIO_FVAL                                                         0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_WDOG_EN_IGNORE_GPIO_ENABLE_WDOG_FVAL                                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE2_BMSK                                                            0x78000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE2_SHFT                                                                  0x1b
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_BMSK                                                   0x4000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_SHFT                                                        0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_NORMAL_MODE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_SECURE_MODE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_BMSK                                                    0x3000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_SHFT                                                         0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_IMEI_ESN0_FVAL                                                0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_IMEI_ESN1_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                                              0xc00000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                                                  0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_BMSK                                                0x200000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_SHFT                                                    0x15
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_KEY0_FVAL                                                0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_KEY1_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SW_FUSE_PROG_DISABLE_BMSK                                                0x100000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SW_FUSE_PROG_DISABLE_SHFT                                                    0x14
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE0_BMSK                                                               0xffe00
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SPARE0_SHFT                                                                   0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                                                   0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                                                     0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                                                     0x80
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                                                      0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                                                      0x40
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                                       0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                                                       0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                                        0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                                       0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                                        0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                                                     0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                                                     0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                                                      0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                                      0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                                       0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                                       0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                                       0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                                       0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_DISABLE_FVAL                                               0x1

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f0)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000f0)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000f0)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                                         0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                                               0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                                              0xff00
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                                                 0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_SPARE0_BMSK                                                                  0xff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_SPARE0_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f4)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000f4)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000f4)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_SPARE0_BMSK                                                            0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_SPARE0_SHFT                                                                  0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                                                        0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                                           0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000f8)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000f8)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000f8)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_BMSK                                                     0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_SHFT                                                           0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_BMSK                                                      0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_SHFT                                                            0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_ENABLE_FVAL                                                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_DISABLE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                                                    0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                                          0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_DISABLE_FVAL                                                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_BMSK                                                    0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_SHFT                                                          0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_DISABLE_FVAL                                                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                                               0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                                                    0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                                               0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                                                    0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_BMSK                                                    0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_SHFT                                                         0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_BMSK                                                    0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_SHFT                                                         0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_BMSK                                                0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_SHFT                                                    0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                                                0x700000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                                                    0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                                                 0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                                                    0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                                                 0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                                                    0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                                                0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                                                   0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_BMSK                                               0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_SHFT                                                  0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_BMSK                                              0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_SHFT                                                 0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                                         0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                                            0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                                         0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                                            0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_BMSK                                                0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_SHFT                                                   0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_BMSK                                                    0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_SHFT                                                      0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_BMSK                                                     0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_SHFT                                                       0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_BMSK                                                   0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_SHFT                                                     0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_BMSK                                                      0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_SHFT                                                        0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_BMSK                                                       0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_SHFT                                                        0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                                            0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                                             0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_ENABLE_FVAL                                                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_DISABLE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_BMSK                                                            0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_SHFT                                                             0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_ENABLE_FVAL                                                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_DISABLE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_BMSK                                                      0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_SHFT                                                       0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_BMSK                                                        0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_SHFT                                                        0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_BMSK                                                  0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_SHFT                                                  0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_ENABLE_PATCHING_FVAL                                  0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_DISABLE_PATCHING_FVAL                                 0x1

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000000fc)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000000fc)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000000fc)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_BMSK                                                      0xf0000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_SHFT                                                            0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                       0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                            0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE0_BMSK                                                            0x7ff8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SPARE0_SHFT                                                                  0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MULTIMEDIA_SPARE_DISABLE_BMSK                                             0x7800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_MULTIMEDIA_SPARE_DISABLE_SHFT                                                0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_BMSK                                                          0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_SHFT                                                            0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_BMSK                                                          0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_SHFT                                                            0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_BMSK                                                           0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_SHFT                                                             0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_ENABLE_FVAL                                                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_DISABLE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_BMSK                                                            0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_SHFT                                                             0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_ENABLE_FVAL                                                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_DISABLE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_BMSK                                                            0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_SHFT                                                             0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_ENABLE_FVAL                                                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_DISABLE_FVAL                                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_BMSK                                                   0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_SHFT                                                    0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_BMSK                                                   0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_SHFT                                                    0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_BMSK                                                    0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_SHFT                                                    0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_BMSK                                                   0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_SHFT                                                   0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_BMSK                                                     0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_SHFT                                                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_BMSK                                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_SHFT                                                            0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_DISABLE_FVAL                                                    0x1

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000100)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000100)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000100)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                                           0xffc00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                                                 0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                                            0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                                                0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_BMSK                                             0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_SHFT                                                 0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_BMSK                                               0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_SHFT                                                  0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_BMSK                                                0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_SHFT                                                   0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_BMSK                                                0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_SHFT                                                   0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                                             0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_BMSK                                               0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_SHFT                                                  0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                                                0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                                                   0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                                                0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                                                   0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_BMSK                                                    0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_SHFT                                                       0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_BMSK                                                     0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_SHFT                                                       0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                                              0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                                                0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_BMSK                                                  0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_SHFT                                                    0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                       0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                         0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_BMSK                                                   0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_SHFT                                                    0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                                                  0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                                                   0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_BMSK                                                 0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_SHFT                                                  0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_BMSK                                                   0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_SHFT                                                    0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_BMSK                                                    0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_SHFT                                                    0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE0_BMSK                                                                  0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SPARE0_SHFT                                                                  0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                                         0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                                         0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_ENABLE_FVAL                                                  0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_DISABLE_FVAL                                                 0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                                             0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_ENABLE_FVAL                                                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_DISABLE_FVAL                                                     0x1

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000104)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000104)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000104)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                                           0xfffc0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                                            0x3c000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                                                0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                                                    0x3fff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000108)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000108)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000108)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000010c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000010c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000010c)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SW_EXPANSION_FUSES_BMSK                                               0xff000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SW_EXPANSION_FUSES_SHFT                                                     0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE0_BMSK                                                             0xf00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE0_SHFT                                                                 0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_BMSK                                                0xf0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_SHFT                                                   0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION3_BMSK                                                  0xf000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION3_SHFT                                                     0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION2_BMSK                                                   0xf00
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION2_SHFT                                                     0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION1_BMSK                                                    0xf0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION1_SHFT                                                     0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION0_BMSK                                                     0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION0_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000110)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000110)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000110)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_BMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_SHFT                                                         0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_ENABLE_FVAL                                                  0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_DISABLE_FVAL                                                 0x1

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000114)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000114)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000114)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_SPARE0_BMSK                                                           0xffff0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_SPARE0_SHFT                                                                 0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_BMSK                                                   0xc000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_SHFT                                                      0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_XO_FVAL                                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_ENUM_75_MHZ_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_ENUM_75_MHZZ_FVAL                                         0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_ENUM_75_MHZZZ_FVAL                                        0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_BMSK                                                    0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_SHFT                                                       0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_BOOT_FROM_RPM_CODE_RAM_ALLOWED_FVAL                        0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_FORCE_BOOT_FROM_ROM_FVAL                                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_APPS_BOOT_FROM_ROM_BMSK                                                   0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_APPS_BOOT_FROM_ROM_SHFT                                                      0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_MSA_ENA_BMSK                                                               0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_MSA_ENA_SHFT                                                                 0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_BMSK                                                     0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_SHFT                                                       0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_MODEM_IMAGE_NOT_AUTHENTICATED_FVAL                         0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_FORCE_MODEM_IMAGE_AUTHENTICATION_FVAL                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FLCB_SKIP_BMSK                                                             0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_FLCB_SKIP_SHFT                                                               0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_BMSK                                              0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_SHFT                                                0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_QC_PMIC_FEATURES_DISABLED_FVAL                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_QC_PMIC_FEATURES_ENABLED_FVAL                       0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_BOOT_ROM_CFG_BMSK                                                           0xe0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_BOOT_ROM_CFG_SHFT                                                            0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_STACKED_MEMORY_ID_BMSK                                                      0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW3_MSB_STACKED_MEMORY_ID_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n)                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000118 + 0x4 * (n))
#define HWIO_QFPROM_RAW_MEM_REDUNn_PHYS(n)                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000118 + 0x4 * (n))
#define HWIO_QFPROM_RAW_MEM_REDUNn_OFFS(n)                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000118 + 0x4 * (n))
#define HWIO_QFPROM_RAW_MEM_REDUNn_RMSK                                                                            0xffffffff
#define HWIO_QFPROM_RAW_MEM_REDUNn_MAXn                                                                                    35
#define HWIO_QFPROM_RAW_MEM_REDUNn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n), HWIO_QFPROM_RAW_MEM_REDUNn_RMSK)
#define HWIO_QFPROM_RAW_MEM_REDUNn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_REDUNn_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_REDUNn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_REDUNn_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_REDUNn_INI(n))
#define HWIO_QFPROM_RAW_MEM_REDUNn_REDUN_DATA_BMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_MEM_REDUNn_REDUN_DATA_SHFT                                                                        0x0

#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a8)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001a8)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001a8)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR, HWIO_QFPROM_RAW_DEBUG_KEY_LSB_RMSK)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_DEBUG_KEY_LSB_ADDR,m,v,HWIO_QFPROM_RAW_DEBUG_KEY_LSB_IN)
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_KEY0_BMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_DEBUG_KEY_LSB_KEY0_SHFT                                                                           0x0

#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001ac)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001ac)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001ac)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR, HWIO_QFPROM_RAW_DEBUG_KEY_MSB_RMSK)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_DEBUG_KEY_MSB_ADDR,m,v,HWIO_QFPROM_RAW_DEBUG_KEY_MSB_IN)
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_KEY1_BMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_DEBUG_KEY_MSB_KEY1_SHFT                                                                           0x0

#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b0)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_PHYS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001b0)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_OFFS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001b0)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_MEM_ACCEL_BMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_LSB_MEM_ACCEL_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b4)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_PHYS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001b4)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_OFFS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001b4)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_MEM_ACCEL_BMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW0_MSB_MEM_ACCEL_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b8)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_PHYS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001b8)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_OFFS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001b8)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_RMSK                                                                        0x3fff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_MEM_ACCEL_BMSK                                                              0x3fff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_LSB_MEM_ACCEL_SHFT                                                                 0x0

#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001bc)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_PHYS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001bc)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_OFFS                                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001bc)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_QFPROM_RAW_MEM_ACCEL_ROW1_MSB_SHFT                                             0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001c0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001c0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_VREF_B0_BMSK                                                                0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_VREF_B0_SHFT                                                                      0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PH_B0M3_BMSK                                                                0x38000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PH_B0M3_SHFT                                                                      0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PH_B0M2_BMSK                                                                 0x7000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PH_B0M2_SHFT                                                                      0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PH_B0M1_BMSK                                                                  0xe00000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PH_B0M1_SHFT                                                                      0x15
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PH_B0M0_BMSK                                                                  0x1c0000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_PH_B0M0_SHFT                                                                      0x12
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_G_B0_BMSK                                                                      0x38000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_G_B0_SHFT                                                                          0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_SAR_B_BMSK                                                                      0x6000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_SAR_B_SHFT                                                                         0xd
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CSI_PHY_BMSK                                                                    0x1f00
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_CSI_PHY_SHFT                                                                       0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_BMSK                                                                      0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_SHFT                                                                       0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSIPHY_PLL_BMSK                                                                    0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSIPHY_PLL_SHFT                                                                    0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001c4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001c4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B2M3_BMSK                                                                0xe0000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B2M3_SHFT                                                                      0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B2M2_BMSK                                                                0x1c000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B2M2_SHFT                                                                      0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B2M1_BMSK                                                                 0x3800000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B2M1_SHFT                                                                      0x17
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B2M0_BMSK                                                                  0x700000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B2M0_SHFT                                                                      0x14
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_G_B2_BMSK                                                                      0xe0000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_G_B2_SHFT                                                                         0x11
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_VREF_B1_BMSK                                                                   0x18000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_VREF_B1_SHFT                                                                       0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B1M3_BMSK                                                                    0x7000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B1M3_SHFT                                                                       0xc
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B1M2_BMSK                                                                     0xe00
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B1M2_SHFT                                                                       0x9
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B1M1_BMSK                                                                     0x1c0
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B1M1_SHFT                                                                       0x6
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B1M0_BMSK                                                                      0x38
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_PH_B1M0_SHFT                                                                       0x3
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_G_B1_BMSK                                                                          0x7
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_G_B1_SHFT                                                                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001c8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001c8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001c8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_SPARE0_BMSK                                                                 0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_SPARE0_SHFT                                                                       0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_PH_B3M1_BMSK                                                                0x70000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_PH_B3M1_SHFT                                                                      0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_PH_B3M0_BMSK                                                                 0xe000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_PH_B3M0_SHFT                                                                      0x19
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_G_B3_BMSK                                                                    0x1c00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_G_B3_SHFT                                                                         0x16
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_VREF_B2_BMSK                                                                  0x300000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_VREF_B2_SHFT                                                                      0x14
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS3_LDO_VREF_TRIM_BMSK                                                       0xf8000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS3_LDO_VREF_TRIM_SHFT                                                           0xf
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS2_LDO_VREF_TRIM_BMSK                                                        0x7c00
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS2_LDO_VREF_TRIM_SHFT                                                           0xa
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS1_LDO_VREF_TRIM_BMSK                                                         0x3e0
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS1_LDO_VREF_TRIM_SHFT                                                           0x5
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS0_LDO_VREF_TRIM_BMSK                                                          0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_APPS0_LDO_VREF_TRIM_SHFT                                                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001cc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001cc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001cc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_BMSK                                                       0xe0000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_SHFT                                                             0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_NO_REDUNDANCY_FVAL                                                0x0
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_CPR_FVAL                                                          0x1
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_BBRX_FVAL                                                         0x2
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_TEMPERATURE_SENSORS_FVAL                                          0x3
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_RESERVED_FVAL                                                     0x4
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_RESERVEDZ_FVAL                                                    0x5
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_RESERVEDZZ_FVAL                                                   0x6
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_NO_REDUNDANCYZ_FVAL                                               0x7
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE0_BMSK                                                                 0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_SPARE0_SHFT                                                                       0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CLK_B_BMSK                                                                   0xc000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CLK_B_SHFT                                                                        0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CAP_B_BMSK                                                                   0x3000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_CAP_B_SHFT                                                                        0x18
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_VREF_B3_BMSK                                                                  0xc00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_VREF_B3_SHFT                                                                      0x16
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_PH_B3M3_BMSK                                                                  0x380000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_PH_B3M3_SHFT                                                                      0x13
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_PH_B3M2_BMSK                                                                   0x70000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_PH_B3M2_SHFT                                                                      0x10
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_Q6SS1_LDO_VREF_TRIM_BMSK                                                        0xff00
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_Q6SS1_LDO_VREF_TRIM_SHFT                                                           0x8
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_Q6SS0_LDO_VREF_TRIM_BMSK                                                          0xff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_Q6SS0_LDO_VREF_TRIM_SHFT                                                           0x0

#define HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR                                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d0)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_PHYS                                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001d0)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OFFS                                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001d0)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK                                                                            0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_LSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_BMSK                                                            0x80000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_SHFT                                                                  0x1f
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION31_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                                            0x40000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                                                  0x1e
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION30_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                                            0x20000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                                                  0x1d
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION29_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                                            0x10000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                                                  0x1c
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION28_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                                             0x8000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                                                  0x1b
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                                             0x4000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                                                  0x1a
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                                             0x2000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                                                  0x19
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                                             0x1000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                                                  0x18
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                                              0x800000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                                                  0x17
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                                              0x400000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                                                  0x16
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                                              0x200000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                                                  0x15
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                                              0x100000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                                                  0x14
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                                               0x80000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                                                  0x13
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                                               0x40000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                                                  0x12
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                                               0x20000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                                                  0x11
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                                               0x10000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                                                  0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                                                0x8000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                                                   0xf
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                                                0x4000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                                                   0xe
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                                                0x2000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                                                   0xd
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                                                0x1000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                                                   0xc
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                                                 0x800
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                                                   0xb
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                                                 0x400
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                                                   0xa
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                                                  0x200
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                                                    0x9
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                                                  0x100
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                                                    0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                                                   0x80
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                                                    0x7
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                                                   0x40
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                                                    0x6
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                                                   0x20
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                                                    0x5
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                                                   0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                                                    0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                                                    0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                                                    0x3
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                                                    0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                                                    0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                                                    0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                                                    0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_ENABLE_FVAL                                                             0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                                                    0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                                                    0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_DISABLE_FVAL                                                            0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_ENABLE_FVAL                                                             0x1

#define HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR                                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d4)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_PHYS                                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001d4)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OFFS                                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001d4)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK                                                                            0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_MSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_QFPROM_RAW_FEC_EN_MSB_BMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_QFPROM_RAW_FEC_EN_MSB_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001d8)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001d8)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001d8)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR, HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_RMSK)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_ADDR,m,v,HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_IN)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_QFPROM_RAW_INTAGLIO_GLUEBIT_LSB_SHFT                                         0x0

#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001dc)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001dc)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001dc)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR, HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_RMSK)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_ADDR,m,v,HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_IN)
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_BMSK                                  0xffffffff
#define HWIO_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_QFPROM_RAW_INTAGLIO_GLUEBIT_MSB_SHFT                                         0x0

#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_PHYS(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001e0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_OFFS(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001e0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_RMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_MAXn                                                                              1
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_KEY_DATA_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_LSB_KEY_DATA_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001e4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_PHYS(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001e4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_OFFS(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001e4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_RMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_MAXn                                                                              1
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_KEY_DATA_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_CUST_KEY_ROWn_MSB_KEY_DATA_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f0)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001f0)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001f0)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR, HWIO_QFPROM_RAW_SERIAL_NUM_LSB_RMSK)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SERIAL_NUM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SERIAL_NUM_LSB_IN)
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_SERIAL_NUM_BMSK                                                             0xffffffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_LSB_SERIAL_NUM_SHFT                                                                    0x0

#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f4)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001f4)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001f4)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_RMSK                                                                            0xffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR, HWIO_QFPROM_RAW_SERIAL_NUM_MSB_RMSK)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SERIAL_NUM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SERIAL_NUM_MSB_IN)
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_CHIP_ID_BMSK                                                                    0xffff
#define HWIO_QFPROM_RAW_SERIAL_NUM_MSB_CHIP_ID_SHFT                                                                       0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f8)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001f8)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001f8)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_RMSK                                                                           0xffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_PIL_SUBSYSTEM_47_32_BMSK                                                       0xffff
#define HWIO_QFPROM_RAW_SPARE_REG19_LSB_PIL_SUBSYSTEM_47_32_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001fc)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000001fc)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000001fc)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG19_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG19_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE_BMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG19_MSB_SPARE_SHFT                                                                        0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000200 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PHYS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000200 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OFFS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000200 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_MAXn                                                                            47
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA0_BMSK                                                        0xffff0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA0_SHFT                                                              0x10
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_ADDR_BMSK                                                             0xffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_ADDR_SHFT                                                                0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000204 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PHYS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000204 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OFFS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000204 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK                                                                    0x7f01ffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_MAXn                                                                            47
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_BMSK                                                          0x7f000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_SHFT                                                                0x18
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                                              0x10000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                                                 0x10
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_DATA1_BMSK                                                            0xffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_DATA1_SHFT                                                               0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000380 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_PHYS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000380 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OFFS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000380 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                                                3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000384 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_PHYS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000384 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OFFS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000384 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                                       0x7fffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                                                3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_BMSK                                             0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                                               0xffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003a0)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_PHYS                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003a0)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OFFS                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003a0)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003a4)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_PHYS                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003a4)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OFFS                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003a4)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                                       0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_BMSK                                             0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_SHFT                                                   0x18

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003a8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_PHYS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003a8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OFFS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003a8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                                                3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003ac + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_PHYS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003ac + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OFFS(n)                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003ac + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                                       0x7fffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                                                3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_BMSK                                             0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_SHFT                                                   0x18
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                                               0xffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003c8)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_PHYS                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003c8)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OFFS                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003c8)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003cc)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_PHYS                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003cc)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OFFS                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003cc)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                                       0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_BMSK                                             0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_SHFT                                                   0x18

#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003d0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_PHYS(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003d0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_OFFS(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003d0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_MAXn                                                                           3
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003d4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_PHYS(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003d4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_OFFS(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003d4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_MAXn                                                                           3
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_FEC_VALUE_BMSK                                                        0x7f000000
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_FEC_VALUE_SHFT                                                              0x18
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                                         0xffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003f0)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003f0)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003f0)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                                       0xffffffff
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003f4)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003f4)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003f4)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_RMSK                                                                  0x7f000000
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_FEC_VALUE_BMSK                                                        0x7f000000
#define HWIO_QFPROM_RAW_OEM_PK_HASH_ROW4_MSB_FEC_VALUE_SHFT                                                              0x18

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003f8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_PHYS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003f8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OFFS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003f8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                                          3
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                                       0xff000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                                             0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                                         0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                                             0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                                           0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                                              0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                                             0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000003fc + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_PHYS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000003fc + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OFFS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000003fc + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RMSK                                                                 0x7fffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                                          3
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_FEC_VALUE_BMSK                                                       0x7f000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_FEC_VALUE_SHFT                                                             0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                                         0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                                             0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                                           0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                                              0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                                             0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                                              0x0

#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000418 + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_PHYS(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000418 + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_OFFS(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000418 + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_MAXn                                                                           3
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                                        0xff000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                                              0x18
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                                          0xff0000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                                              0x10
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                                            0xff00
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                                               0x8
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                                              0xff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                                               0x0

#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000041c + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_PHYS(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000041c + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_OFFS(n)                                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000041c + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_MAXn                                                                           3
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_FEC_VALUE_BMSK                                                        0x7f000000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_FEC_VALUE_SHFT                                                              0x18
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                                          0xff0000
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                                              0x10
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                                            0xff00
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                                               0x8
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                                              0xff
#define HWIO_QFPROM_RAW_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                                               0x0

#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000438)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000438)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000438)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR, HWIO_QFPROM_RAW_USB_VID_PID_LSB_RMSK)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_USB_VID_PID_LSB_ADDR,m,v,HWIO_QFPROM_RAW_USB_VID_PID_LSB_IN)
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PID1_BMSK                                                                  0xffff0000
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PID1_SHFT                                                                        0x10
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PID0_BMSK                                                                      0xffff
#define HWIO_QFPROM_RAW_USB_VID_PID_LSB_PID0_SHFT                                                                         0x0

#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000043c)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000043c)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000043c)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_RMSK                                                                       0x7f00ffff
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR, HWIO_QFPROM_RAW_USB_VID_PID_MSB_RMSK)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_USB_VID_PID_MSB_ADDR,m,v,HWIO_QFPROM_RAW_USB_VID_PID_MSB_IN)
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_FEC_VALUE_BMSK                                                             0x7f000000
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_FEC_VALUE_SHFT                                                                   0x18
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_VID_BMSK                                                                       0xffff
#define HWIO_QFPROM_RAW_USB_VID_PID_MSB_VID_SHFT                                                                          0x0

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000440)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000440)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000440)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_SW_CAL_REDUN_72_41_BMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_LSB_SW_CAL_REDUN_72_41_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000444)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000444)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000444)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SW_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SW_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SW_CAL_REDUN_96_73_BMSK                                                 0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW0_MSB_SW_CAL_REDUN_96_73_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000448)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000448)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000448)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_SW_CAL_REDUN_128_97_BMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_LSB_SW_CAL_REDUN_128_97_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000044c)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000044c)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000044c)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_SW_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_SW_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_SW_CAL_REDUN_152_129_BMSK                                               0xffffff
#define HWIO_QFPROM_RAW_SPARE_REG27_ROW1_MSB_SW_CAL_REDUN_152_129_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000450)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000450)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000450)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_QUOT_IND_BMSK                                                    0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_QUOT_IND_SHFT                                                          0x1f
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_DIS_CPR_BMSK                                                     0x40000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_DIS_CPR_SHFT                                                           0x1e
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_DIS_CPR_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_DIS_CPR_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_QUOT_VMIN_BMSK                                           0x3ffc0000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_QUOT_VMIN_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_STEP_QUOT_BMSK                                              0x3fc00
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_STEP_QUOT_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_TARGET_BMSK                                                   0x3e0
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_TARGET_SHFT                                                     0x5
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_TURBO_TARGET_BMSK                                                      0x1f
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_LSB_CPR0_TURBO_TARGET_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000454)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000454)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000454)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_SW_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_SW_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_SPARE0_BMSK                                                             0xf80000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_SPARE0_SHFT                                                                 0x13
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_CPR0_SVS_ROSEL_BMSK                                                      0x70000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_CPR0_SVS_ROSEL_SHFT                                                         0x10
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_CPR0_SVS_STEP_QUOT_BMSK                                                   0xff00
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_CPR0_SVS_STEP_QUOT_SHFT                                                      0x8
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_CPR0_SVS_TARGET_BMSK                                                        0xf8
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_CPR0_SVS_TARGET_SHFT                                                         0x3
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_CPR0_NOMINAL_ROSEL_BMSK                                                      0x7
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW0_MSB_CPR0_NOMINAL_ROSEL_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000458)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000458)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000458)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_QUOT_IND_BMSK                                                    0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_QUOT_IND_SHFT                                                          0x1f
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_BMSK                                                     0x40000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_SHFT                                                           0x1e
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_QUOT_VMIN_BMSK                                           0x3ffc0000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_QUOT_VMIN_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_STEP_QUOT_BMSK                                              0x3fc00
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_STEP_QUOT_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_TARGET_BMSK                                                   0x3e0
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_TARGET_SHFT                                                     0x5
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_TURBO_TARGET_BMSK                                                      0x1f
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_LSB_CPR1_TURBO_TARGET_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000045c)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000045c)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000045c)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_SW_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_SW_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_SPARE0_BMSK                                                             0xf80000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_SPARE0_SHFT                                                                 0x13
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_CPR1_SVS_ROSEL_BMSK                                                      0x70000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_CPR1_SVS_ROSEL_SHFT                                                         0x10
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_CPR1_SVS_STEP_QUOT_BMSK                                                   0xff00
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_CPR1_SVS_STEP_QUOT_SHFT                                                      0x8
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_CPR1_SVS_TARGET_BMSK                                                        0xf8
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_CPR1_SVS_TARGET_SHFT                                                         0x3
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_CPR1_NOMINAL_ROSEL_BMSK                                                      0x7
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW1_MSB_CPR1_NOMINAL_ROSEL_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000460)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000460)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000460)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_QUOT_IND_BMSK                                                    0x80000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_QUOT_IND_SHFT                                                          0x1f
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_DIS_CPR_BMSK                                                     0x40000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_DIS_CPR_SHFT                                                           0x1e
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_DIS_CPR_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_DIS_CPR_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_QUOT_VMIN_BMSK                                           0x3ffc0000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_QUOT_VMIN_SHFT                                                 0x12
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_STEP_QUOT_BMSK                                              0x3fc00
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_STEP_QUOT_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_TARGET_BMSK                                                   0x3e0
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_TARGET_SHFT                                                     0x5
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_TURBO_TARGET_BMSK                                                      0x1f
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_LSB_CPR2_TURBO_TARGET_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000464)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000464)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000464)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_RMSK                                                                  0x7fffffff
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_SW_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_SW_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_SPARE0_BMSK                                                             0xf80000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_SPARE0_SHFT                                                                 0x13
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_CPR2_SVS_ROSEL_BMSK                                                      0x70000
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_CPR2_SVS_ROSEL_SHFT                                                         0x10
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_CPR2_SVS_STEP_QUOT_BMSK                                                   0xff00
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_CPR2_SVS_STEP_QUOT_SHFT                                                      0x8
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_CPR2_SVS_TARGET_BMSK                                                        0xf8
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_CPR2_SVS_TARGET_SHFT                                                         0x3
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_CPR2_NOMINAL_ROSEL_BMSK                                                      0x7
#define HWIO_QFPROM_RAW_SPARE_REG28_ROW2_MSB_CPR2_NOMINAL_ROSEL_SHFT                                                      0x0

#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000468)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_PHYS                                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000468)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_OFFS                                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000468)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_RMSK                                                                          0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR, HWIO_QFPROM_RAW_HDCP_KSV_LSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_KSV0_BMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KSV_LSB_KSV0_SHFT                                                                            0x0

#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000046c)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_PHYS                                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000046c)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_OFFS                                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000046c)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_RMSK                                                                          0x7f0000ff
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR, HWIO_QFPROM_RAW_HDCP_KSV_MSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KSV_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_FEC_VALUE_BMSK                                                                0x7f000000
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_FEC_VALUE_SHFT                                                                      0x18
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_KSV1_BMSK                                                                           0xff
#define HWIO_QFPROM_RAW_HDCP_KSV_MSB_KSV1_SHFT                                                                            0x0

#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000468 + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_PHYS(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000468 + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_OFFS(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00000468 + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_RMSK                                                                     0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_MAXn                                                                             40
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_KEY_DATA_BMSK                                                            0xffffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_LSB_KEY_DATA_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000046c + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_PHYS(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000046c + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_OFFS(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000046c + 0x8 * (n))
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_RMSK                                                                     0x7fffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_MAXn                                                                             40
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_FEC_VALUE_BMSK                                                           0x7f000000
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_FEC_VALUE_SHFT                                                                 0x18
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_KEY_DATA_BMSK                                                              0xffffff
#define HWIO_QFPROM_RAW_HDCP_KEY_ROWn_MSB_KEY_DATA_SHFT                                                                   0x0

#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000005b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_PHYS(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000005b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_OFFS(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000005b0 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_RMSK                                                               0xffffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_MAXn                                                                       36
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_KEY_DATA_BMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_LSB_KEY_DATA_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000005b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_PHYS(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000005b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_OFFS(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000005b4 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_RMSK                                                               0x7fffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_MAXn                                                                       36
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_KEY_DATA_BMSK                                                        0xffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY0_ROWn_MSB_KEY_DATA_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000006d8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_PHYS(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000006d8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_OFFS(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000006d8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_RMSK                                                               0xffffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_MAXn                                                                       36
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_KEY_DATA_BMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_LSB_KEY_DATA_SHFT                                                             0x0

#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000006dc + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_PHYS(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000006dc + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_OFFS(n)                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000006dc + 0x8 * (n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_RMSK                                                               0x7fffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_MAXn                                                                       36
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_FEC_VALUE_BMSK                                                     0x7f000000
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_FEC_VALUE_SHFT                                                           0x18
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_KEY_DATA_BMSK                                                        0xffffff
#define HWIO_QFPROM_RAW_CUST_PRIV_KEY1_ROWn_MSB_KEY_DATA_SHFT                                                             0x0

#define HWIO_ACC_IR_ADDR                                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002000)
#define HWIO_ACC_IR_PHYS                                                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002000)
#define HWIO_ACC_IR_OFFS                                                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002000)
#define HWIO_ACC_IR_RMSK                                                                                                 0x1f
#define HWIO_ACC_IR_OUT(v)      \
        out_dword(HWIO_ACC_IR_ADDR,v)
#define HWIO_ACC_IR_INSTRUCTION_BMSK                                                                                     0x1f
#define HWIO_ACC_IR_INSTRUCTION_SHFT                                                                                      0x0
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_IDCODE_FVAL                                                                       0x1
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_REQVERID_FVAL                                                                     0x2
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_STARTACC_FVAL                                                                     0x3
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_STOPACC_FVAL                                                                      0x4
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_LOCKOUT_FVAL                                                                      0x5
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_INITFCT_FVAL                                                                      0x6
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_FCT_FVAL                                                                          0x7
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_TESTMEM_FVAL                                                                      0x8
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_TESTROM_FVAL                                                                      0x9
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_TESTNVM_FVAL                                                                      0xa
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_TESTRNG_FVAL                                                                      0xb
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_SCAN_FVAL                                                                         0xc
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_SHARENVMWR_FVAL                                                                   0xd
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_SHARENVMRD_FVAL                                                                   0xe
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_EXITTEST_FVAL                                                                     0xf
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_REQRESP_FVAL                                                                     0x10
#define HWIO_ACC_IR_INSTRUCTION_CMDCODE_BYPASS_FVAL                                                                      0x1f

#define HWIO_ACC_DR_ADDR                                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002004)
#define HWIO_ACC_DR_PHYS                                                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002004)
#define HWIO_ACC_DR_OFFS                                                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002004)
#define HWIO_ACC_DR_RMSK                                                                                           0xffffffff
#define HWIO_ACC_DR_IN          \
        in_dword_masked(HWIO_ACC_DR_ADDR, HWIO_ACC_DR_RMSK)
#define HWIO_ACC_DR_INM(m)      \
        in_dword_masked(HWIO_ACC_DR_ADDR, m)
#define HWIO_ACC_DR_OUT(v)      \
        out_dword(HWIO_ACC_DR_ADDR,v)
#define HWIO_ACC_DR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ACC_DR_ADDR,m,v,HWIO_ACC_DR_IN)
#define HWIO_ACC_DR_DR_BMSK                                                                                        0xffffffff
#define HWIO_ACC_DR_DR_SHFT                                                                                               0x0

#define HWIO_ACC_VERID_ADDR                                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002008)
#define HWIO_ACC_VERID_PHYS                                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002008)
#define HWIO_ACC_VERID_OFFS                                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002008)
#define HWIO_ACC_VERID_RMSK                                                                                            0xffff
#define HWIO_ACC_VERID_IN          \
        in_dword_masked(HWIO_ACC_VERID_ADDR, HWIO_ACC_VERID_RMSK)
#define HWIO_ACC_VERID_INM(m)      \
        in_dword_masked(HWIO_ACC_VERID_ADDR, m)
#define HWIO_ACC_VERID_FWVERID_BMSK                                                                                    0xff00
#define HWIO_ACC_VERID_FWVERID_SHFT                                                                                       0x8
#define HWIO_ACC_VERID_HWVERID_BMSK                                                                                      0xff
#define HWIO_ACC_VERID_HWVERID_SHFT                                                                                       0x0

#define HWIO_ACC_FEATSETn_ADDR(n)                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_PHYS(n)                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_OFFS(n)                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002010 + 0x4 * (n))
#define HWIO_ACC_FEATSETn_RMSK                                                                                     0xffffffff
#define HWIO_ACC_FEATSETn_MAXn                                                                                              7
#define HWIO_ACC_FEATSETn_INI(n)        \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), HWIO_ACC_FEATSETn_RMSK)
#define HWIO_ACC_FEATSETn_INMI(n,mask)    \
        in_dword_masked(HWIO_ACC_FEATSETn_ADDR(n), mask)
#define HWIO_ACC_FEATSETn_FEAT_BMSK                                                                                0xffffffff
#define HWIO_ACC_FEATSETn_FEAT_SHFT                                                                                       0x0

#define HWIO_QFPROM_BLOW_TIMER_ADDR                                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002038)
#define HWIO_QFPROM_BLOW_TIMER_PHYS                                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002038)
#define HWIO_QFPROM_BLOW_TIMER_OFFS                                                                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002038)
#define HWIO_QFPROM_BLOW_TIMER_RMSK                                                                                     0xfff
#define HWIO_QFPROM_BLOW_TIMER_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, HWIO_QFPROM_BLOW_TIMER_RMSK)
#define HWIO_QFPROM_BLOW_TIMER_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, m)
#define HWIO_QFPROM_BLOW_TIMER_OUT(v)      \
        out_dword(HWIO_QFPROM_BLOW_TIMER_ADDR,v)
#define HWIO_QFPROM_BLOW_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BLOW_TIMER_ADDR,m,v,HWIO_QFPROM_BLOW_TIMER_IN)
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_BMSK                                                                          0xfff
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_SHFT                                                                            0x0

#define HWIO_QFPROM_TEST_CTRL_ADDR                                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000203c)
#define HWIO_QFPROM_TEST_CTRL_PHYS                                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000203c)
#define HWIO_QFPROM_TEST_CTRL_OFFS                                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000203c)
#define HWIO_QFPROM_TEST_CTRL_RMSK                                                                                        0xf
#define HWIO_QFPROM_TEST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, HWIO_QFPROM_TEST_CTRL_RMSK)
#define HWIO_QFPROM_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, m)
#define HWIO_QFPROM_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_TEST_CTRL_ADDR,v)
#define HWIO_QFPROM_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_TEST_CTRL_ADDR,m,v,HWIO_QFPROM_TEST_CTRL_IN)
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_BMSK                                                                            0x8
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_SHFT                                                                            0x3
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_BMSK                                                                             0x4
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_SHFT                                                                             0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_BMSK                                                                             0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_SHFT                                                                             0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_BMSK                                                                       0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_SHFT                                                                       0x0

#define HWIO_QFPROM_ACCEL_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002040)
#define HWIO_QFPROM_ACCEL_PHYS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002040)
#define HWIO_QFPROM_ACCEL_OFFS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002040)
#define HWIO_QFPROM_ACCEL_RMSK                                                                                          0xfff
#define HWIO_QFPROM_ACCEL_IN          \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, HWIO_QFPROM_ACCEL_RMSK)
#define HWIO_QFPROM_ACCEL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, m)
#define HWIO_QFPROM_ACCEL_OUT(v)      \
        out_dword(HWIO_QFPROM_ACCEL_ADDR,v)
#define HWIO_QFPROM_ACCEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_ACCEL_ADDR,m,v,HWIO_QFPROM_ACCEL_IN)
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_BMSK                                                                          0x800
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_SHFT                                                                            0xb
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_BMSK                                                                        0x700
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_SHFT                                                                          0x8
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_BMSK                                                                              0xff
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_SHFT                                                                               0x0

#define HWIO_ACC_STATE_ADDR                                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002044)
#define HWIO_ACC_STATE_PHYS                                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002044)
#define HWIO_ACC_STATE_OFFS                                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002044)
#define HWIO_ACC_STATE_RMSK                                                                                               0x7
#define HWIO_ACC_STATE_IN          \
        in_dword_masked(HWIO_ACC_STATE_ADDR, HWIO_ACC_STATE_RMSK)
#define HWIO_ACC_STATE_INM(m)      \
        in_dword_masked(HWIO_ACC_STATE_ADDR, m)
#define HWIO_ACC_STATE_ACC_READY_BMSK                                                                                     0x4
#define HWIO_ACC_STATE_ACC_READY_SHFT                                                                                     0x2
#define HWIO_ACC_STATE_ACC_READY_NOT_READY_FVAL                                                                           0x0
#define HWIO_ACC_STATE_ACC_READY_READY_FVAL                                                                               0x1
#define HWIO_ACC_STATE_ACC_LOCKED_BMSK                                                                                    0x2
#define HWIO_ACC_STATE_ACC_LOCKED_SHFT                                                                                    0x1
#define HWIO_ACC_STATE_ACC_LOCKED_UNLOCKED_FVAL                                                                           0x0
#define HWIO_ACC_STATE_ACC_LOCKED_LOCKED_FVAL                                                                             0x1
#define HWIO_ACC_STATE_ACC_STOP_BMSK                                                                                      0x1
#define HWIO_ACC_STATE_ACC_STOP_SHFT                                                                                      0x0
#define HWIO_ACC_STATE_ACC_STOP_ACTIVE_FVAL                                                                               0x0
#define HWIO_ACC_STATE_ACC_STOP_STOPPED_FVAL                                                                              0x1

#define HWIO_QFPROM_BLOW_STATUS_ADDR                                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_PHYS                                                                               (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_OFFS                                                                               (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_RMSK                                                                                      0x3
#define HWIO_QFPROM_BLOW_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, HWIO_QFPROM_BLOW_STATUS_RMSK)
#define HWIO_QFPROM_BLOW_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, m)
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_BMSK                                                                        0x2
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_SHFT                                                                        0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_NO_ERROR_FVAL                                                               0x0
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_ERROR_FVAL                                                                  0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_BMSK                                                                          0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_SHFT                                                                          0x0

#define HWIO_HW_KEY_STATUS_ADDR                                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000204c)
#define HWIO_HW_KEY_STATUS_PHYS                                                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000204c)
#define HWIO_HW_KEY_STATUS_OFFS                                                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000204c)
#define HWIO_HW_KEY_STATUS_RMSK                                                                                          0x1f
#define HWIO_HW_KEY_STATUS_IN          \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, HWIO_HW_KEY_STATUS_RMSK)
#define HWIO_HW_KEY_STATUS_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, m)
#define HWIO_HW_KEY_STATUS_KDF_DONE_BMSK                                                                                 0x10
#define HWIO_HW_KEY_STATUS_KDF_DONE_SHFT                                                                                  0x4
#define HWIO_HW_KEY_STATUS_KDF_DONE_KDF_OPERATION_IS_PROGRESS_FVAL                                                        0x0
#define HWIO_HW_KEY_STATUS_KDF_DONE_KDF_OPERATION_HAS_COMPLETED_FVAL                                                      0x1
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BMSK                                                                          0x8
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_SHFT                                                                          0x3
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_NOT_BLOCKED_FVAL                                                              0x0
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BLOCKED_FVAL                                                                  0x1
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BMSK                                                                         0x4
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_SHFT                                                                         0x2
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_NOT_BLOCKED_FVAL                                                             0x0
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BLOCKED_FVAL                                                                 0x1
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BMSK                                                              0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_SHFT                                                              0x1
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_NOT_BLOWN_FVAL                                                    0x0
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BLOWN_FVAL                                                        0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BMSK                                                              0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_SHFT                                                              0x0
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_NOT_BLOWN_FVAL                                                    0x0
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BLOWN_FVAL                                                        0x1

#define HWIO_QFPROM_BIST_CTRL_ADDR                                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_PHYS                                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_OFFS                                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002050)
#define HWIO_QFPROM_BIST_CTRL_RMSK                                                                                       0x7f
#define HWIO_QFPROM_BIST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, HWIO_QFPROM_BIST_CTRL_RMSK)
#define HWIO_QFPROM_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, m)
#define HWIO_QFPROM_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_BIST_CTRL_ADDR,v)
#define HWIO_QFPROM_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BIST_CTRL_ADDR,m,v,HWIO_QFPROM_BIST_CTRL_IN)
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_BMSK                                                                           0x7c
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_SHFT                                                                            0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_BMSK                                                                             0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_SHFT                                                                             0x1
#define HWIO_QFPROM_BIST_CTRL_START_BMSK                                                                                  0x1
#define HWIO_QFPROM_BIST_CTRL_START_SHFT                                                                                  0x0

#define HWIO_QFPROM_BIST_ERROR_ADDR                                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_PHYS                                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_OFFS                                                                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002054)
#define HWIO_QFPROM_BIST_ERROR_RMSK                                                                                0xffffffff
#define HWIO_QFPROM_BIST_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, HWIO_QFPROM_BIST_ERROR_RMSK)
#define HWIO_QFPROM_BIST_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR_ERROR_BMSK                                                                          0xffffffff
#define HWIO_QFPROM_BIST_ERROR_ERROR_SHFT                                                                                 0x0
#define HWIO_QFPROM_BIST_ERROR_ERROR_NO_ERROR_FVAL                                                                        0x0
#define HWIO_QFPROM_BIST_ERROR_ERROR_ERROR_FVAL                                                                           0x1

#define HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n)                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_PHYS(n)                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_OFFS(n)                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002060 + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_MAXn                                                                                    7
#define HWIO_QFPROM_HASH_SIGNATUREn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), HWIO_QFPROM_HASH_SIGNATUREn_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_BMSK                                                                0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_SHFT                                                                       0x0

#define HWIO_FEC_ESR_ADDR                                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002080)
#define HWIO_FEC_ESR_PHYS                                                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002080)
#define HWIO_FEC_ESR_OFFS                                                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002080)
#define HWIO_FEC_ESR_RMSK                                                                                               0xfff
#define HWIO_FEC_ESR_IN          \
        in_dword_masked(HWIO_FEC_ESR_ADDR, HWIO_FEC_ESR_RMSK)
#define HWIO_FEC_ESR_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, m)
#define HWIO_FEC_ESR_OUT(v)      \
        out_dword(HWIO_FEC_ESR_ADDR,v)
#define HWIO_FEC_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_ADDR,m,v,HWIO_FEC_ESR_IN)
#define HWIO_FEC_ESR_CORR_SW_ACC_BMSK                                                                                   0x800
#define HWIO_FEC_ESR_CORR_SW_ACC_SHFT                                                                                     0xb
#define HWIO_FEC_ESR_CORR_HDCP_BMSK                                                                                     0x400
#define HWIO_FEC_ESR_CORR_HDCP_SHFT                                                                                       0xa
#define HWIO_FEC_ESR_CORR_BOOT_ROM_BMSK                                                                                 0x200
#define HWIO_FEC_ESR_CORR_BOOT_ROM_SHFT                                                                                   0x9
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_BMSK                                                                               0x100
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_SHFT                                                                                 0x8
#define HWIO_FEC_ESR_CORR_MULT_BMSK                                                                                      0x80
#define HWIO_FEC_ESR_CORR_MULT_SHFT                                                                                       0x7
#define HWIO_FEC_ESR_CORR_SEEN_BMSK                                                                                      0x40
#define HWIO_FEC_ESR_CORR_SEEN_SHFT                                                                                       0x6
#define HWIO_FEC_ESR_CORR_SEEN_NO_CORRECTION_FVAL                                                                         0x0
#define HWIO_FEC_ESR_CORR_SEEN_CORRECTION_FVAL                                                                            0x1
#define HWIO_FEC_ESR_ERR_SW_ACC_BMSK                                                                                     0x20
#define HWIO_FEC_ESR_ERR_SW_ACC_SHFT                                                                                      0x5
#define HWIO_FEC_ESR_ERR_HDCP_BMSK                                                                                       0x10
#define HWIO_FEC_ESR_ERR_HDCP_SHFT                                                                                        0x4
#define HWIO_FEC_ESR_ERR_BOOT_ROM_BMSK                                                                                    0x8
#define HWIO_FEC_ESR_ERR_BOOT_ROM_SHFT                                                                                    0x3
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_BMSK                                                                                  0x4
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_SHFT                                                                                  0x2
#define HWIO_FEC_ESR_ERR_MULT_BMSK                                                                                        0x2
#define HWIO_FEC_ESR_ERR_MULT_SHFT                                                                                        0x1
#define HWIO_FEC_ESR_ERR_SEEN_BMSK                                                                                        0x1
#define HWIO_FEC_ESR_ERR_SEEN_SHFT                                                                                        0x0
#define HWIO_FEC_ESR_ERR_SEEN_NO_ERROR_FVAL                                                                               0x0
#define HWIO_FEC_ESR_ERR_SEEN_ERROR_FVAL                                                                                  0x1

#define HWIO_FEC_EAR_ADDR                                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002084)
#define HWIO_FEC_EAR_PHYS                                                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002084)
#define HWIO_FEC_EAR_OFFS                                                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002084)
#define HWIO_FEC_EAR_RMSK                                                                                          0xffffffff
#define HWIO_FEC_EAR_IN          \
        in_dword_masked(HWIO_FEC_EAR_ADDR, HWIO_FEC_EAR_RMSK)
#define HWIO_FEC_EAR_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, m)
#define HWIO_FEC_EAR_CORR_ADDR_BMSK                                                                                0xffff0000
#define HWIO_FEC_EAR_CORR_ADDR_SHFT                                                                                      0x10
#define HWIO_FEC_EAR_ERR_ADDR_BMSK                                                                                     0xffff
#define HWIO_FEC_EAR_ERR_ADDR_SHFT                                                                                        0x0

#define HWIO_RESET_JDR_STATUS_ADDR                                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002088)
#define HWIO_RESET_JDR_STATUS_PHYS                                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002088)
#define HWIO_RESET_JDR_STATUS_OFFS                                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002088)
#define HWIO_RESET_JDR_STATUS_RMSK                                                                                        0x3
#define HWIO_RESET_JDR_STATUS_IN          \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, HWIO_RESET_JDR_STATUS_RMSK)
#define HWIO_RESET_JDR_STATUS_INM(m)      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, m)
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_BMSK                                                                            0x2
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_SHFT                                                                            0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_BMSK                                                                   0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_SHFT                                                                   0x0

#define HWIO_QFPROM_ROM_ERROR_ADDR                                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002090)
#define HWIO_QFPROM_ROM_ERROR_PHYS                                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002090)
#define HWIO_QFPROM_ROM_ERROR_OFFS                                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002090)
#define HWIO_QFPROM_ROM_ERROR_RMSK                                                                                        0x1
#define HWIO_QFPROM_ROM_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, HWIO_QFPROM_ROM_ERROR_RMSK)
#define HWIO_QFPROM_ROM_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, m)
#define HWIO_QFPROM_ROM_ERROR_ERROR_BMSK                                                                                  0x1
#define HWIO_QFPROM_ROM_ERROR_ERROR_SHFT                                                                                  0x0
#define HWIO_QFPROM_ROM_ERROR_ERROR_NO_ERROR_FVAL                                                                         0x0
#define HWIO_QFPROM_ROM_ERROR_ERROR_ERROR_FVAL                                                                            0x1

#define HWIO_QFPROM0_MATCH_STATUS_ADDR                                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002094)
#define HWIO_QFPROM0_MATCH_STATUS_PHYS                                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002094)
#define HWIO_QFPROM0_MATCH_STATUS_OFFS                                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002094)
#define HWIO_QFPROM0_MATCH_STATUS_RMSK                                                                             0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, HWIO_QFPROM0_MATCH_STATUS_RMSK)
#define HWIO_QFPROM0_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_BMSK                                                                        0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_SHFT                                                                               0x0

#define HWIO_QFPROM1_MATCH_STATUS_ADDR                                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002098)
#define HWIO_QFPROM1_MATCH_STATUS_PHYS                                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00002098)
#define HWIO_QFPROM1_MATCH_STATUS_OFFS                                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00002098)
#define HWIO_QFPROM1_MATCH_STATUS_RMSK                                                                             0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, HWIO_QFPROM1_MATCH_STATUS_RMSK)
#define HWIO_QFPROM1_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_BMSK                                                                        0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_SHFT                                                                               0x0

#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n)                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_PHYS(n)                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_OFFS(n)                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_MAXn                                                                                 39
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), HWIO_QFPROM_CORR_ACC_PRIVATEn_RMSK)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ACC_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_BMSK                                                             0xffffffff
#define HWIO_QFPROM_CORR_ACC_PRIVATEn_ACC_PRIVATE_SHFT                                                                    0x0

#define HWIO_QFPROM_CORR_JTAG_ID_LSB_ADDR                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a0)
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_PHYS                                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040a0)
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_OFFS                                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040a0)
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_RMSK                                                                           0xfffffff
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_LSB_ADDR, HWIO_QFPROM_CORR_JTAG_ID_LSB_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_FEATURE_ID_BMSK                                                                0xff00000
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_FEATURE_ID_SHFT                                                                     0x14
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_JTAG_ID_BMSK                                                                     0xfffff
#define HWIO_QFPROM_CORR_JTAG_ID_LSB_JTAG_ID_SHFT                                                                         0x0

#define HWIO_QFPROM_CORR_JTAG_ID_MSB_ADDR                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a4)
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_PHYS                                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040a4)
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_OFFS                                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040a4)
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_RMSK                                                                          0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_MSB_ADDR, HWIO_QFPROM_CORR_JTAG_ID_MSB_RMSK)
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_JTAG_ID_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_QFPROM_CORR_JTAG_ID_MSB_BMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_JTAG_ID_MSB_QFPROM_CORR_JTAG_ID_MSB_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040a8)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040a8)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040a8)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY1_BMSK                                                        0x80000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY1_SHFT                                                              0x1f
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY1_ALLOW_READ_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY1_DISABLE_READ_FVAL                                                  0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY0_BMSK                                                        0x40000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY0_SHFT                                                              0x1e
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY0_ALLOW_READ_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_PRIV_KEY0_DISABLE_READ_FVAL                                                  0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_HDCP_KEY_BMSK                                                              0x20000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_HDCP_KEY_SHFT                                                                    0x1d
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_HDCP_KEY_ALLOW_READ_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_HDCP_KEY_DISABLE_READ_FVAL                                                        0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE28_BMSK                                                               0x10000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE28_SHFT                                                                     0x1c
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE28_ALLOW_READ_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE28_DISABLE_READ_FVAL                                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE27_BMSK                                                                0x8000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE27_SHFT                                                                     0x1b
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE27_ALLOW_READ_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE27_DISABLE_READ_FVAL                                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_USB_VID_PID_BMSK                                                            0x4000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_USB_VID_PID_SHFT                                                                 0x1a
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_USB_VID_PID_ALLOW_READ_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_USB_VID_PID_DISABLE_READ_FVAL                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_BMSK                                                            0x2000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_SHFT                                                                 0x19
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_ALLOW_READ_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_QC_SEC_BOOT_DISABLE_READ_FVAL                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                                           0x1000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                                                0x18
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_ALLOW_READ_FVAL                                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_SEC_BOOT_DISABLE_READ_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_PK_HASH_BMSK                                                             0x800000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_PK_HASH_SHFT                                                                 0x17
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_PK_HASH_ALLOW_READ_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_PK_HASH_DISABLE_READ_FVAL                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                                  0x400000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                                      0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                                            0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                                          0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                                  0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                                      0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                                            0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                                          0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                                          0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                                              0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_ALLOW_READ_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_BOOT_ROM_PATCH_DISABLE_READ_FVAL                                                  0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_BMSK                                                                  0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_SHFT                                                                     0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_ALLOW_READ_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SPARE19_DISABLE_READ_FVAL                                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SERIAL_NUM_BMSK                                                               0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SERIAL_NUM_SHFT                                                                  0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SERIAL_NUM_ALLOW_READ_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_SERIAL_NUM_DISABLE_READ_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_KEY_BMSK                                                                 0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_KEY_SHFT                                                                    0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_KEY_ALLOW_READ_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CUST_KEY_DISABLE_READ_FVAL                                                        0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_BMSK                                                         0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_SHFT                                                            0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_ALLOW_READ_FVAL                                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_INTAGLIO_GLUEBIT_DISABLE_READ_FVAL                                                0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_BMSK                                                                    0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_SHFT                                                                       0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_ALLOW_READ_FVAL                                                            0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEC_EN_DISABLE_READ_FVAL                                                          0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_BMSK                                                                     0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_SHFT                                                                        0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_ALLOW_READ_FVAL                                                             0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_CALIB_DISABLE_READ_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_ACCEL_BMSK                                                                 0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_ACCEL_SHFT                                                                    0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_ACCEL_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_ACCEL_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_DEBUG_KEY_BMSK                                                                 0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_DEBUG_KEY_SHFT                                                                    0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_DEBUG_KEY_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_DEBUG_KEY_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_REDUN_BMSK                                                                  0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_REDUN_SHFT                                                                    0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_REDUN_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MEM_REDUN_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_BMSK                                                                0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                                  0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_ALLOW_READ_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_FEAT_CONFIG_DISABLE_READ_FVAL                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_BMSK                                                                 0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_SHFT                                                                   0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_ALLOW_READ_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_OEM_CONFIG_DISABLE_READ_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN2_BMSK                                                                  0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN2_SHFT                                                                    0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN2_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN2_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN1_BMSK                                                                   0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN1_SHFT                                                                    0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN1_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN1_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN0_BMSK                                                                   0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN0_SHFT                                                                    0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN0_ALLOW_READ_FVAL                                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_IMEI_ESN0_DISABLE_READ_FVAL                                                       0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_BMSK                                                           0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_SHFT                                                            0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_ALLOW_READ_FVAL                                                 0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_MSA_ANTI_ROLLBACK_DISABLE_READ_FVAL                                               0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_BMSK                                                            0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_SHFT                                                             0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_ALLOW_READ_FVAL                                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_AP_ANTI_ROLLBACK_DISABLE_READ_FVAL                                                0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_BMSK                                                                          0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_SHFT                                                                          0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_ALLOW_READ_FVAL                                                               0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_PTE_DISABLE_READ_FVAL                                                             0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_BMSK                                                                   0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_SHFT                                                                   0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_ALLOW_READ_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_RD_WR_PERM_DISABLE_READ_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_JTAG_ID_BMSK                                                                      0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_JTAG_ID_SHFT                                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_JTAG_ID_ALLOW_READ_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_JTAG_ID_DISABLE_READ_FVAL                                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ACC_PRIVATE_BMSK                                                                  0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ACC_PRIVATE_SHFT                                                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ACC_PRIVATE_ALLOW_READ_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_LSB_ACC_PRIVATE_DISABLE_READ_FVAL                                                     0x1

#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ac)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040ac)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040ac)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY1_BMSK                                                        0x80000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY1_SHFT                                                              0x1f
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY1_ALLOW_WRITE_FVAL                                                   0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY1_DISABLE_WRITE_FVAL                                                 0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY0_BMSK                                                        0x40000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY0_SHFT                                                              0x1e
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY0_ALLOW_WRITE_FVAL                                                   0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_PRIV_KEY0_DISABLE_WRITE_FVAL                                                 0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_HDCP_KEY_BMSK                                                              0x20000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_HDCP_KEY_SHFT                                                                    0x1d
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_HDCP_KEY_ALLOW_WRITE_FVAL                                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_HDCP_KEY_DISABLE_WRITE_FVAL                                                       0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE28_BMSK                                                               0x10000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE28_SHFT                                                                     0x1c
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE28_ALLOW_WRITE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE28_DISABLE_WRITE_FVAL                                                        0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE27_BMSK                                                                0x8000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE27_SHFT                                                                     0x1b
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE27_ALLOW_WRITE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE27_DISABLE_WRITE_FVAL                                                        0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_USB_VID_PID_BMSK                                                            0x4000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_USB_VID_PID_SHFT                                                                 0x1a
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_USB_VID_PID_ALLOW_WRITE_FVAL                                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_USB_VID_PID_DISABLE_WRITE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_BMSK                                                            0x2000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_SHFT                                                                 0x19
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_ALLOW_WRITE_FVAL                                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_QC_SEC_BOOT_DISABLE_WRITE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_BMSK                                                           0x1000000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_SHFT                                                                0x18
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_ALLOW_WRITE_FVAL                                                     0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_SEC_BOOT_DISABLE_WRITE_FVAL                                                   0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_PK_HASH_BMSK                                                             0x800000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_PK_HASH_SHFT                                                                 0x17
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_PK_HASH_ALLOW_WRITE_FVAL                                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_PK_HASH_DISABLE_WRITE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_BMSK                                                  0x400000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_SHFT                                                      0x16
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SEC_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_BMSK                                                  0x200000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_SHFT                                                      0x15
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PRI_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_BMSK                                                          0x100000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_SHFT                                                              0x14
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_ALLOW_WRITE_FVAL                                                   0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_BOOT_ROM_PATCH_DISABLE_WRITE_FVAL                                                 0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_BMSK                                                                  0x80000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_SHFT                                                                     0x13
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_ALLOW_WRITE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SPARE19_DISABLE_WRITE_FVAL                                                        0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SERIAL_NUM_BMSK                                                               0x40000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SERIAL_NUM_SHFT                                                                  0x12
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SERIAL_NUM_ALLOW_WRITE_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_SERIAL_NUM_DISABLE_WRITE_FVAL                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_KEY_BMSK                                                                 0x20000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_KEY_SHFT                                                                    0x11
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_KEY_ALLOW_WRITE_FVAL                                                         0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CUST_KEY_DISABLE_WRITE_FVAL                                                       0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_BMSK                                                         0x10000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_SHFT                                                            0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_ALLOW_WRITE_FVAL                                                 0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_INTAGLIO_GLUEBIT_DISABLE_WRITE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_BMSK                                                                    0x8000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_SHFT                                                                       0xf
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_ALLOW_WRITE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEC_EN_DISABLE_WRITE_FVAL                                                         0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_BMSK                                                                     0x4000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_SHFT                                                                        0xe
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_ALLOW_WRITE_FVAL                                                            0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_CALIB_DISABLE_WRITE_FVAL                                                          0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_ACCEL_BMSK                                                                 0x2000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_ACCEL_SHFT                                                                    0xd
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_ACCEL_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_ACCEL_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_DEBUG_KEY_BMSK                                                                 0x1000
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_DEBUG_KEY_SHFT                                                                    0xc
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_DEBUG_KEY_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_DEBUG_KEY_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_REDUN_BMSK                                                                  0x800
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_REDUN_SHFT                                                                    0xb
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_REDUN_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MEM_REDUN_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_BMSK                                                                0x400
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_SHFT                                                                  0xa
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_ALLOW_WRITE_FVAL                                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_FEAT_CONFIG_DISABLE_WRITE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_BMSK                                                                 0x200
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_SHFT                                                                   0x9
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_ALLOW_WRITE_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_OEM_CONFIG_DISABLE_WRITE_FVAL                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN2_BMSK                                                                  0x100
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN2_SHFT                                                                    0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN2_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN2_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN1_BMSK                                                                   0x80
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN1_SHFT                                                                    0x7
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN1_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN1_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN0_BMSK                                                                   0x40
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN0_SHFT                                                                    0x6
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN0_ALLOW_WRITE_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_IMEI_ESN0_DISABLE_WRITE_FVAL                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_BMSK                                                           0x20
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_SHFT                                                            0x5
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_ALLOW_WRITE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_MSA_ANTI_ROLLBACK_DISABLE_WRITE_FVAL                                              0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_BMSK                                                            0x10
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_SHFT                                                             0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_ALLOW_WRITE_FVAL                                                 0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_AP_ANTI_ROLLBACK_DISABLE_WRITE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_BMSK                                                                          0x8
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_SHFT                                                                          0x3
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_ALLOW_WRITE_FVAL                                                              0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_PTE_DISABLE_WRITE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_BMSK                                                                   0x4
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_SHFT                                                                   0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_ALLOW_WRITE_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_RD_WR_PERM_DISABLE_WRITE_FVAL                                                     0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_JTAG_ID_BMSK                                                                      0x2
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_JTAG_ID_SHFT                                                                      0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_JTAG_ID_ALLOW_WRITE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_JTAG_ID_DISABLE_WRITE_FVAL                                                        0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ACC_PRIVATE_BMSK                                                                  0x1
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ACC_PRIVATE_SHFT                                                                  0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ACC_PRIVATE_ALLOW_WRITE_FVAL                                                      0x0
#define HWIO_QFPROM_CORR_RD_WR_PERM_MSB_ACC_PRIVATE_DISABLE_WRITE_FVAL                                                    0x1

#define HWIO_QFPROM_CORR_PTE_LSB_ADDR                                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040b0)
#define HWIO_QFPROM_CORR_PTE_LSB_PHYS                                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040b0)
#define HWIO_QFPROM_CORR_PTE_LSB_OFFS                                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040b0)
#define HWIO_QFPROM_CORR_PTE_LSB_RMSK                                                                              0xffffffff
#define HWIO_QFPROM_CORR_PTE_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_LSB_ADDR, HWIO_QFPROM_CORR_PTE_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_LSB_PTE_DATA0_BMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_LSB_PTE_DATA0_SHFT                                                                           0x0

#define HWIO_QFPROM_CORR_PTE_MSB_ADDR                                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040b4)
#define HWIO_QFPROM_CORR_PTE_MSB_PHYS                                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040b4)
#define HWIO_QFPROM_CORR_PTE_MSB_OFFS                                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040b4)
#define HWIO_QFPROM_CORR_PTE_MSB_RMSK                                                                              0xffffffff
#define HWIO_QFPROM_CORR_PTE_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_MSB_ADDR, HWIO_QFPROM_CORR_PTE_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_MSB_PTE_DATA1_BMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_PTE_MSB_PTE_DATA1_SHFT                                                                           0x0

#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040b8)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040b8)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_OFFS                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040b8)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_TZ_BMSK                                                         0xffff0000
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_TZ_SHFT                                                               0x10
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_SBL1_BMSK                                                           0xffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_LSB_SBL1_SHFT                                                              0x0

#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040bc)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040bc)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_OFFS                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040bc)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_PIL_SUBSYSTEM_31_0_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW0_MSB_PIL_SUBSYSTEM_31_0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c0)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040c0)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_OFFS                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040c0)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_RMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_APPSBL0_BMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_LSB_APPSBL0_SHFT                                                           0x0

#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c4)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_PHYS                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040c4)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_OFFS                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040c4)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_RMSK                                                             0x3ffffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_RPM_BMSK                                                         0x3fc0000
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_RPM_SHFT                                                              0x12
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_APPSBL1_BMSK                                                       0x3ffff
#define HWIO_QFPROM_CORR_AP_ANTI_ROLLBACK_ROW1_MSB_APPSBL1_SHFT                                                           0x0

#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040c8)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_PHYS                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040c8)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_OFFS                                                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040c8)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_RMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_ADDR, HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_RMSK)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_MSS_BMSK                                                            0xffff0000
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_MSS_SHFT                                                                  0x10
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_MBA_BMSK                                                                0xffff
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_LSB_MBA_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040cc)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_PHYS                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040cc)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_OFFS                                                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040cc)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_RMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_ADDR, HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_RMSK)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_BMSK                              0xffffffff
#define HWIO_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_QFPROM_CORR_MSA_ANTI_ROLLBACK_MSB_SHFT                                     0x0

#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d0)
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040d0)
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040d0)
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN0_LSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN0_LSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS3_POINT1_BMSK                                                          0xfc000000
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS3_POINT1_SHFT                                                                0x1a
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS2_POINT1_BMSK                                                           0x3f00000
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS2_POINT1_SHFT                                                                0x14
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS1_POINT1_BMSK                                                             0xfc000
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS1_POINT1_SHFT                                                                 0xe
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS0_POINT1_BMSK                                                              0x3f00
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS0_POINT1_SHFT                                                                 0x8
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS_BASE0_BMSK                                                                  0xff
#define HWIO_QFPROM_CORR_IMEI_ESN0_LSB_TSENS_BASE0_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d4)
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040d4)
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040d4)
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN0_MSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN0_MSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS_CAL_SEL_1_0_BMSK                                                      0xc0000000
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS_CAL_SEL_1_0_SHFT                                                            0x1e
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS8_POINT1_BMSK                                                          0x3f000000
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS8_POINT1_SHFT                                                                0x18
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS7_POINT1_BMSK                                                            0xfc0000
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS7_POINT1_SHFT                                                                0x12
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS6_POINT1_BMSK                                                             0x3f000
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS6_POINT1_SHFT                                                                 0xc
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS5_POINT1_BMSK                                                               0xfc0
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS5_POINT1_SHFT                                                                 0x6
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS4_POINT1_BMSK                                                                0x3f
#define HWIO_QFPROM_CORR_IMEI_ESN0_MSB_TSENS4_POINT1_SHFT                                                                 0x0

#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040d8)
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040d8)
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040d8)
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN1_LSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN1_LSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS1_POINT2_BMSK                                                          0xfc000000
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS1_POINT2_SHFT                                                                0x1a
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS0_POINT2_BMSK                                                           0x3f00000
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS0_POINT2_SHFT                                                                0x14
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS_BASE1_BMSK                                                               0xff000
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS_BASE1_SHFT                                                                   0xc
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS10_POINT1_BMSK                                                              0xfc0
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS10_POINT1_SHFT                                                                0x6
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS9_POINT1_BMSK                                                                0x3f
#define HWIO_QFPROM_CORR_IMEI_ESN1_LSB_TSENS9_POINT1_SHFT                                                                 0x0

#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040dc)
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040dc)
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040dc)
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN1_MSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN1_MSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_SW_CAL_REDUN_0_BMSK                                                         0x80000000
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_SW_CAL_REDUN_0_SHFT                                                               0x1f
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENSE_CAL_SEL_2_BMSK                                                       0x40000000
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENSE_CAL_SEL_2_SHFT                                                             0x1e
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS6_POINT2_BMSK                                                          0x3f000000
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS6_POINT2_SHFT                                                                0x18
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS5_POINT2_BMSK                                                            0xfc0000
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS5_POINT2_SHFT                                                                0x12
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS4_POINT2_BMSK                                                             0x3f000
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS4_POINT2_SHFT                                                                 0xc
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS3_POINT2_BMSK                                                               0xfc0
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS3_POINT2_SHFT                                                                 0x6
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS2_POINT2_BMSK                                                                0x3f
#define HWIO_QFPROM_CORR_IMEI_ESN1_MSB_TSENS2_POINT2_SHFT                                                                 0x0

#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e0)
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040e0)
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040e0)
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN2_LSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN2_LSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_SW_CAL_REDUN_8_1_BMSK                                                       0xff000000
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_SW_CAL_REDUN_8_1_SHFT                                                             0x18
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_TSENS10_POINT2_BMSK                                                           0xfc0000
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_TSENS10_POINT2_SHFT                                                               0x12
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_TSENS9_POINT2_BMSK                                                             0x3f000
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_TSENS9_POINT2_SHFT                                                                 0xc
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_TSENS8_POINT2_BMSK                                                               0xfc0
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_TSENS8_POINT2_SHFT                                                                 0x6
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_TSENS7_POINT2_BMSK                                                                0x3f
#define HWIO_QFPROM_CORR_IMEI_ESN2_LSB_TSENS7_POINT2_SHFT                                                                 0x0

#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e4)
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040e4)
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040e4)
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN2_MSB_ADDR, HWIO_QFPROM_CORR_IMEI_ESN2_MSB_RMSK)
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_IMEI_ESN2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_SW_CAL_REDUN_40_9_BMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_IMEI_ESN2_MSB_SW_CAL_REDUN_40_9_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040e8)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040e8)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040e8)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_BMSK                                                   0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_DISABLE_SHFT                                                         0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_BMSK                                                   0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_DISABLE_SHFT                                                         0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_BMSK                                            0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_SHFT                                                  0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_BMSK                                                0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_SHFT                                                      0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DAPEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                      0x8000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                           0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_LPASS_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_BMSK                                                 0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_SHFT                                                      0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RPM_DBGEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_BMSK                                                0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_SHFT                                                     0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_BMSK                                               0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_LPASS_DBGEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_BMSK                                                  0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_SHFT                                                      0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_NIDEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_BMSK                                                  0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_SHFT                                                      0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_MSS_DBGEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                                                  0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                                      0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_BMSK                                                             0x1e0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE1_SHFT                                                                 0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                       0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                          0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_BMSK                                                  0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDC_EMMC_MODE1P2_EN_SHFT                                                     0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                                            0x7c00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                                               0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_DEFAULT_FVAL                                                       0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SD_SDC2_EMMC_SDC1_FVAL                                             0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_EMMC_SDC1_FVAL                                                     0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_USB_FVAL                                                           0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_HSIC_FVAL                                                          0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_NAND_FVAL                                                          0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SPI_ON_BLSP1_QUP1_FVAL                                             0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SPI_ON_BLSP1_QUP2_FVAL                                             0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_BMSK                                                         0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_SHFT                                                           0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_FLCB_ROM_WILL_NOT_SKIP_SDP_ENUMERATION_FVAL                    0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDP_ENUM_SKIP_FORCE_THE_FLCB_ROM_TO_SKIP_SDP_ENUMERATION_AND_FALLBACK_TO_ATC_FVAL        0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                                   0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                                     0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_ENUM_19_2_MHZ_FVAL                                       0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_ENUM_25_MHZ_FVAL                                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_BMSK                                                    0xc0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_SHFT                                                     0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_XO_FVAL                                                  0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_ENUM_384_MHZ_FVAL                                        0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_ENUM_614_4_MHZ_FVAL                                      0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_PBL_BOOT_SPEED_ENUM_998_4_MHZ_FVAL                                      0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                                                 0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                                                  0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_BMSK                                                                 0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SPARE0_SHFT                                                                  0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_BMSK                                                 0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_SHFT                                                 0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_DO_NOT_SHUT_DOWN_PHONE_SW_KEPT_IN_FOREVER_LOOP_FVAL        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_PMIC_POWER_DOWN_MSM_QC_PMIC_POWER_DOWN_THE_PHONE_FVAL                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_BMSK                                                        0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_SHFT                                                        0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_TIMEOUT_DISABLED_FVAL                                       0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_USB_ENUM_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_BMSK                                                           0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_SHFT                                                           0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_TIMEOUT_DISABLED_FVAL                                          0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DLOAD_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                                         0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_DOWNLOADER_ENABLED_FVAL                                      0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_DOWNLOADER_DISABLED_FVAL                                     0x1

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040ec)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040ec)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040ec)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WDOG_EN_BMSK                                                          0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WDOG_EN_SHFT                                                                0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WDOG_EN_USE_GPIO_FVAL                                                        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_WDOG_EN_IGNORE_GPIO_ENABLE_WDOG_FVAL                                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE2_BMSK                                                           0x78000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE2_SHFT                                                                 0x1b
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_BMSK                                                  0x4000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_SHFT                                                       0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_NORMAL_MODE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPDM_SECURE_MODE_SECURE_MODE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_BMSK                                                   0x3000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_SHFT                                                        0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_IMEI_ESN0_FVAL                                               0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IMEI_ESN_SELECT_IMEI_ESN1_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_BMSK                                                             0xc00000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE1_SHFT                                                                 0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_BMSK                                               0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_SHFT                                                   0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_KEY0_FVAL                                               0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_CUST_PRIV_KEY_SELECT_KEY1_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SW_FUSE_PROG_DISABLE_BMSK                                               0x100000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SW_FUSE_PROG_DISABLE_SHFT                                                   0x14
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE0_BMSK                                                              0xffe00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SPARE0_SHFT                                                                  0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_BMSK                                                  0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_SHFT                                                    0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_BMSK                                                    0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_SHFT                                                     0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_BMSK                                                     0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_SHFT                                                      0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_BMSK                                                      0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_SHFT                                                       0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_NIDEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_BMSK                                                      0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_SHFT                                                       0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_DAP_DBGEN_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_BMSK                                                    0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_SHFT                                                    0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_BMSK                                                     0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_SHFT                                                     0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                                      0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                                      0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                                      0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                                      0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_DISABLE_FVAL                                              0x1

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f0)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040f0)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040f0)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_BMSK                                                        0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_HW_ID_SHFT                                                              0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                                             0xff00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                                                0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_SPARE0_BMSK                                                                 0xff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_SPARE0_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f4)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_PHYS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040f4)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OFFS                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040f4)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_SPARE0_BMSK                                                           0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_SPARE0_SHFT                                                                 0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                                                       0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                                          0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040f8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040f8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040f8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_BMSK                                                    0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_SHFT                                                          0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDCP_DISABLE_DISABLE_FVAL                                                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_BMSK                                                     0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_SHFT                                                           0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EDP_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                                                   0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                                         0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_BMSK                                                   0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_SHFT                                                         0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ISP_1_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                                              0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                                                   0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                                              0x4000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                                                   0x1a
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_BMSK                                                   0x2000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_SHFT                                                        0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_ENABLE_FVAL                                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_3_DISABLE_DISABLE_FVAL                                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_BMSK                                                   0x1000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_SHFT                                                        0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_ENABLE_FVAL                                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CSID_2_DISABLE_DISABLE_FVAL                                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_BMSK                                               0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_SHFT                                                   0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MACROVISION_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                                               0x700000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                                                   0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                                                0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                                                   0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                                                0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                                                   0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                                               0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                                                  0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_BMSK                                              0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_SHFT                                                 0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_BMSK                                             0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_SHFT                                                0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                                        0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                                           0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_ENABLE_FVAL                                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT2_DISABLE_DISABLE_FVAL                                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                                        0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                                           0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_ENABLE_FVAL                                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_ABOVE_CAT1_DISABLE_DISABLE_FVAL                                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_BMSK                                               0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_SHFT                                                  0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_BMSK                                                   0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_SHFT                                                     0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_BMSK                                                    0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_SHFT                                                      0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_LTE_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_BMSK                                                  0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_SHFT                                                    0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_BMSK                                                     0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_SHFT                                                       0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_BMSK                                                      0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_SHFT                                                       0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                                           0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                                            0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_BMSK                                                           0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_SHFT                                                            0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_BMSK                                                     0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_SHFT                                                      0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPI_SLAVE_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_BMSK                                                       0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_SHFT                                                       0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_APPS_ACG_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_BMSK                                                 0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_SHFT                                                 0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_ENABLE_PATCHING_FVAL                                 0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_BOOT_ROM_PATCH_DISABLE_DISABLE_PATCHING_FVAL                                0x1

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000040fc)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000040fc)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000040fc)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_BMSK                                                     0xf0000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VENDOR_LOCK_SHFT                                                           0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                      0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                           0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE0_BMSK                                                           0x7ff8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SPARE0_SHFT                                                                 0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MULTIMEDIA_SPARE_DISABLE_BMSK                                            0x7800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_MULTIMEDIA_SPARE_DISABLE_SHFT                                               0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_BMSK                                                         0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_SHFT                                                           0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_PCIE_DISABLE_DISABLE_FVAL                                                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_BMSK                                                         0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_SHFT                                                           0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SATA_DISABLE_DISABLE_FVAL                                                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_BMSK                                                          0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_SHFT                                                            0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ACC_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_BMSK                                                           0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_SHFT                                                            0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VPE_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_BMSK                                                           0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_SHFT                                                            0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VFE_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_BMSK                                                  0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_SHFT                                                   0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_ENCODER_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_BMSK                                                  0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_SHFT                                                   0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_JPEG_DECODER_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_BMSK                                                   0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_SHFT                                                   0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_FIELD_3D_GRP_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_BMSK                                                  0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_SHFT                                                  0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_ENCODER_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_BMSK                                                    0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_SHFT                                                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_VIDEO_CODEC_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_BMSK                                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_SHFT                                                           0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_HDMI_DISABLE_DISABLE_FVAL                                                   0x1

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004100)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004100)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004100)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_BMSK                                                          0xffc00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE1_SHFT                                                                0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_BMSK                                           0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_SHFT                                               0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_BMSK                                            0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_SHFT                                                0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_BMSK                                              0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_SHFT                                                 0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_BMSK                                               0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_SHFT                                                  0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_NIDEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_BMSK                                               0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_SHFT                                                  0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_DAP_DBGEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_BMSK                                            0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_SHFT                                               0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_BMSK                                              0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_SHFT                                                 0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                                               0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                                                  0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                                               0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                                                  0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_BMSK                                                   0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE1_DISABLE_SHFT                                                      0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_BMSK                                                    0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SPARE0_DISABLE_SHFT                                                      0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_BMSK                                             0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_SHFT                                               0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_BMSK                                                 0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_SHFT                                                   0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DAPEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                      0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                        0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                 0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_BMSK                                                  0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_SHFT                                                   0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_RPM_DBGEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_BMSK                                                 0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_SHFT                                                  0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_BMSK                                                0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_SHFT                                                 0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_LPASS_DBGEN_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_BMSK                                                  0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_SHFT                                                   0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_NIDEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_BMSK                                                   0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_SHFT                                                   0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_MSS_DBGEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE0_BMSK                                                                 0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SPARE0_SHFT                                                                 0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                                        0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                                        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                                            0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_DISABLE_FVAL                                                    0x1

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004104)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004104)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004104)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                                          0xfffc0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                                                0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_BMSK                                           0x3c000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_CJI_CORE_SEL_DISABLE_SHFT                                               0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                                                   0x3fff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004108)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004108)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004108)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000410c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000410c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000410c)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SW_EXPANSION_FUSES_BMSK                                              0xff000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SW_EXPANSION_FUSES_SHFT                                                    0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE0_BMSK                                                            0xf00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE0_SHFT                                                                0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_BMSK                                               0xf0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_SHFT                                                  0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION3_BMSK                                                 0xf000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION3_SHFT                                                    0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION2_BMSK                                                  0xf00
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION2_SHFT                                                    0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION1_BMSK                                                   0xf0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION1_SHFT                                                    0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION0_BMSK                                                    0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CONFIGURATION0_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004110)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004110)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004110)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_BMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_SHFT                                                        0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_LSB_MDSP_FW_DISABLE_DISABLE_FVAL                                                0x1

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004114)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004114)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004114)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_SPARE0_BMSK                                                          0xffff0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_SPARE0_SHFT                                                                0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_BMSK                                                  0xc000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_SHFT                                                     0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_XO_FVAL                                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_ENUM_75_MHZ_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_ENUM_75_MHZZ_FVAL                                        0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_PBL_BOOT_SPEED_ENUM_75_MHZZZ_FVAL                                       0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_BMSK                                                   0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_SHFT                                                      0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_BOOT_FROM_RPM_CODE_RAM_ALLOWED_FVAL                       0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_RPM_BOOT_FROM_ROM_FORCE_BOOT_FROM_ROM_FVAL                                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_APPS_BOOT_FROM_ROM_BMSK                                                  0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_APPS_BOOT_FROM_ROM_SHFT                                                     0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_MSA_ENA_BMSK                                                              0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_MSA_ENA_SHFT                                                                0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_BMSK                                                    0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_SHFT                                                      0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_MODEM_IMAGE_NOT_AUTHENTICATED_FVAL                        0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FORCE_MSA_AUTH_EN_FORCE_MODEM_IMAGE_AUTHENTICATION_FVAL                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FLCB_SKIP_BMSK                                                            0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_FLCB_SKIP_SHFT                                                              0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_BMSK                                             0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_SHFT                                               0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_QC_PMIC_FEATURES_DISABLED_FVAL                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_QCOM_PMIC_FEATURE_ENABLE_QC_PMIC_FEATURES_ENABLED_FVAL                      0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_BOOT_ROM_CFG_BMSK                                                          0xe0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_BOOT_ROM_CFG_SHFT                                                           0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_STACKED_MEMORY_ID_BMSK                                                     0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW3_MSB_STACKED_MEMORY_ID_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_MEM_REDUNn_ADDR(n)                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004118 + 0x4 * (n))
#define HWIO_QFPROM_CORR_MEM_REDUNn_PHYS(n)                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004118 + 0x4 * (n))
#define HWIO_QFPROM_CORR_MEM_REDUNn_OFFS(n)                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004118 + 0x4 * (n))
#define HWIO_QFPROM_CORR_MEM_REDUNn_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_CORR_MEM_REDUNn_MAXn                                                                                   35
#define HWIO_QFPROM_CORR_MEM_REDUNn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_REDUNn_ADDR(n), HWIO_QFPROM_CORR_MEM_REDUNn_RMSK)
#define HWIO_QFPROM_CORR_MEM_REDUNn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_REDUNn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_REDUNn_REDUN_DATA_BMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_MEM_REDUNn_REDUN_DATA_SHFT                                                                       0x0

#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a8)
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041a8)
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041a8)
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_DEBUG_KEY_LSB_ADDR, HWIO_QFPROM_CORR_DEBUG_KEY_LSB_RMSK)
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_DEBUG_KEY_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_KEY0_BMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_DEBUG_KEY_LSB_KEY0_SHFT                                                                          0x0

#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ac)
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_PHYS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041ac)
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_OFFS                                                                        (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041ac)
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_RMSK                                                                        0xffffffff
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_DEBUG_KEY_MSB_ADDR, HWIO_QFPROM_CORR_DEBUG_KEY_MSB_RMSK)
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_DEBUG_KEY_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_KEY1_BMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_DEBUG_KEY_MSB_KEY1_SHFT                                                                          0x0

#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041b0)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041b0)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_MEM_ACCEL_BMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_LSB_MEM_ACCEL_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b4)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041b4)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041b4)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_MEM_ACCEL_BMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW0_MSB_MEM_ACCEL_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b8)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041b8)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041b8)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_RMSK                                                                       0x3fff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_MEM_ACCEL_BMSK                                                             0x3fff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_LSB_MEM_ACCEL_SHFT                                                                0x0

#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041bc)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_PHYS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041bc)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_OFFS                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041bc)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_BMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_QFPROM_CORR_MEM_ACCEL_ROW1_MSB_SHFT                                           0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041c0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041c0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_VREF_B0_BMSK                                                               0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_VREF_B0_SHFT                                                                     0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PH_B0M3_BMSK                                                               0x38000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PH_B0M3_SHFT                                                                     0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PH_B0M2_BMSK                                                                0x7000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PH_B0M2_SHFT                                                                     0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PH_B0M1_BMSK                                                                 0xe00000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PH_B0M1_SHFT                                                                     0x15
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PH_B0M0_BMSK                                                                 0x1c0000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_PH_B0M0_SHFT                                                                     0x12
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_G_B0_BMSK                                                                     0x38000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_G_B0_SHFT                                                                         0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_SAR_B_BMSK                                                                     0x6000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_SAR_B_SHFT                                                                        0xd
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CSI_PHY_BMSK                                                                   0x1f00
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_CSI_PHY_SHFT                                                                      0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_BMSK                                                                     0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_SHFT                                                                      0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSIPHY_PLL_BMSK                                                                   0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSIPHY_PLL_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041c4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041c4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B2M3_BMSK                                                               0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B2M3_SHFT                                                                     0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B2M2_BMSK                                                               0x1c000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B2M2_SHFT                                                                     0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B2M1_BMSK                                                                0x3800000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B2M1_SHFT                                                                     0x17
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B2M0_BMSK                                                                 0x700000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B2M0_SHFT                                                                     0x14
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_G_B2_BMSK                                                                     0xe0000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_G_B2_SHFT                                                                        0x11
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_VREF_B1_BMSK                                                                  0x18000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_VREF_B1_SHFT                                                                      0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B1M3_BMSK                                                                   0x7000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B1M3_SHFT                                                                      0xc
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B1M2_BMSK                                                                    0xe00
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B1M2_SHFT                                                                      0x9
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B1M1_BMSK                                                                    0x1c0
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B1M1_SHFT                                                                      0x6
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B1M0_BMSK                                                                     0x38
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_PH_B1M0_SHFT                                                                      0x3
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_G_B1_BMSK                                                                         0x7
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_G_B1_SHFT                                                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041c8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041c8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041c8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_SPARE0_BMSK                                                                0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_SPARE0_SHFT                                                                      0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_PH_B3M1_BMSK                                                               0x70000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_PH_B3M1_SHFT                                                                     0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_PH_B3M0_BMSK                                                                0xe000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_PH_B3M0_SHFT                                                                     0x19
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_G_B3_BMSK                                                                   0x1c00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_G_B3_SHFT                                                                        0x16
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_VREF_B2_BMSK                                                                 0x300000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_VREF_B2_SHFT                                                                     0x14
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS3_LDO_VREF_TRIM_BMSK                                                      0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS3_LDO_VREF_TRIM_SHFT                                                          0xf
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS2_LDO_VREF_TRIM_BMSK                                                       0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS2_LDO_VREF_TRIM_SHFT                                                          0xa
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS1_LDO_VREF_TRIM_BMSK                                                        0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS1_LDO_VREF_TRIM_SHFT                                                          0x5
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS0_LDO_VREF_TRIM_BMSK                                                         0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_APPS0_LDO_VREF_TRIM_SHFT                                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041cc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041cc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041cc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_BMSK                                                      0xe0000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_SHFT                                                            0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_NO_REDUNDANCY_FVAL                                               0x0
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_CPR_FVAL                                                         0x1
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_BBRX_FVAL                                                        0x2
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_TEMPERATURE_SENSORS_FVAL                                         0x3
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_RESERVED_FVAL                                                    0x4
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_RESERVEDZ_FVAL                                                   0x5
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_RESERVEDZZ_FVAL                                                  0x6
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SW_CAL_REDUN_SEL_NO_REDUNDANCYZ_FVAL                                              0x7
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE0_BMSK                                                                0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_SPARE0_SHFT                                                                      0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CLK_B_BMSK                                                                  0xc000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CLK_B_SHFT                                                                       0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CAP_B_BMSK                                                                  0x3000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_CAP_B_SHFT                                                                       0x18
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_VREF_B3_BMSK                                                                 0xc00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_VREF_B3_SHFT                                                                     0x16
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_PH_B3M3_BMSK                                                                 0x380000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_PH_B3M3_SHFT                                                                     0x13
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_PH_B3M2_BMSK                                                                  0x70000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_PH_B3M2_SHFT                                                                     0x10
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_Q6SS1_LDO_VREF_TRIM_BMSK                                                       0xff00
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_Q6SS1_LDO_VREF_TRIM_SHFT                                                          0x8
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_Q6SS0_LDO_VREF_TRIM_BMSK                                                         0xff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_Q6SS0_LDO_VREF_TRIM_SHFT                                                          0x0

#define HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d0)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_PHYS                                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041d0)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_OFFS                                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041d0)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_BMSK                                                           0x80000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_SHFT                                                                 0x1f
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION31_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_BMSK                                                           0x40000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_SHFT                                                                 0x1e
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION30_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_BMSK                                                           0x20000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_SHFT                                                                 0x1d
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION29_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_BMSK                                                           0x10000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_SHFT                                                                 0x1c
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION28_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                                            0x8000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                                                 0x1b
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                                            0x4000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                                                 0x1a
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                                            0x2000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                                                 0x19
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                                            0x1000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                                                 0x18
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                                             0x800000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                                                 0x17
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                                             0x400000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                                                 0x16
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                                             0x200000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                                                 0x15
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                                             0x100000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                                                 0x14
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                                              0x80000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                                                 0x13
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                                              0x40000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                                                 0x12
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                                              0x20000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                                                 0x11
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                                              0x10000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                                                 0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                                               0x8000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                                                  0xf
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                                               0x4000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                                                  0xe
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                                               0x2000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                                                  0xd
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                                               0x1000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                                                  0xc
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                                                0x800
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                                                  0xb
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                                                0x400
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                                                  0xa
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_DISABLE_FVAL                                                          0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_ENABLE_FVAL                                                           0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                                                 0x200
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                                                   0x9
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                                                 0x100
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                                                   0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                                                  0x80
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                                                   0x7
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                                                  0x40
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                                                   0x6
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                                                  0x20
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                                                   0x5
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                                                  0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                                                   0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                                                   0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                                                   0x3
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                                                   0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                                                   0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                                                   0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                                                   0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_ENABLE_FVAL                                                            0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                                                   0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                                                   0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_DISABLE_FVAL                                                           0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_ENABLE_FVAL                                                            0x1

#define HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d4)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_PHYS                                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041d4)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_OFFS                                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041d4)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK                                                                           0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_QFPROM_CORR_FEC_EN_MSB_BMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_QFPROM_CORR_FEC_EN_MSB_SHFT                                                           0x0

#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041d8)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041d8)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041d8)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_ADDR, HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_RMSK)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_QFPROM_CORR_INTAGLIO_GLUEBIT_LSB_SHFT                                       0x0

#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041dc)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041dc)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041dc)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_ADDR, HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_RMSK)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_QFPROM_CORR_INTAGLIO_GLUEBIT_MSB_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_PHYS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041e0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_OFFS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041e0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_MAXn                                                                             1
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_KEY_DATA_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_LSB_KEY_DATA_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041e4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_PHYS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041e4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_OFFS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041e4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_MAXn                                                                             1
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_KEY_DATA_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_CUST_KEY_ROWn_MSB_KEY_DATA_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f0)
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041f0)
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041f0)
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_RMSK                                                                       0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_LSB_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_LSB_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_SERIAL_NUM_BMSK                                                            0xffffffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_LSB_SERIAL_NUM_SHFT                                                                   0x0

#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_ADDR                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f4)
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_PHYS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041f4)
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_OFFS                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041f4)
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_RMSK                                                                           0xffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_MSB_ADDR, HWIO_QFPROM_CORR_SERIAL_NUM_MSB_RMSK)
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SERIAL_NUM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_CHIP_ID_BMSK                                                                   0xffff
#define HWIO_QFPROM_CORR_SERIAL_NUM_MSB_CHIP_ID_SHFT                                                                      0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f8)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_PHYS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041f8)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_OFFS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041f8)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK                                                                          0xffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_PIL_SUBSYSTEM_47_32_BMSK                                                      0xffff
#define HWIO_QFPROM_CORR_SPARE_REG19_LSB_PIL_SUBSYSTEM_47_32_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041fc)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_PHYS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000041fc)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_OFFS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000041fc)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK                                                                      0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG19_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG19_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE_BMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG19_MSB_SPARE_SHFT                                                                       0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004200 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PHYS(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004200 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_OFFS(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004200 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK                                                                   0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_MAXn                                                                           47
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA0_BMSK                                                       0xffff0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA0_SHFT                                                             0x10
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_ADDR_BMSK                                                            0xffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_ADDR_SHFT                                                               0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004204 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PHYS(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004204 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_OFFS(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004204 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK                                                                      0x1ffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_MAXn                                                                           47
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                                             0x10000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                                                0x10
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_DATA1_BMSK                                                           0xffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_DATA1_SHFT                                                              0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004380 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_PHYS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004380 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OFFS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004380 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                                               3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004384 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_PHYS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004384 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OFFS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004384 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                                        0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                                               3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                                              0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043a0)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043a0)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OFFS                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043a0)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043a4)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043a4)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OFFS                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043a4)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_BMSK          0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_SHFT                 0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_PHYS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OFFS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043a8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                                               3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_PHYS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OFFS(n)                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043ac + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                                        0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                                               3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                                              0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043c8)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043c8)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OFFS                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043c8)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043cc)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_PHYS                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043cc)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OFFS                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043cc)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_BMSK          0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_SHFT                 0x0

#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043d0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_PHYS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043d0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_OFFS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043d0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_MAXn                                                                          3
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_LSB_HASH_DATA0_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043d4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_PHYS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043d4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_OFFS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043d4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_RMSK                                                                   0xffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_MAXn                                                                          3
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_HASH_DATA1_BMSK                                                        0xffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROWn_MSB_HASH_DATA1_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043f0)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043f0)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043f0)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_HASH_DATA0_BMSK                                                      0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_LSB_HASH_DATA0_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043f4)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043f4)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043f4)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_BMSK                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_QFPROM_CORR_OEM_PK_HASH_ROW4_MSB_SHFT                                       0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043f8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_PHYS(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043f8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_OFFS(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043f8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK                                                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                                         3
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                                      0xff000000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                                            0x18
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                                        0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                                            0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                                          0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                                             0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                                            0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000043fc + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_PHYS(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000043fc + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_OFFS(n)                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000043fc + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK                                                                  0xffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                                         3
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                                        0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                                            0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                                          0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                                             0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                                            0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                                             0x0

#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004418 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_PHYS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004418 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_OFFS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004418 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_MAXn                                                                          3
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                                       0xff000000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                                             0x18
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                                         0xff0000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                                             0x10
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                                           0xff00
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                                              0x8
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                                             0xff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                                              0x0

#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000441c + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_PHYS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000441c + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_OFFS(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000441c + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_RMSK                                                                   0xffffff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_MAXn                                                                          3
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                                         0xff0000
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                                             0x10
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                                           0xff00
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                                              0x8
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                                             0xff
#define HWIO_QFPROM_CORR_QC_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                                              0x0

#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004438)
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PHYS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004438)
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_OFFS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004438)
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_RMSK                                                                      0xffffffff
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_USB_VID_PID_LSB_ADDR, HWIO_QFPROM_CORR_USB_VID_PID_LSB_RMSK)
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_USB_VID_PID_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PID1_BMSK                                                                 0xffff0000
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PID1_SHFT                                                                       0x10
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PID0_BMSK                                                                     0xffff
#define HWIO_QFPROM_CORR_USB_VID_PID_LSB_PID0_SHFT                                                                        0x0

#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000443c)
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_PHYS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000443c)
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_OFFS                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000443c)
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_RMSK                                                                          0xffff
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_USB_VID_PID_MSB_ADDR, HWIO_QFPROM_CORR_USB_VID_PID_MSB_RMSK)
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_USB_VID_PID_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_VID_BMSK                                                                      0xffff
#define HWIO_QFPROM_CORR_USB_VID_PID_MSB_VID_SHFT                                                                         0x0

#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004440)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004440)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004440)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_SW_CAL_REDUN_72_41_BMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_LSB_SW_CAL_REDUN_72_41_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004444)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004444)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004444)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_RMSK                                                                 0x7fffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_SW_FEC_VALUE_BMSK                                                    0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_SW_FEC_VALUE_SHFT                                                          0x18
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_SW_CAL_REDUN_96_73_BMSK                                                0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW0_MSB_SW_CAL_REDUN_96_73_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004448)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004448)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004448)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_SW_CAL_REDUN_128_97_BMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_LSB_SW_CAL_REDUN_128_97_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000444c)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000444c)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000444c)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_RMSK                                                                 0x7fffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_SW_FEC_VALUE_BMSK                                                    0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_SW_FEC_VALUE_SHFT                                                          0x18
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_SW_CAL_REDUN_152_129_BMSK                                              0xffffff
#define HWIO_QFPROM_CORR_SPARE_REG27_ROW1_MSB_SW_CAL_REDUN_152_129_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004450)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004450)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004450)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_QUOT_IND_BMSK                                                   0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_QUOT_IND_SHFT                                                         0x1f
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_DIS_CPR_BMSK                                                    0x40000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_DIS_CPR_SHFT                                                          0x1e
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_DIS_CPR_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_DIS_CPR_DISABLE_FVAL                                                   0x1
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_QUOT_VMIN_BMSK                                          0x3ffc0000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_QUOT_VMIN_SHFT                                                0x12
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_STEP_QUOT_BMSK                                             0x3fc00
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_STEP_QUOT_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_TARGET_BMSK                                                  0x3e0
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_NOMINAL_TARGET_SHFT                                                    0x5
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_TURBO_TARGET_BMSK                                                     0x1f
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_LSB_CPR0_TURBO_TARGET_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004454)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004454)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004454)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_RMSK                                                                 0x7fffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_SW_FEC_VALUE_BMSK                                                    0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_SW_FEC_VALUE_SHFT                                                          0x18
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_SPARE0_BMSK                                                            0xf80000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_SPARE0_SHFT                                                                0x13
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_CPR0_SVS_ROSEL_BMSK                                                     0x70000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_CPR0_SVS_ROSEL_SHFT                                                        0x10
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_CPR0_SVS_STEP_QUOT_BMSK                                                  0xff00
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_CPR0_SVS_STEP_QUOT_SHFT                                                     0x8
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_CPR0_SVS_TARGET_BMSK                                                       0xf8
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_CPR0_SVS_TARGET_SHFT                                                        0x3
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_CPR0_NOMINAL_ROSEL_BMSK                                                     0x7
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW0_MSB_CPR0_NOMINAL_ROSEL_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004458)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004458)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004458)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_QUOT_IND_BMSK                                                   0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_QUOT_IND_SHFT                                                         0x1f
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_BMSK                                                    0x40000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_SHFT                                                          0x1e
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_DIS_CPR_DISABLE_FVAL                                                   0x1
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_QUOT_VMIN_BMSK                                          0x3ffc0000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_QUOT_VMIN_SHFT                                                0x12
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_STEP_QUOT_BMSK                                             0x3fc00
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_STEP_QUOT_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_TARGET_BMSK                                                  0x3e0
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_NOMINAL_TARGET_SHFT                                                    0x5
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_TURBO_TARGET_BMSK                                                     0x1f
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_LSB_CPR1_TURBO_TARGET_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000445c)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000445c)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000445c)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_RMSK                                                                 0x7fffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_SW_FEC_VALUE_BMSK                                                    0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_SW_FEC_VALUE_SHFT                                                          0x18
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_SPARE0_BMSK                                                            0xf80000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_SPARE0_SHFT                                                                0x13
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_CPR1_SVS_ROSEL_BMSK                                                     0x70000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_CPR1_SVS_ROSEL_SHFT                                                        0x10
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_CPR1_SVS_STEP_QUOT_BMSK                                                  0xff00
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_CPR1_SVS_STEP_QUOT_SHFT                                                     0x8
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_CPR1_SVS_TARGET_BMSK                                                       0xf8
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_CPR1_SVS_TARGET_SHFT                                                        0x3
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_CPR1_NOMINAL_ROSEL_BMSK                                                     0x7
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW1_MSB_CPR1_NOMINAL_ROSEL_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004460)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004460)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004460)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_RMSK                                                                 0xffffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_QUOT_IND_BMSK                                                   0x80000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_QUOT_IND_SHFT                                                         0x1f
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_DIS_CPR_BMSK                                                    0x40000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_DIS_CPR_SHFT                                                          0x1e
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_DIS_CPR_ENABLE_FVAL                                                    0x0
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_DIS_CPR_DISABLE_FVAL                                                   0x1
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_QUOT_VMIN_BMSK                                          0x3ffc0000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_QUOT_VMIN_SHFT                                                0x12
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_STEP_QUOT_BMSK                                             0x3fc00
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_STEP_QUOT_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_TARGET_BMSK                                                  0x3e0
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_NOMINAL_TARGET_SHFT                                                    0x5
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_TURBO_TARGET_BMSK                                                     0x1f
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_LSB_CPR2_TURBO_TARGET_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004464)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_PHYS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004464)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004464)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_RMSK                                                                 0x7fffffff
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_SW_FEC_VALUE_BMSK                                                    0x7f000000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_SW_FEC_VALUE_SHFT                                                          0x18
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_SPARE0_BMSK                                                            0xf80000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_SPARE0_SHFT                                                                0x13
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_CPR2_SVS_ROSEL_BMSK                                                     0x70000
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_CPR2_SVS_ROSEL_SHFT                                                        0x10
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_CPR2_SVS_STEP_QUOT_BMSK                                                  0xff00
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_CPR2_SVS_STEP_QUOT_SHFT                                                     0x8
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_CPR2_SVS_TARGET_BMSK                                                       0xf8
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_CPR2_SVS_TARGET_SHFT                                                        0x3
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_CPR2_NOMINAL_ROSEL_BMSK                                                     0x7
#define HWIO_QFPROM_CORR_SPARE_REG28_ROW2_MSB_CPR2_NOMINAL_ROSEL_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004468)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004468)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004468)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_RMSK                                                                         0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR, HWIO_QFPROM_CORR_HDCP_KSV_LSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_KSV0_BMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KSV_LSB_KSV0_SHFT                                                                           0x0

#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000446c)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_PHYS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000446c)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_OFFS                                                                         (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000446c)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_RMSK                                                                               0xff
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR, HWIO_QFPROM_CORR_HDCP_KSV_MSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KSV_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_KSV1_BMSK                                                                          0xff
#define HWIO_QFPROM_CORR_HDCP_KSV_MSB_KSV1_SHFT                                                                           0x0

#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004468 + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_PHYS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00004468 + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_OFFS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00004468 + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_RMSK                                                                    0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_MAXn                                                                            40
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_KEY_DATA_BMSK                                                           0xffffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_LSB_KEY_DATA_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000446c + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_PHYS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000446c + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_OFFS(n)                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000446c + 0x8 * (n))
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_RMSK                                                                      0xffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_MAXn                                                                            40
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_KEY_DATA_BMSK                                                             0xffffff
#define HWIO_QFPROM_CORR_HDCP_KEY_ROWn_MSB_KEY_DATA_SHFT                                                                  0x0

#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000045b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_PHYS(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000045b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_OFFS(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000045b0 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_RMSK                                                              0xffffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_MAXn                                                                      36
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_KEY_DATA_BMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_LSB_KEY_DATA_SHFT                                                            0x0

#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000045b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_PHYS(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000045b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_OFFS(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000045b4 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_RMSK                                                                0xffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_MAXn                                                                      36
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_KEY_DATA_BMSK                                                       0xffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY0_ROWn_MSB_KEY_DATA_SHFT                                                            0x0

#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000046d8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_PHYS(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000046d8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_OFFS(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000046d8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_RMSK                                                              0xffffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_MAXn                                                                      36
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_KEY_DATA_BMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_LSB_KEY_DATA_SHFT                                                            0x0

#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000046dc + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_PHYS(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000046dc + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_OFFS(n)                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000046dc + 0x8 * (n))
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_RMSK                                                                0xffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_MAXn                                                                      36
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_KEY_DATA_BMSK                                                       0xffffff
#define HWIO_QFPROM_CORR_CUST_PRIV_KEY1_ROWn_MSB_KEY_DATA_SHFT                                                            0x0

#define HWIO_SEC_CTRL_HW_VERSION_ADDR                                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_PHYS                                                                              (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_OFFS                                                                              (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_RMSK                                                                              0xffffffff
#define HWIO_SEC_CTRL_HW_VERSION_IN          \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, HWIO_SEC_CTRL_HW_VERSION_RMSK)
#define HWIO_SEC_CTRL_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, m)
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_BMSK                                                                        0xf0000000
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_SHFT                                                                              0x1c
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_BMSK                                                                         0xfff0000
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_SHFT                                                                              0x10
#define HWIO_SEC_CTRL_HW_VERSION_STEP_BMSK                                                                             0xffff
#define HWIO_SEC_CTRL_HW_VERSION_STEP_SHFT                                                                                0x0

#define HWIO_FEATURE_CONFIG0_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006004)
#define HWIO_FEATURE_CONFIG0_PHYS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006004)
#define HWIO_FEATURE_CONFIG0_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006004)
#define HWIO_FEATURE_CONFIG0_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG0_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, HWIO_FEATURE_CONFIG0_RMSK)
#define HWIO_FEATURE_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, m)
#define HWIO_FEATURE_CONFIG0_HDCP_DISABLE_BMSK                                                                     0x80000000
#define HWIO_FEATURE_CONFIG0_HDCP_DISABLE_SHFT                                                                           0x1f
#define HWIO_FEATURE_CONFIG0_HDCP_DISABLE_ENABLE_FVAL                                                                     0x0
#define HWIO_FEATURE_CONFIG0_HDCP_DISABLE_DISABLE_FVAL                                                                    0x1
#define HWIO_FEATURE_CONFIG0_EDP_DISABLE_BMSK                                                                      0x40000000
#define HWIO_FEATURE_CONFIG0_EDP_DISABLE_SHFT                                                                            0x1e
#define HWIO_FEATURE_CONFIG0_EDP_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG0_EDP_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_BMSK                                                                    0x20000000
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_SHFT                                                                          0x1d
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_FEATURE_CONFIG0_DSI_1_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_FEATURE_CONFIG0_ISP_1_DISABLE_BMSK                                                                    0x10000000
#define HWIO_FEATURE_CONFIG0_ISP_1_DISABLE_SHFT                                                                          0x1c
#define HWIO_FEATURE_CONFIG0_ISP_1_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_FEATURE_CONFIG0_ISP_1_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_BMSK                                                               0x8000000
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_SHFT                                                                    0x1b
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_ENABLE_FVAL                                                              0x0
#define HWIO_FEATURE_CONFIG0_VP8_DECODER_DISABLE_DISABLE_FVAL                                                             0x1
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_BMSK                                                               0x4000000
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_SHFT                                                                    0x1a
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_ENABLE_FVAL                                                              0x0
#define HWIO_FEATURE_CONFIG0_VP8_ENCODER_DISABLE_DISABLE_FVAL                                                             0x1
#define HWIO_FEATURE_CONFIG0_CSID_3_DISABLE_BMSK                                                                    0x2000000
#define HWIO_FEATURE_CONFIG0_CSID_3_DISABLE_SHFT                                                                         0x19
#define HWIO_FEATURE_CONFIG0_CSID_3_DISABLE_ENABLE_FVAL                                                                   0x0
#define HWIO_FEATURE_CONFIG0_CSID_3_DISABLE_DISABLE_FVAL                                                                  0x1
#define HWIO_FEATURE_CONFIG0_CSID_2_DISABLE_BMSK                                                                    0x1000000
#define HWIO_FEATURE_CONFIG0_CSID_2_DISABLE_SHFT                                                                         0x18
#define HWIO_FEATURE_CONFIG0_CSID_2_DISABLE_ENABLE_FVAL                                                                   0x0
#define HWIO_FEATURE_CONFIG0_CSID_2_DISABLE_DISABLE_FVAL                                                                  0x1
#define HWIO_FEATURE_CONFIG0_MACROVISION_DISABLE_BMSK                                                                0x800000
#define HWIO_FEATURE_CONFIG0_MACROVISION_DISABLE_SHFT                                                                    0x17
#define HWIO_FEATURE_CONFIG0_MACROVISION_DISABLE_ENABLE_FVAL                                                              0x0
#define HWIO_FEATURE_CONFIG0_MACROVISION_DISABLE_DISABLE_FVAL                                                             0x1
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_BMSK                                                                0x700000
#define HWIO_FEATURE_CONFIG0_MODEM_SPARE_DISABLE_SHFT                                                                    0x14
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_BMSK                                                                 0x80000
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_SHFT                                                                    0x13
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_ENABLE_FVAL                                                              0x0
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX3_DISABLE_DISABLE_FVAL                                                             0x1
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_BMSK                                                                 0x40000
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_SHFT                                                                    0x12
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_ENABLE_FVAL                                                              0x0
#define HWIO_FEATURE_CONFIG0_MODEM_BBRX2_DISABLE_DISABLE_FVAL                                                             0x1
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_BMSK                                                                0x20000
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_SHFT                                                                   0x11
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG0_MODEM_GLOBAL_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_BMSK                                                               0x10000
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_SHFT                                                                  0x10
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_ENABLE_FVAL                                                            0x0
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DC_DISABLE_DISABLE_FVAL                                                           0x1
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_BMSK                                                              0x8000
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_SHFT                                                                 0xf
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_ENABLE_FVAL                                                          0x0
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_MIMO_DISABLE_DISABLE_FVAL                                                         0x1
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_BMSK                                                         0x4000
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_SHFT                                                            0xe
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT2_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_BMSK                                                         0x2000
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_SHFT                                                            0xd
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_ENABLE_FVAL                                                     0x0
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_ABOVE_CAT1_DISABLE_DISABLE_FVAL                                                    0x1
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_BMSK                                                                0x1000
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_SHFT                                                                   0xc
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_ENABLE_FVAL                                                            0x0
#define HWIO_FEATURE_CONFIG0_MODEM_TDSCDMA_DISABLE_DISABLE_FVAL                                                           0x1
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_BMSK                                                                    0x800
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_SHFT                                                                      0xb
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_ENABLE_FVAL                                                               0x0
#define HWIO_FEATURE_CONFIG0_MODEM_HSPA_DISABLE_DISABLE_FVAL                                                              0x1
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_BMSK                                                                     0x400
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_SHFT                                                                       0xa
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_ENABLE_FVAL                                                                0x0
#define HWIO_FEATURE_CONFIG0_MODEM_LTE_DISABLE_DISABLE_FVAL                                                               0x1
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_BMSK                                                                   0x200
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_SHFT                                                                     0x9
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_ENABLE_FVAL                                                              0x0
#define HWIO_FEATURE_CONFIG0_MODEM_WCDMA_DISABLE_DISABLE_FVAL                                                             0x1
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_BMSK                                                                      0x100
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_SHFT                                                                        0x8
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_ENABLE_FVAL                                                                 0x0
#define HWIO_FEATURE_CONFIG0_MODEM_DO_DISABLE_DISABLE_FVAL                                                                0x1
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_BMSK                                                                       0x80
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_SHFT                                                                        0x7
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_ENABLE_FVAL                                                                 0x0
#define HWIO_FEATURE_CONFIG0_MODEM_1X_DISABLE_DISABLE_FVAL                                                                0x1
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_BMSK                                                                            0x40
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_SHFT                                                                             0x6
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG0_NAV_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_BMSK                                                                            0x20
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_SHFT                                                                             0x5
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG0_GSM_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG0_SPI_SLAVE_DISABLE_BMSK                                                                      0x10
#define HWIO_FEATURE_CONFIG0_SPI_SLAVE_DISABLE_SHFT                                                                       0x4
#define HWIO_FEATURE_CONFIG0_SPI_SLAVE_DISABLE_ENABLE_FVAL                                                                0x0
#define HWIO_FEATURE_CONFIG0_SPI_SLAVE_DISABLE_DISABLE_FVAL                                                               0x1
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_BMSK                                                                        0x8
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_SHFT                                                                        0x3
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_ENABLE_FVAL                                                                 0x0
#define HWIO_FEATURE_CONFIG0_APPS_ACG_DISABLE_DISABLE_FVAL                                                                0x1
#define HWIO_FEATURE_CONFIG0_BOOT_ROM_PATCH_DISABLE_BMSK                                                                  0x7
#define HWIO_FEATURE_CONFIG0_BOOT_ROM_PATCH_DISABLE_SHFT                                                                  0x0
#define HWIO_FEATURE_CONFIG0_BOOT_ROM_PATCH_DISABLE_ENABLE_PATCHING_FVAL                                                  0x0
#define HWIO_FEATURE_CONFIG0_BOOT_ROM_PATCH_DISABLE_DISABLE_PATCHING_FVAL                                                 0x1

#define HWIO_FEATURE_CONFIG1_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_PHYS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006008)
#define HWIO_FEATURE_CONFIG1_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_VENDOR_LOCK_BMSK                                                                      0xf0000000
#define HWIO_FEATURE_CONFIG1_VENDOR_LOCK_SHFT                                                                            0x1c
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_BMSK                                                       0x8000000
#define HWIO_FEATURE_CONFIG1_SDC_EMMC_MODE1P2_FORCE_GPIO_SHFT                                                            0x1b
#define HWIO_FEATURE_CONFIG1_SPARE0_BMSK                                                                            0x7ff8000
#define HWIO_FEATURE_CONFIG1_SPARE0_SHFT                                                                                  0xf
#define HWIO_FEATURE_CONFIG1_MULTIMEDIA_SPARE_DISABLE_BMSK                                                             0x7800
#define HWIO_FEATURE_CONFIG1_MULTIMEDIA_SPARE_DISABLE_SHFT                                                                0xb
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_BMSK                                                                          0x400
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_SHFT                                                                            0xa
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_ENABLE_FVAL                                                                     0x0
#define HWIO_FEATURE_CONFIG1_PCIE_DISABLE_DISABLE_FVAL                                                                    0x1
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_BMSK                                                                          0x200
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_SHFT                                                                            0x9
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_ENABLE_FVAL                                                                     0x0
#define HWIO_FEATURE_CONFIG1_SATA_DISABLE_DISABLE_FVAL                                                                    0x1
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_BMSK                                                                           0x100
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_SHFT                                                                             0x8
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG1_ACC_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_BMSK                                                                            0x80
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_SHFT                                                                             0x7
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG1_VPE_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_BMSK                                                                            0x40
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_SHFT                                                                             0x6
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG1_VFE_DISABLE_DISABLE_FVAL                                                                     0x1
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_BMSK                                                                   0x20
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_SHFT                                                                    0x5
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG1_JPEG_ENCODER_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_BMSK                                                                   0x10
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_SHFT                                                                    0x4
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG1_JPEG_DECODER_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_BMSK                                                                    0x8
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_SHFT                                                                    0x3
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG1_FIELD_3D_GRP_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_BMSK                                                                   0x4
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_SHFT                                                                   0x2
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_ENABLE_FVAL                                                            0x0
#define HWIO_FEATURE_CONFIG1_VIDEO_ENCODER_DISABLE_DISABLE_FVAL                                                           0x1
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_BMSK                                                                     0x2
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_SHFT                                                                     0x1
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_ENABLE_FVAL                                                              0x0
#define HWIO_FEATURE_CONFIG1_VIDEO_CODEC_DISABLE_DISABLE_FVAL                                                             0x1
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_BMSK                                                                            0x1
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_SHFT                                                                            0x0
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_ENABLE_FVAL                                                                     0x0
#define HWIO_FEATURE_CONFIG1_HDMI_DISABLE_DISABLE_FVAL                                                                    0x1

#define HWIO_FEATURE_CONFIG2_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_PHYS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_SPARE1_BMSK                                                                           0xffc00000
#define HWIO_FEATURE_CONFIG2_SPARE1_SHFT                                                                                 0x16
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_BMSK                                                            0x200000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_SHFT                                                                0x15
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                                          0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                                         0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_BMSK                                                             0x100000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_SHFT                                                                 0x14
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                                           0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                                          0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_BMSK                                                               0x80000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_SHFT                                                                  0x13
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                                            0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                                           0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_BMSK                                                                0x40000
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_SHFT                                                                   0x12
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_BMSK                                                                0x20000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_SHFT                                                                   0x11
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_BMSK                                                             0x10000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_SHFT                                                                0x10
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                                          0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                                         0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_BMSK                                                               0x8000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_SHFT                                                                  0xf
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                                           0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                                          0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_BMSK                                                                0x4000
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_SHFT                                                                   0xe
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                                                            0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                                                           0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_BMSK                                                                0x2000
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_SHFT                                                                   0xd
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                                                            0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                                                           0x1
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_BMSK                                                                    0x1000
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_SHFT                                                                       0xc
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_BMSK                                                                     0x800
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_SHFT                                                                       0xb
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_BMSK                                                              0x400
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_SHFT                                                                0xa
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                                         0x0
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                                        0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_BMSK                                                                  0x200
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_SHFT                                                                    0x9
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                                       0x100
#define HWIO_FEATURE_CONFIG2_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                                         0x8
#define HWIO_FEATURE_CONFIG2_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                                  0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_LPASS_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                                 0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_BMSK                                                                   0x80
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_SHFT                                                                    0x7
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_BMSK                                                                  0x40
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_SHFT                                                                   0x6
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                                            0x0
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                                           0x1
#define HWIO_FEATURE_CONFIG2_QC_LPASS_DBGEN_DISABLE_BMSK                                                                 0x20
#define HWIO_FEATURE_CONFIG2_QC_LPASS_DBGEN_DISABLE_SHFT                                                                  0x5
#define HWIO_FEATURE_CONFIG2_QC_LPASS_DBGEN_DISABLE_ENABLE_FVAL                                                           0x0
#define HWIO_FEATURE_CONFIG2_QC_LPASS_DBGEN_DISABLE_DISABLE_FVAL                                                          0x1
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_BMSK                                                                   0x10
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_SHFT                                                                    0x4
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_BMSK                                                                    0x8
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_SHFT                                                                    0x3
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_ENABLE_FVAL                                                             0x0
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_DISABLE_FVAL                                                            0x1
#define HWIO_FEATURE_CONFIG2_SPARE0_BMSK                                                                                  0x4
#define HWIO_FEATURE_CONFIG2_SPARE0_SHFT                                                                                  0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_BMSK                                                                         0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_SHFT                                                                         0x1
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_ENABLE_FVAL                                                                  0x0
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_DISABLE_FVAL                                                                 0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_BMSK                                                                             0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_SHFT                                                                             0x0
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_ENABLE_FVAL                                                                      0x0
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_DISABLE_FVAL                                                                     0x1

#define HWIO_FEATURE_CONFIG3_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006010)
#define HWIO_FEATURE_CONFIG3_PHYS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006010)
#define HWIO_FEATURE_CONFIG3_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006010)
#define HWIO_FEATURE_CONFIG3_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG3_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, HWIO_FEATURE_CONFIG3_RMSK)
#define HWIO_FEATURE_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, m)
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_BMSK                                                           0xfffc0000
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_SHFT                                                                 0x12
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_BMSK                                                            0x3c000
#define HWIO_FEATURE_CONFIG3_TAP_CJI_CORE_SEL_DISABLE_SHFT                                                                0xe
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_BMSK                                                                    0x3fff
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_SHFT                                                                       0x0

#define HWIO_FEATURE_CONFIG4_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006014)
#define HWIO_FEATURE_CONFIG4_PHYS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006014)
#define HWIO_FEATURE_CONFIG4_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006014)
#define HWIO_FEATURE_CONFIG4_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG4_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, HWIO_FEATURE_CONFIG4_RMSK)
#define HWIO_FEATURE_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, m)
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_BMSK                                                      0xffffffff
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_SHFT                                                             0x0

#define HWIO_FEATURE_CONFIG5_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006018)
#define HWIO_FEATURE_CONFIG5_PHYS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006018)
#define HWIO_FEATURE_CONFIG5_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006018)
#define HWIO_FEATURE_CONFIG5_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG5_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, HWIO_FEATURE_CONFIG5_RMSK)
#define HWIO_FEATURE_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, m)
#define HWIO_FEATURE_CONFIG5_SW_EXPANSION_FUSES_BMSK                                                               0xff000000
#define HWIO_FEATURE_CONFIG5_SW_EXPANSION_FUSES_SHFT                                                                     0x18
#define HWIO_FEATURE_CONFIG5_SPARE0_BMSK                                                                             0xf00000
#define HWIO_FEATURE_CONFIG5_SPARE0_SHFT                                                                                 0x14
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_BMSK                                                                0xf0000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_SHFT                                                                   0x10
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION3_BMSK                                                                  0xf000
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION3_SHFT                                                                     0xc
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION2_BMSK                                                                   0xf00
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION2_SHFT                                                                     0x8
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION1_BMSK                                                                    0xf0
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION1_SHFT                                                                     0x4
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION0_BMSK                                                                     0xf
#define HWIO_FEATURE_CONFIG5_APPS_CONFIGURATION0_SHFT                                                                     0x0

#define HWIO_FEATURE_CONFIG6_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000601c)
#define HWIO_FEATURE_CONFIG6_PHYS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000601c)
#define HWIO_FEATURE_CONFIG6_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000601c)
#define HWIO_FEATURE_CONFIG6_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG6_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG6_ADDR, HWIO_FEATURE_CONFIG6_RMSK)
#define HWIO_FEATURE_CONFIG6_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG6_ADDR, m)
#define HWIO_FEATURE_CONFIG6_MDSP_FW_DISABLE_BMSK                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG6_MDSP_FW_DISABLE_SHFT                                                                         0x0
#define HWIO_FEATURE_CONFIG6_MDSP_FW_DISABLE_ENABLE_FVAL                                                                  0x0
#define HWIO_FEATURE_CONFIG6_MDSP_FW_DISABLE_DISABLE_FVAL                                                                 0x1

#define HWIO_FEATURE_CONFIG7_ADDR                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006020)
#define HWIO_FEATURE_CONFIG7_PHYS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006020)
#define HWIO_FEATURE_CONFIG7_OFFS                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006020)
#define HWIO_FEATURE_CONFIG7_RMSK                                                                                  0xffffffff
#define HWIO_FEATURE_CONFIG7_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG7_ADDR, HWIO_FEATURE_CONFIG7_RMSK)
#define HWIO_FEATURE_CONFIG7_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG7_ADDR, m)
#define HWIO_FEATURE_CONFIG7_SPARE0_BMSK                                                                           0xffff0000
#define HWIO_FEATURE_CONFIG7_SPARE0_SHFT                                                                                 0x10
#define HWIO_FEATURE_CONFIG7_RPM_PBL_BOOT_SPEED_BMSK                                                                   0xc000
#define HWIO_FEATURE_CONFIG7_RPM_PBL_BOOT_SPEED_SHFT                                                                      0xe
#define HWIO_FEATURE_CONFIG7_RPM_PBL_BOOT_SPEED_XO_FVAL                                                                   0x0
#define HWIO_FEATURE_CONFIG7_RPM_PBL_BOOT_SPEED_ENUM_75_MHZ_FVAL                                                          0x1
#define HWIO_FEATURE_CONFIG7_RPM_PBL_BOOT_SPEED_ENUM_75_MHZZ_FVAL                                                         0x2
#define HWIO_FEATURE_CONFIG7_RPM_PBL_BOOT_SPEED_ENUM_75_MHZZZ_FVAL                                                        0x3
#define HWIO_FEATURE_CONFIG7_RPM_BOOT_FROM_ROM_BMSK                                                                    0x2000
#define HWIO_FEATURE_CONFIG7_RPM_BOOT_FROM_ROM_SHFT                                                                       0xd
#define HWIO_FEATURE_CONFIG7_RPM_BOOT_FROM_ROM_BOOT_FROM_RPM_CODE_RAM_ALLOWED_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG7_RPM_BOOT_FROM_ROM_FORCE_BOOT_FROM_ROM_FVAL                                                   0x1
#define HWIO_FEATURE_CONFIG7_APPS_BOOT_FROM_ROM_BMSK                                                                   0x1000
#define HWIO_FEATURE_CONFIG7_APPS_BOOT_FROM_ROM_SHFT                                                                      0xc
#define HWIO_FEATURE_CONFIG7_MSA_ENA_BMSK                                                                               0x800
#define HWIO_FEATURE_CONFIG7_MSA_ENA_SHFT                                                                                 0xb
#define HWIO_FEATURE_CONFIG7_FORCE_MSA_AUTH_EN_BMSK                                                                     0x400
#define HWIO_FEATURE_CONFIG7_FORCE_MSA_AUTH_EN_SHFT                                                                       0xa
#define HWIO_FEATURE_CONFIG7_FORCE_MSA_AUTH_EN_MODEM_IMAGE_NOT_AUTHENTICATED_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG7_FORCE_MSA_AUTH_EN_FORCE_MODEM_IMAGE_AUTHENTICATION_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG7_FLCB_SKIP_BMSK                                                                             0x200
#define HWIO_FEATURE_CONFIG7_FLCB_SKIP_SHFT                                                                               0x9
#define HWIO_FEATURE_CONFIG7_QCOM_PMIC_FEATURE_ENABLE_BMSK                                                              0x100
#define HWIO_FEATURE_CONFIG7_QCOM_PMIC_FEATURE_ENABLE_SHFT                                                                0x8
#define HWIO_FEATURE_CONFIG7_QCOM_PMIC_FEATURE_ENABLE_QC_PMIC_FEATURES_DISABLED_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG7_QCOM_PMIC_FEATURE_ENABLE_QC_PMIC_FEATURES_ENABLED_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG7_BOOT_ROM_CFG_BMSK                                                                           0xe0
#define HWIO_FEATURE_CONFIG7_BOOT_ROM_CFG_SHFT                                                                            0x5
#define HWIO_FEATURE_CONFIG7_STACKED_MEMORY_ID_BMSK                                                                      0x1f
#define HWIO_FEATURE_CONFIG7_STACKED_MEMORY_ID_SHFT                                                                       0x0

#define HWIO_OEM_CONFIG0_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006024)
#define HWIO_OEM_CONFIG0_PHYS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006024)
#define HWIO_OEM_CONFIG0_OFFS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006024)
#define HWIO_OEM_CONFIG0_RMSK                                                                                      0xffffffff
#define HWIO_OEM_CONFIG0_IN          \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, HWIO_OEM_CONFIG0_RMSK)
#define HWIO_OEM_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, m)
#define HWIO_OEM_CONFIG0_SPARE1_DISABLE_BMSK                                                                       0x80000000
#define HWIO_OEM_CONFIG0_SPARE1_DISABLE_SHFT                                                                             0x1f
#define HWIO_OEM_CONFIG0_SPARE0_DISABLE_BMSK                                                                       0x40000000
#define HWIO_OEM_CONFIG0_SPARE0_DISABLE_SHFT                                                                             0x1e
#define HWIO_OEM_CONFIG0_VENUS_0_DBGEN_DISABLE_BMSK                                                                0x20000000
#define HWIO_OEM_CONFIG0_VENUS_0_DBGEN_DISABLE_SHFT                                                                      0x1d
#define HWIO_OEM_CONFIG0_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                                                0x0
#define HWIO_OEM_CONFIG0_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                                               0x1
#define HWIO_OEM_CONFIG0_RPM_DAPEN_DISABLE_BMSK                                                                    0x10000000
#define HWIO_OEM_CONFIG0_RPM_DAPEN_DISABLE_SHFT                                                                          0x1c
#define HWIO_OEM_CONFIG0_RPM_DAPEN_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_OEM_CONFIG0_RPM_DAPEN_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG0_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                                          0x8000000
#define HWIO_OEM_CONFIG0_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                                               0x1b
#define HWIO_OEM_CONFIG0_RPM_LPASS_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                                         0x0
#define HWIO_OEM_CONFIG0_RPM_LPASS_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                                        0x1
#define HWIO_OEM_CONFIG0_RPM_DBGEN_DISABLE_BMSK                                                                     0x4000000
#define HWIO_OEM_CONFIG0_RPM_DBGEN_DISABLE_SHFT                                                                          0x1a
#define HWIO_OEM_CONFIG0_RPM_DBGEN_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_OEM_CONFIG0_RPM_DBGEN_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG0_WCSS_DBGEN_DISABLE_BMSK                                                                    0x2000000
#define HWIO_OEM_CONFIG0_WCSS_DBGEN_DISABLE_SHFT                                                                         0x19
#define HWIO_OEM_CONFIG0_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                                                   0x0
#define HWIO_OEM_CONFIG0_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                                                  0x1
#define HWIO_OEM_CONFIG0_LPASS_DBGEN_DISABLE_BMSK                                                                   0x1000000
#define HWIO_OEM_CONFIG0_LPASS_DBGEN_DISABLE_SHFT                                                                        0x18
#define HWIO_OEM_CONFIG0_LPASS_DBGEN_DISABLE_ENABLE_FVAL                                                                  0x0
#define HWIO_OEM_CONFIG0_LPASS_DBGEN_DISABLE_DISABLE_FVAL                                                                 0x1
#define HWIO_OEM_CONFIG0_MSS_NIDEN_DISABLE_BMSK                                                                      0x800000
#define HWIO_OEM_CONFIG0_MSS_NIDEN_DISABLE_SHFT                                                                          0x17
#define HWIO_OEM_CONFIG0_MSS_NIDEN_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_OEM_CONFIG0_MSS_NIDEN_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG0_MSS_DBGEN_DISABLE_BMSK                                                                      0x400000
#define HWIO_OEM_CONFIG0_MSS_DBGEN_DISABLE_SHFT                                                                          0x16
#define HWIO_OEM_CONFIG0_MSS_DBGEN_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_OEM_CONFIG0_MSS_DBGEN_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_BMSK                                                                      0x200000
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_SHFT                                                                          0x15
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG0_SPARE1_BMSK                                                                                 0x1e0000
#define HWIO_OEM_CONFIG0_SPARE1_SHFT                                                                                     0x11
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_BMSK                                                           0x10000
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_GPIO_DISABLE_SHFT                                                              0x10
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_BMSK                                                                      0x8000
#define HWIO_OEM_CONFIG0_SDC_EMMC_MODE1P2_EN_SHFT                                                                         0xf
#define HWIO_OEM_CONFIG0_FAST_BOOT_BMSK                                                                                0x7c00
#define HWIO_OEM_CONFIG0_FAST_BOOT_SHFT                                                                                   0xa
#define HWIO_OEM_CONFIG0_FAST_BOOT_DEFAULT_FVAL                                                                           0x0
#define HWIO_OEM_CONFIG0_FAST_BOOT_SD_SDC2_EMMC_SDC1_FVAL                                                                 0x1
#define HWIO_OEM_CONFIG0_FAST_BOOT_EMMC_SDC1_FVAL                                                                         0x2
#define HWIO_OEM_CONFIG0_FAST_BOOT_USB_FVAL                                                                               0x3
#define HWIO_OEM_CONFIG0_FAST_BOOT_HSIC_FVAL                                                                              0x4
#define HWIO_OEM_CONFIG0_FAST_BOOT_NAND_FVAL                                                                              0x5
#define HWIO_OEM_CONFIG0_FAST_BOOT_SPI_ON_BLSP1_QUP1_FVAL                                                                 0x6
#define HWIO_OEM_CONFIG0_FAST_BOOT_SPI_ON_BLSP1_QUP2_FVAL                                                                 0x7
#define HWIO_OEM_CONFIG0_SDP_ENUM_SKIP_BMSK                                                                             0x200
#define HWIO_OEM_CONFIG0_SDP_ENUM_SKIP_SHFT                                                                               0x9
#define HWIO_OEM_CONFIG0_SDP_ENUM_SKIP_FLCB_ROM_WILL_NOT_SKIP_SDP_ENUMERATION_FVAL                                        0x0
#define HWIO_OEM_CONFIG0_SDP_ENUM_SKIP_FORCE_THE_FLCB_ROM_TO_SKIP_SDP_ENUMERATION_AND_FALLBACK_TO_ATC_FVAL                0x1
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_BMSK                                                                       0x100
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_SHFT                                                                         0x8
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_ENUM_19_2_MHZ_FVAL                                                           0x0
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_ENUM_25_MHZ_FVAL                                                             0x1
#define HWIO_OEM_CONFIG0_APPS_PBL_BOOT_SPEED_BMSK                                                                        0xc0
#define HWIO_OEM_CONFIG0_APPS_PBL_BOOT_SPEED_SHFT                                                                         0x6
#define HWIO_OEM_CONFIG0_APPS_PBL_BOOT_SPEED_XO_FVAL                                                                      0x0
#define HWIO_OEM_CONFIG0_APPS_PBL_BOOT_SPEED_ENUM_384_MHZ_FVAL                                                            0x1
#define HWIO_OEM_CONFIG0_APPS_PBL_BOOT_SPEED_ENUM_614_4_MHZ_FVAL                                                          0x2
#define HWIO_OEM_CONFIG0_APPS_PBL_BOOT_SPEED_ENUM_998_4_MHZ_FVAL                                                          0x3
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_BMSK                                                                     0x20
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_SHFT                                                                      0x5
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL                       0x0
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL                                           0x1
#define HWIO_OEM_CONFIG0_SPARE0_BMSK                                                                                     0x10
#define HWIO_OEM_CONFIG0_SPARE0_SHFT                                                                                      0x4
#define HWIO_OEM_CONFIG0_PBL_PMIC_POWER_DOWN_MSM_BMSK                                                                     0x8
#define HWIO_OEM_CONFIG0_PBL_PMIC_POWER_DOWN_MSM_SHFT                                                                     0x3
#define HWIO_OEM_CONFIG0_PBL_PMIC_POWER_DOWN_MSM_DO_NOT_SHUT_DOWN_PHONE_SW_KEPT_IN_FOREVER_LOOP_FVAL                      0x0
#define HWIO_OEM_CONFIG0_PBL_PMIC_POWER_DOWN_MSM_QC_PMIC_POWER_DOWN_THE_PHONE_FVAL                                        0x1
#define HWIO_OEM_CONFIG0_USB_ENUM_TIMEOUT_BMSK                                                                            0x4
#define HWIO_OEM_CONFIG0_USB_ENUM_TIMEOUT_SHFT                                                                            0x2
#define HWIO_OEM_CONFIG0_USB_ENUM_TIMEOUT_TIMEOUT_DISABLED_FVAL                                                           0x0
#define HWIO_OEM_CONFIG0_USB_ENUM_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                                                        0x1
#define HWIO_OEM_CONFIG0_DLOAD_TIMEOUT_BMSK                                                                               0x2
#define HWIO_OEM_CONFIG0_DLOAD_TIMEOUT_SHFT                                                                               0x1
#define HWIO_OEM_CONFIG0_DLOAD_TIMEOUT_TIMEOUT_DISABLED_FVAL                                                              0x0
#define HWIO_OEM_CONFIG0_DLOAD_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                                                           0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_BMSK                                                                             0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_SHFT                                                                             0x0
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_DOWNLOADER_ENABLED_FVAL                                                          0x0
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_DOWNLOADER_DISABLED_FVAL                                                         0x1

#define HWIO_OEM_CONFIG1_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006028)
#define HWIO_OEM_CONFIG1_PHYS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006028)
#define HWIO_OEM_CONFIG1_OFFS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006028)
#define HWIO_OEM_CONFIG1_RMSK                                                                                      0xffffffff
#define HWIO_OEM_CONFIG1_IN          \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, HWIO_OEM_CONFIG1_RMSK)
#define HWIO_OEM_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, m)
#define HWIO_OEM_CONFIG1_WDOG_EN_BMSK                                                                              0x80000000
#define HWIO_OEM_CONFIG1_WDOG_EN_SHFT                                                                                    0x1f
#define HWIO_OEM_CONFIG1_WDOG_EN_USE_GPIO_FVAL                                                                            0x0
#define HWIO_OEM_CONFIG1_WDOG_EN_IGNORE_GPIO_ENABLE_WDOG_FVAL                                                             0x1
#define HWIO_OEM_CONFIG1_SPARE2_BMSK                                                                               0x78000000
#define HWIO_OEM_CONFIG1_SPARE2_SHFT                                                                                     0x1b
#define HWIO_OEM_CONFIG1_SPDM_SECURE_MODE_BMSK                                                                      0x4000000
#define HWIO_OEM_CONFIG1_SPDM_SECURE_MODE_SHFT                                                                           0x1a
#define HWIO_OEM_CONFIG1_SPDM_SECURE_MODE_NORMAL_MODE_FVAL                                                                0x0
#define HWIO_OEM_CONFIG1_SPDM_SECURE_MODE_SECURE_MODE_FVAL                                                                0x1
#define HWIO_OEM_CONFIG1_IMEI_ESN_SELECT_BMSK                                                                       0x3000000
#define HWIO_OEM_CONFIG1_IMEI_ESN_SELECT_SHFT                                                                            0x18
#define HWIO_OEM_CONFIG1_IMEI_ESN_SELECT_IMEI_ESN0_FVAL                                                                   0x0
#define HWIO_OEM_CONFIG1_IMEI_ESN_SELECT_IMEI_ESN1_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG1_SPARE1_BMSK                                                                                 0xc00000
#define HWIO_OEM_CONFIG1_SPARE1_SHFT                                                                                     0x16
#define HWIO_OEM_CONFIG1_CUST_PRIV_KEY_SELECT_BMSK                                                                   0x200000
#define HWIO_OEM_CONFIG1_CUST_PRIV_KEY_SELECT_SHFT                                                                       0x15
#define HWIO_OEM_CONFIG1_CUST_PRIV_KEY_SELECT_KEY0_FVAL                                                                   0x0
#define HWIO_OEM_CONFIG1_CUST_PRIV_KEY_SELECT_KEY1_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG1_SW_FUSE_PROG_DISABLE_BMSK                                                                   0x100000
#define HWIO_OEM_CONFIG1_SW_FUSE_PROG_DISABLE_SHFT                                                                       0x14
#define HWIO_OEM_CONFIG1_SPARE0_BMSK                                                                                  0xffe00
#define HWIO_OEM_CONFIG1_SPARE0_SHFT                                                                                      0x9
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_BMSK                                                                      0x100
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_SHFT                                                                        0x8
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                                                 0x0
#define HWIO_OEM_CONFIG1_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                                                0x1
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_BMSK                                                                        0x80
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_SHFT                                                                         0x7
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                                                  0x0
#define HWIO_OEM_CONFIG1_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                                                 0x1
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_BMSK                                                                         0x40
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_SHFT                                                                          0x6
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                                                   0x0
#define HWIO_OEM_CONFIG1_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                                                  0x1
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_BMSK                                                                          0x20
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_SHFT                                                                           0x5
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_OEM_CONFIG1_DAP_NIDEN_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_BMSK                                                                          0x10
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_SHFT                                                                           0x4
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_ENABLE_FVAL                                                                    0x0
#define HWIO_OEM_CONFIG1_DAP_DBGEN_DISABLE_DISABLE_FVAL                                                                   0x1
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_BMSK                                                                        0x8
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_SHFT                                                                        0x3
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                                                 0x0
#define HWIO_OEM_CONFIG1_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                                                0x1
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_BMSK                                                                         0x4
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_SHFT                                                                         0x2
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                                                  0x0
#define HWIO_OEM_CONFIG1_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                                                 0x1
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_BMSK                                                                          0x2
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_SHFT                                                                          0x1
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_ENABLE_FVAL                                                                   0x0
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_DISABLE_FVAL                                                                  0x1
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_BMSK                                                                          0x1
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_SHFT                                                                          0x0
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_ENABLE_FVAL                                                                   0x0
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_DISABLE_FVAL                                                                  0x1

#define HWIO_OEM_CONFIG2_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000602c)
#define HWIO_OEM_CONFIG2_PHYS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x0000602c)
#define HWIO_OEM_CONFIG2_OFFS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000602c)
#define HWIO_OEM_CONFIG2_RMSK                                                                                      0xffffffff
#define HWIO_OEM_CONFIG2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, HWIO_OEM_CONFIG2_RMSK)
#define HWIO_OEM_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, m)
#define HWIO_OEM_CONFIG2_OEM_HW_ID_BMSK                                                                            0xffff0000
#define HWIO_OEM_CONFIG2_OEM_HW_ID_SHFT                                                                                  0x10
#define HWIO_OEM_CONFIG2_ANTI_ROLLBACK_FEATURE_EN_BMSK                                                                 0xff00
#define HWIO_OEM_CONFIG2_ANTI_ROLLBACK_FEATURE_EN_SHFT                                                                    0x8
#define HWIO_OEM_CONFIG2_SPARE0_BMSK                                                                                     0xff
#define HWIO_OEM_CONFIG2_SPARE0_SHFT                                                                                      0x0

#define HWIO_OEM_CONFIG3_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006030)
#define HWIO_OEM_CONFIG3_PHYS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006030)
#define HWIO_OEM_CONFIG3_OFFS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006030)
#define HWIO_OEM_CONFIG3_RMSK                                                                                      0xffffffff
#define HWIO_OEM_CONFIG3_IN          \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, HWIO_OEM_CONFIG3_RMSK)
#define HWIO_OEM_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, m)
#define HWIO_OEM_CONFIG3_SPARE0_BMSK                                                                               0xffff0000
#define HWIO_OEM_CONFIG3_SPARE0_SHFT                                                                                     0x10
#define HWIO_OEM_CONFIG3_OEM_PRODUCT_ID_BMSK                                                                           0xffff
#define HWIO_OEM_CONFIG3_OEM_PRODUCT_ID_SHFT                                                                              0x0

#define HWIO_BOOT_CONFIG_ADDR                                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006034)
#define HWIO_BOOT_CONFIG_PHYS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006034)
#define HWIO_BOOT_CONFIG_OFFS                                                                                      (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006034)
#define HWIO_BOOT_CONFIG_RMSK                                                                                           0x7ff
#define HWIO_BOOT_CONFIG_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, HWIO_BOOT_CONFIG_RMSK)
#define HWIO_BOOT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, m)
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_BMSK                                                                         0x400
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_SHFT                                                                           0xa
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_DOES_NOT_FORCE_CODE_AUTHENTICATION_FOR_SECURE_BOOT_FVAL                        0x0
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_FORCE_CODE_AUTHENTICATION_FOR_SECURE_BOOT_FVAL                                 0x1
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                                       0x300
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                                         0x8
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_XO_FVAL                                                                      0x0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_384_MHZ_FVAL                                                            0x1
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_614_4_MHZ_FVAL                                                          0x2
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_998_4_MHZ_FVAL                                                          0x3
#define HWIO_BOOT_CONFIG_RPM_BOOT_FROM_ROM_BMSK                                                                          0x80
#define HWIO_BOOT_CONFIG_RPM_BOOT_FROM_ROM_SHFT                                                                           0x7
#define HWIO_BOOT_CONFIG_RPM_BOOT_FROM_ROM_BOOT_FROM_RPM_CODE_RAM_ALLOWED_FVAL                                            0x0
#define HWIO_BOOT_CONFIG_RPM_BOOT_FROM_ROM_FORCE_BOOT_FROM_ROM_FVAL                                                       0x1
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                                         0x40
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                                          0x6
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                                                  0x3e
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                                                   0x1
#define HWIO_BOOT_CONFIG_FAST_BOOT_DEFAULT_FVAL                                                                           0x0
#define HWIO_BOOT_CONFIG_FAST_BOOT_SD_SDC2_EMMC_SDC1_FVAL                                                                 0x1
#define HWIO_BOOT_CONFIG_FAST_BOOT_EMMC_SDC1_FVAL                                                                         0x2
#define HWIO_BOOT_CONFIG_FAST_BOOT_USB_FVAL                                                                               0x3
#define HWIO_BOOT_CONFIG_FAST_BOOT_HSIC_FVAL                                                                              0x4
#define HWIO_BOOT_CONFIG_FAST_BOOT_NAND_FVAL                                                                              0x5
#define HWIO_BOOT_CONFIG_FAST_BOOT_SPI_ON_BLSP1_QUP1_FVAL                                                                 0x6
#define HWIO_BOOT_CONFIG_FAST_BOOT_SPI_ON_BLSP1_QUP2_FVAL                                                                 0x7
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                                     0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                                     0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_GPIO_CONTROLLED_FVAL                                                                     0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_ENABLE_WDOG_FVAL                                                                         0x1

#define HWIO_SECURE_BOOTn_ADDR(n)                                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006038 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_PHYS(n)                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00006038 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_OFFS(n)                                                                                  (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x00006038 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_RMSK                                                                                          0x17f
#define HWIO_SECURE_BOOTn_MAXn                                                                                             28
#define HWIO_SECURE_BOOTn_INI(n)        \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), HWIO_SECURE_BOOTn_RMSK)
#define HWIO_SECURE_BOOTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), mask)
#define HWIO_SECURE_BOOTn_FUSE_SRC_BMSK                                                                                 0x100
#define HWIO_SECURE_BOOTn_FUSE_SRC_SHFT                                                                                   0x8
#define HWIO_SECURE_BOOTn_FUSE_SRC_QUALCOMM_FVAL                                                                          0x0
#define HWIO_SECURE_BOOTn_FUSE_SRC_OEM_FVAL                                                                               0x1
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_BMSK                                                                            0x40
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_SHFT                                                                             0x6
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_USE_OEM_ID_FVAL                                                                  0x0
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_USE_SERIAL_NUM_FVAL                                                              0x1
#define HWIO_SECURE_BOOTn_AUTH_EN_BMSK                                                                                   0x20
#define HWIO_SECURE_BOOTn_AUTH_EN_SHFT                                                                                    0x5
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_BMSK                                                                           0x10
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHFT                                                                            0x4
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHA_256_HASH_OF_ROOT_CERTIFICATE_IS_IN_ROM_FVAL                                 0x0
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHA_256_HASH_OF_ROOT_CERTIFICATE_IS_IN_OEM_PK_HASH_FVAL                         0x1
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_BMSK                                                                          0xf
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_SHFT                                                                          0x0

#define HWIO_OVERRIDE_0_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b0)
#define HWIO_OVERRIDE_0_PHYS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060b0)
#define HWIO_OVERRIDE_0_OFFS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060b0)
#define HWIO_OVERRIDE_0_RMSK                                                                                       0x10000003
#define HWIO_OVERRIDE_0_IN          \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, HWIO_OVERRIDE_0_RMSK)
#define HWIO_OVERRIDE_0_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_0_ADDR, m)
#define HWIO_OVERRIDE_0_OUT(v)      \
        out_dword(HWIO_OVERRIDE_0_ADDR,v)
#define HWIO_OVERRIDE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_0_ADDR,m,v,HWIO_OVERRIDE_0_IN)
#define HWIO_OVERRIDE_0_TX_DISABLE_BMSK                                                                            0x10000000
#define HWIO_OVERRIDE_0_TX_DISABLE_SHFT                                                                                  0x1c
#define HWIO_OVERRIDE_0_TX_DISABLE_ENABLE_TX_FVAL                                                                         0x0
#define HWIO_OVERRIDE_0_TX_DISABLE_DISABLE_TX_FVAL                                                                        0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_BMSK                                                                          0x2
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_SHFT                                                                          0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_ENUM_1_8V_SELECTION_FVAL                                                      0x0
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_EN_ENUM_1_2V_SELECTION_FVAL                                                      0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_BMSK                                                                    0x1
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_SHFT                                                                    0x0
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_DO_NOT_OVERRIDE_FVAL                                                    0x0
#define HWIO_OVERRIDE_0_SDC_EMMC_MODE1P2_OVERRIDE_OVERRIDE_FVAL                                                           0x1

#define HWIO_OVERRIDE_1_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b4)
#define HWIO_OVERRIDE_1_PHYS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060b4)
#define HWIO_OVERRIDE_1_OFFS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060b4)
#define HWIO_OVERRIDE_1_RMSK                                                                                       0xffffffff
#define HWIO_OVERRIDE_1_IN          \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, HWIO_OVERRIDE_1_RMSK)
#define HWIO_OVERRIDE_1_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_1_ADDR, m)
#define HWIO_OVERRIDE_1_OUT(v)      \
        out_dword(HWIO_OVERRIDE_1_ADDR,v)
#define HWIO_OVERRIDE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_1_ADDR,m,v,HWIO_OVERRIDE_1_IN)
#define HWIO_OVERRIDE_1_OVERRIDE_1_BMSK                                                                            0xffffffff
#define HWIO_OVERRIDE_1_OVERRIDE_1_SHFT                                                                                   0x0

#define HWIO_OVERRIDE_2_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060b8)
#define HWIO_OVERRIDE_2_PHYS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060b8)
#define HWIO_OVERRIDE_2_OFFS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060b8)
#define HWIO_OVERRIDE_2_RMSK                                                                                         0x7ffc00
#define HWIO_OVERRIDE_2_IN          \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, HWIO_OVERRIDE_2_RMSK)
#define HWIO_OVERRIDE_2_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_2_ADDR, m)
#define HWIO_OVERRIDE_2_OUT(v)      \
        out_dword(HWIO_OVERRIDE_2_ADDR,v)
#define HWIO_OVERRIDE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_2_ADDR,m,v,HWIO_OVERRIDE_2_IN)
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_BMSK                                                              0x400000
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_SHFT                                                                  0x16
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_FUSE_VALUE_FVAL                                                        0x0
#define HWIO_OVERRIDE_2_OVRID_DAP_DEVICEEN_DISABLE_QC_FUSE_VALUE_FVAL                                                     0x1
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_BMSK                                                                 0x200000
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_SHFT                                                                     0x15
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_FUSE_VALUE_FVAL                                                           0x0
#define HWIO_OVERRIDE_2_OVRID_DAP_NIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                                        0x1
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_BMSK                                                                 0x100000
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_SHFT                                                                     0x14
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_FUSE_VALUE_FVAL                                                           0x0
#define HWIO_OVERRIDE_2_OVRID_DAP_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                                        0x1
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_BMSK                                                                 0x80000
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_SHFT                                                                    0x13
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_FUSE_VALUE_FVAL                                                          0x0
#define HWIO_OVERRIDE_2_OVRID_APPS_NIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                                       0x1
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_BMSK                                                                 0x40000
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_SHFT                                                                    0x12
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_FUSE_VALUE_FVAL                                                          0x0
#define HWIO_OVERRIDE_2_OVRID_APPS_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                                       0x1
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_BMSK                                                                     0x20000
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_SHFT                                                                        0x11
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_FUSE_VALUE_FVAL                                                              0x0
#define HWIO_OVERRIDE_2_OVRID_SPARE1_DISABLE_QC_FUSE_VALUE_FVAL                                                           0x1
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_BMSK                                                                     0x10000
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_SHFT                                                                        0x10
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_FUSE_VALUE_FVAL                                                              0x0
#define HWIO_OVERRIDE_2_OVRID_SPARE0_DISABLE_QC_FUSE_VALUE_FVAL                                                           0x1
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_BMSK                                                               0x8000
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_SHFT                                                                  0xf
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_FUSE_VALUE_FVAL                                                       0x0
#define HWIO_OVERRIDE_2_OVRID_VENUS_0_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                                    0x1
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_BMSK                                                                   0x4000
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_SHFT                                                                      0xe
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_FUSE_VALUE_FVAL                                                           0x0
#define HWIO_OVERRIDE_2_OVRID_RPM_DAPEN_DISABLE_QC_FUSE_VALUE_FVAL                                                        0x1
#define HWIO_OVERRIDE_2_OVRID_RPM_LPASS_WCSS_NIDEN_DISABLE_BMSK                                                        0x2000
#define HWIO_OVERRIDE_2_OVRID_RPM_LPASS_WCSS_NIDEN_DISABLE_SHFT                                                           0xd
#define HWIO_OVERRIDE_2_OVRID_RPM_LPASS_WCSS_NIDEN_DISABLE_FUSE_VALUE_FVAL                                                0x0
#define HWIO_OVERRIDE_2_OVRID_RPM_LPASS_WCSS_NIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                             0x1
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_BMSK                                                                   0x1000
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_SHFT                                                                      0xc
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_FUSE_VALUE_FVAL                                                           0x0
#define HWIO_OVERRIDE_2_OVRID_RPM_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                                        0x1
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_BMSK                                                                   0x800
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_SHFT                                                                     0xb
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_FUSE_VALUE_FVAL                                                          0x0
#define HWIO_OVERRIDE_2_OVRID_WCSS_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                                       0x1
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_BMSK                                                                  0x400
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_SHFT                                                                    0xa
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_FUSE_VALUE_FVAL                                                         0x0
#define HWIO_OVERRIDE_2_OVRID_LPASS_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                                      0x1

#define HWIO_OVERRIDE_3_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060bc)
#define HWIO_OVERRIDE_3_PHYS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060bc)
#define HWIO_OVERRIDE_3_OFFS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060bc)
#define HWIO_OVERRIDE_3_RMSK                                                                                             0x1f
#define HWIO_OVERRIDE_3_IN          \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, HWIO_OVERRIDE_3_RMSK)
#define HWIO_OVERRIDE_3_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_3_ADDR, m)
#define HWIO_OVERRIDE_3_OUT(v)      \
        out_dword(HWIO_OVERRIDE_3_ADDR,v)
#define HWIO_OVERRIDE_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_3_ADDR,m,v,HWIO_OVERRIDE_3_IN)
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_BMSK                                                                   0x10
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_SHFT                                                                    0x4
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_FUSE_VALUE_FVAL                                                         0x0
#define HWIO_OVERRIDE_3_OVRID_DAP_SPNIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                                      0x1
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_BMSK                                                                     0x8
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_SHFT                                                                     0x3
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_FUSE_VALUE_FVAL                                                          0x0
#define HWIO_OVERRIDE_3_OVRID_DAP_SPIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                                       0x1
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_BMSK                                                                   0x4
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_SHFT                                                                   0x2
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_FUSE_VALUE_FVAL                                                        0x0
#define HWIO_OVERRIDE_3_OVRID_APPS_SPNIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                                     0x1
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_BMSK                                                                    0x2
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_SHFT                                                                    0x1
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_FUSE_VALUE_FVAL                                                         0x0
#define HWIO_OVERRIDE_3_OVRID_APPS_SPIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                                      0x1
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_BMSK                                                                       0x1
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_SHFT                                                                       0x0
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_FUSE_VALUE_FVAL                                                            0x0
#define HWIO_OVERRIDE_3_OVRID_SPDM_SECURE_MODE_NORMAL_MODE_FVAL                                                           0x1

#define HWIO_OVERRIDE_4_ADDR                                                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c0)
#define HWIO_OVERRIDE_4_PHYS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060c0)
#define HWIO_OVERRIDE_4_OFFS                                                                                       (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060c0)
#define HWIO_OVERRIDE_4_RMSK                                                                                              0x3
#define HWIO_OVERRIDE_4_IN          \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, HWIO_OVERRIDE_4_RMSK)
#define HWIO_OVERRIDE_4_INM(m)      \
        in_dword_masked(HWIO_OVERRIDE_4_ADDR, m)
#define HWIO_OVERRIDE_4_OUT(v)      \
        out_dword(HWIO_OVERRIDE_4_ADDR,v)
#define HWIO_OVERRIDE_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OVERRIDE_4_ADDR,m,v,HWIO_OVERRIDE_4_IN)
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_BMSK                                                                      0x2
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_SHFT                                                                      0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_FUSE_VALUE_FVAL                                                           0x0
#define HWIO_OVERRIDE_4_OVRID_MSS_NIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                                        0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_BMSK                                                                      0x1
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_SHFT                                                                      0x0
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_FUSE_VALUE_FVAL                                                           0x0
#define HWIO_OVERRIDE_4_OVRID_MSS_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                                        0x1

#define HWIO_CAPT_SEC_GPIO_ADDR                                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c4)
#define HWIO_CAPT_SEC_GPIO_PHYS                                                                                    (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060c4)
#define HWIO_CAPT_SEC_GPIO_OFFS                                                                                    (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060c4)
#define HWIO_CAPT_SEC_GPIO_RMSK                                                                                       0x3ffff
#define HWIO_CAPT_SEC_GPIO_IN          \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, HWIO_CAPT_SEC_GPIO_RMSK)
#define HWIO_CAPT_SEC_GPIO_INM(m)      \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, m)
#define HWIO_CAPT_SEC_GPIO_OUT(v)      \
        out_dword(HWIO_CAPT_SEC_GPIO_ADDR,v)
#define HWIO_CAPT_SEC_GPIO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CAPT_SEC_GPIO_ADDR,m,v,HWIO_CAPT_SEC_GPIO_IN)
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_BMSK                                                                   0x20000
#define HWIO_CAPT_SEC_GPIO_SDC_EMMC_MODE1P2_EN_SHFT                                                                      0x11
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_BMSK                                                                   0x10000
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_SHFT                                                                      0x10
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_BMSK                                                                       0xffff
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_SHFT                                                                          0x0

#define HWIO_APP_PROC_CFG_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c8)
#define HWIO_APP_PROC_CFG_PHYS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060c8)
#define HWIO_APP_PROC_CFG_OFFS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060c8)
#define HWIO_APP_PROC_CFG_RMSK                                                                                      0x1ffffff
#define HWIO_APP_PROC_CFG_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, HWIO_APP_PROC_CFG_RMSK)
#define HWIO_APP_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, m)
#define HWIO_APP_PROC_CFG_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_ADDR,v)
#define HWIO_APP_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_ADDR,m,v,HWIO_APP_PROC_CFG_IN)
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_BMSK                                                                       0x1000000
#define HWIO_APP_PROC_CFG_WCSS_DBG_NIDEN_SHFT                                                                            0x18
#define HWIO_APP_PROC_CFG_LPASS_DBG_NIDEN_BMSK                                                                       0x800000
#define HWIO_APP_PROC_CFG_LPASS_DBG_NIDEN_SHFT                                                                           0x17
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_BMSK                                                                         0x400000
#define HWIO_APP_PROC_CFG_RPM_DBG_NIDEN_SHFT                                                                             0x16
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_BMSK                                                                         0x200000
#define HWIO_APP_PROC_CFG_DAP_DBG_NIDEN_SHFT                                                                             0x15
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_BMSK                                                                       0x100000
#define HWIO_APP_PROC_CFG_DAP_DBG_SPNIDEN_SHFT                                                                           0x14
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC3_BMSK                                                                         0xf0000
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC3_SHFT                                                                            0x10
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC2_BMSK                                                                          0xf000
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC2_SHFT                                                                             0xc
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC1_BMSK                                                                           0xf00
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC1_SHFT                                                                             0x8
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC0_BMSK                                                                            0xf0
#define HWIO_APP_PROC_CFG_APPS_CFG_MISC0_SHFT                                                                             0x4
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_BMSK                                                                             0x8
#define HWIO_APP_PROC_CFG_APPS_DBG_NIDEN_SHFT                                                                             0x3
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_BMSK                                                                           0x4
#define HWIO_APP_PROC_CFG_APPS_DBG_SPNIDEN_SHFT                                                                           0x2
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_BMSK                                                                          0x2
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_SHFT                                                                          0x1
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_BMSK                                                                              0x1
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_SHFT                                                                              0x0
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_NORMAL_FVAL                                                                       0x0
#define HWIO_APP_PROC_CFG_APPS_CFG_NMFI_DISABLE_FVAL                                                                      0x1

#define HWIO_MSS_PROC_CFG_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060cc)
#define HWIO_MSS_PROC_CFG_PHYS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060cc)
#define HWIO_MSS_PROC_CFG_OFFS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060cc)
#define HWIO_MSS_PROC_CFG_RMSK                                                                                            0x1
#define HWIO_MSS_PROC_CFG_IN          \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, HWIO_MSS_PROC_CFG_RMSK)
#define HWIO_MSS_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, m)
#define HWIO_MSS_PROC_CFG_OUT(v)      \
        out_dword(HWIO_MSS_PROC_CFG_ADDR,v)
#define HWIO_MSS_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_PROC_CFG_ADDR,m,v,HWIO_MSS_PROC_CFG_IN)
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_BMSK                                                                              0x1
#define HWIO_MSS_PROC_CFG_MSS_DBG_NIDEN_SHFT                                                                              0x0

#define HWIO_KDF_CFG_ADDR                                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d0)
#define HWIO_KDF_CFG_PHYS                                                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060d0)
#define HWIO_KDF_CFG_OFFS                                                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060d0)
#define HWIO_KDF_CFG_RMSK                                                                                                 0x1
#define HWIO_KDF_CFG_IN          \
        in_dword_masked(HWIO_KDF_CFG_ADDR, HWIO_KDF_CFG_RMSK)
#define HWIO_KDF_CFG_INM(m)      \
        in_dword_masked(HWIO_KDF_CFG_ADDR, m)
#define HWIO_KDF_CFG_OUT(v)      \
        out_dword(HWIO_KDF_CFG_ADDR,v)
#define HWIO_KDF_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_KDF_CFG_ADDR,m,v,HWIO_KDF_CFG_IN)
#define HWIO_KDF_CFG_CE1_CE2_SAME_HW_KEY_BMSK                                                                             0x1
#define HWIO_KDF_CFG_CE1_CE2_SAME_HW_KEY_SHFT                                                                             0x0
#define HWIO_KDF_CFG_CE1_CE2_SAME_HW_KEY_DIFFERENT_KEYS_FVAL                                                              0x0
#define HWIO_KDF_CFG_CE1_CE2_SAME_HW_KEY_SAME_KEYS_FVAL                                                                   0x1

#define HWIO_QFPROM_CLK_CTL_ADDR                                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d4)
#define HWIO_QFPROM_CLK_CTL_PHYS                                                                                   (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060d4)
#define HWIO_QFPROM_CLK_CTL_OFFS                                                                                   (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060d4)
#define HWIO_QFPROM_CLK_CTL_RMSK                                                                                          0x1
#define HWIO_QFPROM_CLK_CTL_IN          \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, HWIO_QFPROM_CLK_CTL_RMSK)
#define HWIO_QFPROM_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, m)
#define HWIO_QFPROM_CLK_CTL_OUT(v)      \
        out_dword(HWIO_QFPROM_CLK_CTL_ADDR,v)
#define HWIO_QFPROM_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CLK_CTL_ADDR,m,v,HWIO_QFPROM_CLK_CTL_IN)
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_BMSK                                                                                 0x1
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_SHFT                                                                                 0x0
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_CLK_ENABLED_FVAL                                                                     0x0
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_CLK_DISABLED_FVAL                                                                    0x1

#define HWIO_HDCP_KSV_LSB_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d8)
#define HWIO_HDCP_KSV_LSB_PHYS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060d8)
#define HWIO_HDCP_KSV_LSB_OFFS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060d8)
#define HWIO_HDCP_KSV_LSB_RMSK                                                                                     0xffffffff
#define HWIO_HDCP_KSV_LSB_IN          \
        in_dword_masked(HWIO_HDCP_KSV_LSB_ADDR, HWIO_HDCP_KSV_LSB_RMSK)
#define HWIO_HDCP_KSV_LSB_INM(m)      \
        in_dword_masked(HWIO_HDCP_KSV_LSB_ADDR, m)
#define HWIO_HDCP_KSV_LSB_KSV_LSB_BMSK                                                                             0xffffffff
#define HWIO_HDCP_KSV_LSB_KSV_LSB_SHFT                                                                                    0x0

#define HWIO_HDCP_KSV_MSB_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060dc)
#define HWIO_HDCP_KSV_MSB_PHYS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060dc)
#define HWIO_HDCP_KSV_MSB_OFFS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060dc)
#define HWIO_HDCP_KSV_MSB_RMSK                                                                                           0xff
#define HWIO_HDCP_KSV_MSB_IN          \
        in_dword_masked(HWIO_HDCP_KSV_MSB_ADDR, HWIO_HDCP_KSV_MSB_RMSK)
#define HWIO_HDCP_KSV_MSB_INM(m)      \
        in_dword_masked(HWIO_HDCP_KSV_MSB_ADDR, m)
#define HWIO_HDCP_KSV_MSB_KSV_MSB_BMSK                                                                                   0xff
#define HWIO_HDCP_KSV_MSB_KSV_MSB_SHFT                                                                                    0x0

#define HWIO_JTAG_ID_ADDR                                                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e0)
#define HWIO_JTAG_ID_PHYS                                                                                          (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060e0)
#define HWIO_JTAG_ID_OFFS                                                                                          (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060e0)
#define HWIO_JTAG_ID_RMSK                                                                                          0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                                                  0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                                         0x0

#define HWIO_OEM_ID_ADDR                                                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e4)
#define HWIO_OEM_ID_PHYS                                                                                           (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060e4)
#define HWIO_OEM_ID_OFFS                                                                                           (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060e4)
#define HWIO_OEM_ID_RMSK                                                                                           0xffffffff
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                                                    0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                                          0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                                                0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                                                   0x0

#define HWIO_TEST_BUS_SEL_ADDR                                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e8)
#define HWIO_TEST_BUS_SEL_PHYS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060e8)
#define HWIO_TEST_BUS_SEL_OFFS                                                                                     (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060e8)
#define HWIO_TEST_BUS_SEL_RMSK                                                                                            0x7
#define HWIO_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, HWIO_TEST_BUS_SEL_RMSK)
#define HWIO_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, m)
#define HWIO_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_TEST_BUS_SEL_ADDR,v)
#define HWIO_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TEST_BUS_SEL_ADDR,m,v,HWIO_TEST_BUS_SEL_IN)
#define HWIO_TEST_BUS_SEL_TEST_EN_BMSK                                                                                    0x4
#define HWIO_TEST_BUS_SEL_TEST_EN_SHFT                                                                                    0x2
#define HWIO_TEST_BUS_SEL_TEST_EN_DEBUG_DISABLED_FVAL                                                                     0x0
#define HWIO_TEST_BUS_SEL_TEST_EN_DEBUG_ENABLED_FVAL                                                                      0x1
#define HWIO_TEST_BUS_SEL_TEST_SELECT_BMSK                                                                                0x3
#define HWIO_TEST_BUS_SEL_TEST_SELECT_SHFT                                                                                0x0
#define HWIO_TEST_BUS_SEL_TEST_SELECT_FUSE_SENSE_FVAL                                                                     0x0
#define HWIO_TEST_BUS_SEL_TEST_SELECT_QFPROM_ARBITER_FVAL                                                                 0x1
#define HWIO_TEST_BUS_SEL_TEST_SELECT_SW_FVAL                                                                             0x2
#define HWIO_TEST_BUS_SEL_TEST_SELECT_ACC_FVAL                                                                            0x3

#define HWIO_SPDM_DYN_SECURE_MODE_ADDR                                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060ec)
#define HWIO_SPDM_DYN_SECURE_MODE_PHYS                                                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060ec)
#define HWIO_SPDM_DYN_SECURE_MODE_OFFS                                                                             (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060ec)
#define HWIO_SPDM_DYN_SECURE_MODE_RMSK                                                                                    0x1
#define HWIO_SPDM_DYN_SECURE_MODE_IN          \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, HWIO_SPDM_DYN_SECURE_MODE_RMSK)
#define HWIO_SPDM_DYN_SECURE_MODE_INM(m)      \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, m)
#define HWIO_SPDM_DYN_SECURE_MODE_OUT(v)      \
        out_dword(HWIO_SPDM_DYN_SECURE_MODE_ADDR,v)
#define HWIO_SPDM_DYN_SECURE_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPDM_DYN_SECURE_MODE_ADDR,m,v,HWIO_SPDM_DYN_SECURE_MODE_IN)
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_BMSK                                                                        0x1
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_SHFT                                                                        0x0
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_DISABLE_FVAL                                                                0x0
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_ENABLE_FVAL                                                                 0x1

#define HWIO_BOOT_PARTITION_SELECT_ADDR                                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060f0)
#define HWIO_BOOT_PARTITION_SELECT_PHYS                                                                            (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x000060f0)
#define HWIO_BOOT_PARTITION_SELECT_OFFS                                                                            (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x000060f0)
#define HWIO_BOOT_PARTITION_SELECT_RMSK                                                                            0xffffffff
#define HWIO_BOOT_PARTITION_SELECT_IN          \
        in_dword_masked(HWIO_BOOT_PARTITION_SELECT_ADDR, HWIO_BOOT_PARTITION_SELECT_RMSK)
#define HWIO_BOOT_PARTITION_SELECT_INM(m)      \
        in_dword_masked(HWIO_BOOT_PARTITION_SELECT_ADDR, m)
#define HWIO_BOOT_PARTITION_SELECT_OUT(v)      \
        out_dword(HWIO_BOOT_PARTITION_SELECT_ADDR,v)
#define HWIO_BOOT_PARTITION_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_BOOT_PARTITION_SELECT_ADDR,m,v,HWIO_BOOT_PARTITION_SELECT_IN)
#define HWIO_BOOT_PARTITION_SELECT_PARTITION_BMSK                                                                  0xffffffff
#define HWIO_BOOT_PARTITION_SELECT_PARTITION_SHFT                                                                         0x0


#endif /* __HALHWIOSECCTRL_H__ */

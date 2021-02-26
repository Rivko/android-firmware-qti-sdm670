#ifndef __HAL_HWIO_TSENS_H__
#define __HAL_HWIO_TSENS_H__
/*============================================================================
  @file HALhwioTsens.h

  Implementation of the TSENS HAL - HWIO info was auto-generated

                Copyright (c) 2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * MODULE: TSENS
 *--------------------------------------------------------------------------*/
#define HWIO_TSENS_HW_VER_ADDR(x)                                                  ((x) + 0x00000000)
#define HWIO_TSENS_HW_VER_RMSK                                                     0xffffffff
#define HWIO_TSENS_HW_VER_IN(x)      \
        in_dword_masked(HWIO_TSENS_HW_VER_ADDR(x), HWIO_TSENS_HW_VER_RMSK)
#define HWIO_TSENS_HW_VER_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_HW_VER_ADDR(x), m)
#define HWIO_TSENS_HW_VER_MAJOR_BMSK                                               0xf0000000
#define HWIO_TSENS_HW_VER_MAJOR_SHFT                                                     0x1c
#define HWIO_TSENS_HW_VER_MINOR_BMSK                                                0xfff0000
#define HWIO_TSENS_HW_VER_MINOR_SHFT                                                     0x10
#define HWIO_TSENS_HW_VER_STEP_BMSK                                                    0xffff
#define HWIO_TSENS_HW_VER_STEP_SHFT                                                       0x0

#define HWIO_TSENS_CTRL_ADDR(x)                                                    ((x) + 0x00000004)
#define HWIO_TSENS_CTRL_RMSK                                                       0x3fffffff
#define HWIO_TSENS_CTRL_IN(x)      \
        in_dword_masked(HWIO_TSENS_CTRL_ADDR(x), HWIO_TSENS_CTRL_RMSK)
#define HWIO_TSENS_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CTRL_ADDR(x), m)
#define HWIO_TSENS_CTRL_OUT(x, v)      \
        out_dword(HWIO_TSENS_CTRL_ADDR(x),v)
#define HWIO_TSENS_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_CTRL_ADDR(x),m,v,HWIO_TSENS_CTRL_IN(x))
#define HWIO_TSENS_CTRL_MAX_TEMP_PWM_EN_BMSK                                       0x20000000
#define HWIO_TSENS_CTRL_MAX_TEMP_PWM_EN_SHFT                                             0x1d
#define HWIO_TSENS_CTRL_MAX_TEMP_PWM_EN_DISABLED_FVAL                                     0x0
#define HWIO_TSENS_CTRL_MAX_TEMP_PWM_EN_ENABLED_FVAL                                      0x1
#define HWIO_TSENS_CTRL_VALID_DELAY_BMSK                                           0x1e000000
#define HWIO_TSENS_CTRL_VALID_DELAY_SHFT                                                 0x19
#define HWIO_TSENS_CTRL_PSHOLD_ARES_EN_BMSK                                         0x1000000
#define HWIO_TSENS_CTRL_PSHOLD_ARES_EN_SHFT                                              0x18
#define HWIO_TSENS_CTRL_PSHOLD_ARES_EN_DISABLED_FVAL                                      0x0
#define HWIO_TSENS_CTRL_PSHOLD_ARES_EN_ENABLED_FVAL                                       0x1
#define HWIO_TSENS_CTRL_TEMP_BROADCAST_EN_BMSK                                       0x800000
#define HWIO_TSENS_CTRL_TEMP_BROADCAST_EN_SHFT                                           0x17
#define HWIO_TSENS_CTRL_TEMP_BROADCAST_EN_DISABLED_FVAL                                   0x0
#define HWIO_TSENS_CTRL_TEMP_BROADCAST_EN_ENABLED_FVAL                                    0x1
#define HWIO_TSENS_CTRL_AUTO_ADJUST_PERIOD_EN_BMSK                                   0x400000
#define HWIO_TSENS_CTRL_AUTO_ADJUST_PERIOD_EN_SHFT                                       0x16
#define HWIO_TSENS_CTRL_AUTO_ADJUST_PERIOD_EN_DISABLED_FVAL                               0x0
#define HWIO_TSENS_CTRL_AUTO_ADJUST_PERIOD_EN_ENABLED_FVAL                                0x1
#define HWIO_TSENS_CTRL_RESULT_FORMAT_CODE_OR_TEMP_BMSK                              0x200000
#define HWIO_TSENS_CTRL_RESULT_FORMAT_CODE_OR_TEMP_SHFT                                  0x15
#define HWIO_TSENS_CTRL_RESULT_FORMAT_CODE_OR_TEMP_ADC_CODE_FVAL                          0x0
#define HWIO_TSENS_CTRL_RESULT_FORMAT_CODE_OR_TEMP_REAL_TEMPERATURE_FVAL                  0x1
#define HWIO_TSENS_CTRL_TSENS_CLAMP_BMSK                                             0x100000
#define HWIO_TSENS_CTRL_TSENS_CLAMP_SHFT                                                 0x14
#define HWIO_TSENS_CTRL_TSENS_CLAMP_UNCLAMPED_FVAL                                        0x0
#define HWIO_TSENS_CTRL_TSENS_CLAMP_CLAMPED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_TSENS_BYPASS_EN_BMSK                                          0x80000
#define HWIO_TSENS_CTRL_TSENS_BYPASS_EN_SHFT                                             0x13
#define HWIO_TSENS_CTRL_TSENS_BYPASS_EN_DISABLED_FVAL                                     0x0
#define HWIO_TSENS_CTRL_TSENS_BYPASS_EN_ENABLED_FVAL                                      0x1
#define HWIO_TSENS_CTRL_SENSOR15_EN_BMSK                                              0x40000
#define HWIO_TSENS_CTRL_SENSOR15_EN_SHFT                                                 0x12
#define HWIO_TSENS_CTRL_SENSOR15_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR15_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR14_EN_BMSK                                              0x20000
#define HWIO_TSENS_CTRL_SENSOR14_EN_SHFT                                                 0x11
#define HWIO_TSENS_CTRL_SENSOR14_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR14_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR13_EN_BMSK                                              0x10000
#define HWIO_TSENS_CTRL_SENSOR13_EN_SHFT                                                 0x10
#define HWIO_TSENS_CTRL_SENSOR13_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR13_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR12_EN_BMSK                                               0x8000
#define HWIO_TSENS_CTRL_SENSOR12_EN_SHFT                                                  0xf
#define HWIO_TSENS_CTRL_SENSOR12_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR12_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR11_EN_BMSK                                               0x4000
#define HWIO_TSENS_CTRL_SENSOR11_EN_SHFT                                                  0xe
#define HWIO_TSENS_CTRL_SENSOR11_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR11_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR10_EN_BMSK                                               0x2000
#define HWIO_TSENS_CTRL_SENSOR10_EN_SHFT                                                  0xd
#define HWIO_TSENS_CTRL_SENSOR10_EN_DISABLED_FVAL                                         0x0
#define HWIO_TSENS_CTRL_SENSOR10_EN_ENABLED_FVAL                                          0x1
#define HWIO_TSENS_CTRL_SENSOR9_EN_BMSK                                                0x1000
#define HWIO_TSENS_CTRL_SENSOR9_EN_SHFT                                                   0xc
#define HWIO_TSENS_CTRL_SENSOR9_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR9_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR8_EN_BMSK                                                 0x800
#define HWIO_TSENS_CTRL_SENSOR8_EN_SHFT                                                   0xb
#define HWIO_TSENS_CTRL_SENSOR8_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR8_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR7_EN_BMSK                                                 0x400
#define HWIO_TSENS_CTRL_SENSOR7_EN_SHFT                                                   0xa
#define HWIO_TSENS_CTRL_SENSOR7_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR7_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR6_EN_BMSK                                                 0x200
#define HWIO_TSENS_CTRL_SENSOR6_EN_SHFT                                                   0x9
#define HWIO_TSENS_CTRL_SENSOR6_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR6_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR5_EN_BMSK                                                 0x100
#define HWIO_TSENS_CTRL_SENSOR5_EN_SHFT                                                   0x8
#define HWIO_TSENS_CTRL_SENSOR5_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR5_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR4_EN_BMSK                                                  0x80
#define HWIO_TSENS_CTRL_SENSOR4_EN_SHFT                                                   0x7
#define HWIO_TSENS_CTRL_SENSOR4_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR4_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR3_EN_BMSK                                                  0x40
#define HWIO_TSENS_CTRL_SENSOR3_EN_SHFT                                                   0x6
#define HWIO_TSENS_CTRL_SENSOR3_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR3_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR2_EN_BMSK                                                  0x20
#define HWIO_TSENS_CTRL_SENSOR2_EN_SHFT                                                   0x5
#define HWIO_TSENS_CTRL_SENSOR2_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR2_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR1_EN_BMSK                                                  0x10
#define HWIO_TSENS_CTRL_SENSOR1_EN_SHFT                                                   0x4
#define HWIO_TSENS_CTRL_SENSOR1_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR1_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_SENSOR0_EN_BMSK                                                   0x8
#define HWIO_TSENS_CTRL_SENSOR0_EN_SHFT                                                   0x3
#define HWIO_TSENS_CTRL_SENSOR0_EN_DISABLED_FVAL                                          0x0
#define HWIO_TSENS_CTRL_SENSOR0_EN_ENABLED_FVAL                                           0x1
#define HWIO_TSENS_CTRL_TSENS_ADC_CLK_SEL_BMSK                                            0x4
#define HWIO_TSENS_CTRL_TSENS_ADC_CLK_SEL_SHFT                                            0x2
#define HWIO_TSENS_CTRL_TSENS_ADC_CLK_SEL_INTERNAL_OSCILLATOR_FVAL                        0x0
#define HWIO_TSENS_CTRL_TSENS_ADC_CLK_SEL_EXTERNAL_CLOCK_SOURCE_FVAL                      0x1
#define HWIO_TSENS_CTRL_TSENS_SW_RST_BMSK                                                 0x2
#define HWIO_TSENS_CTRL_TSENS_SW_RST_SHFT                                                 0x1
#define HWIO_TSENS_CTRL_TSENS_SW_RST_RESET_DEASSERTED_FVAL                                0x0
#define HWIO_TSENS_CTRL_TSENS_SW_RST_RESET_ASSERTED_FVAL                                  0x1
#define HWIO_TSENS_CTRL_TSENS_EN_BMSK                                                     0x1
#define HWIO_TSENS_CTRL_TSENS_EN_SHFT                                                     0x0
#define HWIO_TSENS_CTRL_TSENS_EN_DISABLED_FVAL                                            0x0
#define HWIO_TSENS_CTRL_TSENS_EN_ENABLED_FVAL                                             0x1

#define HWIO_TSENS_MEASURE_PERIOD_ADDR(x)                                          ((x) + 0x00000008)
#define HWIO_TSENS_MEASURE_PERIOD_RMSK                                              0xfffffff
#define HWIO_TSENS_MEASURE_PERIOD_IN(x)      \
        in_dword_masked(HWIO_TSENS_MEASURE_PERIOD_ADDR(x), HWIO_TSENS_MEASURE_PERIOD_RMSK)
#define HWIO_TSENS_MEASURE_PERIOD_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MEASURE_PERIOD_ADDR(x), m)
#define HWIO_TSENS_MEASURE_PERIOD_OUT(x, v)      \
        out_dword(HWIO_TSENS_MEASURE_PERIOD_ADDR(x),v)
#define HWIO_TSENS_MEASURE_PERIOD_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_MEASURE_PERIOD_ADDR(x),m,v,HWIO_TSENS_MEASURE_PERIOD_IN(x))
#define HWIO_TSENS_MEASURE_PERIOD_TBCB_CLK_STRETCH_CMUX_DELAY_BMSK                  0xfc00000
#define HWIO_TSENS_MEASURE_PERIOD_TBCB_CLK_STRETCH_CMUX_DELAY_SHFT                       0x16
#define HWIO_TSENS_MEASURE_PERIOD_TBCB_SSCTX_CLK_STRETCH_CNTR_BMSK                   0x3f0000
#define HWIO_TSENS_MEASURE_PERIOD_TBCB_SSCTX_CLK_STRETCH_CNTR_SHFT                       0x10
#define HWIO_TSENS_MEASURE_PERIOD_POWERDOWN_MEASURE_PERIOD_BMSK                        0xff00
#define HWIO_TSENS_MEASURE_PERIOD_POWERDOWN_MEASURE_PERIOD_SHFT                           0x8
#define HWIO_TSENS_MEASURE_PERIOD_MAIN_MEASURE_PERIOD_BMSK                               0xff
#define HWIO_TSENS_MEASURE_PERIOD_MAIN_MEASURE_PERIOD_SHFT                                0x0

#define HWIO_TSENS_TEST_CTRL_ADDR(x)                                               ((x) + 0x0000000c)
#define HWIO_TSENS_TEST_CTRL_RMSK                                                       0xfff
#define HWIO_TSENS_TEST_CTRL_IN(x)      \
        in_dword_masked(HWIO_TSENS_TEST_CTRL_ADDR(x), HWIO_TSENS_TEST_CTRL_RMSK)
#define HWIO_TSENS_TEST_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TEST_CTRL_ADDR(x), m)
#define HWIO_TSENS_TEST_CTRL_OUT(x, v)      \
        out_dword(HWIO_TSENS_TEST_CTRL_ADDR(x),v)
#define HWIO_TSENS_TEST_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_TEST_CTRL_ADDR(x),m,v,HWIO_TSENS_TEST_CTRL_IN(x))
#define HWIO_TSENS_TEST_CTRL_TSENS_RO_SELECT_OVR_BMSK                                   0x800
#define HWIO_TSENS_TEST_CTRL_TSENS_RO_SELECT_OVR_SHFT                                     0xb
#define HWIO_TSENS_TEST_CTRL_TSENS_RO_ENABLE_OVR_BMSK                                   0x400
#define HWIO_TSENS_TEST_CTRL_TSENS_RO_ENABLE_OVR_SHFT                                     0xa
#define HWIO_TSENS_TEST_CTRL_CM_DFT_ENABLE_BMSK                                         0x200
#define HWIO_TSENS_TEST_CTRL_CM_DFT_ENABLE_SHFT                                           0x9
#define HWIO_TSENS_TEST_CTRL_BYPASS_DIST_SEL_BMSK                                       0x1e0
#define HWIO_TSENS_TEST_CTRL_BYPASS_DIST_SEL_SHFT                                         0x5
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_SEL_BMSK                                         0x1e
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_SEL_SHFT                                          0x1
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_EN_BMSK                                           0x1
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_EN_SHFT                                           0x0
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_DISABLED_FVAL                             0x0
#define HWIO_TSENS_TEST_CTRL_TSENS_TEST_EN_TEST_ENABLED_FVAL                              0x1

#define HWIO_TSENS_MAX_MIN_INT_STATUS_ADDR(x)                                      ((x) + 0x00000010)
#define HWIO_TSENS_MAX_MIN_INT_STATUS_RMSK                                         0xffffffff
#define HWIO_TSENS_MAX_MIN_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_TSENS_MAX_MIN_INT_STATUS_ADDR(x), HWIO_TSENS_MAX_MIN_INT_STATUS_RMSK)
#define HWIO_TSENS_MAX_MIN_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MAX_MIN_INT_STATUS_ADDR(x), m)
#define HWIO_TSENS_MAX_MIN_INT_STATUS_MAX_INT_STATUS_BMSK                          0xffff0000
#define HWIO_TSENS_MAX_MIN_INT_STATUS_MAX_INT_STATUS_SHFT                                0x10
#define HWIO_TSENS_MAX_MIN_INT_STATUS_MIN_INT_STATUS_BMSK                              0xffff
#define HWIO_TSENS_MAX_MIN_INT_STATUS_MIN_INT_STATUS_SHFT                                 0x0

#define HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x)                                       ((x) + 0x00000014)
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_RMSK                                          0xffffffff
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_IN(x)      \
        in_dword_masked(HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x), HWIO_TSENS_MAX_MIN_INT_CLEAR_RMSK)
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x), m)
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_OUT(x, v)      \
        out_dword(HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x),v)
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_MAX_MIN_INT_CLEAR_ADDR(x),m,v,HWIO_TSENS_MAX_MIN_INT_CLEAR_IN(x))
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_MAX_INT_CLEAR_BMSK                            0xffff0000
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_MAX_INT_CLEAR_SHFT                                  0x10
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_MIN_INT_CLEAR_BMSK                                0xffff
#define HWIO_TSENS_MAX_MIN_INT_CLEAR_MIN_INT_CLEAR_SHFT                                   0x0

#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n)                             ((base) + 0x00000020 + 0x4 * (n))
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_RMSK                                      0x3ffffff
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAXn                                             15
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n), HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_RMSK)
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n),val)
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_ADDR(base,n),mask,val,HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_INI(base,n))
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_STATUS_MASK_BMSK                      0x2000000
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_STATUS_MASK_SHFT                           0x19
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_STATUS_MASK_NORMAL_OPERATION_FVAL           0x0
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_STATUS_MASK_MASK_OFF_MAX_STATUS_FVAL        0x1
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_STATUS_MASK_BMSK                      0x1000000
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_STATUS_MASK_SHFT                           0x18
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_STATUS_MASK_NORMAL_OPERATION_FVAL           0x0
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_STATUS_MASK_MASK_OFF_MIN_STATUS_FVAL        0x1
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_THRESHOLD_BMSK                         0xfff000
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MAX_THRESHOLD_SHFT                              0xc
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_THRESHOLD_BMSK                            0xfff
#define HWIO_TSENS_Sn_MAX_MIN_STATUS_CTRL_MIN_THRESHOLD_SHFT                              0x0

#define HWIO_TSENS_Sn_CONVERSION_ADDR(base,n)                                      ((base) + 0x00000060 + 0x4 * (n))
#define HWIO_TSENS_Sn_CONVERSION_RMSK                                               0x1ffffff
#define HWIO_TSENS_Sn_CONVERSION_MAXn                                                      15
#define HWIO_TSENS_Sn_CONVERSION_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_CONVERSION_ADDR(base,n), HWIO_TSENS_Sn_CONVERSION_RMSK)
#define HWIO_TSENS_Sn_CONVERSION_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_CONVERSION_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_CONVERSION_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_CONVERSION_ADDR(base,n),val)
#define HWIO_TSENS_Sn_CONVERSION_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_CONVERSION_ADDR(base,n),mask,val,HWIO_TSENS_Sn_CONVERSION_INI(base,n))
#define HWIO_TSENS_Sn_CONVERSION_SHIFT_BMSK                                         0x1800000
#define HWIO_TSENS_Sn_CONVERSION_SHIFT_SHFT                                              0x17
#define HWIO_TSENS_Sn_CONVERSION_SLOPE_BMSK                                          0x7ffc00
#define HWIO_TSENS_Sn_CONVERSION_SLOPE_SHFT                                               0xa
#define HWIO_TSENS_Sn_CONVERSION_CZERO_BMSK                                             0x3ff
#define HWIO_TSENS_Sn_CONVERSION_CZERO_SHFT                                               0x0

#define HWIO_TSENS_Sn_ID_ASSIGNMENT_ADDR(base,n)                                      ((base) + 0x000000a0 + 0x4 * (n))
#define HWIO_TSENS_Sn_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_Sn_ID_ASSIGNMENT_MAXn                                                   15
#define HWIO_TSENS_Sn_ID_ASSIGNMENT_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_ID_ASSIGNMENT_ADDR(base,n), HWIO_TSENS_Sn_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_Sn_ID_ASSIGNMENT_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_ID_ASSIGNMENT_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_ID_ASSIGNMENT_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_ID_ASSIGNMENT_ADDR(base,n),val)
#define HWIO_TSENS_Sn_ID_ASSIGNMENT_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_ID_ASSIGNMENT_ADDR(base,n),mask,val,HWIO_TSENS_Sn_ID_ASSIGNMENT_INI(base,n))
#define HWIO_TSENS_Sn_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_Sn_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000a0)
#define HWIO_TSENS_S0_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S0_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S0_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S0_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S0_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S0_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S0_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S0_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S0_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S0_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000a4)
#define HWIO_TSENS_S1_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S1_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S1_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S1_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S1_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S1_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S1_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S1_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S1_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S1_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000a8)
#define HWIO_TSENS_S2_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S2_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S2_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S2_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S2_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S2_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S2_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S2_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S2_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S2_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000ac)
#define HWIO_TSENS_S3_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S3_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S3_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S3_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S3_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S3_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S3_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S3_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S3_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S3_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000b0)
#define HWIO_TSENS_S4_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S4_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S4_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S4_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S4_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S4_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S4_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S4_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S4_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S4_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000b4)
#define HWIO_TSENS_S5_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S5_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S5_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S5_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S5_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S5_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S5_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S5_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S5_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S5_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000b8)
#define HWIO_TSENS_S6_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S6_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S6_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S6_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S6_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S6_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S6_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S6_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S6_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S6_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000bc)
#define HWIO_TSENS_S7_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S7_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S7_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S7_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S7_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S7_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S7_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S7_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S7_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S7_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000c0)
#define HWIO_TSENS_S8_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S8_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S8_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S8_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S8_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S8_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S8_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S8_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S8_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S8_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x)                                        ((x) + 0x000000c4)
#define HWIO_TSENS_S9_ID_ASSIGNMENT_RMSK                                                  0xf
#define HWIO_TSENS_S9_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S9_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S9_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S9_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S9_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S9_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S9_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S9_ID_ASSIGNMENT_SENSOR_ID_BMSK                                        0xf
#define HWIO_TSENS_S9_ID_ASSIGNMENT_SENSOR_ID_SHFT                                        0x0

#define HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000c8)
#define HWIO_TSENS_S10_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S10_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S10_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S10_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S10_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S10_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S10_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S10_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S10_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S10_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000cc)
#define HWIO_TSENS_S11_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S11_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S11_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S11_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S11_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S11_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S11_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S11_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S11_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S11_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000d0)
#define HWIO_TSENS_S12_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S12_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S12_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S12_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S12_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S12_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S12_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S12_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S12_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S12_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000d4)
#define HWIO_TSENS_S13_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S13_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S13_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S13_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S13_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S13_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S13_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S13_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S13_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S13_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000d8)
#define HWIO_TSENS_S14_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S14_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S14_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S14_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S14_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S14_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S14_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S14_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S14_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S14_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x)                                       ((x) + 0x000000dc)
#define HWIO_TSENS_S15_ID_ASSIGNMENT_RMSK                                                 0xf
#define HWIO_TSENS_S15_ID_ASSIGNMENT_IN(x)      \
        in_dword_masked(HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x), HWIO_TSENS_S15_ID_ASSIGNMENT_RMSK)
#define HWIO_TSENS_S15_ID_ASSIGNMENT_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x), m)
#define HWIO_TSENS_S15_ID_ASSIGNMENT_OUT(x, v)      \
        out_dword(HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x),v)
#define HWIO_TSENS_S15_ID_ASSIGNMENT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_S15_ID_ASSIGNMENT_ADDR(x),m,v,HWIO_TSENS_S15_ID_ASSIGNMENT_IN(x))
#define HWIO_TSENS_S15_ID_ASSIGNMENT_SENSOR_ID_BMSK                                       0xf
#define HWIO_TSENS_S15_ID_ASSIGNMENT_SENSOR_ID_SHFT                                       0x0

#define HWIO_TS_CONTROL_ADDR(x)                                                    ((x) + 0x000000e0)
#define HWIO_TS_CONTROL_RMSK                                                        0x1ffffff
#define HWIO_TS_CONTROL_IN(x)      \
        in_dword_masked(HWIO_TS_CONTROL_ADDR(x), HWIO_TS_CONTROL_RMSK)
#define HWIO_TS_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_TS_CONTROL_ADDR(x), m)
#define HWIO_TS_CONTROL_OUT(x, v)      \
        out_dword(HWIO_TS_CONTROL_ADDR(x),v)
#define HWIO_TS_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TS_CONTROL_ADDR(x),m,v,HWIO_TS_CONTROL_IN(x))
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_ISENSE_OUT_EN_BMSK                      0x1000000
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_ISENSE_OUT_EN_SHFT                           0x18
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_ISENSE_OUT_EN_DISABLED_FVAL                   0x0
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_ISENSE_OUT_EN_ENABLED_FVAL                    0x1
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_BG_CORE_EN_BMSK                          0x800000
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_BG_CORE_EN_SHFT                              0x17
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_BG_CORE_EN_DISABLED_FVAL                      0x0
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_BG_CORE_EN_ENABLED_FVAL                       0x1
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_FREQ_BMSK                                0x400000
#define HWIO_TS_CONTROL_REF_CURR_MIRROR_DEM_FREQ_SHFT                                    0x16
#define HWIO_TS_CONTROL_SLOPE_CALIBRATION_REF_EN_BMSK                                0x200000
#define HWIO_TS_CONTROL_SLOPE_CALIBRATION_REF_EN_SHFT                                    0x15
#define HWIO_TS_CONTROL_SLOPE_CALIBRATION_REF_EN_DISABLED_FVAL                            0x0
#define HWIO_TS_CONTROL_SLOPE_CALIBRATION_REF_EN_ENABLED_FVAL                             0x1
#define HWIO_TS_CONTROL_REF_BJT_DEM_BMSK                                             0x180000
#define HWIO_TS_CONTROL_REF_BJT_DEM_SHFT                                                 0x13
#define HWIO_TS_CONTROL_SEND_QUANTIZER_OUTPUT_BMSK                                    0x40000
#define HWIO_TS_CONTROL_SEND_QUANTIZER_OUTPUT_SHFT                                       0x12
#define HWIO_TS_CONTROL_SEND_QUANTIZER_OUTPUT_DISABLED_FVAL                               0x0
#define HWIO_TS_CONTROL_SEND_QUANTIZER_OUTPUT_ENABLED_FVAL                                0x1
#define HWIO_TS_CONTROL_REF_OPAMP_CHOPPING_BMSK                                       0x30000
#define HWIO_TS_CONTROL_REF_OPAMP_CHOPPING_SHFT                                          0x10
#define HWIO_TS_CONTROL_DA_RESET_IN_TEST_MODE_BMSK                                     0x8000
#define HWIO_TS_CONTROL_DA_RESET_IN_TEST_MODE_SHFT                                        0xf
#define HWIO_TS_CONTROL_VBE_R_SENSE_CURRENT_DEM_BMSK                                   0x6000
#define HWIO_TS_CONTROL_VBE_R_SENSE_CURRENT_DEM_SHFT                                      0xd
#define HWIO_TS_CONTROL_BANDGAP_CORE_VREF_RES_TRIM_BMSK                                0x1c00
#define HWIO_TS_CONTROL_BANDGAP_CORE_VREF_RES_TRIM_SHFT                                   0xa
#define HWIO_TS_CONTROL_SLOPE_CALIBRATION_REF_SEL_BMSK                                  0x200
#define HWIO_TS_CONTROL_SLOPE_CALIBRATION_REF_SEL_SHFT                                    0x9
#define HWIO_TS_CONTROL_SLOPE_CALIBRATION_REF_SEL_VR1_FVAL                                0x0
#define HWIO_TS_CONTROL_SLOPE_CALIBRATION_REF_SEL_VR2_FVAL                                0x1
#define HWIO_TS_CONTROL_VBE_R_SENSE_OPAMP_CHOPPING_BMSK                                 0x180
#define HWIO_TS_CONTROL_VBE_R_SENSE_OPAMP_CHOPPING_SHFT                                   0x7
#define HWIO_TS_CONTROL_RO_CLK_TO_PIN_BMSK                                               0x40
#define HWIO_TS_CONTROL_RO_CLK_TO_PIN_SHFT                                                0x6
#define HWIO_TS_CONTROL_RO_CLK_TO_PIN_DISABLED_FVAL                                       0x0
#define HWIO_TS_CONTROL_RO_CLK_TO_PIN_ENABLED_FVAL                                        0x1
#define HWIO_TS_CONTROL_BANDGAP_CORE_CTAT_RES_TRIM_BMSK                                  0x3c
#define HWIO_TS_CONTROL_BANDGAP_CORE_CTAT_RES_TRIM_SHFT                                   0x2
#define HWIO_TS_CONTROL_SENSE_BJT_DEM_BMSK                                                0x3
#define HWIO_TS_CONTROL_SENSE_BJT_DEM_SHFT                                                0x0

#define HWIO_TS_CONFIG_ADDR(x)                                                     ((x) + 0x000000e4)
#define HWIO_TS_CONFIG_RMSK                                                              0xff
#define HWIO_TS_CONFIG_IN(x)      \
        in_dword_masked(HWIO_TS_CONFIG_ADDR(x), HWIO_TS_CONFIG_RMSK)
#define HWIO_TS_CONFIG_INM(x, m)      \
        in_dword_masked(HWIO_TS_CONFIG_ADDR(x), m)
#define HWIO_TS_CONFIG_OUT(x, v)      \
        out_dword(HWIO_TS_CONFIG_ADDR(x),v)
#define HWIO_TS_CONFIG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TS_CONFIG_ADDR(x),m,v,HWIO_TS_CONFIG_IN(x))
#define HWIO_TS_CONFIG_CLOCK_FREQ_BMSK                                                   0xc0
#define HWIO_TS_CONFIG_CLOCK_FREQ_SHFT                                                    0x6
#define HWIO_TS_CONFIG_NOT_USED_0_BMSK                                                   0x20
#define HWIO_TS_CONFIG_NOT_USED_0_SHFT                                                    0x5
#define HWIO_TS_CONFIG_SLOPE_CALIBRATION_REF_EN_BMSK                                     0x10
#define HWIO_TS_CONFIG_SLOPE_CALIBRATION_REF_EN_SHFT                                      0x4
#define HWIO_TS_CONFIG_ISENSE_MODE_FOR_BASE_RES_CAL_BMSK                                  0x8
#define HWIO_TS_CONFIG_ISENSE_MODE_FOR_BASE_RES_CAL_SHFT                                  0x3
#define HWIO_TS_CONFIG_VBE_R_SENSE_RES_TRIM_BMSK                                          0x7
#define HWIO_TS_CONFIG_VBE_R_SENSE_RES_TRIM_SHFT                                          0x0

#define HWIO_TSENS_SIDEBAND_EN_ADDR(x)                                             ((x) + 0x000000e8)
#define HWIO_TSENS_SIDEBAND_EN_RMSK                                                    0xffff
#define HWIO_TSENS_SIDEBAND_EN_IN(x)      \
        in_dword_masked(HWIO_TSENS_SIDEBAND_EN_ADDR(x), HWIO_TSENS_SIDEBAND_EN_RMSK)
#define HWIO_TSENS_SIDEBAND_EN_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_SIDEBAND_EN_ADDR(x), m)
#define HWIO_TSENS_SIDEBAND_EN_OUT(x, v)      \
        out_dword(HWIO_TSENS_SIDEBAND_EN_ADDR(x),v)
#define HWIO_TSENS_SIDEBAND_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_SIDEBAND_EN_ADDR(x),m,v,HWIO_TSENS_SIDEBAND_EN_IN(x))
#define HWIO_TSENS_SIDEBAND_EN_SENSOR_EN_BMSK                                          0xffff
#define HWIO_TSENS_SIDEBAND_EN_SENSOR_EN_SHFT                                             0x0

#define HWIO_TSENS_TBCB_CONTROL_ADDR(x)                                            ((x) + 0x000000ec)
#define HWIO_TSENS_TBCB_CONTROL_RMSK                                               0xffffffff
#define HWIO_TSENS_TBCB_CONTROL_IN(x)      \
        in_dword_masked(HWIO_TSENS_TBCB_CONTROL_ADDR(x), HWIO_TSENS_TBCB_CONTROL_RMSK)
#define HWIO_TSENS_TBCB_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TBCB_CONTROL_ADDR(x), m)
#define HWIO_TSENS_TBCB_CONTROL_OUT(x, v)      \
        out_dword(HWIO_TSENS_TBCB_CONTROL_ADDR(x),v)
#define HWIO_TSENS_TBCB_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_TBCB_CONTROL_ADDR(x),m,v,HWIO_TSENS_TBCB_CONTROL_IN(x))
#define HWIO_TSENS_TBCB_CONTROL_TBCB_ACK_DELAY_BMSK                                0xff000000
#define HWIO_TSENS_TBCB_CONTROL_TBCB_ACK_DELAY_SHFT                                      0x18
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLK_DIV_BMSK                                    0xff0000
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLK_DIV_SHFT                                        0x10
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLIENT_EN_BMSK                                    0xffff
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLIENT_EN_SHFT                                       0x0
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLIENT_EN_DISABLED_FVAL                              0x0
#define HWIO_TSENS_TBCB_CONTROL_TBCB_CLIENT_EN_ENABLED_FVAL                               0x1

#define HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n)                                  ((base) + 0x000000f0 + 0x4 * (n))
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_RMSK                                              0xffff
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_MAXn                                                  15
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n), HWIO_TSENS_TBCB_CLIENT_n_REQ_RMSK)
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n), mask)
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n),val)
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_TBCB_CLIENT_n_REQ_ADDR(base,n),mask,val,HWIO_TSENS_TBCB_CLIENT_n_REQ_INI(base,n))
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_TBCB_CLIENT_REQ_SENSOR_BMSK                       0xffff
#define HWIO_TSENS_TBCB_CLIENT_n_REQ_TBCB_CLIENT_REQ_SENSOR_SHFT                          0x0

#define HWIO_TSENS_TBCB_OVERRIDE_ADDR(x)                                           ((x) + 0x00000130)
#define HWIO_TSENS_TBCB_OVERRIDE_RMSK                                                 0x3ffff
#define HWIO_TSENS_TBCB_OVERRIDE_IN(x)      \
        in_dword_masked(HWIO_TSENS_TBCB_OVERRIDE_ADDR(x), HWIO_TSENS_TBCB_OVERRIDE_RMSK)
#define HWIO_TSENS_TBCB_OVERRIDE_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TBCB_OVERRIDE_ADDR(x), m)
#define HWIO_TSENS_TBCB_OVERRIDE_OUT(x, v)      \
        out_dword(HWIO_TSENS_TBCB_OVERRIDE_ADDR(x),v)
#define HWIO_TSENS_TBCB_OVERRIDE_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_TBCB_OVERRIDE_ADDR(x),m,v,HWIO_TSENS_TBCB_OVERRIDE_IN(x))
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_DATA_BMSK                              0x3fffc
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_DATA_SHFT                                  0x2
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_CMD_BMSK                                   0x2
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_CMD_SHFT                                   0x1
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_EN_BMSK                                    0x1
#define HWIO_TSENS_TBCB_OVERRIDE_TBCB_OVERRIDE_EN_SHFT                                    0x0

#define HWIO_TSENS_WATCHDOG_CTRL_ADDR(x)                                           ((x) + 0x00000140)
#define HWIO_TSENS_WATCHDOG_CTRL_RMSK                                                 0xfffff
#define HWIO_TSENS_WATCHDOG_CTRL_IN(x)      \
        in_dword_masked(HWIO_TSENS_WATCHDOG_CTRL_ADDR(x), HWIO_TSENS_WATCHDOG_CTRL_RMSK)
#define HWIO_TSENS_WATCHDOG_CTRL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_WATCHDOG_CTRL_ADDR(x), m)
#define HWIO_TSENS_WATCHDOG_CTRL_OUT(x, v)      \
        out_dword(HWIO_TSENS_WATCHDOG_CTRL_ADDR(x),v)
#define HWIO_TSENS_WATCHDOG_CTRL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_WATCHDOG_CTRL_ADDR(x),m,v,HWIO_TSENS_WATCHDOG_CTRL_IN(x))
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_DISABLE_KNOB_BMSK                           0x80000
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_DISABLE_KNOB_SHFT                              0x13
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_DISABLE_KNOB_TBCB_ONLY_FVAL                     0x0
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_DISABLE_KNOB_MAIN_TSENS_EN_FVAL                 0x1
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_SELF_RESET_BMSK                             0x40000
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_SELF_RESET_SHFT                                0x12
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_LOAD_VAL_BMSK                               0x3fffc
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_LOAD_VAL_SHFT                                   0x2
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_TRIGGER_STATE_BMSK                              0x2
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_TRIGGER_STATE_SHFT                              0x1
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_TRIGGER_STATE_IDLE_FVAL                         0x0
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_TRIGGER_STATE_GLOBAL_MAX_FVAL                   0x1
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_EN_BMSK                                         0x1
#define HWIO_TSENS_WATCHDOG_CTRL_WATCHDOG_EN_SHFT                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: TSENS_TM
 *--------------------------------------------------------------------------*/
#define HWIO_TSENS_CONTROLLER_ID_ADDR(x)                                             ((x) + 0x00000000)
#define HWIO_TSENS_CONTROLLER_ID_RMSK                                                       0xf
#define HWIO_TSENS_CONTROLLER_ID_IN(x)      \
        in_dword_masked(HWIO_TSENS_CONTROLLER_ID_ADDR(x), HWIO_TSENS_CONTROLLER_ID_RMSK)
#define HWIO_TSENS_CONTROLLER_ID_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CONTROLLER_ID_ADDR(x), m)
#define HWIO_TSENS_CONTROLLER_ID_CONTROLLER_ID_BMSK                                         0xf
#define HWIO_TSENS_CONTROLLER_ID_CONTROLLER_ID_SHFT                                         0x0

#define HWIO_TSENS_TM_INT_EN_ADDR(x)                                                 ((x) + 0x00000004)
#define HWIO_TSENS_TM_INT_EN_RMSK                                                           0x7
#define HWIO_TSENS_TM_INT_EN_IN(x)      \
        in_dword_masked(HWIO_TSENS_TM_INT_EN_ADDR(x), HWIO_TSENS_TM_INT_EN_RMSK)
#define HWIO_TSENS_TM_INT_EN_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TM_INT_EN_ADDR(x), m)
#define HWIO_TSENS_TM_INT_EN_OUT(x, v)      \
        out_dword(HWIO_TSENS_TM_INT_EN_ADDR(x),v)
#define HWIO_TSENS_TM_INT_EN_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_TM_INT_EN_ADDR(x),m,v,HWIO_TSENS_TM_INT_EN_IN(x))
#define HWIO_TSENS_TM_INT_EN_CRITICAL_INT_EN_BMSK                                           0x4
#define HWIO_TSENS_TM_INT_EN_CRITICAL_INT_EN_SHFT                                           0x2
#define HWIO_TSENS_TM_INT_EN_CRITICAL_INT_EN_DISABLED_FVAL                                  0x0
#define HWIO_TSENS_TM_INT_EN_CRITICAL_INT_EN_ENABLED_FVAL                                   0x1
#define HWIO_TSENS_TM_INT_EN_UPPER_INT_EN_BMSK                                              0x2
#define HWIO_TSENS_TM_INT_EN_UPPER_INT_EN_SHFT                                              0x1
#define HWIO_TSENS_TM_INT_EN_UPPER_INT_EN_DISABLED_FVAL                                     0x0
#define HWIO_TSENS_TM_INT_EN_UPPER_INT_EN_ENABLED_FVAL                                      0x1
#define HWIO_TSENS_TM_INT_EN_LOWER_INT_EN_BMSK                                              0x1
#define HWIO_TSENS_TM_INT_EN_LOWER_INT_EN_SHFT                                              0x0
#define HWIO_TSENS_TM_INT_EN_LOWER_INT_EN_DISABLED_FVAL                                     0x0
#define HWIO_TSENS_TM_INT_EN_LOWER_INT_EN_ENABLED_FVAL                                      0x1

#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_ADDR(x)                                    ((x) + 0x00000008)
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_RMSK                                       0xffffffff
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_STATUS_ADDR(x), HWIO_TSENS_UPPER_LOWER_INT_STATUS_RMSK)
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_STATUS_ADDR(x), m)
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_UPPER_INT_STATUS_BMSK                      0xffff0000
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_UPPER_INT_STATUS_SHFT                            0x10
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_LOWER_INT_STATUS_BMSK                          0xffff
#define HWIO_TSENS_UPPER_LOWER_INT_STATUS_LOWER_INT_STATUS_SHFT                             0x0

#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x)                                     ((x) + 0x0000000c)
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_RMSK                                        0xffffffff
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_IN(x)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x), HWIO_TSENS_UPPER_LOWER_INT_CLEAR_RMSK)
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x), m)
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_OUT(x, v)      \
        out_dword(HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x),v)
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_UPPER_LOWER_INT_CLEAR_ADDR(x),m,v,HWIO_TSENS_UPPER_LOWER_INT_CLEAR_IN(x))
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_UPPER_INT_CLEAR_BMSK                        0xffff0000
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_UPPER_INT_CLEAR_SHFT                              0x10
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_LOWER_INT_CLEAR_BMSK                            0xffff
#define HWIO_TSENS_UPPER_LOWER_INT_CLEAR_LOWER_INT_CLEAR_SHFT                               0x0

#define HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x)                                      ((x) + 0x00000010)
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_RMSK                                         0xffffffff
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_IN(x)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x), HWIO_TSENS_UPPER_LOWER_INT_MASK_RMSK)
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x), m)
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_OUT(x, v)      \
        out_dword(HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x),v)
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_UPPER_LOWER_INT_MASK_ADDR(x),m,v,HWIO_TSENS_UPPER_LOWER_INT_MASK_IN(x))
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_UPPER_INT_MASK_BMSK                          0xffff0000
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_UPPER_INT_MASK_SHFT                                0x10
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_LOWER_INT_MASK_BMSK                              0xffff
#define HWIO_TSENS_UPPER_LOWER_INT_MASK_LOWER_INT_MASK_SHFT                                 0x0

#define HWIO_TSENS_CRITICAL_INT_STATUS_ADDR(x)                                       ((x) + 0x00000014)
#define HWIO_TSENS_CRITICAL_INT_STATUS_RMSK                                          0xc000ffff
#define HWIO_TSENS_CRITICAL_INT_STATUS_IN(x)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_STATUS_ADDR(x), HWIO_TSENS_CRITICAL_INT_STATUS_RMSK)
#define HWIO_TSENS_CRITICAL_INT_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_STATUS_ADDR(x), m)
#define HWIO_TSENS_CRITICAL_INT_STATUS_WATCHDOG_BARK_BMSK                            0x80000000
#define HWIO_TSENS_CRITICAL_INT_STATUS_WATCHDOG_BARK_SHFT                                  0x1f
#define HWIO_TSENS_CRITICAL_INT_STATUS_CYCLE_COMPLETION_MONITOR_BMSK                 0x40000000
#define HWIO_TSENS_CRITICAL_INT_STATUS_CYCLE_COMPLETION_MONITOR_SHFT                       0x1e
#define HWIO_TSENS_CRITICAL_INT_STATUS_CRITICAL_INT_STATUS_BMSK                          0xffff
#define HWIO_TSENS_CRITICAL_INT_STATUS_CRITICAL_INT_STATUS_SHFT                             0x0

#define HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x)                                        ((x) + 0x00000018)
#define HWIO_TSENS_CRITICAL_INT_CLEAR_RMSK                                           0xc000ffff
#define HWIO_TSENS_CRITICAL_INT_CLEAR_IN(x)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x), HWIO_TSENS_CRITICAL_INT_CLEAR_RMSK)
#define HWIO_TSENS_CRITICAL_INT_CLEAR_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x), m)
#define HWIO_TSENS_CRITICAL_INT_CLEAR_OUT(x, v)      \
        out_dword(HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x),v)
#define HWIO_TSENS_CRITICAL_INT_CLEAR_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_CRITICAL_INT_CLEAR_ADDR(x),m,v,HWIO_TSENS_CRITICAL_INT_CLEAR_IN(x))
#define HWIO_TSENS_CRITICAL_INT_CLEAR_WATCHDOG_BARK_BMSK                             0x80000000
#define HWIO_TSENS_CRITICAL_INT_CLEAR_WATCHDOG_BARK_SHFT                                   0x1f
#define HWIO_TSENS_CRITICAL_INT_CLEAR_CYCLE_COMPLETION_MONITOR_BMSK                  0x40000000
#define HWIO_TSENS_CRITICAL_INT_CLEAR_CYCLE_COMPLETION_MONITOR_SHFT                        0x1e
#define HWIO_TSENS_CRITICAL_INT_CLEAR_CRITICAL_INT_CLEAR_BMSK                            0xffff
#define HWIO_TSENS_CRITICAL_INT_CLEAR_CRITICAL_INT_CLEAR_SHFT                               0x0

#define HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x)                                         ((x) + 0x0000001c)
#define HWIO_TSENS_CRITICAL_INT_MASK_RMSK                                            0xc000ffff
#define HWIO_TSENS_CRITICAL_INT_MASK_IN(x)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x), HWIO_TSENS_CRITICAL_INT_MASK_RMSK)
#define HWIO_TSENS_CRITICAL_INT_MASK_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x), m)
#define HWIO_TSENS_CRITICAL_INT_MASK_OUT(x, v)      \
        out_dword(HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x),v)
#define HWIO_TSENS_CRITICAL_INT_MASK_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_CRITICAL_INT_MASK_ADDR(x),m,v,HWIO_TSENS_CRITICAL_INT_MASK_IN(x))
#define HWIO_TSENS_CRITICAL_INT_MASK_WATCHDOG_BARK_BMSK                              0x80000000
#define HWIO_TSENS_CRITICAL_INT_MASK_WATCHDOG_BARK_SHFT                                    0x1f
#define HWIO_TSENS_CRITICAL_INT_MASK_CYCLE_COMPLETION_MONITOR_BMSK                   0x40000000
#define HWIO_TSENS_CRITICAL_INT_MASK_CYCLE_COMPLETION_MONITOR_SHFT                         0x1e
#define HWIO_TSENS_CRITICAL_INT_MASK_CRITICAL_INT_MASK_BMSK                              0xffff
#define HWIO_TSENS_CRITICAL_INT_MASK_CRITICAL_INT_MASK_SHFT                                 0x0

#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n)                             ((base) + 0x00000020 + 0x4 * (n))
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_RMSK                                       0xffffff
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_MAXn                                             15
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n), HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_RMSK)
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n),val)
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_ADDR(base,n),mask,val,HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_INI(base,n))
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_UPPER_THRESHOLD_BMSK                       0xfff000
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_UPPER_THRESHOLD_SHFT                            0xc
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_LOWER_THRESHOLD_BMSK                          0xfff
#define HWIO_TSENS_Sn_UPPER_LOWER_THRESHOLD_LOWER_THRESHOLD_SHFT                            0x0

#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n)                                ((base) + 0x00000060 + 0x4 * (n))
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_RMSK                                             0xfff
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_MAXn                                                15
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n), HWIO_TSENS_Sn_CRITICAL_THRESHOLD_RMSK)
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_OUTI(base,n,val)    \
        out_dword(HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n),val)
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_TSENS_Sn_CRITICAL_THRESHOLD_ADDR(base,n),mask,val,HWIO_TSENS_Sn_CRITICAL_THRESHOLD_INI(base,n))
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_CRITICAL_THRESHOLD_BMSK                          0xfff
#define HWIO_TSENS_Sn_CRITICAL_THRESHOLD_CRITICAL_THRESHOLD_SHFT                            0x0

#define HWIO_TSENS_Sn_STATUS_ADDR(base,n)                                            ((base) + 0x000000a0 + 0x4 * (n))
#define HWIO_TSENS_Sn_STATUS_RMSK                                                      0x3fffff
#define HWIO_TSENS_Sn_STATUS_MAXn                                                            15
#define HWIO_TSENS_Sn_STATUS_INI(base,n)        \
        in_dword_masked(HWIO_TSENS_Sn_STATUS_ADDR(base,n), HWIO_TSENS_Sn_STATUS_RMSK)
#define HWIO_TSENS_Sn_STATUS_INMI(base,n,mask)    \
        in_dword_masked(HWIO_TSENS_Sn_STATUS_ADDR(base,n), mask)
#define HWIO_TSENS_Sn_STATUS_VALID_BMSK                                                0x200000
#define HWIO_TSENS_Sn_STATUS_VALID_SHFT                                                    0x15
#define HWIO_TSENS_Sn_STATUS_MAX_STATUS_BMSK                                           0x100000
#define HWIO_TSENS_Sn_STATUS_MAX_STATUS_SHFT                                               0x14
#define HWIO_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_NOT_VIOLATED_FVAL                     0x0
#define HWIO_TSENS_Sn_STATUS_MAX_STATUS_MAX_THRESHOLD_VIOLATED_FVAL                         0x1
#define HWIO_TSENS_Sn_STATUS_CRITICAL_STATUS_BMSK                                       0x80000
#define HWIO_TSENS_Sn_STATUS_CRITICAL_STATUS_SHFT                                          0x13
#define HWIO_TSENS_Sn_STATUS_CRITICAL_STATUS_CRITICAL_THRESHOLD_NOT_VIOLATED_FVAL           0x0
#define HWIO_TSENS_Sn_STATUS_CRITICAL_STATUS_CRITICAL_THRESHOLD_VIOLATED_FVAL               0x1
#define HWIO_TSENS_Sn_STATUS_UPPER_STATUS_BMSK                                          0x40000
#define HWIO_TSENS_Sn_STATUS_UPPER_STATUS_SHFT                                             0x12
#define HWIO_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_NOT_VIOLATED_FVAL                 0x0
#define HWIO_TSENS_Sn_STATUS_UPPER_STATUS_UPPER_THRESHOLD_VIOLATED_FVAL                     0x1
#define HWIO_TSENS_Sn_STATUS_LOWER_STATUS_BMSK                                          0x20000
#define HWIO_TSENS_Sn_STATUS_LOWER_STATUS_SHFT                                             0x11
#define HWIO_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_NOT_VIOLATED_FVAL                 0x0
#define HWIO_TSENS_Sn_STATUS_LOWER_STATUS_LOWER_THRESHOLD_VIOLATED_FVAL                     0x1
#define HWIO_TSENS_Sn_STATUS_MIN_STATUS_BMSK                                            0x10000
#define HWIO_TSENS_Sn_STATUS_MIN_STATUS_SHFT                                               0x10
#define HWIO_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_NOT_VIOLATED_FVAL                     0x0
#define HWIO_TSENS_Sn_STATUS_MIN_STATUS_MIN_THRESHOLD_VIOLATED_FVAL                         0x1
#define HWIO_TSENS_Sn_STATUS_SENSOR_ID_BMSK                                              0xf000
#define HWIO_TSENS_Sn_STATUS_SENSOR_ID_SHFT                                                 0xc
#define HWIO_TSENS_Sn_STATUS_LAST_TEMP_BMSK                                               0xfff
#define HWIO_TSENS_Sn_STATUS_LAST_TEMP_SHFT                                                 0x0

#define HWIO_TSENS_MAX_TEMP_ADDR(x)                                                  ((x) + 0x000000e0)
#define HWIO_TSENS_MAX_TEMP_RMSK                                                         0xffff
#define HWIO_TSENS_MAX_TEMP_IN(x)      \
        in_dword_masked(HWIO_TSENS_MAX_TEMP_ADDR(x), HWIO_TSENS_MAX_TEMP_RMSK)
#define HWIO_TSENS_MAX_TEMP_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_MAX_TEMP_ADDR(x), m)
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_SENSOR_ID_BMSK                                      0xf000
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_SENSOR_ID_SHFT                                         0xc
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_BMSK                                                 0xfff
#define HWIO_TSENS_MAX_TEMP_MAX_TEMP_SHFT                                                   0x0

#define HWIO_TSENS_TRDY_ADDR(x)                                                      ((x) + 0x000000e4)
#define HWIO_TSENS_TRDY_RMSK                                                                0xf
#define HWIO_TSENS_TRDY_IN(x)      \
        in_dword_masked(HWIO_TSENS_TRDY_ADDR(x), HWIO_TSENS_TRDY_RMSK)
#define HWIO_TSENS_TRDY_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_TRDY_ADDR(x), m)
#define HWIO_TSENS_TRDY_FIRST_ROUND_COMPLETE_BMSK                                           0x8
#define HWIO_TSENS_TRDY_FIRST_ROUND_COMPLETE_SHFT                                           0x3
#define HWIO_TSENS_TRDY_OSC_CLK_OFF_BMSK                                                    0x4
#define HWIO_TSENS_TRDY_OSC_CLK_OFF_SHFT                                                    0x2
#define HWIO_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_ON_FVAL                                          0x0
#define HWIO_TSENS_TRDY_OSC_CLK_OFF_CLK_IS_OFF_FVAL                                         0x1
#define HWIO_TSENS_TRDY_SLP_CLK_OFF_BMSK                                                    0x2
#define HWIO_TSENS_TRDY_SLP_CLK_OFF_SHFT                                                    0x1
#define HWIO_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_ON_FVAL                                          0x0
#define HWIO_TSENS_TRDY_SLP_CLK_OFF_CLK_IS_OFF_FVAL                                         0x1
#define HWIO_TSENS_TRDY_TRDY_BMSK                                                           0x1
#define HWIO_TSENS_TRDY_TRDY_SHFT                                                           0x0
#define HWIO_TSENS_TRDY_TRDY_TEMPERATURE_MEASUREMENT_IN_PROGRESS_FVAL                       0x0
#define HWIO_TSENS_TRDY_TRDY_TEMPERATURE_READING_IS_READY_FVAL                              0x1

#define HWIO_TSENS_DEBUG_CONTROL_ADDR(x)                                             ((x) + 0x00000130)
#define HWIO_TSENS_DEBUG_CONTROL_RMSK                                                  0x7fffff
#define HWIO_TSENS_DEBUG_CONTROL_IN(x)      \
        in_dword_masked(HWIO_TSENS_DEBUG_CONTROL_ADDR(x), HWIO_TSENS_DEBUG_CONTROL_RMSK)
#define HWIO_TSENS_DEBUG_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_DEBUG_CONTROL_ADDR(x), m)
#define HWIO_TSENS_DEBUG_CONTROL_OUT(x, v)      \
        out_dword(HWIO_TSENS_DEBUG_CONTROL_ADDR(x),v)
#define HWIO_TSENS_DEBUG_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_DEBUG_CONTROL_ADDR(x),m,v,HWIO_TSENS_DEBUG_CONTROL_IN(x))
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_ID_BMSK                               0x7fff80
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_ID_SHFT                                    0x7
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_SLV_DEBUG_BUS_EN_BMSK                          0x40
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_SLV_DEBUG_BUS_EN_SHFT                           0x6
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_SLV_DEBUG_BUS_EN_DISABLED_FVAL                  0x0
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_SLV_DEBUG_BUS_EN_ENABLED_FVAL                   0x1
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_MSTR_DEBUG_BUS_EN_BMSK                         0x20
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_MSTR_DEBUG_BUS_EN_SHFT                          0x5
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_MSTR_DEBUG_BUS_EN_DISABLED_FVAL                 0x0
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_HBRG_MSTR_DEBUG_BUS_EN_ENABLED_FVAL                  0x1
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_SEL_BMSK                                  0x1e
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_SEL_SHFT                                   0x1
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_EN_BMSK                                    0x1
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_EN_SHFT                                    0x0
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_EN_DISABLED_FVAL                           0x0
#define HWIO_TSENS_DEBUG_CONTROL_TSENS_DEBUG_BUS_EN_ENABLED_FVAL                            0x1

#define HWIO_TSENS_DEBUG_READ_ADDR(x)                                                ((x) + 0x00000134)
#define HWIO_TSENS_DEBUG_READ_RMSK                                                   0xffffffff
#define HWIO_TSENS_DEBUG_READ_IN(x)      \
        in_dword_masked(HWIO_TSENS_DEBUG_READ_ADDR(x), HWIO_TSENS_DEBUG_READ_RMSK)
#define HWIO_TSENS_DEBUG_READ_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_DEBUG_READ_ADDR(x), m)
#define HWIO_TSENS_DEBUG_READ_DEBUG_DATA_READ_BMSK                                   0xffffffff
#define HWIO_TSENS_DEBUG_READ_DEBUG_DATA_READ_SHFT                                          0x0

#define HWIO_TSENS_QDSS_CONTROL_ADDR(x)                                              ((x) + 0x00000138)
#define HWIO_TSENS_QDSS_CONTROL_RMSK                                                     0xffff
#define HWIO_TSENS_QDSS_CONTROL_IN(x)      \
        in_dword_masked(HWIO_TSENS_QDSS_CONTROL_ADDR(x), HWIO_TSENS_QDSS_CONTROL_RMSK)
#define HWIO_TSENS_QDSS_CONTROL_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_QDSS_CONTROL_ADDR(x), m)
#define HWIO_TSENS_QDSS_CONTROL_OUT(x, v)      \
        out_dword(HWIO_TSENS_QDSS_CONTROL_ADDR(x),v)
#define HWIO_TSENS_QDSS_CONTROL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_TSENS_QDSS_CONTROL_ADDR(x),m,v,HWIO_TSENS_QDSS_CONTROL_IN(x))
#define HWIO_TSENS_QDSS_CONTROL_QDSS_SENSOR_EN_BMSK                                      0xffff
#define HWIO_TSENS_QDSS_CONTROL_QDSS_SENSOR_EN_SHFT                                         0x0

#define HWIO_TSENS_WATCHDOG_LOG_ADDR(x)                                              ((x) + 0x0000013c)
#define HWIO_TSENS_WATCHDOG_LOG_RMSK                                                       0xff
#define HWIO_TSENS_WATCHDOG_LOG_IN(x)      \
        in_dword_masked(HWIO_TSENS_WATCHDOG_LOG_ADDR(x), HWIO_TSENS_WATCHDOG_LOG_RMSK)
#define HWIO_TSENS_WATCHDOG_LOG_INM(x, m)      \
        in_dword_masked(HWIO_TSENS_WATCHDOG_LOG_ADDR(x), m)
#define HWIO_TSENS_WATCHDOG_LOG_WATCHDOG_BARK_COUNT_BMSK                                   0xff
#define HWIO_TSENS_WATCHDOG_LOG_WATCHDOG_BARK_COUNT_SHFT                                    0x0

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef __HAL_HWIO_TSENS_H__ */


#ifndef __FGADCHALPMIO_H__
#define __FGADCHALPMIO_H__
/*============================================================================
  @file FgAdcHalPmio.h

  PMIO for FG_ADC_RR


                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/fgadc/src/FgAdcHalPmio.h#1 $ */

/*----------------------------------------------------------------------------
 * PERIPHERAL: FG_ADC_RR
 *--------------------------------------------------------------------------*/

#define PMIO_PERPH_TYPE_ADDR(x)                                                ((x) + 0x00000004)
#define PMIO_PERPH_TYPE_RMSK                                                         0xff
#define PMIO_PERPH_TYPE_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_TYPE_ADDR(base), PMIO_PERPH_TYPE_RMSK, 0, val)
#define PMIO_PERPH_TYPE_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_TYPE_ADDR(base), mask, shift, val)
#define PMIO_PERPH_TYPE_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PERPH_TYPE_ADDR(base), val, len)
#define PMIO_PERPH_TYPE_TYPE_BMSK                                                    0xff
#define PMIO_PERPH_TYPE_TYPE_SHFT                                                     0x0

#define PMIO_PERPH_SUBTYPE_ADDR(x)                                             ((x) + 0x00000005)
#define PMIO_PERPH_SUBTYPE_RMSK                                                      0xff
#define PMIO_PERPH_SUBTYPE_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_SUBTYPE_ADDR(base), PMIO_PERPH_SUBTYPE_RMSK, 0, val)
#define PMIO_PERPH_SUBTYPE_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_SUBTYPE_ADDR(base), mask, shift, val)
#define PMIO_PERPH_SUBTYPE_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PERPH_SUBTYPE_ADDR(base), val, len)
#define PMIO_PERPH_SUBTYPE_SUBTYPE_BMSK                                              0xff
#define PMIO_PERPH_SUBTYPE_SUBTYPE_SHFT                                               0x0

#define PMIO_STATUS1_ADDR(x)                                                   ((x) + 0x00000008)
#define PMIO_STATUS1_RMSK                                                            0xfe
#define PMIO_STATUS1_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_STATUS1_ADDR(base), PMIO_STATUS1_RMSK, 0, val)
#define PMIO_STATUS1_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_STATUS1_ADDR(base), mask, shift, val)
#define PMIO_STATUS1_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_STATUS1_ADDR(base), val, len)
#define PMIO_STATUS1_RR_ADC_OK_BMSK                                                  0x80
#define PMIO_STATUS1_RR_ADC_OK_SHFT                                                   0x7
#define PMIO_STATUS1_RR_ADC_OK_NOT_ACTIVE_FVAL                                        0x0
#define PMIO_STATUS1_RR_ADC_OK_ACTIVE_FVAL                                            0x1
#define PMIO_STATUS1_LDO_STS_BMSK                                                    0x40
#define PMIO_STATUS1_LDO_STS_SHFT                                                     0x6
#define PMIO_STATUS1_LDO_STS_LDO_NOT_REQUESTED_FVAL                                   0x0
#define PMIO_STATUS1_LDO_STS_LDO_REQUESTED_FVAL                                       0x1
#define PMIO_STATUS1_BG_BUF_STS_BMSK                                                 0x20
#define PMIO_STATUS1_BG_BUF_STS_SHFT                                                  0x5
#define PMIO_STATUS1_BG_BUF_STS_BG_BUFF_NOT_REQUESTED_FVAL                            0x0
#define PMIO_STATUS1_BG_BUF_STS_BG_BUFF_REQUESTED_FVAL                                0x1
#define PMIO_STATUS1_BG_REQUEST_STS_BMSK                                             0x10
#define PMIO_STATUS1_BG_REQUEST_STS_SHFT                                              0x4
#define PMIO_STATUS1_BG_REQUEST_STS_BG_NOT_REQUESTED_FVAL                             0x0
#define PMIO_STATUS1_BG_REQUEST_STS_BG_REQUESTED_FVAL                                 0x1
#define PMIO_STATUS1_BG_OK_STS_BMSK                                                   0x8
#define PMIO_STATUS1_BG_OK_STS_SHFT                                                   0x3
#define PMIO_STATUS1_BG_OK_STS_BG_NOT_READYD_FVAL                                     0x0
#define PMIO_STATUS1_BG_OK_STS_BG_READY_FVAL                                          0x1
#define PMIO_STATUS1_RR_SHDN_ACK_BMSK                                                 0x4
#define PMIO_STATUS1_RR_SHDN_ACK_SHFT                                                 0x2
#define PMIO_STATUS1_RR_SHDN_ACK_FALSE_FVAL                                           0x0
#define PMIO_STATUS1_RR_SHDN_ACK_TRUE_FVAL                                            0x1
#define PMIO_STATUS1_RR_READY_BMSK                                                    0x2
#define PMIO_STATUS1_RR_READY_SHFT                                                    0x1
#define PMIO_STATUS1_RR_READY_FALSE_FVAL                                              0x0
#define PMIO_STATUS1_RR_READY_TRUE_FVAL                                               0x1

#define PMIO_CHG_PWR_STS_ADDR(x)                                               ((x) + 0x00000009)
#define PMIO_CHG_PWR_STS_RMSK                                                         0xf
#define PMIO_CHG_PWR_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHG_PWR_STS_ADDR(base), PMIO_CHG_PWR_STS_RMSK, 0, val)
#define PMIO_CHG_PWR_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHG_PWR_STS_ADDR(base), mask, shift, val)
#define PMIO_CHG_PWR_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHG_PWR_STS_ADDR(base), val, len)
#define PMIO_CHG_PWR_STS_DC_PWR_SWITCHER_BMSK                                         0x8
#define PMIO_CHG_PWR_STS_DC_PWR_SWITCHER_SHFT                                         0x3
#define PMIO_CHG_PWR_STS_DC_PWR_SWITCHER_FALSE_FVAL                                   0x0
#define PMIO_CHG_PWR_STS_DC_PWR_SWITCHER_TRUE_FVAL                                    0x1
#define PMIO_CHG_PWR_STS_DC_IN_CONNECTED_BMSK                                         0x4
#define PMIO_CHG_PWR_STS_DC_IN_CONNECTED_SHFT                                         0x2
#define PMIO_CHG_PWR_STS_DC_IN_CONNECTED_DISABLE_FVAL                                 0x0
#define PMIO_CHG_PWR_STS_DC_IN_CONNECTED_ENABLE_FVAL                                  0x1
#define PMIO_CHG_PWR_STS_USB_PWR_SWITCHER_BMSK                                        0x2
#define PMIO_CHG_PWR_STS_USB_PWR_SWITCHER_SHFT                                        0x1
#define PMIO_CHG_PWR_STS_USB_PWR_SWITCHER_FALSE_FVAL                                  0x0
#define PMIO_CHG_PWR_STS_USB_PWR_SWITCHER_TRUE_FVAL                                   0x1
#define PMIO_CHG_PWR_STS_USB_IN_CONNECTED_BMSK                                        0x1
#define PMIO_CHG_PWR_STS_USB_IN_CONNECTED_SHFT                                        0x0
#define PMIO_CHG_PWR_STS_USB_IN_CONNECTED_DISABLE_FVAL                                0x0
#define PMIO_CHG_PWR_STS_USB_IN_CONNECTED_ENABLE_FVAL                                 0x1

#define PMIO_REF_STS_ADDR(x)                                                   ((x) + 0x0000000a)
#define PMIO_REF_STS_RMSK                                                             0x7
#define PMIO_REF_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_REF_STS_ADDR(base), PMIO_REF_STS_RMSK, 0, val)
#define PMIO_REF_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_REF_STS_ADDR(base), mask, shift, val)
#define PMIO_REF_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_REF_STS_ADDR(base), val, len)
#define PMIO_REF_STS_REF_SELECTION_BMSK                                               0x6
#define PMIO_REF_STS_REF_SELECTION_SHFT                                               0x1
#define PMIO_REF_STS_REF_SELECTION_REF_IS_RBIAS_FVAL                                  0x0
#define PMIO_REF_STS_REF_SELECTION_REF_IS_5_DIV_3_V_FVAL                              0x1
#define PMIO_REF_STS_REF_SELECTION_REF_IS_5_DIV_2_V_FVAL                              0x2
#define PMIO_REF_STS_REF_SELECTION_REF_IS_5_V_FVAL                                    0x3
#define PMIO_REF_STS_REF_REQUEST_BMSK                                                 0x1
#define PMIO_REF_STS_REF_REQUEST_SHFT                                                 0x0
#define PMIO_REF_STS_REF_REQUEST_REF_NOT_REQUESTED_FVAL                               0x0
#define PMIO_REF_STS_REF_REQUEST_REF_REQUESTED_FVAL                                   0x1

#define PMIO_INT_RT_STS_ADDR(x)                                                ((x) + 0x00000010)
#define PMIO_INT_RT_STS_RMSK                                                          0xf
#define PMIO_INT_RT_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_RT_STS_ADDR(base), PMIO_INT_RT_STS_RMSK, 0, val)
#define PMIO_INT_RT_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_RT_STS_ADDR(base), mask, shift, val)
#define PMIO_INT_RT_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_RT_STS_ADDR(base), val, len)
#define PMIO_INT_RT_STS_RR_SPARE_BMSK                                                 0x8
#define PMIO_INT_RT_STS_RR_SPARE_SHFT                                                 0x3
#define PMIO_INT_RT_STS_RR_SPARE_INT_RT_STATUS_LOW_FVAL                               0x0
#define PMIO_INT_RT_STS_RR_SPARE_INT_RT_STATUS_HIGH_FVAL                              0x1
#define PMIO_INT_RT_STS_RR_BUS_ERR_BMSK                                               0x4
#define PMIO_INT_RT_STS_RR_BUS_ERR_SHFT                                               0x2
#define PMIO_INT_RT_STS_RR_BUS_ERR_INT_RT_STATUS_LOW_FVAL                             0x0
#define PMIO_INT_RT_STS_RR_BUS_ERR_INT_RT_STATUS_HIGH_FVAL                            0x1
#define PMIO_INT_RT_STS_RR_BMSK                                                       0x2
#define PMIO_INT_RT_STS_RR_SHFT                                                       0x1
#define PMIO_INT_RT_STS_RR_INT_RT_STATUS_LOW_FVAL                                     0x0
#define PMIO_INT_RT_STS_RR_INT_RT_STATUS_HIGH_FVAL                                    0x1
#define PMIO_INT_RT_STS_BT_ID_BMSK                                                    0x1
#define PMIO_INT_RT_STS_BT_ID_SHFT                                                    0x0
#define PMIO_INT_RT_STS_BT_ID_INT_RT_STATUS_LOW_FVAL                                  0x0
#define PMIO_INT_RT_STS_BT_ID_INT_RT_STATUS_HIGH_FVAL                                 0x1

#define PMIO_INT_SET_TYPE_ADDR(x)                                              ((x) + 0x00000011)
#define PMIO_INT_SET_TYPE_RMSK                                                        0xf
#define PMIO_INT_SET_TYPE_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_SET_TYPE_ADDR(base), PMIO_INT_SET_TYPE_RMSK, 0, val)
#define PMIO_INT_SET_TYPE_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_SET_TYPE_ADDR(base), mask, shift, val)
#define PMIO_INT_SET_TYPE_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_SET_TYPE_ADDR(base), val, len)
#define PMIO_INT_SET_TYPE_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_SET_TYPE_ADDR(base), val)
#define PMIO_INT_SET_TYPE_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_SET_TYPE_ADDR(base), mask, shift, val)
#define PMIO_INT_SET_TYPE_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_SET_TYPE_ADDR(base), val, len)
#define PMIO_INT_SET_TYPE_RR_SPARE_BMSK                                               0x8
#define PMIO_INT_SET_TYPE_RR_SPARE_SHFT                                               0x3
#define PMIO_INT_SET_TYPE_RR_SPARE_LEVEL_FVAL                                         0x0
#define PMIO_INT_SET_TYPE_RR_SPARE_EDGE_FVAL                                          0x1
#define PMIO_INT_SET_TYPE_RR_BUS_ERR_BMSK                                             0x4
#define PMIO_INT_SET_TYPE_RR_BUS_ERR_SHFT                                             0x2
#define PMIO_INT_SET_TYPE_RR_BUS_ERR_LEVEL_FVAL                                       0x0
#define PMIO_INT_SET_TYPE_RR_BUS_ERR_EDGE_FVAL                                        0x1
#define PMIO_INT_SET_TYPE_RR_BMSK                                                     0x2
#define PMIO_INT_SET_TYPE_RR_SHFT                                                     0x1
#define PMIO_INT_SET_TYPE_RR_LEVEL_FVAL                                               0x0
#define PMIO_INT_SET_TYPE_RR_EDGE_FVAL                                                0x1
#define PMIO_INT_SET_TYPE_BT_ID_BMSK                                                  0x1
#define PMIO_INT_SET_TYPE_BT_ID_SHFT                                                  0x0
#define PMIO_INT_SET_TYPE_BT_ID_LEVEL_FVAL                                            0x0
#define PMIO_INT_SET_TYPE_BT_ID_EDGE_FVAL                                             0x1

#define PMIO_INT_POLARITY_HIGH_ADDR(x)                                         ((x) + 0x00000012)
#define PMIO_INT_POLARITY_HIGH_RMSK                                                   0xf
#define PMIO_INT_POLARITY_HIGH_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_POLARITY_HIGH_ADDR(base), PMIO_INT_POLARITY_HIGH_RMSK, 0, val)
#define PMIO_INT_POLARITY_HIGH_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_POLARITY_HIGH_ADDR(base), mask, shift, val)
#define PMIO_INT_POLARITY_HIGH_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_POLARITY_HIGH_ADDR(base), val, len)
#define PMIO_INT_POLARITY_HIGH_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_POLARITY_HIGH_ADDR(base), val)
#define PMIO_INT_POLARITY_HIGH_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_POLARITY_HIGH_ADDR(base), mask, shift, val)
#define PMIO_INT_POLARITY_HIGH_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_POLARITY_HIGH_ADDR(base), val, len)
#define PMIO_INT_POLARITY_HIGH_RR_SPARE_BMSK                                          0x8
#define PMIO_INT_POLARITY_HIGH_RR_SPARE_SHFT                                          0x3
#define PMIO_INT_POLARITY_HIGH_RR_SPARE_HIGH_TRIGGER_DISABLED_FVAL                    0x0
#define PMIO_INT_POLARITY_HIGH_RR_SPARE_HIGH_TRIGGER_ENABLED_FVAL                     0x1
#define PMIO_INT_POLARITY_HIGH_RR_BUS_ERR_BMSK                                        0x4
#define PMIO_INT_POLARITY_HIGH_RR_BUS_ERR_SHFT                                        0x2
#define PMIO_INT_POLARITY_HIGH_RR_BUS_ERR_HIGH_TRIGGER_DISABLED_FVAL                  0x0
#define PMIO_INT_POLARITY_HIGH_RR_BUS_ERR_HIGH_TRIGGER_ENABLED_FVAL                   0x1
#define PMIO_INT_POLARITY_HIGH_RR_BMSK                                                0x2
#define PMIO_INT_POLARITY_HIGH_RR_SHFT                                                0x1
#define PMIO_INT_POLARITY_HIGH_RR_HIGH_TRIGGER_DISABLED_FVAL                          0x0
#define PMIO_INT_POLARITY_HIGH_RR_HIGH_TRIGGER_ENABLED_FVAL                           0x1
#define PMIO_INT_POLARITY_HIGH_BT_ID_BMSK                                             0x1
#define PMIO_INT_POLARITY_HIGH_BT_ID_SHFT                                             0x0
#define PMIO_INT_POLARITY_HIGH_BT_ID_HIGH_TRIGGER_DISABLED_FVAL                       0x0
#define PMIO_INT_POLARITY_HIGH_BT_ID_HIGH_TRIGGER_ENABLED_FVAL                        0x1

#define PMIO_INT_POLARITY_LOW_ADDR(x)                                          ((x) + 0x00000013)
#define PMIO_INT_POLARITY_LOW_RMSK                                                    0xf
#define PMIO_INT_POLARITY_LOW_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_POLARITY_LOW_ADDR(base), PMIO_INT_POLARITY_LOW_RMSK, 0, val)
#define PMIO_INT_POLARITY_LOW_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_POLARITY_LOW_ADDR(base), mask, shift, val)
#define PMIO_INT_POLARITY_LOW_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_POLARITY_LOW_ADDR(base), val, len)
#define PMIO_INT_POLARITY_LOW_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_POLARITY_LOW_ADDR(base), val)
#define PMIO_INT_POLARITY_LOW_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_POLARITY_LOW_ADDR(base), mask, shift, val)
#define PMIO_INT_POLARITY_LOW_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_POLARITY_LOW_ADDR(base), val, len)
#define PMIO_INT_POLARITY_LOW_RR_SPARE_BMSK                                           0x8
#define PMIO_INT_POLARITY_LOW_RR_SPARE_SHFT                                           0x3
#define PMIO_INT_POLARITY_LOW_RR_SPARE_LOW_TRIGGER_DISABLED_FVAL                      0x0
#define PMIO_INT_POLARITY_LOW_RR_SPARE_LOW_TRIGGER_ENABLED_FVAL                       0x1
#define PMIO_INT_POLARITY_LOW_RR_BUS_ERR_BMSK                                         0x4
#define PMIO_INT_POLARITY_LOW_RR_BUS_ERR_SHFT                                         0x2
#define PMIO_INT_POLARITY_LOW_RR_BUS_ERR_LOW_TRIGGER_DISABLED_FVAL                    0x0
#define PMIO_INT_POLARITY_LOW_RR_BUS_ERR_LOW_TRIGGER_ENABLED_FVAL                     0x1
#define PMIO_INT_POLARITY_LOW_RR_BMSK                                                 0x2
#define PMIO_INT_POLARITY_LOW_RR_SHFT                                                 0x1
#define PMIO_INT_POLARITY_LOW_RR_LOW_TRIGGER_DISABLED_FVAL                            0x0
#define PMIO_INT_POLARITY_LOW_RR_LOW_TRIGGER_ENABLED_FVAL                             0x1
#define PMIO_INT_POLARITY_LOW_BT_ID_BMSK                                              0x1
#define PMIO_INT_POLARITY_LOW_BT_ID_SHFT                                              0x0
#define PMIO_INT_POLARITY_LOW_BT_ID_LOW_TRIGGER_DISABLED_FVAL                         0x0
#define PMIO_INT_POLARITY_LOW_BT_ID_LOW_TRIGGER_ENABLED_FVAL                          0x1

#define PMIO_INT_LATCHED_CLR_ADDR(x)                                           ((x) + 0x00000014)
#define PMIO_INT_LATCHED_CLR_RMSK                                                     0xf
#define PMIO_INT_LATCHED_CLR_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_LATCHED_CLR_ADDR(base), val)
#define PMIO_INT_LATCHED_CLR_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_LATCHED_CLR_ADDR(base), val, len)
#define PMIO_INT_LATCHED_CLR_RR_SPARE_BMSK                                            0x8
#define PMIO_INT_LATCHED_CLR_RR_SPARE_SHFT                                            0x3
#define PMIO_INT_LATCHED_CLR_RR_BUS_ERR_BMSK                                          0x4
#define PMIO_INT_LATCHED_CLR_RR_BUS_ERR_SHFT                                          0x2
#define PMIO_INT_LATCHED_CLR_RR_BMSK                                                  0x2
#define PMIO_INT_LATCHED_CLR_RR_SHFT                                                  0x1
#define PMIO_INT_LATCHED_CLR_BT_ID_BMSK                                               0x1
#define PMIO_INT_LATCHED_CLR_BT_ID_SHFT                                               0x0

#define PMIO_INT_EN_SET_ADDR(x)                                                ((x) + 0x00000015)
#define PMIO_INT_EN_SET_RMSK                                                          0xf
#define PMIO_INT_EN_SET_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_EN_SET_ADDR(base), PMIO_INT_EN_SET_RMSK, 0, val)
#define PMIO_INT_EN_SET_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_EN_SET_ADDR(base), mask, shift, val)
#define PMIO_INT_EN_SET_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_EN_SET_ADDR(base), val, len)
#define PMIO_INT_EN_SET_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_EN_SET_ADDR(base), val)
#define PMIO_INT_EN_SET_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_EN_SET_ADDR(base), mask, shift, val)
#define PMIO_INT_EN_SET_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_EN_SET_ADDR(base), val, len)
#define PMIO_INT_EN_SET_RR_SPARE_BMSK                                                 0x8
#define PMIO_INT_EN_SET_RR_SPARE_SHFT                                                 0x3
#define PMIO_INT_EN_SET_RR_SPARE_INT_DISABLED_FVAL                                    0x0
#define PMIO_INT_EN_SET_RR_SPARE_INT_ENABLED_FVAL                                     0x1
#define PMIO_INT_EN_SET_RR_BUS_ERR_BMSK                                               0x4
#define PMIO_INT_EN_SET_RR_BUS_ERR_SHFT                                               0x2
#define PMIO_INT_EN_SET_RR_BUS_ERR_INT_DISABLED_FVAL                                  0x0
#define PMIO_INT_EN_SET_RR_BUS_ERR_INT_ENABLED_FVAL                                   0x1
#define PMIO_INT_EN_SET_RR_BMSK                                                       0x2
#define PMIO_INT_EN_SET_RR_SHFT                                                       0x1
#define PMIO_INT_EN_SET_RR_INT_DISABLED_FVAL                                          0x0
#define PMIO_INT_EN_SET_RR_INT_ENABLED_FVAL                                           0x1
#define PMIO_INT_EN_SET_BT_ID_BMSK                                                    0x1
#define PMIO_INT_EN_SET_BT_ID_SHFT                                                    0x0
#define PMIO_INT_EN_SET_BT_ID_INT_DISABLED_FVAL                                       0x0
#define PMIO_INT_EN_SET_BT_ID_INT_ENABLED_FVAL                                        0x1

#define PMIO_INT_EN_CLR_ADDR(x)                                                ((x) + 0x00000016)
#define PMIO_INT_EN_CLR_RMSK                                                          0xf
#define PMIO_INT_EN_CLR_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_EN_CLR_ADDR(base), PMIO_INT_EN_CLR_RMSK, 0, val)
#define PMIO_INT_EN_CLR_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_EN_CLR_ADDR(base), mask, shift, val)
#define PMIO_INT_EN_CLR_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_EN_CLR_ADDR(base), val, len)
#define PMIO_INT_EN_CLR_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_EN_CLR_ADDR(base), val)
#define PMIO_INT_EN_CLR_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_EN_CLR_ADDR(base), mask, shift, val)
#define PMIO_INT_EN_CLR_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_EN_CLR_ADDR(base), val, len)
#define PMIO_INT_EN_CLR_RR_SPARE_BMSK                                                 0x8
#define PMIO_INT_EN_CLR_RR_SPARE_SHFT                                                 0x3
#define PMIO_INT_EN_CLR_RR_SPARE_INT_DISABLED_FVAL                                    0x0
#define PMIO_INT_EN_CLR_RR_SPARE_INT_ENABLED_FVAL                                     0x1
#define PMIO_INT_EN_CLR_RR_BUS_ERR_BMSK                                               0x4
#define PMIO_INT_EN_CLR_RR_BUS_ERR_SHFT                                               0x2
#define PMIO_INT_EN_CLR_RR_BUS_ERR_INT_DISABLED_FVAL                                  0x0
#define PMIO_INT_EN_CLR_RR_BUS_ERR_INT_ENABLED_FVAL                                   0x1
#define PMIO_INT_EN_CLR_RR_BMSK                                                       0x2
#define PMIO_INT_EN_CLR_RR_SHFT                                                       0x1
#define PMIO_INT_EN_CLR_RR_INT_DISABLED_FVAL                                          0x0
#define PMIO_INT_EN_CLR_RR_INT_ENABLED_FVAL                                           0x1
#define PMIO_INT_EN_CLR_BT_ID_BMSK                                                    0x1
#define PMIO_INT_EN_CLR_BT_ID_SHFT                                                    0x0
#define PMIO_INT_EN_CLR_BT_ID_INT_DISABLED_FVAL                                       0x0
#define PMIO_INT_EN_CLR_BT_ID_INT_ENABLED_FVAL                                        0x1

#define PMIO_INT_LATCHED_STS_ADDR(x)                                           ((x) + 0x00000018)
#define PMIO_INT_LATCHED_STS_RMSK                                                     0xf
#define PMIO_INT_LATCHED_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_LATCHED_STS_ADDR(base), PMIO_INT_LATCHED_STS_RMSK, 0, val)
#define PMIO_INT_LATCHED_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_LATCHED_STS_ADDR(base), mask, shift, val)
#define PMIO_INT_LATCHED_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_LATCHED_STS_ADDR(base), val, len)
#define PMIO_INT_LATCHED_STS_RR_SPARE_BMSK                                            0x8
#define PMIO_INT_LATCHED_STS_RR_SPARE_SHFT                                            0x3
#define PMIO_INT_LATCHED_STS_RR_SPARE_NO_INT_LATCHED_FVAL                             0x0
#define PMIO_INT_LATCHED_STS_RR_SPARE_INTERRUPT_LATCHED_FVAL                          0x1
#define PMIO_INT_LATCHED_STS_RR_BUS_ERR_BMSK                                          0x4
#define PMIO_INT_LATCHED_STS_RR_BUS_ERR_SHFT                                          0x2
#define PMIO_INT_LATCHED_STS_RR_BUS_ERR_NO_INT_LATCHED_FVAL                           0x0
#define PMIO_INT_LATCHED_STS_RR_BUS_ERR_INTERRUPT_LATCHED_FVAL                        0x1
#define PMIO_INT_LATCHED_STS_RR_BMSK                                                  0x2
#define PMIO_INT_LATCHED_STS_RR_SHFT                                                  0x1
#define PMIO_INT_LATCHED_STS_RR_NO_INT_LATCHED_FVAL                                   0x0
#define PMIO_INT_LATCHED_STS_RR_INTERRUPT_LATCHED_FVAL                                0x1
#define PMIO_INT_LATCHED_STS_BT_ID_BMSK                                               0x1
#define PMIO_INT_LATCHED_STS_BT_ID_SHFT                                               0x0
#define PMIO_INT_LATCHED_STS_BT_ID_NO_INT_LATCHED_FVAL                                0x0
#define PMIO_INT_LATCHED_STS_BT_ID_INTERRUPT_LATCHED_FVAL                             0x1

#define PMIO_INT_PENDING_STS_ADDR(x)                                           ((x) + 0x00000019)
#define PMIO_INT_PENDING_STS_RMSK                                                     0xf
#define PMIO_INT_PENDING_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_PENDING_STS_ADDR(base), PMIO_INT_PENDING_STS_RMSK, 0, val)
#define PMIO_INT_PENDING_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_PENDING_STS_ADDR(base), mask, shift, val)
#define PMIO_INT_PENDING_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_PENDING_STS_ADDR(base), val, len)
#define PMIO_INT_PENDING_STS_RR_SPARE_BMSK                                            0x8
#define PMIO_INT_PENDING_STS_RR_SPARE_SHFT                                            0x3
#define PMIO_INT_PENDING_STS_RR_SPARE_NO_INT_PENDING_FVAL                             0x0
#define PMIO_INT_PENDING_STS_RR_SPARE_INTERRUPT_PENDING_FVAL                          0x1
#define PMIO_INT_PENDING_STS_RR_BUS_ERR_BMSK                                          0x4
#define PMIO_INT_PENDING_STS_RR_BUS_ERR_SHFT                                          0x2
#define PMIO_INT_PENDING_STS_RR_BUS_ERR_NO_INT_PENDING_FVAL                           0x0
#define PMIO_INT_PENDING_STS_RR_BUS_ERR_INTERRUPT_PENDING_FVAL                        0x1
#define PMIO_INT_PENDING_STS_RR_BMSK                                                  0x2
#define PMIO_INT_PENDING_STS_RR_SHFT                                                  0x1
#define PMIO_INT_PENDING_STS_RR_NO_INT_PENDING_FVAL                                   0x0
#define PMIO_INT_PENDING_STS_RR_INTERRUPT_PENDING_FVAL                                0x1
#define PMIO_INT_PENDING_STS_BT_ID_BMSK                                               0x1
#define PMIO_INT_PENDING_STS_BT_ID_SHFT                                               0x0
#define PMIO_INT_PENDING_STS_BT_ID_NO_INT_PENDING_FVAL                                0x0
#define PMIO_INT_PENDING_STS_BT_ID_INTERRUPT_PENDING_FVAL                             0x1

#define PMIO_INT_MID_SEL_ADDR(x)                                               ((x) + 0x0000001a)
#define PMIO_INT_MID_SEL_RMSK                                                         0x3
#define PMIO_INT_MID_SEL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_MID_SEL_ADDR(base), PMIO_INT_MID_SEL_RMSK, 0, val)
#define PMIO_INT_MID_SEL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_MID_SEL_ADDR(base), mask, shift, val)
#define PMIO_INT_MID_SEL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_MID_SEL_ADDR(base), val, len)
#define PMIO_INT_MID_SEL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_MID_SEL_ADDR(base), val)
#define PMIO_INT_MID_SEL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_MID_SEL_ADDR(base), mask, shift, val)
#define PMIO_INT_MID_SEL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_MID_SEL_ADDR(base), val, len)
#define PMIO_INT_MID_SEL_INT_MID_SEL_BMSK                                             0x3
#define PMIO_INT_MID_SEL_INT_MID_SEL_SHFT                                             0x0
#define PMIO_INT_MID_SEL_INT_MID_SEL_MID0_FVAL                                        0x0
#define PMIO_INT_MID_SEL_INT_MID_SEL_MID1_FVAL                                        0x1
#define PMIO_INT_MID_SEL_INT_MID_SEL_MID2_FVAL                                        0x2
#define PMIO_INT_MID_SEL_INT_MID_SEL_MID3_FVAL                                        0x3

#define PMIO_INT_PRIORITY_ADDR(x)                                              ((x) + 0x0000001b)
#define PMIO_INT_PRIORITY_RMSK                                                        0x1
#define PMIO_INT_PRIORITY_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_PRIORITY_ADDR(base), PMIO_INT_PRIORITY_RMSK, 0, val)
#define PMIO_INT_PRIORITY_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_PRIORITY_ADDR(base), mask, shift, val)
#define PMIO_INT_PRIORITY_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_PRIORITY_ADDR(base), val, len)
#define PMIO_INT_PRIORITY_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_PRIORITY_ADDR(base), val)
#define PMIO_INT_PRIORITY_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_PRIORITY_ADDR(base), mask, shift, val)
#define PMIO_INT_PRIORITY_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_PRIORITY_ADDR(base), val, len)
#define PMIO_INT_PRIORITY_INT_PRIORITY_BMSK                                           0x1
#define PMIO_INT_PRIORITY_INT_PRIORITY_SHFT                                           0x0
#define PMIO_INT_PRIORITY_INT_PRIORITY_SR_FVAL                                        0x0
#define PMIO_INT_PRIORITY_INT_PRIORITY_A_FVAL                                         0x1

#define PMIO_EN_CTL_ADDR(x)                                                    ((x) + 0x00000046)
#define PMIO_EN_CTL_RMSK                                                             0x80
#define PMIO_EN_CTL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_EN_CTL_ADDR(base), PMIO_EN_CTL_RMSK, 0, val)
#define PMIO_EN_CTL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_EN_CTL_ADDR(base), mask, shift, val)
#define PMIO_EN_CTL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_EN_CTL_ADDR(base), val, len)
#define PMIO_EN_CTL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_EN_CTL_ADDR(base), val)
#define PMIO_EN_CTL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_EN_CTL_ADDR(base), mask, shift, val)
#define PMIO_EN_CTL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_EN_CTL_ADDR(base), val, len)
#define PMIO_EN_CTL_RR_ADC_EN_BMSK                                                   0x80
#define PMIO_EN_CTL_RR_ADC_EN_SHFT                                                    0x7
#define PMIO_EN_CTL_RR_ADC_EN_DISABLE_FVAL                                            0x0
#define PMIO_EN_CTL_RR_ADC_EN_ENABLE_FVAL                                             0x1

#define PMIO_SKIN_TEMPERATURE_LSB_ADDR(x)                                      ((x) + 0x00000050)
#define PMIO_SKIN_TEMPERATURE_LSB_RMSK                                               0xff
#define PMIO_SKIN_TEMPERATURE_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_SKIN_TEMPERATURE_LSB_ADDR(base), PMIO_SKIN_TEMPERATURE_LSB_RMSK, 0, val)
#define PMIO_SKIN_TEMPERATURE_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_SKIN_TEMPERATURE_LSB_ADDR(base), mask, shift, val)
#define PMIO_SKIN_TEMPERATURE_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_SKIN_TEMPERATURE_LSB_ADDR(base), val, len)
#define PMIO_SKIN_TEMPERATURE_LSB_VALUE_BMSK                                         0xff
#define PMIO_SKIN_TEMPERATURE_LSB_VALUE_SHFT                                          0x0

#define PMIO_SKIN_TEMPERATURE_MSB_ADDR(x)                                      ((x) + 0x00000051)
#define PMIO_SKIN_TEMPERATURE_MSB_RMSK                                                0x7
#define PMIO_SKIN_TEMPERATURE_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_SKIN_TEMPERATURE_MSB_ADDR(base), PMIO_SKIN_TEMPERATURE_MSB_RMSK, 0, val)
#define PMIO_SKIN_TEMPERATURE_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_SKIN_TEMPERATURE_MSB_ADDR(base), mask, shift, val)
#define PMIO_SKIN_TEMPERATURE_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_SKIN_TEMPERATURE_MSB_ADDR(base), val, len)
#define PMIO_SKIN_TEMPERATURE_MSB_VALUE_BMSK                                          0x7
#define PMIO_SKIN_TEMPERATURE_MSB_VALUE_SHFT                                          0x0

#define PMIO_RR_ADC_CTL_ADDR(x)                                                ((x) + 0x00000052)
#define PMIO_RR_ADC_CTL_RMSK                                                          0xf
#define PMIO_RR_ADC_CTL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_RR_ADC_CTL_ADDR(base), PMIO_RR_ADC_CTL_RMSK, 0, val)
#define PMIO_RR_ADC_CTL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_RR_ADC_CTL_ADDR(base), mask, shift, val)
#define PMIO_RR_ADC_CTL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_RR_ADC_CTL_ADDR(base), val, len)
#define PMIO_RR_ADC_CTL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_RR_ADC_CTL_ADDR(base), val)
#define PMIO_RR_ADC_CTL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_RR_ADC_CTL_ADDR(base), mask, shift, val)
#define PMIO_RR_ADC_CTL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_RR_ADC_CTL_ADDR(base), val, len)
#define PMIO_RR_ADC_CTL_CONTINOUS_SEQUENCE_INITIATE_BMSK                              0x8
#define PMIO_RR_ADC_CTL_CONTINOUS_SEQUENCE_INITIATE_SHFT                              0x3
#define PMIO_RR_ADC_CTL_RR_SHDN_BMSK                                                  0x4
#define PMIO_RR_ADC_CTL_RR_SHDN_SHFT                                                  0x2
#define PMIO_RR_ADC_CTL_RR_SHDN_SHDN_NOT_REQUESTED_FVAL                               0x0
#define PMIO_RR_ADC_CTL_RR_SHDN_SHDN_REQUESTED_FVAL                                   0x1
#define PMIO_RR_ADC_CTL_SW_SEQUENCE_INITIATE_BMSK                                     0x2
#define PMIO_RR_ADC_CTL_SW_SEQUENCE_INITIATE_SHFT                                     0x1
#define PMIO_RR_ADC_CTL_HW_SEQUENCE_INITIATE_EN_BMSK                                  0x1
#define PMIO_RR_ADC_CTL_HW_SEQUENCE_INITIATE_EN_SHFT                                  0x0
#define PMIO_RR_ADC_CTL_HW_SEQUENCE_INITIATE_EN_DISABLE_FVAL                          0x0
#define PMIO_RR_ADC_CTL_HW_SEQUENCE_INITIATE_EN_ENABLE_FVAL                           0x1

#define PMIO_RR_ADC_LOG_ADDR(x)                                                ((x) + 0x00000053)
#define PMIO_RR_ADC_LOG_RMSK                                                          0x1
#define PMIO_RR_ADC_LOG_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_RR_ADC_LOG_ADDR(base), PMIO_RR_ADC_LOG_RMSK, 0, val)
#define PMIO_RR_ADC_LOG_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_RR_ADC_LOG_ADDR(base), mask, shift, val)
#define PMIO_RR_ADC_LOG_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_RR_ADC_LOG_ADDR(base), val, len)
#define PMIO_RR_ADC_LOG_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_RR_ADC_LOG_ADDR(base), val)
#define PMIO_RR_ADC_LOG_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_RR_ADC_LOG_ADDR(base), mask, shift, val)
#define PMIO_RR_ADC_LOG_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_RR_ADC_LOG_ADDR(base), val, len)
#define PMIO_RR_ADC_LOG_CLR_CTRL_BMSK                                                 0x1
#define PMIO_RR_ADC_LOG_CLR_CTRL_SHFT                                                 0x0
#define PMIO_RR_ADC_LOG_CLR_CTRL_DO_NOT_CLEAR_FVAL                                    0x0
#define PMIO_RR_ADC_LOG_CLR_CTRL_CLEAR_FVAL                                           0x1

#define PMIO_REF_CFG_ADDR(x)                                                   ((x) + 0x00000054)
#define PMIO_REF_CFG_RMSK                                                             0xf
#define PMIO_REF_CFG_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_REF_CFG_ADDR(base), PMIO_REF_CFG_RMSK, 0, val)
#define PMIO_REF_CFG_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_REF_CFG_ADDR(base), mask, shift, val)
#define PMIO_REF_CFG_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_REF_CFG_ADDR(base), val, len)
#define PMIO_REF_CFG_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_REF_CFG_ADDR(base), val)
#define PMIO_REF_CFG_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_REF_CFG_ADDR(base), mask, shift, val)
#define PMIO_REF_CFG_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_REF_CFG_ADDR(base), val, len)
#define PMIO_REF_CFG_VREG_FG_BYPASS_BMSK                                              0x8
#define PMIO_REF_CFG_VREG_FG_BYPASS_SHFT                                              0x3
#define PMIO_REF_CFG_VREG_FG_BYPASS_NO_CAP_FVAL                                       0x0
#define PMIO_REF_CFG_VREG_FG_BYPASS_CAP_CONNECTED_FVAL                                0x1
#define PMIO_REF_CFG_VREG_FG_LEVEL_BMSK                                               0x7
#define PMIO_REF_CFG_VREG_FG_LEVEL_SHFT                                               0x0
#define PMIO_REF_CFG_VREG_FG_LEVEL_VREG_FG_2P70_V_FVAL                                0x0
#define PMIO_REF_CFG_VREG_FG_LEVEL_VREG_FG_2P57_V_FVAL                                0x1
#define PMIO_REF_CFG_VREG_FG_LEVEL_VREG_FG_2P45_V_FVAL                                0x2
#define PMIO_REF_CFG_VREG_FG_LEVEL_VREG_FG_2P35_V_FVAL                                0x3
#define PMIO_REF_CFG_VREG_FG_LEVEL_VREG_FG_2P84_V_FVAL                                0x4
#define PMIO_REF_CFG_VREG_FG_LEVEL_VREG_FG_3P00_V_FVAL                                0x5
#define PMIO_REF_CFG_VREG_FG_LEVEL_VREG_FG_3P18_V_FVAL                                0x6
#define PMIO_REF_CFG_VREG_FG_LEVEL_VREG_FG_3P38_V_FVAL                                0x7

#define PMIO_RBIAS_CNLS_ADDR(x)                                                ((x) + 0x00000055)
#define PMIO_RBIAS_CNLS_RMSK                                                          0x1
#define PMIO_RBIAS_CNLS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_RBIAS_CNLS_ADDR(base), PMIO_RBIAS_CNLS_RMSK, 0, val)
#define PMIO_RBIAS_CNLS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_RBIAS_CNLS_ADDR(base), mask, shift, val)
#define PMIO_RBIAS_CNLS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_RBIAS_CNLS_ADDR(base), val, len)
#define PMIO_RBIAS_CNLS_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_RBIAS_CNLS_ADDR(base), val)
#define PMIO_RBIAS_CNLS_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_RBIAS_CNLS_ADDR(base), mask, shift, val)
#define PMIO_RBIAS_CNLS_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_RBIAS_CNLS_ADDR(base), val, len)
#define PMIO_RBIAS_CNLS_BUFFER_SEL_BMSK                                               0x1
#define PMIO_RBIAS_CNLS_BUFFER_SEL_SHFT                                               0x0
#define PMIO_RBIAS_CNLS_BUFFER_SEL_RBIAS_REF_UNBUF_FVAL                               0x0
#define PMIO_RBIAS_CNLS_BUFFER_SEL_RBIAS_REF_BUF_FVAL                                 0x1

#define PMIO_FAKE_BATT_LOW_LSB_ADDR(x)                                         ((x) + 0x00000058)
#define PMIO_FAKE_BATT_LOW_LSB_RMSK                                                  0xff
#define PMIO_FAKE_BATT_LOW_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_LOW_LSB_ADDR(base), PMIO_FAKE_BATT_LOW_LSB_RMSK, 0, val)
#define PMIO_FAKE_BATT_LOW_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_LOW_LSB_ADDR(base), mask, shift, val)
#define PMIO_FAKE_BATT_LOW_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_FAKE_BATT_LOW_LSB_ADDR(base), val, len)
#define PMIO_FAKE_BATT_LOW_LSB_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_FAKE_BATT_LOW_LSB_ADDR(base), val)
#define PMIO_FAKE_BATT_LOW_LSB_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_LOW_LSB_ADDR(base), mask, shift, val)
#define PMIO_FAKE_BATT_LOW_LSB_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_FAKE_BATT_LOW_LSB_ADDR(base), val, len)
#define PMIO_FAKE_BATT_LOW_LSB_VALUE_BMSK                                            0xff
#define PMIO_FAKE_BATT_LOW_LSB_VALUE_SHFT                                             0x0

#define PMIO_FAKE_BATT_LOW_MSB_ADDR(x)                                         ((x) + 0x00000059)
#define PMIO_FAKE_BATT_LOW_MSB_RMSK                                                   0x3
#define PMIO_FAKE_BATT_LOW_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_LOW_MSB_ADDR(base), PMIO_FAKE_BATT_LOW_MSB_RMSK, 0, val)
#define PMIO_FAKE_BATT_LOW_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_LOW_MSB_ADDR(base), mask, shift, val)
#define PMIO_FAKE_BATT_LOW_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_FAKE_BATT_LOW_MSB_ADDR(base), val, len)
#define PMIO_FAKE_BATT_LOW_MSB_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_FAKE_BATT_LOW_MSB_ADDR(base), val)
#define PMIO_FAKE_BATT_LOW_MSB_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_LOW_MSB_ADDR(base), mask, shift, val)
#define PMIO_FAKE_BATT_LOW_MSB_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_FAKE_BATT_LOW_MSB_ADDR(base), val, len)
#define PMIO_FAKE_BATT_LOW_MSB_VALUE_BMSK                                             0x3
#define PMIO_FAKE_BATT_LOW_MSB_VALUE_SHFT                                             0x0

#define PMIO_FAKE_BATT_HIGH_LSB_ADDR(x)                                        ((x) + 0x0000005a)
#define PMIO_FAKE_BATT_HIGH_LSB_RMSK                                                 0xff
#define PMIO_FAKE_BATT_HIGH_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_HIGH_LSB_ADDR(base), PMIO_FAKE_BATT_HIGH_LSB_RMSK, 0, val)
#define PMIO_FAKE_BATT_HIGH_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_HIGH_LSB_ADDR(base), mask, shift, val)
#define PMIO_FAKE_BATT_HIGH_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_FAKE_BATT_HIGH_LSB_ADDR(base), val, len)
#define PMIO_FAKE_BATT_HIGH_LSB_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_FAKE_BATT_HIGH_LSB_ADDR(base), val)
#define PMIO_FAKE_BATT_HIGH_LSB_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_HIGH_LSB_ADDR(base), mask, shift, val)
#define PMIO_FAKE_BATT_HIGH_LSB_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_FAKE_BATT_HIGH_LSB_ADDR(base), val, len)
#define PMIO_FAKE_BATT_HIGH_LSB_VALUE_BMSK                                           0xff
#define PMIO_FAKE_BATT_HIGH_LSB_VALUE_SHFT                                            0x0

#define PMIO_FAKE_BATT_HIGH_MSB_ADDR(x)                                        ((x) + 0x0000005b)
#define PMIO_FAKE_BATT_HIGH_MSB_RMSK                                                  0x3
#define PMIO_FAKE_BATT_HIGH_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_HIGH_MSB_ADDR(base), PMIO_FAKE_BATT_HIGH_MSB_RMSK, 0, val)
#define PMIO_FAKE_BATT_HIGH_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_HIGH_MSB_ADDR(base), mask, shift, val)
#define PMIO_FAKE_BATT_HIGH_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_FAKE_BATT_HIGH_MSB_ADDR(base), val, len)
#define PMIO_FAKE_BATT_HIGH_MSB_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_FAKE_BATT_HIGH_MSB_ADDR(base), val)
#define PMIO_FAKE_BATT_HIGH_MSB_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_FAKE_BATT_HIGH_MSB_ADDR(base), mask, shift, val)
#define PMIO_FAKE_BATT_HIGH_MSB_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_FAKE_BATT_HIGH_MSB_ADDR(base), val, len)
#define PMIO_FAKE_BATT_HIGH_MSB_VALUE_BMSK                                            0x3
#define PMIO_FAKE_BATT_HIGH_MSB_VALUE_SHFT                                            0x0

#define PMIO_BATT_ID_CTRL_ADDR(x)                                              ((x) + 0x00000060)
#define PMIO_BATT_ID_CTRL_RMSK                                                        0x3
#define PMIO_BATT_ID_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_CTRL_ADDR(base), PMIO_BATT_ID_CTRL_RMSK, 0, val)
#define PMIO_BATT_ID_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_CTRL_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_CTRL_ADDR(base), val, len)
#define PMIO_BATT_ID_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_BATT_ID_CTRL_ADDR(base), val)
#define PMIO_BATT_ID_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_BATT_ID_CTRL_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_BATT_ID_CTRL_ADDR(base), val, len)
#define PMIO_BATT_ID_CTRL_CHANNEL_IRQ_BMSK                                            0x2
#define PMIO_BATT_ID_CTRL_CHANNEL_IRQ_SHFT                                            0x1
#define PMIO_BATT_ID_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                    0x0
#define PMIO_BATT_ID_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                     0x1
#define PMIO_BATT_ID_CTRL_CHANNEL_CONV_BMSK                                           0x1
#define PMIO_BATT_ID_CTRL_CHANNEL_CONV_SHFT                                           0x0
#define PMIO_BATT_ID_CTRL_CHANNEL_CONV_DISABLE_FVAL                                   0x0
#define PMIO_BATT_ID_CTRL_CHANNEL_CONV_ENABLE_FVAL                                    0x1

#define PMIO_BATT_ID_TRIGGER_ADDR(x)                                           ((x) + 0x00000061)
#define PMIO_BATT_ID_TRIGGER_RMSK                                                     0x1
#define PMIO_BATT_ID_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_TRIGGER_ADDR(base), PMIO_BATT_ID_TRIGGER_RMSK, 0, val)
#define PMIO_BATT_ID_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_TRIGGER_ADDR(base), val, len)
#define PMIO_BATT_ID_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_BATT_ID_TRIGGER_ADDR(base), val)
#define PMIO_BATT_ID_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_BATT_ID_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_BATT_ID_TRIGGER_ADDR(base), val, len)
#define PMIO_BATT_ID_TRIGGER_TRIGGER_CTRL_BMSK                                        0x1
#define PMIO_BATT_ID_TRIGGER_TRIGGER_CTRL_SHFT                                        0x0
#define PMIO_BATT_ID_TRIGGER_TRIGGER_CTRL_DISABLE_FVAL                                0x0
#define PMIO_BATT_ID_TRIGGER_TRIGGER_CTRL_ENABLE_FVAL                                 0x1

#define PMIO_BATT_ID_STS_ADDR(x)                                               ((x) + 0x00000062)
#define PMIO_BATT_ID_STS_RMSK                                                        0x13
#define PMIO_BATT_ID_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_STS_ADDR(base), PMIO_BATT_ID_STS_RMSK, 0, val)
#define PMIO_BATT_ID_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_STS_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_STS_ADDR(base), val, len)
#define PMIO_BATT_ID_STS_FAKE_BATTERY_BMSK                                           0x10
#define PMIO_BATT_ID_STS_FAKE_BATTERY_SHFT                                            0x4
#define PMIO_BATT_ID_STS_CHANNEL_STS_BMSK                                             0x2
#define PMIO_BATT_ID_STS_CHANNEL_STS_SHFT                                             0x1
#define PMIO_BATT_ID_STS_CHANNEL_STS_NOT_READY_FVAL                                   0x0
#define PMIO_BATT_ID_STS_CHANNEL_STS_READY_FVAL                                       0x1
#define PMIO_BATT_ID_STS_READING_STS_BMSK                                             0x1
#define PMIO_BATT_ID_STS_READING_STS_SHFT                                             0x0
#define PMIO_BATT_ID_STS_READING_STS_NOT_AVAILABLE_FVAL                               0x0
#define PMIO_BATT_ID_STS_READING_STS_AVAILABLE_FVAL                                   0x1

#define PMIO_BATT_ID_CFG_ADDR(x)                                               ((x) + 0x00000063)
#define PMIO_BATT_ID_CFG_RMSK                                                        0xe0
#define PMIO_BATT_ID_CFG_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_CFG_ADDR(base), PMIO_BATT_ID_CFG_RMSK, 0, val)
#define PMIO_BATT_ID_CFG_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_CFG_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_CFG_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_CFG_ADDR(base), val, len)
#define PMIO_BATT_ID_CFG_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_BATT_ID_CFG_ADDR(base), val)
#define PMIO_BATT_ID_CFG_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_BATT_ID_CFG_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_CFG_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_BATT_ID_CFG_ADDR(base), val, len)
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BMSK                                              0xe0
#define PMIO_BATT_ID_CFG_BIAS_WAIT_SHFT                                               0x5
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BIASWAIT_0_MS_FVAL                                 0x0
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BIASWAIT_1_MS_FVAL                                 0x1
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BIASWAIT_4_MS_FVAL                                 0x2
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BIASWAIT_12_MS_FVAL                                0x3
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BIASWAIT_20_MS_FVAL                                0x4
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BIASWAIT_40_MS_FVAL                                0x5
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BIASWAIT_60_MS_FVAL                                0x6
#define PMIO_BATT_ID_CFG_BIAS_WAIT_BIASWAIT_80_MS_FVAL                                0x7

#define PMIO_BATT_ID_5_LSB_ADDR(x)                                             ((x) + 0x00000066)
#define PMIO_BATT_ID_5_LSB_RMSK                                                      0xff
#define PMIO_BATT_ID_5_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_5_LSB_ADDR(base), PMIO_BATT_ID_5_LSB_RMSK, 0, val)
#define PMIO_BATT_ID_5_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_5_LSB_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_5_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_5_LSB_ADDR(base), val, len)
#define PMIO_BATT_ID_5_LSB_ADC_READ_BMSK                                             0xff
#define PMIO_BATT_ID_5_LSB_ADC_READ_SHFT                                              0x0

#define PMIO_BATT_ID_5_MSB_ADDR(x)                                             ((x) + 0x00000067)
#define PMIO_BATT_ID_5_MSB_RMSK                                                       0x7
#define PMIO_BATT_ID_5_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_5_MSB_ADDR(base), PMIO_BATT_ID_5_MSB_RMSK, 0, val)
#define PMIO_BATT_ID_5_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_5_MSB_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_5_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_5_MSB_ADDR(base), val, len)
#define PMIO_BATT_ID_5_MSB_ADC_READ_BMSK                                              0x7
#define PMIO_BATT_ID_5_MSB_ADC_READ_SHFT                                              0x0

#define PMIO_BATT_ID_15_LSB_ADDR(x)                                            ((x) + 0x00000068)
#define PMIO_BATT_ID_15_LSB_RMSK                                                     0xff
#define PMIO_BATT_ID_15_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_15_LSB_ADDR(base), PMIO_BATT_ID_15_LSB_RMSK, 0, val)
#define PMIO_BATT_ID_15_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_15_LSB_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_15_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_15_LSB_ADDR(base), val, len)
#define PMIO_BATT_ID_15_LSB_ADC_READ_BMSK                                            0xff
#define PMIO_BATT_ID_15_LSB_ADC_READ_SHFT                                             0x0

#define PMIO_BATT_ID_15_MSB_ADDR(x)                                            ((x) + 0x00000069)
#define PMIO_BATT_ID_15_MSB_RMSK                                                      0x7
#define PMIO_BATT_ID_15_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_15_MSB_ADDR(base), PMIO_BATT_ID_15_MSB_RMSK, 0, val)
#define PMIO_BATT_ID_15_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_15_MSB_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_15_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_15_MSB_ADDR(base), val, len)
#define PMIO_BATT_ID_15_MSB_ADC_READ_BMSK                                             0x7
#define PMIO_BATT_ID_15_MSB_ADC_READ_SHFT                                             0x0

#define PMIO_BATT_ID_150_LSB_ADDR(x)                                           ((x) + 0x0000006a)
#define PMIO_BATT_ID_150_LSB_RMSK                                                    0xff
#define PMIO_BATT_ID_150_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_150_LSB_ADDR(base), PMIO_BATT_ID_150_LSB_RMSK, 0, val)
#define PMIO_BATT_ID_150_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_150_LSB_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_150_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_150_LSB_ADDR(base), val, len)
#define PMIO_BATT_ID_150_LSB_ADC_READ_BMSK                                           0xff
#define PMIO_BATT_ID_150_LSB_ADC_READ_SHFT                                            0x0

#define PMIO_BATT_ID_150_MSB_ADDR(x)                                           ((x) + 0x0000006b)
#define PMIO_BATT_ID_150_MSB_RMSK                                                     0x7
#define PMIO_BATT_ID_150_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_150_MSB_ADDR(base), PMIO_BATT_ID_150_MSB_RMSK, 0, val)
#define PMIO_BATT_ID_150_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_ID_150_MSB_ADDR(base), mask, shift, val)
#define PMIO_BATT_ID_150_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_ID_150_MSB_ADDR(base), val, len)
#define PMIO_BATT_ID_150_MSB_ADC_READ_BMSK                                            0x7
#define PMIO_BATT_ID_150_MSB_ADC_READ_SHFT                                            0x0

#define PMIO_BATT_THERM_CTRL_ADDR(x)                                           ((x) + 0x00000070)
#define PMIO_BATT_THERM_CTRL_RMSK                                                     0x3
#define PMIO_BATT_THERM_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_CTRL_ADDR(base), PMIO_BATT_THERM_CTRL_RMSK, 0, val)
#define PMIO_BATT_THERM_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_CTRL_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_THERM_CTRL_ADDR(base), val, len)
#define PMIO_BATT_THERM_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_BATT_THERM_CTRL_ADDR(base), val)
#define PMIO_BATT_THERM_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_BATT_THERM_CTRL_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_BATT_THERM_CTRL_ADDR(base), val, len)
#define PMIO_BATT_THERM_CTRL_CHANNEL_IRQ_BMSK                                         0x2
#define PMIO_BATT_THERM_CTRL_CHANNEL_IRQ_SHFT                                         0x1
#define PMIO_BATT_THERM_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                 0x0
#define PMIO_BATT_THERM_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                  0x1
#define PMIO_BATT_THERM_CTRL_CHANNEL_CONV_BMSK                                        0x1
#define PMIO_BATT_THERM_CTRL_CHANNEL_CONV_SHFT                                        0x0
#define PMIO_BATT_THERM_CTRL_CHANNEL_CONV_DISABLE_FVAL                                0x0
#define PMIO_BATT_THERM_CTRL_CHANNEL_CONV_ENABLE_FVAL                                 0x1

#define PMIO_BATT_THERM_TRIGGER_ADDR(x)                                        ((x) + 0x00000071)
#define PMIO_BATT_THERM_TRIGGER_RMSK                                                  0x1
#define PMIO_BATT_THERM_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_TRIGGER_ADDR(base), PMIO_BATT_THERM_TRIGGER_RMSK, 0, val)
#define PMIO_BATT_THERM_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_THERM_TRIGGER_ADDR(base), val, len)
#define PMIO_BATT_THERM_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_BATT_THERM_TRIGGER_ADDR(base), val)
#define PMIO_BATT_THERM_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_BATT_THERM_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_BATT_THERM_TRIGGER_ADDR(base), val, len)
#define PMIO_BATT_THERM_TRIGGER_TRIGGER_CTRL_BMSK                                     0x1
#define PMIO_BATT_THERM_TRIGGER_TRIGGER_CTRL_SHFT                                     0x0
#define PMIO_BATT_THERM_TRIGGER_TRIGGER_CTRL_DISABLE_FVAL                             0x0
#define PMIO_BATT_THERM_TRIGGER_TRIGGER_CTRL_ENABLE_FVAL                              0x1

#define PMIO_BATT_THERM_STS_ADDR(x)                                            ((x) + 0x00000072)
#define PMIO_BATT_THERM_STS_RMSK                                                     0x83
#define PMIO_BATT_THERM_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_STS_ADDR(base), PMIO_BATT_THERM_STS_RMSK, 0, val)
#define PMIO_BATT_THERM_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_STS_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_THERM_STS_ADDR(base), val, len)
#define PMIO_BATT_THERM_STS_BATT_THERM_RBIAS_STS_BMSK                                0x80
#define PMIO_BATT_THERM_STS_BATT_THERM_RBIAS_STS_SHFT                                 0x7
#define PMIO_BATT_THERM_STS_BATT_THERM_RBIAS_STS_DISABLE_FVAL                         0x0
#define PMIO_BATT_THERM_STS_BATT_THERM_RBIAS_STS_ENABLE_FVAL                          0x1
#define PMIO_BATT_THERM_STS_CHANNEL_STS_BMSK                                          0x2
#define PMIO_BATT_THERM_STS_CHANNEL_STS_SHFT                                          0x1
#define PMIO_BATT_THERM_STS_CHANNEL_STS_NOT_READY_FVAL                                0x0
#define PMIO_BATT_THERM_STS_CHANNEL_STS_READY_FVAL                                    0x1
#define PMIO_BATT_THERM_STS_READING_STS_BMSK                                          0x1
#define PMIO_BATT_THERM_STS_READING_STS_SHFT                                          0x0
#define PMIO_BATT_THERM_STS_READING_STS_NOT_AVAILABLE_FVAL                            0x0
#define PMIO_BATT_THERM_STS_READING_STS_AVAILABLE_FVAL                                0x1

#define PMIO_BATT_THERM_CFG_ADDR(x)                                            ((x) + 0x00000073)
#define PMIO_BATT_THERM_CFG_RMSK                                                     0xe1
#define PMIO_BATT_THERM_CFG_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_CFG_ADDR(base), PMIO_BATT_THERM_CFG_RMSK, 0, val)
#define PMIO_BATT_THERM_CFG_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_CFG_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_CFG_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_THERM_CFG_ADDR(base), val, len)
#define PMIO_BATT_THERM_CFG_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_BATT_THERM_CFG_ADDR(base), val)
#define PMIO_BATT_THERM_CFG_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_BATT_THERM_CFG_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_CFG_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_BATT_THERM_CFG_ADDR(base), val, len)
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BMSK                                           0xe0
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_SHFT                                            0x5
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BIASWAIT_0_MS_FVAL                              0x0
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BIASWAIT_1_MS_FVAL                              0x1
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BIASWAIT_4_MS_FVAL                              0x2
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BIASWAIT_12_MS_FVAL                             0x3
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BIASWAIT_20_MS_FVAL                             0x4
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BIASWAIT_40_MS_FVAL                             0x5
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BIASWAIT_60_MS_FVAL                             0x6
#define PMIO_BATT_THERM_CFG_BIAS_WAIT_BIASWAIT_80_MS_FVAL                             0x7
#define PMIO_BATT_THERM_CFG_GND_SEL_BMSK                                              0x1
#define PMIO_BATT_THERM_CFG_GND_SEL_SHFT                                              0x0
#define PMIO_BATT_THERM_CFG_GND_SEL_SYSTEM_GROUND_FVAL                                0x0
#define PMIO_BATT_THERM_CFG_GND_SEL_BATT_N_FVAL                                       0x1

#define PMIO_BATT_THERM_LSB_ADDR(x)                                            ((x) + 0x00000074)
#define PMIO_BATT_THERM_LSB_RMSK                                                     0xff
#define PMIO_BATT_THERM_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_LSB_ADDR(base), PMIO_BATT_THERM_LSB_RMSK, 0, val)
#define PMIO_BATT_THERM_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_LSB_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_THERM_LSB_ADDR(base), val, len)
#define PMIO_BATT_THERM_LSB_ADC_READ_BMSK                                            0xff
#define PMIO_BATT_THERM_LSB_ADC_READ_SHFT                                             0x0

#define PMIO_BATT_THERM_MSB_ADDR(x)                                            ((x) + 0x00000075)
#define PMIO_BATT_THERM_MSB_RMSK                                                      0x7
#define PMIO_BATT_THERM_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_MSB_ADDR(base), PMIO_BATT_THERM_MSB_RMSK, 0, val)
#define PMIO_BATT_THERM_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_MSB_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_THERM_MSB_ADDR(base), val, len)
#define PMIO_BATT_THERM_MSB_ADC_READ_BMSK                                             0x7
#define PMIO_BATT_THERM_MSB_ADC_READ_SHFT                                             0x0

#define PMIO_BATT_THERM_FREQ_ADDR(x)                                           ((x) + 0x00000076)
#define PMIO_BATT_THERM_FREQ_RMSK                                                    0xff
#define PMIO_BATT_THERM_FREQ_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_FREQ_ADDR(base), PMIO_BATT_THERM_FREQ_RMSK, 0, val)
#define PMIO_BATT_THERM_FREQ_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_BATT_THERM_FREQ_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_FREQ_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_BATT_THERM_FREQ_ADDR(base), val, len)
#define PMIO_BATT_THERM_FREQ_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_BATT_THERM_FREQ_ADDR(base), val)
#define PMIO_BATT_THERM_FREQ_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_BATT_THERM_FREQ_ADDR(base), mask, shift, val)
#define PMIO_BATT_THERM_FREQ_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_BATT_THERM_FREQ_ADDR(base), val, len)
#define PMIO_BATT_THERM_FREQ_VALUE_BMSK                                              0xff
#define PMIO_BATT_THERM_FREQ_VALUE_SHFT                                               0x0

#define PMIO_AUX_THERM_CTRL_ADDR(x)                                            ((x) + 0x00000080)
#define PMIO_AUX_THERM_CTRL_RMSK                                                      0x7
#define PMIO_AUX_THERM_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_CTRL_ADDR(base), PMIO_AUX_THERM_CTRL_RMSK, 0, val)
#define PMIO_AUX_THERM_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_CTRL_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_CTRL_ADDR(base), val, len)
#define PMIO_AUX_THERM_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_AUX_THERM_CTRL_ADDR(base), val)
#define PMIO_AUX_THERM_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_AUX_THERM_CTRL_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_AUX_THERM_CTRL_ADDR(base), val, len)
#define PMIO_AUX_THERM_CTRL_AUX_CMP_CLEAR_BMSK                                        0x4
#define PMIO_AUX_THERM_CTRL_AUX_CMP_CLEAR_SHFT                                        0x2
#define PMIO_AUX_THERM_CTRL_AUX_CMP_CLEAR_DO_NOT_CLEAR_FVAL                           0x0
#define PMIO_AUX_THERM_CTRL_AUX_CMP_CLEAR_CLEAR_FVAL                                  0x1
#define PMIO_AUX_THERM_CTRL_CHANNEL_IRQ_BMSK                                          0x2
#define PMIO_AUX_THERM_CTRL_CHANNEL_IRQ_SHFT                                          0x1
#define PMIO_AUX_THERM_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                  0x0
#define PMIO_AUX_THERM_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                   0x1
#define PMIO_AUX_THERM_CTRL_CHANNEL_CONV_BMSK                                         0x1
#define PMIO_AUX_THERM_CTRL_CHANNEL_CONV_SHFT                                         0x0
#define PMIO_AUX_THERM_CTRL_CHANNEL_CONV_DISABLE_FVAL                                 0x0
#define PMIO_AUX_THERM_CTRL_CHANNEL_CONV_ENABLE_FVAL                                  0x1

#define PMIO_AUX_THERM_TRIGGER_ADDR(x)                                         ((x) + 0x00000081)
#define PMIO_AUX_THERM_TRIGGER_RMSK                                                  0xff
#define PMIO_AUX_THERM_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_TRIGGER_ADDR(base), PMIO_AUX_THERM_TRIGGER_RMSK, 0, val)
#define PMIO_AUX_THERM_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_TRIGGER_ADDR(base), val, len)
#define PMIO_AUX_THERM_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_AUX_THERM_TRIGGER_ADDR(base), val)
#define PMIO_AUX_THERM_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_AUX_THERM_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_AUX_THERM_TRIGGER_ADDR(base), val, len)
#define PMIO_AUX_THERM_TRIGGER_EVERY_CYCLE_BMSK                                      0x80
#define PMIO_AUX_THERM_TRIGGER_EVERY_CYCLE_SHFT                                       0x7
#define PMIO_AUX_THERM_TRIGGER_EVERY_CYCLE_FALSE_FVAL                                 0x0
#define PMIO_AUX_THERM_TRIGGER_EVERY_CYCLE_TRUE_FVAL                                  0x1
#define PMIO_AUX_THERM_TRIGGER_USB_TRIGGER_BMSK                                      0x40
#define PMIO_AUX_THERM_TRIGGER_USB_TRIGGER_SHFT                                       0x6
#define PMIO_AUX_THERM_TRIGGER_USB_TRIGGER_FALSE_FVAL                                 0x0
#define PMIO_AUX_THERM_TRIGGER_USB_TRIGGER_TRUE_FVAL                                  0x1
#define PMIO_AUX_THERM_TRIGGER_DCIN_TRIGGER_BMSK                                     0x20
#define PMIO_AUX_THERM_TRIGGER_DCIN_TRIGGER_SHFT                                      0x5
#define PMIO_AUX_THERM_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                                0x0
#define PMIO_AUX_THERM_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                                 0x1
#define PMIO_AUX_THERM_TRIGGER_OTG_TRIGGER_BMSK                                      0x10
#define PMIO_AUX_THERM_TRIGGER_OTG_TRIGGER_SHFT                                       0x4
#define PMIO_AUX_THERM_TRIGGER_OTG_TRIGGER_FALSE_FVAL                                 0x0
#define PMIO_AUX_THERM_TRIGGER_OTG_TRIGGER_TRUE_FVAL                                  0x1
#define PMIO_AUX_THERM_TRIGGER_BCL_HPM_TRIGGER_BMSK                                   0x8
#define PMIO_AUX_THERM_TRIGGER_BCL_HPM_TRIGGER_SHFT                                   0x3
#define PMIO_AUX_THERM_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                             0x0
#define PMIO_AUX_THERM_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                              0x1
#define PMIO_AUX_THERM_TRIGGER_DTEST_TRIGGER_BMSK                                     0x4
#define PMIO_AUX_THERM_TRIGGER_DTEST_TRIGGER_SHFT                                     0x2
#define PMIO_AUX_THERM_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                               0x0
#define PMIO_AUX_THERM_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                                0x1
#define PMIO_AUX_THERM_TRIGGER_RFU_TRIGGER_BMSK                                       0x2
#define PMIO_AUX_THERM_TRIGGER_RFU_TRIGGER_SHFT                                       0x1
#define PMIO_AUX_THERM_TRIGGER_RFU_TRIGGER_FALSE_FVAL                                 0x0
#define PMIO_AUX_THERM_TRIGGER_RFU_TRIGGER_TRUE_FVAL                                  0x1
#define PMIO_AUX_THERM_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                              0x1
#define PMIO_AUX_THERM_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                              0x0
#define PMIO_AUX_THERM_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                        0x0
#define PMIO_AUX_THERM_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                         0x1

#define PMIO_AUX_THERM_STS_ADDR(x)                                             ((x) + 0x00000082)
#define PMIO_AUX_THERM_STS_RMSK                                                      0xf3
#define PMIO_AUX_THERM_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_STS_ADDR(base), PMIO_AUX_THERM_STS_RMSK, 0, val)
#define PMIO_AUX_THERM_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_STS_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_STS_ADDR(base), val, len)
#define PMIO_AUX_THERM_STS_AUX_THERM_RBIAS_STS_BMSK                                  0x80
#define PMIO_AUX_THERM_STS_AUX_THERM_RBIAS_STS_SHFT                                   0x7
#define PMIO_AUX_THERM_STS_AUX_THERM_RBIAS_STS_DISABLE_FVAL                           0x0
#define PMIO_AUX_THERM_STS_AUX_THERM_RBIAS_STS_ENABLE_FVAL                            0x1
#define PMIO_AUX_THERM_STS_SKIN_TEMP_STS_BMSK                                        0x40
#define PMIO_AUX_THERM_STS_SKIN_TEMP_STS_SHFT                                         0x6
#define PMIO_AUX_THERM_STS_SKIN_TEMP_STS_NOT_READY_FVAL                               0x0
#define PMIO_AUX_THERM_STS_SKIN_TEMP_STS_READY_FVAL                                   0x1
#define PMIO_AUX_THERM_STS_SKIN_TOO_HOT_STS_BMSK                                     0x20
#define PMIO_AUX_THERM_STS_SKIN_TOO_HOT_STS_SHFT                                      0x5
#define PMIO_AUX_THERM_STS_SKIN_TOO_HOT_STS_SKIN_TEMP_BELOW_FVAL                      0x0
#define PMIO_AUX_THERM_STS_SKIN_TOO_HOT_STS_SKIN_TEMP_ABOVE_FVAL                      0x1
#define PMIO_AUX_THERM_STS_SKIN_HOT_STS_BMSK                                         0x10
#define PMIO_AUX_THERM_STS_SKIN_HOT_STS_SHFT                                          0x4
#define PMIO_AUX_THERM_STS_SKIN_HOT_STS_SKIN_TEMP_BELOW_FVAL                          0x0
#define PMIO_AUX_THERM_STS_SKIN_HOT_STS_SKIN_TEMP_ABOVE_FVAL                          0x1
#define PMIO_AUX_THERM_STS_CHANNEL_STS_BMSK                                           0x2
#define PMIO_AUX_THERM_STS_CHANNEL_STS_SHFT                                           0x1
#define PMIO_AUX_THERM_STS_CHANNEL_STS_NOT_READY_FVAL                                 0x0
#define PMIO_AUX_THERM_STS_CHANNEL_STS_READY_FVAL                                     0x1
#define PMIO_AUX_THERM_STS_READING_STS_BMSK                                           0x1
#define PMIO_AUX_THERM_STS_READING_STS_SHFT                                           0x0
#define PMIO_AUX_THERM_STS_READING_STS_NOT_AVAILABLE_FVAL                             0x0
#define PMIO_AUX_THERM_STS_READING_STS_AVAILABLE_FVAL                                 0x1

#define PMIO_AUX_THERM_CFG_ADDR(x)                                             ((x) + 0x00000083)
#define PMIO_AUX_THERM_CFG_RMSK                                                      0xff
#define PMIO_AUX_THERM_CFG_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_CFG_ADDR(base), PMIO_AUX_THERM_CFG_RMSK, 0, val)
#define PMIO_AUX_THERM_CFG_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_CFG_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_CFG_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_CFG_ADDR(base), val, len)
#define PMIO_AUX_THERM_CFG_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_AUX_THERM_CFG_ADDR(base), val)
#define PMIO_AUX_THERM_CFG_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_AUX_THERM_CFG_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_CFG_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_AUX_THERM_CFG_ADDR(base), val, len)
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BMSK                                            0xe0
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_SHFT                                             0x5
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BIASWAIT_0_MS_FVAL                               0x0
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BIASWAIT_1_MS_FVAL                               0x1
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BIASWAIT_4_MS_FVAL                               0x2
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BIASWAIT_12_MS_FVAL                              0x3
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BIASWAIT_20_MS_FVAL                              0x4
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BIASWAIT_40_MS_FVAL                              0x5
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BIASWAIT_60_MS_FVAL                              0x6
#define PMIO_AUX_THERM_CFG_BIAS_WAIT_BIASWAIT_80_MS_FVAL                              0x7
#define PMIO_AUX_THERM_CFG_AUX_TMP_HYST_BMSK                                         0x18
#define PMIO_AUX_THERM_CFG_AUX_TMP_HYST_SHFT                                          0x3
#define PMIO_AUX_THERM_CFG_AUX_TMP_HYST_NO_HYSTERESIS_FVAL                            0x0
#define PMIO_AUX_THERM_CFG_AUX_TMP_HYST_HYST_1_C_FVAL                                 0x1
#define PMIO_AUX_THERM_CFG_AUX_TMP_HYST_HYST_2_C_FVAL                                 0x2
#define PMIO_AUX_THERM_CFG_AUX_TMP_HYST_HYST_3_C_FVAL                                 0x3
#define PMIO_AUX_THERM_CFG_BATT_THERM_SYNC_BMSK                                       0x4
#define PMIO_AUX_THERM_CFG_BATT_THERM_SYNC_SHFT                                       0x2
#define PMIO_AUX_THERM_CFG_BATT_THERM_SYNC_DISABLE_FVAL                               0x0
#define PMIO_AUX_THERM_CFG_BATT_THERM_SYNC_ENABLE_FVAL                                0x1
#define PMIO_AUX_THERM_CFG_AUX_THERM_MODE_BMSK                                        0x2
#define PMIO_AUX_THERM_CFG_AUX_THERM_MODE_SHFT                                        0x1
#define PMIO_AUX_THERM_CFG_AUX_THERM_MODE_SKIN_FVAL                                   0x0
#define PMIO_AUX_THERM_CFG_AUX_THERM_MODE_AUX_FVAL                                    0x1
#define PMIO_AUX_THERM_CFG_GND_SEL_BMSK                                               0x1
#define PMIO_AUX_THERM_CFG_GND_SEL_SHFT                                               0x0
#define PMIO_AUX_THERM_CFG_GND_SEL_SYSTEM_GROUND_FVAL                                 0x0
#define PMIO_AUX_THERM_CFG_GND_SEL_BATT_N_FVAL                                        0x1

#define PMIO_AUX_THERM_LSB_ADDR(x)                                             ((x) + 0x00000084)
#define PMIO_AUX_THERM_LSB_RMSK                                                      0xff
#define PMIO_AUX_THERM_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_LSB_ADDR(base), PMIO_AUX_THERM_LSB_RMSK, 0, val)
#define PMIO_AUX_THERM_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_LSB_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_LSB_ADDR(base), val, len)
#define PMIO_AUX_THERM_LSB_ADC_READ_BMSK                                             0xff
#define PMIO_AUX_THERM_LSB_ADC_READ_SHFT                                              0x0

#define PMIO_AUX_THERM_MSB_ADDR(x)                                             ((x) + 0x00000085)
#define PMIO_AUX_THERM_MSB_RMSK                                                       0x7
#define PMIO_AUX_THERM_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_MSB_ADDR(base), PMIO_AUX_THERM_MSB_RMSK, 0, val)
#define PMIO_AUX_THERM_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_MSB_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_MSB_ADDR(base), val, len)
#define PMIO_AUX_THERM_MSB_ADC_READ_BMSK                                              0x7
#define PMIO_AUX_THERM_MSB_ADC_READ_SHFT                                              0x0

#define PMIO_SKIN_HOT_ADDR(x)                                                  ((x) + 0x00000086)
#define PMIO_SKIN_HOT_RMSK                                                           0xff
#define PMIO_SKIN_HOT_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_SKIN_HOT_ADDR(base), PMIO_SKIN_HOT_RMSK, 0, val)
#define PMIO_SKIN_HOT_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_SKIN_HOT_ADDR(base), mask, shift, val)
#define PMIO_SKIN_HOT_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_SKIN_HOT_ADDR(base), val, len)
#define PMIO_SKIN_HOT_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_SKIN_HOT_ADDR(base), val)
#define PMIO_SKIN_HOT_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_SKIN_HOT_ADDR(base), mask, shift, val)
#define PMIO_SKIN_HOT_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_SKIN_HOT_ADDR(base), val, len)
#define PMIO_SKIN_HOT_THRESHOLD_BMSK                                                 0xff
#define PMIO_SKIN_HOT_THRESHOLD_SHFT                                                  0x0

#define PMIO_SKIN_TOO_HOT_ADDR(x)                                              ((x) + 0x00000087)
#define PMIO_SKIN_TOO_HOT_RMSK                                                       0xff
#define PMIO_SKIN_TOO_HOT_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_SKIN_TOO_HOT_ADDR(base), PMIO_SKIN_TOO_HOT_RMSK, 0, val)
#define PMIO_SKIN_TOO_HOT_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_SKIN_TOO_HOT_ADDR(base), mask, shift, val)
#define PMIO_SKIN_TOO_HOT_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_SKIN_TOO_HOT_ADDR(base), val, len)
#define PMIO_SKIN_TOO_HOT_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_SKIN_TOO_HOT_ADDR(base), val)
#define PMIO_SKIN_TOO_HOT_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_SKIN_TOO_HOT_ADDR(base), mask, shift, val)
#define PMIO_SKIN_TOO_HOT_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_SKIN_TOO_HOT_ADDR(base), val, len)
#define PMIO_SKIN_TOO_HOT_THRESHOLD_BMSK                                             0xff
#define PMIO_SKIN_TOO_HOT_THRESHOLD_SHFT                                              0x0

#define PMIO_AUX_THERM_C1_ADDR(x)                                              ((x) + 0x00000088)
#define PMIO_AUX_THERM_C1_RMSK                                                       0xff
#define PMIO_AUX_THERM_C1_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C1_ADDR(base), PMIO_AUX_THERM_C1_RMSK, 0, val)
#define PMIO_AUX_THERM_C1_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C1_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_C1_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_C1_ADDR(base), val, len)
#define PMIO_AUX_THERM_C1_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_AUX_THERM_C1_ADDR(base), val)
#define PMIO_AUX_THERM_C1_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C1_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_C1_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_AUX_THERM_C1_ADDR(base), val, len)
#define PMIO_AUX_THERM_C1_COEFF_VALUE_BMSK                                           0xff
#define PMIO_AUX_THERM_C1_COEFF_VALUE_SHFT                                            0x0

#define PMIO_AUX_THERM_C2_ADDR(x)                                              ((x) + 0x00000089)
#define PMIO_AUX_THERM_C2_RMSK                                                       0xff
#define PMIO_AUX_THERM_C2_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C2_ADDR(base), PMIO_AUX_THERM_C2_RMSK, 0, val)
#define PMIO_AUX_THERM_C2_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C2_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_C2_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_C2_ADDR(base), val, len)
#define PMIO_AUX_THERM_C2_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_AUX_THERM_C2_ADDR(base), val)
#define PMIO_AUX_THERM_C2_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C2_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_C2_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_AUX_THERM_C2_ADDR(base), val, len)
#define PMIO_AUX_THERM_C2_COEFF_VALUE_BMSK                                           0xff
#define PMIO_AUX_THERM_C2_COEFF_VALUE_SHFT                                            0x0

#define PMIO_AUX_THERM_C3_ADDR(x)                                              ((x) + 0x0000008a)
#define PMIO_AUX_THERM_C3_RMSK                                                       0xff
#define PMIO_AUX_THERM_C3_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C3_ADDR(base), PMIO_AUX_THERM_C3_RMSK, 0, val)
#define PMIO_AUX_THERM_C3_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C3_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_C3_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_C3_ADDR(base), val, len)
#define PMIO_AUX_THERM_C3_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_AUX_THERM_C3_ADDR(base), val)
#define PMIO_AUX_THERM_C3_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_AUX_THERM_C3_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_C3_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_AUX_THERM_C3_ADDR(base), val, len)
#define PMIO_AUX_THERM_C3_COEFF_VALUE_BMSK                                           0xff
#define PMIO_AUX_THERM_C3_COEFF_VALUE_SHFT                                            0x0

#define PMIO_AUX_THERM_HALF_RANGE_ADDR(x)                                      ((x) + 0x0000008b)
#define PMIO_AUX_THERM_HALF_RANGE_RMSK                                               0xff
#define PMIO_AUX_THERM_HALF_RANGE_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_HALF_RANGE_ADDR(base), PMIO_AUX_THERM_HALF_RANGE_RMSK, 0, val)
#define PMIO_AUX_THERM_HALF_RANGE_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_AUX_THERM_HALF_RANGE_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_HALF_RANGE_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_AUX_THERM_HALF_RANGE_ADDR(base), val, len)
#define PMIO_AUX_THERM_HALF_RANGE_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_AUX_THERM_HALF_RANGE_ADDR(base), val)
#define PMIO_AUX_THERM_HALF_RANGE_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_AUX_THERM_HALF_RANGE_ADDR(base), mask, shift, val)
#define PMIO_AUX_THERM_HALF_RANGE_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_AUX_THERM_HALF_RANGE_ADDR(base), val, len)
#define PMIO_AUX_THERM_HALF_RANGE_TEMP_VALUE_BMSK                                    0xff
#define PMIO_AUX_THERM_HALF_RANGE_TEMP_VALUE_SHFT                                     0x0

#define PMIO_USB_IN_V_CTRL_ADDR(x)                                             ((x) + 0x00000090)
#define PMIO_USB_IN_V_CTRL_RMSK                                                       0x3
#define PMIO_USB_IN_V_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_CTRL_ADDR(base), PMIO_USB_IN_V_CTRL_RMSK, 0, val)
#define PMIO_USB_IN_V_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_CTRL_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_V_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_V_CTRL_ADDR(base), val, len)
#define PMIO_USB_IN_V_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_USB_IN_V_CTRL_ADDR(base), val)
#define PMIO_USB_IN_V_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_USB_IN_V_CTRL_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_V_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_USB_IN_V_CTRL_ADDR(base), val, len)
#define PMIO_USB_IN_V_CTRL_CHANNEL_IRQ_BMSK                                           0x2
#define PMIO_USB_IN_V_CTRL_CHANNEL_IRQ_SHFT                                           0x1
#define PMIO_USB_IN_V_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                   0x0
#define PMIO_USB_IN_V_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                    0x1
#define PMIO_USB_IN_V_CTRL_CHANNEL_CONV_BMSK                                          0x1
#define PMIO_USB_IN_V_CTRL_CHANNEL_CONV_SHFT                                          0x0
#define PMIO_USB_IN_V_CTRL_CHANNEL_CONV_DISABLE_FVAL                                  0x0
#define PMIO_USB_IN_V_CTRL_CHANNEL_CONV_ENABLE_FVAL                                   0x1

#define PMIO_USB_IN_V_TRIGGER_ADDR(x)                                          ((x) + 0x00000091)
#define PMIO_USB_IN_V_TRIGGER_RMSK                                                   0xff
#define PMIO_USB_IN_V_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_TRIGGER_ADDR(base), PMIO_USB_IN_V_TRIGGER_RMSK, 0, val)
#define PMIO_USB_IN_V_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_V_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_V_TRIGGER_ADDR(base), val, len)
#define PMIO_USB_IN_V_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_USB_IN_V_TRIGGER_ADDR(base), val)
#define PMIO_USB_IN_V_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_USB_IN_V_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_V_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_USB_IN_V_TRIGGER_ADDR(base), val, len)
#define PMIO_USB_IN_V_TRIGGER_EVERY_CYCLE_BMSK                                       0x80
#define PMIO_USB_IN_V_TRIGGER_EVERY_CYCLE_SHFT                                        0x7
#define PMIO_USB_IN_V_TRIGGER_EVERY_CYCLE_FALSE_FVAL                                  0x0
#define PMIO_USB_IN_V_TRIGGER_EVERY_CYCLE_TRUE_FVAL                                   0x1
#define PMIO_USB_IN_V_TRIGGER_USB_TRIGGER_BMSK                                       0x40
#define PMIO_USB_IN_V_TRIGGER_USB_TRIGGER_SHFT                                        0x6
#define PMIO_USB_IN_V_TRIGGER_USB_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_USB_IN_V_TRIGGER_USB_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_USB_IN_V_TRIGGER_DCIN_TRIGGER_BMSK                                      0x20
#define PMIO_USB_IN_V_TRIGGER_DCIN_TRIGGER_SHFT                                       0x5
#define PMIO_USB_IN_V_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                                 0x0
#define PMIO_USB_IN_V_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                                  0x1
#define PMIO_USB_IN_V_TRIGGER_OTG_TRIGGER_BMSK                                       0x10
#define PMIO_USB_IN_V_TRIGGER_OTG_TRIGGER_SHFT                                        0x4
#define PMIO_USB_IN_V_TRIGGER_OTG_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_USB_IN_V_TRIGGER_OTG_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_USB_IN_V_TRIGGER_BCL_HPM_TRIGGER_BMSK                                    0x8
#define PMIO_USB_IN_V_TRIGGER_BCL_HPM_TRIGGER_SHFT                                    0x3
#define PMIO_USB_IN_V_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                              0x0
#define PMIO_USB_IN_V_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                               0x1
#define PMIO_USB_IN_V_TRIGGER_DTEST_TRIGGER_BMSK                                      0x4
#define PMIO_USB_IN_V_TRIGGER_DTEST_TRIGGER_SHFT                                      0x2
#define PMIO_USB_IN_V_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                                0x0
#define PMIO_USB_IN_V_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                                 0x1
#define PMIO_USB_IN_V_TRIGGER_RFU_TRIGGER_BMSK                                        0x2
#define PMIO_USB_IN_V_TRIGGER_RFU_TRIGGER_SHFT                                        0x1
#define PMIO_USB_IN_V_TRIGGER_RFU_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_USB_IN_V_TRIGGER_RFU_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_USB_IN_V_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                               0x1
#define PMIO_USB_IN_V_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                               0x0
#define PMIO_USB_IN_V_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                         0x0
#define PMIO_USB_IN_V_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                          0x1

#define PMIO_USB_IN_V_STS_ADDR(x)                                              ((x) + 0x00000092)
#define PMIO_USB_IN_V_STS_RMSK                                                        0x3
#define PMIO_USB_IN_V_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_STS_ADDR(base), PMIO_USB_IN_V_STS_RMSK, 0, val)
#define PMIO_USB_IN_V_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_STS_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_V_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_V_STS_ADDR(base), val, len)
#define PMIO_USB_IN_V_STS_CHANNEL_STS_BMSK                                            0x2
#define PMIO_USB_IN_V_STS_CHANNEL_STS_SHFT                                            0x1
#define PMIO_USB_IN_V_STS_CHANNEL_STS_NOT_READY_FVAL                                  0x0
#define PMIO_USB_IN_V_STS_CHANNEL_STS_READY_FVAL                                      0x1
#define PMIO_USB_IN_V_STS_READING_STS_BMSK                                            0x1
#define PMIO_USB_IN_V_STS_READING_STS_SHFT                                            0x0
#define PMIO_USB_IN_V_STS_READING_STS_NOT_AVAILABLE_FVAL                              0x0
#define PMIO_USB_IN_V_STS_READING_STS_AVAILABLE_FVAL                                  0x1

#define PMIO_USB_IN_V_LSB_ADDR(x)                                              ((x) + 0x00000094)
#define PMIO_USB_IN_V_LSB_RMSK                                                       0xff
#define PMIO_USB_IN_V_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_LSB_ADDR(base), PMIO_USB_IN_V_LSB_RMSK, 0, val)
#define PMIO_USB_IN_V_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_LSB_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_V_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_V_LSB_ADDR(base), val, len)
#define PMIO_USB_IN_V_LSB_ADC_READ_BMSK                                              0xff
#define PMIO_USB_IN_V_LSB_ADC_READ_SHFT                                               0x0

#define PMIO_USB_IN_V_MSB_ADDR(x)                                              ((x) + 0x00000095)
#define PMIO_USB_IN_V_MSB_RMSK                                                        0x7
#define PMIO_USB_IN_V_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_MSB_ADDR(base), PMIO_USB_IN_V_MSB_RMSK, 0, val)
#define PMIO_USB_IN_V_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_V_MSB_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_V_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_V_MSB_ADDR(base), val, len)
#define PMIO_USB_IN_V_MSB_ADC_READ_BMSK                                               0x7
#define PMIO_USB_IN_V_MSB_ADC_READ_SHFT                                               0x0

#define PMIO_USB_IN_I_CTRL_ADDR(x)                                             ((x) + 0x00000098)
#define PMIO_USB_IN_I_CTRL_RMSK                                                       0x3
#define PMIO_USB_IN_I_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_CTRL_ADDR(base), PMIO_USB_IN_I_CTRL_RMSK, 0, val)
#define PMIO_USB_IN_I_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_CTRL_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_I_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_I_CTRL_ADDR(base), val, len)
#define PMIO_USB_IN_I_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_USB_IN_I_CTRL_ADDR(base), val)
#define PMIO_USB_IN_I_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_USB_IN_I_CTRL_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_I_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_USB_IN_I_CTRL_ADDR(base), val, len)
#define PMIO_USB_IN_I_CTRL_CHANNEL_IRQ_BMSK                                           0x2
#define PMIO_USB_IN_I_CTRL_CHANNEL_IRQ_SHFT                                           0x1
#define PMIO_USB_IN_I_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                   0x0
#define PMIO_USB_IN_I_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                    0x1
#define PMIO_USB_IN_I_CTRL_CHANNEL_CONV_BMSK                                          0x1
#define PMIO_USB_IN_I_CTRL_CHANNEL_CONV_SHFT                                          0x0
#define PMIO_USB_IN_I_CTRL_CHANNEL_CONV_DISABLE_FVAL                                  0x0
#define PMIO_USB_IN_I_CTRL_CHANNEL_CONV_ENABLE_FVAL                                   0x1

#define PMIO_USB_IN_I_TRIGGER_ADDR(x)                                          ((x) + 0x00000099)
#define PMIO_USB_IN_I_TRIGGER_RMSK                                                   0xff
#define PMIO_USB_IN_I_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_TRIGGER_ADDR(base), PMIO_USB_IN_I_TRIGGER_RMSK, 0, val)
#define PMIO_USB_IN_I_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_I_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_I_TRIGGER_ADDR(base), val, len)
#define PMIO_USB_IN_I_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_USB_IN_I_TRIGGER_ADDR(base), val)
#define PMIO_USB_IN_I_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_USB_IN_I_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_I_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_USB_IN_I_TRIGGER_ADDR(base), val, len)
#define PMIO_USB_IN_I_TRIGGER_EVERY_CYCLE_BMSK                                       0x80
#define PMIO_USB_IN_I_TRIGGER_EVERY_CYCLE_SHFT                                        0x7
#define PMIO_USB_IN_I_TRIGGER_EVERY_CYCLE_FALSE_FVAL                                  0x0
#define PMIO_USB_IN_I_TRIGGER_EVERY_CYCLE_TRUE_FVAL                                   0x1
#define PMIO_USB_IN_I_TRIGGER_USB_TRIGGER_BMSK                                       0x40
#define PMIO_USB_IN_I_TRIGGER_USB_TRIGGER_SHFT                                        0x6
#define PMIO_USB_IN_I_TRIGGER_USB_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_USB_IN_I_TRIGGER_USB_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_USB_IN_I_TRIGGER_DCIN_TRIGGER_BMSK                                      0x20
#define PMIO_USB_IN_I_TRIGGER_DCIN_TRIGGER_SHFT                                       0x5
#define PMIO_USB_IN_I_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                                 0x0
#define PMIO_USB_IN_I_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                                  0x1
#define PMIO_USB_IN_I_TRIGGER_OTG_TRIGGER_BMSK                                       0x10
#define PMIO_USB_IN_I_TRIGGER_OTG_TRIGGER_SHFT                                        0x4
#define PMIO_USB_IN_I_TRIGGER_OTG_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_USB_IN_I_TRIGGER_OTG_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_USB_IN_I_TRIGGER_BCL_HPM_TRIGGER_BMSK                                    0x8
#define PMIO_USB_IN_I_TRIGGER_BCL_HPM_TRIGGER_SHFT                                    0x3
#define PMIO_USB_IN_I_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                              0x0
#define PMIO_USB_IN_I_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                               0x1
#define PMIO_USB_IN_I_TRIGGER_DTEST_TRIGGER_BMSK                                      0x4
#define PMIO_USB_IN_I_TRIGGER_DTEST_TRIGGER_SHFT                                      0x2
#define PMIO_USB_IN_I_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                                0x0
#define PMIO_USB_IN_I_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                                 0x1
#define PMIO_USB_IN_I_TRIGGER_RFU_TRIGGER_BMSK                                        0x2
#define PMIO_USB_IN_I_TRIGGER_RFU_TRIGGER_SHFT                                        0x1
#define PMIO_USB_IN_I_TRIGGER_RFU_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_USB_IN_I_TRIGGER_RFU_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_USB_IN_I_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                               0x1
#define PMIO_USB_IN_I_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                               0x0
#define PMIO_USB_IN_I_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                         0x0
#define PMIO_USB_IN_I_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                          0x1

#define PMIO_USB_IN_I_STS_ADDR(x)                                              ((x) + 0x0000009a)
#define PMIO_USB_IN_I_STS_RMSK                                                        0x3
#define PMIO_USB_IN_I_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_STS_ADDR(base), PMIO_USB_IN_I_STS_RMSK, 0, val)
#define PMIO_USB_IN_I_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_STS_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_I_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_I_STS_ADDR(base), val, len)
#define PMIO_USB_IN_I_STS_CHANNEL_STS_BMSK                                            0x2
#define PMIO_USB_IN_I_STS_CHANNEL_STS_SHFT                                            0x1
#define PMIO_USB_IN_I_STS_CHANNEL_STS_NOT_READY_FVAL                                  0x0
#define PMIO_USB_IN_I_STS_CHANNEL_STS_READY_FVAL                                      0x1
#define PMIO_USB_IN_I_STS_READING_STS_BMSK                                            0x1
#define PMIO_USB_IN_I_STS_READING_STS_SHFT                                            0x0
#define PMIO_USB_IN_I_STS_READING_STS_NOT_AVAILABLE_FVAL                              0x0
#define PMIO_USB_IN_I_STS_READING_STS_AVAILABLE_FVAL                                  0x1

#define PMIO_USB_IN_I_LSB_ADDR(x)                                              ((x) + 0x0000009c)
#define PMIO_USB_IN_I_LSB_RMSK                                                       0xff
#define PMIO_USB_IN_I_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_LSB_ADDR(base), PMIO_USB_IN_I_LSB_RMSK, 0, val)
#define PMIO_USB_IN_I_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_LSB_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_I_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_I_LSB_ADDR(base), val, len)
#define PMIO_USB_IN_I_LSB_ADC_READ_BMSK                                              0xff
#define PMIO_USB_IN_I_LSB_ADC_READ_SHFT                                               0x0

#define PMIO_USB_IN_I_MSB_ADDR(x)                                              ((x) + 0x0000009d)
#define PMIO_USB_IN_I_MSB_RMSK                                                        0x7
#define PMIO_USB_IN_I_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_MSB_ADDR(base), PMIO_USB_IN_I_MSB_RMSK, 0, val)
#define PMIO_USB_IN_I_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USB_IN_I_MSB_ADDR(base), mask, shift, val)
#define PMIO_USB_IN_I_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USB_IN_I_MSB_ADDR(base), val, len)
#define PMIO_USB_IN_I_MSB_ADC_READ_BMSK                                               0x7
#define PMIO_USB_IN_I_MSB_ADC_READ_SHFT                                               0x0

#define PMIO_DC_IN_V_CTRL_ADDR(x)                                              ((x) + 0x000000a0)
#define PMIO_DC_IN_V_CTRL_RMSK                                                        0x3
#define PMIO_DC_IN_V_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_CTRL_ADDR(base), PMIO_DC_IN_V_CTRL_RMSK, 0, val)
#define PMIO_DC_IN_V_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_CTRL_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_V_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_V_CTRL_ADDR(base), val, len)
#define PMIO_DC_IN_V_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DC_IN_V_CTRL_ADDR(base), val)
#define PMIO_DC_IN_V_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DC_IN_V_CTRL_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_V_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DC_IN_V_CTRL_ADDR(base), val, len)
#define PMIO_DC_IN_V_CTRL_CHANNEL_IRQ_BMSK                                            0x2
#define PMIO_DC_IN_V_CTRL_CHANNEL_IRQ_SHFT                                            0x1
#define PMIO_DC_IN_V_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                    0x0
#define PMIO_DC_IN_V_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                     0x1
#define PMIO_DC_IN_V_CTRL_CHANNEL_CONV_BMSK                                           0x1
#define PMIO_DC_IN_V_CTRL_CHANNEL_CONV_SHFT                                           0x0
#define PMIO_DC_IN_V_CTRL_CHANNEL_CONV_DISABLE_FVAL                                   0x0
#define PMIO_DC_IN_V_CTRL_CHANNEL_CONV_ENABLE_FVAL                                    0x1

#define PMIO_DC_IN_V_TRIGGER_ADDR(x)                                           ((x) + 0x000000a1)
#define PMIO_DC_IN_V_TRIGGER_RMSK                                                    0xff
#define PMIO_DC_IN_V_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_TRIGGER_ADDR(base), PMIO_DC_IN_V_TRIGGER_RMSK, 0, val)
#define PMIO_DC_IN_V_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_V_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_V_TRIGGER_ADDR(base), val, len)
#define PMIO_DC_IN_V_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DC_IN_V_TRIGGER_ADDR(base), val)
#define PMIO_DC_IN_V_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DC_IN_V_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_V_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DC_IN_V_TRIGGER_ADDR(base), val, len)
#define PMIO_DC_IN_V_TRIGGER_EVERY_CYCLE_BMSK                                        0x80
#define PMIO_DC_IN_V_TRIGGER_EVERY_CYCLE_SHFT                                         0x7
#define PMIO_DC_IN_V_TRIGGER_EVERY_CYCLE_FALSE_FVAL                                   0x0
#define PMIO_DC_IN_V_TRIGGER_EVERY_CYCLE_TRUE_FVAL                                    0x1
#define PMIO_DC_IN_V_TRIGGER_USB_TRIGGER_BMSK                                        0x40
#define PMIO_DC_IN_V_TRIGGER_USB_TRIGGER_SHFT                                         0x6
#define PMIO_DC_IN_V_TRIGGER_USB_TRIGGER_FALSE_FVAL                                   0x0
#define PMIO_DC_IN_V_TRIGGER_USB_TRIGGER_TRUE_FVAL                                    0x1
#define PMIO_DC_IN_V_TRIGGER_DCIN_TRIGGER_BMSK                                       0x20
#define PMIO_DC_IN_V_TRIGGER_DCIN_TRIGGER_SHFT                                        0x5
#define PMIO_DC_IN_V_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_DC_IN_V_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_DC_IN_V_TRIGGER_OTG_TRIGGER_BMSK                                        0x10
#define PMIO_DC_IN_V_TRIGGER_OTG_TRIGGER_SHFT                                         0x4
#define PMIO_DC_IN_V_TRIGGER_OTG_TRIGGER_FALSE_FVAL                                   0x0
#define PMIO_DC_IN_V_TRIGGER_OTG_TRIGGER_TRUE_FVAL                                    0x1
#define PMIO_DC_IN_V_TRIGGER_BCL_HPM_TRIGGER_BMSK                                     0x8
#define PMIO_DC_IN_V_TRIGGER_BCL_HPM_TRIGGER_SHFT                                     0x3
#define PMIO_DC_IN_V_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                               0x0
#define PMIO_DC_IN_V_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                                0x1
#define PMIO_DC_IN_V_TRIGGER_DTEST_TRIGGER_BMSK                                       0x4
#define PMIO_DC_IN_V_TRIGGER_DTEST_TRIGGER_SHFT                                       0x2
#define PMIO_DC_IN_V_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                                 0x0
#define PMIO_DC_IN_V_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                                  0x1
#define PMIO_DC_IN_V_TRIGGER_RFU_TRIGGER_BMSK                                         0x2
#define PMIO_DC_IN_V_TRIGGER_RFU_TRIGGER_SHFT                                         0x1
#define PMIO_DC_IN_V_TRIGGER_RFU_TRIGGER_FALSE_FVAL                                   0x0
#define PMIO_DC_IN_V_TRIGGER_RFU_TRIGGER_TRUE_FVAL                                    0x1
#define PMIO_DC_IN_V_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                                0x1
#define PMIO_DC_IN_V_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                                0x0
#define PMIO_DC_IN_V_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                          0x0
#define PMIO_DC_IN_V_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                           0x1

#define PMIO_DC_IN_V_STS_ADDR(x)                                               ((x) + 0x000000a2)
#define PMIO_DC_IN_V_STS_RMSK                                                         0x3
#define PMIO_DC_IN_V_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_STS_ADDR(base), PMIO_DC_IN_V_STS_RMSK, 0, val)
#define PMIO_DC_IN_V_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_STS_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_V_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_V_STS_ADDR(base), val, len)
#define PMIO_DC_IN_V_STS_CHANNEL_STS_BMSK                                             0x2
#define PMIO_DC_IN_V_STS_CHANNEL_STS_SHFT                                             0x1
#define PMIO_DC_IN_V_STS_CHANNEL_STS_NOT_READY_FVAL                                   0x0
#define PMIO_DC_IN_V_STS_CHANNEL_STS_READY_FVAL                                       0x1
#define PMIO_DC_IN_V_STS_READING_STS_BMSK                                             0x1
#define PMIO_DC_IN_V_STS_READING_STS_SHFT                                             0x0
#define PMIO_DC_IN_V_STS_READING_STS_NOT_AVAILABLE_FVAL                               0x0
#define PMIO_DC_IN_V_STS_READING_STS_AVAILABLE_FVAL                                   0x1

#define PMIO_DC_IN_V_LSB_ADDR(x)                                               ((x) + 0x000000a4)
#define PMIO_DC_IN_V_LSB_RMSK                                                        0xff
#define PMIO_DC_IN_V_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_LSB_ADDR(base), PMIO_DC_IN_V_LSB_RMSK, 0, val)
#define PMIO_DC_IN_V_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_LSB_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_V_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_V_LSB_ADDR(base), val, len)
#define PMIO_DC_IN_V_LSB_ADC_READ_BMSK                                               0xff
#define PMIO_DC_IN_V_LSB_ADC_READ_SHFT                                                0x0

#define PMIO_DC_IN_V_MSB_ADDR(x)                                               ((x) + 0x000000a5)
#define PMIO_DC_IN_V_MSB_RMSK                                                         0x7
#define PMIO_DC_IN_V_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_MSB_ADDR(base), PMIO_DC_IN_V_MSB_RMSK, 0, val)
#define PMIO_DC_IN_V_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_V_MSB_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_V_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_V_MSB_ADDR(base), val, len)
#define PMIO_DC_IN_V_MSB_ADC_READ_BMSK                                                0x7
#define PMIO_DC_IN_V_MSB_ADC_READ_SHFT                                                0x0

#define PMIO_DC_IN_I_CTRL_ADDR(x)                                              ((x) + 0x000000a8)
#define PMIO_DC_IN_I_CTRL_RMSK                                                        0x3
#define PMIO_DC_IN_I_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_CTRL_ADDR(base), PMIO_DC_IN_I_CTRL_RMSK, 0, val)
#define PMIO_DC_IN_I_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_CTRL_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_I_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_I_CTRL_ADDR(base), val, len)
#define PMIO_DC_IN_I_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DC_IN_I_CTRL_ADDR(base), val)
#define PMIO_DC_IN_I_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DC_IN_I_CTRL_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_I_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DC_IN_I_CTRL_ADDR(base), val, len)
#define PMIO_DC_IN_I_CTRL_CHANNEL_IRQ_BMSK                                            0x2
#define PMIO_DC_IN_I_CTRL_CHANNEL_IRQ_SHFT                                            0x1
#define PMIO_DC_IN_I_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                    0x0
#define PMIO_DC_IN_I_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                     0x1
#define PMIO_DC_IN_I_CTRL_CHANNEL_CONV_BMSK                                           0x1
#define PMIO_DC_IN_I_CTRL_CHANNEL_CONV_SHFT                                           0x0
#define PMIO_DC_IN_I_CTRL_CHANNEL_CONV_DISABLE_FVAL                                   0x0
#define PMIO_DC_IN_I_CTRL_CHANNEL_CONV_ENABLE_FVAL                                    0x1

#define PMIO_DC_IN_I_TRIGGER_ADDR(x)                                           ((x) + 0x000000a9)
#define PMIO_DC_IN_I_TRIGGER_RMSK                                                    0xff
#define PMIO_DC_IN_I_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_TRIGGER_ADDR(base), PMIO_DC_IN_I_TRIGGER_RMSK, 0, val)
#define PMIO_DC_IN_I_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_I_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_I_TRIGGER_ADDR(base), val, len)
#define PMIO_DC_IN_I_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DC_IN_I_TRIGGER_ADDR(base), val)
#define PMIO_DC_IN_I_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DC_IN_I_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_I_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DC_IN_I_TRIGGER_ADDR(base), val, len)
#define PMIO_DC_IN_I_TRIGGER_EVERY_CYCLE_BMSK                                        0x80
#define PMIO_DC_IN_I_TRIGGER_EVERY_CYCLE_SHFT                                         0x7
#define PMIO_DC_IN_I_TRIGGER_EVERY_CYCLE_FALSE_FVAL                                   0x0
#define PMIO_DC_IN_I_TRIGGER_EVERY_CYCLE_TRUE_FVAL                                    0x1
#define PMIO_DC_IN_I_TRIGGER_USB_TRIGGER_BMSK                                        0x40
#define PMIO_DC_IN_I_TRIGGER_USB_TRIGGER_SHFT                                         0x6
#define PMIO_DC_IN_I_TRIGGER_USB_TRIGGER_FALSE_FVAL                                   0x0
#define PMIO_DC_IN_I_TRIGGER_USB_TRIGGER_TRUE_FVAL                                    0x1
#define PMIO_DC_IN_I_TRIGGER_DCIN_TRIGGER_BMSK                                       0x20
#define PMIO_DC_IN_I_TRIGGER_DCIN_TRIGGER_SHFT                                        0x5
#define PMIO_DC_IN_I_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_DC_IN_I_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_DC_IN_I_TRIGGER_OTG_TRIGGER_BMSK                                        0x10
#define PMIO_DC_IN_I_TRIGGER_OTG_TRIGGER_SHFT                                         0x4
#define PMIO_DC_IN_I_TRIGGER_OTG_TRIGGER_FALSE_FVAL                                   0x0
#define PMIO_DC_IN_I_TRIGGER_OTG_TRIGGER_TRUE_FVAL                                    0x1
#define PMIO_DC_IN_I_TRIGGER_BCL_HPM_TRIGGER_BMSK                                     0x8
#define PMIO_DC_IN_I_TRIGGER_BCL_HPM_TRIGGER_SHFT                                     0x3
#define PMIO_DC_IN_I_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                               0x0
#define PMIO_DC_IN_I_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                                0x1
#define PMIO_DC_IN_I_TRIGGER_DTEST_TRIGGER_BMSK                                       0x4
#define PMIO_DC_IN_I_TRIGGER_DTEST_TRIGGER_SHFT                                       0x2
#define PMIO_DC_IN_I_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                                 0x0
#define PMIO_DC_IN_I_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                                  0x1
#define PMIO_DC_IN_I_TRIGGER_RFU_TRIGGER_BMSK                                         0x2
#define PMIO_DC_IN_I_TRIGGER_RFU_TRIGGER_SHFT                                         0x1
#define PMIO_DC_IN_I_TRIGGER_RFU_TRIGGER_FALSE_FVAL                                   0x0
#define PMIO_DC_IN_I_TRIGGER_RFU_TRIGGER_TRUE_FVAL                                    0x1
#define PMIO_DC_IN_I_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                                0x1
#define PMIO_DC_IN_I_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                                0x0
#define PMIO_DC_IN_I_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                          0x0
#define PMIO_DC_IN_I_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                           0x1

#define PMIO_DC_IN_I_STS_ADDR(x)                                               ((x) + 0x000000aa)
#define PMIO_DC_IN_I_STS_RMSK                                                         0x3
#define PMIO_DC_IN_I_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_STS_ADDR(base), PMIO_DC_IN_I_STS_RMSK, 0, val)
#define PMIO_DC_IN_I_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_STS_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_I_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_I_STS_ADDR(base), val, len)
#define PMIO_DC_IN_I_STS_CHANNEL_STS_BMSK                                             0x2
#define PMIO_DC_IN_I_STS_CHANNEL_STS_SHFT                                             0x1
#define PMIO_DC_IN_I_STS_CHANNEL_STS_NOT_READY_FVAL                                   0x0
#define PMIO_DC_IN_I_STS_CHANNEL_STS_READY_FVAL                                       0x1
#define PMIO_DC_IN_I_STS_READING_STS_BMSK                                             0x1
#define PMIO_DC_IN_I_STS_READING_STS_SHFT                                             0x0
#define PMIO_DC_IN_I_STS_READING_STS_NOT_AVAILABLE_FVAL                               0x0
#define PMIO_DC_IN_I_STS_READING_STS_AVAILABLE_FVAL                                   0x1

#define PMIO_DC_IN_I_LSB_ADDR(x)                                               ((x) + 0x000000ac)
#define PMIO_DC_IN_I_LSB_RMSK                                                        0xff
#define PMIO_DC_IN_I_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_LSB_ADDR(base), PMIO_DC_IN_I_LSB_RMSK, 0, val)
#define PMIO_DC_IN_I_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_LSB_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_I_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_I_LSB_ADDR(base), val, len)
#define PMIO_DC_IN_I_LSB_ADC_READ_BMSK                                               0xff
#define PMIO_DC_IN_I_LSB_ADC_READ_SHFT                                                0x0

#define PMIO_DC_IN_I_MSB_ADDR(x)                                               ((x) + 0x000000ad)
#define PMIO_DC_IN_I_MSB_RMSK                                                         0x7
#define PMIO_DC_IN_I_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_MSB_ADDR(base), PMIO_DC_IN_I_MSB_RMSK, 0, val)
#define PMIO_DC_IN_I_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DC_IN_I_MSB_ADDR(base), mask, shift, val)
#define PMIO_DC_IN_I_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DC_IN_I_MSB_ADDR(base), val, len)
#define PMIO_DC_IN_I_MSB_ADC_READ_BMSK                                                0x7
#define PMIO_DC_IN_I_MSB_ADC_READ_SHFT                                                0x0

#define PMIO_PMI_DIE_TEMP_CTRL_ADDR(x)                                         ((x) + 0x000000b0)
#define PMIO_PMI_DIE_TEMP_CTRL_RMSK                                                   0x3
#define PMIO_PMI_DIE_TEMP_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_CTRL_ADDR(base), PMIO_PMI_DIE_TEMP_CTRL_RMSK, 0, val)
#define PMIO_PMI_DIE_TEMP_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_CTRL_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_CTRL_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_CTRL_ADDR(base), val)
#define PMIO_PMI_DIE_TEMP_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_CTRL_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_PMI_DIE_TEMP_CTRL_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_CTRL_CHANNEL_IRQ_BMSK                                       0x2
#define PMIO_PMI_DIE_TEMP_CTRL_CHANNEL_IRQ_SHFT                                       0x1
#define PMIO_PMI_DIE_TEMP_CTRL_CHANNEL_IRQ_DISABLE_FVAL                               0x0
#define PMIO_PMI_DIE_TEMP_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                0x1
#define PMIO_PMI_DIE_TEMP_CTRL_CHANNEL_CONV_BMSK                                      0x1
#define PMIO_PMI_DIE_TEMP_CTRL_CHANNEL_CONV_SHFT                                      0x0
#define PMIO_PMI_DIE_TEMP_CTRL_CHANNEL_CONV_DISABLE_FVAL                              0x0
#define PMIO_PMI_DIE_TEMP_CTRL_CHANNEL_CONV_ENABLE_FVAL                               0x1

#define PMIO_PMI_DIE_TEMP_TRIGGER_ADDR(x)                                      ((x) + 0x000000b1)
#define PMIO_PMI_DIE_TEMP_TRIGGER_RMSK                                               0xff
#define PMIO_PMI_DIE_TEMP_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_TRIGGER_ADDR(base), PMIO_PMI_DIE_TEMP_TRIGGER_RMSK, 0, val)
#define PMIO_PMI_DIE_TEMP_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_TRIGGER_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_TRIGGER_ADDR(base), val)
#define PMIO_PMI_DIE_TEMP_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_PMI_DIE_TEMP_TRIGGER_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_TRIGGER_EVERY_CYCLE_BMSK                                   0x80
#define PMIO_PMI_DIE_TEMP_TRIGGER_EVERY_CYCLE_SHFT                                    0x7
#define PMIO_PMI_DIE_TEMP_TRIGGER_EVERY_CYCLE_FALSE_FVAL                              0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_EVERY_CYCLE_TRUE_FVAL                               0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_USB_TRIGGER_BMSK                                   0x40
#define PMIO_PMI_DIE_TEMP_TRIGGER_USB_TRIGGER_SHFT                                    0x6
#define PMIO_PMI_DIE_TEMP_TRIGGER_USB_TRIGGER_FALSE_FVAL                              0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_USB_TRIGGER_TRUE_FVAL                               0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_DCIN_TRIGGER_BMSK                                  0x20
#define PMIO_PMI_DIE_TEMP_TRIGGER_DCIN_TRIGGER_SHFT                                   0x5
#define PMIO_PMI_DIE_TEMP_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                             0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                              0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_OTG_TRIGGER_BMSK                                   0x10
#define PMIO_PMI_DIE_TEMP_TRIGGER_OTG_TRIGGER_SHFT                                    0x4
#define PMIO_PMI_DIE_TEMP_TRIGGER_OTG_TRIGGER_FALSE_FVAL                              0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_OTG_TRIGGER_TRUE_FVAL                               0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_BCL_HPM_TRIGGER_BMSK                                0x8
#define PMIO_PMI_DIE_TEMP_TRIGGER_BCL_HPM_TRIGGER_SHFT                                0x3
#define PMIO_PMI_DIE_TEMP_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                          0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                           0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_DTEST_TRIGGER_BMSK                                  0x4
#define PMIO_PMI_DIE_TEMP_TRIGGER_DTEST_TRIGGER_SHFT                                  0x2
#define PMIO_PMI_DIE_TEMP_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                            0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                             0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_RFU_TRIGGER_BMSK                                    0x2
#define PMIO_PMI_DIE_TEMP_TRIGGER_RFU_TRIGGER_SHFT                                    0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_RFU_TRIGGER_FALSE_FVAL                              0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_RFU_TRIGGER_TRUE_FVAL                               0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                           0x1
#define PMIO_PMI_DIE_TEMP_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                           0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                     0x0
#define PMIO_PMI_DIE_TEMP_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                      0x1

#define PMIO_PMI_DIE_TEMP_STS_ADDR(x)                                          ((x) + 0x000000b2)
#define PMIO_PMI_DIE_TEMP_STS_RMSK                                                    0x3
#define PMIO_PMI_DIE_TEMP_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_STS_ADDR(base), PMIO_PMI_DIE_TEMP_STS_RMSK, 0, val)
#define PMIO_PMI_DIE_TEMP_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_STS_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_STS_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_STS_CHANNEL_STS_BMSK                                        0x2
#define PMIO_PMI_DIE_TEMP_STS_CHANNEL_STS_SHFT                                        0x1
#define PMIO_PMI_DIE_TEMP_STS_CHANNEL_STS_NOT_READY_FVAL                              0x0
#define PMIO_PMI_DIE_TEMP_STS_CHANNEL_STS_READY_FVAL                                  0x1
#define PMIO_PMI_DIE_TEMP_STS_READING_STS_BMSK                                        0x1
#define PMIO_PMI_DIE_TEMP_STS_READING_STS_SHFT                                        0x0
#define PMIO_PMI_DIE_TEMP_STS_READING_STS_NOT_AVAILABLE_FVAL                          0x0
#define PMIO_PMI_DIE_TEMP_STS_READING_STS_AVAILABLE_FVAL                              0x1

#define PMIO_PMI_DIE_TEMP_CFG_ADDR(x)                                          ((x) + 0x000000b3)
#define PMIO_PMI_DIE_TEMP_CFG_RMSK                                                   0xe0
#define PMIO_PMI_DIE_TEMP_CFG_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_CFG_ADDR(base), PMIO_PMI_DIE_TEMP_CFG_RMSK, 0, val)
#define PMIO_PMI_DIE_TEMP_CFG_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_CFG_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_CFG_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_CFG_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_CFG_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_CFG_ADDR(base), val)
#define PMIO_PMI_DIE_TEMP_CFG_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_CFG_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_CFG_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_PMI_DIE_TEMP_CFG_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BMSK                                         0xe0
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_SHFT                                          0x5
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BIASWAIT_0_MS_FVAL                            0x0
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BIASWAIT_1_MS_FVAL                            0x1
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BIASWAIT_4_MS_FVAL                            0x2
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BIASWAIT_12_MS_FVAL                           0x3
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BIASWAIT_20_MS_FVAL                           0x4
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BIASWAIT_40_MS_FVAL                           0x5
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BIASWAIT_60_MS_FVAL                           0x6
#define PMIO_PMI_DIE_TEMP_CFG_BIAS_WAIT_BIASWAIT_80_MS_FVAL                           0x7

#define PMIO_PMI_DIE_TEMP_LSB_ADDR(x)                                          ((x) + 0x000000b4)
#define PMIO_PMI_DIE_TEMP_LSB_RMSK                                                   0xff
#define PMIO_PMI_DIE_TEMP_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_LSB_ADDR(base), PMIO_PMI_DIE_TEMP_LSB_RMSK, 0, val)
#define PMIO_PMI_DIE_TEMP_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_LSB_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_LSB_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_LSB_ADC_READ_BMSK                                          0xff
#define PMIO_PMI_DIE_TEMP_LSB_ADC_READ_SHFT                                           0x0

#define PMIO_PMI_DIE_TEMP_MSB_ADDR(x)                                          ((x) + 0x000000b5)
#define PMIO_PMI_DIE_TEMP_MSB_RMSK                                                    0x7
#define PMIO_PMI_DIE_TEMP_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_MSB_ADDR(base), PMIO_PMI_DIE_TEMP_MSB_RMSK, 0, val)
#define PMIO_PMI_DIE_TEMP_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PMI_DIE_TEMP_MSB_ADDR(base), mask, shift, val)
#define PMIO_PMI_DIE_TEMP_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PMI_DIE_TEMP_MSB_ADDR(base), val, len)
#define PMIO_PMI_DIE_TEMP_MSB_ADC_READ_BMSK                                           0x7
#define PMIO_PMI_DIE_TEMP_MSB_ADC_READ_SHFT                                           0x0

#define PMIO_CHARGER_TEMP_CTRL_ADDR(x)                                         ((x) + 0x000000b8)
#define PMIO_CHARGER_TEMP_CTRL_RMSK                                                   0x3
#define PMIO_CHARGER_TEMP_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_CTRL_ADDR(base), PMIO_CHARGER_TEMP_CTRL_RMSK, 0, val)
#define PMIO_CHARGER_TEMP_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_CTRL_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_CTRL_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_CTRL_ADDR(base), val)
#define PMIO_CHARGER_TEMP_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_CTRL_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_CHARGER_TEMP_CTRL_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_CTRL_CHANNEL_IRQ_BMSK                                       0x2
#define PMIO_CHARGER_TEMP_CTRL_CHANNEL_IRQ_SHFT                                       0x1
#define PMIO_CHARGER_TEMP_CTRL_CHANNEL_IRQ_DISABLE_FVAL                               0x0
#define PMIO_CHARGER_TEMP_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                0x1
#define PMIO_CHARGER_TEMP_CTRL_CHANNEL_CONV_BMSK                                      0x1
#define PMIO_CHARGER_TEMP_CTRL_CHANNEL_CONV_SHFT                                      0x0
#define PMIO_CHARGER_TEMP_CTRL_CHANNEL_CONV_DISABLE_FVAL                              0x0
#define PMIO_CHARGER_TEMP_CTRL_CHANNEL_CONV_ENABLE_FVAL                               0x1

#define PMIO_CHARGER_TEMP_TRIGGER_ADDR(x)                                      ((x) + 0x000000b9)
#define PMIO_CHARGER_TEMP_TRIGGER_RMSK                                               0xff
#define PMIO_CHARGER_TEMP_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_TRIGGER_ADDR(base), PMIO_CHARGER_TEMP_TRIGGER_RMSK, 0, val)
#define PMIO_CHARGER_TEMP_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_TRIGGER_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_TRIGGER_ADDR(base), val)
#define PMIO_CHARGER_TEMP_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_CHARGER_TEMP_TRIGGER_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_TRIGGER_EVERY_CYCLE_BMSK                                   0x80
#define PMIO_CHARGER_TEMP_TRIGGER_EVERY_CYCLE_SHFT                                    0x7
#define PMIO_CHARGER_TEMP_TRIGGER_EVERY_CYCLE_FALSE_FVAL                              0x0
#define PMIO_CHARGER_TEMP_TRIGGER_EVERY_CYCLE_TRUE_FVAL                               0x1
#define PMIO_CHARGER_TEMP_TRIGGER_USB_TRIGGER_BMSK                                   0x40
#define PMIO_CHARGER_TEMP_TRIGGER_USB_TRIGGER_SHFT                                    0x6
#define PMIO_CHARGER_TEMP_TRIGGER_USB_TRIGGER_FALSE_FVAL                              0x0
#define PMIO_CHARGER_TEMP_TRIGGER_USB_TRIGGER_TRUE_FVAL                               0x1
#define PMIO_CHARGER_TEMP_TRIGGER_DCIN_TRIGGER_BMSK                                  0x20
#define PMIO_CHARGER_TEMP_TRIGGER_DCIN_TRIGGER_SHFT                                   0x5
#define PMIO_CHARGER_TEMP_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                             0x0
#define PMIO_CHARGER_TEMP_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                              0x1
#define PMIO_CHARGER_TEMP_TRIGGER_OTG_TRIGGER_BMSK                                   0x10
#define PMIO_CHARGER_TEMP_TRIGGER_OTG_TRIGGER_SHFT                                    0x4
#define PMIO_CHARGER_TEMP_TRIGGER_OTG_TRIGGER_FALSE_FVAL                              0x0
#define PMIO_CHARGER_TEMP_TRIGGER_OTG_TRIGGER_TRUE_FVAL                               0x1
#define PMIO_CHARGER_TEMP_TRIGGER_BCL_HPM_TRIGGER_BMSK                                0x8
#define PMIO_CHARGER_TEMP_TRIGGER_BCL_HPM_TRIGGER_SHFT                                0x3
#define PMIO_CHARGER_TEMP_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                          0x0
#define PMIO_CHARGER_TEMP_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                           0x1
#define PMIO_CHARGER_TEMP_TRIGGER_DTEST_TRIGGER_BMSK                                  0x4
#define PMIO_CHARGER_TEMP_TRIGGER_DTEST_TRIGGER_SHFT                                  0x2
#define PMIO_CHARGER_TEMP_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                            0x0
#define PMIO_CHARGER_TEMP_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                             0x1
#define PMIO_CHARGER_TEMP_TRIGGER_RFU_TRIGGER_BMSK                                    0x2
#define PMIO_CHARGER_TEMP_TRIGGER_RFU_TRIGGER_SHFT                                    0x1
#define PMIO_CHARGER_TEMP_TRIGGER_RFU_TRIGGER_FALSE_FVAL                              0x0
#define PMIO_CHARGER_TEMP_TRIGGER_RFU_TRIGGER_TRUE_FVAL                               0x1
#define PMIO_CHARGER_TEMP_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                           0x1
#define PMIO_CHARGER_TEMP_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                           0x0
#define PMIO_CHARGER_TEMP_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                     0x0
#define PMIO_CHARGER_TEMP_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                      0x1

#define PMIO_CHARGER_TEMP_STS_ADDR(x)                                          ((x) + 0x000000ba)
#define PMIO_CHARGER_TEMP_STS_RMSK                                                   0x33
#define PMIO_CHARGER_TEMP_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_STS_ADDR(base), PMIO_CHARGER_TEMP_STS_RMSK, 0, val)
#define PMIO_CHARGER_TEMP_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_STS_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_STS_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_STS_CHARGER_TOO_HOT_STS_BMSK                               0x20
#define PMIO_CHARGER_TEMP_STS_CHARGER_TOO_HOT_STS_SHFT                                0x5
#define PMIO_CHARGER_TEMP_STS_CHARGER_TOO_HOT_STS_CHARGER_TEMP_BELOW_FVAL             0x0
#define PMIO_CHARGER_TEMP_STS_CHARGER_TOO_HOT_STS_CHARGER_TEMP_ABOVE_FVAL             0x1
#define PMIO_CHARGER_TEMP_STS_CHARGER_HOT_STS_BMSK                                   0x10
#define PMIO_CHARGER_TEMP_STS_CHARGER_HOT_STS_SHFT                                    0x4
#define PMIO_CHARGER_TEMP_STS_CHARGER_HOT_STS_CHARGER_TEMP_BELOW_FVAL                 0x0
#define PMIO_CHARGER_TEMP_STS_CHARGER_HOT_STS_CHARGER_TEMP_ABOVE_FVAL                 0x1
#define PMIO_CHARGER_TEMP_STS_CHANNEL_STS_BMSK                                        0x2
#define PMIO_CHARGER_TEMP_STS_CHANNEL_STS_SHFT                                        0x1
#define PMIO_CHARGER_TEMP_STS_CHANNEL_STS_NOT_READY_FVAL                              0x0
#define PMIO_CHARGER_TEMP_STS_CHANNEL_STS_READY_FVAL                                  0x1
#define PMIO_CHARGER_TEMP_STS_READING_STS_BMSK                                        0x1
#define PMIO_CHARGER_TEMP_STS_READING_STS_SHFT                                        0x0
#define PMIO_CHARGER_TEMP_STS_READING_STS_NOT_AVAILABLE_FVAL                          0x0
#define PMIO_CHARGER_TEMP_STS_READING_STS_AVAILABLE_FVAL                              0x1

#define PMIO_CHARGER_TEMP_CFG_ADDR(x)                                          ((x) + 0x000000bb)
#define PMIO_CHARGER_TEMP_CFG_RMSK                                                   0xfc
#define PMIO_CHARGER_TEMP_CFG_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_CFG_ADDR(base), PMIO_CHARGER_TEMP_CFG_RMSK, 0, val)
#define PMIO_CHARGER_TEMP_CFG_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_CFG_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_CFG_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_CFG_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_CFG_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_CFG_ADDR(base), val)
#define PMIO_CHARGER_TEMP_CFG_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_CFG_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_CFG_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_CHARGER_TEMP_CFG_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BMSK                                         0xe0
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_SHFT                                          0x5
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BIASWAIT_0_MS_FVAL                            0x0
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BIASWAIT_1_MS_FVAL                            0x1
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BIASWAIT_4_MS_FVAL                            0x2
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BIASWAIT_12_MS_FVAL                           0x3
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BIASWAIT_20_MS_FVAL                           0x4
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BIASWAIT_40_MS_FVAL                           0x5
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BIASWAIT_60_MS_FVAL                           0x6
#define PMIO_CHARGER_TEMP_CFG_BIAS_WAIT_BIASWAIT_80_MS_FVAL                           0x7
#define PMIO_CHARGER_TEMP_CFG_CHARGER_TMP_HYST_BMSK                                  0x18
#define PMIO_CHARGER_TEMP_CFG_CHARGER_TMP_HYST_SHFT                                   0x3
#define PMIO_CHARGER_TEMP_CFG_CHARGER_TMP_HYST_NO_HYSTERESIS_FVAL                     0x0
#define PMIO_CHARGER_TEMP_CFG_CHARGER_TMP_HYST_HYST_1_C_FVAL                          0x1
#define PMIO_CHARGER_TEMP_CFG_CHARGER_TMP_HYST_HYST_2_C_FVAL                          0x2
#define PMIO_CHARGER_TEMP_CFG_CHARGER_TMP_HYST_HYST_3_C_FVAL                          0x3
#define PMIO_CHARGER_TEMP_CFG_TEMP_CMP_CLEAR_BMSK                                     0x4
#define PMIO_CHARGER_TEMP_CFG_TEMP_CMP_CLEAR_SHFT                                     0x2
#define PMIO_CHARGER_TEMP_CFG_TEMP_CMP_CLEAR_DO_NOT_CLEAR_FVAL                        0x0
#define PMIO_CHARGER_TEMP_CFG_TEMP_CMP_CLEAR_CLEAR_FVAL                               0x1

#define PMIO_CHARGER_TEMP_LSB_ADDR(x)                                          ((x) + 0x000000bc)
#define PMIO_CHARGER_TEMP_LSB_RMSK                                                   0xff
#define PMIO_CHARGER_TEMP_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_LSB_ADDR(base), PMIO_CHARGER_TEMP_LSB_RMSK, 0, val)
#define PMIO_CHARGER_TEMP_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_LSB_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_LSB_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_LSB_ADC_READ_BMSK                                          0xff
#define PMIO_CHARGER_TEMP_LSB_ADC_READ_SHFT                                           0x0

#define PMIO_CHARGER_TEMP_MSB_ADDR(x)                                          ((x) + 0x000000bd)
#define PMIO_CHARGER_TEMP_MSB_RMSK                                                    0x7
#define PMIO_CHARGER_TEMP_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_MSB_ADDR(base), PMIO_CHARGER_TEMP_MSB_RMSK, 0, val)
#define PMIO_CHARGER_TEMP_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TEMP_MSB_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TEMP_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHARGER_TEMP_MSB_ADDR(base), val, len)
#define PMIO_CHARGER_TEMP_MSB_ADC_READ_BMSK                                           0x7
#define PMIO_CHARGER_TEMP_MSB_ADC_READ_SHFT                                           0x0

#define PMIO_CHARGER_HOT_ADDR(x)                                               ((x) + 0x000000be)
#define PMIO_CHARGER_HOT_RMSK                                                        0xff
#define PMIO_CHARGER_HOT_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_HOT_ADDR(base), PMIO_CHARGER_HOT_RMSK, 0, val)
#define PMIO_CHARGER_HOT_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_HOT_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_HOT_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHARGER_HOT_ADDR(base), val, len)
#define PMIO_CHARGER_HOT_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_CHARGER_HOT_ADDR(base), val)
#define PMIO_CHARGER_HOT_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_CHARGER_HOT_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_HOT_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_CHARGER_HOT_ADDR(base), val, len)
#define PMIO_CHARGER_HOT_THRESHOLD_BMSK                                              0xff
#define PMIO_CHARGER_HOT_THRESHOLD_SHFT                                               0x0

#define PMIO_CHARGER_TOO_HOT_ADDR(x)                                           ((x) + 0x000000bf)
#define PMIO_CHARGER_TOO_HOT_RMSK                                                    0xff
#define PMIO_CHARGER_TOO_HOT_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TOO_HOT_ADDR(base), PMIO_CHARGER_TOO_HOT_RMSK, 0, val)
#define PMIO_CHARGER_TOO_HOT_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_CHARGER_TOO_HOT_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TOO_HOT_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_CHARGER_TOO_HOT_ADDR(base), val, len)
#define PMIO_CHARGER_TOO_HOT_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_CHARGER_TOO_HOT_ADDR(base), val)
#define PMIO_CHARGER_TOO_HOT_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_CHARGER_TOO_HOT_ADDR(base), mask, shift, val)
#define PMIO_CHARGER_TOO_HOT_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_CHARGER_TOO_HOT_ADDR(base), val, len)
#define PMIO_CHARGER_TOO_HOT_THRESHOLD_BMSK                                          0xff
#define PMIO_CHARGER_TOO_HOT_THRESHOLD_SHFT                                           0x0

#define PMIO_GPIO_CTRL_ADDR(x)                                                 ((x) + 0x000000c0)
#define PMIO_GPIO_CTRL_RMSK                                                           0x3
#define PMIO_GPIO_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_CTRL_ADDR(base), PMIO_GPIO_CTRL_RMSK, 0, val)
#define PMIO_GPIO_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_CTRL_ADDR(base), mask, shift, val)
#define PMIO_GPIO_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_GPIO_CTRL_ADDR(base), val, len)
#define PMIO_GPIO_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_GPIO_CTRL_ADDR(base), val)
#define PMIO_GPIO_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_GPIO_CTRL_ADDR(base), mask, shift, val)
#define PMIO_GPIO_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_GPIO_CTRL_ADDR(base), val, len)
#define PMIO_GPIO_CTRL_CHANNEL_IRQ_BMSK                                               0x2
#define PMIO_GPIO_CTRL_CHANNEL_IRQ_SHFT                                               0x1
#define PMIO_GPIO_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                       0x0
#define PMIO_GPIO_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                        0x1
#define PMIO_GPIO_CTRL_CHANNEL_CONV_BMSK                                              0x1
#define PMIO_GPIO_CTRL_CHANNEL_CONV_SHFT                                              0x0
#define PMIO_GPIO_CTRL_CHANNEL_CONV_DISABLE_FVAL                                      0x0
#define PMIO_GPIO_CTRL_CHANNEL_CONV_ENABLE_FVAL                                       0x1

#define PMIO_GPIO_TRIGGER_ADDR(x)                                              ((x) + 0x000000c1)
#define PMIO_GPIO_TRIGGER_RMSK                                                       0xff
#define PMIO_GPIO_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_TRIGGER_ADDR(base), PMIO_GPIO_TRIGGER_RMSK, 0, val)
#define PMIO_GPIO_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_GPIO_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_GPIO_TRIGGER_ADDR(base), val, len)
#define PMIO_GPIO_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_GPIO_TRIGGER_ADDR(base), val)
#define PMIO_GPIO_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_GPIO_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_GPIO_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_GPIO_TRIGGER_ADDR(base), val, len)
#define PMIO_GPIO_TRIGGER_EVERY_CYCLE_BMSK                                           0x80
#define PMIO_GPIO_TRIGGER_EVERY_CYCLE_SHFT                                            0x7
#define PMIO_GPIO_TRIGGER_EVERY_CYCLE_FALSE_FVAL                                      0x0
#define PMIO_GPIO_TRIGGER_EVERY_CYCLE_TRUE_FVAL                                       0x1
#define PMIO_GPIO_TRIGGER_USB_TRIGGER_BMSK                                           0x40
#define PMIO_GPIO_TRIGGER_USB_TRIGGER_SHFT                                            0x6
#define PMIO_GPIO_TRIGGER_USB_TRIGGER_FALSE_FVAL                                      0x0
#define PMIO_GPIO_TRIGGER_USB_TRIGGER_TRUE_FVAL                                       0x1
#define PMIO_GPIO_TRIGGER_DCIN_TRIGGER_BMSK                                          0x20
#define PMIO_GPIO_TRIGGER_DCIN_TRIGGER_SHFT                                           0x5
#define PMIO_GPIO_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                                     0x0
#define PMIO_GPIO_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                                      0x1
#define PMIO_GPIO_TRIGGER_OTG_TRIGGER_BMSK                                           0x10
#define PMIO_GPIO_TRIGGER_OTG_TRIGGER_SHFT                                            0x4
#define PMIO_GPIO_TRIGGER_OTG_TRIGGER_FALSE_FVAL                                      0x0
#define PMIO_GPIO_TRIGGER_OTG_TRIGGER_TRUE_FVAL                                       0x1
#define PMIO_GPIO_TRIGGER_BCL_HPM_TRIGGER_BMSK                                        0x8
#define PMIO_GPIO_TRIGGER_BCL_HPM_TRIGGER_SHFT                                        0x3
#define PMIO_GPIO_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                                  0x0
#define PMIO_GPIO_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                                   0x1
#define PMIO_GPIO_TRIGGER_DTEST_TRIGGER_BMSK                                          0x4
#define PMIO_GPIO_TRIGGER_DTEST_TRIGGER_SHFT                                          0x2
#define PMIO_GPIO_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                                    0x0
#define PMIO_GPIO_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                                     0x1
#define PMIO_GPIO_TRIGGER_RFU_TRIGGER_BMSK                                            0x2
#define PMIO_GPIO_TRIGGER_RFU_TRIGGER_SHFT                                            0x1
#define PMIO_GPIO_TRIGGER_RFU_TRIGGER_FALSE_FVAL                                      0x0
#define PMIO_GPIO_TRIGGER_RFU_TRIGGER_TRUE_FVAL                                       0x1
#define PMIO_GPIO_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                                   0x1
#define PMIO_GPIO_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                                   0x0
#define PMIO_GPIO_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                             0x0
#define PMIO_GPIO_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                              0x1

#define PMIO_GPIO_STS_ADDR(x)                                                  ((x) + 0x000000c2)
#define PMIO_GPIO_STS_RMSK                                                            0x3
#define PMIO_GPIO_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_STS_ADDR(base), PMIO_GPIO_STS_RMSK, 0, val)
#define PMIO_GPIO_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_STS_ADDR(base), mask, shift, val)
#define PMIO_GPIO_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_GPIO_STS_ADDR(base), val, len)
#define PMIO_GPIO_STS_CHANNEL_STS_BMSK                                                0x2
#define PMIO_GPIO_STS_CHANNEL_STS_SHFT                                                0x1
#define PMIO_GPIO_STS_CHANNEL_STS_NOT_READY_FVAL                                      0x0
#define PMIO_GPIO_STS_CHANNEL_STS_READY_FVAL                                          0x1
#define PMIO_GPIO_STS_READING_STS_BMSK                                                0x1
#define PMIO_GPIO_STS_READING_STS_SHFT                                                0x0
#define PMIO_GPIO_STS_READING_STS_NOT_AVAILABLE_FVAL                                  0x0
#define PMIO_GPIO_STS_READING_STS_AVAILABLE_FVAL                                      0x1

#define PMIO_GPIO_LSB_ADDR(x)                                                  ((x) + 0x000000c4)
#define PMIO_GPIO_LSB_RMSK                                                           0xff
#define PMIO_GPIO_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_LSB_ADDR(base), PMIO_GPIO_LSB_RMSK, 0, val)
#define PMIO_GPIO_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_LSB_ADDR(base), mask, shift, val)
#define PMIO_GPIO_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_GPIO_LSB_ADDR(base), val, len)
#define PMIO_GPIO_LSB_ADC_READ_BMSK                                                  0xff
#define PMIO_GPIO_LSB_ADC_READ_SHFT                                                   0x0

#define PMIO_GPIO_MSB_ADDR(x)                                                  ((x) + 0x000000c5)
#define PMIO_GPIO_MSB_RMSK                                                            0x7
#define PMIO_GPIO_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_MSB_ADDR(base), PMIO_GPIO_MSB_RMSK, 0, val)
#define PMIO_GPIO_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_GPIO_MSB_ADDR(base), mask, shift, val)
#define PMIO_GPIO_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_GPIO_MSB_ADDR(base), val, len)
#define PMIO_GPIO_MSB_ADC_READ_BMSK                                                   0x7
#define PMIO_GPIO_MSB_ADC_READ_SHFT                                                   0x0

#define PMIO_ATEST_CTRL_ADDR(x)                                                ((x) + 0x000000c8)
#define PMIO_ATEST_CTRL_RMSK                                                          0x3
#define PMIO_ATEST_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_CTRL_ADDR(base), PMIO_ATEST_CTRL_RMSK, 0, val)
#define PMIO_ATEST_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_CTRL_ADDR(base), mask, shift, val)
#define PMIO_ATEST_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_ATEST_CTRL_ADDR(base), val, len)
#define PMIO_ATEST_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_ATEST_CTRL_ADDR(base), val)
#define PMIO_ATEST_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_ATEST_CTRL_ADDR(base), mask, shift, val)
#define PMIO_ATEST_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_ATEST_CTRL_ADDR(base), val, len)
#define PMIO_ATEST_CTRL_CHANNEL_IRQ_BMSK                                              0x2
#define PMIO_ATEST_CTRL_CHANNEL_IRQ_SHFT                                              0x1
#define PMIO_ATEST_CTRL_CHANNEL_IRQ_DISABLE_FVAL                                      0x0
#define PMIO_ATEST_CTRL_CHANNEL_IRQ_ENABLE_FVAL                                       0x1
#define PMIO_ATEST_CTRL_CHANNEL_CONV_BMSK                                             0x1
#define PMIO_ATEST_CTRL_CHANNEL_CONV_SHFT                                             0x0
#define PMIO_ATEST_CTRL_CHANNEL_CONV_DISABLE_FVAL                                     0x0
#define PMIO_ATEST_CTRL_CHANNEL_CONV_ENABLE_FVAL                                      0x1

#define PMIO_ATEST_TRIGGER_ADDR(x)                                             ((x) + 0x000000c9)
#define PMIO_ATEST_TRIGGER_RMSK                                                      0xff
#define PMIO_ATEST_TRIGGER_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_TRIGGER_ADDR(base), PMIO_ATEST_TRIGGER_RMSK, 0, val)
#define PMIO_ATEST_TRIGGER_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_ATEST_TRIGGER_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_ATEST_TRIGGER_ADDR(base), val, len)
#define PMIO_ATEST_TRIGGER_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_ATEST_TRIGGER_ADDR(base), val)
#define PMIO_ATEST_TRIGGER_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_ATEST_TRIGGER_ADDR(base), mask, shift, val)
#define PMIO_ATEST_TRIGGER_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_ATEST_TRIGGER_ADDR(base), val, len)
#define PMIO_ATEST_TRIGGER_EVERY_CYCLE_BMSK                                          0x80
#define PMIO_ATEST_TRIGGER_EVERY_CYCLE_SHFT                                           0x7
#define PMIO_ATEST_TRIGGER_EVERY_CYCLE_FALSE_FVAL                                     0x0
#define PMIO_ATEST_TRIGGER_EVERY_CYCLE_TRUE_FVAL                                      0x1
#define PMIO_ATEST_TRIGGER_USB_TRIGGER_BMSK                                          0x40
#define PMIO_ATEST_TRIGGER_USB_TRIGGER_SHFT                                           0x6
#define PMIO_ATEST_TRIGGER_USB_TRIGGER_FALSE_FVAL                                     0x0
#define PMIO_ATEST_TRIGGER_USB_TRIGGER_TRUE_FVAL                                      0x1
#define PMIO_ATEST_TRIGGER_DCIN_TRIGGER_BMSK                                         0x20
#define PMIO_ATEST_TRIGGER_DCIN_TRIGGER_SHFT                                          0x5
#define PMIO_ATEST_TRIGGER_DCIN_TRIGGER_FALSE_FVAL                                    0x0
#define PMIO_ATEST_TRIGGER_DCIN_TRIGGER_TRUE_FVAL                                     0x1
#define PMIO_ATEST_TRIGGER_OTG_TRIGGER_BMSK                                          0x10
#define PMIO_ATEST_TRIGGER_OTG_TRIGGER_SHFT                                           0x4
#define PMIO_ATEST_TRIGGER_OTG_TRIGGER_FALSE_FVAL                                     0x0
#define PMIO_ATEST_TRIGGER_OTG_TRIGGER_TRUE_FVAL                                      0x1
#define PMIO_ATEST_TRIGGER_BCL_HPM_TRIGGER_BMSK                                       0x8
#define PMIO_ATEST_TRIGGER_BCL_HPM_TRIGGER_SHFT                                       0x3
#define PMIO_ATEST_TRIGGER_BCL_HPM_TRIGGER_FALSE_FVAL                                 0x0
#define PMIO_ATEST_TRIGGER_BCL_HPM_TRIGGER_TRUE_FVAL                                  0x1
#define PMIO_ATEST_TRIGGER_DTEST_TRIGGER_BMSK                                         0x4
#define PMIO_ATEST_TRIGGER_DTEST_TRIGGER_SHFT                                         0x2
#define PMIO_ATEST_TRIGGER_DTEST_TRIGGER_FALSE_FVAL                                   0x0
#define PMIO_ATEST_TRIGGER_DTEST_TRIGGER_TRUE_FVAL                                    0x1
#define PMIO_ATEST_TRIGGER_RFU_TRIGGER_BMSK                                           0x2
#define PMIO_ATEST_TRIGGER_RFU_TRIGGER_SHFT                                           0x1
#define PMIO_ATEST_TRIGGER_RFU_TRIGGER_FALSE_FVAL                                     0x0
#define PMIO_ATEST_TRIGGER_RFU_TRIGGER_TRUE_FVAL                                      0x1
#define PMIO_ATEST_TRIGGER_SLEEP_CONV_QUALIFIER_BMSK                                  0x1
#define PMIO_ATEST_TRIGGER_SLEEP_CONV_QUALIFIER_SHFT                                  0x0
#define PMIO_ATEST_TRIGGER_SLEEP_CONV_QUALIFIER_FALSE_FVAL                            0x0
#define PMIO_ATEST_TRIGGER_SLEEP_CONV_QUALIFIER_TRUE_FVAL                             0x1

#define PMIO_ATEST_STS_ADDR(x)                                                 ((x) + 0x000000ca)
#define PMIO_ATEST_STS_RMSK                                                           0x3
#define PMIO_ATEST_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_STS_ADDR(base), PMIO_ATEST_STS_RMSK, 0, val)
#define PMIO_ATEST_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_STS_ADDR(base), mask, shift, val)
#define PMIO_ATEST_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_ATEST_STS_ADDR(base), val, len)
#define PMIO_ATEST_STS_CHANNEL_STS_BMSK                                               0x2
#define PMIO_ATEST_STS_CHANNEL_STS_SHFT                                               0x1
#define PMIO_ATEST_STS_CHANNEL_STS_NOT_READY_FVAL                                     0x0
#define PMIO_ATEST_STS_CHANNEL_STS_READY_FVAL                                         0x1
#define PMIO_ATEST_STS_READING_STS_BMSK                                               0x1
#define PMIO_ATEST_STS_READING_STS_SHFT                                               0x0
#define PMIO_ATEST_STS_READING_STS_NOT_AVAILABLE_FVAL                                 0x0
#define PMIO_ATEST_STS_READING_STS_AVAILABLE_FVAL                                     0x1

#define PMIO_ATEST_LSB_ADDR(x)                                                 ((x) + 0x000000cc)
#define PMIO_ATEST_LSB_RMSK                                                          0xff
#define PMIO_ATEST_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_LSB_ADDR(base), PMIO_ATEST_LSB_RMSK, 0, val)
#define PMIO_ATEST_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_LSB_ADDR(base), mask, shift, val)
#define PMIO_ATEST_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_ATEST_LSB_ADDR(base), val, len)
#define PMIO_ATEST_LSB_ADC_READ_BMSK                                                 0xff
#define PMIO_ATEST_LSB_ADC_READ_SHFT                                                  0x0

#define PMIO_ATEST_MSB_ADDR(x)                                                 ((x) + 0x000000cd)
#define PMIO_ATEST_MSB_RMSK                                                           0x7
#define PMIO_ATEST_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_MSB_ADDR(base), PMIO_ATEST_MSB_RMSK, 0, val)
#define PMIO_ATEST_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_ATEST_MSB_ADDR(base), mask, shift, val)
#define PMIO_ATEST_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_ATEST_MSB_ADDR(base), val, len)
#define PMIO_ATEST_MSB_ADC_READ_BMSK                                                  0x7
#define PMIO_ATEST_MSB_ADC_READ_SHFT                                                  0x0

#define PMIO_SEC_ACCESS_ADDR(x)                                                ((x) + 0x000000d0)
#define PMIO_SEC_ACCESS_RMSK                                                         0xff
#define PMIO_SEC_ACCESS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_SEC_ACCESS_ADDR(base), PMIO_SEC_ACCESS_RMSK, 0, val)
#define PMIO_SEC_ACCESS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_SEC_ACCESS_ADDR(base), mask, shift, val)
#define PMIO_SEC_ACCESS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_SEC_ACCESS_ADDR(base), val, len)
#define PMIO_SEC_ACCESS_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_SEC_ACCESS_ADDR(base), val)
#define PMIO_SEC_ACCESS_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_SEC_ACCESS_ADDR(base), mask, shift, val)
#define PMIO_SEC_ACCESS_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_SEC_ACCESS_ADDR(base), val, len)
#define PMIO_SEC_ACCESS_SEC_UNLOCK_BMSK                                              0xff
#define PMIO_SEC_ACCESS_SEC_UNLOCK_SHFT                                               0x0

#define PMIO_PERPH_RESET_CTL2_ADDR(x)                                          ((x) + 0x000000d9)
#define PMIO_PERPH_RESET_CTL2_RMSK                                                    0x1
#define PMIO_PERPH_RESET_CTL2_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL2_ADDR(base), PMIO_PERPH_RESET_CTL2_RMSK, 0, val)
#define PMIO_PERPH_RESET_CTL2_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL2_ADDR(base), mask, shift, val)
#define PMIO_PERPH_RESET_CTL2_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PERPH_RESET_CTL2_ADDR(base), val, len)
#define PMIO_PERPH_RESET_CTL2_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_PERPH_RESET_CTL2_ADDR(base), val)
#define PMIO_PERPH_RESET_CTL2_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL2_ADDR(base), mask, shift, val)
#define PMIO_PERPH_RESET_CTL2_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_PERPH_RESET_CTL2_ADDR(base), val, len)
#define PMIO_PERPH_RESET_CTL2_FOLLOW_GLOBAL_SOFT_RB_BMSK                              0x1
#define PMIO_PERPH_RESET_CTL2_FOLLOW_GLOBAL_SOFT_RB_SHFT                              0x0

#define PMIO_PERPH_RESET_CTL3_ADDR(x)                                          ((x) + 0x000000da)
#define PMIO_PERPH_RESET_CTL3_RMSK                                                    0xf
#define PMIO_PERPH_RESET_CTL3_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL3_ADDR(base), PMIO_PERPH_RESET_CTL3_RMSK, 0, val)
#define PMIO_PERPH_RESET_CTL3_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL3_ADDR(base), mask, shift, val)
#define PMIO_PERPH_RESET_CTL3_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PERPH_RESET_CTL3_ADDR(base), val, len)
#define PMIO_PERPH_RESET_CTL3_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_PERPH_RESET_CTL3_ADDR(base), val)
#define PMIO_PERPH_RESET_CTL3_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL3_ADDR(base), mask, shift, val)
#define PMIO_PERPH_RESET_CTL3_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_PERPH_RESET_CTL3_ADDR(base), val, len)
#define PMIO_PERPH_RESET_CTL3_FOLLOW_OTST2_RB_BMSK                                    0x8
#define PMIO_PERPH_RESET_CTL3_FOLLOW_OTST2_RB_SHFT                                    0x3
#define PMIO_PERPH_RESET_CTL3_FOLLOW_WARM_RB_BMSK                                     0x4
#define PMIO_PERPH_RESET_CTL3_FOLLOW_WARM_RB_SHFT                                     0x2
#define PMIO_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN1_RB_BMSK                                0x2
#define PMIO_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN1_RB_SHFT                                0x1
#define PMIO_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN2_RB_BMSK                                0x1
#define PMIO_PERPH_RESET_CTL3_FOLLOW_SHUTDOWN2_RB_SHFT                                0x0

#define PMIO_PERPH_RESET_CTL4_ADDR(x)                                          ((x) + 0x000000db)
#define PMIO_PERPH_RESET_CTL4_RMSK                                                    0x1
#define PMIO_PERPH_RESET_CTL4_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL4_ADDR(base), PMIO_PERPH_RESET_CTL4_RMSK, 0, val)
#define PMIO_PERPH_RESET_CTL4_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL4_ADDR(base), mask, shift, val)
#define PMIO_PERPH_RESET_CTL4_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_PERPH_RESET_CTL4_ADDR(base), val, len)
#define PMIO_PERPH_RESET_CTL4_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_PERPH_RESET_CTL4_ADDR(base), val)
#define PMIO_PERPH_RESET_CTL4_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_PERPH_RESET_CTL4_ADDR(base), mask, shift, val)
#define PMIO_PERPH_RESET_CTL4_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_PERPH_RESET_CTL4_ADDR(base), val, len)
#define PMIO_PERPH_RESET_CTL4_LOCAL_SOFT_RESET_BMSK                                   0x1
#define PMIO_PERPH_RESET_CTL4_LOCAL_SOFT_RESET_SHFT                                   0x0
#define PMIO_PERPH_RESET_CTL4_LOCAL_SOFT_RESET_NORMAL_OPERATION_FVAL                  0x0
#define PMIO_PERPH_RESET_CTL4_LOCAL_SOFT_RESET_SOFT_RESET_ASSERTED_FVAL               0x1

#define PMIO_INT_TEST1_ADDR(x)                                                 ((x) + 0x000000e0)
#define PMIO_INT_TEST1_RMSK                                                          0x80
#define PMIO_INT_TEST1_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_TEST1_ADDR(base), PMIO_INT_TEST1_RMSK, 0, val)
#define PMIO_INT_TEST1_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_TEST1_ADDR(base), mask, shift, val)
#define PMIO_INT_TEST1_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_TEST1_ADDR(base), val, len)
#define PMIO_INT_TEST1_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_TEST1_ADDR(base), val)
#define PMIO_INT_TEST1_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_TEST1_ADDR(base), mask, shift, val)
#define PMIO_INT_TEST1_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_TEST1_ADDR(base), val, len)
#define PMIO_INT_TEST1_INT_TEST_MODE_EN_BMSK                                         0x80
#define PMIO_INT_TEST1_INT_TEST_MODE_EN_SHFT                                          0x7
#define PMIO_INT_TEST1_INT_TEST_MODE_EN_INT_TEST_MODE_DISABLED_FVAL                   0x0
#define PMIO_INT_TEST1_INT_TEST_MODE_EN_INT_TEST_MODE_ENABLED_FVAL                    0x1

#define PMIO_INT_TEST_VAL_ADDR(x)                                              ((x) + 0x000000e1)
#define PMIO_INT_TEST_VAL_RMSK                                                        0xf
#define PMIO_INT_TEST_VAL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_TEST_VAL_ADDR(base), PMIO_INT_TEST_VAL_RMSK, 0, val)
#define PMIO_INT_TEST_VAL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_INT_TEST_VAL_ADDR(base), mask, shift, val)
#define PMIO_INT_TEST_VAL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_INT_TEST_VAL_ADDR(base), val, len)
#define PMIO_INT_TEST_VAL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_INT_TEST_VAL_ADDR(base), val)
#define PMIO_INT_TEST_VAL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_INT_TEST_VAL_ADDR(base), mask, shift, val)
#define PMIO_INT_TEST_VAL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_INT_TEST_VAL_ADDR(base), val, len)
#define PMIO_INT_TEST_VAL_RR_SPARE_BMSK                                               0x8
#define PMIO_INT_TEST_VAL_RR_SPARE_SHFT                                               0x3
#define PMIO_INT_TEST_VAL_RR_SPARE_TEST_VAL_0_FVAL                                    0x0
#define PMIO_INT_TEST_VAL_RR_SPARE_TEST_VAL_1_FVAL                                    0x1
#define PMIO_INT_TEST_VAL_RR_BUS_ERR_BMSK                                             0x4
#define PMIO_INT_TEST_VAL_RR_BUS_ERR_SHFT                                             0x2
#define PMIO_INT_TEST_VAL_RR_BUS_ERR_TEST_VAL_0_FVAL                                  0x0
#define PMIO_INT_TEST_VAL_RR_BUS_ERR_TEST_VAL_1_FVAL                                  0x1
#define PMIO_INT_TEST_VAL_RR_BMSK                                                     0x2
#define PMIO_INT_TEST_VAL_RR_SHFT                                                     0x1
#define PMIO_INT_TEST_VAL_RR_TEST_VAL_0_FVAL                                          0x0
#define PMIO_INT_TEST_VAL_RR_TEST_VAL_1_FVAL                                          0x1
#define PMIO_INT_TEST_VAL_BT_ID_BMSK                                                  0x1
#define PMIO_INT_TEST_VAL_BT_ID_SHFT                                                  0x0
#define PMIO_INT_TEST_VAL_BT_ID_TEST_VAL_0_FVAL                                       0x0
#define PMIO_INT_TEST_VAL_BT_ID_TEST_VAL_1_FVAL                                       0x1

#define PMIO_TM_TRIGGER_CTRLS_ADDR(x)                                          ((x) + 0x000000e2)
#define PMIO_TM_TRIGGER_CTRLS_RMSK                                                   0x1f
#define PMIO_TM_TRIGGER_CTRLS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_TRIGGER_CTRLS_ADDR(base), PMIO_TM_TRIGGER_CTRLS_RMSK, 0, val)
#define PMIO_TM_TRIGGER_CTRLS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_TRIGGER_CTRLS_ADDR(base), mask, shift, val)
#define PMIO_TM_TRIGGER_CTRLS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_TRIGGER_CTRLS_ADDR(base), val, len)
#define PMIO_TM_TRIGGER_CTRLS_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_TRIGGER_CTRLS_ADDR(base), val)
#define PMIO_TM_TRIGGER_CTRLS_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_TRIGGER_CTRLS_ADDR(base), mask, shift, val)
#define PMIO_TM_TRIGGER_CTRLS_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_TRIGGER_CTRLS_ADDR(base), val, len)
#define PMIO_TM_TRIGGER_CTRLS_RR_CONV_TRIGGER_BMSK                                   0x10
#define PMIO_TM_TRIGGER_CTRLS_RR_CONV_TRIGGER_SHFT                                    0x4
#define PMIO_TM_TRIGGER_CTRLS_RR_CONV_TRIGGER_DISABLE_FVAL                            0x0
#define PMIO_TM_TRIGGER_CTRLS_RR_CONV_TRIGGER_ENABLE_FVAL                             0x1
#define PMIO_TM_TRIGGER_CTRLS_RR_CONV_CTRL_BMSK                                       0x8
#define PMIO_TM_TRIGGER_CTRLS_RR_CONV_CTRL_SHFT                                       0x3
#define PMIO_TM_TRIGGER_CTRLS_RR_CONV_CTRL_LOGIC_FVAL                                 0x0
#define PMIO_TM_TRIGGER_CTRLS_RR_CONV_CTRL_REGISTER_FVAL                              0x1
#define PMIO_TM_TRIGGER_CTRLS_FORCE_TRIGGER_BMSK                                      0x4
#define PMIO_TM_TRIGGER_CTRLS_FORCE_TRIGGER_SHFT                                      0x2
#define PMIO_TM_TRIGGER_CTRLS_FORCE_TRIGGER_DISABLE_FVAL                              0x0
#define PMIO_TM_TRIGGER_CTRLS_FORCE_TRIGGER_ENABLE_FVAL                               0x1
#define PMIO_TM_TRIGGER_CTRLS_RR_GOC_BYPASS_BMSK                                      0x3
#define PMIO_TM_TRIGGER_CTRLS_RR_GOC_BYPASS_SHFT                                      0x0
#define PMIO_TM_TRIGGER_CTRLS_RR_GOC_BYPASS_DISABLE_FVAL                              0x0
#define PMIO_TM_TRIGGER_CTRLS_RR_GOC_BYPASS_ENABLE_FVAL                               0x1

#define PMIO_TM_ADC_CTRLS_ADDR(x)                                              ((x) + 0x000000e3)
#define PMIO_TM_ADC_CTRLS_RMSK                                                        0x3
#define PMIO_TM_ADC_CTRLS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ADC_CTRLS_ADDR(base), PMIO_TM_ADC_CTRLS_RMSK, 0, val)
#define PMIO_TM_ADC_CTRLS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ADC_CTRLS_ADDR(base), mask, shift, val)
#define PMIO_TM_ADC_CTRLS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_ADC_CTRLS_ADDR(base), val, len)
#define PMIO_TM_ADC_CTRLS_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_ADC_CTRLS_ADDR(base), val)
#define PMIO_TM_ADC_CTRLS_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_ADC_CTRLS_ADDR(base), mask, shift, val)
#define PMIO_TM_ADC_CTRLS_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_ADC_CTRLS_ADDR(base), val, len)
#define PMIO_TM_ADC_CTRLS_ADC_CONVERSION_BMSK                                         0x2
#define PMIO_TM_ADC_CTRLS_ADC_CONVERSION_SHFT                                         0x1
#define PMIO_TM_ADC_CTRLS_ADC_CONVERSION_DISABLE_FVAL                                 0x0
#define PMIO_TM_ADC_CTRLS_ADC_CONVERSION_ENABLE_FVAL                                  0x1
#define PMIO_TM_ADC_CTRLS_RR_ADC_CTRL_BMSK                                            0x1
#define PMIO_TM_ADC_CTRLS_RR_ADC_CTRL_SHFT                                            0x0
#define PMIO_TM_ADC_CTRLS_RR_ADC_CTRL_DISABLE_FVAL                                    0x0
#define PMIO_TM_ADC_CTRLS_RR_ADC_CTRL_ENABLE_FVAL                                     0x1

#define PMIO_TM_CNL_CTRL_ADDR(x)                                               ((x) + 0x000000e4)
#define PMIO_TM_CNL_CTRL_RMSK                                                         0xf
#define PMIO_TM_CNL_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_CNL_CTRL_ADDR(base), PMIO_TM_CNL_CTRL_RMSK, 0, val)
#define PMIO_TM_CNL_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_CNL_CTRL_ADDR(base), mask, shift, val)
#define PMIO_TM_CNL_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_CNL_CTRL_ADDR(base), val, len)
#define PMIO_TM_CNL_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_CNL_CTRL_ADDR(base), val)
#define PMIO_TM_CNL_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_CNL_CTRL_ADDR(base), mask, shift, val)
#define PMIO_TM_CNL_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_CNL_CTRL_ADDR(base), val, len)
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_BMSK                                             0xf
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_SHFT                                             0x0
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_NONE_FVAL                                        0x0
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_BTID_FVAL                                        0x1
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_BTTHRM_FVAL                                      0x2
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_SKNTHRM_FVAL                                     0x3
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_DIETMPR_FVAL                                     0x4
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_CRGDIETMPR_FVAL                                  0x5
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_USBINI_FVAL                                      0x6
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_USBINV_FVAL                                      0x7
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_DCINI_FVAL                                       0x8
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_DCINV_FVAL                                       0x9
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_MPP_FVAL                                         0xa
#define PMIO_TM_CNL_CTRL_RR_ADC_CTRL_ATST_FVAL                                        0xb

#define PMIO_TM_BATT_ID_CTRL_ADDR(x)                                           ((x) + 0x000000e5)
#define PMIO_TM_BATT_ID_CTRL_RMSK                                                     0x3
#define PMIO_TM_BATT_ID_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_BATT_ID_CTRL_ADDR(base), PMIO_TM_BATT_ID_CTRL_RMSK, 0, val)
#define PMIO_TM_BATT_ID_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_BATT_ID_CTRL_ADDR(base), mask, shift, val)
#define PMIO_TM_BATT_ID_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_BATT_ID_CTRL_ADDR(base), val, len)
#define PMIO_TM_BATT_ID_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_BATT_ID_CTRL_ADDR(base), val)
#define PMIO_TM_BATT_ID_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_BATT_ID_CTRL_ADDR(base), mask, shift, val)
#define PMIO_TM_BATT_ID_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_BATT_ID_CTRL_ADDR(base), val, len)
#define PMIO_TM_BATT_ID_CTRL_FORCE_BATT_ID_BIAS_BMSK                                  0x3
#define PMIO_TM_BATT_ID_CTRL_FORCE_BATT_ID_BIAS_SHFT                                  0x0
#define PMIO_TM_BATT_ID_CTRL_FORCE_BATT_ID_BIAS_BIASNONE_FVAL                         0x0
#define PMIO_TM_BATT_ID_CTRL_FORCE_BATT_ID_BIAS_BIAS5UA_FVAL                          0x1
#define PMIO_TM_BATT_ID_CTRL_FORCE_BATT_ID_BIAS_BIAS15UA_FVAL                         0x2
#define PMIO_TM_BATT_ID_CTRL_FORCE_BATT_ID_BIAS_BIAS150UA_FVAL                        0x3

#define PMIO_TM_THERM_CTRL_ADDR(x)                                             ((x) + 0x000000e6)
#define PMIO_TM_THERM_CTRL_RMSK                                                       0xf
#define PMIO_TM_THERM_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_THERM_CTRL_ADDR(base), PMIO_TM_THERM_CTRL_RMSK, 0, val)
#define PMIO_TM_THERM_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_THERM_CTRL_ADDR(base), mask, shift, val)
#define PMIO_TM_THERM_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_THERM_CTRL_ADDR(base), val, len)
#define PMIO_TM_THERM_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_THERM_CTRL_ADDR(base), val)
#define PMIO_TM_THERM_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_THERM_CTRL_ADDR(base), mask, shift, val)
#define PMIO_TM_THERM_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_THERM_CTRL_ADDR(base), val, len)
#define PMIO_TM_THERM_CTRL_AUX_RBIAS_CTRL_OFF_BMSK                                    0x8
#define PMIO_TM_THERM_CTRL_AUX_RBIAS_CTRL_OFF_SHFT                                    0x3
#define PMIO_TM_THERM_CTRL_AUX_RBIAS_CTRL_OFF_BIAS_VOLTAGE_ENABLED_FVAL               0x0
#define PMIO_TM_THERM_CTRL_AUX_RBIAS_CTRL_OFF_BIAS_VOLTAFE_DISABLED_FVAL              0x1
#define PMIO_TM_THERM_CTRL_BATT_RBIAS_CTRL_OFF_BMSK                                   0x4
#define PMIO_TM_THERM_CTRL_BATT_RBIAS_CTRL_OFF_SHFT                                   0x2
#define PMIO_TM_THERM_CTRL_BATT_RBIAS_CTRL_OFF_BIAS_VOLTAGE_ENABLED_FVAL              0x0
#define PMIO_TM_THERM_CTRL_BATT_RBIAS_CTRL_OFF_BIAS_VOLTAFE_DISABLED_FVAL             0x1
#define PMIO_TM_THERM_CTRL_AUX_THERM_RBIAS_CTRL_BMSK                                  0x2
#define PMIO_TM_THERM_CTRL_AUX_THERM_RBIAS_CTRL_SHFT                                  0x1
#define PMIO_TM_THERM_CTRL_AUX_THERM_RBIAS_CTRL_RBIAS_UNFORCE_FVAL                    0x0
#define PMIO_TM_THERM_CTRL_AUX_THERM_RBIAS_CTRL_RBIAS_FORCED_ON_FVAL                  0x1
#define PMIO_TM_THERM_CTRL_BATT_THERM_RBIAS_CTRL_BMSK                                 0x1
#define PMIO_TM_THERM_CTRL_BATT_THERM_RBIAS_CTRL_SHFT                                 0x0
#define PMIO_TM_THERM_CTRL_BATT_THERM_RBIAS_CTRL_RBIAS_UNFORCE_FVAL                   0x0
#define PMIO_TM_THERM_CTRL_BATT_THERM_RBIAS_CTRL_RBIAS_FORCED_ON_FVAL                 0x1

#define PMIO_TM_CONV_STS_ADDR(x)                                               ((x) + 0x000000e7)
#define PMIO_TM_CONV_STS_RMSK                                                         0x1
#define PMIO_TM_CONV_STS_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_CONV_STS_ADDR(base), PMIO_TM_CONV_STS_RMSK, 0, val)
#define PMIO_TM_CONV_STS_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_CONV_STS_ADDR(base), mask, shift, val)
#define PMIO_TM_CONV_STS_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_CONV_STS_ADDR(base), val, len)
#define PMIO_TM_CONV_STS_ADC_CONV_STS_BMSK                                            0x1
#define PMIO_TM_CONV_STS_ADC_CONV_STS_SHFT                                            0x0
#define PMIO_TM_CONV_STS_ADC_CONV_STS_ADC_DONE_FVAL                                   0x0
#define PMIO_TM_CONV_STS_ADC_CONV_STS_ADC_NOT_DONE_FVAL                               0x1

#define PMIO_TM_ADC_READ_LSB_ADDR(x)                                           ((x) + 0x000000e8)
#define PMIO_TM_ADC_READ_LSB_RMSK                                                    0xff
#define PMIO_TM_ADC_READ_LSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ADC_READ_LSB_ADDR(base), PMIO_TM_ADC_READ_LSB_RMSK, 0, val)
#define PMIO_TM_ADC_READ_LSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ADC_READ_LSB_ADDR(base), mask, shift, val)
#define PMIO_TM_ADC_READ_LSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_ADC_READ_LSB_ADDR(base), val, len)
#define PMIO_TM_ADC_READ_LSB_VALUE_BMSK                                              0xff
#define PMIO_TM_ADC_READ_LSB_VALUE_SHFT                                               0x0

#define PMIO_TM_ADC_READ_MSB_ADDR(x)                                           ((x) + 0x000000e9)
#define PMIO_TM_ADC_READ_MSB_RMSK                                                     0x7
#define PMIO_TM_ADC_READ_MSB_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ADC_READ_MSB_ADDR(base), PMIO_TM_ADC_READ_MSB_RMSK, 0, val)
#define PMIO_TM_ADC_READ_MSB_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ADC_READ_MSB_ADDR(base), mask, shift, val)
#define PMIO_TM_ADC_READ_MSB_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_ADC_READ_MSB_ADDR(base), val, len)
#define PMIO_TM_ADC_READ_MSB_VALUE_BMSK                                               0x7
#define PMIO_TM_ADC_READ_MSB_VALUE_SHFT                                               0x0

#define PMIO_TM_ATEST_MUX_1_ADDR(x)                                            ((x) + 0x000000ea)
#define PMIO_TM_ATEST_MUX_1_RMSK                                                     0x1f
#define PMIO_TM_ATEST_MUX_1_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ATEST_MUX_1_ADDR(base), PMIO_TM_ATEST_MUX_1_RMSK, 0, val)
#define PMIO_TM_ATEST_MUX_1_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ATEST_MUX_1_ADDR(base), mask, shift, val)
#define PMIO_TM_ATEST_MUX_1_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_ATEST_MUX_1_ADDR(base), val, len)
#define PMIO_TM_ATEST_MUX_1_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_ATEST_MUX_1_ADDR(base), val)
#define PMIO_TM_ATEST_MUX_1_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_ATEST_MUX_1_ADDR(base), mask, shift, val)
#define PMIO_TM_ATEST_MUX_1_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_ATEST_MUX_1_ADDR(base), val, len)
#define PMIO_TM_ATEST_MUX_1_MUX_EN_CTRL_BMSK                                         0x10
#define PMIO_TM_ATEST_MUX_1_MUX_EN_CTRL_SHFT                                          0x4
#define PMIO_TM_ATEST_MUX_1_MUX_EN_CTRL_DISABLE_FVAL                                  0x0
#define PMIO_TM_ATEST_MUX_1_MUX_EN_CTRL_ENABLE_FVAL                                   0x1
#define PMIO_TM_ATEST_MUX_1_MUX_SEL_BMSK                                              0xf
#define PMIO_TM_ATEST_MUX_1_MUX_SEL_SHFT                                              0x0

#define PMIO_TM_ATEST_MUX_2_ADDR(x)                                            ((x) + 0x000000eb)
#define PMIO_TM_ATEST_MUX_2_RMSK                                                     0x1f
#define PMIO_TM_ATEST_MUX_2_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ATEST_MUX_2_ADDR(base), PMIO_TM_ATEST_MUX_2_RMSK, 0, val)
#define PMIO_TM_ATEST_MUX_2_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_ATEST_MUX_2_ADDR(base), mask, shift, val)
#define PMIO_TM_ATEST_MUX_2_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_ATEST_MUX_2_ADDR(base), val, len)
#define PMIO_TM_ATEST_MUX_2_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_ATEST_MUX_2_ADDR(base), val)
#define PMIO_TM_ATEST_MUX_2_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_ATEST_MUX_2_ADDR(base), mask, shift, val)
#define PMIO_TM_ATEST_MUX_2_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_ATEST_MUX_2_ADDR(base), val, len)
#define PMIO_TM_ATEST_MUX_2_MUX_EN_CTRL_BMSK                                         0x10
#define PMIO_TM_ATEST_MUX_2_MUX_EN_CTRL_SHFT                                          0x4
#define PMIO_TM_ATEST_MUX_2_MUX_EN_CTRL_DISABLE_FVAL                                  0x0
#define PMIO_TM_ATEST_MUX_2_MUX_EN_CTRL_ENABLE_FVAL                                   0x1
#define PMIO_TM_ATEST_MUX_2_MUX_SEL_BMSK                                              0xf
#define PMIO_TM_ATEST_MUX_2_MUX_SEL_SHFT                                              0x0

#define PMIO_TM_REFERENCES_ADDR(x)                                             ((x) + 0x000000ed)
#define PMIO_TM_REFERENCES_RMSK                                                      0x3f
#define PMIO_TM_REFERENCES_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_REFERENCES_ADDR(base), PMIO_TM_REFERENCES_RMSK, 0, val)
#define PMIO_TM_REFERENCES_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_REFERENCES_ADDR(base), mask, shift, val)
#define PMIO_TM_REFERENCES_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_REFERENCES_ADDR(base), val, len)
#define PMIO_TM_REFERENCES_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_REFERENCES_ADDR(base), val)
#define PMIO_TM_REFERENCES_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_REFERENCES_ADDR(base), mask, shift, val)
#define PMIO_TM_REFERENCES_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_REFERENCES_ADDR(base), val, len)
#define PMIO_TM_REFERENCES_RR_REF_BUF_FORCE_CTRL_BMSK                                0x20
#define PMIO_TM_REFERENCES_RR_REF_BUF_FORCE_CTRL_SHFT                                 0x5
#define PMIO_TM_REFERENCES_RR_REF_BUF_FORCE_CTRL_DO_NOT_FORCE_FVAL                    0x0
#define PMIO_TM_REFERENCES_RR_REF_BUF_FORCE_CTRL_FORCE_ON_FVAL                        0x1
#define PMIO_TM_REFERENCES_RR_REF_BUF_REG_CTRL_BMSK                                  0x10
#define PMIO_TM_REFERENCES_RR_REF_BUF_REG_CTRL_SHFT                                   0x4
#define PMIO_TM_REFERENCES_RR_REF_BUF_REG_CTRL_DISABLE_FVAL                           0x0
#define PMIO_TM_REFERENCES_RR_REF_BUF_REG_CTRL_ENABLE_FVAL                            0x1
#define PMIO_TM_REFERENCES_BG_BUFFER_CTRL_BMSK                                        0x8
#define PMIO_TM_REFERENCES_BG_BUFFER_CTRL_SHFT                                        0x3
#define PMIO_TM_REFERENCES_BG_BUFFER_CTRL_DO_NOT_FORCE_FVAL                           0x0
#define PMIO_TM_REFERENCES_BG_BUFFER_CTRL_FORCE_ON_FVAL                               0x1
#define PMIO_TM_REFERENCES_BG_CTRL_BMSK                                               0x4
#define PMIO_TM_REFERENCES_BG_CTRL_SHFT                                               0x2
#define PMIO_TM_REFERENCES_BG_CTRL_DO_NOT_FORCE_FVAL                                  0x0
#define PMIO_TM_REFERENCES_BG_CTRL_FORCE_ON_FVAL                                      0x1
#define PMIO_TM_REFERENCES_LDO_CONTROL_CTRL_BMSK                                      0x2
#define PMIO_TM_REFERENCES_LDO_CONTROL_CTRL_SHFT                                      0x1
#define PMIO_TM_REFERENCES_LDO_CONTROL_CTRL_DO_NOT_FORCE_FVAL                         0x0
#define PMIO_TM_REFERENCES_LDO_CONTROL_CTRL_FORCE_ON_FVAL                             0x1
#define PMIO_TM_REFERENCES_REF_LOGIC_REG_CTRL_BMSK                                    0x1
#define PMIO_TM_REFERENCES_REF_LOGIC_REG_CTRL_SHFT                                    0x0
#define PMIO_TM_REFERENCES_REF_LOGIC_REG_CTRL_DISABLE_FVAL                            0x0
#define PMIO_TM_REFERENCES_REF_LOGIC_REG_CTRL_ENABLE_FVAL                             0x1

#define PMIO_TM_MISC_CTL_ADDR(x)                                               ((x) + 0x000000ee)
#define PMIO_TM_MISC_CTL_RMSK                                                         0x3
#define PMIO_TM_MISC_CTL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_MISC_CTL_ADDR(base), PMIO_TM_MISC_CTL_RMSK, 0, val)
#define PMIO_TM_MISC_CTL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_MISC_CTL_ADDR(base), mask, shift, val)
#define PMIO_TM_MISC_CTL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_MISC_CTL_ADDR(base), val, len)
#define PMIO_TM_MISC_CTL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_MISC_CTL_ADDR(base), val)
#define PMIO_TM_MISC_CTL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_MISC_CTL_ADDR(base), mask, shift, val)
#define PMIO_TM_MISC_CTL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_MISC_CTL_ADDR(base), val, len)
#define PMIO_TM_MISC_CTL_FORCE_CHARGER_TEMP_REQ_BMSK                                  0x2
#define PMIO_TM_MISC_CTL_FORCE_CHARGER_TEMP_REQ_SHFT                                  0x1
#define PMIO_TM_MISC_CTL_FORCE_CHARGER_TEMP_REQ_NORMAL_FVAL                           0x0
#define PMIO_TM_MISC_CTL_FORCE_CHARGER_TEMP_REQ_FORCE_FVAL                            0x1
#define PMIO_TM_MISC_CTL_FORCE_PMI_DIE_TEMP_REQ_BMSK                                  0x1
#define PMIO_TM_MISC_CTL_FORCE_PMI_DIE_TEMP_REQ_SHFT                                  0x0
#define PMIO_TM_MISC_CTL_FORCE_PMI_DIE_TEMP_REQ_NORMAL_FVAL                           0x0
#define PMIO_TM_MISC_CTL_FORCE_PMI_DIE_TEMP_REQ_FORCE_FVAL                            0x1

#define PMIO_TM_RR_CTRL_ADDR(x)                                                ((x) + 0x000000ef)
#define PMIO_TM_RR_CTRL_RMSK                                                         0xff
#define PMIO_TM_RR_CTRL_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_RR_CTRL_ADDR(base), PMIO_TM_RR_CTRL_RMSK, 0, val)
#define PMIO_TM_RR_CTRL_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TM_RR_CTRL_ADDR(base), mask, shift, val)
#define PMIO_TM_RR_CTRL_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TM_RR_CTRL_ADDR(base), val, len)
#define PMIO_TM_RR_CTRL_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TM_RR_CTRL_ADDR(base), val)
#define PMIO_TM_RR_CTRL_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TM_RR_CTRL_ADDR(base), mask, shift, val)
#define PMIO_TM_RR_CTRL_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TM_RR_CTRL_ADDR(base), val, len)
#define PMIO_TM_RR_CTRL_DC_IN_CONNECTED_BMSK                                         0x80
#define PMIO_TM_RR_CTRL_DC_IN_CONNECTED_SHFT                                          0x7
#define PMIO_TM_RR_CTRL_DC_IN_CONNECTED_DISABLE_FVAL                                  0x0
#define PMIO_TM_RR_CTRL_DC_IN_CONNECTED_ENABLE_FVAL                                   0x1
#define PMIO_TM_RR_CTRL_USB_IN_CONNECTED_BMSK                                        0x40
#define PMIO_TM_RR_CTRL_USB_IN_CONNECTED_SHFT                                         0x6
#define PMIO_TM_RR_CTRL_USB_IN_CONNECTED_DISABLE_FVAL                                 0x0
#define PMIO_TM_RR_CTRL_USB_IN_CONNECTED_ENABLE_FVAL                                  0x1
#define PMIO_TM_RR_CTRL_SWITCHER_USE_DCIN_FORCE_CTRL_BMSK                            0x20
#define PMIO_TM_RR_CTRL_SWITCHER_USE_DCIN_FORCE_CTRL_SHFT                             0x5
#define PMIO_TM_RR_CTRL_SWITCHER_USE_DCIN_FORCE_CTRL_DCIN_NOT_IN_USE_FVAL             0x0
#define PMIO_TM_RR_CTRL_SWITCHER_USE_DCIN_FORCE_CTRL_DCIN_IN_USE_FVAL                 0x1
#define PMIO_TM_RR_CTRL_SWITCHER_USE_USBIN_FORCE_CTRL_BMSK                           0x10
#define PMIO_TM_RR_CTRL_SWITCHER_USE_USBIN_FORCE_CTRL_SHFT                            0x4
#define PMIO_TM_RR_CTRL_SWITCHER_USE_USBIN_FORCE_CTRL_USBIN_NOT_IN_USE_FVAL           0x0
#define PMIO_TM_RR_CTRL_SWITCHER_USE_USBIN_FORCE_CTRL_USBIN_IN_USE_FVAL               0x1
#define PMIO_TM_RR_CTRL_INPUT_CUR_MUX_REG_CTRL_BMSK                                   0x8
#define PMIO_TM_RR_CTRL_INPUT_CUR_MUX_REG_CTRL_SHFT                                   0x3
#define PMIO_TM_RR_CTRL_INPUT_CUR_MUX_REG_CTRL_DISABLE_FVAL                           0x0
#define PMIO_TM_RR_CTRL_INPUT_CUR_MUX_REG_CTRL_ENABLE_FVAL                            0x1
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_FORCE_CTRL_BMSK                                   0x6
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_FORCE_CTRL_SHFT                                   0x1
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_FORCE_CTRL_REF_IS_RBIAS_FVAL                      0x0
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_FORCE_CTRL_REF_IS_5_DIV_3_V_FVAL                  0x1
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_FORCE_CTRL_REF_IS_5_DIV_2_V_FVAL                  0x2
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_FORCE_CTRL_REF_IS_5_V_FVAL                        0x3
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_REG_CTRL_BMSK                                     0x1
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_REG_CTRL_SHFT                                     0x0
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_REG_CTRL_DISABLE_FVAL                             0x0
#define PMIO_TM_RR_CTRL_REF_RNG_SEL_REG_CTRL_ENABLE_FVAL                              0x1

#define PMIO_TRIM_NUM_ADDR(x)                                                  ((x) + 0x000000f0)
#define PMIO_TRIM_NUM_RMSK                                                           0xff
#define PMIO_TRIM_NUM_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TRIM_NUM_ADDR(base), PMIO_TRIM_NUM_RMSK, 0, val)
#define PMIO_TRIM_NUM_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TRIM_NUM_ADDR(base), mask, shift, val)
#define PMIO_TRIM_NUM_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TRIM_NUM_ADDR(base), val, len)
#define PMIO_TRIM_NUM_TRIM_NUM_BMSK                                                  0xff
#define PMIO_TRIM_NUM_TRIM_NUM_SHFT                                                   0x0
#define PMIO_TRIM_NUM_TRIM_NUM_NUMBER_OF_TRIM_REGS_FVAL                               0xd

#define PMIO_THERM_GAIN_ADDR(x)                                                ((x) + 0x000000f1)
#define PMIO_THERM_GAIN_RMSK                                                         0xff
#define PMIO_THERM_GAIN_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_THERM_GAIN_ADDR(base), PMIO_THERM_GAIN_RMSK, 0, val)
#define PMIO_THERM_GAIN_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_THERM_GAIN_ADDR(base), mask, shift, val)
#define PMIO_THERM_GAIN_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_THERM_GAIN_ADDR(base), val, len)
#define PMIO_THERM_GAIN_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_THERM_GAIN_ADDR(base), val)
#define PMIO_THERM_GAIN_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_THERM_GAIN_ADDR(base), mask, shift, val)
#define PMIO_THERM_GAIN_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_THERM_GAIN_ADDR(base), val, len)
#define PMIO_THERM_GAIN_TRIM_VAL_BMSK                                                0xff
#define PMIO_THERM_GAIN_TRIM_VAL_SHFT                                                 0x0

#define PMIO_THERM_OFFSET_ADDR(x)                                              ((x) + 0x000000f2)
#define PMIO_THERM_OFFSET_RMSK                                                       0xff
#define PMIO_THERM_OFFSET_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_THERM_OFFSET_ADDR(base), PMIO_THERM_OFFSET_RMSK, 0, val)
#define PMIO_THERM_OFFSET_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_THERM_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_THERM_OFFSET_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_THERM_OFFSET_ADDR(base), val, len)
#define PMIO_THERM_OFFSET_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_THERM_OFFSET_ADDR(base), val)
#define PMIO_THERM_OFFSET_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_THERM_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_THERM_OFFSET_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_THERM_OFFSET_ADDR(base), val, len)
#define PMIO_THERM_OFFSET_TRIM_VAL_BMSK                                              0xff
#define PMIO_THERM_OFFSET_TRIM_VAL_SHFT                                               0x0

#define PMIO_DIG_5V_GAIN_ADDR(x)                                               ((x) + 0x000000f3)
#define PMIO_DIG_5V_GAIN_RMSK                                                        0xff
#define PMIO_DIG_5V_GAIN_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DIG_5V_GAIN_ADDR(base), PMIO_DIG_5V_GAIN_RMSK, 0, val)
#define PMIO_DIG_5V_GAIN_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DIG_5V_GAIN_ADDR(base), mask, shift, val)
#define PMIO_DIG_5V_GAIN_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DIG_5V_GAIN_ADDR(base), val, len)
#define PMIO_DIG_5V_GAIN_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DIG_5V_GAIN_ADDR(base), val)
#define PMIO_DIG_5V_GAIN_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DIG_5V_GAIN_ADDR(base), mask, shift, val)
#define PMIO_DIG_5V_GAIN_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DIG_5V_GAIN_ADDR(base), val, len)
#define PMIO_DIG_5V_GAIN_TRIM_VAL_BMSK                                               0xff
#define PMIO_DIG_5V_GAIN_TRIM_VAL_SHFT                                                0x0

#define PMIO_DIG_5V_OFFSET_ADDR(x)                                             ((x) + 0x000000f4)
#define PMIO_DIG_5V_OFFSET_RMSK                                                      0xff
#define PMIO_DIG_5V_OFFSET_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DIG_5V_OFFSET_ADDR(base), PMIO_DIG_5V_OFFSET_RMSK, 0, val)
#define PMIO_DIG_5V_OFFSET_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DIG_5V_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_DIG_5V_OFFSET_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DIG_5V_OFFSET_ADDR(base), val, len)
#define PMIO_DIG_5V_OFFSET_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DIG_5V_OFFSET_ADDR(base), val)
#define PMIO_DIG_5V_OFFSET_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DIG_5V_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_DIG_5V_OFFSET_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DIG_5V_OFFSET_ADDR(base), val, len)
#define PMIO_DIG_5V_OFFSET_TRIM_VAL_BMSK                                             0xff
#define PMIO_DIG_5V_OFFSET_TRIM_VAL_SHFT                                              0x0

#define PMIO_USBIN_I_GAIN_ADDR(x)                                              ((x) + 0x000000f5)
#define PMIO_USBIN_I_GAIN_RMSK                                                       0xff
#define PMIO_USBIN_I_GAIN_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USBIN_I_GAIN_ADDR(base), PMIO_USBIN_I_GAIN_RMSK, 0, val)
#define PMIO_USBIN_I_GAIN_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USBIN_I_GAIN_ADDR(base), mask, shift, val)
#define PMIO_USBIN_I_GAIN_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USBIN_I_GAIN_ADDR(base), val, len)
#define PMIO_USBIN_I_GAIN_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_USBIN_I_GAIN_ADDR(base), val)
#define PMIO_USBIN_I_GAIN_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_USBIN_I_GAIN_ADDR(base), mask, shift, val)
#define PMIO_USBIN_I_GAIN_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_USBIN_I_GAIN_ADDR(base), val, len)
#define PMIO_USBIN_I_GAIN_TRIM_VAL_BMSK                                              0xff
#define PMIO_USBIN_I_GAIN_TRIM_VAL_SHFT                                               0x0

#define PMIO_USBIN_I_OFFSET_ADDR(x)                                            ((x) + 0x000000f6)
#define PMIO_USBIN_I_OFFSET_RMSK                                                     0xff
#define PMIO_USBIN_I_OFFSET_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_USBIN_I_OFFSET_ADDR(base), PMIO_USBIN_I_OFFSET_RMSK, 0, val)
#define PMIO_USBIN_I_OFFSET_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_USBIN_I_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_USBIN_I_OFFSET_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_USBIN_I_OFFSET_ADDR(base), val, len)
#define PMIO_USBIN_I_OFFSET_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_USBIN_I_OFFSET_ADDR(base), val)
#define PMIO_USBIN_I_OFFSET_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_USBIN_I_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_USBIN_I_OFFSET_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_USBIN_I_OFFSET_ADDR(base), val, len)
#define PMIO_USBIN_I_OFFSET_TRIM_VAL_BMSK                                            0xff
#define PMIO_USBIN_I_OFFSET_TRIM_VAL_SHFT                                             0x0

#define PMIO_DCIN_I_GAIN_ADDR(x)                                               ((x) + 0x000000f7)
#define PMIO_DCIN_I_GAIN_RMSK                                                        0xff
#define PMIO_DCIN_I_GAIN_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DCIN_I_GAIN_ADDR(base), PMIO_DCIN_I_GAIN_RMSK, 0, val)
#define PMIO_DCIN_I_GAIN_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DCIN_I_GAIN_ADDR(base), mask, shift, val)
#define PMIO_DCIN_I_GAIN_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DCIN_I_GAIN_ADDR(base), val, len)
#define PMIO_DCIN_I_GAIN_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DCIN_I_GAIN_ADDR(base), val)
#define PMIO_DCIN_I_GAIN_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DCIN_I_GAIN_ADDR(base), mask, shift, val)
#define PMIO_DCIN_I_GAIN_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DCIN_I_GAIN_ADDR(base), val, len)
#define PMIO_DCIN_I_GAIN_TRIM_VAL_BMSK                                               0xff
#define PMIO_DCIN_I_GAIN_TRIM_VAL_SHFT                                                0x0

#define PMIO_DCIN_I_OFFSET_ADDR(x)                                             ((x) + 0x000000f8)
#define PMIO_DCIN_I_OFFSET_RMSK                                                      0xff
#define PMIO_DCIN_I_OFFSET_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DCIN_I_OFFSET_ADDR(base), PMIO_DCIN_I_OFFSET_RMSK, 0, val)
#define PMIO_DCIN_I_OFFSET_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DCIN_I_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_DCIN_I_OFFSET_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DCIN_I_OFFSET_ADDR(base), val, len)
#define PMIO_DCIN_I_OFFSET_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DCIN_I_OFFSET_ADDR(base), val)
#define PMIO_DCIN_I_OFFSET_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DCIN_I_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_DCIN_I_OFFSET_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DCIN_I_OFFSET_ADDR(base), val, len)
#define PMIO_DCIN_I_OFFSET_TRIM_VAL_BMSK                                             0xff
#define PMIO_DCIN_I_OFFSET_TRIM_VAL_SHFT                                              0x0

#define PMIO_DIG_2P5_V_GAIN_ADDR(x)                                            ((x) + 0x000000f9)
#define PMIO_DIG_2P5_V_GAIN_RMSK                                                     0xff
#define PMIO_DIG_2P5_V_GAIN_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DIG_2P5_V_GAIN_ADDR(base), PMIO_DIG_2P5_V_GAIN_RMSK, 0, val)
#define PMIO_DIG_2P5_V_GAIN_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DIG_2P5_V_GAIN_ADDR(base), mask, shift, val)
#define PMIO_DIG_2P5_V_GAIN_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DIG_2P5_V_GAIN_ADDR(base), val, len)
#define PMIO_DIG_2P5_V_GAIN_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DIG_2P5_V_GAIN_ADDR(base), val)
#define PMIO_DIG_2P5_V_GAIN_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DIG_2P5_V_GAIN_ADDR(base), mask, shift, val)
#define PMIO_DIG_2P5_V_GAIN_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DIG_2P5_V_GAIN_ADDR(base), val, len)
#define PMIO_DIG_2P5_V_GAIN_TRIM_VAL_BMSK                                            0xff
#define PMIO_DIG_2P5_V_GAIN_TRIM_VAL_SHFT                                             0x0

#define PMIO_DIG_2P5_V_OFFSET_ADDR(x)                                          ((x) + 0x000000fa)
#define PMIO_DIG_2P5_V_OFFSET_RMSK                                                   0xff
#define PMIO_DIG_2P5_V_OFFSET_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_DIG_2P5_V_OFFSET_ADDR(base), PMIO_DIG_2P5_V_OFFSET_RMSK, 0, val)
#define PMIO_DIG_2P5_V_OFFSET_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_DIG_2P5_V_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_DIG_2P5_V_OFFSET_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_DIG_2P5_V_OFFSET_ADDR(base), val, len)
#define PMIO_DIG_2P5_V_OFFSET_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_DIG_2P5_V_OFFSET_ADDR(base), val)
#define PMIO_DIG_2P5_V_OFFSET_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_DIG_2P5_V_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_DIG_2P5_V_OFFSET_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_DIG_2P5_V_OFFSET_ADDR(base), val, len)
#define PMIO_DIG_2P5_V_OFFSET_TRIM_VAL_BMSK                                          0xff
#define PMIO_DIG_2P5_V_OFFSET_TRIM_VAL_SHFT                                           0x0

#define PMIO_TMP_GAIN_ADDR(x)                                                  ((x) + 0x000000fb)
#define PMIO_TMP_GAIN_RMSK                                                           0xff
#define PMIO_TMP_GAIN_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TMP_GAIN_ADDR(base), PMIO_TMP_GAIN_RMSK, 0, val)
#define PMIO_TMP_GAIN_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TMP_GAIN_ADDR(base), mask, shift, val)
#define PMIO_TMP_GAIN_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TMP_GAIN_ADDR(base), val, len)
#define PMIO_TMP_GAIN_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TMP_GAIN_ADDR(base), val)
#define PMIO_TMP_GAIN_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TMP_GAIN_ADDR(base), mask, shift, val)
#define PMIO_TMP_GAIN_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TMP_GAIN_ADDR(base), val, len)
#define PMIO_TMP_GAIN_TRIM_VAL_BMSK                                                  0xff
#define PMIO_TMP_GAIN_TRIM_VAL_SHFT                                                   0x0

#define PMIO_TMP_OFFSET_ADDR(x)                                                ((x) + 0x000000fc)
#define PMIO_TMP_OFFSET_RMSK                                                         0xff
#define PMIO_TMP_OFFSET_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_TMP_OFFSET_ADDR(base), PMIO_TMP_OFFSET_RMSK, 0, val)
#define PMIO_TMP_OFFSET_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_TMP_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_TMP_OFFSET_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_TMP_OFFSET_ADDR(base), val, len)
#define PMIO_TMP_OFFSET_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_TMP_OFFSET_ADDR(base), val)
#define PMIO_TMP_OFFSET_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_TMP_OFFSET_ADDR(base), mask, shift, val)
#define PMIO_TMP_OFFSET_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_TMP_OFFSET_ADDR(base), val, len)
#define PMIO_TMP_OFFSET_TRIM_VAL_BMSK                                                0xff
#define PMIO_TMP_OFFSET_TRIM_VAL_SHFT                                                 0x0

#define PMIO_MISC_TRIM_ADDR(x)                                                 ((x) + 0x000000fd)
#define PMIO_MISC_TRIM_RMSK                                                           0x3
#define PMIO_MISC_TRIM_IN(ctxt, pmic, base, val)\
        in_pmio_field(ctxt, pmic, PMIO_MISC_TRIM_ADDR(base), PMIO_MISC_TRIM_RMSK, 0, val)
#define PMIO_MISC_TRIM_INF(ctxt, pmic, base, mask, shift, val)\
        in_pmio_field(ctxt, pmic, PMIO_MISC_TRIM_ADDR(base), mask, shift, val)
#define PMIO_MISC_TRIM_INN(ctxt, pmic, base, val, len)\
        in_pmio(ctxt, pmic, PMIO_MISC_TRIM_ADDR(base), val, len)
#define PMIO_MISC_TRIM_OUT(ctxt, pmic, base, val)\
        out_pmio(ctxt, pmic, PMIO_MISC_TRIM_ADDR(base), val)
#define PMIO_MISC_TRIM_OUTF(ctxt, pmic, base, mask, shift, val)\
        out_pmio_field(ctxt, pmic, PMIO_MISC_TRIM_ADDR(base), mask, shift, val)
#define PMIO_MISC_TRIM_OUTN(ctxt, pmic, base, val, len)\
        out_pmio_array(ctxt, pmic, PMIO_MISC_TRIM_ADDR(base), val, len)
#define PMIO_MISC_TRIM_THERM_BIAS_TRIM_BMSK                                           0x3
#define PMIO_MISC_TRIM_THERM_BIAS_TRIM_SHFT                                           0x0


#endif /* __FGADCHALPMIO_H__ */


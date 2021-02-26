/*=============================================================================

FILE:         HwioQdssTFunnel.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#ifndef HWIOQDSSTFUNNEL_H_
#define HWIOQDSSTFUNNEL_H_

#include "HALhwio.h"

/*------------------------------------------------------------------------------
 * MODULE: qdss_tfunnel
 *------------------------------------------------------------------------------*/


extern volatile uint32  QDSS_TFUNNEL_BASE_PTR;

#define QDSS_TFUNNEL_BASE                QDSS_TFUNNEL_BASE_PTR


#define QDSS_TFUNNEL_REG_BASE                                                                       (QDSS_TFUNNEL_BASE + 0x00000000)
#define QDSS_TFUNNEL_REG_BASE_PHYS                                                                  (QDSS_TFUNNEL_BASE_PHYS + 0x00000000)

#define HWIO_QDSS_TFUNNEL_FUNCTL_ADDR                                                            (QDSS_TFUNNEL_REG_BASE      + 00000000)
#define HWIO_QDSS_TFUNNEL_FUNCTL_PHYS                                                            (QDSS_TFUNNEL_REG_BASE_PHYS + 00000000)
#define HWIO_QDSS_TFUNNEL_FUNCTL_RMSK                                                                 0xfff
#define HWIO_QDSS_TFUNNEL_FUNCTL_IN                                                              \
        in_dword_masked(HWIO_QDSS_TFUNNEL_FUNCTL_ADDR, HWIO_QDSS_TFUNNEL_FUNCTL_RMSK)
#define HWIO_QDSS_TFUNNEL_FUNCTL_INM(m)                                                          \
        in_dword_masked(HWIO_QDSS_TFUNNEL_FUNCTL_ADDR, m)
#define HWIO_QDSS_TFUNNEL_FUNCTL_OUT(v)                                                          \
        out_dword(HWIO_QDSS_TFUNNEL_FUNCTL_ADDR,v)
#define HWIO_QDSS_TFUNNEL_FUNCTL_OUTM(m,v)                                                       \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNEL_FUNCTL_ADDR,m,v,HWIO_QDSS_TFUNNEL_FUNCTL_IN); \
        HWIO_INTFREE()

#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_ALL_PORT_BMSK 0xff
#define HWIO_QDSS_TFUNNEL_FUNCTL_MINIMUM_HOLD_TIME_BMSK                                               0xf00
#define HWIO_QDSS_TFUNNEL_FUNCTL_MINIMUM_HOLD_TIME_SHFT                                                 0x8
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_7_BMSK                                              0x80
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_7_SHFT                                               0x7
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_6_BMSK                                              0x40
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_6_SHFT                                               0x6
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_5_BMSK                                              0x20
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_5_SHFT                                               0x5
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENALBE_SLAVE_PORT_4_BMSK                                              0x10
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENALBE_SLAVE_PORT_4_SHFT                                               0x4
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_3_BMSK                                               0x8
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_3_SHFT                                               0x3
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_2_BMSK                                               0x4
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_2_SHFT                                               0x2
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_1_BMSK                                               0x2
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_1_SHFT                                               0x1
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_0_BMSK                                               0x1
#define HWIO_QDSS_TFUNNEL_FUNCTL_ENABLE_SLAVE_PORT_0_SHFT                                                 0

#define HWIO_QDSS_TFUNNEL_PRICTL_ADDR                                                            (QDSS_TFUNNEL_REG_BASE      + 0x00000004)
#define HWIO_QDSS_TFUNNEL_PRICTL_PHYS                                                            (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000004)
#define HWIO_QDSS_TFUNNEL_PRICTL_RMSK                                                              0xffffff
#define HWIO_QDSS_TFUNNEL_PRICTL_IN                                                              \
        in_dword_masked(HWIO_QDSS_TFUNNEL_PRICTL_ADDR, HWIO_QDSS_TFUNNEL_PRICTL_RMSK)
#define HWIO_QDSS_TFUNNEL_PRICTL_INM(m)                                                          \
        in_dword_masked(HWIO_QDSS_TFUNNEL_PRICTL_ADDR, m)
#define HWIO_QDSS_TFUNNEL_PRICTL_OUT(v)                                                          \
        out_dword(HWIO_QDSS_TFUNNEL_PRICTL_ADDR,v)
#define HWIO_QDSS_TFUNNEL_PRICTL_OUTM(m,v)                                                       \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNEL_PRICTL_ADDR,m,v,HWIO_QDSS_TFUNNEL_PRICTL_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT7_BMSK                                               0xe00000
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT7_SHFT                                                   0x15
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT6_BMSK                                               0x1c0000
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT6_SHFT                                                   0x12
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT5_BMSK                                                0x38000
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT5_SHFT                                                    0xf
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT4_BMSK                                                 0x7000
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT4_SHFT                                                    0xc
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT3_BMSK                                                  0xe00
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT3_SHFT                                                    0x9
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT2_BMSK                                                  0x1c0
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT2_SHFT                                                    0x6
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT1_BMSK                                                   0x38
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT1_SHFT                                                    0x3
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT0_BMSK                                                    0x7
#define HWIO_QDSS_TFUNNEL_PRICTL_PRIORITY_PORT0_SHFT                                                      0

#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ADDR                                                        (QDSS_TFUNNEL_REG_BASE      + 0x00000eec)
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_PHYS                                                        (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000eec)
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_RMSK                                                              0x1f
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_IN                                                          \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITATBDATA0_ADDR, HWIO_QDSS_TFUNNEL_ITATBDATA0_RMSK)
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_INM(m)                                                      \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITATBDATA0_ADDR, m)
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_OUT(v)                                                      \
        out_dword(HWIO_QDSS_TFUNNEL_ITATBDATA0_ADDR,v)
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_OUTM(m,v)                                                   \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNEL_ITATBDATA0_ADDR,m,v,HWIO_QDSS_TFUNNEL_ITATBDATA0_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_31_BMSK                                                    0x10
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_31_SHFT                                                     0x4
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_23_BMSK                                                     0x8
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_23_SHFT                                                     0x3
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_15_BMSK                                                     0x4
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_15_SHFT                                                     0x2
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_7_BMSK                                                      0x2
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_7_SHFT                                                      0x1
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_0_BMSK                                                      0x1
#define HWIO_QDSS_TFUNNEL_ITATBDATA0_ATDATA_0_SHFT                                                        0

#define HWIO_QDSS_TFUNNEL_ITATBCTR2_ADDR                                                         (QDSS_TFUNNEL_REG_BASE      + 0x00000ef0)
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_PHYS                                                         (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000ef0)
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_RMSK                                                                0x3
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_IN                                                           \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITATBCTR2_ADDR, HWIO_QDSS_TFUNNEL_ITATBCTR2_RMSK)
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_INM(m)                                                       \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITATBCTR2_ADDR, m)
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_OUT(v)                                                       \
        out_dword(HWIO_QDSS_TFUNNEL_ITATBCTR2_ADDR,v)
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_OUTM(m,v)                                                    \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNEL_ITATBCTR2_ADDR,m,v,HWIO_QDSS_TFUNNEL_ITATBCTR2_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_AFVALID_BMSK                                                        0x2
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_AFVALID_SHFT                                                        0x1
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_ATREADY_BMSK                                                        0x1
#define HWIO_QDSS_TFUNNEL_ITATBCTR2_ATREADY_SHFT                                                          0

#define HWIO_QDSS_TFUNNEL_ITATBCTR1_ADDR                                                         (QDSS_TFUNNEL_REG_BASE      + 0x00000ef4)
#define HWIO_QDSS_TFUNNEL_ITATBCTR1_PHYS                                                         (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000ef4)
#define HWIO_QDSS_TFUNNEL_ITATBCTR1_RMSK                                                               0x7f
#define HWIO_QDSS_TFUNNEL_ITATBCTR1_IN                                                           \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITATBCTR1_ADDR, HWIO_QDSS_TFUNNEL_ITATBCTR1_RMSK)
#define HWIO_QDSS_TFUNNEL_ITATBCTR1_INM(m)                                                       \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITATBCTR1_ADDR, m)
#define HWIO_QDSS_TFUNNEL_ITATBCTR1_OUT(v)                                                       \
        out_dword(HWIO_QDSS_TFUNNEL_ITATBCTR1_ADDR,v)
#define HWIO_QDSS_TFUNNEL_ITATBCTR1_OUTM(m,v)                                                    \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNEL_ITATBCTR1_ADDR,m,v,HWIO_QDSS_TFUNNEL_ITATBCTR1_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_TFUNNEL_ITATBCTR1_ATID_BMSK                                                          0x7f
#define HWIO_QDSS_TFUNNEL_ITATBCTR1_ATID_SHFT                                                             0

#define HWIO_QDSS_TFUNNEL_ITATBCTR0_ADDR                                                         (QDSS_TFUNNEL_REG_BASE      + 0x00000ef8)
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_PHYS                                                         (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000ef8)
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_RMSK                                                              0x303
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_IN                                                           \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITATBCTR0_ADDR, HWIO_QDSS_TFUNNEL_ITATBCTR0_RMSK)
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_INM(m)                                                       \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITATBCTR0_ADDR, m)
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_OUT(v)                                                       \
        out_dword(HWIO_QDSS_TFUNNEL_ITATBCTR0_ADDR,v)
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_OUTM(m,v)                                                    \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNEL_ITATBCTR0_ADDR,m,v,HWIO_QDSS_TFUNNEL_ITATBCTR0_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_ATBYTES_BMSK                                                      0x300
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_ATBYTES_SHFT                                                        0x8
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_AFREADYS_BMSK                                                       0x2
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_AFREADYS_SHFT                                                       0x1
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_ATVALID_BMSK                                                        0x1
#define HWIO_QDSS_TFUNNEL_ITATBCTR0_ATVALID_SHFT                                                          0

#define HWIO_QDSS_TFUNNEL_ITCTL_ADDR                                                             (QDSS_TFUNNEL_REG_BASE      + 0x00000f00)
#define HWIO_QDSS_TFUNNEL_ITCTL_PHYS                                                             (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000f00)
#define HWIO_QDSS_TFUNNEL_ITCTL_RMSK                                                                    0x1
#define HWIO_QDSS_TFUNNEL_ITCTL_IN                                                               \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITCTL_ADDR, HWIO_QDSS_TFUNNEL_ITCTL_RMSK)
#define HWIO_QDSS_TFUNNEL_ITCTL_INM(m)                                                           \
        in_dword_masked(HWIO_QDSS_TFUNNEL_ITCTL_ADDR, m)
#define HWIO_QDSS_TFUNNEL_ITCTL_OUT(v)                                                           \
        out_dword(HWIO_QDSS_TFUNNEL_ITCTL_ADDR,v)
#define HWIO_QDSS_TFUNNEL_ITCTL_OUTM(m,v)                                                        \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNEL_ITCTL_ADDR,m,v,HWIO_QDSS_TFUNNEL_ITCTL_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_TFUNNEL_ITCTL_INTEGRATION_MODE_BMSK                                                   0x1
#define HWIO_QDSS_TFUNNEL_ITCTL_INTEGRATION_MODE_SHFT                                                     0

#define HWIO_QDSS_TFUNNELCLAIMSET_ADDR                                                                  (QDSS_TFUNNEL_REG_BASE      + 0x00000fa0)
#define HWIO_QDSS_TFUNNELCLAIMSET_PHYS                                                                  (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000fa0)
#define HWIO_QDSS_TFUNNELCLAIMSET_RMSK                                                                         0xf
#define HWIO_QDSS_TFUNNELCLAIMSET_IN                                                                    \
        in_dword_masked(HWIO_QDSS_TFUNNELCLAIMSET_ADDR, HWIO_QDSS_TFUNNELCLAIMSET_RMSK)
#define HWIO_QDSS_TFUNNELCLAIMSET_INM(m)                                                                \
        in_dword_masked(HWIO_QDSS_TFUNNELCLAIMSET_ADDR, m)
#define HWIO_QDSS_TFUNNELCLAIMSET_OUT(v)                                                                \
        out_dword(HWIO_QDSS_TFUNNELCLAIMSET_ADDR,v)
#define HWIO_QDSS_TFUNNELCLAIMSET_OUTM(m,v)                                                             \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNELCLAIMSET_ADDR,m,v,HWIO_QDSS_TFUNNELCLAIMSET_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_TFUNNELCLAIMSET_CLAIMSET_BMSK                                                                0xf
#define HWIO_QDSS_TFUNNELCLAIMSET_CLAIMSET_SHFT                                                                  0

#define HWIO_QDSS_TFUNNELCLAIMCLR_ADDR                                                                  (QDSS_TFUNNEL_REG_BASE      + 0x00000fa4)
#define HWIO_QDSS_TFUNNELCLAIMCLR_PHYS                                                                  (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000fa4)
#define HWIO_QDSS_TFUNNELCLAIMCLR_RMSK                                                                         0xf
#define HWIO_QDSS_TFUNNELCLAIMCLR_IN                                                                    \
        in_dword_masked(HWIO_QDSS_TFUNNELCLAIMCLR_ADDR, HWIO_QDSS_TFUNNELCLAIMCLR_RMSK)
#define HWIO_QDSS_TFUNNELCLAIMCLR_INM(m)                                                                \
        in_dword_masked(HWIO_QDSS_TFUNNELCLAIMCLR_ADDR, m)
#define HWIO_QDSS_TFUNNELCLAIMCLR_OUT(v)                                                                \
        out_dword(HWIO_QDSS_TFUNNELCLAIMCLR_ADDR,v)
#define HWIO_QDSS_TFUNNELCLAIMCLR_OUTM(m,v)                                                             \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_TFUNNELCLAIMCLR_ADDR,m,v,HWIO_QDSS_TFUNNELCLAIMCLR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_TFUNNELCLAIMCLR_CLAIMCLR_BMSK                                                                0xf
#define HWIO_QDSS_TFUNNELCLAIMCLR_CLAIMCLR_SHFT                                                                  0


#define HWIO_QDSS_TFUNNEL_LOCKACCESS_ADDR                                                              (QDSS_TFUNNEL_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_TFUNNEL_LOCKACCESS_PHYS                                                              (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000fb0)
#define HWIO_QDSS_TFUNNEL_LOCKACCESS_RMSK                                                              0xffffffff
#define HWIO_QDSS_TFUNNEL_LOCKACCESS_OUT(v)                                                            \
        out_dword(HWIO_QDSS_TFUNNEL_LOCKACCESS_ADDR,v)
#define HWIO_QDSS_TFUNNEL_LOCKACCESS_OUTM(m,v)                                                         \
        out_dword_masked(HWIO_QDSS_TFUNNEL_LOCKACCESS_ADDR,m,v,HWIO_QDSS_TFUNNEL_LOCKACCESS_shadow)
#define HWIO_QDSS_TFUNNEL_LOCKACCESS_DATA_BMSK                                                         0xffffffff
#define HWIO_QDSS_TFUNNEL_LOCKACCESS_DATA_SHFT                                                                  0

#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_ADDR                                                                  (QDSS_TFUNNEL_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_PHYS                                                                  (QDSS_TFUNNEL_REG_BASE_PHYS + 0x00000fb4)
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_RMSK                                                                  0xffffffff
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_IN                                                                    \
        in_dword_masked(HWIO_QDSS_TFUNNEL_LOCKSTATUS_ADDR, HWIO_QDSS_TFUNNEL_LOCKSTATUS_RMSK)
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_INM(m)                                                                \
        in_dword_masked(HWIO_QDSS_TFUNNEL_LOCKSTATUS_ADDR, m)
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_DATA_BMSK                                                             0xffffffff
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_DATA_SHFT                                                                      0

#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_LOCKTYPE_BMSK                                                                     0x4
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_LOCKTYPE_SHFT                                                                     0x2
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_LOCKGRANT_BMSK                                                                    0x2
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_LOCKGRANT_SHFT                                                                    0x1
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_LOCKEXIST_BMSK                                                                    0x1
#define HWIO_QDSS_TFUNNEL_LOCKSTATUS_LOCKEXIST_SHFT                                                                      0

#endif  /* HWIOQDSSTFUNNEL_H_ */


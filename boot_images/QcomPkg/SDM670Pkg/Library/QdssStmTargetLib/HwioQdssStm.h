/*=============================================================================

FILE:         HwioQdssStm.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#ifndef HWIOQDSSSTM_H_
#define HWIOQDSSSTM_H_

/*------------------------------------------------------------------------------
 * MODULE: qdss_stm
 *------------------------------------------------------------------------------*/

#define QDSS_STM_BASE                   QDSS_STM_BASE_CONST

#define QDSS_STM_REG_BASE               (QDSS_STM_BASE)
#define QDSS_STM_REG_BASE_PHYS          (QDSS_STM_BASE_PHYS + 0x00000000)

#define HWIO_QDSS_STMDMASTARTR_ADDR                        (QDSS_STM_REG_BASE      + 0x00000c04)
#define HWIO_QDSS_STMDMASTARTR_PHYS                        (QDSS_STM_REG_BASE_PHYS + 0x00000c04)
#define HWIO_QDSS_STMDMASTARTR_RMSK                        0x1
#define HWIO_QDSS_STMDMASTARTR_OUT(v) \
        out_dword(HWIO_QDSS_STMDMASTARTR_ADDR,v)
#define HWIO_QDSS_STMDMASTARTR_OUTM(m,v) \
        out_dword_masked(HWIO_QDSS_STMDMASTARTR_ADDR,m,v,HWIO_QDSS_STMDMASTARTR_shadow)
#define HWIO_QDSS_STMDMASTARTR_START_BMSK                  0x1
#define HWIO_QDSS_STMDMASTARTR_START_SHFT                  0

#define HWIO_QDSS_STMDMASTOPR_ADDR                         (QDSS_STM_REG_BASE      + 0x00000c08)
#define HWIO_QDSS_STMDMASTOPR_PHYS                         (QDSS_STM_REG_BASE_PHYS + 0x00000c08)
#define HWIO_QDSS_STMDMASTOPR_RMSK                         0x1
#define HWIO_QDSS_STMDMASTOPR_OUT(v) \
        out_dword(HWIO_QDSS_STMDMASTOPR_ADDR,v)
#define HWIO_QDSS_STMDMASTOPR_OUTM(m,v) \
        out_dword_masked(HWIO_QDSS_STMDMASTOPR_ADDR,m,v,HWIO_QDSS_STMDMASTOPR_shadow)
#define HWIO_QDSS_STMDMASTOPR_STOP_BMSK                    0x1
#define HWIO_QDSS_STMDMASTOPR_STOP_SHFT                    0

#define HWIO_QDSS_STMDMASTATR_ADDR                         (QDSS_STM_REG_BASE      + 0x00000c0c)
#define HWIO_QDSS_STMDMASTATR_PHYS                         (QDSS_STM_REG_BASE_PHYS + 0x00000c0c)
#define HWIO_QDSS_STMDMASTATR_RMSK                         0x1
#define HWIO_QDSS_STMDMASTATR_IN \
        in_dword_masked(HWIO_QDSS_STMDMASTATR_ADDR, HWIO_QDSS_STMDMASTATR_RMSK)
#define HWIO_QDSS_STMDMASTATR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMDMASTATR_ADDR, m)
#define HWIO_QDSS_STMDMASTATR_STATUS_BMSK                  0x1
#define HWIO_QDSS_STMDMASTATR_STATUS_SHFT                  0

#define HWIO_QDSS_STMDMACTLR_ADDR                          (QDSS_STM_REG_BASE      + 0x00000c10)
#define HWIO_QDSS_STMDMACTLR_PHYS                          (QDSS_STM_REG_BASE_PHYS + 0x00000c10)
#define HWIO_QDSS_STMDMACTLR_RMSK                          0xc
#define HWIO_QDSS_STMDMACTLR_IN \
        in_dword_masked(HWIO_QDSS_STMDMACTLR_ADDR, HWIO_QDSS_STMDMACTLR_RMSK)
#define HWIO_QDSS_STMDMACTLR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMDMACTLR_ADDR, m)
#define HWIO_QDSS_STMDMACTLR_OUT(v) \
        out_dword(HWIO_QDSS_STMDMACTLR_ADDR,v)
#define HWIO_QDSS_STMDMACTLR_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMDMACTLR_ADDR,m,v,HWIO_QDSS_STMDMACTLR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMDMACTLR_SENS_BMSK                     0xc
#define HWIO_QDSS_STMDMACTLR_SENS_SHFT                     0x2

#define HWIO_QDSS_STMDMAIDR_ADDR                           (QDSS_STM_REG_BASE      + 0x00000cfc)
#define HWIO_QDSS_STMDMAIDR_PHYS                           (QDSS_STM_REG_BASE_PHYS + 0x00000cfc)
#define HWIO_QDSS_STMDMAIDR_RMSK                           0xfff
#define HWIO_QDSS_STMDMAIDR_IN \
        in_dword_masked(HWIO_QDSS_STMDMAIDR_ADDR, HWIO_QDSS_STMDMAIDR_RMSK)
#define HWIO_QDSS_STMDMAIDR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMDMAIDR_ADDR, m)
#define HWIO_QDSS_STMDMAIDR_VENDSPEC_BMSK                  0xf00
#define HWIO_QDSS_STMDMAIDR_VENDSPEC_SHFT                  0x8
#define HWIO_QDSS_STMDMAIDR_CLASSREV_BMSK                  0xf0
#define HWIO_QDSS_STMDMAIDR_CLASSREV_SHFT                  0x4
#define HWIO_QDSS_STMDMAIDR_CLASS_BMSK                     0xf
#define HWIO_QDSS_STMDMAIDR_CLASS_SHFT                     0

#define HWIO_QDSS_STMSPER_ADDR                             (QDSS_STM_REG_BASE      + 0x00000e00)
#define HWIO_QDSS_STMSPER_PHYS                             (QDSS_STM_REG_BASE_PHYS + 0x00000e00)
#define HWIO_QDSS_STMSPER_RMSK                             0xffffffff
#define HWIO_QDSS_STMSPER_IN \
        in_dword_masked(HWIO_QDSS_STMSPER_ADDR, HWIO_QDSS_STMSPER_RMSK)
#define HWIO_QDSS_STMSPER_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPER_ADDR, m)
#define HWIO_QDSS_STMSPER_OUT(v) \
        out_dword(HWIO_QDSS_STMSPER_ADDR,v)
#define HWIO_QDSS_STMSPER_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMSPER_ADDR,m,v,HWIO_QDSS_STMSPER_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMSPER_SPE_BMSK                         0xffffffff
#define HWIO_QDSS_STMSPER_SPE_SHFT                         0

#define HWIO_QDSS_STMSPTER_ADDR                            (QDSS_STM_REG_BASE      + 0x00000e20)
#define HWIO_QDSS_STMSPTER_PHYS                            (QDSS_STM_REG_BASE_PHYS + 0x00000e20)
#define HWIO_QDSS_STMSPTER_RMSK                            0xffffffff
#define HWIO_QDSS_STMSPTER_IN \
        in_dword_masked(HWIO_QDSS_STMSPTER_ADDR, HWIO_QDSS_STMSPTER_RMSK)
#define HWIO_QDSS_STMSPTER_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPTER_ADDR, m)
#define HWIO_QDSS_STMSPTER_OUT(v) \
        out_dword(HWIO_QDSS_STMSPTER_ADDR,v)
#define HWIO_QDSS_STMSPTER_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMSPTER_ADDR,m,v,HWIO_QDSS_STMSPTER_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMSPTER_SPTE_BMSK                       0xffffffff
#define HWIO_QDSS_STMSPTER_SPTE_SHFT                       0

#define HWIO_QDSS_STMSPSCR_ADDR                            (QDSS_STM_REG_BASE      + 0x00000e60)
#define HWIO_QDSS_STMSPSCR_PHYS                            (QDSS_STM_REG_BASE_PHYS + 0x00000e60)
#define HWIO_QDSS_STMSPSCR_RMSK                            0xfff00003
#define HWIO_QDSS_STMSPSCR_IN \
        in_dword_masked(HWIO_QDSS_STMSPSCR_ADDR, HWIO_QDSS_STMSPSCR_RMSK)
#define HWIO_QDSS_STMSPSCR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPSCR_ADDR, m)
#define HWIO_QDSS_STMSPSCR_OUT(v) \
        out_dword(HWIO_QDSS_STMSPSCR_ADDR,v)
#define HWIO_QDSS_STMSPSCR_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMSPSCR_ADDR,m,v,HWIO_QDSS_STMSPSCR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMSPSCR_PORTSEL_BMSK                    0xfff00000
#define HWIO_QDSS_STMSPSCR_PORTSEL_SHFT                    0x14
#define HWIO_QDSS_STMSPSCR_PORTCTL_BMSK                    0x3
#define HWIO_QDSS_STMSPSCR_PORTCTL_SHFT                    0

#define HWIO_QDSS_STMSPMSCR_ADDR                           (QDSS_STM_REG_BASE      + 0x00000e64)
#define HWIO_QDSS_STMSPMSCR_PHYS                           (QDSS_STM_REG_BASE_PHYS + 0x00000e64)
#define HWIO_QDSS_STMSPMSCR_RMSK                           0x7f8001
#define HWIO_QDSS_STMSPMSCR_IN \
        in_dword_masked(HWIO_QDSS_STMSPMSCR_ADDR, HWIO_QDSS_STMSPMSCR_RMSK)
#define HWIO_QDSS_STMSPMSCR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPMSCR_ADDR, m)
#define HWIO_QDSS_STMSPMSCR_OUT(v) \
        out_dword(HWIO_QDSS_STMSPMSCR_ADDR,v)
#define HWIO_QDSS_STMSPMSCR_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMSPMSCR_ADDR,m,v,HWIO_QDSS_STMSPMSCR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMSPMSCR_MASTSEL_BMSK                   0x7f8000
#define HWIO_QDSS_STMSPMSCR_MASTSEL_SHFT                   0xf
#define HWIO_QDSS_STMSPMSCR_MASTCTL_BMSK                   0x1
#define HWIO_QDSS_STMSPMSCR_MASTCTL_SHFT                   0

#define HWIO_QDSS_STMSPOVERRIDER_ADDR                      (QDSS_STM_REG_BASE      + 0x00000e68)
#define HWIO_QDSS_STMSPOVERRIDER_PHYS                      (QDSS_STM_REG_BASE_PHYS + 0x00000e68)
#define HWIO_QDSS_STMSPOVERRIDER_RMSK                      0xffff8007
#define HWIO_QDSS_STMSPOVERRIDER_IN \
        in_dword_masked(HWIO_QDSS_STMSPOVERRIDER_ADDR, HWIO_QDSS_STMSPOVERRIDER_RMSK)
#define HWIO_QDSS_STMSPOVERRIDER_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPOVERRIDER_ADDR, m)
#define HWIO_QDSS_STMSPOVERRIDER_OUT(v) \
        out_dword(HWIO_QDSS_STMSPOVERRIDER_ADDR,v)
#define HWIO_QDSS_STMSPOVERRIDER_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMSPOVERRIDER_ADDR,m,v,HWIO_QDSS_STMSPOVERRIDER_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMSPOVERRIDER_PORTSEL_BMSK              0xffff8000
#define HWIO_QDSS_STMSPOVERRIDER_PORTSEL_SHFT              0xf
#define HWIO_QDSS_STMSPOVERRIDER_OVERTS_BMSK               0x4
#define HWIO_QDSS_STMSPOVERRIDER_OVERTS_SHFT               0x2
#define HWIO_QDSS_STMSPOVERRIDER_OVERCTL_BMSK              0x3
#define HWIO_QDSS_STMSPOVERRIDER_OVERCTL_SHFT              0

#define HWIO_QDSS_STMSPMOVERRIDER_ADDR                     (QDSS_STM_REG_BASE      + 0x00000e6c)
#define HWIO_QDSS_STMSPMOVERRIDER_PHYS                     (QDSS_STM_REG_BASE_PHYS + 0x00000e6c)
#define HWIO_QDSS_STMSPMOVERRIDER_RMSK                     0x7f8001
#define HWIO_QDSS_STMSPMOVERRIDER_IN \
        in_dword_masked(HWIO_QDSS_STMSPMOVERRIDER_ADDR, HWIO_QDSS_STMSPMOVERRIDER_RMSK)
#define HWIO_QDSS_STMSPMOVERRIDER_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPMOVERRIDER_ADDR, m)
#define HWIO_QDSS_STMSPMOVERRIDER_OUT(v) \
        out_dword(HWIO_QDSS_STMSPMOVERRIDER_ADDR,v)
#define HWIO_QDSS_STMSPMOVERRIDER_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMSPMOVERRIDER_ADDR,m,v,HWIO_QDSS_STMSPMOVERRIDER_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMSPMOVERRIDER_MASTSEL_BMSK             0x7f8000
#define HWIO_QDSS_STMSPMOVERRIDER_MASTSEL_SHFT             0xf
#define HWIO_QDSS_STMSPMOVERRIDER_MASTCTL_BMSK             0x1
#define HWIO_QDSS_STMSPMOVERRIDER_MASTCTL_SHFT             0

#define HWIO_QDSS_STMSPTRIGCSR_ADDR                        (QDSS_STM_REG_BASE      + 0x00000e70)
#define HWIO_QDSS_STMSPTRIGCSR_PHYS                        (QDSS_STM_REG_BASE_PHYS + 0x00000e70)
#define HWIO_QDSS_STMSPTRIGCSR_RMSK                        0x1f
#define HWIO_QDSS_STMSPTRIGCSR_IN \
        in_dword_masked(HWIO_QDSS_STMSPTRIGCSR_ADDR, HWIO_QDSS_STMSPTRIGCSR_RMSK)
#define HWIO_QDSS_STMSPTRIGCSR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPTRIGCSR_ADDR, m)
#define HWIO_QDSS_STMSPTRIGCSR_OUT(v) \
        out_dword(HWIO_QDSS_STMSPTRIGCSR_ADDR,v)
#define HWIO_QDSS_STMSPTRIGCSR_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMSPTRIGCSR_ADDR,m,v,HWIO_QDSS_STMSPTRIGCSR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMSPTRIGCSR_ATBTRIGEN_DIR_BMSK          0x10
#define HWIO_QDSS_STMSPTRIGCSR_ATBTRIGEN_DIR_SHFT          0x4
#define HWIO_QDSS_STMSPTRIGCSR_ATBTRIGEN_TE_BMSK           0x8
#define HWIO_QDSS_STMSPTRIGCSR_ATBTRIGEN_TE_SHFT           0x3
#define HWIO_QDSS_STMSPTRIGCSR_TRIGCLEAR_BMSK              0x4
#define HWIO_QDSS_STMSPTRIGCSR_TRIGCLEAR_SHFT              0x2
#define HWIO_QDSS_STMSPTRIGCSR_TRIGSTATUS_BMSK             0x2
#define HWIO_QDSS_STMSPTRIGCSR_TRIGSTATUS_SHFT             0x1
#define HWIO_QDSS_STMSPTRIGCSR_TRIGCTL_BMSK                0x1
#define HWIO_QDSS_STMSPTRIGCSR_TRIGCTL_SHFT                  0

#define HWIO_QDSS_STMTCSR_ADDR                             (QDSS_STM_REG_BASE      + 0x00000e80)
#define HWIO_QDSS_STMTCSR_PHYS                             (QDSS_STM_REG_BASE_PHYS + 0x00000e80)
#define HWIO_QDSS_STMTCSR_RMSK                             0xff0027
#define HWIO_QDSS_STMTCSR_IN \
        in_dword_masked(HWIO_QDSS_STMTCSR_ADDR, HWIO_QDSS_STMTCSR_RMSK)
#define HWIO_QDSS_STMTCSR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMTCSR_ADDR, m)
#define HWIO_QDSS_STMTCSR_OUT(v) \
        out_dword(HWIO_QDSS_STMTCSR_ADDR,v)
#define HWIO_QDSS_STMTCSR_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMTCSR_ADDR,m,v,HWIO_QDSS_STMTCSR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMTCSR_TRACEID_BMSK                     0x7f0000
#define HWIO_QDSS_STMTCSR_TRACEID_SHFT                         0x10
#define HWIO_QDSS_STMTCSR_BUSY_BMSK                        0x800000
#define HWIO_QDSS_STMTCSR_BUSY_SHFT                            0x17
#define HWIO_QDSS_STMTCSR_COMPEN_BMSK                          0x20
#define HWIO_QDSS_STMTCSR_COMPEN_SHFT                           0x5
#define HWIO_QDSS_STMTCSR_SYNCEN_BMSK                           0x4
#define HWIO_QDSS_STMTCSR_SYNCEN_SHFT                           0x2
#define HWIO_QDSS_STMTCSR_TSEN_BMSK                             0x2
#define HWIO_QDSS_STMTCSR_TSEN_SHFT                             0x1
#define HWIO_QDSS_STMTCSR_EN_BMSK                               0x1
#define HWIO_QDSS_STMTCSR_EN_SHFT                                 0

#define HWIO_QDSS_STMTSSTIMR_ADDR                          (QDSS_STM_REG_BASE      + 0x00000e84)
#define HWIO_QDSS_STMTSSTIMR_PHYS                          (QDSS_STM_REG_BASE_PHYS + 0x00000e84)
#define HWIO_QDSS_STMTSSTIMR_RMSK                          0x1
#define HWIO_QDSS_STMTSSTIMR_OUT(v) \
        out_dword(HWIO_QDSS_STMTSSTIMR_ADDR,v)
#define HWIO_QDSS_STMTSSTIMR_OUTM(m,v) \
        out_dword_masked(HWIO_QDSS_STMTSSTIMR_ADDR,m,v,HWIO_QDSS_STMTSSTIMR_shadow)
#define HWIO_QDSS_STMTSSTIMR_FORCETS_BMSK                  0x1
#define HWIO_QDSS_STMTSSTIMR_FORCETS_SHFT                  0

#define HWIO_QDSS_STMTSFREQR_ADDR                          (QDSS_STM_REG_BASE      + 0x00000e8c)
#define HWIO_QDSS_STMTSFREQR_PHYS                          (QDSS_STM_REG_BASE_PHYS + 0x00000e8c)
#define HWIO_QDSS_STMTSFREQR_RMSK                          0xffffffff
#define HWIO_QDSS_STMTSFREQR_IN \
        in_dword_masked(HWIO_QDSS_STMTSFREQR_ADDR, HWIO_QDSS_STMTSFREQR_RMSK)
#define HWIO_QDSS_STMTSFREQR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMTSFREQR_ADDR, m)
#define HWIO_QDSS_STMTSFREQR_OUT(v) \
        out_dword(HWIO_QDSS_STMTSFREQR_ADDR,v)
#define HWIO_QDSS_STMTSFREQR_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMTSFREQR_ADDR,m,v,HWIO_QDSS_STMTSFREQR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMTSFREQR_FREQ_BMSK                     0xffffffff
#define HWIO_QDSS_STMTSFREQR_FREQ_SHFT                     0

#define HWIO_QDSS_STMSYNCR_ADDR                            (QDSS_STM_REG_BASE      + 0x00000e90)
#define HWIO_QDSS_STMSYNCR_PHYS                            (QDSS_STM_REG_BASE_PHYS + 0x00000e90)
#define HWIO_QDSS_STMSYNCR_RMSK                            0x1ffc
#define HWIO_QDSS_STMSYNCR_IN \
        in_dword_masked(HWIO_QDSS_STMSYNCR_ADDR, HWIO_QDSS_STMSYNCR_RMSK)
#define HWIO_QDSS_STMSYNCR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSYNCR_ADDR, m)
#define HWIO_QDSS_STMSYNCR_OUT(v) \
        out_dword(HWIO_QDSS_STMSYNCR_ADDR,v)
#define HWIO_QDSS_STMSYNCR_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMSYNCR_ADDR,m,v,HWIO_QDSS_STMSYNCR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMSYNCR_MODE_BMSK                           0x1000
#define HWIO_QDSS_STMSYNCR_MODE_SHFT                              0xc
#define HWIO_QDSS_STMSYNCR_COUNT_BMSK                           0xffc
#define HWIO_QDSS_STMSYNCR_COUNT_SHFT                             0x2

#define HWIO_QDSS_STMAUXCR_ADDR                            (QDSS_STM_REG_BASE      + 0x00000e94)
#define HWIO_QDSS_STMAUXCR_PHYS                            (QDSS_STM_REG_BASE_PHYS + 0x00000e94)
#define HWIO_QDSS_STMAUXCR_RMSK                                  0x1f
#define HWIO_QDSS_STMAUXCR_IN \
        in_dword_masked(HWIO_QDSS_STMAUXCR_ADDR, HWIO_QDSS_STMAUXCR_RMSK)
#define HWIO_QDSS_STMAUXCR_INM(m) \
        in_dword_masked(HWIO_QDSS_STMAUXCR_ADDR, m)
#define HWIO_QDSS_STMAUXCR_OUT(v) \
        out_dword(HWIO_QDSS_STMAUXCR_ADDR,v)
#define HWIO_QDSS_STMAUXCR_OUTM(m,v) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMAUXCR_ADDR,m,v,HWIO_QDSS_STMAUXCR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMAUXCR_AFREADYHIGH_BMSK                0x10
#define HWIO_QDSS_STMAUXCR_AFREADYHIGH_SHFT                0x4
#define HWIO_QDSS_STMAUXCR_CLKON_BMSK                      0x8
#define HWIO_QDSS_STMAUXCR_CLKON_SHFT                      0x3
#define HWIO_QDSS_STMAUXCR_PRIORINVDIS_BMSK                0x4
#define HWIO_QDSS_STMAUXCR_PRIORINVDIS_SHFT                0x2
#define HWIO_QDSS_STMAUXCR_ASYNCPE_BMSK                    0x2
#define HWIO_QDSS_STMAUXCR_ASYNCPE_SHFT                    0x1
#define HWIO_QDSS_STMAUXCR_FIFOAF_BMSK                     0x1
#define HWIO_QDSS_STMAUXCR_FIFOAF_SHFT                       0

#define HWIO_QDSS_STMSPFEAT1R_ADDR                         (QDSS_STM_REG_BASE      + 0x00000ea0)
#define HWIO_QDSS_STMSPFEAT1R_PHYS                         (QDSS_STM_REG_BASE_PHYS + 0x00000ea0)
#define HWIO_QDSS_STMSPFEAT1R_RMSK                           0xffffff
#define HWIO_QDSS_STMSPFEAT1R_IN \
        in_dword_masked(HWIO_QDSS_STMSPFEAT1R_ADDR, HWIO_QDSS_STMSPFEAT1R_RMSK)
#define HWIO_QDSS_STMSPFEAT1R_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPFEAT1R_ADDR, m)
#define HWIO_QDSS_STMSPFEAT1R_SWOEN_BMSK                   0xc00000
#define HWIO_QDSS_STMSPFEAT1R_SWOEN_SHFT                       0x16
#define HWIO_QDSS_STMSPFEAT1R_SYNCEN_BMSK                  0x300000
#define HWIO_QDSS_STMSPFEAT1R_SYNCEN_SHFT                      0x14
#define HWIO_QDSS_STMSPFEAT1R_HWTEN_BMSK                    0xc0000
#define HWIO_QDSS_STMSPFEAT1R_HWTEN_SHFT                       0x12
#define HWIO_QDSS_STMSPFEAT1R_TSPRESCALE_BMSK               0x30000
#define HWIO_QDSS_STMSPFEAT1R_TSPRESCALE_SHFT                  0x10
#define HWIO_QDSS_STMSPFEAT1R_TRIGCTL_BMSK                   0xc000
#define HWIO_QDSS_STMSPFEAT1R_TRIGCTL_SHFT                      0xe
#define HWIO_QDSS_STMSPFEAT1R_TRACEBUS_BMSK                  0x3c00
#define HWIO_QDSS_STMSPFEAT1R_TRACEBUS_SHFT                     0xa
#define HWIO_QDSS_STMSPFEAT1R_SYNC_BMSK                       0x300
#define HWIO_QDSS_STMSPFEAT1R_SYNC_SHFT                         0x8
#define HWIO_QDSS_STMSPFEAT1R_FORCETS_BMSK                     0x80
#define HWIO_QDSS_STMSPFEAT1R_FORCETS_SHFT                      0x7
#define HWIO_QDSS_STMSPFEAT1R_TSFREQ_BMSK                      0x40
#define HWIO_QDSS_STMSPFEAT1R_TSFREQ_SHFT                       0x6
#define HWIO_QDSS_STMSPFEAT1R_TS_BMSK                          0x30
#define HWIO_QDSS_STMSPFEAT1R_TS_SHFT                           0x4
#define HWIO_QDSS_STMSPFEAT1R_PROT_BMSK                         0xf
#define HWIO_QDSS_STMSPFEAT1R_PROT_SHFT                           0

#define HWIO_QDSS_STMSPFEAT2R_ADDR                         (QDSS_STM_REG_BASE      + 0x00000ea4)
#define HWIO_QDSS_STMSPFEAT2R_PHYS                         (QDSS_STM_REG_BASE_PHYS + 0x00000ea4)
#define HWIO_QDSS_STMSPFEAT2R_RMSK                            0x3f7f7
#define HWIO_QDSS_STMSPFEAT2R_IN \
        in_dword_masked(HWIO_QDSS_STMSPFEAT2R_ADDR, HWIO_QDSS_STMSPFEAT2R_RMSK)
#define HWIO_QDSS_STMSPFEAT2R_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPFEAT2R_ADDR, m)
#define HWIO_QDSS_STMSPFEAT2R_SPTYPE_BMSK                  0x30000
#define HWIO_QDSS_STMSPFEAT2R_SPTYPE_SHFT                     0x10
#define HWIO_QDSS_STMSPFEAT2R_DSIZE_BMSK                    0xf000
#define HWIO_QDSS_STMSPFEAT2R_DSIZE_SHFT                       0xc
#define HWIO_QDSS_STMSPFEAT2R_SPTRTYPE_BMSK                  0x600
#define HWIO_QDSS_STMSPFEAT2R_SPTRTYPE_SHFT                    0x9
#define HWIO_QDSS_STMSPFEAT2R_PRIVMASK_BMSK                  0x180
#define HWIO_QDSS_STMSPFEAT2R_PRIVMASK_SHFT                    0x7
#define HWIO_QDSS_STMSPFEAT2R_SPOVERRIDE_BMSK                 0x40
#define HWIO_QDSS_STMSPFEAT2R_SPOVERRIDE_SHFT                  0x6
#define HWIO_QDSS_STMSPFEAT2R_SPCOMP_BMSK                     0x30
#define HWIO_QDSS_STMSPFEAT2R_SPCOMP_SHFT                      0x4
#define HWIO_QDSS_STMSPFEAT2R_SPER_BMSK                        0x4
#define HWIO_QDSS_STMSPFEAT2R_SPER_SHFT                        0x2
#define HWIO_QDSS_STMSPFEAT2R_SPTER_BMSK                       0x3
#define HWIO_QDSS_STMSPFEAT2R_SPTER_SHFT                         0

#define HWIO_QDSS_STMSPFEAT3R_ADDR                         (QDSS_STM_REG_BASE      + 0x00000ea8)
#define HWIO_QDSS_STMSPFEAT3R_PHYS                         (QDSS_STM_REG_BASE_PHYS + 0x00000ea8)
#define HWIO_QDSS_STMSPFEAT3R_RMSK                               0x7f
#define HWIO_QDSS_STMSPFEAT3R_IN \
        in_dword_masked(HWIO_QDSS_STMSPFEAT3R_ADDR, HWIO_QDSS_STMSPFEAT3R_RMSK)
#define HWIO_QDSS_STMSPFEAT3R_INM(m) \
        in_dword_masked(HWIO_QDSS_STMSPFEAT3R_ADDR, m)
#define HWIO_QDSS_STMSPFEAT3R_NUMMAST_BMSK                       0x7f
#define HWIO_QDSS_STMSPFEAT3R_NUMMAST_SHFT                          0

#define HWIO_QDSS_STMITTRIGGER_ADDR                        (QDSS_STM_REG_BASE      + 0x00000ee8)
#define HWIO_QDSS_STMITTRIGGER_PHYS                        (QDSS_STM_REG_BASE_PHYS + 0x00000ee8)
#define HWIO_QDSS_STMITTRIGGER_RMSK                               0xf
#define HWIO_QDSS_STMITTRIGGER_OUT(v) \
        out_dword(HWIO_QDSS_STMITTRIGGER_ADDR,v)
#define HWIO_QDSS_STMITTRIGGER_OUTM(m,v) \
        out_dword_masked(HWIO_QDSS_STMITTRIGGER_ADDR,m,v,HWIO_QDSS_STMITTRIGGER_shadow)
#define HWIO_QDSS_STMITTRIGGER_ASYNCOUT_W_BMSK                    0x8
#define HWIO_QDSS_STMITTRIGGER_ASYNCOUT_W_SHFT                    0x3
#define HWIO_QDSS_STMITTRIGGER_TRIGOUTHETE_W_BMSK                 0x4
#define HWIO_QDSS_STMITTRIGGER_TRIGOUTHETE_W_SHFT                 0x2
#define HWIO_QDSS_STMITTRIGGER_TRIGOUTSW_W_BMSK                   0x2
#define HWIO_QDSS_STMITTRIGGER_TRIGOUTSW_W_SHFT                   0x1
#define HWIO_QDSS_STMITTRIGGER_TRIGOUTSPTE_W_BMSK                 0x1
#define HWIO_QDSS_STMITTRIGGER_TRIGOUTSPTE_W_SHFT                   0

#define HWIO_QDSS_STMITATBDATA0_ADDR                       (QDSS_STM_REG_BASE      + 0x00000eec)
#define HWIO_QDSS_STMITATBDATA0_PHYS                       (QDSS_STM_REG_BASE_PHYS + 0x00000eec)
#define HWIO_QDSS_STMITATBDATA0_RMSK                             0x1f
#define HWIO_QDSS_STMITATBDATA0_OUT(v) \
        out_dword(HWIO_QDSS_STMITATBDATA0_ADDR,v)
#define HWIO_QDSS_STMITATBDATA0_OUTM(m,v) \
        out_dword_masked(HWIO_QDSS_STMITATBDATA0_ADDR,m,v,HWIO_QDSS_STMITATBDATA0_shadow)
#define HWIO_QDSS_STMITATBDATA0_ATDATAM31_W_BMSK                 0x10
#define HWIO_QDSS_STMITATBDATA0_ATDATAM31_W_SHFT                  0x4
#define HWIO_QDSS_STMITATBDATA0_ATDATAM23_W_BMSK                  0x8
#define HWIO_QDSS_STMITATBDATA0_ATDATAM23_W_SHFT                  0x3
#define HWIO_QDSS_STMITATBDATA0_ATDATAM15_W_BMSK                  0x4
#define HWIO_QDSS_STMITATBDATA0_ATDATAM15_W_SHFT                  0x2
#define HWIO_QDSS_STMITATBDATA0_ATDATAM7_W_BMSK                   0x2
#define HWIO_QDSS_STMITATBDATA0_ATDATAM7_W_SHFT                   0x1
#define HWIO_QDSS_STMITATBDATA0_ATDATAM0_W_BMSK                   0x1
#define HWIO_QDSS_STMITATBDATA0_ATDATAM0_W_SHFT                     0

#define HWIO_QDSS_STMITATBCTR2_ADDR                        (QDSS_STM_REG_BASE      + 0x00000ef0)
#define HWIO_QDSS_STMITATBCTR2_PHYS                        (QDSS_STM_REG_BASE_PHYS + 0x00000ef0)
#define HWIO_QDSS_STMITATBCTR2_RMSK                               0x3
#define HWIO_QDSS_STMITATBCTR2_IN \
        in_dword_masked(HWIO_QDSS_STMITATBCTR2_ADDR, HWIO_QDSS_STMITATBCTR2_RMSK)
#define HWIO_QDSS_STMITATBCTR2_INM(m) \
        in_dword_masked(HWIO_QDSS_STMITATBCTR2_ADDR, m)
#define HWIO_QDSS_STMITATBCTR2_AFVALIDM_R_BMSK                    0x2
#define HWIO_QDSS_STMITATBCTR2_AFVALIDM_R_SHFT                    0x1
#define HWIO_QDSS_STMITATBCTR2_ATREADYM_R_BMSK                    0x1
#define HWIO_QDSS_STMITATBCTR2_ATREADYM_R_SHFT                      0

#define HWIO_QDSS_STMITATBID_ADDR                          (QDSS_STM_REG_BASE      + 0x00000ef4)
#define HWIO_QDSS_STMITATBID_PHYS                          (QDSS_STM_REG_BASE_PHYS + 0x00000ef4)
#define HWIO_QDSS_STMITATBID_RMSK                                0x7f
#define HWIO_QDSS_STMITATBID_OUT(v) \
        out_dword(HWIO_QDSS_STMITATBID_ADDR,v)
#define HWIO_QDSS_STMITATBID_OUTM(m,v) \
        out_dword_masked(HWIO_QDSS_STMITATBID_ADDR,m,v,HWIO_QDSS_STMITATBID_shadow)
#define HWIO_QDSS_STMITATBID_ATIDM_W_BMSK                                                                 0x7f
#define HWIO_QDSS_STMITATBID_ATIDM_W_SHFT                                                                    0

#define HWIO_QDSS_STMITATBCTR0_ADDR                                                                 (QDSS_STM_REG_BASE      + 0x00000ef8)
#define HWIO_QDSS_STMITATBCTR0_PHYS                                                                 (QDSS_STM_REG_BASE_PHYS + 0x00000ef8)
#define HWIO_QDSS_STMITATBCTR0_RMSK                                                                      0x303
#define HWIO_QDSS_STMITATBCTR0_OUT(v)                                                               \
        out_dword(HWIO_QDSS_STMITATBCTR0_ADDR,v)
#define HWIO_QDSS_STMITATBCTR0_OUTM(m,v)                                                            \
        out_dword_masked(HWIO_QDSS_STMITATBCTR0_ADDR,m,v,HWIO_QDSS_STMITATBCTR0_shadow)
#define HWIO_QDSS_STMITATBCTR0_ATBYTESM_W_BMSK                                                           0x300
#define HWIO_QDSS_STMITATBCTR0_ATBYTESM_W_SHFT                                                             0x8
#define HWIO_QDSS_STMITATBCTR0_AFREADYM_W_BMSK                                                             0x2
#define HWIO_QDSS_STMITATBCTR0_AFREADYM_W_SHFT                                                             0x1
#define HWIO_QDSS_STMITATBCTR0_ATVALIDM_W_BMSK                                                             0x1
#define HWIO_QDSS_STMITATBCTR0_ATVALIDM_W_SHFT                                                               0

#define HWIO_QDSS_STMITCTRL_ADDR                                                                    (QDSS_STM_REG_BASE      + 0x00000f00)
#define HWIO_QDSS_STMITCTRL_PHYS                                                                    (QDSS_STM_REG_BASE_PHYS + 0x00000f00)
#define HWIO_QDSS_STMITCTRL_RMSK                                                                           0x1
#define HWIO_QDSS_STMITCTRL_IN                                                                      \
        in_dword_masked(HWIO_QDSS_STMITCTRL_ADDR, HWIO_QDSS_STMITCTRL_RMSK)
#define HWIO_QDSS_STMITCTRL_INM(m)                                                                  \
        in_dword_masked(HWIO_QDSS_STMITCTRL_ADDR, m)
#define HWIO_QDSS_STMITCTRL_OUT(v)                                                                  \
        out_dword(HWIO_QDSS_STMITCTRL_ADDR,v)
#define HWIO_QDSS_STMITCTRL_OUTM(m,v)                                                               \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMITCTRL_ADDR,m,v,HWIO_QDSS_STMITCTRL_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMITCTRL_INTEGRATION_MODE_BMSK                                                          0x1
#define HWIO_QDSS_STMITCTRL_INTEGRATION_MODE_SHFT                                                            0

#define HWIO_QDSS_STMCLAIMSET_ADDR                                                                  (QDSS_STM_REG_BASE      + 0x00000fa0)
#define HWIO_QDSS_STMCLAIMSET_PHYS                                                                  (QDSS_STM_REG_BASE_PHYS + 0x00000fa0)
#define HWIO_QDSS_STMCLAIMSET_RMSK                                                                         0xf
#define HWIO_QDSS_STMCLAIMSET_IN                                                                    \
        in_dword_masked(HWIO_QDSS_STMCLAIMSET_ADDR, HWIO_QDSS_STMCLAIMSET_RMSK)
#define HWIO_QDSS_STMCLAIMSET_INM(m)                                                                \
        in_dword_masked(HWIO_QDSS_STMCLAIMSET_ADDR, m)
#define HWIO_QDSS_STMCLAIMSET_OUT(v)                                                                \
        out_dword(HWIO_QDSS_STMCLAIMSET_ADDR,v)
#define HWIO_QDSS_STMCLAIMSET_OUTM(m,v)                                                             \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMCLAIMSET_ADDR,m,v,HWIO_QDSS_STMCLAIMSET_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMCLAIMSET_CLAIMSET_BMSK                                                                0xf
#define HWIO_QDSS_STMCLAIMSET_CLAIMSET_SHFT                                                                  0

#define HWIO_QDSS_STMCLAIMCLR_ADDR                                                                  (QDSS_STM_REG_BASE      + 0x00000fa4)
#define HWIO_QDSS_STMCLAIMCLR_PHYS                                                                  (QDSS_STM_REG_BASE_PHYS + 0x00000fa4)
#define HWIO_QDSS_STMCLAIMCLR_RMSK                                                                         0xf
#define HWIO_QDSS_STMCLAIMCLR_IN                                                                    \
        in_dword_masked(HWIO_QDSS_STMCLAIMCLR_ADDR, HWIO_QDSS_STMCLAIMCLR_RMSK)
#define HWIO_QDSS_STMCLAIMCLR_INM(m)                                                                \
        in_dword_masked(HWIO_QDSS_STMCLAIMCLR_ADDR, m)
#define HWIO_QDSS_STMCLAIMCLR_OUT(v)                                                                \
        out_dword(HWIO_QDSS_STMCLAIMCLR_ADDR,v)
#define HWIO_QDSS_STMCLAIMCLR_OUTM(m,v)                                                             \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_QDSS_STMCLAIMCLR_ADDR,m,v,HWIO_QDSS_STMCLAIMCLR_IN); \
        HWIO_INTFREE()
#define HWIO_QDSS_STMCLAIMCLR_CLAIMCLR_BMSK                                                                0xf
#define HWIO_QDSS_STMCLAIMCLR_CLAIMCLR_SHFT                                                                  0

#define HWIO_QDSS_STMLAR_ADDR                                                                       (QDSS_STM_REG_BASE      + 0x00000fb0)
#define HWIO_QDSS_STMLAR_PHYS                                                                       (QDSS_STM_REG_BASE_PHYS + 0x00000fb0)
#define HWIO_QDSS_STMLAR_RMSK                                                                       0xffffffff
#define HWIO_QDSS_STMLAR_OUT(v)                                                                     \
        out_dword(HWIO_QDSS_STMLAR_ADDR,v)
#define HWIO_QDSS_STMLAR_OUTM(m,v)                                                                  \
        out_dword_masked(HWIO_QDSS_STMLAR_ADDR,m,v,HWIO_QDSS_STMLAR_shadow)
#define HWIO_QDSS_STMLAR_ACCESS_W_BMSK                                                              0xffffffff
#define HWIO_QDSS_STMLAR_ACCESS_W_SHFT                                                                       0

#define HWIO_QDSS_STMLSR_ADDR                                                                       (QDSS_STM_REG_BASE      + 0x00000fb4)
#define HWIO_QDSS_STMLSR_PHYS                                                                       (QDSS_STM_REG_BASE_PHYS + 0x00000fb4)
#define HWIO_QDSS_STMLSR_RMSK                                                                              0x7
#define HWIO_QDSS_STMLSR_IN                                                                         \
        in_dword_masked(HWIO_QDSS_STMLSR_ADDR, HWIO_QDSS_STMLSR_RMSK)
#define HWIO_QDSS_STMLSR_INM(m)                                                                     \
        in_dword_masked(HWIO_QDSS_STMLSR_ADDR, m)
#define HWIO_QDSS_STMLSR_LOCKTYPE_BMSK                                                                     0x4
#define HWIO_QDSS_STMLSR_LOCKTYPE_SHFT                                                                     0x2
#define HWIO_QDSS_STMLSR_LOCKGRANT_BMSK                                                                    0x2
#define HWIO_QDSS_STMLSR_LOCKGRANT_SHFT                                                                    0x1
#define HWIO_QDSS_STMLSR_LOCKEXIST_BMSK                                                                    0x1
#define HWIO_QDSS_STMLSR_LOCKEXIST_SHFT                                                                      0

#define HWIO_QDSS_STMAUTHSTATUS_ADDR                                                                (QDSS_STM_REG_BASE      + 0x00000fb8)
#define HWIO_QDSS_STMAUTHSTATUS_PHYS                                                                (QDSS_STM_REG_BASE_PHYS + 0x00000fb8)
#define HWIO_QDSS_STMAUTHSTATUS_RMSK                                                                      0xff
#define HWIO_QDSS_STMAUTHSTATUS_IN                                                                  \
        in_dword_masked(HWIO_QDSS_STMAUTHSTATUS_ADDR, HWIO_QDSS_STMAUTHSTATUS_RMSK)
#define HWIO_QDSS_STMAUTHSTATUS_INM(m)                                                              \
        in_dword_masked(HWIO_QDSS_STMAUTHSTATUS_ADDR, m)
#define HWIO_QDSS_STMAUTHSTATUS_SNID_BMSK                                                                 0xc0
#define HWIO_QDSS_STMAUTHSTATUS_SNID_SHFT                                                                  0x6
#define HWIO_QDSS_STMAUTHSTATUS_SID_BMSK                                                                  0x30
#define HWIO_QDSS_STMAUTHSTATUS_SID_SHFT                                                                   0x4
#define HWIO_QDSS_STMAUTHSTATUS_NSNID_BMSK                                                                 0xc
#define HWIO_QDSS_STMAUTHSTATUS_NSNID_SHFT                                                                 0x2
#define HWIO_QDSS_STMAUTHSTATUS_NSID_BMSK                                                                  0x3
#define HWIO_QDSS_STMAUTHSTATUS_NSID_SHFT                                                                    0

#define HWIO_QDSS_STMDEVID_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000fc8)
#define HWIO_QDSS_STMDEVID_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000fc8)
#define HWIO_QDSS_STMDEVID_RMSK                                                                        0x1ffff
#define HWIO_QDSS_STMDEVID_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMDEVID_ADDR, HWIO_QDSS_STMDEVID_RMSK)
#define HWIO_QDSS_STMDEVID_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMDEVID_ADDR, m)
#define HWIO_QDSS_STMDEVID_NUMSP_BMSK                                                                  0x1ffff
#define HWIO_QDSS_STMDEVID_NUMSP_SHFT                                                                        0

#define HWIO_QDSS_STMDEVTYPE_ADDR                                                                   (QDSS_STM_REG_BASE      + 0x00000fcc)
#define HWIO_QDSS_STMDEVTYPE_PHYS                                                                   (QDSS_STM_REG_BASE_PHYS + 0x00000fcc)
#define HWIO_QDSS_STMDEVTYPE_RMSK                                                                         0xff
#define HWIO_QDSS_STMDEVTYPE_IN                                                                     \
        in_dword_masked(HWIO_QDSS_STMDEVTYPE_ADDR, HWIO_QDSS_STMDEVTYPE_RMSK)
#define HWIO_QDSS_STMDEVTYPE_INM(m)                                                                 \
        in_dword_masked(HWIO_QDSS_STMDEVTYPE_ADDR, m)
#define HWIO_QDSS_STMDEVTYPE_SUB_TYPE_BMSK                                                                0xf0
#define HWIO_QDSS_STMDEVTYPE_SUB_TYPE_SHFT                                                                 0x4
#define HWIO_QDSS_STMDEVTYPE_MAJOR_TYPE_BMSK                                                               0xf
#define HWIO_QDSS_STMDEVTYPE_MAJOR_TYPE_SHFT                                                                 0

#define HWIO_QDSS_STMPIDR0_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000fe0)
#define HWIO_QDSS_STMPIDR0_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000fe0)
#define HWIO_QDSS_STMPIDR0_RMSK                                                                           0xff
#define HWIO_QDSS_STMPIDR0_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMPIDR0_ADDR, HWIO_QDSS_STMPIDR0_RMSK)
#define HWIO_QDSS_STMPIDR0_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMPIDR0_ADDR, m)
#define HWIO_QDSS_STMPIDR0_PART_NUMBER_BITS7TO0_BMSK                                                      0xff
#define HWIO_QDSS_STMPIDR0_PART_NUMBER_BITS7TO0_SHFT                                                         0

#define HWIO_QDSS_STMPIDR1_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000fe4)
#define HWIO_QDSS_STMPIDR1_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000fe4)
#define HWIO_QDSS_STMPIDR1_RMSK                                                                           0xff
#define HWIO_QDSS_STMPIDR1_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMPIDR1_ADDR, HWIO_QDSS_STMPIDR1_RMSK)
#define HWIO_QDSS_STMPIDR1_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMPIDR1_ADDR, m)
#define HWIO_QDSS_STMPIDR1_JEP106_BITS3TO0_BMSK                                                           0xf0
#define HWIO_QDSS_STMPIDR1_JEP106_BITS3TO0_SHFT                                                            0x4
#define HWIO_QDSS_STMPIDR1_PART_NUMBER_BITS11TO8_BMSK                                                      0xf
#define HWIO_QDSS_STMPIDR1_PART_NUMBER_BITS11TO8_SHFT                                                        0

#define HWIO_QDSS_STMPIDR2_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000fe8)
#define HWIO_QDSS_STMPIDR2_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000fe8)
#define HWIO_QDSS_STMPIDR2_RMSK                                                                           0xff
#define HWIO_QDSS_STMPIDR2_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMPIDR2_ADDR, HWIO_QDSS_STMPIDR2_RMSK)
#define HWIO_QDSS_STMPIDR2_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMPIDR2_ADDR, m)
#define HWIO_QDSS_STMPIDR2_REVISION_BMSK                                                                  0xf0
#define HWIO_QDSS_STMPIDR2_REVISION_SHFT                                                                   0x4
#define HWIO_QDSS_STMPIDR2_JEDEC_BMSK                                                                      0x8
#define HWIO_QDSS_STMPIDR2_JEDEC_SHFT                                                                      0x3
#define HWIO_QDSS_STMPIDR2_JEP106_BITS6TO4_BMSK                                                            0x7
#define HWIO_QDSS_STMPIDR2_JEP106_BITS6TO4_SHFT                                                              0

#define HWIO_QDSS_STMPIDR3_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000fec)
#define HWIO_QDSS_STMPIDR3_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000fec)
#define HWIO_QDSS_STMPIDR3_RMSK                                                                           0xff
#define HWIO_QDSS_STMPIDR3_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMPIDR3_ADDR, HWIO_QDSS_STMPIDR3_RMSK)
#define HWIO_QDSS_STMPIDR3_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMPIDR3_ADDR, m)
#define HWIO_QDSS_STMPIDR3_REVAND_BMSK                                                                    0xf0
#define HWIO_QDSS_STMPIDR3_REVAND_SHFT                                                                     0x4
#define HWIO_QDSS_STMPIDR3_CUSTOMER_MODIFIED_BMSK                                                          0xf
#define HWIO_QDSS_STMPIDR3_CUSTOMER_MODIFIED_SHFT                                                            0

#define HWIO_QDSS_STMPIDR4_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000fd0)
#define HWIO_QDSS_STMPIDR4_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_QDSS_STMPIDR4_RMSK                                                                           0xff
#define HWIO_QDSS_STMPIDR4_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMPIDR4_ADDR, HWIO_QDSS_STMPIDR4_RMSK)
#define HWIO_QDSS_STMPIDR4_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMPIDR4_ADDR, m)
#define HWIO_QDSS_STMPIDR4_FOURKB_COUNT_BMSK                                                              0xf0
#define HWIO_QDSS_STMPIDR4_FOURKB_COUNT_SHFT                                                               0x4
#define HWIO_QDSS_STMPIDR4_JEP106_CONT_BMSK                                                                0xf
#define HWIO_QDSS_STMPIDR4_JEP106_CONT_SHFT                                                                  0

#define HWIO_QDSS_STMCIDR0_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000ff0)
#define HWIO_QDSS_STMCIDR0_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000ff0)
#define HWIO_QDSS_STMCIDR0_RMSK                                                                           0xff
#define HWIO_QDSS_STMCIDR0_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMCIDR0_ADDR, HWIO_QDSS_STMCIDR0_RMSK)
#define HWIO_QDSS_STMCIDR0_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMCIDR0_ADDR, m)
#define HWIO_QDSS_STMCIDR0_PREAMBLE_BMSK                                                                  0xff
#define HWIO_QDSS_STMCIDR0_PREAMBLE_SHFT                                                                     0

#define HWIO_QDSS_STMCIDR1_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000ff4)
#define HWIO_QDSS_STMCIDR1_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000ff4)
#define HWIO_QDSS_STMCIDR1_RMSK                                                                           0xff
#define HWIO_QDSS_STMCIDR1_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMCIDR1_ADDR, HWIO_QDSS_STMCIDR1_RMSK)
#define HWIO_QDSS_STMCIDR1_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMCIDR1_ADDR, m)
#define HWIO_QDSS_STMCIDR1_CLASS_BMSK                                                                     0xf0
#define HWIO_QDSS_STMCIDR1_CLASS_SHFT                                                                      0x4
#define HWIO_QDSS_STMCIDR1_PREAMBLE_BMSK                                                                   0xf
#define HWIO_QDSS_STMCIDR1_PREAMBLE_SHFT                                                                     0

#define HWIO_QDSS_STMCIDR2_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000ff8)
#define HWIO_QDSS_STMCIDR2_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000ff8)
#define HWIO_QDSS_STMCIDR2_RMSK                                                                           0xff
#define HWIO_QDSS_STMCIDR2_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMCIDR2_ADDR, HWIO_QDSS_STMCIDR2_RMSK)
#define HWIO_QDSS_STMCIDR2_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMCIDR2_ADDR, m)
#define HWIO_QDSS_STMCIDR2_PREAMBLE_BMSK                                                                  0xff
#define HWIO_QDSS_STMCIDR2_PREAMBLE_SHFT                                                                     0

#define HWIO_QDSS_STMCIDR3_ADDR                                                                     (QDSS_STM_REG_BASE      + 0x00000ffc)
#define HWIO_QDSS_STMCIDR3_PHYS                                                                     (QDSS_STM_REG_BASE_PHYS + 0x00000ffc)
#define HWIO_QDSS_STMCIDR3_RMSK                                                                           0xff
#define HWIO_QDSS_STMCIDR3_IN                                                                       \
        in_dword_masked(HWIO_QDSS_STMCIDR3_ADDR, HWIO_QDSS_STMCIDR3_RMSK)
#define HWIO_QDSS_STMCIDR3_INM(m)                                                                   \
        in_dword_masked(HWIO_QDSS_STMCIDR3_ADDR, m)
#define HWIO_QDSS_STMCIDR3_PREAMBLE_BMSK                                                                  0xff
#define HWIO_QDSS_STMCIDR3_PREAMBLE_SHFT                                                                     0





#define HWIO_QDSS_STMHEER_ADDR                                 (QDSS_STM_REG_BASE      + 0x00000d00)
#define HWIO_QDSS_STMHEER_RMSK                                 0xffffffff
#define HWIO_QDSS_STMHEER_IN          \
        in_dword_masked(HWIO_QDSS_STMHEER_ADDR, HWIO_QDSS_STMHEER_RMSK)
#define HWIO_QDSS_STMHEER_INM(m)      \
        in_dword_masked(HWIO_QDSS_STMHEER_ADDR, m)
#define HWIO_QDSS_STMHEER_OUT(v)      \
        out_dword(HWIO_QDSS_STMHEER_ADDR,v)
#define HWIO_QDSS_STMHEER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_STMHEER_ADDR,m,v,HWIO_QDSS_STMHEER_IN)
#define HWIO_QDSS_STMHEER_HEE_BMSK                             0xffffffff
#define HWIO_QDSS_STMHEER_HEE_SHFT                                    0x0

#define HWIO_QDSS_STMHETER_ADDR                                (QDSS_STM_REG_BASE      + 0x00000d20)
#define HWIO_QDSS_STMHETER_RMSK                                0xffffffff
#define HWIO_QDSS_STMHETER_IN          \
        in_dword_masked(HWIO_QDSS_STMHETER_ADDR, HWIO_QDSS_STMHETER_RMSK)
#define HWIO_QDSS_STMHETER_INM(m)      \
        in_dword_masked(HWIO_QDSS_STMHETER_ADDR, m)
#define HWIO_QDSS_STMHETER_OUT(v)      \
        out_dword(HWIO_QDSS_STMHETER_ADDR,v)
#define HWIO_QDSS_STMHETER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_STMHETER_ADDR,m,v,HWIO_QDSS_STMHETER_IN)
#define HWIO_QDSS_STMHETER_HETE_BMSK                           0xffffffff
#define HWIO_QDSS_STMHETER_HETE_SHFT                                  0x0

#define HWIO_QDSS_STMHEMCR_ADDR                                (QDSS_STM_REG_BASE      + 0x00000d64)
#define HWIO_QDSS_STMHEMCR_RMSK                                      0xf7
#define HWIO_QDSS_STMHEMCR_IN          \
        in_dword_masked(HWIO_QDSS_STMHEMCR_ADDR, HWIO_QDSS_STMHEMCR_RMSK)
#define HWIO_QDSS_STMHEMCR_INM(m)      \
        in_dword_masked(HWIO_QDSS_STMHEMCR_ADDR, m)
#define HWIO_QDSS_STMHEMCR_OUT(v)      \
        out_dword(HWIO_QDSS_STMHEMCR_ADDR,v)
#define HWIO_QDSS_STMHEMCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSS_STMHEMCR_ADDR,m,v,HWIO_QDSS_STMHEMCR_IN)
#define HWIO_QDSS_STMHEMCR_ATBTRIGEN_BMSK                            0x80
#define HWIO_QDSS_STMHEMCR_ATBTRIGEN_SHFT                             0x7
#define HWIO_QDSS_STMHEMCR_TRIGCLEAR_BMSK                            0x40
#define HWIO_QDSS_STMHEMCR_TRIGCLEAR_SHFT                             0x6
#define HWIO_QDSS_STMHEMCR_TRIGSTATUS_BMSK                           0x20
#define HWIO_QDSS_STMHEMCR_TRIGSTATUS_SHFT                            0x5
#define HWIO_QDSS_STMHEMCR_TRIGCTL_BMSK                              0x10
#define HWIO_QDSS_STMHEMCR_TRIGCTL_SHFT                               0x4
#define HWIO_QDSS_STMHEMCR_ERRDETECT_BMSK                             0x4
#define HWIO_QDSS_STMHEMCR_ERRDETECT_SHFT                             0x2
#define HWIO_QDSS_STMHEMCR_COMPEN_BMSK                                0x2
#define HWIO_QDSS_STMHEMCR_COMPEN_SHFT                                0x1
#define HWIO_QDSS_STMHEMCR_EN_BMSK                                    0x1
#define HWIO_QDSS_STMHEMCR_EN_SHFT                                    0x0


#endif  /* HWIOQDSSSTM_H_ */

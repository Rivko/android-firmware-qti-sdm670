/*Dumping Qtimer Registers*/
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Q t i m e r   H a r d w a r e R e g i s t e r s


EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2018 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/03/18   abh     File created.

=============================================================================*/


/*=============================================================================

                           MACROS

=============================================================================*/
extern uint64 QTIMER_OFFSET;

/*----------------------------------------------------------------------------
 * MODULE: QTMR_V1
 *--------------------------------------------------------------------------*/

#define BASE                                              (LPASS_BASE + QTIMER_OFFSET)

#define HWIO_QTMR_CNTPCT_LO_ADDR                          (BASE      + 0x00000000)
#define HWIO_QTMR_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_QTMR_CNTPCT_LO_IN          \
        in_dword_masked(HWIO_QTMR_CNTPCT_LO_ADDR, HWIO_QTMR_CNTPCT_LO_RMSK)
#define HWIO_QTMR_CNTPCT_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTPCT_LO_ADDR, m)
#define HWIO_QTMR_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_QTMR_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_QTMR_CNTPCT_HI_ADDR                          (BASE      + 0x00000004)
#define HWIO_QTMR_CNTPCT_HI_RMSK                          0xffffff
#define HWIO_QTMR_CNTPCT_HI_IN         \
        in_dword_masked(HWIO_QTMR_CNTPCT_HI_ADDR, HWIO_QTMR_CNTPCT_HI_RMSK)
#define HWIO_QTMR_CNTPCT_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTPCT_HI_ADDR, m)
#define HWIO_QTMR_CNTPCT_HI_CNTPCT_HI_BMSK                0xffffff
#define HWIO_QTMR_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_QTMR_CNTVCT_LO_ADDR                          (BASE      + 0x00000008)
#define HWIO_QTMR_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_QTMR_CNTVCT_LO_IN          \
        in_dword_masked(HWIO_QTMR_CNTVCT_LO_ADDR, HWIO_QTMR_CNTVCT_LO_RMSK)
#define HWIO_QTMR_CNTVCT_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTVCT_LO_ADDR, m)
#define HWIO_QTMR_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_QTMR_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_QTMR_CNTVCT_HI_ADDR                          (BASE      + 0x0000000c)
#define HWIO_QTMR_CNTVCT_HI_RMSK                          0xffffff
#define HWIO_QTMR_CNTVCT_HI_IN          \
        in_dword_masked(HWIO_QTMR_CNTVCT_HI_ADDR, HWIO_QTMR_CNTVCT_HI_RMSK)
#define HWIO_QTMR_CNTVCT_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTVCT_HI_ADDR, m)
#define HWIO_QTMR_CNTVCT_HI_CNTVCT_HI_BMSK                0xffffff
#define HWIO_QTMR_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_QTMR_CNTFRQ_ADDR                             (BASE      + 0x00000010)
#define HWIO_QTMR_CNTFRQ_RMSK                             0xffffffff
#define HWIO_QTMR_CNTFRQ_IN          \
        in_dword_masked(HWIO_QTMR_CNTFRQ_ADDR, HWIO_QTMR_CNTFRQ_RMSK)
#define HWIO_QTMR_CNTFRQ_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTFRQ_ADDR, m)
#define HWIO_QTMR_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_QTMR_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_QTMR_CNTPL0AC_ADDR                           (BASE      + 0x00000014)
#define HWIO_QTMR_CNTPL0AC_RMSK                                0x303
#define HWIO_QTMR_CNTPL0AC_IN          \
        in_dword_masked(HWIO_QTMR_CNTPL0AC_ADDR, HWIO_QTMR_CNTPL0AC_RMSK)
#define HWIO_QTMR_CNTPL0AC_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTPL0AC_ADDR(BASE, m)
#define HWIO_QTMR_CNTPL0AC_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTPL0AC_ADDR,v)
#define HWIO_QTMR_CNTPL0AC_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTPL0AC_ADDR(BASE,m,v,HWIO_QTMR_CNTPL0AC_IN)
#define HWIO_QTMR_CNTPL0AC_PL0CTEN_BMSK                        0x200
#define HWIO_QTMR_CNTPL0AC_PL0CTEN_SHFT                          0x9
#define HWIO_QTMR_CNTPL0AC_PL0VTEN_BMSK                        0x100
#define HWIO_QTMR_CNTPL0AC_PL0VTEN_SHFT                          0x8
#define HWIO_QTMR_CNTPL0AC_PL0VCTEN_BMSK                         0x2
#define HWIO_QTMR_CNTPL0AC_PL0VCTEN_SHFT                         0x1
#define HWIO_QTMR_CNTPL0AC_PL0PCTEN_BMSK                         0x1
#define HWIO_QTMR_CNTPL0AC_PL0PCTEN_SHFT                         0x0

#define HWIO_QTMR_CNTVOFF_LO_ADDR                         (BASE      + 0x00000018)
#define HWIO_QTMR_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_QTMR_CNTVOFF_LO_IN          \
        in_dword_masked(HWIO_QTMR_CNTVOFF_LO_ADDR, HWIO_QTMR_CNTVOFF_LO_RMSK)
#define HWIO_QTMR_CNTVOFF_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTVOFF_LO_ADDR, m)
#define HWIO_QTMR_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_QTMR_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_QTMR_CNTVOFF_HI_ADDR                         (BASE      + 0x0000001c)
#define HWIO_QTMR_CNTVOFF_HI_RMSK                         0xffffff
#define HWIO_QTMR_CNTVOFF_HI_IN          \
        in_dword_masked(HWIO_QTMR_CNTVOFF_HI_ADDR, HWIO_QTMR_CNTVOFF_HI_RMSK)
#define HWIO_QTMR_CNTVOFF_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTVOFF_HI_ADDR, m)
#define HWIO_QTMR_CNTVOFF_HI_CNTVOFF_HI_BMSK              0xffffff
#define HWIO_QTMR_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_QTMR_CNTP_CVAL_LO_ADDR                       (BASE      + 0x00000020)
#define HWIO_QTMR_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_QTMR_CNTP_CVAL_LO_IN          \
        in_dword_masked(HWIO_QTMR_CNTP_CVAL_LO_ADDR, HWIO_QTMR_CNTP_CVAL_LO_RMSK)
#define HWIO_QTMR_CNTP_CVAL_LO_INM(BAE,m)      \
        in_dword_masked(HWIO_QTMR_CNTP_CVAL_LO_ADDR, m)
#define HWIO_QTMR_CNTP_CVAL_LO_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTP_CVAL_LO_ADDR,v)
#define HWIO_QTMR_CNTP_CVAL_LO_OUTM(BAE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTP_CVAL_LO_ADDR,m,v,HWIO_QTMR_CNTP_CVAL_LO_IN)
#define HWIO_QTMR_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_QTMR_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_QTMR_CNTP_CVAL_HI_ADDR                       (BASE      + 0x00000024)
#define HWIO_QTMR_CNTP_CVAL_HI_RMSK                       0xffffff
#define HWIO_QTMR_CNTP_CVAL_HI_IN          \
        in_dword_masked(HWIO_QTMR_CNTP_CVAL_HI_ADDR, HWIO_QTMR_CNTP_CVAL_HI_RMSK)
#define HWIO_QTMR_CNTP_CVAL_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTP_CVAL_HI_ADDR, m)
#define HWIO_QTMR_CNTP_CVAL_HI_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTP_CVAL_HI_ADDR,v)
#define HWIO_QTMR_CNTP_CVAL_HI_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTP_CVAL_HI_ADDR,m,v,HWIO_QTMR_CNTP_CVAL_HI_IN)
#define HWIO_QTMR_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK          0xffffff
#define HWIO_QTMR_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_QTMR_CNTP_TVAL_ADDR                          (BASE      + 0x00000028)
#define HWIO_QTMR_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_QTMR_CNTP_TVAL_IN          \
        in_dword_masked(HWIO_QTMR_CNTP_TVAL_ADDR, HWIO_QTMR_CNTP_TVAL_RMSK)
#define HWIO_QTMR_CNTP_TVAL_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTP_TVAL_ADDR, m)
#define HWIO_QTMR_CNTP_TVAL_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTP_TVAL_ADDR,v)
#define HWIO_QTMR_CNTP_TVAL_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTP_TVAL_ADDR,m,v,HWIO_QTMR_CNTP_TVAL_IN)
#define HWIO_QTMR_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_QTMR_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_QTMR_CNTP_CTL_ADDR                           (BASE      + 0x0000002c)
#define HWIO_QTMR_CNTP_CTL_RMSK                                  0x7
#define HWIO_QTMR_CNTP_CTL_IN          \
        in_dword_masked(HWIO_QTMR_CNTP_CTL_ADDR, HWIO_QTMR_CNTP_CTL_RMSK)
#define HWIO_QTMR_CNTP_CTL_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTP_CTL_ADDR, m)
#define HWIO_QTMR_CNTP_CTL_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTP_CTL_ADDR,v)
#define HWIO_QTMR_CNTP_CTL_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTP_CTL_ADDR,m,v,HWIO_QTMR_CNTP_CTL_IN)
#define HWIO_QTMR_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_QTMR_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_QTMR_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_QTMR_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_QTMR_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_QTMR_CNTP_CTL_EN_SHFT                               0x0

#define HWIO_QTMR_CNTV_CVAL_LO_ADDR                       (BASE      + 0x00000030)
#define HWIO_QTMR_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_QTMR_CNTV_CVAL_LO_IN          \
        in_dword_masked(HWIO_QTMR_CNTV_CVAL_LO_ADDR, HWIO_QTMR_CNTV_CVAL_LO_RMSK)
#define HWIO_QTMR_CNTV_CVAL_LO_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTV_CVAL_LO_ADDR, m)
#define HWIO_QTMR_CNTV_CVAL_LO_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTV_CVAL_LO_ADDR,v)
#define HWIO_QTMR_CNTV_CVAL_LO_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTV_CVAL_LO_ADDR,m,v,HWIO_QTMR_CNTV_CVAL_LO_IN)
#define HWIO_QTMR_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_QTMR_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_QTMR_CNTV_CVAL_HI_ADDR                       (BASE      + 0x00000034)
#define HWIO_QTMR_CNTV_CVAL_HI_RMSK                       0xffffff
#define HWIO_QTMR_CNTV_CVAL_HI_IN          \
        in_dword_masked(HWIO_QTMR_CNTV_CVAL_HI_ADDR, HWIO_QTMR_CNTV_CVAL_HI_RMSK)
#define HWIO_QTMR_CNTV_CVAL_HI_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTV_CVAL_HI_ADDR, m)
#define HWIO_QTMR_CNTV_CVAL_HI_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTV_CVAL_HI_ADDR,v)
#define HWIO_QTMR_CNTV_CVAL_HI_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTV_CVAL_HI_ADDR,m,v,HWIO_QTMR_CNTV_CVAL_HI_IN)
#define HWIO_QTMR_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK          0xffffff
#define HWIO_QTMR_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_QTMR_CNTV_TVAL_ADDR                          (BASE      + 0x00000038)
#define HWIO_QTMR_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_QTMR_CNTV_TVAL_IN          \
        in_dword_masked(HWIO_QTMR_CNTV_TVAL_ADDR, HWIO_QTMR_CNTV_TVAL_RMSK)
#define HWIO_QTMR_CNTV_TVAL_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTV_TVAL_ADDR, m)
#define HWIO_QTMR_CNTV_TVAL_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTV_TVAL_ADDR,v)
#define HWIO_QTMR_CNTV_TVAL_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTV_TVAL_ADDR,m,v,HWIO_QTMR_CNTV_TVAL_IN)
#define HWIO_QTMR_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_QTMR_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_QTMR_CNTV_CTL_ADDR                           (BASE      + 0x0000003c)
#define HWIO_QTMR_CNTV_CTL_RMSK                                  0x7
#define HWIO_QTMR_CNTV_CTL_IN          \
        in_dword_masked(HWIO_QTMR_CNTV_CTL_ADDR, HWIO_QTMR_CNTV_CTL_RMSK)
#define HWIO_QTMR_CNTV_CTL_INM(BASE,m)      \
        in_dword_masked(HWIO_QTMR_CNTV_CTL_ADDR, m)
#define HWIO_QTMR_CNTV_CTL_OUT(BASE,v)      \
        out_dword(HWIO_QTMR_CNTV_CTL_ADDR,v)
#define HWIO_QTMR_CNTV_CTL_OUTM(BASE,m,v) \
        out_dword_masked_ns(HWIO_QTMR_CNTV_CTL_ADDR,m,v,HWIO_QTMR_CNTV_CTL_IN)
#define HWIO_QTMR_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_QTMR_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_QTMR_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_QTMR_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_QTMR_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_QTMR_CNTV_CTL_EN_SHFT                               0x0

#define HWIO_QTMR_VERSION_ADDR                            (BASE      + 0x00000fd0)
#define HWIO_QTMR_VERSION_RMSK                            0xffffffff
#define HWIO_QTMR_VERSION_IN          \
        in_dword_masked(HWIO_QTMR_VERSION_ADDR, HWIO_QTMR_VERSION_RMSK)
#define HWIO_QTMR_VERSION_INM(m)      \
        in_dword_masked(HWIO_QTMR_VERSION_ADDR, m)
#define HWIO_QTMR_VERSION_MAJOR_BMSK                      0xf0000000
#define HWIO_QTMR_VERSION_MAJOR_SHFT                            0x1c
#define HWIO_QTMR_VERSION_MINOR_BMSK                       0xfff0000
#define HWIO_QTMR_VERSION_MINOR_SHFT                            0x10
#define HWIO_QTMR_VERSION_STEP_BMSK                           0xffff
#define HWIO_QTMR_VERSION_STEP_SHFT                              0x0
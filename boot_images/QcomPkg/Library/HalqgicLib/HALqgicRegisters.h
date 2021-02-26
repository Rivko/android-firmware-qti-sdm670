#ifndef __HAL_HAL_QGIC_REGISTERS_H__
#define __HAL_HAL_QGIC_REGISTERS_H__


/*
===========================================================================

FILE:         HALqgicRegisters.h

DESCRIPTION:  
  This is the hardware abstraction layer interface for the Qualcomm
  Generic Interrupt Controller block.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
01/15/10   gfr     Created.

===========================================================================
             Copyright (c) 2010 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 * QGIC Distributor section base address definitions
 */

static UINTN nHalGICDBaseAddress;

/*
 * QGIC CPU Interface section base address definitions
 */

static UINTN nHalGICCBaseAddress;

/*
 * QGIC Hypervisor section base address definitions
 */

static UINTN nHalGICHBaseAddress;

/*
 * QGIC Virtualization section base address definitions
 */

static UINTN nHalGICVBaseAddress;



/*
 *  Distributor Register Definition
 */

#define HWIO_GICD_CTLR_ADDR                                              (nHalGICDBaseAddress   + 00000000)
#define HWIO_GICD_CTLR_RMSK                                                     0x3
#define HWIO_GICD_CTLR_SHFT                                                       0
#define HWIO_GICD_CTLR_IN                                                \
        in_dword_masked(HWIO_GICD_CTLR_ADDR, HWIO_GICD_CTLR_RMSK)
#define HWIO_GICD_CTLR_INM(m)                                            \
        in_dword_masked(HWIO_GICD_CTLR_ADDR, m)
#define HWIO_GICD_CTLR_OUT(v)                                            \
        out_dword(HWIO_GICD_CTLR_ADDR,v)
#define HWIO_GICD_CTLR_OUTM(m,v)                                         \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_CTLR_ADDR,m,v,HWIO_GICD_CTLR_IN); \
        HWIO_INTFREE()
#define HWIO_GICD_CTLR_ENABLE_NS_BMSK                                           0x2
#define HWIO_GICD_CTLR_ENABLE_NS_SHFT                                           0x1
#define HWIO_GICD_CTLR_ENABLE_BMSK                                              0x1
#define HWIO_GICD_CTLR_ENABLE_SHFT                                                0

#define HWIO_GICD_TYPER_ADDR                                             (nHalGICDBaseAddress   + 0x00000004)
#define HWIO_GICD_TYPER_RMSK                                                 0xffff
#define HWIO_GICD_TYPER_SHFT                                                      0
#define HWIO_GICD_TYPER_IN                                               \
        in_dword_masked(HWIO_GICD_TYPER_ADDR, HWIO_GICD_TYPER_RMSK)
#define HWIO_GICD_TYPER_INM(m)                                           \
        in_dword_masked(HWIO_GICD_TYPER_ADDR, m)
#define HWIO_GICD_TYPER_LSPI_BMSK                                            0xf800
#define HWIO_GICD_TYPER_LSPI_SHFT                                               0xb
#define HWIO_GICD_TYPER_TZ_BMSK                                               0x400
#define HWIO_GICD_TYPER_TZ_SHFT                                                 0xa
#define HWIO_GICD_TYPER_CPU_NUM_BMSK                                           0xe0
#define HWIO_GICD_TYPER_CPU_NUM_SHFT                                            0x5
#define HWIO_GICD_TYPER_IT_LINES_BMSK                                          0x1f
#define HWIO_GICD_TYPER_IT_LINES_SHFT                                             0

#define HWIO_GICD_IIDR_ADDR                                              (nHalGICDBaseAddress   + 0x00000008)
#define HWIO_GICD_IIDR_RMSK                                                  0xffff
#define HWIO_GICD_IIDR_SHFT                                                       0
#define HWIO_GICD_IIDR_IN                                                \
        in_dword_masked(HWIO_GICD_IIDR_ADDR, HWIO_GICD_IIDR_RMSK)
#define HWIO_GICD_IIDR_INM(m)                                            \
        in_dword_masked(HWIO_GICD_IIDR_ADDR, m)
#define HWIO_GICD_IIDR_REVISION_BMSK                                         0xf000
#define HWIO_GICD_IIDR_REVISION_SHFT                                            0xc
#define HWIO_GICD_IIDR_IMPLEMENTOR_BMSK                                       0xfff
#define HWIO_GICD_IIDR_IMPLEMENTOR_SHFT                                           0

#define HWIO_GICD_ANSACR_ADDR                                            (nHalGICDBaseAddress   + 0x00000020)
#define HWIO_GICD_ANSACR_RMSK                                                   0x1
#define HWIO_GICD_ANSACR_SHFT                                                     0
#define HWIO_GICD_ANSACR_IN                                              \
        in_dword_masked(HWIO_GICD_ANSACR_ADDR, HWIO_GICD_ANSACR_RMSK)
#define HWIO_GICD_ANSACR_INM(m)                                          \
        in_dword_masked(HWIO_GICD_ANSACR_ADDR, m)
#define HWIO_GICD_ANSACR_OUT(v)                                          \
        out_dword(HWIO_GICD_ANSACR_ADDR,v)
#define HWIO_GICD_ANSACR_OUTM(m,v)                                       \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_ANSACR_ADDR,m,v,HWIO_GICD_ANSACR_IN); \
        HWIO_INTFREE()
#define HWIO_GICD_ANSACR_GICD_CGCR_BMSK                                         0x1
#define HWIO_GICD_ANSACR_GICD_CGCR_SHFT                                           0

#define HWIO_GICD_CGCR_ADDR                                              (nHalGICDBaseAddress   + 0x00000024)
#define HWIO_GICD_CGCR_RMSK                                              0xffffffff
#define HWIO_GICD_CGCR_SHFT                                                       0
#define HWIO_GICD_CGCR_IN                                                \
        in_dword_masked(HWIO_GICD_CGCR_ADDR, HWIO_GICD_CGCR_RMSK)
#define HWIO_GICD_CGCR_INM(m)                                            \
        in_dword_masked(HWIO_GICD_CGCR_ADDR, m)
#define HWIO_GICD_CGCR_OUT(v)                                            \
        out_dword(HWIO_GICD_CGCR_ADDR,v)
#define HWIO_GICD_CGCR_OUTM(m,v)                                         \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_CGCR_ADDR,m,v,HWIO_GICD_CGCR_IN); \
        HWIO_INTFREE()
#define HWIO_GICD_CGCR_TOP_BMSK                                             0x10000
#define HWIO_GICD_CGCR_TOP_SHFT                                                0x10
#define HWIO_GICD_CGCR_DI_SGI_STATE_BMSK                                        0x8
#define HWIO_GICD_CGCR_DI_SGI_STATE_SHFT                                        0x3
#define HWIO_GICD_CGCR_DI_PPI_SPI_STATE_BMSK                                    0x4
#define HWIO_GICD_CGCR_DI_PPI_SPI_STATE_SHFT                                    0x2
#define HWIO_GICD_CGCR_DI_DEMET_BMSK                                            0x2
#define HWIO_GICD_CGCR_DI_DEMET_SHFT                                            0x1
#define HWIO_GICD_CGCR_DI_RD_BMSK                                               0x1
#define HWIO_GICD_CGCR_DI_RD_SHFT                                                 0

#define HWIO_GICD_ISRn_ADDR(n)                                           (nHalGICDBaseAddress   + 0x00000080 + 4 * (n))
#define HWIO_GICD_ISRn_RMSK                                              0xffffffff
#define HWIO_GICD_ISRn_SHFT                                                       0
#define HWIO_GICD_ISRn_MAXn                                                     0x8
#define HWIO_GICD_ISRn_INI(n) \
        in_dword(HWIO_GICD_ISRn_ADDR(n))
#define HWIO_GICD_ISRn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_ISRn_ADDR(n), mask)
#define HWIO_GICD_ISRn_OUTI(n,val) \
        out_dword(HWIO_GICD_ISRn_ADDR(n),val)
#define HWIO_GICD_ISRn_OUTMI(n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_ISRn_ADDR(n),mask,val,HWIO_GICD_ISRn_INI(n));\
        HWIO_INTFREE()
#define HWIO_GICD_ISRn_INT_NS_BMSK                                       0xffffffff
#define HWIO_GICD_ISRn_INT_NS_SHFT                                                0

#define HWIO_GICD_ISENABLERn_ADDR(n)                                     (nHalGICDBaseAddress   + 0x00000100 + 4 * (n))
#define HWIO_GICD_ISENABLERn_RMSK                                        0xffffffff
#define HWIO_GICD_ISENABLERn_SHFT                                                 0
#define HWIO_GICD_ISENABLERn_MAXn                                               0x8
#define HWIO_GICD_ISENABLERn_INI(n) \
        in_dword(HWIO_GICD_ISENABLERn_ADDR(n))
#define HWIO_GICD_ISENABLERn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_ISENABLERn_ADDR(n), mask)
#define HWIO_GICD_ISENABLERn_OUTI(n,val) \
        out_dword(HWIO_GICD_ISENABLERn_ADDR(n),val)
#define HWIO_GICD_ISENABLERn_OUTMI(n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_ISENABLERn_ADDR(n),mask,val,HWIO_GICD_ISENABLERn_INI(n));\
        HWIO_INTFREE()
#define HWIO_GICD_ISENABLERn_INT_BMSK                                    0xffffffff
#define HWIO_GICD_ISENABLERn_INT_SHFT                                             0

#define HWIO_GICD_ICENABLERn_ADDR(n)                                     (nHalGICDBaseAddress   + 0x00000180 + 4 * (n))
#define HWIO_GICD_ICENABLERn_RMSK                                        0xffffffff
#define HWIO_GICD_ICENABLERn_SHFT                                                 0
#define HWIO_GICD_ICENABLERn_MAXn                                               0x8
#define HWIO_GICD_ICENABLERn_INI(n) \
        in_dword(HWIO_GICD_ICENABLERn_ADDR(n))
#define HWIO_GICD_ICENABLERn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_ICENABLERn_ADDR(n), mask)
#define HWIO_GICD_ICENABLERn_OUTI(n,val) \
        out_dword(HWIO_GICD_ICENABLERn_ADDR(n),val)
#define HWIO_GICD_ICENABLERn_OUTMI(n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_ICENABLERn_ADDR(n),mask,val,HWIO_GICD_ICENABLERn_INI(n));\
        HWIO_INTFREE()
#define HWIO_GICD_ICENABLERn_INT_BMSK                                    0xffffffff
#define HWIO_GICD_ICENABLERn_INT_SHFT                                             0

#define HWIO_GICD_ISPENDRn_ADDR(n)                                       (nHalGICDBaseAddress   + 0x00000200 + 4 * (n))
#define HWIO_GICD_ISPENDRn_RMSK                                          0xffffffff
#define HWIO_GICD_ISPENDRn_SHFT                                                   0
#define HWIO_GICD_ISPENDRn_MAXn                                                 0x8
#define HWIO_GICD_ISPENDRn_INI(n) \
        in_dword(HWIO_GICD_ISPENDRn_ADDR(n))
#define HWIO_GICD_ISPENDRn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_ISPENDRn_ADDR(n), mask)
#define HWIO_GICD_ISPENDRn_OUTI(n,val) \
        out_dword(HWIO_GICD_ISPENDRn_ADDR(n),val)
#define HWIO_GICD_ISPENDRn_OUTMI(n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_ISPENDRn_ADDR(n),mask,val,HWIO_GICD_ISPENDRn_INI(n));\
        HWIO_INTFREE()
#define HWIO_GICD_ISPENDRn_INT_BMSK                                      0xffffffff
#define HWIO_GICD_ISPENDRn_INT_SHFT                                               0

#define HWIO_GICD_ICPENDRn_ADDR(n)                                       (nHalGICDBaseAddress   + 0x00000280 + 4 * (n))
#define HWIO_GICD_ICPENDRn_RMSK                                          0xffffffff
#define HWIO_GICD_ICPENDRn_SHFT                                                   0
#define HWIO_GICD_ICPENDRn_MAXn                                                 0x8
#define HWIO_GICD_ICPENDRn_INI(n) \
        in_dword(HWIO_GICD_ICPENDRn_ADDR(n))
#define HWIO_GICD_ICPENDRn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_ICPENDRn_ADDR(n), mask)
#define HWIO_GICD_ICPENDRn_OUTI(n,val) \
        out_dword(HWIO_GICD_ICPENDRn_ADDR(n),val)
#define HWIO_GICD_ICPENDRn_OUTMI(n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_ICPENDRn_ADDR(n),mask,val,HWIO_GICD_ICPENDRn_INI(n));\
        HWIO_INTFREE()
#define HWIO_GICD_ICPENDRn_INT_BMSK                                      0xffffffff
#define HWIO_GICD_ICPENDRn_INT_SHFT                                               0

#define HWIO_GICD_IACTIVERn_ADDR(n)                                      (nHalGICDBaseAddress   + 0x00000300 + 4 * (n))
#define HWIO_GICD_IACTIVERn_RMSK                                         0xffffffff
#define HWIO_GICD_IACTIVERn_SHFT                                                  0
#define HWIO_GICD_IACTIVERn_MAXn                                                0x8
#define HWIO_GICD_IACTIVERn_INI(n) \
        in_dword(HWIO_GICD_IACTIVERn_ADDR(n))
#define HWIO_GICD_IACTIVERn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_IACTIVERn_ADDR(n), mask)
#define HWIO_GICD_IACTIVERn_OUTI(n,val) \
        out_dword(HWIO_GICD_IACTIVERn_ADDR(n),val)
#define HWIO_GICD_IACTIVERn_INT_BMSK                                     0xffffffff
#define HWIO_GICD_IACTIVERn_INT_SHFT                                              0

#define HWIO_GICD_IPRIORITYRn_ADDR(n)                                    (nHalGICDBaseAddress   + 0x00000400 + 4 * (n))
#define HWIO_GICD_IPRIORITYRn_RMSK                                       0xffffffff
#define HWIO_GICD_IPRIORITYRn_SHFT                                                0
#define HWIO_GICD_IPRIORITYRn_MAXn                                             0x47
#define HWIO_GICD_IPRIORITYRn_INI(n) \
        in_dword(HWIO_GICD_IPRIORITYRn_ADDR(n))
#define HWIO_GICD_IPRIORITYRn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_IPRIORITYRn_ADDR(n), mask)
#define HWIO_GICD_IPRIORITYRn_OUTI(n,val) \
        out_dword(HWIO_GICD_IPRIORITYRn_ADDR(n),val)
#define HWIO_GICD_IPRIORITYRn_OUTMI(n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_IPRIORITYRn_ADDR(n),mask,val,HWIO_GICD_IPRIORITYRn_INI(n));\
        HWIO_INTFREE()
#define HWIO_GICD_IPRIORITYRn_INT3_BMSK                                  0xff000000
#define HWIO_GICD_IPRIORITYRn_INT3_SHFT                                        0x18
#define HWIO_GICD_IPRIORITYRn_INT2_BMSK                                    0xff0000
#define HWIO_GICD_IPRIORITYRn_INT2_SHFT                                        0x10
#define HWIO_GICD_IPRIORITYRn_INT1_BMSK                                      0xff00
#define HWIO_GICD_IPRIORITYRn_INT1_SHFT                                         0x8
#define HWIO_GICD_IPRIORITYRn_INT0_BMSK                                        0xff
#define HWIO_GICD_IPRIORITYRn_INT0_SHFT                                           0

#define HWIO_GICD_ITARGETSRn_ADDR(n)                                     (nHalGICDBaseAddress   + 0x00000800 + 4 * (n))
#define HWIO_GICD_ITARGETSRn_RMSK                                        0xffffffff
#define HWIO_GICD_ITARGETSRn_SHFT                                                 0
#define HWIO_GICD_ITARGETSRn_MAXn                                              0x47
#define HWIO_GICD_ITARGETSRn_INI(n) \
        in_dword(HWIO_GICD_ITARGETSRn_ADDR(n))
#define HWIO_GICD_ITARGETSRn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_ITARGETSRn_ADDR(n), mask)
#define HWIO_GICD_ITARGETSRn_OUTI(n,val) \
        out_dword(HWIO_GICD_ITARGETSRn_ADDR(n),val)
#define HWIO_GICD_ITARGETSRn_OUTMI(n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_ITARGETSRn_ADDR(n),mask,val,HWIO_GICD_ITARGETSRn_INI(n));\
        HWIO_INTFREE()
#define HWIO_GICD_ITARGETSRn_INT3_BMSK                                   0xff000000
#define HWIO_GICD_ITARGETSRn_INT3_SHFT                                         0x18
#define HWIO_GICD_ITARGETSRn_INT2_BMSK                                     0xff0000
#define HWIO_GICD_ITARGETSRn_INT2_SHFT                                         0x10
#define HWIO_GICD_ITARGETSRn_INT1_BMSK                                       0xff00
#define HWIO_GICD_ITARGETSRn_INT1_SHFT                                          0x8
#define HWIO_GICD_ITARGETSRn_INT0_BMSK                                         0xff
#define HWIO_GICD_ITARGETSRn_INT0_SHFT                                            0

#define HWIO_GICD_ICFGRn_ADDR(n)                                         (nHalGICDBaseAddress   + 0x00000c00 + 4 * (n))
#define HWIO_GICD_ICFGRn_RMSK                                            0xffffffff
#define HWIO_GICD_ICFGRn_SHFT                                                     0
#define HWIO_GICD_ICFGRn_MAXn                                                  0x11
#define HWIO_GICD_ICFGRn_INI(n) \
        in_dword(HWIO_GICD_ICFGRn_ADDR(n))
#define HWIO_GICD_ICFGRn_INMI(n,mask) \
        in_dword_masked(HWIO_GICD_ICFGRn_ADDR(n), mask)
#define HWIO_GICD_ICFGRn_OUTI(n,val) \
        out_dword(HWIO_GICD_ICFGRn_ADDR(n),val)
#define HWIO_GICD_ICFGRn_OUTMI(n,mask,val) \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICD_ICFGRn_ADDR(n),mask,val,HWIO_GICD_ICFGRn_INI(n));\
        HWIO_INTFREE()
#define HWIO_GICD_ICFGRn_INT15_BMSK                                      0xc0000000
#define HWIO_GICD_ICFGRn_INT15_SHFT                                            0x1e
#define HWIO_GICD_ICFGRn_INT14_BMSK                                      0x30000000
#define HWIO_GICD_ICFGRn_INT14_SHFT                                            0x1c
#define HWIO_GICD_ICFGRn_INT13_BMSK                                       0xc000000
#define HWIO_GICD_ICFGRn_INT13_SHFT                                            0x1a
#define HWIO_GICD_ICFGRn_INT12_BMSK                                       0x3000000
#define HWIO_GICD_ICFGRn_INT12_SHFT                                            0x18
#define HWIO_GICD_ICFGRn_INT11_BMSK                                        0xc00000
#define HWIO_GICD_ICFGRn_INT11_SHFT                                            0x16
#define HWIO_GICD_ICFGRn_INT10_BMSK                                        0x300000
#define HWIO_GICD_ICFGRn_INT10_SHFT                                            0x14
#define HWIO_GICD_ICFGRn_INT9_BMSK                                          0xc0000
#define HWIO_GICD_ICFGRn_INT9_SHFT                                             0x12
#define HWIO_GICD_ICFGRn_INT8_BMSK                                          0x30000
#define HWIO_GICD_ICFGRn_INT8_SHFT                                             0x10
#define HWIO_GICD_ICFGRn_INT7_BMSK                                           0xc000
#define HWIO_GICD_ICFGRn_INT7_SHFT                                              0xe
#define HWIO_GICD_ICFGRn_INT6_BMSK                                           0x3000
#define HWIO_GICD_ICFGRn_INT6_SHFT                                              0xc
#define HWIO_GICD_ICFGRn_INT5_BMSK                                            0xc00
#define HWIO_GICD_ICFGRn_INT5_SHFT                                              0xa
#define HWIO_GICD_ICFGRn_INT4_BMSK                                            0x300
#define HWIO_GICD_ICFGRn_INT4_SHFT                                              0x8
#define HWIO_GICD_ICFGRn_INT3_BMSK                                             0xc0
#define HWIO_GICD_ICFGRn_INT3_SHFT                                              0x6
#define HWIO_GICD_ICFGRn_INT2_BMSK                                             0x30
#define HWIO_GICD_ICFGRn_INT2_SHFT                                              0x4
#define HWIO_GICD_ICFGRn_INT1_BMSK                                              0xc
#define HWIO_GICD_ICFGRn_INT1_SHFT                                              0x2
#define HWIO_GICD_ICFGRn_INT0_BMSK                                              0x3
#define HWIO_GICD_ICFGRn_INT0_SHFT                                                0

#define HWIO_GICD_SGIR_ADDR                                              (nHalGICDBaseAddress   + 0x00000f00)
#define HWIO_GICD_SGIR_RMSK                                              0xffffffff
#define HWIO_GICD_SGIR_SHFT                                                       0
#define HWIO_GICD_SGIR_OUT(v)                                            \
        out_dword(HWIO_GICD_SGIR_ADDR,v)
#define HWIO_GICD_SGIR_T_FILTER_BMSK                                      0x3000000
#define HWIO_GICD_SGIR_T_FILTER_SHFT                                           0x18
#define HWIO_GICD_SGIR_T_LIST_BMSK                                         0xff0000
#define HWIO_GICD_SGIR_T_LIST_SHFT                                             0x10
#define HWIO_GICD_SGIR_SATT_BMSK                                             0x8000
#define HWIO_GICD_SGIR_SATT_SHFT                                                0xf
#define HWIO_GICD_SGIR_INT_ID_BMSK                                              0xf
#define HWIO_GICD_SGIR_INT_ID_SHFT                                                0

#define HWIO_GICD_PIDR0_ADDR                                             (nHalGICDBaseAddress   + 0x00000fd0)
#define HWIO_GICD_PIDR0_RMSK                                                   0xff
#define HWIO_GICD_PIDR0_SHFT                                                      0
#define HWIO_GICD_PIDR0_IN                                               \
        in_dword_masked(HWIO_GICD_PIDR0_ADDR, HWIO_GICD_PIDR0_RMSK)
#define HWIO_GICD_PIDR0_INM(m)                                           \
        in_dword_masked(HWIO_GICD_PIDR0_ADDR, m)
#define HWIO_GICD_PIDR0_PART_NUM_BMSK                                          0xff
#define HWIO_GICD_PIDR0_PART_NUM_SHFT                                             0

#define HWIO_GICD_PIDR1_ADDR                                             (nHalGICDBaseAddress   + 0x00000fd4)
#define HWIO_GICD_PIDR1_RMSK                                                   0xff
#define HWIO_GICD_PIDR1_SHFT                                                      0
#define HWIO_GICD_PIDR1_IN                                               \
        in_dword_masked(HWIO_GICD_PIDR1_ADDR, HWIO_GICD_PIDR1_RMSK)
#define HWIO_GICD_PIDR1_INM(m)                                           \
        in_dword_masked(HWIO_GICD_PIDR1_ADDR, m)
#define HWIO_GICD_PIDR1_DESIGNER_BMSK                                          0xf0
#define HWIO_GICD_PIDR1_DESIGNER_SHFT                                           0x4
#define HWIO_GICD_PIDR1_PART_NUM_BMSK                                           0xf
#define HWIO_GICD_PIDR1_PART_NUM_SHFT                                             0

#define HWIO_GICD_PIDR2_ADDR                                             (nHalGICDBaseAddress   + 0x00000fd8)
#define HWIO_GICD_PIDR2_RMSK                                                   0x1f
#define HWIO_GICD_PIDR2_SHFT                                                      0
#define HWIO_GICD_PIDR2_IN                                               \
        in_dword_masked(HWIO_GICD_PIDR2_ADDR, HWIO_GICD_PIDR2_RMSK)
#define HWIO_GICD_PIDR2_INM(m)                                           \
        in_dword_masked(HWIO_GICD_PIDR2_ADDR, m)
#define HWIO_GICD_PIDR2_ARCH_VERSION_BMSK                                      0x10
#define HWIO_GICD_PIDR2_ARCH_VERSION_SHFT                                       0x4
#define HWIO_GICD_PIDR2_USES_JEP_CODE_BMSK                                      0x8
#define HWIO_GICD_PIDR2_USES_JEP_CODE_SHFT                                      0x3
#define HWIO_GICD_PIDR2_DESIGNER_BMSK                                           0x7
#define HWIO_GICD_PIDR2_DESIGNER_SHFT                                             0

#define HWIO_GICD_PIDR3_ADDR                                             (nHalGICDBaseAddress   + 0x00000fdc)
#define HWIO_GICD_PIDR3_RMSK                                                   0xf0
#define HWIO_GICD_PIDR3_SHFT                                                      0
#define HWIO_GICD_PIDR3_IN                                               \
        in_dword_masked(HWIO_GICD_PIDR3_ADDR, HWIO_GICD_PIDR3_RMSK)
#define HWIO_GICD_PIDR3_INM(m)                                           \
        in_dword_masked(HWIO_GICD_PIDR3_ADDR, m)
#define HWIO_GICD_PIDR3_REVISION_BMSK                                          0xf0
#define HWIO_GICD_PIDR3_REVISION_SHFT                                           0x4

#define HWIO_GICD_PIDR4_ADDR                                             (nHalGICDBaseAddress   + 0x00000fe0)
#define HWIO_GICD_PIDR4_RMSK                                                    0xf
#define HWIO_GICD_PIDR4_SHFT                                                      0
#define HWIO_GICD_PIDR4_IN                                               \
        in_dword_masked(HWIO_GICD_PIDR4_ADDR, HWIO_GICD_PIDR4_RMSK)
#define HWIO_GICD_PIDR4_INM(m)                                           \
        in_dword_masked(HWIO_GICD_PIDR4_ADDR, m)
#define HWIO_GICD_PIDR4_DESIGNER_BMSK                                           0xf
#define HWIO_GICD_PIDR4_DESIGNER_SHFT                                             0

#define HWIO_GICD_PIDR5_ADDR                                             (nHalGICDBaseAddress   + 0x00000fe4)
#define HWIO_GICD_PIDR5_RMSK                                                      0
#define HWIO_GICD_PIDR5_SHFT                                                      0
#define HWIO_GICD_PIDR5_IN                                               \
        in_dword_masked(HWIO_GICD_PIDR5_ADDR, HWIO_GICD_PIDR5_RMSK)
#define HWIO_GICD_PIDR5_INM(m)                                           \
        in_dword_masked(HWIO_GICD_PIDR5_ADDR, m)

#define HWIO_GICD_PIDR6_ADDR                                             (nHalGICDBaseAddress   + 0x00000fe8)
#define HWIO_GICD_PIDR6_RMSK                                                      0
#define HWIO_GICD_PIDR6_SHFT                                                      0
#define HWIO_GICD_PIDR6_IN                                               \
        in_dword_masked(HWIO_GICD_PIDR6_ADDR, HWIO_GICD_PIDR6_RMSK)
#define HWIO_GICD_PIDR6_INM(m)                                           \
        in_dword_masked(HWIO_GICD_PIDR6_ADDR, m)

#define HWIO_GICD_PIDR7_ADDR                                             (nHalGICDBaseAddress   + 0x00000fec)
#define HWIO_GICD_PIDR7_RMSK                                                      0
#define HWIO_GICD_PIDR7_SHFT                                                      0
#define HWIO_GICD_PIDR7_IN                                               \
        in_dword_masked(HWIO_GICD_PIDR7_ADDR, HWIO_GICD_PIDR7_RMSK)
#define HWIO_GICD_PIDR7_INM(m)                                           \
        in_dword_masked(HWIO_GICD_PIDR7_ADDR, m)

#define HWIO_GICD_CIDR0_ADDR                                             (nHalGICDBaseAddress   + 0x00000ff0)
#define HWIO_GICD_CIDR0_RMSK                                                   0xff
#define HWIO_GICD_CIDR0_SHFT                                                      0
#define HWIO_GICD_CIDR0_IN                                               \
        in_dword_masked(HWIO_GICD_CIDR0_ADDR, HWIO_GICD_CIDR0_RMSK)
#define HWIO_GICD_CIDR0_INM(m)                                           \
        in_dword_masked(HWIO_GICD_CIDR0_ADDR, m)
#define HWIO_GICD_CIDR0_COMP_ID_0_BMSK                                         0xff
#define HWIO_GICD_CIDR0_COMP_ID_0_SHFT                                            0

#define HWIO_GICD_CIDR1_ADDR                                             (nHalGICDBaseAddress   + 0x00000ff4)
#define HWIO_GICD_CIDR1_RMSK                                                   0xff
#define HWIO_GICD_CIDR1_SHFT                                                      0
#define HWIO_GICD_CIDR1_IN                                               \
        in_dword_masked(HWIO_GICD_CIDR1_ADDR, HWIO_GICD_CIDR1_RMSK)
#define HWIO_GICD_CIDR1_INM(m)                                           \
        in_dword_masked(HWIO_GICD_CIDR1_ADDR, m)
#define HWIO_GICD_CIDR1_COMP_ID_1_BMSK                                         0xff
#define HWIO_GICD_CIDR1_COMP_ID_1_SHFT                                            0

#define HWIO_GICD_CIDR2_ADDR                                             (nHalGICDBaseAddress   + 0x00000ff8)
#define HWIO_GICD_CIDR2_RMSK                                                   0xff
#define HWIO_GICD_CIDR2_SHFT                                                      0
#define HWIO_GICD_CIDR2_IN                                               \
        in_dword_masked(HWIO_GICD_CIDR2_ADDR, HWIO_GICD_CIDR2_RMSK)
#define HWIO_GICD_CIDR2_INM(m)                                           \
        in_dword_masked(HWIO_GICD_CIDR2_ADDR, m)
#define HWIO_GICD_CIDR2_COMP_ID_2_BMSK                                         0xff
#define HWIO_GICD_CIDR2_COMP_ID_2_SHFT                                            0

#define HWIO_GICD_CIDR3_ADDR                                             (nHalGICDBaseAddress   + 0x00000ffc)
#define HWIO_GICD_CIDR3_RMSK                                                   0xff
#define HWIO_GICD_CIDR3_SHFT                                                      0
#define HWIO_GICD_CIDR3_IN                                               \
        in_dword_masked(HWIO_GICD_CIDR3_ADDR, HWIO_GICD_CIDR3_RMSK)
#define HWIO_GICD_CIDR3_INM(m)                                           \
        in_dword_masked(HWIO_GICD_CIDR3_ADDR, m)
#define HWIO_GICD_CIDR3_COMP_ID_3_BMSK                                         0xff
#define HWIO_GICD_CIDR3_COMP_ID_3_SHFT                                            0


/*
 *  CPU Interface Register Definition
 */

#define HWIO_GICC_CTLR_ADDR                                              (nHalGICCBaseAddress   + 0x00000000)
#define HWIO_GICC_CTLR_RMSK                                                    0x1f
#define HWIO_GICC_CTLR_SHFT                                                       0
#define HWIO_GICC_CTLR_IN                                                \
        in_dword_masked(HWIO_GICC_CTLR_ADDR, HWIO_GICC_CTLR_RMSK)
#define HWIO_GICC_CTLR_INM(m)                                            \
        in_dword_masked(HWIO_GICC_CTLR_ADDR, m)
#define HWIO_GICC_CTLR_OUT(v)                                            \
        out_dword(HWIO_GICC_CTLR_ADDR,v)
#define HWIO_GICC_CTLR_OUTM(m,v)                                         \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICC_CTLR_ADDR,m,v,HWIO_GICC_CTLR_IN); \
        HWIO_INTFREE()
#define HWIO_GICC_CTLR_SBPR_BMSK                                               0x10
#define HWIO_GICC_CTLR_SBPR_SHFT                                                0x4
#define HWIO_GICC_CTLR_S_DEST_BMSK                                              0x8
#define HWIO_GICC_CTLR_S_DEST_SHFT                                              0x3
#define HWIO_GICC_CTLR_S_ACK_BMSK                                               0x4
#define HWIO_GICC_CTLR_S_ACK_SHFT                                               0x2
#define HWIO_GICC_CTLR_ENABLE_NS_BMSK                                           0x2
#define HWIO_GICC_CTLR_ENABLE_NS_SHFT                                           0x1
#define HWIO_GICC_CTLR_ENABLE_BMSK                                              0x1
#define HWIO_GICC_CTLR_ENABLE_SHFT                                                0

#define HWIO_GICC_PMR_ADDR                                               (nHalGICCBaseAddress   + 0x00000004)
#define HWIO_GICC_PMR_RMSK                                                     0xff
#define HWIO_GICC_PMR_SHFT                                                        0
#define HWIO_GICC_PMR_IN                                                 \
        in_dword_masked(HWIO_GICC_PMR_ADDR, HWIO_GICC_PMR_RMSK)
#define HWIO_GICC_PMR_INM(m)                                             \
        in_dword_masked(HWIO_GICC_PMR_ADDR, m)
#define HWIO_GICC_PMR_OUT(v)                                             \
        out_dword(HWIO_GICC_PMR_ADDR,v)
#define HWIO_GICC_PMR_OUTM(m,v)                                          \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICC_PMR_ADDR,m,v,HWIO_GICC_PMR_IN); \
        HWIO_INTFREE()
#define HWIO_GICC_PMR_LEVEL_BMSK                                               0xff
#define HWIO_GICC_PMR_LEVEL_SHFT                                                  0

#define HWIO_GICC_BPR_ADDR                                               (nHalGICCBaseAddress   + 0x00000008)
#define HWIO_GICC_BPR_RMSK                                                      0x7
#define HWIO_GICC_BPR_SHFT                                                        0
#define HWIO_GICC_BPR_IN                                                 \
        in_dword_masked(HWIO_GICC_BPR_ADDR, HWIO_GICC_BPR_RMSK)
#define HWIO_GICC_BPR_INM(m)                                             \
        in_dword_masked(HWIO_GICC_BPR_ADDR, m)
#define HWIO_GICC_BPR_OUT(v)                                             \
        out_dword(HWIO_GICC_BPR_ADDR,v)
#define HWIO_GICC_BPR_OUTM(m,v)                                          \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICC_BPR_ADDR,m,v,HWIO_GICC_BPR_IN); \
        HWIO_INTFREE()
#define HWIO_GICC_BPR_VAL_BMSK                                                  0x7
#define HWIO_GICC_BPR_VAL_SHFT                                                    0

#define HWIO_GICC_IAR_ADDR                                               (nHalGICCBaseAddress   + 0x0000000c)
#define HWIO_GICC_IAR_RMSK                                                   0x1fff
#define HWIO_GICC_IAR_SHFT                                                        0
#define HWIO_GICC_IAR_IN                                                 \
        in_dword_masked(HWIO_GICC_IAR_ADDR, HWIO_GICC_IAR_RMSK)
#define HWIO_GICC_IAR_INM(m)                                             \
        in_dword_masked(HWIO_GICC_IAR_ADDR, m)
#define HWIO_GICC_IAR_CPU_ID_BMSK                                            0x1c00
#define HWIO_GICC_IAR_CPU_ID_SHFT                                               0xa
#define HWIO_GICC_IAR_INT_ID_BMSK                                             0x3ff
#define HWIO_GICC_IAR_INT_ID_SHFT                                                 0

#define HWIO_GICC_EOIR_ADDR                                              (nHalGICCBaseAddress   + 0x00000010)
#define HWIO_GICC_EOIR_RMSK                                                  0x1fff
#define HWIO_GICC_EOIR_SHFT                                                       0
#define HWIO_GICC_EOIR_OUT(v)                                            \
        out_dword(HWIO_GICC_EOIR_ADDR,v)
#define HWIO_GICC_EOIR_CPU_ID_BMSK                                           0x1c00
#define HWIO_GICC_EOIR_CPU_ID_SHFT                                              0xa
#define HWIO_GICC_EOIR_INT_ID_BMSK                                            0x3ff
#define HWIO_GICC_EOIR_INT_ID_SHFT                                                0

#define HWIO_GICC_RPR_ADDR                                               (nHalGICCBaseAddress   + 0x00000014)
#define HWIO_GICC_RPR_RMSK                                                     0xff
#define HWIO_GICC_RPR_SHFT                                                        0
#define HWIO_GICC_RPR_IN                                                 \
        in_dword_masked(HWIO_GICC_RPR_ADDR, HWIO_GICC_RPR_RMSK)
#define HWIO_GICC_RPR_INM(m)                                             \
        in_dword_masked(HWIO_GICC_RPR_ADDR, m)
#define HWIO_GICC_RPR_VAL_BMSK                                                 0xff
#define HWIO_GICC_RPR_VAL_SHFT                                                    0

#define HWIO_GICC_HPPIR_ADDR                                             (nHalGICCBaseAddress   + 0x00000018)
#define HWIO_GICC_HPPIR_RMSK                                                 0x1fff
#define HWIO_GICC_HPPIR_SHFT                                                      0
#define HWIO_GICC_HPPIR_IN                                               \
        in_dword_masked(HWIO_GICC_HPPIR_ADDR, HWIO_GICC_HPPIR_RMSK)
#define HWIO_GICC_HPPIR_INM(m)                                           \
        in_dword_masked(HWIO_GICC_HPPIR_ADDR, m)
#define HWIO_GICC_HPPIR_CPU_ID_BMSK                                          0x1c00
#define HWIO_GICC_HPPIR_CPU_ID_SHFT                                             0xa
#define HWIO_GICC_HPPIR_INT_ID_BMSK                                           0x3ff
#define HWIO_GICC_HPPIR_INT_ID_SHFT                                               0

#define HWIO_GICC_ABPR_ADDR                                              (nHalGICCBaseAddress   + 0x0000001c)
#define HWIO_GICC_ABPR_RMSK                                                     0x7
#define HWIO_GICC_ABPR_SHFT                                                       0
#define HWIO_GICC_ABPR_IN                                                \
        in_dword_masked(HWIO_GICC_ABPR_ADDR, HWIO_GICC_ABPR_RMSK)
#define HWIO_GICC_ABPR_INM(m)                                            \
        in_dword_masked(HWIO_GICC_ABPR_ADDR, m)
#define HWIO_GICC_ABPR_OUT(v)                                            \
        out_dword(HWIO_GICC_ABPR_ADDR,v)
#define HWIO_GICC_ABPR_OUTM(m,v)                                         \
        HWIO_INTLOCK(); \
        out_dword_masked_ns(HWIO_GICC_ABPR_ADDR,m,v,HWIO_GICC_ABPR_IN); \
        HWIO_INTFREE()
#define HWIO_GICC_ABPR_VAL_BMSK                                                 0x7
#define HWIO_GICC_ABPR_VAL_SHFT                                                   0

#define HWIO_GICC_IIDR_ADDR                                              (nHalGICCBaseAddress   + 0x000000fc)
#define HWIO_GICC_IIDR_RMSK                                              0xffffffff
#define HWIO_GICC_IIDR_SHFT                                                       0
#define HWIO_GICC_IIDR_IN                                                \
        in_dword_masked(HWIO_GICC_IIDR_ADDR, HWIO_GICC_IIDR_RMSK)
#define HWIO_GICC_IIDR_INM(m)                                            \
        in_dword_masked(HWIO_GICC_IIDR_ADDR, m)
#define HWIO_GICC_IIDR_PART_NUM_BMSK                                     0xfff00000
#define HWIO_GICC_IIDR_PART_NUM_SHFT                                           0x14
#define HWIO_GICC_IIDR_ARCH_VERSION_BMSK                                    0xf0000
#define HWIO_GICC_IIDR_ARCH_VERSION_SHFT                                       0x10
#define HWIO_GICC_IIDR_REVISION_BMSK                                         0xf000
#define HWIO_GICC_IIDR_REVISION_SHFT                                            0xc
#define HWIO_GICC_IIDR_IMPLEMENTOR_BMSK                                       0xfff
#define HWIO_GICC_IIDR_IMPLEMENTOR_SHFT                                           0

#ifdef __cplusplus
}
#endif

#endif /* __HAL_HAL_QGIC_REGISTERS_H__ */




/*===========================================================================
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/

#ifndef __TEETEST_HAL_HWIO_H
#include <stddef.h>
#include <stdint.h>
#include "teetest_hwio.h"

/******************************************************************************
* HWIO MACRO Definitions
******************************************************************************/
/*
 * Macro to allow forcing an enum to 32 bits.  The argument should be
 * an identifier in the namespace of the enumeration in question, i.e.
 * for the clk HAL we might use HAL_ENUM_32BITS(CLK_xxx).
 */
#define HAL_ENUM_32BITS(x)                               HAL_##x##_FORCE32BITS = 0x7FFFFFFF
#define HWIO_ADDRI(hwiosym, index)                       __msmhwio_addri(hwiosym, index)
#define HWIO_OUT(hwiosym, val)                           __msmhwio_out(hwiosym, val)
#define HWIO_SHFT(hwio_regsym, hwio_fldsym)              __msmhwio_shft(hwio_regsym, hwio_fldsym)
#define HWIO_FMSK(hwio_regsym, hwio_fldsym)              __msmhwio_fmsk(hwio_regsym, hwio_fldsym)
#define HWIO_FVAL(io, field, val)                        (((val) << HWIO_SHFT(io, field)) & HWIO_FMSK(io, field))

/******************************************************************************
* Input/output port macros for memory mapped IO.
******************************************************************************/
#define inp(port)         (*((volatile uint8_t  *)(port)))
#define inpw(port)        (*((volatile uint16_t *)(port)))
#define inpdw(port)       (*((volatile uint32_t *)(port)))
#define inp64(port)       (*((volatile uint64_t *)(port)))

#define outp(port, val)   (*((volatile uint8_t  *)(port)) = ((uint8_t)  (val)))
#define outpw(port, val)  (*((volatile uint16_t *)(port)) = ((uint16_t) (val)))
#define outpdw(port, val) (*((volatile uint32_t *)(port)) = ((uint32_t) (val)))
#define outp64(port, val) (*((volatile uint64_t *)(port)) = ((uint64_t) (val)))

#define __msmhwio_addri(hwiosym, index)                         HWIO_##hwiosym##_ADDR(index)
#define __msmhwio_out(hwiosym, val)                             HWIO_##hwiosym##_OUT(val)
#define __msmhwio_fmsk(hwiosym, hwiofldsym)                     HWIO_##hwiosym##_##hwiofldsym##_BMSK
#define __msmhwio_shft(hwiosym, hwiofldsym)                     HWIO_##hwiosym##_##hwiofldsym##_SHFT
#define __outpdw(port, val)                                     (*((volatile uint32_t *) ((uintnt) port)) = ((uint32_t) (val)))
#define out_dword(addr, val)                                    __outpdw(addr,val)

#define HWIO_GICD_ISRn_ADDR(n)                    (nHalGICDBaseAddress   + 0x00000080 + 4 * (n))
#define HWIO_GICD_IACTIVERn_ADDR(n)               (nHalGICDBaseAddress   + 0x00000300 + 4 * (n))
#define HWIO_GICD_IPRIORITYRn_ADDR(n)             (nHalGICDBaseAddress   + 0x00000400 + 4 * (n))
#define HWIO_GICD_ITARGETSRn_ADDR(n)              (nHalGICDBaseAddress   + 0x00000800 + 4 * (n))
#define HWIO_GICD_ICPENDRn_ADDR(n)                (nHalGICDBaseAddress   + 0x00000280 + 4 * (n))
#define HWIO_GICD_ISPENDRn_ADDR(n)                (nHalGICDBaseAddress   + 0x00000200 + 4 * (n))
#define HWIO_GICD_ICENABLERn_ADDR(n)              (nHalGICCBaseAddress   + 0x00000180 + 4 * (n))
#define HWIO_GICD_ISENABLERn_ADDR(n)              (nHalGICDBaseAddress   + 0x00000100 + 4 * (n))
#define HWIO_GICD_ICFGRn_ADDR(n)                  (nHalGICDBaseAddress   + 0x00000c00 + 4 * (n))

#define HWIO_GICD_SGIR_ADDR    (APCS_QGICDR_QGICDR_GICD_REG_BASE_PHYS    + 0x00000f00)
#define HWIO_GICC_HPPIR_ADDR   (APCS_QGICDR_QGICDR_HPPIR_REG_BASE_PHYS   + 0x00000018)

#define HWIO_GICD_SGIR_T_FILTER_BMSK                                       0x03000000
#define HWIO_GICD_SGIR_T_FILTER_SHFT                                             0x18
#define HWIO_GICD_SGIR_T_LIST_BMSK                                           0xff0000
#define HWIO_GICD_SGIR_T_LIST_SHFT                                               0x10
#define HWIO_GICD_SGIR_SATT_BMSK                                               0x8000
#define HWIO_GICD_SGIR_SATT_SHFT                                                  0xf
#define HWIO_GICD_SGIR_INT_ID_BMSK                                                0xf
#define HWIO_GICD_SGIR_INT_ID_SHFT                                                  0

#define HWIO_GICD_SGIR_OUT(v)                                            \
        out_dword(HWIO_GICD_SGIR_ADDR,v)
#endif  /* __TEETEST_HAL_HWIO_H */

/** 
    @file  hw_io.h
    @brief HW io macros
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#ifndef __HW_IO_H__
#define __HW_IO_H__

#include "i2c_types.h"
#include "hw_regs.h"

#define FLD_BMSK(reg, field) HWIO_##reg##_##field##_BMSK
#define FLD_SHFT(reg, field) HWIO_##reg##_##field##_SHFT

#define REG_IN(base, reg) \
    (*(volatile uint32 *)(base + HWIO_##reg##_OFFS) & HWIO_##reg##_RMSK)

#define REG_INI(base, reg, idx) \
    (*(volatile uint32 *)(base + HWIO_##reg##_OFFS(idx)) & HWIO_##reg##_RMSK)


#define REG_OUT(base, reg, data) \
    (*(volatile uint32 *)(base + HWIO_##reg##_OFFS)) = data;

#define REG_OUTI(base, reg, idx, data) \
    (*(volatile uint32 *)(base + HWIO_##reg##_OFFS(idx))) = data;


#define REG_INF(base, reg, field) \
    ((REG_IN(base, reg) & FLD_BMSK(reg, field)) >> FLD_SHFT(reg, field))

#define REG_OUTF(base, reg, field, data) \
    REG_OUT(base, reg, ((REG_IN(base, reg) & ~(FLD_BMSK(reg, field))) | ((data << FLD_SHFT(reg, field)) & FLD_BMSK(reg, field))))

#define FLD_SET(reg, field, data) \
    (((uint32) data << (uint32) FLD_SHFT(reg, field)) & (uint32) FLD_BMSK(reg, field))

#define FLD_GET(data, reg, field) \
    ((data & FLD_BMSK(reg, field)) >> FLD_SHFT(reg, field))


#endif

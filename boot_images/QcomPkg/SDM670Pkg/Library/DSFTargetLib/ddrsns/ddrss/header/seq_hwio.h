/*********************************************************************************
 *
 * DESCRIPTION
 * - This is an extension of standard msmhwio.h to support relative addressing
 *   scheme used in SCALe auto-generated sequences.
 * - The objective of this new addressing scheme is enable the same C function
 *   definition to be applicable to multiple baseances of the same block. 
 * - Such code reuse is not feasible with the standard HWIO macros that use a
 *   absolute addressing scheme.
 * - Compared to the standard HWIO macros, the new macros defined here take an
 *   additional parameter 'baseance offset'.  So are the C functions generated
 *   by SCALe Autoseq from .seq inputs.
 * - As such, macros defined in this file must be used with 'seq_msmhwiobase.h',
 *   'seq_msmhwioreg.h', and the C codes generated from SCALe Autoseq.
 * - Macros defined in this file leverage the lower-level macros from the
 *   standard 'msmhwio.h', and the two sets of macros are compatible.
 *
 * Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 ********************************************************************************/

#ifndef __SEQ_H__
#define __SEQ_H__

#include "HALhwio.h"



/**** Register Ref Read ****/
#define SEQ_INH(base, regtype, reg) \
        SEQ_##regtype##_INH(base, reg)

/**** Masked Register Read ****/
#define SEQ_INMH(base, regtype, reg, mask) \
        SEQ_##regtype##_INMH(base, reg, mask)


/**** Ref Reg Field Read ****/
#define SEQ_INFH(base, regtype, reg, fld) \
        (SEQ_##regtype##_INMH(base, reg, HWIO_FMSK(regtype, fld)) >> HWIO_SHFT(regtype, fld))


/**** Ref Register  Write ****/
#define SEQ_OUTH(base, regtype, reg, val) \
        SEQ_##regtype##_OUTH(base, reg, val)

/**** Ref Register Masked Write ****/
#define SEQ_OUTMH(base, regtype, reg, mask, val) \
        SEQ_##regtype##_OUTMH(base, reg, mask, val)


/**** Ref Register Field Write ****/
#define SEQ_OUTFH(base, regtype, reg, fld, val) \
        SEQ_##regtype##_OUTMH(base, reg, HWIO_FMSK(regtype, fld), val << HWIO_SHFT(regtype, fld))



/************ seq_wait() ************/

typedef enum {
    SEC,
    MS,
    US,
    NS
} SEQ_TimeUnit;


/************ seq_poll() ************/
uint32 seq_poll(uint32 reg_offset, uint32 expect_value, uint32 value_mask, uint32 value_shift, uint32 max_poll_cnt);

#endif /* __SEQ_H__ */




















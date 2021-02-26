#ifndef PM_PMIO_H
#define PM_PMIO_H
/*
===========================================================================
*/
/**
  @file pm_pmio.h

  Public interface include file for accessing the PMIO definitions.

  The pm_pmio.h file is the public API interface to the PMIC I/O (PMIO)
  register access definitions.
*/
/*
  ====================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/pmic/pm/pm_pmio.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $
  ====================================================================
*/

/**
  The following functions must be defined by the client in order to use
  the PMIO macros. Replace "ERR_TYPE" with the appropriate error type.

  static inline ERR_TYPE in_pmio (void *ctxt, uint8 pmic, uint32 addr, uint8 *val);
  static inline ERR_TYPE out_pmio (void *ctxt, uint8 pmic, uint32 addr, uint8 val);
  static inline ERR_TYPE in_pmio_field (void *ctxt, uint8 pmic, uint32 addr, uint8 mask, uint8 shift, uint8 *val);
  static inline ERR_TYPE out_pmio_field (void *ctxt, uint8 pmic, uint32 addr, uint8 mask, uint8 shift, uint8 val);
*/

/* -----------------------------------------------------------------------
** Macros
** ----------------------------------------------------------------------- */

/**
  @addtogroup pmio_macros
  @{
*/

/**
 * @name Address Macros
 *
 * Macros for getting register addresses.
 * These macros are used for retrieving the address of a register.
 * PMIO_ADDR* will return the directly accessible address (virtual or physical based
 * on environment), PMIO_PHYS* will always return the physical address.
 * The offset from the base region can be retrieved using PMIO_OFFS*.
 * The "X" extension is used for explicit addressing where the base address of
 * the module in question is provided as an argument to the macro.
 *
 * @{
 */
#define PMIO_ADDR(pmiosym)        __pmio_addr(pmiosym)
#define PMIO_ADDRX(base, pmiosym) __pmio_addrx(base, pmiosym)
#define PMIO_OFFS(pmiosym)        __pmio_offs(pmiosym)
/** @} */

/**
 * @name Input Macros
 *
 * These macros are used for reading from a named PMIC register. The "M" suffix
 * indicates that the input will be masked with the supplied mask. The PMIO_INF
 * macro takes a field name and will do the appropriate masking and shifting
 * to return just the value of that field. The "X" extension is used for
 * explicit addressing where the base address of the module in question is
 * provided as an argument to the macro. The "N" extension is used for
 * multi-byte access to read an array of registers.
 *
 * Generally you want to use either PMIO_IN or PMIO_INF (with required indexing).
 *
 * @{
 */
#define PMIO_IN(ctxt, pmic, pmiosym, val)                   __pmio_in(ctxt, pmic, pmiosym, val)
#define PMIO_INM(ctxt, pmic, pmiosym, mask, val)            __pmio_inf(ctxt, pmic, pmiosym, mask, 0, val)
#define PMIO_INF(ctxt, pmic, pmiosym, field, val)           __pmio_inf(ctxt, pmic, pmiosym, PMIO_FMSK(pmiosym, field), PMIO_SHFT(pmiosym, field), val)
#define PMIO_INN(ctxt, pmic, pmiosym, vals, len)            __pmio_inn(ctxt, pmic, pmiosym, vals, len)

#define PMIO_INX(ctxt, pmic, base, pmiosym, val)            __pmio_inx(ctxt, pmic, base, pmiosym, val)
#define PMIO_INMX(ctxt, pmic, base, pmiosym, mask, val)     __pmio_infx(ctxt, pmic, base, pmiosym, mask, 0, val)
#define PMIO_INFX(ctxt, pmic, base, pmiosym, field, val)    __pmio_infx(ctxt, pmic, base, pmiosym, PMIO_FMSK(pmiosym, field), PMIO_SHFT(pmiosym, field), val)
#define PMIO_INNX(ctxt, pmic, base, pmiosym, vals, len)     __pmio_innx(ctxt, pmic, base, pmiosym, vals, len)
/** @} */

/**
 * @name Output Macros
 *
 * These macros are used for writing to a named PMIC register. The "M"
 * suffix indicates that the output will be masked with the supplied mask
 * (meaning these macros do a read first, mask in the supplied data, then
 * write it back). The "X" extension is used for explicit addressing where
 * the base address of the module in question is provided as an argument to
 * the macro. The PMIO_OUTF macro takes a field name and will do the
 * appropriate masking and shifting to output just the value of that field.
 * The "N" extension is used for multi-byte access to write an array of
 * registers.
 *
 * Generally you want to use either PMIO_OUT or PMIO_OUTF (with required indexing).
 *
 * @{
 */
#define PMIO_OUT(ctxt, pmic, pmiosym, val)                  __pmio_out(ctxt, pmic, pmiosym, val)
#define PMIO_OUTM(ctxt, pmic, pmiosym, mask, val)           __pmio_outf(ctxt, pmic, pmiosym, mask, 0, val)
#define PMIO_OUTF(ctxt, pmic, pmiosym, field, val)          __pmio_outf(ctxt, pmic, pmiosym, PMIO_FMSK(pmiosym, field), PMIO_SHFT(pmiosym, field), val)
#define PMIO_OUTN(ctxt, pmic, pmiosym, vals, len)           __pmio_outn(ctxt, pmic, pmiosym, vals, len)

#define PMIO_OUTX(ctxt, pmic, base, pmiosym, val)           __pmio_outx(ctxt, pmic, base, pmiosym, val)
#define PMIO_OUTMX(ctxt, pmic, base, pmiosym, mask, val)    __pmio_outfx(ctxt, pmic, base, pmiosym, mask, val)
#define PMIO_OUTFX(ctxt, pmic, base, pmiosym, field, val)   __pmio_outfx(ctxt, pmic, base, pmiosym, PMIO_FMSK(pmiosym, field), PMIO_SHFT(pmiosym, field), val)
#define PMIO_OUTNX(ctxt, pmic, base, pmiosym, vals, len)    __pmio_outnx(ctxt, pmic, base, pmiosym, vals, len)
/** @} */

/**
 * @name Shift and Mask Macros
 *
 * Macros for getting shift and mask values for fields and registers.
 *  PMIO_RMSK: The mask value for accessing an entire register.  For example:
 *             @code
 *             PMIO_RMSK(REG) -> 0xFFFFFFFF
 *             @endcode
 *  PMIO_SHFT: The right-shift value for accessing a field in a register.  For example:
 *             @code
 *             PMIO_SHFT(REG, FLD) -> 8
 *             @endcode
 *  PMIO_FMSK: The mask value for accessing a field in a register.  For example:
 *             @code
 *             PMIO_FMSK(REG, FLD) -> 0xFF00
 *             @endcode
 *  PMIO_VAL:  The value for a field in a register.  For example:
 *             @code
 *             PMIO_VAL(REG, FLD, ON) -> 0x1
 *             @endcode
 *  PMIO_FVAL: This macro takes a numerical value and will shift and mask it into
 *             the given field position.  For example:
 *             @code
 *             PMIO_FVAL(REG, FLD, 0x1) -> 0x100
 *             @endcode
 * @{
 */
#define PMIO_RMSK(pmiosym)                    __pmio_rmsk(pmiosym)
#define PMIO_SHFT(pmio_regsym, pmio_fldsym)   __pmio_shft(pmio_regsym, pmio_fldsym)
#define PMIO_FMSK(pmio_regsym, pmio_fldsym)   __pmio_fmsk(pmio_regsym, pmio_fldsym)
#define PMIO_VAL(pmiosym, field, val)         __pmio_val(pmiosym, field, val)
#define PMIO_FVAL(pmiosym, field, val)        (((uint32)(val) << PMIO_SHFT(pmiosym, field)) & PMIO_FMSK(pmiosym, field))
#define PMIO_GET_FVAL(pmiosym, field, val)    (((val) & PMIO_FMSK(pmiosym, field)) >> PMIO_SHFT(pmiosym, field))
/** @} */

/**
  @}
*/ /* end_group */

/** @cond */

/*
 * Map to final symbols.  This remapping is done to allow register
 * redefinitions.  If we just define PMIO_IN(xreg) as PMIO_##xreg##_IN
 * then remappings like "#define xreg xregnew" do not work as expected.
 */
#define __pmio_in(ctxt, pmic, pmiosym, val)                         PMIO_##pmiosym##_IN(ctxt, pmic, val)
#define __pmio_inf(ctxt, pmic, pmiosym, mask, shift, val)           PMIO_##pmiosym##_INF(ctxt, pmic, mask, shift, val)
#define __pmio_inn(ctxt, pmic, pmiosym, vals, len)                  PMIO_##pmiosym##_INN(ctxt, pmic, vals, len)
#define __pmio_out(ctxt, pmic, pmiosym, val)                        PMIO_##pmiosym##_OUT(ctxt, pmic, val)
#define __pmio_outf(ctxt, pmic, pmiosym, mask, shift, val)          PMIO_##pmiosym##_OUTF(ctxt, pmic, mask, shift, val)
#define __pmio_outn(ctxt, pmic, pmiosym, vals, len)                 PMIO_##pmiosym##_OUTN(ctxt, pmic, vals, len)
#define __pmio_addr(pmiosym)                                        PMIO_##pmiosym##_ADDR
#define __pmio_addrx(base, pmiosym)                                 PMIO_##pmiosym##_ADDR(base)
#define __pmio_offs(pmiosym)                                        PMIO_##pmiosym##_OFFS
#define __pmio_rmsk(pmiosym)                                        PMIO_##pmiosym##_RMSK
#define __pmio_fmsk(pmiosym, pmiofldsym)                            PMIO_##pmiosym##_##pmiofldsym##_BMSK
#define __pmio_shft(pmiosym, pmiofldsym)                            PMIO_##pmiosym##_##pmiofldsym##_SHFT
#define __pmio_val(pmiosym, pmiofld, pmioval)                       PMIO_##pmiosym##_##pmiofld##_##pmioval##_FVAL

#define __pmio_inx(ctxt, pmic, base, pmiosym, val)                  PMIO_##pmiosym##_IN(ctxt, pmic, base, val)
#define __pmio_infx(ctxt, pmic, base, pmiosym, mask, shift, val)    PMIO_##pmiosym##_INF(ctxt, pmic, base, mask, shift, val)
#define __pmio_innx(ctxt, pmic, base, pmiosym, vals, len)           PMIO_##pmiosym##_INN(ctxt, pmic, base, vals, len)
#define __pmio_outx(ctxt, pmic, base, pmiosym, val)                 PMIO_##pmiosym##_OUT(ctxt, pmic, base, val)
#define __pmio_outfx(ctxt, pmic, base, pmiosym, mask, shift, val)   PMIO_##pmiosym##_OUTF(ctxt, pmic, base, mask, shift, val)
#define __pmio_outnx(ctxt, pmic, base, pmiosym, vals, len)          PMIO_##pmiosym##_OUTN(ctxt, pmic, base, vals, len)

/** @endcond */

#endif /* PM_PMIO_H */


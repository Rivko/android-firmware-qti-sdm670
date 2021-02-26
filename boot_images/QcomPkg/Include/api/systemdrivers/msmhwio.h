#ifndef MSMHWIO_H
#define MSMHWIO_H
/*
===========================================================================
*/
/**
  @file msmhwio.h 

  Deprecated public interface include file for accessing the HWIO macro
  definitions.

  The msmhwio.h file is the legacy public API interface to the HW I/O (HWIO)
  register access definitions.  See HALhwio.h and DDIHWIO.h for the new
  interfaces.
*/
/*  
  ====================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  ====================================================================
*/ 

/*=========================================================================
      Include Files
==========================================================================*/

#ifndef _ARM_ASM_
#include "com_dtypes.h"
#endif

/*
 * Include main HWIO macros.
 */
#include "HALhwio.h"



/*=========================================================================
      External Declarations
==========================================================================*/


#if defined(IMAGE_QDSP6_PROC) && !defined(_ARM_ASM_)
extern void HWIO_AtomicLock (void);
extern void HWIO_AtomicFree (void);
#endif


/*=========================================================================
      Macro Declarations
==========================================================================*/


/** @cond */

/*
 * HWIO_INTLOCK / HWIO_INTFREE
 *
 * These macros are used to disable interrupts around read-mask-write
 * HWIO operations.
 */

#undef HWIO_INTLOCK
#undef HWIO_INTFREE

#if defined(FEATURE_WINCE_OAL) || defined(FEATURE_WINCE_BOOTLOADER) || defined(FEATURE_WINCE) || defined(_ARM_ASM_)

  #define HWIO_INTLOCK()
  #define HWIO_INTFREE()

#elif defined(IMAGE_QDSP6_PROC)

  #define HWIO_INTLOCK()  { HWIO_AtomicLock()
  #define HWIO_INTFREE()    HWIO_AtomicFree(); }

#elif defined(INTLOCK_SAV)

  #define HWIO_INTLOCK() { uint32 hwio_isav; INTLOCK_SAV(hwio_isav)
  #define HWIO_INTFREE()   INTFREE_SAV(hwio_isav); }

#else

  #define HWIO_INTLOCK()   INTLOCK()
  #define HWIO_INTFREE()   INTFREE()

#endif


/*
 * MSM_x (depreciated)
 *
 * These macros should be replaced with the relevant HWIO equivalent.
 */
#define MSM_OUT(io, val)                HWIO_OUT(io, val)
#define MSM_IN(io)                      HWIO_IN(io)
#define MSM_OUTM(io, mask, val)         HWIO_OUTM(io, mask, val)
#define MSM_INM(io, mask)               HWIO_INM(io, mask) 
#define MSM_INF(io, field)              HWIO_INF(io, field)
#define MSM_FIELD(field, val)           (((val) << field##_SHFT) & (field##_BMSK))
#define MSM_OUTSH(io, mask, val)        HWIO_OUTM(io, mask, val)
#define MSM_FOUTSH(io, field, val)      HWIO_OUTM(io, mask, val)
#define MSM_SET(io, field)              HWIO_OUTM(io, HWIO_FMSK(io, field), HWIO_FMSK(io, field))
#define MSM_SET_BIT(io, bit)            HWIO_OUTM(io, (1<<bit), (1<<bit))
#define MSM_SET_i(io, field, index)     HWIO_OUTMI(io, index, HWIO_FMSK(io,field), HWIO_FMSK(io,field))
#define MSM_SET_ij(io, field, index1, index2) HWIO_OUTMI2(io, index1, index2, HWIO_FMSK(io,field), HWIO_FMSK(io,field))
#define MSM_CLEAR(io, field)            HWIO_OUTM(io, HWIO_FMSK(io, field), 0)
#define MSM_CLEAR_BIT(io, bit)          HWIO_OUTM(io, (1<<bit), 0)
#define MSM_CLEAR_i(io, field, n)       HWIO_OUTMI(io, n, HWIO_FMSK(io,field), 0)
#define MSM_CLEAR_ij(io, field, m, n)   HWIO_OUTMI2(io, m, n, HWIO_FMSK(io,field), 0)
#define MSM_SET_MASK(io, mask)          HWIO_OUTM(io, mask, mask)
#define MSM_CLEAR_MASK(io, mask)        HWIO_OUTM(io, mask, 0)
#define MSM_PULSE(io, field)            HWIO_OUTF(io, field, (uint32)1); HWIO_OUTF(io, field, 0)
#define MSM_GET(io, field)              HWIO_INF(io, field)
#define MSM_W(io, val)                  HWIO_OUT(io, val)
#define MSM_W_i(io, val, index)         HWIO_OUTI(io, index, val)
#define MSM_W_ij(io, val, index1, index2) HWIO_OUTI2(io, index1, index2, val )
#define MSM_W_MASKED(io, field, val)    HWIO_OUTF(io, field, val)
#define MSM_W_MASKED_i(io, field, val, index)            HWIO_OUTFI(io, index, field, val)
#define MSM_W_MASKED_ij(io, field, val, index1, index2)  HWIO_OUTFI2(io, index1, index2, field, val)
#define MSM_R(io)                       HWIO_IN(io)
#define MSM_R_MASKED(base, field)       HWIO_INF(base, field)
#define MSM_R_MASKED_i(base, field, n)  HWIO_INFI(base, n, field)
#define MSM_R_MASKED_ij(base, field, index1, index2)  HWIO_INFI2(base, index1, index2, field)
#define MSM_GET_BIT(io, bit)            (HWIO_INM(io, (1<<bit) ) >> bit) 
#define MSM_NOT_USED( i )               if(i) {}


/*
 * MSM_LOCK / MSM_UNLOCK (depreciated)
 *
 * These macros are intended to be used if a client will be issuing a
 * series of HWIO writes to avoid the multiple locking/freeing of interrupts
 * that will otherwise occur.
 * They should be replaced with HWIO_LOCK / HWIO_UNLOCK
 */
#define MSM_LOCK_REQUIRED                 \
  MSM_LOCK_required = MSM_LOCK_required;

#define MSM_LOCK( )                   \
  {                                   \
    boolean MSM_LOCK_required = TRUE; \
    uint32 msm_sav;                   \
    INTLOCK_SAV(msm_sav)

#define MSM_UNLOCK( )      \
    MSM_LOCK_REQUIRED      \
    INTFREE_SAV(msm_sav);  \
  }

#define MSM_UNLOCK_AND_LOCK( ) \
    MSM_LOCK_REQUIRED          \
    INTFREE_SAV( msm_sav ),    \
    INTLOCK_SAV( msm_sav )

#define HWIO_LOCK()
#define HWIO_UNLOCK()


/*
 * INPxx / OUTPxx (depreciated)
 *
 */
#define INP32(addr)          in_dword(addr)
#define INP32M(addr, mask)   in_dword_masked(addr,mask)
#define OUTP32(addr, val)    out_dword(addr, val)
#define OUTP32M(addr, mask, val)                      \
   HWIO_INTLOCK();                                    \
   out_dword_masked_ns(addr, mask, val, INP32(addr)); \
   HWIO_INTFREE()


/** @endcond */

#endif /* MSMHWIO_H */


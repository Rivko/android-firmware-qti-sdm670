/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Q U A D W O R D    S E R V I C E S

GENERAL DESCRIPTION
  This module contains services for manipulating qword (64 bit) types.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright(c) 1990-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/qw.c_v   1.5   08 Mar 2002 12:10:28   rajeevg  $
$Header: //components/rel/core.qdsp6/2.1/services/utils/src/qw.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/10   sri     Porting mis-aligned access methods for Q6
08/13/09   sri     Fixed mis-aligned access bug in qw_set
05/29/09   sri     Fixed mis-aligned access bug in qw_dec, qw_add, qw_sub
01/26/09   etv     Fixed mis-aligned access bug with qw_inc
11/06/07    th     prefast cleanup
06/11/07   enj     Fixing lint errors
03/06/02    gr     Moved pragma that suppresses subexpression elimination to
                   cover entire file.
01/22/02    gr     Added suppression of common subexpression elimination in
                   qw_inc() as a workaround for a compiler bug.
11/29/00   bgc     Fixed qw_div() for the misaligned case.
10/27/00   lad     Optimized with run-time alignment check.
05/24/00   hrk     Simple optimizations to qw_mul().
09/08/99   hrk     explicitly casting to uint32 the intermediate product
                   in qw_mul().
08/09/99    sk     Introduced qc_qword for qword internal usuage.
04/23/99    sk     added PACKED qualifier for proper handling in qw_mul().
04/09/99    ms     Lint cleanup.
03/17/99    ms     Changes for Virtual Boot Block.
                     Removed bb_ prefix
                     Not including bbjmpout.h
02/08/99    sk     Introduced optimized qword routines
02/04/99    ms     Exported qw_shift.
12/16/98   jct     Removed 80186 support
12/04/98    ms     Added qw_div_by_power_of_2.
09/21/98   jct     Incorporated ARM porting changes
04/08/96   dna     Added comment to each func. saying it is in the Boot Block
02/28/96   dna     Prepended bb_ to function names and put module in the
                   Boot Block.  Turned off some lint warnings for the entire
                   file due to _asm code.
07/07/92   ip      Release to DMSS PVCS.
04/29/92   gb      Large model version.
04/23/92   ip      Initial porting of file from Brassboard to DMSS.
04/18/91   rdb     Finished off first set of services.
04/17/91   rdb     Created first cut of module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __ARMCC_VERSION
#if ( __ARMCC_VERSION >= 110000 ) && ( __ARMCC_VERSION < 130000 )
   #pragma Ono_cse
   /* turn off common subexpression elimination as a workaround for a
   ** compiler bug in ADS 1.1 and ADS 1.2 (build 805)
   */
#endif
#endif /* __ARMCC_VERSION */

#include "comdef.h"
#include "qw.h"
#ifdef CUST_H
   #include "customer.h"
#endif
#ifndef FEATURE_WINCE
#include "amssassert.h"
#else
#define ASSERT
#endif

#ifdef FEATURE_WINCE
  #pragma prefast(disable:414, "Alignment checked by QW_NATIVE_ALIGNED")
  /* turn off warning 414: The buffer 'temp_quotient' type has a lower
   *                       alignment than the type it's being casted to.
   *                       This could cause a problem with new compiler
   *                       drops or on different architectures.
   *                       Use __declspec(align(#)) keyword to fix the
   *                       problem.
   *
   * Alignment is being handled in this code using the QW_NATIVE_ALIGNED macro */
#endif



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/* The following macros allow for non-featurized alignment checks in the
   optimized code below. */

#ifdef ARCH_QDSP6
/* QDSP6 is a 64-bit natively aligned architecture */
#define QW_NATIVE_ALIGNED(qw) (!(((int) qw) << 29))
#else /* else assume 32-bit natively aligned architecture/s */
#define QW_NATIVE_ALIGNED(qw) (!(((int) qw) << 30))
#endif /* ARCH_QDSP6 */

/* Strictly 32-bit alignment checking macro */
#define QW_UINT32_ALIGNED(qw) (!(((int) qw) << 30))

/* Handy typedef and macros for accessing upper and lower 32-bit values
   of an unaligned uint64. */
typedef PACKED struct PACKED_POST
{
  uint32 x;
  uint32 y;
} unaligned_uint32_2;

#define PACKED_UINT32_ACCESS_LO(PTR) ((unaligned_uint32_2 *) (PTR))->x
#define PACKED_UINT32_ACCESS_HI(PTR) ((unaligned_uint32_2 *) (PTR))->y

// use non-ARM-optimized code for Q6/gcc or if requested
#if (!defined(FEATURE_QWORD_MISALIGNED_METHODS))


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_SET

DESCRIPTION
  This macro sets the high order 32 bits of a specified qword to a
  specified 32 bit value, and the low order 32 bits to another specified
  value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_set
(
  qc_qword qw,
    /* the qword to set               */
  uint32 hi,
    /* the value for the high 32 bits */
  uint32 lo
    /* the value for the low 32 bits  */
)
{
  ASSERT( (uint32*)qw != NULL );
  if (QW_UINT32_ALIGNED(qw)) {

    qw[0] = lo;
    qw[1] = hi;

  } else {

    PACKED_UINT32_ACCESS_LO(qw) = lo;
    PACKED_UINT32_ACCESS_HI(qw) = hi;

  }

} /* qw_set */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*==========================================================================

FUNCTION QW_HI

DESCRIPTION
  This macro returns the most significant 32 bits of a specified qword.

DEPENDENCIES
  None.

RETURN VALUE
  A uint32 equal to the high order 32 bit bits of the qword.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 qw_hi
(
  qc_qword qw
    /* qword to return the high 32 bits from */
)
{
  ASSERT( (uint32*)qw != NULL );
  if (QW_UINT32_ALIGNED(qw)) {
    uint32 temp = *(((uint32 *) qw) + 1);

    return temp;
  } else {
    return PACKED_UINT32_ACCESS_HI(qw);
  }
} /* END qw_hi */
#endif /* MSM5000_IRAM_FWD */


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_LO

DESCRIPTION
  This macro returns the least significant 32 bits of a specified qword.

DEPENDENCIES
  None.

RETURN VALUE
  A uint32 equal to the low order 32 bits of the qword.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 qw_lo
(
  qc_qword qw
    /* qword to return the low 32 bits from */
)
{
  ASSERT( (uint32*)qw != NULL );
  if (QW_UINT32_ALIGNED(qw)) {
    return *(uint32 *) qw;
  } else {
    return PACKED_UINT32_ACCESS_LO(qw);
  }
} /* qw_lo */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_EQU

DESCRIPTION
  This macro sets the value of one specified qword equal to another
  specified qword.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_equ
(
  qc_qword  qw1,
    /* qw1 will be set equal to qw2       */
  qc_qword  qw2
    /* the qword qw1 will be set equal to */
)
{
  ASSERT( (uint32*)qw1 != NULL );
  ASSERT( (uint32*)qw2 != NULL );
  /* If l-value is aligned, assignment is much faster */
  if (QW_NATIVE_ALIGNED(qw1)) {
    QW_CVT_Q2N(qw1) =
      (QW_NATIVE_ALIGNED(qw2)) ? QW_CVT_Q2N(qw2) : PACKED_UINT64_ACCESS(qw2);
  } else {
    qw_equ_misaligned(qw1, qw2);
  }
} /* qw_equ */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_INC

DESCRIPTION
  This macro increments a specified qword by a specified uint32 value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_inc
(
  qc_qword  qw,
    /* qword to increment  */
  uint32  val
    /* value to add to qw1 */
)
{

  ASSERT( (uint32*)qw != NULL );
  if (QW_NATIVE_ALIGNED(qw)) {
    QW_CVT_Q2N(qw) += val;
  } else {
    PACKED_UINT64_ACCESS(qw) += val;
  }

  return;
} /* qw_inc */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_DEC

DESCRIPTION
  This macro decrements a specified qword by a specified value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_dec
(
  qc_qword qw,
    /* qword to decrement        */
  uint32 val
    /* value to decrement qw1 by */
)
{

  ASSERT( (uint32*)qw != NULL );
  if (QW_NATIVE_ALIGNED(qw)) {
    QW_CVT_Q2N(qw) -= val;
  } else {
    PACKED_UINT64_ACCESS(qw) -= val;
  }

} /* qw_dec */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_ADD

DESCRIPTION
  This macro adds a specified qword adder to a specified qword addend
  and places the result in a specified qword sum.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_add
(
  qc_qword  sum,
    /* the result of the addition   */
  qc_qword  addend,
    /* the first addition argument  */
  qc_qword  adder
    /* the second addition argument */
)
{
  uint64 temp_addend, temp_adder, temp_sum;

  ASSERT( (uint32*)sum    != NULL );
  ASSERT( (uint32*)addend != NULL );
  ASSERT( (uint32*)adder  != NULL );
  /* Extract addend from argument list */
  if (QW_NATIVE_ALIGNED(addend)) {
    temp_addend = QW_CVT_Q2N(addend);
  } else {
    temp_addend = PACKED_UINT64_ACCESS(addend);
  }

  /* Extract adder from argument list */
  if (QW_NATIVE_ALIGNED(adder)) {
    temp_adder = *((uint64 *) adder);
  } else {
    temp_adder = PACKED_UINT64_ACCESS(adder);
  }

  /* Calculate sum in local variable */
  temp_sum = temp_addend + temp_adder;

  /* Write sum into given qword */
  if (QW_NATIVE_ALIGNED(sum)) {
    QW_CVT_Q2N(sum) = temp_sum;
  } else {
    PACKED_UINT64_ACCESS(sum) = temp_sum;
  }
} /* qw_add */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_SUB

DESCRIPTION
  This macro subtracts a specified qword subtractor from a specified
  qword subtrahend and places the result in a specified qword difference.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_sub
(
  qc_qword  difference,
    /* the result of the subtraction       */
  qc_qword  subtrahend,
    /* the first argument for subtraction  */
  qc_qword  subtractor
    /* the second argument for subtraction */
)
{
  uint64 temp_subtrahend, temp_subtractor, temp_difference;

  ASSERT( (uint32*)difference != NULL );
  ASSERT( (uint32*)subtrahend != NULL );
  ASSERT( (uint32*)subtractor != NULL );
  /* Extract subtrahend from argument */
  if (QW_NATIVE_ALIGNED(subtrahend)) {
    temp_subtrahend = QW_CVT_Q2N(subtrahend);
  } else {
    temp_subtrahend = PACKED_UINT64_ACCESS(subtrahend);
  }

  /* Extract subtractor from argument */
  if (QW_NATIVE_ALIGNED(subtractor)) {
    temp_subtractor = QW_CVT_Q2N(subtractor);
  } else {
    temp_subtractor = PACKED_UINT64_ACCESS(subtractor);
  }

  /* Calculate difference in local variable */
  temp_difference = temp_subtrahend - temp_subtractor;

  /* Write difference into given qword */
  if (QW_NATIVE_ALIGNED(difference)) {
    QW_CVT_Q2N(difference) = temp_difference;
  } else {
    PACKED_UINT64_ACCESS(difference) = temp_difference;
  }
} /* qw_sub */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_MUL

DESCRIPTION
  This macro multiplies a specified qword multiplicand by a specified
  uint32 multiplier and places the result in a specified qword product.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_mul
(
  qc_qword  product,
    /* the result of the multiplication       */
  qc_qword  multiplicand,
    /* the first argument for multiplication  */
  uint32  multiplier
    /* the second argument for multiplication */
)
{
  uint64 temp_multiplicand, temp_product;

  ASSERT( (uint32*)product      != NULL );
  ASSERT( (uint32*)multiplicand != NULL );
  if (QW_NATIVE_ALIGNED(multiplicand)) {
    temp_multiplicand = QW_CVT_Q2N(multiplicand);
  } else {
    temp_multiplicand = PACKED_UINT64_ACCESS(multiplicand);
  }

  temp_product = temp_multiplicand * multiplier;

  if (QW_NATIVE_ALIGNED(product)) {
    QW_CVT_Q2N(product) = temp_product;
  } else {
    PACKED_UINT64_ACCESS(product) = temp_product;
  }

  return;
}/* qw_mul */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_DIV

DESCRIPTION
  This macro divides a specified qword dividend by a specified word
  divisor and places the result in a specified qword quotient.

  NOTE: this macro assumes that the size of the divisor does not exceed
        half a machine word.

DEPENDENCIES
  None.

RETURN VALUE
  Remainder of division operation.

SIDE EFFECTS
  None.

===========================================================================*/
word qw_div
(
  qc_qword  quotient,
    /* the result from the division     */
  qc_qword  dividend,
    /* the first argument for division  */
  unsigned short  divisor
    /* the second argument for division */
)
{
  uint64  temp_dividend;
  uint64  temp_quotient;

  ASSERT( (uint32*)quotient != NULL );
  ASSERT( (uint32*)dividend != NULL );
  if (QW_NATIVE_ALIGNED(dividend)) {
    temp_dividend = QW_CVT_Q2N(dividend);
  } else {
    temp_dividend = PACKED_UINT64_ACCESS(dividend);
  }

  temp_quotient = temp_dividend / divisor;

  if (QW_NATIVE_ALIGNED(quotient)) {
    QW_CVT_Q2N(quotient) = temp_quotient;
  } else {
    PACKED_UINT64_ACCESS(quotient) = temp_quotient;
  }

  return (word)(temp_dividend % divisor);

} /* qw_div */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_DIV_BY_POWER_OF_2

DESCRIPTION
  This is the native 64 bit math version of qw_div_by_power_of_2.

DEPENDENCIES
  This function assumes that the number of bits (num_bits) does not exceed 16.
  This is VERY IMPORTANT for the returned remainder to be valid.

RETURN VALUE
  Remainder of division operation.

SIDE EFFECTS
  None.

===========================================================================*/
word qw_div_by_power_of_2
(
  qc_qword           quotient, /* the result from the division     */
  qc_qword           dividend, /* the number to be divided         */
  unsigned short     num_bits  /* number of bits to shift          */
)
{
  uint64  temp_quotient;
  word remainder = (1 << num_bits) - 1;

  ASSERT( (uint32*)quotient != NULL );
  ASSERT( (uint32*)dividend != NULL );
  if (QW_NATIVE_ALIGNED(dividend)) {
    temp_quotient = QW_CVT_Q2N(dividend);
  } else {
    temp_quotient = PACKED_UINT64_ACCESS(dividend);
  }

  remainder &= (word)temp_quotient;
  temp_quotient >>= num_bits;

  if (QW_NATIVE_ALIGNED(quotient)) {
    QW_CVT_Q2N(quotient) = temp_quotient;
  } else {
    PACKED_UINT64_ACCESS(quotient) = temp_quotient;
  }

  return remainder;

}
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_CMP

DESCRIPTION
  This macro compares 2 specified qwords, and returns a value based on
  their relation to each other.

DEPENDENCIES
  None.

RETURN VALUE
  -1  if qword1 < qword2
   0  if qword1 = qword2
  +1  if qword1 > qword2

SIDE EFFECTS
  None.

===========================================================================*/
int qw_cmp
(
  qc_qword  qw1,
    /* the first qword to compare  */
  qc_qword  qw2
    /* the second qword to compare */
)
{
  uint64 val1, val2;

  ASSERT( (uint32*)qw1 != NULL );
  ASSERT( (uint32*)qw2 != NULL );
  if (QW_NATIVE_ALIGNED(qw1)) {
    val1 = QW_CVT_Q2N(qw1);
  } else {
    val1 = PACKED_UINT64_ACCESS(qw1);
  }

  if (QW_NATIVE_ALIGNED(qw2)) {
    val2 = QW_CVT_Q2N(qw2);
  } else {
    val2 = PACKED_UINT64_ACCESS(qw2);
  }

  return ( ( val1 == val2 ) ? 0 : ( val1 > val2 ) ? 1 : -1 );

} /* qw_cmp */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_SHIFT

DESCRIPTION
  Bit shift the qword passed in. A positive shiftidend will shift left. A
  negative shiftidend will shift to the right.

  This is the native 64 bit math version of qw_shift.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_shift
(
  qc_qword shifticand,
    /* qword to shift                                               */
  int   shiftidend
    /* bits to shift, positive = shift left, negative = shift right */
)
{

  ASSERT( (uint32*)shifticand != NULL );
  if (QW_NATIVE_ALIGNED(shifticand)) {

    if (shiftidend > 0)
    {
      QW_CVT_Q2N(shifticand) <<= shiftidend;
    }
    else if (shiftidend < 0)
    {
      shiftidend *= -1;
      QW_CVT_Q2N(shifticand) >>= shiftidend;
    }

  } else {

    if (shiftidend > 0)
    {
      PACKED_UINT64_ACCESS(shifticand) <<= shiftidend;
    }
    else if (shiftidend < 0)
    {
      shiftidend *= -1;
      PACKED_UINT64_ACCESS(shifticand) >>= shiftidend;
    }

  }
} /* qw_shift */
#endif /*MSM5000_IRAM_FWD*/

#else /* FEATURE_QWORD_MISALIGNED_METHODS */

/* Because of the _asm code in this module, lint can not determine that the
** input parameters are referenced or that values are being returned.
** I have looked over this module carefully and determined that all input
** parameters are used and valid values returned, so I am disabling these
** errors for this entire file.  Any new code should be added at the bottom
** of the file, after the errors are re-enabled.
*/
/*lint -e715 input parameters not referenced */
/*lint -e533 no value returned */

#define WORD_BITCOUNT 32

#define HALFWORD_BITCOUNT (1L << (32 / 2))

#define lower_halfword(t)   ((dword) (t) % HALFWORD_BITCOUNT)
#define upper_halfword(t)   ((dword) (t) / HALFWORD_BITCOUNT)
#define upshift_halfword(t) ((dword) (t) * HALFWORD_BITCOUNT)



#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_SET

DESCRIPTION
  This function sets the high order 32 bits of a specified qword to a
  specified 32 bit value, and the low order 32 bits to another specified
  value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_set
(
  qc_qword qw,
    /* the qword to set               */
  dword hi,
    /* the value for the high 32 bits */
  dword  lo
    /* the value for the low 32 bits  */
)
{
   qw[0] = lo;
   qw[1] = hi;

   return;
} /* END qw_set */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_HI

DESCRIPTION
  This function returns the most significant 32 bits of a specified qword.

DEPENDENCIES
  None.

RETURN VALUE
  A dword equal to the high order 32 bit bits of the qword.

SIDE EFFECTS
  None.

===========================================================================*/
dword qw_hi
(
  qc_qword  qw
    /* qword to return the high 32 bits from */
)
{
   return qw[1];
} /* END qw_hi */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_LO

DESCRIPTION
  This function returns the least significant 32 bits of a specified qword.

DEPENDENCIES
  None.

RETURN VALUE
  A dword equal to the low order 32 bits of the qword.

SIDE EFFECTS
  None.

===========================================================================*/
dword qw_lo
(
  qc_qword  qw
    /* qword to return the low 32 bits from */
)
{
   return qw[0];
} /* END qw_lo */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_EQU

DESCRIPTION
  This function sets the value of one specified qword equal to another
  specified qword.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_equ
(
  qc_qword  qw1,
    /* qw1 will be set equal to qw2       */
  qc_qword  qw2
    /* the qword qw1 will be set equal to */
)
{
  word x;
    /* index                    */
  byte *p1 = (byte *) qw1;
    /* temp pointer for copying */
  byte *p2 = (byte *) qw2;
    /* temp pointer for copying */

  for(x = 0; x < sizeof(qc_qword); x++)
  {
    p1[x] = p2[x];
  }

  return;
} /* END qw_equ */
#endif /*MSM5000_IRAM_FWD*/

#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_INC

DESCRIPTION
  This function increments a specified qword by a specified dword value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_inc
(
  qc_qword qw1,
    /* the qword to increment */
  dword val
    /* value to add to qw1    */
)
{
   dword qw1_lo_temp = *((PACKED dword *) &qw1[0]);
   if (0xFFFFFFFFUL - qw1[0] < val)
   {
     qw1[1] = (*((PACKED dword *) &qw1[1]) + 1);
   }

   qw1[0] = qw1_lo_temp + val;

   return;
} /* END qw_inc */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_DEC

DESCRIPTION
  This function decrements a specified qword by a specified value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_dec
(
  qc_qword  qw1,
    /* qword to decrement        */
  dword  val
    /* value to decrement qw1 by */
)
{
   dword qw1_lo_temp = *((PACKED dword *) &qw1[0]);
   if (qw1_lo_temp < val)
   {
     qw1[1] = (*((PACKED dword *) &qw1[1]) - 1);
   }

   qw1[0] = qw1_lo_temp - val;

   return;
} /* END qw_dec */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_ADD

DESCRIPTION
  This function adds a specified qword adder to a specified qword addend
  and places the result in a specified qword sum.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_add
(
  qc_qword  sum,
    /* the result of the addition   */
  qc_qword  addend,
    /* the first addition argument  */
  qc_qword  adder
    /* the second addition argument */
)
{
   dword temp_addend_hi = *((PACKED dword *) &addend[1]);
   dword temp_addend_lo = *((PACKED dword *) &addend[0]);
   dword temp_adder_hi = *((PACKED dword *) &adder[1]);
   dword temp_adder_lo = *((PACKED dword *) &adder[0]);

   sum[1] = temp_addend_hi + temp_adder_hi;
   if (0xFFFFFFFFUL - temp_adder_lo < temp_addend_lo)
   {
      sum[1] = (*((PACKED dword *) &sum[1]) + 1);
   }

   sum[0] = temp_adder_lo + temp_addend_lo;

   return;
} /* END qw_add */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_SUB

DESCRIPTION
  This function subtracts a specified qword subtractor from a specified
  qword subtrahend and places the result in a specified qword difference.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_sub
(
  qc_qword  difference,
    /* the result of subtraction           */
  qc_qword  subtrahend,
    /* the first argument for subtraction  */
  qc_qword  subtractor
    /* the second argument for subtraction */
)
{
   dword temp_subtrahend_hi = *((PACKED dword *) &subtrahend[1]);
   dword temp_subtrahend_lo = *((PACKED dword *) &subtrahend[0]);
   dword temp_subtractor_hi = *((PACKED dword *) &subtractor[1]);
   dword temp_subtractor_lo = *((PACKED dword *) &subtractor[0]);

   difference[1] = temp_subtrahend_hi - temp_subtractor_hi;

   if (temp_subtrahend_lo < temp_subtractor_lo)
   {
      difference[1] = (*((PACKED dword *) &difference[1]) - 1);
   }

   difference[0] = temp_subtrahend_lo - temp_subtractor_lo;

   return;
} /* END qw_sub */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_SHIFT

DESCRIPTION
  Bit shift the qword passed in. A positive shiftend will shift left. A
  negative shiftend will shift to the right.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_shift
(
  qc_qword shifticand,
    /* qword to shift                                               */
  int   shiftidend
    /* bits to shift, positive = shift left, negative = shift right */
)
{
  if (shiftidend > 0)
  {
    if (shiftidend <= 32) {
       shifticand[1] = ( shifticand[1] << shiftidend ) |
                       ( shifticand[0] >> (32 - shiftidend) );
       shifticand[0] = shifticand[0] << shiftidend;
    }
    else {
       shifticand[1] = shifticand[0] << (shiftidend - 32);
       shifticand[0] = 0;
    }
  }

  else if (shiftidend < 0)
  {
    shiftidend *= -1;
    if (shiftidend <= 32) {
       shifticand[0] = ( shifticand[0] >> shiftidend ) |
                       ( shifticand[1] << (32 - shiftidend) );
       shifticand[1] = shifticand[1] >> shiftidend;
    }
    else {
       shifticand[0] = shifticand[1] >> (shiftidend - 32);
       shifticand[1] = 0;
    }
  }

  return;
} /* END qw_shift */
#endif /*MSM5000_IRAM_FWD*/

#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_MUL

DESCRIPTION
  This function multiplies a specified qword multiplicand by a specified
  dword multiplier and places the result in a specified qword product.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qw_mul
(
  qc_qword  product,
    /* the result of the multiplication       */
  qc_qword  multiplicand,
    /* the first argument for mulitplication  */
  dword  multiplier
    /* the second argument for multiplication */
)
{
   /*lint -e740 Unusual pointer cast (incompatible indirect types) */
   PACKED unsigned short  *mul1 = (PACKED unsigned short*) multiplicand;
   unsigned short  *mul2 = (unsigned short*) &multiplier;
   /*lint +e740 Unusual pointer cast (incompatible indirect types) */

   qword           iresult;
   qword           result;
   int             x;
   int             y;
   int             max_iter = 3;

   qw_set(result,0,0);

   while (!mul1[max_iter])
   {
     max_iter--;
   }

   for(x = max_iter; x>=0; x--) {
     qw_shift(result, 16);

     for(y=1; y>=0; y--) {
       if (mul2[y]) {
         iresult[0] = (mul1[x] * mul2[y]) << (y * 16);
         iresult[1] = ((dword) (mul1[x] * mul2[y])) >> (32-y*16);
         qw_add(result, result, iresult);
       }
     }
   }
   qw_equ(product,result);

   return;
} /* END qw_mul */
#endif /*MSM5000_IRAM_FWD*/

#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_DIV

DESCRIPTION
  This function divides a specified qword dividend by a specified word
  divisor and places the result in a specified qword quotient.

  NOTE: this function assumes that the size of the divisor does not exceed
        half a machine word.

DEPENDENCIES
  None.

RETURN VALUE
  Remainder of division operation.

SIDE EFFECTS
  None.

===========================================================================*/
word qw_div
(
  qc_qword           quotient,
    /* the result from the division     */
  qc_qword           dividend,
    /* the first argument for division  */
  unsigned short  divisor
    /* the second argument for division */
)
{
   /*----------------------------------------------------------------------
   ** Initialize temporary dividend to upper half of qword dividend, and
   ** copy lower half of qword dividend into local variable to prevent
   ** problems when quotient and dividend are the same qword
   **--------------------------------------------------------------------*/
   dword dtemp     = dividend[1];
   dword dividend0 = dividend[0];

   /* Upper word in quotient */
   quotient[1] = dtemp / divisor;

   /*---------------------------------------------------------------------
   ** Calculate remainder and shift in next half word of dividend
   ** (the upper half word of the temporary dividend will be zero since the
   ** remainder can be no larger than the divisor which is half a machine word)
   **-------------------------------------------------------------------*/
   dtemp = (upshift_halfword(dtemp % divisor)|upper_halfword(dividend0));

   /* Upper half of lower word in quotient */
   quotient[0] = upshift_halfword(dtemp/divisor);

   /* Calculate remainder and shift in remaining half word of dividend */
   dtemp = upshift_halfword(dtemp % divisor)|lower_halfword(dividend0);

   /* Lower half of lower word in quotient */
   quotient[0] |= dtemp / divisor;

   /* Remainder */
   return (word)(dtemp % divisor);
} /* END qw_div */
#endif /*MSM5000_IRAM_FWD*/



#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_DIV_BY_POWER_OF_2

DESCRIPTION
  This function right shifts the specified qword by the specified number of
  bits and puts the result in a specified qword quotient. The bits which are
  shifted out of the qword constitute the remainder of the equivalent division.

DEPENDENCIES
  This function assumes that the number of bits (num_bits) does not exceed 16.
  This is VERY IMPORTANT for the returned remainder to be valid.

RETURN VALUE
  Remainder of division operation.

SIDE EFFECTS
  None.

===========================================================================*/
word qw_div_by_power_of_2
(
  qc_qword           quotient, /* the result from the division     */
  qc_qword           dividend, /* the number to be divided         */
  unsigned short  num_bits  /* number of bits to shift          */
)
{
   dword          div_lo              = dividend[0];
   dword          div_hi              = dividend[1];
   unsigned short num_bits_complement = 32 - num_bits;

   quotient[0] =   ( div_lo >> num_bits )
                 | ( div_hi << num_bits_complement);
   quotient[1] = div_hi >> num_bits;

   return (word)((div_lo << num_bits_complement) >> num_bits_complement);

} /* END qw_div_by_power_of_2 */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_CMP

DESCRIPTION
  This function compares 2 specified qword, and returns a value based on
  their relation to each other.

DEPENDENCIES
  None.

RETURN VALUE
  -1  if qword1 < qword2
   0  if qword1 = qword2
  +1  if qword1 > qword2

SIDE EFFECTS
  None.

===========================================================================*/
int qw_cmp
(
  qc_qword  qw1,
    /* the first qword to compare  */
  qc_qword  qw2
    /* the second qword to compare */
)
{
  if (qw1[1] == qw2[1])
  {
    if (qw1[0] == qw2[0])
    {
      return 0;
    }
    else if (qw1[0] < qw2[0])
    {
      return -1;
    }
    else
    {
      return 1;
    }
  }
  else if (qw1[1] < qw2[1])
  {
    return -1;
  }
  else
  {
    return 1;
  }
} /* END qw_cmp */
#endif /*MSM5000_IRAM_FWD*/

#endif /* FEATURE_QWORD_MISALIGNED_METHODS */

#ifdef FEATURE_WINCE
   #pragma prefast(pop)
#endif

#ifdef __ARMCC_VERSION
#if ( __ARMCC_VERSION >= 110000 ) && ( __ARMCC_VERSION < 130000 )
   #pragma Ocse
   /* turn on common subexpression elimination optimization
   */
#endif
#endif /* __ARMCC_VERSION */

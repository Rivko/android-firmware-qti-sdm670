#ifndef QW_H
#define QW_H
/*===========================================================================

                  Q U A D W O R D   S E R V I C E S
                        H E A D E R    F I L E

DESCRIPTION
  This file contains delarations necessary to use the Quadword Services.

Copyright (c) 1991-2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/qw.h_v   1.2   23 Apr 2001 13:42:02   rajeevg  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/10   sri     Added QW_8BYTE_ALIGNED() macro
09/13/05   kwo     Lint cleanup
04/12/01   dna     Converted inline functions to macros.
10/27/00   lad     Optimizations.
09/08/99   jct     Added C++ guards around function signatures
04/09/99    sk     Used qword for application declaration and qc_qword for
            		   internal quad word implementation.Warning removal for
                   ARM 2.50.
04/09/99    ms     Lint cleanup.
03/17/99    ms     Removed comments related to bb_ for Virtual Boot Block.
02/18/99    sk     Introduced FEATURE_ALIGNED_QWORD
02/04/99    sk     Removed inline Qword Fn's.
02/04/99    ms     Exported qw_shift.
12/16/98   jct     Removed 80186 support
12/04/98    ms     Added qw_div_by_power_of_2.
09/21/98   jct     Incorporate ARM porting changes
07/08/92   jah     Added QW_BYTE() macro
07/07/92   ip      Release to DMSS PVCS.
04/29/92   gb      Large model version.
04/23/92   ip      Initial porting of file from Brassboard to DMSS.
04/18/91   rdb     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#ifdef CUST_H
	#include CUST_H
#endif
#include <string.h>

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef unsigned long qword[ 2 ];

#ifdef ARCH_QDSP6
  typedef unsigned long qc_qword[ 2 ];
#else
  #ifdef FEATURE_QUBE
    typedef unsigned long qc_qword[ 2 ];
  #else /* FEATURE_QUBE */
    typedef PACKED unsigned long qc_qword[ 2 ];
  #endif /* FEATURE_QUBE */
#endif

/*=========================================================================*/

#define HALF_WORD_BITS (32/2)



/*===========================================================================

MACRO QW_BYTE

DESCRIPTION
  This macro returns a selected byte from a specified quadword.

DEPENDENCIES
  None.

RETURN VALUE
  Specified byte out of quadword.

SIDE EFFECTS
  None.

===========================================================================*/
#define QW_BYTE(val,idx)  \
(                         \
  ( (byte*)(val) )[ idx ] \
)

/*===========================================================================

MACRO  QW_CVT_Q2N - Convert a 'qword' to a "native" 64-bit type (uint64)

DESCRIPTION
  This macro converts the defined quadword type to a native 64-bit unsigned
  integer type.  Since this macro can be used as an l-value of a "C"
  assignment operation, one cannot use INLINE here.

DEPENDENCIES
  None.

RETURN VALUE
  Converted native 64-bit value.

SIDE EFFECTS
  None.

===========================================================================*/
#define QW_CVT_Q2N(qw) \
          /*lint -save -e740 This pointer cast is ok */ \
          (*( (uint64 *)(qw) )) \
          /*lint -restore */

/*===========================================================================

MACRO QW_CVT_N2Q - Convert a  "native 64-bit type (uint64)" to a 'qword'.

DESCRIPTION
  This macro converts the defined quadword type to a native 64-bit unsigned
  integer type.  INLINE is used to force type checking, but it is used like
  a macro.

DEPENDENCIES
  None.

RETURN VALUE
  Cast of the address of the 64-bit input variable as a 'unsigned long *'.

SIDE EFFECTS
  None.

===========================================================================*/
#define  QW_CVT_N2Q(p_n64) ((unsigned long *)(p_n64))

/*===========================================================================

MACRO QW_EQU_MISALIGNED

DESCRIPTION
  This function sets the value of one specified qword equal to another
  specified qword.

  This macro should be used if either or both of the quadwords is not aligned
  to a 32-bit word boundary in memory.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#define qw_equ_misaligned( qw1,qw2 )  \
   memcpy((void *) (qw1), (void *) (qw2), sizeof(qc_qword))

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef __cplusplus
   extern "C"
   {
#endif


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
    /* the qword to set                */
  uint32 hi,
    /* the value for the high 32 bits */
  uint32 lo
    /* the value for the low 32 bits  */
);


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
  qc_qword qw1,
    /* the qword to set equal to qw2      */
  qc_qword qw2
    /* the value qw1 will be set equal to */
);


/*===========================================================================

FUNCTION QW_HI

DESCRIPTION
  This function returns the most significant 32 bits of a specified qword.

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
);


/*===========================================================================

FUNCTION QW_LO

DESCRIPTION
  This function returns the least significant 32 bits of a specified qword.

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
   /* qword to return the low 32 bits from  */
);


/*===========================================================================

FUNCTION QW_INC

DESCRIPTION
  This function increments a specified qword by a specified uint32 value.

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
    /* qword to increment  */
  uint32 val
    /* value to add to qw1 */
);


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
  qc_qword qw1,
    /* qword to decrement        */
  uint32 val
    /* value to decrement qw1 by */
);


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
  qc_qword sum,
    /* the result of the addition   */
  qc_qword addend,
    /* the first addition argument  */
  qc_qword adder
    /* the second addition argument */
);


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
  qc_qword difference,
    /* the result of the subtraction       */
  qc_qword subtrahend,
    /* the first argument for subtraction  */
  qc_qword subtractor
    /* the second argument for subtraction */
);


/*===========================================================================

FUNCTION QW_MUL

DESCRIPTION
  This function multiplies a specified qword multiplicand by a specified
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
  qc_qword product,
    /* the result of the multiplication       */
  qc_qword multiplicand,
    /* the first argument for multiplication  */
  uint32 multiplier
    /* the second argument for multiplication */
);


/*===========================================================================

FUNCTION QW_DIV

DESCRIPTION
  This function divides a specified qword dividend by a specified word
  divisor and places the result in a specified qword quotient. The word
  remainder is returned as the function's value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word qw_div
(
  qc_qword          quotient,
    /* the result from the division     */
  qc_qword          dividend,
    /* the first argument for division  */
  word divisor
    /* the second argument for division */
);


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
);


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
void  qw_shift
(
  qc_qword shifticand,
    /* qword to shift                                               */
  int   shiftidend
    /* bits to shift, positive = shift left, negative = shift right */
);


/*===========================================================================

FUNCTION QW_CMP

DESCRIPTION
  This function compares 2 specified qwords, and returns a value based on
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
  qc_qword qw1,
    /* the first qword to compare  */
  qc_qword qw2
    /* the second qword to compare */
);

#ifdef __cplusplus
   }
#endif

#endif /* QW_H */



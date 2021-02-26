#ifndef QW_H
#define QW_H
/**
  @file qw.h
  @brief This file contains delarations necessary to use the Quadword Services.  

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_queue" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

                  Q U A D W O R D   S E R V I C E S
                        H E A D E R    F I L E

DESCRIPTION
  This file contains delarations necessary to use the Quadword Services.

Copyright (c) 1991-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/qw.h_v   1.2   23 Apr 2001 13:42:02   rajeevg  $
$Header: //components/rel/core.qdsp6/2.1.c4/api/services/qw.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/10/10   EBR     Doxygenated File.
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
   #include "customer.h"
#endif
#include <string.h>

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/** @addtogroup utils_qword
@{ */

/** A quadword is a variable type composed of two unsigned longs. */
typedef unsigned long qword[ 2 ];

#ifdef ARCH_QDSP6
  /** A quadword is a variable type composed of two unsigned longs. */
  typedef unsigned long qc_qword[ 2 ];
#else
  #ifdef FEATURE_QUBE
    typedef unsigned long qc_qword[ 2 ];
  #else /* FEATURE_QUBE */
    typedef PACKED unsigned long qc_qword[ 2 ];
  #endif /* FEATURE_QUBE */
#endif

/*=========================================================================*/

/** @cond
*/
#define HALF_WORD_BITS (32/2)
/** @endcond */


/**
  Returns a selected byte from a specified quadword.

  @param[in] val Quadword to be indexed.
  @param[in] idx Byte to be returned.

  @return
  Specified byte.

  @dependencies
  None.
*/
#define QW_BYTE(val,idx)  \
(                         \
  ( (byte*)(val) )[ idx ] \
)


/**
  Converts the defined quadword type to a native 64-bit unsigned
  integer type. Since this macro can be used as a l-value of a C
  assignment operation, users cannot use INLINE here.
 
  @param[in] qw Quadword to be converted.

  @return
  Converted native 64-bit value.

  @dependencies
  None.
*/
#define QW_CVT_Q2N(qw) \
          /*lint -save -e740 This pointer cast is ok */ \
          (*( (uint64 *)(qw) )) \
          /*lint -restore */


/**
  Converts the defined quadword type to a native 64-bit unsigned
  integer type. INLINE is used to force type checking; however, it is used 
  like a macro.
 
  @param[in] p_n64 Native 64-bit type to be converted.

  @return
  Cast of the address of the 64-bit input variable as an ``unsigned long *''.

  @dependencies
  None.
*/
#define  QW_CVT_N2Q(p_n64) ((unsigned long *)(p_n64))


/**
  Sets the value of one specified quadword to be equal to another
  specified quadword.

  @note1hang This macro must be used if either or both of the quadwords 
  are not aligned to a 32-bit word boundary in memory.
 
  @param[out] qw1 Destination for the quadword.
  @param[in] qw2  Source for the quadword to be copied.

  @return
  None.

  @dependencies
  None.
*/
#define qw_equ_misaligned( qw1,qw2 )  \
   memcpy((void *) (qw1), (void *) (qw2), sizeof(qc_qword))

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef __cplusplus
   extern "C"
   {
#endif


/**
  Sets the high-order 32 bits of a specified quadword to a
  32-bit value and the low-order 32 bits to another value.
 
  @param[out] qw Quadword to be set.
  @param[in] hi  Value for the high 32 bits.
  @param[in] lo  Value for the low 32 bits.

  @return
  None.

  @dependencies
  None.
*/
void qw_set
(
  qc_qword qw,
  uint32 hi,
  uint32 lo
);


/**
  Sets the value of one specified quadword equal to 
  another quadword.

  @param[out] qw1 Quadword to be set equal to qw2.
  @param[in] qw2  Value to which qw1 is to be set.

  @return
  None.

  @dependencies
  None.
*/
void qw_equ
(
  qc_qword qw1,
    /* the qword to set equal to qw2      */
  qc_qword qw2
    /* the value qw1 will be set equal to */
);


/**
  Returns the most significant 32 bits of a specified quadword.

  @param[in] qw Quadword from which to return the high 32 bits.

  @return
  A uint32 equal to the high-order 32 bits.

  @dependencies
  None.
*/
uint32 qw_hi
(
  qc_qword qw
);


/**
  Returns the least significant 32 bits of a specified quadword.

  @param[in] qw Quadword from which to return the low 32 bits.

  @return
  A unit32 equal to the low-order 32 bits.

  @dependencies
  None.
*/
uint32 qw_lo
(
  qc_qword qw
);


/**
  Increases a specified quadword by a uint32 value.

  @param[out] qw1 Quadword to be increased.
  @param[in] val  Value to be added to qw1.

  @return
  None.

  @dependencies
  None.
*/
void qw_inc
(
  qc_qword qw1,
  uint32 val
);


/**
  Decreases a specified quadword by a uint32 value.

  @param[out] qw1 Quadword to be decreased.
  @param[in] val  Value by which to decrease qw1.

  @return
  None.

  @dependencies
  None.
*/
void qw_dec
(
  qc_qword qw1,
  uint32 val
);


/**
  Adds a specified quadword adder to a quadword addend
  and places the result in a quadword sum.
 
  @param[out] sum   Result of the addition.
  @param[in] addend First argument for addition.
  @param[in] adder  Second argument for addition.

  @return
  None.

  @dependencies
  None.
*/
void qw_add
(
  qc_qword sum,
  qc_qword addend,
  qc_qword adder
);


/**
  Subtracts a specified quadword subtractor from a 
  quadword subtrahend and places the result in a quadword 
  difference.

  @param[out] difference Result of the subtraction.
  @param[in] subtrahend  First argument for subtraction.
  @param[in] subtractor  Second argument for subtraction.

  @return
  None.

  @dependencies
  None.
*/
void qw_sub
(
  qc_qword difference,
  qc_qword subtrahend,
  qc_qword subtractor
);


/**
  Multiplies a specified quadword multiplicand by a 
  uint32 multiplier and places the result in a quadword product.
 
  @param[out] product     Result of the multiplication.
  @param[in] multiplicand First argument for multiplication.
  @param[in] multiplier   Second argument for multiplication.

  @return
  None.

  @dependencies
  None.
*/
void qw_mul
(
  qc_qword product,
  qc_qword multiplicand,
  uint32 multiplier
);


/**
  Divides a specified quadword dividend by a word
  divisor and places the result in a quadword quotient. The word
  remainder is returned as the function's value.
 
  @param[out] quotient Result of the division.
  @param[in] dividend  First argument for division.
  @param[in] divisor   Second argument for division.

  @return
  Remainder of the division operation.

  @dependencies
  None.
*/
word qw_div
(
  qc_qword          quotient,
  qc_qword          dividend,
  word divisor
);



/**
  Right shifts the specified quadword by a specific number of
  bits and puts the result in a quadword quotient. The bits that
  are shifted out of the quadword constitute the remainder of the 
  equivalent division.
 
  @param[out] quotient Result of the division.
  @param[in] dividend  Number to be divided.
  @param[in] num_bits  Number of bits to be shifted. Value must NOT exceed 16;
                       otherwise, the returned remainder will be invalid.

  @return
  Remainder of division operation.

  @dependencies
  For a valid returned remainder, this function assumes that the number 
  of bits (num_bits) does NOT exceed 16.
*/
word qw_div_by_power_of_2
(
  qc_qword           quotient,
  qc_qword           dividend,
  unsigned short  num_bits
);


/**
  Bit shifts the quadword passed in. A positive shiftidend shifts to 
  the left. A negative shiftidend shifts to the right.
 
  @param[out] shifticand Quadword to be shifted.
  @param[in] shiftidend  Bits to shift; positive shifts left, 
                         negative shifts right.

  @return
  None.

  @dependencies
  None.
*/
void  qw_shift
(
  qc_qword shifticand,
  int   shiftidend
);


/**
  Compares two specified quadwords and returns a value based on
  their relationship to each other.
 
  @param[in] qw1 First quadword to be compared.
  @param[in] qw2 Second quadword to be compared.

  @return
  -1 -- qw1 is less than qw2. \n
   0 -- qw1 equals qw2. \n
  +1 -- qw1 is greater than qw2.

  @dependencies
  None.
*/
int qw_cmp
(
  qc_qword qw1,
  qc_qword qw2
);

#ifdef __cplusplus
   }
#endif

/** @} */ /* end_addtogroup utils_qword */
#endif /* QW_H */



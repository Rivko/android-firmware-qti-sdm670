#ifndef BIT_H
#define BIT_H

/**
  @file bit.h
  @brief The following declarations are for use with the Bit Manipulation 
  Services.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_number_manip" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

              B I T    M A N I P U L A T I O N    S E R V I C E S

                            D E C L A R A T I O N S

DESCRIPTION
  The following declarations are for use with the Bit Manipulation Services.

Copyright (c) 1991-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/bit.h_v   1.1   03 Oct 2001 10:18:34   rajeevg  $
$Header: //components/rel/core.qdsp6/2.1.c4/api/services/bit.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/12/10   EBR     Doxygenated File.
10/03/01    gr     Moved bit manipulation macros from bit.c into bit.h.
03/31/99    ms     Added prototypes for b_copy_ltobw_s2d0 and b_copy_ltobw_s2d0
12/15/98   jct     Removed 80186 support
11/20/98    ms     Added prototype for function b_copy_litend_to_bigend_word.
08/28/98   jct     Added updates from ARM port, added comments, moved debug
                   definitions to this header
12/20/96   rdh     Changed functions to Pascal calling convention.
07/07/92   ip      Release to DMSS PVCS.
04/29/92   gb      Large model version.
04/23/92   ip      Initial porting of file from Brassboard to DMSS.
01/23/91   rdb     Created first cut of file.

===========================================================================*/

#include "comdef.h"
#include "qw.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                            MACRO DECLARATIONS

===========================================================================*/
/** @addtogroup utils_number_manip
@{ */

/**
  Computes the size in bits of the specified data type.

  @param[in] type Data type to be computed.

  @return
  Size in bits.

  @dependencies
  None.
*/
#define bitsize(type) (sizeof(type) * 8)


/**
  Creates a mask of bits sized to the number of bits in the specified type.

  @param[in] type Data type for which to create the mask.

  @return
  Mask for the data type.

  @dependencies
  None.
*/
#define copymask(type) ((0xffffffff) >> (32 - bitsize(type)))


/**
  Masks the bits in the data at the specified offset for a specific 
  number of width bits.

  @param[in] width  Number of width bits to use.
  @param[in] offset Offset at which to start the mask.
  @param[in] data   Data to be masked.

  @return
  The data with the bits masked.

  @dependencies
  None.
*/
#define MASK(width, offset, data) \
    /*lint -e701 shift left  of signed quantity  */  \
    /*lint -e702 shift right of signed quantity  */  \
    /*lint -e572 Excessive shift value           */  \
    /*lint -e573 Signed-unsigned mix with divide */  \
    /*lint -e506 Constant value boolean          */  \
    /*lint -e649 Sign fill during constant shift */  \
                                                     \
   (((width) == bitsize(data)) ? (data) :   \
   ((((copymask(data) << (bitsize(data) - ((width) % bitsize(data)))) & copymask(data)) >>  (offset)) & (data))) \
                     \
    /*lint +e701 */  \
    /*lint +e702 */  \
    /*lint +e572 */  \
    /*lint +e573 */  \
    /*lint +e506 */  \
    /*lint +e649 */  


/**
  Masks the bits in the data at the specified offset for a specific number 
  of width bits, and also shifts the data in the result by the specified 
  number of shift bits.

  @param[in] width  Number of width bits to use.
  @param[in] offset Offset at which to start the mask.
  @param[in] data   Data to be masked.
  @param[in] shift  Amount by which to shift the data.

  @return
  The data with the bits masked and shifted.

  @dependencies
  None.
*/
#define MASK_AND_SHIFT(width, offset, shift, data)  \
    /*lint -e504 Unusual shifter value */  \
                  ((((signed) (shift)) < 0) ?       \
                    MASK((width), (offset), (data)) << -(shift) :  \
                    MASK((width), (offset), (data)) >>  (((unsigned) (shift)))) \
    /*lint +e504 */


/**
  Masks the bits in the data at the specified offset for a specific number 
  of width bits, and also shifts the data in the result to the right by the
  specified number of shift bits.

  @param[in] width  Number of width bits to use.
  @param[in] offset Offset at which to start the mask.
  @param[in] data   Data to be masked.
  @param[in] shift  Amount by which to shift the data to the right.

  @return
  The data with the bits masked and shifted to the right.

  @dependencies
  None.
*/
#define MASK_AND_SHIFT_RIGHT(width, offset, shift, data)   \
                    (MASK((width), (offset), (data)) >>  ((unsigned)(shift)))

/*===========================================================================
MACRO MASK_AND_SHIFT_LEFT

DESCRIPTION
   Same as the macro MASK except also shifts the data in the result left
   by the given number of shift bits.
===========================================================================*/
/**
  Masks the bits in the data at the specified offset for a specific number 
  of width bits, and also shifts the data in the result to the left by the
  specified number of shift bits.

  @param[in] width  Number of width bits to use.
  @param[in] offset Offset at which to start the mask.
  @param[in] data   Data to be masked.
  @param[in] shift  Amount by which to shift the data to the left.

  @return
  The data with the bits masked and shifted to the left.

  @dependencies
  None.
*/
#define MASK_AND_SHIFT_LEFT(width, offset, shift, data)    \
                    (MASK((width), (offset), (data)) << ((unsigned)(shift)))


/**
  Masks the number of bits specified by length starting at a specific 
  offset. Unlike MASK() and MASK_AND_SHIFT(), this macro only creates the 
  mask; it does not operate on the data buffer.
 
  @param[in] offset Offset at which to start.
  @param[in] len    Length of the mask.

  @return
  The requested mask.

  @dependencies
  None.
*/
#define MASK_B(offset, len) \
  ((0xff >> offset) & (0xff << (8 - (offset + len))))


/**
  Adds an offset to a little endian word buffer. The pointer p to a word 
  buffer must be initially XORed with 1. Further add operations on such a 
  pointer using this macro will lead to the correct offset in a little 
  endian word buffer.
 
  @param[in] p Word buffer to modify.
  @param[in] i Size of the offset.

  @return
  None.

  @dependencies
  Pointer p to a word buffer must be initially XORed with 1.
*/
#define LITLEND_PTR_ADD(p,i)     (((((dword)(p))^1)+(i))^1)

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/**
  Packs the specified byte into the destination at a specific offset for 
  the specified number of length bits.
 
  @param[in] val  Byte to be packed.
  @param[out] buf Array where the byte is to be placed.
  @param[in] pos  Offset into which to pack the byte.
  @param[in] len  Number of length bits for packing the byte.

  @return
  None.

  @dependencies
  None.
*/
extern void  b_packb ( byte  val, byte buf[ ], word pos, word len );
/**
  Packs the specified word into the destination at a specific offset for 
  the specified number of length bits.
 
  @param[in] val  Word to be packed.
  @param[out] buf Array where the word is to be placed.
  @param[in] pos  Offset into which to pack the word.
  @param[in] len  Number of length bits for packing the word.

  @return
  None.

  @dependencies
  None.
*/
extern void  b_packw ( word  val, byte buf[ ], word pos, word len );
/**
  Packs the specified dword into the destination at a specific offset for 
  the specified number of length bits.
 
  @param[in] val  Dword to be packed.
  @param[out] buf Array where the dword is to be placed.
  @param[in] pos  Offset into which to pack the dword.
  @param[in] len  Number of length bits for packing the dword.

  @return
  None.

  @dependencies
  None.
*/
extern void  b_packd ( dword val, byte buf[ ], word pos, word len );
/**
  Packs the specified qword into the destination at a specific offset for 
  the specified number of length bits.
 
  @param[in] val  Qword to be packed.
  @param[out] buf Array where the qword is to be placed.
  @param[in] pos  Offset into which to pack the qword.
  @param[in] len  Number of length bits for packing the qword.

  @return
  None.

  @dependencies
  None.
*/
extern void  b_packq ( qword val, byte buf[ ], word pos, word len );


/**
  For a specified buffer and offset, unpacks the requested number of bits into
  a byte.
 
  @param[in] buf Buffer from which to unpack.
  @param[in] pos Position at which to start the unpacking.
  @param[in] len Length to be unpacked.

  @return
  The unpacked value.

  @dependencies
  None.
*/
extern byte   b_unpackb ( byte buf[ ], word pos, word len );
/**
  For a specified buffer and offset, unpacks the requested number of bits into
  a word.
 
  @param[in] buf Buffer from which to unpack.
  @param[in] pos Position at which to start the unpacking.
  @param[in] len Length to be unpacked.

  @return
  The unpacked value.

  @dependencies
  None.
*/
extern word   b_unpackw ( byte buf[ ], word pos, word len );
/**
  For a specified buffer and offset, unpacks the requested number of bits into
  a dword.
 
  @param[in] buf Buffer from which to unpack.
  @param[in] pos Position at which to start the unpacking.
  @param[in] len Length to be unpacked.

  @return
  The unpacked value.

  @dependencies
  None.
*/
extern dword  b_unpackd ( byte buf[ ], word pos, word len );
/**
  For a specified buffer and offset, unpacks the requested number of bits into
  a qword.
 
  @param[in] buf Buffer from which to unpack.
  @param[in] pos Position at which to start the unpacking.
  @param[in] len Length to be unpacked.
  @param[out] qw Quadword into which to unpack the bits.

  @return
  None.

  @dependencies
  None.
*/
extern void   b_unpackq ( byte buf[ ], word pos, word len, qword qw );

/**
  Copies a specified bitfield of a specific length to another specified 
  bitfield.
 
  @param[in] sbuf  Source bitfield buffer.
  @param[in] spos  Source bitfield offset.
  @param[out] dbuf Destination bitfield buffer.
  @param[out] dpos Destination bitfield offset.
  @param[in] len   Length in bits of the bitfield to be copied.

  @return
  None.

  @dependencies
  This function assumes that the two specified bitfields do not overlap.
  Users must be careful about the type of buffer they pass to this
  function and clearly understand what they will get in the 
  destination buffer.
*/
extern void  b_copy ( 
                   void  *sbuf,
                   word  spos,
                   void  *dbuf,
                   word  dpos,
                   word  len      );
/**
  This function is semantically identical to b_copy().
  It treats the source buffer as a little endian word buffer and the
  destination buffer as a big endian word buffer. The output buffer pointer
  must not be used as a word buffer on a little endian machine.

  The code is the same as in b_copy() except for the operation
  on the pointers. Since the source buffer is considered little endian,
  all arithmetic on the source pointer is performed using the  
  LITLEND_PTR_ADD() macro.
 
  @param[in] sbuf  Source bitfield buffer.
  @param[in] spos  Source bitfield offset.
  @param[out] dbuf Destination bitfield buffer.
  @param[out] dpos Destination bitfield offset.
  @param[in] len   Length in bits of the bitfield to be copied.

  @return
  None.

  @dependencies
  This function assumes that the two specified bitfields do not overlap.
  Users must be careful about the type of buffer they pass to this
  function and clearly understand what they will get in the 
  destination buffer.
*/
extern void  b_copy_litend_to_bigend_word ( 
                   word  *sbuf,
                   word  spos,
                   byte  *dbuf,
                   word  dpos,
                   word  len      );
/**
  This function is a special case of b_copy_litend_to_bigend_word().
  It assumes that spos = 1 and dpos = 0.
 
  @param[in] sbuf  Source bitfield buffer.
  @param[out] dbuf Destination bitfield buffer.
  @param[in] len   Length in bits of the bitfield to be copied.

  @return
  None.

  @dependencies
  This function assumes that the two specified bitfields do not overlap.
  Users must be careful about the type of buffer they pass to this
  function and clearly understand what they will get in the 
  destination buffer.
*/
extern void  b_copy_ltobw_s1d0 ( 
                   word  *sbuf,
                   byte  *dbuf,
                   word  len      );
/**
  This function is a special case of b_copy_litend_to_bigend_word().
  It assumes that spos = 2 and dpos = 0;
 
  @param[in] sbuf  Source bitfield buffer.
  @param[out] dbuf Destination bitfield buffer.
  @param[in] len   Length in bits of the bitfield to be copied.

  @return
  None.

  @dependencies
  This function assumes that the two specified bitfields do not overlap.
  Users must be careful about the type of buffer they pass to this
  function and clearly understand what they will get in the 
  destination buffer.
*/
extern void  b_copy_ltobw_s2d0 ( 
                   word  *sbuf,
                   byte  *dbuf,
                   word  len      );

/** @} */ /* end_addtogroup utils_number_manip */
#endif /* BIT_H */

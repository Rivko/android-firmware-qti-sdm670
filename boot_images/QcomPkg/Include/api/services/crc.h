#ifndef CRC_H
#define CRC_H
/**
  @file crc.h
  @brief The following declarations are for use with the CRC Services.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_crc" group 
      description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

              C R C    S E R V I C E S    D E C L A R A T I O N S

DESCRIPTION
  The following declarations are for use with the CRC Services.

Copyright (c) 1992-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/crc.h_v   1.2   28 Aug 2002 14:35:54   ldefauw  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/09/10   EBR     Doxygenated File.
04/06/05   ss      Moved 32-bit crc related declaratoin from hdrcrc.h to this 
                   file.
11/29/01   lad     Added step-wise CRC30 calculation.
03/17/99   ms      Removed comments related to bb_ for Virtual Boot Block.
12/16/98   jct     Removed 80186 support
09/21/98   jct     Incorporate ARM porting changes, cdecl changes
02/28/96   dna     Changed crc_16_l_table extern to be a pointer so the 
                   macro now accesses the table through the Boot Block jump
                   table.
06/21/93   jah     Corrected CRC_16_L_OK to match with crc_16_l_calc().
06/07/93   jah     Added prototype for crc_16_l_calc(), and improved comments
                   for CRC_16_L_STEP().
04/26/93   jah     Added extern for crc_16_l_table[] for macro.
04/22/93   jah     Added CRC definitions and macro to support CRC CCITT-16
                   processed LSB first.
07/09/92   jah     Changed crc_16_step() from bits to bytes
06/18/92   jah     Ported from brassboard and converted to table-driven.

===========================================================================*/

#include "comdef.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/** @addtogroup utils_crc
@{ */

/** Residual CRC value to compare against a return value of crc_16_calc().
 Use crc_16_calc() to calculate a 16-bit CRC and append it to the buffer.
 When crc_16_calc() is applied to the unchanged result, it returns 
 @latexonly\label{crc16ok}@endlatexonly 
 CRC_16_OK.
*/
#define CRC_16_OK               0xE2F0

/** Mask for a CRC-16 polynomial:
  
  x^16 + x^12 + x^5 + 1

  This is more commonly called CCITT-16.

  @note1hang The x^16 tap is left off; it is implicit.
*/
#define CRC_16_POLYNOMIAL       0x1021

/** Seed value for the CRC register. The all ones seed is part of CCITT-16, 
  and it allows for detection of an entire data stream of zeroes.
*/
#define CRC_16_SEED             0xFFFF

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/** Seed value for the CRC register. The all zeroes seed is inverted before
  being used in the step-wise CRC CCITT-16. This macro behaves like CRC_16_SEED.
*/
#define CRC_16_STEP_SEED        (~((word) CRC_16_SEED))

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/** Residual CRC value to compare against a return value of crc_30_calc().
  Use crc_30_calc() to calculate a 30-bit CRC and append it to the buffer.
  When crc_30_calc() is applied to the unchanged result, it returns 
  @latexonly\label{crc30ok}@endlatexonly
  CRC_30_OK.
*/
#define  CRC_30_OK  0x0B105AA5

/** Mask for a CRC-30 polynomial:

  x^30+x^29+x^21+x^20+x^15+x^13+x^12+x^11+x^8+x^7+x^6+x^2+x^1+1

  @note1hang The x^30 tap is left off; it is implicit.
*/
#define CRC_30_POLYNOMIAL       0x6030B9C7

/** Seed value for the CRC register. The all ones seed allows for detection of
  an entire data stream of zeroes.
*/
#define CRC_30_SEED             0x3FFFFFFF

/** Seed value for the CRC30 register. The CRC30 seed is inverted before
  being used in the step-wise CRC-30. This macro behaves like CRC_30_SEED.
*/
#define CRC_30_STEP_SEED        (~((dword) CRC_30_SEED))

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/** Mask for a CRC-16 polynomial:

  x^16 + x^12 + x^5 + 1

  This is more commonly called CCITT-16.

@note1hang The x^16 tap is left off; it is implicit.
*/
#define CRC_16_L_POLYNOMIAL     0x8408

/** Seed value for the CRC register. The all ones seed is part of CCITT-16, 
  and it allows for detection of an entire data stream of zeroes.
*/
#define CRC_16_L_SEED           0xFFFF

/** Seed value for the CRC register. The all ones seed is inverted before
  being used in the step-wise CRC CCITT-16. This macro behaves like 
  CRC_16_L_SEED.
*/
#define CRC_16_L_STEP_SEED        ((word) ~((word) CRC_16_L_SEED))

/** Residual CRC value to compare against a return value of a CRC_16_L_STEP().
  Use CRC_16_L_STEP() to calculate a 16-bit CRC, complement the result,
  and append it to the buffer. When CRC_16_L_STEP() is applied to the
  unchanged entire buffer and is complemented, it returns 
  @latexonly\label{crc16lok}@endlatexonly
  CRC_16_L_OK.
  A crc_16_l_calc() of the same entire buffer returns CRC_16_L_OK.
*/
#define CRC_16_L_OK             0x0F47


/** Seed value for a 32-bit CRC. 
*/
#define CRC_32_SEED             0x00000000UL

/** Mask for a 32-bit CRC polynomial:

  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+1

  @note1hang The x^32 tap is left off; it is implicit.
*/
#define CRC_32_POLYNOMIAL       0x04c11db7UL

/** Most significant bitmask for a CRC.
*/
#define CRC_32_MSB_MASK         0x80000000UL


/** Pointer in the Boot Block jump table that points to the actual 
  table crc_16_l_table. 
*/
extern const word crc_16_l_table[];       /* Extern for macro (global) */

/**
  Calculates a one-byte step of an LSB-first 16-bit CRC over
  a specified number of data bits. This macro can be used to produce a CRC 
  and to check a CRC.
 
  @param[in] xx_crc Current value of the CRC calculation; 16 bits.
  @param[in] xx_c   New byte to compute into the CRC; 8 bits.

  @return
  The new CRC value, 16-bits. If this macro is used to check a
  CRC and is run over a range of bytes, the return value is equal
  to CRC_16_L_OK (see Section @latexonly\ref{crc16lok}@endlatexonly) 
  if the CRC checks correctly. When
  generating a CRC to be appended to such a range of bytes, the final
  result must be XORed with CRC_16_L_SEED before being appended.

  @dependencies
  None.

  @sideeffects
  xx_crc is evaluated twice within this marco.
*/
#define CRC_16_L_STEP(xx_crc,xx_c) \
  (((xx_crc) >> 8) ^ crc_16_l_table[((xx_crc) ^ (xx_c)) & 0x00ff])


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/**
  Calculates a 16-bit CRC over a specified number of data
  bits. This function can be used to produce a CRC and to check a CRC.
 
  @param[in] buf_ptr Pointer to the bytes containing the data for the CRC. 
                     The bitstream starts in the most significant bit of 
                     the first byte.
  @param[in] len     Number of data bits over which to calculate the CRC.

  @return
  A word holding 16 bits that are the contents of the CRC
  register as calculated over the specified data bits. If this
  function is used to check a CRC, the return value is
  equal to CRC_16_OK (see Section @latexonly\ref{crc16ok}@endlatexonly) 
  if the CRC checks correctly.

  @dependencies
  None.
*/
extern word crc_16_calc
(
  byte *buf_ptr,
  word len
);


/**
  Calculates an LSB-first 16-bit CRC over a specified number
  of data bits. This function can be used to produce a CRC and to check a CRC.
 
  @param[in] buf_ptr Pointer to the bytes containing the data for the CRC. 
                     The bitstream starts in the least significant bit of 
                     the first byte.
  @param[in] len     Number of data bits over which to calculate the CRC.

  @return
  A word holding 16 bits that are the contents of the CRC
  register as calculated over the specified data bits. If this
  function is used to check a CRC, the return value is
  equal to CRC_16_L_OK (see Section @latexonly\ref{crc16lok}@endlatexonly)
  if the CRC checks correctly.
 
  @dependencies
  None.
*/
extern word crc_16_l_calc 
(
  byte *buf_ptr,
  word len
);


/**
  Calculates a 30-bit CRC over a specified number of data
  bits. This function can be used to produce a CRC and to check a CRC.
 
  @param[in] buf_ptr Pointer to the bytes containing the data for the CRC. 
                     The bitstream starts in the most significant bit of 
                     the first byte.
  @param[in] len     Number of data bits over which to calculate the CRC.

  @return
  A double word holding 30 bits that are the contents of the
  CRC register as calculated over the specified data bits. If this
  function is used to check a CRC, the return value is
  equal to CRC_30_OK (see Section @latexonly\ref{crc30ok}@endlatexonly) 
  if the CRC checks correctly.
 
  @dependencies
  None.
*/
extern dword crc_30_calc
(
  byte *buf_ptr,
  word len
);

/**
  Calculates a 30-bit CRC over a specified number of data
  bits. This function can be used to produce a CRC and to check a CRC. The 
  CRC value passed in is used to continue the CRC calculation from a previous 
  call, which allows this routine to be used for a CRC on discontinuous data.

  @param[in] seed    Either the result of a previous crc_30_step(), or 
                     CRC_16_STEP_SEED the first time the routine is called. 
                     Note that the input is inverted before use, which
                     counteracts the inversion applied when it was returned
                     as a result of the previous step.
  @param[in] buf_ptr Pointer to the bytes containing the data for the CRC. 
                     The bitstream starts in the most significant bit of 
                     the first byte.
  @param[in] len     Number of data bits over which to calculate the CRC.

  @return
  A double word holding 30 bits that are the contents of the
  CRC register as calculated over the specified data bits. If this
  function is used to check a CRC, the return value is
  equal to CRC_30_OK (see Section @latexonly\ref{crc30ok}@endlatexonly) 
  if the CRC checks correctly.
 
  @note1hang The caller is expected to mask the bottom 30 bits from the value.

  @dependencies
  None.
*/
extern dword crc_30_step
(
  dword seed,
  byte *buf_ptr,
  word len
);


/**
  Calculates a 16-bit CRC over a specified number of data
  bits. This function can be used to produce a CRC and to check a CRC. The 
  CRC value passed in is used to continue the CRC calculation from a previous
  call, which allows this routine to be used for a CRC on discontinuous data.
 
  @param[in] seed     Either the result of a previous crc_16_step(), or 
                      CRC_16_STEP_SEED the first time the routine is called. 
                      Note that the input is inverted before use, which 
                      counteracts the inversion applied when it was returned 
                      as a result of the previous step.
  @param[in] buf_ptr  Pointer to the bytes containing the data for the CRC. 
                      The bitstream starts in the most significant bit of 
                      the first byte.
  @param[in] byte_len Number of data bytes over which to calculate the CRC.

  @return
  A word holding 16 bits that are the contents of the CRC
  register as calculated over the specified data bits. If this
  function is used to check a CRC, the return value is
  equal to CRC_16_OK (see Section @latexonly\ref{crc16ok}@endlatexonly) 
  if the CRC checks correctly.
 
  @dependencies
  None.
*/
extern word crc_16_step
(
  word seed,
  byte *buf_ptr,
  word byte_len
);


/**
  Calculates an LSB-first 16-bit CRC over a specified number
  of data bytes. This function can be used to produce a CRC and to check a 
  CRC. The CRC value passed in is used to continue the CRC calculation from 
  a previous call, which allows this routine to be used for a CRC on 
  discontinuous data.
 
  @param[in] seed     Either the result of a previous crc_16_l_step(), or 
                      CRC_16_L_STEP_SEED the first time the routine is called. 
                      Note that the input is inverted before use, which 
                      counteracts the inversion applied when it was returned 
                      as a result of the previous step.
  @param[in] buf_ptr  Pointer to bytes containing the data for the CRC. 
                      The bit stream starts in the least significant bit of 
                      the first byte.
  @param[in] byte_len Number of data bytes over which to calculate the CRC.

  @return
  A word holding 16 bits that are the contents of the CRC
  register as calculated over the specified data bits. If this
  function is used to check a CRC, the return value is
  equal to CRC_16_L_OK (see Section @latexonly\ref{crc16lok}@endlatexonly) 
  if the CRC checks correctly.

  @dependencies
  None.
*/
extern word crc_16_l_step
(
  word seed,
  const void *buf_ptr,
  unsigned int byte_len
);

/* EJECT */
/**
  Calculates a 32-bit CRC over a specified number of data
  bits. This function can be used to produce a CRC and to check a CRC. The 
  CRC seed value can be used to compute a CRC over multiple (nonadjacent)
  chunks of data.
 
  @param[in] buf_ptr Pointer to the data over which to compute the CRC.
  @param[in] len     Number of bits over which to compute the CRC.
  @param[in] seed    Seed value for the CRC computation.

  @return
  A 32-bit word holding the contents of the CRC register as
  calculated over the specified data bits.
 
  @dependencies
  None.
*/
extern uint32 crc_32_calc
(
  uint8  *buf_ptr,
  uint16  len,
  uint32  seed
);

/** @} */ /* end_addtogroup utils_crc */
#endif /* CRC_H */

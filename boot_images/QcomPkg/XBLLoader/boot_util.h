#ifndef BOOT_UTIL_H
#define BOOT_UTIL_H

/*=============================================================================
                        Boot Utility Header File 

GENERAL DESCRIPTION     
  This module defines miscellaneous utility function for boot software.
  
Copyright 2005-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who          what, where, why
--------   ---          -------------------------------------------------- 
06/09/14   kedara       Update for 64 bit compilation using LLVM (remove inline "asm")
05/07/14   kedara       Make inlined api boot_get_bit_set_position static
10/25/12   dh           Add boot_count_set_bits
                        Remove pragma to surpass inline assemly since armct5.01 allows it
10/13/11   kedara       Suppress inline assembly warning for rvct4.1
09/01/11   kedara       Added arm pragma.
06/28/10   plc          Fix bug for C-implemented solution for Arm7 CLZ 
04/29/10   dxiang       Add interface for fast_memcpy() and fast_memzi()
03/03/10   plc          Add support for compiling for Arm7 Processor
12/02/09   tnk          Compilation of boot_get_bit_set_position in arm mode 
                        and rest of the file in thumb mode for 512B Nand
                        Support feature. 
11/13/09   tnk          Disableing the boot_get_bit_set_position compilation 
                        if DBL is compiled for Thumbmode. 
8/30/05   John Alayari  Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

#undef  WORDSIZE
#define WORDSIZE   sizeof(int)
#undef  WORDMASK
#define WORDMASK   (WORDSIZE - 1)

typedef uint32 BLONG;

/*=========================================================================
                            
                          FUNCTION DEFINITIONS

=========================================================================*/


/*===========================================================================

**  Function :  boot_strlen

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion calculate the length of a string, excluding the NULL terminator.
*
* @param[in] str  start address of the string
*
* @par Dependencies
*   None
*   
* @retval
*   length of the string, excluding the NULL terminator.
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_strlen(char *str);


/*===========================================================================
**  Function : bload8

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function takes a byte pointer and it returns the byte value in 
*   in 32 bits. 
*   
*   This function ensures that read opeartions from memory are 32 bit 
*   aligned.
* 
* @par Dependencies
*   None
* 
* @param
*   p        - a Byte  Pointer.
*   
* @retval
*   Byte value in 32 bits.
* 
* @par Side Effects
*   None
*/
uint32 bLoad8 (const uint8* p);

/*===========================================================================
**  Function : bstore8

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function takes 32 bits input value and stores it at the address
*   pointed by the input byte pointer.
*   
*   This function ensures that write opeation to memory are 32 bit aligned.
* 
* @par Dependencies
*   None
* 
* @param
*   p        - Byte pointer.
*   val      - 32 bit data to be stored in memory.
*
* @retval
*  None.
* 
* @par Side Effects
*   None
*/
void bStor8 (uint8* p, uint32 val);


/*===========================================================================

**  Function : bbytecompare

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function compares two buffers byte by byte.
* 
*   This function replaces the memcmp utility in the C library and
*   ensures that read opeartions from memory are 32 bit aligned.
* 
*   Note: 
*   
*   Same optimization as bByteCopy could be applied here. but
*   this is not a high runner code in secure boot, optimization is 
*   left for future.
* 
* 
* @par Dependencies
*   None
* 
* @param
*   value1_ptr  - Pointer to buffer 1.
*   value1_ptr  - Pointer to buffer 2.
*   byte_len    - Length to compare
*
* @retval
*  None.
* 
* @par Side Effects
*   None
*/
uint32 bByteCompare(
  const uint8*  value1_ptr,
  const uint8*  value2_ptr,
  const uint32  byte_len
);


/*===========================================================================

**  Function : qmemcpy

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function copies a block of memory, handling overlap.
*   This routine implements memcpy, and memmove. And ensures 
*   that read and write operations from/to memory are 32 bit 
*   aligned.
* 
* @par Dependencies
*   None.
* 
* @param
*   src_ptr           - Pointer to source buffer.
*   dst_ptr           - Pointer to destination buffer.
*   len               - Length to copy.
* 
* @retval
*   None.
* 
* @par Side Effects
*   None.
*/
void qmemcpy( void*, const void*, uint32 );


/*===========================================================================

**  Function : qmemscpy

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   Currently a wrapper for memscpy(), can drop in optimized version of memscpy() as replacement 
*   when optimization is required.
* 
* @par Dependencies
*   None.
* 
* @param
*   destination_ptr      - Pointer to destination buffer.
*   dst_size                 - destination Length to copy.
*   source_ptr             - Pointer to source buffer.
*   src_size                 - source Length to copy.
* 
* @retval
*   None.
* 
* @par Side Effects
*   None.
*/
void qmemscpy( void* destination_ptr, uint32 dst_size, const void* source_ptr, uint32 src_size );


/*===========================================================================

**  Function : qmemset

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function set a block of memory to agiven value.
*  
*   This routine implements memset and ensures that read 
*   and write operations from/to memory are 32 bit aligned 
*   This function sets Len bytes of destination to input value.
* 
*  
* @par Dependencies
*   None.
* 
* @param
*   dst_ptr         - Pointer to destination buffer.
*   val             - Value  to be set
*   len             - Length to set
*
* @retval
*   None.
* 
* @par Side Effects
*   None.
*/
void qmemset (
  void* dst_ptr,
  uint32 val,
  uint32 len
);


/*===========================================================================

**  Function : boot_get_bit_set_position

** ==========================================================================
*/
/*!
* 
* @brief
*   Given a unsigned integer that is a power of 2, this function returns
*   the nth bit set.
* 
* @par Dependencies
*   None
* 
* @retval
*   N'th bit set
* 
* @par Side Effects
*   None
* 
*/
static __inline uint32 boot_get_bit_set_position
(
  uint32 num
)
{
  uint32 shift = 0;

  if (num == 0)
  {
      shift = 32;
  }
  else
  {
    while (!(num & 0x80000000) && num) 
    {
      shift++;
      num = num << 1;
    }
  }
  shift = 31 - shift;

  return shift;

} /* boot_get_bit_set_position() */

/*===========================================================================

**  Function : fast_memcpy

** ==========================================================================
*/
/*!
* 
* @brief
*   This is an assembly optimized implementation of memcpy(). This utilizes
*   the load and store multiple register instructions.
* 
* @par Dependencies
*   Size should be evenly divisible by 4 bytes; if not, it will overwrite
*   up to 3 unspecified bytes.
* 
* @retval
*   None.
* 
* @par Side Effects
*   None.
*/
void fast_memcpy(
  void* dst_ptr,        /* Destination address  */
  const void* src_ptr,  /* Source address       */
  uint32 len            /* Length of copy       */
);

/*===========================================================================

**  Function : fast_memzi

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This is an assembly optimized implementation of memzi(). This utilizes
*   the store multiple registers instruction.
* 
* @par Dependencies
*   None 
* 
* @retval
*   None.
* 
* @par Side Effects
*   None.
*/
void fast_memzi(
  void* dst_ptr,        /* Destination address */ 
  uint32 len            /* Length to be to set to zero */
);


/*===========================================================================

**  Function : boot_count_set_bits
 
** ==========================================================================
*/
/*!
* 
* @brief
*    This function Counts the number of bits set in an uint32.
*   
* @param[in] input_val:  uint32 Input value whose set bits need to be 
*                        counted. 
* 
* @par Dependencies
*    None
* 
* @retval
*    count: uint32 Number of bits set to 1.
* 
* @par Side Effects
*    None
* 
*/
uint32 boot_count_set_bits(uint32 input_val);

#endif /* BOOT_UTIL_H */


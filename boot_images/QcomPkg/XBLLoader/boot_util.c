/*=============================================================================
                        Boot Utility File 

GENERAL DESCRIPTION     
  This module defines miscellaneous utility function for boot software.
  
Copyright 2005, 2010, 2012, 2014-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who         what, where, why
--------   ---         --------------------------------------------------
10/05/15   ck          Corrected copyright
05/22/15   as          Updates for AArch64 addresses.
07/16/14   ck          Added string include for memscpy usage
06/09/14   kedara      Add support for 64 bit compilation using llvm.
                       add uintnt cast to support 64-bit armv8
10/25/12   dh          Add boot_count_set_bits
09/27/10   dh          add boot_strlen
08/30/05   ja          Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <string.h>
#include "boot_util.h"
#include "LoaderUtils.h" 
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
uint32 boot_strlen(char *str)
{
   char * ptr;

   for (ptr = str; *ptr; ptr++);
   
   return ptr - str;
}
/*boot_strlen*/


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
uint32 bLoad8 (const uint8* p)
{  
  /* align the address to 64 bits boundary */
  uintnt aligned_p = ((uint64)p & (uint64)(~0x00000003)); 
  
  /* byte position in 64 bit address */  
  uintnt index = ((uint64)p & (uint64) 0x00000003);
  /* - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - */
  
  /* return the byte in 32 bits after masking other bytes with zero */  
  return (( *((uint32*)(uintnt) aligned_p) >> (index<<3)) &  0x000000FF ) ;
}
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
* @retval
*  None.
* 
* @par Side Effects
*   None
*/
void bStor8 (uint8* p, uint32 val)
{  
  /* align the address to 64 bits boundary */
  uintnt aligned_p = ((uintnt) p & (uintnt)(~0x00000003));
  
  /* byte position in 64 bit address */  
  uintnt index = ((uintnt) p & (uintnt)0x00000003);
  
  /* read the current data at 64 bit aligned address */
  uint32 current_value = *((uintnt*) aligned_p);
  /* - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - */
  
  /* ------------------------------------------------------------------ */
  /* store the input value after shifting it in the proper position and */
  /* ORing it with current_value after the target byte in 32 bit        */
  /* current_value is cleared by masking it with zero                   */
  /* ------------------------------------------------------------------ */
  *((uint32*)(uintnt) aligned_p) =  ((current_value & ~(0xFF << (index<<3)))) | 
                             (val<< (index <<3));
}
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
* @par Dependencies
*   None
* 
* @param
*   value1_ptr  - Pointer to buffer 1.
*   value1_ptr  - Pointer to buffer 2.
*   byte_len    - Lenght to compare
* @retval
*  None.
* 
* @par Side Effects
*   None
*/
uint32 bByteCompare
(
  const uint8*  value1_ptr,
  const uint8*  value2_ptr,
  const uint32  byte_len
)
{
  int   i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for ( i=0; i< (int) byte_len; i++ )
  {
    if ( bLoad8((uint8*) (value1_ptr++)) != 
         bLoad8((uint8*) (value2_ptr++)) 
       ) 
      return (uint32)(i+1);
  }

  return 0;
} /* bByteCompare */

/*===========================================================================

**  Function : qmemcpy

** ==========================================================================
*/
/*!
* 
* @brief
*   
*   This function is now a wrapper for memscpy() and will be deprecated in future releases.  Please
*   Use qmemscpy() to replace.
* 
* @par Dependencies
*   None.
* 
* @param
*   source_ptr           - Pointer to source buffer.
*   destination_ptr      - Pointer to destination buffer.
*   len                  - Length to copy.
* 
* @retval
*   None.
* 
* @par Side Effects
*   None.
*/
void qmemcpy( void* destination_ptr, const void* source_ptr, uint32 len )
{
  memscpy(destination_ptr, len, source_ptr, len);
}/* qmemcpy */

          
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
void qmemscpy( void* destination_ptr, uint32 dst_size, const void* source_ptr, uint32 src_size )
{
  memscpy(destination_ptr, dst_size, source_ptr, src_size);
} /* qmemscpy */


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
uint32 boot_count_set_bits(uint32 input_val)
{
  uint32 count = 0;
  for(count = 0; input_val; count++)
  {
    input_val &= input_val - 1 ;
  }
  return count;
}

void qmemset (
  void* dst_ptr,
  uint32 val,
  uint32 len
)
{
  //
  // Declare the local variables that actually move the data elements as
  // volatile to prevent the optimizer from replacing this function with
  // the intrinsic memset()
  //
  volatile UINT8                    *Pointer8;
  volatile UINT32                   *Pointer32;
  volatile UINT64                   *Pointer64;
  UINT32                            Value32;
  UINT64                            Value64;

  if ((((UINTN)dst_ptr & 0x7) == 0) && (len >= 8)) {
    // Generate the 64bit value
    Value32 = (val << 24) | (val << 16) | (val << 8) | val;
    Value64 = (((UINT64)Value32) << 32) | Value32;

    Pointer64 = (UINT64*)dst_ptr;
    while (len >= 8) {
      *(Pointer64++) = Value64;
      len -= 8;
    }

    // Finish with bytes if needed
    Pointer8 = (UINT8*)Pointer64;
    while (len-- > 0) {
      *(Pointer8++) = val;
    }
  } else if ((((UINTN)dst_ptr & 0x3) == 0) && (len >= 4)) {
    // Generate the 32bit value
    Value32 = (val << 24) | (val << 16) | (val << 8) | val;

    Pointer32 = (UINT32*)dst_ptr;
    while (len >= 4) {
      *(Pointer32++) = Value32;
      len -= 4;
    }

    // Finish with bytes if needed
    Pointer8 = (UINT8*)Pointer32;
    while (len-- > 0) {
      *(Pointer8++) = val;
    }
  } else {
    Pointer8 = (UINT8*)dst_ptr;
    while (len-- > 0) {
      *(Pointer8++) = val;
    }
  }
}



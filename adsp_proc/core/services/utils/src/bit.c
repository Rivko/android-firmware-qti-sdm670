/*===========================================================================

              B I T    M A N I P U L A T I O N    S E R V I C E S

                            D E C L A R A T I O N S

DESCRIPTION
  The following declarations are for use with the Bit Manipulation Services.

Copyright (c) 1991, 1992, 1998 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1999--2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/bit.c_v   1.2   03 Oct 2001 10:18:58   rajeevg  $
$Header: //components/rel/core.qdsp6/2.1/services/utils/src/bit.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/07   enj     Fixing lint errors
10/03/01    gr     Moved bit manipulation macros from bit.c into bit.h. Added
                   explicit inclusion of comdef.h and qw.h instead of depending
                   on bit.h to pick up these files.
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                     Added support for MSM5000_IRAM_FWD
04/09/99    ms     Lint cleanup.
04/01/99    ms     Lint cleanup.
03/31/99    ms     Added two new functions for MDR use...
                       b_copy_ltobw_s1d0 and b_copy_ltobw_s2d0.
01/27/99    ms     Fixed masking problems in b_packw, b_packd and b_packq.
01/26/99    ms     Masking src before ORing into dst in function b_packb.
11/20/98    ms     Added function b_copy_litend_to_bigend_word.
08/29/98   jct     Removed unused code, upgraded comments
07/18/98   udp     Ported to ARM and updated for release
12/20/96   rdh     Changed functions to Pascal calling convention.
07/07/92   ip      Release to DMSS PVCS.
04/29/92   gb      Large model version.
04/23/92   ip      Initial porting of file from Brassboard to DMSS.
01/23/91   rdb     Created first cut of file.

===========================================================================*/
/*lint +fpm Precision of an operator is bound by max precision of its operands */

#include "comdef.h"
#include "bit.h"
#include "qw.h"
#ifndef FEATURE_WINCE
#include "amssassert.h"
#else
#include "assert.h"
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* --------------------------------------------------
** Macro to set destination buffer with the given
** data item
** -------------------------------------------------- */
#define FLUSH_DATA(dst, data) (*(dst) = (data))

#ifndef MSM5000_IRAM_FWD
/*============================================================================

FUNCTION B_PACKB

DESCRIPTION
  Packs the given byte into the destination at the given offset for the
  given number of length bits

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_packb(
   byte src, 
   byte dst[], 
   word pos, 
   word len 
)
{
   word   t_pos = pos % 8;
   word   bits  = 8 - t_pos;

   ASSERT( dst != NULL );
   dst += (pos+len-1)/8;

   if ( bits >= len )
   {
       *dst &= (byte) ~MASK_B(t_pos, len);
       *dst |= (byte) (MASK_B(t_pos, len) & (src << (bits - len)));
   }
   else /* len > bits */
   {
       dst--;
       *dst &= (byte) ~MASK_B(t_pos, bits);
       *dst |= (byte) (MASK_B(t_pos, bits) & (src >> (len - bits)));

       dst++;
       *dst &= (byte) ~MASK_B(0, (len - bits));
       *dst |= (byte) (MASK_B(0, (len - bits)) & (src << (8 - (len - bits))));
   }
} /* END b_packb */
#endif /*MSM5000_IRAM_FWD*/

#ifndef MSM5000_IRAM_FWD
/*============================================================================

FUNCTION B_PACKW

DESCRIPTION
  Packs the given word into the destination at the given offset for the
  given number of length bits

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_packw(
   word src, 
   byte dst[], 
   word pos, 
   word len 
)
{
   int bits, start;
   byte   mask;
 
   ASSERT( dst != NULL );
   dst += (len+pos-1)/8;        /* point to last byte to be written */
   pos  = (len+pos-1)%8;        /* index of last bit to be written */
   
   if (len > pos)  /* if we are filling all of the left part of the byte */
   {
     start = 0;  
   }
   else            /* There are going to be untouched bits at left of 
                   ** destination byte.                                   */
   {
     start = (pos+1) - len;     
   }
   bits = (pos - start) + 1;    /* # of bits to be written in this byte */
   
   *dst &= (byte) ~MASK_B(start,bits);  /* clear the bits to be written */
   
   *dst |= (byte) (   ( src << (7 - pos) )    /* left-shift src to line up */
                    & MASK_B(start, bits) );  /* only touch desired bits */
   
   dst--;                /* back up one byte */
   src >>= bits;         /* get rid of bits we've consumed already */
   
   if(len > bits)        /* if we need to write into other bytes */
   {
     len -= bits;        /* compute remaining length  */
     
     /* for full bytes, we can just overwrite the old value with the new */
     for ( ; len >= 8 ; len -= 8 ) {
       *dst = (byte)( src );
       dst--;                         /* back up one byte */
       src >>= 8;                     /* get rid of used bits */
     }
 
     if (len > 0)     /* if some bits are leftover... */
     {
       mask = (byte) (0xff << len);   
       *dst &= mask;                  /* clear bits on right side of byte */
       *dst |= ( (byte)( src ) & ~mask);        /* set appropriate bits */
     }
 
   }
} /* END b_packw */
#endif /*MSM5000_IRAM_FWD*/

  
/*============================================================================

FUNCTION B_PACKD

DESCRIPTION
  Packs the given dword into the destination at the given offset for the
  given number of length bits

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_packd(
   dword src, 
   byte dst[], 
   word pos, 
   word len 
)
{
   int bits, start;
   byte mask;
 
   ASSERT( dst != NULL );
   dst += (len+pos-1)/8;    /* point to last byte to be written */
   pos  = (len+pos-1)%8;    /* index of last bit to be written  */
 
   if (len > pos )  /* if we are filling all of the left part of the byte */
   {
     start = 0;        
   }
   else             /* some of the left bits of byte won't get touched */
   {
     start = (pos+1) - len;
   }
   bits = (pos - start) + 1;     /* # of bits to be written into this byte */
   
   *dst &= (byte) ~MASK_B(start,bits);   /* clear the bits to be written */
   
   *dst |= (byte) (  (src << (7 - pos) )     /* left shift src to line up */
                    & MASK_B(start,bits) );  /* only touch desired bits */
   
   dst--;                /* back up one byte */
   src >>= bits;         /* get rid of bits we've consumed already */
   
   if(len > bits)        /* if we need to write into other bytes */
   {
     len -= bits;                        /* compute remaining length */
     
     /* for full bytes, we can just overwrite the old value with the new */
     for ( ; len >= 8 ; len -= 8 ) {
       *dst = (byte)( src );
       dst--;                            /* back up one byte */
       src >>= 8;                        /* get rid of consumed bits */
     }
 
     if (len > 0)                /* if some bits are leftover .... */
     {
        mask = (byte) (0xff << len);
        *dst &= mask;            /* clear bits on right side of byte */
        
       *dst |= ( (byte)( src ) & ~mask );     /* set appropriate bits */
     }
   }
} /* END b_packd */
  
  
/*============================================================================

FUNCTION B_PACKQ

DESCRIPTION
  Packs the given qword into the destination at the given offset for the
  given number of length bits

DEPENDENCIES
  None
   
RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_packq(
   qword src, 
   byte dst[], 
   word pos, 
   word len 
)
{
   int bits, start;
   dword src_hi,src_lo;
   byte mask;
 
   src_hi = qw_hi( src);          /* grab high order dword */
   src_lo = qw_lo( src);          /* grab low order dword  */
 
   ASSERT( dst != NULL );
   dst += (len+pos-1)/8;          /* point to last byte to be written to */
   pos  = (len+pos-1)%8;          /* index of last bit to be written to  */
 
   if (len > pos )  /* if we are filling all of the left part of the byte */
   {
     start = 0;
   }
   else             /* Some bits on the left won't be touched */
   {
     start = (pos+1) - len;
   }
   bits = (pos - start) + 1;   /*# of bits to be written to this byte */
   
   *dst &= (byte) ~MASK_B(start, bits);   /* clear the bits to be written */
   
   *dst |= (byte) ( (src_lo << (7 - pos) )  /* left shift src to line up */  
                    & MASK_B(start,bits) ); /* only touch desired bits */

   dst--;                  /* back up one byte */
 
   src_lo >>= bits;        /* get rid of bits we've already consumed */
   
 
   if ( len > bitsize(dword) )  /* if any data in high-order dword */
   {
     /* move data from low-end of high-order dword into low-order dword */
     src_lo |= src_hi << ( bitsize(dword) - bits );  
     src_hi >>= bits;
   } 
   
   if(len > bits)    /* if we need to write into other bytes */
   {
     len -= bits;    /* compute remaining length             */
     
     /* for full bytes, just overwrite old value with new    */
     for ( ; len >= 8 ; len -= 8 ) {
       *dst = (byte)( src_lo );
       dst--;                                       /* backup a byte  */
       src_lo >>= 8;
       
       /* if there is still data in the high word */
       if ( len > bitsize(dword) ) 
       {
         /* grab 8 more bits from the high order word */
         src_lo |= src_hi << ( bitsize(dword) - 8 );
         src_hi >>= 8;                              /* get rid of used bits */
       }
     }
 
     if (len > 0)
     {
       mask = (byte) (0xff << len);
       *dst &= mask;               /* clear bits on right-hand side of byte */
       *dst |=  ( (byte)( src_lo ) & ~mask );
     }
 
   }
} /* END b_packq */


/*============================================================================

FUNCTION B_UNPACKQ

DESCRIPTION
  Given a buffer and an offset, unpacks the requested number of bits into
  a qword

DEPENDENCIES
  None
   
RETURN VALUE
  Unpacked item

SIDE EFFECTS
  None
   
============================================================================*/
void b_unpackq(
   byte src[], 
   word pos, 
   word len, 
   qword qw
)
{ 
   dword result_lo = 0;
   dword result_hi = 0;
   word bitcount = 0;
   int rshift = 0;

   qw_set(qw,0,0);

   ASSERT( src != NULL );
   src += pos/8;
   pos %= 8;
   
   rshift = MAX( 8 - (pos + len), 0);

   if ( rshift > 0 ) {

      result_lo = MASK_AND_SHIFT(len, pos, rshift, *src);      // less than 8 bits contained in only 1 byte

   } else {

      result_lo = MASK(8-pos, pos, *src);
      src++;
      bitcount = 8-pos;
      len -= bitcount;

      for ( ; len >= 8 ; len-=8)
      {
         if ( bitcount > (bitsize(dword)-8) ) {
            result_hi = result_hi<<8 | result_lo>>(bitsize(dword)-8);
         }
         result_lo = ( result_lo<<8 ) | *src++;
         bitcount += 8;
      }

      if ( len > 0 ) {
         
         if ( bitcount > (bitsize(dword)-8) ) {
            result_hi = result_hi<<len | result_lo>>(bitsize(dword)-len);
         }
         result_lo = ( result_lo<<len ) | (*src >> (8-len));  // if any bits left
      }
   }

   qw_set(qw,result_hi,result_lo);
} /* END b_unpackq */

#ifndef MSM5000_IRAM_FWD
/*============================================================================

FUNCTION B_UNPACKB

DESCRIPTION
  Given a buffer and an offset, unpacks the requested number of bits into
  a byte

DEPENDENCIES
  None
   
RETURN VALUE
  Unpacked item

SIDE EFFECTS
  None
   
============================================================================*/
byte b_unpackb(
   byte *src, 
   word pos, 
   word len
)
{
   byte result = 0;
   int rshift = 0;

   ASSERT( src != NULL );
   src += pos/8;
   pos %= 8;
   
   rshift = MAX( 8 - (pos + len), 0);

   if ( rshift > 0 ) {

     result = MASK_AND_SHIFT(len, pos, rshift, *src);
  
   } else {

     result = MASK(8-pos, pos, *src);
     src++;
     len -= 8 - pos;

      if ( len > 0 ) result = ( result<<len ) | (*src >> (8-len));  // if any bits left
   }

   return result;
} /* END b_unpackb */
#endif /*MSM5000_IRAM_FWD*/

#ifndef MSM5000_IRAM_FWD
/*============================================================================

FUNCTION B_UNPACKW

DESCRIPTION
  Given a buffer and an offset, unpacks the requested number of bits into
  a word

DEPENDENCIES
  None
   
RETURN VALUE
  Unpacked item

SIDE EFFECTS
  None
   
============================================================================*/
word b_unpackw(
   byte src[], 
   word pos, 
   word len
)
{ 
   word result = 0;
   int rshift = 0;

   ASSERT( src != NULL );
   src += pos/8;
   pos %= 8;

   rshift = MAX( 8 - (pos + len), 0);

   if ( rshift > 0 ) {

     result = MASK_AND_SHIFT(len, pos, rshift, *src);
  
   } else {

      result = MASK(8-pos, pos, *src);
      src++;
      len -= 8 - pos;

      for ( ; len >= 8  ; len-=8)
      {
         result = ( result<<8 ) | *src++;
      }

      if ( len > 0 ) result = ( result<<len ) | (*src >> (8-len));  // if any bits left
   }

   return result;
} /* END b_unpackw */
#endif /*MSM5000_IRAM_FWD*/


/*============================================================================

FUNCTION B_UNPACKD

DESCRIPTION
  Given a buffer and an offset, unpacks the requested number of bits into
  a dword

DEPENDENCIES
  None
   
RETURN VALUE
  Unpacked item

SIDE EFFECTS
  None
   
============================================================================*/
dword b_unpackd(
   byte src[], 
   word pos, 
   word len
)
{ 
  dword result = 0;
  int rshift = 0;

  ASSERT( src != NULL );
  src += pos/8;
  pos %= 8;

  rshift = MAX( 8 - (pos + len), 0);

  if ( rshift > 0 )  {

     result = MASK_AND_SHIFT(len, pos, rshift, *src);
 
  } else {

     result = MASK(8-pos, pos, *src);
     src++;
     len -= 8 - pos;

     for ( ; len >= 8  ; len-=8)
     {
        result = ( result<<8 ) | *src++;
     }

     if ( len > 0 ) result = ( result<<len ) | (*src >> (8-len));  // if any bits left
  }

  return result;
} /* END b_unpackd */

#ifndef MSM5000_IRAM_FWD
/*============================================================================

FUNCTION B_COPY

DESCRIPTION
  This function copies a specified bit field of a specified length to 
  another specified bit field. Note: b_copy is not intended for use
  with overlapping bit fields.
  
FORMAL PARAMETERS
  p_sbuf = source bit field buffer.
  p_spos = source bit field offset.
  p_dbuf = destination bit field buffer.
  p_dpos = destination bit field offset.
  p_len  = length, in bits, of bit field to be copied.

DEPENDENCIES
  b_copy assumes that the 2 specified bit fields do not overlap.

RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_copy( 
   void *src_ptr, 
   word  spos, 
   void  *dst_ptr, 
   word  dpos, 
   word  len 
)
{
  // Special cases
  // spos == dpos : Phase 1 will consume a whole word of dst, then the aligned case will be used in phase 2.
  // spos = 0     : go directly to phase 2
  // dpos < spos  : Phase 1 consumes 2 words from dst.
  // dpos > spos  : Phase 1 consumes 1 word of dst
  // len < spos   : return from phase 1.

  byte this_val, *src = (byte *) src_ptr, *dst = (byte *) dst_ptr;
  const int copysize = bitsize(*src);
  int bit_shift, this_len;

#ifdef BIT_DEBUG
  curr_mask = 0;
#endif

  if(len == 0) return;

  // Calculate the src and dst addresses on aligned boundries.
  ASSERT( src != NULL );
  src += spos / copysize;
  spos %= copysize;

  ASSERT( dst != NULL );
  dst += dpos / copysize;
  dpos %= copysize;

  bit_shift = dpos - spos;

  // Phase 1

  // Get bits from dst < dpos.

  this_val = (dpos == 0) ? 0 : MASK(dpos, 0, *dst);

  if(spos != 0) {
    // Align src.
    int align_length = MIN( copysize - spos, len);
    
    if((dpos + align_length) <= copysize) {
      // bit_shift < 0
      // Copy takes one word from dst.

      this_val |= MASK_AND_SHIFT(align_length, spos, bit_shift, *src);

      // Get bits from dst if the length of the copy will not cover it.
      if(len == align_length) {
        if(copysize - (align_length + dpos)) {
          this_val |= MASK(copysize - (align_length + dpos), align_length + dpos, *dst);
        }

        FLUSH_DATA(dst++, this_val);
        return;
      }

      len -= (word) align_length;
      if(len == 0) {
        FLUSH_DATA(dst++, this_val);
        return;
      }

      // Increment dst if we finished the whole word.
      if((dpos + align_length) == copysize) {
        // This implies spos == dpos.
        FLUSH_DATA(dst++, this_val);
        this_val = 0;
      }

      bit_shift = (copysize+bit_shift) % copysize;

    } else {
      // Copy takes two words from dst.
      // bit_shift > 0

      this_val |= MASK_AND_SHIFT(copysize - dpos, spos, bit_shift, *src);

      FLUSH_DATA(dst++, this_val);

      align_length -= copysize - dpos;
      len -= copysize - dpos;

      this_val = MASK_AND_SHIFT(align_length, copysize - bit_shift, bit_shift - copysize, *src);
      len -= align_length;

      // Get bits from dst if the bits in src will not cover it.
      if(len == 0) {
        if(copysize - align_length) {
          this_val |= MASK(copysize - align_length, align_length, *dst);
        }

        FLUSH_DATA(dst++, this_val);
        return;
      }
    }
    src++;
  }

  // Phase 2
  // spos always == 0 
  // spos and dpos unused below this. 
  // Break out the easy case.

  if(bit_shift == 0) {
    // note that this_val is null here since we finished of dst when we aligned it above.
    while(len >= copysize) {
      *dst++ = *src++;
      len -= copysize;
    }
    if(len == 0) return;
  } else {

    // Note that existing data in dst is only important on the ends.
    while(len >= copysize) {
      
      // High order part of src goes to low order of dst.
      
      this_val |= MASK_AND_SHIFT( copysize - bit_shift, 0, bit_shift, *src);

      FLUSH_DATA(dst++, this_val);

      this_val  = MASK_AND_SHIFT( bit_shift, copysize - bit_shift, bit_shift - copysize, *src);
      src++;
      len -= copysize;
    }
  }

  // Phase 3
  // Copy the leftovers.

  if(len == 0) {
    // Here we know that we are src aligned. In the loop above we added bit_shift bits to this_val
    // so there is always room for copysize-bit_shift bits in *dst.

    this_val |= MASK( copysize - bit_shift, bit_shift, *dst);

    FLUSH_DATA(dst++, this_val);


  } else if( len <= (copysize - bit_shift)) {
    // Copy takes one word from dst.

    // High order part of src goes to low order of dst.

    this_val |= MASK_AND_SHIFT( len, 0, bit_shift, *src);

    this_len = copysize - bit_shift - len;
    if(this_len) {

      this_val |= MASK(this_len, copysize - this_len, *dst);
    }

    FLUSH_DATA(dst++, this_val);

  } else {
    // Copy takes two words from dst.

    this_val |= MASK_AND_SHIFT(copysize - bit_shift, 0, bit_shift, *src);

    FLUSH_DATA(dst++, this_val);

    len -= copysize - bit_shift;
    
    // low order part of src goes to high order part of dst
    this_val  = MASK_AND_SHIFT( len, copysize-bit_shift, bit_shift-copysize, *src);
    this_val |= MASK(copysize - len, len, *dst);

    FLUSH_DATA(dst++, this_val);

  }

} /* End b_copy */
#endif /*MSM5000_IRAM_FWD*/

/*============================================================================

FUNCTION B_COPY_LITEND_TO_BIGEND_WORD

DESCRIPTION
  This function is semantically identical to b_copy.
  It treats the source buffer as a little-endian word buffer and the
  destination buffer as a big-endian word buffer. The output buffer pointer
  should not be used as a word buffer on a little-endian machine.

  The code is exactly the same as in b_copy except for the operation
  on pointers. Since the source buffer is considered little-endian,
  all arithmetic on the source pointer is done using the macro LITLEND_PTR_ADD.
  
FORMAL PARAMETERS
  src_ptr = source bit field buffer.
  spos    = source bit field offset.
  dst_ptr = destination bit field buffer.
  dpos    = destination bit field offset.
  len     = length, in bits, of bit field to be copied.

DEPENDENCIES
  This function assumes that the 2 specified bit fields do not overlap.
  Users must be very careful about the kind of buffer they pass to this
  function and should clearly understand what they will get in the 
  destination buffer.

RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_copy_litend_to_bigend_word( 
   word *src_ptr, 
   word  spos, 
   byte  *dst_ptr, 
   word  dpos, 
   word  len 
)
{
  // Special cases
  // spos == dpos : Phase 1 will consume a whole word of dst, then the aligned case will be used in phase 2.
  // spos = 0     : go directly to phase 2
  // dpos < spos  : Phase 1 consumes 2 words from dst.
  // dpos > spos  : Phase 1 consumes 1 word of dst
  // len < spos   : return from phase 1.

  byte this_val;
  byte *src = (byte *) src_ptr;
  byte *dst = (byte *) dst_ptr;
  const int copysize = bitsize(*src);
  int bit_shift, this_len;

#ifdef BIT_DEBUG
  curr_mask = 0;
#endif

  ASSERT( src != NULL );
  src++;
  
  if(len == 0) return;

  // Calculate the src and dst addresses on aligned boundries.
  src = (byte *)LITLEND_PTR_ADD(src, spos/copysize);
  spos %= copysize;

  ASSERT( dst != NULL );
  dst += dpos/copysize;
  dpos %= copysize;

  bit_shift = dpos - spos;

  // Phase 1

  // Get bits from dst < dpos.

  this_val = (dpos == 0) ? 0 : MASK(dpos, 0, *dst);

  if(spos != 0) {
    // Align src.
    int align_length = MIN( copysize - spos, len);
    
    if((dpos + align_length) <= copysize) {
      // bit_shift < 0
      // Copy takes one word from dst.

      this_val |= MASK_AND_SHIFT(align_length, spos, bit_shift, *src);

      // Get bits from dst if the length of the copy will not cover it.
      if(len == align_length) {
        if(copysize - (align_length + dpos)) {
          this_val |= MASK(copysize - (align_length + dpos), align_length + dpos, *dst);
        }

        FLUSH_DATA(dst++, this_val);
        return;
      }

      len -= (word) align_length;
      if(len == 0) {
        FLUSH_DATA(dst++, this_val);
        return;
      }

      // Increment dst if we finished the whole word.
      if((dpos + align_length) == copysize) {
        // This implies spos == dpos.
        FLUSH_DATA(dst++, this_val);
        this_val = 0;
      }

      bit_shift = (copysize+bit_shift) % copysize;

    } else {
      // Copy takes two words from dst.
      // bit_shift > 0

      this_val |= MASK_AND_SHIFT(copysize - dpos, spos, bit_shift, *src);

      FLUSH_DATA(dst++, this_val);

      align_length -= copysize - dpos;
      len -= copysize - dpos;

      this_val = MASK_AND_SHIFT(align_length, copysize - bit_shift, bit_shift - copysize, *src);
      len -= align_length;

      // Get bits from dst if the bits in src will not cover it.
      if(len == 0) {
        if(copysize - align_length) {
          this_val |= MASK(copysize - align_length, align_length, *dst);
        }

        FLUSH_DATA(dst++, this_val);
        return;
      }
    }
    src = (byte *)LITLEND_PTR_ADD(src, 1);
  }

  // Phase 2
  // spos always == 0 
  // spos and dpos unused below this. 
  // Break out the easy case.

  if(bit_shift == 0) {
    // note that this_val is null here since we finished of dst when we aligned it above.
    while(len >= copysize) {
      *dst++ = *src;
      src = (byte *)LITLEND_PTR_ADD(src, 1);
      len -= copysize;
    }
    if(len == 0) return;
  } else {

    // Note that existing data in dst is only important on the ends.
    while(len >= copysize) {
      
      // High order part of src goes to low order of dst.
      
      this_val |= MASK_AND_SHIFT( copysize - bit_shift, 0, bit_shift, *src);

      FLUSH_DATA(dst++, this_val);

      this_val  = MASK_AND_SHIFT( bit_shift, copysize - bit_shift, bit_shift - copysize, *src);
      src =  (byte *)LITLEND_PTR_ADD(src, 1);
      len -= copysize;
    }
  }

  // Phase 3
  // Copy the leftovers.

  if(len == 0) {
    // Here we know that we are src aligned. In the loop above we added bit_shift bits to this_val
    // so there is always room for copysize-bit_shift bits in *dst.

    this_val |= MASK( copysize - bit_shift, bit_shift, *dst);

    FLUSH_DATA(dst++, this_val);


  } else if( len <= (copysize - bit_shift)) {
    // Copy takes one word from dst.

    // High order part of src goes to low order of dst.

    this_val |= MASK_AND_SHIFT( len, 0, bit_shift, *src);

    this_len = copysize - bit_shift - len;
    if(this_len) {

      this_val |= MASK(this_len, copysize - this_len, *dst);
    }

    FLUSH_DATA(dst++, this_val);

  } else {
    // Copy takes two words from dst.

    this_val |= MASK_AND_SHIFT(copysize - bit_shift, 0, bit_shift, *src);

    FLUSH_DATA(dst++, this_val);

    len -= copysize - bit_shift;
    
    // low order part of src goes to high order part of dst
    this_val  = MASK_AND_SHIFT( len, copysize-bit_shift, bit_shift-copysize, *src);
    this_val |= MASK(copysize - len, len, *dst);

    FLUSH_DATA(dst++, this_val);

  }
} /* End b_copy_litend_to_bigend_word */ 

/*============================================================================

FUNCTION B_COPY_LTOBW_S1D0

DESCRIPTION
  This function is a special case of b_copy_litend_to_bigend_word.
  It assumes that spos = 1 and dpos=0;

  
FORMAL PARAMETERS
  src_ptr = source bit field buffer.
  dst_ptr = destination bit field buffer.
  len     = length, in bits, of bit field to be copied.

DEPENDENCIES
  This function assumes that the 2 specified bit fields do not overlap.
  Users must be very careful about the kind of buffer they pass to this
  function and should clearly understand what they will get in the 
  destination buffer.

RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_copy_ltobw_s1d0 (
   word *src_ptr, 
   byte  *dst_ptr, 
   word  len 
)
{
  byte this_val;
  byte *src = (byte *) src_ptr;
  byte *dst = (byte *) dst_ptr;
  int this_len;

#ifdef BIT_DEBUG
  curr_mask = 0;
#endif

  ASSERT( dst != NULL );
  ASSERT( src != NULL );
  src++;
  
  if(len == 0) return;

  /* Get bits from dst < dpos. */
  this_val = 0;
  /* Copy takes one word from dst. */
  this_val |= MASK_AND_SHIFT_LEFT(7, 1, 1, *src);
  len -= 7;
  src = (byte *)LITLEND_PTR_ADD(src, 1);

  /* Note that existing data in dst is only important on the ends. */
  while(len >= 8) {
      
    /* High order part of src goes to low order of dst. */
      
    this_val |= MASK_AND_SHIFT_RIGHT( 1, 0, 7, *src);

    FLUSH_DATA(dst++, this_val);

    this_val  = MASK_AND_SHIFT_LEFT( 7, 1, 1, *src);
    src =  (byte *)LITLEND_PTR_ADD(src, 1);
    len -= 8;
  }

  /* Copy the leftovers. */

  if(len == 0) {
    /* Here we know that we are src aligned. 
     */

    this_val |= MASK( 1, 7, *dst);

    FLUSH_DATA(dst, this_val);


  } else if( len <= 1) {
    /* Copy takes one word from dst. */
    /* High order part of src goes to low order of dst. */

    this_val |= MASK_AND_SHIFT_RIGHT( len, 0, 7, *src);

    this_len = 1 - len;
    if(this_len) {

      this_val |= MASK(this_len, 8 - this_len, *dst);
    }

    FLUSH_DATA(dst, this_val);

  } else {
    /* Copy takes two words from dst. */

    this_val |= MASK_AND_SHIFT_RIGHT(1, 0, 7, *src);

    FLUSH_DATA(dst++, this_val);

    len -= 1;
    
    /* low order part of src goes to high order part of dst */
    this_val  = MASK_AND_SHIFT_LEFT( len, 1, 1, *src);
    this_val |= MASK(8 - len, len, *dst);

    FLUSH_DATA(dst, this_val);

  }
} /* End b_copy_ltobw_s1d0 */ 


/*============================================================================

FUNCTION B_COPY_LTOBW_S2D0

DESCRIPTION
  This function is a special case of b_copy_litend_to_bigend_word.
  It assumes that spos = 2 and dpos = 0;
  
FORMAL PARAMETERS
  src_ptr = source bit field buffer.
  dst_ptr = destination bit field buffer.
  len     = length, in bits, of bit field to be copied.

DEPENDENCIES
  This function assumes that the 2 specified bit fields do not overlap.
  Users must be very careful about the kind of buffer they pass to this
  function and should clearly understand what they will get in the 
  destination buffer.

RETURN VALUE
  None

SIDE EFFECTS
  None
   
============================================================================*/
void b_copy_ltobw_s2d0 (
   word *src_ptr, 
   byte  *dst_ptr, 
   word  len 
)
{
  byte this_val;
  byte *src = (byte *) src_ptr;
  byte *dst = (byte *) dst_ptr;
  int this_len;

#ifdef BIT_DEBUG
  curr_mask = 0;
#endif

  ASSERT( dst != NULL );
  ASSERT( src != NULL );
  src++;
  
  if(len == 0) return;

  /* Get bits from dst < dpos. */

  this_val = 0;
  this_val |= MASK_AND_SHIFT_LEFT(6, 2, 2, *src);
  len -= 6;
  src = (byte *)LITLEND_PTR_ADD(src, 1);

  /* Note that existing data in dst is only important on the ends. */
  while(len >= 8) {
      
    /* High order part of src goes to low order of dst. */
      
    this_val |= MASK_AND_SHIFT_RIGHT( 2, 0, 6, *src);

    FLUSH_DATA(dst++, this_val);

    this_val  = MASK_AND_SHIFT_LEFT( 6, 2, 2, *src);
    src =  (byte *)LITLEND_PTR_ADD(src, 1);
    len -= 8;
  }

  /* Copy the leftovers. */

  if(len == 0) {
    /* Here we know that we are src aligned. 
     */

    this_val |= MASK( 2, 6, *dst);

    FLUSH_DATA(dst, this_val);


  } else if( len <= 2) {
    /* Copy takes one word from dst. */
    /* High order part of src goes to low order of dst. */

    this_val |= MASK_AND_SHIFT_RIGHT( len, 0, 6, *src);

    this_len = 2 - len;
    if(this_len) {

      this_val |= MASK(this_len, 8 - this_len, *dst);
    }

    FLUSH_DATA(dst, this_val);

  } else {
    /* Copy takes two words from dst. */

    this_val |= MASK_AND_SHIFT_RIGHT(2, 0, 6, *src);

    FLUSH_DATA(dst++, this_val);

    len -= 2;
    
    /* low order part of src goes to high order part of dst */
    this_val  = MASK_AND_SHIFT_LEFT( len, 2, 2, *src);
    this_val |= MASK(8 - len, len, *dst);

    FLUSH_DATA(dst, this_val);

  }
} /* End b_copy_ltobw_s2d0 */ 

/*lint -fpm */

/*============================================================================

                           !!  BIT UNIT TEST  !!

DESCRIPTION
   Provides test routines to compile and run the bit code off target and
   displaying results via printf

============================================================================*/

// #define BIT_UNIT_TEST
#if defined(BIT_UNIT_TEST)

/* -------------------------------------------------
** Support macros for printing bit fields
** ------------------------------------------------- */
#define bit(val, x) (((1 << (x)) & (val)) >> (x))
#define nibble(val, x) (((0x0f << ((x) * 4)) & (val)) >> (x))

/* -------------------------------------------------
** Routine to print nibbles of a byte
** ------------------------------------------------- */
void print_nibbles(byte val)
{
  extern int printf(char *, ...);
  int x;
  for(x=bitsize(byte)/4; x; x--) {
    printf("%1x%1x", nibble(val, x+1), nibble(val, x-1)); 
  }
}

/* -------------------------------------------------
** Routine to print data in bits
** ------------------------------------------------- */
void print_bits(byte val)
{
  extern int printf(char *, ...);
  int x;
  for(x=bitsize(byte) - 1; x>=0; x--) { 
    printf("%c", bit(val, x) ? 'x' : 'o');
  }
}

/* -------------------------------------------------
** Routine to print a bit string
** ------------------------------------------------- */
void print_bit_string(byte *bits, int offset, int len)
{
  extern int printf(char *, ...);
  int x;

  bits  += (offset / bitsize(byte));
  offset = (offset % bitsize(byte));

  if(offset) {
    for(x=bitsize(byte) - offset; x && len; x--, len--) {
      printf("%c", bit(*bits, x-1) ? 'x' : 'o');
    }
    bits++;
  }

  if(len == 0) return;

  //  printf(":");

  while(len >= bitsize(byte)) {
    print_bits(*bits++);
    //    print_nibbles(*bits++);
    // printf("%8x ", *bits++);
    len -= bitsize(byte);
  }

  for(x=bitsize(byte) - 1; len; len--, x--) {
    printf("%c", bit(*bits, x) ? 'x' : 'o');
  }
}

/* ------------------------------------------------
** Given two bit strings, compares and returns
** whether or not they match 
** ------------------------------------------------ */
int compare_bit_strings(
   byte *bits1, 
   byte *bits2, 
   int offset, 
   int len
)
{
  extern int printf(char *, ...);
  int x, cnt;
  int rval = 0;

  bits1 += (offset / bitsize(byte));
  bits2 += (offset / bitsize(byte));
  offset = (offset % bitsize(byte));


  if(offset) {
    for(x=bitsize(byte) - offset, cnt=0; x && len; x--, len--) {
      if(bit(*bits1, x-1) != bit(*bits2, x-1)) {
        if(cnt == 0) printf("Head:");
        cnt ++;
        rval = -1;
      }    
    }

    bits1++;
    bits2++;
  }

  if(len == 0) return rval;

  for(cnt=0; len >= bitsize(byte); len-=bitsize(byte)) {
    if(*bits1++ != *bits2++) {
      if(cnt == 0) printf("Body:");
      cnt++;
      rval= -1;
    }
  }

  if(len == 0) return rval;

  for(x=bitsize(byte), cnt=0; len; x--, len--) {
    if(bit(*bits1, x-1) != bit(*bits2, x-1)) {
      if(cnt == 0) printf("Tail:");
      cnt ++;
      rval = -1;
    }
  }

  for(x=bitsize(byte); len; len--, x--) {
    if(bit(*bits1, x-1) != bit(*bits2, x-1)) {
      if(cnt == 0) printf("Tail:");
      cnt ++;
      rval = -1;
    }
  }
  return rval;
}

/* ------------------------------------------------
** Packing Tests
** ------------------------------------------------ */
byte data1[512];
byte data2[512];
byte data3[512];

void
pack_em(int offset, int size)
{
  extern int printf(char *, ...);
  int x;
  // shift all 1's on a base of 0's 
  for(x=0; x<sizeof(data1) / sizeof(byte); x++) data1[x]=-1;
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=0;
  for(x=0; x<sizeof(data3) / sizeof(byte); x++) data3[x]=-1;

  b_packx(&data1[0], &data2[0], offset, size);
  b_unpackx(&data2[0], &data1[0], offset, size);
  if((compare_bit_strings(&data1[0], &data3[0], 0, size+offset+10) != 0)) {
    printf("-- pack_em : 1's over 0's (%d, %d)", offset, size);

    printf("\n\t");  print_bit_string(&data1[0], 0, size+offset+10); 
    printf("\n\t");  print_bit_string(&data2[0], 0, size+offset+10); 
    printf("\n\t");  print_bit_string(&data3[0], 0, size+offset+10); 
    printf("\n");
  } else {
    printf("++ pack_em : 1's over 0's (%d, %d)\n", offset, size);
  }
}


/* ------------------------------------------------
** Shifting copy tests
** ------------------------------------------------ */
void
shift_em(int src_offset, int dst_offset, word size)
{
  extern int printf(char *, ...);
  int x;

  if(size > bitsize(data1)) return;

  // shift all 1's on a base of 0's 
  for(x=0; x<sizeof(data1) / sizeof(byte); x++) data1[x]=-1;
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=0;
  for(x=0; x<sizeof(data3) / sizeof(byte); x++) data2[x]=0;

  b_copy(&data1[0], src_offset, &data2[0], dst_offset, size);
  b_copy(&data2[0], dst_offset, &data3[0], src_offset, size);
  if((compare_bit_strings(&data1[0], &data3[0], src_offset, size) != 0)) {
    printf("-- shift_em : 1's over 0's (%d, %d, %d)", src_offset, dst_offset, size);

    printf("\n\t");  print_bit_string(&data1[0], src_offset, size); 
    printf("\n\t");  print_bit_string(&data2[0], dst_offset, size); 
    printf("\n\t");  print_bit_string(&data3[0], src_offset, size); 
    printf("\n");
  } else {
    printf("++ shift_em : 1's over 0's (%d, %d)\n", src_offset, dst_offset);
  }

  // in place incrementing on a base of 0's 
  for(x=0; x<sizeof(data1)*2;x++) ((byte *) data1)[x/2]= ((x+1) & 0x0f) + ((x & 0x0f) << 4);
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=0;
  for(x=0; x<sizeof(data3)*2;x++) ((byte *) data3)[x/2]= ((x+1) & 0x0f) + ((x & 0x0f) << 4);

  b_copy(&data1[0], src_offset, &data2[0], dst_offset, size);
  b_copy(&data2[0], dst_offset, &data1[0], src_offset, size);
  if((compare_bit_strings(&data1[0], &data3[0], 0, bitsize(data1)) != 0)) {
    printf("-- shift_em : in place (%d, %d, %d)", src_offset, dst_offset,  size);

    printf("\n\t");  print_bit_string(&data1[0], 0, size+20); 
    // printf("\n\t");  print_bit_string(&data2[0], dst_offset, size+20); 
    printf("\n\t");  print_bit_string(&data3[0], 0, size+20); 
    printf("\n");
  } else {
    //    printf("++ shift_em : in place (%d, %d)\n", src_offset, dst_offset);
  }

  // in place shift incrementing on a base of 1's 
  for(x=0; x<sizeof(data1)*2;x++) ((byte *) data1)[x/2]= ((x+1) & 0x0f) + ((x & 0x0f) << 4);
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=-1;
  for(x=0; x<sizeof(data3)*2;x++) ((byte *) data3)[x/2]= ((x+1) & 0x0f) + ((x & 0x0f) << 4);

  b_copy(&data1[0], src_offset, &data2[0], dst_offset, size);
  b_copy(&data2[0], dst_offset, &data1[0], src_offset, size);
  if((compare_bit_strings(&data1[0], &data3[0], 0, bitsize(data1)) != 0)) {
    printf("-- shift_em : in place (%d, %d, %d)", src_offset, dst_offset,  size);

    printf("\n\t");  print_bit_string(&data1[0], 0, size+20); 
    //    printf("\n\t");  print_bit_string(&data2[0], 0, size+20); 
    printf("\n\t");  print_bit_string(&data3[0], 0, size+20); 
    printf("\n");
  } else {
    //    printf("++ shift_em : in place (%d, %d)\n", src_offset, dst_offset);
  }



  // shift all 0's on a base of 1's

  for(x=0; x<sizeof(data1) / sizeof(byte); x++) data1[x]=0;
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=-1;
  for(x=0; x<sizeof(data3) / sizeof(byte); x++) data3[x]=0;

  b_copy(&data1[0], src_offset, &data2[0], dst_offset, size);
  b_copy(&data2[0], dst_offset, &data3[0], src_offset, size);
  if((compare_bit_strings(&data1[0], &data3[0], src_offset, size) != 0)) {
    printf("-- shift_em : 0's over 1's (%d, %d, %d)", src_offset, dst_offset, size);

    printf("\n\t");  print_bit_string(&data1[0], src_offset, size); 
    printf("\n\t");  print_bit_string(&data2[0], dst_offset, size); 
    printf("\n\t");  print_bit_string(&data3[0], src_offset, size); 
    printf("\n");
  } else {
    //    printf("++ shift_em : 0's over 1's (%d, %d)\n", src_offset, dst_offset);
  }

  // shift an incrementing sequence on a base of 0's

  for(x=0; x<sizeof(data1)*2;x++)
    ((byte *) data1)[x/2]= ((x+1) & 0x0f) + ((x & 0x0f) << 4);
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=-1;
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=0;

  b_copy(&data1[0], src_offset, &data2[0], dst_offset, size);
  b_copy(&data2[0], dst_offset, &data3[0], src_offset, size);
  if((compare_bit_strings(&data1[0], &data3[0], src_offset, size) != 0)) {
    printf("-- shift_em : incrementing over 1's (%d, %d, %d)", src_offset, dst_offset, size);

    printf("\n\t");  print_bit_string(&data1[0], src_offset, size); 
    printf("\n\t");  print_bit_string(&data2[0], dst_offset, size); 
    printf("\n\t");  print_bit_string(&data3[0], src_offset, size); 
    printf("\n");
  } else {
    //    printf("++ shift_em : incrementing over 1's (%d, %d)\n", src_offset, dst_offset);

  }


  // shift an incrementing sequence on a base of 0's

  for(x=0; x<sizeof(data1)*2;x++)
    ((byte *) data1)[x/2]= ((x+1) & 0x0f) + ((x & 0x0f) << 4);
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=0;
  for(x=0; x<sizeof(data2) / sizeof(byte); x++) data2[x]=0;

  b_copy(&data1[0], src_offset, &data2[0], dst_offset, size);
  b_copy(&data2[0], dst_offset, &data3[0], src_offset, size);
  if((compare_bit_strings(&data1[0], &data3[0], src_offset, size) != 0)) {
    printf("-- shift_em : incrementing over 0's (%d, %d, %d)", src_offset, dst_offset, size);

    printf("\n\t");  print_bit_string(&data1[0], src_offset, size); 
    printf("\n\t");  print_bit_string(&data2[0], dst_offset, size); 
    printf("\n\t");  print_bit_string(&data3[0], src_offset, size); 
    printf("\n");
  } else {
    //    printf("++ shift_em : incrementing over 0's (%d, %d)\n", src_offset, dst_offset);

  }
}

/* ---------------------------------------------------
** Test entry point
** --------------------------------------------------- */
void main(void)
{
  extern int printf(char *, ...);
  int x, y, z;

  shift_em(7,7, 2);
  shift_em(5,2, 5);
  shift_em(1,0, 72);
  shift_em(0,1, 72);
  shift_em(2,5, 5);

  pack_em(1,20);
  pack_em(0,48);

  for(x=0; x<16; x++) {
    for(z=72; z>=0 ; z-=13)
      pack_em(x, z);
  }


  for(x=0; x<bitsize(byte)*2; x+=3) {
    for(y=0; y<bitsize(byte)*2; y+=3) {
      for(z=72; z>=0 ; z-=13)
        shift_em(x, y, z);
    }
  }

  printf("\n");
}

#endif /* END BIT Unit Test */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              D S M  S P E C I F I C  B I T  M A N I P U L A T I O N 
                             S E R V I C E S

GENERAL DESCRIPTION
  This file contains bit manipulation routines specific to DSM. It has
  unpacking routines for extracting fields of a received message in contained
  in dsm items.

EXTERNALIZED FUNCTIONS
  dsmbit_unpack8  - Given a dsm item and an offset, unpacks the requested 
                    number of bits into a uint8
    
  dsmbit_unpack16 - Given a dsm item and an offset, unpacks the requested 
                    number of bits into a uin16

  dsmbit_unpack32 - Given a dsm item and an offset, unpacks the requested 
                    number of bits into a uint32

  dsmbit_unpack64 - Given a dsm item and an offset, unpacks the requested 
                    number of bits into a qword

  dsmbit_pack8_tail  - Packs a given uint8 into a dsm item, allocating new
                       dsm items as necessary

  dsmbit_pack16_tail - Packs a given uint16 into a dsm item, allocating new
                       dsm items as necessary

  dsmbit_pack32_tail - Packs a given uint32 into a dsm item, allocating new
                       dsm items as necessary

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/dsmbit.c_v   1.1   21 Sep 2001 19:15:16   omichael  $
$Header: //components/rel/core.qdsp6/2.1.c4/memory/dsm/src/dsmbit.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/17/09   ag      Fixed lint warnings.
04/27/09   ag      Replaced assert.h with amssassert.h
01/01/05   pjb     added #define for default pool.
09/21/01   om      Added dsm_bit_copy()
12/04/00   nmn     Updates for final code review
11/09/00   nmn     Rewrote pack functions.  They take single * and accept
                   dups and links.  Changed names to pack*_tail.  Returns
                   number of bits packed.
10/25/00   nmn     Added more comments for code review
10/19/00   nmn     Fixed pack function problems.  Pack now takes 
                   dsm_item_type  ** for when pullup & pushdown is used.
10/12/00   nmn     Changed some pack comparisons
07/27/00   nmn     Added pack functions
05/15/00   vas     Created module.

===========================================================================*/

/* <EJECT> */


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "amssassert.h"
#ifdef FEATURE_DSM_NATIVE_ASSERT
#define DSM_ASSERT( cond )      assert( cond )
#endif
#include "dsmbit.h"
#include "bit.h"
#include "err.h"
#include "dsmi.h"


/*============================================================================

                        DATA DECLARATIONS

=============================================================================*/
#define MAX_8_BIT_BUF  2 /* Max # of bufs req to hold offset 8 bit field  */
#define MAX_16_BIT_BUF 3 /* Max # of bufs req to hold offset 16 bit field */
#define MAX_32_BIT_BUF 5 /* Max # of bufs req to hold offset 32 bit field */
#define MAX_64_BIT_BUF 9 /* Max # of bufs req to hold offset 64 bit field */

/* Masks used to copy items bit by bit */
uint8 dsmbit_bit_copy_mask[8] = { 0x80, 0x40, 0x20, 0x10, 
                                  0x08, 0x04, 0x02, 0x01 };

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSMBIT_UNPACK8

DESCRIPTION
  Given a dsm item and an offset, unpacks the requested number of bits into
  a uint8. Since data can span multiple dsm items, we first need to extract
  the desired data bytes using the dsm interface & then unpack these bytes
  for the desired field.
  
DEPENDENCIES
  Calling routine should allocate the destination buffer memory (pointed to
  by ret_val_ptr) before calling this function.

PARAMETERS
  item_ptr    - Pointer to dsm item containing message
  ret_val_ptr - Pointer to the destination buffer into which the unpacked 
                value gets copied.
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  Number of bits extracted. Will be 0 if operation fails.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 dsmbit_unpack8 
( 
  dsm_item_type *item_ptr, 
  uint8  *ret_val_ptr,
  uint16  offset, 
  uint16  len 
)
{
  uint16 byte_offset;          /* Position in terms of bytes of reqd field 
                                  from which to start extracting */
  uint16 num_bytes;            /* Number of bytes to extract */
  uint8  buffer[MAX_8_BIT_BUF];/* Buffer to hold the extracted field. Field
                                  might be split across 2 bytes */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
  Clear buffer
  -------------------------------------------------------------------------*/
  buffer[0] = 0x00;
  buffer[1] = 0x00;

  /*-------------------------------------------------------------------------
  Determine position in terms of bytes & convert it to the next lower integer
  -------------------------------------------------------------------------*/
  byte_offset = offset/8;

  /*-------------------------------------------------------------------------
  Determine the actual number of bytes to extract. If (offset + len ) is not
  an integer number of bytes, then we need to extract the additional byte.
  -------------------------------------------------------------------------*/
  if( (( offset + len ) % 8) == 0 )
  {
    num_bytes = ((offset + len ) / 8 ) - byte_offset ;
  }
  else
  {
    num_bytes = ((offset + len ) / 8 ) - byte_offset + 1;
  }

  /*-------------------------------------------------------------------------
  Extract the field from dsm item. Return an error if no bytes are extracted.
  -------------------------------------------------------------------------*/
  if( dsm_extract( item_ptr, byte_offset, buffer, num_bytes ) < num_bytes )
  {
    return 0;
  }

  /*-------------------------------------------------------------------------
  Determine the position of the required field within the extracted bytes
  -------------------------------------------------------------------------*/
  offset = offset % 8;

  /*-------------------------------------------------------------------------
  Extract the actual field from the extracted bytes 
  -------------------------------------------------------------------------*/
  *ret_val_ptr = ((uint8) b_unpackb( buffer , offset , len ));

  return len;
}

/* <EJECT> */

/*===========================================================================
FUNCTION DSMBIT_UNPACK16

DESCRIPTION
  Given a dsm item and an offset, unpacks the requested number of bits into
  a uint16. Since data can span multiple dsm items, we first need to extract
  the desired data bytes using the dsm interface & then unpack these bytes
  for the desired field.
 
DEPENDENCIES
  Calling routine should allocate the destination buffer memory (pointed to
  by ret_val_ptr) before calling this function.

PARAMETERS
  item_ptr    - Pointer to dsm item containing message
  ret_val_ptr - Pointer to the destination buffer into which the unpacked 
                value gets copied.
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  Number of bits extracted. Will be 0 if operation fails.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 dsmbit_unpack16 
( 
  dsm_item_type *item_ptr, 
  uint16 *ret_val_ptr,
  uint16  offset, 
  uint16  len 
)
{
  uint16 byte_offset;           /* Position in terms of bytes of reqd field */
  uint16 num_bytes;             /* Number of bytes to extract */
  uint8  buffer[MAX_16_BIT_BUF];/* Buffer to hold the extracted field. Field 
                                   might be split across 3 bytes */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
  Clear buffer
  -------------------------------------------------------------------------*/
  memset((void *)buffer, 0, sizeof(buffer));

  /*-------------------------------------------------------------------------
  Determine position in terms of bytes & convert it to the next lower integer
  -------------------------------------------------------------------------*/
  byte_offset = (uint16) offset/8;

  /*-------------------------------------------------------------------------
  Determine the actual number of bytes to extract. If (offset + len ) is not
  an integer number of bytes, then we need to extract the additional byte.
  -------------------------------------------------------------------------*/
  if( (( offset + len ) % 8) == 0 )
  {
    num_bytes = ((offset + len ) / 8 ) - byte_offset ;
  }
  else
  {
    num_bytes = ((offset + len ) / 8 ) - byte_offset + 1;
  }

  /*-------------------------------------------------------------------------
  Extract the field from dsm item. Return an error if no bytes are extracted.
  -------------------------------------------------------------------------*/
  if( dsm_extract( item_ptr, byte_offset, buffer, num_bytes ) < num_bytes )
  {
    return 0;
  }

  /*-------------------------------------------------------------------------
  Determine the position of the required field within the extracted bytes.
  This will be a modulo 8 number.
  -------------------------------------------------------------------------*/
  offset = offset % 8;

  /*-------------------------------------------------------------------------
  Extract the actual field from the extracted bytes 
  -------------------------------------------------------------------------*/
  *ret_val_ptr = ((uint16) b_unpackw( buffer , offset , len ));

  return len;
}

/* <EJECT> */

/*===========================================================================
FUNCTION DSMBIT_UNPACK32

DESCRIPTION
  Given a dsm item and an offset, unpacks the requested number of bits into
  a uint32. Since data can span multiple dsm items, we first need to extract
  the desired data bytes using the dsm interface & then unpack these bytes
  for the desired field.

  
DEPENDENCIES
  Calling routine should allocate the destination buffer memory (pointed to
  by ret_val_ptr) before calling this function.

PARAMETERS
  item_ptr    - Pointer to dsm item containing message
  ret_val_ptr - Pointer to the destination buffer into which the unpacked 
                value gets copied.
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  Number of bits extracted. Will be 0 if operation fails.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 dsmbit_unpack32 
(
  dsm_item_type *item_ptr,
  uint32 *ret_val_ptr,
  uint16  offset,
  uint16  len 
)
{
  uint16 byte_offset;           /* Position in terms of bytes of reqd field */
  uint16 num_bytes;             /* Number of bytes to extract */
  uint8  buffer[MAX_32_BIT_BUF];/* Buffer to hold the extracted field. Field 
                                   might be split across 5 bytes */


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
  Clear buffer
  -------------------------------------------------------------------------*/
  memset((void *)buffer, 0, sizeof(buffer));

  /*-------------------------------------------------------------------------
  Determine position in terms of bytes & convert it to the next lower integer
  -------------------------------------------------------------------------*/
  byte_offset = (uint16) offset/8;

  /*-------------------------------------------------------------------------
  Determine the actual number of bytes to extract. If (offset + len ) is not
  an integer number of bytes, then we need to extract the additional byte.
  -------------------------------------------------------------------------*/
  if( (( offset + len ) % 8) == 0 )
  {
    num_bytes = ((offset + len ) / 8 ) - byte_offset ;
  }
  else
  {
    num_bytes = ((offset + len ) / 8 ) - byte_offset + 1;
  }

  /*-------------------------------------------------------------------------
  Extract the field from dsm item. Return an error if no bytes are extracted.
  -------------------------------------------------------------------------*/
  if( dsm_extract( item_ptr, byte_offset, buffer, num_bytes ) < num_bytes )
  {
    return 0;
  }

  /*-------------------------------------------------------------------------
  Determine the position of the required field within the extracted bytes.
  This will be a modulo 8 number.
  -------------------------------------------------------------------------*/
  offset = offset % 8 ;

  /*-------------------------------------------------------------------------
  Extract the actual field from the extracted bytes 
  -------------------------------------------------------------------------*/
  *ret_val_ptr = ((uint32) b_unpackd( buffer , offset , len ));

  return len;
}

/* <EJECT> */

/*===========================================================================
FUNCTION DSMBIT_UNPACK64

DESCRIPTION
  Given a dsm item and an offset, unpacks the requested number of bits into
  a qword. Since data can span multiple dsm items, we first need to extract
  the desired data bytes using the dsm interface & then unpack these bytes
  for the desired field.
  
DEPENDENCIES
  Calling routine should allocate the destination buffer memory (pointed to
  by ret_val_ptr) before calling this function.

PARAMETERS
  item_ptr    - Pointer to dsm item containing message
  ret_val     - Pointer to the destination buffer into which the unpacked 
                value gets copied. ( Note: qword is a pointer )
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  Number of bits extracted. Will be 0 if operation fails.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 dsmbit_unpack64
( 
  dsm_item_type *item_ptr, 
  qword   ret_val,
  uint16  offset, 
  uint16  len 
)
{
  uint16 byte_offset;           /* Position in terms of bytes of reqd field */
  uint16 num_bytes;             /* Number of bytes to extract */
  uint8  buffer[MAX_64_BIT_BUF];/* Buffer to hold the extracted field. Field
                                   might be split across 2 bytes */


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
  Clear buffer
  -------------------------------------------------------------------------*/
  memset((void *)buffer, 0, sizeof(buffer));

  /*-------------------------------------------------------------------------
  Determine position in terms of bytes & convert it to the next lower integer
  -------------------------------------------------------------------------*/
  byte_offset = (uint16) offset/8;

  /*-------------------------------------------------------------------------
  Determine the actual number of bytes to extract. If (offset + len ) is not
  an integer number of bytes, then we need to extract the additional byte.
  -------------------------------------------------------------------------*/
  if( (( offset + len ) % 8) == 0 )
  {
    num_bytes = ((offset + len ) / 8 ) - byte_offset ;
  }
  else
  {
    num_bytes = ((offset + len ) / 8 ) - byte_offset + 1;
  }

  /*-------------------------------------------------------------------------
  Extract the field from dsm item. Return an error if no bytes are extracted.
  -------------------------------------------------------------------------*/
  if( dsm_extract( item_ptr, byte_offset, buffer, num_bytes ) < num_bytes )
  {
    return 0;
  }

  /*-------------------------------------------------------------------------
  Determine the position of the required field within the extracted bytes
  -------------------------------------------------------------------------*/
  offset = offset % 8;

  /*-------------------------------------------------------------------------
  Extract the actual field from the extracted bytes 
  -------------------------------------------------------------------------*/
  b_unpackq( buffer , offset , len , ret_val);

  return len;
}

/* <EJECT> */

/*===========================================================================
FUNCTION DSMBIT_PACK8_TAIL

DESCRIPTION
  Packs a given value (up to eight bits) into a given dsm item at a
  specified offset.

DEPENDENCIES
  Data must be packed into the last used byte or the byte immediately 
  following it.

  item_ptr can not be NULL.
  
PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack

RETURN VALUE
  Number of bits successfully packed

SIDE_EFFECTS
  A new dsm item may be acquired and linked.

===========================================================================*/
uint16 dsmbiti_pack8_tail
(
  dsm_item_type * item_ptr,
  uint8 pack_data,
  uint16 offset,
  uint16 len,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
)
{
  uint16 byte_offset;           /* Position in terms of bytes of reqd field */

  uint16 num_pushed_bits = 0;   /* stores number of bits pushed */

  uint16 length_packet;         /* stores the length of the passed packet */

  uint8 bit_pack_data;          /* data to bit_pack into last byte */


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  byte_offset = (uint16) offset / 8;
  offset = offset % 8;

  /* have to be writing bits into the last used byte, or to the byte
     immediately following.  
     dsm_length_packet starts counting at 1, byte_offset starts counting at
     0, thus the apparent off-by-1 discrepancy. */
    
  DSM_ASSERT(NULL != item_ptr);

  length_packet = (uint16)dsm_length_packet(item_ptr);

  if (offset == 0)
  {
    DSM_ASSERT( length_packet == byte_offset );
  }
  else //offset != 0
  {
    DSM_ASSERT( length_packet == (byte_offset + 1) );
  }

  /* if we are packing some bits into the last used byte, go to it directly
     and pack those bits in */
  if (offset != 0)
  {
    while (item_ptr->pkt_ptr != NULL)
    {
      byte_offset -= item_ptr->used;
      item_ptr = item_ptr->pkt_ptr;
    }

    /* move the bits to be packed into the LSB of the temporary variable */
    bit_pack_data = pack_data >> MAX((len + offset - 8),0);

    /* pack the bits into the last used byte */
    b_packb (bit_pack_data, (item_ptr->data_ptr + byte_offset), 
             offset, (word) MIN(8 - offset, len));

    /* count the number of pushed bits */
    num_pushed_bits += MIN(8 - offset, len);

    /* decrease the remaining length by the number of bits pushed */
    len = MAX((len + offset - 8), 0);
  }

  /* len is number of bits to pushdown_tail. it may have changed from top 
     of the function, as a result of packing some bits directly. */
  if (len != 0)
  {
    /* shift out the bits that have already been packed. */
    pack_data = pack_data << (8 - len);

    /* if the pushdown was successful */
    if (dsmi_pushdown_tail_long(&item_ptr, &pack_data, 1, pool_id, file, line) == 1)
    {
      /* assume all bits were pushed and increment the counter */
      num_pushed_bits += len;
    }
  }
  return num_pushed_bits;
} /* dsmbit_pack8_tail() */

/* <EJECT> */

/*===========================================================================
FUNCTION DSMBIT_PACK16_TAIL

DESCRIPTION
  Packs a given value (up to sixteen bits) into a given dsm item at a
  specified offset.  

DEPENDENCIES
  Data must be packed into the last used byte or the byte immediately
  following it.

  item_ptr can not be NULL.
  
PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  Number of bits successfully packed

SIDE_EFFECTS
  A new dsm item may be acquired and linked.
===========================================================================*/
uint16 dsmbiti_pack16_tail
(
  dsm_item_type * item_ptr,
  uint16 pack_data,
  uint16 offset,
  uint16 len,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line

)
{
  uint16 byte_offset;            /* Position in terms of bytes of reqd field */

  uint16 bit_pack_data;          /* data to bit_pack into last byte */

  uint16 num_pushed_bits = 0;    /* stores number of bits pushed */

  uint16 length_packet;          /* stores the length of the passed packet */

  uint16 pushdown_bytes;          /* number of bytes to push down */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  byte_offset = (uint16) offset / 8;
  offset = offset % 8;

  /* have to be writing bits into the last used byte, or to the byte
     immediately following.
     dsm_length_packet starts counting at 1, byte_offset starts counting at 0,
     thus the apparent off-by-1 discrepancy. */

  DSM_ASSERT (NULL != item_ptr);

  length_packet = (uint16)dsm_length_packet(item_ptr);

  if (offset == 0)
  {
    DSM_ASSERT( length_packet == byte_offset);
  }
  else //offset != 0
  {
    DSM_ASSERT( length_packet == (byte_offset + 1));
  }

  /* if we are packing some bits into the last used byte, go to it directly
     and pack those bits in */
  if (offset != 0)
  {
    while (item_ptr->pkt_ptr != NULL)
    {
      byte_offset -= item_ptr->used;
      item_ptr = item_ptr->pkt_ptr;
    }

    /* move the bits to be packed into the LSB of the temporary variable */
    bit_pack_data = pack_data >> MAX((len + offset - 8), 0);

    /* pack the bits into the last used byte */
    b_packw (bit_pack_data, (item_ptr->data_ptr + byte_offset),
             offset, (word) MIN(8 - offset, len));

    /* count the number of pushed bits */
    num_pushed_bits += MIN(8 - offset, len);

    /* decrease the remaining length by the number of bits pushed */
    len = MAX((len + offset - 8), 0);
  }

  /* len is number of bits to pushdown_tail.  It may have changed from
     the top of the function, as a result of packing some bits directly. */
  if (len != 0)
  {
    /* shift out the bits that have already been packed. */
    pack_data = pack_data << (16 - len);

    /* swap big-endian to little-endian in 16-bit and 32-bit values
       before pushing down */
    pack_data = NTOHS(pack_data);

    pushdown_bytes = 1 + ((len - 1) / 8);

    /* if the pushdown was successful */
    if (dsmi_pushdown_tail(&item_ptr, &pack_data, pushdown_bytes, pool_id, file,line ) ==
        pushdown_bytes)
    {
      /* assume all bits were pushed and increment the counter */
      num_pushed_bits += len;
    }
  }

  return num_pushed_bits;
} /* dsmbit_pack16_tail() */

/* <EJECT> */

/*===========================================================================
FUNCTION DSMBIT_PACK32_TAIL

DESCRIPTION
  Packs a given value (up to thirty-two bits) into a given dsm item at a
  specified offset.  

DEPENDENCIES
  Data must be packed into the last used byte or the byte immediately
  following it.

  item_ptr can not be NULL.
  
PARAMETERS
  item_ptr  - Pointer to dsm item where packed data should be placed.
  pack_data - Data to be packed
  offset    - Number of bits to skip before placing this data
  len       - Number of bits of pack_data to pack (LSB of the variable)

RETURN VALUE
  Number of bits successfully packed

SIDE_EFFECTS
  A new dsm item may be acquired and linked.

===========================================================================*/
uint16 dsmbiti_pack32_tail
(
  dsm_item_type * item_ptr,
  uint32 pack_data,
  uint16 offset,
  uint16 len,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
)
{
  uint32 bit_pack_data;          /* data to bit_pack into last byte */

  uint16 byte_offset;            /* Position in terms of bytes of reqd field */

  uint16 num_pushed_bits = 0;    /* stores number of bits pushed */

  uint16 length_packet;          /* stores the length of the passed packet */

  uint16 pushdown_bytes;          /* number of bytes to push down */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  byte_offset = (uint16) offset / 8;
  offset = offset % 8;

  /* have to be writing bits into the last used byte, or to the byte
     immediately following.
     dsm_length_packet starts counting at 1, byte_offset starts counting at 0,
     thus the apparent off-by-1 discrepancy. */

  DSM_ASSERT (NULL != item_ptr);

  length_packet = (uint16)dsm_length_packet(item_ptr);

  if (offset == 0)
  {
    DSM_ASSERT( length_packet == byte_offset);
  }
  else //offset != 0
  {
    DSM_ASSERT (length_packet == (byte_offset + 1));
  }

  /* if we are packing some bits into the last used byte, go to it directly
     and pack those bits in */
  if (offset != 0)
  {
    while (item_ptr->pkt_ptr != NULL)
    {
      byte_offset -= item_ptr->used;
      item_ptr = item_ptr->pkt_ptr;
    }

    /* move the bits to be packed into the LSB of the temporary variable */
    bit_pack_data = pack_data >> MAX((len + offset - 8), 0);

    /* pack the bits into the last used byte */
    b_packd (bit_pack_data, (item_ptr->data_ptr + byte_offset),
             offset, (word) MIN(8-offset, len));

    /* count the number of pushed bits */
    num_pushed_bits += MIN(8 - offset, len);

    /* decrease the remaining length by the number of bits pushed */
    len = MAX((len + offset - 8), 0);
  }

  /* len is number of bits to pushdown_tail.  It may have changed from
     the top of the function, as a result of packing some bits directly */
  if (len != 0)
  {
    /* shift out the bits that have already been packed */
    pack_data = pack_data << (32 - len);

    /* swap big-endian to little-endian in 16-bit and 32-bit values
       before pushing down */
    pack_data = NTOHL(pack_data);

    pushdown_bytes = 1 + ((len - 1) / 8);

    /* if the pushdown was successful */
    if (dsmi_pushdown_tail(&item_ptr, &pack_data, pushdown_bytes, pool_id, file, line) ==
        pushdown_bytes)
    {
      /* assume all bits were pushed and increment the counter */
      num_pushed_bits += len;
    }
  }

  return num_pushed_bits;
} /* dsmbit_pack32_tail() */

/* <EJECT> */

/*===========================================================================
FUNCTION DSMBIT_BIT_COPY

DESCRIPTION
  Copies bitwise from one item into another.

DEPENDENCIES
  Only copies from one item into one item are supported.

PARAMETERS
  src_ptr     - DSM item to copy from
  dst_ptr     - DSM item to copy into
  src_offset  - Bit offset to start copying from
  dst_offset  - Bit offset to start copying into
  len         - Number of bits to copy

RETURN VALUE
  Number of bits copied

SIDE_EFFECTS

===========================================================================*/
uint16 dsmbiti_bit_copy
(
  dsm_item_type*  src_ptr,
  dsm_item_type*  dst_ptr,
  uint16          src_offset,
  uint16          dst_offset,
  uint16          len,
  const char * file,
  uint32 line
)
{
  int i;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check on the item pointers */
  if ( (src_ptr == NULL) || (dst_ptr == NULL) )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"NULL dsm item pointer");
    return 0;
  }

  /* Sanity chack that we copy within the items */
  if ( ((((src_offset + len) / 8) + 1) > src_ptr->used)
       ||
       ((((dst_offset + len) / 8) + 1) > DSMI_SIZE(dst_ptr) ) ) 
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Copy length %d exceeds item size", len);
    return 0;
  }

  /* Copy each bit */
  for ( i=0; i<len; i++ )
  {
    /* Null out destination bit */
    *(dst_ptr->data_ptr + ((dst_offset + i) / 8)) &= 
      ~dsmbit_bit_copy_mask[(dst_offset + i) % 8];

    /* Or source bit into the very spot */
    *(dst_ptr->data_ptr + ((dst_offset + i) / 8)) |= 
      ((*(src_ptr->data_ptr + ((src_offset + i) / 8))
        & dsmbit_bit_copy_mask[(src_offset + i) % 8]) 
       >> (7 - ((src_offset + i) % 8)))
      << (7 - ((dst_offset + i) % 8));
  }

  /* Update the destination item's used field */
  dst_ptr->used = ((dst_offset + len) / 8) + (((dst_offset + len) % 8) * 1);

#ifdef FEATURE_DSM_MEM_CHK
  dsmi_touch_item(dst_ptr,file,line);
#endif

  return len;
}

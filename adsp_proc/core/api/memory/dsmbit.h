#ifndef DSMBIT_H
#define DSMBIT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              D S M  S P E C I F I C  B I T  M A N I P U L A T I O N 
                             S E R V I C E S

GENERAL DESCRIPTION
  This file contains declarations for bit manipulation routines specific to 
  DSM. It has unpacking routines for extracting fields of a received message 
  in contained in dsm items.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/dsmbit.h_v   1.1   21 Sep 2001 19:15:22   omichael  $
$Header: //components/rel/core.qdsp6/2.1/api/memory/dsmbit.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/06   mjb     Decoupled DSM pools.
09/21/01   om      Added dsm_bit_copy()
12/04/00   nmn     Updates for final code review
11/09/00   nmn     Changed interface to pack functions.
10/19/00   nmn     Pack now takes dsm_item_type ** for pullup/pushdown
07/27/00   nmn     added pack functions
05/15/00   vas     Created module.

===========================================================================*/

#include "comdef.h"
#include "dsm_item.h"
#include "qw.h"

/* needed for decoupling the small pool */
#include "dsm_init.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*----------------------------------------------------------------------------
  Define which dsm pool the legacy functions allocate new dsm items from. 
----------------------------------------------------------------------------*/
#define DSMBIT_POOL DSM_DS_SMALL_ITEM_POOL

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */ 

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
extern uint16 dsmbit_unpack8 
( 
  dsm_item_type *item_ptr, 
  uint8  *ret_val_ptr,
  uint16  offset, 
  uint16  len 
);

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
extern uint16 dsmbit_unpack16
(
  dsm_item_type *item_ptr,
  uint16 *ret_val_ptr,
  uint16  offset, 
  uint16  len 
);

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
extern uint16 dsmbit_unpack32 
(
  dsm_item_type *item_ptr,
  uint32  *ret_val_ptr,
  uint16 offset, 
  uint16 len 
);

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
                value gets copied.
  offset      - Offset (in term of bits from the start of dsm data) of the 
                first bit to start extraction from.
  len         - Number of bits to extract

RETURN VALUE
  Number of bits extracted. Will be 0 if operation fails.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 dsmbit_unpack64
( 
  dsm_item_type *item_ptr, 
  qword   ret_val,
  uint16  offset, 
  uint16  len 
);

/* <EJECT> */ 

/*===========================================================================
FUNCTION DSMBIT_PACK8

DESCRIPTION
  Packs a given value (up to eight bits) into a given dsm item at a
  specified offset.  If the dsm item pointer points to a NULL pointer,
  a new dsm item will be acquired.

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
extern uint16 dsmbiti_pack8_tail
(
  dsm_item_type * item_ptr,
  uint8 pack_data,
  uint16 offset,
  uint16 len,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);
#define dsmbit_pack8_tail(item_ptr,pack_data,offset,len) \
  dsmbiti_pack8_tail(item_ptr,pack_data,offset,len,DSMBIT_POOL,\
                     __FILE__,__LINE__)

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
extern uint16 dsmbiti_pack16_tail
(
  dsm_item_type * item_ptr,
  uint16 pack_data,
  uint16 offset,
  uint16 len,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);
#define dsmbit_pack16_tail(item_ptr,pack_data,offset,len) \
  dsmbiti_pack16_tail(item_ptr,pack_data,offset,len,DSMBIT_POOL,\
                     __FILE__,__LINE__)


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
extern uint16 dsmbiti_pack32_tail
(
  dsm_item_type * item_ptr,
  uint32 pack_data,
  uint16 offset,
  uint16 len,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);
#define dsmbit_pack32_tail(item_ptr,pack_data,offset,len) \
  dsmbiti_pack32_tail(item_ptr,pack_data,offset,len,DSMBIT_POOL,\
                     __FILE__,__LINE__)

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
);

#define dsmbit_bit_copy(src_ptr,dst_ptr,src_offset,dst_offset,len) \
  dsmbiti_bit_copy(src_ptr,dst_ptr,src_offset,dst_offset,len,__FILE__,\
                   __LINE__)

/*===========================================================================
MACRO NTOHS
MACRO NTOHL

Macro to switch a value between network and host ordering.  Switches from 
big-endian to little-endian.

===========================================================================*/

#define NTOHS(x) \
        ((uint16)((((uint16)(x) & 0x00FF) << 8) | \
                  (((uint16)(x) & 0xFF00) >> 8)))

#define NTOHL(x) \
        ((uint32)((((uint32)(x) & 0x000000FFU) << 24) | \
                  (((uint32)(x) & 0x0000FF00U) <<  8) | \
                  (((uint32)(x) & 0x00FF0000U) >>  8) | \
                  (((uint32)(x) & 0xFF000000U) >> 24)))

#endif /* DSMBIT_H */


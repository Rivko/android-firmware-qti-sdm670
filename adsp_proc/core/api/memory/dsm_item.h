#ifndef DSM_ITEM_H
#define DSM_ITEM_H
/*===========================================================================

                                  D S M _ I T E M . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.qdsp6/2.1/api/memory/dsm_item.h#1 $

when        who    what, where, why
----------  ---    -------------------------------------------------------- 
01/13/2010  ag     Remove featurization for CMI Phase 2.
===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "dsm_pool.h"

#ifndef DSM_TRACE_LEN
#define DSM_TRACE_LEN (1)
#endif

#define DSM_SIZE
#define DSM_PRIORITY
#define DSM_APP_FIELD
#define DSM_APP_PTR
#define DSM_TRACER
#define DSM_KIND

/* Header size + pad + cookies + free-stack */
#define DSM_ITEM_HEADER_SIZE (64+4)

    

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/

typedef struct dsm_item_trace_s
{
  const char          *file;
  uint32              line;
} dsm_item_trace;

struct dsm_item_s
{
  q_link_type        link;	/* field for put/getting to/from Qs        */

  struct dsm_item_s  *pkt_ptr;	/* Pointer, if used, to next buffer item   */
                                /* in packet chain                         */
  struct dsm_item_s  *dup_ptr;	/* Pointer, if used, to any duplicated     */
                                /* buffer item                             */
  uint8              *data_ptr; /* working data pointer.                   */

  uint32             app_field;	/* user defined field                      */
  
  void               *app_ptr;  /* Another user defined field              */

  dsm_mempool_id_type pool_id;	/* The pool to which this buffer belongs   */

  uint16              size;	/* maximum size of array allocation        */

  uint16              used;	/* current length of stuff in data array   */

  uint8               references; /* Reference count (ie duplicate         */
                                  /* count)                                */
  uint8               kind;	/* protocol type or TCP command/response,  */ 

  uint8               priority;	/* priority field                          */

  uint8               tracer;

  /*-------------------------------------------------------------------------
    These fields hold the file and line number from the last time STA_TRACE 
    was invoked.
  -------------------------------------------------------------------------*/
  dsm_item_trace      trace[DSM_TRACE_LEN];

};


/*---------------------------------------------------------------------------
  Macro to TOUCH the current item..  This should ursurp tracers.
---------------------------------------------------------------------------*/
#define DSM_TOUCH_ITEM(dsm_item_ptr)                                        \
    dsmi_touch_item(dsm_item_ptr,__FILE__,__LINE__)
void dsmi_touch_item(dsm_item_type * item_ptr, const char * file, int line);


/*---------------------------------------------------------------------------
  Macro to set the whole item header back to it starting state. 
---------------------------------------------------------------------------*/
#define DSM_ITEM_RESET(bp) dsmi_item_reset(bp)
void dsmi_item_reset(dsm_item_type * item_ptr);

/*---------------------------------------------------------------------------
  This MACRO will return the memory pool ID for the DS memory pool item
  that is passed bp parameter.
---------------------------------------------------------------------------*/
#define DSM_ITEM_POOL(bp)                                                   \
  ((dsm_mempool_id_type)((bp)->pool_id))

/*---------------------------------------------------------------------------
  This MACRO will return true if the item referenced has been FREED
---------------------------------------------------------------------------*/
#define DSM_IS_ITEM_FREE(bp)                                                \
  ((bp)->references == 0)

				     

/*===========================================================================
                      FUNCTION DECLARATIONS

===========================================================================*/

/*================ MEMORY MANAGMENT FUNCTION ==============================*/

/*===========================================================================
FUNCTION DSM_NEW_BUFFER()

DESCRIPTION
  This function will return a pointer to the next free item from the free
  queue. Pool id is passed in as parameter. 

DEPENDENCIES
  None.

PARAMETERS
  pool_id - Pool from which new item should be allocated. 

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_new_buffer(pool_id) dsmi_new_buffer(pool_id,__FILE__,__LINE__)

dsm_item_type *dsmi_new_buffer
(
 dsm_mempool_id_type pool_id,
 const char * file,
 uint32 line
);


/*===========================================================================
FUNCTION DSM_OFFSET_NEW_BUFFER()

DESCRIPTION
  This function will return a pointer to the next free item from the free
  pool represented by the passed pool ID parameter.  

  In addition, the data_ptr field of the DSM item will be offset the 
  specified number of bytes into the payload.  A specified offset larger 
  than the size of the item in the pool ID will result in an error and 
  return a NULL DSM item.

DEPENDENCIES
  None.

PARAMETERS
  pool_id - Memory pool new item should be allocated from .
  offset - Space that should be reserved at the head of the item

RETURN VALUE
  A pointer to a 'dsm_item_type'.  The data_ptr will be moved by "offset"
  number of bytes.

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_offset_new_buffer(pool_id,offset) \
             dsmi_offset_new_buffer(pool_id, offset, __FILE__, __LINE__)
dsm_item_type *dsmi_offset_new_buffer
(
  dsm_mempool_id_type pool_id,
  uint16 offset,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_FREE_BUFFER()

DESCRIPTION
  This function will return the passed buffer to the proper DSM free queue.
  This includes freeing any duplicated buffer items.

  CAUTION!!!
  You probably want to call dsm_free_packet instead of this routine.
  This function probably does NOT do what you expect.
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to item to return to free queue. 

RETURN VALUE
  The next item in the packet chain.

SIDE EFFECTS
  Note that this does not clear pointer, unlike free_packet
===========================================================================*/
#define dsm_free_buffer(item_ptr) \
             dsmi_free_buffer(item_ptr, __FILE__, __LINE__)
dsm_item_type *dsmi_free_buffer
(
 dsm_item_type *item_ptr,
 const char * file,
 uint32 line
);


/*===========================================================================
FUNCTION DSM_FREE_PACKET()

DESCRIPTION
  This function will return to the proper DSM free queue
  the entire passed packet buffer chain.

  NOTE: This function can accept a NULL pointer parameter.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Address of pointer to chain of items to return.  This
                 pointer will be set to NULL on return

RETURN VALUE
  None

SIDE EFFECTS
  The packet gets freed.
===========================================================================*/
#define dsm_free_packet(item_ptr) \
             dsmi_free_packet(item_ptr, __FILE__, __LINE__)
void dsmi_free_packet
(
 dsm_item_type **pkt_head_ptr,
 const char * file,
 uint32 line
);


/*================ PACKET MANAGMENT FUNCTIONS =============================*/
/*===========================================================================
FUNCTION DSM_LENGTH_PACKET()

DESCRIPTION
  This function counts and returns the number of bytes in a packet buffer
  chain.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pinter to item for which the length should be calculated.

RETURN VALUE
  The number of bytes in the packet.  Returns 0 if bp is null.

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsm_length_packet
(
  dsm_item_type *item_ptr
);


/*===========================================================================
FUNCTION DSM_DUP_PACKET()

DESCRIPTION
  Duplicate first 'cnt' bytes of packet starting at 'offset'.  

  This is done by obtaining a new item from the DUP pool.  Payload data is 
  not copied.  Only the header information is duplicated, and the data_ptr 
  of the dup item points into the source item's data memory.  (i.e., the 
  pointers are set up to share the data segment of the original item).  

  The return pointer is passed back through the first argument, and the 
  return value is the number of bytes actually duplicated. 

DEPENDENCIES
  None

PARAMETERS
  dup_ptr - Pointer to pointer where new packet will be saved.
  src_ptr - Pointer to the packet to be duped.
  offset - Offset into src_ptr where duping should start
  cnt - Number of bytes from offset that should be duped

RETURN VALUE
  The total size in bytes of the duplicated packet.

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_dup_packet_pool(dup_ptr, pool, src_ptr, offset, cnt) \
            dsmi_dup_packet(dup_ptr, pool, src_ptr, offset, cnt,\
                            __FILE__,__LINE__)
uint16 dsmi_dup_packet
(
  dsm_item_type **dup_ptr,
  dsm_mempool_id_type pool,
  dsm_item_type *src_ptr,
  uint16 offset,
  uint16 cnt,
  const char * file,
  uint32 line
);

#define dsm_dup_packet_pool_long(dup_ptr, pool, src_ptr, offset, cnt) \
       dsmi_dup_packet_long(dup_ptr, pool, src_ptr, offset, cnt,\
                                  __FILE__,__LINE__)
uint32 dsmi_dup_packet_long
(
  dsm_item_type **dup_ptr,
  dsm_mempool_id_type pool,
  dsm_item_type *src_ptr,
  uint32 offset,
  uint32 cnt,
  const char * file,
  uint32 line
);

/*===========================================================================
FUNCTION DSM_DUP_EXTERNAL_BUFFER()

DESCRIPTION
  Duplicate first 'cnt' bytes of external buffer starting at 'src_ptr'. 

  This is done by obtaining a new item from the DUP pool.  Payload data is
  not copied.  The header information is initialized with size and used
  fields set to the 'cnt' value, and the data_ptr
  of the dup item points into the source item's data memory.  (i.e., the
  pointers are set up to share the data segment of the original item). 
  The return pointer is passed back through the first argument, and the
  return value is the number of bytes actually duplicated.

DEPENDENCIES
  None

PARAMETERS
  dup_ptr - Pointer to pointer where new packet will be saved.
  src_ptr - Pointer to the external buffer to be duped.
  cnt     - Number of bytes from src_ptr that should be duped

RETURN VALUE
  The total size in bytes of the duplicated bufer.

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_dup_external_buffer_pool( dup_ptr, pool, src_ptr, cnt) \
           dsmi_dup_external_buffer( dup_ptr, pool, src_ptr, cnt,  \
                                     __FILE__,__LINE__)

uint16 dsmi_dup_external_buffer
(
  dsm_item_type **dup_ptr,
  dsm_mempool_id_type pool,
  uint8          *src_ptr,
  uint16 cnt,
  const char * file,
  uint32 line
);

#define dsm_dup_external_buffer_pool_long( dup_ptr, pool, src_ptr, cnt) \
            dsmi_dup_external_buffer_long( dup_ptr, pool, src_ptr, cnt, \
                                           __FILE__,__LINE__)

uint32 dsmi_dup_external_buffer_long
(
  dsm_item_type **dup_ptr,
  dsm_mempool_id_type pool,
  uint8          *src_ptr,
  uint32          cnt,
  const char *    file,
  uint32          line
);


/*===========================================================================
FUNCTION DSM_TRIM_PACKET()

DESCRIPTION
  Trim passed packet to the specified length.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer of packet to trim.
  length - Number of bytes to leave at the head of the packet.  Ie.  The
           packet will be the lesser of this length or its current length 
           when this operation completes.

RETURN VALUE
  None

SIDE EFFECTS
  *pkt_head_ptr will get set to null if the length to trim is longer
  than the packet.
===========================================================================*/
#define dsm_trim_packet(pkt_head_ptr,length) \
       dsmi_trim_packet(pkt_head_ptr,length,__FILE__,__LINE__)
void dsmi_trim_packet
(
  dsm_item_type **pkt_head_ptr,
  uint16 length,
  const char * file,
  uint32 line
);

#define dsm_trim_packet_long(pkt_head_ptr,length) \
       dsmi_trim_packet_long(pkt_head_ptr,length,__FILE__,__LINE__)
void dsmi_trim_packet_long
(
  dsm_item_type **pkt_head_ptr,
  uint32 length,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_APPEND()

DESCRIPTION
  Append a DSM buffer to the end of a DSM buffer chain (packet).

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer to first buffer
  pkt2_head_ptr - Pointer to pointer to packet to append to first packet. 

RETURN VALUE
  None

SIDE EFFECTS
  The passed data_item_ptr pointer variable is set to NULL.
===========================================================================*/
#define dsm_append(pkt_head_ptr,pkt2_head_ptr) \
        dsmi_append(pkt_head_ptr,pkt2_head_ptr,__FILE__,__LINE__)
void dsmi_append
(
  dsm_item_type **pkt_head_ptr,
  dsm_item_type **pkt2_head_ptr,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_PUSHDOWN()

DESCRIPTION
  Insert specified amount of contiguous new space at the beginning of
  an dsm_item_type chain. If enough space is available in the first
  dsm_item_type, no new space is allocated. Otherwise a dsm_item_type
  of the appropriate size is allocated and tacked on the front of the
  chain.  The cnt is updated.

  The passed pointer address variable (pkt_head_ptr) may be changed,
  if the pushdown requires an additional data item.
  
  If the pool id passed in is for a ds pool (large or small), "size" 
  parameter is used to get the appropriate memory pool item. 
  So, if this function is being used for DS, either pass in 
  DSM_DS_LARGE_ITEM_POOL or DSM_DS_SMALL_ITEM_POOL.

  The 'used' field of the newly-created packet front is set to 'size'.

  This operation is the logical inverse of pullup(), hence the name.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to pushdown on.
  buf - Pointer to buffer of data to pushdown on packet
  size - Size of data to pushdown 
  pool_id - Pool id from which new buffers should be allocated.

RETURN VALUE
  The number of bytes that were pushed on to the head of the packet. 

SIDE EFFECTS
  The 1st parameter may be assigned in this function
===========================================================================*/
#define dsm_pushdown(pkt_head_ptr,buf,size,pool_id) \
       dsmi_pushdown(pkt_head_ptr,buf,size,pool_id,__FILE__,__LINE__)
uint16 dsmi_pushdown
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint16 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);

#define dsm_pushdown_long(pkt_head_ptr,buf,size,pool_id) \
       dsmi_pushdown_long(pkt_head_ptr,buf,size,pool_id,__FILE__,__LINE__)
uint32 dsmi_pushdown_long
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint32 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_PUSHDOWN_PACKED()

DESCRIPTION
  Insert specified amount of contiguous new space at the beginning of
  an dsm_item_type chain. If enough space is available in the first
  dsm_item_type, no new space is allocated. Otherwise a dsm_item_type
  of the appropriate size, minus the space already available at the
  front of the chain, is allocated and tacked on the front of the
  chain.  The cnt is updated. Note:  This will pack the data against 
  the existing data, leaveing any unused space at the FRONT of the  
  chain.

  The passed pointer address variable (pkt_head_ptr) may be changed,
  if the pushdown requires an additional data item.
  
  If the pool id passed in is for a ds pool (large or small), "size" 
  parameter is used to get the appropriate memory pool item. 
  So, if this function is being used for DS, either pass in 
  DSM_DS_LARGE_ITEM_POOL or DSM_DS_SMALL_ITEM_POOL.

  This operation is the logical inverse of pullup(), hence the name.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to pushdown on.
  buf - Pointer to buffer of data to pushdown on packet
  size - Size of data to pushdown 
  pool_id - Pool id from which new buffers should be allocated.

RETURN VALUE
  The number of bytes that were pushed on to the head of the packet. 

SIDE EFFECTS
  The 1st parameter may be assigned in this function
===========================================================================*/
//FIXME History of DSM_PUSHDOWN_PACKED_VER ?

#define DSM_PUSHDOWN_PACKED_VER 1
#define dsm_pushdown_packed(pkt_head_ptr,buf,size,pool_id) \
       dsmi_pushdown_packed(pkt_head_ptr,buf,size,pool_id,__FILE__,__LINE__)
uint16 dsmi_pushdown_packed
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint16 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);

#define dsm_pushdown_packed_long(pkt_head_ptr,buf,size,pool_id) \
       dsmi_pushdown_packed_long(pkt_head_ptr,buf,size,pool_id, \
                                 __FILE__,__LINE__)
uint32 dsmi_pushdown_packed_long
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint32 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_PUSHDOWN_TAIL()

DESCRIPTION
  Insert specified amount of memory space at the end of a dsm_item_type 
  chain and copy data into this space if provided. If enough space is 
  available in the last item, no new space is allocated.  Otherwise 
  sufficient dsm items of the same type to supply the requested memory
  space are allocated and appended to the end of the chain.  

  This operation is the logical inverse of pullup_tail(), hence the name.

DEPENDENCIES
  None.

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to pushdown on.
  buf - Pointer to buffer of data to pushdown on packet
  size - Size of data to pushdown 
  pool_id - Pool id from which new buffers should be allocated.

RETURN VALUE
  The number of bytes that were pushed on to the tail end of the last item.
  Note this can be less than the requested amount.

SIDE EFFECTS
  If the pushdown requires an additional data item, the pkt_ptr field of the 
  last item in the passed packet chain may be changed.
===========================================================================*/
#define dsm_pushdown_tail(pkt_head_ptr,buf,size,pool_id) \
       dsmi_pushdown_tail(pkt_head_ptr,buf,size,pool_id,__FILE__,__LINE__)
uint16 dsmi_pushdown_tail
(
  dsm_item_type **pkt_head_ptr,
  const void *buf,
  uint16 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);

#define dsm_pushdown_tail_long(pkt_head_ptr,buf,size,pool_id) \
       dsmi_pushdown_tail_long(pkt_head_ptr,buf,size,pool_id,__FILE__,__LINE__)

uint32 dsmi_pushdown_tail_long
(
  dsm_item_type **pkt_head_ptr,
  const void *buf,
  uint32 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
);



/*===========================================================================
FUNCTION DSM_PULLUP()

DESCRIPTION
 Copy and remove "cnt" bytes from beginning of packet. Return number of
 bytes actually pulled off.

DEPENDENCIES
  The passed data item must not be on a queue.

PARAMETERS
  pkt_head_ptr - Pointer to pointer to head of the packet.
  buf - Destination pointer for the data form the packet.
  cnt - The byte count to pull from packet head. 

RETURN VALUE
  The number of bytes pulled from the passed packet head ptr.

SIDE EFFECTS
  When the last byte is pulled from a buffer item the item is returned to
  its free queue.
===========================================================================*/
#define dsm_pullup(pkt_head_ptr,buf,cnt) \
        dsmi_pullup(pkt_head_ptr,buf,cnt,__FILE__,__LINE__)
uint16 dsmi_pullup
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint16 cnt,
  const char * file,
  uint32 line
);
#define dsm_pullup_long(pkt_head_ptr,buf,cnt) \
        dsmi_pullup_long(pkt_head_ptr,buf,cnt,__FILE__,__LINE__)

uint32 dsmi_pullup_long
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint32 cnt,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_PULLUP_TAIL()

DESCRIPTION
 Copy and remove "cnt" bytes from the end of packet. Return number of
 bytes actually pulled off.  

DEPENDENCIES
  The passed data item must not be on a queue.

PARAMETERS
  pkt_head_ptr - Address of ptr to head of packet. 
  buf - Destination buffer for the data from the packet
  cnt - Number of bytes to pull from the packet.  

RETURN VALUE
  The number of bytes pulled from the passed packet head ptr. If the number 
  of bytes is greater than the  packet chain, the entire packet chain is 
  returned.

SIDE EFFECTS
  When the last byte is pulled from a buffer item the item is returned to
  its free queue.
===========================================================================*/
#define dsm_pullup_tail(pkt_head_ptr,buf,cnt) \
        dsmi_pullup_tail(pkt_head_ptr,buf,cnt,__FILE__,__LINE__)
uint16 dsmi_pullup_tail
(
  dsm_item_type **pkt_head_ptr,
  void          *buf,
  uint16        cnt,
  const char * file,
  uint32 line
);

#define dsm_pullup_tail_long(pkt_head_ptr,buf,cnt) \
        dsmi_pullup_tail_long(pkt_head_ptr,buf,cnt,__FILE__,__LINE__)
uint32 dsmi_pullup_tail_long
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint32 cnt,
  const char * file,
  uint32 line
);

/*===========================================================================
FUNCTION DSM_PULL8()

DESCRIPTION
  Pull single character from dsm_item_type

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to item pull from

RETURN VALUE
  returns -1 if nothing can be pulled from passed data item.

SIDE EFFECTS
  When the last byte is pulled from a buffer item the item is returned to
  its free queue (in dsm_pullup).
===========================================================================*/
#define dsm_pull8(pkt_head_ptr) \
        dsmi_pull8(pkt_head_ptr,__FILE__,__LINE__)

int16 dsmi_pull8
(
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_PULL16()

DESCRIPTION
 Pull a 16-bit integer in host order from buffer in network byte order.
 Return -1 on error

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to item pull from

RETURN VALUE
  returns -1 if nothing.  Otherwise the a uint16 that is in HOST byte order.

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_pull16(pkt_head_ptr) \
        dsmi_pull16(pkt_head_ptr,__FILE__,__LINE__)

int32 dsmi_pull16
(
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_PULL32()

DESCRIPTION
 Pull a 32-bit integer in host order from buffer in network byte order.
 On error, return 0. Note that this is indistinguishable from a normal
 return.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer of item to pull from 

RETURN VALUE
  returns 0 if nothing.  Otherwise uint32 in HOST byte order. 

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_pull32(pkt_head_ptr) \
        dsmi_pull32(pkt_head_ptr,__FILE__,__LINE__)

uint32 dsmi_pull32
(
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION DSM_EXTRACT()

DESCRIPTION
 Copy data from within dsm item to user-provided buffer, starting at
 'offset' bytes from start of dsm item and copying no more than 'len'
 bytes. Return actual number of bytes copied

DEPENDENCIES
  None

PARAMETERS
  packet_ptr - Pointer to packet to remove data from
  offset - Offset this number of bytes from packet head. 
  buf - Pointer to destination buffer
  len - length in bytes of extraction. 

RETURN VALUE
  returns  0 if nothing

SIDE EFFECTS
  None
===========================================================================*/
uint16 dsm_extract
(
  dsm_item_type *packet_ptr,
  uint16 offset,
  void *buf,
  uint16 len
);

uint32 dsm_extract_long
(
  dsm_item_type *packet_ptr,
  uint32 offset,
  void *buf,
  uint32 len
);


/*===========================================================================
FUNCTION DSM_INSERT()

DESCRIPTION
 Copy data to within dsm item from user-provided buffer, starting at
 'offset' bytes from start of dsm item and copying (overwriting) no more
 than 'len' bytes. Return actual number of bytes copied.
  
 If the item has has dependencies, make a copy of the dependent item and
 write to that instead.

 If the len, spans the end of the packet, the packet will be extended. If the
 new data starts off the end of the packet this WILL fail.. (Ie. offset must
 fall within the packet, but offset+len may not fall within the packet.)


DEPENDENCIES
  None

PARAMETERS
  packet_ptr - Pointer to packet to remove data from 
  pool_id - Should it be necessary to allocate more memory, do it from here.
  offset - Offset this number of bytes from packet head. 
  buf - Pointer to destination buffer
  len - length in bytes of extraction. 

RETURN VALUE
  Returns number of bytes written, 0 if nothing happened.

SIDE EFFECTS
  None
===========================================================================*/

#define dsm_insert(packet_ptr,pool_id,offset,buf,len) \
        dsmi_insert(packet_ptr,pool_id,offset,buf,len,__FILE__,__LINE__)
uint16 dsmi_insert
(
  dsm_item_type **packet_ptr,
  dsm_mempool_id_type pool_id,
  uint16 offset,
  void * buf,
  uint16 len,
  const char * file,
  uint32 line
);


#define dsm_insert_long(packet_ptr,pool_id,offset,buf,len) \
        dsmi_insert_long(packet_ptr,pool_id,offset,buf,len,__FILE__,__LINE__)
uint32 dsmi_insert_long
(
  dsm_item_type **packet_ptr,
  dsm_mempool_id_type pool_id,
  uint32 offset,
  void * buf,
  uint32 len,
  const char * file,
  uint32 line
);


/*===========================================================================
FUNCTION  DSM_SEEK_EXTRACT()

DESCRIPTION
  This function will seek offset bytes into the item that is passed in.  If
  'buf' is not NULL it will copy 'len' bytes into it. If there are not 'len'
  bytes to copy all the butes from offesetuntil the end of the item chain
  will be copied.
  If the seek or the extract tries go beyond the end of the item chain the
  function will return FALSE, and niether 'item_head_ptr' nor 'offset_ptr
  will' be modified.  Otherwise 'item_head_ptr' and 'offset_ptr' will be set
  to indicate the SEEK end point (which is not the EXTRACT end point, this
  means that the offset will indicate the first byte that was copied).

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  We have successfully seeked to the right byte, and if we were doing
         an extract we copied ALL of the bytes indicated by 'len.'
           pkt_head_ptr: set to the item in which the seek ended
           offset_ptr: set to the offset in the item in which the seek ended
           len: if bytes were extracted this is set to the number that were

  FALSE: We have seeked past the end of the packet without finding the byte
         we are interesed in or we have seeked to the right place but not
         succeeded in copying all of the bytes.  Niether 'pkt_head_ptr' or
         'offset_ptr' are modified.  However:
             len: will be changed to reflect the number of bytes that were
                  copied


                                ***WARNING***
  Successful completion of this call WILL modify the first two arguments, and
  in all cases the last two arguments MAY be modified.
  

SIDE EFFECTS
  None
===========================================================================*/
boolean dsm_seek_extract
(
  dsm_item_type **item_head_ptr,
  uint16 *offset_ptr,
  void *buf,
  uint16 *len
);

boolean dsm_seek_extract_long
(
  dsm_item_type **pkt_head_ptr,
  uint32 *offset_ptr,
  void *buf,
  uint32 *len
);


/*===========================================================================
FUNCTION DSM_PEEK_BYTE()

DESCRIPTION
  This routine peeks at the value at the specified offset in a packet.
  
DEPENDENCIES
  None

PARAMETERS
  item_head_ptr - Ponter to head of packet chain. 
  offset - Offset to peek at. 
  value - Pointer to byte to put result. 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean dsm_peek_byte
(
  dsm_item_type *item_head_ptr,
  uint16 offset,
  uint8 *value
);

boolean dsm_peek_byte_long
(
  dsm_item_type* item_head_ptr,
  uint32 offset,
  uint8* value
);


/*===========================================================================
FUNCTION DSM_VERIFY_PACKET()

DESCRIPTION
  Verify that an item is in good working order. 

DEPENDENCIES

PARAMETERS
  item_ptr - pointer to the item you would like verified.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define dsm_verify_packet(item) dsmi_verify_packet(item)

void dsmi_verify_packet
(
  dsm_item_type *item_ptr
);


/*===========================================================================
FUNCTION DSM_SPLIT_PACKET()

DESCRIPTION
  This routine splits a dsm packet chain at LENGTH bytes.  HEAD will
  point at the head of the original packet which will be trimmed to LENGTH
  bytes.  TAIL will point at head of the second part of the packet that starts
  at LENGTH+1 in the original packet.
  
DEPENDENCIES
  None

PARAMETERS
  head          - pointer to packet to split
  tail          - recipient of rest of the packet
  length        - amount to retain in "head" packet
  dup_pool      - dup pool to use for new item.

RETURN VALUE
  Amount of bytes split. This should equal LENGTH.

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsmi_split_packet
(
  dsm_item_type ** pkt_head_ptr,
  dsm_item_type ** pkt_tail_ptr,
  uint32 length,
  dsm_mempool_id_type dup_pool_id,
  const char * file,
  uint32 line
);

#define dsm_split_packet(pkt_head_ptr,pkt_tail_ptr,length,dup_pool_id) \
       dsmi_split_packet(pkt_head_ptr,pkt_tail_ptr,length,dup_pool_id, \
                         __FILE__,__LINE__)
     
/*===========================================================================
FUNCTION DSM_ITERATOR()

DESCRIPTION
  Iterates down a DSM item chain and calls the user specified "iterator" function  on each dsm buffer. 
  The iterator function is allowed to modify the contents of each item as a buffer, using the data_ptr and
  used as the location and length allowed.

  The iterator function returns 0 or 1 depending on whether it wishes to
  continue processing or to stop processing.

  An example would be a search iterator. The search iterator would return a 1
  when the found condition was met. The user data could point to the search
  parameters and track and return the offset of found information.
  
DEPENDENCIES
  None

PARAMETERS
  pkt_ptr       - pointer to item chain
  offset        - offset into chain before starting iteration
  len           - amount of data to process through iterator
  iterator_f    - pointer to iterator function that takes pointer/length
                  and returns non-zero if it wants to stop iterating.

RETURN VALUE
  0 - If iterator completed without stopping.
  non 0 - If stopped on iterator function condition.

SIDE EFFECTS
  None
===========================================================================*/
int 
dsmi_iterator
(
  dsm_item_type * pkt_ptr,
  uint32 offset,
  uint32 len,     
  void * user_data,
  int (*iterator_f)(void *user_data, const char * buf, uint32 len),
  const char * file,
  uint32 line
);

#define dsm_iterator(pkt_ptr,offset,len,user_data,iterator_f)\
  dsmi_iterator(pkt_ptr,offset,len,user_data,iterator_f,__FILE__,__LINE__)

#endif /* DSM_ITEM_H */

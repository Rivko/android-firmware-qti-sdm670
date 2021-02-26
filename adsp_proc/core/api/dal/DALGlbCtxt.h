#ifndef _DALGLBCTXT_H
#define _DALGLBCTXT_H

/*==================-===========================================================
  Copyright (c) 2009 Qualcomm Technologies Incorporated.
           All Rights Reserved.

$Header: //components/rel/core.qdsp6/2.1/api/dal/DALGlbCtxt.h#1 $

when       who     description
--------   ---     ----------------------------------------------------------
03/12/09   VK      Changed DALLOCK prefix to DALGLBCTXT
03/07/09   VK      Initial revision

==============================================================================*/
#include "DALSys.h"

// the following lock types are unused as support for global context locking
// has been withdrawn
#define DALGLBCTXT_LOCK_TYPE_NONE            0x0
#define DALGLBCTXT_LOCK_TYPE_SPINLOCK        0x010000
#define DALGLBCTXT_LOCK_TYPE_SHARED_TOKEN    0x020000

#define DALGLBCTXT_GLB_HDR_NAME_LEN  12

typedef struct _DAL_LOCK
{
   uint32 lock;
   uint32 reserved;
   uint32 type;
   uint32 version; 
}
DAL_LOCK;

typedef struct _DALGLB_HEADER
{
   uint32 size;
   char name[DALGLBCTXT_GLB_HDR_NAME_LEN]; 
   DAL_LOCK lock;
}
DALGLB_HEADER;


/*--------------------------------------------------
  Each context must start with the following header

  An example for i2c
  typedef struct _i2c_ctxt
  {
      DALGLB_HEADER hdr;
      uint32 hwConfigChangeCount;
      uint32 freq;
      etc...
  }
  i2c_ctxt


So alloc would look like this:

DALGLB_AllocCtxt( "I2CHWSTATE", sizeof(i2c_ctxt), DALMP_LOCK_TYPE_SHARED_TOKEN, &pHdr );

--------------------------------------------------*/

/*
  @brief Initialize global context functionality

  This may be called multiple times with no adverse effects.
  Global context is also initialized when DALGLBCTXT_AllocCtxt or 
  DALGLBCTXT_FindCtxt is called before this is called, 

  @param None

  @return DAL_SUCCESS if initialization succeeds, DAL_ERROR otherwise
*/
DALResult
DALGLBCTXT_Init(void);

/*
  @brief allocate a named global context

  A global context is essentially a dynamically allocated shared memory
  Once allocated, it can be accessed by other processors.
  If a global context to be allocated already exists, the existing 
  global context is returned.

  @param    name           global context name
            size_req       requested global context size in bytes
            unused_param   previously lock_type but lock is no longer supported
            ctxt           out pointer to global context memory

  @return   DAL_SUCCESS if the global context can be allocated or found, 
            DAL_ERROR otherwise
*/
DALResult 
DALGLBCTXT_AllocCtxt(const char * name, uint32 size_req, uint32 unused_param, 
      void ** ctxt);

/*
  @brief find a named global context

  A global context is essentially a dynamically allocated shared memory
  Once allocated, it can be accessed by other processors.

  @param    name        global context name
            ctxt        out pointer to global context memory

  @return   DAL_SUCCESS if the global context is found, 
            DAL_ERROR otherwise
*/
DALResult 
DALGLBCTXT_FindCtxt(const char * name, void ** ctxt);

#endif /* _DALGLBCTXT_H */


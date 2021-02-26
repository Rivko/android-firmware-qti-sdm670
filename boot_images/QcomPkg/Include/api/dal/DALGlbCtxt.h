#ifndef _DALGLBCTXT_H
#define _DALGLBCTXT_H

/*==================-===========================================================
  Copyright (c) 2009, 2014 QUALCOMM Technologies, Inc.
           All Rights Reserved.
  Qualcomm Confidential and Proprietary


when       who     description
--------   ---     ----------------------------------------------------------
03/12/09   VK      Changed DALLOCK prefix to DALGLBCTXT
03/07/09   VK      Initial revision

==============================================================================*/


#include "DALSys.h"

#define DALGLBCTXT_VERSION                   0x00010001

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

DALResult
DALGLBCTXT_Init(void);

DALResult 
DALGLBCTXT_AllocCtxt(const char * name, uint32 size_req, uint32 lock_type, void ** ctxt);

DALResult 
DALGLBCTXT_FindCtxt(const char * name, void ** ctxt);

#endif /* _DALGLBCTXT_H */


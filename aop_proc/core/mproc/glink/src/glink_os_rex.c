
/*===========================================================================

                    GLINK Rex OS Abstraction

 Copyright (c) 2016 by QUALCOMM Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/mproc/glink/src/glink_os_rex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/16    dks     Initial version. 
===========================================================================*/



#include "glink_os_utils.h"
#include <stdlib.h>
#include "aop_ipc.h"


/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

void glink_os_send_interrupt( glink_os_ipc_intr_type id )
{
  AOP_IPC_SendInterrupt ((AOP_IPC_SubsystemType)id, AOP_IPC_PrimaryInterrupt);
}


void glink_os_mem_set(void* ptr,int value,unsigned int size)
{
  uint32* start, *end;

  /* MSGRAM can be accessed only in 4byte aligned address. So both size
   and MSGRAM address should be 4 byte aligned */
  if(! ALIGNED(32,(uint32)ptr))
  {
    ERR_FATAL("memset: non-aligned memory ptr",0,0,0);
  }
  if(! ALIGNED(32,size))
  {
    ERR_FATAL("memset: non-aligned size",0,0,0);
  }

  start = (uint32*)ptr;
  end = start + size/sizeof(uint32);

  while(start <= end) 
  {
    *start++ = value;
  }
}

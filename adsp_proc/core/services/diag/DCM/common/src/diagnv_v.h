#ifndef DIAGNV_V_H
#define DIAGNV_V_H
/*==========================================================================
  
General Description
 This module contains prototypes for diag_nv_read & diag_nv_write.
                
Copyright (c) 2009, 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagnv_v.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/22/13   sr     Removed PACK from pointer references 
11/02/09   as     Merged DUAL sim support 
07/10/09   as     Mainlined code under FEATURE_DIAG_CACHE
16/03/09   vk     Created file.
                   
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "log_codes.h"
#include "feature.h"
#include "nv.h"
#include "diagnv.h"



/*===========================================================================

FUNCTION DIAGNV_PEEK

DESCRIPTION
  This procedure encapsulates an NV peek operation for the diag task.
  This capability is not recommended and is intended to be deprecated.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_peek (nv_peek_type *peek);



/*===========================================================================

FUNCTION DIAGNV_POKE
DESCRIPTION
  This procedure encapsulates an NV poke operation for the diag task.
  This capability is not recommended and is intended to be deprecated.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_poke (nv_poke_type *poke);


/*===========================================================================

FUNCTION DIAGNV_GET_MEID

DESCRIPTION
  Method to extract MEID.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_get_meid  (qword);

/*===========================================================================

FUNCTION DIAGNV_GET_MEID_EXT

DESCRIPTION
  Method to extract MEID.when context is specified.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_get_meid_ext  (qword, uint16);


/*===========================================================================

FUNCTION DIAGNV_REFRESH_CACHE

DESCRIPTION
  This procedure reads the NV data for the status packet into cache.
  This is done after the mc_task_start call and after OTASP successfully
  completes.

===========================================================================*/
void diagnv_refresh_cache (void);

/*===========================================================================

FUNCTION DIAG_NV_READ_EXT

DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task when a 
  context is specified.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_read_ext ( 
  nv_items_enum_type item,              /* Which NV item to read */
  void * buf_ptr,           /* Where to put the data */
  uint16 item_context
);

/*===========================================================================

FUNCTION DIAG_NV_WRITE_EXT
DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_write_ext ( 
  nv_items_enum_type item,
  void * buf_ptr,
  uint16 item_context
);

#endif /* DIAGNV_V_H*/


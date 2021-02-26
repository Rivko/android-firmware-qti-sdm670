/** vi: tw=128 ts=3 sw=3 et
@file pd_dump_user.c
@brief This file contains the API details for the Protection Domain Monitor, API 1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 - 2019 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/pd_mon/src/pd_dump_user.c#3 $
$DateTime: 2019/02/18 01:48:20 $
$Change: 18354452 $
$Author: pwbldsvc $
===============================================================================*/

#include "pd_mon_qurt.h"
#include "pd_dump.h"
#include "qurt_memory.h"
#include "qurt_thread_context.h"

// Defines a size of virtual memory range which will be used to communicate TCB data into 
// process memory dump.  No physical on device memory will be consumed, just a virtual address range
#define PD_DUMP_MAGIC_REGION_BYTES sizeof(struct qurt_debug_thread_info) * 1024 

#define PD_DUMP_DEVICE "dump/"

#define PD_DUMP_ADD_NOTE_SECTION      (0+QDI_PRIVATE)

typedef struct pd_dump_magic_region_v1_struct
{
  struct qurt_debug_thread_info * addr;
  unsigned int size;
} pd_dump_magic_region_v1_type;

// do not rename this symbol, it has crashscope dependents
static pd_dump_magic_region_v1_type pd_dump_magic_region_v1 = {0,PD_DUMP_MAGIC_REGION_BYTES};
static qurt_mem_region_t pd_dump_magic_region;

/**
API, pd_dump_init
@param[in] PD_DUMP_NAME NULL Terminated Device Name
@return
PD_DUMP_HANDLE Opaque Device Handle
*/
PD_DUMP_HANDLE pd_dump_init(void)
{
   PD_DUMP_HANDLE rc = -1;

   /**** 
     Following block reserves a non-mapped region of virtual memory addresses
     into which the pddump utility will place process specific data which is
     not directly associated with the process memory and therefore not dumped
     by default
   ****/
   {
     qurt_mem_region_attr_t magic_attr;

     qurt_mem_region_attr_init(&magic_attr);
     qurt_mem_region_attr_set_mapping(&magic_attr, QURT_MEM_MAPPING_NONE);
     
     if (QURT_EOK != qurt_mem_region_create(&pd_dump_magic_region, 
                            PD_DUMP_MAGIC_REGION_BYTES,
                            qurt_mem_default_pool,
                            &magic_attr))
     {//failure
       qurt_printf("%s, qurt_mem_region_create failed for size 0x%x\n", __func__, pd_dump_magic_region_v1.size);

       pd_dump_magic_region = 0;  // not sure what QURT does with it, probably -1
       pd_dump_magic_region_v1.addr = 0;  // make it clear we got nothing
       pd_dump_magic_region_v1.size = 0;  // make it clear we got nothing
     }
     else
     {//success
       qurt_mem_region_attr_get(pd_dump_magic_region, &magic_attr); //seems to be a standard pattern
       qurt_mem_region_attr_get_virtaddr(&magic_attr, (unsigned int *)&pd_dump_magic_region_v1.addr);

       qurt_printf("%s, pd_dump_magic_region: 0x%x @ 0x%x\n", 
                   __func__, PD_DUMP_MAGIC_REGION_BYTES, pd_dump_magic_region_v1.addr);
     }
   }

   rc = qurt_qdi_open(PD_DUMP_DEVICE, pd_dump_magic_region_v1.addr, PD_DUMP_MAGIC_REGION_BYTES, &pd_dump_magic_region_v1.size);

   qurt_printf("%s, device name '%s', return %d\n", __func__, PD_DUMP_DEVICE, rc);

   return rc;
}

DLL_API_GLOBAL PD_DUMP_STATUS pd_dump_add_note_section(PD_DUMP_HANDLE handle, char * name, char * desc, uint32 descsz, uint32 type )
{
   return qurt_qdi_handle_invoke(handle, PD_DUMP_ADD_NOTE_SECTION, name, desc, descsz, type);
}

/**
API, pd_dump_destroy
@param[in] PD_DUMP_HANDLE Opaque Device Handle
@return
PD_DUMP_STATUS Error Indication
*/
PD_DUMP_STATUS pd_dump_destroy(PD_DUMP_HANDLE handle)
{
   PD_DUMP_STATUS rc = PD_DUMP_STATUS_ERROR;

   if (pd_dump_magic_region_v1.addr)
   {
     qurt_mem_region_delete((qurt_mem_region_t)pd_dump_magic_region_v1.addr);
   }

   rc = 0 <= qurt_qdi_close(handle) ? PD_DUMP_STATUS_SUCCESS : PD_DUMP_STATUS_ERROR;

   qurt_printf("%s, return %d\n", __func__, rc);

   return rc;
}

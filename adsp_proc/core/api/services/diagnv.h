#ifndef DIAGNV_H
#define DIAGNV_H

/*! 
  @ingroup diag_misc 
  @file diagnv.h 
  @brief 
    Diagnostic services miscellaneous header File.
    Provides Diagnostic NV services miscellaneous APIs 
*/

/*
==========================================================================
                
Copyright (c) 2001-2010, 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            Edit History

  $Header: //components/rel/core.qdsp6/2.1/api/services/diagnv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/17   sp      Removed packed void * parameters
05/21/2010 SUG     Doxygenation Changes
09/30/09   JV      Moved definition of diag_nv_cache_type and extern diagnv
                   from diagi.h to here.
07/15/09   mad     Removed unnecessary includes:diagcmd.h,log_codes.h
07/10/09   as      Mainlined code under FEATURE_MEID_SUPPORT
05/07/09   vk      removed dependencies from diagbuf_v.h
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/04/05   as     Modified MEID code to use DIAG_MEID_ENABLE
03/30/05   as     Replaced FEATURE_NV_MEID_ENABLE with
                  FEATURE_DIAG_MEID_ENABLE.
03/14/05   as     Added prototype for diagnv_get_meid.
11/11/01   as     Created file.
                   
===========================================================================*/

#include "comdef.h"
#include "diagpkt.h" /* prldiag.c dependency */
#include "nv.h"


/*! 
   @name DIAG_MEID_ENABLE macro    
   @ingroup diag_misc   
*/ 
#define DIAG_MEID_ENABLE

/*! 
  @ingroup diag_misc
  @brief
    This procedure encapsulates an NV read operation for the
    diag task.
    Assumes that diag_init has set up the NV command buffer,
    ncmd.
    
    @param[in] item nv_items_enum_type
    @param[out] buf_ptr to put the read data
    
    @dependencies
    diag_init
    
    @sideeffects
    None
    
    @return Status of NV operation of type nv_stat_enum_type
	    <ul> 	
             <li> NV_DONE_S - Request completed okay
    		 <li> NV_BUSY_S - Request is queued
    		 <li> NV_BADCMD_S - Unrecognizable command field
    		 <li> NV_FULL_S - The NVM is full
    		 <li> NV_FAIL_S - Command failed, reason other than NVM was full
    		 <li> NV_NOTACTIVE_S - Variable was not active
    		 <li> NV_BADPARM_S - Bad parameter in command block
    		 <li> NV_READONLY_S - Parameter is write-protected and thus read only
    		 <li> NV_BADTG_S - Item not valid for Target
    		 <li> NV_NOMEM_S - free memory exhausted 
    		 <li> NV_NOTALLOC_S - address is not a valid allocation 
    		 <li> NV_STAT_ENUM_PAD = 0x7FFF - Pad to 16 bits on ARM
    		 <li> NV_STAT_ENUM_MAX = 0x7FFFFFFF - Pad to 16 bits on ARM
          </ul>

         
    @sa
    None
 
*/
nv_stat_enum_type diag_nv_read ( 
  nv_items_enum_type item,        /*!< Which NV item to read */
  void * buf_ptr            /*!< buffer pointer to put the read data */
);


/*! 
    @ingroup diag_misc
    @brief
    This procedure encapsulates an NV write operation for the
    diag task.
    Assumes that diag_init has set up the NV command buffer,
    ncmd.
    
    @param[in] item  enum nv_items_enum_type
    @param[in] buf_ptr buffer pointer pointing to the data to be
          written
    
    @dependencies
    diag_init
    
    @sideeffects
    None
    
    @return Status of NV operation of type nv_stat_enum_type 
    	    <ul> 
             <li> NV_DONE_S - Request completed okay
    		 <li> NV_BUSY_S - Request is queued
    		 <li> NV_BADCMD_S - Unrecognizable command field
    		 <li> NV_FULL_S - The NVM is full
    		 <li> NV_FAIL_S - Command failed, reason other than NVM was full
    		 <li> NV_NOTACTIVE_S - Variable was not active
    		 <li> NV_BADPARM_S - Bad parameter in command block
    		 <li> NV_READONLY_S - Parameter is write-protected and thus read only
    		 <li> NV_BADTG_S - Item not valid for Target
    		 <li> NV_NOMEM_S - free memory exhausted 
    		 <li> NV_NOTALLOC_S - address is not a valid allocation 
    		 <li> NV_STAT_ENUM_PAD = 0x7FFF - Pad to 16 bits on ARM
    		 <li> NV_STAT_ENUM_MAX = 0x7FFFFFFF - Pad to 16 bits on ARM
          </ul>

          
    @sa
    None
 
*/
nv_stat_enum_type diag_nv_write ( 
  nv_items_enum_type item,  /*!< Which NV item to write */
  void * buf_ptr      /*!< buffer pointer pointing to the data to be written */
);


/*! 
  @ingroup diag_misc
  @brief
    This procedure encapsulates an NV get esn operation    
    
  @dependencies
    None
    
  @sideeffects
    None
    
    @return Status of GET_ESN operation
          <ul> 
             <li> Error Code 0  – on success
             <li> Error Code -1 – on failure
          </ul>
    @sa
    None
 
*/
  
dword diagnv_get_esn  (void);


/*! 
  @ingroup diag_misc
  Structure to cache NV data in memory
*/
typedef struct
{
  dword esn;            /*!< ESN */

  byte current_nam;     /*!< current nam index (0-3) */
  dword min1[NV_MAX_NAMS][NV_MAX_MINS]; /*!< All the MIN1s */
  word min2[NV_MAX_NAMS][NV_MAX_MINS];  /*!< All the MIN2s */
}
diag_nv_cache_type;

extern diag_nv_cache_type diagnv; /*!< diagnv to cache the data in memory */
extern nv_item_type nitem;  /*!< NV Item buffer for NV interface */

#endif /* DIAGNV_H*/


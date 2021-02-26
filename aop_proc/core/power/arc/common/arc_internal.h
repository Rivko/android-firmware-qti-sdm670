#ifndef __ARC_INTERNAL_H__
#define __ARC_INTERNAL_H__
/*===========================================================================
                              arc_internal.h

DESCRIPTION:
  This file provides internal interface and typedefs used within ARC drvier

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                       Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "comdef.h"
#include "arc.h"

//---------------------------------------------------------------------------
//  Type Definitions
//---------------------------------------------------------------------------

/* Structure containing mapping for an ARC resource to its vote address */
typedef struct arc_rm_addr_map_s
{
  const char *rm;       /* resource/rm name */
  uint32 addr;          /* resource vote address */
} arc_rm_addr_map_t;

//---------------------------------------------------------------------------
//  External declaraions
//---------------------------------------------------------------------------

/* List of resource to address mapping - ordered by what is in actual HW */
extern arc_rm_addr_map_t g_arc_rm_addr_map[];

/* Size of above resource to address mapping list - typically same as 
 * arc rm count */
extern int8 g_arc_rm_addr_map_size;

//===========================================================================
//                       Function Declarations
//===========================================================================

/**
 * <!-- arc_internal_init -->
 *
 * @brief This function performs any internal initialization ARC driver may
 *        have outside of rinit.
 */
void arc_internal_init (void);

#endif   // __ARC_INTERNAL_H__

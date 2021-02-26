/*===========================================================================
                              arc_cfg.c

DESCRIPTION:
  This file provides contains target specific data and configuration for
  ARC module.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------

#include "arc.h"
#include "arc_internal.h"

//---------------------------------------------------------------------------
//  Global Variables
//---------------------------------------------------------------------------

/* List of resource to address mapping - ordered by what is in actual HW.
 * Address field is set up during initialization via command db queries */
arc_rm_addr_map_t g_arc_rm_addr_map[] = 
{
  {"cx.lvl"},
  {"mx.lvl"},
  {"ebi.lvl"},
  {"lcx.lvl"},
  {"lmx.lvl"},
  {"gfx.lvl"},
  {"mss.lvl"},
  {"ddr.lvl"},
  {"xo.lvl"}
};

/* Size of above resource to address mapping list - typically same as 
 * arc rm count */
int8 g_arc_rm_addr_map_size = 
  (int8)(sizeof (g_arc_rm_addr_map) / sizeof (g_arc_rm_addr_map[0]));

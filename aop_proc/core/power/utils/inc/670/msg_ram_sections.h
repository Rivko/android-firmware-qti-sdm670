#ifndef MSG_RAM_SECTIONS_H
#define MSG_RAM_SECTIONS_H
/*===========================================================================
                             msg_ram_sections.h

DESCRIPTION:
  Message RAM section list

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

#include "comdef.h"


//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
//THIS CANNOT BE ADDED TO - SECTIONS CAN ONLY BE RECLAIMED
//This maps the sections of message ram. We can reclaim some of these areas 
//     for stats/logs/etc. if needed (reclaim tier 1 first) 
typedef enum
{
    MSG_RAM_TZ          = 0x0, //RSC_DRV_TZ,          //tier 3 reclaim
    MSG_RAM_HYP         = 0x1, //RSC_DRV_HYP,         //tier 2 reclaim
    MSG_RAM_HLOS        = 0x2, //RSC_DRV_HLOS,       
    MSG_RAM_SECPROC     = 0x3, //RSC_DRV_SECPROC,     //tier 3 reclaim
    MSG_RAM_AUDIO       = 0x4, //RSC_DRV_AUDIO,       
    MSG_RAM_SENSORS     = 0x5, //RSC_DRV_SENSORS,     
    MSG_RAM_AOP         = 0x6, //RSC_DRV_AOP,         //tier 1 reclaim
    MSG_RAM_AOP_LOGGING = 0x7, //RSC_DRV_DEBUG,
    MSG_RAM_GPU         = 0x8, //RSC_DRV_GPU,         //tier 1 reclaim
    MSG_RAM_DISPLAY     = 0x9, //RSC_DRV_DISPLAY,     //tier 1 reclaim  
    MSG_RAM_COMPUTE_DSP = 0xA, //RSC_DRV_COMPUTE_DSP,
    MSG_RAM_MODEM_SW    = 0xB, //RSC_DRV_MODEM_SW,  
    MSG_RAM_DDR_STATS   = 0xC, //RSC_DRV_MODEM_HW,
    MSG_RAM_ARC_CPRF    = 0xD, //RSC_DRV_ARC_CPRF,    //tier 1 reclaim
    MSG_RAM_SMEM_TABLE  = 0xE, //RSC_DRV_BCM_CD0,     
    MSG_RAM_AOP_INFO_0  = 0xF, //RSC_DRV_BCM,
    MSG_RAM_MAX,
} msg_ram_section_usage_enum;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//===========================================================================
//                             Function Definitions
//===========================================================================

/**
 * <!-- msg_ram_get_mem_area -->
 *
 * @brief Return the pointer to a msg ram memory area 
 *
 * @param section : which memory section we want to retrieve
 */ 
void *msg_ram_get_mem_area(msg_ram_section_usage_enum section);


#endif // MSG_RAM_SECTIONS_H

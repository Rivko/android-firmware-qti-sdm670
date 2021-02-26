/*===========================================================================
                             msg_ram_sections.c

DESCRIPTION:
    This file contains the msg ram lookup table

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

#include "msg_ram_layout.h"
#include "msg_ram_sections.h"

/* MSG RAM memory map. 16 1k sections for each DRV. We are currently re-using DRV15 for AOP Info */
msg_ram_sec_t msg_ram_sections[MSG_RAM_MAX] = 
{
  {MSG_RAM_SECTION(MSG_RAM_TZ),         MSG_RAM_DRV_SIZE},        //0x1300000++0x400   
  {MSG_RAM_SECTION(MSG_RAM_HYP),        MSG_RAM_DRV_SIZE},        //0x1310000++0x400 	
  {MSG_RAM_SECTION(MSG_RAM_HLOS),       MSG_RAM_DRV_SIZE},        //0x1320000++0x400
  {MSG_RAM_SECTION(MSG_RAM_SECPROC),    MSG_RAM_DRV_SIZE},        //0x1330000++0x400 
  {MSG_RAM_SECTION(MSG_RAM_AUDIO),      MSG_RAM_DRV_SIZE},        //0x1340000++0x400
  {MSG_RAM_SECTION(MSG_RAM_SENSORS),    MSG_RAM_DRV_SIZE},        //0x1350000++0x400
  {MSG_RAM_SECTION(MSG_RAM_AOP),        MSG_RAM_DRV_SIZE},        //0x1360000++0x400 
  {MSG_RAM_SECTION(MSG_RAM_AOP_LOGGING),MSG_RAM_DRV_SIZE},        //0x1370000++0x400
  {MSG_RAM_SECTION(MSG_RAM_GPU),        MSG_RAM_DRV_SIZE},        //0x1380000++0x400
  {MSG_RAM_SECTION(MSG_RAM_DISPLAY),    MSG_RAM_DRV_SIZE},        //0x1390000++0x400
  {MSG_RAM_SECTION(MSG_RAM_MODEM_SW),   MSG_RAM_DRV_SIZE},        //0x13A0000++0x400    
  {MSG_RAM_SECTION(MSG_RAM_DDR_STATS),  MSG_RAM_DRV_SIZE},        //0x13B0000++0x400
  {MSG_RAM_SECTION(MSG_RAM_COMPUTE_DSP),MSG_RAM_DRV_SIZE},        //0x13C0000++0x400
  {MSG_RAM_SECTION(MSG_RAM_ARC_CPRF),   MSG_RAM_DRV_SIZE},        //0x13D0000++0x400  
  {MSG_RAM_SECTION(MSG_RAM_SMEM_TABLE), MSG_RAM_SMEM_TABLE_SIZE}, //0x13E0000++0x400
  {MSG_RAM_SECTION(MSG_RAM_AOP_INFO_0), MSG_RAM_INFO_0_SIZE}      //0x13F0000++0x400
};


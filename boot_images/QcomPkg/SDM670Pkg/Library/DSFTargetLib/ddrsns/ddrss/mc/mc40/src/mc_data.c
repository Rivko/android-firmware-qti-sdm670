/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
================================================================================*/

#include "mc_common.h"


/**
 * @brief Geometry Tables
 *        Number of rows and columns for each device density
 */        
uint8   lpddr_geometry_table[][2][2] = {
  /* Device_Width x16  |  Device_Width x8  */
  /* nrows, ncols      |  nrows, ncols     */
    {{14,    10},         {15,    10}},  //  2 Gb per channel
    {{15,    10},         {16,    10}},  //  3 Gb per channel
    {{15,    10},         {16,    10}},  //  4 Gb per channel
    {{16,    10},         {17,    10}},  //  6 Gb per channel
    {{16,    10},         {17,    10}},  //  8 Gb per channel
    {{17,    10},         {17,    10}},  // 12 Gb per channel (TBD as per JEDEC spec)
    {{17,    10},         {17,    10}}   // 16 Gb per channel (TBD as per JEDEC spec) updated as per 16Gigabit micron ref
};

uint16   lpddr_size_table[][2][2] = {
  /* Device_Width x16         | Device_Width x8 (double the size, one x16 rank -> two x8 DRAM channels) */
  /* size_mb, addr_offset_bit | size_mb, addr_offset_bit*/
    {{0x100,       28},          {0x200,       29}},  //  2 Gb per channel
    {{0x180,       29},          {0x300,       30}},  //  3 Gb per channel
    {{0x200,       29},          {0x400,       30}},  //  4 Gb per channel
    {{0x300,       30},          {0x600,       31}},  //  6 Gb per channel
    {{0x400,       30},          {0x800,       31}},  //  8 Gb per channel
    {{0x600,       31},          {0xC00,       32}},  // 12 Gb per channel (TBD as per JEDEC spec)
    {{0x800,       31},          {0x1000,      32}}   // 16 Gb per channel (TBD as per JEDEC spec)
};

uint32   lpddr_timing_table[][2] = {
  /* tRFC_AB_in_ps, tRFC_PB_in_ps */
    {130000,        60000, },  //  2 Gb per channel
    {180000,        90000, },  //  3 Gb per channel
    {180000,        90000, },  //  4 Gb per channel
    {280000,        140000,},  //  6 Gb per channel
    {280000,        140000,},  //  8 Gb per channel
    {380000,        190000,},  // 12 Gb per channel (TBD as per JEDEC spec)  //micron 12Gigabit ref
    {380000,        190000,}   // 16 Gb per channel (TBD as per JEDEC spec)  //micron 16Gigabit ref
};

uint16   lpddr_byte_mode_timing_table[][2] = {
  /* tWR, tWTR in 1ps*/
  /* Device_Width x16 | Device_Width x8  */
    {18000,               20000},  //tWR.in_ps  
    {    6,                   4},  //tWR.in_ck  
    {10000,               12000},  //tWTR.in_ps  
    {    8,                   8}   //tWTR.in_ck  
};


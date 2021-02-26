//===========================================================================
//
// FILE:         QUPAC_670_Private.c
//
// DESCRIPTION:  This file lists hardware configuration for all QUPS
//
//===========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.tz/2.0.2/settings/buses/qup_accesscontrol/qupv3/config/670/QUPAC_Private.c#2 $
//
// when       who     what, where, why
// 29/01/18   RC      Splitted the clocks array per QUP
// 09/01/17   DPK     Initial revision for SDM670
//
//===========================================================================
//             Copyright (c) 2017-2018 QUALCOMM Technologies, Incorporated.
//                    All Rights Reserved.
//                QUALCOMM Confidential & Proprietary
//===========================================================================
#include "QupACCommonIds.h" 

const QUPv3_se_resource_type qupv3_se_hw[] =
{
  { QUPV3_0_SE0, (uint8*)0x880000, "gcc_qupv3_wrap0_s0_clk" }, // { 0,  1,  2,  3 } 
  { QUPV3_0_SE1, (uint8*)0x884000, "gcc_qupv3_wrap0_s1_clk" }, // { 17, 18, 19, 20 }
  { QUPV3_0_SE2, (uint8*)0x888000, "gcc_qupv3_wrap0_s2_clk" }, // { 27, 28, 29, 30 }
  { QUPV3_0_SE3, (uint8*)0x88c000, "gcc_qupv3_wrap0_s3_clk" }, // { 41, 42, 43, 44 }
  { QUPV3_0_SE4, (uint8*)0x890000, "gcc_qupv3_wrap0_s4_clk" }, // { 89, 90, 91, 92 }
  { QUPV3_0_SE5, (uint8*)0x894000, "gcc_qupv3_wrap0_s5_clk" }, // { 85, 86, 87, 88 }
  { QUPV3_0_SE6, (uint8*)0x898000, "gcc_qupv3_wrap0_s6_clk" }, // { 45, 46, 47, 48 }
  { QUPV3_0_SE7, (uint8*)0x89c000, "gcc_qupv3_wrap0_s7_clk" }, // { 93, 94, 95, 96 }
  { QUPV3_1_SE0, (uint8*)0xa80000, "gcc_qupv3_wrap1_s0_clk" }, // { 65, 66, 67, 68 }
  { QUPV3_1_SE1, (uint8*)0xa84000, "gcc_qupv3_wrap1_s1_clk" }, // {  4,  5,  6,  7 }
  { QUPV3_1_SE2, (uint8*)0xa88000, "gcc_qupv3_wrap1_s2_clk" }, // { 53, 54, 55, 56 }
  { QUPV3_1_SE3, (uint8*)0xa8c000, "gcc_qupv3_wrap1_s3_clk" }, // { 31, 32, 33, 34 }
  { QUPV3_1_SE4, (uint8*)0xa90000, "gcc_qupv3_wrap1_s4_clk" }, // { 49, 50, 51, 52 }
  { QUPV3_1_SE5, (uint8*)0xa94000, "gcc_qupv3_wrap1_s5_clk" }, // {105,106,107,108 }
  { QUPV3_1_SE6, (uint8*)0xa98000, "gcc_qupv3_wrap1_s6_clk" }, // { 31, 32, 33, 34 }
  { QUPV3_1_SE7, (uint8*)0xa9c000, "gcc_qupv3_wrap1_s7_clk" }, // { 81, 82, 83, 84 }
};

const uint32 qupv3_se_hw_size = sizeof(qupv3_se_hw)/sizeof(qupv3_se_hw[0]);

const QUPv3_common_resource_type qupv3_common_hw[] =
{
  { (uint8*)0x8c0000, "QUP_0C", "QUP_0CS" },
  { (uint8*)0xac0000, "QUP_1C", "QUP_1CS" },
};

const uint32 qupv3_common_hw_size = sizeof(qupv3_common_hw)/sizeof(qupv3_common_hw[0]);

const char *qupv3_0_clocks_arr[] =
{
  "gcc_qupv3_wrap0_core_2x_clk",
  "gcc_qupv3_wrap0_core_clk",
  "gcc_qupv3_wrap_0_m_ahb_clk",
  "gcc_qupv3_wrap_0_s_ahb_clk",
};

const uint32 qupv3_0_clocks_arr_size = sizeof(qupv3_0_clocks_arr)/sizeof(qupv3_0_clocks_arr[0]);

const char *qupv3_1_clocks_arr[] =
{
  "gcc_qupv3_wrap1_core_2x_clk",
  "gcc_qupv3_wrap1_core_clk",
  "gcc_qupv3_wrap_1_m_ahb_clk",
  "gcc_qupv3_wrap_1_s_ahb_clk",
};
const uint32 qupv3_1_clocks_arr_size = sizeof(qupv3_1_clocks_arr)/sizeof(qupv3_1_clocks_arr[0]);

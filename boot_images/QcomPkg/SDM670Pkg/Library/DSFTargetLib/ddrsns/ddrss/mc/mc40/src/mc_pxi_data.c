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

#include "mc_pxi.h"


/**
 * @brief PXI Setting Table
 */
struct pxi_setting_struct pxi_setting_table[] =
{
/*         raw    in    out   dly_caop  dly_cawd  dly_dqop  dm_done_   cq_ts dq_ts cq_op dq_op    cqop   cawd   dqop   dqwd   dqrd
  clk_on,  _open, _sel, _sel, _one_clk, _one_clk, _one_clk, in_mc_sel, _sel, _sel, _sel, _sel,    _addr, _addr, _addr, _addr, _addr */
  { 0x01, { 0x00, 0x00, 0x00 }, { 0x00,    0x00,     0x00,     0x01,    0x00, 0x00, 0x00, 0x00 }, { 0x00, 0x00,  0x00,  0x00,  0x00 } },
  { 0x01, { 0x01, 0x01, 0x01 }, { 0x00,    0x00,     0x00,     0x01,    0x00, 0x00, 0x03, 0x03 }, { 0x00, 0x00,  0x00,  0x00,  0x00 } }
};


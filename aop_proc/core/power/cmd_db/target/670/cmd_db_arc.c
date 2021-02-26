/*===========================================================================
                             cmd_db_arc.c

DESCRIPTION:
  Static data for command DB ARC.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

#include "cmd_db_int.h"
#include "cmd_db_arc.h"

#define SIZE(n) (n * sizeof(struct arc_db))
#define ARRAY_SIZE(n) (sizeof(n) / sizeof(*(n)))

#ifdef CMD_DB_IN_DDR
__attribute__((section("rinit_data_sec")))
#endif
static struct arc_db arc_aux[] =  {
 { /* attributes for cx.lvl */
  .val0 = 0,
  .val1 = 16,
  .val2 = 48,
  .val3 = 64,
  .val4 = 128,
  .val5 = 192,
  .val6 = 256,
  .val7 = 384,
 },
 { /* attributes for mx.lvl */
  .val0 = 0,
  .val1 = 16,
  .val2 = 16,
  .val3 = 256,
  .val4 = 384,
 },
 { /* attributes for ebi.lvl */
  .val0 = 0,
  .val1 = 16,
  .val2 = 48,
  .val3 = 64,
  .val4 = 128,
  .val5 = 192,
  .val6 = 256,
  .val7 = 384,
 },
 { /* attributes for lcx.lvl */
  .val0 = 0,
  .val1 = 16,
  .val2 = 48,
  .val3 = 64,
  .val4 = 128,
  .val5 = 192,
  .val6 = 256,
  .val7 = 384,
 },
 { /* attributes for lmx.lvl */
  .val0 = 0,
  .val1 = 16,
  .val2 = 192,
  .val3 = 256,
  .val4 = 384,
 },
 { /* attributes for gfx.lvl */
  .val0 = 0,
  .val1 = 16,
  .val2 = 48,
  .val3 = 64,
  .val4 = 128,
  .val5 = 192,
  .val6 = 256,
  .val7 = 320,
  .val8 = 384,
  .val9 = 416,
 },
 { /* attributes for mss.lvl */
  .val0 = 0,
  .val1 = 16,
  .val2 = 48,
  .val3 = 64,
  .val4 = 128,
  .val5 = 192,
  .val6 = 256,
  .val7 = 320,
  .val8 = 384,
 },
 { /* attributes for ddr.lvl */
  .val0 = 0,
  .val1 = 32,
  .val2 = 128,
  .val3 = 160,
 },
 { /* attributes for xo.lvl */
  .val0 = 0,
  .val1 = 32,
  .val2 = 80,
  .val3 = 128,
 },
};
#ifdef CMD_DB_IN_DDR
__attribute__((section("rinit_data_sec")))
#endif
static cmd_db_entry_header arc_db_headers[] = {
 { /* header for cx.lvl */
  .res_id = 0x6c766c2e7863LLU,
  .addr = 0x30000,
  .priority[0] = 3388,
  .offset = SIZE(0),
  .len = SIZE(1),
 },
 { /* header for cx.tmr */
  .res_id = 0x726d742e7863LLU,
  .addr = 0x30004,
  .offset = SIZE(1),
  .len = SIZE(0),
 },
 { /* header for mx.lvl */
  .res_id = 0x6c766c2e786dLLU,
  .addr = 0x30010,
  .priority[0] = 3388,
  .offset = SIZE(1),
  .len = SIZE(1),
 },
 { /* header for mx.tmr */
  .res_id = 0x726d742e786dLLU,
  .addr = 0x30014,
  .offset = SIZE(2),
  .len = SIZE(0),
 },
 { /* header for ebi.lvl */
  .res_id = 0x6c766c2e696265LLU,
  .addr = 0x30020,
  .priority[0] = 0,
  .offset = SIZE(2),
  .len = SIZE(1),
 },
 { /* header for ebi.tmr */
  .res_id = 0x726d742e696265LLU,
  .addr = 0x30024,
  .offset = SIZE(3),
  .len = SIZE(0),
 },
 { /* header for lcx.lvl */
  .res_id = 0x6c766c2e78636cLLU,
  .addr = 0x30030,
  .priority[0] = 32,
  .offset = SIZE(3),
  .len = SIZE(1),
 },
 { /* header for lcx.tmr */
  .res_id = 0x726d742e78636cLLU,
  .addr = 0x30034,
  .offset = SIZE(4),
  .len = SIZE(0),
 },
 { /* header for lmx.lvl */
  .res_id = 0x6c766c2e786d6cLLU,
  .addr = 0x30040,
  .priority[0] = 32,
  .offset = SIZE(4),
  .len = SIZE(1),
 },
 { /* header for lmx.tmr */
  .res_id = 0x726d742e786d6cLLU,
  .addr = 0x30044,
  .offset = SIZE(5),
  .len = SIZE(0),
 },
 { /* header for gfx.lvl */
  .res_id = 0x6c766c2e786667LLU,
  .addr = 0x30050,
  .priority[0] = 256,
  .offset = SIZE(5),
  .len = SIZE(1),
 },
 { /* header for gfx.tmr */
  .res_id = 0x726d742e786667LLU,
  .addr = 0x30054,
  .offset = SIZE(6),
  .len = SIZE(0),
 },
 { /* header for mss.lvl */
  .res_id = 0x6c766c2e73736dLLU,
  .addr = 0x30060,
  .priority[0] = 2048,
  .offset = SIZE(6),
  .len = SIZE(1),
 },
 { /* header for mss.tmr */
  .res_id = 0x726d742e73736dLLU,
  .addr = 0x30064,
  .offset = SIZE(7),
  .len = SIZE(0),
 },
 { /* header for ddr.lvl */
  .res_id = 0x6c766c2e726464LLU,
  .addr = 0x30070,
  .priority[0] = 0,
  .offset = SIZE(7),
  .len = SIZE(1),
 },
 { /* header for ddr.tmr */
  .res_id = 0x726d742e726464LLU,
  .addr = 0x30074,
  .offset = SIZE(8),
  .len = SIZE(0),
 },
 { /* header for xo.lvl */
  .res_id = 0x6c766c2e6f78LLU,
  .addr = 0x30080,
  .priority[0] = 36,
  .offset = SIZE(8),
  .len = SIZE(1),
 },
 { /* header for xo.tmr */
  .res_id = 0x726d742e6f78LLU,
  .addr = 0x30084,
  .offset = SIZE(9),
  .len = SIZE(0),
 },
};

const uint16_t arc_db_headers_cnt = ARRAY_SIZE(arc_db_headers);
const uint16_t arc_aux_size = sizeof(arc_aux);
const uint8_t *arc_aux_ext = (uint8_t *)arc_aux;
const cmd_db_entry_header *arc_db_headers_ext = arc_db_headers;

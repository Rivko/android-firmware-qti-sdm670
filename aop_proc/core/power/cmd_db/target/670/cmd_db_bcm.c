/*===========================================================================
                             cmd_db_bcm.c

DESCRIPTION:
  Static data for command DB BCM.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

#include "cmd_db_int.h"
#include "cmd_db_bcm.h"

#define SIZE(n) (n * sizeof(struct bcm_db))
#define ARRAY_SIZE(n) (sizeof(n) / sizeof(*(n)))

#ifdef CMD_DB_IN_DDR
__attribute__((section("rinit_data_sec")))
#endif
static struct bcm_db bcm_aux[] =  {
 { /* attributes for MC0 */
  .bcm_port = 8,
  .bw_unit = 1000000,
  .clk_id = 0,
 },
 { /* attributes for MC1 */
  .bcm_port = 0,
  .bw_unit = 1000000,
  .clk_id = 0,
 },
 { /* attributes for MC2 */
  .bcm_port = 0,
  .bw_unit = 1000000,
  .clk_id = 0,
 },
 { /* attributes for SH0 */
  .bcm_port = 32,
  .bw_unit = 2000000,
  .clk_id = 1,
 },
 { /* attributes for SH1 */
  .bcm_port = 32,
  .bw_unit = 2000000,
  .clk_id = 1,
 },
 { /* attributes for SH2 */
  .bcm_port = 8,
  .bw_unit = 2000000,
  .clk_id = 1,
 },
 { /* attributes for SH3 */
  .bcm_port = 8,
  .bw_unit = 2000000,
  .clk_id = 1,
 },
 { /* attributes for SH4 */
  .bcm_port = 32,
  .bw_unit = 2000000,
  .clk_id = 1,
 },
 { /* attributes for SH5 */
  .bcm_port = 32,
  .bw_unit = 2000000,
  .clk_id = 1,
 },
 { /* attributes for SH6 */
  .bcm_port = 0,
  .bw_unit = 2000000,
  .clk_id = 1,
 },
 { /* attributes for SH7 */
  .bcm_port = 0,
  .bw_unit = 2000000,
  .clk_id = 1,
 },
 { /* attributes for MM0 */
  .bcm_port = 64,
  .bw_unit = 2000000,
  .clk_id = 4,
 },
 { /* attributes for MM1 */
  .bcm_port = 32,
  .bw_unit = 2000000,
  .clk_id = 4,
 },
 { /* attributes for MM2 */
  .bcm_port = 32,
  .bw_unit = 2000000,
  .clk_id = 4,
 },
 { /* attributes for MM3 */
  .bcm_port = 32,
  .bw_unit = 2000000,
  .clk_id = 4,
 },
 { /* attributes for SN0 */
  .bcm_port = 16,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN1 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN2 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN3 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN4 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN5 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN6 */
  .bcm_port = 4,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN7 */
  .bcm_port = 4,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN8 */
  .bcm_port = 16,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN9 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN10 */
  .bcm_port = 16,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN11 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN12 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for SN13 */
  .bcm_port = 8,
  .bw_unit = 600000,
  .clk_id = 3,
 },
 { /* attributes for CE0 */
  .bcm_port = 8,
  .bw_unit = 6802,
  .clk_id = 6,
 },
 { /* attributes for IP0 */
  .bcm_port = 8,
  .bw_unit = 40,
  .clk_id = 7,
 },
 { /* attributes for CN0 */
  .bcm_port = 4,
  .bw_unit = 32768,
  .clk_id = 5,
 },
 { /* attributes for QUP0 */
  .bcm_port = 4,
  .bw_unit = 1,
  .clk_id = 8,
 },
 { /* attributes for ACV */
  .bcm_port = 0,
  .bw_unit = 1000000,
  .clk_id = 11,
 },
 { /* attributes for ALC */
  .bcm_port = 0,
  .bw_unit = 1000000,
  .clk_id = 10,
 },
 { /* attributes for NU0 */
  .bcm_port = 0,
  .bw_unit = 1000000,
  .clk_id = 15,
 },
};
#ifdef CMD_DB_IN_DDR
__attribute__((section("rinit_data_sec")))
#endif
static cmd_db_entry_header bcm_db_headers[] = {
 { /* header for MC0 */
  .res_id = 0x30434dLU,
  .addr = 0x50000,
  .priority[0] = 3900,
  .offset = SIZE(0),
  .len = SIZE(1),
 },
 { /* header for MC1 */
  .res_id = 0x31434dLU,
  .addr = 0x500e0,
  .priority[0] = 2048,
  .offset = SIZE(1),
  .len = SIZE(1),
 },
 { /* header for MC2 */
  .res_id = 0x32434dLU,
  .addr = 0x500e4,
  .priority[0] = 16,
  .offset = SIZE(2),
  .len = SIZE(1),
 },
 { /* header for SH0 */
  .res_id = 0x304853LU,
  .addr = 0x5003c,
  .priority[0] = 3900,
  .offset = SIZE(3),
  .len = SIZE(1),
 },
 { /* header for SH1 */
  .res_id = 0x314853LU,
  .addr = 0x50038,
  .priority[0] = 0,
  .offset = SIZE(4),
  .len = SIZE(1),
 },
 { /* header for SH2 */
  .res_id = 0x324853LU,
  .addr = 0x50034,
  .priority[0] = 0,
  .offset = SIZE(5),
  .len = SIZE(1),
 },
 { /* header for SH3 */
  .res_id = 0x334853LU,
  .addr = 0x50030,
  .priority[0] = 0,
  .offset = SIZE(6),
  .len = SIZE(1),
 },
 { /* header for SH4 */
  .res_id = 0x344853LU,
  .addr = 0x5002c,
  .priority[0] = 1024,
  .offset = SIZE(7),
  .len = SIZE(1),
 },
 { /* header for SH5 */
  .res_id = 0x354853LU,
  .addr = 0x50028,
  .priority[0] = 4,
  .offset = SIZE(8),
  .len = SIZE(1),
 },
 { /* header for SH6 */
  .res_id = 0x364853LU,
  .addr = 0x500e8,
  .priority[0] = 2048,
  .offset = SIZE(9),
  .len = SIZE(1),
 },
 { /* header for SH7 */
  .res_id = 0x374853LU,
  .addr = 0x500ec,
  .priority[0] = 16,
  .offset = SIZE(10),
  .len = SIZE(1),
 },
 { /* header for MM0 */
  .res_id = 0x304d4dLU,
  .addr = 0x50084,
  .priority[0] = 513,
  .offset = SIZE(11),
  .len = SIZE(1),
 },
 { /* header for MM1 */
  .res_id = 0x314d4dLU,
  .addr = 0x50088,
  .priority[0] = 512,
  .offset = SIZE(12),
  .len = SIZE(1),
 },
 { /* header for MM2 */
  .res_id = 0x324d4dLU,
  .addr = 0x5008c,
  .priority[0] = 512,
  .offset = SIZE(13),
  .len = SIZE(1),
 },
 { /* header for MM3 */
  .res_id = 0x334d4dLU,
  .addr = 0x50090,
  .priority[0] = 0,
  .offset = SIZE(14),
  .len = SIZE(1),
 },
 { /* header for SN0 */
  .res_id = 0x304e53LU,
  .addr = 0x50078,
  .priority[0] = 2104,
  .offset = SIZE(15),
  .len = SIZE(1),
 },
 { /* header for SN1 */
  .res_id = 0x314e53LU,
  .addr = 0x50044,
  .priority[0] = 0,
  .offset = SIZE(16),
  .len = SIZE(1),
 },
 { /* header for SN2 */
  .res_id = 0x324e53LU,
  .addr = 0x50048,
  .priority[0] = 0,
  .offset = SIZE(17),
  .len = SIZE(1),
 },
 { /* header for SN3 */
  .res_id = 0x334e53LU,
  .addr = 0x5004c,
  .priority[0] = 0,
  .offset = SIZE(18),
  .len = SIZE(1),
 },
 { /* header for SN4 */
  .res_id = 0x344e53LU,
  .addr = 0x50050,
  .priority[0] = 0,
  .offset = SIZE(19),
  .len = SIZE(1),
 },
 { /* header for SN5 */
  .res_id = 0x354e53LU,
  .addr = 0x50054,
  .priority[0] = 0,
  .offset = SIZE(20),
  .len = SIZE(1),
 },
 { /* header for SN6 */
  .res_id = 0x364e53LU,
  .addr = 0x50058,
  .priority[0] = 0,
  .offset = SIZE(21),
  .len = SIZE(1),
 },
 { /* header for SN7 */
  .res_id = 0x374e53LU,
  .addr = 0x5005c,
  .priority[0] = 0,
  .offset = SIZE(22),
  .len = SIZE(1),
 },
 { /* header for SN8 */
  .res_id = 0x384e53LU,
  .addr = 0x50064,
  .priority[0] = 0,
  .offset = SIZE(23),
  .len = SIZE(1),
 },
 { /* header for SN9 */
  .res_id = 0x394e53LU,
  .addr = 0x50068,
  .priority[0] = 0,
  .offset = SIZE(24),
  .len = SIZE(1),
 },
 { /* header for SN10 */
  .res_id = 0x30314e53LU,
  .addr = 0x5006c,
  .priority[0] = 2048,
  .offset = SIZE(25),
  .len = SIZE(1),
 },
 { /* header for SN11 */
  .res_id = 0x31314e53LU,
  .addr = 0x50070,
  .priority[0] = 40,
  .offset = SIZE(26),
  .len = SIZE(1),
 },
 { /* header for SN12 */
  .res_id = 0x32314e53LU,
  .addr = 0x50074,
  .priority[0] = 0,
  .offset = SIZE(27),
  .len = SIZE(1),
 },
 { /* header for SN13 */
  .res_id = 0x33314e53LU,
  .addr = 0x5007c,
  .priority[0] = 16,
  .offset = SIZE(28),
  .len = SIZE(1),
 },
 { /* header for CE0 */
  .res_id = 0x304543LU,
  .addr = 0x500a0,
  .priority[0] = 0,
  .offset = SIZE(29),
  .len = SIZE(1),
 },
 { /* header for IP0 */
  .res_id = 0x305049LU,
  .addr = 0x500a4,
  .priority[0] = 0,
  .offset = SIZE(30),
  .len = SIZE(1),
 },
 { /* header for CN0 */
  .res_id = 0x304e43LU,
  .addr = 0x500a8,
  .priority[0] = 0,
  .offset = SIZE(31),
  .len = SIZE(1),
 },
 { /* header for QUP0 */
  .res_id = 0x30505551LU,
  .addr = 0x50080,
  .priority[0] = 0,
  .offset = SIZE(32),
  .len = SIZE(1),
 },
 { /* header for ACV */
  .res_id = 0x564341LU,
  .addr = 0x5000c,
  .priority[0] = 3900,
  .offset = SIZE(33),
  .len = SIZE(1),
 },
 { /* header for ALC */
  .res_id = 0x434c41LU,
  .addr = 0x50010,
  .priority[0] = 3900,
  .offset = SIZE(34),
  .len = SIZE(1),
 },
 { /* header for NU0 */
  .res_id = 0x30554eLU,
  .addr = 0x500dc,
  .priority[0] = 0,
  .offset = SIZE(35),
  .len = SIZE(1),
 },
};

const uint16_t bcm_db_headers_cnt = ARRAY_SIZE(bcm_db_headers);
const uint16_t bcm_aux_size = sizeof(bcm_aux);
const uint8_t *bcm_aux_ext = (uint8_t *)bcm_aux;
const cmd_db_entry_header *bcm_db_headers_ext = bcm_db_headers;

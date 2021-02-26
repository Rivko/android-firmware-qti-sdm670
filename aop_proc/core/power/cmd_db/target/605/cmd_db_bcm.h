#ifndef CMD_DB_BCM_H
#define CMD_DB_BCM_H
/*===========================================================================
                             cmd_db_bcm.h

DESCRIPTION:
  Export header for command DB BCM data.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

struct bcm_db{
 uint32_t bw_unit;
 uint16_t bcm_port;
 uint8_t clk_id;
};
#endif // CMD_DB_BCM_H

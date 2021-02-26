#ifndef CMD_DB_BCM_H
#define CMD_DB_BCM_H
/*
===========================================================================

FILE:         cmd_db_bcm.h

DESCRIPTION:  Structure definition for BCM AUX data

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/cmd_db_bcm.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2016/09/20   sds     Initial revision.

===========================================================================
             Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stdint.h>

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
struct bcm_db {
  uint32_t bw_unit;
  uint16_t bcm_port;
  uint8_t  clk_id;
};

#endif /* CMD_DB_BCM_H */

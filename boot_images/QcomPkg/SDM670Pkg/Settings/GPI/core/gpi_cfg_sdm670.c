/*
===========================================================================

FILE:         gpi_cfg_sdm670.c

DESCRIPTION:  This file implements the GPI Config Data.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     -------------------------------------------------------- 
01/31/17   ts      ported for UEFI.
10/13/16   ah      Initial revision for SDM670 GPI.

===========================================================================
             Copyright (c) 2016-2017 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved
                   QUALCOMM Proprietary
===========================================================================
*/

#include "gpitgtcfgdata.h"
#include "msmhwiobase.h"

#define GSI_0_BASE       (QUPV3_0_QUPV3_ID_1_BASE + 0x00004000)
#define GSI_1_BASE       (QUPV3_1_QUPV3_ID_1_BASE + 0x00004000)
#define GSI_2_BASE       (LPASS_BASE + 0x00604000)

const tgt_gpi_config_type  tgt_gpi_config[] =
{
   { TRUE,  0, GSI_0_BASE,   64 },
   { TRUE,  1, GSI_1_BASE,   65 },
   { FALSE, 2, GSI_2_BASE,   0  }
};

const tgt_gpii_config_type  tgt_gpii_config[] =
{
    {
      /* .qup_type   */ 0,   /* QUP HW instance type (SSC)            */
      /* .gpii_id    */ 0,   /* GPII ID                               */
      /* .protocol   */ 1,   /* Protocol running on this GPII         */
      /* .options    */ 0,   /* For future use. Options bitmask       */
      /* .ee         */ 3,   /* Local execution environment index     */
      /* .se         */ 0,   /* Serial Engine                         */
      /* .irq        */ 0    /* Interrupt for this GPII in this EE    */
   },
   {
      /* .qup_type   */ 0,
      /* .gpii_id    */ 2,
      /* .protocol   */ 1,
      /* .options    */ 0,
      /* .ee         */ 3,
      /* .se         */ 2,
      /* .irq        */ 0
   },
   {
      /* .qup_type   */ 0,
      /* .gpii_id    */ 10,
      /* .protocol   */ 2,
      /* .options    */ 0,
      /* .ee         */ 3,
      /* .se         */ 6,
      /* .irq        */ 0
   },
   {
      /* .qup_type   */ 0,
      /* .gpii_id    */ 11,
      /* .protocol   */ 2,
      /* .options    */ 0,
      /* .ee         */ 3,
      /* .se         */ 7,
      /* .irq        */ 0
    },
   {
     /* .qup_type   */ 1,
     /* .gpii_id    */ 0,
     /* .protocol   */ 1,
     /* .options    */ 0,
     /* .ee         */ 3,
     /* .se         */ 0,
     /* .irq        */ 0
  },
  {
     /* .qup_type   */ 1,
     /* .gpii_id    */ 2,
     /* .protocol   */ 2,
     /* .options    */ 0,
     /* .ee         */ 3,
     /* .se         */ 2,
     /* .irq        */ 0
  },
  {
     /* .qup_type   */ 1,
     /* .gpii_id    */ 10,
     /* .protocol   */ 2,
     /* .options    */ 0,
     /* .ee         */ 3,
     /* .se         */ 6,
     /* .irq        */ 0
  },
  {
     /* .qup_type   */ 1,
     /* .gpii_id    */ 11,
     /* .protocol   */ 2,
     /* .options    */ 0,
     /* .ee         */ 3,
     /* .se         */ 7,
     /* .irq        */ 0
   }
};


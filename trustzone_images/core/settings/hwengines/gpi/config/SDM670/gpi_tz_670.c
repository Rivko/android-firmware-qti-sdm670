/*
===========================================================================

FILE:         gpi_tz_670.c

DESCRIPTION:  This file implements the GPI Config Data.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     -------------------------------------------------------- 
07/24/17   pc      Updated EE for GPII configuration
02/22/17   ah      Modified GSI base definitions.
10/13/16   ah      Initial revision for Napali SSC.

===========================================================================
             Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
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
   { TRUE, 0, GSI_0_BASE, 0 },   /* QUP_0, GSI_0_REG_BASE */
   { TRUE, 1, GSI_1_BASE, 0 },   /* QUP_1, GSI_1_REG_BASE */
   { TRUE, 2, GSI_2_BASE, 0 }    /* QUP_SSC, GSI_REG_BASE */
};

const tgt_gpii_config_type  tgt_gpii_config[] =
{
    {
      /* .qup_type   */ 0,   /* QUP HW instance type (South)        */
      /* .gpii_id    */ 8,   /* GPII ID                             */
      /* .protocol   */ 1,   /* Protocol running on this GPII       */
      /* .options    */ 0,   /* For future use. Options bitmask     */
      /* .ee         */ 5,   /* Local execution environment index   */
      /* .se         */ 0,   /* Serial Engine                       */
      /* .irq        */ 284  /* Interrupt for this GPII in this EE  */
   },
   {
      /* .qup_type   */ 0,
      /* .gpii_id    */ 9,
      /* .protocol   */ 1,
      /* .options    */ 0,
      /* .ee         */ 5,
      /* .se         */ 2,
      /* .irq        */ 285
   },
   {
      /* .qup_type   */ 1,
      /* .gpii_id    */ 8,
      /* .protocol   */ 3,
      /* .options    */ 0,
      /* .ee         */ 5,
      /* .se         */ 1,
      /* .irq        */ 327
   },
   {
      /* .qup_type   */ 1,
      /* .gpii_id    */ 9,
      /* .protocol   */ 1,
      /* .options    */ 0,
      /* .ee         */ 5,
      /* .se         */ 7,
      /* .irq        */ 328
   },
   {
      /* .qup_type   */ 2,
      /* .gpii_id    */ 12,
      /* .protocol   */ 1,
      /* .options    */ 0,
      /* .ee         */ 5,
      /* .se         */ 2,
      /* .irq        */ 461
   }
};


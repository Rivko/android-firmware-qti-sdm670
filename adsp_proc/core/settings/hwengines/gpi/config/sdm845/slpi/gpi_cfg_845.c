/*
===========================================================================

FILE:         gpi_cfg_845.c

DESCRIPTION:  This file implements the GPI Config Data.

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     -------------------------------------------------------- 
02/22/17   ah      Changed SE# of GPII#0 from 2 to 0.
10/13/16   ah      Initial revision for Napali SSC.

===========================================================================
             Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved
                   QUALCOMM Proprietary
===========================================================================
*/

#include "gpitgtcfgdata.h"
#include "msmhwiobase.h"

#define GSI_2_BASE   (SSC_BASE + 0x00604000)

const tgt_gpi_config_type  tgt_gpi_config[] =
{
   { FALSE, 0, 0         , 0 },
   { FALSE, 1, 0         , 0 },
   { TRUE , 2, GSI_2_BASE, 0 }
};

const tgt_gpii_config_type  tgt_gpii_config[] =
{
    {
      /* .qup_type   */ 2,   /* QUP HW instance type (SSC)            */
      /* .gpii_id    */ 0,   /* GPII ID                               */
      /* .protocol   */ 3,   /* Protocol running on this GPII         */
      /* .options    */ 0,   /* For future use. Options bitmask       */
      /* .ee         */ 0,   /* Local execution environment index     */
      /* .se         */ 0,   /* Serial Engine                         */
      /* .irq        */ 12   /* Interrupt for this GPII in this EE    */
   },
   {
      /* .qup_type   */ 2,
      /* .gpii_id    */ 1,
      /* .protocol   */ 1,
      /* .options    */ 0,
      /* .ee         */ 1,
      /* .se         */ 1,
      /* .irq        */ 13
   },
   {
      /* .qup_type   */ 2,
      /* .gpii_id    */ 2,
      /* .protocol   */ 1,
      /* .options    */ 0,
      /* .ee         */ 2,
      /* .se         */ 2,
      /* .irq        */ 14
   },
   {
      /* .qup_type   */ 2,
      /* .gpii_id    */ 3,
      /* .protocol   */ 2,
      /* .options    */ 0,
      /* .ee         */ 3,
      /* .se         */ 3,
      /* .irq        */ 15
    },
    {
      /* .qup_type   */ 2,
     /* .gpii_id     */ 4,
     /* .protocol    */ 2,
     /* .options     */ 0,
      /* .ee         */ 4,
      /* .se         */ 4,
      /* .irq        */ 16
   },
   {
      /* .qup_type   */ 2,
      /* .gpii_id    */ 5,
      /* .protocol   */ 3,
      /* .options    */ 0,
      /* .ee         */ 5,
      /* .se         */ 5,
      /* .irq        */ 17
    }
};


#ifndef __VRM_CONFIG_TARGET_H__
#define __VRM_CONFIG_TARGET_H__

/*! \file vrm_config_target.h
*
*  \brief This header file contains internal VRM target specific config related definitions.
*  \n
*  &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/target/605/vrm_config_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/


//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_NUM_RSRC_TYPE     12
#define VRM_NUM_VREG          46
#define VRM_NUM_XOB           8
#define VRM_NUM_XO            1


typedef enum 
{
   VRM_IRQ_FIRST            = 75, // same as VOLTAGE_VOTE irq
   VRM_IRQ_VOLTAGE_VOTE     = VRM_IRQ_FIRST,
   VRM_IRQ_ENABLE_VOTE      = 76,
   VRM_IRQ_MODE_VOTE        = 77,
   VRM_IRQ_HEADROOM_VOTE    = 78,
   VRM_IRQ_VOLTAGE_CHANGE   = 79,
   VRM_IRQ_ENABLE_CHANGE    = 80,
   VRM_IRQ_MODE_CHANGE      = 81,
   VRM_IRQ_HEADROOM_CHANGE  = 82,
   VRM_IRQ_VOLTAGE_SETTLED  = 83,
   VRM_IRQ_ENABLE_SETTLED   = 84,
   VRM_IRQ_MODE_SETTLED     = 85,
   VRM_IRQ_HEADROOM_SETTLED = 86,
   VRM_IRQ_SEQUENCER        = 87,
   VRM_IRQ_ERROR            = 88,
   VRM_IRQ_LAST,
}vrm_irq_num;

#endif

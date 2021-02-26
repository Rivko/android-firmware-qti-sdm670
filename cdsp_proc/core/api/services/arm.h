#ifndef ARM_H
#define ARM_H
/*==========================================================================

                         ARM Definitions Header File

DESCRIPTION
  Macros and constants specific to the ARM processor

Copyright (c) 1997,1998 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 1999      by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/arm.h_v   1.0   Aug 21 2000 11:51:02   lpetrucc  $
$Header: //components/rel/core.qdsp6/2.1.c4/api/services/arm.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/98    ms     Added P_IRQ_VECT and P_FIQ_VECT for T_MSM3
11/02/98    ms     Added P_INT0_VECT and P_INT1_VECT declarations.
07/10/98   jct     Revised for coding standard, removed unused code
01/01/98   bwj     Created

===========================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

MACRO PRIMASK_DISABLE
MACRO PRIMASK_ENABLE

DESCRIPTION
  Uses the Priority Mask feature of the 80186 to turn interrupts off
  (PRIMASK_DISABLE) and restore them to the previous state (PRIMASK_ENABLE).
  This scheme isn't used anywhere else in the DMSS code, so there's no
  need to save and restore the interrupt priority mask state.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/* Interrupts must be level 0 or higher to be serviced.
** Since we don't have any interrupts at level 0, no interrupts
** will be serviced.
**
** N.B.! Make sure we do not have any interrupts at level 0 including
** timer ( defaults to 0 ) and DMA interrupts.
**
** Account for pipeline delay of priority mask register by disabling
** interrupts around the disable instruction to avoid interrupts
** ( and subsequent tasks ) from running for an unknown length of time
** with all interrupts disabled.
*/
#define PRIMASK_DISABLE()

/* Interrupts must be level 7 or higher to be serviced.
** Since level 7 is the lowest priority, all interrupts that
** are otherwise enabled will be serviced normally.
*/
#define PRIMASK_ENABLE()


/*===========================================================================

MACRO SET_VECT

DESCRIPTION
  SET_VECT Stores a specified ISR address at a specified interrupt vector.

PARAMETERS
  vect_num      Interrupt vector number, as defined in this file
  ptr           Pointer to the ISR for vect_num

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define  SET_VECT( vect_num, ptr )   rex_set_interrupt_vector(vect_num, ptr)
#define  ENABLE_INTERRUPT(vect_num)  rex_enable_interrupt(vect_num)
#define  DISABLE_INTERRUPT(vect_num) rex_disable_interrupt(vect_num)

/* -----------------------------------------------------------------------
**                  Peripheral Chip Select Registers 
** ----------------------------------------------------------------------- */

#define P_IRQ_VECT   0  /* IRQ   */
#define P_FIQ_VECT   1  /* FIQ   */

#define P_PCS_BASE   0
#define IO_BASE      0

#endif /* END ARM_H */

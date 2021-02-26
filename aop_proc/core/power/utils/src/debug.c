/*===========================================================================
                             debug.c

DESCRIPTION:
    This file implements the prwup and pwrdwn req ISRs as indicated by the SWAON HPG.
    AOP is required to write a couple CSR registers to prevent AOSS sleep when pwrup
    happens, and clear the CSR when pwrdwn happens

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "CoreVerify.h"
#include "rex.h"
#include "HALhwio.h"
#include "aop_hwio.h"
#include "msg_ram_layout.h"

//---------------------------------------------------------------------------
//  Internal Type Definitions
//---------------------------------------------------------------------------
#define AOSSPWRUPREQ 54
#define AOSSPWRDWNREQ 56
#define SW_WDOG_FREEZE   0x0fee0fee

//---------------------------------------------------------------------------
//  Variables
//---------------------------------------------------------------------------

//===========================================================================
//                       Function Defintions
//===========================================================================

extern void aop_sleep_wfi_config(boolean);

//---------------------------------------------------------------------------
//  Public Function Definition
//---------------------------------------------------------------------------

void aoss_pwr_dwn_isr (void)
{
  //debugger is dettached, set pwr hold to 0
  HWIO_OUTF(AOP_ALWAYS_ON_PWR_HOLD, ALWAYS_ON_PWR_HOLD, 0);

  //notify sleep layer to re-enable proc_clock_halt in wfi
  aop_sleep_wfi_config(FALSE);

  //re-enable sw wdog
  msg_ram_set_wdog_cookie(0);

  //enable pwrup and disable pwrdwn
  rex_enable_interrupt (AOSSPWRUPREQ);
  rex_disable_interrupt (AOSSPWRDWNREQ);
}

void aoss_pwr_up_isr (void)
{
  //debugger is attached, set pwr hold to 1	
  HWIO_OUTF(AOP_ALWAYS_ON_PWR_HOLD, ALWAYS_ON_PWR_HOLD, 1);

  //notify sleep layer to disable proc_clock_halt in wfi
  aop_sleep_wfi_config(TRUE);

  //disable sw wdog
  msg_ram_set_wdog_cookie(SW_WDOG_FREEZE);

  //enable pwrdwn and disable pwrup
  rex_enable_interrupt (AOSSPWRDWNREQ);
  rex_disable_interrupt (AOSSPWRUPREQ);
}

void debug_init()
{
  //only enable pwr up to start since by default the PWR_HOLD registers are 0	
  rex_set_interrupt_vector(AOSSPWRUPREQ, (rex_isr_func_type)aoss_pwr_up_isr);
  rex_clear_interrupt (AOSSPWRUPREQ);
  rex_enable_interrupt(AOSSPWRUPREQ);


  rex_set_interrupt_vector (AOSSPWRDWNREQ, (rex_isr_func_type)aoss_pwr_dwn_isr);
  rex_clear_interrupt (AOSSPWRDWNREQ);
}

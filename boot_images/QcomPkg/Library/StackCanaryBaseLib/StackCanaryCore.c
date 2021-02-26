/******************************************************************//**
 * @file StackCanryCore.c
 *
 * @brief StackCanryCore functions
 *
 * Copyright (c) 2016 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    --------------------------------------------------- 
07/06/16   vk      Initial version
========================================================================*/
#include <Library/RandomStackCanaryLib.h>
#include <Library/DebugLib.h>
#include <Library/SerialPortShLib.h>

void* __stack_chk_guard = (void*)0xc0c0c0c0;

VOID InitStackCanary (VOID);

/**
 * Initialize stack check canary
 */
VOID InitStackCanary (VOID)
{
#ifndef PRE_SIL
  __stack_chk_guard = (VOID*) GetRandomStackCanary();
#endif
}

 
/**
 * Callback if stack cananry is corrupted
 * */
void __stack_chk_fail (void)
{
  volatile UINT32 i = 1;
  /* Loop forever in case of stack overflow. Avoid
  calling into another api in case of stack corruption/overflow */
  DEBUG((EFI_D_ERROR,"__stack_chk_fail : Stack corruption detected \n"));
  SerialPortFlush();
  while (i);
}
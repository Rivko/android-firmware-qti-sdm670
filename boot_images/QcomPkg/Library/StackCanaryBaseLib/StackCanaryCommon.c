/******************************************************************//**
 * @file StackCanryCommon.c
 *
 * @brief StackCanryCommon functions
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

void* __stack_chk_guard = (void*) 0xc0c0c0c0;

/**
 * Callback if stack cananry is corrupted
 * */
void __stack_chk_fail (void)
{
  volatile UINT32 i = 1;
  /* Loop forever in case of stack overflow. Avoid
  calling into another api in case of stack corruption/overflow */ 
  while (i);
}


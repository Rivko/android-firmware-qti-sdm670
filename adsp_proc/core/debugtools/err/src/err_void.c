/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S   STUB

GENERAL DESCRIPTION
  This module provides ecompilation and linking stubs only

Copyright (c) 1992-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_void.c#1 $

08/10/11   mcg     NAV/GSS initial integration


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "err.h"
#include "err_qurt.h"

void bootstrap_exception_handler(void) { /* NULL */ }

void err_fatal_handler ( void ) ERR_NORETURN_ATTRIBUTE;

boolean err_crash_cb_dereg(err_cb_ptr cb) { return 0; };
boolean err_crash_cb_register(err_cb_ptr cb) { return 0; };

void err_init (void) { /* NULL */ };
//rr_stat_type err_clr_log (byte log_num) { return ERR_DONE_S; };
void err_put_log (word line, const char *file_ptr) { /* NULL */ };
//void err_disable_fs_log (void) { /* NULL */ };
//void err_fatal_jettison_core (unsigned int line, const char *file_name,
//  const char *format, uint32 param1, uint32 param2, uint32 param3)
//{
//  err_fatal_handler();
//};
void err_fatal_lock( void ) { /* NULL */ };
void jettison_core( void )
{
  err_fatal_handler();
};
void err_SaveFatal3 (const err_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
  err_fatal_handler();
};
//void err_SaveFatal2 (const err_const_type* const_blk, uint32 code1, uint32 code2) { /* NULL */ };
//void err_SaveFatal1 (const err_const_type* const_blk, uint32 code1) { /* NULL */ };
void err_SaveFatal0 (const err_const_type* const_blk)
{
  err_fatal_handler();
};
//void err_Fatal3 (const err_FatalParam_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
//{
//  err_fatal_handler();
//};
//void err_Fatal2 (const err_FatalParam_type* const_blk, uint32 code1, uint32 code2) { /* NULL */ };
//void err_Fatal1 (const err_FatalParam_type* const_blk, uint32 code1) { /* NULL */ };
//void err_Fatal0 (const err_FatalParam_type* const_blk) { /* NULL */ };
//void err_set_auto_action(err_action_type action) { /* NULL */ };
void err_modem_reset( void ) { /* NULL */ };
//ERR_DEPRECATED_ATTRIBUTE int err_add_to_mem_table (char *name, void *ptr, unsigned long length);
//ERR_DEPRECATED_ATTRIBUTE int err_delete_from_mem_table (void *ptr);
void err_store_info_to_flash (void) { /* NULL */ };

void err_fatal_handler ( void )
{

  ERR_FATAL_ENTER_SINGLE_THREADED_MODE();
  ERR_FATAL_FLUSH_CACHE_NO_RETURN();

  //loop just in case it returned
  while(1)
  {
    //no api to kick dog on ADSP/DSPS
  }

};


#include "err_reset_detect.h"
void err_reset_detect_enable(void) { /* NULL */ };
void err_reset_detect_disable(void) { /* NULL */ };
err_reset_type err_reset_detect_status(void) { return ERR_RESET_NONE; };
void err_reset_detect_set(err_reset_type state) { /* NULL */ };
boolean err_reset_detect_init(void) { return 0; };
boolean err_reset_detect_boot_alloc(void) { return 0; };
int err_reset_log_init(char *register_log, int sizeof_register_log) { return 0; };
void err_reset_log_finalize(void) { /* NULL */ };

//externed functions
//void err_exception_handler(void) {rex_wait(1);};
void err_exception_handler(void) {while(1) qurt_timer_sleep(1000000);}

/*=========================================================================

FUNCTION err_crash_cb_reg_next_to_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called immediately after
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback,
  this API was provided only for special case handling to stop ULT Audio Core
  in DPM PL
===========================================================================*/
boolean err_crash_cb_reg_next_to_STM(err_cb_ptr cb)
{
  return TRUE;
}

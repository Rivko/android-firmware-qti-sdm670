/*===========================================================================
                              rpmh_image_os.c

DESCRIPTION: 
  Stubbed/offtarget OS functionality. One version of this file will exist for
  each supported image. Functionality includes locking, logging, verification 
  (core_verify), fatal errors, and interrupt management.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
#include "comdef.h"
#include "rpmh_os.h"

#ifdef RPMH_OFFTARGET_TEST
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

#define PRINT_TO_FILE 1
#define PRINT_TO_CONSOLE 0

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------
static uint32 num_core_verify_failures = 0;
#endif //RPMH_OFFTARGET_TEST

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------

/* OS related objects stored in a single structure for debug ease */
rpmh_os_t rpmh_os;

FILE *rpmh_external_log;	

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

/**
 * <!-- rpmh_os_init -->
 *
 * @brief Call before using any other rpm os API 
 */
void rpmh_os_init ()
{
#if PRINT_TO_FILE	
  rpmh_external_log = fopen("rpmh_driver_log.txt", "w");	
#endif //PRINT_TO_FILE
}

/**
 * <!-- rpmh_lock -->
 *
 * @brief Lock the global RPMh driver lock  
 */
void rpmh_lock (void)
{
}

/**
 * <!-- rpmh_unlock -->
 *
 * @brief Release the global RPMh driver lock  
 */
void rpmh_unlock (void)
{
}

/**
 * <!-- rpmh_get_core_verify_failures -->
 * @brief Used to track the # of core_verify failures for offtarget testing.  
 *
 * @return The # of core_verify failures so far in this run 
 */
#ifdef RPMH_OFFTARGET_TEST
uint32 get_core_verify_failures (void)
{
  return num_core_verify_failures;
}
#endif //RPMH_OFFTARGET_TEST

/**
 * <!-- rpmh_core_verify -->
 *
 * @brief Verify the given value is TRUE. Force a crash if FALSE  
 *
 * @param val : value to verify
 */
void rpmh_core_verify(boolean val)
{
#ifdef RPMH_OFFTARGET_TEST
  //printf ("\trpmh_core_verify: %d\n", val);      
  if (!val)
  {	    
#if PRINT_TO_FILE	
    fprintf (rpmh_external_log, "\t!!!!!!!!!!CORE_VERIFY_FAIL!!!!!!!!!!\n");      
#endif //PRINT_TO_FILE
    printf ("\t!!!!!!!!!!CORE_VERIFY_FAIL!!!!!!!!!!\n");      
    num_core_verify_failures += 1;
    exit (1);
  }	
#endif //RPMH_OFFTARGET_TEST
}

/**
 * <!-- rpmh_core_verify_ptr -->
 *
 * @brief Verify the given pointer is valid. Force a crash if NULL  
 *
 * @param val : pointer to verify
 */
void rpmh_core_verify_ptr (void *ptr)
{
#ifdef RPMH_OFFTARGET_TEST
  //printf("\trpmh_core_verify_ptr: 0x%08x\n", ptr);      
  //if(ptr)	
  //{
  //    printf("\trpmh_core_verify_ptr_val: 0x%08x\n", (*((uint32*)ptr)));      
  //}
  //
  if (!ptr)
  {	    
#if PRINT_TO_FILE	
      fprintf (rpmh_external_log, "\t!!!!!!!!!!CORE_VERIFY_PTR_FAIL!!!!!!!!!!\n");      
#endif //PRINT_TO_FILE
      printf ("\t!!!!!!!!!!CORE_VERIFY_PTR_FAIL!!!!!!!!!!\n");      
      num_core_verify_failures += 1;
      exit(1);
  }	
#endif //RPMH_OFFTARGET_TEST
}

/**
 * <!-- rpmh_log_4 -->
 *
 * @brief Add an RPM log entry with the given string and 4 values  
 *
 * @param string : log message
 * @param val : param 1
 * @param val2 : param 2
 * @param val3 : param 3
 * @param val4 : param 4
 */
void rpmh_log_4 (char *string, uint32 val, uint32 val2, uint32 val3, uint32 val4)
{
#ifdef RPMH_OFFTARGET_TEST
#if PRINT_TO_FILE	
  fprintf(rpmh_external_log, string, val, val2, val3, val4);    
#endif //PRINT_TO_FILE
#if PRINT_TO_CONSOLE
  printf(string, val, val2, val3, val4);    
#endif //PRINT_TO_FILE/CONSOLE
#endif //RPMH_OFFTARGET_TEST
}	

/**
 * <!-- rpmh_log_3 -->
 *
 * @brief Add an RPM log entry with the given string and 3 values  
 *
 * @param string : log message
 * @param val : param 1
 * @param val2 : param 2
 * @param val3 : param 3
 */
void rpmh_log_3 (char *string, uint32 val, uint32 val2, uint32 val3)
{
#ifdef RPMH_OFFTARGET_TEST
#if PRINT_TO_FILE	
  fprintf(rpmh_external_log, string, val, val2, val3);    
#endif //PRINT_TO_FILE
#if PRINT_TO_CONSOLE
  printf(string, val, val2, val3);    
#endif //PRINT_TO_CONSOLE
#endif //RPMH_OFFTARGET_TEST
}	

/**
 * <!-- rpmh_log_2 -->
 *
 * @brief Add an RPM log entry with the given string and 2 values  
 *
 * @param string : log message
 * @param val : param 1
 * @param val2 : param 2
 */
void rpmh_log_2 (char *string, uint32 val, uint32 val2)
{
#ifdef RPMH_OFFTARGET_TEST
#if PRINT_TO_FILE	
  fprintf(rpmh_external_log, string, val, val2);    
#endif //PRINT_TO_FILE
#if PRINT_TO_CONSOLE
  printf(string, val, val2);    
#endif //PRINT_TO_CONSOLE
#endif //RPMH_OFFTARGET_TEST
}	

/**
 * <!-- rpmh_log_1 -->
 *
 * @brief Add an RPM log entry with the given string and 1 values  
 *
 * @param string : log message
 * @param val : param 1
 */
void rpmh_log_1 (char *string, uint32 val)
{
#ifdef RPMH_OFFTARGET_TEST
#if PRINT_TO_FILE	
  fprintf(rpmh_external_log, string, val);    
#endif //PRINT_TO_FILE
#if PRINT_TO_CONSOLE
  printf(string, val);    
#endif //PRINT_TO_CONSOLE
#endif //RPMH_OFFTARGET_TEST
}	

/**
 * <!-- rpmh_log_0 -->
 *
 * @brief Add an RPM log entry with the given string  
 *
 * @param string : log message
 */
void rpmh_log_0 (char *string)
{
#ifdef RPMH_OFFTARGET_TEST
#if PRINT_TO_FILE	
  fprintf(rpmh_external_log, string);    
#endif //PRINT_TO_FILE
#if PRINT_TO_CONSOLE
  printf(string);    
#endif //PRINT_TO_CONSOLE

#endif //RPMH_OFFTARGET_TEST
}	

void rpmh_register_isr (uint32 irq, rpmh_cb_fn isr, void* cb_data)
{

}

/**
 * <!-- rpmh_err_fatal -->
 *
 * @brief Unrecoverable error, force a crash  
 */
void rpmh_err_fatal (void *val)
{
  rpmh_log_0("\n!!!!RPMH_ERR_FATAL!!!!\n");
  rpmh_core_verify(FALSE);
}	

/**
 * <!-- rpmh_clz -->
 *
 * @brief count leading zeros implementation  
 *
 * @param num : Num to calculat clz on
 *
 * @return the number of leading zeros
 */
uint32 rpmh_clz (uint32 num)
{
  int i;
  for (i = 0; i < 32; i++)
  {
      if ((num & 0x80000000) != 0)
      {
          return i;
      }
      num <<= 1;
  }
  return 32;
}

/**
 * <!-- rpmh_get_timestamp -->
 *
 * @brief Returns a 64 bit current timestamp   
 */
uint64 rpmh_get_timestamp (void)
{
  uint64 timer_val = 0;
  return timer_val;
}




#ifndef _PDC_TARGET_H_
#define _PDC_TARGET_H_
/*==============================================================================
  FILE:         pdc_target.h
  
  OVERVIEW:     This file contains target specific definitions for the PDC interrupt
                Controller.
 
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/src/pdc_target.h#1 $
$DateTime: 2017/07/21 22:10:47 $
=============================================================================*/ 
#include "comdef.h"

/*=========================================================================
 *                     INTERNAL OS SPECIFIC DEFINITIONS
 *=========================================================================*/
/* The macros defined below are usually OS depedent. New definitions must be
 * added as support for new OS comes or it will result into default value
 * that may not work as expected on all OSes. */
#if 0

  /* Add OS specific defintions here */
  #define PDC_LOCK_TYPE    void*
  #define PDC_LOCK()
  #define PDC_UNLOCK()
  
#else
  /* Default definitions */
  #include "CoreMutex.h"
  #include "CoreVerify.h"
  
  /* Name of the mapping data lock variable to prevent concurent access */  
  #define PDC_LOCK_NAME       g_pdcIntLock

  /* Type of the lock variable */
  #define PDC_LOCK_TYPE       CoreMutex*

  /* External reference */
  extern PDC_LOCK_TYPE        PDC_LOCK_NAME;

  /* Lock access macros */
  #define PDC_LOCK_INIT()     PDC_LOCK_NAME = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT); \
                              CORE_VERIFY_PTR(g_pdcIntLock)

  #define PDC_LOCK()          if(NULL != PDC_LOCK_NAME) {Core_MutexLock(PDC_LOCK_NAME);}
  #define PDC_UNLOCK()        if(NULL != PDC_LOCK_NAME) {Core_MutexUnlock(PDC_LOCK_NAME);}

#endif

/*===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================*/
/** 
 * pdcTarget_initialize 
 *  
 * @brief Initializes all target specific data for common driver to use. 
 *        Depending on the OS, this function can query DevCfg APIs to obtain
 *        and then intialize target specific data or can have direct assignment.
 *
 * @return none
 */
void pdcTarget_initialize( void );

#endif  /* _PDC_TARGET_H_ */


#ifndef _ELITE_MAIN_H_
#define _ELITE_MAIN_H_

/**
@file EliteMain.h

@brief This file provides an API wrapper for high level elite frame work functions
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
   Copyright (c) 2010, 2013-2015 Qualcomm Technologies, Inc.  All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/05/14   sw      (Tech Pubs) Merged Doxygen markup from 2.4.
03/20/13   sw      (Tech Pubs) Edited Doxygen markup/comments for 2.0.
02/04/10   mspk      Created file.
==========================================================================*/


/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/** @ingroup elitemain_func_init
  Initializes the Elite framework.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_init(void);

/** @ingroup elitemain_func_deinit
  De-initializes the Elite framework.

  @return
  Success or failure code.

  @dependencies
  None. @newpage
*/
ADSPResult elite_framework_deinit(void);


/** @ingroup elitemain_func_start
  Starts the Elite framework, including static services.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_start(void);

/** @ingroup elitemain_func_stop
  Stops the Elite framework, including static services.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_stop(void);


/** @ingroup elitemain_func_tests
  Performs the framework unit test cases and also tests the framework test
  case.

  @return
  Success or failure code.

  @dependencies
  None.
*/
ADSPResult elite_framework_unit_tsts(void);

/* no comment required */
int elite_framework_sim_execution(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_ELITE_MAIN_H_
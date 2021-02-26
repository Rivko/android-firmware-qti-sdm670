/*==============================================================================

FILE: DalTF_Bootstrap.h

DESCRIPTION: 

  Declaration and implementation of the DalTF Bootstrapper (BSL) API.
  The API selects and runs tests dumped into memory by the Trace32 debugger.
  The results are then collected in a result buffer and passed to the client
 
INITIALIZATION AND SEQUENCING REQUIREMENTS: 
    The execution sequence is as follows.
    DalTF_BSL_Init is called, initializing a test buffer structure.
    DalTF_BSL_Load_Tests is called, loading the tests using the T32
    DalTF_BSL_Start_Tests is called beginning the test run sequence
    with function declaration in DalTFi_BSL.h.

PUBLIC CLASSES: None
IALIZATION AND SEQUENCING REQUIREMENTS: N/A

      Copyright (c) 2012 Qualcomm Technologies Incorporated.
              All Rights Reserved.
          QUALCOMM Proprietary/GTDR
==============================================================================*/

#ifndef _DALTF_BOOTSTRAP
#define _DALTF_BOOTSTRAP

#include "DDITF.h"
#include "DALSys.h"



/**
 * @brief Start executing the test cases
 *
 * @param [in] BSL_DalTF_handle   valid Dal handle
 * @param [in] DalTF_BSL_Test_Buf populated BSL buffer, containg the test list
 */

DALResult 
DalTF_BSL_Start_Tests(DalTFHandle* BSL_DalTF_handle, TF_BSL_Buffer* bsl_running_tests);
/**
 * @brief Prepares the buffer to which to load the list of test cases.
 * This operation of this function is heavily manipulated by the T32 script
 *
 * Detailed Operation
 * Upon entering, this function, the T32 script will calculate a size for
 * @param [in] DalTF_BSL_TestBufferSize and update it out of context.
 * The script will then step over the malloc and check its return,
 *
 * If the malloc was successful, the script will load the list of tests at the address
 * of the malloc'ed block and update TF_BSL_Buffer->bIsFull flag, 
 * and resume normal flow.
 */
DALResult
DalTF_BSL_Load_Tests(DalTFHandle* BSL_DalTF_handle,TF_BSL_Buffer* DalTF_BSL_Buf);

/**
 * @brief Allocate some memory for the DalTF test buffer
 *
 * 
 * @param [in]   BSL_DalTF_handle    valid Dal handle.
 * @param [out]  DalTF_BSL_Test_Buf  allocated memory for a TF_BSL_Buffer
 */
DALResult
DalTF_BSL_Init(DalTFHandle* BSL_DalTF_handle,TF_BSL_Buffer** DalTF_BSL_Test_Buf);



#endif

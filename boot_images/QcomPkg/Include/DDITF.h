
/*==============================================================================
 
FILE: DDITF.h
 
DESCRIPTION: This file exposes a dalified testing framework's backend code         
 
PUBLIC CLASSES:  DalTFHandle
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
      Copyright (c) 2009 - 2011 Qualcomm Technologies Incorporated.
      Copyright (c) 2012 - 2017 QUALCOMM Technologies Inc.
              All Rights Reserved.
       Qualcomm Technologies Proprietary and Confidential.
==============================================================================*/

#ifndef __DDITF_H__
#define __DDITF_H__

#define DALTF_INTERFACE_VERSION DALINTERFACE_VERSION(1,2)

#include<DalDevice.h>
#include<DALStdErr.h>
#include<DALStdDef.h>
#include<DALDeviceId.h>

// Configurable Properties
#define MIN_TEST_PERIOD 50
#define MAX_TEST_NAME_LENGTH 32
#define MAX_TEST_NUM 200
#define MAX_BUF_SIZE 2048

// Parameter Types
#define TF_PARAM_IDENTIFIER_SIZE 2  /**< -- # of bytes in each TF_PARAM_*              */
#define TF_PARAM_UINT8  "0"         /**< -- Specifies that the parameter is a uint8    */
#define TF_PARAM_UINT16 "1"         /**< -- Specifies that the parameter is a uint16   */
#define TF_PARAM_UINT32 "2"         /**< -- Specifies that the parameter is a uint32   */
#define TF_PARAM_INT8   "3"         /**< -- Specifies that the parameter is a byte     */
#define TF_PARAM_INT16  "4"         /**< -- Specifies that the parameter is a int16    */
#define TF_PARAM_INT32  "5"         /**< -- Specifies that the parameter is a int32    */
#define TF_PARAM_STRING "6"         /**< -- Specifies that the parameter is a string   */

/** @brief Testing Framework User (APTTESTS) Response Codes
   @detail
   These are the values that should be returned from test cases.
 */
typedef enum
{
   TF_RESULT_CODE_SUCCESS     = 0,     /**< -- if test case runs successfully          */
   TF_RESULT_CODE_FAILURE     = 1,     /**< -- if test case run unsuccessfully         */
   TF_RESULT_CODE_BAD_PARAM   = 2      /**< -- if test case's parameters are invalid   */
} TF_TestCaseResponseCodes;

/** @brief Testing Framework API return Codes
   @detail
   These are the values that should be returned from testframework internal apis only.
 */
typedef enum
{
   TF_SUCCESS                 = 0, 		/**< -- api successful     */
   TF_FAILURE				  = 0xFFFF, /**< -- api failure     */
   TF_ERROR_GENERIC           = 0x8001, /**< -- undefined error       */
   TF_ERROR_TEST_NONEXIST 	  = 0x8002, /**< -- test case not found               */
   TF_ERROR_ALLOC             = 0x8003, /**< -- malloc operation failed            */
   TF_ERROR_BAD_PARAM         = 0x8004, /**< -- bad arg was passed in  */
   TF_ERROR_MEM_FULL          = 0x8005, /**< -- memory full  */
   TF_ERROR_SYNC			  = 0x8006, /**< -- sync error need reset driver context  */
   TF_ERROR_CMD_UNKNOWN       = 0x8007
}TF_Result;

/** @brief Test Case Function Pointer Type -- Version 0
    @detail
    Function prototype of test case entry points; no return buffer.
 */
typedef uint32 (*TF_TCFunctionPtr)(uint32 dwParam, char *apszParam[]);

/** @brief Testing Framwork Parameter Descriptions
    @detail
    Description of a test case parameter's type, name and help/description.
 */
typedef struct 
{
   char *pszType;         /**< -- parameter's type. Values are of type TF_PARAM        */ 
   char *pszName;         /**< -- name of the parameter. Nullable                      */ 
   char *pszHelpString;   /**< -- General help info about this parameter. Nullable     */
} TF_ParamDescr;

/** @brief Testing Framework Test Context description
    @detail
    Used to describe a test case's context: what the test case is being used on
 */
typedef struct
{
   uint8 cSubsysId;            /**< -- Subsystem id of the test case                   */
   uint16 wContextId;          /**< -- Context id of the test case                     */
} TF_ContextDescr;

/** @brief Help Description
   @detail 
   Contains information about a specific test case. 
   Parameter information does not need to be included.
 */
typedef struct 
{
   const char *pszFunctionDescr;  /**< --  Test case description. Should not be NULL    */
   uint32 dwParametersLength;      /**< --  Number of parameters expected.   Can be zero */
   const TF_ParamDescr *pParameters;/**< --  Parameter descriptions. Can be NULL         */
} TF_HelpDescr;

/** @brief Testing Framework Test Function Description
    @detail
    Describes everything about a test case needed to run it. The testing table is
    an array of these elements. Each one of these is used to register a single
    test case into the Test Framework.
 */
typedef struct 
{
   const char *pszTestName;        /**< --  Unique name for the test case. Nonnullable  */
   TF_TCFunctionPtr pTestFunc;     /**< --  Pointer to test entry point. Nonnullable    */
   const TF_HelpDescr *pHelp;      /**< --  Information about test case. Nullable       */
   const TF_ContextDescr *pContext;/**< --  Context the tc desiged for. Nullable        */
   uint32 dwTestFuncType;          /**< --  Determines version for pTestFunc member;
                                            must be listed as at least the 5th member
                                            of this struct so it gets initialized to
                                            0 in old test case code.                    */
} TF_TestFunction_Descr;


typedef struct 
{
   const char *pszName;        				/**< --  Unique name for the test list  */
   const TF_TestFunction_Descr** TestList;        /**< --  array of the test list        **/
   
} TF_TestList_Descr;


typedef struct
{
   struct DalDevice DalDevice;
   //daltf class DDIs
   DALResult (*AddTests)(DalDeviceHandle *, const TF_TestFunction_Descr*[], uint32);
   DALResult (*RemoveTests)(DalDeviceHandle *, const TF_TestFunction_Descr*[], uint32);
   DALResult (*HandleCommand)(DalDeviceHandle *, const char*);
}DalTF;

//this is the DalDeviceHandle
typedef struct
{
   uint32 dwDalHandleId;
   const DalTF * pVtbl;
   void * pClientCtxt; 
}DalTFHandle;

/* ============================================================================
**             Initialization APIs
** ========================================================================== */
/*!
   @addtogroup testframework_dal_intialization_api
*/

/* ============================================================================
**  Function : DALTF_DeviceAttach
** ========================================================================== */
/*!
   @brief
   Establish communication link with the testing framework

   @param DevID         - [IN] DalTF's Device Id
   @param hDalDevice    - [OUT] Contains DalTF Handle, 
                                if return code is DAL_SUCCESS

   @par Dependencies
   None

   @par Side Effects
   None

   @return DAL_SUCCESS when the device attaches successfully, DAL_ERROR otherwise
*/
#define DAL_TFDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTF_INTERFACE_VERSION,hDalDevice)
		/* END DALTF_DeviceAttach */

/* ============================================================================
**  Function : DalTF_AddTests
** ========================================================================== */
/*!
   @brief
   Adds a number of test cases to the testing
   framework so they can be called from diag

   @param _h          - [IN] Handle to the DalTF logical device
   @param apTests     - [IN] test case descriptions to be added to DalTF
   @param dwToAdd     - [IN] The number of elements in the apTestsToAdd array

   @par Dependencies
   None

   @par Side Effects
   None

   @return DAL_SUCCESS if all test cases could be added, DAL_ERROR otherwise

*/
static __inline DALResult
DalTF_AddTests(DalDeviceHandle* _h,
            const TF_TestFunction_Descr* apTests[], uint32 dwToAdd)
{
   return ((DalTFHandle *)_h)->pVtbl->AddTests( _h, apTests, dwToAdd);
}

/* ============================================================================
**  Function : DalTF_RemoveTests
** ========================================================================== */
/*!
   @brief
   Finds and removes pointers to test cases in the test case table.

   @param _h                - [IN] Handle to the DalTF logical device
   @param apTests     		- [IN] Test cases to be removed from DalTF
   @param dwTestsToRemove   - [IN] Number of elements in the apTestsToAdd array

   @par Dependencies
   None

   @par Side Effects
   None

   @return DAL_SUCCESS if all test cases could be removed, DAL_ERROR otherwise

*/
static __inline DALResult
DalTF_RemoveTests(DalDeviceHandle * _h, const TF_TestFunction_Descr* apTests[],
              uint32  dwTestsToRemove)
{
   return ((DalTFHandle *)_h)->pVtbl->RemoveTests( _h, apTests, dwTestsToRemove);
}


/* ============================================================================
**  Function : DalTF_HandleCommand
** ========================================================================== */
/*!
   @brief
   process commands: LIST, STATUS, RUN, HELP, MESSAGE

   @param _h          - [IN] Handle to the DalTF logical device
   @param request     - [IN] command to be run

   @par Dependencies
   None

   @par Side Effects
   None

   @return DAL_SUCCESS test commands has been executed without error, DAL_ERROR otherwise

*/
static __inline DALResult
DalTF_HandleCommand(DalDeviceHandle* _h, const char* request)
{
   return ((DalTFHandle *)_h)->pVtbl->HandleCommand( _h, request);
}


//void Daltf_ClientDeInit(void);

//void DalTF_ClientInit(DalDeviceHandle** h);

#endif /*__DDITF_H__*/

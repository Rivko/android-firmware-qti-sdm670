#ifndef QMP_MSG_DRIV_H
#define QMP_MSG_DRIV_H
/*===========================================================================
                             qmp_msg_driver.h

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the structs, macros, etc. defined in
  this file.

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <string.h>
#include "json_parser.h"
#include "glink.h"


//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------


#define INIT_MAP_SIZE 10
#define NUM_OUTSTANDING_MSG 5
#define MAX_CLASS_VALUE_LEN 10
#define MAX_MSG_LEN         0x64

#ifndef SDX24_STUBS
#define NUM_REMOTES 5
#else
#define NUM_REMOTES 2
#endif

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef enum SUBSYSTEMS_SUPPORTED
{
    _APPS=0,
    _MPSS,
    _SLPI,
    _CDSP,
    _ADSP,
    NUM_OF_SUBSYSTEMS
}EDEGES;

typedef enum
{
    MSG_ALLOC_ERROR=-2,
    MSG_GENERAL_ERROR=-1,
    MSG_SUCESS,
}msgResult;

// callback of the type void()(char*)
typedef void (*cb_func_type)(char *, EDEGES);


//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

/** 
 * <!-- qmp_msg_init() -->
 *
 * @brief Initializes msg driver
 *
 *  This is used to notify the resource that the work request is
 *  complete and the client no longer needs the resource
 *
 *  Any request state that has been learned will persist
 *  
 *  Resource state should be updated to account for completed work
 *
 *  @param void : void
 *
 *  @return void
 */
void qmp_msg_init(void);


/** 
 * <!-- qmp_msg_register_cb() -->
 *
 * @brief Register a callback 
 *
 *  Registers a callback of the form void ()(char*) against a
 *  null terminated class value string.
 *  The registered cb will persist until it is unregistered
 * 
 *
 *  @param cb : callback function ptr void (*cb)(char*)
 *  @param class_string : value of the class in the passed
 *                      message
 *
 *  @return status result
 */
msgResult qmp_msg_register_cb(cb_func_type cb, char* class_string);




/** 
 * <!-- service_msg_queue() -->
 *
 * @brief Service the pending msgs in the workaround Queue by
 *        calling the callback functions
 *
 * @param void
 *
 * @return void
 */
void service_msg_queue(void);




#endif // QMP_MSG_DRIV_H

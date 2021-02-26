#ifndef DALSTDERR_H
#define DALSTDERR_H
/*
=======================================================================
        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
=======================================================================
*/

//
// Basic Error Codes
//
//

#define  DAL_SUCCESS                       0  // No error
#define  DAL_ERROR                        -1  // General failure
#define  DAL_WORK_LOOP_EXIT                1  // when work loop wants to exit
#define  DAL_ERROR_TIMEOUT                 2
#define  DAL_ERROR_OUT_OF_RANGE_PARAMETER -100

//Interrupt Controller Interrupt Status
#define  DAL_INTERRUPT_SET                0  // Interrupt is set

// Async Request Outcomes
// Values 0-0x7FFFFFFF are used for protocol specific outcomes
#define  DAL_ASYNC_REQ_INVALID            0x80000000
#define  DAL_ASYNC_REQ_ACCEPTED           0x80000001
#define  DAL_ASYNC_REQ_BUSY               0x80000002
#define  DAL_ASYNC_REQ_ACCEPTED_BUSY      0x80000003
#define  DAL_ASYNC_REQ_IN_PROGRESS        0x80000004
#define  DAL_ERROR_PORT_CONN_BROKEN       0x80000005
#define  DAL_ERROR_DEVICE_NOT_FOUND       0x80000006
#define  DAL_ERROR_DDI_VERSION            0x80000007


/*
============================================================================
   ERRORS DOCUMENTATION
==============================================================================
Basic DAL Error Codes

This section lists the set of basic errors returned, the codes associated
with the errors, and descriptions of the errors.


Basic error codes

Definition:

Error                Code   Description

DAL_SUCCESS             0   Operation Successful
==================================================================
*/
#endif /* #ifndef DALSTDERR_H */


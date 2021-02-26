#ifndef DALSTDERR_H
#define DALSTDERR_H
/*
=======================================================================
               
  Copyright (c) 2008, 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
=======================================================================
*/

/*-----------------------------------------------------------------------
** Defined Values for DALResult
**-----------------------------------------------------------------------*/

#define  DAL_SUCCESS                      0  // No error

// all standard DAL error codes are negative and have unique values
#define  DAL_ERROR                        -1  // General failure
#define  DAL_ERROR_TIMEOUT                -2
#define  DAL_ERROR_INTERNAL               -3
#define  DAL_ERROR_INVALID_HANDLE         -20
#define  DAL_ERROR_INVALID_POINTER        -21
#define  DAL_ERROR_INVALID_ADDRESS        -22
#define  DAL_ERROR_INVALID_DATA           -23
#define  DAL_ERROR_INVALID_PARAMETER      -24
#define  DAL_ERROR_INVALID_TYPE           -25
#define  DAL_ERROR_NOT_INITIALIZED        -40
#define  DAL_ERROR_NOT_FOUND              -41
#define  DAL_ERROR_NOT_SUPPORTED          -42
#define  DAL_ERROR_NOT_ALLOWED            -43
#define  DAL_ERROR_INSUFFICIENT_MEMORY    -60
#define  DAL_ERROR_DEVICE_ACCESS_DENIED   -80
#define  DAL_ERROR_PORT_CONN_BROKEN       -81
#define  DAL_ERROR_DEVICE_NOT_FOUND       -82
#define  DAL_ERROR_DDI_VERSION            -83
#define  DAL_ERROR_OUT_OF_RANGE_PARAMETER -100
#define  DAL_ERROR_BUSY_RESOURCE          -120
#define  DAL_ERROR_PROP_ACCESS_DENIED     -130
#define  DAL_ERROR_PROP_NOT_FOUND         -131

// driver error codes are interface specific and
// codes from different interfaces may have the same value
// driver error codes must be equal or less than the offset below
#define  DAL_DRIVER_ERROR_OFFSET          -1000

// all non-standard DAL result codes are positive, defined by the 
// interfaces that use them, and may share same values across different 
// contexts
#define  DAL_WORK_LOOP_EXIT               1  // workloop termination
#define  DAL_INTERRUPT_SET                2  // Interrupt is set
#define  DAL_SYNC_ENTER_FAILED            3

// evaluate if the result is a standard DAL error
#define  DAL_ERROR_RETURNED(result)       ((result) < 0)
// evaluate if the result is not a standard DAL error
#define  DAL_NO_ERROR_RETURNED(result)    (!DAL_ERROR_RETURNED(result))

// Async Request Outcomes
// Values 0-0x7FFFFFFF are used for protocol specific outcomes
#define  DAL_ASYNC_REQ_INVALID            0x80000000
#define  DAL_ASYNC_REQ_ACCEPTED           0x80000001
#define  DAL_ASYNC_REQ_BUSY               0x80000002
#define  DAL_ASYNC_REQ_ACCEPTED_BUSY      0x80000003
#define  DAL_ASYNC_REQ_IN_PROGRESS        0x80000004
#endif /* #ifndef DALSTDERR_H */


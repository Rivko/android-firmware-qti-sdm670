/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2012-2016,2017 QUALCOMM Technologies, Incorporated.         *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   listen_result.h				    TYPE: C-header file          *]
[* DESCRIPTION: Listen result code						                     *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   2012-08-30 acho    Initial revision                                     *]
[*****************************************************************************/

#ifndef _LISTEN_RESULT_H_
#define _LISTEN_RESULT_H_

// Listen result value definition
typedef enum 
{ 
	// result
	LISTEN_SUCCESS=0,	
	LISTEN_FAIL,
	LISTEN_NEEDMORE,
	LISTEN_NOTPREPARED,	
	// errors
	LISTEN_NULLREF,
	LISTEN_WRONGSIZE,
	LISTEN_WRONGINDEX,

} ListenResult;

#endif
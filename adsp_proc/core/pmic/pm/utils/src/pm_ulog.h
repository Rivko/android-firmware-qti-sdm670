#ifndef __PMICULOG_H__
#define __PMICULOG_H__
/*
===========================================================================
*/
/**
  @file pm_ulog.h

  This header provides with initializaiton of PMIC ULOG.

  This file provides an abstraction layer for PMIC ULOG.

*/
/*
  ====================================================================

  Copyright (c) 2011-13 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: 
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  02/08/15   rl     Created.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ULogFront.h"
#include "CoreVerify.h"

/*=========================================================================
      Structures
==========================================================================*/

typedef struct 
{
    ULogHandle pm_log;  
    uint32 log_size;
}pm_ulog_vars_type;

typedef enum pm_ulog_level_type{
    PM_LOG_LEVEL_NONE      = 0,
    PM_LOG_LEVEL_ERR       = 1,
    PM_LOG_LEVEL_INFO      = 2,
    PM_LOG_LEVEL_DEBUG     = 3, 
    PM_LOG_LEVEL_ALL_LOGS
}pm_log_level_type;


extern pm_ulog_vars_type pm_ulog_vars; 

extern pm_log_level_type pm_log_level;

#define PM_LOG_NUM_ARGS(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,N,...) N

#define PM_LOG(...) ULogFront_RealTimePrintf(pm_ulog_vars.pm_log, ((PM_LOG_NUM_ARGS(__VA_ARGS__, 11,10,9,8,7,6,5,4,3,2,1))-1), __VA_ARGS__)

/**
Description:
Macro to print important information which makes debugging easier
Prints up to nine variables

Example Usage: 
PM_LOG_MSG_INFO("var1 = %d, var2 = %s, var3 = 0x%x", some_var, some_str, some_hex_var);

Output: 
<hex_timestamp>: Info: <function_name>: var1 = <value of var1> var2 = <string> var3 = 0x<hex value of var3>
**/

#define PM_LOG_MSG_INFO(msg, args...) do{if(pm_log_level >= PM_LOG_LEVEL_INFO) PM_LOG("Info: %s: "msg, __func__, ##args); }while(FALSE) 

/**
Description:  
Macro to print error messages. 
Printed when pm_log_level >= PM_LOG_LEVEL_ERR and err != PM_ERR_FLAG__SUCCESS
Prints up to nine variables

Example Usage: 
PM_LOG_MSG_ERROR(err_flag, "var1 = %d, var2 = %s, var3 = 0x%x", some_var, some_str, some_hex_var);

Output: 
<hex_timestamp>: ERROR: <function_name>: var1 = <value of var1> var2 = <string> var3 = 0x<hex value of var3>
**/

#define PM_LOG_MSG_ERROR(err, msg, args...) do{if((pm_log_level >= PM_LOG_LEVEL_ERR) && (err != PM_ERR_FLAG__SUCCESS)) PM_LOG("ERROR: %s: "msg, __func__, ##args); }while(FALSE)

/**
Description:
Macro to print debug messages, printed when pm_log_level >= PM_LOG_LEVEL_DEBUG
Prints up to nine variables

Example Usage: 
PM_LOG_MSG_DEBUG("var1 = %d, var2 = %s, var3 = 0x%x", some_var, some_str, some_hex_var);

Output: 
<hex_timestamp>: Debug: <function_name>: var1 = <value of var1> var2 = <string> var3 = 0x<hex value of var3>
**/

#define PM_LOG_MSG_DEBUG(msg, args...) do{if(pm_log_level >= PM_LOG_LEVEL_DEBUG) PM_LOG("Debug: %s: "msg, __func__, ##args); }while(FALSE)

/**
Description:
Macro to error fatal device after printing messages

Example Usage: 
PM_LOG_MSG_ERR_FATAL("var1 = %d, var2 = %s, var3 = 0x%x", some_var, some_str, some_hex_var);

Output: 
<hex_timestamp>: ERROR_FATAL: <function_name>: var1 = <value of var1> var2 = <string> var3 = 0x<hex value of var3>

Dependencies:
Include CoreVerify.h in files using this macro. 
Due to memory optimization in err.h used by CoreVerify.h it is not recommended to include CoreVerify.h in Header Files
**/

#define PM_LOG_MSG_ERR_FATAL(msg, args...) do{PM_LOG("ERROR_FATAL: %s: "msg, __func__, ##args); CORE_VERIFY(0);}while(FALSE)


/**
Description:
Log init function
**/ 
    
void pm_ulog_init(void);


#endif // __PMICULOG_H__


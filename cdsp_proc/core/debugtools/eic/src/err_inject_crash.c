/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      ERR service crash simulator
                
GENERAL DESCRIPTION
  Command handler for injecting various types of crashes for testing

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Diag must be initialized

Copyright (c) 2011 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*==========================================================================

                        EDIT HISTORY FOR MODULE
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/eic/src/err_inject_crash.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/25/17   jlg     Established settings interface header file
07/23/14   din     Removed warnings for llvm .
04/18/14   din     Added changes for ADSP island mode.
08/01/13   rks     Incorporated the timer API changes
10/28/11   mcg     File created.

===========================================================================*/


#ifndef ERR_INJECT_CRASH
  #error Required definition is missing, check configuration
#endif //ERR_INJECT_CRASH

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "err.h"
#include "err_inject_crash.h"
#include "err_inject_crash_settings.h"
#include "tms_diag.h"
#include "osal.h"
#include "timer.h"
#include "timetick.h"

#include <stdint.h>
#include "DALSysTypes.h"
#include "DALPropDef.h"
#include "tms_dll_api.h"

/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern void uerr_inject_crash_initiate(err_inject_crash_type);
extern void err_inject_crash_wdog(void);
extern void err_inject_crash_wdog_nmi(void);
extern void err_inject_crash_wdog_multithreaded(void); 
extern boolean err_inject_crash_validate(err_inject_crash_type);

/*===========================================================================

                     INTERNAL DATA DECLARATIONS

===========================================================================*/
// default value if no delay is provided (min value for device config input)
#define ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT  1
#define ERR_INJECT_CRASH_DELAY_DEVCFG_MIN   60

/* table for diag packet command handler registration */
static diagpkt_user_table_entry_type err_inject_crash_diagpkt_tbl[TMS_DIAG_TBLSIZE];

typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
  uint8 crash_type;
  uint8 crash_delay;
} err_inject_crash_req_type;

static volatile int err_inject_crash_zero = 0;

static timer_type       eicTimer;
static timer_group_type eic_timer_group = {0};
static boolean          crash_pending = FALSE;

typedef enum
{
  ERR_UIMAGE_MODE,
  ERR_MAINIMAGE_MODE
} err_image_mode_t;

/* Data Declarations for uImage */
typedef struct err_crash_info_s
{
  err_inject_crash_type crash;
  uint8                 delay;
  err_image_mode_t      image_mode;
} err_crash_info_t;

static err_crash_info_t crash_info;

/*===========================================================================

                        Private function prototypes

===========================================================================*/
void * err_inject_crash_diag (void *, uint16);
void * err_inject_crash_diag_uimg (void * req_ptr, uint16 pkt_len);

static void err_inject_crash_err_fatal(void);
static void err_inject_null_access(void);
static void err_inject_crash_div0(void);

static void err_inject_crash_initiate(err_inject_crash_type, uint32, 
		                      err_image_mode_t image_mode);
static void err_inject_crash_execute(timer_cb_data_type);
static void err_inject_crash_devcfg_check(void);
boolean err_inject_crash_island_validate(err_inject_crash_type injected_crash);

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION err_inject_crash_init

DESCRIPTION
  Initializes the crash injection feature

DEPENDENCIES
  diag must be available to register command handler

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
DLL_API_GLOBAL void err_inject_crash_init (void)
{
#ifndef ERR_USES_DEVCFG
  ERR_FATAL("Device Config must be enabled for crash injection",0,0,0);
#endif //ERR_USES_DEVCFG
  //Check for device config method before registering with diag (thus avoiding a mutex)
  err_inject_crash_devcfg_check();
  err_inject_crash_diagpkt_tbl[0] = tms_diag_build_table_entry( TMS_DIAGPKT_ERR_CORE_DUMP, err_inject_crash_diag);
  #ifdef UERR_ISLAND_MODE
    err_inject_crash_diagpkt_tbl[1] = tms_diag_build_table_entry_uimg( TMS_DIAGPKT_ERR_CORE_DUMP, err_inject_crash_diag_uimg);
  #endif
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (ERR_DIAG_PROC_ID, DIAG_SUBSYS_DEBUG, err_inject_crash_diagpkt_tbl);
                                         
  //initialize timer group to be safe
  memset(&eic_timer_group, 0, sizeof(eic_timer_group)); 
  timer_def_osal(&eicTimer, &eic_timer_group, TIMER_FUNC1_CB_TYPE, err_inject_crash_execute, (timer_cb_data_type)&crash_info);
                                       
  //Test case
  //err_inject_crash_initiate(ERR_INJECT_ERR_FATAL, 0, ERR_UIMAGE_MODE);
                                       
}

/*===========================================================================

FUNCTION err_inject_crash_island_validate

DESCRIPTION
  Validates the crash injected for Island mode

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean err_inject_crash_island_validate(err_inject_crash_type injected_crash)
{
  boolean is_valid = TRUE;

  // add USER PD unsupported commands here
  switch(injected_crash)
  {
    case ERR_INJECT_WDOG_TIMEOUT:
      is_valid = FALSE;
      break;
    case ERR_INJECT_WDOG_NMI:
      is_valid = FALSE;
      break;
    case ERR_INJECT_WDOG_MULTITHREADED:
      is_valid = FALSE;
      break;
    case ERR_INJECT_DYNAMIC_ERR_FATAL:
      is_valid = FALSE;
      break;
    default:
      break;
  }
  
  if (injected_crash >= SIZEOF_ERR_INJECT_CRASH_TYPE)
  {
    is_valid = FALSE;
  }
  
  return is_valid;
} 

/*===========================================================================

FUNCTION err_inject_crash_diag

DESCRIPTION
  Force a crash to occur remotely by calling ERR_FATAL

DEPENDENCIES
  err_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr

SIDE EFFECTS

===========================================================================*/
void *
err_inject_crash_diag (void * req_ptr, uint16 pkt_len)
{
  err_inject_crash_req_type *req;
  err_inject_crash_type injected_crash = ERR_INJECT_ERR_FATAL;
  uint8 injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;

  // Diag only handles one request at a time, and dalcfg is checked before we register with diag
  // so simple mutual exclusion is sufficient (for now)  
  if (crash_pending)
  {
    TMS_MSG_HIGH("Crash pending, ignoring subsequent request");
    tms_diag_send_generic_response(req_ptr);
    return(NULL);
  }
  else
  {
    crash_pending = TRUE;  //set before diag response to guarantee no reentrancy issues
  }
  
  // decode request
  req = (err_inject_crash_req_type *) req_ptr;

  // default parameters
  if (pkt_len <= sizeof(req->hdr)) 
  {
    injected_crash = ERR_INJECT_ERR_FATAL;
    injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;
  }
  // default delay parameter
  else if (pkt_len <= (sizeof(req->hdr) + sizeof(req->crash_type)) )
  {
    injected_crash = (err_inject_crash_type)(req->crash_type);
    injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;
  }
  else
  {
    injected_crash = (err_inject_crash_type)(req->crash_type);
    injected_delay = (err_inject_crash_type)(req->crash_delay);
  }
    
  // validate crash cause
  if (FALSE == err_inject_crash_validate(injected_crash)) 
  {
    //do not err_fatal, as this may present false positives
    TMS_MSG_ERROR_1("Invalid crash ID %#x", injected_crash);
    crash_pending = FALSE;
    tms_diag_send_error_response(req_ptr, pkt_len);
    return(NULL);
  }

  tms_diag_send_generic_response(req_ptr);
  err_inject_crash_initiate(injected_crash, injected_delay, ERR_MAINIMAGE_MODE);

  return(NULL);  //return NULL since we already called diagpkt_commit above
}

/*===========================================================================

FUNCTION err_inject_crash_diag_uimg

DESCRIPTION
  Force a crash to occur remotely by calling ERR_FATAL

DEPENDENCIES
  err_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr

SIDE EFFECTS

===========================================================================*/
void *
err_inject_crash_diag_uimg (void * req_ptr, uint16 pkt_len)
{
  err_inject_crash_req_type *req;
  err_inject_crash_type injected_crash = ERR_INJECT_ERR_FATAL;
  uint8 injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;

  // Diag only handles one request at a time, and dalcfg is checked before we register with diag
  // so simple mutual exclusion is sufficient (for now)  
  if (crash_pending)
  {
    TMS_MSG_HIGH("Crash pending, ignoring subsequent request");
    tms_diag_send_generic_response(req_ptr);
    return(NULL);
  }
  else
  {
    crash_pending = TRUE;  //set before diag response to guarantee no reentrancy issues
  }

  // decode request
  req = (err_inject_crash_req_type *) req_ptr;

  // default parameters
  if (pkt_len <= sizeof(req->hdr)) 
  {
    injected_crash = ERR_INJECT_ERR_FATAL;
    injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;
  }
  // default delay parameter
  else if (pkt_len <= (sizeof(req->hdr) + sizeof(req->crash_type)) )
  {
    injected_crash = (err_inject_crash_type)(req->crash_type);
    injected_delay = ERR_INJECT_CRASH_DELAY_DIAG_DEFAULT;
  }
  else
  {
    injected_crash = (err_inject_crash_type)(req->crash_type);
    injected_delay = (err_inject_crash_type)(req->crash_delay);
  }

  // validate crash cause
  if (FALSE == err_inject_crash_island_validate(injected_crash)) 
  {
    //do not err_fatal, as this may present false positives
    TMS_MSG_ERROR_1("Invalid crash ID %#x", injected_crash);
    crash_pending = FALSE;
    tms_diag_send_error_response(req_ptr, pkt_len);
    return(NULL);
  }

  tms_diag_send_generic_response(req_ptr);
  err_inject_crash_initiate(injected_crash, injected_delay, ERR_UIMAGE_MODE);

  return(NULL);
}

/*===========================================================================

FUNCTION err_inject_crash_initiate

DESCRIPTION
  Initiates the crash task which will perform the actual crash once timer expires

DEPENDENCIES
  err_init must be called prior

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void err_inject_crash_initiate
(
  err_inject_crash_type req_crash, 
  uint32 req_delay,
  err_image_mode_t image_mode
)
{
  crash_info.crash      = req_crash;
  crash_info.delay      = req_delay;
  crash_info.image_mode = image_mode;
  
  //check for immediate crash (from caller context)
  if (0==req_delay)
  {
    err_inject_crash_execute((timer_cb_data_type)&crash_info);
  }
  else
  {
    //set cb timer to trigger actual crash
    timer_set_64(&eicTimer, (timetick_type)req_delay, 0, T_SEC);
  }

}

static void err_inject_crash_err_fatal(void)
{
  ERR_FATAL("Crash injected via Diag",0,0,0);
}

static void err_inject_null_access(void)
{
  static int result;

  result = *(int*)err_inject_crash_zero;

  TMS_MSG_ERROR_2("Should have crashed due to null access: %#x->%#x", err_inject_crash_zero, result);
}

static void err_inject_crash_div0(void)
{
  static float result;

  result = 7/err_inject_crash_zero;

  TMS_MSG_ERROR_2("Should have crashed due to div0: 7/%u=%d", err_inject_crash_zero, result);
}

void err_inject_crash_execute(timer_cb_data_type data)
{
  char                 temp[80];
  char* str = "Dynamic EIC: This should be SFR in kernel log and coredump.err.aux_msg";
  int buf_len;

  err_crash_info_t * temp_crash_info = 
	                (err_crash_info_t *)data; 

  //crash  
  if ( temp_crash_info->image_mode == ERR_MAINIMAGE_MODE )
  {
    switch(temp_crash_info->crash)
    {
    case ERR_INJECT_DIV0: 
      err_inject_crash_div0();
      break;
    case ERR_INJECT_WDOG_TIMEOUT:
      err_inject_crash_wdog();
      break;
    case ERR_INJECT_NULLPTR:
      err_inject_null_access();
      break;
    case ERR_INJECT_ERR_FATAL :
      err_inject_crash_err_fatal();
      break;
    case ERR_INJECT_WDOG_NMI:
      err_inject_crash_wdog_nmi();
      break;
    case ERR_INJECT_WDOG_MULTITHREADED:
      err_inject_crash_wdog_multithreaded();
      break;
    case ERR_INJECT_DYNAMIC_ERR_FATAL :
      buf_len = tms_utils_fmt(temp, 80, "MSG: %s ", str );
      ERR_FATAL_AUX_MSG("Dynamic EIC: this should not be kernel log SFR", temp, buf_len);
      break;
    default:
      TMS_MSG_ERROR_1("Unhandled crash ID %#x", temp_crash_info->crash);
      break;
    }
  }
  else
  {
#ifdef UERR_ISLAND_MODE
      uerr_inject_crash_initiate(temp_crash_info->crash);
#endif
  }

  crash_pending = FALSE;
}

static void err_inject_crash_devcfg_check(void)
{
  err_inject_crash_type eic_type;
  uint32 eic_delay = ERR_INJECT_CRASH_DELAY_DEVCFG_MIN;

  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

  DALSYS_GetDALPropertyHandleStr("tms_eic",pHandle);

  // Check for devcfg crashing enabled
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"eic_crash_enable", 0, &propValue))
  {
    if(0 == propValue.Val.dwVal)
    {
      return;
    }
  }
  else
  {
    TMS_MSG_HIGH("Could not read eic_crash_enable devcfg");
    return;
  }

  // Crashing enabled, read eic_crash_type parameter
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"eic_crash_type", 0, &propValue))
  {
    if (FALSE == err_inject_crash_validate(propValue.Val.dwVal))
    {
      TMS_MSG_HIGH("Bad enum in eic_crash_type dalconfig");
      return;
    }

    eic_type = (err_inject_crash_type)propValue.Val.dwVal;
  }
  else
  {
    TMS_MSG_HIGH("Could not read eic_crash_type devcfg");
    return;
  }

  // Read eic_crash_delay parameter (initialized to DEFAULT above)
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"eic_crash_delay", 0, &propValue))
  {
    if (ERR_INJECT_CRASH_DELAY_DEVCFG_MIN > propValue.Val.dwVal)        
    {
      TMS_MSG_HIGH_1("Small eic_crash_delay dalconfig, using %d", ERR_INJECT_CRASH_DELAY_DEVCFG_MIN);
      eic_delay = ERR_INJECT_CRASH_DELAY_DEVCFG_MIN;
    }
    else
    {
      eic_delay = propValue.Val.dwVal;
    }
  }
  else
  {
    TMS_MSG_HIGH_1("Could not read eic_crash_delay devcfg, using %d ", ERR_INJECT_CRASH_DELAY_DEVCFG_MIN);
    eic_delay = ERR_INJECT_CRASH_DELAY_DEVCFG_MIN;
  }
    
  //good to initiate timer
  if ((!crash_pending))  //this should *ALWAYS* be true since devcfg is first in eic_init()
  {
      crash_pending = TRUE;  // prevent diag packets from causing re-entrancy
      err_inject_crash_initiate(eic_type, eic_delay, ERR_MAINIMAGE_MODE);
  }
}

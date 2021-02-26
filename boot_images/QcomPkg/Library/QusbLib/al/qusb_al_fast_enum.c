/*=======================================================================*//**
  @file         qusb_al_fast_enum.c

  @brief        Adaptation layer for fast enumeration

  @details      
                In case of data card on PC, BIOS may want to enumerate all
                attached devices in order to check every device against its
                whitelist.
                Application starts stack, polls it until enumeration done,
                then shuts down stack.

                There are 3 flavors:
                - separate descriptor set
                - HDLC descriptor set


                Copyright (c) 2014 Qualcomm Technologies Incorporated 
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  when        who       what, where, why
  ----------  ---      -------------------------------------------------------
  2014-07-25  shreyasr  Initial Draft of the code 
  
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "qusb_dci_api.h"         // qusb_dci_disconnect
#include "qusb_dcd.h"
#include "qusb_al_fast_enum.h" 
#include "qusb_log.h" 
#include "qusb_fd.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#ifndef QUSB_FAST_ENUM_PID
#define QUSB_FAST_ENUM_PID (0x901C)
#endif

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static struct qusb_dcd_dsc_device* dev = NULL;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
// iProduct
static char qusb_fast_enum_str[] = "QUSB_Fast_Enum";

// ===========================================================================
/**
* @function    qusb_al_fast_enum_init
* 
* @brief       Initialize the Function Driver for performing fast enumeration
*
* @param       [IN] speed_required- Maximum Speed Required
*
*          
* @return      None
* 
* @Note        For fast enum, currently we support only HS.   
*/
// ===========================================================================
void qusb_al_fast_enum_init(qusb_max_speed_required_t speed_required)
{
  qusb_log(QUSB_AL_FAST_ENUM_INIT_LOG, 0, 0);

  dev = qusb_fd_init(NULL, 
                     qusb_fast_enum_str, 
                     NULL, 
                     NULL, 
                     NULL, 
                     QUSB_FAST_ENUM_PID, 
                     speed_required); 

  if (NULL == dev)
  {
    qusb_error_log(FAST_ENUM_NULL_PTR_ERROR_LOG, 0, __LINE__);
  }
}
// ===========================================================================
/**
* @function    qusb_al_fast_enum_shutdown
* 
* @brief       API to perform the shutdown of the function driver 
*
* @param       None
*
*          
* @return      None
* 
* @Note          
*/
// ===========================================================================
void qusb_al_fast_enum_shutdown(void)
{
  qusb_log(QUSB_AL_FAST_ENUM_SHUTDOWN_LOG, 0, 0);
  qusb_dci_set_run_stop(FALSE); 
  qusb_fd_set_shutdown_mode(QUSB_COMPLETE_SHUTDOWN_MODE);
  qusb_fd_shutdown();
}
// ===========================================================================
/**
* @function    qusb_al_fast_enum_poll
* 
* @brief       API to poll for USB events to perform enumeration 
*
* @param       None
*
*          
* @return      TRUE : If SET_CONFIGURED is received
*              FALSE: If SET_CONFIGURED is not received
* 
* @Note          
*/
// ===========================================================================
boolean qusb_al_fast_enum_poll(void)
{
  qusb_dcd_poll(dev);
  return(0 != dev->config_value);
}
// ===========================================================================
/**
* @function    qusb_al_do_fast_enum
* 
* @brief       API to perform Fast Enumeration- Extern API 
*
* @param       [IN]- speed_required - Maximum Speed at which the device should 
*                                     be configured
*
*          
* @return      None
* 
* @Note        Sahara Layer should call this function to perform the
*              the enumeration. 
*/
// ===========================================================================
void qusb_al_do_fast_enum(qusb_max_speed_required_t speed_required)
{
#ifdef FEATURE_QUSB_FAST_ENUMERATION
  qusb_log(QUSB_AL_DO_FAST_ENUM_LOG, 0, 0);

  //initialize the stack 
  qusb_al_fast_enum_init(speed_required);

  if(dev != NULL)
  {
    //wait for set_configuration to occur 
    while(!qusb_al_fast_enum_poll()); 
  }

  //clean the enumeration
  qusb_al_fast_enum_shutdown(); 
#else
  qusb_log(QUSB_AL_DO_FAST_ENUM_UNSUPPORTED_LOG, 0, 0);
#endif
}

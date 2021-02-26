/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S I O    W R A P P E R   F U N C T I O N S
                            
       SIO wrapper functions for serving all devices that uses SIO 

GENERAL DESCRIPTION
  This module contains the wrapper function used to access SIO server
  layer.  This layer is only use temporary.  Each port that wish to access
  the SIO layer should register their own function with SIO server, insteaded
  going though the wrapper functions.
  
NOTE DURING DEVELOPEMNT
    Each served device must have thses functions wrapped:
        HW_open
        HW_close
        HW_ioctl
        HW_flush_tx
        HW_disable_device
        HW_control_transmit
        HW_control_close
        HW_control_open



        open
        close
        ioctl
        flush_tx
        disable_device
        control_transmit
        control_close
        control_open

    Magic number:
        Value used to globally identify a device.  32 bit number divided into
        16 bit Major and 16 bit minor number    
  
EXTERNALIZED FUNCTIONS

    sio_wrapper_init()
      Used to install all the function with the SIO layer.  

SERVED FUNCTIONS

  
Copyright (c) 2007-2009,2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

mm/dd/yy   who     what, where, why
--------   ---     ----------------------------------------------------------
24/03/14   vv     created sio_wrapper.c for Qube
===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/
#include "target.h"                /* Target specific definitions          */
#include "comdef.h"                /* Definitions for byte, word, etc.     */
#include "sio.h"                   /* Serial I/O Services.                 */
#include "assert.h"
#include "sio_priv.h"
#include "sio_wrapper.h"
#include "msg.h"
#include "qube.h"
#include "amssassert.h"
#include "err.h"
/* UART header for UART driver initialization function.*/
#include "uart_init.h"

/*===========================================================================
  Extern: variables/Functions
===========================================================================*/

/* list of driver's init_func pointers */
extern sio_driver_init_func_type   sio_driver_init_list[SIO_MAX_DRIVERS];

/* Critical section/Mutex for SIO (QUBE) */
static qmutex_t sio_crit_sect;



/*===========================================================================

FUNCTION sio_set_driver_init_list();                         INTERNAL FUNCTION

DESCRIPTION
  set driver init functions (target specific featurized function)
  
DEPENDENCIES
  Should be called just once.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void sio_set_driver_init_list(void)
{
  int index = 0;

  sio_driver_init_list[index++] = sio_uart_driver_init;
  
  ASSERT(index <= SIO_MAX_DRIVERS);
  /* -- UPDATE THE SIO_MAX_DRIVERS AFTER ADDING DRIVER INIT HERE IN FUTURE -- */
}

/*===========================================================================

FUNCTION sio_init_rdm_sdm();                         INTERNAL FUNCTION

DESCRIPTION
  Initialize rdm and sdm (not implemented for WM)
  
DEPENDENCIES
  Should be called just once.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/

void sio_init_rdm_sdm()
{ 
  qmutex_attr_t  qmutexattr;
  qmutexattr.type = QMUTEX_LOCAL;
  if(qmutex_create(&sio_crit_sect, &qmutexattr) != EOK)
  {
     ERR_FATAL("SIO: sio_init_rdm_sdm() - failed to create sio_crit_sect.", 0, 0, 0);
  }
}


/*===========================================================================

FUNCTION sio_enter_crit_section 

DESCRIPTION
  sio_enter_crit_section
      
DEPENDENCIES
  sio_init_crit_section() needs to be called first
      
RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
===========================================================================*/
void sio_enter_crit_section()
{
    if(qmutex_lock(sio_crit_sect) != EOK)
    {
       ERR_FATAL("SIO: sio_enter_crit_section() - qmutex_lock failed.", 0, 0, 0);
    }
}

/*===========================================================================

FUNCTION sio_leave_crit_section 

DESCRIPTION
  sio_leave_crit_section
      
DEPENDENCIES
  sio_init_crit_section() needs to be called first
      
RETURN VALUE
  None
    
SIDE EFFECTS
  None
  
===========================================================================*/
void sio_leave_crit_section()
{
    if(qmutex_unlock(sio_crit_sect) != EOK)
    {
       ERR_FATAL("SIO: sio_leave_crit_section() - qmutex_unlock failed.", 0, 0, 0);
    }
}


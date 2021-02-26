#ifndef SIO_PRIV_H
#define SIO_PRIV_H

/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                P R I V A TE    H E A D E R    F I L E

DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial I/O Services.

Copyright (c) 2007-2008,2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

MM/DD/YY   who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/09   amv     Moved public reference to sio.h (CMI #2)
03/24/08   bfc     Added some new SMD DATA ports.
01/11/07   rh      Spin off from sio.h

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "comdef.h"     /**< Definitions for byte, word, etc.                */
#include "dsm.h"        /**< Data service buffer definition                  */
#include "sio.h"        /**< Defines all public SIO data types               */

#if defined(_WIN32_WCE) && (_WIN32_WCE >= 0x700)
#include "timetick.h"
#include "timer.h"
#else  // (_WIN32_WCE >= 0x700)
#include "customer.h"   /* Customer Specific Definitions                   */
#endif // (_WIN32_WCE >= 0x700)
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define SIO_DEVICE_HEAD_MAGIC_NUM 0xDEADBEEF

#define SIO_MAX_MAJOR_NUM 3                /**< Maximum major numbers supported in SIO */
#define SIO_MAX_MINOR_NUM 5                /**< Maximum minor numbers supported in SIO */
#define SIO_MAX_DRIVERS SIO_MAX_MAJOR_NUM   /**< Max. number of drivers supported in SIO */


/**---------------------------------------------------------------------------
 This is the function pointer type which will be passed to clk_reg.
---------------------------------------------------------------------------*/

typedef void (*sio_clk_routine_ptr_type)( int4 ms_interval);

/*===========================================================================

FUNCTION SIO_INIT                                    EXTERNALIZED FUNCTION

@brief
  Initializes SIO.  
  
@dependencies
  None
    
@return
  None
  
@sideeffects
  None
  
===========================================================================*/

extern void sio_init
(
  void
);

/*===========================================================================

FUNCTION SIO_CLOSE_EXT                           EXTERNALIZED FUNCTION

@brief
  This procedure does all the book-keeping necessary to close a stream.  
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.
                  
@dependencies
  This function is only supported in shared memory ports 
      
@return
  None
  
@sideeffects
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

extern void sio_close_ext
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  /* Function to call when transmission is complete.                       */
  void (*close_func_ptr)(void * cb_data),
  void *ext_cb_data
);

/*===========================================================================

FUNCTION SIO_FLUSH_TX_EXT                           EXTERNALIZED FUNCTION

@brief
  This function will transmit any queued data and then call the registered
  callback.

@dependencies
  This is only supported on Shared memroy ports
  
@return
  None
  
@sideeffects
  None
  
===========================================================================*/

extern void sio_flush_tx_ext
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  /* Function to call when transmission is complete.                       */
  void (*flush_func_ptr)(void * cb_data),
  void *ext_cb_data
);

/*===========================================================================

FUNCTION SIO_GET_STREAM_ID                                  REGIONAL FUNCTION

@brief
  Return stream ID which is not currently allocated (i.e. opened).  
  INTLOCK_SAV statement and INTFREE_SAV statement makes this function
  re-entrant.
      
@dependencies
  None
                                                       
@return
  First available stream ID. 
  If no stream identifiers are available, return SIO_NO_STREAM_ID.
    
@sideeffects
  None
  
===========================================================================*/

sio_stream_id_type sio_get_stream_id( void);

/*===========================================================================

FUNCTION SIO_PUT_STREAM_ID                                  REGIONAL FUNCTION

@brief
  Allows caller to deallocate or put back stream ID previously allocated.
  INTLOCK_SAV & INTFREE_SAV statements make this procedure re-entrant.
      
@dependencies
  None
                                                       
@return
  None
  
@sideeffects
  None
  
===========================================================================*/

void sio_put_stream_id( stream_id_type stream_id);

/*===========================================================================

FUNCTION SIO_INITIALIZE_STREAM_OPENED                       REGIONAL FUNCTION

@brief
  Allows caller to initialize data structure used in indicating whether or
  not certain streams are open.
      
@dependencies
  None
                                                       
@return
  
@sideeffects
  None
  
===========================================================================*/

void sio_initialize_stream_opened( void);

/*===========================================================================

FUNCTION SIO_DISABLE_DEVICE                             EXTERNALIZED FUNCTION

@brief
  This routine disables given device, including turning off clock to save
  power.  This is different from UART powerdown feature which attempts to
  save power with minimal loss of data.  Instead, this function is meant 
  for customers who may not want to use both UARTs.
  
  Effects of this function may be reversed by calling sio_init.

@dependencies
  None

@return
  None

@sideeffects
  None
  
===========================================================================*/

extern void sio_disable_device
(
  sio_port_id_type   port_id              /* SIO Port ID                   */  
);

/*===========================================================================

FUNCTION SIO_DRIVER_INIT                                     INTERNAL FUNCTION

@brief
  Initializes Drivers to register ports.
  
@dependencies
  Should be called just once.
  
@return
  None
  
@sideeffects
  None
  
===========================================================================*/

void sio_driver_init(void);

/*===========================================================================

FUNCTION SIO_INIT_RDM_SDM                    INTERNAL FUNCTION (OS-dependent)

@brief
  Initializes RDM and SDM. (implemented in sio_wrapper.c as it is os-dependent)
  
@dependencies
  Should be called just once.
  
@return
  None
  
@sideeffects
  None
  
===========================================================================*/

void sio_init_rdm_sdm(void);

/*===========================================================================

FUNCTION sio_set_driver_init_list();                         INTERNAL FUNCTION

@brief
  set driver init functions (target specific featurized function)
  (implemented in sio_wrapper.c as it is os-dependent)
  
@dependencies
  Should be called just once.
  
@return
  None
  
@sideeffects
  None
  
===========================================================================*/

void sio_set_driver_init_list(void);


/* Include at the end for compile time dependencies */
#include "sio_wrapper.h"

#endif /* SIO_PRIV_H */

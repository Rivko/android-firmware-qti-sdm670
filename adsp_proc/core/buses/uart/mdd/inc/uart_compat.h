#ifndef UART_COMPAT_H
#define UART_COMPAT_H     1    
/*============================================================================

GENERAL DESCRIPTION

  This module contains code for compatability between different versions
  of the UART driver and the code it integrates with.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008, 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.  Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/inc/uart_compat.h#1 $

when         user      what, where, why
-----------  --------  -------------------------------------------------------
15-Jan-2009  cmihalik  Initial revision.
22-Jul-2009  cmihalik  Expand the compatibility layer to include a lot more things.
============================================================================*/

#include "target.h"
#include "comdef.h"
#include "sio.h"
#include "DALStdDef.h"

//  xxx:  There is no DAL invalid device id but 0 is not used. 

#define UART_INVALID_DALDEVICEID    0

//----------------------------------------------------------------------------
//  Count the number of UARTs.
//----------------------------------------------------------------------------

#define UART_FORCE_SIZE_TO_32_BITS     0x12345678

#ifdef FEATURE_CXM_UART
  #define UART_NUM_DEVICES    4
#elif FEATURE_THIRD_UART
  #define UART_NUM_DEVICES    3
#elif defined FEATURE_SECOND_UART
  #define UART_NUM_DEVICES    2
#elif defined FEATURE_FIRST_UART
  #define UART_NUM_DEVICES    1
#else
  #define UART_NUM_DEVICES    0
#endif

//----------------------------------------------------------------------------
///  Choice values for driver type in custsio.h.
//----------------------------------------------------------------------------

#define NO_DRIVER               0   //  Should only be used with NO_PHYS_DEVICE.
#define PIO_DRIVER              1   //  Programmed I/O => don't use DMA.  Run in "legacy" mode.
                                    //  Can be used with legacy UARTs (non DM) or UART DM.
#define BAM_DRIVER              2   //  Use BAM for DMA.     
#define DM_DRIVER               3   //  Use DM for DMA.     

//----------------------------------------------------------------------------
//  Choice values for UART physical devices (SIORS_{FIRST,SECOND,THIRD}_UART_PHYS_DEVICE)
//  in custsio.h.
//----------------------------------------------------------------------------

#define INVALID_PHYS_DEVICE     0
#define NO_PHYS_DEVICE          1

#define UART1_PHYS_DEVICE       101
#define UART2_PHYS_DEVICE       102
#define UART3_PHYS_DEVICE       103

#define UART1DM_PHYS_DEVICE     201
#define UART2DM_PHYS_DEVICE     202

#define GSBI0_PHYS_DEVICE       300
#define GSBI1_PHYS_DEVICE       301
#define GSBI2_PHYS_DEVICE       302
#define GSBI3_PHYS_DEVICE       303
#define GSBI4_PHYS_DEVICE       304
#define GSBI5_PHYS_DEVICE       305
#define GSBI6_PHYS_DEVICE       306
#define GSBI7_PHYS_DEVICE       307
#define GSBI8_PHYS_DEVICE       308
#define GSBI9_PHYS_DEVICE       309
#define GSBI10_PHYS_DEVICE      310
#define GSBI11_PHYS_DEVICE      311
#define GSBI12_PHYS_DEVICE      312

#define UARTBAM1_PHYS_DEVICE    400
#define UARTBAM2_PHYS_DEVICE    401
#define UARTBAM3_PHYS_DEVICE    402
#define UARTBAM4_PHYS_DEVICE    403
#define UARTBAM5_PHYS_DEVICE    404
#define UARTBAM6_PHYS_DEVICE    405
#define UARTBAM7_PHYS_DEVICE    406
#define UARTBAM8_PHYS_DEVICE    407
#define UARTBAM9_PHYS_DEVICE    408
#define UARTBAM10_PHYS_DEVICE   409
#define UARTBAM11_PHYS_DEVICE   410
#define UARTBAM12_PHYS_DEVICE   411
#define UARTCXM_PHYS_DEVICE     412

//----------------------------------------------------------------------------
//  uart_device_id_type
//----------------------------------------------------------------------------

typedef enum 
{
  //  UART devices.

  UART_DEVICE_INVALID = -1,
  UART_DEVICE_1 = 0,
  UART_DEVICE_2,
  UART_DEVICE_3,
  UART_DEVICE_CXM,

  //  Aliases used for enumeration.
  //  Enumeration is ( i = FIRST; i < LAST; i++ ), i.e. LAST is not included.

  UART_DEVICE_FIRST = UART_DEVICE_1,
  UART_DEVICE_LAST  = UART_NUM_DEVICES,

  //  Force size. 

  UART_DEVICE_FORCE_SIZE = UART_FORCE_SIZE_TO_32_BITS

} uart_device_id_type;

//----------------------------------------------------------------------------
//  uart_device_info
//----------------------------------------------------------------------------

//  The purpose of this structure is to convert all of the compile
//  time configuration into something that can be accessed at runtime.

typedef struct uart_device_info
{
  boolean                  is_present;
  boolean                  is_valid;
  boolean                  enable_dma;
  boolean                  can_wake_on_rx_event;
  DALDEVICEID              dal_device_id;

} uart_device_info;

//----------------------------------------------------------------------------
//  FUNCTION 
//    uart_sio_port_id_to_device_id
//
//  DESCRIPTION
//    This function converts the input SIO port id to the corresponding 
//    device id.
//
//  DEPENDENCIES
//    None
//
//  RETURN VALUE
//    device_id if input port_id is valid.
//    UART_DEVICE_INVALID otherwise.
//
//  SIDE EFFECTS
//    None
//----------------------------------------------------------------------------

uart_device_id_type      uart_sio_port_id_to_device_id( sio_port_id_type port_id );
boolean                  uart_get_device_info( uart_device_id_type device_id, uart_device_info* pInfo );

#endif


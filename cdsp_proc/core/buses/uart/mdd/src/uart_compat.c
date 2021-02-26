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

  $Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/mdd/src/uart_compat.c#1 $

when         user      what, where, why
-----------  --------  -------------------------------------------------------
15-Jan-2009  cmihalik  Initial revision.
22-Jul-2009  cmihalik  Greatly expand the scope to the compat layer.

============================================================================*/

#include "target.h"
#include "comdef.h"
#include "err.h"
#include "uart_compat.h"
#include "DALDeviceId.h"
#include "DDIUart.h"
#include "uart_log.h"

#define UART_TRACE_BUFFER_FILE_ID      7

//  Something not likely to conflict with any port id.
#ifndef SIO_PORT_ILLEGAL
#define SIO_PORT_ILLEGAL   (0x7fffffff)
#endif

//----------------------------------------------------------------------------
//  uart_sio_port_id_to_device_id
//----------------------------------------------------------------------------

uart_device_id_type
uart_sio_port_id_to_device_id( sio_port_id_type port_id )
{
   uart_device_id_type device_id;

   //  This code assumes the client and the driver are compiled
   //  with the same version of the sio.h header file.  If not,
   //  something smarter needs to be done here.
   //
   //  Cannot use a simple switch statement.  Why?  The SIO port
   //  definitions may be replaced with SIO_PORT_ILLEGAL, indicating
   //  the port definition is not valid for the given set of 
   //  feature defines.  You can end up with a given case listed 
   //  multiple times -- a compile error.  The nested if doesn't
   //  have that problem.

   if( SIO_PORT_UART_MAIN == port_id )
   {
      #if( defined( FEATURE_FIRST_UART ))
         device_id = UART_DEVICE_1;
      #else
         device_id = UART_DEVICE_INVALID;
      #endif
   }
   else if( SIO_PORT_UART_AUX == port_id )
   {
      #if( defined( FEATURE_SECOND_UART ))
         device_id = UART_DEVICE_2;
      #else
         device_id = UART_DEVICE_INVALID;
      #endif
   }
   else if( SIO_PORT_UART_THIRD == port_id )
   {
      #if( defined( FEATURE_THIRD_UART ))
         device_id = UART_DEVICE_3;
      #else
         device_id = UART_DEVICE_INVALID;
      #endif
   }
   else if( SIO_PORT_UART_CXM == port_id )
   {
      #if( defined( FEATURE_CXM_UART ))
         device_id = UART_DEVICE_CXM;
      #else
         device_id = UART_DEVICE_INVALID;
      #endif
   }
   else
   {
      device_id = UART_DEVICE_INVALID;
   }

   return( device_id );
}

//----------------------------------------------------------------------------
//  uart_phys_device_to_dal_device_id
//----------------------------------------------------------------------------

//  Get rid of compiler warnings about unused function.

#if( defined( FEATURE_FIRST_UART) || \
     defined( FEATURE_FIRST_UART) || \
     defined( FEATURE_FIRST_UART))   //{  

static DALDEVICEID 
uart_phys_device_to_dal_device_id( int phys_device )
{
   DALDEVICEID    dal_device_id = UART_INVALID_DALDEVICEID;

   switch( phys_device )
   {
      case UART1_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTLG_DEVICE_1;    break;
      case UART2_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTLG_DEVICE_2;    break;
      case UART3_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTLG_DEVICE_3;    break;

      case UART1DM_PHYS_DEVICE:   dal_device_id = DALDEVICEID_UARTDM_DEVICE_1;    break;
      case UART2DM_PHYS_DEVICE:   dal_device_id = DALDEVICEID_UARTDM_DEVICE_2;    break; 

      case GSBI0_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_0;  break;
      case GSBI1_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_1;  break;
      case GSBI2_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_2;  break;
      case GSBI3_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_3;  break;
      case GSBI4_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_4;  break;
      case GSBI5_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_5;  break;
      case GSBI6_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_6;  break;
      case GSBI7_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_7;  break;
      case GSBI8_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_8;  break;
      case GSBI9_PHYS_DEVICE:     dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_9;  break;
      case GSBI10_PHYS_DEVICE:    dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_10; break;
      case GSBI11_PHYS_DEVICE:    dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_11; break;
      case GSBI12_PHYS_DEVICE:    dal_device_id = DALDEVICEID_UARTGSBI_DEVICE_12; break;

      case UARTBAM1_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_1;   break;
      case UARTBAM2_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_2;   break;
      case UARTBAM3_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_3;   break;
      case UARTBAM4_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_4;   break;
      case UARTBAM5_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_5;   break;
      case UARTBAM6_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_6;   break;
      case UARTBAM7_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_7;   break;
      case UARTBAM8_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_8;   break;
      case UARTBAM9_PHYS_DEVICE:  dal_device_id = DALDEVICEID_UARTBAM_DEVICE_9;   break;
      case UARTBAM10_PHYS_DEVICE: dal_device_id = DALDEVICEID_UARTBAM_DEVICE_10;  break;
      case UARTBAM11_PHYS_DEVICE: dal_device_id = DALDEVICEID_UARTBAM_DEVICE_11;  break;
      case UARTBAM12_PHYS_DEVICE: dal_device_id = DALDEVICEID_UARTBAM_DEVICE_12;  break;

      case UARTCXM_PHYS_DEVICE:   dal_device_id = DALDEVICEID_UARTCXM;            break;
   }

   return( dal_device_id );
}

//----------------------------------------------------------------------------
//  uart_device_supports_dma
//----------------------------------------------------------------------------

static boolean
uart_device_supports_dma( int phys_part )
{
   boolean supports_dma = FALSE;

   switch( phys_part )
   {
      case UART1DM_PHYS_DEVICE:
      case UART2DM_PHYS_DEVICE:

      case GSBI0_PHYS_DEVICE:
      case GSBI1_PHYS_DEVICE:
      case GSBI2_PHYS_DEVICE:
      case GSBI3_PHYS_DEVICE:
      case GSBI4_PHYS_DEVICE:
      case GSBI5_PHYS_DEVICE:
      case GSBI6_PHYS_DEVICE:
      case GSBI7_PHYS_DEVICE:
      case GSBI8_PHYS_DEVICE:
      case GSBI9_PHYS_DEVICE:
      case GSBI10_PHYS_DEVICE:
      case GSBI11_PHYS_DEVICE:
      case GSBI12_PHYS_DEVICE:

      case UARTBAM1_PHYS_DEVICE:
      case UARTBAM2_PHYS_DEVICE:
      case UARTBAM3_PHYS_DEVICE:
      case UARTBAM4_PHYS_DEVICE:
      case UARTBAM5_PHYS_DEVICE:
      case UARTBAM6_PHYS_DEVICE:
      case UARTBAM7_PHYS_DEVICE:
      case UARTBAM8_PHYS_DEVICE:
      case UARTBAM9_PHYS_DEVICE:
      case UARTBAM10_PHYS_DEVICE:
      case UARTBAM11_PHYS_DEVICE:
      case UARTBAM12_PHYS_DEVICE:

          supports_dma = TRUE;
          break;
   }

   return( supports_dma );
}

//----------------------------------------------------------------------------
//  uart_get_device_info
//
//  Description:
//  Isolate all featurizations in this one function.  This function
//  turns all of the compile time switches (featurization) into a run 
//  time accessible structure.  That way, all of the ugliness of featurization 
//  can be hidden in this function and not pollute the rest of the code.
//
//  Returns TRUE if device is valid, FALSE if the device is not valid
//
//----------------------------------------------------------------------------

boolean                  
uart_get_device_info( uart_device_id_type device_id, uart_device_info* pInfo )
{
   int              driver_type;
   int              phys_device;
   DALDEVICEID      dal_device_id;

   UART_LOG_1( TRACE, "[%d] +uart_get_device_info", device_id );

   pInfo->is_present = FALSE;

   //  Isolate all featurizations in this one function.  This function
   //  turns all of the compile time switches (featurization) into a run 
   //  time accessible structure.  That way, all of the ugliness of featurization 
   //  can be hidden in this function and not pollute the rest of the code.

   if( !(( UART_DEVICE_FIRST <= device_id) && ( device_id < UART_DEVICE_LAST )))
   {
      UART_LOG_1( ERROR, "[%d] -uart_get_device_info: unrecognized device ID", device_id );
      goto error;
   }

   //  Set the physical device and driver type.

   phys_device = NO_PHYS_DEVICE; 
   driver_type = NO_DRIVER;

   switch( device_id )
   {
      case UART_DEVICE_1:

         #if( defined( FEATURE_FIRST_UART ))
            #if( defined( UART_FIRST_UART_PHYS_DEVICE ))
               phys_device = UART_FIRST_UART_PHYS_DEVICE;
            #endif
            #if( defined( UART_FIRST_UART_DRIVER_TYPE ))
               driver_type = UART_FIRST_UART_DRIVER_TYPE;
            #endif
            #if( defined( UART_FIRST_UART_PD_WAKE_ON_RX ))
               pInfo->can_wake_on_rx_event = UART_FIRST_UART_PD_WAKE_ON_RX;
            #endif
         #endif
         break;

      case UART_DEVICE_2:

         #if( defined( FEATURE_SECOND_UART ))
            #if( defined( UART_SECOND_UART_PHYS_DEVICE ))
               phys_device = UART_SECOND_UART_PHYS_DEVICE;
            #endif
            #if( defined( UART_SECOND_UART_DRIVER_TYPE ))
               driver_type = UART_SECOND_UART_DRIVER_TYPE;
            #endif
            #if( defined( UART_SECOND_UART_PD_WAKE_ON_RX ))
               pInfo->can_wake_on_rx_event = UART_SECOND_UART_PD_WAKE_ON_RX;
            #endif
         #endif
         break;

      case UART_DEVICE_3:

         #if( defined( FEATURE_THIRD_UART ))
            #if( defined( UART_THIRD_UART_PHYS_DEVICE ))
               phys_device = UART_THIRD_UART_PHYS_DEVICE;
            #endif
            #if( defined( UART_THIRD_UART_DRIVER_TYPE ))
               driver_type = UART_THIRD_UART_DRIVER_TYPE;
            #endif
            #if( defined( UART_THIRD_UART_PD_WAKE_ON_RX ))
               pInfo->can_wake_on_rx_event = UART_THIRD_UART_PD_WAKE_ON_RX;
            #endif
         #endif
         break;

      case UART_DEVICE_CXM:

         #if( defined( FEATURE_CXM_UART ))
            #if( defined( UART_CXM_UART_PHYS_DEVICE ))
               phys_device = UART_CXM_UART_PHYS_DEVICE;
            #endif
            #if( defined( UART_CXM_UART_DRIVER_TYPE ))
               driver_type = UART_CXM_UART_DRIVER_TYPE;
            #endif
            pInfo->can_wake_on_rx_event = FALSE;
         #endif
         break;

      default:

         UART_LOG_1( ERROR, "[%d] is not first, second, third, or cxm UART", device_id );
         goto error;
   }

   UART_LOG_2( TRACE, "[%d] phys_device: %d", device_id, phys_device );
   UART_LOG_2( TRACE, "[%d] driver_type: %d", device_id, driver_type );
   UART_LOG_2( TRACE, "[%d] can_wake_on_rx_event: %d", device_id, pInfo->can_wake_on_rx_event );

   //  Set the rest of the data in the info structure based
   //  on the physical device and driver type.

   dal_device_id = uart_phys_device_to_dal_device_id( phys_device );
   if( dal_device_id == UART_INVALID_DALDEVICEID )
   {
      pInfo->is_present = FALSE;
      goto error;
   }
   else
   {
      pInfo->is_present = (phys_device != NO_PHYS_DEVICE);
      pInfo->dal_device_id = dal_device_id;
   }

   UART_LOG_2( TRACE, "[%d] dal_device_id: 0x%08x", device_id, dal_device_id );

   pInfo->enable_dma = FALSE;
   switch( driver_type )
   {
      case BAM_DRIVER:
      case DM_DRIVER:

         pInfo->enable_dma = TRUE;
         break;
   }

   if( pInfo->enable_dma && !uart_device_supports_dma( phys_device ))
   {
      UART_LOG_1( ERROR, "[%d] does not support DMA", device_id );
      goto error;
   }

   if( driver_type == NO_DRIVER )
   {
      UART_LOG_1( ERROR, "[%d] NO DRIVER", device_id );
      goto error;
   }

   UART_LOG_1( TRACE, "[%d] -uart_get_device_info", device_id );
   pInfo->is_valid = TRUE;
   return ( TRUE );

error:
   UART_LOG_1( TRACE, "[%d] -uart_get_device_info (error)", device_id );
   pInfo->is_valid = FALSE;
   return( FALSE );
}

#endif //}


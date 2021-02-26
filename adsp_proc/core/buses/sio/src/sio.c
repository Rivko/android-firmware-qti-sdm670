/*====*====*====*====*====*===*====*====*====*====*====*====*====*====*====*/
/*
                            S I O    
                            
                       SIO Server Layer

GENERAL DESCRIPTION
  This module contains the interface layer for SIO.  
  
EXTERNALIZED FUNCTIONS

  sio_init()
    Initialize SIO.
                                                     
  sio_open()
    This procedure opens a stream across the serial link.
    
  sio_close()
    This procedure closes a stream.

  sio_transmit()
    Transmit data.

  sio_ioctl()
    Control open stream.
         
  sio_flush_tx()
    Transmits any queued data and then call the registered callback.

  sio_convert_fctl()
    Conversion utility between DS flow control values and SIO flow control
    values.

  sio_disable_device()
    Disables given device, including turning off clock to save power.  

  sio_is_baud_valid()
    Check whether the specified baud rate is valid under the current
    feature set.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

  sio_init()     must be called by a task upon power-up.  This needs to be done
                 once and only once.  
                 
  sio_open()     must be called by each caller (task or ISR) that wants to use 
                 SIO services.  
                 
  sio_close()    needs to be called when the caller wants to give up the use
                 of SIO services to allow some other caller to use SIO.
  
Copyright (c) 1998-2009.2012-2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/13   rc	   Fixed the klock work warning for checking the major number,minor number.
08/27/09   amv     Fix for forward bridge compatiblity
06/23/09   amv     Added sio critical sections, smd bridge forward compatiblity 
                   and 8200 target specific changes
09/18/08   vkm     SIO Defeaturization, Making SIO a DLL, Commonizing WM 6 and WM 7 SIO
09/12/08   amv     Added SMD ports for MODEM, APP and QDSP processor
09/12/08   amv     Reverted DLL changes made by Jim except sio.def
01/07/08   jmm     Revised to change from sio static library to DLL.  Added DllMain();
                   changed from using INTLOCK/INTFREE to WinCE mutex.  Added debug msgs.
11/09/07   jmm     Fixed warning for missing header file
08/24/07   rh      There may be trouble with high speed USB driver
07/30/07   rh      Created WM specific branch
10/13/06   TMR     Merging in changes for supporting QMI bridges
10/11/06   rh      Adding support for FEATURE_HS_USB
09/25/06   rh      Add support for custom baud rate
09/19/06   dsn     Reinsert changes to exclude BT ports from SMD bridge
09/18/06   ptm     Remove separate close callback handling by bridge code.
09/01/06   bfc     Featurized smd_port_mgr
08/23/06   sg      Remove support for brew extended device.
05/24/06   bfc     Cleaned up some #ifdefs
07/31/06   sr      Add support for 3000kbps
07/27/06   rh      Add support for sio port SIO_PORT_SMD_GPS_NMEA
07/17/06   taw     Added support for SMD bridging on apps when USB is on apps.
06/27/06   gd      Added code to work USB Diag on apps side which was there 
                   in intial version, removed in second version. Added again 
                   in this version under FEATURE_USB_ON_APPS_PROC.
06/26/06   gd      Removed MMC feature hack.
06/19/06   bt      Added FEATURE_USB_BREW_EXTENDED_DEVICE support. 
06/12/06   dsn     Exclude SIO_PORT_BT_SPP, SIO_PORT_BT_NA from SMD bridge
05/24/06   bfc     Cleaned up some #ifdefs
05/18/06   sr      Add support for 921.6kbps
05/01/06   bfc     Added featurization for smd_bridge related code
04/25/06   ptm     Added SMD port for GPS NMEA and RPC reply.
03/28/06   bfc     Changed the arguement for extended callback function from
                   a uint32 to a void *
02/28/06   vs      Removed port_id from sio_close_ext and sio_flush_tx_ext
01/26/06   bfc     Added sio_close_ext and sio_flush_tx_ext which work 
                   specifically with SMD streams.
01/18/06   TMR     Added FEATURE_NO_USB_HW_ON_PROCESSOR
01/04/06   ptm     Restore SMEM code, expanded SMD ports as required.
11/18/05   vs      Removed / supressed lint warnings
09/28/05   TMR     Added MSM7500 bridging capability, removed SIO_PORT_SMEMDS_DIAG_RSP
                   and added smd_sio_dev_init()
07/24/05   vs      7500 featurizes first UART, for all other targets this feature 
                    should be defined. 
06/05/05   TMR     Merged in MSM7500 code
06/01/05   adm     Added support for additional SMD DIAG pipe SIO_PORT_SMEMDS_DIAG_RSP
04/14/05   adm     Added smd_sio related calls.
04/14/05   tmr     Removing RDM calls from apps build.
04/14/05   adm     Added smd_sio related calls.
<<<<
03/24/05   jhs     Removed T_RUMI2 protection from sio_usb_dev_init call.
11/09/04   adm     Modified sio_get_stream_id to return fixed stream_is'd
                   in case of SMEMDS related ports 
10/20/04   ptm     Enabled more routines in sio_init for msm7500/RUMI builds.
10/20/04   adm     Added support for featurization of UART1. Featurized USB init.
                   Added support for multiple DS pipes.
04/20/04   pn      Added support for FEATURE_BT in sio_flush_tx().
04/06/05   cc      Corporate in code review comments. 
                   Move MSG_ERROR out of INTLOCK/INTFREE. 
                   Control stream not available for bulk only USB_MMC device.  
03/08/05   cc      Merged in OSIFDev_Dec04 branck. 
12/23/04   cc      Renamed SIO_PORT_USB_CDC_ACM, _DIAG, _NMEA to be 
                   SIO_PORT_USB_MDM,_SER1 and _SER2 respectively. 
                   Added sio_control_stream_port to memorize the port opened
                   each control stream. 
11/24/04   cc      Added SIO_PORT_USB_RRDM and SIO_PORT_USB_NET_WWAN support. 
                   Added sio_control_open/close/transmit() APIs. 
04/20/04   pn      Added support for FEATURE_BT in sio_flush_tx().
11/24/03   cc      Added FEATURE_JCDMA_SIO and OR it with FEATURE_JCDMA_DS.                     
02/21/03   cc      Added FEATURE_USB_DIAG_NMEA support. 
10/17/02   cc      Added FEATURE_JCDMA_1X support. 
07/31/02   cc      added pkt_ptr handling in sio_transmit(), UART, USB and BT 
                   would be able to handling pkt_ptr.
07/15/02   ry      Added support for the LSM_CSD port
07/15/02   ak      Added FEATURE_DATA_MM support.
03/18/02   cc      Changed ASSERT to ERR if(tx_ptr == NULL) in sio_transmit().
                   This fixes CR19326/19895. 
                   Also, added SIO_PORT_USB_MMC handling for bit_rate changing
                   in sio_open().  
02/06/02   cc      Add third UART support.
10/16/01   cc      Remove code dependency on T_MSM3.
10/16/01   cc      Merge in mods of SIMBA from MSM5200_SIO.00.00.17, includes 
                   following 1 item.
05/01/01   dwp     Change arg type for INTLOCK_SAV() to uint32.
06/18/01   dwp     Add support for FEATURE_UART_RATE1.
06/01/01   dwp     Removed dependencies on DS.
03/26/01   dl      Add support for SMEM_DS port.
01/26/01   ych     Merged T-53/JCDMA features for SK.
01/09/01   dwp     Removed bt_sio_cmd_initialize() from sio_init().
11/27/00   dwp     Change multiple SIO_PORT_BT_RFCOMM's to SIO_PORT_BT_SPP.
                   Added support for SIO_PORT_BT_NA.
11/17/00   dwp     Added support for MMC over USB.
11/14/00   dwp     Added support for FEATURE_RUNTIME_DEVMAP.
11/01/00   dwp     Removed call to dsm_init().
09/05/00   dwp     Additonal modifications for FEATURE_BT.
07/27/00   dwp     Added support for FEATURE_USB_DIAG and FEATURE_BT.
05/23/00   jwh     Added sio_is_baud_valid(), mainly to check NV values.
09/09/99   jwh     Added support for FEATURE_SERIAL_DEVICE_MAPPER with a
                   STATIC runtime port map.  Function names to all lower case.
                   SIO Port name changes.  File "siodev.h" became 
                   "siors232.h."  Improved comments. Merge with MSM3100 1.0C 
                   changes.
04/09/99   jwh     Moved #include "comdef.h" outside of conditional file 
                   inclusion to avoid "no external declaration . . ."
                   compiler warning.
03/08/99   jwh     Removed the dependency of "FEATURE_SECOND_UART" on
                   "#ifdef T_SURF".
02/24/99   ak      Removed warning in sio_tx_flush (unused local var)
02/17/99   ak      Added function sio_tx_flush.
01/17/99   ak      Featurized entire file based on whether or not user wants
                   to use old or new SIO (FEATURE_NEW_SIO)
08/03/98   jjn     Created module.
===========================================================================*/


/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

/*---------------------------------------------------------------------------
 Need to include this header file before any reference to T_MSM3.
---------------------------------------------------------------------------*/
#ifndef UNDER_CE
#include "target.h"                /* Target specific definitions          */
#else
#include <windows.h>
#endif

/**---------------------------------------------------------------------------
 Need an external declaration regardless of feature to avoid a compiler warning.
---------------------------------------------------------------------------*/
#include "comdef.h"                /**< Definitions for byte, word, etc.     */
#include "sio.h"                   /**< Serial I/O Services.                 */
#include "sio_priv.h"
#include "sio_wrapper.h"           /**< Adaptation layer */
#include "msg.h"                   /**< Message Logging Services.            */
#include "err.h"                   /**< Error Handling Services.             */
#include "assert.h"                /**< Assert Services.                     */
#ifndef UNDER_CE
#include "customer.h"              /**< Customer specific configuration.     */
#endif
#include "memory.h"                /**< Standard memory management routines. */

/*===========================================================================

                     CONSTANT AND VARIABLE DECLARATIONS

===========================================================================*/
/** Array indicating whether or not given stream has been allocated SIO
   resources.                                                              */
boolean              sio_stream_opened[SIO_MAX_STREAM];
/** Array indicating port used by stream with given stream
 *  ID. */
sio_port_id_type     sio_stream_port[SIO_MAX_STREAM];
/** points to the current available stream id in pool   */
uint32 sio_stream_mark = 0;
/** array of driver_interface */
sio_driver_interface        sio_driver_list[SIO_MAX_MAJOR_NUM];
/** array of device interface */
sio_device_interafce        sio_device_list[SIO_MAX_MAJOR_NUM][SIO_MAX_MINOR_NUM];
/** list of driver's init_func pointers */
sio_driver_init_func_type   sio_driver_init_list[SIO_MAX_DRIVERS];
/*===========================================================================

                                FUNCTIONS

===========================================================================*/
#ifdef UNDER_CE
CRITICAL_SECTION g_cs_SIO;
#define sio_enter_crit_section()      EnterCriticalSection(&g_cs_SIO)
#define sio_leave_crit_section()      LeaveCriticalSection(&g_cs_SIO)
#endif

/**==========================================================================

FUNCTION sio_null_xxx                         EXTERNALIZED FUNCTION

@brief
  These function are a set of null function that is executed when the registered
  driver do not register for a particular API

@dependencies
  None

@return
  sio_null_open and sio_null_close returns SIO_DONE_S

@sideeffects
  None
  
===========================================================================*/
static sio_status_type sio_null_open (sio_open_type *open_ptr)
{
    MSG_ERROR("SIO port %x  does not support open interface", open_ptr->port_id, 0, 0);
    return SIO_UNAVAIL_S;
}

static void sio_null_close (sio_stream_id_type stream_id,
                     sio_port_id_type      port_id,
                        void (*close_func_ptr)(void))
{
    MSG_ERROR("SIO port %d  does not support close interafce", stream_id , 0, 0);
}

static void sio_null_ioctl (sio_stream_id_type stream_id,
                      sio_port_id_type      port_id,
                      sio_ioctl_cmd_type    cmd,
                      sio_ioctl_param_type *param)
{
    MSG_ERROR("SIO port %x  does not support ioctl interface", port_id, 0, 0);
}


static void sio_null_transmit (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         dsm_item_type *tx_ptr)
{
    MSG_ERROR("SIO port %x  does not support transmit interface", port_id, 0, 0);
    ASSERT(FALSE); /* ASSERTING on null_transmit call as it can cause memory leaks,
                      Incorrect usage of apis, when not supported */
}


static void sio_null_flush_tx (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         void (*flush_func_ptr)(void))
{
    MSG_ERROR("SIO port %x  does not support flush interface", port_id, 0, 0);
}

static void sio_null_disable_device (sio_port_id_type port_id)
{
    MSG_ERROR("SIO port %x  does not support disable_device interface", port_id, 0, 0);
}

static sio_status_type sio_null_control_open (sio_open_type *open_ptr)
{
    MSG_ERROR("SIO port %x  does not support control open interface", open_ptr->port_id, 0, 0);
    return SIO_UNAVAIL_S;
}

static void sio_null_control_close (sio_stream_id_type stream_id,
                             sio_port_id_type      port_id,
                              void (*close_func_ptr)(void))
{
    MSG_ERROR("SIO port %x does not support control_close interface", port_id, 0, 0);
}

static void sio_null_control_ioctl (sio_stream_id_type stream_id,
                             sio_port_id_type      port_id,
                             sio_ioctl_cmd_type    cmd,
                             sio_ioctl_param_type *param)
{
    MSG_ERROR("SIO port %x  does not support control_ioctl interface", port_id, 0, 0);
}

static void sio_null_control_transmit (sio_stream_id_type stream_id,
                                sio_port_id_type      port_id,
                                 dsm_item_type     *tx_ptr)
{
    MSG_ERROR("SIO port %x does not support control_transmit interface", port_id, 0, 0);
}

static boolean sio_check_device_id(uint32 device_id,uint32 *major_num_ptr,uint32 *minor_num_ptr)
{
    if(device_id <= SIO_PORT_NULL)
    {
        MSG_ERROR("incorrect device_id, device_id = 0x%x",device_id,0,0);
        return FALSE;
    }    
    *major_num_ptr = SIO_MAJOR_NUM_OF_DEVICE(device_id);
    *minor_num_ptr = SIO_MINOR_NUM_OF_DEVICE(device_id);
    if( (*major_num_ptr > SIO_MAX_MAJOR_NUM) || (*minor_num_ptr > SIO_MAX_MINOR_NUM))
    {
        MSG_ERROR("incorrect magic number, device_id = 0x%x",device_id,0,0);
        return FALSE;
    }
    return TRUE;
}

/**==========================================================================

FUNCTION sio_register_driver                                EXTERNAL FUNCTION

@brief
  Register driver interface with SIO 
      
@dependencies
  None
                                                       
@return
  0 - Failed to register the driver with sio service
  1 - Success
    
@sideeffects
  None
  
===========================================================================*/

uint32 sio_register_driver(uint32 major_num, sio_driver_interface * driver_iface_ptr) 
{
    /* To prevent more than one process from registering driver */
    sio_enter_crit_section();                   

    ASSERT(major_num < SIO_MAX_MAJOR_NUM);
   
    if(driver_iface_ptr->HW_open != NULL)
        sio_driver_list[major_num].HW_open =                driver_iface_ptr->HW_open;
    else
        sio_driver_list[major_num].HW_open =                sio_null_open;

    if(driver_iface_ptr->HW_close != NULL)
        sio_driver_list[major_num].HW_close =               driver_iface_ptr->HW_close;
    else
        sio_driver_list[major_num].HW_close =               sio_null_close;

    if(driver_iface_ptr->HW_ioctl != NULL)
        sio_driver_list[major_num].HW_ioctl =               driver_iface_ptr->HW_ioctl;
    else
        sio_driver_list[major_num].HW_ioctl =               sio_null_ioctl;

    if(driver_iface_ptr->HW_transmit != NULL)
        sio_driver_list[major_num].HW_transmit =            driver_iface_ptr->HW_transmit;
    else
        sio_driver_list[major_num].HW_transmit =            sio_null_transmit;

    if(driver_iface_ptr->HW_flush_tx != NULL)
        sio_driver_list[major_num].HW_flush_tx =            driver_iface_ptr->HW_flush_tx;
    else
        sio_driver_list[major_num].HW_flush_tx =            sio_null_flush_tx;

    if(driver_iface_ptr->HW_disable_device != NULL)
        sio_driver_list[major_num].HW_disable_device =      driver_iface_ptr->HW_disable_device;
    else
        sio_driver_list[major_num].HW_disable_device =      sio_null_disable_device;

    if(driver_iface_ptr->HW_control_open != NULL)
        sio_driver_list[major_num].HW_control_open =        driver_iface_ptr->HW_control_open;
    else
        sio_driver_list[major_num].HW_control_open =        sio_null_control_open;

    if(driver_iface_ptr->HW_control_close != NULL)
        sio_driver_list[major_num].HW_control_close =       driver_iface_ptr->HW_control_close;
    else
        sio_driver_list[major_num].HW_control_close =       sio_null_control_close;

    if(driver_iface_ptr->HW_control_ioctl != NULL)
        sio_driver_list[major_num].HW_control_ioctl =       driver_iface_ptr->HW_control_ioctl;
    else
        sio_driver_list[major_num].HW_control_ioctl =       sio_null_control_ioctl;

    if(driver_iface_ptr->HW_control_transmit != NULL)
        sio_driver_list[major_num].HW_control_transmit =    driver_iface_ptr->HW_control_transmit;
    else
        sio_driver_list[major_num].HW_control_transmit =    sio_null_control_transmit;

    sio_driver_list[major_num].is_registered = TRUE;

    sio_leave_crit_section();
    return 1;
}
/**==========================================================================

FUNCTION sio_register_device_driver                                EXTERNAL FUNCTION

@brief
  Register SIO devices of driver with SIO 
      
@dependencies
  None
                                                       
@return
  0 - Failed to register the SIO device with SIO service
  1 - Success
    
@sideeffects
  None
  
===========================================================================*/

uint32 sio_register_device_driver(sio_device_interafce * device_iface_ptr)
{
 uint32 major_num, minor_num, device_id;

 /* To prevent more than one process from registering driver */
 sio_enter_crit_section();                   
 device_id = device_iface_ptr->device_id;
 if(!(sio_check_device_id(device_id,&major_num,&minor_num)))
 {
     sio_leave_crit_section();	 
     return 0;
 }
 sio_device_list[major_num][minor_num].magic_num =          SIO_DEVICE_HEAD_MAGIC_NUM;
 sio_device_list[major_num][minor_num].device_id =          device_iface_ptr->device_id;
// sio_device_list[major_num][minor_num].forward_port =       device_iface_ptr->forward_port;
 sio_device_list[major_num][minor_num].driver_available =   device_iface_ptr->driver_available;
// sio_device_list[major_num][minor_num].forward_port_id =    device_iface_ptr->forward_port_id;

 if(!sio_driver_list[major_num].is_registered)
 {
     sio_register_driver(major_num, device_iface_ptr->driver_interface);
 }
 if(device_iface_ptr->driver_available)
 {
  sio_device_list[major_num][minor_num].driver_interface = &sio_driver_list[major_num];
 }
 else
 {
  sio_device_list[major_num][minor_num].driver_interface = &sio_driver_list[SIO_NULL_DRIVER];   
 }

 sio_leave_crit_section();
 return 1;
}


/**==========================================================================

FUNCTION sio_get_driver_interface                                EXTERNAL FUNCTION

@brief
  Gets driver interface associated with device_id 
      
@dependencies
  None
                                                       
@return
   pointer to driver interface
    
@sideeffects
  None
 ============================================================================*/

sio_driver_interface*  sio_get_driver_interface(uint32 device_id)
{
 uint32 major_num, minor_num;
 sio_driver_interface* return_driver_iface_ptr = NULL;

 if(!(sio_check_device_id(device_id,&major_num,&minor_num)))
 {
     return return_driver_iface_ptr;
 }
 sio_enter_crit_section();
 if(sio_device_list[major_num][minor_num].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
 {
     ERR( "Incorrect magic number, Driver registration error ! device_id = 0x%x",device_id,0,0);
     return_driver_iface_ptr = &sio_driver_list[SIO_NULL_DRIVER];
 }
 else
 {
     ASSERT((sio_device_list[major_num][minor_num].driver_interface) != NULL);
     return_driver_iface_ptr = (sio_device_list[major_num][minor_num].driver_interface);
 }
 sio_leave_crit_section();
 return return_driver_iface_ptr;
}
/**==========================================================================

FUNCTION SIO_GET_STREAM_ID                                  REGIONAL FUNCTION

@brief
  Return stream ID which is not currently allocated (i.e. opened).  
  INTLOCK statement and INTFREE statement makes this function
  re-entrant.
      
@dependencies
  None
                                                       
@return
  First available stream ID. 
  If no stream identifiers are available, return SIO_NO_STREAM_ID.
    
@sideeffects
  None
  
===========================================================================*/

sio_stream_id_type sio_get_stream_id( void)
{
  uint32               i;           /* Counter                               */
  sio_stream_id_type stream_id = SIO_NO_STREAM_ID;  /* Return Value          */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  sio_enter_crit_section();
  for(i = sio_stream_mark; i < SIO_MAX_STREAM; i++)
  {
    if (sio_stream_opened[i] == FALSE)
    {
      stream_id = (sio_stream_id_type)i;
      break;  
    }
  }
  if(stream_id == SIO_NO_STREAM_ID)
  {
      for (i = 0; i < sio_stream_mark; i++)
      {
        if (sio_stream_opened[i] == FALSE)
        {
          stream_id = (sio_stream_id_type)i;
          break;  
        }
      }
  }

  if (stream_id != SIO_NO_STREAM_ID)
  {
    sio_stream_opened[stream_id] = TRUE;
    sio_stream_mark = (stream_id+1)%SIO_MAX_STREAM;      // pointing to the next stream_id.
  }
  
  sio_leave_crit_section();

  return stream_id;
} /* sio_get_stream_id */

/*lint restore*/
/**==========================================================================

FUNCTION SIO_PUT_STREAM_ID                                  REGIONAL FUNCTION

@brief
  Allows caller to deallocate or put back stream ID previously allocated.
  INTLOCK & INTFREE statements make this procedure re-entrant.
      
@dependencies
  None
                                                       
@return
  None
  
@sideeffects
  None
  
===========================================================================*/

void sio_put_stream_id( stream_id_type stream_id)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( stream_id < SIO_MAX_STREAM);

  sio_enter_crit_section();

  sio_stream_opened[ stream_id] = FALSE;
  sio_stream_port[ stream_id] = SIO_PORT_NULL;

  sio_leave_crit_section();

} /* sio_put_stream_id */


/**==========================================================================

FUNCTION SIO_STREAM_TO_PORT                                    REGIONAL FUNCTION

@brief
  Checks whether stream is open and returns Port ID
      
@dependencies
  None
                                                       
@return
  Port ID:  If stream open
  SIO_PORT_NULL: If stream is not open
  
@sideeffects
  None
  
===========================================================================*/

sio_port_id_type sio_stream_to_port( stream_id_type stream_id)
{
  sio_port_id_type   port_id;               /* Port which corresponds to
                                               stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sio_enter_crit_section();
  if ( sio_stream_opened[ stream_id] == FALSE 
       || sio_stream_port[stream_id] == SIO_PORT_NULL)
  {
    ERR( "Attempt to access stream which was not opened or already closed",0,0,0);
    port_id = SIO_PORT_NULL;
  }
  else
  {
    port_id = sio_stream_port[ stream_id];
  }
  sio_leave_crit_section();
  return port_id;

} /* sio_stream_to_port */

/**==========================================================================

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

void sio_initialize_stream_opened( void)
{
  memset( sio_stream_opened, FALSE, SIO_MAX_STREAM);

  memset( sio_stream_port, (int) SIO_PORT_NULL, SIO_MAX_STREAM);

  return;
} /* sio_initialize_stream_opened */

/**==========================================================================

FUNCTION sio_register_null_driver                         INTERNAL FUNCTION

@brief
  registers null driver
  
@dependencies
  None
  
@return
  None
  
@sideeffects
  None
  
===========================================================================*/
void sio_register_null_driver(void)
{
  sio_driver_interface driver_iface;
  driver_iface.HW_close = NULL;
  driver_iface.HW_control_close = NULL;
  driver_iface.HW_control_ioctl = NULL;
  driver_iface.HW_control_open = NULL;
  driver_iface.HW_control_transmit = NULL;
  driver_iface.HW_disable_device = NULL;
  driver_iface.HW_disable_device = NULL;
  driver_iface.HW_flush_tx = NULL;
  driver_iface.HW_ioctl = NULL;
  driver_iface.HW_open = NULL;
  driver_iface.HW_transmit = NULL;
  sio_register_driver(SIO_NULL_DRIVER, &driver_iface);

  memset(sio_device_list, 0, sizeof(sio_device_list));
}

/**==========================================================================

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
void sio_driver_init(void)
{
 uint32 driver_index, ret=0;
 sio_driver_init_func_type init_function;

 sio_set_driver_init_list();

 for(driver_index = 0; driver_index < SIO_MAX_DRIVERS ; driver_index++)
 {
     init_function = sio_driver_init_list[driver_index];
     if(init_function != NULL)
     {
         ret = init_function(sio_register_device_driver);
         if(!ret)
         {
             ERR( "SIO could not initialize driver %d",driver_index,0,0);
             return;
         }
     }
 }
}


/**==========================================================================

FUNCTION SIO_INIT                                       EXTERNALIZED FUNCTION

@brief
  Initializes SIO.
  
@dependencies
  Should be called just once.
  
@return
  None
  
@sideeffects
  None
  
===========================================================================*/
void sio_init ( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize everything in a critical section.
  */

  sio_init_rdm_sdm();

  sio_initialize_stream_opened();

  /* Register NULL device driver */
  sio_register_null_driver();

  /* Initialize all the driver to register their ports */ 
  sio_driver_init();


} /* sio_init */

/**==========================================================================

FUNCTION SIO_OPEN                                       EXTERNALIZED FUNCTION

@brief
  This function opens a stream across the serial link.
      
@dependencies
  sio_init must have been called.  sio_init needs to be called by just one
  task and only once upon power-up.
      
@return
  SIO_NO_STREAM_ID:  The open request could not be completed. (ERROR)

  If successful, the "stream_id" is returned for use in future SIO requests.
    
@sideeffects
  None
  
===========================================================================*/

sio_stream_id_type sio_open
( 
  sio_open_type *open_ptr                       /* SIO Open structure      */
)
{
  sio_stream_id_type   stream_id;               /* Stream ID               */
  sio_status_type      return_val = SIO_DONE_S; /* Return Value            */
  boolean              rx_defined = FALSE;      /* Is RX defined?          */
  boolean              tx_defined = FALSE;      /* Is TX defined?          */
  sio_ioctl_param_type param;                   /* IOCTL parameter         */
  sio_port_id_type     port_id;                 /* Port ID                 */
  sio_driver_interface * driver_iface;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( open_ptr != NULL);

  port_id = open_ptr->port_id;
  /* Get stream id */
  stream_id = sio_get_stream_id();

  /* If there are no more stream identifiers available,
  ** indicate error and return immediately.  */
  if ( stream_id == SIO_NO_STREAM_ID)
  {
    ERR( "No Stream IDs available",0,0,0);
    return stream_id; 
  }

  open_ptr->stream_id = stream_id;

  if ( open_ptr->stream_mode >= SIO_MAX_MODE)
  { 
    ERR_FATAL( "Invalid stream mode ! port_id = 0x%x",port_id,0,0);
  } 

  if ( ( open_ptr->rx_queue    == NULL) && 
       ( open_ptr->tx_queue    == NULL) && 
       ( open_ptr->rx_func_ptr == NULL))
  {
    ERR_FATAL( "sio_open called with both RX and TX undefined ! port_id = 0x%x",port_id,0,0);
  }

  /* If tx_queue is defined, a method of handing data to
  ** caller is defined.  Therefore, we take the fact that
  ** tx_queue is defined as evidence that TX is defined.  */  
  if ( open_ptr->tx_queue)
  {
    tx_defined = TRUE;  
  } 
  
  /* If either rx_queue or rx_func_ptr are defined, then
  ** some method of handing data to caller (task or ISR
  ** which called this routine) is defined.  So, we take
  ** the fact that either of these parameters are defined
  ** as evidence that RX is defined.  */
  if ( ( open_ptr->rx_queue) || ( open_ptr->rx_func_ptr))
  {
    rx_defined = TRUE;  
  }
  
  if ( rx_defined && tx_defined)
  {
    open_ptr->stream_type = SIO_STREAM_RXTX;  
  }
  else if ( rx_defined)
  {
    open_ptr->stream_type = SIO_STREAM_RX_ONLY;  
  }
  else
  {
    open_ptr->stream_type = SIO_STREAM_TX_ONLY;  
  }

  /* Calling the driver that is registered with SIO */
  driver_iface = sio_get_driver_interface(port_id);
  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_open, port_id: %d", port_id, 0, 0 );
     return SIO_NO_STREAM_ID;
  }    
  return_val = (driver_iface->HW_open) (open_ptr);

  if ( return_val != SIO_DONE_S)
  {
    /* If we could not allocate TX, exit */
    sio_put_stream_id( stream_id);
    /* Restore previously saved port_id */
    open_ptr->port_id = port_id;
    return SIO_NO_STREAM_ID;  
  }
  
  /* Save port ID */
  sio_enter_crit_section();
  sio_stream_port[ stream_id] = port_id;
  sio_leave_crit_section();

  /* Set flow control method.
  */
  param.flow_ctl.tx_flow = open_ptr->tx_flow;
  param.flow_ctl.rx_flow = open_ptr->rx_flow;

  sio_ioctl( stream_id, SIO_IOCTL_SET_FLOW_CTL, &param);

  /* Restore previously saved port_id */
  open_ptr->port_id = port_id;
  return stream_id;
} /* sio_open */


/**==========================================================================

FUNCTION SIO_CLOSE                                      EXTERNALIZED FUNCTION
                                                                             
@brief
  This procedure does all the book-keeping necessary to close a stream.  
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.
                  
@dependencies
  None
      
@return
  None
  
@sideeffects
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

void sio_close
( 
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
)
{
   sio_port_id_type   port_id;               /* Port which corresponds to
                                               stream_id                   */
   sio_driver_interface * driver_iface;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  sio_enter_crit_section();
  port_id = sio_stream_to_port(stream_id);

  driver_iface = sio_get_driver_interface(port_id);
  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_close, stream_id: %d", stream_id, 0, 0 );
     sio_leave_crit_section();
     return;
  }
  (driver_iface->HW_close) (stream_id,port_id, close_func_ptr);

  sio_put_stream_id( stream_id);
  sio_leave_crit_section();
} /* sio_close */


/**==========================================================================

FUNCTION SIO_FLUSH_TX                                  EXTERNALIZED FUNCTION
                                                                             
@brief
  This function will cause any queued data to be sent out of the UART, with 
  the caveat that a callback will be called when it's done.  This assumes
  that the user won't queue more data until after the callback is executed.
                    
@dependencies
  None
      
@return
  None
  
@sideeffects
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

void sio_flush_tx
( 
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*flush_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
)
{
  sio_port_id_type   port_id;               /* Port which corresponds to
                                               stream_id                   */
  sio_driver_interface * driver_iface;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  port_id = sio_stream_to_port(stream_id);
  driver_iface = sio_get_driver_interface(port_id);
  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_flush_tx, stream_id: %d", stream_id, 0, 0 );
     return;
  }
  (driver_iface->HW_flush_tx) (stream_id, port_id, flush_func_ptr);

} /* sio_flush_tx */

/**==========================================================================

FUNCTION SIO_TRANSMIT                                   EXTERNALIZED FUNCTION

@brief
  This function will transmit a packet or if transmitter is busy, schedule
  the transmission of a packet.

@dependencies
  None
  
@return
  None
  
@sideeffects
  None
  
===========================================================================*/
void sio_transmit
( 
  sio_stream_id_type stream_id,           /* Stream ID                     */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
  sio_port_id_type   port_id;             /* SIO Port ID                   */
  sio_driver_interface * driver_iface;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( tx_ptr == NULL) 
  {
     MSG_ERROR("NULL dsm_item ptr for transmit, stream_id: %d", stream_id, 0, 0 );
     return;
  }

  port_id = sio_stream_to_port(stream_id);
  driver_iface = sio_get_driver_interface(port_id);
  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_transmit, stream_id: %d", stream_id, 0, 0 );
     return;
  }
  (driver_iface->HW_transmit) (stream_id, port_id, tx_ptr);

} /* sio_transmit */


/**==========================================================================

FUNCTION SIO_IOCTL                                      EXTERNALIZED FUNCTION

@brief
  Allows control of open stream.

@dependencies
  None

@return
  None

@sideeffects
  None
  
===========================================================================*/
void sio_ioctl
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
)
{
   sio_port_id_type   port_id;             /* SIO Port ID                   */
   sio_driver_interface * driver_iface;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  port_id = sio_stream_to_port( stream_id);
  
  driver_iface = sio_get_driver_interface(port_id);

  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_IOCTL stream_id: %d", stream_id, 0, 0 );
     return;
  }
  (driver_iface->HW_ioctl) (stream_id, port_id, cmd, param);

} /* sio_ioctl */


/**==========================================================================

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

void sio_disable_device
(
  sio_port_id_type   port_id              /* SIO Port ID                   */  
)
{
    sio_driver_interface * driver_iface;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    driver_iface = sio_get_driver_interface(port_id);
    if(driver_iface == NULL)
    {
       MSG_ERROR("NULL driver interface for sio_disable_device, port_id: %d", port_id, 0, 0 );
       return;
    }
    (driver_iface->HW_disable_device)(port_id);

} /* sio_disable_device */

/**==========================================================================

FUNCTION SIO_CONTROL_OPEN                               EXTERNALIZED FUNCTION

@brief
  This function opens a control stream across the serial link.
      
@dependencies
  sio_init must have been called.  sio_init needs to be called by just one
  task and only once upon power-up.
  
  Only USE currently support control streams associated with a data stream.  
      
@return
  SIO_NO_STREAM_ID:  The open request could not be completed. (ERROR)

  If successful, the "stream_id" is returned for use in future SIO requests.
    
@sideeffects
  None
  
===========================================================================*/

sio_stream_id_type sio_control_open
( 
  sio_open_type *open_ptr                       /* SIO Open structure      */
)
{
  sio_stream_id_type   stream_id;               /* Stream ID               */
  sio_status_type      return_val = SIO_DONE_S; /* Return Value            */
  sio_port_id_type     port_id;                 /* Port ID                 */
  sio_port_id_type     saved_port_id;           /* Saved Port ID           */
  sio_driver_interface * driver_iface; 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( open_ptr != NULL);

  saved_port_id = open_ptr->port_id;
  port_id = open_ptr->port_id;

  /* Get stream identifier  */
  stream_id = sio_get_stream_id();

  /* If there are no more stream identifiers available,
  ** indicate error and return immediately.
  */
  if ( stream_id == SIO_NO_STREAM_ID)
  {
    ERR( "No Stream IDs available",0,0,0);
    return stream_id; 
  }

  open_ptr->stream_id = stream_id;

  if ( ( open_ptr->tx_queue    == NULL) || 
     ( ( open_ptr->rx_queue    == NULL) &&
       ( open_ptr->rx_func_ptr == NULL)) )
  {
    ERR_FATAL( "sio_control_open called with either RX or TX undefined ! port_id = 0x%x",saved_port_id,0,0);
  }

  driver_iface = sio_get_driver_interface(port_id);
  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_control_open, port_id: %d", port_id, 0, 0 );
     return SIO_NO_STREAM_ID;
  }
  
  return_val = (driver_iface->HW_control_open) (open_ptr);
  if ( return_val != SIO_DONE_S)
  {
    /* If we could not allocate TX, exit */
    sio_put_stream_id( stream_id);
    /* Restore previously saved port_id */
    open_ptr->port_id = saved_port_id;
    return SIO_NO_STREAM_ID;  
  }

  sio_enter_crit_section();
  sio_stream_port[ stream_id] = port_id;
  sio_leave_crit_section();
  /* Restore previously saved port_id */
  open_ptr->port_id = saved_port_id;
  return stream_id;
} /* sio_control_open */


/**==========================================================================

FUNCTION SIO_CONTROL_CLOSE                              EXTERNALIZED FUNCTION
                                                                             
@brief
  This procedure does all the book-keeping necessary to close a control stream.  
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.
                  
@dependencies
  Only USB supports control streams. 
          
@return
  None
  
@sideeffects
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

void sio_control_close
( 
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
)
{
  sio_port_id_type   port_id;                   /* Port ID                 */
  sio_driver_interface * driver_iface;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  sio_enter_crit_section();
  port_id = sio_stream_to_port(stream_id);

  driver_iface = sio_get_driver_interface(port_id);
  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_control_close, stream_id: %d", stream_id, 0, 0 );
     return;
  }
  (driver_iface->HW_control_close) (stream_id,port_id, close_func_ptr);

  sio_put_stream_id( stream_id);
  sio_leave_crit_section();
} /* sio_control_close */

/**==========================================================================

FUNCTION SIO_CONTROL_TRANSMIT                           EXTERNALIZED FUNCTION

@brief
  This function will transmit a packet or if transmitter is busy, schedule
  the transmission of a packet.

@dependencies
  None
  
@return
  None
  
@sideeffects
  None
  
===========================================================================*/
void sio_control_transmit
( 
  sio_stream_id_type stream_id,           /* Stream ID                     */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
  sio_port_id_type   port_id;             /* SIO Port ID                   */
  sio_driver_interface * driver_iface;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( tx_ptr == NULL) 
  {
     MSG_ERROR("NULL dsm_item ptr for transmit, stream_id: %d", stream_id, 0, 0 );
     return;
  }

  port_id = sio_stream_to_port(stream_id);
  driver_iface = sio_get_driver_interface(port_id);
  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_control_transmit, stream_id: %d", stream_id, 0, 0 );
     return;
  }
  (driver_iface->HW_control_transmit) (stream_id, port_id, tx_ptr);

} /* sio_control_transmit */

/**==========================================================================

FUNCTION SIO_CONTROL_IOCTL                              EXTERNALIZED FUNCTION

@brief
  Allows control of open control stream.

@dependencies
  None

@return
  None

@sideeffects
  None
  
===========================================================================*/
void sio_control_ioctl
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
)
{
   sio_port_id_type   port_id;             /* SIO Port ID                   */
   sio_driver_interface * driver_iface;
  
  port_id = sio_stream_to_port(stream_id);
  driver_iface = sio_get_driver_interface(port_id);
  if(driver_iface == NULL)
  {
     MSG_ERROR("NULL driver interface for sio_control_ioctl stream_id: %d", stream_id, 0, 0 );
     return;
  }
  (driver_iface->HW_control_ioctl) (stream_id, port_id, cmd, param);

} /* sio_control_ioctl */

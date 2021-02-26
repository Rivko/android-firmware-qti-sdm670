#ifndef CUSTSIO_H
#define CUSTSIO_H
/*===========================================================================

DESCRIPTION
  Configuration for SIO

  Copyright (c) 2002  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/
                                        

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/build.tz/2.0.2/cust/custsio_8660.h#1 $ $DateTime: 2018/02/06 03:02:41 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/07   gfr     Move TRAMP defines to tramp.
02/20/07   vk      Disabling USB2 and USB3 ports.
11/28/06   hn      Data card build changes.
11/10/06   gfr     Wrap FEATURE_MMC with FEATURE_BUILD_MMC, use tramp
                   API instead of direct register writes.
08/28/06   bfc     Pulled in changes from sio VU mail MSMSHARED_SIO.00.00.38
07/21/06   bfc     re-enabled a couple of USB features.  This currenlty breaks
                   the FFA build but the fix should go in soon
07/17/06   jmm     Enable USB2 and USB3 ports
06/26/06   gd      added support for MMC port
04/17/06   sr      Added support for UART1DM
03/22/06   hn      Redisable UART POWERDOWN for 7200
12/22/05   ddh     Reenable UART POWERDOWN
07/25/05   ptm     Delete FEATURE_SECOND_UART it's defined in builds file
                   depending on UART allocation between processors
07/13/05   jhs     Modified USB defines to handle SURF, RUMI, and FFA.
06/28/05   ptm     Remove // comments for L4 merge.
06/09/05   cc      Added FEATURE_USB_USE_DMOV and FEATURE_USB_READ_XCVR_SELECT_SWITCH
09/24/03   ht      Removed FEATURE_USB_MICREL
09/23/03   ht      Update USB features for MSM6500 USB integration and OTG
07/28/02   jct     Added top level build time control of FEATURE_USB
05/01/02   jct     Created
===========================================================================*/

#ifdef T_FFA6500
#include "custffa.h"
#endif

/* Enable USB support
*/
#ifdef FEATURE_BUILD_USB
   #define FEATURE_USB
#endif

/*Enable to open MMC port*/
#ifdef FEATURE_BUILD_MMC
   #define FEATURE_MMC
#endif

#define FEATURE_NO_SIO_USB_DEV_INIT

#define FEATURE_AHB_MSM_ABORT_ENABLED

#ifdef FEATURE_USE_UART1DM
  #define SIORS_USES_EXTERNAL_MND_CNT
  #define SIORS_HAVE_TXRDY_INTR
  #define SIORS_USES_LONG_UART_REG_NAME
  #define SIORS_USES_SEPERATE_CLK_REGIME
#endif

/* Enable UART clock to be TCXO not TCXO/4 */
#ifndef FEATURE_MSM6500_UART_USE_TCXO_DIV_4
#define FEATURE_UART_TCXO_CLK_FREQ         
#endif

#ifdef IMAGE_APPS_PROC
  #define FEATURE_UART_POWERDOWN
#endif

//  xxx:  No UARTs assigned.  Don't know which GSBI blocks UART is 
//  supposed to be using.

//------------------------------------------------------------------------------
//  UART 1 - Dedicated to Bluetooth?
//------------------------------------------------------------------------------
   
#define SIORS_FIRST_UART_PHYS_DEVICE        NO_PHYS_DEVICE
#define SIORS_FIRST_UART_TYPE               HW_NONE
#define SIORS_FIRST_UART_PD_WAKE_ON_RX      FALSE
#define SIORS_FIRST_UART_PD_CABLE_DETECT    FALSE
#define SIORS_FIRST_UART_UIM_PWR_VOTE       FALSE
#define SIORS_FIRST_UART_NEGATE_ARM_HALT    FALSE

//------------------------------------------------------------------------------
//  UART 2 - General purpose UART.
//------------------------------------------------------------------------------
   
#define SIORS_SECOND_UART_PHYS_DEVICE       NO_PHYS_DEVICE
#define SIORS_SECOND_UART_TYPE              HW_NONE        
#define SIORS_SECOND_UART_PD_WAKE_ON_RX     FALSE
#define SIORS_SECOND_UART_PD_CABLE_DETECT   FALSE
#define SIORS_SECOND_UART_UIM_PWR_VOTE      FALSE
#define SIORS_SECOND_UART_NEGATE_ARM_HALT   FALSE

//------------------------------------------------------------------------------
//  UART 3 - General purpose UART.
//------------------------------------------------------------------------------

#define SIORS_THIRD_UART_PHYS_DEVICE        NO_PHYS_DEVICE
#define SIORS_THIRD_UART_TYPE               HW_NONE
#define SIORS_THIRD_UART_PD_WAKE_ON_RX      FALSE
#define SIORS_THIRD_UART_PD_CABLE_DETECT    FALSE
#define SIORS_THIRD_UART_UIM_PWR_VOTE       FALSE
#define SIORS_THIRD_UART_NEGATE_ARM_HALT    FALSE

#define SIORS_USES_DIAGDSM

#define SIORS_UART1_USES_4WIRE_ONLY
#define SIORS_MODEM_LINES                   MODEM_LINES_NOOP

#define SIORS_TRAMP_VERSION                 TRAMP_VERSION_2_0

#define SIORS_CLK_RGM_VERSION               UART_CLK_RGM_8660

#define SIORS_TLMM_VERSION                  UART_TLMM_8660

#define SIORS_GSBI_VERSION                  UART_GSBI_8660

//------------------------------------------------------------------------------
//  
//------------------------------------------------------------------------------

/* Runtime Device Mapper services.  These are mutually exclusive
** with FEATURE_SDEVMAP_UI_MENU & FEATURE_SDEVMAP_MENU_ITEM_NV.
*/
#define FEATURE_RUNTIME_DEVMAP

/* Enables UI selection of port mapping. The port map is stored in
** NV and retrieved - requires power cycle for changes to take effect.
** These should be defined or undefined together.
*/
#ifndef FEATURE_RUNTIME_DEVMAP
  #define FEATURE_SDEVMAP_UI_MENU
  #define FEATURE_SDEVMAP_MENU_ITEM_NV
#endif

#ifdef FEATURE_DS
  /* Enables UI selection of DS baud rate and the DS baud to be stored in
  ** and retrieved from NV.  These should be defined or undefined together.
  */
  #define  FEATURE_DS_UI_BAUD
  #define  FEATURE_DS_DEFAULT_BITRATE_NV
#endif /* FEATURE_DS */

/* Enables use of DSM Items for DIAG
*/
#define FEATURE_DSM_DIAG_ITEMS

/* Enables UI selection of DIAG baud rate and the DIAG baud to be stored in
** and retrieved from NV.  These should be defined or undefined together.
*/
#define FEATURE_DIAG_UI_BAUD
#define FEATURE_DIAG_DEFAULT_BITRATE_NV

//------------------------------------------------------------------------------
//  FEATURE_USB
//------------------------------------------------------------------------------

#ifdef FEATURE_USB //{

  /* Enables USB CDC ACM Port.  Must define
  ** FEATURE_NEW_SIO for this to be effective.
  */
  #define FEATURE_USB_CDC_ACM

  /* Enable diag over USB
  */
  #define FEATURE_USB_DIAG
  
  #ifdef FEATURE_USB_OTG
  
    #define FEATURE_USB_OTG_XCVR
  
  #else
  
    #define FEATURE_USB_FIVE_WIRE_XCVR
    
  #endif /* FEATURE_USB_OTG */
  

  #ifdef FEATURE_RUNTIME_DEVMAP
    /* Map DS to USB as the default configuration
    */
    #define FEATURE_RDEVMAP_DS_DEFAULT_TO_USB

    /* When defined, allows DS to be mappable to UART2.  However, since
    ** UART2 does not have a full set of RS232 signals, not all DS services
    ** are possible on UART2.
    */
    #undef FEATURE_DS_MAPPABLE_TO_UART2

  #endif

  #define FEATURE_USB_USE_DMOV
  
 #ifdef T_RUMI
    #define USB_PHILIPS_XCVR
 #elif defined(T_SURF)
    #define FEATURE_USB_READ_XCVR_SELECT_SWITCH
 #endif /* T_RUMI */
#endif //} 

//------------------------------------------------------------------------------
//  FEATURE_BUILD_USB
//------------------------------------------------------------------------------

#ifdef FEATURE_BUILD_USB //{
    #ifdef T_SURF
      #define FEATURE_USB2_HOST_PORT_IS_UNUSED   /* unused host transceiver on usb2 port */
      #define FEATURE_USB3_HOST_PORT_IS_UNUSED   /* unused host transceiver on usb3 port */
    #elif defined (T_FFA)
      #define FEATURE_USB2_HOST_PORT_IS_UNUSED   /* unused host transceiver on usb2 port */
    #endif   

    #if 0
    #define FEATURE_USB3_HOST_PORT /* host transceiver on usb3 port */
    #endif
    
    #if defined(FEATURE_USB2_HOST_PORT) || defined (FEATURE_USB3_HOST_PORT)
      #define USB_PHILIPS_XCVR_ON_HOST_PORTS
      #define FEATURE_USB_HOST_MULTIPORT
    #endif /* defined(FEATURE_USB2_HOST_PORT) || defined (FEATURE_USB3_HOST_PORT) */

   #define FEATURE_USB
   #define FEATURE_USB_HOST
   #define FEATURE_USB_HOST_ENHANCED
   #define FEATURE_USB_HOST_HID
   #define FEATURE_PMIC_USB_OTG
   #define FEATURE_USB_HOST_ACQUIRE_VBUS
#if !defined(FEATURE_WCDMA_PLT) && \
    !defined(FEATURE_GSM_PLT) && \
    !defined(FEATURE_THIN_UI) && \
    !defined(IMAGE_MODEM_PROC)
   #define FEATURE_USB_HOST_MASS_STORAGE
   #define FEATURE_USB_HOST_TEST_ENHANCED
   #define FEATURE_USB_HOST_HID_TEST_ENHANCED
#endif 
#endif //}

//------------------------------------------------------------------------------
//  RDEVMAP features.
//------------------------------------------------------------------------------

#define FEATURE_RDEVMAP_DIAG_DEFAULT_TO_USB

#define RDM_USB_SER1_INIT_SRVC    RDM_DIAG_SRVC

#ifdef FEATURE_BT_SOC_UART_TL
  #define RDM_UART1_INIT_SRVC     RDM_BT_SOC_SRVC
#endif

#define RDM_USB_MDM_INIT_SRVC     RDM_DATA_SRVC

#define RDM_SMD_DS_INIT_SRVC      RDM_DATA_SRVC
#define RDM_SMD_DATA_INIT_SRVC    RDM_DATA_SRVC

#define FEATURE_RDM_3_0
#define FEATURE_RDM_NO_MISC_API

#define RDM_NO_WWAN_SRVC_MAPPING

//------------------------------------------------------------------------------
//  Obsolete.  Should be removed but may still be used.
//------------------------------------------------------------------------------

#define FEATURE_USB_CDC_ACM
#define FEATURE_NEW_SIO
#define FEATURE_SERIAL_DEVICE_MAPPER
#define FEATURE_UART_WRAPPER_FREE
#define SIORS_USES_MDDPDD_DRIVER

//  The next two are related to obsolete features using 7 wire UART.

//  Inverts the polarity of DTR line.
//  Fixes uart disconnect problem on cougar.
#define FEATURE_SIO_DTR_ACTIVE_LOW

//  Enables a nested GPIO interrupt to be used for DTR.  This is required for
//  MSM5105 since all GPIO interrupts are nested
#define FEATURE_SIO_DTR_HIGH_GPIO

#endif /* CUSTSIO_H */


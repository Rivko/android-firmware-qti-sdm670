#ifndef BOOT_EXTERN_QUSB_INTERFACE_H
#define BOOT_EXTERN_QUSB_INTERFACE_H

/*===========================================================================

                    BOOT EXTERN USB DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external qusb drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/14   ck      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "boot_extern_qusb_interface.h"
#include "qusb_al_bulk.h"
#include "qusb_common.h"
#include "qusb_al_fast_enum.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================

**  Function :  boot_qusb_al_bulk_init

** ==========================================================================
*/
/*!
 * 
 * @brief   This function initializes the USB core and keeps polling until
 *          enumeration is complete.
 *
 * @details This function initializes the USB core and keeps polling until
 *          enumeration is complete.
 * 
 * @param   speed_required   -  [IN] The maximum speed at which USB should be
 *                            configured
 * 
 * @return  None.
 * 
 */
void boot_qusb_al_bulk_init(qusb_max_speed_required_t speed_required);


/*===========================================================================

**  Function :  boot_usb_al_bulk_poll

** ==========================================================================
*/
/*!
 * 
 * @brief   This function polls the hardware for events.
 *
 * @details This function polls the hardware for events.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
void boot_usb_al_bulk_poll(void);


/*===========================================================================

**  Function :  boot_usb_al_bulk_shutdown

** ==========================================================================
*/
/*!
 * 
 * @brief   This function performs a shutdown of the hardware.
 *
 * @details This function performs a shutdown of the hardware.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
void boot_usb_al_bulk_shutdown(void);


/*===========================================================================

**  Function :  boot_usb_al_bulk_receive

** ==========================================================================
*/
/*!
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the receive
 *          URB is not NULL, upon which the data will be copied to the
 *          input receive buffer.
 * 
 * @param   *rx_buf     -  [IN/OUT] Receive buffer for data, must be 4 byte aligned 
 * @param   len         -  [IN    ] Length of buffer, must be multiples of max packet size
 * @param   rx_cb       -  [IN    ] Return callback function
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes receieved.
 * 
 */
uint32 boot_usb_al_bulk_receive(byte *rx_buf, 
                                uint32 len, 
                                sahara_rx_cb_type rx_cb, 
                                uint32 * err_code);


/*===========================================================================

**  Function :  boot_usb_al_bulk_transmit

** ==========================================================================
*/
/*!
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the transmit
 *          URB transfer is complete.
 * 
 * @param   *tx_buf     -  [IN/OUT] Transmit buffer for data, must be 4 byte aligned 
 * @param   len         -  [IN    ] Length of data to trasmit in bytes
 * @param   tx_cb       -  [IN    ] Transmit complete callback
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes transmitted.
 * 
 */
uint32 boot_usb_al_bulk_transmit(byte *tx_buf,    
                                 uint32 len,       
                                 sahara_tx_cb_type tx_cb, 
                                 uint32 * err_code);


/*===========================================================================

**  Function :  boot_usb_al_bulk_get_max_packet_size

** ==========================================================================
*/
/*!
 * 
 * @brief   This function returns whether USB spec defined max packet sizes
 *
 * @details Correct packet sizes are determined internally and will be
 *          returned to external interface
 *          
 * @return  Max packet sizes
 * 
 */
uint32 boot_usb_al_bulk_get_max_packet_size(void);


/*===========================================================================

**  Function :  boot_usb_al_bulk_get_max_raw_data_size

** ==========================================================================
*/
/*!
 * 
 * @brief   Returns the maximum bulk transfer level size that can be requested
 *          by the client
 *
 * @details Windows only support maximum bulk transfer size of 1MB, whereas
 *          Linux supports a larger maximum 
 *          
 * @return  Max USB bulk transfer level size
 * 
 */
uint32 boot_usb_al_bulk_get_max_raw_data_size(void);


#ifdef FEATURE_QUSB_FAST_ENUMERATION
/*===========================================================================

**  Function :  boot_qusb_al_do_fast_enum

** ==========================================================================
*/
/*!
 * 
 * @brief   Does fast enumeration 
 *
 * @details In case of data card on PC, BIOS may want to enumerate all
            attached devices in order to check every device against its
            whitelist.
            Application starts stack, polls it until enumeration done,
            then shuts down stack.
 *          
 * @param   speed_required   -  [IN] The maximum speed at which USB should be
 *                            configured
 * 
 * @return  None
 * 
 */
void boot_qusb_al_do_fast_enum(qusb_max_speed_required_t speed_required);
#endif


#endif /* BOOT_EXTERN_QUSB_INTERFACE_H */

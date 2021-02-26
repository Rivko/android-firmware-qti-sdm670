#ifndef QHSUSB_AL_BULK_H
#define QHSUSB_AL_BULK_H
/*=======================================================================*//**
 * @file        qhsusb_al_bulk.c
 * @author:     dpatnaik
 * @date        12-Feb-2011
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) implementation.
 *
 * @details     This file contains the implementation of the APIs to be used by
 *              the boot Sahara downloader.
 *
 * @note        
 *
 *              Copyright 2011, 2014 QUALCOMM Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when       who     what, where, why
// --------   ---     ----------------------------------------------------------
// 07/28/14   ck      Removed Sahara coupling
// 02/12/11   dpatnaik  Initial revision
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "com_dtypes.h"  // common defines - basic types as byte word uint32 etc

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

/* read completion callback - currently not used */
typedef void (* bulk_rx_cb_type) (uint32 bytes_read, uint32 err_code);

/* write completion callback - currently not used */
typedef void (* bulk_tx_cb_type) (uint32 bytes_written, uint32 err_code);


/** Enumeration to decide whether Serial + MS mode is 
  * to be used or just Serial Mode is to be used
  */
typedef enum
{
	QHSUSB_MODE__SER_MS, 
	QHSUSB_MODE__SER_ONLY,
	QHSUSB_MODE__SER_ONLY__DL_MODE 
}qhsusb_bulk_mode_type; 



//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------
// ===========================================================================
/**
 * @function    qhsusb_al_check_for_pbl_dload
 * 
 * @brief   This function checks if the USB D+ line is grounded.
 *
 * 
 * @param   core_id   -  [IN] USB core ID to initialize
 * 
 *
 * @return  boolean - returns whether we should enter PBL EDL mode.
 * 
 */
// ===========================================================================
boolean qhsusb_al_check_for_pbl_dload(uint32 core_id);
// ===========================================================================
/**
 * @function    qhsusb_al_bulk_init
 * 
 * @brief   This function initializes the USB core and keeps polling until
 *          enumeration is complete.
 *
 * @details This function initializes the USB core and keeps polling until
 *          enumeration is complete.
 * 
 * @param   core_id   -  [IN] USB core ID to initialize
 * 
 * @param 	bulk_mode_type  - [IN] Type of Bulk Mode to initialize
 *
 * @return  None.
 * 
 */
// ===========================================================================
void qhsusb_al_bulk_init(int core_id, qhsusb_bulk_mode_type bulk_mode_type);
// ===========================================================================
/**
 * @function    qhsusb_al_bulk_poll
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
// ===========================================================================
void qhsusb_al_bulk_poll(void);

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_shutdown
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
// ===========================================================================
void qhsusb_al_bulk_shutdown(void);

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_receive
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the receive
 *          URB is not NULL, upon which the data will be copied to the
 *          input receive buffer.
 * 
 * @param   *rx_buf     -  [IN/OUT] Receive buffer for data
 * @param   len         -  [IN    ] Length of buffer
 * @param   rx_cb       -  [IN    ] Return callback function
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes receieved.
 * 
 */
uint32 qhsusb_al_bulk_receive(byte *rx_buf, /* must be 4 byte aligned       */
                              uint32 len,   /* read buffer size             */
                                            /* must be multiples of max     */
                                            /* packet size                  */
                              bulk_rx_cb_type rx_cb, /* completion callback */
                              uint32 * err_code);    /* error code returned */

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_transmit
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the transmit
 *          URB transfer is complete.
 * 
 * @param   *tx_buf     -  [IN/OUT] Transmit buffer for data
 * @param   len         -  [IN    ] Length of data to trasmit in bytes
 * @param   tx_cb       -  [IN    ] Transmit complete callback
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes transmitted.
 * 
 */
uint32 qhsusb_al_bulk_transmit(byte *tx_buf,     /* must be 4 byte aligned   */
                               uint32 len,       /* number of bytes to send  */
                               bulk_tx_cb_type tx_cb, /* completion callback */
                               uint32 * err_code);    /* returned error code */

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_get_max_packet_size
 * 
 * @brief   This function returns whether USB spec defined max packet sizes
 *          for FS USB or HS USB
 *
 * @details Correct packet sizes are determined internally and will be
 *          returned to external interface
 *          
 * @return  Max packet sizes
 * 
 */
uint32 qhsusb_al_bulk_get_max_packet_size(void);

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_get_max_raw_data_size
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
uint32 qhsusb_al_bulk_get_max_raw_data_size(void);

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_set_zlp_mode
 * 
 * @brief   Sets the ZLP mode based on host type.
 *
 * @param 	[IN] Whether host supports ZLP
 *
 *          
 * @return  None
 * 
 */
 // ===========================================================================
 void qhsusb_al_bulk_set_zlp_mode(boolean host_sends_zlp); 


#endif /* QHSUSB_AL_BULK_H */

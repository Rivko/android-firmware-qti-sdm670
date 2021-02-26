#ifndef QUSB_URB_H
#define QUSB_URB_H
/*=======================================================================*//**
  @file         qusb_urb.h
 
  @brief        URB
 
  @details      
                Theory of operation 
                 
                URB stands for USB Request Buffer; it facilitate data 
                exchange between application and DCI. 
                 
                URB is allocated by application. Application provides also 
                transmit buffer. Application fills: 
                 - signature 
                 - transfer_buffer_ptr
                 - transfer_length
                 - ep
                 - is_tx
                 - complete_callback
                 - app_priv_ptr
                 
                Then, application calls qusb_submit_urb. This function 
                returns error code. If error occurs (returned is negative 
                value), submission failed and URB was not passed to DCI. If 
                success (0) is returned, from this moment, ownership over URB 
                passed to DCI and application can't access URB. 
                 
                Eventually, when DCI finished with this URB, it call URB's 
                complete() callback. At this moment, ownership over URB 
                returns to application. Application analyse transfer_status 
                to decide on next actions. DCI fills: 
                - transfer_status 
                - actual_length 
                - in case of Rx, buffer filled with data 
 
               Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
struct qusb_dcd_dsc_device;

/**
 * USB Request buffer
 */
typedef struct qusb_urb {
  uint32 signature;     //should match \a QUSB_URB_SIGNATURE 
#define QUSB_URB_SIGNATURE (0x42525551) // 'QURB'
  /********************* control *********************/
  //struct qusb_urb* next; /**< link URB's for the EP */
  /**
  * Transaction status.
  *  - DCD set to 0 prior to submission;
  *  - DCI modifies to ACTIVE when submited, and to
  *    COMPLETE/ERROR on completeion.
  *  - in complete(), DCD check status.
  */
  uint32 transfer_status;
#define QUSB_URB_STATUS_QUEUED          (1)   /**< Request queued */
#define QUSB_URB_STATUS_ACTIVE          (2)   /**< Transfer in progress */
#define QUSB_URB_STATUS_COMPLETE_OK     (3)   /**< Completed, success */
#define QUSB_URB_STATUS_COMPLETE_ERROR  (4)   /**< completed, error */

  struct qusb_dcd_dsc_device* usb_device;    /**< DCD context */
  /**
  * queued for this EP, 
  *  - bits 3:0 - EP number, 
  *  - bit 7 - IN(1)/OUT(0)
  */
  uint8 endpoint_address;
  uint8 reserved0[3]; // align to 32 bits
  /*************** data *****************/
  /**
  * Data buffer
  */
  void* transfer_buffer_ptr;
  /**
  * Length, bytes, of buffer
  */
  uint32 transfer_length;
  /**
  * Bytes user in buffer. Filled by DCI upon Rx
  */
  uint32 actual_length;
  /**
  * Flag to let FD layer to specify whether we want to transmit a ZLP 
  */
  uint8 send_zlp;

  /* index returned by the HW */
  uint8 transfer_index;

  uint8 reserved1[2]; // align to 32 bits
  /******************** application use ********************/
  /**
  * callback to be executed upon transmission completion.
  */
  void (*complete_callback)(struct qusb_urb* urb_ptr);

  /**
  * private data to be used by application / driver
  */
  void* app_priv_ptr; // high level
  void* dcd_priv_ptr;
  void* dci_priv_ptr; // low level 

} qusb_urb_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Submit URB
 * 
 * URB should be filled by upper layer as required
 * 
 * After this function, ownership over URB passed to
 * DCI and one should not access any fields. 
 *  
 * Non blocking. 
 * 
 * Submitter will regain ownership over URB when 
 * \a complete_callback called.
 * 
 * @param urb    Buffer to transfer. Can't be NULL.
 * 
 * @return - 0 if OK
 *         - negative for error
 */
int qusb_submit_urb(struct qusb_urb* urb);

#endif /*QUSB_URB_H*/


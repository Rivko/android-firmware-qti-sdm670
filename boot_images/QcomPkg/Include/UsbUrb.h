#ifndef USB_URB_H
#define USB_URB_H
/*=======================================================================*//**
  @file         UsbUrb.h

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

                Then, application calls qhsusb_submit_urb. This function
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

               Copyright (c) 2008,2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/*=============================================================================
 EDIT HISTORY

 when       who      what, where, why
 --------   ---      -----------------------------------------------------------
 01/20/15   ck       Update and clean up EFIUsbfnIo Protocol 
 =============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "com_dtypes.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
struct qhsusb_dcd_dsc_device;

/**
 * USB Request buffer
 */
typedef struct qhsusb_urb {
   uint32 signature; /**< should match \a QHSUSB_URB_SIGNATURE */
#define QHSUSB_URB_SIGNATURE (0x42525551) // 'QURB'
   /********************* control *********************/
   //struct qhsusb_urb* next; /**< link URB's for the EP */
   /**
    * Transaction status.
    *  - DCD set to 0 prior to submission;
    *  - DCI modifies to ACTIVE when submited, and to
    *    COMPLETE/ERROR on completeion.
    *  - in complete(), DCD check status.
    */
   uint32 transfer_status;
#define QHSUSB_URB_STATUS_ACTIVE          (UsbTransferStatusActive)     /**< Transfer in progress */
#define QHSUSB_URB_STATUS_COMPLETE_OK     (UsbTransferStatusComplete)   /**< Completed, success */
#define QHSUSB_URB_STATUS_COMPLETE_ERROR  (UsbTransferStatusAborted)    /**< Completed, error */
#define QHSUSB_URB_STATUS_REUSABLE        (UsbTransferStatusUnknown)    /**< Processed by user, reusable */

   struct qhsusb_dcd_dsc_device* usb_device;    /**< DCD context */
   uint32 core_id;                           /**< DCI context */
   /**
    * queued for this EP,
    *  - bits 3:0 - EP number,
    *  - bit 7 - IN(1)/OUT(0)
    */
   uint8 endpoint_address;
   uint8 reserved[3]; // align to 32 bits
   /*************** data *****************/
   /**
    * Data buffer
    */
   uint32 transfer_buffer_ptr;
   /**
    * Length, bytes, of buffer
    */
   uint32 transfer_length;

   uint32 *transfer_length_dTD;

   uint32 current_dTD_index;

   uint32 max_dTD_index;
   /**
    * Bytes user in buffer. Filled by DCI upon Rx
    */
   uint32 actual_length;
   /******************** application use ********************/
   /**
    * callback to be executed upon transmission completion.
    */
   void (*complete_callback)(struct qhsusb_urb* urb_ptr);

   /**
    * private data to be used by application / driver
    */
   void* app_priv_ptr; // high level
   void* dcd_priv_ptr;
   void* dci_priv_ptr; // low level

   /**
    * Tx Data buffer. This is used for FFU workaround
    */
   void* transfer_buffer_ptr_tx;

} qhsusb_urb_t;


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
int qhsusb_submit_urb(struct qhsusb_urb* urb);

#endif //USB_URB_H

#ifndef QHSUSB_FD_HDLC_H
#define QHSUSB_FD_HDLC_H
/*=======================================================================*//**
  @file qusb_fd.h

  Functional driver for download function

  @file         qusb_fd.h
 
  @brief        Skeleton documentation example
 
  @details      There is no standard class for download. 
                Instead, vendor-specific interface with 
                2 bulk EP (1-IN, 1-OUT) get used.
 
               Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qusb_urb.h"
#include "qusb_ch9.h" /* for USB descriptor types */
#include "qusb_common.h"

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

#define USB_PRODUCT_DIAG_ONLY (0x900E) //SBL enumerates with 0x900E to differentiate from PBL 

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

 /* Status of HDLC transfers */
enum qusb_fd_error
{
  FD_SUCCESS         = 0x0,       // success 
  FD_ERR_PERM        = 0x1,       // operation not permitted 
  FD_ERR_NO_MEM      = 0x2,       // out of heap memory 
  FD_ERR_CANCEL      = 0x3,       // transfer cancelled 
  FD_ERR_SUSPENDED   = 0x4,       // device is suspended 
  FD_ERR_ENUMERATION = 0x5,       // enumeration failed 
  FD_ERR_TIMEOUT     = 0x6,       // operation timed out 
  FD_ERR_BAD_PARAM   = 0x7,       // bad parameter 
  FD_ERR_UNKNOWN     = 0x8,       // general error 
  FD_ERR_READ        = 0x9,       // read error 
  FD_ERR_WRITE       = 0xA,       // write error 
  FD_LAST_ERROR      = 0x7FFFFFFF // last error code 
};

//Sahara protocol would be using the default mode which is 
//client supplied buffer mode 
typedef enum
{
  FD_CLIENT_SUPPLIED_BUFFER_MODE = 0x0,   //default mode 
  FD_USB_INTERNAL_BUFFER_MODE = 0x1       // legacy mode 
} qusb_transfer_mode;

PACKED struct qusb_config_struct_obex {
  usb_desc_configuration_t conf1;
  usb_desc_interface_t ifc_data;
  usb_desc_endpoint_t ep_in;
  usb_desc_endpoint_t ep_out;
} PACKED_POST;


PACKED struct qusb_config_struct_obex_ss {
  usb_desc_configuration_t conf1;
  usb_desc_interface_t ifc_data;
  usb_desc_endpoint_t ep_in;
  usb_ep_comp_desc_t  ep_in_comp;
  usb_desc_endpoint_t ep_out;
  usb_ep_comp_desc_t  ep_out_comp;
} PACKED_POST;

PACKED struct qusb_bos_struct_obex {
  usb_bos_desc_t bos_desc;
  usb_ss_cap_desc_t ss_cap_desc;
  usb_extn_cap_desc_t extn_cap_desc;
} PACKED_POST;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
 * Set USB core to use
 *
 * Should be called prior to qusb_fd_init
 *
 * @param core_id Core to use,
 *                - 0 - regular one (default)
 *                - 1 - HSIC
 */
void qusb_fd_set_core(int core_id);

/**
 * Init
 * 
 * @note Should be called prior to any other functions
 * @param rx_callback
 *               URB completion callback,
 *               will be called for every successfully received URB
 * @param str_product
 *               Product name string
 * @param tx_callback
 *                   URB completion callback,
 *                   will be called for every successfully transmitted URB
 * @param enum_complete
 *                   Enumeration completion callback, will be
 *                   called after enumeration is complete
 * @param speed
 *                   The speed with which the device needs to be configured
 * 
 * @return device software descriptor, NULL if error
 */
struct qusb_dcd_dsc_device* qusb_fd_init(void (*rx_callback)(struct qusb_urb* urb),
                                                  char* str_product,
                                                  void (*tx_callback)(struct qusb_urb* urb),
                                                  void (*enum_complete)(boolean enum_flag),
                                                  void (*error_handler)(void),
                                                  uint16 pid,
                                                  qusb_max_speed_required_t speed_required);

/**
 * Sets shutdown mode.
 */
void qusb_fd_set_shutdown_mode(qusb_shutdown_mode_t mode);

/**
 * Reads shutdown mode.
 * @return - 0 : Complete shutdown mode
 * @return - 1 : Single enumeration shutdown mode
 */
qusb_shutdown_mode_t qusb_fd_get_shutdown_mode(void);

/**
 * Terminates connection.
 * After this function, interface is not valid
 * and its functions can't be used.
 */
void qusb_fd_shutdown(void);

/**
 * schedule byte for Tx
 * 
 * @attention May block till USB finish data transfer.
 * 
 */
void qusb_fd_tx_pkt(unsigned char *pkt, uint32 len);


/**
 * Query whether USB is busy doing Tx.
 * 
 * @return - 0 : no transfer in progress
 *         - any other value : USB is not finished transfers
 */
int qusb_fd_is_tx_active(void);

/**
 * Indicate Rx URB processing by upper layers is finished
 * and URB may be re-used.
 */

uint32 qusb_fd_rx_pkt(void *buff_ptr, uint32 len);

void qusb_fd_rx_completed(struct qusb_urb* urb);

/**
 * API to set the buffer mode to be operated by the 
 * function driver. There are two modes 
 * Internal buffer mode and client supplied buffer
 * mode. Sahara Uses Client supplied Mode but
 * flash tools uses the Internal buffer mode. 
 */
void qusb_fd_set_buffer_mode(qusb_transfer_mode buffer_mode);

/**
 * schedule byte for Tx
 * 
 * @attention May block till USB finish data transfer.
 * 
 */
void qusb_fd_tx_byte(unsigned char chr); 
/**
 * Rx Completed API call for internal Buffer mode 
 * Rx Path. This API is traditionally used to support the 
 * DMSS kind of a protocol RX completion.
 *
 */
void qusb_rx_completed_int_buf_mode(struct qusb_urb* urb); 
										  

#endif /* #ifndef QHSUSB_FD_HDLC_H */


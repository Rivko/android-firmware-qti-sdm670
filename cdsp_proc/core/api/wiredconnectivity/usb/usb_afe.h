// ===============================================================================
//
// USB External Interface to the AFE
//
// GENERAL DESCRIPTION
//    
//
//
// Copyright (c) 2016 by QUALCOMM Technologies Incorporated.
// All Rights Reserved.
// QUALCOMM Proprietary/GTDR
//
// -------------------------------------------------------------------------------
//
//  $Header: //components/rel/core.qdsp6/2.1.c4/api/wiredconnectivity/usb/usb_afe.h#1 $
//  $DateTime: 2018/07/30 01:21:36 $
//  $Author: pwbldsvc $
//                      EDIT HISTORY FOR FILE
//
//  This section contains comments describing changes made to the module.
//  Notice that changes are listed in reverse chronological order.
//
// when       who      what, where, why
// --------   ---      ---------------------------------------------------------
// 02/01/16   cheunga  Initial

#ifndef _USB_AFE__H_
#define _USB_AFE__H_

#if defined(__cplusplus)
extern "C" {
#endif

// +++ Notes and terminologies
// AUDIO TX == MIC PATH      == USB IN
// AUDIO RX == SPEAKER PATH  == USB OUT
// --- Notes and terminologies

#define USB_AFE__H (0x0006 << 16)

typedef void* uac_hnd;

typedef enum 
{
  UAC_STATUS_SUCCESS = 0,
  UAC_STATUS_FAILURE,
  UAC_STATUS_BUFFER_OVERFLOW,
  UAC_STATUS_BUFFER_UNDERFLOW,
  UAC_STATUS_DROPPED_FRAME, // Indication to AFE to restart the session
} uac_status_e;


typedef enum
{
  UAC_DATA_RIGHT_JUSTIFIED,
  UAC_DATA_LEFT_JUSTIFIED,
} uac_justification_e;


typedef enum
{
  USB_AUDIO_SPEAKER,
  USB_AUDIO_MIC,
} uac_dir_e;


typedef enum
{
  UAC_IOCTL_SET_CFG,
  UAC_IOCTL_GET_DATA_PATH_DELAY,

  // USB shall handle data path in its own thread

  // Start and stop session are from AFE thread
  UAC_IOCTL_START_SESSION,
  UAC_IOCTL_STOP_SESSION,

  UAC_IOCTL_READ_DATA,
  UAC_IOCTL_WRITE_DATA,

  UAC_IOCTL_GET_TIMESTAMP, // Get timestamp associated with transfer
  UAC_IOCTL_GET_INITIAL_TIMESTAMP, // Get initial timestamp for offseting
} uac_ioctl_e;

typedef struct uac_session_ctx_type
{
  void *client_data;
} uac_session_ctx_t;

typedef enum
{
  UAC_LINEAR_PCM,
} uac_data_format;


typedef struct uac_session_lpcm_med_fmt_type
{
  uint32 media_format;
// Media format definition:
// bit 0: LE = 0, BE = 1
// bit 1: Signed = 0, Unsigned = 1
} uac_session_lpcm_med_fmt_t;


// For use in UAC_IOCTL_GET_CFG/UAC_IOCTL_SET_CFG
typedef struct uac_session_cfg_type
{
  uint32               hlos_token; // From HLOS USB -> HLOS Audio -> LPASS Audio -> UAC.  Has EP DIR and USB address information
  uint32               sampling_rate; // in Hertz
  uint16               bit_width;
  uint16               num_channels;
  uint32               num_of_samples; // Actual number of samples per buffer exchange
  uint32               sample_size_in_bytes; // Number of bytes, range of values 2, 4; this is not bSubSlotSize
  uac_dir_e            dir;
  uac_data_format      data_format;
  uac_justification_e  justification;
  void*                media_format; // Buffer from client to convey media information.  Type cast from void* based on data_format.
  uint32               media_format_buf_size;
  uint32               service_interval; // In microseconds.  USB driver will translate to bInterval.
} uac_session_cfg_t;


typedef struct uac_session_start_type
{
  // USB shall invoke client CB and populate the CB with client_data_ptr as parameter when data is available
  void (*uac_data_cb)(void *client_data_ptr);
  void *client_data_ptr;
} uac_session_start_t;


typedef struct uac_session_data_path_delay_type
{
  // USB HUB delay
  // USB device (buffering, internal processing)
  // Transit delay (from controller to device or vice-versa)
  // Account for delivery within each SOF
  uint64   data_path_delay_us;
} uac_session_data_path_delay_t;

// MIC
typedef struct uac_session_read_data_type
{
  uint64   timestamp_us; // OUT - USB populate
  void     *buffer_ptr; // IN - AFE populate
  uint32   num_bytes; // IN - AFE populate
  uint32   actual_num_bytes_read; // OUT - USB populate
  int64    acc_drift_us; // OUT - USB populate
} uac_session_read_data_t;

// SPEAKER
typedef struct uac_session_write_data_type
{
  void     *buffer_ptr; // IN - AFE populate
  uint32   num_bytes; // IN - AFE populate
  int64    acc_drift_us; // OUT - USB populate
} uac_session_write_data_t;

// Get timestamp for next transfer
typedef struct uac_get_timestamp_type
{
  uint64 timestamp_us;
} uac_get_timestamp_type_t;

// Get initial timestamp for offset
typedef struct uac_get_initial_timestamp_type
{
  uint64 timestamp_us;
} uac_get_initial_timestamp_t;

// ++++
// ++++
// ++++
// ++++
// ++++ Session related APIs

/**
 * ------------------------------------------------------
 * Open the requested data transfer link to the requested device. 
 * Does not start the session.
 * USB will internally associate the session handle with all other session related data structures for handling subsequent IOCTLs
 *
 * @param[in] uac_sess_hnd for use in all USB session related APIs until uac_usb_close is called
 *  
 * @return  UAC_STATUS_SUCCESS on success, an error code on error
 *  
 * @dependencies
 * 
 *
 */
uac_status_e uac_usb_open(uac_hnd *uac_sess_hnd);


/**
 * ------------------------------------------------------
 * Close the requested data transfer link to the requested device. 
 *
 * @param[in] uac_sess_hnd pointer obtained from uac_usb_open()
 *  
 * @return UAC_STATUS_SUCCESS on success, an error code on error 
 *  
 * @dependencies
 * uac_usb_open()
 *
 */
uac_status_e uac_usb_close(uac_hnd uac_sess_hnd);



/**
 * ------------------------------------------------------
 * Close the requested data transfer link to the requested device. 
 *
 * @param[in] uac_sess_hnd pointer obtained from uac_usb_open()
 *  
 * @return UAC_STATUS_SUCCESS on success, an error code on error 
 *  
 * @dependencies
 * uac_usb_open()
 *
 */
uac_status_e uac_usb_ioctl(uac_hnd uac_sess_hnd, uac_ioctl_e ioctl, void* ioctl_data, uint32 size_of_ioctl_data);

// ---- Session and device related APIs
// ----
// ----
// ----
// ----


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _USB_AFE__H_

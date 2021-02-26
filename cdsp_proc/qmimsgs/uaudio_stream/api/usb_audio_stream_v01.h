#ifndef UAUDIO_STREAM_SERVICE_01_H
#define UAUDIO_STREAM_SERVICE_01_H
/**
  @file usb_audio_stream_v01.h

  @brief This is the public header file which defines the uaudio_stream service Data structures.

  This header file defines the types and structures that were defined in
  uaudio_stream. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.



  $Header: //components/rel/qmimsgs.adsp/2.6/uaudio_stream/api/usb_audio_stream_v01.h#3 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7 
   It was generated on: Thu Apr 12 2018 (Spin 0)
   From IDL File: usb_audio_stream_v01.idl */

/** @defgroup uaudio_stream_qmi_consts Constant values defined in the IDL */
/** @defgroup uaudio_stream_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup uaudio_stream_qmi_enums Enumerated types used in QMI messages */
/** @defgroup uaudio_stream_qmi_messages Structures sent as QMI messages */
/** @defgroup uaudio_stream_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup uaudio_stream_qmi_accessor Accessor for QMI service object */
/** @defgroup uaudio_stream_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup uaudio_stream_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define UAUDIO_STREAM_V01_IDL_MAJOR_VERS 0x01
/** Revision Number of the IDL used to generate this file */
#define UAUDIO_STREAM_V01_IDL_MINOR_VERS 0x03
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define UAUDIO_STREAM_V01_IDL_TOOL_VERS 0x06
/** Maximum Defined Message ID */
#define UAUDIO_STREAM_V01_MAX_MESSAGE_ID 0x0001
/**
    @}
  */


/** @addtogroup uaudio_stream_qmi_consts
    @{
  */
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t va;
  /**<   IOMMU virtual address assigned by the application processor USB driver. */

  uint64_t pa;
  /**<   IOMMU physical address. */

  uint32_t size;
  /**<   Size of the memory. */
}mem_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_aggregates
    @{
  */
typedef struct {

  mem_info_v01 evt_ring;
  /**<   Memory information for secondary event ring.  */

  mem_info_v01 tr_data;
  /**<   Audio stream data endpoint transfer ring.  */

  mem_info_v01 tr_sync;
  /**<   Audio stream sync endpoint transfer ring.  */

  mem_info_v01 xfer_buff;
  /**<   Audio stream data and sync transfer request buffer. */

  mem_info_v01 dcba;
  /**<   USB device context base address. */
}apps_mem_info_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t bLength;
  /**<   Size of the descriptor in bytes.  */

  uint8_t bDescriptorType;
  /**<   Endpoint descriptor type.  */

  uint8_t bEndpointAddress;
  /**<   Address of the endpoint on the USB device.  */

  uint8_t bmAttributes;
  /**<   Endpoint attributes. */

  uint16_t wMaxPacketSize;
  /**<   Maximum packet size this endpoint is capable of sending or receiving. */

  uint8_t bInterval;
  /**<   Interval for polling endpoint for data transfers (expressed in frames
  or microframes based on speed of operation).  */

  uint8_t bRefresh;
  /**<   Rate at which an isochronous pipe provides new synchronization feedback data.  */

  uint8_t bSynchAddress;
  /**<   Address of the endpoint used to communicate synchronization information. */
}usb_endpoint_descriptor_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t bLength;
  /**<   Size of the descriptor in bytes.  */

  uint8_t bDescriptorType;
  /**<   Interface descriptor type.  */

  uint8_t bInterfaceNumber;
  /**<   Number of this interface.  */

  uint8_t bAlternateSetting;
  /**<   Value used to select the alternate setting for the identified interface.  */

  uint8_t bNumEndpoints;
  /**<   Number of endpoints used by this interface.  */

  uint8_t bInterfaceClass;
  /**<   Class code (assigned by the USB-IF).  */

  uint8_t bInterfaceSubClass;
  /**<   Subclass code (assigned by the USB-IF).  */

  uint8_t bInterfaceProtocol;
  /**<   Protocol code (assigned by the USB).  */

  uint8_t iInterface;
  /**<   Index of string descriptor describing this interface. */
}usb_interface_descriptor_v01;  /* Type */
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_enums
    @{
  */
typedef enum {
  USB_AUDIO_STREAM_STATUS_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  USB_AUDIO_STREAM_REQ_SUCCESS_V01 = 0, /**<  Audio stream enable or disable request successful.  */
  USB_AUDIO_STREAM_REQ_FAILURE_V01 = 1, /**<  Audio stream enable or disable request failed for unknown reason.  */
  USB_AUDIO_STREAM_REQ_FAILURE_NOT_FOUND_V01 = 2, /**<  Application processor side could not find audio stream requested.  */
  USB_AUDIO_STREAM_REQ_FAILURE_INVALID_PARAM_V01 = 3, /**<  Invalid audio stream request parameter.  */
  USB_AUDIO_STREAM_REQ_FAILURE_MEMALLOC_V01 = 4, /**<  Failed to allocate memory for audio stream.  */
  USB_AUDIO_STREAM_STATUS_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}usb_audio_stream_status_enum_v01;
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_enums
    @{
  */
typedef enum {
  USB_AUDIO_DEVICE_INDICATION_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  USB_AUDIO_DEV_CONNECT_V01 = 0, /**<  USB audio device connect indication.  */
  USB_AUDIO_DEV_DISCONNECT_V01 = 1, /**<  USB audio device disconnect indication.  */
  USB_AUDIO_DEV_SUSPEND_V01 = 2, /**<  USB audio device suspend indication.  */
  USB_AUDIO_DEV_RESUME_V01 = 3, /**<  USB audio device resume indication.  */
  USB_AUDIO_DEVICE_INDICATION_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}usb_audio_device_indication_enum_v01;
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_enums
    @{
  */
typedef enum {
  USB_AUDIO_DEVICE_SPEED_ENUM_MIN_ENUM_VAL_V01 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  USB_AUDIO_DEVICE_SPEED_INVALID_V01 = 0, /**<  Invalid speed information.  */
  USB_AUDIO_DEVICE_SPEED_LOW_V01 = 1, /**<  Low speed audio device.  */
  USB_AUDIO_DEVICE_SPEED_FULL_V01 = 2, /**<  Full speed audio device.  */
  USB_AUDIO_DEVICE_SPEED_HIGH_V01 = 3, /**<  High speed audio device.  */
  USB_AUDIO_DEVICE_SPEED_SUPER_V01 = 4, /**<  Super speed audio device.  */
  USB_AUDIO_DEVICE_SPEED_SUPER_PLUS_V01 = 5, /**<  Super speed plus audio device.  */
  USB_AUDIO_DEVICE_SPEED_ENUM_MAX_ENUM_VAL_V01 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}usb_audio_device_speed_enum_v01;
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_messages
    @{
  */
/** Request Message; Returns information about the enumerated USB audio device. */
typedef struct {

  /* Mandatory */
  /*  Enable Selected Audio Stream */
  uint8_t enable;
  /**<   Enables the selected audio stream.
  */

  /* Mandatory */
  /*  Platform Agnostic Unique USB Stream Identifier */
  uint32_t usb_token;
  /**<   Identifies the USB stream.
  */

  /* Optional */
  /*  Audio Configuration Parameters */
  uint8_t audio_format_valid;  /**< Must be set to true if audio_format is being passed */
  uint32_t audio_format;
  /**<   Provides the audio configuration parameters.
  */

  /* Optional */
  /*  Number of Channels */
  uint8_t number_of_ch_valid;  /**< Must be set to true if number_of_ch is being passed */
  uint32_t number_of_ch;
  /**<   Number of active audio channels. 
  */

  /* Optional */
  /*  Bit Rate */
  uint8_t bit_rate_valid;  /**< Must be set to true if bit_rate is being passed */
  uint32_t bit_rate;
  /**<   Audio stream bit rate information.
  */

  /* Optional */
  /*  Size of Transfer Buffer Requested */
  uint8_t xfer_buff_size_valid;  /**< Must be set to true if xfer_buff_size is being passed */
  uint32_t xfer_buff_size;
  /**<   Size of the transfer buffer requested.
  */

  /* Optional */
  /*  Service Interval Request */
  uint8_t service_interval_valid;  /**< Must be set to true if service_interval is being passed */
  uint32_t service_interval;
  /**<   Data endpoint service interval request in microseconds.
    \n If this value is not in the request, USB driver will assume a default service interval of the highest value allowed in the advertised configuration.
  */
}qmi_uaudio_stream_req_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_messages
    @{
  */
/** Response Message; Returns information about the enumerated USB audio device. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
  /**<   Standard response type. Contains the following data members: \n
   - qmi_result_type -- QMI_RESULT_SUCCESS or QMI_RESULT_FAILURE \n
   - qmi_error_type  -- Error code. Possible error code values are described
                        in the error codes section of each message definition.
  */

  /* Optional */
  /*  Audio Stream Request Response */
  uint8_t status_valid;  /**< Must be set to true if status is being passed */
  usb_audio_stream_status_enum_v01 status;
  /**<   Audio steam request response. Values \n
      - USB_AUDIO_STREAM_REQ_SUCCESS (0) --  Audio stream enable or disable request successful. 
      - USB_AUDIO_STREAM_REQ_FAILURE (1) --  Audio stream enable or disable request failed for unknown reason. 
      - USB_AUDIO_STREAM_REQ_FAILURE_NOT_FOUND (2) --  Application processor side could not find audio stream requested. 
      - USB_AUDIO_STREAM_REQ_FAILURE_INVALID_PARAM (3) --  Invalid audio stream request parameter. 
      - USB_AUDIO_STREAM_REQ_FAILURE_MEMALLOC (4) --  Failed to allocate memory for audio stream. 
 */

  /* Optional */
  /*  Internal Status */
  uint8_t internal_status_valid;  /**< Must be set to true if internal_status is being passed */
  uint32_t internal_status;
  /**<   Internal status reported by the application processor subsytem. 
  */

  /* Optional */
  /*  USB Host Controller */
  uint8_t slot_id_valid;  /**< Must be set to true if slot_id is being passed */
  uint32_t slot_id;
  /**<   Identifies the USB host controller to which the device is connected.
       Defined in the \hyperref[XHCI]{extensible host controller interface (XHCI)
       specification v1.0}.
  */

  /* Optional */
  /*  Platform Agnostic Unique USB Stream Identifier */
  uint8_t usb_token_valid;  /**< Must be set to true if usb_token is being passed */
  uint32_t usb_token;
  /**<   Identifies the USB stream
  */

  /* Optional */
  /*  USB Interface Descriptor  */
  uint8_t std_as_opr_intf_desc_valid;  /**< Must be set to true if std_as_opr_intf_desc is being passed */
  usb_interface_descriptor_v01 std_as_opr_intf_desc;
  /**<   USB interface descriptor. */

  /* Optional */
  /*  USB Endpoint Data Descriptor */
  uint8_t std_as_data_ep_desc_valid;  /**< Must be set to true if std_as_data_ep_desc is being passed */
  usb_endpoint_descriptor_v01 std_as_data_ep_desc;

  /* Optional */
  /*  USB Endpoint Sync Descriptor */
  uint8_t std_as_sync_ep_desc_valid;  /**< Must be set to true if std_as_sync_ep_desc is being passed */
  usb_endpoint_descriptor_v01 std_as_sync_ep_desc;

  /* Optional */
  /*  USB Audio Class Specification */
  uint8_t usb_audio_spec_revision_valid;  /**< Must be set to true if usb_audio_spec_revision is being passed */
  uint16_t usb_audio_spec_revision;
  /**<   USB audio class specification in binary coded decimal. Represents the bcdADC
       field from the USB class-specific audio class interface descriptor
  */

  /* Optional */
  /*  Audio Data Stream Delay  */
  uint8_t data_path_delay_valid;  /**< Must be set to true if data_path_delay is being passed */
  uint8_t data_path_delay;
  /**<   Delay introduced in audio data stream due to internal processing of signal
       within the audio function; expressed in number of frames or micro-frames. \n
       Represents the bDelay field from the USB class-specific audio stream interface descriptor. 
  */

  /* Optional */
  /*  Audio Subslot or Subframe Bytes  */
  uint8_t usb_audio_subslot_size_valid;  /**< Must be set to true if usb_audio_subslot_size is being passed */
  uint8_t usb_audio_subslot_size;
  /**<   The number of bytes occupied by one audio subslot or subframe. \n
       This field is the same as the bSubSlotSize or bSubFrameSize fields defined in 
       the \hyperref[USBCDC]{USB Audio Class Formats Specification} - Section 2.2.5 
       Type I Format Type Descriptor.
  */

  /* Optional */
  /*  Application Processor Allocated Memory Information */
  uint8_t xhci_mem_info_valid;  /**< Must be set to true if xhci_mem_info is being passed */
  apps_mem_info_v01 xhci_mem_info;
  /**<   \n Application processor allocated memory information.
  */

  /* Optional */
  /*  ADSP Interrupter Number */
  uint8_t interrupter_num_valid;  /**< Must be set to true if interrupter_num is being passed */
  uint8_t interrupter_num;
  /**<   ADSP interrupter number. 
  */

  /* Optional */
  /*  USB Device Speed Information */
  uint8_t speed_info_valid;  /**< Must be set to true if speed_info is being passed */
  usb_audio_device_speed_enum_v01 speed_info;
  /**<   Speed information for the connected USB device.
  */

  /* Optional */
  /*  USB Controller Number */
  uint8_t controller_num_valid;  /**< Must be set to true if controller_num is being passed */
  uint8_t controller_num;
  /**<   Host controller number to which the device is connected.
       \n Count commences with 0.
  */
}qmi_uaudio_stream_resp_msg_v01;  /* Message */
/**
    @}
  */

/** @addtogroup uaudio_stream_qmi_messages
    @{
  */
/** Indication Message; Informs the UAC about usb_token and descriptor information.    */
typedef struct {

  /* Mandatory */
  /*  USB Audio Device Indication Type */
  usb_audio_device_indication_enum_v01 dev_event;
  /**<   USB audio device indication type. Values: \n
      - USB_AUDIO_DEV_CONNECT (0) --  USB audio device connect indication. 
      - USB_AUDIO_DEV_DISCONNECT (1) --  USB audio device disconnect indication. 
      - USB_AUDIO_DEV_SUSPEND (2) --  USB audio device suspend indication. 
      - USB_AUDIO_DEV_RESUME (3) --  USB audio device resume indication.  
 */

  /* Mandatory */
  /*  USB Host Controller */
  uint32_t slot_id;
  /**<   Identifies the USB host controller to which the device is connected.
       Defined in the \hyperref[XHCI]{extensible host controller interface (XHCI)
       specification v1.0}.
  */

  /* Optional */
  /*  Platform Agnostic Unique USB Stream Identifier */
  uint8_t usb_token_valid;  /**< Must be set to true if usb_token is being passed */
  uint32_t usb_token;
  /**<   Identifies the USB stream */

  /* Optional */
  /*  USB Interface Descriptor  */
  uint8_t std_as_opr_intf_desc_valid;  /**< Must be set to true if std_as_opr_intf_desc is being passed */
  usb_interface_descriptor_v01 std_as_opr_intf_desc;
  /**<   USB interface descriptor. */

  /* Optional */
  /*  USB Endpoint Data Descriptor */
  uint8_t std_as_data_ep_desc_valid;  /**< Must be set to true if std_as_data_ep_desc is being passed */
  usb_endpoint_descriptor_v01 std_as_data_ep_desc;

  /* Optional */
  /*  USB Endpoint Sync Descriptor */
  uint8_t std_as_sync_ep_desc_valid;  /**< Must be set to true if std_as_sync_ep_desc is being passed */
  usb_endpoint_descriptor_v01 std_as_sync_ep_desc;

  /* Optional */
  /*  USB Audio Class Specification */
  uint8_t usb_audio_spec_revision_valid;  /**< Must be set to true if usb_audio_spec_revision is being passed */
  uint16_t usb_audio_spec_revision;
  /**<   USB audio class specification in binary coded decimal. Represents the bcdADC
       field from the USB class-specific audio class interface descriptor
  */

  /* Optional */
  /*  Audio Data Delay Stream  */
  uint8_t data_path_delay_valid;  /**< Must be set to true if data_path_delay is being passed */
  uint8_t data_path_delay;
  /**<   Delay introduced in audio data stream due to internal processing of signal
       within the audio function; expressed in number of frames or micro-frames. \n
       Represents the bDelay field from the USB class-specific audio stream interface descriptor. 
  */

  /* Optional */
  /*  Audio Subslot or Subframe Bytes */
  uint8_t usb_audio_subslot_size_valid;  /**< Must be set to true if usb_audio_subslot_size is being passed */
  uint8_t usb_audio_subslot_size;
  /**<   The number of bytes occupied by one audio subslot or subframe. \n
       This field is the same as the bSubSlotSize or bSubFrameSize fields defined in 
       the \hyperref[USBCDC]{USB Audio Class Formats Specification} - Section 2.2.5 
       Type I Format Type Descriptor. 
  */

  /* Optional */
  /*  Application Processor Allocated Memory Information */
  uint8_t xhci_mem_info_valid;  /**< Must be set to true if xhci_mem_info is being passed */
  apps_mem_info_v01 xhci_mem_info;
  /**<   \n Application processor allocated memory information.
  */

  /* Optional */
  /*  ADSP Interrupter Number */
  uint8_t interrupter_num_valid;  /**< Must be set to true if interrupter_num is being passed */
  uint8_t interrupter_num;
  /**<   ADSP interrupter number. 
  */

  /* Optional */
  /*  USB Controller Number */
  uint8_t controller_num_valid;  /**< Must be set to true if controller_num is being passed */
  uint8_t controller_num;
  /**<   Host controller number to which the device is connected.
       \n Count commences with 0.
  */
}qmi_uaudio_stream_ind_msg_v01;  /* Message */
/**
    @}
  */

/* Conditional compilation tags for message removal */ 
//#define REMOVE_QMI_UAUDIO_STREAM_V01 
//#define REMOVE_QMI_UAUDIO_STREAM_IND_V01 

/*Service Message Definition*/
/** @addtogroup uaudio_stream_qmi_msg_ids
    @{
  */
#define QMI_UAUDIO_STREAM_REQ_V01 0x0001
#define QMI_UAUDIO_STREAM_RESP_V01 0x0001
#define QMI_UAUDIO_STREAM_IND_V01 0x0001
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro uaudio_stream_get_service_object_v01( ) that takes in no arguments. */
qmi_idl_service_object_type uaudio_stream_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define uaudio_stream_get_service_object_v01( ) \
          uaudio_stream_get_service_object_internal_v01( \
            UAUDIO_STREAM_V01_IDL_MAJOR_VERS, UAUDIO_STREAM_V01_IDL_MINOR_VERS, \
            UAUDIO_STREAM_V01_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif


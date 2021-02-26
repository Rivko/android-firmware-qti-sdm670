#ifndef ELITE_FWK_EXTNS_HPCM_H
#define ELITE_FWK_EXTNS_HPCM_H

/**
  @file Elite_fwk_extns_hpcm.h

  @brief Framework extension header for Host-PCM (HPCM) module

  This file defines framework extension, parameters, events and other behaviors associated with HPCM module.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description below the Edit History displays in the PDF.
      Contact Tech Pubs for assistance.
===========================================================================*/
/*=============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
=============================================================================*/
/*=============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_hpcm.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  08/20/16    sw      (Tech Pubs) Edited Doxygen markup/comments for AVS 2.8.
  03/01/16   pgurram   initial version

=============================================================================*/

/** @weakgroup weakf_capiv2_ext_host_pcm_intro

The #FWK_EXTN_HPCM framework extension allows a client to use a
client-specified sampling rate and receive or inject multichannel PCM data at
a specified tap point. HPCM operates seamlessly across handovers, without any
client knowledge.

HPCM exchanges PCM data with the client on a 20 ms timeline, which should be an
integer multiple of the timeline of the underlying tap point. PCM data exchange
is therefore synchronous to the tap point operation, and thus gives the client
a time trigger for driving its own buffer handling.

The PCM data exchange is signaled through events, which contain physical
address pointers to the actual PCM data held in shared memory buffers.
The client is responsible for proper memory mapping of the shared memory
regions used for PCM data exchange, and also for managing the pool of buffers.
A shared memory client buffer can hold multichannel, non-interleaved PCM data.
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2_types.h"


/** @addtogroup capiv2_ext_host_pcm
@{ */

/** Unique identifier of the custom framework extension for a module that
    supports HPCM.

  This extension supports the following parameters and event:
  - #FWK_EXTN_PROPERTY_ID_HPCM_SS_ENABLE
  - #FWK_EXTN_PARAM_ID_HPCM_START
  - #FWK_EXTN_PARAM_ID_HPCM_STOP
  - #FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF
  - #FWK_EXTN_EVENT_ID_HPCM_HOST_BUF_AVAILABLE
 */
#define FWK_EXTN_HPCM (0x00010EA3)

/*------------------------------------------------------------------------------
 * Property ID definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/

/** Custom property used to specify whether sample slip functionality is
    required in the module.

    @msgpayload{fwk_extn_property_id_hpcm_ss_enable_t}
    @tablens{weak__fwk__extn__property__id__hpcm__ss__enable__t} @newpage
 */
#define FWK_EXTN_PROPERTY_ID_HPCM_SS_ENABLE (0x00010EA4)

/*------------------------------------------------------------------------------
 * Payload structures of Property IDs
 *----------------------------------------------------------------------------*/

/** Used to set custom properties in the module through #CAPI_V2_CUSTOM_PROPERTY.

    The aDSP service uses this custom property structure used to statically
    set or get the custom properties of the HPCM module.
*/
typedef struct fwk_extn_property_id_hpcm_custom_prop_t
{
   uint32_t                  sec_property_id;
   /**< Secondary property ID of the HPCM property. */

   void                     *hpcm_info;
   /**< Pointer to the structure that corresponds to sec_property_id */

}/** @cond */fwk_extn_property_id_hpcm_custom_prop_t/** @endcond */;

/** @weakgroup weak_fwk_extn_property_id_hpcm_ss_enable_t
@{ */
/* structure to set if sample slip functionality is required or not */
/** The aDSP service must provide this information to the module.
*/
typedef struct fwk_extn_property_id_hpcm_ss_enable_t
{
   uint8_t       ss_enable;
   /**< Indicates whether sample slip functionality is required.

        @values
        - TRUE -- Sample slip functionality is required
        - FALSE -- Sample slip functionality is not required @tablebulletend */

}/** @cond */fwk_extn_property_id_hpcm_ss_enable_t/** @endcond */;
/** @} *//* end_weakgroup weak_fwk_extn_property_id_hpcm_ss_enable_t */


/*------------------------------------------------------------------------------
 * Parameter ID definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/

/** ID of the parameter that indicates the start of an HPCM session and passes
    tap point configuration information to the module.

    @msgpayload{fwk_extn_param_id_hpcm_start_t}
    @table{weak__fwk__extn__param__id__hpcm__start__t}
 */
#define FWK_EXTN_PARAM_ID_HPCM_START (0x00010EA5)

/** ID of the parameter that tells the module to stop HPCM in both the read and
    write directions.

    @msgpayload
    None.
 */
#define FWK_EXTN_PARAM_ID_HPCM_STOP (0x00010EA6)

/** ID of the parameter that sends the payload of VSS_IVPCM_EVT_PUSH_BUFFER_V2
    to the module.

    VSS_IVPCM_EVT_PUSH_BUFFER_V2 is an APR event sent by the
    client. It passes metadata containing information about the read and write
    buffers available for use.

    @note1hang For information on the VSS event, refer to
               @xrefcond{Q8,80-NF774-31,80-NA610-31}.

    @msgpayload{fwk_extn_param_id_hpcm_push_host_buf_t}
    @table{weak__fwk__extn__param__id__hpcm__push__host__buf__t}
 */
#define FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF (0x00010EA7)

/*------------------------------------------------------------------------------
 * Payload structures of Parameter IDs
 *----------------------------------------------------------------------------*/

/** @weakgroup weak_fwk_extn_param_id_hpcm_start_t
@{ */
/* payload of the FWK_EXTN_PARAM_ID_HPCM_START, which specifies HPCM tap-point
   configuration 
 */
typedef struct fwk_extn_param_id_hpcm_start_t
{
  uint32_t tap_point;
  /**< GUID of the configured tap point.

       @values (refer to @xrefcond{Q9,80-NF774-2,80-NA610-2})
       - VOICEPROC_MODULE_TX
       - VOICEPROC_MODULE_RX
       - VOICESTREAM_MODULE_TX
       - VOICESTREAM_MODULE_RX @tablebulletend */

  uint16_t direction;
  /**< Specifies the operating mode of the tap point.

       @values
       - 1 -- Read
       - 2 -- Write

       The values can be ORed to define a simultaneous read/write session. The
       direction is with respect to the client:
       - A tap point with a read session enabled delivers data to the client.
       - A tap point with a write session enabled injects data from the client.
       - A read + write session first reads data from the tap point to the
         client, and then injects data from the client for downstream
         processing. @tablebulletend */

  uint16_t sampling_rate;
  /**< Client sampling rate for the configured tap point.

       The aDSP resamples between the client sampling rate and the operating
       rate of the tap point.

       @values 8, 16, 32, 48 kHz */

  uint16_t duration_ms;
  /**< (Not supported) Duration of the buffer in milliseconds.

       Clients must set this field to 0. */

  uint16_t reserved;
  /**< Clients must set this field to 0. */

  uint32_t mem_map_handle;
   /**< Memory map handle returned by the aDSP.

        This handle defines the property of the shared memory used for HPCM
        data buffer transfer. The handle is expected to be valid for all the
        HPCM buffers until VSS_IVPCM_CMD_STOP is issued.

        @values (refer to @xrefcond{Q8,80-NF774-31,80-NA610-31})
        - VSS_IMEMORY_CMD_MAP_VIRTUAL
        - VSS_IMEMORY_CMD_MAP_PHYSICAL

        The value used is based on how the client and CVD interact. If the
        client and CVD are on the same subsystem (like MDM targets), virtual
        mapping can be done. On MSM targets, physical mapping is done.

        For either value, the CVD uses internal commands to provide the mapped
        memory information to the voice services. */

}/** @cond */fwk_extn_param_id_hpcm_start_t/** @endcond */;
/** @} *//* end_weakgroup weak_fwk_extn_param_id_hpcm_start_t */

/** @weakgroup weak_fwk_extn_param_id_hpcm_push_host_buf_t
@{ */
/* Payload of the FWK_EXTN_PARAM_ID_HPCM_PUSH_HOST_BUF, which sends or
   receives the PCM samples to or from the host PCM.
 */
typedef struct fwk_extn_param_id_hpcm_push_host_buf_t
{
   uint32_t tap_point;
   /**< GUID of the tap point where the buffers are to be used.

        @values (refer to @xrefcond{Q9,80-NF774-2,80-NA610-2})
        - VOICEPROC_MODULE_TX
        - VOICEPROC_MODULE_RX
        - VOICESTREAM_MODULE_TX
        - VOICESTREAM_MODULE_RX @tablebulletend */

   uint32_t mask;
   /**< Buffer types that are available to be pushed.

        @values{for bit 0}
        - 1 -- Read buffer is available
        - 0 -- Read buffer is not available

        @values{for bit 1}
        - 1 -- Write buffer is available
        - 0 -- Write buffer is not available

        The values can be ORed to set multiple paths. The mask setting
        specifies how the firmware looks at the buffer read/write fields. */

   uint32_t rd_buff_addr_lsw;
   /**< Lower 32 bits of the shared memory address (physical or virtual) of
        the read buffer to be filled with data from the specified tap point.

        This address is valid if the mask field indicates that the read buffer
        is available. */

   uint32_t rd_buff_addr_msw;
   /**< Upper 32 bits of the shared memory address (physical or virtual) of
        the read buffer to be filled with data from the specified tap point.

        This address is valid if the mask field indicates that the read buffer
        is available.

        The 64-bit number formed by rd_buff_addr_lsw and rd_buff_addr_msw must
        be 32-byte aligned and must have been previously mapped. */

   uint32_t wr_buff_addr_lsw;
   /**< Lower 32 bits of the shared memory address (physical or virtual) of
        the write buffer that contains data to inject at the tap point for
        downstream processing.

        This address is valid if the mask field indicates that the write buffer
        is available.

        If the memory region containing this buffer is mapped with
        property_flag == VOICE_LOCAL_HEAP_MEM_ADDR, this field must be
        4-byte aligned. */

   uint32_t wr_buff_addr_msw;
   /**< Upper 32 bits of the shared memory address (physical or virtual) of
        the write buffer that contains data to inject at the tap point for
        downstream processing.

        This address is valid if the mask field indicates that the write buffer
        is available.

        The 64-bit number formed by wr_buff_addr_lsw and wr_buff_addr_msw must
        be 4-byte aligned and must have been previously mapped. */

   uint16_t rd_buff_size;
   /**< Maximum size of the read buffer to be filled.

        This value is valid if the mask field indicates that the read buffer is
        to be filled.

        This field must be in multiples of 32 bytes. */

   uint16_t wr_buff_size;
   /**< Size of the write buffer to be consumed.

        This value is valid if the mask field indicates that the write buffer
        is to be consumed. */

   uint16_t sampling_rate;
   /**< Sampling rate of the buffer.

        @values 8, 16, 32, 48 kHz

        This rate must match the client-configured host sampling rate. It is
        provided for informational purposes and can be used when calculating
        the duration. */

   uint16_t wr_num_chan;
   /**< Number of channels at the selected tap point for the write buffer.

        This value is valid if the mask field indicates that the write buffer
        is to be consumed.

        The client can use this parameter when calculating the duration. */

}/** @cond */fwk_extn_param_id_hpcm_push_host_buf_t/** @endcond */;
/** @} *//* end_weakgroup weak_fwk_extn_param_id_hpcm_push_host_buf_t */

/*------------------------------------------------------------------------------
 * Event ID definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/

/** ID of the event that the HPCM module uses to command the service to send
    an APR event with a payload containing metadata information about the
     processed read and write buffers.
     - An event that contains a read buffer notifies the client that PCM data
       is available for read.
     - An event that contains a write buffer notifies the client that PCM data
       has been consumed.

     The event also contains timetick information and error status information.
     One read and one write buffer can be returned to the client in a single
     event.

     @msgpayload{fwk_extn_event_id_hpcm_host_buf_available_t}
     @table{weak__fwk__extn__event__id__hpcm__host__buf__available__t} @newpage
 */
#define FWK_EXTN_EVENT_ID_HPCM_HOST_BUF_AVAILABLE (0x00010EA8)

/*------------------------------------------------------------------------------
 * Payload structures of Event IDs
 *----------------------------------------------------------------------------*/

/** @weakgroup weak_fwk_extn_event_id_hpcm_host_buf_available_t
@{ */
/* Payload of the FWK_EXTN_EVENT_ID_HPCM_HOST_BUF_AVAILABLE to notify the host of buffers
   filled/consumed
 */
typedef struct fwk_extn_event_id_hpcm_host_buf_available_t
{
   uint32_t tap_point;
   /**< GUID of the tap point where the buffers are used.

        @values (refer to @xrefcond{Q9,80-NF774-2,80-NA610-2})
        - VOICEPROC_MODULE_TX
        - VOICEPROC_MODULE_RX
        - VOICESTREAM_MODULE_TX
        - VOICESTREAM_MODULE_RX @tablebulletend */

   uint32_t mask;
   /**< Notification mode.

        @values
        - 0 -- Default mask value
        - 1 -- For the following bits:
           - Bit 0 -- Read buffer is filled
           - Bit 1 -- Write buffer is consumed
           - Bit 2 -- Timing tick that provides buffer sizing and duration
                      information
           - Bit 3 -- Error occurred during read processing
           - Bit 4 -- Error occurred during write processing
           - Other bits are ignored

        These values can be ORed to set multiple notifications. */

   uint32_t rd_buff_addr_lsw;
   /**< Lower 32 bits of the shared memory address of the read buffer that was
        filled with data from the specified tap point.

        This address is valid if the mask field indicates that the read buffer
        is filled. */

   uint32_t rd_buff_addr_msw;
   /**< Upper bits of the shared memory address of the read buffer that was
        filled with data from the specified tap point.

        This address is valid if the mask field indicates that the read buffer
        is filled.

        Data was resampled, if necessary, to the client-configured HPCM
        sampling rate.

        The client provided this shared memory buffer via
        VSS_IVPCM_EVT_PUSH_BUFFER_V2 (refer to 
        @xrefcond{Q8,80-NF774-31,80-NA610-31}). */

   uint32_t wr_buff_addr_lsw;
   /**< Lower 32 bits of the shared memory address of the consumed write buffer.

        This address is valid if the mask field indicates that the write buffer
        is consumed. */

   uint32_t wr_buff_addr_msw;
   /**< Upper 32 bits of the shared memory address of the consumed write buffer.

        This address is valid if the mask field indicates that the write buffer
        is consumed.

        Consumed data was resampled, if necessary, from the client-configured
        HPCM sampling rate to the operating rate of the tap point.

        This shared memory buffer was provided by client via the
        VSS_IVPCM_EVT_PUSH_BUFFER_V2 command (refer to
        @xrefcond{Q8,80-NF774-31,80-NA610-31}). */

   uint16_t rd_buff_size;
   /**< Actual size of the filled read buffer.

        This value is valid if the mask field indicates that the read buffer is
        filled. If the mask field has only a timing tick set, this value
        represents the required buffer size for the next push event.

        If an error occurred in processing, the size may be given as 0, and
        the buffer can be reclaimed. */

   uint16_t wr_buff_size;
   /**< Requested size of the next write buffer to be pushed.

        This value is valid if the mask field indicates that the write buffer
        is consumed.

        The client can use this field to determine buffer size requirements
        for subsequent write buffers, and to calculate the requested duration
        of data required by the firmware at the selected tap point. */

   uint16_t sampling_rate;
   /**< Sampling rate of the buffers.

        @values 8, 16, 32, 48 kHz

        This rate must match the client-configured host sampling rate. It is
        provided for informational purposes in the notification event, and it
        can be used when calculating duration.*/

   uint16_t rd_num_chan;
   /**< Actual number of channels associated with the read buffer
        (non-interleaved).

        This value is valid if the mask field indicates that the read buffer is
        filled. */

   uint16_t wr_num_chan;
   /**< Number of channels at the selected tap point for the next pushed write
        buffer (non-interleaved).

        The client can use this value when calculating duration. */

   uint16_t reserved;
   /**< Clients must set this field to 0. */

}/** @cond */fwk_extn_event_id_hpcm_host_buf_available_t/** @endcond */;
/** @} *//* end_weakgroup weak_fwk_extn_event_id_hpcm_host_buf_available_t */

/** @} *//* end_addtogroup capiv2_ext_host_pcm */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_HPCM_H */

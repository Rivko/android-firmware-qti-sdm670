/*========================================================================*/
/**
@file adsp_usm_data_commands.h

@brief This file contains USM data commands and events structures definitions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Ultrasound_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Ultrasound_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/ultrasound.adsp/2.7/api/adsp_usm_data_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/01/13   NR      Add support for family - B memory APIs  
10/06/12   sw      (Tech Pubs) Updated legal statements (lines 20 & 22) for QTI;
                   updated Doxygen markup for D.2 templates.
02/14/12   sw      (Tech Pubs) Edited Doxygen markup/comments for Rev B of the IS.
10/18/11   sw      (Tech Pubs) Edited Doxygen markup/comments for new Interface Spec.
04/15/10   rkc      Created file.

========================================================================== */

#ifndef _ADSP_USM_DATA_COMMANDS_H_
#define _ADSP_USM_DATA_COMMANDS_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup usm_data_read_v2
    Delivers an empty data buffer to be filled by the USM.

  @apr_hdr_fields
    Opcode -- USM_DATA_CMD_READ \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID (0 is reserved)
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION (0 is reserved)
  @apr_msg_payload{usm_data_cmd_read_t}
    @tablens{weak__usm__data__cmd__read__v2__t}

  @detdesc
    The USM calculates the buffers' start addresses, and the buffer IDs are
    calculated accordingly. The client must not send a number of buffers
    that would exceed the allocated shared memory.

  @return
    #USM_DATA_EVENT_READ_V2_DONE is raised when the buffer is consumed.

  @dependencies
    The session/stream must be a valid and opened read session/stream.
*/
#define USM_DATA_CMD_READ_V2                                            0x00012724

/* Payload for USM data read command */
typedef struct usm_data_cmd_read_v2_t usm_data_cmd_read_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_data_cmd_read_v2_t
@{ */
/* Payload of the USM_DATA_CMD_READ_V2 command, which delivers a data
    buffer to the USM for encoded data. */
/** Each frame's buffer address must be to a physical address that has
    been mapped via #USM_CMD_SHARED_MEM_MAP_REGIONS.
*/
struct usm_data_cmd_read_v2_t
{
    uint32_t                  buf_addr_lsw;
    /**< The 64 bit address msw-lsw should be a valid mapped address and 
         should be a multiple of 32 bytes */

    uint32_t                  buf_addr_msw;
    /**< The 64 bit address msw-lsw should be a valid mapped address and should be a multiple of 32 bytes.
        - Address of the buffer where the DSP puts the encoded data, potentially,                                                                                                                                      .
         at an offset specified by the uOffset field in USM_DATA_EVENT_READ_V2_DONE structure                                                                                                                         .
         The buffer should be aligned to a 32 byte boundary.
       - In the case of 32 bit Shared memory address, msw field must be set to zero.
       - In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero.
     */

    uint32_t                  mem_map_handle;
    /**< Memory map handle returned by DSP through USM_CMD_SHARED_MEM_MAP_REGIONS command. */


    uint32_t                  buf_size;
    /**< Number of bytes available for the aDSP to write. The aDSP starts
         writing from buf_addr.

         @values > 0

         @note1hang This is the size of each num_buffer buffer; it is not the
                    size of the entire buffer. */

    uint32_t                  seq_id;
    /**< Optional buffer sequence ID. */

    uint32_t                  num_buffers;
    /**< Number of buffers supplied.

         @values > 0 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_data_cmd_read_v2_t */

/** @ingroup usm_data_evt_read_done_v2
    Indicates that the referenced buffer has been filled and is available to
    the client.

  @apr_hdr_fields
    Opcode -- USM_DATA_EVENT_READ_DONE_V2 \n
    Token  -- Matches the token in the original read buffer \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID (0 is reserved)
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @apr_msg_payload{usm_data_event_read_done_t}
    @table{weak__usm__data__event__read__done__v2__t}
      @inputtable{USM_DATA_EVENT_READ_DONE_V2_APR_msg_payload.tex}

  @return
    None.

  @dependencies
      The session/stream must be a valid and open write or read/write
      session/stream.
      @par
      A corresponding #USM_DATA_CMD_READ_V2 command must have been received and
      processed.
*/
#define USM_DATA_EVENT_READ_DONE_V2                                    0x00012725

/* Structure for data read done event payload */
typedef struct usm_data_event_read_done_v2_t usm_data_event_read_done_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_data_event_read_done_v2_t
@{ */
/* Payload of the USM_DATA_EVENT_READ_DONE_V2 event, which indicates
    that the referenced buffer has been filled and is available to the client.
*/
struct usm_data_event_read_done_v2_t
{
   uint32_t                  status;
   /**< Status message (error code).

        @values Refer to @xhyperref{Q3,[Q3]} */
   /* Q3 = Asynchronous Packet Router API (for MDM9x15 and Newer) Interface Spec
      (80-N1463-2) */

   uint32_t                  buf_addr_lsw;
   /**< 64 bit address msw-lsw is a valid, mapped address. 64 bit address is a multiple of 32 bytes. */

   uint32_t                  buf_addr_msw;
   /**< 64 bit address msw-lsw is a valid, mapped address. 64 bit address is a multiple of 32 bytes.
    
      -Same address provided by the client in USM_DATA_CMD_READ_V2
      -In the case of 32 bit Shared memory address, msw field is set to zero.
      -In the case of 36 bit shared memory address, bit 31 to bit 4 of msw is set to zero.
     */

   uint32_t                  mem_map_handle;
   /**< memory map handle in the USM_DATA_CMD_READ_V2  */
      
   uint32_t                  enc_frames_total_size;
   /**< Total size of the encoded frames in bytes.

        @values @ge 0 */

   uint32_t                  offset;
   /**< Offset from buf_addr to the first byte of the first encoded frame.
        All encoded frames are consecutive, starting from this offset.
          
        @values @ge 0 */

   uint32_t                  reserved0;
   /**< This field must be set to zero. */

   uint32_t                  reserved1;
   /**< This field must be set to zero. */

   uint32_t                  reserved2;
   /**< This field must be set to zero. */

   uint32_t                  num_frames;
   /**< Number of encoded frames in the buffer. */

   uint32_t                  seq_id;
   /**< Optional buffer sequence ID. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_data_event_read_done_v2_t */

/** @ingroup usm_data_cmd_write_v2
    Delivers a data buffer to the USM.

  @apr_hdr_fields
    Opcode -- USM_DATA_CMD_WRITE_V2 \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID (0 is reserved)
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @apr_msg_payload{usm_data_cmd_write_v2_t}
    @table{weak__usm__data__cmd__write__v2__t}

  @detdesc
    The decoder copies the data buffer to an internal buffer and generates
    a #USM_DATA_EVENT_WRITE_DONE_V2 event. The decoder transmits this data
    buffer repeatedly according to the requested sampling rate until another
    USM_DATA_CMD_WRITE command is received.

  @return
    #USM_DATA_EVENT_WRITE_DONE_V2 is raised when the buffer is fully consumed.

  @dependencies
    The session/stream must be a valid and opened write session/stream.
    @par
    Each frame's buffer address must be a physical address that has been
    mapped via #USM_CMD_SHARED_MEM_MAP_REGIONS.
 */
#define USM_DATA_CMD_WRITE_V2                                           0x00012726

/* Payload for USM data write command */
typedef struct usm_data_cmd_write_v2_t usm_data_cmd_write_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_data_cmd_write_v2_t
@{ */
/* Payload of the USM_DATA_CMD_WRITE_V2 command, which delivers a data
    buffer to the USM for decoding.
*/
struct usm_data_cmd_write_v2_t
{
    uint32_t                  buf_addr_lsw;
    /**< The 64 bit address msw-lsw should be a valid, mapped address. 64 bit address should be a multiple of 32 bytes*/

    uint32_t                  buf_addr_msw;
    /**< The 64 bit address msw-lsw should be a valid, mapped address. 64 bit address should be a multiple of 32 bytes.
                                                                                                                  .
       Address of the buffer containing the data to be decoded. The buffer should be aligned to a 32 byte boundary.
       In the case of 32 bit Shared memory address, msw field must be set to zero.
       In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero.
    */
    uint32_t                  mem_map_handle;
    /**< memory map handle returned by DSP through USM_CMD_SHARED_MEM_MAP_REGIONS command */

    uint32_t                  buf_size;
    /**< Number of valid bytes available in the buffer for decoding. The
         first byte starts at buf_addr.

         @values > 0 bytes*/

    uint32_t                  seq_id;
    /**< Optional buffer sequence ID. */

    uint32_t                  reserved0;
    /**< This field must be set to zero. */

    uint32_t                  reserved1;
    /**< This field must be set to zero. */

    uint32_t                  reserved2;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_data_cmd_write_v2_t */

/** @ingroup usm_data_evt_write_done
    Indicates that the referenced buffer has been fully consumed and is again
    available to the client.

  @apr_hdr_fields
    Opcode -- USM_DATA_EVENT_WRITE_DONE_V2 \n
    Token  -- Matches the token in the original write buffer \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID (0 is reserved)
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @apr_msg_payload{usm_data_event_write_done_v2_t}
    @table{weak__usm__data__event__write__done__v2__t}

  @return
    None.

  @dependencies
    The session/stream must be a valid and open write or read/write
    session/stream.
    @par
    A corresponding #USM_DATA_CMD_WRITE_V2 command must have been received and
    processed.
*/
#define USM_DATA_EVENT_WRITE_DONE_V2                                    0x00012727

/* Structure for data write done event payload */
typedef struct usm_data_event_write_done_v2_t usm_data_event_write_done_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_data_event_write_done_v2_t
@{ */
/* Payload of the USM_DATA_EVENT_WRITE_DONE_V2 event, which indicates
    that the referenced buffer has been fully consumed and is again available
    to the client.
*/
struct usm_data_event_write_done_v2_t
{
    uint32_t                  status;
    /**< Status message (error code) that indicates whether the referenced
         buffer has been successfully consumed.

         @values Refer to @xhyperref{Q3,[Q3]} */

     uint32_t                  buf_addr_lsw;
    /**< lsw of the 64 bit address */

     uint32_t                  buf_addr_msw;
     /**< msw of the 64 bit address. address given by the client in USM_DATA_CMD_WRITE_V2 command. */

     uint32_t                  mem_map_handle;
    /**< memory map handle in the USM_DATA_CMD_WRITE_V2  */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_data_event_write_done_v2_t */

/** @ingroup usm_data_cmd_media_format_update
    Indicates an update to the media format, which applies to future buffers
    written to this stream.

  @apr_hdr_fields
    Opcode -- USM_DATA_CMD_MEDIA_FMT_UPDATE \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID (0 is reserved)
            - Stream ID 1 -- #USM_MAX_STREAMS_PER_SESSION (0 is reserved)

  @apr_msg_payload{usm_data_cmd_media_fmt_update_t}
    @tablens{weak__usm__data__cmd__media__fmt__update__t}

  @detdesc
    The client also sends this command after opening a write stream or
    read/write stream and before sending any #USM_DATA_CMD_WRITE commands.
    This is required to properly configure the decoder.
    @par
    If a USM_DATA_CMD_WRITE_V2 command arrives before this command, the data
    buffers are discarded and a #USM_DATA_EVENT_WRITE_DONE_V2 event is raised
    with an error status for this command.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    The session/stream must be a valid and opened write session/stream in
    the Open (not Run) state.
*/
#define USM_DATA_CMD_MEDIA_FMT_UPDATE                            0x00011272

#include "adsp_begin_pack.h"

/* Structure for media format update command payload */
typedef struct usm_data_cmd_media_fmt_update_t usm_data_cmd_media_fmt_update_t;

/** @weakgroup weak_usm_data_cmd_media_fmt_update_t
@{ */
/* Payload of the USM_DATA_CMD_MEDIA_FMT_UPDATE command, which
    indicates an update to the media format that applies to future buffers
    written to this stream. */
/** Immediately following this structure is a media format block
    (usm_free_form_dec_cfg_t) of size fmt_blk_size.
*/
struct usm_data_cmd_media_fmt_update_t
{
    uint32_t                    fmt_id;
    /**< Media type of the stream. 

         @values #USM_MEDIA_FMT_FREE_FORM */

    uint32_t                    fmt_blk_size;
    /**< Media format block size in bytes.

         @values > 0 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_data_cmd_media_fmt_update_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_USM_DATA_COMMANDS_H_ */

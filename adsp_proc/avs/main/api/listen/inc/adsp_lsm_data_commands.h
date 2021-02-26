/*========================================================================*/
/**
@file adsp_lsm_data_commands.h

This file contains LSM data commands and events structures definitions.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      Any descriptions that display in the PDF are located in the
      LSM_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2013,2014 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/adsp_lsm_data_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/09/2014 sw      (Tech Pubs) Edited Doxygen comments for 2.6.
08/13/2013 SivaNaga   Initial version
========================================================================== */

#ifndef _ADSP_LSM_DATA_COMMANDS_H_
#define _ADSP_LSM_DATA_COMMANDS_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup lsmdata_cmd_read
    Delivers an empty data buffer to be filled by the LSM.

  @apr_hdr_fields
    Opcode -- LSM_DATA_CMD_READ @vertspace{3} \n
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_data_cmd_read_t}
    @table{weak__lsm__data__cmd__read__t}

  @return
    #LSM_DATA_EVENT_READ_DONE is sent from the LSM to the client processor 
    when the read buffer is filled.

  @dependencies
    The LSM stream must be valid and opened. @vertspace{3} \n
    A memory map handle must have been obtained from the aDSP using
    #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.
*/
#define LSM_DATA_CMD_READ                                         0x00012A8A

/* Payload for an LSM Data Read command. */
typedef struct lsm_data_cmd_read_t lsm_data_cmd_read_t;

#include "adsp_begin_pack.h"
                      
/** @weakgroup weak_lsm_data_cmd_read_t
@{ */
/* Payload of the LSM_DATA_CMD_READ command, which delivers a data
    buffer to the LSM.
*/
struct lsm_data_cmd_read_t
{
    uint32_t                  buf_addr_lsw;
    /**< Lower 32 bits of the address of the buffer where the aDSP buffers the
         data. */

    uint32_t                  buf_addr_msw;
    /**< Upper 32 bits of the address of the buffer where the aDSP buffers the
         data.

         The buffer address for each frame must be a valid, mapped, physical
         address that has been mapped via
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS. 

         The 64-bit number formed by buf_addr_lsw and buf_addr_msw must be
         in contiguous memory and aligned to a 32-byte boundary.

         @tblsubhd{For a 32-bit shared memory address} This buf_addr_msw
         field must be set to 0.

         @tblsubhd{For a 36-bit shared memory address} Bits 31 to 4 of this
         buf_addr_msw field must be set to 0. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS. */

    uint32_t                  buf_size;
    /**< Number of bytes available for the aDSP to write. The aDSP starts
         writing from the buffer address.

         @values > 0  */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_data_cmd_read_t */

/** @ingroup lsmdata_evt_read_done
    Indicates that the referenced buffer has been filled and is available to
    the client.

  @apr_hdr_fields
    Opcode -- LSM_DATA_EVENT_READ_DONE
    @par
    Token  -- Matches the token in the #LSM_DATA_CMD_READ command for which
              this event is raised
    @par
    Src_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_data_event_read_done_t}
    @table{weak__lsm__data__event__read__done__t}

  @detdesc
    The referenced buffer contains a variable amount of empty space for listen
    data.

  @return
    None.

  @dependencies
    A corresponding #LSM_DATA_CMD_READ command must have been received and
    processed.
*/
#define LSM_DATA_EVENT_READ_DONE                                  0x00012B02

/* Structure for the data read done event payload. */
typedef struct lsm_data_event_read_done_t lsm_data_event_read_done_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_data_event_read_done_t
@{ */
/* Payload of the LSM_DATA_EVENT_READ_DONE event, which indicates
    that the referenced buffer has been filled and is available to the client.
*/
struct lsm_data_event_read_done_t
{
   uint32_t                  status;
   /**< Status message (error code).

        @values Refer to @xhyperref{Q3,80-N1463-2} */
   /* Q3 = Async Packet Router API (for MDM9x15 and Newer) IS (80-N1463-2) */

   uint32_t                  buf_addr_lsw;
   /**< Lower 32 bits of the valid, mapped address of the buffered data. */

   uint32_t                  buf_addr_msw;
   /**< Upper 32 bits of the valid, mapped address of the buffered data.

        The 64-bit number formed by buf_addr_lsw and buf_addr_msw must be in
        contiguous memory and aligned to a 32-byte boundary.
        This address is the same as the address the client processor provides
        in #LSM_DATA_CMD_READ.

        @tblsubhd{For a 32-bit shared memory address} This buf_addr_msw field
        must be set to 0.

        @tblsubhd{For a 36-bit shared memory address} Bits 31 to 4 of this
        buf_addr_msw field must be set to 0. */

   uint32_t                  mem_map_handle;
   /**< Unique identifier for the shared memory address.

        The aDSP returns this memory map handle in
        #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS. */

   uint32_t                  total_size;
   /**< Number of bytes available for the client processor to read.
        The client starts reading from the buffer address.

        @values @ge 0 */

   uint32_t                  offset;
   /**< Offset from the buffer address to the first byte of the PCM data.
        All PCM frames are consecutive, starting from this offset.

        @values 0

        This variable is not used for look ahead buffering. */

   uint32_t                  timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer.

        @values 0

        This variable is not used for look ahead buffering. */

   uint32_t                  timestamp_msw;
   /**< Upper 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer.

        @values 0

        This variable is not used for look ahead buffering. */

   uint32_t                  flags;
   /**< Bit field of flags.

        @values{for bit 0}
        - 1 -- Timestamp is valid
        - 0 -- Timestamp is invalid

        @values{for bit 1}
        - 1 -- Frame metadata is present
        - 0 -- Frame metadata is absent

         All other bits are reserved; the aDSP sets them to 0 for the look ahead
         buffer. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_data_event_read_done_t */

/** @addtogroup lsmdata_evt_read_status
@{ */
/** Indicates the internal buffer status to the client.

  @apr_hdr_fields
    Opcode -- LSM_DATA_EVENT_STATUS
    @par
    Src_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_data_event_status_t}
    @table{weak__lsm__data__event__status__t}

  @return
    None.

  @dependencies
    None.
*/
#define LSM_DATA_EVENT_STATUS                                   0x00012B03

/** Bitmask used when a buffer overflow occurs. */
#define LSM_DATA_OVERFLOW_BIT_MASK                              0x00000001

/** @} */ /* end_addtogroup lsmdata_evt_read_status */

/* Structure for the data overflow event payload. */
typedef struct lsm_data_event_status_t lsm_data_event_status_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_data_event_status_t
@{ */
/* Payload of the LSM_DATA_EVENT_STATUS event, which indicates
    data status to the client.
*/
struct lsm_data_event_status_t
{
   uint32_t                  status;
   /**< Status bit field.

        @values{for bit 0}
        - 0 -- No overflow
        - #LSM_DATA_OVERFLOW_BIT_MASK

        All other bits are reserved. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_data_event_status_t */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_LSM_DATA_COMMANDS_H_ */

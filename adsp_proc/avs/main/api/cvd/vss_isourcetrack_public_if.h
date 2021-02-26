#ifndef __VSS_ISOURCETRACK_PUBLIC_IF_H__
#define __VSS_ISOURCETRACK_PUBLIC_IF_H__

/**
  @file vss_isourcetrack_public_if.h
*/
/*===========================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.  All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated by the U.S. Government,
Diversion contrary to U.S. law prohibited.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

when        who    what, where, why
-------     ---    ----------------------------------------------------------
11/24/15     sw    (Tech Pubs) Edited Doxygen comments for 8952.2.6.

===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * SOURCE TRACK DEFINES                                                     *
 ****************************************************************************/

/** @weakgroup weakb_source_track_values
@{ */

/** Indicates that voice activity detector output is not available. */
#define VSS_ISOURCETRACK_VOICE_ACTIVITY_NOT_AVAILABLE ( 0xFF )

/** Indicates that the arrival direction of a talker or interferer is not
    available. */
#define VSS_ISOURCETRACK_DOA_NOT_AVAILABLE ( 0xFFFF )

/** @} */ /* end_weakgroup weakb_source_track_values */

/****************************************************************************
 * SOURCE TRACK APIS                                                        *
 ****************************************************************************/

/** @ingroup cvd_source_cmd_get_activity
  Gets the sound activity data.

  @note1hang For guidelines and other information on using shared memory, see
             Section @xref{dox:ShMemGuidelines}.

  @apr_msgpayload{vss_isourcetrack_cmd_get_activity_t}
  @table{weak__vss__isourcetrack__cmd__get__activity__t}

  @par Layout of sound activity data (vss_isourcetrack_activity_data_t)
  @table{weak__vss__isourcetrack__activity__data__t}

  @detdesc
  Sound activity data include the voice activity detector output,
  arrival direction of a talker, arrival direction of an interferer, 
  and sound strengths at each degree on 360-degree circle.
  @par
  Recommendation: get sound activity data every 50 ms.
  @par
  Client only reads the data from shared memory after receiving APR_EOK as
  the command response. The layout of sound activity data in shared memory is
  defined by the vss_isourcetrack_activity_data_t structure.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xrefcond{Q2,80-N1463-2,80-NA601-5}).

  @dependencies
  Before using this command, the client must have mapped memory with the CVD
  successfully.

  @inputfile{group__weakb__source__track__values.tex}
*/
#define VSS_ISOURCETRACK_CMD_GET_ACTIVITY ( 0x00013136 )

/* Type definition for vss_isourcetrack_cmd_get_activity_t. */
typedef struct vss_isourcetrack_cmd_get_activity_t vss_isourcetrack_cmd_get_activity_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_isourcetrack_cmd_get_activity_t
@{ */
/* Payload structure for the VSS_ISOURCETRACK_CMD_GET_ACTIVITY command.
 */
struct vss_isourcetrack_cmd_get_activity_t
{
  uint32_t mem_handle;
  /**< Unique identifier for the shared memory address of the sound activity
       data payload. */

  uint64_t mem_address;
  /**< Shared memory address of the payload. */

  uint32_t mem_size;
  /**< Size of the memory allocated for the payload.

       @values > 0 bytes

       The size must also be large enough to accommodate the sound activity
       data as defined in the vss_isourcetrack_activity_data_t structure.

       Also, the amount of memory allocated at cal_mem_address must meet the
       requirement as specified in Section @xref{dox:ShMemGuidelines}. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_isourcetrack_cmd_get_activity_t */

/* Type definition for vss_isourcetrack_activity_data_t. */
typedef struct vss_isourcetrack_activity_data_t vss_isourcetrack_activity_data_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_isourcetrack_activity_data_t
@{ */
/* Structure for the sound activity data.
*/
struct vss_isourcetrack_activity_data_t
{
  uint8_t voice_active[ 8 ];
  /**< Array of voice activity detector outputs that correspond to each sector
       (for information on sectors, see #VSS_ISOUNDFOCUS_CMD_SET_SECTORS).

       @values 
       - TRUE -- Voice is active on the corresponding sector
       - FALSE -- Voice is not active on the corresponding sector
       - #VSS_ISOURCETRACK_VOICE_ACTIVITY_NOT_AVAILABLE @vertspace{-28} */

  uint16_t talker_doa;
  /**< Arrival direction (in degrees) of a talker.

       @values 
       - If the arrival direction is available -- Value between 0 and 359
       - Otherwise -- #VSS_ISOURCETRACK_DOA_NOT_AVAILABLE @vertspace{-28} */

  uint16_t interferer_doa[ 3 ];
  /**< Array of arrival directions (in degrees) of interferers. 
       The maximum number of interferers that can be tracked is three.

       @values
       - If the arrival direction is available -- Value between 0 and 359
       - Otherwise -- #VSS_ISOURCETRACK_DOA_NOT_AVAILABLE @vertspace{-28} */

  uint8_t sound_strength[ 360 ];
  /**< Array of sound strength at each degree on a 360-degree plane. 

       @values Integer between 0 and 255 */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_isourcetrack_activity_data_t */


#endif /* __VSS_ISOURCETRACK_PUBLIC_IF_H__ */

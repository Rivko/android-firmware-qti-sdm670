/*========================================================================*/
/**
@file adsp_usm_session_commands.h

@brief This file contains USM session commands and events structures
definitions.
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

$Header: //components/rel/ultrasound.adsp/2.7/api/adsp_usm_session_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/06/12   sw      (Tech Pubs) Updated legal statements (lines 20 & 22) for QTI;
                   updated Doxygen markup for D.2 templates.
02/14/12   sw      (Tech Pubs) Edited Doxygen markup/comments for Rev B of the IS.
10/18/11   sw      (Tech Pubs) Edited Doxygen markup/comments for new Interface Spec.
10/13/10   NR      File created

========================================================================== */

#ifndef _ADSP_USM_SESSION_COMMANDS_H_
#define _ADSP_USM_SESSION_COMMANDS_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup usm_session_cmd_start_session
    Starts all streams in an ultrasound session.

  @apr_hdr_fields
    Opcode -- USM_SESSION_CMD_RUN \n
    Dst_port:
            - Session ID 1 -- #USM_MAX_SESSION_ID
            - Stream ID is ignored

  @apr_msg_payload{usm_session_cmd_run_t}
    @table{weak__usm__session__cmd__run__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    At least one stream in this session must have already been opened and
    not yet closed.
*/
/* Q3 = Asynchronous Packet Router API (for MDM9x15 and Newer) Interface Spec
   (80-N1463-2) */
#define USM_SESSION_CMD_RUN                                         0x00012306

/** @addtogroup usm_constants
@{ */
/** Maximum number of possible session IDs. IDs start from 1; 0 is reserved. */
#define USM_MAX_SESSION_ID                                          (2)

/** Maximum number of sessions. */
#define USM_MAX_NUM_SESSIONS                                USM_MAX_SESSION_ID

/** Maximum number of streams per session. */
#define USM_MAX_STREAMS_PER_SESSION                                 (1)

/** @} */ /* end_addtogroup usm_constants */

/* Payload structure for USM session run command */
typedef struct usm_session_cmd_run_t usm_session_cmd_run_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_session_cmd_run_t
@{ */
/* Payload of the USM_SESSION_CMD_RUN command, which starts all
    streams in an ultrasound session.
*/
struct usm_session_cmd_run_t
{
    uint32_t                  reserved0;
    /**< This field must be set to zero. */

    uint32_t                  reserved1;
    /**< This field must be set to zero. */

    uint32_t                  reserved2;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_session_cmd_run_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_USM_SESSION_COMMANDS_H_ */

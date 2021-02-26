/*========================================================================
adsp_privatedefs.h

This file contains private ADSP APIs

Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_PrivateApi.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/25/11   RP      Created

========================================================================== */
/**
@file adsp_privatedefs.h

@brief This file contains private ADSP APIs
*/

#ifndef _ADSP_PRIVATEAPI_H_
#define _ADSP_PRIVATEAPI_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup adsp_privatedefs
    @{ */

/**  Internal API. Opcode for exiting the premordial main
 *   thread */
#define AVCS_CMD_EXIT_PREMORDIAL_THREAD     0x00012910

/**  Internal API. Opcode for deleting all the stored licenses. */
#define AVCS_CMD_DELETE_LICENSES             0x00012911

/**  Internal API. Opcode for deleting all the custom topologies. */
#define AVCS_CMD_CLEAR_PP_DBS   0x00012913


/**
    ID of the AVSYNC Test module.for collecting AVSYNC statistics

    This module supports the following parameter IDs:
    - #AFE_PRV_PARAM_ID_AVSYNC_STATS
    - #AFE_PARAM_ID_ENABLE

    Opcode -- AFE_PRV_MODULE_AVSYNC_TEST
  @newpage
*/
#define AFE_PRV_MODULE_AVSYNC_TEST			0x0001021A

#define AFE_PRV_PARAM_ID_AVSYNC_STATS		0x0001021B

typedef struct afe_prv_avsync_stats_param_t afe_prv_avsync_stats_param_t;

#include "adsp_begin_pack.h"
struct afe_prv_avsync_stats_param_t
{
   uint32_t 	  non_zero_ts_lsw;
   /**< LSW of the timestamp of the nonzero sample */

   uint32_t 	  non_zero_ts_msw;
   /**< MSW of the timestamp of the nonzero sample. the 64 bit value formed by LSW and MSW is interpreted as an unsigned number */

   uint16_t       is_valid;
   /**< whether above TS is valid. 1-> valid, 0-> invalid.*/

   uint16_t       reserved;
   /**< must be set to zero */
}
#include "adsp_end_pack.h"
;

/** @ingroup adm_cmd_set_pspd_mtmx_strtr_param_v5
    This command sets a parameter related to a parameter on
    matrix mixer or stream router on the per-stream, per-device
    pathway.

  @apr_hdr_fields
    Opcode -- ADM_CMD_SET_PSPD_MTMX_STRTR_PARAM_V5 \n
    Dst_port: ignored

  @apr_msg_payload{adm_cmd_set_pspd_mtmx_strtr_param_v5_t}
    @table{weak_adm_cmd_set_pspd_mtmx_strtr_param_v5_t}

  @par Parameter data variable payload 
       (adm_pspd_param_data_t)
    @tablens{weak_adm_pspd_param_data_t}

  @keep{6} @detdesc
    This command sets a parameter related to a parameter on
    matrix mixer or stream router on the per-stream, per-device
    pathway.

    @par
    If mem map handle =NULL, a series of
    adm_pspd_param_data_t structures immediately follow,
    whose total size is data_payload_size bytes.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    The session and the device must be opened and valid.
*/
#define ADM_CMD_SET_PSPD_MTMX_STRTR_PARAMS_V5                   0x0001033A

/* Structure for a set param command on matrix or stream router on the per-stream, per-device pathway. */
typedef struct adm_cmd_set_pspd_mtmx_strtr_params_v5_t adm_cmd_set_pspd_mtmx_strtr_params_v5_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_adm_cmd_set_pspd_mtmx_strtr_params_v5_t
@{ */
/* Payload of the ADM_CMD_SET_PSPD_MTMX_STRTR_PARAMS_V5 command, which allows
    one or more parameters to be set on a matrix or stream router on the per-strean, per-device pathway.
*/
struct adm_cmd_set_pspd_mtmx_strtr_params_v5_t
{
    uint32_t                  data_payload_addr_lsw;
    uint32_t                  data_payload_addr_msw;
    uint32_t                  mem_map_handle;
   /**< Parameter data payload address (LSW, MSW) and map handle.

        @values for map handle
        - NULL -- Message is in the payload
        - Non-NULL -- Physical address to the payload data in shared memory

        If the map handle is non-NULL, the <b>Parameter data variable
        payload</b> (see below) begins at the specified address. */

    uint32_t                  data_payload_size;
    /**< Actual size of the variable payload accompanying the message, or in
         shared memory. This field is used for parsing the parameter payload.

         @values > 0 bytes */

    uint16_t                  direction;
    /**< The direction of the entity on which parameter is to be set.

         @values
         0 - RX 
         1 - TX  */

    uint16_t                  data_format;
    /**< Indicates which service this set param is intended for, by using the data format.

         @values
         0 - LPCM
         1 - Compressed  */

    uint16_t                  session_id;
    /**< Indicates which session this command is applicable for..

         @values
         1 -- #ASM_MAX_SESSION_ID */

    uint16_t                  device_id;
    /**< Indicates which device this command is applicable for..

         @values
         For LPCM, its is the COPP ID: 0 @le device_id < ADM_MAX_COPPS
         For compressed, it is the AFE port ID*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_adm_cmd_set_pspd_mtmx_strtr_params_v5_t */

/* Structure for session parameter data. */
typedef struct adm_pspd_param_data_t adm_pspd_param_data_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_adm_pspd_param_data_t
@{ */
/* Payload of the per-session, per-device parameter data of the
    #ADM_CMD_SET_PSPD_MTMX_STRTR_PARAMS_V5 command. */
/** Immediately following this structure are param_size bytes of parameter
    data. The structure and size depend on the module_id/param_id pair.
*/
struct adm_pspd_param_data_t
{
    uint32_t                  module_id;
    /**< Valid ID of the module to be configured (see Section
         @xref{hdr:MatrixStrmRtrModuleIDs}). */

    uint32_t                  param_id;
    /**< Valid ID of the parameter to be configured (see Section
         @xref{hdr:MatrixStrmRtrParamIDs}). */

    uint16_t                  param_size;
    /**< Data size of the param_id/module_id combination. This is a multiple
         of four bytes.

         @values > 0 bytes */

    uint16_t                  reserved;
    /**< This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_adm_pspd_param_data_t */

/** @cond OEM_only */
/** @addtogroup asm_strm_cmd_set_encdec_params
@{ */

#define ASM_PARAM_ID_REG_DEC_ERR_EVENT                                0x00010DCE

typedef struct asm_dec_err_param_t asm_dec_err_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_dec_err_param_t
@{ */
/* Payload of the ASM_PARAM_ID_REG_DEC_ERR_EVENT parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_dec_err_param_t
{
    uint16_t                 n_dec_err_threshold;
    /**< Paremter specifying how many consecutive decoder errors occur before an event
     *   is raised.  0 means disabling.

         @values
         #0 - Deregister event raising
         # all positive values*/
    uint16_t                 timeout_ms;
    /**< Paremter specifying how many miliseconds will occur before an event
         *   is raised on consecutive error before threshold.
         *   0 means disabling.

             @values
             #0 - no timeout, always wait until threshold is reached.
             # all positive values*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_dec_err_param_t */
/** @endcond*/

/** @ingroup asm_data_event_dec_err_update
    Notifies the client of a consecutive error reaching a preset number. This
    event is raised by the decoder service.

  @apr_hdr_fields
    Opcode -- ASM_DATA_EVENT_DEC_ERR_NOTIFY \n
    Src_port:
            - Session ID 1 -- #ASM_MAX_SUPPORTED_SESSION_ID
            - Stream ID 1 -- #ASM_MAX_STREAMS_PER_SESSION

  @apr_msg_payload{asm_data_event_dec_err_t}
   none. This event does not carry a payload.

  @detdesc
    This event is raised when all of the following conditions are true:
      - Such functionality has been enabled by an
        ASM_STREAM_CMD_SET_ENCDEC_PARAM command, where the preset number is
        specified, denoted as N, the preset timeout is specified also, denoted
        as T
      - One of the following has occured
         - N consecutive decoder error has occured.
         - >1 consecutive decoder error has occured and T ms has passed.
    @par
    After this event is raised, decoder will continue processing and wait for
    instructions from client.

  @return
    None.

  @dependencies
    None.*/

#define ASM_DATA_EVENT_DEC_ERR_NOTIFY                        0x00010DCF




/** @} */ /* end_addtogroup adsp_privatedefs */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_PRIVATEAPI_H_ */

/**
@file adsp_lsm_session_commands.h

@brief This file contains LSM session commands and events structures
definitions.
*/

/*==============================================================================
NOTE: The @brief description above does not appear in the PDF.
      Any descriptions that display in the PDF are located in the
      LSM_mainpage.dox file. Contact Tech Pubs for assistance.
==============================================================================*/

/*==============================================================================
  Copyright (c) 2012, 2014-2017 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/api/listen/inc/adsp_lsm_session_commands.h#1 $

  when         who      what, where, why
  --------     ---      -------------------------------------------------------
  03/17/2017   unni     Moved module related details to separate files
  07/16/2016   sw       (Tech Pubs) Edited Doxygen comments for AVS 2.8.
  10/30/2015   sw       (Tech Pubs) Merged updated Doxygen comments from 8952.
  12/09/2014   sw       (Tech Pubs) Edited Doxygen comments for 2.6.
  07/17/2014   Unni     Added LSM_SESSION_CMD_OPEN_TX_V2 working on topology_id
  05/12/2014   sw       (Tech Pubs) Edited Doxygen comments/markup for 2.4.
  11/07/2012   Sudhir   Initial version
==============================================================================*/

#ifndef _ADSP_LSM_SESSION_COMMANDS_H_
#define _ADSP_LSM_SESSION_COMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#include "mmdefs.h"
#include "framework_module.h"
#include "lab.h"
#include "audio_common.h"
#include "detection_engine.h"


/** @ingroup lsmsess_datatypes
    Maximum number of possible session IDs. IDs start from 1; 0 is reserved. */
#define LSM_MAX_SESSION_ID                        (8)

/** @ingroup lsmdepr_command
    Deprecated.

    Maximum number of LSM sessions.  @newpage*/
#define LSM_MAX_NUM_SESSIONS                      LSM_MAX_SESSION_ID

/** @addtogroup lsmsess_cmd_sh_mem_map_reg
@{ */
/** Commands the LSM session to map multiple shared memory regions. All the
    mapped regions must be from the same memory pool.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmd_shared_mem_map_regions_t}
    @tablens{weak__lsm__session__cmd__shared__mem__map__regions__t}

  @mappayload{lsm_session_shared_map_region_payload_t}
    @table{weak__lsm__session__shared__map__region__payload__t}

  @return
    #LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS if successful.
    @par
    Error code for a failure (refer to @xrefcond{Q4,80-NF774-3,80-NA610-3}).

  @dependencies
    None.
*/
#define LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS    0x00012A7F

/** Enumeration for the shared memory, 8-byte addressable, 4-KB aligned
    pool ID.
 */
#define ADSP_MEMORY_MAP_SHMEM8_4K_POOL            3

/** @} */ /* end_addtogroup lsmsess_cmd_sh_mem_map_reg */

/* LSM Session shared memory map regions structure  */
typedef struct  lsm_session_cmd_shared_mem_map_regions_t
                lsm_session_cmd_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_shared_mem_map_regions_t
@{ */
/* Memory map regions command payload used by the
    LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.
    This structure allows clients to map multiple shared memory regions in a
    single command. */
/** Following this structure are num_regions of
    lsm_session_shared_map_region_payload_t.
*/
struct lsm_session_cmd_shared_mem_map_regions_t
{
    uint16_t                  mem_pool_id;
    /**< Type of memory on which this memory region is mapped.

         @values
         - #ADSP_MEMORY_MAP_SHMEM8_4K_POOL
         - Other values are reserved

         The memory pool ID implicitly defines the characteristics of the
         memory.
         Characteristics can include alignment type, permissions, and so on.

         ADSP_MEMORY_MAP_SHMEM8_4K_POOL is shared memory, 8-byte addressable,
         and 4-KB aligned. */

    uint16_t                  num_regions;
    /**< Number of regions to map.

         @values > 0 */

    uint32_t                  property_flag;
    /**< Configures one common property for all the regions in the payload.

         @values 0x00000000 to 0x00000001

         Bit 0 indicates physical or virtual mapping:
         - 0 -- The shared memory address provided in
           lsm_session_shared_map_region_payload_t is a physical address.
           The shared memory must be mapped (hardware TLB entry), and a
           software entry must be added for internal bookkeeping.
         - 1 -- The shared memory address provided in
           lsm_session_shared_map_region_payload_t is a virtual address.
           The shared memory must not be mapped (because the hardware TLB entry
           is already available), but a software entry must be added for
           internal bookkeeping. \n @vertspace{2}
           This mapping is useful if two services within the aDSP are
           communicating via the APR. They can directly communicate via the
           virtual address instead of the physical address. The virtual regions
           must be contiguous. In this case, num_regions must be 1.

         Bits 31 to 1 are reserved and must be set to 0. */
} 
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_shared_mem_map_regions_t */

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_shared_map_region_payload_t
@{ */
/* Map region payload used by the lsm_session_cmd_shared_mem_map_regions_t structure.
*/
struct lsm_session_shared_map_region_payload_t
{
    uint32_t                  shm_addr_lsw;
    /**< Lower 32 bits of the shared memory address of the memory region to
         map.

         @values Any valid 32-bit address */

    uint32_t                  shm_addr_msw;
    /**< Upper 32 bits of the shared memory address of the memory region to
         map.

         The 64-bit number formed by shm_addr_lsw and shm_addr_msw word must be
         in contiguous memory and aligned to a 32-byte boundary.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 must be set to 0.
         - For a 64-bit shared memory address, any 32-bit value.
         @tablebulletend */

    uint32_t                  mem_size_bytes;
    /**< Number of bytes in the region.

         @values Multiples of 4 KB

         The aDSP always maps the regions as virtual contiguous memory.
         To avoid gaps in the virtual contiguous mapped memory,
         the memory size must be in multiples of 4 KB. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_shared_map_region_payload_t */

/** @ingroup lsmsess_rsp_sh_mem_map_reg
    Sent by the LSM session to the client processor in response to the
    successful processing of #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS

  @apr_msgpayload{lsm_session_cmdrsp_shared_mem_map_regions_t}
    @table{weak__lsm__session__cmdrsp__shared__mem__map__regions__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    A corresponding #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS command must have
    been sent to receive this response from the LSM session.
*/
#define LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS         0x00012A80

/* LSM Session shared memory map regions response structure  */
typedef struct  lsm_session_cmdrsp_shared_mem_map_regions_t
                lsm_session_cmdrsp_shared_mem_map_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmdrsp_shared_mem_map_regions_t
@{ */
/* Memory map command response payload used by the
    LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS commands.

    This structure is  response to the memory mapping
    regions command. mem_map_handle must later be used by the
    client to unmap regions that are previously mapped.
*/
struct lsm_session_cmdrsp_shared_mem_map_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         @values Any 32-bit value

         The aDSP returns the memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS. This handle can be used as
         an abstract representation of the shared memory regions that are
         being mapped. If the command fails, a generic APR error response is
         returned to the client.

         The aDSP uses this handle to retrieve the shared memory attributes.

         #LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS uses this handle to unmap
         the mapped memory. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmdrsp_shared_mem_map_regions_t */

/** @ingroup lsmsess_cmd_sh_mem_unmap_reg
    Commands the LSM to unmap multiple shared memory regions
    that were previously mapped.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmd_shared_mem_unmap_regions_t}
    @table{weak__lsm__session__cmd__shared__mem__unmap__regions__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    A corresponding #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS command must have
    been processed successfully.
*/
#define LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS          0x00012A81

/* LSM session shared memory unmap regions command structure */
typedef struct lsm_session_cmd_shared_mem_unmap_regions_t
               lsm_session_cmd_shared_mem_unmap_regions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_shared_mem_unmap_regions_t
@{ */
/* Memory unmap regions command payload used by the
    #LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS
    This structure allows clients to unmap multiple shared memory regions in
    a single command.
*/
struct lsm_session_cmd_shared_mem_unmap_regions_t
{
    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         @values Any 32-bit value

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

         The aDSP uses this handle to retrieve the associated shared
         memory regions and unmap them. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_shared_mem_unmap_regions_t */

/** @ingroup lsmdepr_command
    Deprecated. Use #LSM_SESSION_CMD_OPEN_TX_V2.

    This command creates a dynamic LSM session for a listen application. The
    type of application depends on the application ID specified in the payload.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_OPEN_TX
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID (0 is reserved)
    
  @apr_msgpayload{lsm_session_cmd_open_tx_t}
    @table{weak__lsm__session__cmd__open__tx__t}

  @detdesc
    When creating an LSM session, the client processor must specify the session
    ID. Currently, up to #LSM_MAX_SESSION_ID groups are supported.
    @par
    After receiving this command, the LSM creates a dynamic service that
    handles the detection engine algorithm.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    None. @newpage
*/
#define LSM_SESSION_CMD_OPEN_TX               0x00012A82

/** @addtogroup lsmdepr_app_ids
@{ */
/** @cond *//* not published for customers*/
/* Deprecated; functionality removed.

    Voice wake up application ID.
*/
#define LSM_VOICE_WAKEUP_APP                  0x1

/* Deprecated. Use LSM_MODULE_VOICE_WAKEUP_V2.

    Voice wake up application ID for detecting multiple keywords and multiple
    users.
*/
#define LSM_VOICE_WAKEUP_APP_V2               0x2
/** @endcond */

/** Deprecated.

    Size of the range of application IDs for Hexagon customer algorithms and
    third party algorithms.
*/
#define LSM_APP_ID_RANGE_FOR_OPEN_DSP         0x10

/** Deprecated.

    Start of the range of application IDs for Hexagon customer algorithms and
    third party algorithms.
*/
#define LSM_APP_ID_START_FOR_OPEN_DSP         0x1000

/** Deprecated.

    End of the range of application IDs for Hexagon Access and
    third-party algorithms. @hideinitializer
*/
#define LSM_APP_ID_END_FOR_OPEN_DSP      ( LSM_APP_ID_START_FOR_OPEN_DSP + \
                                           LSM_APP_ID_RANGE_FOR_OPEN_DSP - 1 )

/** @} */ /* end_addtogroup lsmdepr_app_ids */

/* Deprecated. Use lsm_session_cmd_open_tx_v2_t instead.
   Payload format for an LSM open session command.
*/
typedef struct lsm_session_cmd_open_tx_t lsm_session_cmd_open_tx_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_open_tx_t
@{ */
/* Deprecated. Use lsm_session_cmd_open_tx_v2_t instead.
   Payload of the command LSM_SESSION_CMD_OPEN_TX,
           which is used for opening LSM session.
*/
struct lsm_session_cmd_open_tx_t
{
    uint16_t app_id;
    /**< ID of the application the LSM is to create/open.

         @values
         - Range of #LSM_APP_ID_START_FOR_OPEN_DSP to
           #LSM_APP_ID_END_FOR_OPEN_DSP (for Hexagon customers who want to use
           a new ID instead of an existing application ID). @tablebulletend */

    uint16_t reserved;
    /**< Used for alignment; must be set to 0. */

    uint32_t sample_rate;
    /**< Sampling rate at which the detection engine application or target
         sound detection application runs.

         @values 16000 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_open_tx_t */

/** @addtogroup lsmsess_cmd_open_tx2
@{ */
/** Creates dynamic sessions in the Listen server.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_OPEN_TX_V2
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID (0 is reserved)

  @apr_msgpayload{lsm_session_cmd_open_tx_v2_t}
    @table{weak__lsm__session__cmd__open__tx__v2__t}

  @description
    The session type depends on the collection of modules (graph) specified
    in the payload.
    @par
    When creating a session, the client processor must specify the session IDs.
    Currently, up to #LSM_MAX_SESSION_ID groups are supported.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    None.
*/
#define LSM_SESSION_CMD_OPEN_TX_V2        0x00012A8B

/** Enumeration for setting the maximum number of modules in an open
    LSM session.
*/
#define LSM_MAX_MODULES_PER_GRAPH         8

/** @} */ /* end_addtogroup lsmsess_cmd_open_tx2 */

/* Payload format for an LSM open session command (v2). */
typedef struct lsm_session_cmd_open_tx_v2_t lsm_session_cmd_open_tx_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_open_tx_v2_t
@{ */
/*  payload of the command LSM_SESSION_CMD_OPEN_TX_V2,
           which is used for opening LSM session.
*/
struct lsm_session_cmd_open_tx_v2_t
{
    uint32_t topology_id;
    /**< Topology to use in this LSM session.

         @values See Chapter @xref{dox:topos} */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup lsmsess_session_cmd_open_tx_v2 */

/** @ingroup lsmdepr_command
    Deprecated. Use #LSM_SESSION_CMD_SET_PARAMS_V3.

    Sets one or more parameters in the LSM session.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_SET_PARAMS
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmd_set_params_t}
    @tablens{weak__lsm__session__cmd__set__params__t} @newpage

  @par Parameter data variable payload (lsm_session_param_data_t)
    @tablens{weak__lsm__session__param__data__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    This command is accepted only after #LSM_SESSION_CMD_OPEN_TX_V2 is
    processed successfully. @newpage
*/
#define LSM_SESSION_CMD_SET_PARAMS        0x00012A83

/* Payload format for a  Set Parameters command. */
typedef struct lsm_session_cmd_set_params_t lsm_session_cmd_set_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_set_params_t
@{ */
/* Payload of the LSM_SESSION_CMD_SET_PARAMS command, which allows
    one or more parameters to set on an open session. */
/** If the 64-bit data_payload_addr=NULL, a series of %lsm_session_param_data_t
    structures immediately follow, whose total size is data_payload_size bytes.
*/
struct lsm_session_cmd_set_params_t
{

    uint32_t                  data_payload_size;
   /**< Actual size of the variable payload accompanying the message or in
        shared memory. This field is used for parsing both in-band and
        out-of-band data.

        @values > 0 bytes */

    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the payload physical address. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the payload physical address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be in contiguous memory and aligned to a
         32-byte boundary.

         @values
         - For in-band data, this field must be set to 0.
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

         @values
          - NULL -- In-band
             - The message is in the payload.
             - A series of %lsm_session_param_data_t structures immediately
               follow this structure.
             - The total size is data_payload_size bytes. @vertspace{2}
          - Non-NULL -- Out-of-band
             - The payload begins at the specified physical address.
             - The total size is data_payload_size bytes. @tablebulletend */

        /* An optional field is available if parameter data is in-band:
        %lsm_session_param_data_t param_data[...] */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_set_params_t */

/* Payload format for  parameter data. */
typedef struct lsm_session_param_data_t lsm_session_param_data_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_param_data_t
@{ */
/* Payload of the session parameter data of the
    LSM_SESSION_CMD_SET_PARAMS command.*/
/** Immediately following this structure are param_size bytes of calibration
    data, which must be aligned to 4 bytes. The structure and size depend on
    the module_id/param_id combination.
*/
struct lsm_session_param_data_t
{
    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint16_t                  param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_param_data_t */

/** @ingroup lsmdepr_command
    Deprecated. Use #LSM_SESSION_CMD_GET_PARAMS_V3.

    Gets a configuration parameter from an LSM module.

    For in-band messages, the requested value is populated in the command
    response. For out-of-band messages, the requested value is present in the
    shared memory location pointed to by the data payload address field.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_GET_PARAMS
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmd_get_params_t}
    If the 64-bit data_payload_addr=NULL, an %lsm_session_param_data_t
    structure immediately follows, whose total size is data_payload_size bytes.
    @tablens{weak__lsm__session__cmd__get__params__t}

  @par Parameter data variable payload (lsm_session_param_data_t)
    @tablens{weak__lsm__session__param__data__t}

  @return
    #LSM_SESSION_CMDRSP_GET_PARAMS

  @dependencies
    This command is accepted only after #LSM_SESSION_CMD_OPEN_TX_V2 is
    processed successfully. @newpage
*/
#define LSM_SESSION_CMD_GET_PARAMS        0x00012A8D

/* Payload format for a  Get Parameter command. */
typedef struct lsm_session_cmd_get_params_t lsm_session_cmd_get_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_get_params_t
@{ */
/* Payload of the LSM_SESSION_CMD_GET_PARAMS command, which allows
   a single parameter to be queried form the module.
*/
struct lsm_session_cmd_get_params_t
{

    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the payload physical address.

         @values For in-band data, this field must be set to 0. */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the payload physical address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be in contiguous memory and aligned to a
         32-byte boundary.

         @values
         - For in-band data -- Both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address -- This field must be set to 0.
         - For a 36-bit shared memory address -- Bits 31 to 4 of this field
           must be set to 0.

         If the size of the shared memory is specified, it must be large enough
         to contain the entire payload. */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

         @values
          - NULL -- In-band
             - The message is in the payload.
             - A series of %lsm_session_param_data_t structures immediately
               follow this structure.
             - The total size is data_payload_size bytes. @vertspace{2}
          - Non-NULL -- Out-of-band
             - The payload begins at the specified physical address.
             - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint32_t                  param_max_size;
    /**< Maximum size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_get_params_t */


/** @ingroup lsmdepr_command
    Deprecated. Use #LSM_SESSION_CMDRSP_GET_PARAMS_V3.

    Returns parameter values in response to an #LSM_SESSION_CMD_GET_PARAMS
    command.

    For in-band messages, the lsm_session_param_data_t structure and actual
    content being queried follow the status field. For out-of-band messages,
    the actual payload is returned in the allocated out-of-band memory.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMDRSP_GET_PARAMS
    @par
    Src_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmdrsp_get_params_t}
    Immediately following this structure is the acknowledgment <b>Parameter
    data variable payload</b>.
    @tablens{weak__lsm__session__cmdrsp__get__params__t}

  @par Acknowledgment parameter data variable payload (lsm_session_param_data_t)
    @tablens{weak__lsm__session__param__data__t}

  @return
    None.

  @dependencies
    None. @newpage
*/
#define LSM_SESSION_CMDRSP_GET_PARAMS                             0x00012A8E

/* Payload for LSM_SESSION_CMDRSP_GET_PARAMS. */
typedef struct lsm_session_cmdrsp_get_params_t lsm_session_cmdrsp_get_params_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmdrsp_get_params_t
@{ */
/* Payload of the LSM_SESSION_CMDRSP_GET_PARAMS message, which returns
    parameter values in response to an LSM_SESSION_CMD_GET_PARAMS command.
*/
struct lsm_session_cmdrsp_get_params_t
{
    uint32_t                  status;
    /**< Status message (error code).

         @values Refer to @xrefcond{Q4,80-NF774-3,80-NA610-3} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmdrsp_get_params_t */

/** @ingroup lsmdepr_command
  Deprecated. Use #LSM_SESSION_CMD_SET_PARAMS_V3.

  Sets one or more parameters in the LSM session.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_SET_PARAMS_V2
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmd_set_params_v2_t}
    @tablens{weak__lsm__session__cmd__set__params__v2__t}

  @par Parameter data variable payload (lsm_session_param_data_v2_t)
    @tablens{weak__lsm__session__param__data__v2__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    This command is accepted only after #LSM_SESSION_CMD_OPEN_TX_V2 is
    processed successfully. @newpage
*/
#define LSM_SESSION_CMD_SET_PARAMS_V2        0x00012A8F

/* Payload format for a Set Parameters V2 command. */
typedef struct lsm_session_cmd_set_params_v2_t lsm_session_cmd_set_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_set_params_v2_t
@{ */
/* Payload of the LSM_SESSION_CMD_SET_PARAMS_V2 command, which allows
    one or more parameters to set on an open session. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%lsm_session_param_data_v2_t).
*/
struct lsm_session_cmd_set_params_v2_t
{

    uint32_t                  data_payload_size;
   /**< Actual size of the variable payload accompanying the message or in
        shared memory. This field is used for parsing both in-band and
        out-of-band data.

        @values > 0 bytes */

    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the payload address.

         @values For in-band data, this field must be set to 0 */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be in contiguous memory and aligned to a
         32-byte boundary.

         @values
         - For in-band data, both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

    uint32_t                  mem_map_handle;
   /**< Unique identifier for the shared memory address.

        The aDSP returns this handle in
        #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

        @values
         - NULL -- In-band
            - The message is in the payload.
            - A series of %lsm_session_param_data_v2_t structures immediately
              follow this structure.
            - The total size is data_payload_size bytes. @vertspace{2}
         - Non-NULL -- Out-of-band
            - The payload begins at the address specified by a pointer
              to the physical address in shared memory.
            - The total size is data_payload_size bytes. @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_set_params_v2_t */

/* Payload format for  parameter data v2. */
typedef struct lsm_session_param_data_v2_t lsm_session_param_data_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_param_data_v2_t
@{ */
/* Payload of the session parameter data of the
    LSM_SESSION_CMD_SET_PARAMS_V2 command.*/
/** Immediately following this structure are param_size bytes of calibration
    data. The structure and size depend on the module_id/param_id combination.
*/
struct lsm_session_param_data_v2_t
{
    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint32_t                  param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_param_data_v2_t */

/** @ingroup lsmsess_cmd_set_params3
    Configures one or more parameters in the LSM session.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_SET_PARAMS_V3
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmd_set_params_v3_t}
    @tablens{weak__lsm__session__cmd__set__params__v3__t}

  @par Parameter data variable payload (lsm_session_param_data_v3_t)
    @tablens{weak__lsm__session__param__data__v3__t}

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    This command is accepted only after #LSM_SESSION_CMD_OPEN_TX_V2 is
    processed successfully.
*/
#define LSM_SESSION_CMD_SET_PARAMS_V3        0x00012A92

/* Payload format for a Set Parameters V3 command. */
typedef struct lsm_session_cmd_set_params_v3_t lsm_session_cmd_set_params_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_set_params_v3_t
@{ */
/* Payload of the LSM_SESSION_CMD_SET_PARAMS_V3 command, which allows
    one or more parameters to set on an open session. */
/** For in-band data, immediately following this structure is the
    <b>Parameter data variable payload</b> (%lsm_session_param_data_v3_t).
*/
struct lsm_session_cmd_set_params_v3_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the payload address.

         @values For in-band data, this field must be set to 0 */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be in contiguous memory and aligned to a
         32-byte boundary.

         @values
         - For in-band data, both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

         @values
          - NULL -- In-band
             - The message is in the payload.
             - A series of %lsm_session_param_data_t structures immediately
               follow this structure.
             - The total size is data_payload_size bytes. @vertspace{2}
          - Non-NULL -- Out-of-band
             - The payload begins at the specified physical address.
             - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  data_payload_size;
   /**< Actual size of the variable payload accompanying the message or in
        shared memory. This field is used for parsing both in-band and
        out-of-band data.

        @values > 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_set_params_v3_t */

/* Payload format for  parameter data v3. */
typedef struct lsm_session_param_data_v3_t lsm_session_param_data_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_param_data_v3_t
@{ */
/* Payload of the session parameter data of the
    LSM_SESSION_CMD_SET_PARAMS_V3 command.*/
/** Immediately following this structure are param_size bytes of calibration
    data, which must be aligned to 4 bytes. The structure and size depend on
    the module_id/param_id combination.
*/
struct lsm_session_param_data_v3_t
{
    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint16_t                  instance_id;
    /**< Valid ID of the module instance. */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint32_t                  param_size;
    /**< Size of the parameter data based on the module_id/param_id
         combination.

        @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_param_data_v3_t */

/** @ingroup lsmdepr_command
  Deprecated. Use #LSM_SESSION_CMD_GET_PARAMS_V3.

  Gets a configuration parameter from an LSM module.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_GET_PARAMS_V2
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmd_get_params_v2_t}
    @tablens{weak__lsm__session__cmd__get__params__v2__t}

  @return
    #LSM_SESSION_CMDRSP_GET_PARAMS_V2

  @dependencies
    This command is accepted only after #LSM_SESSION_CMD_OPEN_TX_V2 is
    processed successfully. @newpage
*/
#define LSM_SESSION_CMD_GET_PARAMS_V2        0x00012A90

/* Payload format for a  Get Parameter command. */
typedef struct lsm_session_cmd_get_params_v2_t lsm_session_cmd_get_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_get_params_v2_t
@{ */
/* Payload of the LSM_SESSION_CMD_GET_PARAMS_V2 command, which allows
   a single parameter to be queried form the module. */
/** Immediately following this structure are param_size bytes of calibration
    data. The structure and size depend on the module_id/param_id combination.
*/
struct lsm_session_cmd_get_params_v2_t
{

    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the payload address.

         @values For in-band data, this field must be set to 0 */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be in contiguous memory and aligned to a
         32-byte boundary.

         @values
         - For in-band data, both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

         @values
          - NULL -- In-band
             - The message is in the payload.
             - A series of %lsm_session_param_data_t structures immediately
               follow this structure.
             - The total size is data_payload_size bytes. @vertspace{2}
          - Non-NULL -- Out-of-band
             - The payload begins at the specified physical address.
             - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint32_t                  param_max_size;
    /**< Maximum size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_get_params_v2_t */

/** @ingroup lsmdepr_command
  Deprecated. Use #LSM_SESSION_CMDRSP_GET_PARAMS_V3.

  Returns parameter values in response to an #LSM_SESSION_CMD_GET_PARAMS_V2
  command.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMDRSP_GET_PARAMS_V2
    @par
    Src_port -- Dest_port in corresponding LSM_SESSION_CMD_GET_PARAMS_V2

  @apr_msgpayload{lsm_session_cmdrsp_get_params_v2_t}
    @tablens{weak__lsm__session__cmdrsp__get__params__v2__t}

  @par Parameter data variable payload (lsm_session_param_data_v2_t)
    @tablens{weak__lsm__session__param__data__v2__t}

  @return
    None.

  @dependencies
    None. @newpage
*/
#define LSM_SESSION_CMDRSP_GET_PARAMS_V2                           0x00012A91

/* Payload for LSM_SESSION_CMDRSP_GET_PARAMS_V2. */
typedef struct lsm_session_cmdrsp_get_params_v2_t
               lsm_session_cmdrsp_get_params_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmdrsp_get_params_v2_t
@{ */
/* Payload of the LSM_SESSION_CMDRSP_GET_PARAMS_V2 message, which returns
    parameter values in response to an LSM_SESSION_CMD_GET_PARAMS_V2 command. */
/** For in-band data, immediately following this structure is the acknowledgment
    <b>Parameter data variable payload</b> (%lsm_session_param_data_v2_t).
    The payload size depends on the parameter.
    For out-of-band messages, the actual payload is returned in the allocated
    shared memory. 
*/
struct lsm_session_cmdrsp_get_params_v2_t
{
    uint32_t                  status;
    /**< Status message or error code.

         @values Refer to @xrefcond{Q4,80-NF774-3,80-NA610-3} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmdrsp_get_params_v2_t */

/** @ingroup lsmsess_cmd_get_params3
    Gets a configuration parameter from an LSM module.

    This command supports module instances. It can be called on different
    instances of the same module.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_GET_PARAMS_V3
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID

  @apr_msgpayload{lsm_session_cmd_get_params_v3_t}
    @tablens{weak__lsm__session__cmd__get__params__v3__t}

  @return
    #LSM_SESSION_CMDRSP_GET_PARAMS_V3

  @dependencies
    This command is accepted only after #LSM_SESSION_CMD_OPEN_TX_V2 is
    processed successfully.
*/
#define LSM_SESSION_CMD_GET_PARAMS_V3        0x00012A93

/* Payload format for a  Get Parameter command. */
typedef struct lsm_session_cmd_get_params_v3_t lsm_session_cmd_get_params_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_get_params_v3_t
@{ */
/* Payload of the LSM_SESSION_CMD_GET_PARAMS_V3 command, which allows
   a single parameter to be queried form the module.*/
/** Immediately following this structure are param_size bytes of calibration
    data. The structure and size depend on the module_id/param_id combination.
*/
struct lsm_session_cmd_get_params_v3_t
{

    uint32_t                  data_payload_addr_lsw;
    /**< Lower 32 bits of the payload address.

         @values For in-band data, this field must be set to 0 */

    uint32_t                  data_payload_addr_msw;
    /**< Upper 32 bits of the payload address.

         The 64-bit number formed by data_payload_addr_lsw and
         data_payload_addr_msw must be in contiguous memory and aligned to a
         32-byte boundary.

         @values
         - For in-band data, both data_payload_addr_lsw and
           data_payload_addr_msw must be set to 0.
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 of this field must
           be set to 0. @tablebulletend */

    uint32_t                  mem_map_handle;
    /**< Unique identifier for the shared memory address.

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS.

         @values
          - NULL -- In-band
             - The message is in the payload.
             - A series of %lsm_session_param_data_t structures immediately
               follow this structure.
             - The total size is data_payload_size bytes. @vertspace{2}
          - Non-NULL -- Out-of-band
             - The payload begins at the specified physical address.
             - The total size is data_payload_size bytes. @tablebulletend */

    uint32_t                  module_id;
    /**< Valid ID of the module. */

    uint16_t                  instance_id;
    /**< Valid ID of the module instance. */

    uint16_t                  reserved;
    /**< Used for alignment; must be set to 0. */

    uint32_t                  param_id;
    /**< Valid ID of the parameter. */

    uint32_t                  param_max_size;
    /**< Maximum size of the parameter data based on the module_id/param_id
         combination.

         @values > 0 bytes, in multiples of 4 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_get_params_v3_t */

/** @ingroup lsmsess_rsp_get_params3
    Returns parameter values in response to an #LSM_SESSION_CMD_GET_PARAMS_V3
    command.

    This command supports module instances. It can be called on different
    instances of the same module.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMDRSP_GET_PARAMS_V3
    @par
    Src_port -- Dest_port in corresponding LSM_SESSION_CMD_GET_PARAMS_V3

  @apr_msgpayload{lsm_session_cmdrsp_get_params_v3_t}
    @tablens{weak__lsm__session__cmdrsp__get__params__v3__t}

  @par Parameter data variable payload (lsm_session_param_data_v3_t)
    @tablens{weak__lsm__session__param__data__v3__t}

  @return
    None.

  @dependencies
    None.
*/
#define LSM_SESSION_CMDRSP_GET_PARAMS_V3                           0x00012A94

/* Payload for LSM_SESSION_CMDRSP_GET_PARAMS_V3. */
typedef struct lsm_session_cmdrsp_get_params_v3_t
               lsm_session_cmdrsp_get_params_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmdrsp_get_params_v3_t
@{ */
/* Payload of the LSM_SESSION_CMDRSP_GET_PARAMS_V3 message, which returns
    parameter values in response to an LSM_SESSION_CMD_GET_PARAMS_V3 command. */
/** For in-band data, immediately following this structure is the acknowledgment
    <b>Parameter data variable payload</b> (%lsm_session_param_data_v3_t).
    The payload size depends on the parameter.
    For out-of-band messages, the actual payload is returned in the allocated
    shared memory.
*/
struct lsm_session_cmdrsp_get_params_v3_t
{
    uint32_t                  status;
    /**< Status message or error code.

         @values Refer to @xrefcond{Q4,80-NF774-3,80-NA610-3} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmdrsp_get_params_v3_t */


/** @} */ /* end_addtogroup lsmmodule_common */

/** @ingroup lsmdepr_command
    Deprecated. Use #LSM_SESSION_CMD_SET_PARAMS_V3 and
    LSM_PARAM_ID_REGISTER_SOUND_MODEL.

    Registers sound model data with the Detection Engine module in the LSM
    session.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_REGISTER_SOUND_MODEL
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID (0 is reserved)

  @apr_msgpayload{lsm_session_cmd_register_sound_model_t}
    @table{weak__lsm__session__cmd__register__sound__model__t}

  @detdesc
    This command is accepted only when the LSM session is in the Init state.
    @par
    When creating a session, the client processor must specify the session IDs.
    Currently, up to #LSM_MAX_SESSION_ID groups are supported.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    This command can be sent only after #LSM_SESSION_CMD_OPEN_TX_V2 is
    processed successfully.
    @par
    Before sending this command, sound model memory must be mapped using
    #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS. @newpage
*/
#define LSM_SESSION_CMD_REGISTER_SOUND_MODEL              0x00012A84

/* Payload format for an LSM register sound model command. */
typedef struct lsm_session_cmd_register_sound_model_t
               lsm_session_cmd_register_sound_model_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_cmd_register_sound_model_t
@{ */
/* Payload of LSM_SESSION_CMD_REGISTER_SOUND_MODEL command
*/
struct lsm_session_cmd_register_sound_model_t
{
    uint32_t model_size;
    /**< Size of the registering sound model.

         @values > 0 bytes */

    uint32_t model_addr_lsw;
    /**< Lower 32 bits of the 64-bit parameter data payload address. */

    uint32_t model_addr_msw;
    /**< Upper 32 bits of the 64-bit parameter data payload address.

         The 64-bit number formed by model_addr_lsw and model_addr_msw word
         must be in contiguous memory and aligned to a 32-byte boundary.

         @values
         - For a 32-bit shared memory address, this field must be set to 0.
         - For a 36-bit shared memory address, bits 31 to 4 must be set to 0.
         - For a 64-bit shared memory address, any 32-bit value.
         @tablebulletend */

    uint32_t mem_map_handle; 
    /**< Unique identifier for the shared memory address.

         @values Any 32-bit value

         The aDSP returns this memory map handle in
         #LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_cmd_register_sound_model_t */

/** @ingroup lsmdepr_command
    Deprecated. Use #LSM_SESSION_CMD_SET_PARAMS_V3 and
    LSM_PARAM_ID_DEREGISTER_SOUND_MODEL.

    Deregisters sound model data from the LSM session.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_DEREGISTER_SOUND_MODEL
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID (0 is reserved)

  @apr_msgpayload
    None.

  @detdesc
    This command is accepted only when the LSM session is in the Init state.
    If the session is in the Active state, it must be switched to the Init
    state before sending this deregister command.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    The session ID must be for a valid opened session.
    @par
    The sound model must have been registered with the session.
*/
#define LSM_SESSION_CMD_DEREGISTER_SOUND_MODEL            0x00012A85

/** @ingroup lsmsess_cmd_start
    Starts a specified LSM session.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_START
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID (0 is reserved)

  @apr_msgpayload
    None.

  @detdesc
    This command is accepted only when the session is in the Init state.
    After processing this command successfully, the session transitions to the
    Active state.
    @par
    The LSM session processes data only after receiving this command.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    The session ID must be for a valid opened session.
*/
#define LSM_SESSION_CMD_START                             0x00012A86

/** @ingroup lsmsess_cmd_stop
    Stops a specified LSM session.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_STOP
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID (0 is reserved)

  @apr_msgpayload
    None.

  @detdesc
    The session sends this command to the detection engine thread and disables
    data processing.
    After processing this command, the session transitions from the Stop state
    to the Init state.
    @par
    This command is helpful if the application processor stops detection engine
    processing due to concurrency restrictions. To resume detection engine
    processing, the client processor resends #LSM_SESSION_CMD_START.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    The session ID must be for a valid opened session.
*/
#define LSM_SESSION_CMD_STOP                              0x00012A87

/** @ingroup lsmsess_cmd_close
    Stops and closes a specified LSM session, tearing down all dynamic threads
    that were created in #LSM_SESSION_CMD_OPEN_TX_V2.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_CLOSE
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID (0 is reserved)

  @apr_msgpayload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    The session ID must be for a valid opened LSM session.
*/
#define LSM_SESSION_CMD_CLOSE                             0x00012A88

/** @ingroup lsmsess_cmd_eob
    Stops look ahead buffering (LAB) in a specified LSM session. Any data
    buffers held by the aDSP are returned to the client.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_CMD_EOB
    @par
    Dst_port -- 1 to #LSM_MAX_SESSION_ID (0 is reserved)

  @apr_msgpayload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,80-N1463-2}).

  @dependencies
    The session ID must be a valid active session.
*/
#define LSM_SESSION_CMD_EOB                               0x00012A89

/** @addtogroup lsmsess_datatypes
@{ */
/** Status (Detected) when the Detection Engine module detects a keyword or
    user.
 */
#define LSM_VOICE_WAKEUP_STATUS_DETECTED                  0x2

/** Status (Rejected) when the Detection Engine module rejects a keyword or
    user.
 */
#define LSM_VOICE_WAKEUP_STATUS_REJECTED                  0x4

/** @} */ /* end_addtogroup lsmsess_datatypes */

/* Payload format for an LSM session event detection. */
typedef struct lsm_session_event_detection_status_t
               lsm_session_event_detection_status_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_event_detection_status_t
@{ */
/* payload of LSM_SESSION_EVENT_DETECTION_STATUS command. */
/** Following this structure is a parameter data payload of size payload_size.
*/
struct lsm_session_event_detection_status_t
{
    uint16_t  detection_status;
    /**< Status sent by the Detection Engine module to the client.

         @values
         - #LSM_VOICE_WAKEUP_STATUS_DETECTED
         - #LSM_VOICE_WAKEUP_STATUS_REJECTED @tablebulletend */

    uint16_t  payload_size;
    /**< Actual size of the parameter data payload that follows this structure.

         @values > 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_event_detection_status_t */


/** @ingroup lsmsess_evt_detect_status2
    Event sent by the aDSP to the HLOS processor when a registered keyword
    is detected.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_EVENT_DETECTION_STATUS_V2

  @apr_msgpayload{lsm_session_event_detection_status_v2_t}
    @tablens{weak__lsm__session__event__detection__status__v2__t}

  @detdesc
    When the framework mode is not set or is set to LSM_NON_TIME_STAMP_MODE,
    the LSM sends this event to the client processor after processing the data.
    The aDSP sends the detection_status in the APR payload of this event.

  @return
    None.

  @dependencies
    None.
*/
#define LSM_SESSION_EVENT_DETECTION_STATUS_V2             0x00012B01

/* Payload format for an LSM session event detection. */
typedef struct lsm_session_event_detection_status_v2_t
               lsm_session_event_detection_status_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_event_detection_status_v2_t
@{ */
/* payload of LSM_SESSION_EVENT_DETECTION_STATUS_V2 command. */
/** Immediately following this structure is the parameter data payload of size
    payload_size.
*/
struct lsm_session_event_detection_status_v2_t
{
    uint8_t  detection_status;
    /**< Status sent by the Detection Engine module to the client.

         @values
         - #LSM_VOICE_WAKEUP_STATUS_DETECTED
         - #LSM_VOICE_WAKEUP_STATUS_REJECTED @tablebulletend */

    uint8_t  payload_size;
    /**< Actual size of the parameter data payload that follows this structure.

         @values > 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_event_detection_status_v2_t */


/** @ingroup lsmsess_evt_detect_status3
    Event sent by the aDSP to the HLOS processor when a registered keyword
    is detected.

    This command supports a timestamp mode.

  @apr_hdr_fields
    Opcode -- LSM_SESSION_EVENT_DETECTION_STATUS_V3

  @apr_msgpayload{lsm_session_event_detection_status_v3_t}
    @tablens{weak__lsm__session__event__detection__status__v3__t}

  @detdesc
    When the framework mode is set to LSM_TIME_STAMP_MODE, the LSM sends this
    event to the client processor after processing the data.
    The aDSP sends the detection_status in the APR payload of this event.

  @return
    None.

  @dependencies
    None.
*/
#define LSM_SESSION_EVENT_DETECTION_STATUS_V3             0x00012B04

/* Payload format for an LSM session event detection. */
typedef struct lsm_session_event_detection_status_v3_t
               lsm_session_event_detection_status_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_lsm_session_event_detection_status_v3_t
@{ */
/* payload of LSM_SESSION_EVENT_DETECTION_STATUS_V3 command. */
/** Immediately following this structure is the parameter data payload of size
    payload_size.
 */
struct lsm_session_event_detection_status_v3_t
{
  uint32_t timestamp_lsw;
  /**< Lower 32 bits of the 64-bit timestamp at the time of detection. */

  uint32_t timestamp_msw;
  /**< Upper 32 bits of the 64-bit timestamp at the time of detection. */

  uint8_t  detection_status;
  /**< Status sent by the Detection Engine module to the client.

       @values
       - #LSM_VOICE_WAKEUP_STATUS_DETECTED
       - #LSM_VOICE_WAKEUP_STATUS_REJECTED @tablebulletend */

  uint8_t  payload_size;
  /**< Actual size of the parameter data payload that follows this structure.

       @values > 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_lsm_session_event_detection_status_v3_t */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_LSM_SESSION_COMMANDS_H_ */

#ifndef QSEE_INTERFACE_ARMV8_H
#define QSEE_INTERFACE_ARMV8_H

/**
   @file qsee_interface_armv8.h

   @brief QSEE Interface Header File.

   This header file contains definitions of data structures used by
   non-secure side to send request to QSEE Library.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The TZ_API_mainpage.dox contains all descriptions that are
      displayed in the output PDF generated using Doxygen and LaTeX. To edit
      or update any of the file/group text in the PDF, edit the
      TZ_API_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
   Copyright (c) 2012-2017 by Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
09/26/14   sm       Changed to UEFI types
05/08/13   leo      Edited Doxygen comments and markup for Family B updates.
03/08/12   leo      (Tech Pubs) Edited/added Doxygen comments and markup.
03/05/12   sa       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "tz_interface_armv8.h"

 /**
 *
 */
#define TZ_OS_APP_START_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x01)

#define TZ_OS_APP_START_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_APP_SHUTDOWN_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x02)

#define TZ_OS_APP_SHUTDOWN_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_APP_LOOKUP_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x03)

#define TZ_OS_APP_LOOKUP_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_APP_GET_STATE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x04)

#define TZ_OS_APP_GET_STATE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_APP_REGION_NOTIFICATION_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x05)

#define TZ_OS_APP_REGION_NOTIFICATION_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_REGISTER_LOG_BUFFER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x06)

#define TZ_OS_REGISTER_LOG_BUFFER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_BUF_RW, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_LOAD_SERVICES_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x07)

#define TZ_OS_LOAD_SERVICES_IMAGE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_UNLOAD_SERVICES_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_APP_MGR, 0x08)

#define TZ_OS_UNLOAD_SERVICES_IMAGE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
 *
 */
#define TZ_OS_REGISTER_LISTENER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x01)

#define TZ_OS_REGISTER_LISTENER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_DEREGISTER_LISTENER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x02)

#define TZ_OS_DEREGISTER_LISTENER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_LISTENER_RESPONSE_HANDLER_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_LISTENER, 0x03)

#define TZ_OS_LISTENER_RESPONSE_HANDLER_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_2( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_LOAD_EXTERNAL_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_EXTERNAL, 0x01)

#define TZ_OS_LOAD_EXTERNAL_IMAGE_ID_PARAM_ID \
  TZ_CREATE_PARAM_ID_3( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_VAL, \
      TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_UNLOAD_EXTERNAL_IMAGE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_EXTERNAL, 0x02)

#define TZ_OS_UNLOAD_EXTERNAL_IMAGE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
 *
 */
#define TZ_OS_RPMB_PROVISION_KEY_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_RPMB, 0x01)

#define TZ_OS_RPMB_PROVISION_KEY_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_1( TZ_SYSCALL_PARAM_TYPE_VAL )

/**
 *
 */
#define TZ_OS_RPMB_ERASE_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_RPMB, 0x02)

#define TZ_OS_RPMB_ERASE_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0

/**
 *
 */
#define TZ_OS_RPMB_CHECK_PROV_STATUS_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_QSEE_OS, TZ_SVC_RPMB, 0x03)
#define TZ_OS_RPMB_CHECK_PROV_STATUS_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_0
/**
 *
 */  
#define TZ_APP_QSAPP_SEND_DATA_ID \
  TZ_SYSCALL_CREATE_SMC_ID(TZ_OWNER_TZ_APPS, TZ_SVC_APP_ID_PLACEHOLDER, 0x01)

#define TZ_APP_QSAPP_SEND_DATA_ID_PARAM_ID \
  TZ_SYSCALL_CREATE_PARAM_ID_5( \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL, TZ_SYSCALL_PARAM_TYPE_BUF_RW, \
      TZ_SYSCALL_PARAM_TYPE_VAL )


#define QSEE_APP_START_COMMAND           0x1
#define QSEE_APP_SHUTDOWN_COMMAND        0x2
#define QSEE_APP_LOOKUP_COMMAND          0x3
#define QSEE_REGISTER_LISTENER           0x4
#define QSEE_DEREGISTER_LISTENER         0x5
#define QSEE_CLIENT_SEND_DATA_COMMAND    0x6
#define QSEE_LISTENER_DATA_RSP_COMMAND   0x7
#define QSEE_LOAD_EXTERNAL_ELF_COMMAND   0x8
#define QSEE_UNLOAD_EXTERNAL_ELF_COMMAND 0x9
#define QSEE_GET_APP_STATE_COMMAND       0xA
#define QSEE_LOAD_SERV_IMAGE_CMD         0xB
#define QSEE_UNLOAD_SERV_IMAGE_CMD       0xC
#define QSEE_REGION_NOTIFICATION_CMD     0xD
#define QSEE_REGISTER_LOG_BUF_COMMAND    0xE
#define QSEE_PROVISION_RPMB_KEY_COMMAND  0xF
#define QSEE_RPMB_ERASE_COMMAND          0x10 

/** @addtogroup qsee_datatypes
@{ */

/**
   QSEE application layer response type.
*/
typedef enum
{
  QSEE_APP_ID      = 0xEE01, /**< QSEE response type when the response
                                  data is the application ID. */
  QSEE_LISTENER_ID = 0xEE02, /**< QSEE response type when the response
                                  data is the listener ID. */
  QSEE_APP_STATE   = 0xEE03, /**< QSEE response type when the response
                                  data is the Application state. */
  QSEE_RESERVED    = MAX_INT32 /**< Required to make the enum four bytes. */
} qsee_command_resp_t;

/** @} */ /* end_addtogroup_qsee_datatypes */

/**
   @ingroup qsee_macros_constants

   Size of the Magic marker in the standard ELF header.
*/
#define ELFINFO_MAGIC_SIZE (16)

/** @addtogroup qsee_macros_constants
@{ */

/*  QSEE Response Result Types */

/** Syscall completed successfully. */
#define TZOS_RESULT_SUCCESS        0
/** Syscall is incomplete since the application requested service. */
#define TZOS_RESULT_INCOMPLETE     1
/** Syscall is terminated due to an error. */
#define TZOS_RESULT_FAILURE        0xFFFFFFFF
/** Maximum character string for applications */
#define QSEE_MAX_NAME_SIZE         32

/** @} */ /* end_addtogroup qsee_macros_constants */

/** @addtogroup qsee_datatypes
@{ */
#ifdef _MSC_VER
#define __attribute__(x)
#pragma pack(push, 1)
#endif

/* The first 52 bytes of the file is the ELF header  */
/**
   ELF header structure.
*/
typedef struct
{
  unsigned char e_ident[ELFINFO_MAGIC_SIZE]; /**< Magic number and other
                                                  information. */
  UINT16        e_type;                      /**< Object file type. */
  UINT16        e_machine;                   /**< Architecture. */
  UINT32        e_version;                   /**< Object file version. */
  UINT32        e_entry;                     /**< Entry point virtual
                                                  address. */
  UINT32        e_phoff;                     /**< Program header table
                                                  file offset. */
  UINT32        e_shoff;                     /**< Section header table
                                                  file offset.    */
  UINT32        e_flags;                     /**< Processor-specific
                                                  flags. */
  UINT16        e_ehsize;                    /**< ELF header size in
                                                  bytes. */
  UINT16        e_phentsize;                 /**< Program header table
                                                  entry size. */
  UINT16        e_phnum;                     /**< Program header table
                                                  entry count. */
  UINT16        e_shentsize;                 /**< Section header table
                                                  entry size. */
  UINT16        e_shnum;                     /**< Section header table
                                                  entry count. */
  UINT16        e_shstrndx;                  /**< Section header string
                                                  table index. */
} __attribute__ ((packed)) Elf32_Ehdr;

/**
   Program segment header structure.
*/
typedef struct
{
  UINT32 p_type;                   /**< Segment type. */
  UINT32 p_offset;                 /**< Segment file offset. */
  UINT32 p_vaddr;                  /**< Segment virtual address. */
  UINT32 p_paddr;                  /**< Segment physical address. */
  UINT32 p_filesz;                 /**< Segment size in file. */
  UINT32 p_memsz;                  /**< Segment size in memory. */
  UINT32 p_flags;                  /**< Segment flags. */
  UINT32 p_align;                  /**< Segment alignment.
                                        @newpagetable */
} __attribute__ ((packed)) Elf32_Phdr;

/** @} */ /* end_addtogroup_qsee_datatypes */

/*
   The Format of Image of the elf image that pa should point to is:
   Elf Header
   Prog header 1
   Prog header 2
   ...
   ...
   Prog header n
   Segment 1
   Segment 2
   ...
   ...
   Segment n
*/

/** @addtogroup qsee_datatypes
@{ */

/**
   Response structure for all QSEE application layer commands.

   The data field has meaning depending on the result field, which
   can be SUCCESS, FAILURE, or INCOMPLETE. The resp_type field can
   be APP_ID, LISTENER_ID, or APP_STATE. The data field will be the 
   application ID or the listener ID or the application's state 
   depending on the resp_type. When the result is INCOMPLETE, 
   it indicates that the secure application has requested service 
   from the listener indicated by the listener ID in the data field.
*/
typedef struct qsee_command_resp_info_s
{
  UINT64                 status;
  UINT64                 result;    /**< Result of the QSEE command. */
  UINT64                 resp_type; /**< Data ID type. */
  UINT64                 data;      /**< Response data. */
} __attribute__ ((packed)) qsee_command_resp_info_t;

/** @} */ /* end_addtogroup qsee_datatypes */

/*
   Top level TZ syscall request structure for
   QSEE_APP_START_COMMAND. */

/** @weakgroup weak_tz_app_start_s
@{ */
typedef struct tzos_app_start_s
{
  UINT64 mdt_len;                    /**< Combined length of the ELF
                                          and all program headers. */
  UINT64 img_len;                    /**< Combined length of all the
                                          loadable segments. */
  UINT64 pa;                         /**< PA points to the region
                                          where the application and
                                          the headers are located as
                                          described in Elf32_Ehdr and
                                          Elf32_Phdr. */
} __attribute__ ((packed)) tzos_app_start_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_APP_SHUTDOWN_COMMAND. */

/** @weakgroup weak_tzos_app_shutdown_s
@{ */
typedef struct tzos_app_shutdown_s
{
  UINT64                     app_id;        // ID of the application to be shutdown
} __attribute__ ((packed)) tzos_app_shutdown_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_APP_LOOKUP_COMMAND. */

/** @weakgroup weak_tzos_app_lookup_s
@{ */
typedef struct tzos_app_lookup_s
{
  UINT64                     app_name;      // Pointer to application name.                                      
  UINT64                     name_len;      // Length of application name.
} __attribute__ ((packed)) tzos_app_lookup_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_GET_APP_STATE_COMMAND. */

/** @weakgroup weak_tzos_app_get_state_s
@{ */
typedef struct tzos_app_get_state_s
{
  UINT64                     app_id;         // ID of the application being queried.
} __attribute__ ((packed)) tzos_app_get_state_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_CLIENT_SEND_DATA_COMMAND. 

   The request and response pointers must point to locations outside the
   top-level syscall request structure. A response must immediately follow
   a request to form a physically contiguous region of request followed by
   response. The nonsecure region containing the request and response
   are xPU-protected during the time of this syscall, hence the
   nonsecure side must be careful not to place any other data in
   the 4K regions containing the request and response. */

/** @weakgroup weak_tzapp_qsapp_send_data_s
@{ */
typedef struct tzapp_qsapp_send_data_s
{  
  UINT64                     app_id;         // ID of the application to which the request is routed.
  UINT64                     req_ptr;        // Pointer to the application-specific request structure.
  UINT64                     req_len;        // Length of the application-specific request structure.
  UINT64                     rsp_ptr;        // Pointer to the application-specific response structure.
  UINT64                     rsp_len;        // Length of the application-specific response structure.
} __attribute__ ((packed)) tzapp_qsapp_send_data_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_REGION_NOTIFICATION_CMD. */

/** @weakgroup weak_tzos_app_region_notification_s
@{ */
typedef struct tzos_app_region_notification_s
{  
  UINT64                     app_rgn_addr;
  UINT64                     app_rgn_size; 
} __attribute__ ((packed)) tzos_app_region_type_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_LOG_BUFFER_CMD. */

/** @weakgroup weak_tzos_log_buffer_notification_s
@{ */
typedef struct tzos_log_buffer_notification_s
{  
  UINT64                     log_addr;
  UINT64                     log_size;
} __attribute__ ((packed)) tzos_log_buffer_type_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_REGISTER_LISTENER. */

/** @weakgroup weak_tzos_register_listener_s
@{ */
typedef struct tzos_register_listener_s
{
  
  UINT64                     listener_id;    // ID of the listener service.
  UINT64                     req_ptr;        // Pointer to the shared buffer.
  UINT64                     req_len;        // Length of the shared buffer.
} __attribute__ ((packed)) tzos_register_listener_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_DEREGISTER_LISTENER. */

/** @weakgroup weak_tzos_deregister_listener_s
@{ */
typedef struct tzos_deregister_listener_s
{
  UINT64                     listener_id;    // Listener service ID
} __attribute__ ((packed)) tzos_deregister_listener_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_LISTENER_DATA_RSP_COMMAND. */

/** @weakgroup weak_tzos_listener_response_handler_s
@{ */
typedef struct tzos_listener_response_handler_s
{  
  UINT64                     listener_id;    // Listener service ID.
  UINT64                     status;         // Status of the Listener service
} __attribute__ ((packed)) tzos_listener_response_handler_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_LOAD_SERV_IMAGE_CMD. */

/** @weakgroup weak_tzos_load_services_image_s
@{ */
typedef struct tzos_load_services_image_s
{
  UINT64                     mdt_len;
  UINT64                     img_len;
  UINT64                     pa;
} __attribute__ ((packed)) tzos_load_services_image_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_REGISTER_LOG_BUF_COMMAND. */

/** @weakgroup weak_tzos_register_log_buffer_s
@{ */
typedef struct tzos_register_log_buffer_s
{
  UINT64                     phy_addr;
  UINT64                     len;
} __attribute__ ((packed)) tzos_register_log_buffer_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_PROVISION_RPMB_KEY_COMMAND. */

/** @weakgroup weak_tzos_rpmb_provision_key_s
@{ */
typedef struct tzos_rpmb_provision_key_s
{
  UINT64                     key_type;
} __attribute__ ((packed)) tzos_rpmb_provision_key_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_LOAD_EXTERNAL_ELF_COMMAND. */

/** @weakgroup weak_tzos_load_external_image_s
@{ */
typedef struct tzos_load_external_image_s
{
  UINT64                     mdt_len;
  UINT64                     img_len;
  UINT64                     pa;
} __attribute__ ((packed)) tzos_load_external_image_t;

/** @} */ /* end_weakgroup */

#ifdef _MSC_VER
#pragma pack(pop)
#endif
#endif /* QSEE_INTERFACE_ARMV8_H */

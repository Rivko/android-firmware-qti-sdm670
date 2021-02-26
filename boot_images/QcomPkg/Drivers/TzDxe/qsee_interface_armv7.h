#ifndef QSEE_INTERFACE_ARMV7_H
#define QSEE_INTERFACE_ARMV7_H

/**
   @file qsee_interface_armv7.h

   @brief QSEE Interface Header File.

   This header file contains definitions of data structures used by
   non-secure side to send request to QSEE Library.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.

      The TZ_SysCall_API_mainpage.dox contains all descriptions that are
      displayed in the output PDF generated using Doxygen and LaTeX. To edit
      or update any of the file/group text in the PDF, edit the
      TZ_SysCall_API_mainpage.dox file or contact Tech Pubs.
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
#include "tz_interface_armv7.h"


/**
   @ingroup app_start_cmd

   Loads/starts a secure application.

   @sys_call_params{tz_app_start_syscall_s}
     @table{weak__tz__app__start__syscall__s}

   @return
     Return value is indicated by the result field of #qsee_command_resp_info_s.
*/
#define QSEE_APP_START_COMMAND           0x1

/**
   @ingroup app_shutdown_cmd

   Unloads/shuts down a secure application.

   @sys_call_params{tz_app_shutdown_syscall_s}
     @table{weak__tz__app__shutdown__syscall__s}

   @return
     Return value is indicated by the result field of #qsee_command_resp_info_s.
*/
#define QSEE_APP_SHUTDOWN_COMMAND        0x2

/**
   @ingroup app_lookup_cmd

   Looks up an application ID based on the application name.

   @sys_call_params{tz_app_lookup_syscall_s}
     @table{weak__tz__app__lookup__syscall__s}

   @return
     Return value is indicated by the result field of #qsee_command_resp_info_s.
*/
#define QSEE_APP_LOOKUP_COMMAND          0x3

/**
   @ingroup reg_listener

   Registers a nonsecure listener service.

   The shared buffer pointer must point to locations outside the
   top-level syscall request structure. This must point to the
   nonsecure shared buffer used by secure applications and nonsecure
   listeners to share the request and response data.

   @sys_call_params{tz_app_register_listener_syscall_s}
     @table{weak__tz__app__register__listener__syscall__s}

   @return
     Return value is indicated by the result field of #qsee_command_resp_info_s.
*/
#define QSEE_REGISTER_LISTENER           0x4

/**
   @ingroup dereg_listener

   Deregisters a nonsecure listener service.

   @sys_call_params{tz_app_deregister_listener_syscall_s}
     @table{weak__tz__app__deregister__listener__syscall__s}

   @return
     Return value is indicated by the result field of #qsee_command_resp_info_s.
*/
#define QSEE_DEREGISTER_LISTENER         0x5

/**
   @ingroup client_send_data

   Sends an application-specific command to a secure application.

   The request and response pointers must point to locations outside the
   top-level syscall request structure. A response must immediately follow
   a request to form a physically contiguous region of request followed by
   response. The nonsecure region containing the request and response
   are xPU-protected during the time of this syscall, hence the
   nonsecure side must be careful not to place any other data in
   the 4K regions containing the request and response.

   @sys_call_params{tz_app_send_data_syscall_s}
     @table{weak__tz__app__send__data__syscall__s}

   @return
     Return value is indicated by the result field of #qsee_command_resp_info_s.
*/
#define QSEE_CLIENT_SEND_DATA_COMMAND    0x6

/**
   @ingroup listener_data_rsp

   Sends a response from a listener to a secure application.

   The listener ID indicates the listener service that responded
   to the service requested by the secure application.

   @sys_call_params{tz_app_listener_response_syscall_s}
     @table{weak__tz__app__listener__response__syscall__s}

   @return
     Return value is indicated by the result field of #qsee_command_resp_info_s.
*/
#define QSEE_LISTENER_DATA_RSP_COMMAND   0x7

#define QSEE_LOAD_EXTERNAL_ELF_COMMAND   0x8

#define QSEE_UNLOAD_EXTERNAL_ELF_COMMAND   0x9

/**
   @ingroup get_app_state

   Queries for the state of an application: Suspended or Active. 

   @sys_call_params{tz_app_get_state_syscall_s}
     @table{weak__tz__app__get__state__syscall__s}

   @return
     Return value is indicated by the result field of #qsee_command_resp_info_s.
*/
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

#define QSEE_LOG_BUFFER_MAX  (1024*32) // 32k
#ifdef _MSC_VER
#define __attribute__(x)
#pragma pack(push, 1)
#endif
/**
* Log ring buffer position management
*/
typedef struct qsee_log_pos_s {
	UINT16 wrap;        // # of times log buffer has wrapped around
	UINT16 offset;      // next offset in buffer
} __attribute__ ((packed)) qsee_log_pos_t;

typedef struct qsee_get_app_state_type {
	UINT32  app_id;
} __attribute__ ((packed)) qsee_get_app_state_t;

/* register log buffer */
typedef struct qsee_reg_log_buf_req_type {
	UINT32   phy_addr;    /* physical address of log buffer area */
	UINT32   len;         /* Length of buffer */
} __attribute__ ((packed)) qsee_reg_log_buf_req_t;
typedef struct qsee_load_serv_image_type
{
	UINT32  mdt_len;
	UINT32  img_len;
	UINT32  pa;
} __attribute__ ((packed)) qsee_load_serv_image_t;

/** @addtogroup qsee_datatypes
@{ */

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

/**
   Program segment header structure.
*/
typedef struct qsee_app_img_info_s
{
  UINT32 mdt_len;                    /**< Combined length of the ELF
                                          and all program headers. */
  UINT32 img_len;                    /**< Combined length of all the
                                          loadable segments. */
  UINT32 pa;                         /**< PA points to the region
                                          where the application and
                                          the headers are located as
                                          described in Elf32_Ehdr and
                                          Elf32_Phdr. */
  char   app_name[QSEE_MAX_NAME_SIZE]; 
                                     /**< Name of the application. */
} __attribute__ ((packed)) qsee_app_img_info_t;

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

/*
   QSEE request structure for the command QSEE_CLIENT_SEND_DATA_COMMAND.

   The request and response pointers must point to locations outside the
   top-level syscall request structure. A response must immediately follow
   a request to form a physically contiguous region of request followed by
   response. The nonsecure region containing the request and response
   are xPU-protected during the time of this syscall, hence the
   nonsecure side must be careful not to place any other data in
   the 4K regions containing the request and response. */

/** @weakgroup weak_qsee_client_send_data_req_s
@{ */
typedef struct qsee_client_send_data_req_s
{
  UINT32 app_id;     /**< ID of the application to which the request
                          is routed. */
  UINT32 req_ptr;    /**< Pointer to the application-specific request
                          structure. */
  UINT32 req_len;    /**< Length of the application-specific request
                          structure. */
  UINT32 rsp_ptr;    /**< Pointer to the application-specific response
                          structure. */
  UINT32 rsp_len;    /**< Length of the application-specific response
                          structure. */
} __attribute__ ((packed)) qsee_client_send_data_req_t;

/** @} */ /* end_weakgroup */

/*
   QSEE request structure for the command QSEE_REGISTER_LISTENER.

   The shared buffer pointer must point to locations outside the
   top-level syscall request structure. This must point to the
   nonsecure shared buffer used by secure applications and nonsecure
   listeners to share the request and response data. */

/** @weakgroup weak_qsee_register_listener_s
@{ */
typedef struct qsee_register_listener_s
{
  UINT32 listener_id;  /**< ID of the listener service. */
  UINT32 sb_ptr;       /**< Pointer to the shared buffer. */
  UINT32 sb_len;       /**< Length of the shared buffer. */
} __attribute__ ((packed)) qsee_register_listener_t;

/** @} */ /* end_weakgroup */

/*
   QSEE request structure for the command QSEE_DEREGISTER_LISTENER.

   The listener ID indicates the listener service that will be deregistered from QSEE. */

/** @weakgroup weak_qsee_deregister_listener_s
@{ */
typedef struct qsee_deregister_listener_s 
{
  UINT32 listener_id;          /**< Listener service ID. */
} __attribute__ ((packed)) qsee_deregister_listener_t;

/** @} */ /* end_weakgroup */

/*
   QSEE request structure for the command QSEE_LISTENER_DATA_RSP_COMMAND.

   The listener ID indicates the listener service that responded
   to the service requested by the secure application. */

/** @weakgroup weak_qsee_listener_rsp_s
@{ */
typedef struct qsee_listener_rsp_s
{
  UINT32  listener_id;  /**< Listener service ID. */
  UINT32  status;       /**Status of the Listener service**/
} __attribute__ ((packed)) qsee_listener_rsp_t;


/** @} */ /* end_weakgroup */

// Structures for App region notification command.
// This will let TZ know where to load all the apps
typedef struct qsee_apps_region_notificaton_s
{
  UINT32  applications_region_addr;
  UINT32  applications_region_size; 
} __attribute__ ((packed)) qsee_apps_region_notification_t;


// Structures for provisioning RPMB command.
typedef struct qsee_provision_rpmb_key_req_type{
  UINT32 key_type;
} __attribute__ ((packed)) qsee_provision_rpmb_key_req_t;

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
  UINT32                 result;    /**< Result of the QSEE command. */
  qsee_command_resp_t    resp_type; /**< Data ID type. */
  UINT32                 data;      /**< Response data. */
} __attribute__ ((packed)) qsee_command_resp_info_t;

/** @} */ /* end_addtogroup qsee_datatypes */

/*
   Top level TZ syscall request structure for
   QSEE_APP_START_COMMAND. */

/** @weakgroup weak_tz_app_start_syscall_s
@{ */
typedef struct tz_app_start_syscall_s
{
  struct tz_syscall_req_s    common_req;  /**< QSEE blocking kernel layer
                                               common request structure. */
  UINT32                     cmd_id;      /**< Command ID. */
  qsee_app_img_info_t        req;         /**< Application start-specific
                                               request. */
  UINT32                     req_len;     /**< QSEE OS command-specific
                                               request length. */
  struct tz_syscall_rsp_s    common_rsp;  /**< QSEE blocking kernel layer
                                               common response structure. */
  qsee_command_resp_info_t   rsp;         /**< QSEE application layer
                                               response. */

} __attribute__ ((packed)) tz_app_start_syscall_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_APP_SHUTDOWN_COMMAND. */

/** @weakgroup weak_tz_app_shutdown_syscall_s
@{ */
typedef struct tz_app_shutdown_syscall_s
{
  struct tz_syscall_req_s    common_req;  /**< QSEE blocking kernel layer
                                               common request structure. */
  UINT32                     cmd_id;      /**< Command ID. */
  UINT32                     app_id;      /**< Application shutdown-specific
                                               request. */
  UINT32                     req_len;     /**< QSEE OS command-specific
                                               request length. */
  struct tz_syscall_rsp_s    common_rsp;  /**< QSEE blocking kernel layer
                                               common response structure. */
  qsee_command_resp_info_t   rsp;         /**< QSEE enhanced kernel layer
                                               response. */

} __attribute__ ((packed)) tz_app_shutdown_syscall_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_APP_LOOKUP_COMMAND. */

/** @weakgroup weak_tz_app_lookup_syscall_s
@{ */
typedef struct tz_app_lookup_syscall_s
{
  struct tz_syscall_req_s    common_req;   /**< QSEE blocking kernel layer
                                                common request structure. */
  UINT32                     cmd_id;       /**< Command ID. */
  char                       app_name[QSEE_MAX_NAME_SIZE];
                                           /**< Application
                                                lookup-specific
                                                request. */
  UINT32                     req_len;      /**< QSEE OS command-specific
                                                request length. */
  struct tz_syscall_rsp_s    common_rsp;   /**< QSEE blocking kernel layer
                                                common response structure. */
  qsee_command_resp_info_t   rsp;          /**< QSEE enhanced kernel layer
                                                response. */

} __attribute__ ((packed)) tz_app_lookup_syscall_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_GET_APP_STATE_COMMAND. */

/** @weakgroup weak_tz_app_get_state_syscall_s
@{ */
typedef struct tz_app_get_state_syscall_s
{
  struct tz_syscall_req_s     common_req;  /**< QSEE blocking kernel layer common request structure. */
  UINT32                      cmd_id;      /**< Command ID. */
  qsee_get_app_state_t        req;         /**< Get state specific request. */
  UINT32                      req_len;     /**< QSEE OS command-specific request length. */
  struct tz_syscall_rsp_s     common_rsp;  /**< QSEE blocking kernel layer common response structure. */
  qsee_command_resp_info_t    rsp;         /**< QSEE enhanced kernel layer response. */
} __attribute__ ((packed)) tz_app_get_state_syscall_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_REGISTER_LISTENER. */

/** @weakgroup weak_tz_app_register_listener_syscall_s
@{ */
typedef struct tz_app_register_listener_syscall_s
{
  struct tz_syscall_req_s    common_req;  /**< QSEE blocking kernel layer common request structure. */
  UINT32                     cmd_id;      /**< Command ID. */
  qsee_register_listener_t   req;         /**< Register listener-specific request. */
  UINT32                     req_len;     /**< QSEE OS command-specific request length. */
  struct tz_syscall_rsp_s    common_rsp;  /**< QSEE blocking kernel layer common response structure. */
  qsee_command_resp_info_t   rsp;         /**< QSEE enhanced kernel layer response. */
} __attribute__ ((packed)) tz_app_register_listener_syscall_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_DEREGISTER_LISTENER. */

/** @weakgroup weak_tz_app_deregister_listener_syscall_s
@{ */
typedef struct tz_app_deregister_listener_syscall_s
{
  struct tz_syscall_req_s    common_req;  /**< QSEE blocking kernel layer common request structure. */
  UINT32                     cmd_id;      /**< Command ID. */
  struct qsee_deregister_listener_s req;  /**< Deregister a listener-specific request. */
  UINT32                     req_len;     /**< QSEE OS command-specific request length. */
  struct tz_syscall_rsp_s    common_rsp;  /**< QSEE blocking kernel layer common response structure. */
  qsee_command_resp_info_t   rsp;         /**< QSEE enhanced kernel layer response. */
} __attribute__ ((packed)) tz_app_deregister_listener_syscall_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_CLIENT_SEND_DATA_COMMAND. */

/** @weakgroup weak_tz_app_send_data_syscall_s
@{ */
typedef struct tz_app_send_data_syscall_s
{
  struct tz_syscall_req_s      common_req;  /**< QSEE blocking kernel layer common request structure. */
  UINT32                       cmd_id;      /**< Command ID. */
  qsee_client_send_data_req_t  req;         /**< Client send data-specific request. */
  UINT32                       req_len;     /**< QSEE OS command-specific request length. */
  struct tz_syscall_rsp_s      common_rsp;  /**< QSEE blocking kernel layer common response structure. */
  qsee_command_resp_info_t     rsp;         /**< QSEE enhanced kernel layer response. */
} __attribute__ ((packed)) tz_app_send_data_syscall_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_LISTENER_DATA_RSP_COMMAND. */

/** @weakgroup weak_tz_app_listener_response_syscall_s
@{ */
typedef struct tz_app_listener_response_syscall_s
{
  struct tz_syscall_req_s    common_req; /**< QSEE blocking kernel layer common request structure. */
  UINT32                     cmd_id;     /**< Command ID. */
  qsee_listener_rsp_t        req;        /**< Listener response command-specific request. */
  UINT32                     req_len;    /**< QSEE OS command-specific request length. */
  struct tz_syscall_rsp_s    common_rsp; /**< QSEE blocking kernel layer common response structure. */
  qsee_command_resp_info_t   rsp;        /**< QSEE enhanced kernel layer response. */
} __attribute__ ((packed)) tz_app_listener_response_syscall_t;

/** @} */ /* end_weakgroup */

/*
   Top level TZ syscall request structure for
   QSEE_REGISTER_LOG_BUF_COMMAND. */

/** @weakgroup weak_tz_app_register_logbuffer_syscall_s
@{ */
typedef struct tz_app_register_logbuffer_syscall_s
{
	struct tz_syscall_req_s    common_req;  /**< TZBSP common request structure. */
	UINT32                     cmd_id;      /**< Command ID. */
	qsee_reg_log_buf_req_t     req;         /**< Register log buffer-specific request. */
	UINT32                     req_len;     /**< TZOS command-specific request length. */
	struct tz_syscall_rsp_s    common_rsp;  /**< TZBSP common response structure. */
	qsee_command_resp_info_t   rsp;         /**< QSEE response. */
} __attribute__ ((packed)) tz_app_register_logbuffer_syscall_t;


// Top level TZ syscall response structure for APP_REGION_NOTIFICATION_CMD.
typedef struct tz_apps_region_syscall_s
{
  struct tz_syscall_req_s         common_req;  /**< QSEE blocking kernel layer common request structure. */
  UINT32                          cmd_id;      /**< Qsee Command ID. */
  qsee_apps_region_notification_t req;         /**< app region notification */
  UINT32                          req_len;     /**< TZOS command-specific request length. */
  struct tz_syscall_rsp_s         common_rsp;  /**< TZBSP common response structure. */
  qsee_command_resp_info_t        rsp;         /**< QSEE response. */
} __attribute__ ((packed)) tz_apps_region_syscall_t;

// Top level TZ syscall response structure for APP_LOAD_SERV_IMAGE_CMD.
typedef struct tz_serv_image_syscall_s
{
  struct tz_syscall_req_s        common_req;  /**< QSEE blocking kernel layer common request structure. */
  UINT32                         cmd_id;      /**< Qsee Command ID. */
  qsee_load_serv_image_t         req;         /**< app region notification */
  UINT32                         req_len;     /**< TZOS command-specific request length. */
  struct tz_syscall_rsp_s        common_rsp;  /**< TZBSP common response structure. */
  qsee_command_resp_info_t       rsp;         /**< QSEE response. */
} __attribute__ ((packed)) tz_serv_image_syscall_t;

// RPMB provisioning structure for APP_PROVISION_RPMB_KEY_COMMAND
typedef struct tz_provision_rpmb_key_syscall_s
{
  struct tz_syscall_req_s        common_req;  /**< QSEE blocking kernel layer common request structure. */
  UINT32                         cmd_id;      /**< Qsee Command ID. */
  qsee_provision_rpmb_key_req_t  req;         /**< rpmb provisioning */
  UINT32                         req_len;     /**< TZOS command-specific request length. */
  struct tz_syscall_rsp_s        common_rsp;  /**< TZBSP common response structure. */
  qsee_command_resp_info_t       rsp;         /**< QSEE response. */
} __attribute__ ((packed)) tz_provision_rpmb_key_syscall_t;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif /* QSEE_INTERFACE_ARMV7_H */

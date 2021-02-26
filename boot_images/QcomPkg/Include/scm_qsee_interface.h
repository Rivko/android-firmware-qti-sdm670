#ifndef SCM_QSEE_INTERFACE_H
#define SCM_QSEE_INTERFACE_H

/**
   @file scm_qsee_interface.h
   @brief Provide the SYSCALL API infrastructure
*/

/*===========================================================================
   Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE


when       who      what, where, why
--------   ---      ------------------------------------
09/04/14   sm       Initial version.

===========================================================================*/

/** Maximum character string for applications */

#define QSEE_MAX_NAME_SIZE         32

#define __attribute__(x)
#pragma pack(push, 1)
typedef struct qsee_app_start_req_s
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
} __attribute__ ((packed)) qsee_app_start_req_t;

typedef struct qsee_app_shutdown_req_s
{
  UINT32                     app_id;      /**< Application shutdown-specific
                                               request. */
} __attribute__ ((packed)) qsee_app_shutdown_req_t;

typedef struct qsee_app_lookup_req_s
{
  char                       app_name[QSEE_MAX_NAME_SIZE];
                                           /**< Application
                                                lookup-specific
                                                request. */
} __attribute__ ((packed)) qsee_app_lookup_req_t;

typedef struct qsee_register_listener_s
{
  UINT32 listener_id;  /**< ID of the listener service. */
  UINT32 sb_ptr;       /**< Pointer to the shared buffer. */
  UINT32 sb_len;       /**< Length of the shared buffer. */
} __attribute__ ((packed)) qsee_register_listener_t;

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

typedef struct qsee_apps_region_notificaton_s
{

  UINT32  applications_region_addr;
  UINT32  applications_region_size; 
} __attribute__ ((packed)) qsee_apps_region_notification_t;

typedef struct qsee_register_log_buffer_s
{

  UINT32  log_buffer_addr;
  UINT32  log_buffer_size; 
} __attribute__ ((packed)) qsee_register_log_buffer_t;

typedef struct qsee_load_serv_image_s
{

  UINT32  mdt_len;
  UINT32  img_len;
  UINT32  pa;
} __attribute__ ((packed)) qsee_load_serv_image_t;

typedef struct qsee_provision_rpmb_key_req_s
{
  UINT32 key_type;
} __attribute__ ((packed)) qsee_provision_rpmb_key_req_t;

#pragma pack(pop)
#endif

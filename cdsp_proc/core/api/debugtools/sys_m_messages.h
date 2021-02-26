#ifndef SYS_M_MESSAGES_H
#define SYS_M_MESSAGES_H
/*===========================================================================

           SYS_M_MESSAGES . H

DESCRIPTION

Copyright (c) 2011-17 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/sys_m_messages.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/
#if defined(__cplusplus)
extern "C"
{
#endif

/** @addtogroup sys_m_api @{ */

/**
API, Typedefs and Enumerations
*/
typedef char const* SYS_M_NAME;                                                  /**< Name */
typedef void* SYS_M_HANDLE;                                                      /**< Opaque Handle */
typedef void (*SYS_M_FNSIG)(void);                                               /**< Specific Function Signature */

#define SYS_M_NULL ((void*)0)                                                    /**< ISO/ANSI-C, Forward reference NULL pointer */

/**
API, SYS_M_NAME
*/
#define SYS_M_SSR_LPASS_BEFORE_SHUTDOWN      "ssr:lpass:before_shutdown"         /**< SYS_M_NAME */
#define SYS_M_SSR_ADSP_BEFORE_SHUTDOWN       "ssr:adsp:before_shutdown"          /**< SYS_M_NAME */
#define SYS_M_SSR_SLPI_BEFORE_SHUTDOWN       "ssr:slpi:before_shutdown"          /**< SYS_M_NAME */
#define SYS_M_SSR_WCNSS_BEFORE_SHUTDOWN      "ssr:wcnss:before_shutdown"         /**< SYS_M_NAME */
#define SYS_M_SSR_DSPS_BEFORE_SHUTDOWN       "ssr:dsps:before_shutdown"          /**< SYS_M_NAME */
#define SYS_M_SSR_MODEM_BEFORE_SHUTDOWN      "ssr:modem:before_shutdown"         /**< SYS_M_NAME */
#define SYS_M_SSR_GSS_BEFORE_SHUTDOWN        "ssr:gnss:before_shutdown"          /**< SYS_M_NAME */
#define SYS_M_SSR_EXT_MODEM_BEFORE_SHUTDOWN  "ssr:ext_modem:before_shutdown"     /**< SYS_M_NAME */

#define SYS_M_SSR_LPASS_BEFORE_POWERUP       "ssr:lpass:before_powerup"          /**< SYS_M_NAME */
#define SYS_M_SSR_ADSP_BEFORE_POWERUP        "ssr:adsp:before_powerup"           /**< SYS_M_NAME */
#define SYS_M_SSR_SLPI_BEFORE_POWERUP        "ssr:slpi:before_powerup"           /**< SYS_M_NAME */
#define SYS_M_SSR_WCNSS_BEFORE_POWERUP       "ssr:wcnss:before_powerup"          /**< SYS_M_NAME */
#define SYS_M_SSR_DSPS_BEFORE_POWERUP        "ssr:dsps:before_powerup"           /**< SYS_M_NAME */
#define SYS_M_SSR_MODEM_BEFORE_POWERUP       "ssr:modem:before_powerup"          /**< SYS_M_NAME */
#define SYS_M_SSR_GSS_BEFORE_POWERUP         "ssr:gnss:before_powerup"           /**< SYS_M_NAME */
#define SYS_M_SSR_EXT_MODEM_BEFORE_POWERUP   "ssr:ext_modem:before_powerup"      /**< SYS_M_NAME */

#define SYS_M_SSR_LPASS_AFTER_POWERUP        "ssr:lpass:after_powerup"           /**< SYS_M_NAME */
#define SYS_M_SSR_ADSP_AFTER_POWERUP         "ssr:adsp:after_powerup"            /**< SYS_M_NAME */
#define SYS_M_SSR_SLPI_AFTER_POWERUP         "ssr:slpi:after_powerup"            /**< SYS_M_NAME */
#define SYS_M_SSR_WCNSS_AFTER_POWERUP        "ssr:wcnss:after_powerup"           /**< SYS_M_NAME */
#define SYS_M_SSR_DSPS_AFTER_POWERUP         "ssr:dsps:after_powerup"            /**< SYS_M_NAME */
#define SYS_M_SSR_MODEM_AFTER_POWERUP        "ssr:modem:after_powerup"           /**< SYS_M_NAME */
#define SYS_M_SSR_GSS_AFTER_POWERUP          "ssr:gnss:after_powerup"            /**< SYS_M_NAME */
#define SYS_M_SSR_EXT_MODEM_AFTER_POWERUP    "ssr:ext_modem:after_powerup"       /**< SYS_M_NAME */

#define SYS_M_SSR_LPASS_AFTER_SHUTDOWN       "ssr:lpass:after_shutdown"          /**< SYS_M_NAME */
#define SYS_M_SSR_ADSP_AFTER_SHUTDOWN        "ssr:adsp:after_shutdown"           /**< SYS_M_NAME */
#define SYS_M_SSR_SLPI_AFTER_SHUTDOWN        "ssr:slpi:after_shutdown"           /**< SYS_M_NAME */
#define SYS_M_SSR_WCNSS_AFTER_SHUTDOWN       "ssr:wcnss:after_shutdown"          /**< SYS_M_NAME */
#define SYS_M_SSR_DSPS_AFTER_SHUTDOWN        "ssr:dsps:after_shutdown"           /**< SYS_M_NAME */
#define SYS_M_SSR_MODEM_AFTER_SHUTDOWN       "ssr:modem:after_shutdown"          /**< SYS_M_NAME */
#define SYS_M_SSR_GSS_AFTER_SHUTDOWN         "ssr:gnss:after_shutdown"           /**< SYS_M_NAME */
#define SYS_M_SSR_EXT_MODEM_AFTER_SHUTDOWN   "ssr:ext_modem:after_shutdown"      /**< SYS_M_NAME */

#define SYS_M_SHUTDOWN_STM_CLEANUP           "sys_m:shutdown_stm_cleanup"

/**
API, Register Listen Function to Event by Name
See prerequsite note with rcecb_create_name
@param[in]     SYS_M_NAME                    SYS_M_NAME Event Name, Not Arbitrary
@param[in]     SYS_M_FNSIG                   Listen Callback Function Pointer
@return        SYS_M_HANDLE                  Opaque Handle, Operation Success Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
SYS_M_HANDLE sys_m_listen_func_register_name(SYS_M_NAME const name, SYS_M_FNSIG const func);

/**
API, Unregister Listen Function to Event by Name
@param[in]     SYS_M_HANDLE                  Opaque Handle
@return        SYS_M_HANDLE                  Operation Success Indication
*/
#if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__clang__)
__attribute__((warn_unused_result))
#endif
SYS_M_HANDLE sys_m_listen_func_unregister_name(SYS_M_NAME const name, SYS_M_FNSIG const func);



/** @} end_addtogroup sys_m_api */

#if defined(__cplusplus)
}
#endif

#endif  /* SYS_M_MESSAGES_H */

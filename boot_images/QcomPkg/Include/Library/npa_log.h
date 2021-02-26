/*============================================================================
@file npa_log.h

NPA Log Function Declarations

Copyright (c) 2012-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#ifndef NPA_LOG_H
#define NPA_LOG_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "npa.h"

/* Name of default NPA Log  */
#define NPA_DEFAULT_LOG_NAME "NPA Log"

#ifdef __cplusplus
extern "C" {
#endif

struct npa_resource;

/**
 * <!-- npa_add_resource_log_by_name -->
 *
 * @brief Copies the npa logging messages associated with the specified
 * resource(s) to a separate log.
 *
 *
 * Users could specify a single resource by its full resource name or a 
 * bunch of resources by the common prefix followed by a wildcard (...). 
 * For example,
 * /core/cpu         a single resource with its full name
 * /core/cpu/...     all resources starting with "/core/cpu/"
 * 
 * Note: in the initial implementation, we only support a single trailing
 * wildcard: "..." at the directory level, which means it should follow
 * a slash. The use cases below are not supported right now:
 * /core/.../latency
 * /core/c...  
 *
 * Note: the resource(s) should be created before the call to this function
 * for the associated messages to be copied to the named log.
 *
 * @param ulog_name : Name of the log to which the messages should go, the
 * name must follow the ulog naming restrictions
 * @param resource_name : Name or name root of the resource(s)
 *
 * @return success/failure (npa_status)
 */
npa_status npa_add_resource_log_by_name( char *log_name, char *resource_name );


/**
 * <!-- npa_remove_resource_log_by_name -->
 *
 * @brief Stops copying messages associated with the specified resources to the named log
 * 
 * Users could specify a single resource by its full resource name or a 
 * bunch of resources by the common prefix followed by a wildcard (...). 
 * For example,
 * /core/cpu         a single resource with its full name
 * /core/cpu/...     all resources starting with "/core/cpu/"
 * 
 * It is not permissable to remove the default NPA log ("NPA Log") by
 * resource name.  This log can only be removed via
 * npa_remove_resource_log_by_handle()
 *
 * Note: in the initial implementation, we only support a single trailing
 * wildcard "..." at the directory level, which means it should follow
 * a slash. The use cases below are not supported right now:
 * /core/.../latency
 * /core/c...  
 *
 * Note: the resource(s) must exist, if not, then the function will return an 
 * error.
 *
 * @param ulog_name : Name of the log, which must follow the ulog naming restrictions
 * @param resource_name : Name or name root of the resource(s)
 *
 * @return success/failure (npa_status)
 */
npa_status npa_remove_resource_log_by_name( char *log_name, 
                                            char * resource_name );


/**
 * <!-- npa_add_resource_log_by_handle -->
 *
 * @brief Copies the npa logging messages associated with an npa resource to a separate log
 *
 * @param ulog_name : Name of the log to which the messages should go, the
 * name must follow the ulog naming restrictions
 * @param resource : Handle to the resource that is of interested
 *
 * @return success/failure (npa_status)
 */
npa_status npa_add_resource_log_by_handle( char *log_name,
                                           struct npa_resource *resource );


/**
 * <!-- npa_remove_resource_log_by_handle -->
 *
 * @brief Stops copying messages associated with an npa resource to the named log
 *
 * @param ulog_name : Name of the log, which must follow the ulog naming restrictions
 * @param resource : Handle to the resource that is of interested
 *
 * @return success/failure (npa_status)
 */
npa_status npa_remove_resource_log_by_handle( char *log_name,
                                              struct npa_resource *resource );

#ifdef __cplusplus
}
#endif

#endif /* NPA_LOG_H */
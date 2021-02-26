/*=============================================================================

                procinfo_qdi.h  --  Header File

GENERAL DESCRIPTION
      Process Information server QDI Header File

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1/debugtools/api/procinfo_qdi.h#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/12/16   bbhatt     File created to support Process Information server QDI driver.

=============================================================================*/
#ifndef PROCINFO_QDI_H
#define PROCINFO_QDI_H

#include "stdlib.h"
#include "procinfo.h"
#include "qurt.h"
#include "qurt_qdi_driver.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define PROCINFO_DRIVER_NAME                     "/dev/procinfo"
#define PROCINFO_QDI_INTERNAL_NAME_LEN           QURT_MAX_NAME_LEN
#define PROCINFO_QDI_OEM_NAME_LEN                16
#define PROCINFO_QDI_DOMAIN_NAME_LEN             80

#define PROCINFO_QDI_OP                          (0+QDI_PRIVATE)

typedef struct procinfo_qdi_device_data_s
{
  boolean islocal;
  unsigned int  pid;
  char    oem_name[PROCINFO_QDI_OEM_NAME_LEN];
  char    internal_name[PROCINFO_QDI_INTERNAL_NAME_LEN];
  char    domain_name[PROCINFO_QDI_DOMAIN_NAME_LEN];
} procinfo_qdi_device_data_t, * procinfo_qdi_device_data_p;

typedef struct procinfo_qdi_device_s
{
  qurt_qdi_obj_t                  qdiobj; /* Should be the first member */
  procinfo_qdi_device_data_t      user_pd_data;
  struct procinfo_qdi_device_s    *next;
} procinfo_qdi_device_t, * procinfo_qdi_device_p;

/** =====================================================================
 * Function:
 *     procinfo_get_name_from_pid
 *
 * Description: 
 *     Get name of the process using PID
 *
 * Parameters:
 *     name_type        : Value of name type from enum
 *     pid              : ASID of the process
 *     dest             : Destination buffer to recieve domain name
 *     dest_len         : Length of user buffer
 *
 * Returns:
 *    PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *    Refer to the enum PROCINFO_RESULT for a list of possible results 
 * =====================================================================  */
PROCINFO_RESULT procinfo_get_name_from_pid(uint32_t ntype, uint32_t pid, char *dest, size_t dest_len);

/** =====================================================================
 * Function:
 *     procinfo_client_get_name
 *
 * Description: 
 *     Get name for its own process domain
 *
 * Parameters:
 *     ntype            : Value of name type from enum
 *     dest             : Destination buffer to recieve domain name
 *     dest_len         : Length of user buffer
 *
 * Returns:
 *    PROCINFO_QDI_SUCCESS (0) or PROCINFO_QDI_FAILURE(-1)
 *    Refer to the enum PROCINFO_RESULT for a list of possible results 
 * =====================================================================  */
PROCINFO_RESULT procinfo_client_get_name(uint32_t ntype, char *dest, size_t dest_len);

/** =====================================================================
 * Function:
 *     procinfo_qdi_init
 *
 * Description:
 *     Initialization function. Registers a QDI device with the generic QDI object
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     None
 * =====================================================================  */
void procinfo_qdi_init(void);

#if defined(__cplusplus)
}
#endif

#endif /* PROCINFO_QDI_H */
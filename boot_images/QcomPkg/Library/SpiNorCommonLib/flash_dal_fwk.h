#ifndef FLASH_DAL_FWK_H
#define FLASH_DAL_FWK_H
/*=============================================================================
 *
 * FILE:      flash_dal_fwk.h
 *
 * DESCRIPTION: Function declarations specific to Flash DAL framework
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 *        Copyright © 2008-2009 Qualcomm Technologies Incorporated.
 *               All Rights Reserved.
 *            QUALCOMM Proprietary/GTDR
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALStdDef.h"

#include "flash_dal.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


/*  This function alldoes the following:
 *     1. Alocates memory for the flash client context,
 *     2. Intializes the flash handle function table vtbl, 
 *     3. Links in the client ctx to static_flash_clients.
 *     4. Increments the number of clients (ref count)
 *     5. Attach device to DALFW
 */
int flash_fwk_attach(const char *pszArg, DALDEVICEID dev_id, 
  DalDeviceHandle **handle_dal_device);
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef FLASH_DAL_FWK_H */


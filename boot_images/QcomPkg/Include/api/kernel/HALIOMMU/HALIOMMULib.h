#ifndef __HAL_IOMMU_LIB_H__
#define __HAL_IOMMU_LIB_H__

/*==============================================================================
@file HAL_IOMMULib.h

    This file contains the function declarations for the LIB interface that
    other drivers can use to interact with the HAL_IOMMUDxe driver.

        Copyright (c) 2008,2010,2014 Qualcomm Technologies, Inc.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary

==============================================================================*/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/19/17   amo     Created

=============================================================================*/

/*------------------------------------------------------------------------------
* Include Files
*-----------------------------------------------------------------------------*/
#include "HALIOMMUTypes.h"
#include <Protocol/EFIHALIOMMUProtocol.h>
#include <Library/UefiBootServicesTableLib.h>

/*------------------------------------------------------------------------------
* Function Declarations
*-----------------------------------------------------------------------------*/

/** 
 * Function: hal_iommu_domain_create
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_domain_create(void **pp_domain);

/** 
 * Function: hal_iommu_domain_delete
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_domain_delete(void *p_domain);

/** 
 * Function: hal_iommu_domain_attach_device
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_domain_attach_device(void *p_domain, void *client_handle, uint32_t arid, uint32_t mask);

/** 
 * Function: hal_iommu_domain_detach_device
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_domain_detach_device(void *p_domain, void *client_handle, uint32_t arid, uint32_t mask);

/** 
 * Function: hal_iommu_domain_configure
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_domain_configure(void *p_domain, struct hal_iommu_domain_config *p_domain_cfg);

/** 
 * Function: hal_iommu_flush_domain
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_flush_domain(void *p_domain);

/** 
 * Function: hal_iommu_flush_domain_va_list
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_flush_domain_va_list(void *p_domain, boolean last_level, uint32_t number, uint64_t *va_list);

/** 
 * Function: hal_iommu_register_fault_handler
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_register_fault_handler(void *p_domain, fp_hal_iommu_fault_handler_t handler, void *p_context);

/** 
 * Function: hal_iommu_configure_bypass_domain
 *
 * Description: Allow clients to configure a simple bypass domain
 *
 * Input: p_domain -- Domain handle
 *        type     -- Bypass domain type
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t hal_iommu_configure_bypass_domain(void *p_domain, enum hal_iommu_bypass_domain_type type);



/** 
 * Function: IDIOM_Detach
 *
 * Description: Allow clients to detach device from  IDIOM.
 *
 * Input:  
 *
 * Output:
 *
 * Returns: HAL_IOMMU_ERR_OK on success, error code otherwise.
 */
HAL_IOMMUResult_t IDIOM_Detach(void *client_handle, uint32_t arid, uint32_t mask);
#endif /* __HAL_IOMMU_LIB_H__ */

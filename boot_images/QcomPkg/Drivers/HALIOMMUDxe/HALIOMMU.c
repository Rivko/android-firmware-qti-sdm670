#include <Library/MemoryAllocationLib.h>
#include <Library/LoaderUtils.h>
#include "HALIOMMULib.h"
#include "HALIOMMUReg.h"
#include "IORT.h"

#define HAL_IOMMU_MAX_ARIDS 16 //(todo) move?
#define DEVICE_NAME_LEN 32

#define SCLTR_IO_COHERENCY_MASK     0x00C00001
#define SCLTR_IO_COHERENT_VAL       0x00800000
  

struct hal_iommu_device_list_obj {
    char device_name[DEVICE_NAME_LEN];
    uint32_t arids[HAL_IOMMU_MAX_ARIDS];
    struct hal_iommu_device_list_obj *next;
};

struct hal_iommu_list_obj {
    void *base_addr;
    enum hal_iommu_mmu_arch arch;
    uint32_t cb_idx;
    struct hal_iommu_device_list_obj *dev_list_head;
    struct hal_iommu_list_obj *next;
};

struct hal_iommu_domain_obj {
    enum hal_iommu_vmsa pt_type;
    union {
        struct hal_iommu_domain_aarch64 domain_cfg64;
    } config;
    struct hal_iommu_list_obj *hal_iommu_list_head;
    uint32_t ref_count;
};

/*external functions used */
extern HAL_IOMMUResult_t IDIOM_domain_lookup(void**p_domain_local, void *client_handle);

void hal_iommu_memory_barrier(void)
{
    __asm("DSB sy");
}

void hal_iommu_invalidate_tlb_all(struct hal_iommu_list_obj *p_hal_iommu_obj)
{
    if(p_hal_iommu_obj->arch == HAL_IOMMU_ARCH_MMU500 || p_hal_iommu_obj->arch == HAL_IOMMU_ARCH_QSMMU) {
        HAL_IOMMU_InvalidateCBTLBAll((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx);
    } else {
        // no other architectures supported 
    }
    
    return;
}

void hal_iommu_invalidate_tlb_va_list(struct hal_iommu_list_obj *p_hal_iommu_obj, uint32_t number, uint64_t *va_list)
{
    int i;
    for(i = 0; i < number; i++)
    {
        HAL_IOMMU_InvalidateCBTLBVA((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx, va_list[i]);
    }
    
    return;
}

static HAL_IOMMUResult_t program_smr_s2cr_to_cb(struct hal_iommu_list_obj *p_hal_iommu_obj, uint32_t stream_id)
{
    HAL_IOMMU_SMRn_t smr;
    HAL_IOMMU_S2CRn_t s2cr;
    HAL_IOMMU_Secure_IDR0_t idr0;
    uint32_t i;
    
    /* Input Validation */
    if(!p_hal_iommu_obj) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    HAL_IOMMU_GetSecureIDR0((uint32_t)p_hal_iommu_obj->base_addr, &idr0);
    
    for(i = 0; i < idr0.formatted.NUMSMRG - 1; i++) {
        HAL_IOMMU_GetSMRn((uint32_t)p_hal_iommu_obj->base_addr, i, &smr);
        
        if(smr.formatted.smr_type.type_exid0.VALID)
            continue;
        
        smr.raw = stream_id | HAL_IOMMU_VALID_MASK;
        HAL_IOMMU_SetSMRn_raw((uint32_t)p_hal_iommu_obj->base_addr, i, smr);
        
        s2cr.raw = 0;
        s2cr.formatted.TYPE = HAL_IOMMU_S2CR_TYPE_TRANSLATION_CONTEXT;
        s2cr.formatted.FIELD = p_hal_iommu_obj->cb_idx;
        HAL_IOMMU_SetS2CRn((uint32_t)p_hal_iommu_obj->base_addr, i, s2cr);
        
        return HAL_IOMMU_ERR_OK;
    }
    
    // No free SMR was found
    return HAL_IOMMU_ERR_OUT_OF_RANGE;
}

static HAL_IOMMUResult_t remove_smr_s2cr_from_cb(struct hal_iommu_list_obj *p_hal_iommu_obj, uint32_t stream_id)
{
    HAL_IOMMU_SMRn_t smr;
    HAL_IOMMU_S2CRn_t s2cr;
    HAL_IOMMU_Secure_IDR0_t idr0;
    uint32_t i;
    
    /* Input Validation */
    if(!p_hal_iommu_obj) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    HAL_IOMMU_GetSecureIDR0((uint32_t)p_hal_iommu_obj->base_addr, &idr0);
    
    for(i = 0; i < idr0.formatted.NUMSMRG - 1; i++) {
        HAL_IOMMU_GetSMRn((uint32_t)p_hal_iommu_obj->base_addr, i, &smr);
        
        if(!smr.formatted.smr_type.type_exid0.VALID)
            continue;
        
        if(smr.raw == (stream_id | HAL_IOMMU_VALID_MASK)) {
            smr.raw = 0;
            s2cr.raw = 0;
            HAL_IOMMU_SetSMRn_raw((uint32_t)p_hal_iommu_obj->base_addr, i, smr);
            HAL_IOMMU_SetS2CRn((uint32_t)p_hal_iommu_obj->base_addr, i, s2cr);
            return HAL_IOMMU_ERR_OK;
        }
    }
    return HAL_IOMMU_ERR_SID_NOT_FOUND;
}

static void get_free_cb(void *base_addr, enum hal_iommu_vmsa pt_type, uint32_t *cb_idx)
{
    HAL_IOMMU_Secure_IDR1_t idr1;
    HAL_IOMMU_CBARn_t cbar;
    HAL_IOMMU_CBA2Rn_t cba2r;
    uint32_t i;
    
    /* Input Validation */
    if(!base_addr || !cb_idx) {
        return;
    }
    
    // read NUMCB from IDR
    HAL_IOMMU_GetSecureIDR1((uint32_t)base_addr, &idr1);
    
    for(i = 0; i < idr1.formatted.NUMCB; i++) {
        // get CBARi
        HAL_IOMMU_GetCBARn((uint32_t)base_addr, i, &cbar);
        
        // check if CBAR is used or in POR state (both AC and no_AC)
        if( (cbar.formatted.cbar_type.type_s1.TYPE == HAL_IOMMU_CB_S1_CTX_S2_FAULT) || (cbar.raw == 0) || 
            ((cbar.formatted.cbar_type.type_s1.TYPE == HAL_IOMMU_CB_S1_CTX_S2_BYPASS) && (cbar.formatted.cbar_type.type_s1.VMID == HAL_IOMMU_CB_UNUSED_VMID)) ) {
            // CB is free, configure CBAR, CBA2R, and return cb_idx
            // TODO Configure VMID?  As what?
            cbar.formatted.cbar_type.type_s1.TYPE = HAL_IOMMU_CB_S1_CTX_S2_BYPASS;
            cbar.formatted.cbar_type.type_s1.VMID = 0;
            HAL_IOMMU_SetCBARn((uint32_t)base_addr, i, cbar);
            HAL_IOMMU_GetCBA2Rn((uint32_t)base_addr, i, &cba2r);
            cba2r.formatted.VA64 = (pt_type == HAL_IOMMU_ARM_ARCH_AARCH64) ? 1 : 0;
            HAL_IOMMU_SetCBA2Rn((uint32_t)base_addr, i, cba2r);
            *cb_idx = i;
            return;
        }
    }
    
    // Could not find an available context bank, cb_idx will remain INVALID
    return;
}

static void release_cb_idx(struct hal_iommu_list_obj *p_hal_iommu_obj)
{
    HAL_IOMMU_CBARn_t cbar;
    
    cbar.raw = 0;
    cbar.formatted.cbar_type.type_s1.TYPE = HAL_IOMMU_CB_S1_CTX_S2_BYPASS;
    cbar.formatted.cbar_type.type_s1.VMID = HAL_IOMMU_CB_UNUSED_VMID;
    HAL_IOMMU_SetCBARn((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx, cbar);
    p_hal_iommu_obj->cb_idx = HAL_IOMMU_INVALID_CB_IDX;
    
    return;
}

static void *alloc_dev_list_node(void)
{
    struct hal_iommu_device_list_obj *dev_list_node;
    int i;
    
    dev_list_node = (struct hal_iommu_device_list_obj *)AllocatePool(sizeof(struct hal_iommu_device_list_obj));
    
    if (dev_list_node) {
        // Initialize node
        memset((void*)dev_list_node, 0x0, sizeof(struct hal_iommu_device_list_obj));
        for(i = 0; i < HAL_IOMMU_MAX_ARIDS; i++) {
            dev_list_node->arids[i] = HAL_IOMMU_INVALID_ARID;
        }
    }
    
    return dev_list_node;
}

static void free_dev_list_node(void *dev_list_node)
{
    if(dev_list_node)
        FreePool(dev_list_node);
    
    return;
}

static struct hal_iommu_list_obj *retrieve_hal_iommu_obj_from_domain(struct hal_iommu_domain_obj *p_domain_local, void *base_addr)
{
    struct hal_iommu_list_obj *curr_iommu, *ret = NULL;
    
    /* Input Validation */
    if(!p_domain_local || !base_addr) {
        return NULL;
    }
    
    curr_iommu = p_domain_local->hal_iommu_list_head;
    
    while(curr_iommu != NULL) {
        if(curr_iommu->base_addr == base_addr) {
            // IOMMU object already exists in this domain.  Just return the pointer.
            ret = curr_iommu;
            break;
        }
        curr_iommu = curr_iommu->next;
    } // end while loop
    
    return ret;
}

static HAL_IOMMUResult_t add_device_to_iommu(struct hal_iommu_list_obj *p_hal_iommu_obj, void *device_name, uint32_t arid)
{
    struct hal_iommu_device_list_obj *curr_device, *prev_device;
    int i;
    
    if(!p_hal_iommu_obj || !device_name || arid == HAL_IOMMU_INVALID_ARID) {
        return HAL_IOMMU_ERR_INVALID_PARAM;
    }
    
    if(strlen((char *)device_name) >= DEVICE_NAME_LEN) {
        return HAL_IOMMU_ERR_INVALID_PARAM;
    }
    
    curr_device = p_hal_iommu_obj->dev_list_head;
    prev_device = NULL;
    
    while(curr_device != NULL) {
        if(strncmp(curr_device->device_name, (char *)device_name, DEVICE_NAME_LEN) == 0) {
            // Device is already present, attempt to add ARID
            for(i = 0; i < HAL_IOMMU_MAX_ARIDS; i++) {
                if(curr_device->arids[i] == arid) {
                    // ARID already present, attempted configuration already present.
                    return HAL_IOMMU_ERR_REDUNDANT_CONFIGURATION;
                } else if(curr_device->arids[i] == HAL_IOMMU_INVALID_ARID) {
                    // Found empty ARID slot
                    curr_device->arids[i] = arid;
                    return HAL_IOMMU_ERR_OK;
                }
            } // end for loop
            // If we reach this point, no available ARID slots remain
            return HAL_IOMMU_ERR_OUT_OF_RANGE;
        }
        // Iterate through the list
        prev_device = curr_device;
        curr_device = curr_device->next;
    } // end while loop
    
    // Reached the end, allocate new node
    curr_device = (struct hal_iommu_device_list_obj *)alloc_dev_list_node();
    if(!curr_device)
        return HAL_IOMMU_ERR_MALLOC_FAILED;
    
    // Initialize and Attach new node to the end of the list
    curr_device->next = NULL; // redundant, as alloc initializes non-arid members to NULL, but good practice
    strlcpy(curr_device->device_name, (char *)device_name, DEVICE_NAME_LEN);
    curr_device->arids[0] = arid;
    if(prev_device == NULL)
        p_hal_iommu_obj->dev_list_head = curr_device;
    else
        prev_device->next = curr_device;
    
    return HAL_IOMMU_ERR_OK;
}

static HAL_IOMMUResult_t remove_device_from_iommu(struct hal_iommu_list_obj *p_hal_iommu_obj, void *device_name, uint32_t arid)
{
    struct hal_iommu_device_list_obj *curr_device, *prev_device;
    int i;
    
    if(!p_hal_iommu_obj || !device_name || arid == HAL_IOMMU_INVALID_ARID) {
        return HAL_IOMMU_ERR_INVALID_PARAM;
    }
    
    if(strlen((char *)device_name) >= DEVICE_NAME_LEN) {
        return HAL_IOMMU_ERR_INVALID_PARAM;
    }
    
    curr_device = p_hal_iommu_obj->dev_list_head;
    
    while(curr_device != NULL) {
        if(strncmp(curr_device->device_name, (char *)device_name, DEVICE_NAME_LEN) == 0) {
            // Found our device
            for(i = 0; i < HAL_IOMMU_MAX_ARIDS; i++) {
                if(curr_device->arids[i] == arid) {
                    // Found our ARID, collapse array to fill the void at index i.
                    if(i == HAL_IOMMU_MAX_ARIDS - 1) {
                        // ARID is at last index, just wipe it out
                        curr_device->arids[i] = HAL_IOMMU_INVALID_ARID;
                    } else {
                        // shift contents from i+1 to the end of the array into i
                        // To Illustrate (if i=2):  [1, 2, 3 , 4, -1, -1, -1, ..., -1] ==> [1, 2, 4, -1, -1, -1, -1, ..., -1]
                        size_t src_size, dest_size;
                        dest_size = (sizeof(uint32_t) * HAL_IOMMU_MAX_ARIDS) - (i * sizeof(uint32));
                        src_size = dest_size - sizeof(uint32_t);
                        memscpy(&curr_device->arids[i], dest_size, &curr_device->arids[i+1], src_size);
                    }
                    // ARID was found and removed
                    break;
                }
            } // end for loop 
            
            if(i == HAL_IOMMU_MAX_ARIDS) {
                // Found device but ARID was not present.  Redundant or spurious remove requested.
                return HAL_IOMMU_ERR_REDUNDANT_CONFIGURATION;
            } else {
                // Our device was found and ARID removed
                break;
            }            
        }
        prev_device = curr_device;
        curr_device = curr_device->next;
    } // end while loop
    
    if(curr_device != NULL) {
        // Check if array is emptry. Valid ARIDs are always in the front
        if(curr_device->arids[0] == HAL_IOMMU_INVALID_ARID) {
            // No more ARIDs are attached to device... can free node
            if(p_hal_iommu_obj->dev_list_head == curr_device)
                p_hal_iommu_obj->dev_list_head = curr_device->next;
            else
                prev_device->next = curr_device->next;
            free_dev_list_node(curr_device);
        }
        // Return SUCCESS
        return HAL_IOMMU_ERR_OK;
    }

    // If we got here, then curr_device == NULL which means we did not find our device in the list
    return HAL_IOMMU_ERR_GENERIC_ERROR;
}

static void *allocate_hal_iommu_list_node(void) 
{
    return AllocatePool(sizeof(struct hal_iommu_list_obj));
}

static void free_hal_iommu_list_node(void *hal_iommu_list_node)
{
    FreePool(hal_iommu_list_node);
}

static void update_hal_iommu_configuration(struct hal_iommu_domain_obj *p_domain_local)
{
    struct hal_iommu_list_obj *curr_iommu;
    HAL_IOMMU_CBn_SCTLR_t sctlr;
    HAL_IOMMU_CBn_TTBCR_t tcr;
    
    curr_iommu = p_domain_local->hal_iommu_list_head;
    
    while(curr_iommu != NULL) {
        if(curr_iommu->dev_list_head && (curr_iommu->cb_idx != HAL_IOMMU_INVALID_CB_IDX)) {
            // There is a device attached to this IOMMU and it has been configured.  Update the configuration now.
            /* Configure IOMMU Context Bank for this Domain */
            if(p_domain_local->pt_type == HAL_IOMMU_ARM_ARCH_AARCH64) {
                sctlr.raw = p_domain_local->config.domain_cfg64.SCTLR;
                tcr.raw = p_domain_local->config.domain_cfg64.TCR;
                HAL_IOMMU_SetSCTLR((uint32_t)curr_iommu->base_addr, curr_iommu->cb_idx, sctlr);
                HAL_IOMMU_SetTTBR0((uint32_t)curr_iommu->base_addr, curr_iommu->cb_idx, p_domain_local->config.domain_cfg64.TTBR0);
                HAL_IOMMU_SetTTBR1((uint32_t)curr_iommu->base_addr, curr_iommu->cb_idx, p_domain_local->config.domain_cfg64.TTBR1);
                HAL_IOMMU_SetMAIR0((uint32_t)curr_iommu->base_addr, curr_iommu->cb_idx, p_domain_local->config.domain_cfg64.MAIR0);
                HAL_IOMMU_SetMAIR1((uint32_t)curr_iommu->base_addr, curr_iommu->cb_idx, p_domain_local->config.domain_cfg64.MAIR1);
                HAL_IOMMU_SetTTBCR((uint32_t)curr_iommu->base_addr, curr_iommu->cb_idx, tcr);    
            } else {
                // No support for AARCH32 at this time
            }
        }
        curr_iommu = curr_iommu->next;
    }
    
    hal_iommu_memory_barrier();
    
    return;
}

static HAL_IOMMUResult_t add_iommu_to_domain(struct hal_iommu_domain_obj *p_domain_local, void *base_addr, enum hal_iommu_mmu_arch arch, struct hal_iommu_list_obj **p_hal_iommu_obj)
{
    struct hal_iommu_list_obj *curr_iommu, *prev_iommu;
    
    /* Input Validation */
    if(!p_domain_local || !base_addr || !p_hal_iommu_obj) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    curr_iommu = p_domain_local->hal_iommu_list_head;
    prev_iommu = NULL;
    
    while(curr_iommu != NULL) {
        if(curr_iommu->base_addr == base_addr) {
            // IOMMU object already exists in this domain.  Just return the pointer.
            *p_hal_iommu_obj = curr_iommu;
            return HAL_IOMMU_ERR_OK;
        }
        // Iterate through the list
        prev_iommu = curr_iommu;
        curr_iommu = curr_iommu->next;
    } // end while loop
    
    /* Reached the end.  Allocate a new IOMMU object and initialze it. */
    curr_iommu = (struct hal_iommu_list_obj *)allocate_hal_iommu_list_node();
    if(!curr_iommu)
        return HAL_IOMMU_ERR_MALLOC_FAILED;
    
    // Initialize members
    curr_iommu->base_addr = base_addr;
    curr_iommu->arch = arch;
    curr_iommu->dev_list_head = NULL;
    curr_iommu->next = NULL;
    curr_iommu->cb_idx = HAL_IOMMU_INVALID_CB_IDX;
    
    // Add curr_iommu to the end of the list or as the head
    if(prev_iommu == NULL)
        p_domain_local->hal_iommu_list_head = curr_iommu;
    else
        prev_iommu->next = curr_iommu;
    
    // return newly allocated iommu object
    *p_hal_iommu_obj = curr_iommu;
    return HAL_IOMMU_ERR_OK;
}


/*------------------------------------------------------------------------------
* External Function Definitions
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
 * Returns:
 */
HAL_IOMMUResult_t hal_iommu_domain_create(void **pp_domain)
{
    struct hal_iommu_domain_obj *p_new_domain = NULL;
    
    /* Input Validation */
    if (pp_domain == NULL) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    /* Allocate Domain Object */
    p_new_domain = (struct hal_iommu_domain_obj *)AllocatePool(sizeof(struct hal_iommu_domain_obj));
    if(!p_new_domain) {
        return HAL_IOMMU_ERR_MALLOC_FAILED;
    }
    
    /* Initialize Members */
    memset((void*)p_new_domain, 0x0, sizeof(struct hal_iommu_domain_obj));
    p_new_domain->pt_type = HAL_IOMMU_ARM_ARCH_INVALID;
    
    /* Return opaque domain handle to caller */
    *pp_domain = (void*)p_new_domain;
    
    return HAL_IOMMU_ERR_OK;
}

/** 
 * Function: hal_iommu_domain_delete
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns:
 */
HAL_IOMMUResult_t hal_iommu_domain_delete(void *p_domain)
{
    struct hal_iommu_domain_obj *p_domain_local = (struct hal_iommu_domain_obj *)p_domain;
    struct hal_iommu_list_obj *next_iommu = NULL;
    
    /* Input Validation */
    if(!p_domain) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    if(p_domain_local->ref_count) {
        // Someone else is still using this domain
        return HAL_IOMMU_ERR_DOMAIN_IN_USE;
    }
    
    // Free all IOMMU devices
    while(p_domain_local->hal_iommu_list_head != NULL) {
        next_iommu = p_domain_local->hal_iommu_list_head->next;
        if(p_domain_local->hal_iommu_list_head->dev_list_head) {
            // Somehow Ref Count is 0 but there is still a device attached
            return HAL_IOMMU_ERR_GENERIC_ERROR;
        }
        /* release context bank and free IOMMU list node before moving to the next */
        release_cb_idx(p_domain_local->hal_iommu_list_head);
        free_hal_iommu_list_node((void *)p_domain_local->hal_iommu_list_head);
        p_domain_local->hal_iommu_list_head = next_iommu;
    }
    
    // Free Handle
    FreePool(p_domain);
    
    return HAL_IOMMU_ERR_OK;
}

/** 
 * Function: hal_iommu_domain_attach_device
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns:
 */
HAL_IOMMUResult_t hal_iommu_domain_attach_device(void *p_domain, void *client_handle, uint32_t arid, uint32_t mask)
{
    IORT_NamedComponentInfo named_component_info;
    IORT_SMMUv1v2Info smmu_info;
    IORT_IDMapping *p_arid_mapping = NULL, *p_id_mappings = NULL;
    HAL_IOMMU_CBn_SCTLR_t sctlr;
    HAL_IOMMU_CBn_TTBCR_t tcr;
    struct hal_iommu_domain_obj *p_domain_local = (struct hal_iommu_domain_obj *)p_domain;
    struct hal_iommu_list_obj *p_hal_iommu_obj = NULL;
    enum hal_iommu_mmu_arch arch = HAL_IOMMU_ARCH_INVALID;
    uint64_t base_addr;
    uint32_t stream_id;
    HAL_IOMMUResult_t ecode;
    int i;
    
    /* Input Validation */
    if(!p_domain || !client_handle) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    // Ensure Domain is Configured
    if(p_domain_local->pt_type == HAL_IOMMU_ARM_ARCH_INVALID) {
        return HAL_IOMMU_ERR_INVALID_PARAM;
    }
    
    /* Get Device Info from IORT using client_handle */
    if(E_SUCCESS != IORT_GetNamedComponentInfo((char *)client_handle, &named_component_info)) {
        return HAL_IOMMU_ERR_GET_IORT_INFO_FAILED;
    }
    
    /* Make sure CCA/Non-CCA devices only are added to domains with the same configuration */
    if((named_component_info.CacheCoherentAttr == 0x0 && ((p_domain_local->config.domain_cfg64.SCTLR & SCLTR_IO_COHERENCY_MASK) == SCLTR_IO_COHERENT_VAL)) || 
       (named_component_info.CacheCoherentAttr == 0x1 && ((p_domain_local->config.domain_cfg64.SCTLR & SCLTR_IO_COHERENCY_MASK) != SCLTR_IO_COHERENT_VAL))) {
        return HAL_IOMMU_ERR_DEVICE_INCOMPATIBLE_WITH_DOMAIN;
    }
    
    // allocate memory for SID mapping list using number of mappings
    p_id_mappings = (IORT_IDMapping *)AllocatePool(named_component_info.NumIDMappings * sizeof(IORT_IDMapping));
    if(!p_id_mappings) {
        return HAL_IOMMU_ERR_MALLOC_FAILED;
    }
    
    // get SID mapping list
    if(E_SUCCESS != IORT_GetDeviceSMMUIDMappings((char *)client_handle, 0, named_component_info.NumIDMappings, p_id_mappings)) {
        FreePool(p_id_mappings);
        return HAL_IOMMU_ERR_GET_IORT_INFO_FAILED;
    }
    
    // find mapping from arid
    for(i = 0; i < named_component_info.NumIDMappings; i++) {
        if(p_id_mappings[i].InputBase == arid) {
            p_arid_mapping = &p_id_mappings[i];
            break;
        }
    }
    
    if(!p_arid_mapping) {
        // Could not find ARID
        FreePool(p_id_mappings);
        return HAL_IOMMU_ERR_INVALID_PARAM;
    }
    
    // find iommu base address from mapping
    base_addr = p_arid_mapping->OutputSMMUBaseAddr;
    
    // get iommu architecture from base address
    if(E_SUCCESS != IORT_GetSMMUv1v2Info(base_addr, &smmu_info)) {
        FreePool(p_id_mappings);
        return HAL_IOMMU_ERR_GET_IORT_INFO_FAILED;
    }
    
    arch = (smmu_info.Model == IORT_MODEL_ARM_CORELINK_MMU_500) ? HAL_IOMMU_ARCH_MMU500 : HAL_IOMMU_ARCH_QSMMU;
    
    /* Add IOMMU to domain and acquire pointer to IOMMU object */
    ecode = add_iommu_to_domain(p_domain_local, (void*)base_addr, arch, &p_hal_iommu_obj);
    if(ecode != HAL_IOMMU_ERR_OK || !p_hal_iommu_obj) {
        FreePool(p_id_mappings);
        return ecode;
    }
    
    /* If CB has not been configured yet for this domain on this IOMMU, find a free one and configure it */
    if(p_hal_iommu_obj->cb_idx == HAL_IOMMU_INVALID_CB_IDX) {
        /* Configure IOMMU Context Bank for this Domain */
        if(p_domain_local->pt_type == HAL_IOMMU_ARM_ARCH_AARCH64) {
            // find free context bank for this IOMMU and populate cb_idx
            get_free_cb(p_hal_iommu_obj->base_addr, p_domain_local->pt_type, &p_hal_iommu_obj->cb_idx);
            
            if(p_hal_iommu_obj->cb_idx == HAL_IOMMU_INVALID_CB_IDX) {
                // No CB could be found
                FreePool(p_id_mappings);
                return HAL_IOMMU_ERR_OUT_OF_RANGE;
            }
            sctlr.raw = p_domain_local->config.domain_cfg64.SCTLR;
            tcr.raw = p_domain_local->config.domain_cfg64.TCR;
            HAL_IOMMU_SetSCTLR((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx, sctlr);
            HAL_IOMMU_SetTTBR0((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx, p_domain_local->config.domain_cfg64.TTBR0);
            HAL_IOMMU_SetTTBR1((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx, p_domain_local->config.domain_cfg64.TTBR1);
            HAL_IOMMU_SetMAIR0((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx, p_domain_local->config.domain_cfg64.MAIR0);
            HAL_IOMMU_SetMAIR1((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx, p_domain_local->config.domain_cfg64.MAIR1);
            HAL_IOMMU_SetTTBCR((uint32_t)p_hal_iommu_obj->base_addr, p_hal_iommu_obj->cb_idx, tcr);
        } else {
            // No support for AARCH32 at this time
            FreePool(p_id_mappings);
            return HAL_IOMMU_ERR_UNSUPPORTED_INSTANCE_FOR_TARGET;
        }
    } // end if(p_hal_iommu_obj->cb_idx == HAL_IOMMU_INVALID_CB_IDX)

    hal_iommu_memory_barrier();
    
    /* Add device handle + arid to the IOMMU object */
    ecode = add_device_to_iommu(p_hal_iommu_obj, client_handle, arid);
    if(ecode != HAL_IOMMU_ERR_OK) {
        // free id mappings buffer
        FreePool(p_id_mappings);
        // Redundant config is fine I suppose, just return success to client w/o doing anything.
        if(ecode == HAL_IOMMU_ERR_REDUNDANT_CONFIGURATION) {
            p_domain_local->ref_count++;
            return HAL_IOMMU_ERR_OK;
        } else {
            return ecode;
        }
    }
    
    /* Configure Stream IDs associated with ARID */
    // Construct stream ID from output base + num IDs 
    if(mask) {
        // TODO: Hack for now.  If they pass a mask assume they know better
        stream_id = p_arid_mapping->OutputBase | (mask << HAL_IOMMU_SID_MASK_SHIFT);
    } else {
        // TODO: How do we calculate mask???  If NumIDs not a power of 2 then we have a problem below
        stream_id = p_arid_mapping->OutputBase | ((p_arid_mapping->NumIDs-1) << HAL_IOMMU_SID_MASK_SHIFT);
    }
    /* Program SID into HW */
    ecode = program_smr_s2cr_to_cb(p_hal_iommu_obj, stream_id);
    if(ecode != HAL_IOMMU_ERR_OK) {
        //Programming all SIDs for ARID failed, unwind the process
        //remove_smr_s2cr_from_cb(p_hal_iommu_obj, stream_id);
        remove_device_from_iommu(p_hal_iommu_obj, client_handle, arid);
        
        // Free id mappings buffer
        FreePool(p_id_mappings);
        return ecode;
    }
    
    hal_iommu_memory_barrier();
    
    /* Increment domain ref count */
    p_domain_local->ref_count++;
    
    // Free id mappings buffer
    FreePool(p_id_mappings);
    
    return HAL_IOMMU_ERR_OK;
}

/** 
 * Function: hal_iommu_domain_detach_device
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns:
 */
HAL_IOMMUResult_t hal_iommu_domain_detach_device(void *p_domain, void *client_handle, uint32_t arid, uint32_t mask)
{
    IORT_NamedComponentInfo named_component_info;
    IORT_IDMapping *p_arid_mapping = NULL, *p_id_mappings = NULL;
    struct hal_iommu_domain_obj *p_domain_local = (struct hal_iommu_domain_obj *)p_domain;
    struct hal_iommu_list_obj *p_hal_iommu_obj = NULL;
    uint64_t base_addr;
    uint32_t stream_id;
    HAL_IOMMUResult_t ecode;
    int i;
    
    /* Input Validation */
    if(!p_domain || !client_handle) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    /* Get Device Info from IORT using client_handle */
    if(E_SUCCESS != IORT_GetNamedComponentInfo((char *)client_handle, &named_component_info)) {
        return HAL_IOMMU_ERR_GET_IORT_INFO_FAILED;
    }
    
    // allocate memory for SID mapping list using number of mappings
    p_id_mappings = (IORT_IDMapping *)AllocatePool(named_component_info.NumIDMappings * sizeof(IORT_IDMapping));
    if(!p_id_mappings) {
        return HAL_IOMMU_ERR_MALLOC_FAILED;
    }
    
    // get SID mapping list
    if(E_SUCCESS != IORT_GetDeviceSMMUIDMappings((char *)client_handle, 0, named_component_info.NumIDMappings, p_id_mappings)) {
        FreePool(p_id_mappings);
        return HAL_IOMMU_ERR_GET_IORT_INFO_FAILED;
    }
    
    // find mapping from arid
    for(i = 0; i < named_component_info.NumIDMappings; i++) {
        if(p_id_mappings[i].InputBase == arid) {
            p_arid_mapping = &p_id_mappings[i];
            break;
        }
    }
    
    if(!p_arid_mapping) {
        // Could not find ARID
        FreePool(p_id_mappings);
        return HAL_IOMMU_ERR_INVALID_PARAM;
    }
    
    // find iommu base address from mapping
    base_addr = p_arid_mapping->OutputSMMUBaseAddr;
    
    p_hal_iommu_obj = retrieve_hal_iommu_obj_from_domain(p_domain_local, (void *)base_addr);
    if(!p_hal_iommu_obj)
    {
        // The IOMMU object is not present in this domain, remove before add requested
        FreePool(p_id_mappings);
        return HAL_IOMMU_ERR_DEREGISTER_MMU_FAILED;
    }
    
    // remove ARID configuration from device list
    ecode = remove_device_from_iommu(p_hal_iommu_obj, client_handle, arid);
    if(ecode != HAL_IOMMU_ERR_OK) {
        // Free id mappings buffer
        FreePool(p_id_mappings);
        // Redundant config is fine I suppose, just return success to client w/o doing anything.
        if(ecode == HAL_IOMMU_ERR_REDUNDANT_CONFIGURATION) {
            p_domain_local->ref_count--;
            return HAL_IOMMU_ERR_OK;
        } else {
            return ecode;
        }
    }
    
    /* ***NOTE***: Technically we could check and see if there were any devices still attached to the 
                    IOMMU object and if there aren't any we can free the IOMMU object now.  However, there 
                    isn't really a need to do this since we already know the IOMMU exists, other clients may
                    attach their devices to it later.  So instead of freeing the IOMMU object only to possibly
                    allocate it again later, we will leave it be until the domain is deleated.  It will leave a 
                    context bank reserved for this domain on this IOMMU, but if we run out of context banks we can
                    update this code.
     */
    
    // Clear SMRs/S2CRs corresponding to this IOMMU for this domain
    // Construct Stream ID from OutputBase + NumIDs 
    if(mask) {
        // TODO: Hack for now.  If they pass a mask assume they know better
        stream_id = p_arid_mapping->OutputBase | (mask << HAL_IOMMU_SID_MASK_SHIFT);
    } else {
        // TODO: How do we calculate mask???  If NumIDs not a power of 2 then we have a problem below
        stream_id = p_arid_mapping->OutputBase | ((p_arid_mapping->NumIDs-1) << HAL_IOMMU_SID_MASK_SHIFT);
    }
    ecode = remove_smr_s2cr_from_cb(p_hal_iommu_obj, stream_id);
    if(ecode != HAL_IOMMU_ERR_OK) {
        // free id mappings buffer
        FreePool(p_id_mappings);
        return ecode;
    }
    
    hal_iommu_memory_barrier();
    
    /* Decrement ref count */
    p_domain_local->ref_count--;
    
    // free id mappings buffer
    FreePool(p_id_mappings);
    
    return HAL_IOMMU_ERR_OK;
}

/** 
 * Function: hal_iommu_domain_configure
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns:
 */
HAL_IOMMUResult_t hal_iommu_domain_configure(void *p_domain, struct hal_iommu_domain_config *p_domain_cfg)
{
    struct hal_iommu_domain_obj *p_domain_local = (struct hal_iommu_domain_obj *)p_domain;
    
    /* Input Validation */
    if(!p_domain || !p_domain_cfg) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    /* Populate domain members */
    p_domain_local->pt_type = p_domain_cfg->vmsa;
    if(p_domain_cfg->vmsa == HAL_IOMMU_ARM_ARCH_AARCH64) {
        p_domain_local->config.domain_cfg64.TTBR0 = p_domain_cfg->config.domain_cfg64.TTBR0;
        p_domain_local->config.domain_cfg64.TTBR1 = p_domain_cfg->config.domain_cfg64.TTBR1;
        p_domain_local->config.domain_cfg64.MAIR0 = p_domain_cfg->config.domain_cfg64.MAIR0;
        p_domain_local->config.domain_cfg64.MAIR1 = p_domain_cfg->config.domain_cfg64.MAIR1;
        p_domain_local->config.domain_cfg64.SCTLR = p_domain_cfg->config.domain_cfg64.SCTLR;
        p_domain_local->config.domain_cfg64.TCR = p_domain_cfg->config.domain_cfg64.TCR;
    }
    else {
        // AARCH32 not supported at this time
        return HAL_IOMMU_ERR_UNSUPPORTED_INSTANCE_FOR_TARGET;
    }
    
    /* If domain has already been attached (has non-null device list) update configuration for those IOMMUs */
    update_hal_iommu_configuration(p_domain_local);
    
    return HAL_IOMMU_ERR_OK;
}

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
HAL_IOMMUResult_t hal_iommu_configure_bypass_domain(void *p_domain, enum hal_iommu_bypass_domain_type type)
{
    struct hal_iommu_domain_obj *p_domain_local = (struct hal_iommu_domain_obj *)p_domain;
    
    /* Input Validation */
    if(!p_domain || type >= HAL_IOMMU_DOMAIN_TYPE_INVALID) {
        return HAL_IOMMU_ERR_INVALID_PARAM;
    }
    
    /* Populate domain members */
    p_domain_local->config.domain_cfg64.TTBR0 = 0;
    p_domain_local->config.domain_cfg64.TTBR1 = 0;
    p_domain_local->config.domain_cfg64.MAIR0 = 0;
    p_domain_local->config.domain_cfg64.MAIR1 = 0;
    
    switch(type) {
        case HAL_IOMMU_AARCH64_NON_CCA_DOMAIN_TYPE:
            p_domain_local->pt_type = HAL_IOMMU_ARM_ARCH_AARCH64;
            p_domain_local->config.domain_cfg64.SCTLR = HAL_IOMMU_BYPASS_NON_CCA_SCTLR_VAL;
            p_domain_local->config.domain_cfg64.TCR = HAL_IOMMU_BYPASS_NON_CCA_TCR_VAL;
            break;
        case HAL_IOMMU_AARCH64_CCA_DOMAIN_TYPE:
            p_domain_local->pt_type = HAL_IOMMU_ARM_ARCH_AARCH64;
            p_domain_local->config.domain_cfg64.SCTLR = HAL_IOMMU_BYPASS_CCA_SCTLR_VAL;
            p_domain_local->config.domain_cfg64.TCR = HAL_IOMMU_BYPASS_CCA_TCR_VAL;
            break;
        default:
            return HAL_IOMMU_ERR_OUT_OF_RANGE;
    }
    
    /* If domain has already been attached (has non-null device list) update configuration for those IOMMUs */
    update_hal_iommu_configuration(p_domain_local);
    
    return HAL_IOMMU_ERR_OK;
}

/** 
 * Function: hal_iommu_flush_domain
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns:
 */
HAL_IOMMUResult_t hal_iommu_flush_domain(void *p_domain)
{
    struct hal_iommu_domain_obj *p_domain_local = (struct hal_iommu_domain_obj *)p_domain;
    struct hal_iommu_list_obj *p_hal_iommu_obj;
    
    /* Input Validation */
    if(!p_domain) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    p_hal_iommu_obj = p_domain_local->hal_iommu_list_head;
    
    while(p_hal_iommu_obj) {
        hal_iommu_invalidate_tlb_all(p_hal_iommu_obj);
        p_hal_iommu_obj = p_hal_iommu_obj->next;
    }
    
    hal_iommu_memory_barrier();
    
    return HAL_IOMMU_ERR_OK;
}

/** 
 * Function: hal_iommu_flush_domain_va_list
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns:
 */
HAL_IOMMUResult_t hal_iommu_flush_domain_va_list(void *p_domain, boolean last_level, uint32_t number, uint64_t *va_list)
{
    struct hal_iommu_domain_obj *p_domain_local = (struct hal_iommu_domain_obj *)p_domain;
    struct hal_iommu_list_obj *p_hal_iommu_obj;
    
    /* Input Validation */
    if(!p_domain) {
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
    
    p_hal_iommu_obj = p_domain_local->hal_iommu_list_head;
    
    while(p_hal_iommu_obj) {
        hal_iommu_invalidate_tlb_va_list(p_hal_iommu_obj, number, va_list);
        p_hal_iommu_obj = p_hal_iommu_obj->next;
    }
    
    hal_iommu_memory_barrier();
    
    return HAL_IOMMU_ERR_OK;
}

/** 
 * Function: hal_iommu_register_fault_handler
 *
 * Description:
 *
 * Input:
 *
 * Output:
 *
 * Returns:
 */
HAL_IOMMUResult_t hal_iommu_register_fault_handler(void *p_domain, fp_hal_iommu_fault_handler_t handler, void *p_context)
{
    //struct hal_iommu_domain_obj *p_domain_local = (struct hal_iommu_domain_obj *)p_domain;
    
    // Not Implemented in UEFI
    
    return HAL_IOMMU_ERR_OK;
}


HAL_IOMMUResult_t IDIOM_Detach(void *client_handle, uint32_t arid, uint32_t mask)
{
	struct hal_iommu_domain_obj *p_domain_local ;
	void *domain_handle;
	HAL_IOMMUResult_t ret = HAL_IOMMU_ERR_OK;
	/* Input Validation */
    if(!client_handle) 
	{
        return HAL_IOMMU_ERR_NULL_POINTER;
    }
	
 	//Get the domain for the said device
	ret = IDIOM_domain_lookup(&domain_handle, client_handle);
	if(ret != HAL_IOMMU_ERR_OK)
	{
		return ret;
	}
	
	p_domain_local = (struct hal_iommu_domain_obj *)domain_handle;
	/*Detach the device first from the domain*/
	ret = hal_iommu_domain_detach_device(p_domain_local, client_handle, arid, mask);
	if(ret != HAL_IOMMU_ERR_OK)
	{
		return ret;
	}
	
	/*Delete the domain if it has no more devices*/
	if(p_domain_local->hal_iommu_list_head == NULL)
	{
		ret = hal_iommu_domain_delete(p_domain_local);	
		if(ret != HAL_IOMMU_ERR_OK)
		{
			return ret;
		}
	}
	
	return ret;	
}

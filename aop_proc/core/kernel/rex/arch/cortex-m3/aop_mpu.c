/*==============================================================================

FILE:      aop_mpu.c

DESCRIPTION: This file implements AOP MPU implementations and configurations as follows

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential


$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/arch/cortex-m3/aop_mpu.c#1 $
$Date: 2018/02/16 $

==============================================================================*/

/*==============================================================================
**
**      Include Files
**
**==============================================================================
*/

#include "cortex-m3.h"
#include "aop_mpu.h"
#include "CoreVerify.h"


/*==============================================================================
**
**      Variables
**
**==============================================================================
*/
static unsigned aop_mpu_num_supported_regions = 0;


/*==============================================================================
**
**      Function Definitions
**
**==============================================================================
*/
/**
 *
 * <!-- aop_mpu_memory_barrier -->
 *
 * @brief ensures all subsequence data transfers before this complete and
 *        instructions after this are fetched from cache or memory.
 *
 *  NOTE:
 *  - The barrier intrinsics do not implicitly act as a sequence point.
 *    It is necessary to insert __schedule_barrier intrinsics either side of the
 *    memory barrier intrinsic. It creates a special sequence point that prevents
 *    operations with side effects from moving past it under all circumstances.
 *
 *  - http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka14552.html
 *
 * @param void
 *
 * @return void
 */
static void aop_mpu_memory_barrier(void)
{
    __schedule_barrier();

    /* Force Memory Writes before continuing */
    __dsb(0xf);

    __schedule_barrier();

    /* Flush and refill pipline */
    __isb(0xf);

    __schedule_barrier();
}

/**
 * <!-- aop_mpu_enable -->
 *
 * @brief Enables/disables MPU.
 *
 * @param enable - TRUE to enable MPU, FALSE to disable MPU
 *
 * @return void
 */
void aop_mpu_enable(bool enable)
{
    if(enable)
    {
        SCS.MPU.Ctrl = (AOP_MPU_ENABLE_BG_RGN<<2) | AOP_MPU_ENABLE;

        /*
         * ensure subsequent memory accesses use updated MPU settings.
         */
        aop_mpu_memory_barrier();
    }
    else
    {
        /*
         * ensure any data transfers complete before disabling MPU
         */
        aop_mpu_memory_barrier();

        SCS.MPU.Ctrl = 0;
    }
}

/**
 * <!-- aop_mpu_setup_region -->
 *
 * @breif sets up the base address, size, and attributes of the specified MPU region 
 *        disables the region if its configuration is not defined in aop_mpu_cfg[].
 *
 * @param rgn_num - an MPU region number
 *
 * @return void
 */
static void aop_mpu_setup_region(int rgn_num)
{
    if(rgn_num < AOP_MPU_RGN_COUNT)
    {
        CORE_VERIFY(rgn_num == aop_mpu_cfg[rgn_num].rgn_id);

        SCS.MPU.RegionNumber   = rgn_num;
        SCS.MPU.RegionBaseAddr = aop_mpu_cfg[rgn_num].base_addr;
        SCS.MPU.RegionAttrSize = (aop_mpu_cfg[rgn_num].attr_size.execute_disable<<28) |
                                 (aop_mpu_cfg[rgn_num].attr_size.permission<<24) |
                                 (AOP_MPU_RGN_ATTR_SCB<<16) |
                                 (aop_mpu_cfg[rgn_num].attr_size.sub_rgn_disable<<8) |
                                 (aop_mpu_cfg[rgn_num].attr_size.size<<1) |
                                 (aop_mpu_cfg[rgn_num].attr_size.enable);
    }
    else
    {
        SCS.MPU.RegionNumber   = rgn_num;
        SCS.MPU.RegionBaseAddr = 0;
        SCS.MPU.RegionAttrSize = 0;
    }
}

/**
 * <!-- aop_mpu_enable_region -->
 *
 * @brief enables/disables memory protection for the specified region.
 *
 * @param region - an MPU region name enum
 * @param enable - TRUE to enable protection for the region, FALSE to disable
 *
 * @return void
 */
void aop_mpu_enable_region(aop_mpu_rgn_num_t region, bool enable)
{
    CORE_VERIFY(region < AOP_MPU_RGN_COUNT);

    aop_mpu_cfg[region].attr_size.enable = enable;

    aop_mpu_enable(FALSE);

    aop_mpu_setup_region(region);

    aop_mpu_enable(TRUE);
}

/**
 * <!-- aop_mpu_enable_psp_stack_guard -->
 *
 * @brief Enables/disables memory protection for the specified base address of PSP stack.
 *
 * @param enable - TRUE to enable protection for the region, FALSE to disable.
 * @param base_addr - 32-byte aligned base address of PSP stack.
 *
 * @return void
 */
void aop_mpu_enable_psp_stack_guard(bool enable, unsigned base_addr)
{
    /*
    ** Region base address must be a multiple of its size
    */
    CORE_VERIFY((base_addr % AOP_MPU_STACK_GUARD_SIZE) == 0);

    aop_mpu_cfg[AOP_MPU_RGN_PSP_STACK_GUARD].base_addr = base_addr;

    aop_mpu_enable_region(AOP_MPU_RGN_PSP_STACK_GUARD, enable);
}

/**
 * <!-- aop_mpu_get_num_supported_regions -->
 *
 * @brief gets the number of supported MPU regions. 0 if MPU is not present.
 *
 * @param void
 *
 * @return the number of supported MPU regions
 */
//__attribute__((section("mpu_dram_reclaim_pool")))
static unsigned aop_mpu_get_num_supported_regions(void)
{
    return 0xff & (SCS.MPU.Type >> 8);
}

/**
 * <!-- aop_mpu_init -->
 *
 * @brief initializes MPU and enables it.
 *
 * @param void
 *
 * @return void
 */
//__attribute__((section("mpu_dram_reclaim_pool")))
void aop_mpu_init(void)
{
    int i;

    aop_mpu_num_supported_regions = aop_mpu_get_num_supported_regions();

    CORE_VERIFY(aop_mpu_num_supported_regions);
    CORE_VERIFY(AOP_MPU_RGN_COUNT <= aop_mpu_num_supported_regions);

    aop_mpu_cfg_init();

    aop_mpu_enable(FALSE);

    for(i = 0; i < aop_mpu_num_supported_regions; i++)
    {
        aop_mpu_setup_region(i);
    }

    aop_mpu_enable(TRUE);
}


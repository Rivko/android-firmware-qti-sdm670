#include "cortex-m3.h"
#include "aop_mpu.h"
#include "CoreVerify.h"


extern char __aop_coderam_base[];
extern char __aop_coderam_limit[];
extern char __aop_heap_base[];
extern char __aop_heap_limit[];
extern char __aop_stack_base[];
extern char __aop_stack_limit[];


/*
** NOTE:
** - Region start address must be a multiple of its size
** - Regions can overlap other regions
** - Region 7 has highest priority, region 0 has lowest priority
** - Attributes of overlapping regions will be set to the attributes of
**   the higher priority region (if overlapping regions enabled)
*/
static struct aop_mpu_rgn_cfg_s aop_mpu_cfg_data[AOP_MPU_RGN_COUNT] =
{
    /*
     * Set the first 32 bytes of AOP CODE_RAM to read-only and executable in order to detect null pointer dereference.
     */
    [AOP_MPU_RGN_CODE_RAM] =
    {
        .rgn_id    = AOP_MPU_RGN_CODE_RAM,
        .attr_size =
        {
            .enable          = TRUE,
            /* .size is optional since it's populated in set_base_addr_and_size() */
            .permission      = AOP_MPU_RGN_AP_RO,
            .execute_disable = FALSE,
        }
    },

    /*
     * Set the permission for the top 32 bytes of MSP Stack to read-only in order to detect stack overflow.
     * MSP is used dy main() and ISR.
     */
    [AOP_MPU_RGN_MSP_STACK_GUARD] =
    {
        .rgn_id    = AOP_MPU_RGN_MSP_STACK_GUARD,
        .attr_size =
        {
            .enable          = TRUE,
            /* .size is optional since it's populated in set_base_addr_and_size() */
            .permission      = AOP_MPU_RGN_AP_RO,
            .execute_disable = TRUE,
        }
    },

    /*
     * Set the permission for the top 32 bytes of PSP Stack to read-only in order to detect stack overflow.
     * PSP is used by user tasks. This region is updated for task's stack when context switch happens 
     */
    [AOP_MPU_RGN_PSP_STACK_GUARD] =
    {
        .rgn_id    = AOP_MPU_RGN_PSP_STACK_GUARD,
        .attr_size =
        {
            .enable          = FALSE,   /* Enabled when context switch happens */
            .size            = AOP_MPU_RGN_SIZE_32B, /* set .size here so that we don't need to set .size in aop_mpu_enable_psp_stack_guard() */
            .permission      = AOP_MPU_RGN_AP_RO,
            .execute_disable = TRUE,
        }
    },
};

struct aop_mpu_rgn_cfg_s * const aop_mpu_cfg = aop_mpu_cfg_data;


/**
 * <!-- set_base_addr_and_size -->
 *
 * @brief Sets the region size based on the size in bytes.
 *
 *  Sets a base address aligned to be a multiple of region size.
 *  disables sub-regions which are outside the given region if region size > 128B.
 *
 *  NOTE:
 *  - It uses the fact that SIZE is the exponent of power of 2 minus 1.
 *
 *       (Region size in bytes) = 2^(SIZE+1)
 *
 *    e.g. if size is 7500 bytes, 7500 is < 2^13 and SIZE is 13 - 1 = 12, which is AOP_MPU_RGN_SIZE_8KB.
 *
 *  - The region size must be >= 32B and <= 4GB.
 *  - http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/Cihegaib.html
 *
 * @param rgn_num             - region number
 * @param base_addr           - initial base address
 * @param end_addr            - size of region in bytes
 * @param base_addr_inclusive - TRUE to ensure the sub-region containing the give base_addr is enabled.
 *                              FALSE to disable all sub-regions which base address is lower than the given base_addr 
 * @param end_addr_inclusive  - TRUE to ensure the sub-region containing the give end_addr is enabled.
 *                              FALSE to disable all sub-regions which end address is higher than the given end_addr 
 *
 * @return void
 */
//__attribute__((section("mpu_dram_reclaim_pool")))
static void set_base_addr_and_size(aop_mpu_rgn_num_t rgn_num, unsigned base_addr, unsigned end_addr, bool base_addr_inclusive, bool end_addr_inclusive)
{
    unsigned size_bytes = end_addr - base_addr;
    unsigned exponent = 31 - __clz(size_bytes);
    unsigned rgn_size_type = 0;
    unsigned aligned_base_addr;
    unsigned aligned_size_bytes;

    CORE_VERIFY((base_addr < end_addr) && (size_bytes >= 32));

    if(size_bytes & ((1<<exponent)-1))
    {
        exponent += 1;
    }

    /*
     * find supported region size and base address which covers the given range
     * NOTE:
     * - The region size MUST be a power of 2
     * - The base address MUST be a multiple of the region size.
     */
    while(TRUE)
    {
        aligned_size_bytes = (1 << exponent);
        aligned_base_addr  = base_addr & ~(aligned_size_bytes - 1);

        if(aligned_base_addr + aligned_size_bytes >= end_addr)
        {
            break;
        }

        CORE_VERIFY(exponent <= AOP_MPU_RGN_SIZE_4GB);

        exponent++;
    }

    CORE_VERIFY((aligned_base_addr % aligned_size_bytes) == 0);

    rgn_size_type = exponent - 1;

    CORE_VERIFY((rgn_size_type >= AOP_MPU_RGN_SIZE_32B) && (rgn_size_type <= AOP_MPU_RGN_SIZE_4GB));

    aop_mpu_cfg[rgn_num].base_addr      = aligned_base_addr;
    aop_mpu_cfg[rgn_num].attr_size.size = (aop_mpu_rgn_size_t)rgn_size_type;

    /*
     * Disable sub-regoins which are outside the given range
     * The sub-region disable bits are Unpredictable for region sizes of 32 bytes, 64 bytes, and 128 bytes.
     */
    if(rgn_size_type > AOP_MPU_RGN_SIZE_128B)
    {
        unsigned sub_rgn_size = aligned_size_bytes / AOP_MPU_NUM_SUB_RGN;
        unsigned sub_rgn_base_addr;
        unsigned sub_rgn_end_addr;
        int sub_rgn_num;

        /*
         * disable sub-regions at lower address than the given base_addr
         */
        sub_rgn_num       = 0;
        sub_rgn_base_addr = aligned_base_addr;

        while((sub_rgn_base_addr < base_addr) && (sub_rgn_num < AOP_MPU_NUM_SUB_RGN))
        {
            if((!base_addr_inclusive) || (sub_rgn_base_addr + sub_rgn_size <= base_addr))
            {
                aop_mpu_cfg[rgn_num].attr_size.sub_rgn_disable |= (1 << sub_rgn_num);
            }

            sub_rgn_base_addr += sub_rgn_size;
            sub_rgn_num++;
        }

        /*
         * disable sub-regions at higher address than the end address
         */
        sub_rgn_num      = AOP_MPU_NUM_SUB_RGN - 1;
        sub_rgn_end_addr = aligned_base_addr + aligned_size_bytes;

        while((sub_rgn_end_addr > end_addr) && (sub_rgn_num >= 0))
        {
            if((!end_addr_inclusive) || (sub_rgn_end_addr - sub_rgn_size >= end_addr))
            {
                aop_mpu_cfg[rgn_num].attr_size.sub_rgn_disable |= (1 << sub_rgn_num);
            }

            sub_rgn_end_addr -= sub_rgn_size;
            sub_rgn_num--;
        }

        /*
         * abort if all sub-regions were disabled
         */
        CORE_VERIFY(aop_mpu_cfg[rgn_num].attr_size.sub_rgn_disable < 0xFF);
    }
}

/**
 * <!-- get_effective_base_addr -->
 *
 * @brief Returns the base address of the given region from which the region permission is
 *        taken effect.
 *
 * @param rgn_num - region number
 *
 * @return The effective base address of the given region
 */
//__attribute__((section("mpu_dram_reclaim_pool")))
static unsigned get_effective_base_addr(aop_mpu_rgn_num_t rgn_num)
{
    unsigned base_addr          = aop_mpu_cfg[rgn_num].base_addr;
    unsigned rgn_size_bytes     = (1 << (aop_mpu_cfg[rgn_num].attr_size.size + 1));
    unsigned sub_rgn_size_bytes = rgn_size_bytes / AOP_MPU_NUM_SUB_RGN;
    unsigned sub_rgn_disable    = aop_mpu_cfg[rgn_num].attr_size.sub_rgn_disable;

    /*
     * find the base address of the lowest sub region which is not disabled
     */
    while(sub_rgn_disable & 0x1)
    {
        base_addr += sub_rgn_size_bytes;
        sub_rgn_disable >>= 1;
    }

    return base_addr;
}

/**
 * <!-- aop_mpu_cfg_init -->
 *
 * @brief Sets up the configuration for MPU regions.
 *
 *  This is used to set the base address and size of the configuration for MPU regions.
 *  This does not program and configure MPU.
 *
 * @param void
 *
 * @return void
 */
//__attribute__((section("mpu_dram_reclaim_pool")))
void aop_mpu_cfg_init(void)
{
    /*
     * Set the first 32 byte of CODE RAM to detect null-pointer dereference.
     */
    set_base_addr_and_size(AOP_MPU_RGN_CODE_RAM, (unsigned)__aop_coderam_base, (unsigned)__aop_coderam_base+32, TRUE, TRUE);

    /*
     * Set MSP stack guard. The PSP stack guard is set when context switch happens.
     */
    set_base_addr_and_size(AOP_MPU_RGN_MSP_STACK_GUARD, (unsigned)__aop_stack_base-32, (unsigned)__aop_stack_base, TRUE, TRUE);
}


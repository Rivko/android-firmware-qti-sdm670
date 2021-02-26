/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_hwio.c
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#include "msmhwiobase.h" // for LPASS_BASE_PHYS
#include "dangergen_hwio.h"
#include "hwio_lpass_qos.h" // auto-generated HWIO header
#include "hwio_lpass_tcsr.h" // auto-generated HWIO header
#include "hwio_lpass_qos_generic.h" // auto-generated HWIO header
#include "DalDevice.h"
#include "DDIHWIO.h"
#include "HALhwio.h"
#include "DALDeviceId.h"
#include "DDIChipInfo.h"
#include "CoreVerify.h"

// Offset between register sets for the individual generators
#define DANGERGEN_HW_INSTANCE_OFFSET \
    (HWIO_LPASS_QOS_CNT1_MONITOR_OFFS - HWIO_LPASS_QOS_CNT0_MONITOR_OFFS)

// LPASS_BASE will be defined in msmhwiobase.h but we want to redefine it, so
// that the address will be resolved dynamically based on the generator's
// index
#if defined(LPASS_BASE)
#undef LPASS_BASE
#endif

#define LPASS_BASE gDangerHwio.lpass_base_virt

// Convenience macro for writing to a specific field of a register
#define DANGERGEN_HWIO_OUTF(reg, field, val) \
    (HWIO_OUTM(reg, HWIO_FMSK(reg, field), (val) << HWIO_SHFT(reg, field)))

// Convenience macro for writing to a specific field of a register, with
// automatic indexing
#define DANGERGEN_HWIO_OUTFn(reg, field, val) \
    out_dword_masked_ns( \
        HWIO_##reg##_ADDR + (generator->index * DANGERGEN_HW_INSTANCE_OFFSET), \
        HWIO_FMSK(reg, field), (val) << HWIO_SHFT(reg, field), \
        HWIO_##reg##_IN)

// Convenience macro for writing to a register, with automatic indexing
#define DANGERGEN_HWIO_OUTn(reg, val) \
    out_dword( \
        HWIO_##reg##_ADDR + (generator->index * DANGERGEN_HW_INSTANCE_OFFSET), \
        val)

// Counter frequency in kHz
#define COUNTER_FREQ_KHZ 19200

// Convert from nanoseconds to counter cycles
#define NS_TO_CYCLES(ns) \
    (uint32)((uint64)(ns) * COUNTER_FREQ_KHZ / 1000000)

//Pre-processor to get Danger HW version
#define DANGER_HW_MAJOR_VERSION       (HWIO_LPASS_QOS_HW_VERSION_INM(HWIO_LPASS_QOS_HW_VERSION_MAJOR_BMSK)\
                                                           >> HWIO_LPASS_QOS_HW_VERSION_MAJOR_SHFT)



typedef struct
{
    // Virtual address corresponding to physical address LPASS_BASE_PHYS
    uint8* lpass_base_virt;
    DalChipInfoFamilyType chip_family;
    DalChipInfoVersionType chip_version;
    bool skip_qos_enable;
    bool skip_qos_noc;
} dangergen_hwio_type;

static dangergen_hwio_type gDangerHwio;

// Sets the LPASS danger output to be sourced from the counter-based generator
static void dangergen_hwio_set_thread_enable(uint32 index)
{
    switch(index)
    {
        case 0:
            DANGERGEN_HWIO_OUTF(LPASS_QOS_DANGER_THREAD_ENABLE, THREAD0, 0x2);
            break;
        case 1:
            DANGERGEN_HWIO_OUTF(LPASS_QOS_DANGER_THREAD_ENABLE, THREAD1, 0x2);
            break;
        case 2:
            DANGERGEN_HWIO_OUTF(LPASS_QOS_DANGER_THREAD_ENABLE, THREAD2, 0x2);
            break;
        case 3:
            DANGERGEN_HWIO_OUTF(LPASS_QOS_DANGER_THREAD_ENABLE, THREAD3, 0x2);
            break;
        default:
            break;
    }
}

void dangergen_hwio_initialize_module(void)
{

    dangergen_hwio_type* self = &gDangerHwio;

    memset(self, 0, sizeof(dangergen_hwio_type));

    DalDeviceHandle* hHwio = NULL;

    // Map HWIO base and store vitual address
    CORE_DAL_VERIFY(DAL_DeviceAttach(DALDEVICEID_HWIO, &hHwio));
    if(DAL_SUCCESS != DalHWIO_MapRegionByAddress(
        hHwio, (uint8*) LPASS_BASE_PHYS, &self->lpass_base_virt))
    {
        // ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "Cannot map HWIO base");
    }
    CORE_DAL_VERIFY(DAL_DeviceDetach(hHwio));

    self->chip_family = DalChipInfo_ChipFamily();
    self->chip_version = DalChipInfo_ChipVersion();

    // In case we're runing on 8996 v1, skip the setting of QOS_ENABLE in
    // LPASS_TCSR_QOS_CTL because that register is not present on v1
    self->skip_qos_enable =
        (self->chip_family == DALCHIPINFO_FAMILY_MSM8996) &&
        (self->chip_version < DALCHIPINFO_VERSION(2, 0));
    // Also skip setting of DANGER bit in LPASS_QOS_NOC_CGC on 8996 v1
    self->skip_qos_noc =
        (self->chip_family == DALCHIPINFO_FAMILY_MSM8996) &&
        (self->chip_version < DALCHIPINFO_VERSION(2, 0));

}

void dangergen_hwio_initialize(dangergen_generator_type* generator)
{

    // Reset all registers to POR state
    // HW accesses below will be indexed using generator->index
    DANGERGEN_HWIO_OUTn(LPASS_QOS_CNT0_MONITOR,
                        HWIO_LPASS_QOS_CNT0_MONITOR_POR);
    DANGERGEN_HWIO_OUTn(LPASS_QOS_CNT0_COUNTER,
                        HWIO_LPASS_QOS_CNT0_COUNTER_POR);
    DANGERGEN_HWIO_OUTn(LPASS_QOS_CNT0_WINDOW,
                        HWIO_LPASS_QOS_CNT0_WINDOW_POR);
    DANGERGEN_HWIO_OUTn(LPASS_QOS_CNT0_THRESHOLD0,
                        HWIO_LPASS_QOS_CNT0_THRESHOLD0_POR);
    DANGERGEN_HWIO_OUTn(LPASS_QOS_CNT0_THRESHOLD1,
                        HWIO_LPASS_QOS_CNT0_THRESHOLD1_POR);
    DANGERGEN_HWIO_OUTn(LPASS_QOS_CNT0_THRESHOLD2,
                        HWIO_LPASS_QOS_CNT0_THRESHOLD2_POR);

    // Danger levels 1, 2, 3 set to level 2
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_MONITOR, DEF_DANGER_LEVEL_1, 2);
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_MONITOR, DEF_DANGER_LEVEL_2, 2);
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_MONITOR, DEF_DANGER_LEVEL_3, 2);

}


void dangergen_hwio_interrupt_configure(dangergen_generator_type* generator)
{
   dangergen_hwio_set_thread_enable(generator->index);
   DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_IRQ_SELECT, SEL, \
                        generator->interrupt_id);
}

void dangergen_hwio_switch_mode(dangergen_generator_type* generator, dangergen_generator_enable_mode mode)
{
   dangergen_hwio_set_thread_enable(generator->index);
   DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_MONITOR, HW_MODE, \
                        mode);
}

void dangergen_hwio_configure(dangergen_generator_type* generator)
{

    dangergen_hwio_set_thread_enable(generator->index);

    // HW accesses below will be indexed using generator->index
    // HWIO macros for threshold register access are not correct according to
    // what HW expects.  Right-shift of 8 applied to threshold values to ensure
    // register is written correctly.
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_COUNTER, CNT_SUB_CONST, \
        NS_TO_CYCLES(generator->period_ns));
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_WINDOW, WINDOW, \
        NS_TO_CYCLES(generator->window_ns));
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_THRESHOLD0, THRESHOLD0, \
        (NS_TO_CYCLES(generator->threshold1_ns) >> 8));
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_THRESHOLD1, THRESHOLD1, \
        (NS_TO_CYCLES(generator->threshold2_ns) >> 8));
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_THRESHOLD2, THRESHOLD2, \
        (NS_TO_CYCLES(generator->threshold3_ns) >> 8));

}

void dangergen_hwio_enable(dangergen_generator_type* generator)
{

    // HW accesses below will be indexed using generator->index
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_MONITOR, MONITOR_EN, 1);

}

void dangergen_hwio_disable(dangergen_generator_type* generator)
{

    // HW accesses below will be indexed using generator->index
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_MONITOR, MONITOR_EN, 0);

}

void dangergen_hwio_subtract(dangergen_generator_type* generator)
{

    // HW accesses below will be indexed using generator->index
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_COUNTER, CNT_SUB_MODE, 1);
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_COUNTER, CNT_RESET, 1);

}

void dangergen_hwio_reset(dangergen_generator_type* generator)
{

    // HW accesses below will be indexed using generator->index
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_COUNTER, CNT_SUB_MODE, 0);
    DANGERGEN_HWIO_OUTFn(LPASS_QOS_CNT0_COUNTER, CNT_RESET, 1);

}

void dangergen_hwio_set_qos_ctl(bool enabled)
{	
#ifndef QOS_SKIP_ENABLE
    dangergen_hwio_type* self = &gDangerHwio;

    if(!self->skip_qos_enable)
    {
        DANGERGEN_HWIO_OUTF(LPASS_TCSR_QOS_CTL, QOS_ENABLE, (enabled ? 1 : 0));
    }
#endif
}
void dangergen_hwio_set_qos_noc(bool enabled)
{

    dangergen_hwio_type* self = &gDangerHwio;

    if(!self->skip_qos_noc)
    {
        DANGERGEN_HWIO_OUTF(LPASS_QOS_NOC_CGC, DANGER, (enabled ? 1 : 0));
    }

}

bool  dangergen_hwio_interrupt_select_mode_status (uint32 ver) 
{
   return ((DANGER_HW_MAJOR_VERSION >= ver)? TRUE:FALSE);
}

bool  dangergen_hwio_hw_enable_mode_status (uint32 ver) 
{
   return ((DANGER_HW_MAJOR_VERSION >= ver)? TRUE:FALSE);
}


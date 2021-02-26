/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================*/

/*================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
10/09/16   anunays     First version, includes decoding of Parameter entried and PRFS bands        Version-1
================================================================================*/

#include "ddrss.h"
#include "ddr_phy.h"
#include "mc.h"
#include "target_config.h"
#include "ddr_phy_seq_hwioreg.h"

// Entry type
#define ENTRY_TYPE_MASK 0xC000
#define ENTRY_TYPE_SHIFT 14

// Entry types
#define TERMINATE 0
#define TARGET 1
#define PARAMETER 2
#define TMRS 3

// Entry subtype
#define SUBTYPE_MASK 0x3000
#define SUBTYPE_SHIFT 12

// Terminate entry subtypes
#define TERM 0
#define QUIT 1

// Quit and/or clear entry
#define QUIT_BIT_MASK 0x0800
#define TARGET_RESTRICTIONS_CLEAR_MASK 0x07FF
#define MANUFACTURER_ID_BIT 0x001
#define REVISION_ID_1_BIT 0x002
#define REVISION_ID_2_BIT 0x004
#define DENSITY_BIT 0x008
#define LOW_FREQ_BIT 0x010
#define HIGH_FREQ_BIT 0x020
#define IO_WIDTH_BIT 0x040
#define REVISION_ID_1_RANK_1_BIT 0x080
#define REVISION_ID_2_RANK_1_BIT 0x100
#define DENSITY_RANK_1_BIT 0x200
#define IO_WIDTH_RANK_1_BIT 0x400

// Target entry subtypes
#define MODE_REGISTER 0
#define FREQUENCY_MASK 1

// Mode register entry
#define MODE_SUBTYPE_MASK 0x0F00
#define MODE_SUBTYPE_SHIFT 8
#define MODE_VALUE_MASK 0x00FF
#define MANUFACTURER_ID 0
#define REVISION_ID_1 1
#define REVISION_ID_2 2
#define DENSITY 3
#define IO_WIDTH 4
#define REVISION_ID_1_RANK_1 5
#define REVISION_ID_2_RANK_1 6
#define DENSITY_RANK_1 7
#define IO_WIDTH_RANK_1 8

// Frequency mask entry
#define FREQ_MASK_SUBTYPE_MASK 0x0C00
#define FREQ_MASK_SUBTYPE_SHIFT 10
#define LOW_FREQ 0
#define HIGH_FREQ 3
#define HIGH_FREQ_MASK 0x007F
#define HIGH_FREQ_SHIFT 8
#define LOW_FREQ_MASK 0x00FF
#define CLEAR_BIT_MASK 0x0200

// Parameter entry
#define PARAMETER_TYPE_MASK 0x2000
#define PARAMETER_TYPE_SHIFT 13
#define MSM_PARAMETER 0
#define DRAM_PARAMETER 1
#define param_index_MASK 0x2F00
#define param_index_SHIFT 8
#define PARAMETER_VALUE_MASK 0x00FF
#define CA_PU       0
#define CA_PD       1
#define CA_VOH      2
#define CK_PU       4
#define CK_PD       5
#define CK_VOH      6
#define DQ_PU       8
#define DQ_PD       9
#define DQ_VOH      10
#define DQ_ODT      11
#define DQS_PU      12
#define DQS_PD      13
#define DQS_VOH     14
#define DQS_ODT     15
#define DRAM_CA_ODT 32
#define DRAM_DQ_ODT 33
#define DRAM_DQ_VOH 34
//#define DRAM_DQ_PU  35
#define DRAM_DQ_PD  36

// TMRS entry
#define MR_ADDR_MASK 0x3F00
#define MR_ADDR_SHIFT 8
#define MR_VALUE_MASK 0x00FF

// Private functions
boolean DDRSS_eCDT_Decode_Terminate_Entry(BOOT_DDR_STRUCT *ddr, uint16 entry);
boolean DDRSS_eCDT_Decode_Target_Entry(BOOT_DDR_STRUCT *ddr, uint16 entry);
boolean DDRSS_eCDT_Decode_Parameter_Entry(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint16 entry);
boolean DDRSS_eCDT_Decode_TMRS_Entry(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint16 entry);
void DDRSS_eCDT_Populate_PRFS_MC_Masks(BOOT_DDR_STRUCT *ddr);

/** @Global_variable
*    @brief
*     termination variable describes when to terminate the eCDT Decode. When termination goes to 1, eCDT Decode will terminate.
*    target_mismatch_mask
*/

static boolean terminate = FALSE;
static uint8 target_mismatch_mask = 0;
static uint16 freq_mask = 0;
static uint16 prfs_mask = 0;
static uint16 mc_mask = 0;

//================================================================================================//
//Take list List from eCDT Encoder by opening the .bin file and taking entries
//================================================================================================//
/** @API 1. eCDT_Decode
*    @brief eCDT_Decode will decode list from .bin file
*/

//================================================================================================//
//Decode DDRSS_eCDT_Decode_Entry
//================================================================================================//
/** @API 2. DDRSS_eCDT_Decode_Entry
*    @brief DDRSS_eCDT_Decode_Entry will decode different types of entries using the list coming from populate_ecdt_from_file API.
*/

boolean DDRSS_eCDT_Decode_Entry(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint16* list)
{
    uint8 type = 0;
    uint16 entry = 0;
    uint16 index = 0;

    if(list == NULL) return TRUE;

    for(index = 0; list[index] != 0; index++)
    {
        // Get entry type
        entry = list[index];
        type = (entry & ENTRY_TYPE_MASK) >> ENTRY_TYPE_SHIFT;

        // Process entry based on type
        if(type == TERMINATE)
        {
            if(DDRSS_eCDT_Decode_Terminate_Entry(ddr, entry) == FALSE)
            {
                return FALSE;
            }
        }
        else if(type == TARGET)
        {
            if(DDRSS_eCDT_Decode_Target_Entry(ddr, entry) == FALSE)
            {
                return FALSE;
            }
        }
        else if(type == PARAMETER)
        {
            // Decode parameter only if target matches
            if(target_mismatch_mask == 0)
            {
                if(DDRSS_eCDT_Decode_Parameter_Entry(ddr, channel, entry) == FALSE)
                {
                    return FALSE;
                }
            }
        }
        else if(type == TMRS)
        {
            // Decode TMRS only if target matches
            if(target_mismatch_mask == 0)
            {
                if(DDRSS_eCDT_Decode_TMRS_Entry(ddr, channel, entry) == FALSE)
                {
                    return FALSE;
                }
            }
        }
        else
        {
            return FALSE;
        }

        // Stop processing entries if termination is requested
        if(terminate == TRUE)
        {
            break;
        }
    }

    return TRUE;
}


//================================================================================================//
//DDRSS_eCDT_Decode_Terminate_Entry
//    To decode Terminate Entry
//================================================================================================//
/** @API 3. DDRSS_eCDT_Decode_Terminate_Entry
*    @brief DDRSS_eCDT_Decode_Terminate_Entry has two different sub entries.
*    If TERM is accounted the API will not consider any otehr entry and will temrinate
*   If Quit is found, the API will terminate only if Mismatch is found to be 0.
*/
boolean DDRSS_eCDT_Decode_Terminate_Entry(BOOT_DDR_STRUCT *ddr, uint16 entry)
{
    uint8 subtype = 0;
    uint8 clear_mask = 0;

    // Get entry subtype
    subtype = (entry & SUBTYPE_MASK) >> SUBTYPE_SHIFT;

    // Process entry based on subtype
    if(subtype == TERM)
    {
        terminate = TRUE;
        return TRUE;
    }
    else if(subtype == QUIT)
    {
        // Check if we should quit the decoding
        if(entry & QUIT_BIT_MASK)
        {
            if(target_mismatch_mask == 0)
            {
                terminate = TRUE;
                return TRUE;
            }
        }

        // Get clear mask
        clear_mask = entry & TARGET_RESTRICTIONS_CLEAR_MASK;

        // Clear mismatches for mode register targets
        if(clear_mask & MANUFACTURER_ID_BIT)
        {
            target_mismatch_mask &= ~MANUFACTURER_ID_BIT;
        }

        if(clear_mask & REVISION_ID_1_BIT)
        {
            target_mismatch_mask &= ~REVISION_ID_1_BIT;
        }

        if(clear_mask & REVISION_ID_2_BIT)
        {
            target_mismatch_mask &= ~REVISION_ID_2_BIT;
        }

        if(clear_mask & DENSITY_BIT)
        {
            target_mismatch_mask &= ~DENSITY_BIT;
        }

        if(clear_mask & IO_WIDTH_BIT)
        {
            target_mismatch_mask &= ~IO_WIDTH_BIT;
        }

        if(clear_mask & REVISION_ID_1_RANK_1_BIT)
        {
            target_mismatch_mask &= ~REVISION_ID_1_RANK_1_BIT;
        }

        if(clear_mask & REVISION_ID_2_RANK_1_BIT)
        {
            target_mismatch_mask &= ~REVISION_ID_2_RANK_1_BIT;
        }

        if(clear_mask & DENSITY_RANK_1_BIT)
        {
            target_mismatch_mask &= ~DENSITY_RANK_1_BIT;
        }

        if(clear_mask & IO_WIDTH_RANK_1_BIT)
        {
            target_mismatch_mask &= ~IO_WIDTH_RANK_1_BIT;
        }

        if((clear_mask & LOW_FREQ_BIT) || (clear_mask & HIGH_FREQ_BIT))
        {
            if(clear_mask & LOW_FREQ_BIT)
            {
                freq_mask &= HIGH_FREQ_MASK << HIGH_FREQ_SHIFT;
            }

            if(clear_mask & HIGH_FREQ_BIT)
            {
                freq_mask &= LOW_FREQ_MASK;
            }

            DDRSS_eCDT_Populate_PRFS_MC_Masks(ddr);
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

//================================================================================================//
//DDRSS_eCDT_Decode_Target_Entry
//    To decode Target Entry
//================================================================================================//

/** @API 4. DDRSS_eCDT_Decode_Target_Entry
*    @brief DDRSS_eCDT_Decode_Target_Entry has two different sub entries.
*    MODE_REGISTER and FREQUENCY_MASK.
*    MODE_REGISTER type has 4 types which can be found depending on the Modesubtype entry values. Depending on the MODE_REGISTER type, if the DRAM MODE_REGISTER entries matches the the list entry Mismatch bit related to the corresponding MODE_REGISTER value is reset.
*    FREQUENCY_MASK type has 2 types i.e. Low freq and High freq. This frequency mask is required to use fill the register values for PRFS and BIMC registers. Freqeuencymask is used to find prfs_mask and mc_mask which will later apply settings to corresponding registers.
*/
boolean DDRSS_eCDT_Decode_Target_Entry(BOOT_DDR_STRUCT *ddr, uint16 entry)
{
    uint8 subtype = 0;
    uint8 mr_field = 0;
    uint8 mr_value = 0;
    uint8 freq_mask_subtype = 0;

    // Get entry subtype
    subtype = (entry & SUBTYPE_MASK) >> SUBTYPE_SHIFT;

    // Process entry based on subtype
    if(subtype == MODE_REGISTER)
    {
        // Confirm pointer to detected memory device information from mode register fields was provided
        if(ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect == 0)
        {
            return FALSE;
        }

        // Get mode register field and value
        mr_field = (entry & MODE_SUBTYPE_MASK) >> MODE_SUBTYPE_SHIFT;
        mr_value = (entry & MODE_VALUE_MASK);

        // Check field value matches detected memory device information
        if(mr_field == MANUFACTURER_ID)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].manufacturer_id != mr_value)
            {
                target_mismatch_mask |= MANUFACTURER_ID_BIT;
            }
        }
        else if(mr_field == REVISION_ID_1)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].revision_id_1[0] != mr_value)
            {
                target_mismatch_mask |= REVISION_ID_1_BIT;
            }
        }
        else if(mr_field == REVISION_ID_2)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].revision_id_2[0] != mr_value)
            {
                target_mismatch_mask |= REVISION_ID_2_BIT;
            }
        }
        else if(mr_field == DENSITY)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].device_density[0] != (mr_value >> 2))
            {
                target_mismatch_mask |= DENSITY_BIT;
            }
        }
        else if(mr_field == IO_WIDTH)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].device_io_width[0] != (mr_value >> 6))
            {
                target_mismatch_mask |= IO_WIDTH_BIT;
            }
        }
        else if(mr_field == REVISION_ID_1_RANK_1)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].revision_id_1[1] != mr_value)
            {
                target_mismatch_mask |= REVISION_ID_1_RANK_1_BIT;
            }
        }
        else if(mr_field == REVISION_ID_2_RANK_1)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].revision_id_2[1] != mr_value)
            {
                target_mismatch_mask |= REVISION_ID_2_RANK_1_BIT;
            }
        }
        else if(mr_field == DENSITY_RANK_1)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].device_density[1] != (mr_value >> 2))
            {
                target_mismatch_mask |= DENSITY_RANK_1_BIT;
            }
        }
        else if(mr_field == IO_WIDTH_RANK_1)
        {
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].device_io_width[1] != (mr_value >> 6))
            {
                target_mismatch_mask |= IO_WIDTH_RANK_1_BIT;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else if(subtype == FREQUENCY_MASK)
    {
        freq_mask_subtype = (entry & FREQ_MASK_SUBTYPE_MASK) >> FREQ_MASK_SUBTYPE_SHIFT;
        if(freq_mask_subtype == LOW_FREQ)
        {
            freq_mask &= HIGH_FREQ_MASK << HIGH_FREQ_SHIFT;
            freq_mask |= (entry & LOW_FREQ_MASK);
        }
        else if(freq_mask_subtype == HIGH_FREQ)
        {
            freq_mask &= LOW_FREQ_MASK;
            freq_mask |= (entry & HIGH_FREQ_MASK) << HIGH_FREQ_SHIFT;

            // Check if low frequency mask should be cleared
            if(entry & CLEAR_BIT_MASK)
            {
                freq_mask &= HIGH_FREQ_MASK << HIGH_FREQ_SHIFT;
            }
        }
        else
        {
            return FALSE;
        }

        DDRSS_eCDT_Populate_PRFS_MC_Masks(ddr);
    }

    return TRUE;
}

//================================================================================================//
//DDRSS_eCDT_Decode_Parameter_Entry
//    To decode Parameter Entry
//================================================================================================//
/** @API 5. DDRSS_eCDT_Decode_Parameter_Entry
*    @brief DDRSS_eCDT_Decode_Parameter_Entry has two different sub entries.
*    MSM_PARAMETER_TYPE and DRAM_PARAMETER_TYPE.
*    MSM_PARAMETER_TYPE applies the settings to all the PRFS registers while DRAM_PARAMETER_TYPE applies settings to all the BIMC registers.
*/
#define PRFS_OFFSET 0x4

boolean DDRSS_eCDT_Decode_Parameter_Entry(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint16 entry)
{
    uint8 type = 0;
    uint8 index = 0;
    uint8 value = 0;
    uint8 mc_index = 0;
    uint8 prfs_index = 0;
    uint8 phy_index = 0;
    uint8 phy_mask = 0;
    uint8 ch = 0;
    uint8 num_ch = 0;
    uint8 shift = 0;
    uint32 mask = 0;
    uint32 reg_offset = 0;

    PHY_BUS dq = DQ;
    PHY_BUS ca = CA;
    
    num_ch = ddr->shrm_ddr_ptr->misc.num_channel;
    
    type = (entry & PARAMETER_TYPE_MASK) >> PARAMETER_TYPE_SHIFT;
    index = (entry & param_index_MASK) >> param_index_SHIFT;
    value = entry & PARAMETER_VALUE_MASK;
    
    if(type == DRAM_PARAMETER)
    {
        // Storing mode registers value at respective places w.r.t frequency in ecdt runtime bimc structure
        for(mc_index = 0; mc_index < NUM_FREQ_SWITCH_STRUCTS; mc_index++)
        {
            if(((mc_mask >> mc_index) & 0x1)
                || (index == DRAM_DQ_VOH)) // Set PU-Cal the same for all frequencies
            {
                if(index == DRAM_CA_ODT)
                {
                    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR11 &= 0x8F;
                    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR11 |= (value << 4);
                }
                else if(index == DRAM_DQ_ODT)
                {
                    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR11 &= 0xF8;
                    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR11 |= value;
                }
                else if(index == DRAM_DQ_VOH)
                {
                    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR3 &= 0xFE;
                    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR3 |= value;
                }
                else if(index == DRAM_DQ_PD)
                {
                    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR3 &= 0xC7;
                    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR3 |= (value << 3);
                }
                //else if(index == DRAM_DQ_PU)
                //{
                //    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR22 &= 0xF8;
                //    ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR22 |= value;
                //}
                else
                {
                    return FALSE;
                }
            }
        } // mc_index
    }
    else if(type == MSM_PARAMETER)
    {
        if(index < DQ_PU)
        {
            phy_mask = 0x1;
        }
        else
        {
            phy_mask = 0x6;
        }
        
        for(ch = 0; ch < num_ch; ch++)
        {
            if((channel >> ch) & 0x1)
            {
                for(prfs_index = 0; prfs_index < NUM_PRFS_BANDS; prfs_index++)
                {
                    if((prfs_mask >> prfs_index) & 0x1)
                    {
                        for(phy_index = 0; phy_index < (ca + dq); phy_index++)
                        {
                            if((phy_mask >> phy_index) & 0x1)
                            {
                                reg_offset = REG_OFFSET_DDR_PHY_CH(ch) + (PRFS_OFFSET * prfs_index) + (DDR_PHY_OFFSET * phy_index);
                                
                                if((index == DQ_PU) || (index == CA_PU))
                                {
                                    mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, PULL_UP_DQ_CNTL);
                                    shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, PULL_UP_DQ_CNTL);
                                }
                                else if((index == DQ_PD) || (index == CA_PD))
                                {
                                    mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODRV_DQ);
                                    shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODRV_DQ);
                                }
                                else if(index == DQ_ODT)
                                {
                                    mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODT_IN_DQ);
                                    shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODT_IN_DQ);
                                }
                                else if((index == DQ_VOH) || (index == DQS_VOH)
                                    || (index == CA_VOH) || (index == CK_VOH))
                                {
                                    mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, DRIVER_VOH_MODE);
                                    shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, DRIVER_VOH_MODE);
                                }
                                else if((index == DQS_PU) || (index == CK_PU))
                                {
                                    mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, PULL_UP_DQS_CNTL);
                                    shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, PULL_UP_DQS_CNTL);
                                }
                                else if((index == DQS_PD) || (index == CK_PD))
                                {
                                    mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODRV_DQS);
                                    shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODRV_DQS);
                                }
                                else if(index == DQS_ODT)
                                {
                                    mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODT_IN_DQS);
                                    shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODT_IN_DQS);
                                }
                                else
                                {
                                    return FALSE;
                                }
                                
                                // Read, modify, and write register
                                HWIO_OUTXM(reg_offset, DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, mask, (uint32)(value) << shift);
                            }
                        } // phy_index
                    }
                } // prfs_index
            }
        } // ch
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

//================================================================================================//
//DDRSS_eCDT_Decode_TMRS_Entry
//    To decode TMRS Entry
//================================================================================================//
/** @API 3. DDRSS_eCDT_Decode_TMRS_Entry
*    @brief DDRSS_eCDT_Decode_TMRS_Entry triggers a mode register write to all channels and ranks.
*/
boolean DDRSS_eCDT_Decode_TMRS_Entry(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, uint16 entry)
{
    uint8 ch = 0;
    uint8 cs = 0;
    uint8 mr_addr = 0;
    uint8 mr_value = 0;
    DDR_CHIPSELECT populated_chipselect = DDR_CS_NONE;
    
    // Get mode register address
    mr_addr = (entry & MR_ADDR_MASK) >> MR_ADDR_SHIFT;
    
    // Get mode regsiter value
    mr_value = entry & MR_VALUE_MASK;
    
    for(ch = 0; ch < NUM_CH; ch++)
    {
        if((channel >> ch) & 0x1)
        {
            // Get populated chipselects
            populated_chipselect = ddr->shrm_ddr_ptr->detected_ddr_device[ch].populated_chipselect;
            
            for(cs = 0; cs < NUM_CS; cs++)
            {
                if((populated_chipselect >> cs) & 0x1)
                {
                    // Write mode register
                    MC_MR_Write(ddr, CH_1HOT(ch), CS_1HOT(cs), mr_addr, mr_value);
                }
            }
        }
    }

    return TRUE;
}

//================================================================================================//
//DDRSS_eCDT_Populate_PRFS_MC_Masks
//    To populate PRFS and MC index masks
//================================================================================================//

/** @API 4. DDRSS_eCDT_Populate_PRFS_MC_Masks
*    @brief DDRSS_eCDT_Populate_PRFS_MC_Masks
*/
void DDRSS_eCDT_Populate_PRFS_MC_Masks(BOOT_DDR_STRUCT *ddr)
{
    uint8 freq_index = 0;
    uint8 prfs_index = 0;
    uint8 mc_index = 0;
    uint32 clk_khz = 0;
    uint32 clk_in_ps = 0;

    // Clear PRFS and MC index masks
    prfs_mask = 0;
    mc_mask = 0;

    // Populate PRFS and MC index masks
    for(freq_index = 0; freq_index < MAX_NUM_CLOCK_PLAN; freq_index++)
    {
        if((freq_mask >> freq_index) & 1)
        {
            clk_khz = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[freq_index].clk_freq_in_khz;

            prfs_index = DDRSS_Get_Freq_Index(ddr, clk_khz);
            prfs_mask |= (1 << prfs_index);

            clk_in_ps = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[freq_index].clk_period_in_psec;

            mc_index = MC_Freq_Switch_Params_Index(ddr->shrm_ddr_ptr, clk_in_ps);
            mc_mask |= (1 << mc_index);
        }
    }
}

//================================================================================================//
//DDRSS_eCDT_Read_Parameter
//    To read parameter default values
//================================================================================================//

/** @API 4. DDRSS_eCDT_Read_Parameter
*    @brief DDRSS_eCDT_Read_Parameter
*/
int HAL_DDR_Read_eCDT_Parameter_Values(BOOT_DDR_STRUCT *ddr, uint32 clk_freq_khz, void* buffer_ptr, size_t buffer_size)
{
    uint8 type = 0;
    uint8 index = 0;
    uint8 mc_index = 0;
    uint8 prfs_index = 0;
    uint8 phy_index = 0;
    uint8 clk_index = 0;
    uint8 param_index = 0;
    uint8 shift = 0;
    uint32 mask = 0;
    uint32 reg_offset = 0;
    uint8 value = 0;
    uint32 clk_in_ps = 0;
    uint8 *output_ptr = NULL;
    int out_count = 0;
    
    uint8 parameters[] = 
    {
        CA_PU,
        CA_PD,
        CA_VOH,
        CK_PU,
        CK_PD,
        //CK_VOH,
        DQ_PU,
        DQ_PD,
        DQ_VOH,
        DQ_ODT,
        DQS_PU,
        DQS_PD,
        //DQS_VOH,
        DQS_ODT,
        DRAM_CA_ODT,
        DRAM_DQ_ODT,
        DRAM_DQ_VOH,
        //DRAM_DQ_PU,
        DRAM_DQ_PD,
    };
    
    // Check buffer is large enough
    if(buffer_size < (sizeof(parameters) * 2))
    {
        return -1;
    }
    
    // Convert point to easier type to work with
    output_ptr = (uint8*)buffer_ptr;
    
    // Get index into clock plan
    clk_index = DDRSS_Find_Clk_Index(ddr, clk_freq_khz);
    
    for(param_index = 0; param_index < sizeof(parameters); param_index++)
    {
        type = (parameters[param_index] >> 5) & 0x1;
        index = parameters[param_index];

        if(type == DRAM_PARAMETER)
        {
            clk_in_ps = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[clk_index].clk_period_in_psec;
            mc_index = MC_Freq_Switch_Params_Index(ddr->shrm_ddr_ptr, clk_in_ps);

            if(index == DRAM_CA_ODT)
            {
                value = (ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR11 & 0x70) >> 4;
            }
            else if(index == DRAM_DQ_ODT)
            {
                value = ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR11 & 0x7;
            }
            else if(index == DRAM_DQ_VOH)
            {
                value = ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR3 & 0x1;
            }
            else if(index == DRAM_DQ_PD)
            {
                value = (ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR3 & 0x38) >> 3;
            }
            //else if(index == DRAM_DQ_PU)
            //{
            //    value = ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[mc_index].MR22 & 0x7;
            //}
            else
            {
                return -1;
            }
        }
        else if(type == MSM_PARAMETER)
        {
            prfs_index = DDRSS_Get_Freq_Index(ddr, clk_freq_khz);
            
            if(index < DQ_PU)
            {
                phy_index = 0;
            }
            else
            {
                phy_index = 1;
            }
            
            reg_offset = REG_OFFSET_DDR_PHY_CH(0) + (PRFS_OFFSET * prfs_index) + (DDR_PHY_OFFSET * phy_index);
            
            if((index == DQ_PU) || (index == CA_PU))
            {
                mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, PULL_UP_DQ_CNTL);
                shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, PULL_UP_DQ_CNTL);
            }
            else if((index == DQ_PD) || (index == CA_PD))
            {
                mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODRV_DQ);
                shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODRV_DQ);
            }
            else if(index == DQ_ODT)
            {
                mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODT_IN_DQ);
                shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODT_IN_DQ);
            }
            //else if((index == DQ_VOH) || (index == DQS_VOH)
            //    || (index == CA_VOH) || (index == CK_VOH))
            else if((index == DQ_VOH) || (index == CA_VOH))
            {
                mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, DRIVER_VOH_MODE);
                shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, DRIVER_VOH_MODE);
            }
            else if((index == DQS_PU) || (index == CK_PU))
            {
                mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, PULL_UP_DQS_CNTL);
                shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, PULL_UP_DQS_CNTL);
            }
            else if((index == DQS_PD) || (index == CK_PD))
            {
                mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODRV_DQS);
                shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODRV_DQS);
            }
            else if(index == DQS_ODT)
            {
                mask = HWIO_FMSK(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODT_IN_DQS);
                shift = HWIO_SHFT(DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, ODT_IN_DQS);
            }
            else
            {
                return -1;
            }
            
            // Read bit field
            value = HWIO_INXM(reg_offset, DDR_PHY_DDRPHY_PADEXT_PRFS_0_FPM_CNTL0_CFG, mask) >> shift;
        }
        else
        {
            return -1;
        }
        
        // Save paramter index and value
        output_ptr[param_index * 2] = index;
        output_ptr[(param_index * 2) + 1] =  value;
        out_count+=2;
    } // param_index
    
    return out_count;
}


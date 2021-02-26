/**
 * @file:  SpmiMaster.c
 * 
 * Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/core/hal/hoya/SpmiMaster.c#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 9/2/14   Modified config sequence for boot
 * 10/1/13  Initial Version
 */

#include "SpmiMaster.h"
#include "SpmiUtils.h"
#include "SpmiHal.h"
#include "SpmiLogs.h"
#include "SpmiInfo.h"

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static void clearSpmiMemory(SpmiDevInfo *pDev)
{
    uint32 i, k;
    uint8 * base_addr = pDev->baseAddrs;
    
    SPMI_LOG_VERBOSE( "Clearing SPMI PIC HW regs" );
    
    for(i = 0; i < SPMI_NUM_CHANNELS_SUPPORTED(pDev); i++) {
        SPMI_HWIO_OUT( HWIO_SPMI_PIC_IRQ_CLEARn_ADDR( base_addr, i ), HWIO_SPMI_PIC_IRQ_CLEARn_INT_CLEAR_BMSK );
        SPMI_HWIO_OUT( HWIO_SPMI_PIC_ACC_ENABLEn_ADDR( base_addr, i ), 0 );
        SPMI_HWIO_OUT( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( base_addr, i ), 0 );
        SPMI_HWIO_OUT( HWIO_PMIC_ARB_REG_ADDRp_ADDR( base_addr, i ), 0 );
    }
    
    for(i = 0; i < SPMI_NUM_MASTERS(pDev); i++)
    {
        for(k = 0; k < SPMI_NUM_PIC_ACC_STATUS_REGS(pDev); k++)
        {
            uint32 val = SPMI_HWIO_IN( HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR( base_addr, i, k ) );
            if(val != 0) {
                SPMI_LOG_WARNING( "ACC Status %d,%d - %X", i, k, val );
            }
        }
    }
}

/*
 * From HPG 4.3.3.3.1  Boot minimal initialization
 */
static Spmi_Result loadMin(SpmiDevInfo *pDev, uint8 masterId)
{
    uint8 * base_addr = pDev->baseAddrs;

    // 0. Don't make assumptions - set ser_clk_sel to cc_spmi_ahb_clk for RAM programming
    SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_GENI_CLK_CTRL_ADDR( base_addr ), HWIO_SPMI_GENI_CLK_CTRL_SER_CLK_SEL_BMSK );
    
    // 1. FW Image download
    SpmiHal_LoadFirmware( base_addr, SPMI_NUM_PROG_RAM_REGS(pDev) );
    
    // 2. Initiate FORCE-DEFAULT command
    SPMI_HWIO_OUT_SET( HWIO_SPMI_GENI_FORCE_DEFAULT_REG_ADDR( base_addr ), HWIO_SPMI_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_BMSK );
    
    // 3. Switch PROGRAM-RAM clock muxing to serial clock
    SPMI_HWIO_OUT_SET( HWIO_SPMI_GENI_CLK_CTRL_ADDR( base_addr ), HWIO_SPMI_GENI_CLK_CTRL_SER_CLK_SEL_BMSK );
    
    // 4. Disable all security features (enabled after owner tables are populated)
    SPMI_HWIO_OUT_SET( HWIO_SPMI_SEC_DISABLE_REG_ADDR( base_addr ), HWIO_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_BMSK );
    
    // 5: Disable forced High-Z driven to GENI outputs. Set bits SOE0_EN and SOE1_EN of GENI_OUTPUT_CTRL_REG register to 1
    SPMI_HWIO_OUT_SET( HWIO_SPMI_GENI_OUTPUT_CTRL_ADDR( base_addr ), HWIO_SPMI_GENI_OUTPUT_CTRL_SOE0_EN_BMSK |
                                                                     HWIO_SPMI_GENI_OUTPUT_CTRL_SOE1_EN_BMSK );
    
    // 6. Configure SPMI_MID_REG register
    SPMI_HWIO_OUT_FIELD( HWIO_SPMI_MID_REG_ADDR(base_addr), HWIO_SPMI_MID_REG_MID, masterId );
    
    // 7. Set fields FORCE_MASTER_WRITE_ON_ERROR and ARBITER_ENABLE in SPMI_CFG_REG
    SPMI_HWIO_OUT_SET( HWIO_SPMI_CFG_REG_ADDR( base_addr ), HWIO_SPMI_CFG_REG_FORCE_MASTER_WRITE_ON_ERROR_BMSK );
    SPMI_HWIO_OUT_SET( HWIO_SPMI_CFG_REG_ADDR( base_addr ), HWIO_SPMI_CFG_REG_ARBITER_ENABLE_BMSK );

    // 8. Set DONE_IRQ_EN to true
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_MISC_CONFIG_ADDR( base_addr ), HWIO_PMIC_ARB_MISC_CONFIG_DONE_IRQ_EN_BMSK );

    // *Not in the HPG* -- Core resets in bypass mode: disable bypass
    SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_CFG_REG_ADDR( base_addr ), HWIO_SPMI_CFG_REG_ARBITER_BYPASS_BMSK );
    
    return SPMI_SUCCESS;
}

/*
 * From HPG 4.3.3.3.3 
 *          Fully operational initialization (without minimal boot initialization)
 */
static Spmi_Result loadFull(SpmiDevInfo *pDev, uint8 masterId)
{
    uint8 * base_addr = pDev->baseAddrs;

    // Insure bus is idle
    if(SPMI_HWIO_IN_FIELD( HWIO_SPMI_GENI_STATUS_ADDR( base_addr ), HWIO_SPMI_GENI_STATUS_M_GENI_CMD_ACTIVE )) {
        return SPMI_FAILURE_BUS_BUSY;
    }
    
    // 1 - 7. 
    Spmi_Result rslt = loadMin( pDev, masterId );
    if(rslt != SPMI_SUCCESS) {
        return rslt;
    }
    
    // 7a. Set SSC_WINDOW_EN 
    SPMI_HWIO_OUT_SET( HWIO_SPMI_CFG_REG_ADDR( base_addr ), HWIO_SPMI_CFG_REG_SSC_WINDOW_EN_BMSK );
    
    // 8. RPU initialization N/A
    
    // 9. Mapping Table (radix tree - set later)
    
    // 10. SPMI_PERIPHm_2OWNER_TABLE_REG (owner table APID2PPID - set later)
    
    // 11. Set SPMI_CMPR_EN_REG.CMPR_ENABLE (set later when owner table is populated)
    
    // 12. SPMI_PERIPHm_2OWNER_TABLE_REG (owner table PERIPH2OWNER - set later)
    
    // 13. Enable global interrupt flag (enabled after owner tables are populated)
    
    // 14. Change ROM and RAM mux to output RAM Data.
    SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_ADDR( base_addr ), HWIO_SPMI_GENI_PROG_ROM_CTRL_REG_PROG_ROM_EN_BMSK );

    // 15. Enabling HW control CGC
    SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_GENI_CGC_CTRL_ADDR( base_addr ), HWIO_SPMI_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK );
    SPMI_HWIO_OUT_SET( HWIO_SPMI_GENI_CGC_CTRL_ADDR( base_addr ), 
                       HWIO_SPMI_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK |
                       HWIO_SPMI_GENI_CGC_CTRL_EXT_CLK_CGC_ON_BMSK |
                       HWIO_SPMI_GENI_CGC_CTRL_RX_CLK_CGC_ON_BMSK |
                       HWIO_SPMI_GENI_CGC_CTRL_TX_CLK_CGC_ON_BMSK |
                       HWIO_SPMI_GENI_CGC_CTRL_SCLK_CGC_ON_BMSK |
                       HWIO_SPMI_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_BMSK |
                       HWIO_SPMI_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_BMSK |
                       HWIO_SPMI_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_BMSK );
    
    return SPMI_SUCCESS;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiMaster_ConfigHW(SpmiDevInfo *pDev, uint8 masterId, SpmiBusCfg_ConfigType cfgType)
{
    SPMI_LOG_INFO( "Configuring SPMI Master. Id: %d, Type: %d", masterId, cfgType );
    
    if(masterId > SPMI_MAX_MASTER_ID) {
        return SPMI_FAILURE_INVALID_MASTER_ID;
    }
    
    // HW CR QCTDD01233061 - Clear registers before any configuration
    clearSpmiMemory( pDev );
    
    switch(cfgType)
    {
        case SPMI_CFG_MINIMAL:
            return loadMin( pDev, masterId );
            
        case SPMI_CFG_FULL:
            return loadFull( pDev, masterId );
        
        default:
            break;
    }
    
    SPMI_LOG_FATAL( "Invalid config type for SPMI Master: %d", cfgType );
    return SPMI_FAILURE_INIT_FAILED;
}

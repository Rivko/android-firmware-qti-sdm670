/** 
    @file  fw_config.c
    @brief fw loading implementation
 */
/*=============================================================================
            Copyright (c) 2017 Qualcomm Technologies, Incorporated.
                              All rights reserved.
              Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

#include "fw_config.h"
#include "fw_devcfg.h"
#include "qup_log.h"
#include "gpi.h"
#include "DALSys.h"
#include "DDIClock.h"
#include "ClockDefs.h"
#include "uClock.h"
#include "HALqupv3CommonHwio.h"
#include "HALqupv3SeHwio.h"
#include "HALhwio.h"
#include "DALDeviceId.h"

#define MAX_SES_PER_QUP 8
#define FW_LOAD_FREQUENCY_HZ 19200000

static   DalDeviceHandle *clock_handle                  = NULL;
static   uint32           qupv3_hw_ver                  = 0;
volatile boolean          se_fw_loaded[MAX_SES_PER_QUP] = { FALSE };
volatile boolean          qup_fw_loaded                 = FALSE;

// t32 control
volatile boolean disable_fw_loading = FALSE;

boolean fw_disable_se_clock (uint32 se_index)
{
    DALResult res;
    ClockIdType se_id;
    uint8 se_clock[] = "scc_qupv3_seN_clk";

    if (clock_handle == NULL) { return FALSE; }

    se_clock[12] = (uint8) (0x30 + se_index);

    res = DalClock_GetClockId(clock_handle, (const char *) se_clock, &se_id);
    if (DAL_SUCCESS != res) { return FALSE; }

    res = DalClock_DisableClock(clock_handle, se_id);
    if (DAL_SUCCESS != res) { return FALSE; }

    return TRUE;
}

boolean fw_enable_se_clock (uint32 se_index, boolean enable_dfs)
{
    DALResult res;
    ClockIdType se_id;
    uint8 se_clock[] = "scc_qupv3_seN_clk";

    if (clock_handle == NULL) { return FALSE; }

    se_clock[12] = (uint8) (0x30 + se_index);

    res = DalClock_GetClockId(clock_handle, (const char *) se_clock, &se_id);
    if (DAL_SUCCESS != res) { return FALSE; }

    if (enable_dfs)
    {
        res = DalClock_EnableDFS(clock_handle, se_id);
        if (DAL_SUCCESS != res) { return FALSE; }
    }

    res = DalClock_SetClockFrequency(clock_handle, se_id, FW_LOAD_FREQUENCY_HZ, CLOCK_FREQUENCY_HZ_EXACT, NULL);
    if (DAL_SUCCESS != res) { return FALSE; }

    res = DalClock_EnableClock(clock_handle, se_id);
    if (DAL_SUCCESS != res) { return FALSE; }

    return TRUE;
}

boolean fw_disable_common_clocks (void)
{
    return uClock_DisableClock(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
           uClock_DisableClock(CLOCK_SCC_QUPV3_CORE_CLK)   &&
           uClock_DisableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
           uClock_DisableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK);
}

boolean fw_enable_common_clocks (void)
{
    return uClock_EnableClock(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
           uClock_EnableClock(CLOCK_SCC_QUPV3_CORE_CLK)   &&
           uClock_EnableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
           uClock_EnableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK);
}

// from tzbsp_copy_init_se_firmware
void init_and_load_se_firmware (uint8 *se_base, elf_se_hdr *hdr, se_cfg *scfg)
{
    const uint32 *fw_val_arr  = (const uint32 *)((uint8*) hdr + hdr->fw_offset);
    const uint8  *cfg_idx_arr = (const uint8  *)          hdr + hdr->cfg_idx_offset;
    const uint32 *cfg_val_arr = (const uint32 *)((uint8*) hdr + hdr->cfg_val_offset);

    uint32 mask;
    uint32 i;

    /* First, ensure GENI FW is disabled */
    HWIO_OUTXF(se_base, QUPV3_SEn_GENI_OUTPUT_CTRL, IO_OUTPUT_CTRL, 0x0);
    HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DFS_IF_CFG, DFS_IF_EN, 0x0);

    mask = (HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, PROG_RAM_SCLK_OFF) |
            HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, PROG_RAM_HCLK_OFF));
    HWIO_OUTXM(se_base, QUPV3_SEn_GENI_CGC_CTRL, mask, mask);
    HWIO_OUTXF(se_base, QUPV3_SEn_GENI_CLK_CTRL, SER_CLK_SEL, 0x0);
    HWIO_OUTXM(se_base, QUPV3_SEn_GENI_CGC_CTRL, mask, 0x0);

    /* HPG section 3.1.7.1 */
    if (scfg->dfs_mode)
    {
        HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DFS_IF_CFG, DFS_IF_EN, 0x1);
    }

    /* HPG section 3.1.7.2 */
    /* No Init Required */

    /* HPG section 3.1.7.3 */
    if (qupv3_hw_ver >= 0x10010000) 
    {
        HWIO_OUTX(se_base, QUPV3_SEn_DMA_GENERAL_CFG,
                (HWIO_FMSK(QUPV3_SEn_DMA_GENERAL_CFG, AHB_SEC_SLV_CLK_CGC_ON) |
                 HWIO_FMSK(QUPV3_SEn_DMA_GENERAL_CFG, DMA_AHB_SLV_CLK_CGC_ON) |
                 HWIO_FMSK(QUPV3_SEn_DMA_GENERAL_CFG, DMA_TX_CLK_CGC_ON)      |
                 HWIO_FMSK(QUPV3_SEn_DMA_GENERAL_CFG, DMA_RX_CLK_CGC_ON)));

        HWIO_OUTX(se_base, QUPV3_SEn_GENI_CGC_CTRL, 
                (HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, EXT_CLK_CGC_ON)         |
                 HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, RX_CLK_CGC_ON)          |
                 HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, TX_CLK_CGC_ON)          |
                 HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, SCLK_CGC_ON)            |
                 HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, DATA_AHB_CLK_CGC_ON)    |
                 HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, CFG_AHB_WR_CLK_CGC_ON)  |
                 HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, CFG_AHB_CLK_CGC_ON)));
    }

    /* HPG section 3.1.7.4 */
    HWIO_OUTX(se_base, QUPV3_SEn_GENI_INIT_CFG_REVISION,
             HWIO_FVAL(QUPV3_SEn_GENI_INIT_CFG_REVISION, VERSION, hdr->cfg_version)); 

    HWIO_OUTX(se_base, QUPV3_SEn_GENI_S_INIT_CFG_REVISION,
             HWIO_FVAL(QUPV3_SEn_GENI_S_INIT_CFG_REVISION, VERSION, hdr->cfg_version)); 

    for (i = 0; i < hdr->cfg_size_in_items; i++)
    {
        HWIO_OUTX(se_base + cfg_idx_arr[i] * sizeof(uint32), QUPV3_SEn_GENI_CFG_REG0, cfg_val_arr[i]);
    }

    /* HPG section 3.1.7.9 */
    /* non-UART configuration, UART driver can configure as desired for UART */
    HWIO_OUTX(se_base, QUPV3_SEn_GENI_RX_RFR_WATERMARK_REG,
             HWIO_FVAL(QUPV3_SEn_GENI_RX_RFR_WATERMARK_REG, RX_RFR_WATERMARK,
                HWIO_INXF(se_base, QUPV3_SEn_SE_HW_PARAM_1, RX_FIFO_DEPTH) - 2));

    /* HPG section 3.1.7.5 */
    /* Don't change any SPI polarity, client driver will handle this */
    /* TBD: Caused data abort */
    // HWIO_OUTX(se_base, QUPV3_SEn_GENI_FORCE_DEFAULT_REG, 0x1);
    HWIO_OUTXF(se_base, QUPV3_SEn_GENI_OUTPUT_CTRL, IO_OUTPUT_CTRL, 0x7f);

    /* HPG section 3.1.7.6 */
    if (scfg->mode == GSI)
    {
        HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0x1);
        HWIO_OUTX (se_base, QUPV3_SEn_SE_IRQ_EN, 0x0);
        HWIO_OUTX (se_base, QUPV3_SEn_SE_GSI_EVENT_EN, HWIO_QUPV3_SEn_SE_GSI_EVENT_EN_RMSK);
    }
    else if (scfg->mode == FIFO)
    {
        HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0x0);
        HWIO_OUTX (se_base, QUPV3_SEn_SE_IRQ_EN, HWIO_QUPV3_SEn_SE_IRQ_EN_RMSK);
        HWIO_OUTX (se_base, QUPV3_SEn_SE_GSI_EVENT_EN, 0x0);
    }
    else if (scfg->mode == CPU_DMA) 
    {
        HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0x1);
        HWIO_OUTX (se_base, QUPV3_SEn_SE_IRQ_EN, HWIO_QUPV3_SEn_SE_IRQ_EN_RMSK);
        HWIO_OUTX (se_base, QUPV3_SEn_SE_GSI_EVENT_EN, 0x0);
    }

    /* HPG section 3.1.7.7 */
    /* GPI mode */
    mask = (HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, M_CMD_OVERRUN_EN)    |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, M_ILLEGAL_CMD_EN)    |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, M_CMD_FAILURE_EN)    |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, M_CMD_CANCEL_EN)     |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, M_CMD_ABORT_EN)      |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, M_TIMESTAMP_EN)      |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, IO_DATA_ASSERT_EN)   |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, IO_DATA_DEASSERT_EN) |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, RX_FIFO_WR_ERR_EN)   |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, RX_FIFO_RD_ERR_EN)   |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, TX_FIFO_WR_ERR_EN)   |
            HWIO_FMSK(QUPV3_SEn_GENI_M_IRQ_ENABLE, TX_FIFO_RD_ERR_EN));
    HWIO_OUTXM(se_base, QUPV3_SEn_GENI_M_IRQ_ENABLE, mask, mask);

    mask = (HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, S_CMD_OVERRUN_EN)  |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, S_ILLEGAL_CMD_EN)  |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, S_CMD_CANCEL_EN)   |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, S_CMD_ABORT_EN)    |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, S_GP_IRQ_0_EN)     |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, S_GP_IRQ_1_EN)     |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, S_GP_IRQ_2_EN)     |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, S_GP_IRQ_3_EN)     |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, RX_FIFO_WR_ERR_EN) |
            HWIO_FMSK(QUPV3_SEn_GENI_S_IRQ_ENABLE, RX_FIFO_RD_ERR_EN));
    HWIO_OUTXM(se_base, QUPV3_SEn_GENI_S_IRQ_ENABLE, mask, mask);

    /* HPG section 3.1.7.8 */
    /* GPI/DMA mode */
    mask = (HWIO_FMSK(QUPV3_SEn_DMA_TX_IRQ_EN_SET, RESET_DONE_EN_SET) |
            HWIO_FMSK(QUPV3_SEn_DMA_TX_IRQ_EN_SET, SBE_EN_SET)        |
            HWIO_FMSK(QUPV3_SEn_DMA_TX_IRQ_EN_SET, DMA_DONE_EN_SET));
    HWIO_OUTX(se_base, QUPV3_SEn_DMA_TX_IRQ_EN_SET, mask);

    mask = (HWIO_FMSK(QUPV3_SEn_DMA_RX_IRQ_EN_SET, FLUSH_DONE_EN_SET) |
            HWIO_FMSK(QUPV3_SEn_DMA_RX_IRQ_EN_SET, RESET_DONE_EN_SET) |
            HWIO_FMSK(QUPV3_SEn_DMA_RX_IRQ_EN_SET, SBE_EN_SET)        |
            HWIO_FMSK(QUPV3_SEn_DMA_RX_IRQ_EN_SET, DMA_DONE_EN_SET));
    HWIO_OUTX(se_base, QUPV3_SEn_DMA_RX_IRQ_EN_SET, mask);

    /* HPG section 3.1.7.10 */
    HWIO_OUTX(se_base, QUPV3_SEn_GENI_FW_REVISION,
            (HWIO_FVAL(QUPV3_SEn_GENI_FW_REVISION, PROTOCOL, hdr->serial_protocol) | 
             HWIO_FVAL(QUPV3_SEn_GENI_FW_REVISION, VERSION,  hdr->fw_version)));

    HWIO_OUTX(se_base, QUPV3_SEn_GENI_S_FW_REVISION,
            (HWIO_FVAL(QUPV3_SEn_GENI_S_FW_REVISION, PROTOCOL, hdr->serial_protocol) | 
             HWIO_FVAL(QUPV3_SEn_GENI_S_FW_REVISION, VERSION,  hdr->fw_version)));

    memcpy(HWIO_QUPV3_SEn_GENI_CFG_RAMn_ADDR(se_base, 0),
           fw_val_arr,
           hdr->fw_size_in_items * sizeof(uint32));

    /* HPG section 3.1.7.12 */
    /* TBD try this here*/
    HWIO_OUTX(se_base, QUPV3_SEn_GENI_FORCE_DEFAULT_REG, 0x1);

    mask = (HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, PROG_RAM_SCLK_OFF) |
            HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, PROG_RAM_HCLK_OFF));
    HWIO_OUTXM(se_base, QUPV3_SEn_GENI_CGC_CTRL, mask, mask);
    HWIO_OUTXF(se_base, QUPV3_SEn_GENI_CLK_CTRL, SER_CLK_SEL, 0x1);
    HWIO_OUTXM(se_base, QUPV3_SEn_GENI_CGC_CTRL, mask, 0x0);

    /* HPG section 3.1.7.13 */
    /* GSI/DMA mode */
    HWIO_OUTXF(se_base, QUPV3_SEn_DMA_IF_EN, DMA_IF_EN, 0x1);

    /* HPG section 3.1.7.14 */
    if ((scfg->mode == MIXED) || (scfg->mode == FIFO))
    {
        HWIO_OUTXF(se_base, QUPV3_SEn_FIFO_IF_DISABLE, FIFO_IF_DISABLE, 0x0);
    }
    else
    {
        HWIO_OUTXF(se_base, QUPV3_SEn_FIFO_IF_DISABLE, FIFO_IF_DISABLE, 0x1);
    }
}

boolean se_fw_load(uint8 *se_base, fw_type *fw_list, se_cfg *scfg, uint32 se_index)
{
    elf_se_hdr *hdr = NULL;
    uint32 index = 0;

    while (fw_list->start != NULL)
    {
        hdr = (elf_se_hdr *) (fw_list->start);
        if ((hdr->magic             == 0x57464553)      &&
            (hdr->serial_protocol   == scfg->protocol)  &&
            (hdr->core_version      <= qupv3_hw_ver))
        {
            QUP_LOG(LEVEL_INFO, "[QUP_FW] fw_list se  = %d\n", index);
            break;
        }
        hdr = NULL;
        fw_list++;
        index++;
    }
    
    if (hdr == NULL) { return FALSE; }

    if (fw_enable_se_clock (se_index, scfg->dfs_mode) == FALSE)  { return FALSE; }
    init_and_load_se_firmware (se_base, hdr, scfg);
    if (fw_disable_se_clock (se_index) == FALSE) { return FALSE; }

    return TRUE;
}

boolean gpi_fw_load(uint8 *qup_base, fw_type *fw_list)
{
    elf_qsi_hdr *hdr = NULL;
    uint32 index = 0;

    while (fw_list->start != NULL)
    {
        hdr = (elf_qsi_hdr *) (fw_list->start);
        if ((hdr->magic         == 0x20495351)  &&
            (hdr->core_version  <= qupv3_hw_ver))
        {
            QUP_LOG(LEVEL_INFO, "[QUP_FW] fw_list gsi = %d\n", index);
            break;
        }
        hdr = NULL;
        fw_list++;
        index++;
    }
    
    if (hdr == NULL) { return FALSE; }

    gpi_firmware_load(fw_list->start, fw_list->size);

    return TRUE;
}

// reference - tz
void fw_init_qup_common(uint8 *qup_common_base)
{
    qupv3_hw_ver = HWIO_INX(qup_common_base, QUPV3_QUPV3_HW_VERSION);
    QUP_LOG(LEVEL_INFO, "[QUP_FW] qupv3_hw_ver   = 0x%08x\n", qupv3_hw_ver);

    /* HPG section 3.1.2 */
    HWIO_OUTXF(qup_common_base, QUPV3_QUPV3_COMMON_CFG,
                                FAST_SWITCH_TO_HIGH_DISABLE,
                                0x01);
    /* HPG section 3.1.7.3 */
    HWIO_OUTXF(qup_common_base, QUPV3_QUPV3_SE_AHB_M_CFG,
                                AHB_M_CLK_CGC_ON,
                                0x01);
}

void qup_fw_init ()
{
    DALSYS_PROPERTY_HANDLE_DECLARE (prop_handle);
    DALSYSPropertyVar               prop_var;
    DALResult                       result = DAL_SUCCESS;

    uint8   dev_name[]  = "/dev/qup_ssc";
    uint8   se_str[]    = "seN_cfg";
    fw_type *fw_list    = NULL;
    uint32  i;

    if (disable_fw_loading) { return; }

    se_cfg  *scfg = NULL;
    qup_cfg *qcfg = NULL;

    result = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &clock_handle);
    if (DAL_SUCCESS != result) { return; }

    result = DALSYS_GetDALPropertyHandleStr((const char *) dev_name, prop_handle);
    if (result != DAL_SUCCESS) { return; }

    result = DALSYS_GetPropertyValue(prop_handle, "fw_list", 0, &prop_var);
    if (result != DAL_SUCCESS) { return; }

    fw_list = (fw_type *) prop_var.Val.pStruct;
    if (fw_list == NULL) { return; }

    result = DALSYS_GetPropertyValue(prop_handle, "qup_cfg", 0, &prop_var);
    if (result != DAL_SUCCESS) { return; }

    qcfg = (qup_cfg *) prop_var.Val.pStruct;
    if (qcfg == NULL) { return; }

    QUP_LOG(LEVEL_INFO, "[QUP_FW] qup_base       = 0x%08x\n", qcfg->qup_base);
    QUP_LOG(LEVEL_INFO, "[QUP_FW] common_offset  = 0x%08x\n", qcfg->common_offset);

    if (fw_enable_common_clocks () == FALSE) { return; }

    fw_init_qup_common(qcfg->qup_base + qcfg->common_offset);

    // assuming one QUP in sensors always
    for (i = 0; i < MAX_SES_PER_QUP; i++)
    {
        uint8 *se_base;
        se_str[2] = '0' + i;

        result = DALSYS_GetPropertyValue(prop_handle, (const char *) se_str, 0, &prop_var);
        if (result != DAL_SUCCESS)  { continue; }

        scfg = (se_cfg *) prop_var.Val.pStruct;
        if (scfg == NULL)           { continue; }

        QUP_LOG(LEVEL_INFO, "[QUP_FW] offset      = 0x%08x\n", scfg->offset);
        QUP_LOG(LEVEL_INFO, "[QUP_FW] protocol    = %d\n",     scfg->protocol);
        QUP_LOG(LEVEL_INFO, "[QUP_FW] mode        = %d\n",     scfg->mode);
        QUP_LOG(LEVEL_INFO, "[QUP_FW] load_fw     = %d\n",     scfg->load_fw);
        QUP_LOG(LEVEL_INFO, "[QUP_FW] dfs_mode    = %d\n",     scfg->dfs_mode);

        if (scfg->load_fw == FALSE) { continue; }

        se_base = (uint8 *) (qcfg->qup_base + scfg->offset);
        se_fw_loaded[i] = se_fw_load(se_base, fw_list, scfg, i);

        QUP_LOG(LEVEL_INFO, "[QUP_FW] se loaded   = %d\n", se_fw_loaded[i]);
    }

    qup_fw_loaded = gpi_fw_load(qcfg->qup_base, fw_list);

    QUP_LOG(LEVEL_INFO, "[QUP_FW] GSI loaded = %d\n", qup_fw_loaded);

    if (fw_disable_common_clocks () == FALSE) { return; }

    if (clock_handle != NULL)
    {
        DAL_DeviceDetach(clock_handle);
    }
}

boolean se_is_fw_loaded (uint32 se_index)
{
    return se_fw_loaded[se_index];
}

boolean qup_is_fw_loaded ()
{
    return qup_fw_loaded;
}

uint32 qup_hw_version ()
{
    return qupv3_hw_ver;
}

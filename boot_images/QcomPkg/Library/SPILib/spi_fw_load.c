/*==================================================================================================

FILE: spi_fw_load.c

DESCRIPTION: This module provides the firmware image for SPI Core.

Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

==================================================================================================*/
/*==================================================================================================
Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
01/07/19   bng     Initial revision

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <string.h>
#include "spi_types.h"
#include "spi_fw_load.h"
#include "HALhwio.h"
#include "HALqupv3CommonHwio.h"
#include "HALqupv3SeHwio.h"
#include <spi_config.h>
#include "SPIFWImage.h"
#include "LoaderUtils.h"


typedef struct __attribute__ ((packed)) elf_se_hdr
{
    uint32 magic;               /* = 'SEFW' */
    uint32 version;             /* Structure version number */
    uint32 core_version;        /* QUPV3_HW_VERSION */
    uint16 serial_protocol;     /* Programmed into GENI_FW_REVISION */
    uint16 fw_version;          /* Programmed into GENI_FW_REVISION */
    uint16 cfg_version;         /* Programmed into GENI_INIT_CFG_REVISION */
    uint16 fw_size_in_items;    /* Number of (uint32) GENI_FW_RAM words */
    uint16 fw_offset;           /* Byte offset of GENI_FW_RAM array */
    uint16 cfg_size_in_items;   /* Number of GENI_FW_CFG index/value pairs */
    uint16 cfg_idx_offset;      /* Byte offset of GENI_FW_CFG indexes array */
    uint16 cfg_val_offset;      /* Byte offset of GENI_FW_CFG values array */
} elf_se_hdr;

static uint32 spi_detect_rumi= TRUE;
static uint32 spi_fw_protocol = 1;
/*==================================================================================================

FUNCTION: fw_init

DESCRIPTION:

==================================================================================================*/
spi_status fw_init(spi_plat_device_config *dcfg)
{
   uint32      se_base = (uint32)dcfg->core_base_addr + dcfg->core_offset;
   uint32      qup_common_base = (uint32)dcfg->common_base_addr;
   uint32      qupv3_hw_ver = HWIO_INX(qup_common_base, QUPV3_QUPV3_HW_VERSION);
   uint32 mask,i,result;

   uint32      qupv3_protocol = HWIO_INXF(se_base, QUPV3_SEn_GENI_FW_REVISION_RO, PROTOCOL);
   elf_se_hdr* hdr = NULL;

   if (qupv3_protocol == spi_fw_protocol) { return SPI_SUCCESS; }     //SPI FW Already Loaded
   
   result = spi_get_fw((uint8**) &hdr, qupv3_hw_ver);
   if(result !=SPI_SUCCESS )
   	{
      return result;
    }

   const uint32 *fw_val_arr  = (const uint32 *)((uint8*) hdr + hdr->fw_offset);
   const uint8  *cfg_idx_arr = (const uint8  *)          hdr + hdr->cfg_idx_offset;
   const uint32 *cfg_val_arr = (const uint32 *)((uint8*) hdr + hdr->cfg_val_offset);


   /* HPG section 3.1.2 */
   HWIO_OUTXF(qup_common_base, QUPV3_QUPV3_COMMON_CFG,
                             FAST_SWITCH_TO_HIGH_DISABLE,
                             0x01);
   /* HPG section 3.1.7.3 */
   HWIO_OUTXF(qup_common_base, QUPV3_QUPV3_SE_AHB_M_CFG,
                             AHB_M_CLK_CGC_ON,
                             0x01);


   /* First, ensure GENI FW is disabled */
   HWIO_OUTXF(se_base, QUPV3_SEn_GENI_OUTPUT_CTRL, IO_OUTPUT_CTRL, 0x0);
   HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DFS_IF_CFG, DFS_IF_EN, 0x0);

   mask = (HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, PROG_RAM_SCLK_OFF) |
           HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, PROG_RAM_HCLK_OFF));
   HWIO_OUTXM(se_base, QUPV3_SEn_GENI_CGC_CTRL, mask, mask);
   HWIO_OUTXF(se_base, QUPV3_SEn_GENI_CLK_CTRL, SER_CLK_SEL, 0x0);
   HWIO_OUTXM(se_base, QUPV3_SEn_GENI_CGC_CTRL, mask, 0x0);


   /* HPG section 3.1.7.2 */
   /* No Init Required */

   // QUPv3 HPG. 3.1.7.3. SE CGC initialization
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
// if (mode == GSI)
// {
//    HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0x1);
//    HWIO_OUTX (se_base, QUPV3_SEn_SE_IRQ_EN, 0x0);
//    HWIO_OUTX (se_base, QUPV3_SEn_SE_GSI_EVENT_EN, HWIO_QUPV3_SEn_SE_GSI_EVENT_EN_RMSK);
// }
// else if (mode == FIFO)
// {
      HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0x0);
      HWIO_OUTX (se_base, QUPV3_SEn_SE_IRQ_EN, HWIO_QUPV3_SEn_SE_IRQ_EN_RMSK);
      HWIO_OUTX (se_base, QUPV3_SEn_SE_GSI_EVENT_EN, 0x0);
// }
// else if (mode == CPU_DMA)
// {
//    HWIO_OUTXF(se_base, QUPV3_SEn_GENI_DMA_MODE_EN, GENI_DMA_MODE_EN, 0x1);
//    HWIO_OUTX (se_base, QUPV3_SEn_SE_IRQ_EN, HWIO_QUPV3_SEn_SE_IRQ_EN_RMSK);
//    HWIO_OUTX (se_base, QUPV3_SEn_SE_GSI_EVENT_EN, 0x0);
// }

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


   /* HPG section 3.1.7.8 */
   /* GPI/DMA mode */
   /*mask = (HWIO_FMSK(QUPV3_SEn_DMA_TX_IRQ_EN_SET, RESET_DONE_EN_SET) |
           HWIO_FMSK(QUPV3_SEn_DMA_TX_IRQ_EN_SET, SBE_EN_SET)        |
           HWIO_FMSK(QUPV3_SEn_DMA_TX_IRQ_EN_SET, DMA_DONE_EN_SET));
   HWIO_OUTX(se_base, QUPV3_SEn_DMA_TX_IRQ_EN_SET, mask);

   mask = (HWIO_FMSK(QUPV3_SEn_DMA_RX_IRQ_EN_SET, FLUSH_DONE_EN_SET) |
           HWIO_FMSK(QUPV3_SEn_DMA_RX_IRQ_EN_SET, RESET_DONE_EN_SET) |
           HWIO_FMSK(QUPV3_SEn_DMA_RX_IRQ_EN_SET, SBE_EN_SET)        |
           HWIO_FMSK(QUPV3_SEn_DMA_RX_IRQ_EN_SET, DMA_DONE_EN_SET));
   HWIO_OUTX(se_base, QUPV3_SEn_DMA_RX_IRQ_EN_SET, mask);*/

   /* HPG section 3.1.7.10 */
   HWIO_OUTX(se_base, QUPV3_SEn_GENI_FW_REVISION,
             (HWIO_FVAL(QUPV3_SEn_GENI_FW_REVISION, PROTOCOL, hdr->serial_protocol) |
              HWIO_FVAL(QUPV3_SEn_GENI_FW_REVISION, VERSION,  hdr->fw_version)));

   HWIO_OUTX(se_base, QUPV3_SEn_GENI_S_FW_REVISION,
             (HWIO_FVAL(QUPV3_SEn_GENI_S_FW_REVISION, PROTOCOL, hdr->serial_protocol) |
              HWIO_FVAL(QUPV3_SEn_GENI_S_FW_REVISION, VERSION,  hdr->fw_version)));

   memscpy((void*)(HWIO_QUPV3_SEn_GENI_CFG_RAMn_ADDR(se_base , 0) & 0xFFFFFFFFFFFFFFFF),
		  hdr->fw_size_in_items * sizeof(uint32),
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
   HWIO_OUTXF(se_base, QUPV3_SEn_DMA_IF_EN, DMA_IF_EN, 0x0);

   /* HPG section 3.1.7.14 */
// if ((mode == MIXED) || (mode == FIFO))
// {
      HWIO_OUTXF(se_base, QUPV3_SEn_FIFO_IF_DISABLE, FIFO_IF_DISABLE, 0x0);
// }
// else
// {
//    HWIO_OUTXF(se_base, QUPV3_SEn_FIFO_IF_DISABLE, FIFO_IF_DISABLE, 0x1);
// }
   return result;
}

void spi_detect_rumi_and_init(spi_plat_device_config *dcfg)
{
	uint32	se_base = (uint32) dcfg->core_base_addr + dcfg->core_offset;


   fw_init(dcfg);

   if (spi_detect_rumi)
   {
      // FW initialization is called before GPIO configurations. So if the GENI_IOS is not zero,
      // then it is mostly RUMI as it bypasses TLMM HW and directly connects to external line.
      if (HWIO_INX(se_base , QUPV3_SEn_GENI_IOS) != 0)
      {

         uint32 mask;

         /* First, ensure GENI FW is disabled */
         HWIO_OUTXF(se_base, QUPV3_SEn_GENI_OUTPUT_CTRL, IO_OUTPUT_CTRL, 0x0);

         mask = (HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, PROG_RAM_SCLK_OFF) |
                 HWIO_FMSK(QUPV3_SEn_GENI_CGC_CTRL, PROG_RAM_HCLK_OFF));
         HWIO_OUTXM(se_base, QUPV3_SEn_GENI_CGC_CTRL, mask, mask);
         HWIO_OUTXF(se_base, QUPV3_SEn_GENI_CLK_CTRL, SER_CLK_SEL, 0x0);
         HWIO_OUTXM(se_base, QUPV3_SEn_GENI_CGC_CTRL, mask, 0x0);


         HWIO_OUTX(se_base, QUPV3_SEn_GENI_FW_REVISION, 0);
         HWIO_OUTX(se_base, QUPV3_SEn_GENI_S_FW_REVISION, 0);

        // uart_ctxt->properties->uart_base = 0xA88000;
         fw_init(dcfg);
      }
   }
}


spi_status spi_fw_load(spi_plat_device_config *dcfg)
{
   uint32 result;

#if 1
   result = fw_init(dcfg);
   if(SPI_SUCCESS != result)
   {
     return result;
   }
#else
   spi_detect_rumi_and_init(dcfg);
#endif

   return result;

}

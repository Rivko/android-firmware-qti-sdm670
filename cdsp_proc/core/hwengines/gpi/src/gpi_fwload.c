/**
  @file gpi_fwload.c
  @brief
  This file contains implementation to load GSI firmware.
*/

/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/06/17   ah      Updated logging.
06/14/17   ah      Added delay after writing to GENERIC_CMD register
03/13/17   ts      Created

===============================================================================
                   Copyright (c) 2017 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "gpi_fwload.h"
#include "gpi_utils.h"

/**
 * gpi_validate_firmware_integrity()
 * 
 * Check the integrity of firmware buffer.
 *
 * Performs check on firmware header to avoid buffer overrun.
 *
 * @param
 *  fwBuf - void pointer to the GSI firmware segment base address
 *  size  - size in bytes of the firmware buffer
 *
 * @return
 *  success or failure return status if the integrity check passed or failed
 */
static inline GPI_RETURN_STATUS gpi_validate_firmware_integrity
  (
    const void * fw,
    uint32 fwSegSize
  )
{
  gsi_fw_hdr_t * pGsiFwHdr;
  uint32 expectedSize = 0;;

  /* Assign the firmware buffer base to firmware header */
  pGsiFwHdr = (gsi_fw_hdr_t *)fw;

  /* 
   * If the passed segment buffer size is less than firmware 
   * header size, we are creating a scenario of buffer overrun
   * when we try to check the integrity of the firmware in 
   * the next step
   */
  if ( fwSegSize < sizeof(gsi_fw_hdr_t) )
  {
    gpi_log(GPI_ERROR, 0, "validate_firmware_integrity - ERR - buffer size less than hdr\n");
    return GPI_STATUS_ERROR;
  }

  /* If Magic does not match the expected ASCII value, return */
  if (pGsiFwHdr->magic != GSI_FW_MAGIC)
  {
    gpi_log(GPI_DEBUG, 0, "validate_firmware_integrity - ERR - FW magic mismatch\n");
    return GPI_STATUS_ERROR;
  }

  /* Addup the size of firmware header itself in bytes */
  expectedSize += sizeof(gsi_fw_hdr_t);

  /* Addup the size of IEP in bytes */
  expectedSize += ( pGsiFwHdr->iep_size_in_items * sizeof(gsi_fw_iep_t) );

  /* Addup the size of loadable Firmware (ech fw item is 8 bytes) in bytes */
  expectedSize += ( pGsiFwHdr->fw_size_in_items * sizeof(gsi_fw_iram_t) );

  /*
   * If calculated Firmware size is not matching incoming buffer size
   * dont shy away to throw an error and stop right there
   */
  if (expectedSize != fwSegSize)
  {
    gpi_log(GPI_DEBUG, 0, "validate_firmware_integrity - ERR - FW size did not match the buffer\n");
    return GPI_STATUS_ERROR;
  }

  return GPI_STATUS_SUCCESS;
}

/**
 * gpi_program_IEP()
 * 
 * Load the IEP.
 *
 * Program IEP entries.
 *
 * @param
 *  instance - uint32 value denoting current QUPV3 instance to program IEP to 
 *  fwIep    - A pointer to gsi_fw_iep_t to program IEP
 *  numIep   - number of fwIep entries to be programmed to 
 *
 * @return
 *  success or failure return status
 */
static GPI_RETURN_STATUS gpi_program_IEP
  (
    uint32 instance,
    gsi_fw_iep_t * fwIep,
    uint32 numIep
  )
{
  uint32 i = 0;
  uint64 base = qupv3_gsi_base[instance];

  for (i = 0; i < numIep; i++)
  {
    /*
     * Check if offset does not exceed GSI address space size
     */
    if ( fwIep[i].offset < qupv3_gsi_base_size[instance] )
    {
      out_dword(base + fwIep[i].offset, fwIep[i].value);
    }
  }

  return GPI_STATUS_SUCCESS;
}

/**
 * gpi_program_IRAM()
 * 
 * Load the firmware in IRAM.
 *
 * Program firmware in IRAM.
 *
 * @param
 *  instance  - uint32 value denoting current QUPV3 instance to program IEP to 
 *  fwIRam    - A pointer to gsi_fw_iram_t to program IRAM
 *  numFwLine - number of fwIRam entries to be programmed to 
 *
 * @return
 *  success or failure return status
 */
static GPI_RETURN_STATUS gpi_program_IRAM
  (
    uint32 instance,
    gsi_fw_iram_t * fwIRam,
    uint32 numFwLine
  )
{
  uint64 base = qupv3_gsi_base[instance];

  if ( (numFwLine * 2) > (HWIO_GSI_INST_RAM_n_MAXn + 1) )
  {
    gpi_log(GPI_ERROR, 0, "program_IRAM - ERR - Number of fw lines exceeds IRAM size\n");
    return GPI_STATUS_ERROR;
  }

  /* Program Firmware version */
  HWIO_GSI_MANAGER_MCS_CODE_VER_OUT(base, fwIRam[0].iram_dword0);

  memcpy((void *)HWIO_GSI_INST_RAM_n_ADDR(base,0),
         (void *)fwIRam,
	 (numFwLine * GSI_FW_BYTES_PER_LINE));

  return GPI_STATUS_SUCCESS;
}

/**
 * gpi_firmware_load()
 * 
 * Load the GSI firmware.
 *
 * Performs sanity checks on the passed firmware buffer and load it into
 * GSI IRAM.
 *
 * @param
 *  fwBuf - void pointer to the GSI firmware segment base address
 *  size  - size in bytes of the firmware buffer
 *
 * @return
 *  success or failure return status
 */
GPI_RETURN_STATUS gpi_firmware_load
  (
    const void * fwBuf,
    uint32 fwBufSize
  )
{
  gsi_fw_hdr_t      * pGsiFwHdr;
  gsi_fw_iep_t      * fwIep;
  gsi_fw_iram_t     * fwIRam;
  int                 nqupv3, nqupv3_toplevel;

  /* Dont go any further if the passed buffer is NULL */
  if( fwBuf == NULL )
  {
    gpi_log(GPI_ERROR, 0, "firmware_load - ERR - NULL fw buffer\n");
    return GPI_STATUS_ERROR;
  }

  /*
   * Check if the passed firmware image is intact interms of size
   */
  if ( GPI_STATUS_SUCCESS != gpi_validate_firmware_integrity(fwBuf, fwBufSize) )
  {
    return GPI_STATUS_ERROR;
  }

  /* number of toplevel QupV3 instance handled by APPS */
  nqupv3_toplevel = MAX_NUM_QUP_BLOCKS - 1;

  /* Assign firmware header base */
  pGsiFwHdr = (gsi_fw_hdr_t *)fwBuf;
  /* Assign IEP entry base */
  fwIep = (gsi_fw_iep_t *)( ( (uint8 *)fwBuf ) + pGsiFwHdr->iep_offset);
  /* Assign firmware IRAM entry base */
  fwIRam = (gsi_fw_iram_t *)( ( (uint8 *)fwBuf ) + pGsiFwHdr->fw_offset);
  
  /* For all the instances program IEP and firmware IRAM */
  for (nqupv3 = 2; nqupv3 <= nqupv3_toplevel; nqupv3++)
  {
    /*
     * Load IEP
     */
    if ( GPI_STATUS_SUCCESS != gpi_program_IEP(nqupv3,
                                               fwIep,
                                               pGsiFwHdr->iep_size_in_items))
    {
      return GPI_STATUS_ERROR;
    }

    /*
     * Load firmware in IRAM
     */
    if ( GPI_STATUS_SUCCESS != gpi_program_IRAM(nqupv3,
                                                fwIRam,
						pGsiFwHdr->fw_size_in_items))
    {
      return GPI_STATUS_ERROR;
    }

    HWIO_GSI_MCS_CFG_OUTM(qupv3_gsi_base[nqupv3], HWIO_GSI_MCS_CFG_MCS_ENABLE_BMSK, 1);

    HWIO_GSI_CFG_OUT(qupv3_gsi_base[nqupv3],
            HWIO_FMSK(GSI_CFG, DOUBLE_MCS_CLK_FREQ) |
            HWIO_FMSK(GSI_CFG, GSI_ENABLE));

    HWIO_EE_n_GSI_EE_GENERIC_CMD_OUTI(qupv3_gsi_base[nqupv3], 0, 0x61);
    /*  Initialization takes 11 + 9*SE# instructions. In worst case we have 83 instructions.
        Assume MCS runs @100Mhz -> 10ns per instruction.
        =>	830ns.
        =>	+some spare => 2us
    */
    gpi_sleep(2);
  }

  return GPI_STATUS_SUCCESS;
}

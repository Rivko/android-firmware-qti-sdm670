/** 
 @file PcieEcamInit.c

  This file implements routines to configure the ECAM space of root ports.

  Copyright (c) 2016, Qualcomm Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---    ------------------------------------------------------------
 08/10/16 tselvam  Returned EFI_SUCCESS in config space init, so as to keep
                   PciHostBridgeDxe image in memory to update AML variable
 07/28/16 tselvam  Removed resource enumeration and programming as this done
                   in PciBusDxe and Supported I/O port access
 07/15/16 tselvam  Formatted strings to %a in debug prints
 06/28/16 tselvam  Initial version spun off from PciLink.c
=============================================================================*/


#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/BaseLib.h>
#include <Library/PcieConfigLib.h>

#include "PcieLink.h"
#include "PcieBsp.h"

/***************************************************************************//**
 * @fn PcieConfigLibSetupMiscConfig
 * @brief Sets up miscellaneous configuration space registers
 *
 * Sets up Miscellaneous configuration space register for root port
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibSetupMiscConfig(UINT32 RootPortIndex)
{
  UINT32               TempReg;
  rootPort_t         * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  CONST memRegions_t * mem;
  portInfo_t         * port;

  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: No RootPorts Left to configure\r\n", __func__));
    return EFI_SUCCESS;
  }

  port = &pcie_root_port_info->port_info[RootPortIndex];

  /*
   * Get the memory regions base address
   */
  mem = pcie_root_port_info->port_info[RootPortIndex].mem;
  if (mem == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: mem regiosn is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  /* Memory Enable Compliance */
  MmioOr32((HWIO_OFFS(PCIE_0_TYPE1_STATUS_COMMAND_REG) + mem[dbi_reg].pa),
             HWIO_PCIE_0_TYPE1_STATUS_COMMAND_REG_MSE_BMSK);

  /* Writing Slave address space size */
  MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_SLV_ADDR_SPACE_SIZE) + mem[parf_reg].pa),
              (UINT32)mem[axi_reg].sz);
  MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_SLV_ADDR_SPACE_SIZE_HI) + mem[parf_reg].pa),
              (UINT32)(mem[axi_reg].sz >> 32));

  /* Clear REQ_NOT_ENTER_L1 field in PARF PM_CTRL to allow L1 entry */
  MmioAnd32((HWIO_OFFS(PCIE_0_PCIE20_PARF_PM_CTRL) + mem[parf_reg].pa),
             ~((UINT32)HWIO_PCIE_0_PCIE20_PARF_PM_CTRL_REQ_NOT_ENTR_L1_BMSK));

  /* Enable DBI_RO_WR_EN to access CS1 region */
  MmioWrite32((HWIO_OFFS(PCIE_0_MISC_CONTROL_1_REG) + mem[dbi_reg].pa),
               HWIO_PCIE_0_MISC_CONTROL_1_REG_DBI_RO_WR_EN_BMSK);
  MemoryFence();

  /* Update Base class and Sub class */
  TempReg = MmioRead32(HWIO_OFFS(PCIE_0_TYPE1_CLASS_CODE_REV_ID_REG) + mem[dbi_reg].pa);
  TempReg = TempReg & (~(UINT32)(HWIO_PCIE_0_TYPE1_CLASS_CODE_REV_ID_REG_BASE_CLASS_CODE_BMSK));
  TempReg = TempReg | (PCIE_CLASS_CODE_BRIDGE_DEVICE << HWIO_PCIE_0_TYPE1_CLASS_CODE_REV_ID_REG_BASE_CLASS_CODE_SHFT);
  TempReg = TempReg & (~(UINT32)(HWIO_PCIE_0_TYPE1_CLASS_CODE_REV_ID_REG_SUBCLASS_CODE_BMSK));
  TempReg = TempReg | (PCIE_SUBCLASS_CODE_PCI_2_PCI_BRIDGE << HWIO_PCIE_0_TYPE1_CLASS_CODE_REV_ID_REG_SUBCLASS_CODE_SHFT);
  MmioWrite32((HWIO_OFFS(PCIE_0_TYPE1_CLASS_CODE_REV_ID_REG) + mem[dbi_reg].pa), TempReg);

  /* Assert CS2 */
  MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_ELBI_CS2_ENABLE) + mem[elbi_reg].pa), HWIO_PCIE_0_PCIE20_ELBI_CS2_ENABLE_RMSK);
  /* Disable BAR0 and BAR1 */
  MmioWrite32((HWIO_OFFS(PCIE_0_BAR0_REG) + mem[dbi_reg].pa), 0x0);
  MmioWrite32((HWIO_OFFS(PCIE_0_BAR1_REG) + mem[dbi_reg].pa), 0x0);
  /* De-Assert CS2 */
  MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_ELBI_CS2_ENABLE) + mem[elbi_reg].pa), !(HWIO_PCIE_0_PCIE20_ELBI_CS2_ENABLE_RMSK));

  /* Disable DBI_RO_WR_EN bit to access CS1 region */
  MmioWrite32((HWIO_OFFS(PCIE_0_MISC_CONTROL_1_REG) + mem[dbi_reg].pa),
               !(HWIO_PCIE_0_MISC_CONTROL_1_REG_DBI_RO_WR_EN_BMSK));
  MemoryFence();

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibSetupOutboundiATU
 * @brief Sets up Outbound iATU
 *
 * Sets up iATU for specified root port with specified parameters
 *
 * @param[in] RootPortIndex, Root port index in portInfo_t struct
 * @param[in] Index, Index of the iATU to be configured
 * @param[in] Type, TLP type of the outbound window
 * @param[in] CpuAddrSpaceBase, iATU region base address
 * @param[in] CpuAddrSpaceLimit, iATU region limit address
 * @param[in] PciAddrSpaceBase, iATU translated target base address
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibSetupOutboundiATU (
  UINT32 RootPortIndex,
  iATUIndex_t Index,
  TlpType_t Type,
  UINT64 CpuAddrSpaceBase,
  UINT64 CpuAddrSpaceLimit,
  UINT64 PciAddrSpaceBase
  )
{
  rootPort_t         * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  CONST memRegions_t * mem;
  portInfo_t         * port;

  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: RootPortIndex out of range\r\n", __func__));
    return EFI_INVALID_PARAMETER;
  }

  port = &pcie_root_port_info->port_info[RootPortIndex];

  /*
   * Get the memory regions base address
   */
  mem = pcie_root_port_info->port_info[RootPortIndex].mem;
  if (mem == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: mem regiosn is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  /* Step 1. Setup the Index Register. */
  MmioWrite32((HWIO_OFFS(PCIE_0_IATU_VIEWPORT_REG) + mem[dbi_reg].pa), Index);
  /* Step 2. Setup the Region Base and Limit Address Registers. */
  MmioWrite32((HWIO_OFFS(PCIE_0_PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0) + mem[dbi_reg].pa),
              (UINT32)CpuAddrSpaceBase);
  MmioWrite32((HWIO_OFFS(PCIE_0_PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0) + mem[dbi_reg].pa),
              (UINT32)(CpuAddrSpaceBase >> 32));
  MmioWrite32((HWIO_OFFS(PCIE_0_PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0) + mem[dbi_reg].pa),
              (UINT32)CpuAddrSpaceLimit);
  /* Step 3. Setup the Target Address Registers. */
  MmioWrite32((HWIO_OFFS(PCIE_0_PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0) + mem[dbi_reg].pa),
              (UINT32)PciAddrSpaceBase);
  MmioWrite32((HWIO_OFFS(PCIE_0_PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0) + mem[dbi_reg].pa),
              (UINT32)(PciAddrSpaceBase >> 32));
  /* Step 4. Configure the region through the Region Control 1 Register. */
  MmioWrite32((HWIO_OFFS(PCIE_0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0) + mem[dbi_reg].pa),
              Type);
  /* Step 5. Enable the region. */
  MmioWrite32((HWIO_OFFS(PCIE_0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0) + mem[dbi_reg].pa),
              HWIO_PCIE_0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_BMSK);

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibSetupiATU
 * @brief Sets up iATU
 *
 * Sets up Configuration space iATU for specified root port
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibSetupiATU(UINT32 RootPortIndex)
{
  UINT64               EndPointConfigAddressBase, EndPointConfigAddressLimit;
  rootPort_t         * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  CONST memRegions_t * mem;
  portInfo_t         * port;
  EFI_STATUS           Status = EFI_SUCCESS;

  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: No RootPorts Left to configure\r\n", __func__));
    return EFI_SUCCESS;
  }

  port = &pcie_root_port_info->port_info[RootPortIndex];

  /*
   * Get the memory regions base address
   */
  mem = pcie_root_port_info->port_info[RootPortIndex].mem;
  if (mem == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: mem region is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  EndPointConfigAddressBase = mem[dbi_reg].pa + PCIE_SINGLE_BUS_ADDR_SPACE_SIZE;
  EndPointConfigAddressLimit = EndPointConfigAddressBase +
                               PCIE_SINGLE_BUS_ADDR_SPACE_SIZE -1;

  Status = PcieConfigLibSetupOutboundiATU (RootPortIndex,
                                           OutCfg,
                                           TlpTypeCfgRdWr0,
										   EndPointConfigAddressBase,
										   EndPointConfigAddressLimit,
                                           (1 << 24) /* Read the config space of Bus 1 */ );
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: PcieConfigLibSetupOutboundiATU Failed\r\n", __func__));
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibSetupECAM
 * @brief Sets up ECAM Blocker regions
 *
 * Sets up ECAM blocker regions for the specified root port
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibSetupECAM(UINT32 RootPortIndex)
{
  UINT32               Temp = 0;
  UINT64               Temp64 = 0;
  rootPort_t         * pcie_root_port_info = PcieConfigLibGetPciRootPortInfo();
  CONST memRegions_t * mem;
  portInfo_t         * port;

  if(pcie_root_port_info == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: root port info is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if (RootPortIndex >= pcie_root_port_info->num_rp)
  {
    DEBUG(( EFI_D_WARN, "%a: No RootPorts Left to configure\r\n", __func__));
    return EFI_SUCCESS;
  }

  port = &pcie_root_port_info->port_info[RootPortIndex];

  /*
   * Get the memory regions base address
   */
  mem = pcie_root_port_info->port_info[RootPortIndex].mem;
  if (mem == NULL)
  {
    DEBUG(( EFI_D_WARN, "%a: mem regiosn is NULL\r\n", __func__));
    return EFI_LOAD_ERROR;
  }

  if( port->link->trained)
  {
    /* ECAM base */
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_ECAM_BASE) + mem[parf_reg].pa),
                (UINT32)mem[dbi_reg].pa);

    /* ECAM blocker wr & rd base for RC */
    Temp64 = mem[dbi_reg].pa + PCIE_CONFIG_SPACE_LENGTH;
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE) + mem[parf_reg].pa),
                (UINT32)Temp64);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_HI) + mem[parf_reg].pa),
                (UINT32)(Temp64 >> 32));

    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE) + mem[parf_reg].pa),
                (UINT32)Temp64);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_HI) + mem[parf_reg].pa),
                (UINT32)(Temp64 >> 32));

    /* ECAM blocker wr & rd limit for RC */
    Temp64 = mem[dbi_reg].pa + (PCIE_SINGLE_BUS_ADDR_SPACE_SIZE -1);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT) + mem[parf_reg].pa),
                (UINT32)Temp64);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_HI) + mem[parf_reg].pa),
                (UINT32)(Temp64 >> 32));

    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT) + mem[parf_reg].pa),
                (UINT32)Temp64);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_HI) + mem[parf_reg].pa),
                (UINT32)(Temp64 >> 32));

    /* ECAM blocker wr & rd base for EP */
    Temp64 = mem[dbi_reg].pa + PCIE_SINGLE_BUS_ADDR_SPACE_SIZE + PCIE_CONFIG_SPACE_LENGTH;
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2) + mem[parf_reg].pa),
                (UINT32)Temp64);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2_HI) + mem[parf_reg].pa),
                (UINT32)(Temp64 >> 32));

    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2) + mem[parf_reg].pa),
                (UINT32)Temp64);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2_HI) + mem[parf_reg].pa),
                (UINT32)(Temp64 >> 32));

    /* ECAM blocker wr & rd limit for EP */
    Temp64 = mem[dbi_reg].pa + PCIE_SINGLE_BUS_ADDR_SPACE_SIZE + (PCIE_SINGLE_BUS_ADDR_SPACE_SIZE -1);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2) + mem[parf_reg].pa),
                (UINT32)Temp64);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2_HI) + mem[parf_reg].pa),
                (UINT32)(Temp64 >> 32));

    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2) + mem[parf_reg].pa),
                (UINT32)Temp64);
    MmioWrite32((HWIO_OFFS(PCIE_0_PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2_HI) + mem[parf_reg].pa),
                (UINT32)(Temp64 >> 32));


    /* ECAM_BLOCKER_EN & ECAM_BLOCKER_EN_RANGE2_BMSK */
    Temp = (HWIO_PCIE_0_PCIE20_PARF_SYS_CTRL_ECAM_BLOCKER_EN_BMSK |
            HWIO_PCIE_0_PCIE20_PARF_SYS_CTRL_ECAM_BLOCKER_EN_RANGE2_BMSK);
    MmioOr32((HWIO_OFFS(PCIE_0_PCIE20_PARF_SYS_CTRL) + mem[parf_reg].pa), Temp);
  }

  return EFI_SUCCESS;
}

/***************************************************************************//**
 * @fn PcieConfigLibInitializeConfigSpace
 * @brief Initialize Root Bridge Configuration Space
 *
 * Initialize the config space by setting up the base address,
 * setup the ATU's and check if the EP is connected
 *
 * @param[in] Root port index in portInfo_t struct
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibInitializeConfigSpace(UINT32 RootPortIndex)
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = PcieConfigLibSetupMiscConfig(RootPortIndex);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: SetupMiscConfig Failed\r\n", __func__));
    return Status;
  }

  Status = PcieConfigLibSetupiATU(RootPortIndex);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: SetupiATU Failed\r\n", __func__));
    return Status;
  }

  Status = PcieConfigLibSetupECAM(RootPortIndex);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: SetupECAM Failed\r\n", __func__));
    return Status;
  }

  return Status;
}

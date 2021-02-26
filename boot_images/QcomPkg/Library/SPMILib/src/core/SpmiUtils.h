/**
 * @file:  SpmiUtils.h
 * @brief: Helper functions, macros, and types used internally by the SPMI driver
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/core/SpmiUtils.h#1 $
 * $Change: 14402555 $
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIUTILS_H
#define	SPMIUTILS_H

#define SPMI_TIMEOUT_USEC 400

#define SPMI_MAX_MASTER_ID   0x03
#define SPMI_MAX_SLAVE_ID    0x0F
#define SPMI_MAX_VSLAVE_ID   0xFF
#define SPMI_MAX_PERIPH_ID   0xFF
#define SPMI_MAX_REG_OFFSET  0xFF

#define SPMI_MAX_REG_ADDRESS 0xFFFF

#define REG_ADDR_TO_PERIPH_ID(addr)  (((addr) >> 8) & 0xFF)
#define REG_ADDR_TO_REG_OFFSET(addr) ((addr) & 0xFF)

#define SLAVE_PERIPH_TO_PPID(slave, periph) (((slave) << 8) | (periph))

#define PPID_TO_SLAVE_ID(ppid)  (((ppid) >> 8) & SPMI_MAX_SLAVE_ID)
#define PPID_TO_VSLAVE_ID(ppid)  (((ppid) >> 8) & SPMI_MAX_VSLAVE_ID)
#define PPID_TO_PERIPH_ID(ppid) ((ppid) & SPMI_MAX_PERIPH_ID)

typedef enum
{
    SPMI_CFG_NONE,   /** No static configuration will be done */
    SPMI_CFG_FULL,   /** All supported features will be enabled */
    SPMI_CFG_MINIMAL /** A minimal number of features will be enabled */
} SpmiBusCfg_ConfigType;

typedef enum 
{
    PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG = 0,
    PMIC_ARB_CMD_EXTENDED_REG_READ_LONG = 1,
    PMIC_ARB_CMD_EXTENDED_REG_WRITE = 2,
    PMIC_ARB_CMD_RESET = 3,
    PMIC_ARB_CMD_SLEEP = 4,
    PMIC_ARB_CMD_SHUTDOWN = 5,
    PMIC_ARB_CMD_WAKEUP = 6,
    PMIC_ARB_CMD_AUTHENTICATE = 7,
    PMIC_ARB_CMD_MASTER_READ = 8,
    PMIC_ARB_CMD_MASTER_WRITE = 9,
    PMIC_ARB_CMD_TRANSFER_BUS_OWNERSHIP = 10,
    PMIC_ARB_CMD_DEVICE_DESC_BLOCK_MASTER = 11,
    PMIC_ARB_CMD_DEVICE_DESC_BLOCK_SLAVE = 12,
    PMIC_ARB_CMD_EXTENDED_REGISTER_READ = 13,
    PMIC_ARB_CMD_REGISTER_WRITE = 14,
    PMIC_ARB_CMD_REGISTER_READ = 15,
    PMIC_ARB_CMD_REGISTER_0_WRITE = 16,
    PMIC_ARB_CMD_MAX
} PmicArbCmd;

#endif


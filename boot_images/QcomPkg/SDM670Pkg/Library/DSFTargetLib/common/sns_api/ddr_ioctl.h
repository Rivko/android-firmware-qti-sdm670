#ifndef __DDR_IOCTL_H__
#define __DDR_IOCTL_H__

/**
 * @file ddr_ioctl.h
 * @brief
 * Header file for DDR IOCTL API
 */
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who          what, where, why
--------   ---          -------------------------------------------------------------
05/08/14   arindamm     Initial version.
================================================================================
                      Copyright 2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/

/** IOCTL Commands. */
typedef enum
{
    IOCTL_CMD_GET_CORE_REGISTERS_FOR_CRASH_DEBUG    = 0,         /**< Cmd to retrieve core registers for debug */
    IOCTL_CMD_GET_HIGHEST_BANK_BIT                  = 1,         /**< Cmd to return highest DRAM bank bit in the system address to HLOS software */
    IOCTL_CMD_TEMP_CTRL_MIN_FREQ                    = 2,         /**< Cmd to return min ddr freq upon temperature based refresh rate */
    IOCTL_CMD_MAX                                   = 0x7f       /**< Forces enumerator to 8 bits */
} IOCTL_CMD;

/** IOCTL Result Codes. */
typedef enum
{
    IOCTL_RESULT_ILLEGAL_COMMAND                    = 0,        /**< Illegal command */                                   
    IOCTL_RESULT_ILLEGAL_ARGUMENTS                  = 1,        /**< Legal command, illegal arguments with command */ 
    IOCTL_RESULT_COMMAND_NOT_IMPLEMENTED            = 2,        /**< Command not yet implemented */                                                 
    IOCTL_RESULT_MAX                                = 0x7f      /**< Forces enumerator to 8 bits */
} IOCTL_RESULT;

/** IOCTL Command Block */
typedef struct 
{
    uint8           result_code;
    uint32          params;
    uint32          results;
} IOCTL_ARG;
 
#endif /* __DDR_IOCTL_H__ */

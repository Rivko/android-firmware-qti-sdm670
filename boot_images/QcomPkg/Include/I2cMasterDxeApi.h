/*==============================================================================
#      Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
#               Qualcomm Technologies Confidential and Proprietary
#=============================================================================*/
/** @file   I2cMasterDxeApi.h
 *  @Author Ross Aiken (raiken@qti.qualcomm.com)
 *  @date   2015.05.14
 *  @brief  I2C Master DXE Driver additional API -- for stuff not in the official API
 */
#ifndef I2CMASTERDXEAPI_H
#define I2CMASTERDXEAPI_H

EFI_STATUS
I2cMasterGetHandle(
    IN  EFI_I2C_MASTER_PROTOCOL *This,
    OUT VOID                    *I2cHandle);

#endif /* I2CMASTERDXEAPI_H */

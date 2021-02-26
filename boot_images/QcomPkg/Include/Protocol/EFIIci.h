/**
  @file EFIIci.h
  @brief Inline Cryptographic Interface (ICI) Protocol definitions.
*/
/*=============================================================================
  Copyright (c) 2015-16 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.

  
  Portions Copyright (c) 2015, Intel Corporation. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when         who     what, where, why
 ----------   -----   --------------------------------------------------------- 
 2016/01/28    jt      Add 3rd party copyright 
 2015/03/16    rm      Initial version 
  
=============================================================================*/
#ifndef __EFIICI_H__
#define __EFIICI_H__

#define EFI_BLOCK_IO_CRYPTO_PROTOCOL_GUID \
  { \
    0xa00490ba, 0x3f1a, 0x4b4c, {0xab, 0x90, 0x4f, 0xa9, 0x97, 0x26, 0xa1, 0xe8} \
  }  

/**
  Block IO Cryptol protocol data
**/
typedef struct {
  ///
  /// GUID of the algorithm.
  ///  
  EFI_GUID Algorithm;
  
  ///
  /// Specifies KeySize in bits used with this Algorithm.
  ///  
  UINT64   KeySize;

  ///
  /// Specifies bitmask of block sizes supported by this algorithm. 
  /// Bit j being set means that 2^j bytes crypto block size is supported.
  ///  
  UINT64   CryptoBlockSizeBitMask;
} EFI_BLOCK_IO_CRYPTO_CAPABILITY;

#define EFI_BLOCK_IO_CRYPTO_INDEX_ANY 0xFFFFFFFFFFFFFFFF

typedef struct {
  ///
  /// Is inline cryptographic capability supported on this device.
  ///      
  BOOLEAN                        Supported;

  ///
  /// Maximum number of keys that can be configured at the same time.
  ///    
  UINT64                         KeyCount;
  
  ///
  /// Number of supported capabilities.
  ///    
  UINT64                         CapabilityCount;
  
  ///
  /// Array of supported capabilities.
  ///    
  EFI_BLOCK_IO_CRYPTO_CAPABILITY Capabilities[1];
} EFI_BLOCK_IO_CRYPTO_CAPABILITIES;

typedef struct {
  ///
  /// Configuration table index. A special Index EFI_BLOCK_IO_CRYPTO_INDEX_ANY 
  /// can be used to set any available entry in the configuration table.
  ///     
  UINT64                         Index;
  
  ///
  /// Identifies the owner of the configuration table entry. Entry can also be
  /// used with the Nil value to clear key from the configuration table index.
  ///   
  EFI_GUID                       KeyOwnerGuid;
  
  ///
  /// A supported capability to be used. The CryptoBlockSizeBitMask field of 
  /// the structure should have only one bit set from the supported mask.
  ///   
  EFI_BLOCK_IO_CRYPTO_CAPABILITY Capability;
  
  ///
  /// Pointer to the key. The size of the key is defined by the KeySize field
  /// of the capability specified by the Capability parameter.
  ///   
  VOID                           *CryptoKey;
} EFI_BLOCK_IO_CRYPTO_CONFIGURATION_TABLE_ENTRY;

typedef struct {
  ///
  /// Configuration table index.
  ///     
  UINT64                         Index;
  
  ///
  /// Identifies the current owner of the entry.
  ///   
  EFI_GUID                       KeyOwnerGuid;
  
  ///
  /// The capability to be used. The CryptoBlockSizeBitMask field of the 
  /// structure has only one bit set from the supported mask.
  ///   
  EFI_BLOCK_IO_CRYPTO_CAPABILITY Capability;
} EFI_BLOCK_IO_CRYPTO_RESPONSE_CONFIGURATION_ENTRY;

/**
  Resets the block device hardware.

  @param  This                  Pointer to the EFI_BLOCK_IO_CRYPTO_PROTOCOL 
                                instance.
  @param  ExtendedVerification  Indicates that the driver may perform a more 
                                exhaustive verification operation of the 
                                device during reset.
                                
  @retval EFI_SUCCESS           The device was reset.
  @retval EFI_DEVICE_ERROR      The device is not functioning properly and could
                                not be reset.
  @retval EFI_INVALID_PARAMETER This is NULL.
                               
**/
typedef
EFI_STATUS
(EFIAPI *EFI_BLOCK_IO_CRYPTO_RESET) (
  IN EFI_BLOCK_IO_CRYPTO_PROTOCOL        *This, 
  IN BOOLEAN                             ExtendedVerification
  );

/**
  Get the capabilities of the underlying inline cryptographic interface.

  @param  This                  Pointer to the EFI_BLOCK_IO_CRYPTO_PROTOCOL 
                                instance.
  @param  Capabilities          Pointer to the EFI_BLOCK_IO_CRYPTO_CAPABILITIES 
                                structure.
                                
  @retval EFI_SUCCESS           The ICI is ready for use.
  @retval EFI_BUFFER_TOO_SMALL  The Capabilities structure was too small. The 
                                number of entries needed is returned in the 
                                CapabilityCount field of the structure.
  @retval EFI_NO_RESPONSE       No response was received from the ICI.
  @retval EFI_DEVICE_ERROR      An error occurred when attempting to access the
                                ICI.
  @retval EFI_INVALID_PARAMETER This is NULL or Capabilities is NULL.
                               
**/
typedef
EFI_STATUS
(EFIAPI *EFI_BLOCK_IO_CRYPTO_GET_CAPABILITIES) (
  IN EFI_BLOCK_IO_CRYPTO_PROTOCOL        *This, 
  OUT EFI_BLOCK_IO_CRYPTO_CAPABILITIES   *Capabilities
  );  

/**
  Set the configuration of the underlying inline cryptographic interface.

  @param  This                  Pointer to the EFI_BLOCK_IO_CRYPTO_PROTOCOL 
                                instance.
  @param  ConfigurationCount    Number of entries being configured with this 
                                call.
  @param  ConfigurationTable    Pointer to a table used to populate the 
                                configuration table.
  @param  ResultingTable        Optional pointer to a table that receives the 
                                newly configured entries. 								
                                
  @retval EFI_SUCCESS           The ICI is ready for use.
  @retval EFI_NO_RESPONSE       No response was received from the ICI.
  @retval EFI_DEVICE_ERROR      An error occurred when attempting to access the
                                ICI.
  @retval EFI_INVALID_PARAMETER This is NULL or ConfigurationTable is NULL or 
                                ConfigurationCount is 0
  @retval EFI_OUT_OF_RESOURCES  Could not find the requested number of available
                                entries in the configuration table.                               
                                 
**/
typedef
EFI_STATUS
(EFIAPI *EFI_BLOCK_IO_CRYPTO_SET_CONFIGURATION) (
  IN EFI_BLOCK_IO_CRYPTO_PROTOCOL                      *This, 
  IN UINT64                                            ConfigurationCount,
  IN EFI_BLOCK_IO_CRYPTO_CONFIGURATION_TABLE_ENTRY     *ConfigurationTable,
  OUT EFI_BLOCK_IO_CRYPTO_RESPONSE_CONFIGURATION_ENTRY *ResultingTable OPTIONAL
  );   

/**
  Set the configuration of the underlying inline cryptographic interface.

  @param  This                  Pointer to the EFI_BLOCK_IO_CRYPTO_PROTOCOL 
                                instance.
  @param  StartIndex            Configuration table index at which to start
                                the configuration query.                               
  @param  ConfigurationCount    Number of entries to return in the response 
                                table.
  @param  KeyOwnerGuid          Optional parameter to filter response down to
                                entries with a given owner. A pointer to the 
                                Nil value can be used to return available 
                                entries. Set to NULL when no owner filtering 
                                is required. 	                                
  @param  ConfigurationTable    Table of configured configuration table entries 
                                (with no CryptoKey returned): configuration 
                                table index, KeyOwnerGuid, Capability. Should 
                                have sufficient space to store up to 
                                ConfigurationCount entries.							
                                
  @retval EFI_SUCCESS           The ICI is ready for use.
  @retval EFI_NO_RESPONSE       No response was received from the ICI.
  @retval EFI_DEVICE_ERROR      An error occurred when attempting to access the
                                ICI.
  @retval EFI_INVALID_PARAMETER This is NULL or ConfigurationTable is NULL or 
                                StartIndex is out of bounds                               
                                 
**/
typedef
EFI_STATUS
(EFIAPI *EFI_BLOCK_IO_CRYPTO_GET_CONFIGURATION) (
  IN EFI_BLOCK_IO_CRYPTO_PROTOCOL                      *This,
  IN UINT64                                            StartIndex,
  IN UINT64                                            ConfigurationCount,
  IN EFI_GUID                                          *KeyOwnerGuid OPTIONAL,
  OUT EFI_BLOCK_IO_CRYPTO_RESPONSE_CONFIGURATION_ENTRY *ConfigurationTable
  );
  
/**
  Perform an extended read operation.

  @param  This                  Pointer to the EFI_BLOCK_IO_CRYPTO_PROTOCOL 
                                instance.
  @param  LBA                   The starting logical block address to read 
                                from on the device. Type EFI_LBA is defined in
                                the EFI_BLOCK_IO_PROTOCOL description.
  @param  BufferSize            The size of the Buffer in bytes. This must be 
                                a multiple of the intrinsic block size of the 
                                device.
  @param  Buffer                A pointer to the destination buffer for the data.
                                The caller is responsible for either having 
                                implicit or explicit ownership of the buffer.
  @param  Index                 A pointer to the configuration table index. This 
                                is optional.
  @param  CryptoIvInput         A pointer to a buffer that contains additional 
                                cryptographic parameters as required by the 
                                capability referenced by the configuration table 
                                index, such as cryptographic initialization 
                                vector. 								
                                
  @retval EFI_SUCCESS           The ICI is ready for use.
  @retval EFI_NO_RESPONSE       No response was received from the ICI.
  @retval EFI_DEVICE_ERROR      An error occurred when attempting to access the
                                ICI.
  @retval EFI_INVALID_PARAMETER This is NULL or CryptoIvInput is incorrect.                        
                                 
**/
typedef
EFI_STATUS
(EFIAPI *EFI_BLOCK_IO_CRYPTO_READ_EXTENDED) (
  IN EFI_BLOCK_IO_CRYPTO_PROTOCOL  *This, 
  IN EFI_LBA                       LBA,
  IN UINT64                        BufferSize,
  OUT VOID                         *Buffer,
  IN UINT64                        *Index OPTIONAL,
  IN VOID                          *CryptoIvInput OPTIONAL
  );  

/**
  Perform an extended write operation.

  @param  This                  Pointer to the EFI_BLOCK_IO_CRYPTO_PROTOCOL 
                                instance.
  @param  LBA                   The starting logical block address to read 
                                from on the device. Type EFI_LBA is defined in
                                the EFI_BLOCK_IO_PROTOCOL description.
  @param  BufferSize            The size of the Buffer in bytes. This must be 
                                a multiple of the intrinsic block size of the 
                                device.
  @param  Buffer                A pointer to the source buffer for the data.
  @param  Index                 A pointer to the configuration table index. This 
                                is optional.
  @param  CryptoIvInput         A pointer to a buffer that contains additional 
                                cryptographic parameters as required by the 
                                capability referenced by the configuration table 
                                index, such as cryptographic initialization 
                                vector. 								
                                
  @retval EFI_SUCCESS           The ICI is ready for use.
  @retval EFI_NO_RESPONSE       No response was received from the ICI.
  @retval EFI_DEVICE_ERROR      An error occurred when attempting to access the
                                ICI.
  @retval EFI_INVALID_PARAMETER This is NULL or CryptoIvInput is incorrect.                        
                                 
**/
typedef
EFI_STATUS
(EFIAPI *EFI_BLOCK_IO_CRYPTO_WRITE_EXTENDED) (
  IN EFI_BLOCK_IO_CRYPTO_PROTOCOL  *This, 
  IN EFI_LBA                       LBA,
  IN UINT64                        BufferSize,
  IN VOID                          *Buffer,
  IN UINT64                        *Index, OPTIONAL
  IN VOID                          *CryptoIvInput OPTIONAL
  );

/**
  Flushes all modified data to a physical block device.

  @param  This                  Pointer to the EFI_BLOCK_IO_CRYPTO_PROTOCOL 
                                instance.
                                
  @retval EFI_SUCCESS           All outstanding data were written correctly to 
                                the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while attempting 
                                to write data
  @retval EFI_NO_MEDIA          There is no media in the device                                
  @retval EFI_INVALID_PARAMETER This is NULL.
                               
**/
typedef
EFI_STATUS
(EFIAPI *EFI_BLOCK_IO_CRYPTO_FLUSH) (
  IN EFI_BLOCK_IO_CRYPTO_PROTOCOL  *This
  );
  
///
///  This protocol provides control over block devices with crypto engine.
///  
typedef struct _EFI_BLOCK_IO_CRYPTO_PROTOCOL {
  EFI_BLOCK_IO_MEDIA                         *Media;
  EFI_BLOCK_IO_CRYPTO_RESET                  Reset;
  EFI_BLOCK_IO_CRYPTO_GET_CAPABILITIES       GetCapabilities;
  EFI_BLOCK_IO_CRYPTO_SET_CONFIGURATION      SetConfiguration;
  EFI_BLOCK_IO_CRYPTO_GET_CONFIGURATION      GetConfiguration;
  EFI_BLOCK_IO_CRYPTO_READ_DEVICE_EXTENDED   ReadExtended;
  EFI_BLOCK_IO_CRYPTO_WRITE_DEVICE_EXTENDED  WriteExtended;
  EFI_BLOCK_IO_CRYPTO_FLUSH                  FlushBlocks;
} EFI_BLOCK_IO_CRYPTO_PROTOCOL;

#endif /* __EFIICI_H__ */


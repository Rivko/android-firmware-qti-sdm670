/**
  @file EFIAcpiPlatform.h
  @brief Declaration of AcpiPlatform DXE Driver Interface for getting and
  updating ACPI tables
*/
/*=============================================================================
  Copyright (c) 2011-2013 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies Inc., Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when        who     what, where, why
 --------    ---    -----------------------------------------------------------
 02/03/14    llg    (Tech Pubs) Edited/added Doxygen comments and markup. 
 02/14/14    swi    Added macro signature for all tables
					Added register interface to enable AML variable query/update
 08/12/13    sahn   Implemented callback mechanism to update ACPI tables
 07/22/13    zzx    Added register interface to enable functionality of ACPI table 
                    query/update with ACPITableLoadEvent.
 08/13/12    llg    (Tech Pubs) Edited/added Doxygen comments and markup.
 04/02/12    shl    Initial version.

=============================================================================*/
#ifndef __EFI_ACPIPLATFORM_H__
#define __EFI_ACPIPLATFORM_H__

/*===========================================================================
  DATA TYPE DEFINITIONS
===========================================================================*/
/** @ingroup efi_acpiPlatform_protocol */
extern EFI_GUID gQcomAcpiPlatformProtocolGuid;

/** @addtogroup efi_acpiPlatform_data_types 
@{ */
/** Mode of the ACPITableGetCallbackPtr() callback function as implemented 
    by the client driver. */
typedef enum
{
  GETMODE_PREFIX    = 0x1, /**< Prefix. */
  GETMODE_POSTFIX   = 0x2, /**< Postfix. */
  GETMODE_BOTH      = 0x3  /**< Both. */ // 0x1 | 0x2
} GETMODE;

/** ACPI table entry. */
typedef struct 
{
  VOID   *Table;     /**< Pointer to the table. */
  UINTN   TableSize; /**< Table size. */
  UINT32  Signature; /**< Table signature. */
} AcpiTableEntry;
/** @} */ /* end_addtogroup efi_acpiPlatform_data_types */
#define AML_NAMESTRING_LENGTH 4
#define AML_NULL_CHAR 0x0
/** Encoding is Term Object Type + Name + Data Type. See encoding struct **/
#define AML_NAMESPACE_HEADER_SIZE (2+AML_NAMESTRING_LENGTH)
/** AML variable entry. */
typedef struct 
{
  VOID   *AmlVariable;          /**< Pointer to the aml encoding of variable. */
  UINTN   AmlVariableSize;      /**< Buffer size of the aml encoding. */
  CHAR8   AmlVariableName[AML_NAMESTRING_LENGTH];  /**< AML variable name. */
  UINTN   TableOffset;			/**< Offset value for Variable buffer in the table */
} AmlVariableEntry;

/** Aml entry encoding **/
typedef struct 
{
  UINT8   AmlTermObjectType;            /**< Encoding value for Name Op is AML_NAME_OP **/ 
  UINT8   AmlVariableName[AML_NAMESTRING_LENGTH]; /**< root char followed by 4-letter
												Variable Name in ASCII with null char **/
  UINT8   AmlVariableDataType;		/**< Aml data object indicating data type **/
  VOID	  *AmlVariableDataValue;	/**< Buffer for variable value **/
} AmlVariableEncoding;

/** @cond */
typedef struct _EFI_QCOM_ACPIPLATFORM_PROTOCOL EFI_QCOM_ACPIPLATFORM_PROTOCOL;
/** @endcond */

/** @ingroup efi_acpiPlatform_constants */
/** Protocol version. */
#define ACPIPLATFORM_REVISION 0x0000000000010002
/** TableSignature macro for all tables in AcpiTableRegister  **/
#define ACPI_TABLE_SIGNATURE_ALL 0x0

/*===========================================================================
  Callback Function Prototype for Clients
===========================================================================*/
/** @addtogroup efi_acpi_platform_callback_functions 
@{ */
/* ACPITableGetCallbackPtr */
/**
  @par Summary
  This callback function is called when a table with the registered signature
  is read. The client driver can use this callback to save a copy of the table.

  @note1hang Because loading ACPI tables into system memory is blocked until 
  callback functions are complete, client drivers must return from callback 
  functions as soon as possible.

  @param[in] AcpiTable      ACPI table.
  @param[in] AcpiTableSize  ACPI table size.
  @param[in] IsPrefix       Whether to get the prefix tables or the postfix 
                            tables.

  @par Description
  The memory to hold ACPI tables is allocated by ACPIPlatfromDXE. It is the 
  client driver's responsibility to release the memory with FreePool when it is 
  done using the memory. If the registered table signature is not valid (i.e., 
  no table with the registered signature is found), this callback function is 
  called with the following parameters: AcpiTableSize is 0 and AcpiTable is a 
  NULL pointer.
  @par
  If there is an error condition, this callback function prints an error
  message and returns an appropriate error code. The error condition 
  must not halt the system.
  @par
  If the IsPrefix parameter is set to TRUE, the content of the ACPI tables is 
  before the update (``as is'' from the disk); otherwise, the content is after 
  the update (as seen by HLOS). If TableSignature is 0x0 when registering, 
  ACPIPlatformDXE gets the content of all ACPI tables, and the size indicates 
  the total size of all ACPI tables. 

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef
EFI_STATUS
(EFIAPI *ACPITableGetCallbackPtr)(
  IN VOID           *AcpiTable,
  IN UINTN           AcpiTableSize,
  IN BOOLEAN         IsPrefix
);

/* ACPITableSetCallbackPtr */
/**
  @par Summary
  In this callback function, the client updates the ACPI table obtained from 
  ACPIPlatformDXE and passes it back. 
  
  @note1hang Because loading ACPI tables into system memory is blocked until 
  callback functions are complete, client drivers must return from callback 
  functions as soon as possible.

  @param[in,out] AcpiTable      ACPI table.
  @param[in,out] AcpiTableSize  ACPI table size.

  @par Description
  The memory to hold ACPI tables is allocated <i>and</i> released by 
  ACPIPlatfromDXE. The ACPI table must be modified in place. If AcpiTableSize 
  is not large enough to store the size of the updated table, the following 
  must be done to create a larger buffer for the updated table: @vertspace{3} 
  -# Free the buffer by calling FreePool on *AcpiTable.
  -# Allocate a larger buffer by calling AllocatePool.
  -# Set *AcpiTable to point to the new buffer.
  @par
  If the registered table signature is not valid, AcpiTableSize is 0 and 
  AcpiTable is a NULL pointer. 
  @par
  If there is an error condition, this callback function prints an error
  message and returns an appropriate error code. The error condition 
  must not halt the system.

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  Other values -- Failure.
*/
typedef
EFI_STATUS
(EFIAPI *ACPITableSetCallbackPtr)(
  IN OUT  VOID         **AcpiTable,
  IN OUT  UINTN         *AcpiTableSize
);

/**
 This callback function is called when a variable with the registered name
 is read. The client driver can use this callback to obtain the variable's value.
 Since loading of ACPI tables into system memory is blocked till the callback functions 
 complete, client drivers should return from callback functions as soon as possible.
 The memory to hold AmlVariableBuffer is allocated by ACPIPlatfromDXE, it is client 
 driver's responsibility to release the memory with FreePool when it is done 
 using it. If the registered variable name is not valid, this callback will have the 
 following parameters: AmlVariableBufferSize is 0, and AmlVariableBuffer is a NULL pointer.
 If this is an error condition, the callback function should print an error
 message and return an appropriate error code. It should not halt the system.
 If IsPrefix is set to be TRUE, the variable's value is from before update 
 (as is from disk); Otherwise it is after update (as seen by HLOS).
 If variable name passed is 'ALL' when registering, ACPIPlatformDXE passes info on all the variables,
 and the size indicates total size of all AmlVariableEntry. Client driver will have to
 traverse through buffer correctly using the AmlVariableEntry->Length to determine size of
 the value, since string variables have variable length.

 The callback function should return EFI_SUCCESS if the function completes
 successfully, or the appropriate error code on failure.
 */
typedef
EFI_STATUS
(EFIAPI *AMLVariableGetCallbackPtr )
(
  IN VOID           *AmlVariableBuffer,
  IN UINTN           AmlVariableBufferSize,
  IN BOOLEAN         IsPrefix
);

/**
 In this callback function, client should update AML global variables obtained from 
 ACPIPlatformDXE and pass it back. Since loading of ACPI tables into system 
 memory is blocked till the callback functions complete, client drivers should 
 return from callback functions as soon as possible.
 The memory to hold the variable entry is allocated AND released by ACPIPlatfromDXE. 
 The variable entry should be modified in place. 
 If the registered variable name is not valid, AmlVariableBufferSize will be 0, and 
 AcpiTable is a NULL pointer. If this is an error condition, the callback 
 function should print an error message and return an appropriate error code.
 It should not halt the system.
 
 We are only supporting static changes to the AML binary data, thus we cannot accept increases 
 to the size of our binary buffer. This introduces some restrictions on what can be updated:
 Only the actual variable data should change, the name, type and size should remain constant.
 If the variable is a string, then the length of the string cannot be increased, if it is shortened
 then the rest of the string must be padded with AML_ZERO_OP. Failure to follow these restrictions 
 will result in ACPIPlatformDXE returning EFI_INVALID_PARAMETER error code after verifying the client driver
 updates. Also, if the data does not adhere to the variable type, it will be truncated (i.e. variable type is
 byte, but buffer contains a word). ACPIPlatformDXE has no way of knowing if buffer was modified to be larger
 so it will only copy the expected size, thus truncating any exceeding data.

 The callback function should return EFI_SUCCESS on success or the appropriate
 error code on failure.
*/
typedef
EFI_STATUS
(EFIAPI *AMLVariableSetCallbackPtr )
(
  IN OUT  VOID         **AmlVariableBuffer,
  IN      UINTN          AmlVariableBufferSize
);

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_ACPI_PLATFORM_REGISTER */
/** @ingroup efi_acpi_platform_register
  @par Summary
  Registers the callback function to support ACPI table query and update 
  services.

  @param[in] This              Pointer to the EFI_QCOM_ACPIPLATFORM_PROTOCOL 
                               instance.
  @param[in] TableSignature    Signature of the ACPI table with which to be 
                               associated. The signature must be a valid 
                               signature of an ACPI table found on the system. 
                               If the signature is 0x0, ACPIPLatformDXE assumes 
                               that the client driver operates on all ACPI 
                               tables.
  @param[in] GetMode           Mode of the ACPITableGetCallbackPtr() callback 
                               function; see #GETMODE for details.
  @param[in] GetTableCallback  Pointer to the ACPITableGetCallbackPtr() 
                               callback function; can be NULL. However, 
                               it cannot be NULL if SetTableCallback 
                               is not NULL.
  @param[in] SetTableCallback  Pointer to the ACPITableSetCallbackPtr() 
                               callback function; can be NULL.

  @return
  EFI_SUCCESS           -- Registration succeeded.
  @par
  EFI_OUT_OF_RESOURCES  -- Request could not be completed due to lack of 
                           resources. The maximum number of registrations is 
                           defined as MAX_ACPI_REGISTERS.
  @par
  EFI_INVALID_PARAMETER -- Possible descriptions are:
                           - Registration was closed when ACPIPlatformDxe 
                             started working on ACPITableLoadEvent
                           - The <i>This</i> parameter is NULL
                           - The <i>GetMode</i> parameter is invalid
                           - Same table has already been registered for updating
                           - Signature is 0x0 and the <i>SetTableCallback</i> 
                             parameter is not NULL
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ACPI_PLATFORM_REGISTER)(
  IN  EFI_QCOM_ACPIPLATFORM_PROTOCOL    *This,
  IN  UINT32                             TableSignature,
  IN  GETMODE                            GetMode,                                       
  IN  ACPITableGetCallbackPtr            GetTableCallback,
  IN  ACPITableSetCallbackPtr            SetTableCallback
);

/* EFI_ACPI_AML_VARIABLE_REGISTER */
/** 
  Register callback function to support ACPI table query/update services.

  @param[in] This                  		A pointer to the EFI_QCOM_ACPIPLATFORM_REGISTER_PROTOCOL instance.
  @param[in] VariableName           	Name of the AML Variable being registered for callback. It needs to 
										be a valid name of a variable found in the AML. Or if the name is
										"ALL", ACPIPLatformDXE will return a list of all variables and their values
  @param[in] GetMode                	Mode of the GetAmlVariableCallback function. There are 3 modes: prefix,
										postfix, or both.
  @param[in] GetAmlVariableCallback    This is the pointer to get AML variable callback function. Could be NULL.
										It could not be NULL if SetAmlVariableCallback is not NULL.
  @param[in] SetAmlVariableCallback    This is the pointer to set postfix AML variable callback function. Could be NULL.

  @return
    EFI_SUCCESS            Registration succeed. 
    EFI_OUT_OF_RESOURCES   The request could not be completed due to a lack of resources,
                           Maximum number of registrations is defined as MAX_AML_VARIABLES_REGISTERS.
    EFI_INVALID_PARAMETER  Registration is closed when ACPIPlatformDxe start working on ACPITableLoadEvent
    EFI_INVALID_PARAMETER  This is NULL
    EFI_INVALID_PARAMETER  GetMode is invalid
    EFI_INVALID_PARAMETER  Same variable was already registered to be updated 
    EFI_INVALID_PARAMETER  Signature is 0x0 while SetTableCallback is not NULL
*/
typedef
EFI_STATUS
(EFIAPI *EFI_ACPI_AML_VARIABLE_REGISTER )
(
  IN  EFI_QCOM_ACPIPLATFORM_PROTOCOL    *This,
  IN  CHAR8                              VariableName[AML_NAMESTRING_LENGTH],
  IN  GETMODE                            GetMode,                                       
  IN  AMLVariableGetCallbackPtr          GetAmlVariableCallback,
  IN  AMLVariableSetCallbackPtr          SetAmlVariableCallback
);

/* EFI_ACPIPLATFORM_PROTOCOL_GET_ACPI_TABLE */
/** @ingroup efi_acpi_platform_get_acpi_table
  @par Summary
  Gets all ACPI tables.
  
  @param[in]   This           Pointer to the EFI_QCOM_ACPIPLATFORM_PROTOCOL 
                              instance.
  @param[out]  AcpiTable      ACPI table; see #AcpiTableEntry for details.
  @param[out]  AcpiTableSize  ACPI table size; see #AcpiTableEntry for details.
  @param[in]   IsPreFix       Whether to get the prefix tables or the postfix 
                              tables.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS (EFIAPI *EFI_ACPIPLATFORM_PROTOCOL_GET_ACPI_TABLE)(
  IN       EFI_QCOM_ACPIPLATFORM_PROTOCOL       *This,
  OUT      VOID                                 *AcpiTable,
  OUT      UINTN                                *AcpiTableSize,
  IN       BOOLEAN                               IsPreFix
);

/* EFI_ACPIPLATFORM_PROTOCOL_GET_ACPI_TABLE_SIZE */
/** @ingroup efi_acpi_platform_get_acpi_table_size
  @par Summary
  Gets the ACPI table size.
  
  @param[in]   This           Pointer to the EFI_QCOM_ACPIPLATFORM_PROTOCOL 
                              instance.
  @param[out]  PreFixUpAcpiTableSize   Pre-fixup table size.
  @param[out]  PostFixUpAcpiTableSize  Post-fixup table size. 
  @param[out]  TotalNumOfAcpiTables    Total number of tables.

  @return
  EFI_SUCCESS        -- Function completed successfully. \n
  EFI_PROTOCOL_ERROR -- Error occurred during the operation.
*/
typedef 
EFI_STATUS (EFIAPI *EFI_ACPIPLATFORM_PROTOCOL_GET_ACPI_TABLE_SIZE)(
  IN EFI_QCOM_ACPIPLATFORM_PROTOCOL    *This,
  OUT UINTN                            *PreFixUpAcpiTableSize,
  OUT UINTN                            *PostFixUpAcpiTableSize, 
  OUT UINTN                            *TotalNumOfAcpiTables 
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_acpiPlatform_protocol
  @par Summary
  Qualcomm Advanced Configuration and Power Interface (ACPI) Platform Protocol 
  interface.

  @par Parameters
  @inputprotoparams{acpi_platform_proto_params.tex} 
*/
struct _EFI_QCOM_ACPIPLATFORM_PROTOCOL {
   UINT64                                            Revision;
   EFI_ACPIPLATFORM_PROTOCOL_GET_ACPI_TABLE          GetAcpiTable;
   EFI_ACPIPLATFORM_PROTOCOL_GET_ACPI_TABLE_SIZE     GetAcpiTableSize;
   EFI_ACPI_PLATFORM_REGISTER                        AcpiTableRegister;
   EFI_ACPI_AML_VARIABLE_REGISTER                    AmlVariableRegister;
}; 

#endif

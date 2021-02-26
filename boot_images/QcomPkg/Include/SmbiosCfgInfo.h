#ifndef __SMBIOS_CFG_INFO_H__
#define __SMBIOS_CFG_INFO_H__

/*=============================================================================

                    Smbios Table Config  Information  Header File

GENERAL DESCRIPTION
  This is an internal header file. Contains definitions and functions to help
  Uefi load Smbios tables.

Copyright 2012-2014 by Qualcomm Technologies Inc, Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/14   vk      Fix warning
01/30/13   vk      Fix warning
07/09/12   yg      Add FileSize as param
03/23/12   kpa     Initial creation
            
============================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define  MAX_TABLE_SIZE                        256
#define  MAX_STRINGS                           20
#define  MAX_LINE_BUFFER_SIZE                  64

typedef struct _SMBIOS_TABLE_STRINGS
{
  UINTN              StringNum;     
  UINT8              *String;
} SMBIOS_TABLE_STRINGS;

typedef struct _SMBIOS_TABLE_NODE
{
  // Flag to skip generic add for tables added via 
  // explicit update api's.
  BOOLEAN                   SkipAdd;

  // Pointer to Table Buffer to be added.
  VOID                      *TablePtr;

  UINTN                    TableType;     
  EFI_SMBIOS_HANDLE         TableHandle;

  // Number of Strings to be updated.
  UINTN                    StringCount;
  SMBIOS_TABLE_STRINGS      TableStr[MAX_STRINGS];
  struct _SMBIOS_TABLE_NODE *next;

} SMBIOS_TABLE_NODE;

typedef enum
{
    // ID's for tables types that can repeat. Needed to uniquely identify 
    // a table in node.  Start with a random high number.
    SMBIOS_L1_INSTN_CACHE_TABLE = 126, /* in UINT8 range */
    SMBIOS_L1_DATA_CACHE_TABLE, 
    SMBIOS_L2_CACHE_TABLE,
    SMBIOS_L3_CACHE_TABLE,
} SMBIOS_LIST_NODE_TABLE_TYPES;

/* Config  Init procedure type */
typedef EFI_STATUS (*SmbiosConfigInitFuncType)
(
  SMBIOS_TABLE_NODE     *SmbiosTableNode
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/**
  Store Pointer to Smbios linked list

  @param  HeadNodePtr   Pointer to the Smbios Table head node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS SetSmbiosListHeadNode ( IN SMBIOS_TABLE_NODE   *HeadNodePtr);

/**
  Retrieve Pointer to Smbios linked list

  @param  HeadNodePtr   Pointer to the Smbios Table head node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS GetSmbiosListHeadNode ( OUT SMBIOS_TABLE_NODE   **HeadNodePtr);

/**
  Setup the Smbios table List

  @param  TableListPtr   Pointer to the Smbios Table list

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS SmbiosConfigInit ( OUT SMBIOS_TABLE_NODE   **TableListPtr);

/**
  Allocate a new node for Smbios table List

  @param  TableNodePtr   Pointer to the Smbios Table list node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS AllocSmbiosListNode (OUT SMBIOS_TABLE_NODE   **TableNodePtr);

/**
  Add Smbios tables from table List to system

  @param  Smbios        Pointer Smbios protocol 
          tableListPtr  Pointer to table list

  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
AddSmbiosTables ( 
                    EFI_SMBIOS_PROTOCOL    *Smbios,
                    SMBIOS_TABLE_NODE      *TableListPtr );

/**
  Retrieve List node data for a particular table ID/type

  @param  Smbios        Pointer Smbios protocol 
          tableListPtr  Pointer to table list

  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
GetTableData ( 
                    IN OUT SMBIOS_TABLE_NODE      **TableListPtr,
                    IN UINTN                     TableId );

/**
  Append  a new node to Smbios table List 

  @param  TableListPtr        Pointer to smbios linked list
          NewNodePtr          New node to be appended.

  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
AppendSmbiosTableToList ( 
                    IN SMBIOS_TABLE_NODE      *TableListPtr,
                    IN SMBIOS_TABLE_NODE      *NewNodePtr);

/**
   Update Table linked list node with table handle. Add 
   Strings if any to system
 
  @param  Smbios        Pointer Smbios protocol 
          tableListPtr  Pointer to table list

  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
UpdateTableData ( 
                    IN EFI_SMBIOS_PROTOCOL    *Smbios,
                    IN EFI_SMBIOS_HANDLE       TableHandle,
                    IN EFI_SMBIOS_TYPE         TableType );


/**
  Setup Smbios tables with data from config file.

  @param  CfgFileBuffPtr   Pointer to the buffer containing 
                           config file

  @retval EFI_SUCCESS    Sucessful update
  

**/
EFI_STATUS
UpdateCfgBasedInfoTable ( 
                    IN UINT8                  *CfgFileBuffPtr, 
                    IN UINTN                 FileSize,
                    IN SMBIOS_TABLE_NODE      *HeadNode );

/**
   Traverse linked list table nodes and Print Data 

   @param  TableListPtr  Pointer to table list
          
  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
PrintSmbiosTables ( IN SMBIOS_TABLE_NODE      *TableListPtr);

#endif  /* __SMBIOS_CFG_INFO_H__ */


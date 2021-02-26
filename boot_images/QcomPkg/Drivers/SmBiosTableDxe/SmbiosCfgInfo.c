/** @file
  Implements the BIOS Info Table for SMBios. 
   
  Copyright (c) 2010-2014,2017, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/13/17   ltg     Enable adding Type 19 table
 01/14/15   bh      Fixed LLVM warning, change UINT32 to UINTN
 09/05/14   vk      KW fix
 07/09/13   vk      Add Type 3 from cfg file
 04/08/13   yg      Disable debug messages
 07/26/12   vishalo Enable adding Type 17 table
 07/20/12   vishalo Disable adding Type 17 table
 07/09/12   yg      Some code cleanup and base on new parser
 04/25/12   yg      Revert Type 3 table removal
 04/24/12   yg      Fix some table loading issues
 03/15/12   kedara  Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Protocol/Smbios.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/QcomLib.h>
#include <Library/ParserLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <SmbiosCfgInfo.h>

#define CACHE_TABLES 0x3
#define TAB_BUFF_PRINT_SIZE 40

// external Api's to initialize Smbios tables.
EFI_STATUS BiosInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS SystemInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS SysEnclosureTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS ProcessorInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS MemoryArrayTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode); 
EFI_STATUS MemoryDeviceInfoInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS MemoryArrayMappedAddressInfoInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS L1InstCacheInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS L1DataCacheInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS L2CacheInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);
EFI_STATUS L3CacheInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode);

SmbiosConfigInitFuncType LoadSmbiosTableBuffersToList[] = 
{
  //   Add Bios info /Type 0 table to list
    BiosInfoTableInit,
   //  Add System info /Type 1 table  to list
    SystemInfoTableInit,
   //  Add System Enclosure info /Type 3 to list
#if 0    
    /* NOTE: Added in SMBIOS.CFG */ 
    SysEnclosureTableInit,
#endif
  //   Add Processor info /Type 4  table to list
    ProcessorInfoTableInit,    
  //   Add Memory Array info /Type 16 table to list
    MemoryArrayTableInit,
  //   Add Memory Device info /Type 17 table to list
    MemoryDeviceInfoInit,
  //   Add Memory Array Mapped Address info /Type 19 table to list
    MemoryArrayMappedAddressInfoInit,
  //   Add L1 Instruction cache info /Type 7 table to list
    L1InstCacheInfoTableInit,
  //   Add  L1 Data cache info /Type 7 table to list
    L1DataCacheInfoTableInit,
  //   Add L2 cache info /Type 7 table to list
    L2CacheInfoTableInit,
  //   Add L3 cache info /Type 7 table to list
    L3CacheInfoTableInit,
 // Last entry in the table.  
  NULL 
};



// Global pointer to Smbios table list.
SMBIOS_TABLE_NODE *gSmbiosListPtr = NULL;

/**
  Store Pointer to Smbios linked list

  @param  HeadNodePtr   Pointer to the Smbios Table head node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS SetSmbiosListHeadNode (
  IN SMBIOS_TABLE_NODE   *HeadNodePtr
  )
{
  if (HeadNodePtr == NULL)
    return EFI_INVALID_PARAMETER;

  gSmbiosListPtr = HeadNodePtr;

  return EFI_SUCCESS;
}

/**
  Retrieve Pointer to Smbios linked list

  @param  HeadNodePtr   Pointer to the Smbios Table head node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS GetSmbiosListHeadNode (
  OUT SMBIOS_TABLE_NODE   **HeadNodePtr
  )
{
  if (HeadNodePtr == NULL)
    return EFI_INVALID_PARAMETER;

  *HeadNodePtr = gSmbiosListPtr;

  return EFI_SUCCESS;
}

/**
  Allocate a new node for Smbios table List

  @param  TableNodePtr   Pointer to the Smbios Table list node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS AllocSmbiosListNode (
  OUT SMBIOS_TABLE_NODE   **TableNodePtr
  )
{
  SMBIOS_TABLE_NODE                 *newNode = NULL;

  if (TableNodePtr == NULL)
    return EFI_INVALID_PARAMETER;

  // allocate node
  newNode = (SMBIOS_TABLE_NODE*)AllocateRuntimePool(sizeof(SMBIOS_TABLE_NODE));
  if(newNode == NULL)
    return EFI_BUFFER_TOO_SMALL;

  // Initialize data.
  newNode->TablePtr = NULL;
  newNode->TableHandle = 0;
  newNode->next = NULL;
  newNode->TableType = 0xFFFFFFFF;
  newNode->SkipAdd = TRUE;
  newNode->StringCount = 0;
  *TableNodePtr = newNode;
    
  return EFI_SUCCESS;
}


/**
  Append  a new node to Smbios table List 

  @param  TableListPtr      Pointer to smbios linked list
          NewNodePtr        New node to be appended.

  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
AppendSmbiosTableToList ( 
  IN SMBIOS_TABLE_NODE      *TableListPtr,
  IN SMBIOS_TABLE_NODE      *NewNodePtr
  )
{
  if( TableListPtr == NULL || NewNodePtr == NULL )
    return EFI_INVALID_PARAMETER;

  // Traverse the list till last node.
  while (TableListPtr->next)
    TableListPtr = TableListPtr->next;

  TableListPtr->next = NewNodePtr;

  return EFI_SUCCESS;
}


/**
  Setup the Smbios table buffers List

  @param  TableListPtr   Pointer to the Smbios Table list

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS SmbiosConfigInit (
  OUT SMBIOS_TABLE_NODE   **TableListPtr
  )
{
  BOOLEAN                HeadNodeLinked = FALSE;
  EFI_STATUS             Status = EFI_INVALID_PARAMETER;
  SMBIOS_TABLE_NODE     *NodePtr = NULL, *NewNodePtr = NULL;
  SmbiosConfigInitFuncType *CurrTableProc;
  
  if (TableListPtr == NULL)
    return EFI_INVALID_PARAMETER;

  // Iterate over config init api array and create linked list.
  for (CurrTableProc = LoadSmbiosTableBuffersToList; 
        *CurrTableProc != NULL; CurrTableProc++ )
  {
    //Allocate new node
    Status = AllocSmbiosListNode(&NewNodePtr);

    if(Status != EFI_SUCCESS)
      break;

    // Check to see if start of linked list. If so store the start/head node
    if (HeadNodeLinked == FALSE)
    {
      *TableListPtr = NewNodePtr;
      NodePtr = NewNodePtr;
      HeadNodeLinked = TRUE;
    } 
    else
    {
      // Link the node.
      NodePtr->next = NewNodePtr;
      NodePtr = NodePtr->next;
    }

    // Add the table buffer to node
    Status = (*CurrTableProc) (NodePtr);

    if (Status != EFI_SUCCESS)
      break;
  }

  return Status;
}    


/**
  Add binary table data to a new Smbios node in List

  @param  String Buffer     Buffer containing Add Command buffer

  @retval Table Type found

**/
UINTN ProcessAddCommand (UINT8 *DataStr)
{
  UINT8 HexByte[3], *ListTableBuffer = NULL;
  EFI_STATUS  Status = EFI_INVALID_PARAMETER;
  UINTN DataOffset = 0;
  UINTN  TypeFound = 0xFFFF;
  SMBIOS_TABLE_NODE  *NodePtr = NULL , *HeadNode = NULL;

  if (DataStr == NULL)
    return TypeFound;
  
  // Create a new node for the table/ binary blob to be added.
  Status = AllocSmbiosListNode(&NodePtr);
  if(Status != EFI_SUCCESS)
    return TypeFound;

  //Modify flag to enable generic add api to add this table
  NodePtr->SkipAdd = FALSE;
  // Allocate space to hold the table.
  NodePtr->TablePtr = (SMBIOS_TABLE_NODE*) AllocateRuntimePool(MAX_TABLE_SIZE);
  if (NodePtr->TablePtr == NULL)
    return TypeFound;

  ListTableBuffer = (UINT8 *)NodePtr->TablePtr;
  CopyMem(HexByte, (VOID *)DataStr , 0x2); // extract a hex byte string, 2 chars.

  // Null terminate temporary data string.
  HexByte[2] = 0;
  NodePtr->TableType = AsciiStrToHex((CHAR8*)HexByte, 2);
  TypeFound = NodePtr->TableType;
  DataOffset = 0;

  // Populate table with binary data.
  while(*DataStr && (DataOffset < MAX_TABLE_SIZE) )
  {
    CopyMem(HexByte, (VOID*)DataStr , 0x2);
    ListTableBuffer[DataOffset] = (UINT8)AsciiStrToHex((CHAR8*)HexByte, 2);
    DataOffset++ ;
    DataStr += 2;
  }                 
  
  // Get pointer to start of linked list. 
  GetSmbiosListHeadNode(&HeadNode);
  if (HeadNode == NULL)
    return TypeFound;

  // Add new table to the list. 
  AppendSmbiosTableToList(HeadNode, NodePtr);  

  return TypeFound;
}

/*
#  Type,Byte Offset,Data Type,Data
#
#  Type        : 2 chars in Hex format (TT)
#  Byte Offset : 2 chars in Hex (if XX ignored, used for strings only types) (BB)
#  Data Type   : 1 char, 'I' for Integer type, 'S' for String type
#  Data        : As many bytes to fill, following details for each data type
#        I : As many number of Hex chars can be given, one byte (2 hex chars)
#            are dealt at a time. Starts at the byte offset indicated, goes on
#            filling the data as much is presented in the config line. ie the
#            data should be in the order of its memory address. To keep
#            readability multiple config lines can be used with appropriate
#            offset. For filling the memory for WORD, DWORD, GUID fields the
#            byte stream should be formatted appropriately. Space can be used
#            to enhance readability (Space ignored only for integer types).
#            Number of chars should be even, if odd number of chars are listed
#            then the behavior is undefined.
#
#        S : String in double Quotes. If Byte Offset is indicated then the
#            string in that offset is replaced. If Byte Offset is XX then its
#            config creator to make appended to the last string. But the string
#            offset should be available already after the structure in string
#            fields.
#            XX strings applicable only for tables added via this Config file
*  
*  Example formats
*
*  TT,BB,S,"????????"
*  TT,BB,I,aa bb cc dd ee ffff
*
*/
void ProcessUpdateCommand (UINT8* TokenPtrArray[])
{
  UINT8 *ListTableBuffer = NULL;
  UINT8  DataType = 0, *DataStr = NULL, HexByte[4];
  UINTN TableIndex = 0, DataOffset = 0, StringNum = 0, StrLength = 0;
  SMBIOS_TABLE_NODE  *NodePtr = NULL;

  if (TokenPtrArray[2] == NULL || TokenPtrArray[3] == NULL)
    return;

  TableIndex = AsciiStrToHex((CHAR8*)TokenPtrArray[0], 2);
  DataOffset = AsciiStrToHex((CHAR8*)TokenPtrArray[1], 2);

  // Retrieve table pointer from list according to table type
  GetTableData (&NodePtr, TableIndex);
  // If Table Does not exist, update not possible.
  if (NodePtr == NULL)
    return;
  
  DataType = *TokenPtrArray[2];
  DataStr = TokenPtrArray[3];

  ListTableBuffer = (UINT8 *)NodePtr->TablePtr;

  if (DataType == 'I')
  {
    // Update data bytes from buffer at given offset
    while (*DataStr && (DataOffset < MAX_TABLE_SIZE))
    { 
      // Process one byte at a time. TokenPtrArray string assumed to have even number of bytes  
      CopyMem (HexByte, (void*)DataStr , 0x2);
      ListTableBuffer[DataOffset] = (UINT8)AsciiStrToHex((CHAR8*)HexByte, 0x3);
      DataStr += 2;  // increment 2 chars in string
      // Point to next byte in Table Buffer
      DataOffset++;
    }
  }
  else if (DataType == 'S')
  {  
    if (AsciiStrnCmp((CHAR8*)TokenPtrArray[1], "XX", 2) != 0)
    {
      // Update Strings with offsets
      StringNum =  ListTableBuffer[DataOffset];
    }
    else
    {
      //Applies specifically to OEM strings Table Type 11. Append string to next available slot.
      // Get number of existing number of strings .
      StringNum = NodePtr->StringCount + 1;

      // "XX" Append-offset Strings can only be used for tables added via  Config file.
      if (NodePtr->SkipAdd == TRUE)
      {
         DEBUG ((DEBUG_WARN, "\n Error: 'XX' offset type not allowed for table"));
         return;
      }
    }
      
    DEBUG ((DEBUG_INFO, " Tbl %d Str @ %d (%d)\n", TableIndex, DataOffset, StringNum));

    if (!(NodePtr->StringCount < MAX_STRINGS))
      return;

    NodePtr->TableStr[NodePtr->StringCount].StringNum = StringNum;
    StrLength = AsciiStrLen ((CHAR8*)DataStr);
    NodePtr->TableStr[NodePtr->StringCount].String = (UINT8*) AllocateRuntimePool (StrLength + 1);

    if (NodePtr->TableStr[NodePtr->StringCount].String == NULL)
      return;

    NodePtr->TableStr[NodePtr->StringCount].String[StrLength] = 0;
    CopyMem (NodePtr->TableStr[NodePtr->StringCount].String, DataStr, StrLength);
    NodePtr->StringCount++;
  }
}

/**
  Process tokens from the config file. decode command token 
  and take corrosponding action 

  @param  TokenPtrArray     Array of token string pointers
          max_tokens          size of array, max allowed tokens
                              

  @retval EFI_SUCCESS    Sucessful update

**/
void ProcessCfgFileTokens (UINT8* TokenPtrArray[], UINTN MaxTokens)
{
  UINT8 *TokenStr = NULL;

  // At least, first two tokens should never be null.
  if (TokenPtrArray[0] == NULL || TokenPtrArray[1] == NULL)
    return;
    
  //  Parse First token . If first character numeric its a update command.
  //    else if its a alphabet, it might be a command.
  TokenStr = TokenPtrArray[0];
  
  /* TODO: This will not work if the Table type is greater than 0x9f ie 159 */
  if ((*TokenStr >= 'a' && *TokenStr <= 'z') ||
      (*TokenStr >= 'A' && *TokenStr <= 'Z'))
  {
    // Convert all chars to lowercase to make command character case neutral.
    AsciiStrToLower ((CHAR8*)TokenStr, MAX_LINE_BUFFER_SIZE);

    //Check if CFG or VER tokens, ignore these for now.
    if ((AsciiStrnCmp((CHAR8*)TokenStr, "cfg", 3) == 0) || 
        (AsciiStrnCmp((CHAR8*)TokenStr, "ver", 3) == 0))
      return;

    //Check if ADD command  string
    if(AsciiStrnCmp((CHAR8*)TokenStr, "add", 3) == 0)
    {
      UINTN Typ;
      Typ = ProcessAddCommand (TokenPtrArray[1]);
      DEBUG ((DEBUG_INFO, "ADD Type: %d\n", Typ));
    }
  }
  else if (*TokenStr >= '0' && *TokenStr <= '9')
  {
    ProcessUpdateCommand (TokenPtrArray);
  }
  else
    DEBUG ((DEBUG_INFO, "ERROR: Token not expected \n" ));
}

/**
  Retrieve List node data for a particular table ID/type

  @param  TableListPtr        Pointer to Smbios list start 
          TableId             table type whos data is to be
                              retrived

  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
GetTableData ( 
  IN OUT SMBIOS_TABLE_NODE      **TableListPtr,
  IN UINTN                     TableId
  )
{
  SMBIOS_TABLE_NODE      *NodePtr = NULL;

  // Get pointer to start of linked list. 
  GetSmbiosListHeadNode(&NodePtr);
  if (TableListPtr == NULL || NodePtr == NULL)
    return EFI_INVALID_PARAMETER;

  *TableListPtr = NULL;

  // Traverse the list and locate the table.
  while(NodePtr)
  {
    if (NodePtr->TableType == TableId)
    {
      *TableListPtr = NodePtr;
      break;
    }
    NodePtr = NodePtr->next;
  }
  
  return EFI_SUCCESS;
}


/**
   Update linked list table node with table handle. Add 
   Strings if any to system
 
  @param  Smbios        Pointer Smbios protocol 
          TableListPtr  Pointer to table list

  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
UpdateTableData ( 
  IN EFI_SMBIOS_PROTOCOL    *Smbios,
  IN EFI_SMBIOS_HANDLE       TableHandle,
  IN EFI_SMBIOS_TYPE         TableType
  )
{
  UINTN                  index = 0;
  SMBIOS_TABLE_NODE      *NodePtr = NULL;
  EFI_STATUS              Status = EFI_INVALID_PARAMETER;

  if (Smbios == NULL)
    return EFI_INVALID_PARAMETER;

  // Locate table with input table type
  GetTableData (&NodePtr, TableType);
  if (NodePtr == NULL)
    return EFI_INVALID_PARAMETER;

  //Store handle
  if (TableHandle)
    NodePtr->TableHandle = TableHandle;

  //Update Strings if any
  if (NodePtr->StringCount > 0)
  {
    DEBUG ((DEBUG_INFO, "Update Type: %d\n",NodePtr->TableType));
    for (index = 0; index < NodePtr->StringCount ; index++)
    {
      DEBUG ((DEBUG_INFO, "  Str @ %d = %a\n",
                                       NodePtr->TableStr[index].StringNum, 
                                       NodePtr->TableStr[index].String));

      Status = Smbios->UpdateString (Smbios,
                                     &(NodePtr->TableHandle),  
                                     &(NodePtr->TableStr[index].StringNum), 
                                     (CHAR8*)NodePtr->TableStr[index].String);

      if(Status != EFI_SUCCESS)
        return Status;
    }
  }

  return Status;
}

/**
   Traverse linked list table nodes and Print Data 

   @param  TableListPtr  Pointer to table list
          
  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
PrintSmbiosTables ( 
  IN SMBIOS_TABLE_NODE      *TableListPtr
  )
{
  UINT8 *ptr = NULL;
  UINTN index = 0;

  if (TableListPtr == NULL)
    return EFI_INVALID_PARAMETER;

  // Traverse the list and add all the tables to system.
  // Return success if all nodes skipped.
  DEBUG ((DEBUG_LOAD, "\n\t\tTable List" ));

  while(TableListPtr)
  {
    DEBUG ((DEBUG_LOAD, "\n--------------------------------------" ));
    DEBUG ((DEBUG_LOAD, "\n  Table Type: %d",TableListPtr->TableType ));
    DEBUG ((DEBUG_LOAD, "\n  Table TablePtr: 0x%x",TableListPtr->TablePtr ));
    DEBUG ((DEBUG_LOAD, "\n  Skip flag : %d",TableListPtr->SkipAdd ));
    DEBUG ((DEBUG_LOAD, "\n  Next Node : 0x%x",TableListPtr->next ));      
    DEBUG ((DEBUG_LOAD, "\n  Strings: %d\n", TableListPtr->StringCount));

    for (index = 0; index < TableListPtr->StringCount; index++)
      DEBUG ((DEBUG_LOAD, "\n %d] <%a>  str# %d", index, TableListPtr->TableStr[index].String, 
                                                  TableListPtr->TableStr[index].StringNum));

    DEBUG ((DEBUG_LOAD, "\n\n  Data : \n"));

    ptr = (UINT8 *)TableListPtr->TablePtr;

    for (index = 0; index < TAB_BUFF_PRINT_SIZE; index++)
      DEBUG ((DEBUG_LOAD, " %2x",*ptr++));

    DEBUG ((DEBUG_LOAD, "\n--------------------------------------" ));

    TableListPtr = TableListPtr->next;
  }

  return EFI_SUCCESS;
}


/**
  Add Smbios tables from table List to system

  @param  Smbios        Pointer Smbios protocol 
          TableListPtr  Pointer to table list

  @retval EFI_SUCCESS    Sucessful update

**/
EFI_STATUS
AddSmbiosTables ( 
  IN EFI_SMBIOS_PROTOCOL    *Smbios,
  IN SMBIOS_TABLE_NODE      *TableListPtr
  )
{
  EFI_STATUS  Status = EFI_INVALID_PARAMETER;
  UINTN index = 0;
  EFI_SMBIOS_HANDLE TableHandle;

  if (TableListPtr == NULL)
    return EFI_INVALID_PARAMETER;

  // Traverse the list and add all the tables to system.
  // Return success if all nodes skipped.
  Status = EFI_SUCCESS;
  while (TableListPtr)
  {
    if (TableListPtr->SkipAdd == FALSE)
    {
      Status = Smbios->Add(Smbios, NULL, &TableListPtr->TableHandle, 
                         (EFI_SMBIOS_TABLE_HEADER*)TableListPtr->TablePtr);

      if (Status != EFI_SUCCESS)
        return Status;

      // Update Strings if any.
      if (TableListPtr->StringCount > 0)
      {
        DEBUG ((DEBUG_INFO, "Update Type: %d\n",TableListPtr->TableType));
        TableHandle = TableListPtr->TableHandle;
        for(index =0; index < TableListPtr->StringCount ; index++)
        {
          DEBUG ((DEBUG_INFO, "  Num: %d Str: %a\n",
                                     TableListPtr->TableStr[index].StringNum, 
                                     TableListPtr->TableStr[index].String ));

          Status = Smbios->UpdateString(Smbios,  
                                 &TableHandle,  
                                 &(TableListPtr->TableStr[index].StringNum), 
                                 (CHAR8*)TableListPtr->TableStr[index].String);

          if(Status != EFI_SUCCESS)
            return Status;

        }
      }
    }
    TableListPtr = TableListPtr->next;
  }

  return Status;
}

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
  IN SMBIOS_TABLE_NODE      *HeadNode
  )
{
  int Pd;

  if(CfgFileBuffPtr == NULL || HeadNode == NULL)
    return EFI_INVALID_PARAMETER;
  
  Pd = OpenParser (CfgFileBuffPtr, FileSize, NULL);

  // Initialize the Head Node;
  SetSmbiosListHeadNode (HeadNode);

  EnumCommaDelimItems (Pd, ProcessCfgFileTokens);

  CloseParser (Pd);
  
  return EFI_SUCCESS;
}

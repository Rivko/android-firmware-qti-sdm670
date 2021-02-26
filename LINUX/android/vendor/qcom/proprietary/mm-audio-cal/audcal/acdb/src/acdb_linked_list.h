#ifndef ACDB_LINKED_LIST_H
#define ACDB_LINKED_LIST_H
/*==============================================================================

FILE: acdb_linked_list.h

DESCRIPTION: Functions and definitions to access the ACDB data structure.

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: N/A

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/
/* $Header: //source/qcom/qct/multimedia2/Audio/audcal4/acdb_sw/main/latest/acdb/src/acdb_linked_list.h#13 $ */
/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when who what, where, why
-------- --- ----------------------------------------------------------
05/28/14 mh SW migration from 32-bit to 64-bit architecture
07/23/10 ernanl Introduce new heap optimization APIs
07/06/10 ernanl Initial revision.

===========================================================================*/

/* ---------------------------------------------------------------------------
* Include Files
*--------------------------------------------------------------------------- */
#include "acdb_os_includes.h"

/* ---------------------------------------------------------------------------
* Preprocessor Definitions and Constants
*--------------------------------------------------------------------------- */

#define ACDB_HEAP_FREE_NODE 0x00000006
#define ACDB_HEAP_NOTFREE_NODE 0x00000007

/*------------------------------------------------------------------------------
Target specific definitions
------------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
* Type Declarations
*--------------------------------------------------------------------------- */

typedef struct _AcdbDataLookupKeyType AcdbDataLookupKeyType;
#include "acdb_begin_pack.h"
/**
@struct AcdbDataLookupKeyType
@brief The lookup key structure

@param nLookupKey: An intermediate lookup key

This structure provides an intermediate lookup key that is used
to simplify looking up data between several calls to the data base.
*/
struct _AcdbDataLookupKeyType {
	uint32_t nTableId;
	uintptr_t nLookupKey;
}
#include "acdb_end_pack.h"
;

typedef struct _AcdbDataLookupCVDKeyType AcdbDataLookupCVDKeyType;
#include "acdb_begin_pack.h"
/**
@struct AcdbDataLookupKeyType
@brief The lookup key structure

@param nLookupKey: An intermediate lookup key

This structure provides an intermediate lookup key that is used
to simplify looking up data between several calls to the data base.
*/
struct _AcdbDataLookupCVDKeyType {
	uint32_t nTableId;
	uintptr_t nLookupKey;
	uintptr_t nSecondaryLookupKey;
}
#include "acdb_end_pack.h"
;

typedef struct _AcdbDataLookupTblKeyType AcdbDataLookupTblKeyType;
#include "acdb_begin_pack.h"
/**
@struct AcdbDataLookupKeyType
@brief The lookup key structure

@param nLookupKey: An intermediate lookup key

This structure provides an intermediate lookup key that is used
to simplify looking up data between several calls to the data base.
*/
struct _AcdbDataLookupTblKeyType {
	uint32_t nTableId;
}
#include "acdb_end_pack.h"
;

typedef struct _AcdbDataLookupPrimaryKeyType AcdbDataLookupPrimaryKeyType;
#include "acdb_begin_pack.h"
/**
@struct AcdbDataLookupKeyType
@brief The lookup key structure

@param nLookupKey: An intermediate lookup key

This structure provides an intermediate lookup key that is used
to simplify looking up data between several calls to the data base.
*/
struct _AcdbDataLookupPrimaryKeyType {
	uintptr_t nLookupKey;
}
#include "acdb_end_pack.h"
;

typedef struct _AcdbDataLookupSecondaryKeyType AcdbDataLookupSecondaryKeyType;
#include "acdb_begin_pack.h"
/**
@struct AcdbDataLookupKeyType
@brief The lookup key structure

@param nLookupKey: An intermediate lookup key

This structure provides an intermediate lookup key that is used
to simplify looking up data between several calls to the data base.
*/
struct _AcdbDataLookupSecondaryKeyType {
	uintptr_t nSecondaryLookupKey;
}
#include "acdb_end_pack.h"
;

/**
@struct AcdbDeltaDataKeyType
@brief The lookup key structure for delta file data.

@param nLookupKey: An intermediate lookup key

This structure provides an lookup key that is used
to locate data between several calls to the data base.
*/
typedef struct _AcdbDeltaDataKeyType AcdbDeltaDataKeyType;
#include "acdb_begin_pack.h"
struct _AcdbDeltaDataKeyType {
	uint32_t nTableId;
	uint32_t nIndicesCount;
	uint32_t mid;
	uint32_t pid;
	uint8_t *pIndices;
}
#include "acdb_end_pack.h"
;

/**
@struct AcdbDataUniqueDataNodeType
@brief This structure contains a pointer and length to a unique data entry.

@param pUniqueData: A pointer to the unique data entry.
@param ulDataLen: The size of the unique data in bytes.

This structure contains a pointer and length to a unique data entry.
*/
typedef struct _AcdbDataUniqueDataNodeType AcdbDataUniqueDataNodeType;
#include "acdb_begin_pack.h"
struct _AcdbDataUniqueDataNodeType{
	const uint8_t *pUniqueData;
	const uint32_t ulDataLen;
}
#include "acdb_end_pack.h"
;

/**
@struct AcdbDataTopologyType
@brief Topology element description structure

@param ulModuleId: The module id
@param ulParamId: The parameter id
@param ulMaxParamLen: The maximum length for the data of this node.

The topology element description structure is used in parallel to the
Calibration table to provide metadata for each table entry. This is
separate from the table because it's common information and would add
24 to 32 bytes for every calibration entry in the database (this is
can cause the database to become very large!).
*/
typedef struct _AcdbDataTopologyType AcdbDataTopologyType;
#include "acdb_begin_pack.h"
struct _AcdbDataTopologyType {
	const uint32_t ulModuleId;
	const uint32_t ulParamId;
	const uint32_t ulMaxParamLen;
}
#include "acdb_end_pack.h"
;
/**
@struct AcdbDataGeneralInfoType
@brief Response structure containing a byte buffer to be translated by
the specific command it is used by.

@param nBufferLength: The number of bytes provided in the buffer.
@param pBuffer: A pointer to a buffer containing a payload.

Response structure containing a byte buffer to be translated by the
specific command it is used by. Please refer to the specific command
for more information on the format of the payload.
*/
typedef struct _AcdbDataGeneralInfoType AcdbDataGeneralInfoType;
#include "acdb_begin_pack.h"
struct _AcdbDataGeneralInfoType {
	uint32_t nBufferLength;
	uint8_t* pBuffer;
}
#include "acdb_end_pack.h"
;

//////////////////////////////////////////////////////
typedef struct AcdbDynamicUniqueDatastruct AcdbDynamicUniqueDataType;
#include "acdb_begin_pack.h"
struct AcdbDynamicUniqueDatastruct{
	uint32_t refcount;
	uint32_t ulParamId;
	uint8_t *ulDataBuf;
	uint32_t ulDataLen;
	struct AcdbDynamicUniqueDatastruct *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicDataNodeStruct AcdbDynamicDataNodeType;
#include "acdb_begin_pack.h"
struct AcdbDynamicDataNodeStruct{
	AcdbDynamicUniqueDataType *pDatalHead;
	AcdbDynamicUniqueDataType *pDatalTail;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbParamIDStruct AcdbParamIDType;
#include "acdb_begin_pack.h"
struct AcdbParamIDStruct{
	uint32_t ulParamId;
	struct AcdbParamIDStruct *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicTopologyStruct AcdbDynamicTopologyType;
#include "acdb_begin_pack.h"
struct AcdbDynamicTopologyStruct{
	uint32_t ulModuleId;
	AcdbDynamicUniqueDataType *pDataNode;
	struct AcdbDynamicTopologyStruct *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicTopologyNodeStruct AcdbDynamicTopologyNodeType;
#include "acdb_begin_pack.h"
struct AcdbDynamicTopologyNodeStruct{
	AcdbDynamicTopologyType *pTopHead;
	AcdbDynamicTopologyType *pTopTail;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicSecondaryKeyStruct AcdbDynamicSecondaryKeyType;
#include "acdb_begin_pack.h"
struct AcdbDynamicSecondaryKeyStruct{
	AcdbDataLookupSecondaryKeyType *pKey;
	struct AcdbDynamicTopologyNodeStruct *pTopologyNode;
	struct AcdbDynamicSecondaryKeyStruct *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicTblStruct AcdbDynamicTblType;
#include "acdb_begin_pack.h"
struct AcdbDynamicTblStruct{
	AcdbDataLookupKeyType *pKey;
	struct AcdbDynamicTopologyNodeStruct *pTopologyNode;
	struct AcdbDynamicTblStruct *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicSecondaryKeyNodeStructEx AcdbDynamicSecondaryKeyNodeTypeEx;
#include "acdb_begin_pack.h"
struct AcdbDynamicSecondaryKeyNodeStructEx{
	AcdbDynamicSecondaryKeyType *pTblHead;
	AcdbDynamicSecondaryKeyType *pTblTail;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicTblStructEx AcdbDynamicTblTypeEx;
#include "acdb_begin_pack.h"
struct AcdbDynamicTblStructEx{
	AcdbDataLookupTblKeyType *pKey;
	struct AcdbDynamicPrimaryKeyNodeStructEx *pPrimaryKeyNode;
	struct AcdbDynamicTblStructEx *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicPrimaryKeyStruct AcdbDynamicPrimaryKeyType;
#include "acdb_begin_pack.h"
struct AcdbDynamicPrimaryKeyStruct{
	AcdbDataLookupPrimaryKeyType *pKey;
	struct AcdbDynamicSecondaryKeyNodeStructEx *pSecondaryNode;
	struct AcdbDynamicPrimaryKeyStruct *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicPrimaryKeyNodeStructEx AcdbDynamicPrimaryKeyNodeTypeEx;
#include "acdb_begin_pack.h"
struct AcdbDynamicPrimaryKeyNodeStructEx{
	AcdbDynamicPrimaryKeyType *pTblHead;
	AcdbDynamicPrimaryKeyType *pTblTail;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicDeltaInstanceStruct AcdbDynamicDeltaInstanceType;
#include "acdb_begin_pack.h"
struct AcdbDynamicDeltaInstanceStruct{
	AcdbDeltaDataKeyType *pKey;
	AcdbDynamicUniqueDataType *pDataNode;
	struct AcdbDynamicDeltaInstanceStruct *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicTblNodeStruct AcdbDynamicTblNodeType;
#include "acdb_begin_pack.h"
struct AcdbDynamicTblNodeStruct{
	AcdbDynamicTblType *pTblHead;
	AcdbDynamicTblType *pTblTail;
}
#include "acdb_end_pack.h"
;
typedef struct AcdbDynamicTblNodeStructEx AcdbDynamicTblNodeTypeEx;
#include "acdb_begin_pack.h"
struct AcdbDynamicTblNodeStructEx{
	AcdbDynamicTblTypeEx *pTblHead;
	AcdbDynamicTblTypeEx *pTblTail;
}
#include "acdb_end_pack.h"
;
typedef struct AcdbDynamicDeltaFileDataStruct AcdbDynamicDeltaFileDataType;
#include "acdb_begin_pack.h"
struct AcdbDynamicDeltaFileDataStruct{
	AcdbDynamicDeltaInstanceType *pFileHead;
	AcdbDynamicDeltaInstanceType *pFileTail;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicAdieTblStruct AcdbDynamicAdieTblType;
#include "acdb_begin_pack.h"
struct AcdbDynamicAdieTblStruct{
	AcdbDataLookupKeyType *pKey;
	AcdbDynamicUniqueDataType *pDataNode;
	struct AcdbDynamicAdieTblStruct *pNext;
}
#include "acdb_end_pack.h"
;

typedef struct AcdbDynamicAdieTblNodeStruct AcdbDynamicAdieTblNodeType;
#include "acdb_begin_pack.h"
struct AcdbDynamicAdieTblNodeStruct{
	AcdbDynamicAdieTblType *pAdieTblHead;
	AcdbDynamicAdieTblType *pAdieTblTail;
}
#include "acdb_end_pack.h"
;
//linked list node
#include "acdb_begin_pack.h"
struct node
{
	int data;
	struct node *next;
}
#include "acdb_end_pack.h"
;
//append the value to the list
int append(int num,struct node **head );

//this function looks for the index location and updates with new value if the index is not availble appeds the new value
int update(int num,int index,struct node **head );

//this function looks for element in the list
int Find(int num,struct node **head ,int *index);

//this function gets element at random position
int GetAt(int index,struct node **head );

/* Function to delete the entire linked list */
void deleteList(struct node** head_ref);

/* ---------------------------------------------------------------------------
* Function Declarations and Documentation
*--------------------------------------------------------------------------- */

int32_t IsDataNodeOnHeap(uint32_t *pParamId,
	AcdbDynamicDataNodeType *pUniqDataOnHeap
	);

int32_t IsDataOnHeap(uint32_t *ulParamId,
	uint8_t *pUniqueData,
	const uint32_t ulDataLen,
	AcdbDynamicDataNodeType *pUniqDataOnHeap
	);

int32_t FindTableNodeOnHeap(AcdbDataLookupKeyType *pKey,
	AcdbDynamicTblNodeType *pTblOnHeap,
	AcdbDynamicTblType **ppTblNode
	);
int32_t FindTopologyNodeOnHeap(uint32_t *pModuleId,
	uint32_t *pParamId,
	AcdbDynamicTblType *pTblOnHeap,
	AcdbDynamicTopologyType **ppTblNode
	);

int32_t FindTopologyNodeOnHeapEx(uint32_t *pModuleId,
	uint32_t *pParamId,
	AcdbDynamicSecondaryKeyType *pSecondaryKeyNode,
	AcdbDynamicTopologyType **ppTopNode
	);
int32_t FindDataNodeOnHeap(uint32_t *pParamId,
	uint8_t *pInData,
	const uint32_t InDataLen,
	AcdbDynamicDataNodeType *pUniqDataOnHeap,
	AcdbDynamicUniqueDataType **ppDataNode
	);
int32_t FindTableNodeOnHeapEx(AcdbDataLookupCVDKeyType *pKey,
	AcdbDynamicTblNodeTypeEx *pTblOnHeap,
	AcdbDynamicTblTypeEx **ppTblNode
	);
   int32_t FindPrimaryKeyNodeOnHeapEx(AcdbDataLookupCVDKeyType *pKey,
	AcdbDynamicPrimaryKeyNodeTypeEx *pPrimaryKeyOnHeap,
	AcdbDynamicPrimaryKeyType **ppPrimaryNode
	);
   int32_t FindSecondaryKeyNodeOnHeapEx(AcdbDataLookupCVDKeyType *pKey,
	AcdbDynamicSecondaryKeyNodeTypeEx *pSecondaryKeyOnHeap,
	AcdbDynamicSecondaryKeyType **ppSecondaryNode
	);
   int32_t FindSecondaryKeyNodeFromCVDKeyOnHeapEx(AcdbDataLookupCVDKeyType *pKey,
	AcdbDynamicTblNodeTypeEx *pTblOnHeap,
	AcdbDynamicSecondaryKeyType **ppSecondaryNode
	);
int32_t CreateDataNodeOnHeap(uint32_t *pParamId,
	uint8_t *pInData,
	const uint32_t InDataLen,
	AcdbDynamicDataNodeType *pUniqDataNode,
	AcdbDynamicUniqueDataType **pDataNode
	);

int32_t CreateTableNodeOnHeap(AcdbDataLookupKeyType *pKey,
	AcdbDynamicTblNodeType *pTblOnHeap,
	AcdbDynamicTblType **pTblNode
	);

int32_t CreateTableNodeOnHeapEx(AcdbDataLookupCVDKeyType *pKey,
	AcdbDynamicTblNodeTypeEx *pTblOnHeap,
	AcdbDynamicTblTypeEx **pTblNode
	);
   int32_t GetSecondaryNodeFromCVDKeyOnHeapEx(AcdbDataLookupCVDKeyType *pKey,
	AcdbDynamicTblNodeTypeEx *pTblOnHeap,
	AcdbDynamicSecondaryKeyType **pSecondaryKeyNode
	);

int32_t CreateTopologyNodeOnHeap(uint32_t *pModuleId,
	uint32_t *pParamId,
	AcdbDynamicUniqueDataType *pDataNode,
	AcdbDynamicTblType *pTbNodelOnHeap
	);
int32_t CreateTopologyNodeOnHeapEx(uint32_t *pModuleId,
	uint32_t *pParamId,
	AcdbDynamicUniqueDataType *pDataNode,
	AcdbDynamicSecondaryKeyType *pSecondaryKeyNodeOnHeap
	);
int32_t FreeTableNodeEx(AcdbDataLookupCVDKeyType *pKey,
	AcdbDynamicTblNodeTypeEx *pTblOnHeap
	);
int32_t FreePrimaryNodeEx(AcdbDataLookupCVDKeyType *pCVDKey,
	AcdbDynamicPrimaryKeyNodeTypeEx *pTblOnHeap
	);
int32_t FreeSecondaryNodeEx(AcdbDataLookupCVDKeyType *pKey,
	AcdbDynamicSecondaryKeyNodeTypeEx *pTblOnHeap
	);
int32_t UpdateDeltaDataNodeOnHeap(AcdbDeltaDataKeyType nKeyToCheck,
	AcdbDynamicUniqueDataType *pDataNode,
	AcdbDynamicDeltaFileDataType *pDeltaData
	);

int32_t FreeTopologyNode(uint32_t *pModuleId,
	uint32_t *pParamId,
	AcdbDynamicTblType *pTblNode,
	uint32_t *fReeTblResult
	);

   int32_t FreeTopologyNodeEx(uint32_t *pModuleId,
	uint32_t *pParamId,
	AcdbDynamicSecondaryKeyType *pSecondaryKeyNode,
	uint32_t *fReeTblResult
	);

int32_t CompareDeltaDataNodeKeys(AcdbDeltaDataKeyType *pKey1,
	AcdbDeltaDataKeyType *pKey2
	);

int32_t CreateDeltaDataNodeOnHeap(AcdbDeltaDataKeyType *pKeyToCreate,
	AcdbDynamicUniqueDataType *pDataNode,
	AcdbDynamicDeltaFileDataType **pDeltaDataNode
	);

int32_t FreeDeltaDataNode(AcdbDeltaDataKeyType nKeyToCheck,
	AcdbDynamicDeltaFileDataType *pDeltaData
	);

int32_t FreeTableNode(AcdbDataLookupKeyType *pKey,
	AcdbDynamicTblNodeType *pTblOnHeap
	);

int32_t FreeDataNode(uint32_t *pParamId,
	AcdbDynamicDataNodeType *pDataOnHeap
	);

//int32_t CompareStaticData(uint32_t *pModuleId,
// uint32_t *pParamId,
// AcdbDataUniqueDataNodeType **ppCalTbl,
// AcdbDataTopologyType *pTopology,
// const uint32_t nTopologyEntries,
// uint8_t *pInBufPtr,
// const uint32_t InBufLen
// );

//int32_t GetDataCal(AcdbDataLookupKeyType *pKey,
// uint32_t *pModuleId,
// uint32_t *pParamId,
// AcdbDynamicTblNodeType *pTbl,
// AcdbDynamicUniqueDataType **ppDataNode
// );

//int32_t IsInfoDataOnHeap(AcdbDataGeneralInfoType* pInput,
// AcdbDataGeneralInfoType* pInfoData
// );

//int32_t GetInfoDataNodeOnHeap(AcdbDataGeneralInfoType* pInput,
// AcdbDataGeneralInfoType* pInfoData
// );

//int32_t CreateInfoDataNodeOnHeap(AcdbDataGeneralInfoType* pInput,
// AcdbDataGeneralInfoType** ppInfoData
// );

//int32_t FreeInfoDataNodeOnHeap(AcdbDataGeneralInfoType** ppInfoData
// );

int32_t FindAdieTableNodeOnHeap(AcdbDataLookupKeyType *pKey,
	AcdbDynamicAdieTblNodeType *pTblOnHeap,
	AcdbDynamicAdieTblType **ppTblNode
	);

//int32_t FreeAdieTableNode(AcdbDataLookupKeyType *pKey,
// AcdbDynamicAdieTblNodeType *pTblOnHeap
// );

int32_t CreateAdieTableNodeOnHeap(AcdbDataLookupKeyType *pKey,
	AcdbDynamicUniqueDataType *pDataNode,
	AcdbDynamicAdieTblNodeType *pTblOnHeap
	);

//int32_t GetDataNodeOnHeap(uint32_t *pParamId,
// AcdbDynamicDataNodeType *pUniqDataOnHeap,
// AcdbDynamicUniqueDataType **ppDataNode
// );

#endif//ACDB_LINKED_LIST_H


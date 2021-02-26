////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  chicommon.h
/// @brief Defines Chi Common Tag interface
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CHICOMMON_H
#define CHICOMMON_H

#include "camxcdktypes.h"
#include "chicommontypes.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/// @brief Pack data structures to ensure consistent layout.
#pragma pack(push, 8)


/// @brief This is setup to allow us to duplicate the definitions of metadata for certain pools use
enum class MetadataPoolSection : UINT32
{
    // Result is implicit, and 0x0000
    Input = 0x0800, // Input must be bitwise exclusive to the other so we can uniquely identify for both tags and props
    Usecase = 0x2000,
    Static  = 0x7000,
    // 0x3000 - Result Prop
    // 0x4000 - Internal Prop
    // 0x5000 - Usecase Prop
    // 0x6000 - DebugData Prop
};

/// Mask to OR with tags to represent input pool residency
static const UINT32 InputMetadataSectionMask = static_cast<UINT32>(MetadataPoolSection::Input) << 16;

/// Mask to OR with tags to represent usecase pool residency
static const UINT32 UsecaseMetadataSectionMask = static_cast<UINT32>(MetadataPoolSection::Usecase) << 16;

typedef VOID*          CHIHANDLE;            ///< Handle to a chi context
typedef VOID*          CHIMETADATAHANDLE;    ///< Handle to a chi meta data
typedef VOID*          CHIFENCEHANDLE;       ///< Handle to a chi fence
typedef VOID*          CHIREQUESTPAYLOAD;    ///< Handle to a request payload

typedef enum ChiMetadataType
{
    ChiMetadataStatic   = 0x1,          ///< Static pool of metadata
    ChiMetadataDynamic  = 0x2,          ///< The non-static metadata
    ChiMetadataEnd      = 0xFFFFFFFF    ///< Mark the end
} CHIMETADATATYPE;

/// @brief Define enum to tag visibility
typedef enum ChiTagSectionVisibility
{
    ChiTagSectionVisibleToOEM       = 0x1,              ///< VenderTag section visible to OEM
    ChiTagSectionVisibleToFramework = 0x2,              ///< VenderTag section visible to Android framework
    ChiTagSectionVisibleToAll       = 0xFFFFFFFF,       ///< VenderTag section visible to All
} CHITAGSECTIONVISIBILITY;

/// @brief Encapsulates essential data describing a vendor tag
typedef struct ChiVendorTagData
{
    const CHAR*     pVendorTagName; ///< The string representation of the vendor tag name
    UINT8           vendorTagType;  ///< The type of the vendor tag
    SIZE_T          numUnits;       ///< The number of units of vendorTagType needed to program this tag
} CHIVENDORTAGDATA;

/// @brief Encapsulates essential data describing a vendor tag section
typedef struct ChiVendorTagSectionData
{
    const CHAR*             pVendorTagSectionName;              ///< The string representing the vendor tag section name
    UINT32                  firstVendorTag;                     ///< The first vendor tag in the vendor tag section
    UINT32                  numTags;                            ///< The number of vendor tags in the section
    CHIVENDORTAGDATA*       pVendorTagaData;                    ///< An array of vendor tag data arrays
    CHITAGSECTIONVISIBILITY visbility;                          ///< Visibility of this tag section
} CHIVENDORTAGSECTIONDATA;

/// @brief Camera  vendor tags capabilities. These capabilities will be used to populate suported vendor tags by node.
typedef struct ChiVendorTagInfo
{
    CHIVENDORTAGSECTIONDATA*    pVendorTagDataArray;             ///< An array of vendor tag section
    UINT32                      numSections;                     ///< The number of vendor tag section in pVendorTagDataArray
} CHIVENDORTAGINFO;

typedef struct ChiQueryVendorTag
{
    UINT32               size;            ///< The size of this structure
    CHIVENDORTAGINFO*    pVendorTagInfo;  ///< Pointer to the vendor tag section
} CHIQUERYVENDORTAG;

/// @brief Defines the data structure for the Chi tag data
typedef struct ChiTagData
{
    UINT32  size;       ///< The size of this structure
    UINT64  requestId;  ///< The request id associated with this tag
    VOID*   pData;      ///< The actual data list
    SIZE_T  dataSize;   ///< The count of data
    UINT64  offset;     ///< The offset from the request id to request data from
    BOOL    negate;     ///< Flag to determine if the offset value should be treated as negative
} CHITAGDATA;

/// @brief Vendor tag base information.
typedef struct ChiVendorTagBaseInfo
{
    UINT32      size;           ///< Size of this structure
    UINT32      vendorTagBase;  ///< Chi assigned runtime vendor tag base for the component
    const CHAR* pComponentName; ///< Name of component associated with the vendor tag base
    const CHAR* pTagName;       ///< The tagName to query, this could be NULL, if it's null,
                                ///  the API just query for the base of giving section(pComponentName)
} CHIVENDORTAGBASEINFO;

/// @brief Metadata list information.
typedef struct ChiMetadataInfo
{
    UINT32              size;        ///< Size of this structure
    CHIHANDLE           chiSession;  ///< Chi driver handle that node can use to callback into Chi
    UINT32              tagNum;      ///< Number of tag in the tagList
    UINT32*             pTagList;    ///< pointer to list of tags
    CHITAGDATA*         pTagData;    ///< pointer to Chi tag data
    CHIMETADATATYPE     metadataType;///< The type of metadata to query for
} CHIMETADATAINFO;

/// @brief Specifies the type of the fence
typedef enum ChiFenceType
{
    ChiFenceTypeInternal = 0,   ///< Represents a fence internal to CHI
    ChiFenceTypeNative,         ///< Represent a native fence
    ChiFenceTypeEGL             ///< Represents an EGL sync object
} CHIFENCETYPE;

typedef struct ChiFenceCreateParams
{
    UINT32              size;           ///< Size of this structure
    CHIFENCETYPE        type;           ///< Type of the fence
    union
    {
        UINT64          eglSync;        ///< EGL sync object (need to cast to EGLSyncKHR)
        INT             nativeFenceFD;  ///< Native fence file descriptor
    };
} CHIFENCECREATEPARAMS;

/// @brief Request done structure used for callback for a chi fence
typedef struct ChiFenceCallbackInfo
{
    UINT32    size;              ///< Size of this structure
    CHIHANDLE hChiSession;       ///< Chi driver handle that node can use to callback into Chi
    UINT64    frameNum;          ///< Frame number for current request
    INT32     processSequenceId; /// Sequence Id
    VOID*     pUserData;         ///< Userdata payload used for the callback function
} CHIFENCECALLBACKINFO;

/// @brief This enumerates the data request types possible
typedef enum  ChiDataRequestType
{
    ChiFetchData,     ///< Fetch data
    ChiIterateData,   ///< Iterate data
} CHIDATAREQUESTTYPE;

/// @brief This enumerates the data sources available.
typedef enum  ChiDataSource
{
    ChiDataGyro,      ///< Gyro Service
    ChiDataAccel,     ///< Accelerometer Service
    ChiTuningManager, ///< Tuning manager
    ChiDataMax,       ///< Data Source Max
} CHIDATASOURCETYPE;

/// @brief This enumerates the data sources configuration.
typedef struct  ChiDataSourceConfig
{
    CHIDATASOURCETYPE sourceType; ///< Data source type
    VOID*             pConfig;    ///< Data source config pointer
} CHIDATASOURCECONFIG;

/// @brief This enumerates the data sources handle
typedef struct ChiDataSourceHandle
{
    CHIDATASOURCETYPE dataSourceType;  ///< Data Source type
    VOID*             pHandle;         ///< Internal data handle
} CHIDATASOURCE;

/// @brief This defines the Chi data request payload structure
typedef struct ChiDataRequest
{
    ChiDataRequestType requestType;   ///< Data request type (fetch/iterate)
    CHIREQUESTPAYLOAD  hRequestPd;    ///< Handle to the request payload
    INT                index;         ///< Index of the data to fetch while iterating through a appropriate data source.
} CHIDATAREQUEST;

typedef CHIDATASOURCE* CHIDATASOURCEHANDLE;  ///< Handle to a chi data source

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIFENCECALLBACK
///
/// @brief  Chi fence async wait callback signature
///
/// @param  hChiFence   Handle to Chi fence this callback is called for
/// @param  pUserData   User data provided when waiting on the fence
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*PFNCHIFENCECALLBACK)(CHIFENCEHANDLE hChiFence, VOID* pUserData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIGETMETADATA
///
/// @brief  Gets a list of metadata information based on metadata tags.
///
/// The tag can be an Android tag or a vendor tag. If the metadata information associated with the tag is not published,
/// Chi returns those tags as unpublished when this function returns. The component can add them in the dependency reporting.
///
/// @param  pMetadataInfo   Pointer to a structure that defines the list of metadata information requested by the component.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIGETMETADATA)(CHIMETADATAINFO* pMetadataInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIGETMETADATACOUNT
///
/// @brief  Gets a metadata count information based on metadata tag id.
///
/// The tag can be an Android tag or a vendor tag. If the metadata information associated with the tag is not published,
/// Chi returns count as zero.
///
/// @param  requestId         The request id for current request
/// @param  tagId             The id of tag to get from camera system
/// @param  metadataType      Metadata type Static/Dynamic
/// @param  hChiSession       The Chi session handle
/// @param  allowSticky       For input pool, allow use of sticky tag data, or force use of count in request metadata
///
/// @return tag count value
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef SIZE_T (*PFNCHIGETMETADATACOUNT)(UINT64            requestId,
                                         UINT32            tagId,
                                         CHIMETADATATYPE   metadataType,
                                         CHIHANDLE         hChiSession,
                                         BOOL              allowSticky);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHISETMETADATA
///
/// @brief  Sets a list of metadata information based on metadata tags.
///
/// The tag can be an Android tag or a vendor tag. When published, Chi driver will notify all other component that reported
/// these tags as dependencies.
///
/// @param  pMetadataInfo Pointer to a structure that defines the list of metadata information published by the component.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHISETMETADATA)(CHIMETADATAINFO* pMetadataInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIGETVENDORTAGBASE
///
/// @brief  Get the base of vendor tags of other component.
///
/// It serves two purposes.
/// 1. Get the vendor tag base for the tags exported from the chi component.
/// The tag can be from Chi default components or other third-party custom component on which this component is dependent
/// for request processing. The component uses this base and the actual tag offset to derive the tag ID (= base + offset) to
/// report the dependencies.
/// 2. Get the tag value for a specific tag(string).
/// The tag could be any tags populated by other components.
///
/// @param  pVendorTagBaseInfo  Pointer to a structure that defines the run-time Chi assigned vendor tag base.
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIGETVENDORTAGBASE)(CHIVENDORTAGBASEINFO* pVendorTagBaseInfo);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PFNCHIQUERYVENDORTAG
///
/// @brief  Query the vendor tag exported from the Chi node
///
/// It should export the vendor tag supported by componennt. Chi driver appends these vendor tags to overall vendor tag superset.
/// This superset comprises all enumerated vendor tags from other extension components, including Chi default
/// component-supported vendor tags.
///
/// @param  pQueryVendorTag Pointer to a structure that returns the exported vendor tag
///
/// @return CDKResultSuccess if success or appropriate error code.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CDKResult (*PFNCHIQUERYVENDORTAG)(CHIQUERYVENDORTAG* pQueryVendorTag);

#pragma pack(pop)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CHICOMMON_H


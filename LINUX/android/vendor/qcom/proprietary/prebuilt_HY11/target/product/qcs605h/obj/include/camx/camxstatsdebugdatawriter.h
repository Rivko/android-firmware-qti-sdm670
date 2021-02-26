////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  camxstatsdebugdatawriter.h
/// @brief Debug data writer class declaration & implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXSTATSDEBUGDATAWRITER_H
#define CAMXSTATSDEBUGDATAWRITER_H

#include "camxstatsdebugdatatypes.h"

enum class WriterId : UINT8
{
    AECWriter,
    AWBWriter,
    AFWriter,
    ASDWriter,
    AFDWriter,
    OtherWriter,
    WriterMax
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief The class that implements things for the DebugDataWriter class.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DebugDataWriter
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// DebugDataWriter
    ///
    /// @brief  Constructor
    ///
    /// @param  writerType       Specify the id of writer
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    DebugDataWriter(
        WriterId writerType)
        : m_pBuffer(NULL)
        , m_bufferSize(0)
        , m_bytesWritten(0)
    {
        m_writerId = writerType;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~DebugDataWriter
    ///
    /// @brief  Destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual ~DebugDataWriter() { };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SetBufferPointer
    ///
    /// @brief  Set the buffer pointer
    ///
    /// @param  pBuffer       Pointer to buffer allocated outside
    /// @param  bufferSize    The size of the buffer
    ///
    /// @return  Success or failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult SetBufferPointer(
        BYTE*  pBuffer,
        SIZE_T bufferSize)
    {
        CamxResult result = CamxResultSuccess;

        m_pBuffer       = pBuffer;
        m_bytesWritten  = 0;

        if (NULL == pBuffer)
        {
            m_bufferSize = 0;
        }
        else
        {
            CAMX_ASSERT_MESSAGE(bufferSize >= sizeof(DebugDataTag),
                "The buffer is not sufficiently large to hold UnusedSpace tag");
            if (bufferSize < sizeof(DebugDataTag))
            {
                result = CamxResultEInvalidArg;
            }
            else
            {
                m_bufferSize = bufferSize - sizeof(DebugDataTag);   // Reserved for an unused tag
                result = SealUnusedSpace();
            }
            if (CamxResultSuccess == result)
            {
                result = AddTypeDefinitionTags();
            }
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsSameBufferPointer
    ///
    /// @brief  Compare the buffer pointer
    ///
    /// @param  pBuffer       Pointer to buffer allocated outside
    /// @param  bufferSize    The size of the buffer
    ///
    /// @return  True if same buffer pointer is currently in use
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE BOOL IsSameBufferPointer(
        BYTE*   pBuffer,
        SIZE_T  bufferSize)
    {
        return ((pBuffer == m_pBuffer) && (bufferSize == (m_bufferSize + sizeof(DebugDataTag))));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetSizeWritten
    ///
    /// @brief      Gets the size of the data written to the buffer.
    ///
    /// @return     Size in bytes.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    SIZE_T GetSizeWritten()
    {
        return m_bytesWritten;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetSize
    ///
    /// @brief      Get the total size of the buffer which was set via SetBufferPointer()
    ///
    /// @return     Size in bytes.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    SIZE_T GetSize()
    {
        return (m_bufferSize + sizeof(DebugDataTag));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// IsCustomType
    ///
    /// @brief  If type is custom type
    ///
    /// @param  type    The tag type to be check
    ///
    /// @return BOOL
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL IsCustomType(
        DebugDataTagType type)
    {
        return ((type >= DebugDataTagType::CustomStart) && (type <= DebugDataTagType::CustomEnd));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddTypeDefinitionTag
    ///
    /// @brief  Add a type definition tag
    ///
    /// @param  type    The tag type to be added
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult AddTypeDefinitionTag(
        DebugDataTagType type)
    {
        CamxResult      result          = CamxResultSuccess;
        BYTE*           pBuffer         = GetCurBufferPos();    // Current buffer position
        DebugDataTag*   pTag            = NULL;

        pTag                = reinterpret_cast<DebugDataTag*>(pBuffer);
        m_bufferRemaining   = static_cast<INT32>(m_bufferSize - m_bytesWritten);
        m_bufferRemaining  -= static_cast<INT32>(sizeof(DebugDataTag));
        m_fieldsID          = 0;
        m_sanitySize        = 0;

        if (NULL == pBuffer)
        {
            result = CamxResultEInvalidPointer;
            return result;
        }
        if (m_bufferRemaining <= 0)
        {
            result = CamxResultENoMemory;
            CAMX_LOG_ERROR(CamxLogGroupStats, "Insufficient size to write data");
            return result;
        }

        // Have our specific implementation populate our buffer with type information
        result = PopulateTypeDefinitionTag(type);
        if (CamxResultSuccess != result)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Error! Unrecognized type or the given type is not a custom type: %u", type);
            result = CamxResultEUnsupported;
        }

        if (CamxResultSuccess == result)
        {
            if (m_sanitySize != GetSizeOfType(type))
            {
                CAMX_ASSERT_ALWAYS_MESSAGE(
                    "Implementation Error! Typedef tag-ID: 0x%x, total-field size: %u bytes. "
                    "Expected size returned by GetSizeOfType(): %u bytes. This is caused by incorrect code change by developer",
                    static_cast<DebugDataTagID>(type),
                    m_sanitySize,
                    GetSizeOfType(type));
                result = CamxResultENotImplemented;
            }
            else
            {
                pTag->id        = static_cast<DebugDataTagID>(type);
                pTag->type      = DebugDataTagType::TypeDefinition;
                pTag->count     = m_fieldsID;
                m_bytesWritten  = m_bufferSize - m_bufferRemaining;

                result = SealUnusedSpace();
                CAMX_LOG_VERBOSE(CamxLogGroupStats, "Typedef tag-ID: 0x%x, size: %u bytes", pTag->id, GetSizeOfType(type));
            }
        }
        else
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Error, failed to add the type definition tag-ID 0x%x", type);
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PopulateTypeDefinitionTag
    ///
    /// @brief  Adds all fields for this specific type of tag. This is a pure virtual function.
    ///
    /// @param  type    The tag type to be added
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult PopulateTypeDefinitionTag(
        DebugDataTagType type) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddTypeDefinitionTags
    ///
    /// @brief  Add a set of type definition tags
    ///
    /// @param  typeBegin    The first tag type to be added
    /// @param  typeEnd      The last tag type to be added
    ///
    /// @return  CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult AddTypeDefinitionTags(
        DebugDataTagType typeBegin,
        DebugDataTagType typeEnd)
    {
        CamxResult result = CamxResultSuccess;
        UINT16 begin      = static_cast<UINT16>(typeBegin);
        UINT16 end        = static_cast<UINT16>(typeEnd);

        if (begin >= end)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Error, invalid input!");
            result = CamxResultEInvalidArg;
        }
        else
        {
            for (UINT16 type = begin; type <= end; type++)
            {
                BOOL bDeprecated = FALSE;
                // Ensure the type definition which we are adding is not deprecated
                for (UINT32 i = 0; i < CAMX_ARRAY_SIZE(DebugDataTagTypeDeprecated); i++)
                {
                    if (type == static_cast<UINT16>(DebugDataTagTypeDeprecated[i]))
                    {
                        CAMX_LOG_VERBOSE(CamxLogGroupStats, "Skipping deprecated tag type %u", type);
                        bDeprecated = TRUE;
                        break;
                    }
                }

                if (FALSE == bDeprecated)
                {
                    result = AddTypeDefinitionTag(static_cast<DebugDataTagType>(type));
                }
            }
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddTag
    ///
    /// @brief  Add tag to header
    ///
    /// @param  id          The tag id of the data
    /// @param  type        The tag type of the data
    /// @param  count       Count of the data elements. If the count is zero then nothing will be written
    /// @param  pData       Pointer to the data to be written to the debug data buffer
    /// @apram  dataSize    The size of the data pointed to by pData parameter
    ///
    /// @return DebugDataTagHeader
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult AddDataTag(
        DebugDataTagID   id,
        DebugDataTagType type,
        TagCount         count,
        VOID*            pData,
        SIZE_T           dataSize)
    {
        CamxResult      result          = CamxResultSuccess;
        DebugDataTag*   pTag            = reinterpret_cast<DebugDataTag*>(GetCurBufferPos());
        SIZE_T          sizeLeft        = m_bufferSize - m_bytesWritten;
        SIZE_T          expectedSize    = GetSizeOfType(type) * count;

        if (DebugDataTagID::UnusedSpace == id)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Called with invalid argument of UnusedSpace");
            result = CamxResultEInvalidArg;
        }
        else if (NULL == pTag)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Debug data buffer is NULL");
            result = CamxResultEInvalidState;
        }
        else if (NULL == pData && dataSize > 0)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats,
                           "Error! Tag ID %u has no payload but tag size indicates presence of a payload", id);
            result = CamxResultEInvalidArg;
        }
        else if (sizeof(DebugDataTag) + dataSize > sizeLeft)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats,
                "Insufficient size to write data for tag ID: %u. Free space left: %zu bytes. payload size: %u bytes",
                id, sizeLeft, dataSize);
            result = CamxResultEOverflow;
        }

        if (CamxResultSuccess == result)
        {
            // Ensure that the tag ID is not deprecated
            for (UINT32 i = 0; i < CAMX_ARRAY_SIZE(DebugDataTagIDDeprecated); i++)
            {
                if (id == DebugDataTagIDDeprecated[i])
                {
                    CAMX_LOG_ERROR(CamxLogGroupStats, "Deprecated tag ID %u!", id);
                    result = CamxResultEUnsupported;
                    break;
                }
            }
        }

        if (CamxResultSuccess == result)
        {
            if (count == 0)
            {
                CAMX_LOG_INFO(CamxLogGroupStats, "Skipping tag ID: %u because the data count is zero", id)
                result = CamxResultSuccess;
            }
            else if (dataSize != expectedSize)
            {
                CAMX_LOG_ERROR(CamxLogGroupStats,
                    "Payload size of %u does not match the expected size of %u. Tag ID %u, Type: %u",
                    dataSize, expectedSize, id, type);
                result = CamxResultEInvalidArg;
            }
            else
            {
                CAMX_LOG_VERBOSE(CamxLogGroupStats, "Adding debug data tag ID: %u, size: %u bytes, count: %u",
                    id, dataSize, count);

                pTag->id    = id;
                pTag->type  = type;
                pTag->count = count;
                m_bytesWritten += sizeof(DebugDataTag);
                if (NULL != pData)
                {
                    CamX::Utils::Memcpy(GetCurBufferPos(), pData, dataSize);
                    m_bytesWritten += dataSize;
                }

                result = SealUnusedSpace();
            }
        }
        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddTwoDimArrayDataTag
    ///
    /// @brief  Add a two dim array data tag to debug data
    ///
    /// @param  id              The tag id of the data
    /// @param  dimension1      Dimension 1 of two dim array
    /// @param  dimension2      Dimension 2 of two dim array
    /// @param  elementType     The element type of the array
    /// @param  pData           The pointer of the data
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult AddTwoDimArrayDataTag(
        DebugDataTagID      id,
        UINT16              dimension1,
        UINT16              dimension2,
        DebugDataTagType    elementType,
        VOID*               pData)
    {
        CamxResult              result          = CamxResultSuccess;
        DebugDataTag*           pTag            = reinterpret_cast<DebugDataTag*>(GetCurBufferPos());
        DebugData2DArrayHeader* p2DArrayHeader  = NULL;
        SIZE_T                  arraySize       = 0;
        SIZE_T                  totalSize       = 0;

        if (NULL == pTag)
        {
            result = CamxResultEInvalidPointer;
            return result;
        }

        arraySize   = dimension1 * dimension2 * GetSizeOfType(elementType);
        totalSize   = sizeof(DebugDataTag) + sizeof(DebugData2DArrayHeader) + arraySize;
        if (m_bytesWritten + totalSize > m_bufferSize)
        {
            result = CamxResultENoMemory;
            CAMX_LOG_ERROR(CamxLogGroupStats, "Insufficient size to write data");
            return result;
        }

        // Write the tag header
        pTag->id                        = id;
        pTag->type                      = DebugDataTagType::TwoDimArray;
        pTag->count                     = static_cast<TagCount>(sizeof(DebugData2DArrayHeader) + arraySize);
        m_bytesWritten                 += sizeof(DebugDataTag);
        // Write the array header
        p2DArrayHeader                  = reinterpret_cast<DebugData2DArrayHeader*>(GetCurBufferPos());
        p2DArrayHeader->underlyingType  = elementType;
        p2DArrayHeader->dimension1      = dimension1;
        p2DArrayHeader->dimension2      = dimension2;
        m_bytesWritten                 += sizeof(DebugData2DArrayHeader);
        // Write the array payload
        CamX::Utils::Memcpy(GetCurBufferPos(), pData, arraySize);
        m_bytesWritten                 += arraySize;
        return result;
    }

protected:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddTypedefFieldArray
    ///
    /// @brief  Declares a field of a type. This is for arrays
    ///
    /// @param  fieldType   A field type from our predeclared list
    /// @param  count       The number of fields. This should be greater than or equal to 1
    ///
    /// @return Size of data written in this field
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL AddTypedefFieldArray(
        DebugDataTagType    fieldType,
        UINT16              count)
    {
        BOOL result = TRUE;

        if (static_cast<SIZE_T>(m_bufferRemaining) < sizeof(TypedefFieldInfo))
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Insufficient size to write data");
            result = FALSE;
        }
        else if (count <= 0)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Invalid field count of %u. Value must be greater than zero", count);
            result = FALSE;
        }
        else if (DebugDataTagType::TypeDefinition == fieldType)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Invalid field type! Cannot use typedefs as a field type");
            result = FALSE;
        }

        TypedefFieldInfo* pField = reinterpret_cast<TypedefFieldInfo*>(m_pBuffer + m_bufferSize - m_bufferRemaining);
        pField->fieldID    = ++m_fieldsID;
        pField->type       = fieldType;
        pField->count      = static_cast<TypedefFieldLen>(count);
        m_bufferRemaining -= static_cast<INT32>(sizeof(TypedefFieldInfo));
        m_sanitySize      += (count * GetSizeOfType(fieldType));

        return TRUE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddTypedefField2DArray
    ///
    /// @brief  Declares a field of a type in two dimensions
    ///
    /// @param  fieldType   A field type from our predeclared list
    /// @param  dimension1  Count of the elements of the 2D array's first dimension (number of rows)
    /// @param  dimension2  Count of the elements of the 2D array's first dimension (number of columns)
    ///
    /// @return Size of data written in this field
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE BOOL AddTypedefField2DArray(
        DebugDataTagType    fieldType,
        UINT16              dimension1,
        UINT16              dimension2)
    {
        if (static_cast<SIZE_T>(m_bufferRemaining) < 2 * sizeof(TypedefFieldInfo))
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Insufficient size to write data");
            return FALSE;
        }
        else if (dimension1 <= 0 || dimension2 <= 0)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Invalid dimensions for the 2D array! Values must be greater than zero");
            return FALSE;
        }

        TypedefFieldInfo* pField = reinterpret_cast<TypedefFieldInfo*>(m_pBuffer + m_bufferSize - m_bufferRemaining);
        pField->fieldID    = ++m_fieldsID;
        pField->type       = DebugDataTagType::TwoDimArray;
        pField->count      = static_cast<TypedefFieldLen>(dimension1);
        m_bufferRemaining -= static_cast<INT32>(sizeof(TypedefFieldInfo));

        pField++;
        // Corner case: 2D arrays require two repeating field-info's with the same IDs. Don't increment the ID again
        pField->fieldID    = m_fieldsID;
        pField->type       = fieldType;
        pField->count      = static_cast<TypedefFieldLen>(dimension2);
        m_bufferRemaining -= sizeof(TypedefFieldInfo);
        m_sanitySize      += (dimension1 * dimension2 * GetSizeOfType(fieldType));

        return TRUE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddTypedefField
    ///
    /// @brief  Declares a field of a type
    ///
    /// @param  fieldType   A field type from our predeclared list
    ///
    /// @return Size of data written in this field
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE BOOL AddTypedefField(
        DebugDataTagType    fieldType)
    {
        return AddTypedefFieldArray(fieldType, 1);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetSizeOfType
    ///
    /// @brief  Get the size of customer type
    ///
    /// @param  type    The data type to get the size of
    ///
    /// @return Size of data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual SIZE_T GetSizeOfType(DebugDataTagType type) = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetSizeOfBuiltInType
    ///
    /// @brief  Get the size of a built-in type. Inheritors of this class should call this from their
    ///         GetSizeOfType function
    ///
    /// @param  type    The data type to get the size of
    ///
    /// @return Size of data
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT32 GetSizeOfBuiltInType(
        DebugDataTagType type)
    {
        UINT32 size = 0;
        switch(type)
        {
            case DebugDataTagType::TypeDefinition:
                // Special case: payload of a type definition tag is an array of TypedefFieldInfo[]
                size = sizeof(TypedefFieldInfo);
                break;
            case DebugDataTagType::Char:
                // Byte stream. Tag count describes the overall size of tag
                size = sizeof(CHAR);
                break;
            case DebugDataTagType::Bool:
                // Explicitly ensure that bool is one byte. Depending on implementation, BOOL/bool could be 4 bytes
                size = sizeof(UINT8);
                break;
            case DebugDataTagType::UInt8:
                size = sizeof(UINT8);
                break;
            case DebugDataTagType::UInt16:
                size = sizeof(UINT16);
                break;
            case DebugDataTagType::UInt32:
                size = sizeof(UINT32);
                break;
            case DebugDataTagType::UInt64:
                size = sizeof(UINT64);
                break;
            case DebugDataTagType::Int8:
                size = sizeof(INT8);
                break;
            case DebugDataTagType::Int16:
                size = sizeof(INT16);
                break;
            case DebugDataTagType::Int32:
                size = sizeof(INT32);
                break;
            case DebugDataTagType::Int64:
                size = sizeof(INT64);
                break;
            case DebugDataTagType::Float:
                size = sizeof(float);
                break;
            case DebugDataTagType::Double:
                size = sizeof(double);
                break;
            case DebugDataTagType::QNumber:
                size = sizeof(DebugDataQNumber);
                break;
            case DebugDataTagType::UQNumber:
                size = sizeof(DebugDataUQNumber);
                break;
            case DebugDataTagType::Rational:
                size = sizeof(DebugDataRational);
                break;
            case DebugDataTagType::URational:
                size = sizeof(DebugDataURational);
                break;
            case DebugDataTagType::TwoDimArray:
                // Byte stream. Tag count describes the overall size of tag
                // The payload contains the header 'DebugDataTwoDimArrayHeader' to describe the array dimensions
                size = sizeof(char);
                break;

            default :
                break;
        }
        return size;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetCurBufferPos
    ///
    /// @brief  Gets a pointer to the current buffer position, or NULL if no buffer is used
    ///
    /// @return Pointer to the current buffer location
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BYTE* GetCurBufferPos()
    {
        return (NULL == m_pBuffer ? m_pBuffer : (m_pBuffer + m_bytesWritten));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SealUnusedSpace
    ///
    /// @brief  Ensures that the last tag in the data is a DebugDataTagID::UnusedSpace tag, containing the remainder of the
    ///         Debug Data buffer
    ///
    /// @return Success or failure
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult SealUnusedSpace()
    {
        CamxResult      result  = CamxResultSuccess;
        DebugDataTag*   pTag    = reinterpret_cast<DebugDataTag*>(GetCurBufferPos());

        if (NULL == pTag)
        {
            CAMX_LOG_ERROR(CamxLogGroupStats, "Debug data buffer is NULL");
            result = CamxResultEInvalidState;
        }
        else
        {
            // NOTE: The UnusedSpace tag can always be added at the end of the buffer because we have adjusted the buffer size
            // to take that into account. Ignore size checks for that tag, and intentionally don't increment the bytes written
            // member variable. This tag is intended to be overwritten by the next call to AddDataTag()
            pTag->id    = DebugDataTagID::UnusedSpace;
            pTag->type  = DebugDataTagType::UInt8;
            pTag->count = static_cast<TagCount>(m_bufferSize - m_bytesWritten);
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// AddTypeDefinitionTags
    ///
    /// @brief  Add a set of type definition tags based on the implementation requirements
    ///
    /// @return  CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual CamxResult AddTypeDefinitionTags() = 0;

    WriterId    m_writerId;             ///< The writer identifier
    BYTE*       m_pBuffer;              ///< Pointer to buffer allocated outside
    SIZE_T      m_bufferSize;           ///< The size of the buffer
    SIZE_T      m_bytesWritten;         ///< The number of bytes written to the buffer

    // These are reset with each call to AddTypeDefinitionTag() and should not be referenced anywhere else
    UINT8       m_fieldsID;             ///< What's the next field ID?
    INT32       m_bufferRemaining;      ///< How many bytes are left in the buffer
    SIZE_T      m_sanitySize;           ///< Sanity check: size of the current typedef fields defined
};

#endif // CAMXSTATSDEBUGDATAWRITER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxhashmap.h
///
/// @brief Defines a hashmap class.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXHASHMAP_H
#define CAMXHASHMAP_H

#include "camxdefs.h"
#include "camxtypes.h"
#include "camxlist.h"

CAMX_NAMESPACE_BEGIN

/// @brief Configuration parameters for Hashmap. If no key/value size is set to 0, then the passed-in pointers will be
///        treated as "values" instead of pointers to values (this is similar to List behavior). If anything other than
///        pointers are to be used as key/val, then the corresponding field must be set in the HashmapParams; then the
///        passed-in key/vals will be treated as "pointers" to key/vals.
/// @todo  (CAMX-333) Add support for custom hash, equal, and key/val copy functions.
struct HashmapParams
{
    UINT    maxNumBuckets;      ///< The expected max number of buckets; if 0, a default will be used.
    FLOAT   loadFactor;         ///< The loading factor of the hash table; if 0, a default will be used.
    UINT    keySize;            ///< The size of key; if 0, VOID* will be assumed.
    UINT    valSize;            ///< The size of value; if 0, VOID* will be assumed.
    BOOL    preallocateBuckets; ///< Allocates all bucket lists at create rather than on use
    UINT    multiMap;           ///< 1 if the hashmap should operate in multimap mode, 0 otherwise. In multimap mode,
                                ///  repetitive keys are allowed in the sense that data with the same key will not overwrite
                                ///  the existing data (with the same key) but rather, add it to the hashmap. Get*() work as
                                ///  before but only the first encountered key/val pair with the given key is returned. To
                                ///  access all key/vals, Foreach should be used.
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Function to call for each data element of hash
///
/// @param pData Data referenced by hashmap entry
///
/// @return None
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef VOID (*HashmapDataOp)(
    VOID* pData);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief A hashmap Implementation. It supports any key/value type by accepting pointer to key and value. The size of the key
///        and value are set at construction time and are invariant throughout the lifetime of the object. Use may provide
///        extra configuration parameters for the behavior.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Hashmap
{
public:
    /// @brief A variable-size (fixed at configuration time) data placeholder that will hold the data for both key and value of
    ///        a pair. It's variable-size to support any key/value type. But the key and value type will be determined at
    ///        construction and will never change.
    struct HashmapNode
    {
        BYTE   data[1]; ///< Array of bytes that will be shaped based on the given key/val sizes at construction time.
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Create
    ///
    /// @brief  Static method to create an instance of Hashmap.
    ///
    /// @param  pParams List initialization parameters.
    ///
    /// @return Pointer to newly created list object on success, NULL on failure.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static Hashmap* Create(
        const HashmapParams* pParams);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Destroy
    ///
    /// @brief  Destroys this instance of Hashmap.
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Destroy();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Put
    ///
    /// @brief  Sets the value for the given key. If the key does not exist, it will be added; otherwise, the corresponding
    ///         value is overwritten.
    ///
    /// @param  pKey Pointer to the key
    /// @param  pVal Pointer to the value
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Put(
        VOID*  pKey,
        VOID*  pVal);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Get
    ///
    /// @brief  Get the value for the given key. This copies the value to the provided location.
    ///
    /// @param  pKey        Pointer to the key
    /// @param  pVal        Pointer where the value will be written to if found.
    ///
    /// @return CamxResult  CamxResultENoSuch if the key cannot be found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Get(
        VOID*   pKey,
        VOID*   pVal
        ) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetInPlace
    ///
    /// @brief  Get the pointer to the value for the given key. This can be used to modify the value in place.
    ///
    /// @param  pKey        Pointer to the key
    /// @param  ppVal       Pointer to the value in the map
    ///
    /// @return CamxResult  CamxResultENoSuch if the key cannot be found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult GetInPlace(
        VOID*   pKey,
        VOID**  ppVal
        ) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Remove
    ///
    /// @brief  Remove the key/val pair corresponding to the given key from the hashmap.
    ///
    /// @param  pKey        Pointer to the key
    ///
    /// @return CamxResult  CamxResultENoSuch if the key cannot be found.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Remove(
        VOID*  pKey);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Size
    ///
    /// @brief  Return the number of pairs in the map
    ///
    /// @return The number of pairs contained in the map
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UINT Size() const
    {
        return m_numPairs;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Empty
    ///
    /// @brief  Check if the map is empty.
    ///
    /// @return TRUE if the map is empty.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BOOL Empty() const
    {
        return (m_numPairs == 0) ? TRUE
                                 : FALSE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Clear
    ///
    /// @brief  Remove all elements from the hashmap
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Clear();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetKey
    ///
    /// @brief  Retrieve the key of the given pair (copies).
    ///
    /// @param  pNode   Pointer to a hashmap node (representing a key/val pair)
    /// @param  pKey    Pointer to where key will be copied
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult GetKey(
        HashmapNode* pNode,
        VOID*        pKey
        ) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetVal
    ///
    /// @brief  Retrieve the value of the given pair (copies).
    ///
    /// @param  pNode   Pointer to a hashmap node (representing a key/val pair)
    /// @param  pVal    Pointer to where value will be copied
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult GetVal(
        HashmapNode* pNode,
        VOID*        pVal
        ) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// GetValInPlace
    ///
    /// @brief  Retrieve a pointer to the value of the given pair (allows in-place edits).
    ///
    /// @param  pNode   Pointer to a hashmap node (representing a key/val pair)
    /// @param  ppVal   Address where the poiunter value will be copied
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult GetValInPlace(
        HashmapNode*    pNode,
        VOID**          ppVal
    ) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Foreach
    ///
    /// @brief  Iterate over list and call function for each val in map
    ///
    /// @param  func   Function to call
    /// @param  remove Remove entries as they are encountered
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Foreach(
        HashmapDataOp func,
        BOOL          remove);

private:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Hashmap
    ///
    /// @brief  Hashmap constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Hashmap() = default;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~Hashmap
    ///
    /// @brief  Hashmap destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ~Hashmap();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Initialize
    ///
    /// @brief  Initialize a newly created Hashmap object
    ///
    /// @param  pParams Hashmap initialization parameters.
    ///
    /// @return CamxResultSuccess if successful
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult Initialize(
        const HashmapParams* pParams);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FindBucket
    ///
    /// @brief  Utility method to find the bucket where the given key is expected to be.
    ///
    /// @param  pKey                Pointer to the key
    /// @param  createIfNotExists   Indicates whether the bucket should be created if it doesn't exist.
    ///
    /// @return Pointer to a LDLL representing the bucket.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LightweightDoublyLinkedList* FindBucket(
        VOID* pKey,
        BOOL  createIfNotExists
        ) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FindElement
    ///
    /// @brief  Utility method to find the list node corresponding to the given key.
    ///
    /// @param  pBucket Pointer to bucket
    /// @param  pKey    Pointer to the key
    ///
    /// @return List node representing the hash node.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LDLLNode* FindElement(
        LightweightDoublyLinkedList* pBucket,
        VOID*                        pKey
        ) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CopyKeyToNode
    ///
    /// @brief  Copy the key to the node
    ///
    /// @param  pNode   Pointer to a hashmap node (representing a key/val pair)
    /// @param  pKey    Pointer to the key (or the key itself if configured so.)
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult CopyKeyToNode(
        HashmapNode* pNode,
        const VOID*  pKey);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// CopyValToNode
    ///
    /// @brief  copy the value of the given node.
    ///
    /// @param  pNode   Pointer to a hashmap node (representing a key/val pair)
    /// @param  pVal    Pointer to the value (or the key itself if configured so.)
    ///
    /// @return CamxResult
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CamxResult CopyValToNode(
        HashmapNode* pNode,
        const VOID*  pVal);

    // Do not implement the copy constructor or assignment operator
    Hashmap(const Hashmap&) = delete;
    Hashmap& operator=(const Hashmap&) = delete;

    HashmapParams                 m_params;               ///< Parameters of the hashmap
    LightweightDoublyLinkedList** m_ppTable;              ///< The hash table array (array of List*).
    SIZE_T                        m_realKeySize;          ///< The actual key size
    SIZE_T                        m_realValSize;          ///< The actual value size
    SIZE_T                        m_realHashmapNodeSize;  ///< Total size of the data for a hashmap node
    UINT                          m_numPairs;             ///< The number of pairs in the map
};

CAMX_NAMESPACE_END

#endif // CAMXHASHMAP_H

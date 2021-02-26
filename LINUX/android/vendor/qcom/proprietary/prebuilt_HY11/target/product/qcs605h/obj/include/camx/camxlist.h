////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file camxlist.h
///
/// @brief Defines standard list data structure
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAMXLIST_H
#define CAMXLIST_H

#include "camxincs.h"
#include "camxmem.h"

CAMX_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Compare two elements in the list.
///
/// @param  pElementA   Element A to compare to element B.
/// @param  pElementB   Element B.
///
/// @return If *pElementA > *pElementB return a positive number,
///         else if *pElementA == *pElementB return 0,
///         else (if) *pElementA < *pElementB return a negative number.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef INT (*ListCompareElements)(
    const VOID* pElementA,
    const VOID* pElementB);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Generic lightweight doubly linked list node
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct LightweightDoublyLinkedListNode
{
    VOID*                                   pData; ///< Pointer to the data that needs to be stored.
    struct LightweightDoublyLinkedListNode* pPrev; ///< Pointer to the previous element in the list.
    struct LightweightDoublyLinkedListNode* pNext; ///< Pointer to the next element in the list.
};

typedef LightweightDoublyLinkedListNode LDLLNode;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Element that contains required information for sorting an array of LDLLNode
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct LDLLCompareElement
{
    LDLLNode*           pNode;      ///< Pointer to the node being represented by array element
    ListCompareElements compare;    ///< Function pointer that all elements of array must share for compare to one another
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Generic POD lightweight doubly linked list implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LightweightDoublyLinkedList final
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InsertToHead
    ///
    /// @brief  Insert node at head of list
    ///
    /// @param  pNodeToInsert the node to insert
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID InsertToHead(
        LightweightDoublyLinkedListNode* pNodeToInsert)
    {
        CAMX_ASSERT(pNodeToInsert->pPrev == NULL);
        CAMX_ASSERT(pNodeToInsert->pNext == NULL);

        if (m_pHead == NULL)
        {
            m_pHead = pNodeToInsert;

            // First node going into the doubly linked list, so our head/tail are the same.
            m_pTail = m_pHead;
        }
        else
        {
            // Since we can always know where the head is, inserts become trivial.
            pNodeToInsert->pNext = m_pHead;
            m_pHead->pPrev       = pNodeToInsert;

            m_pHead              = pNodeToInsert;
        }

        m_numNodes++;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InsertToTail
    ///
    /// @brief  Insert node at tail of list
    ///
    /// @param  pNodeToInsert the node to insert
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID InsertToTail(
        LightweightDoublyLinkedListNode* pNodeToInsert)
    {
        CAMX_ASSERT(pNodeToInsert->pPrev == NULL);
        CAMX_ASSERT(pNodeToInsert->pNext == NULL);

        if (m_pHead == NULL)
        {
            m_pHead = pNodeToInsert;

            // First node going into the doubly linked list, so our head/tail are the same.
            m_pTail = m_pHead;
        }
        else
        {
            // Since we can always know where the tail it, inserts become trivial.
            m_pTail->pNext       = pNodeToInsert;
            pNodeToInsert->pPrev = m_pTail;

            m_pTail              = pNodeToInsert;
        }

        m_numNodes++;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// InsertAfterNode
    ///
    /// @brief  Insert node after node
    ///
    /// @param  pLeadingNode  the node after which to insert new node
    /// @param  pNodeToInsert the node to insert
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID InsertAfterNode(
        LightweightDoublyLinkedListNode* pLeadingNode,
        LightweightDoublyLinkedListNode* pNodeToInsert)
    {
        CAMX_ASSERT(pNodeToInsert->pPrev == NULL);
        CAMX_ASSERT(pNodeToInsert->pNext == NULL);

        if (NULL == pLeadingNode)
        {
            InsertToHead(pNodeToInsert);
        }
        else
        {
            if (NULL != pLeadingNode->pNext)
            {
                pLeadingNode->pNext->pPrev = pNodeToInsert;
            }
            pNodeToInsert->pNext = pLeadingNode->pNext;
            pLeadingNode->pNext  = pNodeToInsert;
            pNodeToInsert->pPrev = pLeadingNode;

            m_numNodes++;
        }

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RemoveFromHead
    ///
    /// @brief  Remove and return head node
    ///
    /// @return removed node pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE LightweightDoublyLinkedListNode* RemoveFromHead()
    {
        LightweightDoublyLinkedListNode* pNode = m_pHead;

        if (pNode != NULL)
        {
            // If the only node was removed, the tail must be updated to reflect the empty list.
            if (m_numNodes == 1)
            {
                CAMX_ASSERT(pNode == m_pTail);

                m_pTail = NULL;
                m_pHead = NULL;
            }
            else
            {
                m_pHead        = pNode->pNext;
                if (NULL != m_pHead)
                {
                    m_pHead->pPrev = NULL;
                }
            }

            pNode->pPrev = NULL;
            pNode->pNext = NULL;

            m_numNodes--;
        }

        return pNode;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RemoveFromTail
    ///
    /// @brief  Remove and return tail node
    ///
    /// @return removed node pointer
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE LightweightDoublyLinkedListNode* RemoveFromTail()
    {
        LightweightDoublyLinkedListNode* pNode = m_pTail;

        if (pNode != NULL)
        {
            // If the only node was removed, the tail must be updated to reflect the empty list.
            if (m_numNodes == 1)
            {
                CAMX_ASSERT(pNode == m_pHead);

                m_pTail = NULL;
                m_pHead = NULL;
            }
            else
            {
                m_pTail        = pNode->pPrev;
                m_pTail->pNext = NULL;
            }

            pNode->pPrev = NULL;
            pNode->pNext = NULL;

            m_numNodes--;
        }

        return pNode;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RemoveAllNodesAndFreeMem
    ///
    /// @brief  Remove all the node from list and free the data and node memory
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID RemoveAllNodesAndFreeMem()
    {
        if (NULL != m_pHead)
        {
            LightweightDoublyLinkedListNode* pNode;
            pNode = m_pHead;
            while (NULL != pNode)
            {
                LightweightDoublyLinkedListNode* pNext = pNode->pNext;
                RemoveNode(pNode);

                if (NULL != pNode->pData)
                {
                    CAMX_FREE(pNode->pData);
                }
                CAMX_FREE(pNode);
                pNode = pNext;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RemoveNode
    ///
    /// @brief  Remove node from list
    ///
    /// @param  pNode node to remove from list
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID RemoveNode(
        LightweightDoublyLinkedListNode* pNode)
    {
        if (NULL != pNode)
        {
            if (pNode == m_pHead)
            {
                m_pHead = pNode->pNext;

                if (NULL != m_pHead)
                {
                    m_pHead->pPrev = NULL;
                }
            }
            else
            {
                pNode->pPrev->pNext = pNode->pNext;

                if (NULL != pNode->pNext)
                {
                    pNode->pNext->pPrev = pNode->pPrev;
                }
            }

            if (pNode == m_pTail)
            {
                m_pTail = pNode->pPrev;
            }

            pNode->pPrev = NULL;
            pNode->pNext = NULL;

            m_numNodes--;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// RemoveByValue
    ///
    /// @brief  Remove node containing data from list. Only the first matching node.
    ///
    /// @param  pData Data to match to a node for removal, does not free pData
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE VOID RemoveByValue(
        VOID* pData)
    {
        LDLLNode* pNode = m_pHead;

        while (NULL != pNode)
        {
            if (pNode->pData == pData)
            {
                RemoveNode(pNode);
                CAMX_FREE(pNode);
                pNode = NULL;
                break;
            }
            pNode = NextNode(pNode);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// FindByValue
    ///
    /// @brief  Find node containing data from list. Only the first matching node.
    ///
    /// @param  pData Data to match to a node
    ///
    /// @return node
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE LightweightDoublyLinkedListNode* FindByValue(
        VOID* pData)
    {
        LDLLNode* pNode = m_pHead;

        while (NULL != pNode)
        {
            if (pNode->pData == pData)
            {
                break;
            }
            pNode = NextNode(pNode);
        }
        return pNode;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// SortFunc
    ///
    /// @brief  Function to compare two LDLLCompareElements
    ///
    /// @param  pA First element
    /// @param  pB Second element
    ///
    /// @return Difference between element's effective values as define by the data their nodes reference
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static INT SortFunc(
        const VOID* pA,
        const VOID* pB)
    {
        const LDLLCompareElement* pCompareA = reinterpret_cast<const LDLLCompareElement*>(pA);
        const LDLLCompareElement* pCompareB = reinterpret_cast<const LDLLCompareElement*>(pB);

        return pCompareA->compare(pCompareA->pNode->pData, pCompareB->pNode->pData);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sort
    ///
    /// @brief  Sort entire list by comparison function
    ///
    /// @param  compareFunc Function that defines the relative order of nodes
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    VOID Sort(
        ListCompareElements compareFunc)
    {
        if (2 > m_numNodes)
        {
            return;
        }

        LDLLCompareElement* pNodeArray = static_cast<LDLLCompareElement*>(CAMX_CALLOC(m_numNodes * sizeof(LDLLCompareElement)));
        LDLLNode*           pNode      = Head();
        LDLLNode*           pPrev      = NULL;
        UINT                i          = 0;

        while (NULL != pNode)
        {
            pNodeArray[i].compare = compareFunc;
            pNodeArray[i++].pNode = pNode;
            pNode = NextNode(pNode);
        }

        Utils::Qsort(pNodeArray, m_numNodes, sizeof(LDLLCompareElement), SortFunc);

        m_pHead = pNodeArray[0].pNode;

        for (i = 0; i < m_numNodes; i++)
        {
            pNode = pNodeArray[i].pNode;
            pNode->pPrev = pPrev;

            if (NULL != pPrev)
            {
                pPrev->pNext = pNode;
            }
            pPrev = pNode;
        }

        m_pTail = pPrev;
        m_pTail->pNext = NULL;

        CAMX_FREE(pNodeArray);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// NextNode
    ///
    /// @brief  Helper to fetch next node from current node
    ///
    /// @param  pNode node to lookup next
    ///
    /// @return Pointer to next node
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE static LightweightDoublyLinkedListNode* NextNode(
        LightweightDoublyLinkedListNode* pNode)
    {
        return (NULL != pNode) ? pNode->pNext : NULL;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// PrevNode
    ///
    /// @brief  Helper to fetch previous node from current node
    ///
    /// @param  pNode node to lookup prev
    ///
    /// @return Pointer to prev node
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE static LightweightDoublyLinkedListNode* PrevNode(
        LightweightDoublyLinkedListNode* pNode)
    {
        return (NULL != pNode) ? pNode->pPrev : NULL;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Head
    ///
    /// @brief  Helper to fetch head node
    ///
    /// @return Pointer to node
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE LightweightDoublyLinkedListNode* Head() const { return m_pHead; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Tail
    ///
    /// @brief  Helper to fetch tail node
    ///
    /// @return Pointer to node
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE LightweightDoublyLinkedListNode* Tail() const { return m_pTail; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// NumNodes
    ///
    /// @brief  Helper to fetch node count
    ///
    /// @return count of nodes in list
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CAMX_INLINE UINT NumNodes() const { return m_numNodes; }

    /// Constructs the doubly linked list and initializes it to be empty.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// LightweightDoublyLinkedList
    ///
    /// @brief  constructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOWHINE CP016: Basic final class wont have overrides
    LightweightDoublyLinkedList()
        // NOWHINE CP020: Helps readability
        : m_pHead(NULL),
        m_pTail(NULL),
        m_numNodes(0)
    {}

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// ~LightweightDoublyLinkedList
    ///
    /// @brief  destructor
    ///
    /// @return None
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // NOWHINE CP022: Basic final class wont have overrides
    ~LightweightDoublyLinkedList()
    {
        /// Caller owns the lists memory and must have freed the list!
        CAMX_ASSERT(NumNodes() == 0);
    }

protected:

private:
    // Member methods
    // Do not implement the copy constructor, or assignment operator
    LightweightDoublyLinkedList(const LightweightDoublyLinkedList&) = delete;
    LightweightDoublyLinkedList& operator=(const LightweightDoublyLinkedList&) = delete;

    LightweightDoublyLinkedListNode* m_pHead;    ///< The first element in the list
    LightweightDoublyLinkedListNode* m_pTail;    ///< The last element in the list
    UINT                             m_numNodes; ///< The number of elements in the list
};

CAMX_NAMESPACE_END

#endif // CAMXLIST_H

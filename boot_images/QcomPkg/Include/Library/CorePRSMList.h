/*============================================================================
@file CorePRSMList.h

Provides a singly-linked PRSM list implementation

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

==============================================================================*/
#ifndef CORE_PRSMLIST_H
#define CORE_PRSMLIST_H

/* An element in a PRSM list */
typedef struct CorePRSMListElemType
{
  struct CorePRSMListElemType *next;
  void                        *data;
} CorePRSMListElemType;

/* Opaque */
typedef struct CorePRSMListType CorePRSMListType;

/**
  @brief Core_PRSMList_Create - Creates and returns a PRSM list

  @return A PRSM list
*/
CorePRSMListType* Core_PRSMList_Create( void );

/**
  @brief Core_PRSMList_ReadLock - Acquires a read lock on the PRSM list

  A client must acquire a read lock before iterating through or accessing an
  element of the list.

  @param list: The PRSM list
  @return An opaque lock handle
*/
void* Core_PRSMList_ReadLock( CorePRSMListType *list );

/**
  @brief Core_PRSMList_ReadUnlock - Releases a read lock on the PRSM list

  A client must release a read lock after iteration or access

  @param list: The PRSM list
  @param lock: The opaque lock handle returned from Core_PRSMList_ReadLock
*/
void Core_PRSMList_ReadUnlock( CorePRSMListType *list, void *lock );

/**
  @brief Core_PRSMList_ModifyLock - Acquires a write/modify lock on the PRSM list

  In general, users will not need to call this function explicitly. List modify
  routines in this module internally acquire and release the write lock.
  
  @param list: The PRSM list
  @return An opaque lock handle
*/
void* Core_PRSMList_ModifyLock( CorePRSMListType *list );

/**
  @brief Core_PRSMList_ModifyUnlock - Releases a write/modify lock on the PRSM list

  In general, users will not need to call this function explicitly. List modify
  routines in this module internally acquire and release the write lock.
  
  @param list: The PRSM list
  @param lock: The opaque lock handle returned from Core_PRSMList_ModifyLock
*/
void Core_PRSMList_ModifyUnlock( CorePRSMListType *list, void *lock );

/**
  @brief Core_PRSMList_PushFront - Adds an element to the head of the list
*/  
void Core_PRSMList_PushFront( CorePRSMListType *list, void *data );

/**
  @brief Core_PRSMList_PopFront - Removes and returns the element at the
  head of the list
*/  
CorePRSMListElemType* Core_PRSMList_PopFront( CorePRSMListType *list,
                                              void ( *clean_up )( void* ) );
                                              
/**
  @brief Core_PRSMList_PeekFront - Returns the element at head of list, without
  removing it
  
  MUST be invoked after calling Core_PRSMList_ReadLock()
*/
CorePRSMListElemType* Core_PRSMList_PeekFront( CorePRSMListType *list );

#endif /* CORE_PRSMLIST_H */

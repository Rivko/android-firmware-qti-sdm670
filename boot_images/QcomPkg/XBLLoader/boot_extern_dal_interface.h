#ifndef BOOT_EXTERN_DAL_INTERFACE_H
#define BOOT_EXTERN_DAL_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN DAL DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external dal drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2012, 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/17   yps     Added boot_DALSYS_HeapForceDeInit
05/02/17   yps     Added boot_DALSYS_HeapDeInit
09/23/14   ck      Removed feature flag as API is always enabled in XBL
09/04/12   dh      Add boot_DALSYS_HeapInit
06/06/12   kedara  change inline to __inline to fix compilation error for 
                   boot_DAL_DeviceAttachEx
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "DALFramework.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_DALFW_MemDescInit

** ==========================================================================
*/
/*!
* 
  @brief Intialize the memory decriptor list

  This API must be used by clients to initialize memory descriptors for a given
  DDI IO  Memory region. Client can access the buffers within the DDI IO Memory
  region through these memory descriptors.

  @param hMem          : Handle to the DALSYS Mem Object
         pMemDescList   : Pointer to the Memory Descriptor
         dwNumBufs     : Number of buffers to be intialized

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
* 
*/
DALResult boot_DALFW_MemDescInit(DALSYSMemHandle hMem, 
                                 DALSysMemDescList * pMemDescList,
                                 uint32 dwNumBufs);


/*===========================================================================

**  Function :  boot_DALFW_MemDescBufPtr

** ==========================================================================
*/
/*!
* 
  @brief Given an index return a buf pointer from the memory decriptor list


  @param pMemDescList   : Pointer to the Memory Descriptor
         idx     : index of buffer pointer to be returned

  @return NULL of such pointer doesn't exist
* 
*/
DALSysMemDescBuf * boot_DALFW_MemDescBufPtr(DALSysMemDescList * pMemDescList,
                                            uint32 idx);


/*===========================================================================

**  Function :  boot_DALSYS_InitMod

** ==========================================================================
*/
/*!
  @brief Initialize/Register Module with the DALSYS library

  DAL Modules must invoke this API prior to any DALSYS usage.

  @param pCfg: DALSYS config struct pointer

  @return None
*/
void boot_DALSYS_InitMod(DALSYSConfig * pCfg);

/*===========================================================================

**  Function :  boot_DALSYS_DeInitMod

** ==========================================================================
*/
/*!
  @brief De-Initialize Module with the DALSYS library

  @return None
*/
void boot_DALSYS_DeInitMod(void);
/*===========================================================================

**  Function :  boot_DALSYS_MemRegionAlloc

** ==========================================================================
*/
/*!
  @brief Construct a DALSYS Mem Object

  Users must invoke this API to construct a Mem Object. Users have the option
  of pre-allocating memory required for this object, or rely upon dynamic
  memory allocation.

  This DALSys Mem object accomplishes virtual memory allocation,
  physical memory allocation and HWIO memory mapping.

  All lengths and offsets must be aligned to the architectures natural
  page boundaries.

  - Use DALSYS_MEM_PROPS_MAP if it is desired that physical memory be
  allocated at this time.
  - When specifying DALSYS_MEM_PROPS_HWIO, this function requires that
  the physical address is specified and this API will map it
  (i.e. DALSYS_MEM_PROPS_MAP is ignored when used with DALSYS_MEM_PROPS_HWIO).

  These memory regions may be used in conjunction with DDI IO Mem Regions.

  @param dwAttribs   : Memory Attributes i.e Cached | UnCached etc..
                       (see common defs)
         VirtualAddr : Desired Virtual Addr, if no preference use
                       DALSYS_MEM_ADDR_NOT_SPECIFIED

         PhysicalAddr: Desired Physical Addr, if no preference use
                       DALSYS_MEM_ADDR_NOT_SPECIFIED

         dwLen       : Size of virtual memory
         phMem       : Return parameter, upon successful invocation, points to
                       the DALSYS Mem Handle

         pMemObj    :  Pointer to the Memory Object if user doesnt desire
                     dynamic memory allocation for this Object.
                     NULL otherwise

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult boot_DALSYS_MemRegionAlloc(uint32 dwAttribs, 
                                     DALSYSMemAddr VirtualAddr,
                                     DALSYSMemAddr PhysicalAddr, 
                                     uint32 dwLen, 
                                     DALSYSMemHandle *phMem,
                                     DALSYSMemObj *pMemObj);


/*===========================================================================

**  Function :  boot_DALSYS_MemInfo

** ==========================================================================
*/
/*!
  @brief Return Information regarding the Memory Section

  @param hMem         : DALSYS Mem Handle
         pMemInfo     : Pointer to Mem Info structure, filled up by callee,
                        Valid only if return code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult boot_DALSYS_MemInfo(DALSYSMemHandle hMem,
                              DALSYSMemInfo *pMemInfo);


/*===========================================================================

**  Function :  boot_DAL_DeviceAttach

** ==========================================================================
*/
/*!
  @brief Establish communication link with the desired device

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device

  @param DeviceId : Desired Device Id
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @see DALSYS_GetEnv
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult boot_DAL_DeviceAttach(DALDEVICEID DeviceId,
                                DalDeviceHandle **phDevice);


/*===========================================================================

**  Function :  boot_DAL_DeviceAttachEx

** ==========================================================================
*/
/*!
  @brief Establish communication link with the desired device

  User(s)/client(s) of DAL drivers must use this API to obtain a handle to the
  desired device. Upon successful invocation of the API the Client will have
  successful established a communication link with the desired device.
  This function will also search for drivers remotely.

  @param pszArg   : User Argument
         DeviceId : Desired Device Id
         ClientVersion: Version from the Iterface Header file for this interface
         phDevice : Return parameter, contains DAL Device Handle, if return
                    code is DAL_SUCCESS
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult boot_DAL_DeviceAttachEx(const char *pszArg, 
                                  DALDEVICEID DeviceId,
                                  DALInterfaceVersion ClientVersion,
                                  DalDeviceHandle **phDevice);
               

/*===========================================================================

**  Function :  boot_DalDevice_Close

** ==========================================================================
*/
/*!
  @brief Close a Dal device

  @param hDevice : DAL Device Handle
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult boot_DalDevice_Close(DalDeviceHandle *hDevice);


/*===========================================================================

**  Function :  boot_DAL_DeviceDetach

** ==========================================================================
*/
/*!
  @brief Relinquish communication link with the desired device

  User(s)/client(s) of DAL drivers must use this API to release the
  desired device. Upon successful invocation of the API the Client will have
  successful relinquished communication link with the desired device

  @param hDevice : DAL Device Handle

  @see DALSYS_DeviceAttach
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult boot_DAL_DeviceDetach(DalDeviceHandle *hDevice);


/*===========================================================================

**  Function :  boot_DALSYS_Malloc

** ==========================================================================
*/ 
/*!
  @brief Allocate memory from local heap

  This is similar to Malloc from the C library, but this will allocate memory
  within the DAL Environment Context. Memory alloced through this call should be
  freed by calling DALSYS_Free

  @param dwSize   : Requested size of memory to be alloced
         ppMem    : Return parameter, callee will populate this to point to the
                    block of alloced memory, valid only upon DAL_SUCCESS return
                    code
  @see DALSYS_Free

  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult boot_DALSYS_Malloc(uint32 dwSize,
                             void **ppMem);


/*===========================================================================

**  Function :  boot_DALSYS_Free

** ==========================================================================
*/ 
/*!
  @brief Free allocated memory from local heap

  This is similar to free from the C library, but this will free memory alloced
  within the DAL Environment Context.

  @param pMem     : Pointer to block of memory alloced through DALSYS_Malloc

  @see DALSYS_Malloc
  @return Return Code, DAL_SUCCESS on successful completion, error code otherwise
*/
DALResult boot_DALSYS_Free(void *pmem);


/*===========================================================================

**  Function :  boot_DALSYS_HeapInit

** ==========================================================================
*/ 
/**
  @brief Initialize internal and external heap

  Users of this api must invoke only to publish availability of new heap 
  to heap manager.

  @param pHeapBase  : Base address of the heap
  @param dwHeapSz   : Size of the heap
  @param dwInitType : Type of heap

  @return void
  
  @note : Inorder to initialize internal heap dwInitType passed to api must be 0.
          for any external api, like DDR any unsigned non zero value , 1 preferred.

*/
void boot_DALSYS_HeapInit(void *pHeapBase,
                          uint32 dwHeapSz,
                          uint32 dwInitType);
/*===========================================================================

**  Function :  boot_DALSYS_HeapDeInit

** ==========================================================================
*/ 
/**
  @brief De-Initialize internal and external heap

  Users of this api must invoke only to publish availability of new heap 
  to heap manager.

  @param pHeapBase  : Base address of the heap

  @return void
  @note : Heap memory should be released before call this api
  
*/
void boot_DALSYS_HeapDeInit(void *pHeapBase);

/*===========================================================================

**  Function :  boot_DALSYS_HeapForceDeInit

** ==========================================================================
*/ 
/**
  @brief De-Initialize internal and external heap

  Users of this api must invoke only to publish availability of new heap 
  to heap manager.

  @param pHeapBase  : Base address of the heap

  @return void
  
*/
void boot_DALSYS_HeapForceDeInit(void *pHeapBase);
#endif /* BOOT_EXTERN_DAL_INTERFACE_H */

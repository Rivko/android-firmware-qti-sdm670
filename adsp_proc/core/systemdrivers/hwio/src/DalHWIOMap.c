/*============================================================================

FILE:      DalHWIOMap.c

DESCRIPTION:
  This file contains routines to map user-mode virtual memory for the HWIO
  hardware blocks.

PUBLIC CLASSES:
  HWIO_MapRegion
  HWIO_MapRegionByAddress
  HWIO_UnMapRegion
  HWIO_MapAllRegions 

============================================================================
   Copyright (c) 2009-2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/hwio/src/DalHWIOMap.c#2 $ $DateTime: 2017/11/29 23:57:55 $ $Author: pwbldsvc $
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DalHWIO.h"
#include "DALDeviceId.h"
#include "string.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * Define the page size for HWIO regions - 4K.
 */
#define HWIO_PAGE_SIZE 0x1000


/*
 * Define macro to get the page base and offset given an address.
 */
#define HWIO_PAGE_BASE(nAddr)   (nAddr & ~(HWIO_PAGE_SIZE - 1))
#define HWIO_PAGE_OFFSET(nAddr) (nAddr & (HWIO_PAGE_SIZE - 1))



/*=========================================================================
      Function Definitions
==========================================================================*/


/*==========================================================================

  FUNCTION      HWIO_AllocEntry

  DESCRIPTION   This function maps the physical memory to virtual memory for
                the HWIO hardware blocks.  The base and size of the 
                physical memory are passed as parameters and are used to call 
                into MmMapIoSpace() to perform the actual mapping.

  PARAMETERS    pPhysEntry - Pointer to HWIO entry structure

  DEPENDENCIES  None.

  RETURN VALUE  None.

==========================================================================*/

static void HWIO_AllocEntry
(
  HWIODrvCtxt     *pCtxt,
  HWIOPhysRegionType *pPhysEntry,
  HWIOVirtRegionType     *pVirtEntry
)
{
  DALSYSMemInfo MemInfo;
  uint32 nOffset;
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Check reference count, for static mappings nothing else is required.  */
  /*-----------------------------------------------------------------------*/

  if (pPhysEntry->pVirtAddr != 0)
  {
    pVirtEntry->pVirtAddr = pPhysEntry->pVirtAddr;
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Lock mutex.                                                           */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncEnter(pCtxt->hMapSync);

  /*-----------------------------------------------------------------------*/
  /* Map the memory.                                                       */
  /*-----------------------------------------------------------------------*/

  if (pVirtEntry->dwRefCount == 0)
  {
    if (pVirtEntry->pVirtAddr != NULL)
    {
      DALSYS_LogEvent(
        DALDEVICEID_HWIO, DALSYS_LOGEVENT_WARNING,
        "HWIO mapping already exists: %s @ 0x%08x", pPhysEntry->szName,
        pVirtEntry->pVirtAddr);
    }

    /*
     * Get the offset from the page boundary and map the aligned region.
     */
    nOffset = HWIO_PAGE_OFFSET(pPhysEntry->pPhysAddr);

    eResult = DALSYS_MemRegionAlloc(
      DALSYS_MEM_PROPS_HWIO,
      DALSYS_MEM_ADDR_NOT_SPECIFIED,
      HWIO_PAGE_BASE(pPhysEntry->pPhysAddr),
      pPhysEntry->dwSizeInBytes + nOffset,
      &pVirtEntry->hMem,
      NULL);
    
    if (eResult == DAL_SUCCESS)
    {
      DALSYS_MemInfo(pVirtEntry->hMem, &MemInfo);

      pVirtEntry->pVirtAddr = MemInfo.VirtualAddr + nOffset;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Increase the reference count.                                         */
  /*-----------------------------------------------------------------------*/
    
  if (pVirtEntry->pVirtAddr != NULL)
  {
    pVirtEntry->dwRefCount++;
  }
  else
  {
    DALSYS_LogEvent(
      DALDEVICEID_HWIO, DALSYS_LOGEVENT_ERROR,
      "HWIO mapping failed: %s @ 0x%08x", pPhysEntry->szName, pPhysEntry->pPhysAddr);
  }

  /*-----------------------------------------------------------------------*/
  /* Log message.                                                          */
  /*-----------------------------------------------------------------------*/

  DALSYS_LogEvent(
    DALDEVICEID_HWIO, DALSYS_LOGEVENT_INFO,
    "HWIO mapping: %s, physical address 0x%08x, virtual address 0x%08x",
    pPhysEntry->szName, pPhysEntry->pPhysAddr, pVirtEntry->pVirtAddr);

  /*-----------------------------------------------------------------------*/
  /* Unlock mutex.                                                         */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncLeave(pCtxt->hMapSync);

} /* END HWIO_AllocEntry */


/*==========================================================================

  FUNCTION      HWIO_FreeEntry

  DESCRIPTION   This function releases the virtual memory mapped to physical
                memory for the HWIO hardware blocks.                 

  PARAMETERS    pPhysEntry - Pointer to HWIO entry structure

  DEPENDENCIES  None.

  RETURN VALUE  None.

==========================================================================*/

static DALResult HWIO_FreeEntry
(
  HWIODrvCtxt     *pCtxt,
  HWIOPhysRegionType *pPhysEntry,
  HWIOVirtRegionType     *pVirtEntry
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check.                                                         */
  /*-----------------------------------------------------------------------*/

  if (pPhysEntry == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Check reference count, for static mappings nothing else is required.  */
  /*-----------------------------------------------------------------------*/

  if (pPhysEntry->pVirtAddr != 0)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Lock mutex.                                                           */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncEnter(pCtxt->hMapSync);

  /*-----------------------------------------------------------------------*/
  /* Check reference count.                                                */
  /*-----------------------------------------------------------------------*/

  if (pVirtEntry->dwRefCount == 0)
  {
    DALSYS_SyncLeave(pCtxt->hMapSync);
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Decrease reference count.                                             */
  /*-----------------------------------------------------------------------*/

  pVirtEntry->dwRefCount--;

  /*-----------------------------------------------------------------------*/
  /* If the last remaining reference, release the memory.                  */
  /*-----------------------------------------------------------------------*/

  if (pVirtEntry->dwRefCount == 0)
  {
    DALSYS_DestroyObject(pVirtEntry->hMem);

    pVirtEntry->pVirtAddr = NULL;
  }

  /*-----------------------------------------------------------------------*/
  /* Unlock mutex.                                                         */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncLeave(pCtxt->hMapSync);

  return DAL_SUCCESS;

} /* END HWIO_FreeEntry */


/*==========================================================================

  FUNCTION      HWIO_MapRegion

  DESCRIPTION   See DDIHWIO.h

==========================================================================*/

DALResult HWIO_MapRegion
(
  HWIODrvCtxt *pCtxt,
  const char  *szBase,
  uint8      **ppVirtAddr
) 
{
  HWIOPhysRegionType *pPhysEntry;
  HWIOVirtRegionType *pVirtEntry;
  HWIOModuleType     *pModule;
  boolean             bMatchModule = FALSE;

  /*-----------------------------------------------------------------------*/
  /* First check for NULL.                                                 */
  /*-----------------------------------------------------------------------*/

  if (ppVirtAddr == NULL || szBase == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Check if we are looking for a module.                                 */
  /*-----------------------------------------------------------------------*/

  if (szBase[0] == '+')
  {
    szBase++;
    bMatchModule = TRUE;
  }

  /*-----------------------------------------------------------------------*/
  /* Find the entry.                                                       */
  /*-----------------------------------------------------------------------*/

  pPhysEntry = pCtxt->PhysMap;
  pVirtEntry = pCtxt->VirtMap;

  while (pPhysEntry->szName != NULL)
  {
    if (!bMatchModule)
    {
      if (strcmp(szBase, pPhysEntry->szName) == 0)
      {
        pModule = NULL;
        break;
      }
    }
    else
    {
      pModule = pPhysEntry->pModules;
      while (pModule->szName != NULL)
      {
        if (strcmp(szBase, pModule->szName) == 0)
        {
          break;
        }
        pModule++;
      }
      if (pModule->szName != NULL)
      {
        break;
      }
    }

    pPhysEntry++;
    pVirtEntry++;
  }

  /*
   * Check if we failed to find the region.
   */
  if (pPhysEntry->szName == NULL)
  {
    DALSYS_LogEvent(
      DALDEVICEID_HWIO, DALSYS_LOGEVENT_WARNING,
      "HWIO: Unable to find \"%s\"", szBase);

    *ppVirtAddr = NULL;

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Map the entry.                                                        */
  /*-----------------------------------------------------------------------*/

  HWIO_AllocEntry(pCtxt, pPhysEntry, pVirtEntry);

  /*-----------------------------------------------------------------------*/
  /* Set the virtual address.                                              */
  /*-----------------------------------------------------------------------*/

  if (pVirtEntry->pVirtAddr != NULL)
  {
    *ppVirtAddr = (uint8 *)pVirtEntry->pVirtAddr;

    if (pModule != NULL)
    {
      *ppVirtAddr += pModule->nOffset;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return ok if the address was mapped.                                  */
  /*-----------------------------------------------------------------------*/

  return (pVirtEntry->pVirtAddr == NULL ? DAL_ERROR : DAL_SUCCESS);
 
} /* END HWIO_MapRegion */


/*==========================================================================

  FUNCTION      HWIO_MapRegionByAddress

  DESCRIPTION   See DDIHWIO.h

==========================================================================*/

DALResult HWIO_MapRegionByAddress
(
  HWIODrvCtxt *pCtxt,
  uint8       *pPhysAddr,
  uint8      **ppVirtAddr
) 
{
  HWIOPhysRegionType *pPhysEntry;
  HWIOVirtRegionType *pVirtEntry;
  HWIOModuleType     *pModule;
  uint32              nOffset;

  /*-----------------------------------------------------------------------*/
  /* First check for NULL.                                                 */
  /*-----------------------------------------------------------------------*/

  if (ppVirtAddr == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Find the entry.                                                       */
  /*-----------------------------------------------------------------------*/

  pPhysEntry = pCtxt->PhysMap;
  pVirtEntry = pCtxt->VirtMap;

  while (pPhysEntry->szName != NULL)
  {
    if ((uint8 *)pPhysEntry->pPhysAddr == pPhysAddr)
    {
      pModule = NULL;
      break;
    }

    /*
     * Check the set of modules in the base as well if the given address
     * falls within this base.
     */
    if (pPhysAddr > (uint8 *)pPhysEntry->pPhysAddr &&
        pPhysAddr < (uint8 *)pPhysEntry->pPhysAddr + pPhysEntry->dwSizeInBytes)
    {
      nOffset = (uint32)(pPhysAddr - pPhysEntry->pPhysAddr);
      pModule = pPhysEntry->pModules;
      while (pModule->szName != NULL)
      {
        if (pModule->nOffset == nOffset)
        {
          break;
        }
        pModule++;
      }
      if (pModule->szName != NULL)
      {
        break;
      }
    }

    pPhysEntry++;
    pVirtEntry++;
  }

  /*
   * Check if we failed to find the region.
   */
  if (pPhysEntry->szName == NULL)
  {
    DALSYS_LogEvent(
      DALDEVICEID_HWIO, DALSYS_LOGEVENT_WARNING,
      "HWIO: Unable to find address \"0x%08x\"", pPhysAddr);

    *ppVirtAddr = NULL;

    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Map the entry.                                                        */
  /*-----------------------------------------------------------------------*/

  HWIO_AllocEntry(pCtxt, pPhysEntry, pVirtEntry);

  /*-----------------------------------------------------------------------*/
  /* Set the virtual address.                                              */
  /*-----------------------------------------------------------------------*/

  if (pVirtEntry->pVirtAddr != NULL)
  {
    *ppVirtAddr = (uint8 *)pVirtEntry->pVirtAddr;

    if (pModule != NULL)
    {
      *ppVirtAddr += pModule->nOffset;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return ok if the address was mapped.                                  */
  /*-----------------------------------------------------------------------*/

  return (pVirtEntry->pVirtAddr == NULL ? DAL_ERROR : DAL_SUCCESS);
 
} /* END HWIO_MapRegionByAddress */


/*==========================================================================

  FUNCTION      HWIO_UnMapRegion

  DESCRIPTION   See DDIHWIO.h

==========================================================================*/

DALResult HWIO_UnMapRegion
(
  HWIODrvCtxt *pCtxt,
  uint8       *pVirtAddr
) 
{
  HWIOPhysRegionType *pPhysEntry;
  HWIOVirtRegionType *pVirtEntry;
  HWIOModuleType     *pModule;

  /*-----------------------------------------------------------------------*/
  /* First check for NULL.                                                 */
  /*-----------------------------------------------------------------------*/

  if (pVirtAddr == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Find the entry.                                                       */
  /*-----------------------------------------------------------------------*/

  pPhysEntry = pCtxt->PhysMap;
  pVirtEntry = pCtxt->VirtMap;

  while (pPhysEntry->szName != NULL)
  {
    if (pVirtEntry->pVirtAddr == (DALSYSMemAddr)pVirtAddr)
    {
      break;
    }

    /*
     * Check the set of modules in the base as well if the given virtual
     * address falls within the base.
     */
    if (pVirtAddr > (uint8 *)pVirtEntry->pVirtAddr &&
        pVirtAddr < (uint8 *)pVirtEntry->pVirtAddr + pPhysEntry->dwSizeInBytes)
    {
      pModule = pPhysEntry->pModules;
      while (pModule->szName != NULL)
      {
        if ((uint8 *)pVirtEntry->pVirtAddr + pModule->nOffset == pVirtAddr)
        {
          break;
        }
      }
      if (pModule->szName != NULL)
      {
        break;
      }
    }

    pPhysEntry++;
    pVirtEntry++;
  }

  /*
   * Check if we failed to find the region.
   */
  if (pPhysEntry->szName == NULL)
  {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Free the entry.                                                       */
  /*-----------------------------------------------------------------------*/

  return HWIO_FreeEntry(pCtxt, pPhysEntry, pVirtEntry);

} /* END HWIO_UnMapRegion */


/*==========================================================================

  FUNCTION      HWIO_MapAllRegions

  DESCRIPTION   See DalHWIO.h

==========================================================================*/

DALResult HWIO_MapAllRegions
(
  HWIODrvCtxt *pCtxt
) 
{
  HWIOPhysRegionType *pPhysEntry;
  HWIOVirtRegionType *pVirtEntry;
  DALSYSMemInfo MemInfo;
  uint32 nOffset;
  DALResult eResult = DAL_SUCCESS;
  DALSYSMemAddr nVirtAddr;

  /*-----------------------------------------------------------------------*/
  /* Lock mutex.                                                           */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncEnter(pCtxt->hMapSync);

  /*-----------------------------------------------------------------------*/
  /* Map all entries.                                                      */
  /*-----------------------------------------------------------------------*/

  pPhysEntry = pCtxt->PhysMap;
  pVirtEntry = pCtxt->VirtMap;

  while (pPhysEntry->szName != NULL)
  {
    /*
     * Get the offset from the page boundary and map the aligned region.
     */
    nOffset = HWIO_PAGE_OFFSET(pPhysEntry->pPhysAddr);

    /*
     * Define the virtual address if provided.
     */
    if (pVirtEntry->pVirtAddr != NULL)
    {
      nVirtAddr = HWIO_PAGE_BASE(pVirtEntry->pVirtAddr);
    }
    else
    {
      nVirtAddr = DALSYS_MEM_ADDR_NOT_SPECIFIED;
    }
 
    /*
     * Map the region.
     */
    DALSYS_MemRegionAlloc(
      DALSYS_MEM_PROPS_HWIO,
      nVirtAddr,
      HWIO_PAGE_BASE(pPhysEntry->pPhysAddr),
      pPhysEntry->dwSizeInBytes + nOffset,
      &pVirtEntry->hMem,
      NULL);
  
    DALSYS_MemInfo(pVirtEntry->hMem, &MemInfo);

    /*
     * Verify that the requested matches the assigned.
     */
    if (pVirtEntry->pVirtAddr != NULL &&
        pVirtEntry->pVirtAddr != MemInfo.VirtualAddr + nOffset)
    {
      DALSYS_LogEvent(
        DALDEVICEID_HWIO, DALSYS_LOGEVENT_ERROR,
        "HWIO mapping failed: %s @ 0x%08x", pPhysEntry->szName,
        pPhysEntry->pPhysAddr);

      eResult = DAL_ERROR;
    }

    /*
     * Assign the virtual address.
     */
    pVirtEntry->pVirtAddr = MemInfo.VirtualAddr + nOffset;

    /*
     * Increase our reference count for dynamic mappings.
     */
    if (pVirtEntry->dwRefCount != DALHWIO_REFCOUNT_STATIC)
    {
      pVirtEntry->dwRefCount++;
    }

    pPhysEntry++;
    pVirtEntry++;
  }

  /*-----------------------------------------------------------------------*/
  /* Unlock mutex.                                                         */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncLeave(pCtxt->hMapSync);

  /*-----------------------------------------------------------------------*/
  /* Return ok.                                                            */
  /*-----------------------------------------------------------------------*/

  return eResult;
 
} /* END HWIO_MapAllRegions */


/*==========================================================================

  FUNCTION      HWIO_GetMaxStrSize

  DESCRIPTION   See DalHWIO.h

==========================================================================*/

uint32 HWIO_GetMaxStrSize (HWIODrvCtxt *pCtxt)
{
  uint32 nMaxSize = 0, nEntrySize = 0;
  HWIOPhysRegionType *pPhysEntry = pCtxt->PhysMap;
  HWIOModuleType     *pModule;

  /*
   * Check if it has already been populated 
   * This data should never change dynamically, so no need to acquire the mutex. 
   */
  if ( pCtxt->nMaxStrSize > 0 )
  {
    return pCtxt->nMaxStrSize;
  }

  /*-----------------------------------------------------------------------*/
  /* Lock mutex.                                                           */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncEnter(pCtxt->hMapSync);

  /*
   * Traverse list and find max string size
   */

  while (pPhysEntry->szName != NULL)
  {
    nEntrySize = strlen( pPhysEntry->szName );
    if ( nEntrySize > nMaxSize )
    {
      nMaxSize = nEntrySize;
    }
    pModule = pPhysEntry->pModules;
    while (pModule->szName != NULL)
    {
      nEntrySize = strlen( pModule->szName );
      if ( nEntrySize > nMaxSize )
      {
        nMaxSize = nEntrySize;
      }
      pModule++;
    }

    pPhysEntry++;
  }

  /*
   * Cache value for next time
   */
  pCtxt->nMaxStrSize = ( nMaxSize + 1 );

  /*-----------------------------------------------------------------------*/
  /* Unlock mutex.                                                         */
  /*-----------------------------------------------------------------------*/

  DALSYS_SyncLeave(pCtxt->hMapSync);

  return ( nMaxSize + 1 );

} /* END HWIO_GetMaxStrSize */

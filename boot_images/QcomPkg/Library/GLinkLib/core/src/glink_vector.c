/*===========================================================================

                    GLINK Vector Source File

-----------------------------------------------------------------------------
Copyright (c) 2014 QUALCOMM Technologies Incorporated
-----------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/core/src/glink_vector.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/19/14   an      Initial version.
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "smem_list.h"
#include "glink_core_if.h"
#include "glink_transport_if.h"
#include "glink_vector.h"
#include "glink_os_utils.h"

/*===========================================================================
                       LOCAL DATA DECLARATIONS 
===========================================================================*/


/*===========================================================================
                       PRIVATE FUNCTIONS 
===========================================================================*/

/*===========================================================================
FUNCTION      glink_dummy_tx_vprovider
===========================================================================*/
/**

  Buffer provider for virtual space that operates on a non-vectored buffer.

  @param[in]  iovec   Pointer to the dummy vector.
  @param[in]  offset  Offset within the dummy vector.
  @param[out] size    Size of the provied buffer.

  @return     virtual address of the buffer.

  @sideeffects  None.
*/
/*=========================================================================*/
void* glink_dummy_tx_vprovider
(
  void*  iovec,
  size_t offset,
  size_t *size
)
{
  glink_core_tx_pkt_type* pkt = (glink_core_tx_pkt_type*)iovec;

  if (pkt == NULL || size == NULL || pkt->size <= offset)
  {
    return NULL;
  }

  *size = pkt->size - offset;

  return (char*)(pkt->data) + offset;
} 

/*===========================================================================
FUNCTION      glink_iovec_vprovider
===========================================================================*/
/**

  Buffer provider for virtual space that operates on a Glink iovec.

  @param[in]  iovec   Pointer to the dummy vector.
  @param[in]  offset  Offset within the dummy vector.
  @param[out] size    Size of the provied buffer.

  @return     virtual address of the buffer.

  @sideeffects  None.
*/
/*=========================================================================*/
void* glink_iovec_vprovider
(
  void*  iovec,
  size_t offset,
  size_t *size
)
{
  glink_iovector_type* iovec_l = (glink_iovector_type*)iovec;

  if (iovec_l == NULL || size == NULL)
  {
    return NULL;
  }

  if (!iovec_l->vlast || iovec_l->vlast->start_offset > offset) 
  {
       iovec_l->vlast = iovec_l->vlist;
  }

  while (iovec_l->vlast && 
         iovec_l->vlast->start_offset + iovec_l->vlast->size <= offset) 
  {
    iovec_l->vlast = iovec_l->vlast->next;
  }

  if (iovec_l->vlast == NULL) 
  {
    return NULL;
  }

  offset -= iovec_l->vlast->start_offset;
  *size = iovec_l->vlast->size - offset;

  return (char*)(iovec_l->vlast->data) + offset;
} 

/*===========================================================================
FUNCTION      glink_iovec_pprovider
===========================================================================*/
/**

  Buffer provider for physical space that operates on a Glink iovec.

  @param[in]  iovec   Pointer to the dummy vector.
  @param[in]  offset  Offset within the dummy vector.
  @param[out] size    Size of the provied buffer.

  @return     physical address of the buffer.

  @sideeffects  None.
*/
/*=========================================================================*/
void* glink_iovec_pprovider
(
  void*  iovec,
  size_t offset,
  size_t *size
)
{
  glink_iovector_type* iovec_l = (glink_iovector_type*)iovec;

  if (iovec_l == NULL || size == NULL)
  {
    return NULL;
  }

  if (!iovec_l->plast || iovec_l->plast->start_offset > offset) 
  {
       iovec_l->plast = iovec_l->plist;
  }

  while (iovec_l->plast && 
         iovec_l->plast->start_offset + iovec_l->plast->size <= offset) 
  {
    iovec_l->plast = iovec_l->plast->next;
  }

  if (iovec_l->plast == NULL) 
  {
    return NULL;
  }

  offset -= iovec_l->plast->start_offset;
  *size = iovec_l->plast->size - offset;

  return (char*)(iovec_l->plast->data) + offset;
}

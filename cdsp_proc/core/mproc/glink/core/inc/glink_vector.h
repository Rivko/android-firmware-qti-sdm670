#ifndef GLINK_VECTOR_H
#define GLINK_VECTOR_H

/*===========================================================================

                    GLink Vector Header File


-----------------------------------------------------------------------------
Copyright (c) 2014 QUALCOMM Technologies Incorporated
-----------------------------------------------------------------------------
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/15   bc      Remove unnecessary header
08/19/14   an      Initial version. 
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        FEATURE DEFINITIONS
===========================================================================*/

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/
typedef struct _glink_iovector_element_type {
  struct _glink_iovector_element_type *next;
  void* data;
  size_t start_offset;
  size_t size;
} glink_iovector_element_type;

typedef struct _glink_iovector_type {
  glink_iovector_element_type *vlist; /* virtual buffers list */
  glink_iovector_element_type *vlast; /* last accessed element in vlist */
  glink_iovector_element_type *plist; /* physical buffers list */
  glink_iovector_element_type *plast; /* last accessed element in plist */
} glink_iovector_type;

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
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
);

/*===========================================================================
FUNCTION      glink_dummy_rx_vprovider
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
);

/*===========================================================================
FUNCTION      glink_dummy_rx_pprovider
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
);

#endif /* GLINK_VECTOR_H */

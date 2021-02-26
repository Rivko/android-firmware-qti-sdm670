#ifndef FLASH_TRANS_IF_H
#define FLASH_TRANS_IF_H

/*===========================================================================

                Flash Translation Interface Header File

GENERAL DESCRIPTION
  This header file contains the definition of the public interface to
  the generic flash translation module/layer.

Copyright 2007-2008, 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
05/07/14   kedara  Make inlined api's static.
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_module_base.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define FLASH_TRANS_IF_VERSION 1

/* Forward declaration */
struct flash_trans_if_type;


/* Definition of the translation module virtual table structure */
typedef struct flash_trans_tvbl_type
{
/*=========================================================================

  **  Function : read

** ==========================================================================
*/
/*!
* 
*   @brief
*     Function to copy a region of data from a logical byte address
*     to the destination address.
* 
*   @par Dependencies
*     None
*   
*   @retval
*     TRUE if the operation succeeded, FALSE otherwise
* 
*   @par Side Effects
*     None
* 
  */
  boolean (*read)
  (
    struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                               interface */
    void   *dest_addr,                      /* Destination address */
    uint32  byte_offset,                    /* Logical source byte address */
    uint32  size                            /* Size to copy in bytes */
  );

  /*=========================================================================

  **  Function : sloppy_read

** ==========================================================================
*/
/*!
* 
*   @brief
*     Function to copy a region of data from a logical byte address to the
*     destination address, and to allow the copy operation to round the start
*     and end addresses of the copy region up or down to the nearest page
*     boundaries (for page or sector based devices) for efficiency.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     TRUE if the operation succeeded, FALSE otherwise
* 
*   @par Side Effects
*     This function may adjust the start address of the copy down to the next
*     lowest page boundary (for NAND flash), and adjust the size of the copy
*     up to match the next highest page boundary (for NAND flash).  This means
*     that this function may copy data to RAM below the buffer pointer and
*     beyond the limit of the buffer as defined by the size parameter.  This
*     function should not be used to copy data into a fixed buffer, only to
*     load whole images from flash to RAM, where it is safe to copy whole
*     pages from flash to RAM (for NAND).
* 
  */
  boolean (*sloppy_read)
  (
    struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                               interface */
    void   *dest_addr,                      /* Destination address */
    uint32  byte_offset,                    /* Logical source byte address */
    uint32  size                            /* Size to copy in bytes */
  );

  /*=========================================================================

  **  Function : get_logical_addr

** ==========================================================================
*/
/*!
* 
*   @brief
*     Function to translate a device-dependent physical flash source address
*     to a logical byte address.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     The logical byte address.
* 
*   @par Side Effects
*     None
* 
  */
  uint32 (*get_logical_addr)
  (
    struct flash_trans_if_type *trans_if,  /* Pointer to the translation
                                              interface */
    uint32 image_source                    /* Device-dependent physical
                                              flash source address */
  );

} flash_trans_vtbl_type;


/* Definition of public interface to the translation module */
typedef struct flash_trans_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  flash_trans_vtbl_type *vtbl;

} flash_trans_if_type;


/* Definition of the translation factory module virtual table structure */
typedef struct flash_trans_factory_vtbl_type
{
  /*=========================================================================

  **  Function : open

** ==========================================================================
*/
/*!
* 
*   @brief
*     Requests the factory to open an instance of the boot translation module.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     Pointer to the opened boot translation interface
* 
*   @par Side Effects
*     None
* 
  */
  flash_trans_if_type* (*open)
  (
    uint32 region_base_offset,      /* Base offset of the flash region */
    uint32 region_size              /* Maximum size of the flash region */
  );

  /*=========================================================================

  **  Function : close

** ==========================================================================
*/
/*!
* 
*   @brief
*     Requests the factory to close an instance of the boot translation module.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     None
* 
*   @par Side Effects
*     None
* 
  */
  void (*close)
  (
    flash_trans_if_type **p_trans_if
  );
} flash_trans_factory_vtbl_type;


/* Definition of public interface to translation factory module */
typedef struct flash_trans_factory_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  flash_trans_factory_vtbl_type *vtbl;

} flash_trans_factory_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

**  Function :  flash_trans_read

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address
*   to the destination address.
* 
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean flash_trans_read
(
  struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size                            /* Size to copy in bytes */
)
{
  return trans_if->vtbl->read( trans_if, dest_addr, byte_offset, size );
} /* flash_trans_read() */

/*=========================================================================

**  Function :  flash_trans_sloppy_read

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address to the
*   destination address, and to allow the copy operation to round the start
*   and end addresses of the copy region up or down to the nearest page
*   boundaries (for page or sector based devices) for efficiency.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
*  @par Side Effects
*   This function may adjust the start address of the copy down to the next
*   lowest page boundary (for NAND flash), and adjust the size of the copy
*   up to match the next highest page boundary (for NAND flash).  This means
*   that this function may copy data to RAM below the buffer pointer and
*   beyond the limit of the buffer as defined by the size parameter.  This
*   function should not be used to copy data into a fixed buffer, only to
*   load whole images from flash to RAM, where it is safe to copy whole
*   pages from flash to RAM (for NAND).
* 
*/
static __inline boolean flash_trans_sloppy_read
(
  struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size                            /* Size to copy in bytes */
)
{
  return trans_if->vtbl->sloppy_read( trans_if, dest_addr, byte_offset, size );
} /* flash_trans_sloppy_read() */

/*=========================================================================

**  Function :  flash_trans_get_logical_addr

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to translate a device-dependent physical flash source address
*   to a logical byte address.
* 
* @par Dependencies
*   None
* 
* @retval
*   The logical byte address.
* 
* @par Side Effects
*   None
* 
*/
static __inline uint32 flash_trans_get_logical_addr
(
  struct flash_trans_if_type *trans_if,  /* Pointer to the translation
                                            interface */
  uint32 image_source                    /* Device-dependent physical
                                            flash source address */
)
{
  return trans_if->vtbl->get_logical_addr( trans_if, image_source );
} /* flash_trans_get_logical_addr() */


/*=========================================================================

**  Function :  flash_trans_factory_open

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to open an instance of the translation module.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the opened boot translation interface
* 
* @par Side Effects
*   None
* 
*/
static __inline flash_trans_if_type* flash_trans_factory_open
(
  flash_trans_factory_if_type *trans_fact_if,
  uint32 region_base_offset,      /* Base offset of the flash region */
  uint32 region_size              /* Maximum size of the flash region */
)
{
  return trans_fact_if->vtbl->open( region_base_offset,
                                    region_size );
} /* flash_trans_factory_open() */

/*=========================================================================

**  Function :  flash_trans_factory_close

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to close an instance of the translation module.
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static __inline void flash_trans_factory_close
(
  flash_trans_factory_if_type *trans_fact_if,
  flash_trans_if_type **p_trans_if
)
{
  trans_fact_if->vtbl->close( p_trans_if );
} /* flash_trans_factory_close() */

#endif /* FLASH_TRANS_IF_H */


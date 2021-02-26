/*=============================================================================

                         SMEM TARGET

 Copyright (c)  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/inc/smem_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/16   rv      Initial version.
===========================================================================*/

#ifndef SMEM_TARGET_H
#define SMEM_TARGET_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "com_dtypes.h"
#if defined(MPROC_RPM_PROC)
#include "rpm_hwio.h"
#include "HALhwio.h"
#include "page_select.h"
#endif

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/
#if defined(MPROC_APPS_PROC)
  #define SMEM_APPS_PROC
  #define SMEM_THIS_HOST  SMEM_APPS
#elif defined(MPROC_MODEM_PROC)
  #define SMEM_MODEM_PROC
  #define SMEM_THIS_HOST  SMEM_MODEM
#elif defined(MPROC_ADSP_PROC) || defined(MPROC_LPASS_PROC)
  #define SMEM_ADSP_PROC
  #define SMEM_LPASS_PROC
  #define SMEM_THIS_HOST  SMEM_ADSP
#elif defined(MPROC_SLPI_PROC)
  #define SMEM_SLPI_PROC
  #define SMEM_THIS_HOST  SMEM_SSC
#elif defined(MPROC_WCN_PROC)
  #define SMEM_WCN_PROC
  #define SMEM_RIVA_PROC
  #define SMEM_THIS_HOST  SMEM_WCN
#elif defined(MPROC_CDSP_PROC)
  #define SMEM_CDSP_PROC
  #define SMEM_THIS_HOST  SMEM_CDSP
#elif defined(MPROC_RPM_PROC)
  #define SMEM_RPM_PROC
  #define SMEM_THIS_HOST  SMEM_RPM
#elif defined(MPROC_TZ_PROC)
  #ifdef SMEM_TZ_HOST_NOT_SUPPORTED
    #define SMEM_APPS_PROC
    #define SMEM_THIS_HOST  SMEM_APPS
  #else
    #define SMEM_TZ_PROC
    #define SMEM_THIS_HOST  SMEM_TZ
  #endif
#elif defined(MPROC_SPSS_PROC)
  #define SMEM_SPSS_PROC
  #define SMEM_THIS_HOST  SMEM_SPSS
#elif defined(MPROC_HYP_PROC)
  #ifdef SMEM_TZ_HOST_NOT_SUPPORTED
    #define SMEM_APPS_PROC
    #define SMEM_THIS_HOST  SMEM_APPS
  #else
    #define SMEM_HYP_PROC
    #define SMEM_THIS_HOST  SMEM_HYP
  #endif
#elif defined(MPROC_BOOT_PROC)
  #define SMEM_APPS_PROC
  #define SMEM_THIS_HOST  SMEM_APPS
#else
#error "SMEM_THIS_HOST not defined"
#endif

/* Spinlock ID for this host */
#define SMEM_SPINLOCK_PID  (SMEM_THIS_HOST + 1)

/* Identifier for the SMEM target info struct.
 * "SIII" in little-endian. */
#define SMEM_TARG_INFO_IDENTIFIER     0x49494953

#if defined(MPROC_RPM_PROC)

/** RPM page select configuration needed to get DDR shared memory access
 *  CSR:0x02 <- DDR shared memory
 *  CSR:0x00 <- RPM message RAM */
#define SMEM_ACCESS_CONFIGURE_SHARED_RAM()       HWIO_RPM_PAGE_SELECT_OUT(0x2)
#define SMEM_ACCESS_CONFIGURE_RESTORE()          HWIO_RPM_PAGE_SELECT_OUT(0x0)

#else

#define SMEM_ACCESS_CONFIGURE_SHARED_RAM()       do{}while(0)
#define SMEM_ACCESS_CONFIGURE_RESTORE()          do{}while(0)

#endif

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
/*----------------------------------------------------------------------
 * The structure that is filled in by smem_boot_init() to
 * maintain a single source that determines target information like the SMEM
 * base physical address and size.  All images then use these values to set the
 * base virtual address pointer at the beginning of smem_init().
 *
 * Note: We cannot move these fields, and this struct must grow towards greater
 * addresses in order to support interprocessor versioning.  Badger only has a
 * set 16-Bytes of reserved space, so any fields added to this struct cannot
 * be used on Badger targets.
 *----------------------------------------------------------------------*/
typedef struct
{
  uint32  identifier;
  uint32  smem_size;
  uint64  smem_base_phys_addr;
  uint16  smem_max_items;
  uint16  smem_reserved;
} smem_targ_info_type;

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  smem_targ_init
=============================================================================*/
/**
  This function does the target specific initialization.

  @return
  None.
 */
/*===========================================================================*/
void smem_targ_init(void);

/*=============================================================================
  FUNCTION  smem_targ_base_addr_get
=============================================================================*/
/**
  This function returns the SMEM base physical address.

  @return
  SMEM base physical address.
 */
/*===========================================================================*/
uint8 *smem_targ_base_addr_get(void);

/*=============================================================================
  FUNCTION  smem_boot_targ_size_get
=============================================================================*/
/**
  This function returns the SMEM size.

  @return
  SMEM size.
 */
/*===========================================================================*/
uint32 smem_targ_size_get(void);

/*=============================================================================
  FUNCTION  smem_targ_max_items_get
=============================================================================*/
/**
  This function returns the max number of SMEM items supported

  @return
  Max SMEM items.
 */
/*===========================================================================*/
uint16 smem_targ_max_items_get(void);

/*=============================================================================
  FUNCTION  smem_targ_spin_lock
=============================================================================*/
/**
  Acquires spinlock by writing a non-zero value to the hardware mutex.

  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked

  Locking the Mutex:
   - If Mutex is unlocked and PID is written, then the mutex is locked
   - If Mutex is locked and PID is written, then mutex register is not updated
     and the mutex remains locked

  A more detailed description can be found in System Fast Peripheral Bus
  (80-VR139-1A Rev. A) documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies
  HW mutexes must be supported.
  Must lock a local OS mutex before calling this function.

  @sideeffects
  Locks the hardware mutex.
 */
/*===========================================================================*/
void smem_targ_spin_lock(uint8 lock);

/*=============================================================================
  FUNCTION  smem_targ_spin_unlock
=============================================================================*/
/**
  Releases spinlock by writing zero to the hardware mutex.

  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked

  Unlocking the mutex:
   - If '0' is written, then mutex register is updated and the mutex is
     unlocked

  A more detailed description can be found in System Fast Peripheral Bus
  (80-VR139-1A Rev. A) documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies
  HW mutexes must be supported.
  Unlock local OS mutex after calling this function.

  @sideeffects
  Unlocks the hardware mutex.
 */
/*===========================================================================*/
void smem_targ_spin_unlock(uint8 lock);

/*=============================================================================
  FUNCTION  smem_targ_spin_lock_get
=============================================================================*/
/**
  This function retrieves the HW mutex value of the specified lock.

  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked

  A more detailed description can be found in System FPB (80-VR139-1A Rev. A)
  documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies
  None.

  @sideeffects
  None.
 */
/*===========================================================================*/
uint32 smem_targ_spin_lock_get(uint8 lock);

#ifdef __cplusplus
}
#endif

#endif /* SMEM_TARGET_H */

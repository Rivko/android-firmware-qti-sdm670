/*=============================================================================

                         SMEM TARGET

 Copyright (c)  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/src/smem_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/16   rv      Initial version.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "smem_os.h"
#if !defined(MPROC_BOOT_PROC) && !defined(MPROC_HYP_PROC)
#include "DALSys.h"
#include "DALFramework.h"
#endif /* !defined(MPROC_BOOT_PROC) && !defined(MPROC_HYP_PROC) */
#include "smem_type.h"
#include "smem_toc.h"
#include "spinlock.h"
#include "smem_internal.h"
#include "smem_target.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                       PUBLIC/GLOBAL DATA DEFINATIONS

=============================================================================*/


/*=============================================================================

                       LOCAL DATA DEFINATIONS

=============================================================================*/
/** Address containing the SMEM memory info, e.g physical address and size,
 *  and possibly other information. */
static smem_targ_info_type smem_targ_info;

#if defined(MPROC_APPS_PROC) || defined(MPROC_TZ_PROC)
static boolean smem_targ_spinlock_locked[SPINLOCK_NUM_LOCKS] = {0};
#endif

#if defined(MPROC_BOOT_PROC) || defined(MPROC_HYP_PROC)
/*----------------------------------------------------------------------------
 * CORE_TOP_CSR
 *--------------------------------------------------------------------------*/
extern const uint32 smem_targ_hwio_tcsr_base;

/*----------------------------------------------------------------------------
 * TCSR_TCSR_MUTEX
 *--------------------------------------------------------------------------*/
extern const uint32 smem_targ_hwio_mutex_reg_offsets[SPINLOCK_NUM_LOCKS];

/*----------------------------------------------------------------------------
 * TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/
extern const uint32 smem_targ_hwio_wonce_reg_offsets[2];

#endif

/* Mutex register addresses */
volatile uint32 *smem_targ_mutex_reg_addrs[SPINLOCK_NUM_LOCKS];

/* Wonce register addresses */
volatile uint32 *smem_targ_wonce_reg_addrs[2];

/*=============================================================================

                      LOCAL FUNCTION DEFINATIONS

=============================================================================*/


/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  smem_targ_base_addr_get
=============================================================================*/
/**
  This function returns the SMEM base physical address.

  @return
  SMEM base physical address.
 */
/*===========================================================================*/
uint8 *smem_targ_base_addr_get(void)
{
  uint8 *base_addr;

#if defined(MPROC_RPM_PROC)
  base_addr = (uint8 *)(uintptr_t)set_ddr_access(smem_targ_info.smem_base_phys_addr);
#else
  base_addr = (uint8 *)(uintptr_t)(smem_targ_info.smem_base_phys_addr);
#endif
  return base_addr;
}

/*=============================================================================
  FUNCTION  smem_targ_size_get
=============================================================================*/
/**
  This function returns the SMEM size.

  @return
  SMEM size.
 */
/*===========================================================================*/
uint32 smem_targ_size_get(void)
{
  return smem_targ_info.smem_size;
}
/*=============================================================================
  FUNCTION  smem_targ_max_items_get
=============================================================================*/
/**
  This function returns the max number of SMEM items supported

  @return
  Max SMEM items.
 */
/*===========================================================================*/
uint16 smem_targ_max_items_get(void)
{
  if (smem_targ_info.smem_max_items)
  {
    return smem_targ_info.smem_max_items;
  }
  else
  {
    return SMEM_MEM_LAST + 1;
  }
}

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
void smem_targ_spin_lock(uint8 lock)
{
  if (SMEM_SPINLOCK_PID == smem_targ_spin_lock_get(lock))
  {
    return;
  }

  do
  {
    /* Attempt to write PID */
    *smem_targ_mutex_reg_addrs[lock] = SMEM_SPINLOCK_PID;

    /* Check if PID was written (i.e. the hardware mutex is locked) */
  } while(*smem_targ_mutex_reg_addrs[lock] != SMEM_SPINLOCK_PID);

#if defined(MPROC_APPS_PROC) || defined(MPROC_TZ_PROC)
  /* It acquired hardware mutex, so it needs to release it in unlock */
  smem_targ_spinlock_locked[lock] = TRUE;
#endif
}

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
void smem_targ_spin_unlock(uint8 lock)
{
  uint32 spinlock_value = smem_targ_spin_lock_get(lock);

  if (spinlock_value != SMEM_SPINLOCK_PID)
  {
    SMEM_OS_LOG_FATAL(2, "Unexpected mutex value %d. Expected %d.",
            spinlock_value, SMEM_SPINLOCK_PID);
  }

#if defined(MPROC_APPS_PROC) || defined(MPROC_TZ_PROC)
  /* Release hardware mutex only when it acquired. */
  if (smem_targ_spinlock_locked[lock] == TRUE)
  {
    smem_targ_spinlock_locked[lock] = FALSE;
  }
  else
  {
    return;
  }
#endif

  /* Write '0' to the hardware mutex to unlock */
  *smem_targ_mutex_reg_addrs[lock] = 0;
}

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
uint32 smem_targ_spin_lock_get(uint8 lock)
{
  return *smem_targ_mutex_reg_addrs[lock];
}

/*=============================================================================
  FUNCTION  smem_targ_init
=============================================================================*/
/**
  This function does the target specific initialization.

  @return
  None.
 */
/*===========================================================================*/
SMEM_OS_INIT_FUNC_RECLAIM_SECTION
void smem_targ_init(void)
{
  smem_targ_info_type *smem_targ_info_ptr;
  uint64              smem_targ_info_addr;
  uint32              idx;

#if !defined(MPROC_BOOT_PROC) && !defined(MPROC_HYP_PROC)
  /* The handle is a uint32[2], so it won't be null after declaration. */
  DALSYS_PROPERTY_HANDLE_DECLARE(prop_handle);
  DALSYSPropertyVar prop;
  DALResult         dal_result;
  uint8             *tcsr_base;


  dal_result = DALSYS_GetDALPropertyHandleStr("/dev/core/mproc/smem",
                                              prop_handle);
  if (dal_result != DAL_SUCCESS)
  {
    SMEM_OS_LOG_FATAL(1, "Cannot get DALProp handle dal_result=%d.", dal_result);
    return;
  }

  /* Get the TCSR base address */
  #if defined(MPROC_TZ_PROC)
    dal_result = DALSYS_GetPropertyValue(prop_handle, "tcsr_base", 0, &prop);
    if ((dal_result != DAL_SUCCESS) || (prop.dwType != DALSYS_PROP_TYPE_UINT32))
    {
      SMEM_OS_LOG_FATAL(1, "DAL devcfg tcsr_base get failed! dal_result=%d.", dal_result);
      return;
    }
    tcsr_base = (uint8 *)(uintptr_t)prop.Val.dwVal;

  #else /* !defined(MPROC_TZ_PROC) */
    dal_result = DALSYS_GetPropertyValue(prop_handle, "tcsr_base_name", 0, &prop);
    if ((dal_result != DAL_SUCCESS) || (prop.dwType != DALSYS_PROP_TYPE_STR_PTR))
    {
      SMEM_OS_LOG_FATAL(1, "DAL devcfg tcsr_base_name get failed! dal_result=%d.", dal_result);
      return;
    }

    tcsr_base = smem_os_map_hwio_region(prop.Val.pszVal);
    if (tcsr_base == NULL)
    {
      SMEM_OS_LOG_FATAL(0, "SMEM Target HW mutex mapping failed.");
      return;
    }
  #endif /* defined(MPROC_TZ_PROC) */

  /* Get the Mutex register offsets */
  dal_result = DALSYS_GetPropertyValue(prop_handle, "mutex_offsets", 0, &prop);
  if ((dal_result != DAL_SUCCESS) ||
     (prop.dwType != DALSYS_PROP_TYPE_UINT32_PTR) ||
     (prop.dwLen  != SPINLOCK_NUM_LOCKS))
  {
    SMEM_OS_LOG_FATAL(3, "DAL devcfg mutex_offsets get failed! dal_result=%d, prop.dwType=%d,"
                         " prop.dwLen=%d", dal_result, prop.dwType, prop.dwLen);
    return;
  }

  /* Update the Mutex registers addresses */
  for (idx = 0; idx < SPINLOCK_NUM_LOCKS; idx++)
  {
    smem_targ_mutex_reg_addrs[idx] = (volatile uint32*)(uintptr_t)
            (tcsr_base + prop.Val.pdwVal[idx]);
  }

  /* Get the Wonce register offsets */
  dal_result = DALSYS_GetPropertyValue(prop_handle, "wonce_offsets", 0, &prop);
  if ((dal_result != DAL_SUCCESS) ||
     (prop.dwType != DALSYS_PROP_TYPE_UINT32_PTR) ||
     (prop.dwLen  != 2))
  {
    SMEM_OS_LOG_FATAL(3, "DAL devcfg wonce_offsets get failed! dal_result=%d, prop.dwType=%d,"
                         " prop.dwLen=%d", dal_result, prop.dwType, prop.dwLen);
    return;
  }

  /* Update the Wonce registers addresses */
  smem_targ_wonce_reg_addrs[0] = (volatile uint32*)(uintptr_t)
            (tcsr_base + prop.Val.pdwVal[0]);

  smem_targ_wonce_reg_addrs[1] = (volatile uint32*)(uintptr_t)
            (tcsr_base + prop.Val.pdwVal[1]);

#else /* defined(MPROC_BOOT_PROC) || defined(MPROC_HYP_PROC) */

  /* Update the Mutex registers addresses */
  for (idx = 0; idx < SPINLOCK_NUM_LOCKS; idx++)
  {
    smem_targ_mutex_reg_addrs[idx] = (volatile uint32*)(uintptr_t)
            (smem_targ_hwio_tcsr_base + smem_targ_hwio_mutex_reg_offsets[idx]);
  }

  /* Update the Wonce registers addresses */
  smem_targ_wonce_reg_addrs[0] = (volatile uint32*)(uintptr_t)
            (smem_targ_hwio_tcsr_base + smem_targ_hwio_wonce_reg_offsets[0]);

  smem_targ_wonce_reg_addrs[1] = (volatile uint32*)(uintptr_t)
            (smem_targ_hwio_tcsr_base + smem_targ_hwio_wonce_reg_offsets[1]);

#endif /* !defined(MPROC_BOOT_PROC) && !defined(MPROC_HYP_PROC) */

  /* Read SMEM target info address from WONCE registers */
  smem_targ_info_addr = *smem_targ_wonce_reg_addrs[0] |
                       (uint64)*smem_targ_wonce_reg_addrs[1] << 32;

  /* Map target info physical address to virtual address */
  smem_targ_info_ptr = smem_os_map_memory_va((void *)smem_targ_info_addr,
                                             sizeof(smem_targ_info_type));

  if (smem_targ_info_ptr == NULL)
  {
    SMEM_OS_LOG_FATAL(1, "SMEM Target info addr=0x%08X memory mapping failed.",
                           smem_targ_info_addr);
    return;
  }
  
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
#if defined(MPROC_RPM_PROC)
  smem_targ_info_ptr = (smem_targ_info_type *)(uintptr_t)
                            set_ddr_access((uint64_t)smem_targ_info_ptr);
#endif

  smem_targ_info = *smem_targ_info_ptr;
  SMEM_ACCESS_CONFIGURE_RESTORE();

  if (smem_targ_info.identifier != SMEM_TARG_INFO_IDENTIFIER)
  {
    SMEM_OS_LOG_FATAL(0, "SMEM Target info is not present.");
  }
}
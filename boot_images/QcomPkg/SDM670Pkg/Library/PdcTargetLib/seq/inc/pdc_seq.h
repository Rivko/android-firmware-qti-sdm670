#ifndef __PDC_SEQ_H__
#define __PDC_SEQ_H__
/*===========================================================================
                              pdc_seq.h

DESCRIPTION:
  This file defines public interface for PDC sequencer.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Type Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------
#include "comdef.h"
#include <stdlib.h>

//---------------------------------------------------------------------------
//  Type Declarations
//---------------------------------------------------------------------------

/* Forward declaration of internal configuration structure that can be 
 * version specific. Here, it can only be used as reference or pointer */
struct pdc_seq_hal_bsp_cfg_s;

/* Forward declaration of low power mode structure for PDC sequencer */
struct pdc_seq_hal_bsp_mode_s;

/* Structure containing information about specific PDC sequencer instance.
 * Depending on the subsystem, there can be more than one PDC, its SW
 * may configure */
typedef struct pdc_seq_s
{
  const char *name;                      /* Name of the PDC instance */
  struct pdc_seq_hal_bsp_cfg_s *cfg;     /* Static config */
  struct pdc_seq_hal_bsp_mode_s *modes;  /* List of LPMs for this PDC */
  size_t mode_count;                     /* Number of modes in above list */
  size_t offset;                         /* PDC offset from AOSS base */
  size_t rsc_offset;                     /* Attached RSC offset in SS  */
  
  /* Internal fields */
  uint8* addr;                    /* PDC instance's mapped address */
  uint8* rsc_addr;                /* RSC's mapped address */
  uint64* profile_ts;             /* Profiling time stamps */
  uint64* event_ts;               /* Event time stamps */
  uint16 copied_cmds;             /* Number of seq cmds copied to instance */

} pdc_seq_t;

typedef pdc_seq_t* pdc_seq_handle;

/* Return value enumberation for different PDC sequencer API */
typedef enum
{
  PDC_SEQ_SUCCESS = 0,
  PDC_SEQ_INVALID_HANDLE = -1,
  PDC_SEQ_INVALID_PARAM = -2,
  PDC_SEQ_UNSUPPORTED_MODE = -3,
  PDC_SEQ_NO_MEM = -4,
  PDC_SEQ_ERROR = -65536
} pdc_seq_result_t;


//===========================================================================
//                       Function Declarations
//===========================================================================

/**
 * <!-- pdc_seq_driver_init -->
 *
 * @brief Performs common driver initialization for PDC sequencer 
 *        independent of PDC instance.
 *
 * @note This function must be called before calling any other PDC sequencer
 *       functions.
 */
void pdc_seq_driver_init (void);

/**
 * <!-- pdc_seq_count -->
 * 
 * @brief Returns the number of PDC associated with input entity. This
 *        function is used before querying for PDC sequencer handles.
 *
 * On a given subsystem, driver will contain only PDCs for entities it 
 * handles. It will not be exhaustive for entire system.
 *
 * Current expectation is that each entity will have only one associated
 * PDC but we are keeping interfce a generic, mainly to be compatible with
 * other sequencers we have and have better flexibility going forward.
 *
 * @dependency pdc_seq_driver_init
 *
 * @param entity: Name of the entity for which we are querying PDC count
 *                (e.g. "aop", "mss", etc)
 *
 * @return Number of PDC sequencers associated with input entity.
 */
uint32 pdc_seq_count (const char *entity);

/**
 * <!-- pdc_seq_handles -->
 *
 * @brief Returns the array of pdc handles associated with input entity
 *        in an input buffer. It will return minimum of available handles
 *        and handle buffer size.
 *
 * Current expectation is that each entity will have only one associated
 * PDC but we are keeping interfce a generic, mainly to be compatible with
 * other sequencers we have and have better flexibility going forward.
 *
 * @dependency pdc_seq_driver_init
 *             pdc_seq_get_count (if not known, also allows more flexibility)
 *
 * @param entity: Name of the entity for which we are querying PDC count
 *                (e.g. "aop", "mss", etc)
 * @param count: Number of handles input handles' buffer (next param) can 
 *               contain (ideally obtained from get_count function for
 *               flexibility).
 * @param handles: Input handles' buffer where PDC sequencer handles will 
 *                 be returned.
 * 
 * @return Number of PDC sequencer handles returned for the input entity.
 *         It should ideally be same as input param 'count'.
 */
uint32 pdc_seq_handles 
(
  const char *entity, 
  uint32 count, 
  pdc_seq_handle *handles
);

/**
 * <!-- pdc_seq_handle_init -->
 *
 * @brief Performs PDC sequencer instance specific initialization.
 *
 * @note This function must be called on a handle/instance before calling
 *       any other APIs that take handle as input.
 *
 * @dependency pdc_seq_driver_init
 *
 * @param handle: PDC sequencer handle to initialize.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_handle_init (pdc_seq_handle handle);

/**
 * <!-- pdc_seq_sys_init -->
 *
 * @brief This function initializes common PDC sequencer driver as well
 *        as all the PDC instances supported on a given subsystem.
 *
 * This function is provided where it is difficult for primary PDC
 * sequencer client to have it intiailized and enabled before RPMh 
 * communication is up. The intended usage is to plug this funciton
 * in subsystem initialization mechanism.
 *
 * @note If a subsystem calls this function, PDC sequencer clients do
 *       not need to intialize their respective PDC instances separately.
 */
void pdc_seq_sys_init (void);

/**
 * <!-- pdc_enable -->
 *
 * @brief Enables or disables PDC.
 *
 * There is no dedicated sequencer enable logic. Enabling logic applies to
 * entire PDC block.
 *
 * @dependency pdc_seq_driver_init, 
 *             pdc_seq_handle_init
 *
 * @param handle: Handle to PDC sequencer that we want to enable or disable.
 * @param enable: TRUE to enable PDC sequencer else FALSE.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
#define pdc_enable pdc_seq_enable
pdc_seq_result_t pdc_seq_enable (pdc_seq_handle handle, boolean enable);

/**
 * <!-- pdc_seq_set_lpm -->
 *
 * @brief Sets the low power mode (lpm) configuration for the PDC sequencer.
 *        This configuration will be effective when the PDC sequencer kicks
 *        in next time.
 *
 * @dependency pdc_seq_driver_init, 
 *             pdc_seq_handle_init
 *
 * @param handle: Handle to PDC sequencer we are configuring for LPM.
 * @param mode_id: low power mode id.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_set_lpm (pdc_seq_handle handle, uint16 mode_id);

/**
 * <!-- pdc_set_wakeup_time -->
 *
 * @brief Sets up the wake up time for the given PDC.
 *
 * @dependency pdc_seq_driver_init, 
 *             pdc_seq_handle_init
 *
 * @param handle: Handle to PDC sequencer for which we are configuring 
 *                wakeup time.
 * @param wakeup: Absolute time at which we want PDC to wake up.
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
#define pdc_set_wakeup_time pdc_seq_set_wakeup_time
pdc_seq_result_t pdc_seq_set_wakeup_time (pdc_seq_handle handle, uint64 wakeup);

/**
 * <!-- pdc_seq_profile_ts_count -->
 *
 * @brief Returns number of profiling timestamps available for a PDC instance.
 *
 * Expected usage is calling this function once during init time.
 *
 * @dependency pdc_seq_driver_init, 
 *             pdc_seq_handle_init
 *
 * @param handle: PDC handle for which we are querying timestamp count.
 * 
 * @return Number of profiling timestamps
 *         Non-zero if present
 *         0 if no profiling timestamps or invalid input.
 */
uint32 pdc_seq_profile_ts_count (pdc_seq_handle);

/**
 * <!-- pdc_seq_enable_profile_ts -->
 *
 * @brief Enables profiling timestamps for PDC sequencer.
 *
 * These timestamps are enabled by default.
 *
 * @dependency pdc_seq_driver_init, 
 *             pdc_seq_handle_init
 *
 * @param handle: PDC handle to enable profiling timestamps for.
 * @param enable: TRUE to enable profiling else FALSE.
 *                (Enabled by default)
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
pdc_seq_result_t pdc_seq_enable_profile_ts
(
  pdc_seq_handle handle, 
  boolean enable
);

/**
 * <!-- pdc_seq_profile_ts -->
 *
 * @brief Returns a pointer to buffer that has latest profiling timestamps.
 *
 * @dependency pdc_seq_driver_init, 
 *             pdc_seq_handle_init
 *
 * @param handle: PDC handle for which we want to read profiling timestamps.
 * @param count: Number of timestamps read (output)
 *
 * @return Pointer to timestamp buffer if successful else NULL.
 */
const uint64* pdc_seq_profile_ts (pdc_seq_handle handle, uint32 *count);

/**
 * <!-- pdc_enable_arc_timer -->
 *
 * @brief Enables timer valid signal to ARC from a given PDC. Also allows 
 *        propagating PDC timer match value to ARC in HW.
 *
 * By default, this is enabled and should be overrided only when necessary.
 *
 * @dependency pdc_seq_driver_init,
 *             pdc_seq_handle_init
 *
 * @param handle: Handle to PDC for which we enable/disable timer sent to ARC.
 * @param enable: TRUE to enable else FALSE
 *
 * @return PDC sequencer error code (@see pdc_seq_result_t)
 */
#define pdc_enable_arc_timer pdc_seq_enable_arc_timer
pdc_seq_result_t pdc_seq_enable_arc_timer
(
  pdc_seq_handle handle, 
  boolean enable
);

#endif // __PDC_SEQ_H__

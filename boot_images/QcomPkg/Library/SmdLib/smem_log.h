#ifndef SMEM_LOG_H 
#define SMEM_LOG_H

/*===========================================================================

            S M E M L O G . H  

===========================================================================*/

/**
  @file smem_log.h
*/
/* A feature in the Shared Memory subsystem that logs information to a single 
  shared buffer in shared memory.
*/


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMEM_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMEM_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smem_log" group 
         description in the SMEM_mainpage file. 
*/


/*
USAGE    Step 1: Add "use DEBUG" (excluding Quotes) to the build of your choice in 
            msm7500.builds
    Step 2: #include "smem_log.h" in the file you want to use the shared memory
            log buffer.
    Step 3: Start writing to shared memory log buffer with debug value(s) by 
            calling SMEM_LOG_EVENT. Quick example. 
			See services/oncrpc/oncrpctask.c
*/

/*===========================================================================
Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/11   ap      Added SMEM_LOG EVENT BASE for IPC_ROUTER, QCCI, QCSI
04/04/11   bt      Remove the Static log and its defines.
08/12/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
05/24/10   hwu     Added SMEM_LOG_RRC_EVENT_BASE 
03/16/10   hwu     Added PMIC log ID. 
02/16/10   tl/esh  Added Doxygen tags
02/16/10   tl      Corrected function call for SMEM_LOG_EVENT6_EX macro
01/28/10   hwu     Added changes to move smem_log into OAL.
11/19/09   sa      warning cleanup, sync-up with symbian
06/10/09   bfc     CMI updates.
04/01/09   ih      Removed featurization
03/20/08   sa      Added support for rpc router messages.
08/16/07   ebb     Added smem_log_event_static declaration.
06/29/07   ptm     Add sleep event base.
03/20/07   taw     Include smem_target.h instead of customer.h, comdef.h and
                   target.h.
11/21/06   hwu     Added SMEM_LOG_DCVS_EVENT_BASE.
11/14/06   taw     Added the SMEM_LOG_NUM_ENTRIES define and removed the 
                   arguments to smem_log_init().
05/16/06   ptm     Added new smem event base symbol for ERROR.
03/24/06   ptm     Add init API.
02/22/06   ptm     Add 6 data value API.
01/27/06   ddh     Added new smem event base symbol for DEM
04/20/05   ptm     Add event base symbols for debug, smem, tmc and timetick.
03/17/05   ~SN     Initial Version.

===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup smemlog
@{ */

/** @name Event Base
  Each shared memory log event is uniquely identified by an event ID, stored
  in bits 27 to 0 of smem_log_struct_type::identifier. The event ID consists of
  the event base, which is defined here, and the per-component event ID,
  which is determined by each component.
@{ */
/** Event base for debug log events. */
#define SMEM_LOG_DEBUG_EVENT_BASE         0x00000000

/** Event base for ONCRPC log events. */
#define SMEM_LOG_ONCRPC_EVENT_BASE        0x00010000

/** Event base for shared memory and SMD log events. */
#define SMEM_LOG_SMEM_EVENT_BASE          0x00020000

/** Event base for Task and Mode Controller log events. */
#define SMEM_LOG_TMC_EVENT_BASE           0x00030000

/** Event base for timetick log events. */
#define SMEM_LOG_TIMETICK_EVENT_BASE      0x00040000

/** Event base for Dynamic Environment Manager (DEM) log events. */
#define SMEM_LOG_DEM_EVENT_BASE           0x00050000

/** Event base for error log events. */
#define SMEM_LOG_ERROR_EVENT_BASE         0x00060000

/** Event base for Dynamic Clock and Voltage Scaling (DCVS) log events. */
#define SMEM_LOG_DCVS_EVENT_BASE          0x00070000

/** Event base for sleep log events. */
#define SMEM_LOG_SLEEP_EVENT_BASE         0x00080000

/** Event base for RPC router log events. */
#define SMEM_LOG_RPC_ROUTER_EVENT_BASE    0x00090000

/** Event base for clock regime log events. */
#define SMEM_LOG_CLKREGIM_EVENT_BASE      0x000A0000

/** Event base for PMIC log events. */ 
#define SMEM_LOG_PMIC_EVENT_BASE          0x000B0000 

/** Event base for WCDMA log events. */
#define SMEM_LOG_RRC_EVENT_BASE           0x000C0000

/** Event base for IPC Router log events. */  
#define SMEM_LOG_IPC_ROUTER_EVENT_BASE    0x000D0000  

/** Event base for QMI Client log events. */  
#define SMEM_LOG_QMI_CCI_EVENT_BASE       0x000E0000  

/** Event base for QMI Service log events. */  
#define SMEM_LOG_QMI_CSI_EVENT_BASE       0x000F0000  

/** @} */ /* end_name_group Event Base */


/** @name Windows Mobile IOCTL
  For Windows&reg;&nbsp;Mobile, smem_log is compiled into the OEM Adaptation 
  Layer (OAL). To access logging, call KernelIoControl with one of the Windows 
  Mobile OAL I/O control codes (SMEM_WM_IOCTL_LOG_*).
@{ */
/** Calls the smem_log_event I/O control function with three data parameters. */
#define SMEM_WM_IOCTL_LOG_EVENT_3         0x1

/** Calls the smem_log_event I/O control function with six data parameters. */
#define SMEM_WM_IOCTL_LOG_EVENT_6         0x2

/** Calls the smem_log_open I/O control function. */
#define SMEM_WM_IOCTL_LOG_OPEN            0x3

/** Calls the smem_log_read I/O control function. */
#define SMEM_WM_IOCTL_LOG_READ            0x4

/** @} */ /* end_name_group WM IOCTL */

/** Identifies the available log areas.
*/
typedef enum {
  /** Identifies the first available log entry. */
  SMEM_LOG_AREA_FIRST,

  /** Circular multiprocessor logging area that is used by the
      ONCRPC and SMD. */
  SMEM_LOG_AREA_CIRCULAR_MPROC = SMEM_LOG_AREA_FIRST,

  /** Circular power logging area that is used by power optimization. */
  SMEM_LOG_AREA_CIRCULAR_POWER,

  /** Identifies the number of log entries available. */
  SMEM_LOG_AREA_COUNT
} smem_log_area;

/** @brief Structure of a single log entry in shared memory.
*/
typedef struct {
  /** Log entry identifier.
 
   This identifier consists of the processor ID, continuation number, subsystem 
   ID, and event ID.

   - Bits 31-30 are the processor ID: 10 => application, 00 => modem, 
     01 => Hexagon.
   - Bits 29-28 are the continuation number.
   - Bits 27-16 are the subsystem ID (event base); see SMEM_LOG_*_EVENT_BASE.
   - Bits 15-0  are the event ID.

   Bit 28 is set for log entries that are a continuation of the previous log
   entry, allowing more data to be written in a single transaction. In the
   current implementation, bit 29 is reserved.

   The subsystem ID and event ID uniquely identify a log entry type, and
   dictate the identity of their arguments. */
  uint32 identifier;

  /** Log entry timestamp from the 32 kHz timer.

   Each processor has its own timer, which is synchronized early in the
   boot process, but not necessarily before any log entries can be written.
   Timetick values are to be regarded as mostly accurate, but it is possible
   that the values may drift over time, and it is possible for later
   timeticks to precede earlier timeticks. Only the ordering in the shared
   memory log should be used to determine which events occurred first.

   Hexagon does not have a 32 kHz timer; all Hexagon log entries will have an 
   arbitrary timetick value. */
  uint32 timetick;

  /** First word of data provided by the user. */
  uint32 data1;

  /** Second word of data provided by the user. */
  uint32 data2;

  /** Third word of data provided by the user. */
  uint32 data3;
} smem_log_struct_type;

/** @brief Command structure for the smem_log IOCTL.
*/
typedef struct
{ 
  uint32  subIoctl;    /**< SMEM I/O control command. */
  smem_log_area area;  /**< Log area to use. */
  uint32  id;          /**< ID tag for the event. */
  uint32  reserved;    /**< Reserved - must be zero. */
  uint32  data1;       /**< User data (depends upon the value of subIoctl). */
  uint32  data2;       /**< User data (depends upon the value of subIoctl). */
  uint32  data3;       /**< User data (depends upon the value of subIoctl). */
  uint32  data4;       /**< User data (depends upon the value of subIoctl). */
  uint32  data5;       /**< User data (depends upon the value of subIoctl). */
  uint32  data6;       /**< User data (depends upon the value of subIoctl). */

  smem_log_struct_type *events_ptr; /**< Pointer to the array of logged events. */
  uint32  event_count;              /**< Number of events in the array. */
  uint32  *lost_count_ptr;          /**< Number of events overwritten 
                                         since the log was last read. */
  uint32  *events_remaining_ptr;    /**< Number of events remaining in the 
                                         SMEM logging area. */
}smem_log_ioctl; 

/*=========================================================================== 

                      PUBLIC FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================
FUNCTION      smem_log_init
===========================================================================*/
/**
  Initializes the smem_log subsystem by opening SMEM_LOG_AREA_CIRCULAR_MPROC and 
  SMEM_LOG_AREA_STATIC. This function must be called before any other functions 
  are used.

  @return
  None.

  @dependencies
  Shared memory must have been initialized using smem_init() or smem_boot_init().
*/
/*=========================================================================*/
extern void smem_log_init( void );

/*===========================================================================
FUNCTION      smem_log_open
===========================================================================*/
/**
  Initializes the shared memory log area. This function must be called before 
  any other functions are used with the same log area.

  @param[in] area   Logging area to open.

  @return
  0 -- Log open was successful.\n
  -1 -- Log open failed.

  @dependencies
  Shared memory must have been initialized using smem_init() or smem_boot_init().

  @sideeffects
  Allocates SMEM entries for the logging area.
*/
/*=========================================================================*/
extern int smem_log_open( smem_log_area area );

/*===========================================================================
FUNCTION      smem_log_read
===========================================================================*/
/**
  Reads shared memory log events from a specified area into a provided buffer.
   
  This function reads up to event_count log entries from the log specified by 
  log_area into the memory pointed to by events. The function maintains the 
  last read position.

  If the log wrapped and overwrote log entries before they 
  could be read on this processor, *lost_count is set to the number of 
  entries that were lost. The function returns the number of log entries that 
  were read into local memory.

  The client is responsible for calling the smem_log_read() function at an 
  appropriate interval so that log entries are read from the SMEM log before the 
  log is overwritten by new log entries. This function guarantees that all log 
  entries returned from the function will be valid and complete, but does not
  guarantee that no log entries will be lost. The client can use the value 
  returned by lost_count to request changes in the size of the log at compile 
  time, or to increase the frequency at which the log is read.

  Only log areas configured as SMEM_LOG_READWRITE can be read using 
  smem_log_read() to prevent a race condition when smem_log_read() tries to read 
  an entry that has not been fully written. The static log and new logs are
  configured as SMEM_LOG_READWRITE, which keeps the spinlock locked until the 
  entire log entry has been written, eliminating the possibility for this 
  race condition.

  @param[in] area          Logging area to read from.
  @param[in] event_count   Number of events to read.
  @param[out] events       Buffer containing at least event_count shared 
                           memory log events.
  @param[out] lost_count   Number of events that were overwritten since the 
                           log was last read.
  @param[out] events_remaining   Number of events that remain in this SMEM 
                                 logging area. Clients can use this value to 
                                 determine the buffer size to pass to this 
                                 function.

  @return
  The number of SMEM log entries that were read into events, or a negative 
  number if an error occurred.

  @dependencies
  The shared memory log specified by the area must have been opened by calling 
  smem_log_open() or smem_log_init().

  @sideeffects
  Advances the local read position on the log area.\n
  Uses a spinlock for exclusive access to the event log.
*/
/*=========================================================================*/
extern int smem_log_read( smem_log_area area,
                          uint32 event_count,
                          smem_log_struct_type * events,
                          uint32 * lost_count,
                          uint32 * events_remaining );

/*===========================================================================
FUNCTION      smem_log_event
===========================================================================*/
/**
  Writes an entry in the shared memory event log. The log index is treated with 
  extreme caution to prevent writing outside of the event array.

  @param[in] area         Logging area to which to write.
  @param[in] identifier   ID tag for the event.
  @param[in] reserved     Must be 0.
  @param[in] data1        First word of the user data.
  @param[in] data2        Second word of the user data.
  @param[in] data3        Third word of the user data.

  @return
  None.

  @dependencies
  The shared memory log specified by the area must have been opened by calling 
  smem_log_open() or smem_log_init().

  @sideeffects
  Uses a spinlock for exclusive access to the event log.
*/
/*=========================================================================*/
extern void smem_log_event( smem_log_area area,
                            uint32 identifier,
                            uint32 reserved,
                            uint32 data1,
                            uint32 data2,
                            uint32 data3 );

/*===========================================================================
FUNCTION      smem_log_event6
===========================================================================*/
/**
  Writes two consecutive entries of three words each into the shared memory 
  event log to store a total of six words of data.
  
  This function allows the caller to write more data items than smem_log_event(). 
  The log index is treated with extreme caution to prevent writing outside of 
  the event array.

  This function is not supported on static logs; attempts to use it to log to 
  static logs are redirected to the MPROC log.

  @param[in] area        Logging area to which to write.
  @param[in] identifier  ID tag for the event.
  @param[in] reserved    Must be 0.
  @param[in] data1       First word of the user data.
  @param[in] data2       Second word of the user data.
  @param[in] data3       Third word of the user data.
  @param[in] data4       Fourth word of the user data.
  @param[in] data5       Fifth word of the user data.
  @param[in] data6       Sixth word of the user data.

  @return
  None.

  @dependencies
  The shared memory log specified by the area must have been opened by calling 
  smem_log_open() or smem_log_init().

  @sideeffects
  This function uses a spinlock for exclusive access to the event log.
*/
/*=========================================================================*/
extern void smem_log_event6( smem_log_area area,
                             uint32 identifier,
                             uint32 reserved,
                             uint32 data1,
                             uint32 data2,
                             uint32 data3,
                             uint32 data4,
                             uint32 data5,
                             uint32 data6 );

/** @name Helper Macros
  Helper macros can be used to call the shared memory logging functions, 
  although they exist primarily for historical reasons.

  Historically, these helper macros were defined differently depending on the 
  state of FEATURE_SMEM_LOG. If the feature was defined, these macros were 
  defined to call the underlying shared memory log functions. If the feature 
  was not defined, these macros were defined to evaluate to an empty expression.
  
  In more modern times, these macros are always defined to call the underlying 
  function, and an empty implementation is selected at link time if 
  FEATURE_SMEM_LOG is not defined.
@{ */

/** Helper macro to log a three-entry log event; calls smem_log_event(). */
#define SMEM_LOG_EVENT(identifier,data1,data2,data3) \
        smem_log_event(SMEM_LOG_AREA_CIRCULAR_MPROC,identifier,0,data1,data2,data3)

/** Helper macro to log a six-entry log event; calls smem_log_event6(). */
#define SMEM_LOG_EVENT6(identifier,data1,data2,data3,data4,data5,data6) \
        smem_log_event6(SMEM_LOG_AREA_CIRCULAR_MPROC,identifier,0,data1,data2,data3,data4,data5,data6)

/** Helper macro to log a three-entry log event; calls smem_log_event(). */
#define SMEM_LOG_EVENT_EX(area, id, reserved, data1, data2, data3) \
        smem_log_event(area, id, reserved, data1, data2, data3)

/** Helper macro to log a six-entry log event; calls smem_log_event6(). */
#define SMEM_LOG_EVENT6_EX(area, id, reserved, d1, d2, d3, d4, d5, d6) \
	smem_log_event6(area, id, reserved, d1, d2, d3, d4, d5, d6)

/** @} */ /* end_name_group Helper Macros */

/** @} */ /* end_addtogroup smemlog */

#ifdef __cplusplus
}
#endif

#endif /* SMEM_LOG_H */


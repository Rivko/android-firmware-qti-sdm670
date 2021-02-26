#ifndef SMSM_TYPE_H
#define SMSM_TYPE_H

/**
 * @file smsm_type.h
 *
 * Public state bit definitions and enumerations used by SMSM
 */

/*==============================================================================
     Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/13   rs      Add SMSM_NETWORK_DRIVER_UNMOUNT/_ACK for use by A2.
01/29/13   bt      Fix comment.
08/02/12   pa      Add SMSM_A2_POWER_CONTROL_NAK.
06/15/12   bt      Rename SMSM_LTE_COEX_AWAKE to SMSM_PROC_AWAKE.
05/15/12   bt      Rename SMSM_ERR_SRV_READY to SMSM_SUBSYS2AP_STATUS.
05/07/12   bt      Add SMSM_ERR_SRV_READY bit for detecting early failures.
11/02/11   bm      Add SMSM_A2_RESET_BAM bit for USB(Apps) - A2(Modem) power
                   mode signalling.
10/19/11   bm      Add SMSM_A2_FORCE_SHUTDOWN bit for USB(Apps) - A2(Modem)
                   A2 power down signaling.
10/13/11   bt      SMSM_LTE_COEX_AWAKE bit for LTE Coex remote state awareness.
08/08/11   bt      Add SMSM_A2_POWER_CONTROL_ACK for further A2 PC support.
07/25/11   bt      Changed RIVA name to WCNSS to match Linux implementation.
05/10/11   bt      Add SMSM_A2_POWER_CONTROL bit for power-collapse support.
05/05/11   tl      Created separate header file for SMSM enums and defines
===========================================================================*/

/*--------------------------------------------------------------------------
  State Bit Definitions
---------------------------------------------------------------------------*/

/** @name Processor State Bit Definition
  The SMSM state is a 32-bit bitfield with any combination of the following 
  values.
@{ */
/** Value used by the SMSM API to indicate that the SMSM state is unknown. */
#define SMSM_UNKNOWN                    0x80000000

/** State that is set when SMSM shared state data is initialized and SMSM ISRs
  are installed. */
#define SMSM_INIT                       0x00000001

/* Used to indicate the power status of the Algorithm Accelerator during 
 * power collapse on 8960.  Both Apps and Modem set this bit in their 
 * corresponding state entry, and any other processor can read them. */
#define SMSM_A2_POWER_CONTROL           0x00000002

/** State in which the two processors can wait on each other to get to the point 
  where the SMD is ready for initialization. It synchronizes SMD initialization. */
#define SMSM_SMDWAIT                    0x00000004

/** State in which the two processors can wait on each other to get to the point 
  where SMD initialization is complete. RPC startup cannot proceed until SMD 
  initialization is complete. */
#define SMSM_SMDINIT                    0x00000008

/** State in which the two processors can wait on each other to get to the point 
  where ONCRPC is ready for initialization. It synchronizes RPC initialization. */
#define SMSM_RPCWAIT                    0x00000010

/** State in which the two processors can wait on each other to get to the point 
  where ONCRPC initialization is complete. The UI and other clients of 
  cross-processor APIs will fail if the RPC is not initialized. */
#define SMSM_RPCINIT                    0x00000020

/** State that indicates a processor is restarting. This typically happens as a 
  result of an ERR_FATAL error. */
#define SMSM_RESET                      0x00000040

/** State used by Modem and Q6 to indicate that a particular audio event has 
 *  occurred on the opposite processor.  Used by Audio team for general voice 
 *  debugging purposes. */
#define SMSM_VOICE_DEBUG                0x00000080

/** Either processor enters the Run state when SMD and RPC are running on both
  processors. */
#define SMSM_RUN                        0x00000100

/** Used by WLAN Team.  This represents whether the DXE descriptor rings 
  (in aCPU DDR) for the TX channels (high and low priority) have descriptors 
  not yet returned/completed by the DXE HW (in RIVA). */
#define SMSM_WLAN_TX_RINGS_EMPTY        0x00000200

/** Used by WLAN Team.  This is used to enable the DXE TX channels 
 * (high and low) to cause DXE HW to process the next batch of descriptors. */
#define SMSM_WLAN_TX_ENABLE             0x00000400

/** This is used by the A2 to ack the power request status which is 
 *  requested by the SMSM_A2_POWER_CONTROL bit. */
#define SMSM_A2_POWER_CONTROL_ACK       0x00000800

/** Used by the LTE Coexistence team on ModemSW and RIVA, so that each 
 * processor can tell when the other processor (and hence LTE) is asleep, and 
 * refrain from writing messages or sending interrupts during that time.
 */
#define SMSM_PROC_AWAKE                 0x00001000

/** Used by the HSUSB driver on AP to signal a "hard" power collapse of A2
 * to A2 drivers running on Modem, which, then power down the A2 perepheral
 * (Currently used for 9x15) */
#define SMSM_A2_FORCE_SHUTDOWN          0x00002000

/** Used to co-ordinate A2 power collapse, specifically to give A2 the ability 
 *  to tell Apps if it cannot go into power collapse under certain circumstances
 */
#define SMSM_A2_POWER_CONTROL_NAK       0x00004000

/** Used by the HSUSB driver on AP (triggered by HSIC Bus reset) to signal
 * BAM Reset to A2 drivers running on Modem, which, then powers down the 
 * A2-BAM after immidietely flushing any pending data.
 * Preserved for backwards compatibility, although SMSM_A2_POWER_CONTROL_NAK
 * is the correct name of the bit going forward. */
#define SMSM_A2_RESET_BAM               0x00004000

/** Indicates when the subsystem's Err Notification service is enabled.  
 * Owned by CoreBSP ERR team.  On MPSS, this refers to the watchdog.  Since the 
 * watchdog may not be enabled until late in the boot process, a subsystem 
 * may “silently fail” before that, during bootup or following SSR. 
 * Apps will set a timer when it restarts the subsystem and if it doesn’t see 
 * this bit get set after a period of time, then it will restart the 
 * subsystem again. */
#define SMSM_SUBSYS2AP_STATUS           0x00008000

/** Modem processor enters the Release state when it is finished 
  with the OEM Secondary Boot Loader (SBL) section. Once this state is set, the 
  applications processor can access that section (currently, it is part of the 
  Program Memory (PMEM) pool). */
#define SMSM_OEMSBL_RELEASE             0x00010000

/** Modem processor enters the Power-down state when it is to power 
  down the chip. */
#define SMSM_SYSTEM_POWER_DOWN          0x00040000

/** Applications processor enters the Reboot state when the modem is to 
  reboot the system. */
#define SMSM_SYSTEM_REBOOT              0x00080000

/** Applications processor enters the Download state when the modem is 
  to enter Download mode. */
#define SMSM_SYSTEM_DOWNLOAD            0x00100000

/** Either processor can enter the Loopback state to enable SMD Loopback tests. */
#define SMSM_SMD_LOOPBACK               0x00800000

/** Used by the Network Driver on AP to signal an unmount or
 * crash signal to A2 Driver running on Modem, which then powers
 * down the A2 peripheral. */
#define SMSM_NETWORK_DRIVER_UNMOUNT     0x01000000

/** Used by the A2 Driver on Modem to acknowledge unmount
 * or crash signal to the Network Driver on AP side after the
 * cleanup and shutdown sequence has been completed. */
#define SMSM_NETWORK_DRIVER_UNMOUNT_ACK 0x01000000

/** Applications processor enters the Wait state to tell the modem to 
  go into Wait state. */
#define SMSM_MODEM_WAIT                 0x02000000

/** Applications processor enters the modem-break state when it is put in
  a Debugbreak state. This state tells the modem to disable its software
  watchdog. */
#define SMSM_MODEM_BREAK                0x04000000

/** Applications processor enters the Continue state when it is notified 
  to transition from the Debugbreak state. This state tells the modem to enable 
  its software watchdog. */
#define SMSM_MODEM_CONTINUE             0x08000000 

/** QDSP uses this to tell other processor it is in RESET now.*/
#define SMSM_MM_QDSP_RESET              0x10000000 

/** User initiated REBOOT. Modem sets this state to let apps
 *  reboot the system. */
#define SMSM_SYSTEM_REBOOT_USR          0x20000000 

/** User initiated PowerDown. Modem sets this state to let
 *  apps to powerdown the system. */
#define SMSM_SYSTEM_POWERDOWN_USR       0x40000000 

/** @} */ /* end_name_group Processor State Bit Definition */
 
/*--------------------------------------------------------------------------
  Enumerated types
---------------------------------------------------------------------------*/

/**
  List of the supported processors. */
typedef enum 
{
  SMSM_APPS      = 0,  /**< &nbsp; */
  SMSM_MODEM     = 1,  /**< &nbsp; */
  SMSM_Q6        = 2,  /**< &nbsp; */
  SMSM_WCNSS     = 3,  /**< &nbsp; */
  SMSM_DSPS      = 4,  /**< &nbsp; */
  SMSM_NUM_HOSTS = 5,  /**< &nbsp; */
  SMSM_INVALID_HOST,   /**< &nbsp; */
}smsm_host_type;

/**
  List of the supported entries.
  
  @note The first SMSM_NUM_HOSTS entries must be identical to smsm_host_type.
*/
typedef enum 
{
  SMSM_APPS_STATE        = 0,  /**< &nbsp; */
  SMSM_MODEM_STATE       = 1,  /**< &nbsp; */
  SMSM_Q6_STATE          = 2,  /**< &nbsp; */
  SMSM_WCNSS_STATE       = 3,  /**< &nbsp; */
  SMSM_DSPS_STATE        = 4,  /**< &nbsp; */
  SMSM_RESERVED_0        = 5,  /**< &nbsp; */
  SMSM_RESERVED_1        = 6,  /**< &nbsp; */
  SMSM_RESERVED_2        = 7,  /**< &nbsp; */
  SMSM_NUM_ENTRIES       = 8,  /**< &nbsp; */
  SMSM_INVALID_ENTRY,          /**< &nbsp; */
} smsm_entry_type;

#endif /* SMSM_TYPE_H */

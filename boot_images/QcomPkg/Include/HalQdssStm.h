/*=============================================================================

FILE:         HalQdssStm.h

DESCRIPTION:     

Contains  functionalities to access the 4KB of memory mapped STM register set
that are connected through the APB bus. The STM hal is used by the STM
configuration driver in order to enable the generation of software instrumented
trace.  Functionalites exposed by the APIs include initializtion, wrappers to
configure/Control, read/write into hardware registers.


================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#ifndef HALQDSSSTM_H
#define HALQDSSSTM_H


/*============================================================================
                          INCLUDE FILES
============================================================================*/

#include "HALhwio.h"

/*============================================================================
                         CONSTANT DEFINITIONS
============================================================================*/

typedef enum
{
   PORT_SELECT_CTL_SELECT_ALL=0x0,
   PORT_SELECT_CTL_TRIGGER_ONLY=0x1,
   PORT_SELECT_CTL_TRACE_TRIGGER=0x3,
   PORT_SELECT_CTL_DEFAULT=PORT_SELECT_CTL_TRACE_TRIGGER,
   HAL_ENUM_32BITS(PORT_SELECT_CTL)
}HAL_qdss_stm_PortSelectCtl;

typedef enum
{
   MASTER_SELECT_CTL_ALL=0,
   MASTER_SELECT_CTL_SUBSET=1,
   MASTER_SELECT_CTL_DEFAULT=MASTER_SELECT_CTL_ALL,
   HAL_ENUM_32BITS(MASTER_SELECT_CTL)
}HAL_qdss_stm_MasterSelectCtl;

/**
 Define synchronization period in N bytes or 2^N bytes
 */
typedef enum {
   STM_SYNC_MODE_PERIOD_N_BYTES=0,
   STM_SYNC_MODE_PERIOD_2_POWER_N_BYTES=1,
   STM_SYNC_MODE_PERIOD_LAST=0x7fffffff
}HAL_qdss_SyncModeType;

typedef enum
{
   QDSS_STM_FEAT_SWOEN_FIELD=0,
   QDSS_STM_FEAT_SYNCEN_FIELD,
   QDSS_STM_FEAT_HWTEN_FIELD,
   QDSS_STM_FEAT_TSPRESCALE_FIELD,
   QDSS_STM_FEAT_TRIGCTL_FIELD,
   QDSS_STM_FEAT_TRACEBUS_FIELD,
   QDSS_STM_FEAT_STMSYNCR_FIELD,
   QDSS_STM_FEAT_FORCETS_FIELD,
   QDSS_STM_FEAT_TSFREQ_FIELD,
   QDSS_STM_FEAT_TS_FIELD,
   QDSS_STM_FEAT_PROT_FIELD,
   QDSS_STM_NUM_FEAT,
   HAL_ENUM_32BITS(QDSS_STM_FEAT_NAME)

/*TBD: fields in FEAT2 and FEAT3 registers*/
}HAL_qdss_stm_FeatureName;

typedef enum {
   QDSS_STM_FEAT_SWOEN_NOFEAT=0x0,
   QDSS_STM_FEAT_SWOEN_UNDEF=0x1,
   QDSS_STM_FEAT_SWOEN_DEFINE=0x2,

   QDSS_STM_FEAT_SYNCEN_NOFEAT=0x0,
   QDSS_STM_FEAT_SYNCEN_DEFINE_READ0=0x1,
   QDSS_STM_FEAT_SYNCEN_DEFINE_READ1=0x2,
   QDSS_STM_FEAT_SYNCEN_DEFINE_RW=0x3,
   HAL_ENUM_32BITS(QDSS_STM_FEAT_VALUE)
}HAL_qdss_stm_FeatureValue;


/*============================================================================
                        DATA DECLARATIONS
============================================================================*/

/*
  RW bit fields within STMSPTRIGCSR
*/
typedef struct{
   unsigned char bTrigDirWriteEnable:1;
   unsigned char bTrigSPTEREnable:1;
   unsigned char bTrigCtl:1;
   unsigned char:0;
} HAL_qdss_stm_TriggerCfgType;


typedef struct {
   HAL_qdss_stm_FeatureName eSTMFeatName;
   HAL_qdss_stm_FeatureValue eSTMFeatValue;
} HAL_qdss_stm_FeatureType;

typedef struct {
   uint32 nStartPort;
   uint32 nEndPort;
}HAL_qdss_stm_PortSelRet;

typedef struct {
   uint32 nStartMaster;
   uint32 nEndMaster;
}HAL_qdss_stm_MasterSelRet;


#define HAL_QDSS_STM_GET_FEAT_VAL(featname)  \
HAL_QDSS_STM_GET_FEAT_VAL_TRANS(QDSS_STM_FEAT_##featname##_FIELD)

#define HAL_QDSS_STM_GET_FEAT_VAL_TRANS(index) HAL_qdss_stm_GetFeature(index)

#define HAL_QDSS_STM_FEAT_NAME(name) QDSS_STM_FEAT_##name##_FIELD


/*============================================================================
                          PUBLIC FUNCTION
============================================================================*/

///*
//Function Name: HAL_qdss_stm_HalConfigInit
//
//Input:
//   nBaseAddr: Base address of the STM hardware block to be used for computing
//register offsets.
//
//Description:
//   Invoked by the driver to initialize the STM HAL module. The top level client
//must provide nBaseAddr identifying the register base address to be used in
//computing offsets. The HAL module SHOULD NOT reference the hardware by using
//physical address.
//   Internally the function obtains the base address of the STM hardware block
//and saves it to compute the register offsets.This API does NOT configure the
//STM hardware. Configuration of the STM block is the responsibility of the client
//driver which uses the HAL.
//
//Return: None
//
//*/
//void HAL_qdss_stm_HalConfigInit(uint32 nBaseAddr);
//
//
///*
//Function name: HAL_qdss_stm_SetConfigPhysAddr
//
//Input: Physical address
//
//Description:
//   The client driver sets the *physical address* of the STM configuration
//hardware block in order to map a 4KB virtual page that will be used as the base
//address for hardware access.
//
//Return: None
//*/
//void HAL_qdss_stm_SetConfigPhysAddr();

/*
Function Name: HAL_qdss_stm_SetConfigStimulusPort
*/

typedef uint32 hal_hwio_qdss_addr_type;

void HAL_qdss_stm_SetConfigStimulusPort(void);
hal_hwio_qdss_addr_type HAL_qdss_stm_GetStimulusPortStart(void);
uint32 HAL_qdss_stm_GetNumStimulusPorts(void);
uint8 *HAL_qdss_stm_GetStmPortInUseArray(void);

/*
Function Name: HAL_qdss_stm_Enable

Input: None

Description:
   Accepts writes to the STM extended stimulus ports through the AXI slaves
interface by setting the STMTCSR.EN bit to b'1. This API is not to be confused
with the Lock Access Register which controls write access to the STM registers.

Return: None
*/
void HAL_qdss_stm_Enable(void);

/*
Function Name: HAL_qdss_stm_Disable

Input: None

Description:
   Writes to the stimulus ports are dropped by the STM by setting the
STMTCSR.EN bit to b'0. This API is not to be confused with the Lock Access
Register (which controls write access to the STM registers).
   STM hardware flushes its trace buffer to output pending writes to the ATB
while ignoring subsequent write requests to the stimulus ports once the STM is
disabled.
   This function waits for the STM to empty its buffers before returning by
polling the STMTCSR.BUSY bit.

Return: None
*/

void HAL_qdss_stm_Disable(void);

/*
Function Name: HAL_qdss_stm_isEnabled

Input: None

Description:
   Returns the status of the STM Enable bit. Certain STM configurations can only
be performed depending on the status of this bit.

Return:
   TRUE: STM is enabled.
   FALSE: STM is disabled.
*/

extern boolean  HAL_qdss_stm_isEnabled(void);

/*
Function Name: HAL_qdss_stm_isBusy

Input: None

Description:
   Returns the status of the busy bit in the STMTCSR.

Return:
   TRUE: STM is busy. Occurs when the trace fifo is not empty.
   FALSE:  STM is not busy.
*/
extern boolean  HAL_qdss_stm_isBusy(void);
/*
Function Name: HAL_qdss_stm_TraceIdCfg

Input:
   nTraceId: Integer value of the payload of the ATID transaction.

Description
   When trigger is enabled and a trigger event occurs, a ATID transaction value
of 0x7D is output along with a payload defined by nTraceId(lower 6-bits only).
Configures the STMTCSR.TRACEID field.

Side Effect:
   To prevent trace corruption, this interface should be used only when STM is
disabled and STM isn't busy.

Return: None
*/

/**
*** TBD:Research its use on target
**/
void HAL_qdss_stm_TraceIdCfg(uint32 nTraceId);


/*
Function Name: HAL_qdss_stm_isCompEnabled

Input: None

Description:
   Determines if compression of STM data packets is enabled or not.

Return:
   TRUE: Compression is enabled
   FALSE: Compression is disabled
*/
boolean HAL_qdss_stm_isCompEnabled(void);

/*
Function Name: HAL_qdss_stm_CompEnable

Input: None

Description:
   Enables compression of STM packets generated.

Return: None
*/
void HAL_qdss_stm_CompEnable(void);

/*
Function Name: HAL_qdss_stm_CompDisable

Input: None

Description:
   Disables compression of STM packets generated.

Return: None
*/
void HAL_qdss_stm_CompDisable(void);


///*
//Function Name: HAL_qdss_stm_HWEventEnable
//
//Input: None
//
//Description:
//   Enable tracing of hadware events.
//
//Return: None
//*/
//extern void  HAL_qdss_stm_HWEventEnable(void);

/*
Function Name: HAL_qdss_stm_HWEventDisable

Input: None

Description:
   Disable tracing of hadware events.

Return: None
*/
extern void  HAL_qdss_stm_HWEventDisable(void);

/*
Function Name: HAL_qdss_stm_HWevent_isEnabled

Input: None

Description:
   Returns the status of the STM HW Events Enabled bit.

Return:
   TRUE: HWEvents is enabled
   FALSE: HWEvents is disabled
*/

extern boolean  HAL_qdss_stm_HWevent_isEnabled(void);


/*
Function Name: HAL_qdss_isSyncEnabled

Description:
   Determines if synchronization of STM packets is enabled or not by reading the
STMTCSR.SYNCEN field.

Return:
   TRUE: Synchronization is enabled.
   FALSE: Synchronization is diabled.
*/
boolean HAL_qdss_isSyncEnabled(void);

/*
Function Name: HAL_qdss_stm_SyncEnable

Description:
   Enable synchronization packets. If STMSYNCR register is implemented
STMTCSR.SYNCEN is a RO register and is always set to b'1.

Return: None
*/
void HAL_qdss_stm_SyncEnable(void);

/*
Function Name: HAL_qdss_stm_SyncDisable

Input: None

Description:
   Disable synchronization packets. If STMSYNCR register is implemented
STMTCSR.SYNCEN is a RO register and is always set to b'1. Synchronization is then
disabled by writting zero to STMSYNCR.

Return: None
*/
void HAL_qdss_stm_SyncDisable(void);

/*
Function Name: HAL_qdss_stm_SyncPeriodCfg

Input:
   eSyncMode:
    STM_SYNC_MODE_PERIOD_N_BYTES -- period is nBytes
    STM_SYNC_MODE_PERIOD_2_POWER_N_BYTES -- period is 2^nBytes
   nBytes: Number of bytes

Description:
   Configures the STM packet synchronization period in bytes by writing into the
STMSYNCR register.

Side Effect:
   Any writes to this register results in the STM performing a sync followed by
setting the register to the use defined period. Defining a period of zero
results in disabling sync packets.

Return: None
*/
void HAL_qdss_stm_SyncPeriodCfg(HAL_qdss_SyncModeType eSyncMode,uint32 nBytes);


/*
Function Name: HAL_qdss_stm_AccessUnLock

Description:
   Allows write access to the STM 4KB memory mapped registers. Access to the STM
harware block is required in order to configure the STM hardware.

Return: None
*/
extern void  HAL_qdss_stm_AccessUnLock(void);

/*
Function Name: HAL_qdss_stm_AccessLock

Description:
   Prevents write access to the STM 4KB memory mapped registers. Access to the
STM harware block is required in order to configure the STM hardware.

Return: None
*/
extern void  HAL_qdss_stm_AccessLock(void);

/*
Function Name: HAL_qdss_stm_isAccessLocked

Input: None

Description:
   Determines if access to the STM is locked or not.

Return:
   TRUE: STM is locked.
   FALSE: STM is unlocked.
*/

extern boolean  HAL_qdss_stm_isAccessLocked(void);


/*
Function Name: HAL_qdss_stm_SetClaimTag

Input:
   nTag: integer value NOT exceeding the maximum number of bits supported by the
register.

Description:
   Sets the STMCLAIMSET register to value provided by the nTag. Usage of the
claim tag registers are OPTIONAL and the protocol for setting the bit masks is
determined by the driver.

Return: None
*/
void HAL_qdss_stm_SetClaimTag(uint32 nTag);


/*
Function Name: HAL_qdss_stm_ClrClaimTag

Input:
   nTag: integer value NOT exceeding the maximum number of bits supported by the
register.

Description:
   Sets the STMCLAIMCLR register to value provided by the nTag. Usage of the
claim tag registers are OPTIONAL and the protocol for setting the bit masks is
determined by the driver.

Return: None
*/

void HAL_qdss_stm_ClrClaimTag(uint32 nTag);



/*
Function Name: HAL_qdss_stm_ClaimTagImplementMask

Input: None

Description:
   Returns a bit-mask of number of claim-tag bit fields implemented in
STMCLAIMSET. For example, if 4 bits are implemented it returns 0xF

Return:
   Bit mask

*/
extern uint32  HAL_qdss_stm_ClaimTagImplementMask(void);

/*
Function Name: HAL_qdss_stm_GetClaimTag

Description:
   Indicates the  value set in STMCLAIMSET. This achieved by reading the
STMCLAIMCLR register.

Return:
   Integer value of the claim tag in use.
*/
extern uint32  HAL_qdss_stm_GetClaimTag(void);


/*STM port enable
Function Name: HAL_qdss_stm_PortEnableMask

Input:
   mPortEnableMask: Bit mask that enables/disables specific ports at intervals of
   32 ports.b'1 enables a port and b'0 disables.

Description:
   Configures the STMSPER. Each bit defines the offset at which enablement
of port(s) at intervals of 32 starting with a particular port(selected by
HAL_qdss_stm_PortSelectCfg) is controlled. Bit 0 would control ports at START+0,
START+32,...,END and so on. Where START/END are the port # decided by the
configuration of STMSPSCR. Similarly bit 32 would control ports START+31,
START+63,...,END+31.

Return:
   QDSS_SUCCESS: operation was successful
   Any other code indicates a failure
*/

void HAL_qdss_stm_PortEnableMask(uint32 mPortEnableMask);

/*
Function Name: HAL_qdss_stm_PortSelectCfgAll

Description:
   Configures all ports so they can be enable/disabled through
   the HAL_qdss_stm_PortEnableMask() API. It is equivalent to
   calling HAL_qdss_stm_PortSelectCfg with PORT_SELECT_CTL_ALL.
*/

void HAL_qdss_stm_PortSelectCfgAll(void);


/*
Function Name: HAL_qdss_stm_PortSelectCfg

Input:
   nStartPort: Starting port-id of the extended stimulus port range.
   nNumPorts: Power of 2 number of ports to select starting with nStartPort.
   ePortSelectCtl: Apply the port select configuration for trigger and/or trace
   generation. If PORT_SELECT_CTL_ALL is chosen, the first two parameters are
   ignored and port selection applies to ALL ports.

Description:
   Controls the selection of the port range that can be enabled/disabled
through the HAL_qdss_stm_PortEnableMask() API.

   Configures the STM Stimulus Port Select Configuration Register(STMSPSCR)
based on the input parameters. In configuring the extended stimulus ports for
trace generation, the 64K ports per Master are organized into 32 ports per
group. The bit fields in STMSPSCR register are highlighted below. The lower n
bits of PORTSEL define the mask for the group interval. The b'1 at the n'th bit
is a delimiter for the mask. Bits from (n+1) to m when concatenated with the
mask provides the group selection range. Starting group-id is calculated by
discarding the b'1 delimiter.
Example 1: PORTSEL=b'000000000001 implies select group b'00000000000.
Translates to 32 ports starting at port 0.
Example 2: PORTSEL=b'000000000010 imples select group b'00000000000 to
b'00000000001. Translates to 32*2=64 ports starting port 0.

31........20|19.......2|...1.....0|
   PORTSEL    RESERVED     CTL

        PORTSEL
11(m)........(n+1)|(n)|(n-1)........0
                    1       xxxxxx

Return:
   pPort: The structure is populated with actual start and end port which may
 differ from the requested start and end port.

*/
void HAL_qdss_stm_PortSelectCfg(uint32 nStartPort,uint32 nEndPort,
   HAL_qdss_stm_PortSelectCtl ePortSelectCtl,HAL_qdss_stm_PortSelRet *pPortSel);

/*
Function Name: HAL_qdss_stm_MasterSelectCfgAll

Description:
 This function selects all stimulus port masters. It is equivalent to
calling with HAL_qdss_stm_MasterSelectCfg MASTER_SELECT_CTL_ALL.

*/

void HAL_qdss_stm_MasterSelectCfgAll(void);

/*
Function Name: HAL_qdss_stm_MasterSelectCfg

Input:
   nMasterID: Master ID to begin selection
   eMasSelCtl: Control the selection of stimulus port masters.
   MASTER_SELECT_CTL_ALL selects all masters. MASTER_SELECT_CTL_SUBSET applies
   the section only to the master-id specified by nMasterID.

Description:
   Enables a subset of Masters starting at nStartMasterID. nNumMasters controls
the number of masters to select by configuring STMSPMSCR. The register layout of
STMSPMSCR is:

   .31..............15.|.14........1.|.0.
         MASTSEL            RES        CTL

where, MASTSEL has a layout similar to PORTSEL where bottom N bits provide the
mask for selecting the interval along with the upper N+1 to Mth bit governing
the selection of the master with b'1 in Nth bit being a delimiter. The CTL bit
is always set to 1.

Return: None

*/

void HAL_qdss_stm_MasterSelectCfg(uint32 nMasterID,
      HAL_qdss_stm_MasterSelectCtl eMasSelCtl);



/*
Function Name: HAL_qdss_stm_hasTriggered

Input: None

Description:
   Probes the trigger status register and returns whether a trigger has occurred
or not. The value is valid only in single-shot mode.

Return:
   TRUE:  trigger has occurred
   FALSE: trigger has not occurred
*/
boolean HAL_qdss_stm_hasTriggered(void);

/*
Function Name: HAL_qdss_stm_TriggerStatusClear

Input: None

Description:
   Clear the trigger status register by  setting STMSPTRIGCSR.TRIGCLEAR to b'1

Return: None
*/
void HAL_qdss_stm_TriggerStatusClear(void);

/*Description: Single/multi-shot*/
void HAL_qdss_stm_TriggerShotCfg(HAL_qdss_stm_TriggerCfgType eSTMTrigger);
boolean HAL_qdss_stm_isSingleShot(void);

/*
Function name: HAL_qdss_stm_TimeStampEnable

Input: None

Description:
   Sets the STMTCSR.TSEN field to b'1 thus enabling the generation of time stamp
packets on every write to STMTSSTIMR register and in the generation of STPv2
FREQ packets during every sync period.

Return: None
*/
extern void  HAL_qdss_stm_TimeStampEnable(void);


/*
Function name: HAL_qdss_stm_TimeStampDisable

Input: None

Description:
   Sets the STMTCSR.TSEN field to b'0. Complement of the functionality enabled
by HAL_qdss_stm_TimeStampEnable

Return: None
*/
extern void  HAL_qdss_stm_TimeStampDisable(void);



/*
Function Name: HAL_qdss_stm_GenerateTimeStamp

Input: None

Description:
   Sets the FORCETS field in STMTSSTIMR to 1 there by forcing ONLY the
subsequent write to a stimulus port to generate a time stamp.

Return: None
*/
extern void  HAL_qdss_stm_GenerateTimeStamp(void);


/*
Function Name: HAL_qdss_stm_GetTimeStampFreq

Input: none

Description:
   Read value of STMTSFREQR register which specifies the frequency of the time
   stamp counter. It should be noted that this register does not configure the
   frequency of the time stamp counter clock but only serves as an indication
   of its clock frequency which is consumed by profiling tools.
*/
extern uint32 HAL_qdss_stm_GetTimeStampFreq(void);



/*
Function Name: HAL_qdss_stm_SetTimeStampFreq

Input:
   nFreq: Integer value of the frequency value to be used in the FREQ/FREQ_TS
STP packets that are generated during every sync period when TSEN is enabled.
Writing to this register also causes frequency packets to be generated
indicating the new frequency.
*/
extern void  HAL_qdss_stm_SetTimeStampFreq(uint32 nFreq);


/*
Function Name: HAL_qdss_stm_GetFeatures

Input: None

Description:
   The HAL internally maintains a list of hardware features that it  supports.
The hardware feature set is discovered at run time during Init() and saved
within the HAL as a static array(not allocated dynamically). Clients can request
access to this list.

Return:
   Pointer to an array of feature [name,value] pairs.
*/
HAL_qdss_stm_FeatureType const *HAL_qdss_stm_GetFeature(
         HAL_qdss_stm_FeatureName eSTMFeatName);


/*
**TBD*
1. Auxiliary and Qualcomm specific implementation registers
   HAL interface for the following registers will be added in subsequent
revisions depending on requirement. To begin with these registers will be left
untouched at their default reset state. The registers are:
STMAUXCR,STMITTRIGGER, STMITATBDATA0, STMITATBCTR2, STMITATBID, STMITATBCTR0

2. Enabling and Control of hardware events

3. Control triggers
*/

extern void halqdss_stm_test(void);

extern void HAL_qdss_stm_TrigGenTrigLocation(void);
extern void HAL_qdss_stm_EnableSingleShot(void);
extern void HAL_qdss_stm_DisableSingleShot(void);
extern void HAL_qdss_stm_HWEventSelectAll(void);
extern void HAL_qdss_stm_HWEventTriggerEnableAll(void);
extern void HAL_qdss_stm_HWEventErrorDetectEnable(void);
extern void HAL_qdss_stm_HWEventCompressionEnable(void);
extern void HAL_qdss_stm_HWEventATIDTriggerGenDisable(void);
extern void HAL_qdss_stm_HWEventTrigOutMultiShotMode(void);
extern void HAL_qdss_stm_SetDefaultSyncPeriod(void);


#endif  /* HALQDSSSTM_H_ */


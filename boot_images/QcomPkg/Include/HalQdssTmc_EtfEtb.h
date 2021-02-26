/*=============================================================================

FILE:         HalQdssTmc_EtfEtb.h     (was halqdss_etfetb.h)

DESCRIPTION:

================================================================================
                Copyright 2015 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
================================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/HalQdssTmc_EtfEtb.h#1 $
==============================================================================*/
#ifndef HALQDSSTMC_ETFETB_H_
#define HALQDSSTMC_ETFETB_H_

#include "HALcomdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize ETFETB HAL
 *
 * This function initializes the base address for ETFETB registers.
 * All ETFETB registers are accessed as offsets to this base address
 *
 */
void HAL_qdss_etfetb_HalInit(void);


/**
 * Unlock the ETFETB registers for access
 *
 * This method needs to be called after initializing ETFETB HAL and before
 * any other methods are called.
 */
void  HAL_qdss_etfetb_AccessUnLock(void);


/**
 * Lock the ETFETB registers and prevent further access.
 *
 */
void  HAL_qdss_etfetb_AccessLock(void);


/**
 * Returns ETFETB RAM size in number of 32 bit words.
 *
 */
uint32 HAL_qdss_etfetb_GetRamDepth(void);


/**
 * Sets the ETFETB write pointer to the specified value
 *
 * Trace capture should be disabled before this method is called
 *
 * @param dwVal New ETFETB write pointer value
 */
void  HAL_qdss_etfetb_SetWritePtr(uint32 dwVal);


/**
 * Gets the current value of the ETFETB Write pointer
 *
 * Trace capture should be disabled before this method is called
 *
 * @return ETFETB write pointer value
 */
uint32  HAL_qdss_etfetb_GetWritePtr(void);


/**
 * Sets the ETFETB trigger counter register
 *
 * Trigger counter specifies the number of words that should be captured
 * after a trigger event has occured. The ETFETB formatter stops accepting
 * trace data after the specified number of words are captured after a
 * a trigger occurs during trace data collection.
 * Trace capture should be disabled before this method is called.
 *
 * @param ETFETB trigger counter value to be set in number of 32 bit words
 */
void  HAL_qdss_etfetb_SetTriggerCounter(uint32 dwVal);


/**
 * Gets the current value of the ETFETB Read Pointer
 *
 * Trace capture should be disabled before this method is called.
 *
 * @return ETFETB read pointer value
 */
uint32  HAL_qdss_etfetb_GetReadPtr(void);


/**
 * Sets the ETFETB read pointer to the specified value
 *
 * Trace capture should be disabled before this method is called
 *
 * @param dwVal New ETFETB read pointer value
 */
void  HAL_qdss_etfetb_SetReadPtr(uint32 dwVal);


/**
 * Writes data to the ETFETB RAM
 *
 * The method writes the specified data value at the location pointed
 * to by ETFETB write pointer. The write pointer is incremented automatically
 * after write. Trace capture should be disabled before this method is called.
 *
 * @param dwVal data to be written
 */
void  HAL_qdss_etfetb_WriteData(uint32 dwVal);

/**
 * Reads data from the ETFETB RAM
 *
 * The method writes reads data from the ETFETB RAM.
 * The ETFETB read pointer is incremented automatically after read.
 * Trace capture should be disabled before this method is called.
 *
 * @param dwVal data to be written
 */
uint32  HAL_qdss_etfetb_ReadData(void);


/**
 * Disable ETFETB Trace Capture
 *
 */
void HAL_qdss_etfetb_DisableTraceCapture(void);


/**
 * Enable ETFETB Trace Capture
 *
 */
void HAL_qdss_etfetb_EnableTraceCapture(void);


/**
 * Sets the ETFETB Format and Flush Control Register to the specified value
 *
 * This method is requires knowledge of the FFCR register bits and is
 * typically not required. Other ETFETB HAL methods provide for the needed
 * functionality.
 *
 */
void HAL_qdss_etfetb_SetFFCR(uint32 dwVal);


/**
 * Flush the QDSS subsystem
 *
 * This method initiates a manual flush of the ETFETB which in turn results
 * in flushing other QDSS components that are upstream in the trace data
 * path.
 *
 */
void HAL_qdss_etfetb_Flush(void);

/**
 * Wait for flush completion
 *
 * This method spins in a busy loop until QDSS flush is complete.
 *
 */
void HAL_qdss_etfetb_FlushWait(void);


/**
 * Enable formatting of Trace data
 *
 * ETFETB formatter is turned off on reset and raw trace data is output
 * as acontinuous stream on trace capture. Calling this function before
 * starting trace capture results in trace data being output according to the
 * trace data formatter specification. When there are multiple sources
 * outputting trace, turning on formatter becomes necessary.
 *
 */
void HAL_qdss_etfetb_EnableFormatting(void);


#define QDSS_TMC_MODE_CBUF   0
#define QDSS_TMC_MODE_SWFIFO 1
#define QDSS_TMC_MODE_HWFIFO 2

void HAL_qdss_etfetb_SetMode(uint32 mode);
uint32 HAL_qdss_etfetb_GetMode(void);

void HAL_qdss_etfetb_SetFFCRETBDefault(void);

boolean  HAL_qdss_etfetb_IsEnabled(void);
uint32  HAL_qdss_etfetb_GetFFCR(void);
void HAL_qdss_etfetb_StopOnFlush(uint8 bEnable);
void HAL_qdss_etfetb_WaitForTMCReadyVal(uint8 val);
void HAL_qdss_etfetb_Drain(void);


#ifdef __cplusplus
}
#endif

#endif /* HALQDSSTMC_ETFETB_H_ */


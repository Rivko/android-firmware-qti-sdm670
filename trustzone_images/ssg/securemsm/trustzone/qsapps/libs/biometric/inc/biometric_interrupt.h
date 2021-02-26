/**
@file biometric_interrupt.h
@brief This file defines an interface that allows a TA to wait for an HLOS event (e.g. HW interrupts)
*/
/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

when         who      what, where, why
--------     ---      ------------------------------------
Jun/22/2015  el       Initial version.
===========================================================================*/
#ifndef BIOMETRIC_INTERRUPT_H
#define BIOMETRIC_INTERRUPT_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
#include "biometric.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define MAX_FILE_NAME_LEN 128
#define BIO_INFINITE 0xFFFFFFFF

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum bio_hlos_event_type_
{
	// use this event type if you want to poll some file or sysfs node (e.g. /sys/class/gpio/gpio<N>/value)
	BIO_HLOS_EVENT_FILE_POLL,

} bio_hlos_event_type;


typedef struct bio_file_poll_config_
{
	// the name of the file or sysfs node that needs to be poll()ed
	char file_name[MAX_FILE_NAME_LEN+1];

} bio_file_poll_config;


typedef union bio_hlos_event_config_
{
	bio_file_poll_config file_poll;

} bio_hlos_event_config;

 
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
 
/**
 * @brief Blocks the calling thread until the desired HLOS event or a timeout expiration takes place.
 *
 * @param[in] event_type    The type of event to wait for
 * @param[in] event_config  A pointer to a union struct that holds a description of the event we should wait for. This parameter can't be NULL.
 * @param[in] event_timeout timeout interval in millisecond that once expires the function needs to return no matter if the event took place or not.
 *                          use the value 'BIO_INFINITE' to indicate endless waiting for the event.
 *
 * @returns BIO_NO_ERROR when the HLOS event occurs,
 *          BIO_TIMEOUT_EXPIRATION if the event didn't take place in the specified timeout interval
 *          or any other error code to indicate the exact failure reason.
*/
BIO_ERROR_CODE bio_wait_for_hlos_event(bio_hlos_event_type          event_type,
									   const bio_hlos_event_config* event_config,
									   uint32_t                     event_timeout);

#endif // BIOMETRIC_INTERRUPT_H

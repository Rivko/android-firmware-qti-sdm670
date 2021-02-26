#ifndef QUSB_AL_FAST_ENUM_H
#define QUSB_AL_FAST_ENUM_H
/*=======================================================================*//**
 * @file        qusb_al_fast_enum.h
 * @author:     shreyasr
 * @date        25-JUL-2014
 *
 * @brief       QUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) implementation.
 *
 * @details     This file contains the implementation of the APIs to be used by
 *              the boot Sahara layer for performing fast enumeration.
 *
 * @note        
 *
 *              Copyright 2014 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when       who     what, where, why
// --------   ---     ----------------------------------------------------------
// 07/25/14   shreyasr  Initial revision
//
// ===========================================================================
//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"     // basic types as byte word uint32 etc
#include "qusb_common.h"

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------
// ===========================================================================
/**
 * @function    qusb_al_fast_enum_init
 * 
 * @brief       Initialize the Function Driver for performing fast enumeration
 *
 * @param       [IN] speed_required- Maximum Speed Required
 *
 *          
 * @return      None
 * 
 * @Note        For fast enum, currently we support only HS.   
 */
 // ===========================================================================
void qusb_al_fast_enum_init(qusb_max_speed_required_t speed_required);
// ===========================================================================
/**
 * @function    qusb_al_fast_enum_shutdown
 * 
 * @brief       API to perform the shutdown of the function driver 
 *
 * @param       None
 *
 *          
 * @return      None
 * 
 * @Note          
 */
 // ===========================================================================
void qusb_al_fast_enum_shutdown(void);
// ===========================================================================
/**
* @function    qusb_al_fast_enum_poll
* 
* @brief       API to poll for USB events to perform enumeration 
*
* @param       None
*
*          
* @return      TRUE : If SET_CONFIGURED is received
*              FALSE: If SET_CONFIGURED is not received
* 
* @Note          
*/
// ===========================================================================
boolean qusb_al_fast_enum_poll(void);
// ===========================================================================
/**
 * @function    qusb_al_do_fast_enum
 * 
 * @brief       API to perform Fast Enumeration- Extern API 
 *
 * @param       [IN]- speed_required - Maximum Speed at which the device should 
 *                                     be configured
 *
 *          
 * @return      None
 * 
 * @Note        Sahara Layer should call this function to perform the
 *              the enumeration. 
 */
 // ===========================================================================
void qusb_al_do_fast_enum(qusb_max_speed_required_t speed_required);

#endif /*QUSB_AL_FAST_ENUM_H*/

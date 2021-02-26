#ifndef TIMETICK_UEFI_H
#define TIMETICK_UEFI_H

/*==============================================================================

FILE:      TimetickUEFI.h

DESCRIPTION: Public API interface for the timetick module.

===========================================================================
             Copyright (c) 2012-2013 QUALCOMM Technologies, Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================

                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/07/13   pbi      Added Runtime support for Timetick.
 06/11/12   pbi      Updated for B family.
 01/18/12   pbi      Added a new API, Timetick_SetTimerPeriod
 03/15/11   pbi      Added the header Uefi.h
                     Added a new API, Timetick_Init.
 02/08/11   pbi      Created.


=============================================================================*/
#include <Uefi.h>
#include "com_dtypes.h"

/*
 *
 * List of all timers supported on UEFI.
 */
typedef enum
{
  TIMETICK_TIMER_GPT0                 = 0,
  TIMETICK_TIMER_GPT1                 = 1,
  TIMETICK_TIMER_ADGT                 = 2,
  TIMETICK_QTIMER                     = 3,
  TIMETICK_NUM_TIMERS,

} Timetick_timer_Type;


typedef UINT32                    Timetick_time_32Type;  /**< -- Time in 32bit ticks*/ 

typedef UINT64                    Timetick_time_64Type; /**< -- Time in 64bit ticks*/ 

/* ============================================================================
**  Function : Timetick_Init
** ============================================================================
*/
/*!
    @brief
    Initializes the Timetick module.

    @details
    For B family, gathers the QTimer addresses from the Pcd variables and configures them
    to allow access.

    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval EFI_SUCCESS always.     
    @sa None
*/
EFI_STATUS Timetick_Init( void );


/* ============================================================================
**  Function : Timetick_Enable
** ============================================================================
*/
/*!
    @brief
    Enables/Disables the Timer for A family 
    Enables/Disables the Timer interrupt for B family 

    @details
    The QTimer will be running always. This enable is to just enable the
    QTimer interrupt to be generated.
 
    @param[in] nTimer  timer of type Timetick_timer_Type
    @param[in] enable  Boolean to enable/disable the timer(interrupt).
                   
    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval EFI_INVALID_PARAMETER if an invalid parameter is passed for
            Timetick_timer_Type, EFI_SUCCESS otherwise.
    
    @sa None
*/
EFI_STATUS Timetick_Enable( Timetick_timer_Type, boolean );

/* ============================================================================
**  Function : Timetick_ClearOnMatchEnable
** ============================================================================
*/
/*!
    @brief
    Enables/Disables the Timetick Clear on match feature of the specified timer.

    @details
    Calling this API before calling Timetick_Enable ensures that the timer is
    cleared the first time it matches the value programmed in the match value
    register.Setting the Match value to zero and enabling the ClearOnMatch is
    not supported.
    This API is not valid for B family.


    @param[in] nTimer  timer of type Timetick_timer_Type
    @param[in] enable  Boolean to enable/disable.
                   
    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval EFI_INVALID_PARAMETER if an invalid parameter is passed for
            Timetick_timer_Type, EFI_SUCCESS otherwise.
    
    @sa None
*/
EFI_STATUS Timetick_ClearOnMatchEnable( Timetick_timer_Type, boolean);



/* ============================================================================
**  Function : Timetick_GetCount
** ============================================================================
*/
/*!
    @brief
    Returns the current timetick count.
 
    @details

    @param[in] nTimer  timer of type Timetick_timer_Type
    @param[out] *pTick   Timetick counter value    


    @par Dependencies
    For this API to work in run time environment,
    Timetick_RunTimeInit should have been called once before this API is called.
    
    @par Side Effects
    None 
 
    @retval EFI_INVALID_PARAMETER if an invalid parameter is passed for
            Timetick_timer_Type, EFI_SUCCESS otherwise.
    
    @sa None
*/

EFI_STATUS Timetick_GetCount(Timetick_timer_Type, Timetick_time_64Type*);


/* ============================================================================
**  Function : Timetick_SetMatchValue
** ============================================================================
*/
/*!
    @brief
    Writes to the match value register of the specified timer.

    @details

    @param[in] nTimer       timer of type Timetick_timer_Type 
    @param[in] matchCount   count to be written to the match value register
    @param[out] *match_val  Matchvalue set

                    
    @par Dependencies
    None
    
    @par Side Effects
    Interrupt when timetick counter reaches given value
    
    @retval EFI_INVALID_PARAMETER if an invalid parameter is passed for
            Timetick_timer_Type, EFI_SUCCESS otherwise.
    
    @sa None
*/
EFI_STATUS Timetick_SetMatchValue( Timetick_timer_Type, 
                                   Timetick_time_64Type, 
                                   Timetick_time_64Type*);



/* ============================================================================
**  Function : Timetick_GetMatchValue
** ============================================================================
*/
/*!
    @brief
    Reads from the match value register of the specified timer.

    @details
   
    @param[in] nTimer       timer of type Timetick_timer_Type 
    @param[out] *match_val  Matchvalue read

                    
    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval EFI_INVALID_PARAMETER if an invalid parameter is passed for
            Timetick_timer_Type, EFI_SUCCESS otherwise.
    
    @sa None
*/
EFI_STATUS Timetick_GetMatchValue( Timetick_timer_Type, Timetick_time_64Type*);



/* ============================================================================
**  Function : Timetick_GetFreq
** ============================================================================
*/
/*!
    @brief
    Returns the frequency at which the timer is running.

    @details
   
    @param[in] nTimer       timer of type Timetick_timer_Type 
    @param[out] *nFreq      frequency

                    
    @par Dependencies
    For this API to work in run time environment,
    Timetick_RunTimeInit should have been called once before this API is called.
    
    @par Side Effects
    None
    
    @retval EFI_INVALID_PARAMETER if an invalid parameter is passed for
            Timetick_timer_Type, EFI_SUCCESS otherwise.
    
    @sa None
*/

EFI_STATUS Timetick_GetFreq( Timetick_timer_Type, UINT32* );



/* ============================================================================
**  Function : Timetick_ClearTimer
** ============================================================================
*/
/*!
    @brief
    Clears the count register of the timer specified.

    @details
    This API is not valid for B family.

    @param[in] nTimer       timer of type Timetick_timer_Type 
                    
    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval EFI_INVALID_PARAMETER if an invalid parameter is passed for
            Timetick_timer_Type, EFI_SUCCESS otherwise.
    
    @sa None
*/
EFI_STATUS Timetick_ClearTimer(Timetick_timer_Type);



/* ============================================================================
**  Function : Timetick_SetTimerPeriod
** ============================================================================
*/
/*!
    @brief
    Sets the timer period by writing to the match value register of the
    specified timer. It does not take MIN_REARM_SCLK into account.

    @details
    This API is not valid for B family.

    @param[in] nTimer       timer of type Timetick_timer_Type 
    @param[in] matchCount   count to be written to the match value register
    @param[out] *match_val  Matchvalue set

                    
    @par Dependencies
    None
    
    @par Side Effects
    Interrupt when timetick counter reaches given value
    
    @retval EFI_INVALID_PARAMETER if an invalid parameter is passed for
            Timetick_timer_Type, EFI_SUCCESS otherwise.
    
    @sa None
*/
EFI_STATUS Timetick_SetTimerPeriod( Timetick_timer_Type, Timetick_time_32Type, Timetick_time_32Type*);


/* ============================================================================
**  Function : Timetick_RunTimeInit
** ============================================================================
*/
/*!
    @brief
    Performs the necessary initialization for run time support.

    @details
    None

    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval EFI_SUCCESS always.     
    @sa None
*/
EFI_STATUS Timetick_RunTimeInit(void);


/* ============================================================================
**  Function : Timetick_UpdateRunTimeAddress
** ============================================================================
*/
/*!
    @brief
    Call back function that gets called when there is a change in the QTimer virtual 
    address.

    @details
    Updates the timetick lib with the new virtual address to be used while reading the 
    QTimer count and frequency.

    @param[in] address       New QTimer base virtual address

    @par Dependencies
    None    
    @par Side Effects
    None
    
    @retval None.     
    @sa None
   
*/
void Timetick_UpdateRunTimeAddress(UINTN);

#endif /* TIMETICK_UEFI_H */

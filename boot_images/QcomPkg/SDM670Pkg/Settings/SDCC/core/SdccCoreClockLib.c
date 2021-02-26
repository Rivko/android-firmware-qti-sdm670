/** @file SdccCoreClockLib.c

  SDCC Core clock driver BSP
  This file implements the SDCC driver BSP for XBL Core clocks

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
2017-06-12   sb      Add eMMC support for SDM670  
2016-05-13   jt      Use busywait instead of MicroSecondDelay 
2016-01-18   jt      Initial version. Branch from 8996 XBL 

=============================================================================*/

#include "SdccBsp.h"
#include "HALcomdef.h"

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIClock.h>
#include <api/systemdrivers/busywait.h>

typedef struct _SDCC_CLOCK_INFO
{
   CHAR8* const clock_name;
   UINTN        clock_id;
}SDCC_CLOCK_INFO;

typedef struct _SDCC_CLOCKS
{
   SDCC_CLOCK_INFO sdcc_apps_clock;
   SDCC_CLOCK_INFO sdcc_ahb_clock;
}SDCC_CLOCKS;

static SDCC_CLOCKS sdcc_clocks_array[] =
{
  {{"gcc_sdcc1_apps_clk", 0 }, {"gcc_sdcc1_ahb_clk", 0 }},
  {{"gcc_sdcc2_apps_clk", 0 }, {"gcc_sdcc2_ahb_clk", 0 }},
  {{NULL, 0 }, {NULL, 0 }},
  {{"gcc_sdcc4_apps_clk", 0 }, {"gcc_sdcc4_ahb_clk", 0 }}
};

static BOOLEAN sdcc_clock_initialized = FALSE;
static EFI_CLOCK_PROTOCOL *hSdcc_Clock = NULL;

/******************************************************************************
                           S D C C    B S P
******************************************************************************/

/******************************************************************************
* Name: sdcc_bsp_get_clock_handle
*
* Description:
*    This function gets the clock protocol handle.
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
static VOID sdcc_bsp_get_clock_handle (VOID)
{
   EFI_STATUS    eResult = EFI_SUCCESS;
   
   if (NULL == hSdcc_Clock)
   {
      eResult = gBS->LocateProtocol(&gEfiClockProtocolGuid,
                                    NULL,
                                    (VOID **)&hSdcc_Clock);
   
      if (EFI_SUCCESS != eResult || NULL == hSdcc_Clock)
      {
          hSdcc_Clock = NULL;      
          ASSERT_EFI_ERROR(eResult);
      }
   }
}

/******************************************************************************
* Name: sdcc_bsp_initialize_clock
*
* Description:
*    This function initializes clocks for all SDCs
*
* Arguments:
*    None
* Returns:
*    sdcc_bsp_err_type: SDCC_BSP_NO_ERROR means success. Error otherwise.
*
******************************************************************************/
static sdcc_bsp_err_type
sdcc_bsp_initialize_clock (VOID)
{
   EFI_STATUS  eResult = EFI_SUCCESS;
   UINT8  driveno = 0;
   UINT32 sdcc_bsp_num_slots = 0;
   
   if (NULL == hSdcc_Clock)
   {
       sdcc_bsp_get_clock_handle();
       if (NULL == hSdcc_Clock)
       {      
          return SDCC_BSP_ERR_UNKNOWN;
       }  
   }
   
   sdcc_bsp_num_slots = sdcc_bsp_get_slot_count();
   
   for (driveno = 0; driveno < sdcc_bsp_num_slots; driveno++) 
   {
      if (sdcc_bsp_get_slot_access_type(driveno) != SDCC_BSP_SLOT_INVALID)
      {
         eResult = hSdcc_Clock->GetClockID(hSdcc_Clock, 
                                           sdcc_clocks_array[driveno].sdcc_apps_clock.clock_name, 
                                           &sdcc_clocks_array[driveno].sdcc_apps_clock.clock_id);
         
         if (EFI_SUCCESS != eResult)
         {
            ASSERT_EFI_ERROR(eResult);
            return SDCC_BSP_ERR_UNKNOWN;
         }
         
         eResult = hSdcc_Clock->GetClockID(hSdcc_Clock, 
                                           sdcc_clocks_array[driveno].sdcc_ahb_clock.clock_name, 
                                           &sdcc_clocks_array[driveno].sdcc_ahb_clock.clock_id);   
                   
         if (EFI_SUCCESS != eResult)
         {
            ASSERT_EFI_ERROR(eResult);
            return SDCC_BSP_ERR_UNKNOWN;
         }
      }
   }
   
   sdcc_clock_initialized = TRUE;
   return SDCC_BSP_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_enable_mclk
*
* Description:
*    This function is to enable the SDCC controller gcc_sdcc_apps clock.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_enable_mclk (UINT32 driveno)
{
   EFI_STATUS    eResult = EFI_SUCCESS;
   UINTN nClockID = 0;

   if (sdcc_clock_initialized == FALSE)
   {
      if (SDCC_BSP_NO_ERROR != sdcc_bsp_initialize_clock())
      {
         return SDCC_BSP_ERR_UNKNOWN;
      }
   }
   
   if (sdcc_bsp_get_slot_access_type(driveno) == SDCC_BSP_SLOT_INVALID)
   {
      return SDCC_BSP_ERR_UNKNOWN;
   }      
   
   nClockID = sdcc_clocks_array[driveno].sdcc_apps_clock.clock_id;
   eResult = hSdcc_Clock->EnableClock(hSdcc_Clock, nClockID);
   if (EFI_SUCCESS != eResult)
   {  
      ASSERT_EFI_ERROR(eResult);      
      return SDCC_BSP_ERR_UNKNOWN;
   }
  
   return SDCC_BSP_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_disable_mclk
*
* Description:
*    This function is to disable the SDCC controller gcc1_sdcc_apps_clk.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_disable_mclk (UINT32 driveno)
{
   EFI_STATUS    eResult = EFI_SUCCESS;
   UINTN nClockID = 0;

   if (sdcc_clock_initialized == FALSE)
   {
      if (SDCC_BSP_NO_ERROR != sdcc_bsp_initialize_clock())
      {
         return SDCC_BSP_ERR_UNKNOWN;
      }
   }
   
   if (sdcc_bsp_get_slot_access_type(driveno) == SDCC_BSP_SLOT_INVALID)
   {
      return SDCC_BSP_ERR_UNKNOWN;
   }

   nClockID = sdcc_clocks_array[driveno].sdcc_apps_clock.clock_id;
   eResult = hSdcc_Clock->DisableClock(hSdcc_Clock, nClockID);
   if (EFI_SUCCESS != eResult)
   {  
      ASSERT_EFI_ERROR(eResult);      
      return SDCC_BSP_ERR_UNKNOWN;
   }
 
   return SDCC_BSP_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_set_mclk_frequency
*
* Description:
*    This function is to select the SDCC mclock rate.
*
* Arguments:
*    driveno          [IN] : drive number
*    sdcc_mclk        [IN] : clock speed
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_set_mclk_frequency (UINT32 driveno, sdcc_bsp_clk_type sdcc_mclk)
{
   EFI_STATUS    eResult = EFI_SUCCESS;
   UINTN         nClockID = 0;
   UINT32        nFreqHz = 0;

   if (sdcc_clock_initialized == FALSE)
   {
      if (SDCC_BSP_NO_ERROR != sdcc_bsp_initialize_clock())
      {
         return SDCC_BSP_ERR_UNKNOWN;
      }
   }
   
   if (sdcc_bsp_get_slot_access_type(driveno) == SDCC_BSP_SLOT_INVALID)
   {
      return SDCC_BSP_ERR_UNKNOWN;
   } 
   
   nClockID = sdcc_clocks_array[driveno].sdcc_apps_clock.clock_id;
   eResult = hSdcc_Clock->SetClockFreqHz(hSdcc_Clock, nClockID, (UINT32)sdcc_mclk,
                                        EFI_CLOCK_FREQUENCY_HZ_AT_MOST, &nFreqHz);
   if (EFI_SUCCESS != eResult)
   {  
      ASSERT_EFI_ERROR(eResult);      
      return SDCC_BSP_ERR_UNKNOWN;
   }

   return SDCC_BSP_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_reset_mclk
*
* Description:
*    This function resets the mclk
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*     sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_reset_mclk( UINT32 driveno )
{
   EFI_STATUS    eResult = EFI_SUCCESS;
   UINTN nClockID = 0;

   if (sdcc_clock_initialized == FALSE)
   {
      if (SDCC_BSP_NO_ERROR != sdcc_bsp_initialize_clock())
      {
         return SDCC_BSP_ERR_UNKNOWN;
      }
   }
   
   if (sdcc_bsp_get_slot_access_type(driveno) == SDCC_BSP_SLOT_INVALID)
   {
      return SDCC_BSP_ERR_UNKNOWN;
   } 
   
   nClockID = sdcc_clocks_array[driveno].sdcc_apps_clock.clock_id;
   eResult = hSdcc_Clock->DisableClock(hSdcc_Clock, nClockID);
   if (EFI_SUCCESS != eResult)
   {  
      ASSERT_EFI_ERROR(eResult);      
      return SDCC_BSP_ERR_UNKNOWN;
   }
   
   busywait(100);

   eResult = hSdcc_Clock->EnableClock(hSdcc_Clock, nClockID);
   if (EFI_SUCCESS != eResult)
   {  
      ASSERT_EFI_ERROR(eResult);      
      return SDCC_BSP_ERR_UNKNOWN;
   }

   return SDCC_BSP_NO_ERROR;
} /* sdcc_reset */

/******************************************************************************
* Name: sdcc_ahb_clk_init
*
* Description:
*    This function is to initialize the SDCC AHB clock.
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_ahb_clk_init ( UINT32 driveno )
{
   EFI_STATUS    eResult = EFI_SUCCESS;
   UINTN nClockID = 0;
   
   if (sdcc_clock_initialized == FALSE)
   {
      if (SDCC_BSP_NO_ERROR != sdcc_bsp_initialize_clock())
      {
         return SDCC_BSP_ERR_UNKNOWN;
      }
   }
   
   if (sdcc_bsp_get_slot_access_type(driveno) == SDCC_BSP_SLOT_INVALID)
   {
      return SDCC_BSP_ERR_UNKNOWN;
   } 
   
   nClockID = sdcc_clocks_array[driveno].sdcc_ahb_clock.clock_id;
   eResult = hSdcc_Clock->EnableClock(hSdcc_Clock, nClockID);
   if (EFI_SUCCESS != eResult)
   {  
      ASSERT_EFI_ERROR(eResult);      
      return SDCC_BSP_ERR_UNKNOWN;
   }

   return SDCC_BSP_NO_ERROR;
}

/******************************************************************************
* Name: sdcc_ahb_clk_disable
*
* Description:
*    This function is to disable the SDCC AHB clock
*
* Arguments:
*    driveno          [IN] : drive number
*
* Returns:
*    sdcc_bsp_err_type
*
******************************************************************************/
sdcc_bsp_err_type
sdcc_ahb_clk_disable ( UINT32 driveno )
{
   EFI_STATUS    eResult = EFI_SUCCESS;
   UINTN nClockID = 0;

   if (sdcc_clock_initialized == FALSE)
   {
      if (SDCC_BSP_NO_ERROR != sdcc_bsp_initialize_clock())
      {
         return SDCC_BSP_ERR_UNKNOWN;
      }
   }
   
   if (sdcc_bsp_get_slot_access_type(driveno) == SDCC_BSP_SLOT_INVALID)
   {
      return SDCC_BSP_ERR_UNKNOWN;
   } 
   
   nClockID = sdcc_clocks_array[driveno].sdcc_ahb_clock.clock_id;
   eResult = hSdcc_Clock->DisableClock(hSdcc_Clock, nClockID);
   if (EFI_SUCCESS != eResult)
   {  
      ASSERT_EFI_ERROR(eResult);      
      return SDCC_BSP_ERR_UNKNOWN;
   }
  
   return SDCC_BSP_NO_ERROR;
}


/*==============================================================================
  FILE:         cti_irq_island.c

  OVERVIEW:     This file has general CTI debug interrupt functionality particular to island, 
                if applicable on this processor. Shared between MPSS/ADSP/SLPI/CDSP.

  DEPENDENCIES: None

                Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1/products/cti/src/cti_irq_island.c#2 $
$DateTime: 2017/10/17 05:00:15 $
==============================================================================*/
#include <stdint.h>
#include "timer.h"
#include "CoreVerify.h"
#include "com_dtypes.h"
#include "DDIInterruptController.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "DALSys.h"
#include "err.h"
#include "CoreVerify.h"
#include "msg.h"
#include "rcinit.h"
#include "npa.h"
#include "cti.h"
#include "ClockDefs.h"
#include "DDIClock.h"
//#include "ChipInfoLocal.h"
/* ==================================================================
                     Forward Declarations
   ================================================================== */
void CTI_Wakeup_Handler(uint32 param);  

DALResult cti_workloop(DALSYSEventHandle hEvent, void *pCtx);

unsigned int cti_read_imem(void);

void islandcti_enableDebug(boolean enable);

DalChipInfoFamilyType   cpuFam;
DalDeviceHandle         *hChipInfo;  
npa_client_handle       npa_handle;
DalDeviceHandle         *hDALIntCtl;

uint32                  IMEM_Base;
uint32                  CTI_imem_1; 
uint32                  CTI_imem_2; 
uint32                  CTI_imem_3; 
uint32                  Cookie_Location;

//Island Variables
cti_target_config       cti_cfg;                
boolean                 QDSS_ClockVote_Feature_Enabled;   
boolean                 NPA_QDSS_Increase_Request_Issued; 
boolean                 NPA_QDSS_Decrease_Request_Issued; 

uint32                  CTI_Interrupt_Number;

/* ==================================================================
                     FUNCTIONS
   ================================================================== */


//////////////////////////////
///
/// CTI_Wakeup_Handler
///
/// @brief Handles interrupt from CTI. Checks Shared IMEM location
/// for action (Expects shared IMEM to be populated by JTAG with 
/// opcode indicating desired functionaliy - busywait, crash etc.)
///
/// @author JBILLING
///
//////////////////////////////

void CTI_Wakeup_Handler(uint32 param)
{
    
    //Always vote for island clock if we get CTI interrupt.
    if (ENABLE_ISLAND_CLOCKS_FOR_JTAG){
        islandcti_enableDebug(TRUE);
    }
    
    //Populate cti struct with IMEM contents
    //If in bus isolation, populate with defaults.    
    if (DAL_SUCCESS != cti_read_imem() )
        ERR_FATAL("CTI interrupt handler - could not read from Shared-IMEM!", 0, 0, 0);

    if ((PROCESSOR_SELECT_MASK & cti_cfg.processor_select)||(PROCESSOR_ALL_FLAG & cti_cfg.all_processor_flag))
    {
        switch(cti_cfg.cookie_action){
            case COOKIE_BUSYWAIT:
                //Request higher clock speed indefinitely - assume that we're using JTAG
                if ((QDSS_ClockVote_Feature_Enabled == TRUE)&&(qurt_island_get_status())){
                    if ((npa_handle != NULL )&&(NPA_QDSS_Increase_Request_Issued==FALSE)){
                        npa_issue_scalar_request(npa_handle, 2);
                        NPA_QDSS_Increase_Request_Issued=TRUE;
                        NPA_QDSS_Decrease_Request_Issued=FALSE;
                    }
                }
                if (DAL_SUCCESS != DALSYS_EventCtrl(cti_cfg.event, DALSYS_EVENT_CTRL_TRIGGER))
                    ERR_FATAL("Couldn't Trigger CTI Workloop EVENT", 0, 0, 0);
                break;
            case COOKIE_BUSYWAIT_DEVOTE_QDSS_CLOCK:
                if ((QDSS_ClockVote_Feature_Enabled == TRUE)&&(qurt_island_get_status())){
                    if ((npa_handle != NULL )&&(NPA_QDSS_Decrease_Request_Issued==FALSE)){
                        npa_issue_scalar_request(npa_handle, 0);
                        NPA_QDSS_Decrease_Request_Issued=TRUE;
                        NPA_QDSS_Increase_Request_Issued=FALSE;
                    }
                }
                if (DAL_SUCCESS != DALSYS_EventCtrl(cti_cfg.event, DALSYS_EVENT_CTRL_TRIGGER))
                    ERR_FATAL("Couldn't Trigger CTI Workloop EVENT", 0, 0, 0);
                break;
                break;
            case COOKIE_BKRPT:
                //Request higher clock speed indefinitely - assume that we're using JTAG
                if (npa_handle != NULL ){
                    npa_issue_scalar_request(npa_handle, 2);
                }

                MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "CTI - Cookie BKRPT.", 0, 0, 0);
                __asm__ __volatile__ ("brkpt \n");
                break;
            case COOKIE_CRASH:
                MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "CTI - Cookie Crash.", 0, 0, 0);
                ERR_FATAL("CTI - Triggered Crash", 0, 0, 0);
                break;
            case COOKIE_TRACE:
                MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "CTI - Cookie Trace.", 0, 0, 0);
                break;
            case COOKIE_ETM_COMPARATOR:
                MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "CTI - Cookie ETM Comparator.", 0, 0, 0);
                // ETM COMPARE THING
                break;
            case COOKIE_DISABLE_SLEEP:
                MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "CTI - Cookie Disable Sleep.", 0, 0, 0);
                // Sleep Enable+Disable DONT FORGET.
                break;
            case COOKIE_READ_REG:
                cti_read_registers();
                MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "CTI - Read Registers. R1= 0x%x, R2= 0x%x, R3= 0x%x", cti_cfg.reg1, cti_cfg.reg2, cti_cfg.reg3);
                // Sleep Enable+Disable DONT FORGET.
                break;
            default:
                MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "CTI - Invalid Cookie", 0, 0, 0);
                break;
            }
    }
    return;
}

#ifdef CTI_INTERRUPT_PORT_2
//////////////////////////////////////////////
///
/// @brief Simple wrapper used for second CTI interrupt (two interrupts cannot have the same callback). 
/// Simply calls CTI_Wakeup_Handler
///
////////////////////////////////////////////////
void CTI_Wakeup_Handler_wrapper(uint32 param){
    CTI_Wakeup_Handler(param);
}
#endif

//////////////////////////////
///
/// cti_workloop
///
/// @brief Registers workloop with DAL, so that function can be called later via interrupt.
///
/// @author JBILLING
///
//////////////////////////////
DALResult cti_workloop(DALSYSEventHandle hEvent, void *pCtx)
{
    DALSYS_EventCtrl(cti_cfg.event, DALSYS_EVENT_CTRL_RESET);

    while ( 1 )
    {
        DALSYS_EventWait(cti_cfg.event);
        DALSYS_EventCtrl(cti_cfg.event, DALSYS_EVENT_CTRL_RESET);

        if (DAL_SUCCESS != cti_read_imem() ){
            ERR_FATAL("CTI interrupt handler - could not read from Shared-IMEM!", 0, 0, 0);
        }
        
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "CTI - Cookie BusyWait.", 0, 0, 0);
        DALSYS_BusyWait(CTI_BUSYWAIT_DELAY);
        
    }
}

//////////////////////////////
///
/// cti_read_imem
///
/// @brief Retrieves and parses opcode from shared IMEM.
///
/// Expects JTAG will have written this value first, 
/// then triggered CTI interrupt.
/// Parses value into processor value (most significant byte) and 
/// opcode value (least significant  3 bytes).
/// 
/// @author JBILLING
///
//////////////////////////////
unsigned int cti_read_imem(void)
{
    if (qurt_island_get_status()){
        
        cti_cfg.all_processor_flag = PROCESSOR_ALL_FLAG;
        cti_cfg.processor_select= 0;
        cti_cfg.cookie_action = COOKIE_SELECT_ISLAND_MODE;  
        
    }
    else{        
        
        volatile unsigned int* debug_cookie = (volatile unsigned int*)(Cookie_Location);
        
        cti_cfg.cookie = *debug_cookie;
        cti_cfg.all_processor_flag = (cti_cfg.cookie & PROCESSOR_ALL_FLAG_MASK)>>PROCESSOR_ALL_FLAG_SHIFT;
        cti_cfg.processor_select= (cti_cfg.cookie & PROCESSOR_VALUE_SELECT_MASK)>>PROCESSOR_VALUE_SELECT_SHIFT;
        cti_cfg.cookie_action = cti_cfg.cookie & COOKIE_ACTION_MASK;
        
    }
    
    //JTAG doesn't know if system is in island mode. In case it couldn't access Shared IMEM, default to BusyWait.
    //However, if it did program Shared IMEM, then go with those values.
#ifdef ISLAND_PROCESSOR
    //if (cti_cfg.cookie_action == 0x0){
    if ((cti_cfg.cookie_action & GOOD_COOKIE_OPCODE_MASK) != GOOD_COOKIE_OPCODE_MASK){
        
        cti_cfg.cookie_action = COOKIE_SELECT_ISLAND_MODE; //Defaults to busywait if not action given.
        cti_cfg.all_processor_flag = PROCESSOR_ALL_FLAG;
        cti_cfg.processor_select= 0;
    }

#endif //ISLAND_PROCESSOR
    return DAL_SUCCESS;
}

void cti_read_registers(void)
{
    
   cti_cfg.reg1 =(*  ((volatile unsigned int*)(CTI_imem_1)));
   cti_cfg.reg2 =(*  ((volatile unsigned int*)(CTI_imem_2)));
   cti_cfg.reg3 =(*  ((volatile unsigned int*)(CTI_imem_3)));
   
   return;
}
///////////////////////////////////
///
/// islandcti_enableDebug
/// @brief For island mode - enables specific clocks needed by SSC island 
/// which allow JTAG to access Q6.
/// @author JBILLING
///
////////////////////////////////////
void islandcti_enableDebug(boolean enable)
{
  const char  *pClkName1 = "scc_at_clk";
  const char  *pClkName2 = "scc_pclkdbg_clk";
  const char  *pClkName3 = "scc_sdc_dap_clk";
  DALResult   eDalResult = DAL_SUCCESS;

  static ClockIdType      nClkId1 = 0;
  static ClockIdType      nClkId2 = 0;
  static ClockIdType      nClkId3 = 0;
  static DalDeviceHandle  *clockHandle = NULL;

  if(NULL == clockHandle)
  {
    eDalResult = DAL_DeviceAttach(DALDEVICEID_CLOCK, &clockHandle);
    if (NULL == clockHandle){
        ERR_FATAL("Error - clock handle returned was NULL", 0, 0, 0);
    }
    eDalResult = DalClock_GetClockId(clockHandle, pClkName1, &nClkId1);
    eDalResult = DalClock_GetClockId(clockHandle, pClkName2, &nClkId2);
    eDalResult = DalClock_GetClockId(clockHandle, pClkName3, &nClkId3);
  }

  if(TRUE == enable)
  {
    eDalResult = DalClock_EnableClock(clockHandle, nClkId1);
    eDalResult = DalClock_EnableClock(clockHandle, nClkId1);
    eDalResult = DalClock_EnableClock(clockHandle, nClkId3);
  }
  else
  {
    eDalResult = DalClock_DisableClock(clockHandle, nClkId1);
    eDalResult = DalClock_DisableClock(clockHandle, nClkId1);
    eDalResult = DalClock_DisableClock(clockHandle, nClkId3);
  }

  return;
}
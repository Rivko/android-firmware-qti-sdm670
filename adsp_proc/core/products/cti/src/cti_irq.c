/*==============================================================================
  FILE:         cti_irq.c

  OVERVIEW:     This file provides CTI debug interrupt functionality.
                Shared between MPSS/ADSP/SLPI/CDSP.

  DEPENDENCIES: None

                Copyright (c) 2011-2017 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1/products/cti/src/cti_irq.c#1 $
$DateTime: 2017/07/21 22:10:47 $
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
/// cti_irq_init
///
/// @brief Initializes interrupts, DAL workloop, shared IMEM address.
/// If applicable, votes and devotes debug clocks.
///
/// @author JBILLING
///
//////////////////////////////
void cti_irq_init(void)
{

    NPA_QDSS_Increase_Request_Issued=FALSE;
    NPA_QDSS_Decrease_Request_Issued=FALSE;
    QDSS_ClockVote_Feature_Enabled=0;
    
    CTI_Interrupt_Number = CTI_INTERRUPT_PORT;
    
    /* Attaching to interrupt controller and registering interrupt handlers */
    CORE_VERIFY( DAL_DeviceAttach( DALDEVICEID_INTERRUPTCONTROLLER,
                                 &hDALIntCtl ) == DAL_SUCCESS );

    CORE_VERIFY( DalInterruptController_RegisterISR( 
                hDALIntCtl,
                (uint32)CTI_Interrupt_Number,
                (DALIRQ)CTI_Wakeup_Handler,
                (const DALIRQCtx) 0,
                (DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER)
              ) == DAL_SUCCESS );
              
    //Register second interrupt if defined (For SSC Island only)
    
#ifdef CTI_INTERRUPT_PORT_2
        
        CTI_Interrupt_Number = CTI_INTERRUPT_PORT_2;
        
        CORE_VERIFY( DalInterruptController_RegisterISR( 
                hDALIntCtl,
                (uint32)CTI_Interrupt_Number,
                (DALIRQ)CTI_Wakeup_Handler_wrapper,
                (const DALIRQCtx) 0,
                (DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER)
              ) == DAL_SUCCESS );
    
#endif
    
    /* Get global chip architecture  */
    if( DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipInfo) == DAL_SUCCESS )
    {
      DalDevice_Open(hChipInfo, DAL_OPEN_SHARED);
      DalChipInfo_GetChipFamily(hChipInfo, &cpuFam);
      DalDevice_Close(hChipInfo);
    }
    else ERR_FATAL("CTI - error on DAL_DeviceAttach - ChipInfo", 0, 0, 0);
    
    
    if      (cpuFam == DALCHIPINFO_FAMILY_MSM8994)   IMEM_Base = CTI_IMEM_BASE_8994;
    else if (cpuFam == DALCHIPINFO_FAMILY_MSM8992)   IMEM_Base = CTI_IMEM_BASE_8992;
    else if (cpuFam == DALCHIPINFO_FAMILY_MSM8996)   IMEM_Base = CTI_IMEM_BASE_8996;
    else if (cpuFam == DALCHIPINFO_FAMILY_MSM8996SG) IMEM_Base = CTI_IMEM_BASE_8996;
    else if (cpuFam == DALCHIPINFO_FAMILY_MSM8998)   IMEM_Base = CTI_IMEM_BASE_8998;
    else if (cpuFam == DALCHIPINFO_FAMILY_MSM8997)   IMEM_Base = CTI_IMEM_BASE_8998;
    else if (cpuFam == DALCHIPINFO_FAMILY_MDM9x55)   IMEM_Base = CTI_IMEM_BASE_9X55;
    else if (cpuFam == DALCHIPINFO_FAMILY_SDM670)    IMEM_Base = CTI_IMEM_BASE_SDM670;
    else if (cpuFam == DALCHIPINFO_FAMILY_MDM9x65){
        IMEM_Base = CTI_IMEM_BASE_9X65;
        QDSS_ClockVote_Feature_Enabled=1;
    }
    else if (cpuFam == DALCHIPINFO_FAMILY_SDM845){
        IMEM_Base = CTI_IMEM_BASE_SDM845;
        QDSS_ClockVote_Feature_Enabled=SDM845_CLOCKVOTE_ENABLE_FLAG;
    }
    else IMEM_Base = IMEM_BASE_DEFAULT;
    //else {
    //    ERR_FATAL("CTI init - chip family not recognized", 0, 0, 0);
    //}
    
    CTI_imem_1        = IMEM_Base+CTI_IMEM_OFFSET_1;
    CTI_imem_2        = IMEM_Base+CTI_IMEM_OFFSET_2;
    CTI_imem_3        = IMEM_Base+CTI_IMEM_OFFSET_3;
    Cookie_Location   = CTI_imem_3;

    if (QDSS_ClockVote_Feature_Enabled==TRUE){
        npa_handle = npa_create_sync_client("/clk/qdss", "products", NPA_CLIENT_REQUIRED);
        npa_issue_scalar_request(npa_handle, 2);	
        //votes qdss clock back to 0 after 1 second
        ProductsTimerFunc();
    }
    
    cti_init_workloop();
}

//////////////////////////////////////
///
/// ProductsTimerFunc
///
/// @brief Votes debug clocks on to assist with faster debugger response.
///
/// Due to QuRT VTLB feature, debugging is slowed due to 
/// large data traffic in transferring pagetables to debugger.
/// Solution is to vote required clocks on for small period of time
/// in case user is debugging, then vote them off a few seconds later.
/// If hotattach feature is used, clocks are voted on permanently 
/// (Assumes debug mode).
///
///  @author JBILLING
///
/////////////////////////////////////
void ProductsTimerFunc(){

    uint64 ui64TimerInitValue  = PRODUCTS_QDSS_CLOCK_VOTE_TIMER_VALUE;
    
    static timer_type sgtTimer1;
    static timer_group_type sgtTimerGroup;
    timer_error_type timer_err;   
    DALResult dalResult = DAL_SUCCESS;
    
    uint32 sigs;

    qurt_anysignal_t products_timer_obj;

    DalDeviceHandle *hTimetick = NULL;
    uint64           ui64TickFreqHz = 0;
    //uint64           ui64LocalStartTimeTick = 0ULL;
    //uint64           ui64LocalEndTimeTick = 0ULL;

    // Attach to timetick handle
    dalResult = DalTimetick_Attach("SystemTimer", &hTimetick);
    if (DAL_SUCCESS != dalResult || NULL == hTimetick)
    {
        //Vote clock off and exit. Fail silently
        npa_issue_scalar_request(npa_handle, 0);
        return;
    }
    else
    {
        dalResult = DalTimetick_GetFreq(hTimetick, (uint32*)&ui64TickFreqHz);
    }
    
    if (DAL_SUCCESS != dalResult)
    {
        //Vote clock off and exit. Fail silently
        npa_issue_scalar_request(npa_handle, 0);
        return;
    }
    
    qurt_anysignal_init (&products_timer_obj);
    qurt_anysignal_clear(&products_timer_obj,PRODUCTS_QDSS_CLOCK_TIMER_SIGNAL);
            
    timer_def_osal( &sgtTimer1, 
                    &sgtTimerGroup,
                    TIMER_NATIVE_OS_SIGNAL_TYPE, 
                    &products_timer_obj, 
                    PRODUCTS_QDSS_CLOCK_TIMER_SIGNAL);
    
    /*Set timer as non deferable*/
    timer_err = timer_group_set_deferrable(&sgtTimerGroup,TIMER_BOOL_NON_DEFERABLE);
    if (timer_err != TE_SUCCESS){
        npa_issue_scalar_request(npa_handle, 0);
        return;
    }
    /* Start the timer */
    timer_set_64(&sgtTimer1,
                 ui64TimerInitValue, 
                 0, 
                 T_MSEC);
                
    /* Wait for the signal which is the indication that the timer expires */
    sigs = qurt_anysignal_wait( &products_timer_obj, PRODUCTS_QDSS_CLOCK_TIMER_SIGNAL);                
    
    
    //now remove vote for clock
    npa_issue_scalar_request(npa_handle, 0);    
    
    
    qurt_anysignal_clear( &products_timer_obj, PRODUCTS_QDSS_CLOCK_TIMER_SIGNAL);                

    /* Undefine the timer */
    timer_undef(&sgtTimer1);                
    
    return;
}

//////////////////////////////
///
/// cti_init_workloop
///
/// @brief Initializes DAL workloop.
///
/// @author JBILLING
///
//////////////////////////////
void cti_init_workloop(void)
{
#ifdef MSS_CTI
    RCINIT_INFO info_handle = NULL;
    RCINIT_PRIO prio = 0;
    unsigned long stksz = 0;
#else

    unsigned long prio = CTI_WORKLOOP_PRIORITY;	
	unsigned long stksz = CTI_WORKLOOP_STACK_SIZE;
#endif
    DALSYS_InitMod(NULL);


    if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
                                          &(cti_cfg.event),
                                          NULL))
        ERR_FATAL("Couldn't Create CTI workloop EVENT", 0, 0, 0);

    if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                          &(cti_cfg.start_event),
                                          NULL))
        ERR_FATAL("Couldn't Create CTI workloop Start EVENT", 0, 0, 0);
#ifdef MSS_CTI
    info_handle = rcinit_lookup("cti_workloop");
        
    if (!info_handle) 
    {
        ERR_FATAL("cti_workloop task info not found",0 , 0, 0);
    }
    else
    {
        prio = rcinit_lookup_prio_info(info_handle);
        stksz = rcinit_lookup_stksz_info(info_handle);
        if ((prio > 255) || (stksz == 0)) 
        {
           ERR_FATAL("Invalid Priority:%d or Stack Size: %d",prio, stksz, 0);
        }
    }
#endif
    if (DAL_SUCCESS != DALSYS_RegisterWorkLoopEx("cti_workloop",
                                                 stksz,
                                                 prio,
                                                 CTI_WORKLOOP_MAX_EVENTS,
                                                 &(cti_cfg.handle), NULL))
        ERR_FATAL("Couldn't Register CTI workloop", 0, 0, 0);

    if (DAL_SUCCESS != DALSYS_AddEventToWorkLoop(cti_cfg.handle,
                                                 cti_workloop,
                                                 NULL,
                                                 cti_cfg.start_event,
                                                 NULL))
        ERR_FATAL("Couldn't Add CTI workloop Start EVENT", 0, 0, 0);

        
    if (DAL_SUCCESS != DALSYS_EventCtrl(cti_cfg.start_event,
                                        DALSYS_EVENT_CTRL_TRIGGER))
        ERR_FATAL("Couldn't Trigger CTI workloop Start EVENT", 0, 0, 0);

    
}

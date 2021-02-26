/*==============================================================================

FILE:      NOC_error.c

DESCRIPTION: This file implements NOC Error Handler.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Edit History

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/icb/src/common/NOC_error.c#2 $
$DateTime: 2017/12/06 09:58:14 $
$Author: pwbldsvc $
$Change: 14996571 $

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2017/06/01  sds    Add deferred NoC error programming support
2017/03/01  sds    Add timeout enable register support
2017/02/06  sds    Rewrite for QNoC 4.x
2016/05/09  sds    Use tzbsp_set_err_fatal()
2015/11/23  pm     Added Sideband Manager support
2015/09/14  ddk    Port to 8937/8952.
2015/05/18  tb     Switched lock to the shared AC lock and XPU/SMMU API calls added
2015/05/11  tb     New handling of NoC errors in one pass added
2014/12/09  tb     Split OEM modifiable data from internal data
2014/10/20  tb     Added new NoC revID for FlexNoC version 2.11.1
2014/09/03  tb     Updated for DAL dev config
2013/11/07  pm     Added HW version for Bear family FlexNoC version 2.8.6-1
2013/11/07  pm     Fixed missing break when switching on HW version
2013/08/28  pm     Removed Clock_EnableBusAccess() call in interrupt context
2013/05/17  pm     Moved NOCInfo initialization before use in NOC_REG_ADDR() 
2013/05/16  pm     Enabled clocks at right time in NOC_Error_Init() 
2013/05/08  pm     Added FlexNoC version 2.8 
2012/10/03  av     Created

        Copyright (c) 2012 - 2017 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icb_error.h"
#include "NOC_error.h"
#include "NOC_error_platform.h"
#include "err.h"
#include "DALDeviceId.h"
#include "DDIClock.h"

/*============================================================================
                          DEFINEs
============================================================================*/

/*============================================================================
                          MACROs
============================================================================*/
#define NOC_OUT8(addr, data)   (*((volatile uint8_t *) (addr)) = ((uint8_t) (data)))
#define NOC_OUT16(addr, data)  (*((volatile uint16_t *) (addr)) = ((uint16_t) (data)))
#define NOC_OUT32(addr, data)  (*((volatile uint32_t *) (addr)) = ((uint32_t) (data)))

#define NOC_IN8(addr)   ((uint8_t) (*((volatile uint8_t *) (addr))))
#define NOC_IN16(addr)  ((uin16) (*((volatile uint16_t *) (addr))))
#define NOC_IN32(addr)  ((uint32_t)(*((volatile uint32_t *) (addr))))

#define NOC_REG_ADDR(base,offset)   (((uint8_t*)(base)) + (offset))

#define REGISTER_VALID(offs) ((offs)!=REGISTER_NOT_APPLICABLE)

/*============================================================================
                          STRUCTURE DECLARATIONS
============================================================================*/

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/
static NOCERR_propdata_type* NOCERR_propdata;
static NOCERR_propdata_type_oem* NOCERR_propdata_oem;
static NOCERR_info_type* NOCInfo;
static NOCERR_info_type_oem* NOCInfoOEM;

/*============================================================================
                                 FUNCTION PROTOTYPES
============================================================================*/
void NOC_Error_PreInit(DalDeviceHandle *, NOCERR_info_type *    NOCInfo);
void NOC_Error_PostInit(DalDeviceHandle *, NOCERR_info_type *    NOCInfo);

/*============================================================================
                             INTERNAL FUNCTIONS
============================================================================*/
static void NOC_Error_InitNoC
(
  NOCERR_info_type *    NOCInfo,
  NOCERR_info_type_oem* NOCInfoOEM
)
{
   uint32_t idx;
   /* Write error clear, if necessary. */
   if( NOCInfoOEM->intr_clear )
   {
      NOC_OUT32(NOC_REG_ADDR(NOCInfo->base_addr, NOCInfo->hw->err_clear_low), 0x1);
   }

   /* Write sideband configuration */
   for( idx = 0; idx < NOCInfo->num_sbms; idx++ )
   {
      if( REGISTER_VALID(NOCInfo->sb_hw[idx]->faultin_en0_low) )
      {
         NOC_OUT32(NOC_REG_ADDR(NOCInfo->sb_base_addrs[idx],
                                NOCInfo->sb_hw[idx]->faultin_en0_low),
                   NOCInfoOEM->sbms[idx].faultin_en0_low);
      }
      if( REGISTER_VALID(NOCInfo->sb_hw[idx]->faultin_en0_high) )
      {
         NOC_OUT32(NOC_REG_ADDR(NOCInfo->sb_base_addrs[idx],
                                NOCInfo->sb_hw[idx]->faultin_en0_high),
                   NOCInfoOEM->sbms[idx].faultin_en0_high);
      }
   }

   /* Write timeout enable configuration */
   for( idx = 0; idx < NOCInfo->num_tos; idx++ )
   {
      NOC_OUT32(NOCInfo->to_addrs[idx], NOCInfoOEM->to_reg_vals[idx]);
   }

   /* Set/clear enable bit. */
   if( NOCInfoOEM->intr_enable )
   {
      NOC_OUT32(NOC_REG_ADDR(NOCInfo->base_addr,NOCInfo->hw->main_ctl_low), 0x1);
   }
   else
   {
      NOC_OUT32(NOC_REG_ADDR(NOCInfo->base_addr,NOCInfo->hw->main_ctl_low), 0x0);
   }
}

/*============================================================================
                                 FUNCTIONS
============================================================================*/

/*===========================================================================*/
/**
@brief 
      Handler function for NOC error interrupt.
 
@param[in]  void* arg Context data for client.

@return    
      void* arg.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==============================================================================*/
void* NOC_Error_Handle_Interrupt(void* arg)
{
   uint32_t noc_idx, idx, val;
   uint32_t intr_vector = (uint32_t) arg;
   NOCERR_info_type *noc_info = NULL;
   NOCERR_info_type_oem* noc_info_oem = NULL;
   bool fault_detected = false;
   DalDeviceHandle *hClock = NULL;

   /* Attach to clock driver for any power domain or clock domain ids */
   if(DAL_SUCCESS != DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &hClock))
   {
      ERR_FATAL("NOC_ERROR: Device attach to Clock failed.", 0, 0, 0);
   }
   
  
   // Find which NOC generated interrupt
   for(noc_idx = 0;
       noc_idx < NOCERR_propdata->len;
       noc_idx++)
   {
      if( NOCInfo[noc_idx].intr_vector == intr_vector )
      {
         NOC_Error_PreInit(hClock, &NOCInfo[noc_idx]);
         noc_info = &NOCInfo[noc_idx];
         noc_info_oem = &NOCInfoOEM[noc_idx];
         break;
      }
   }
   if(NULL == noc_info || NULL == noc_info_oem)
   {
     ERR("Invalid Interrupt Vector!", 0, 0, 0);
     return arg;
   }

   // If there is an OBS fault, then log the OBS block syndrome registers
   if (NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->err_valid_low)))
   {
      // Log OBS Block Syndrome Registers
      fault_detected = true;
      val = NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->errlog0_low));
      if( 0 != val )
      {
         noc_info->syndrome.ERRLOG0_LOW = val;
         ERR("%s ERROR: ERRLOG0_LOW = 0x%08x", noc_info->name, val, 0);
      }

      val = NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->errlog0_high));
      if( 0 != val )
      {
         noc_info->syndrome.ERRLOG0_HIGH = val;
         ERR("%s ERROR: ERRLOG0_HIGH = 0x%08x", noc_info->name, val, 0);
      }

      val = NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->errlog1_low));
      if( 0 != val )
      {
         noc_info->syndrome.ERRLOG1_LOW = val;
         ERR("%s ERROR: ERRLOG1_LOW = 0x%08x", noc_info->name, val, 0);
      }

      val = NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->errlog1_high));
      if( 0 != val )
      {
         noc_info->syndrome.ERRLOG1_HIGH = val;
         ERR("%s ERROR: ERRLOG1_HIGH = 0x%08x", noc_info->name, val, 0);
      }

      val = NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->errlog2_low));
      if( 0 != val )
      {
         noc_info->syndrome.ERRLOG2_LOW = val;
         ERR("%s ERROR: ERRLOG2_LOW = 0x%08x", noc_info->name, val, 0);
      }

      val = NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->errlog2_high));
      if( 0 != val )
      {
         noc_info->syndrome.ERRLOG2_HIGH = val;
         ERR("%s ERROR: ERRLOG2_HIGH = 0x%08x", noc_info->name, val, 0);
      }

      val = NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->errlog3_low));
      if( 0 != val )
      {
         noc_info->syndrome.ERRLOG3_LOW = val;
         ERR("%s ERROR: ERRLOG3_LOW = 0x%08x", noc_info->name, val, 0);
      }

      val = NOC_IN32(NOC_REG_ADDR(noc_info->base_addr,noc_info->hw->errlog3_high));
      if( 0 != val )
      {
         noc_info->syndrome.ERRLOG3_HIGH = val;
         ERR("%s ERROR: ERRLOG3_HIGH = 0x%08x", noc_info->name, val, 0);
      }
   }

   // If Sideband Manager exists and there is a sideband fault, 
   // log the Sideband Manager syndrome info
   for( idx = 0; idx < noc_info->num_sbms; idx++ )
   {
      if( REGISTER_VALID(noc_info->sb_hw[idx]->faultin_status0_low) )
      {
         val = NOC_IN32(NOC_REG_ADDR(noc_info->sb_base_addrs[idx],
                                     noc_info->sb_hw[idx]->faultin_status0_low));
         if( 0 != val )
         {
            noc_info->syndrome.sbms[idx].FAULTINSTATUS0_LOW = val;
            ERR("%s ERROR: SBM%d FAULTINSTATUS0_LOW = 0x%08x",
                noc_info->name, idx, val);
            fault_detected = true;
         }
      }
      if( REGISTER_VALID(noc_info->sb_hw[idx]->faultin_status0_high) )
      {
         val = NOC_IN32(NOC_REG_ADDR(noc_info->sb_base_addrs[idx],
                                     noc_info->sb_hw[idx]->faultin_status0_high));
         if( 0 != val )
         {
            noc_info->syndrome.sbms[idx].FAULTINSTATUS0_HIGH = val;
            ERR("%s ERROR: SBM%d FAULTINSTATUS0_HIGH = 0x%08x",
                noc_info->name, idx, val);
            fault_detected = true;
         }
      }
   }
 
   /* Clear OBS error status */
   NOC_OUT32(NOC_REG_ADDR(noc_info->base_addr, noc_info->hw->err_clear_low), 0x1);

   /* Clear any sideband managers. */
   for( idx = 0; idx < noc_info->num_sbms; idx++ )
   {
      if( REGISTER_VALID(noc_info->sb_hw[idx]->faultin_status0_low) )
      {
         val = NOC_IN32(NOC_REG_ADDR(noc_info->sb_base_addrs[idx],
                                     noc_info->sb_hw[idx]->faultin_status0_low));

         /* First SBM low bit is always OBS input. Don't clear the enable for that. */
         if( 0 == idx )
         {
            val = val & ~0x1;
         }

         NOC_OUT32(NOC_REG_ADDR(noc_info->sb_base_addrs[idx],
                                noc_info->sb_hw[idx]->faultin_en0_low), 
                   (NOC_IN32(NOC_REG_ADDR(noc_info->sb_base_addrs[idx],
                                          noc_info->sb_hw[idx]->faultin_en0_low)) & val));
      }
      if( REGISTER_VALID(noc_info->sb_hw[idx]->faultin_status0_high) )
      {
         val = NOC_IN32(NOC_REG_ADDR(noc_info->sb_base_addrs[idx],
                                     noc_info->sb_hw[idx]->faultin_status0_high));

         NOC_OUT32(NOC_REG_ADDR(noc_info->sb_base_addrs[idx],
                                noc_info->sb_hw[idx]->faultin_en0_high), 
                   (NOC_IN32(NOC_REG_ADDR(noc_info->sb_base_addrs[idx],
                                          noc_info->sb_hw[idx]->faultin_en0_high)) & val));
      }
   }

   if(fault_detected && noc_info_oem->error_fatal)
   {
      ERR_FATAL("NOC ERROR DETECTED.", 0, 0, 0);
   }

   NOC_Error_PostInit(hClock, noc_info);
   
  //To avoid Clock DAL handle exhaust issue as power domian enable can be called multiple times
   if(DAL_SUCCESS != DAL_DeviceDetach( hClock))
   {
     ERR_FATAL("NOC_ERROR: Device deattach to Clock failed.", 0, 0, 0);
   }
  
   return NULL;
}

//*============================================================================*/
/**
@brief
      Performs pre NOC_Error_InitNoC initialization for NOC error handler.
      It enables required clocks and power domain.
 
@param[in]  DalDeviceHandle *Clock_handle - .handle for getting clock and power domain ids.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None.  
*/ 
/*============================================================================*/
void NOC_Error_PreInit( DalDeviceHandle *Clock_handle, NOCERR_info_type *    NOCInfo )
{
   ClockIdType nId;
   uint32_t clock_num;

      // /* Initialize power domain, if present and defer configuration. */
      // if( NULL != NOCInfo->power_domain )
      // {
         // if(NOCInfo->power_domain_id == 0)
         // {
            // DalClock_GetPowerDomainId(Clock_handle,
                                   // NOCInfo->power_domain,
                                   // &NOCInfo->power_domain_id);
            // DalClock_EnablePowerDomain(Clock_handle,NOCInfo->power_domain_id);
         // }
      // }
      for(clock_num =0; clock_num<NOCInfo->num_of_clk; clock_num++)
      {
        if(DAL_SUCCESS != 
              DalClock_GetClockId(Clock_handle, NOCInfo->clk_name_array[clock_num],&nId))
        {
          ERR_FATAL("NOCError: Could not get ClockId.", 0, 0, 0);
          return ;
        }
      
        if(DAL_SUCCESS != DalClock_EnableClock(Clock_handle, nId))
        {
          ERR_FATAL("NOCError: Could not enable clock.", 0, 0, 0);
          return ;
        }
      }
}

//*============================================================================*/
/**
@brief 
      Performs post NOC_Error_InitNoC initialization for NOC error handler.
      It disables required clocks and power domain.
 
@param[in]  DalDeviceHandle *Clock_handle - .handle for getting clock ids.

@return    
      None.

@dependencies
      NOC_Error_PreInit and NOC_Error_InitNoC should have been called .
 
@sideeffects 
      None.  
*/ 
/*============================================================================*/
void NOC_Error_PostInit( DalDeviceHandle *Clock_handle, NOCERR_info_type *    NOCInfo )
{
   ClockIdType nId;
   uint32_t clock_num;
   
      for(clock_num =0; clock_num<NOCInfo->num_of_clk; clock_num++)
      {
        if(NOCInfo->clk_disable_array[clock_num])
        {
           if(DAL_SUCCESS != 
                 DalClock_GetClockId(Clock_handle, NOCInfo->clk_name_array[clock_num],&nId))
           {
             ERR_FATAL("NOCError: Could not get ClockId.", 0, 0, 0);
             return ;
           }
          
           if(DAL_SUCCESS != DalClock_DisableClock(Clock_handle, nId))
           {
             ERR_FATAL("NOCError: Could not enable clock.", 0, 0, 0);
             return ;
           }
        }
      }

}

//*============================================================================*/
/**
@brief
      Performs initialization for NOC Error Handler.
      It enables interrupts required to handle NOC errors.

@param[in]  None.

@return
      None.

@dependencies
      None.

@sideeffects
      None.
*/
/*============================================================================*/
void NOC_Error_Init( void )
{
   uint32_t i;
   DalDeviceHandle *hClock = NULL;

   // Get device property data for configuration
   NOCERR_propdata = NOC_Error_Platform_Get_Propdata();
   if(NOCERR_propdata == NULL)
   {
      return;  // NOC data structure is not initialized in data file
   }
   NOCERR_propdata_oem = NOC_Error_Platform_Get_Propdata_OEM();
   if(NOCERR_propdata_oem == NULL)
   {
      return;  // NOC data structure is not initialized in data file
   }

   NOCInfo = NOCERR_propdata->NOCInfo;
   if(NOCInfo == NULL)
   {
      return;  // NOC data structure is not initialized in data file
   }
   NOCInfoOEM = NOCERR_propdata_oem->NOCInfoOEM;
   if(NOCInfoOEM == NULL)
   {
      return;  // NOC data structure is not initialized in data file
   }

   /* Attach to clock driver for any power domain ids */
   if(DAL_SUCCESS != DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &hClock))
   {
      return;
   }

   for(i=0; i < NOCERR_propdata->len; i++)
   {
      /* Initialize power domain, if present and defer configuration. */
      if( NULL != NOCInfo[i].power_domain )
      {
         DalClock_GetPowerDomainId(hClock,
                                   NOCInfo[i].power_domain,
                                   &NOCInfo[i].power_domain_id);
      }
      /* No power domain, initialize now. */
      else
      {
         NOC_Error_PreInit(hClock, &NOCInfo[i]);
         NOC_Error_InitNoC(&NOCInfo[i], &NOCInfoOEM[i]);
         NOC_Error_PostInit(hClock, &NOCInfo[i]);
      }

      /* Enable Interrupts, if required. */
      if(NOCInfoOEM[i].intr_enable)
      {
         /* The interrupt may not be handled locally even if it's
          * configured locally. Check for that here. */
         if(INTERRUPT_UNUSED != NOCInfo[i].intr_vector)
         {
            /* Register IRQ handler */
            if(!NOC_Error_Platform_Register_Interrupt(&NOCInfo[i]))
            {
               ERR_FATAL("NOC init error !", 0, 0, 0);
               return;
            }
         }
      }
   }

   // Enable timeout clocks
   for(i = 0; i < NOCERR_propdata->num_clock_regs; i++)
   {
      NOC_OUT32(NOCERR_propdata->clock_reg_addrs[i], NOCERR_propdata_oem->clock_reg_vals[i]);
   }

   /* Detach from clock driver. We're done with it now. */
   DAL_DeviceDetach(hClock);
}

//*============================================================================*/
/**
@brief 
      Performs deferred initialization for NOC error handler.
      It enables interrupts required to handle NOC errors.
 
@param[in]  id  - the power domain being enabled

@return    
      None.

@dependencies
      NOC_Error_Init() must have been called first.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void NOC_Error_InitDeferred
(
   ClockPowerDomainIdType id
)
{
   uint32_t i;
   DalDeviceHandle *hClock = NULL;

   /* Attach to clock driver for any power domain or clock domain ids */
   if(DAL_SUCCESS != DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &hClock))
   {
      ERR_FATAL("NOC_ERROR: Device attach to Clock failed.", 0, 0, 0);
      return;
   }

   /* Make sure we have data, otherwise skip. */
   if( NULL != NOCInfo && NULL != NOCInfoOEM )
   {
      for( i = 0; i < NOCERR_propdata->len; i++ )
      {
         /* Initialize the matching NoC, if found and deferral is enabled. */
         if( id == NOCInfo[i].power_domain_id &&
             (NOCERR_DEFER_ONCE == NOCInfo[i].deferral_pref ||
              NOCERR_DEFER_ALWAYS == NOCInfo[i].deferral_pref) )
         {
            NOC_Error_PreInit(hClock, &NOCInfo[i]);
            NOC_Error_InitNoC(&NOCInfo[i], &NOCInfoOEM[i]);
            NOC_Error_PostInit(hClock, &NOCInfo[i]);
            /* If the preference was to defer only once,
             * set the preference to NONE, so that we don't do
             * this on subsequent passes. */
            if( NOCERR_DEFER_ONCE == NOCInfo[i].deferral_pref )
            {
               NOCInfo[i].deferral_pref = NOCERR_DEFER_NONE;
            }
            break;
         }
      }
   }
      
  //To avoid Clock DAL handle exhaust issue as power domian enable can be called multiple times
  if(DAL_SUCCESS != DAL_DeviceDetach( hClock))
  {
    ERR_FATAL("NOC_ERROR: Device deattach to Clock failed.", 0, 0, 0);
    return ;
  }
}

/* vim: set ts=3 sts=3 sw=3 et :*/ 

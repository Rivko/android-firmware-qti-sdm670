/*==============================================================================

FILE:      AHBTimeout.c

DESCRIPTION: This file implements AHB Timeout driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/core.qdsp6/2.1/systemdrivers/icb/src/common/ABTimeout.c#2 $ 
$DateTime: 2018/04/06 00:39:57 $
$Author: pwbldsvc $
$Change: 15863303 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2018/04/03  ak     Add deferred ABT programming support
2016/02/08  bc     Added safety for multiple PD sharing.
2013/11/14  tb     Added support for multiple enable/status registers 
2013/08/30  pm     Clear any pending interrupts to cover subsystem restart 
2013/05/26  pm     Fix clearing syndrome valid and interrupt status
2012/10/04  av     Support for disabling ABT 
2012/05/31  av     Created
 
        Copyright (c) 2013-2016 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "ABTimeout.h"
#include "ABT_internal.h"
#include "ABT_platform.h"
#include "string.h"

/*============================================================================
                                  DEFINES
============================================================================*/

#define MAX_NUM_INTR_REGS 2

/**
 * ABT Platform Data type
 */
typedef struct
{
    uint8  hw_version;              /**< ABT_HW_VERSION offset */
    uint8  inst_id;                 /**< ABT_INST_ID offset */
    uint8  num_slaves;              /**< ABT_NUM_SLAVES offset */
    uint8  timer_loadval;           /**< ABT_TIMER_LOADVAL offset */
    uint8  timer_mode;              /**< ABT_MODE offset */
    uint8  intr_status;             /**< ABT_INTR_STATUS offset */
    uint8  intr_clear;              /**< ABT_INTR_CLEAR offset */
    uint8  intr_enable;             /**< ABT_INTR_ENABLE offset */
    uint8  synd_valid;              /**< ABT_SYND_VALID offset */
    uint8  synd_clear;              /**< ABT_SYND_CLEAR offset */
    uint8  synd_id;                 /**< ABT_SYND_ID offset */
    uint8  synd_addr0;              /**< ABT_SYND_ADDR0 offset */
    uint8  synd_addr1;              /**< ABT_SYND_ADDR1 offset */
    uint8  synd_hready;             /**< ABT_SYND_HREADY offset */   
}ABT_hw_type;

/*============================================================================
                                  MACROS
============================================================================*/
#define ABT_OUT8(addr, data)   (*((volatile uint8 *) (addr)) = ((uint8) (data)))
#define ABT_OUT16(addr, data)  (*((volatile uint16 *) (addr)) = ((uint16) (data)))
#define ABT_OUT32(addr, data)  (*((volatile uint32 *) (addr)) = ((uint32) (data)))

#define ABT_IN8(addr)   ((uint8) (*((volatile uint8 *) (addr))))
#define ABT_IN16(addr)  ((uin16) (*((volatile uint16 *) (addr))))
#define ABT_IN32(addr)  ((uint32)(*((volatile uint32 *) (addr))))

#define ABT_REG_ADDR(x,offset)   (((uint8*)SlaveInfo[(x)].base_addr) + (offset))


/*============================================================================
                                 Globals
============================================================================*/

static ABT_propdata_type* PropData;
static ABT_slave_info_type* SlaveInfo;

/*============================================================================
                            ABT HW DEFINITION 
============================================================================*/
 /*Define HW_VERSION_(MAJOR)_(MINOR)_(STEP) as ABT_HW_VERSION default reg value*/

 /*Define All supported HW versions supported by this driver.
   Initialize offsets for all valid registers, invalid register for a
   HW version should be initialized to ZERO */

/* HW Version 1.0.0 */
#define HW_VERSION_1_0_0                 0x10000000

/* ABT HW Version 1.0.0 Offsets */
ABT_hw_type abt_hw_1_0_0 = {
    0x00,              /**< ABT_HW_VERSION offset */
    0x04,              /**< ABT_INST_ID offset */
    0x08,              /**< ABT_NUM_SLAVES offset */
    0x0c,              /**< ABT_TIMER_LOADVAL offset */
    0x10,              /**< ABT_MODE offset */
    0x14,              /**< ABT_INTR_STATUS offset */
    0x18,              /**< ABT_INTR_CLEAR offset */
    0x1c,              /**< ABT_INTR_ENABLE offset */
    0x20,              /**< ABT_SYND_VALID offset */
    0x24,              /**< ABT_SYND_CLEAR offset */
    0x28,              /**< ABT_SYND_ID offset */
    0x2c,              /**< ABT_SYND_ADDR0 offset */
    0x30,              /**< ABT_SYND_ADDR1 offset */
    0x34,              /**< ABT_SYND_HREADY offset */   
};

/* Add future HW versions below...*/


//*============================================================================*/
/**
@brief 
      This function returns pointer to ABT_hw_type structure for specified
      Version ID.
 
@param[in]  Version ID.

@return    
      ABT_hw_type*: If version is supported, returns Pointer to ABT_hw_type
      NULL:         If version is not supported, returns NULL

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
static ABT_hw_type* ABT_Get_HW_type(uint32 version)
{
    switch(version)
    {
        case HW_VERSION_1_0_0:
            return &abt_hw_1_0_0;

        /* Add future HW versions below...*/

        default:
            break;
    }
    return NULL;
}

/*===========================================================================*/
/**
@brief 
      This function reads Interrupt status register and captures syndrome
      registers from corresponding ABT slaves.
 
@param[in]  void* arg Context data for client.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==============================================================================*/
void* ABT_Handle_Interrupt(void* arg)
{
 uint8 i, index, slave;
 uint32 intr_status[MAX_NUM_INTR_REGS], synd_valid, hang_detected = 0;
 ABT_hw_type* hw;
 ABT_syndrome_info_type syndrome;

 for(i=0; i < PropData->len; i++)
 {
      // Perform Platform-specific Initialization
     if(ABT_SUCCESS != ABT_Platform_Init(PropData,0,i))
     {
         // Cannot enable clocks to read syndrome regs. Report bus-hang to err-services
         ABT_Platform_Report_Hang(1,NULL); 
     }
 }

 // Get INTR status
 memset(intr_status, 0, sizeof(intr_status));
 for(i=0; i<PropData->platform_info->num_status_reg && i<MAX_NUM_INTR_REGS; i++)
 {
   intr_status[i] = ABT_IN32(PropData->platform_info->intr_status_addr[i]);
   intr_status[i] &= ABT_IN32(PropData->platform_info->intr_map_addr[i]);
 }

 for(i=0; i<PropData->len; i++)
 {   
     index = SlaveInfo[i].slave_id >> 5;
     slave = SlaveInfo[i].slave_id & 0x1F;
     // Read interrupt status 
     if(intr_status[index] & (1 << slave))
     {
        // Get HW Version type of the Slave
         hw = ABT_Get_HW_type(ABT_IN32((SlaveInfo[i].base_addr)));

         if(hw == NULL)
         {
             // Invalid HW, we shouldn't get here.
             continue;
         }

        // Read syndrom registers
         synd_valid = ABT_IN32(ABT_REG_ADDR(i, hw->synd_valid));  
         if(synd_valid != 0)
         {
             syndrome.slave_name = SlaveInfo[i].name;
             syndrome.id = ABT_IN32(ABT_REG_ADDR(i, hw->synd_id));
             syndrome.addr0 = ABT_IN32(ABT_REG_ADDR(i, hw->synd_addr0));
             syndrome.addr1 = ABT_IN32(ABT_REG_ADDR(i, hw->synd_addr1));
             syndrome.hready = ABT_IN32(ABT_REG_ADDR(i, hw->synd_hready));
             syndrome.num_slaves = ABT_IN32(ABT_REG_ADDR(i, hw->num_slaves));

             // Log syndrome information
             ABT_Platform_Log_Syndrome(&syndrome);

             // Log number of hangs detected
             hang_detected++;

             //Clear synd valid
             ABT_OUT32(ABT_REG_ADDR(i, hw->synd_clear), 0x1);

             //Clear Interrupt status
             ABT_OUT32(ABT_REG_ADDR(i, hw->intr_clear), 0x1);
         }
     }
 }

 if(hang_detected > 0)
 {
     // Report bus-hang to err-services
     ABT_Platform_Report_Hang(hang_detected,&syndrome);
 }
 return arg;
}

//*============================================================================*/
/**
@brief 
      Initializes AHB config driver. It reads AHB configuration data (register 
      address and config data) from DAL Device Property file and configures 
      AHB registers.
 
@param[in]  None.

@return    
      None.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*============================================================================*/
void ABT_InitDeferred( ClockPowerDomainIdType id )
{
   uint8 i, index, slave;
   uint32 reg_data[MAX_NUM_INTR_REGS];
   ABT_hw_type* hw;
     
   // Get device property data for configuration
   PropData = ABT_Platform_Get_Propdata();
   if(PropData == NULL)
   {
       return; // ABT_ERROR_INIT_FAILURE
   }

   //Don't think this is really needed
   //Added to keep consistant with previous revision
   for(i=0;i<PropData->platform_info->num_status_reg && i<MAX_NUM_INTR_REGS;i++)
   {
     reg_data[i]=0;
   }

   // Check Timeout Slave Global EN
   if(!(ABT_IN32(PropData->platform_info->globa_en_addr) & 0x1))
   {
       // ABT is disabled. Nothing to initialize.
       return;
   }

   SlaveInfo = PropData->slave_info;

   // Enable Interrupts & set timeout values
   for(i=0; i < PropData->len; i++)
   {
       if(id==0 && PropData->slave_info[i].deferral_pref == ABT_DEFER_ALWAYS )
       {    
           continue; //skipping the AHB TO configuration for slaves with defer always settings when called from ABT_init.
       }
       // Perform Platform-specific Initialization
       if(ABT_SUCCESS != ABT_Platform_Init(PropData,id,i))
       {
           continue; // ABT_ERROR_INIT_FAILURE
       }
       index = SlaveInfo[i].slave_id >> 5;
       slave = SlaveInfo[i].slave_id & 0x1F;
       // Generate bit-mask for interrupt mappings 
       reg_data[index] |= (SlaveInfo[i].intr_enable << slave); 

       // Get HW Version type of the Slave
       hw = ABT_Get_HW_type(ABT_IN32((SlaveInfo[i].base_addr)));

       if(hw == NULL)
       {
           // Invalid HW, we shouldn't get here.
           continue;
       }
       //enabling timebase ctl
       if(SlaveInfo[i].timebase)
       {
           ABT_OUT32(SlaveInfo[i].timebase,1);
       }
       // Clear any pending interrupts (covering the subsystem restart case)
       // Clear synd valid
       ABT_OUT32(ABT_REG_ADDR(i, hw->synd_clear), 0x1);
       // Clear Interrupt status
       ABT_OUT32(ABT_REG_ADDR(i, hw->intr_clear), 0x1);

       // Program inst_id 
       ABT_OUT32(ABT_REG_ADDR(i, hw->inst_id), i);

       //Set timer mode to load.
       ABT_OUT32(ABT_REG_ADDR(i, hw->timer_mode), 0x00);

       if(SlaveInfo[i].slave_enable == TRUE && SlaveInfo[i].timeout_val != 0)
       {
           //Set timeout value
           ABT_OUT32((ABT_REG_ADDR(i, hw->timer_loadval)), SlaveInfo[i].timeout_val);

           //Set timer mode to run mode
           ABT_OUT32(ABT_REG_ADDR(i, hw->timer_mode), 0x01);
       }

       // Enable Interrupt
       if(SlaveInfo[i].intr_enable)
       {
           ABT_OUT32(ABT_REG_ADDR(i, hw->intr_enable), 0x01);
       }
   }
   
   // Configure Interrupt Mapping
   for(i=0;i<PropData->platform_info->num_status_reg && i<MAX_NUM_INTR_REGS;i++)
   {
     // Only set desired bits, since this register can be shared
     // (e.g. between Modem Host PD ABT, WCSS (Modem User PD) ABT, and
     // WCSS NOC error)
     ABT_OUT32(PropData->platform_info->intr_map_addr[i],
               ABT_IN32(PropData->platform_info->intr_map_addr[i])|reg_data[i]);
   }

   // Perform Platform-specific De-Initialization
   if(ABT_SUCCESS != ABT_Platform_Deinit(PropData))
   {
       return; // ABT_ERROR_INIT_FAILURE
   }

   return; // ABT_SUCCESS
}

void ABT_Init( void )
{

   // Get device property data for configuration
   PropData = ABT_Platform_Get_Propdata();
   if(PropData == NULL)
   {
       return; // ABT_ERROR_INIT_FAILURE
   }
   
   ABT_InitDeferred(0);
   // Register ISR
   if(ABT_SUCCESS != ABT_Platform_Register_Interrupt(PropData))
   {
       return; // ABT_ERROR_INIT_FAILURE
   }

   return; // ABT_SUCCESS
}


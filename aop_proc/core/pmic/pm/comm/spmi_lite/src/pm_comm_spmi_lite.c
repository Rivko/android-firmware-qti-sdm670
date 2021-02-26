/*! \file pm_comm_spmi_lite.c
*  \n
*  \brief pmic spmi lite driver
*  \n
* *  \n &copy; Copyright 2014-2016 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/comm/spmi_lite/src/pm_comm_spmi_lite.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/14   aks     created  
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_comm.h"  
#include "SpmiBus.h"
#include "CoreVerify.h"

#define PM_MAX_REGS     0xFFFE

static boolean spmi_initialized = FALSE;

__attribute__((section("pm_cram_reclaim_pool")))
pm_err_flag_type pm_comm_channel_init_internal(void)
{
    Spmi_Result spmi_result = SPMI_SUCCESS;
    pm_err_flag_type err = PM_ERR_FLAG_SUCCESS ;
    
    // initialize the SPMI BUS
    spmi_result = SpmiBus_Init();

    if(SPMI_SUCCESS  == spmi_result )
    {
        spmi_initialized = TRUE;
    }
    else
    {
        CORE_VERIFY(0); //if spmi_bus init fails err_fatal
    }
    
    return err;
}

pm_err_flag_type pm_comm_read_byte(uint32 slave_id, uint16 addr, uint8* data, uint8 priority)
{
    pm_err_flag_type err = PM_ERR_FLAG_SUCCESS ;
    Spmi_Result spmi_result = SPMI_SUCCESS;
    /* Read the data from the SPMI */
    uint32               dataReadLength = 0;
    
    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG_BUS_ERR ;
        }
        else
        {
            spmi_result = SpmiBus_ReadLong( slave_id, (Spmi_AccessPriority)priority, addr, data, 1, &dataReadLength);

            if ( spmi_result != SPMI_SUCCESS ) { err = PM_ERR_FLAG_BUS_ERR; }
        }
    }
    else
    {
        err = PM_ERR_FLAG_BUS_ERR;
    }

    return err;
}

pm_err_flag_type pm_comm_read_byte_mask(uint32 slave_id, uint16 addr, uint8 mask, uint8* data, uint8 priority)
{
    pm_err_flag_type err = PM_ERR_FLAG_SUCCESS;

    err = pm_comm_read_byte(slave_id,addr,data,priority);
    if ( err != PM_ERR_FLAG_SUCCESS ) 
    { 
        err = PM_ERR_FLAG_BUS_ERR; 
    }
    else 
    { 
        *data = *data & mask; 
    }

    
    return err;
}



pm_err_flag_type pm_comm_write_byte(uint32 slave_id, uint16 addr, uint8 data, uint8 priority)
{
    pm_err_flag_type err = PM_ERR_FLAG_SUCCESS ;
    Spmi_Result spmi_result = SPMI_SUCCESS;
    
    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG_BUS_ERR ;
        }
        else
        {
            spmi_result = SpmiBus_WriteLong(slave_id, (Spmi_AccessPriority)priority, addr, &data, 1);

            if ( spmi_result != SPMI_SUCCESS ) { err = PM_ERR_FLAG_BUS_ERR; }
        }
    }
    else
    {
        err = PM_ERR_FLAG_BUS_ERR;
    }

    return err;
}

pm_err_flag_type pm_comm_write_byte_mask(uint32 slave_id, uint16 addr, uint8 mask, uint8 value, uint8 priority)
{
    pm_err_flag_type         err = PM_ERR_FLAG_SUCCESS ;
    Spmi_Result       spmi_result = SPMI_SUCCESS;
    uint32                   dataReadLength = 1;
    uint8                    data = 0 ;

    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG_BUS_ERR ;
        }
        else
        {
             /* Read the data from the SPMI and leave all the bits other than the mask to the previous state */
            spmi_result = SpmiBus_ReadLong(slave_id, (Spmi_AccessPriority)priority, addr, &data, 1, &dataReadLength);

            if ( spmi_result == SPMI_SUCCESS )
            {
                data &= (~mask); 
                data |= (value & mask);

                spmi_result = SpmiBus_WriteLong(slave_id, (Spmi_AccessPriority)priority, addr, &data, 1);

                if ( spmi_result != SPMI_SUCCESS ) { err = PM_ERR_FLAG_BUS_ERR; }
            }
        }
    }
    else
    {
        err = PM_ERR_FLAG_BUS_ERR;
    }


    return err;
}


pm_err_flag_type pm_comm_write_byte_array(uint32 slave_id, uint16 addr, uint32 num_bytes, uint8* data, uint8 priority) 
{
    pm_err_flag_type err = PM_ERR_FLAG_SUCCESS ;
    Spmi_Result spmi_result = SPMI_SUCCESS;

    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG_BUS_ERR ;
        }
        else
        {
            spmi_result = SpmiBus_WriteLong(slave_id, (Spmi_AccessPriority)priority, addr, data, num_bytes);

            if ( spmi_result != SPMI_SUCCESS ) { err = PM_ERR_FLAG_BUS_ERR ; }
        }
    }
    else
    {
        err = PM_ERR_FLAG_BUS_ERR ;
    }

    return err ;
}

pm_err_flag_type pm_comm_read_byte_array (uint32 slave_id, uint16 addr, uint32 num_bytes, uint8* data, uint8 priority) 
{
    pm_err_flag_type err = PM_ERR_FLAG_SUCCESS ;
    Spmi_Result spmi_result = SPMI_SUCCESS;
    /* Read the data from the SPMI */
    uint32               dataReadLength = 0;

   
    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG_BUS_ERR ;
        }
        else
        {
            spmi_result = SpmiBus_ReadLong(slave_id, (Spmi_AccessPriority)priority, addr, data, num_bytes, &dataReadLength);

            if ( spmi_result != SPMI_SUCCESS ) { err = PM_ERR_FLAG_BUS_ERR; }
        }
    }
    else
    {
        err = PM_ERR_FLAG_BUS_ERR;
    }

    return err;
}

__attribute__((section("pm_cram_reclaim_pool")))
pm_err_flag_type pm_comm_get_vrm_id (uint32 slave_id, uint16 addr, uint16* vrm_id) 
{
    pm_err_flag_type err = PM_ERR_FLAG_SUCCESS;
    Spmi_Result spmi_result = SPMI_SUCCESS;
    uint8 periph_id = 0;

   
    if(TRUE == spmi_initialized)
    {
        /* check for out-of-bounds index */
        if ( addr > PM_MAX_REGS)
        { 
            err = PM_ERR_FLAG_BUS_ERR ;
        }
        else
        {
            periph_id = ((addr>>8)&0xFF);

            spmi_result = SpmiBus_GetVrmId(slave_id, periph_id, vrm_id);

            if ( spmi_result != SPMI_SUCCESS ) { err = PM_ERR_FLAG_BUS_ERR; }
        }
    }
    else
    {
        err = PM_ERR_FLAG_BUS_ERR;
    }

    return err;
}

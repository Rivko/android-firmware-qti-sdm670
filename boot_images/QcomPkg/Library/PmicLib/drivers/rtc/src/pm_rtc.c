/*! \file
*  
*  \brief  pm_rtc.c ----File contains the implementation of the public APIs for RTC resource type.
*  \details Implementation file for RTC resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.
*  
*    PMIC code generation Version: 2.0.0.22
*    This file contains code for Target specific settings and modes.
*  
*  &copy; Copyright 2012 - 2017 QUALCOMM Technologies, Inc All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/15   al      Adding API to read RTC alarm and interrupt
08/20/14   al      Updating comm lib 
05/20/14   al      Architecture update
08/03/12   al      New arch 
 
===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/

#include "pm_rtc.h"
#include "pm_rtc_driver.h"


/*************************************
NOTE: Regular METHOD IMPLEMENTATION
**************************************/ 
pm_register_data_type pm_rtc_time_32_bits_to_rtc_counter_val(pm_rtc_val_shift_type  bits_shift, uint32  sec_32_bits)
{
    pm_register_data_type data = 0;
    
    /* */
    data = (pm_register_data_type) ( sec_32_bits >> bits_shift ) ;
    return (data);
}

uint32 pm_rtc_counter_val_to_time_32_bits(pm_rtc_val_shift_type  bits_shift, pm_register_data_type  rtc_val_8_bits)
{
    uint32 time = 0 ;
    uint32 temp32 = rtc_val_8_bits ;
    
    /* */
    time = temp32 << bits_shift ;
    return time ;
}

/*************************************
NOTE: VIRTUAL METHOD IMPLEMENTATION
**************************************/

pm_err_flag_type pm_hal_rtc_start(uint32 pmic_index, const  pm_hal_rtc_time_type *start_time_ptr)
{
    pm_err_flag_type      err_flag   = PM_ERR_FLAG_SUCCESS;

    uint8 internalResourceIndex =0;

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_index);
    
    if (rtc_ptr == NULL)
    { 
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else 
    {
        if (NULL == start_time_ptr)
        {
            err_flag= PM_ERR_FLAG_INVALID_PARAMETER;
        }
        else
        {
            pm_register_address_type reg_rtc_en_ctl = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->rtc_en_ctl;
            pm_register_address_type reg_wdata0 = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->wdata0;
            pm_register_address_type reg_wdata1 = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->wdata1;
            pm_register_address_type reg_wdata2 = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->wdata2;
            pm_register_address_type reg_wdata3 = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->wdata3;

            //INTLOCK();
            /* clear bit 7 of the SEC_RTC_CNTRL register to disable RTC */
            /* This means taht every time a HalRtcStart API is called. It will STOP RTC first before it START RTC */

            err_flag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_rtc_en_ctl , 0x80, 0, 0);
            
            //TransactionManager::StartTransaction(this, false) TBD;
            err_flag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_wdata3, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL3_SHIFT, start_time_ptr->sec), 0);
            err_flag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_wdata2, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL2_SHIFT, start_time_ptr->sec), 0);
            err_flag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_wdata1, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL1_SHIFT, start_time_ptr->sec), 0);
            err_flag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, reg_wdata0, pm_rtc_time_32_bits_to_rtc_counter_val(PM_RTC_VAL0_SHIFT, start_time_ptr->sec), 0);
           //TransactionManager::EndTransaction(this, false) TBD;                        
            /* set bit 7 of the SEC_RTC_CNTRL register to enable RTC */
            err_flag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_rtc_en_ctl , 0x80, 0x80, 0);

            //INTFREE();
        }
    }
    return err_flag;
}

pm_err_flag_type pm_rtc_hal_rtc_stop(uint32 pmic_index, uint8  internalResourceIndex)
{
    pm_err_flag_type      err_flag   = PM_ERR_FLAG_SUCCESS;

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_index);
    
    if (rtc_ptr == NULL)
    { 
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else 
    {
        pm_register_address_type reg_rtc_en_ctl = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->rtc_en_ctl;

        err_flag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_rtc_en_ctl, 0x80, 0, 0);
    }
    return err_flag;
}

pm_err_flag_type pm_rtc_hal_rtc_get_time(uint32 pmic_index, pm_hal_rtc_time_type  *time_ptr)
{
    pm_err_flag_type		err_flag = PM_ERR_FLAG_SUCCESS;
    uint8 internalResourceIndex =0;
    pm_register_data_type	rtcONOFF, rtc_val0, rtc_val1, rtc_val2, rtc_val3; 

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_index);
    
    rtcONOFF = 0;
    
    if (rtc_ptr == NULL)
    { 
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else
    {
        /* TODO: Find what the initial time is and calculate the maximum time we can pass in. */
        if (NULL == time_ptr)
        {
            err_flag =  PM_ERR_FLAG_INVALID_PARAMETER;
        }
        else
        {
            pm_register_address_type reg_rtc_en_ctl = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->rtc_en_ctl;
            pm_register_address_type reg_rdata0 = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->rdata0;
            pm_register_address_type reg_rdata1 = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->rdata1;
            pm_register_address_type reg_rdata2 = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->rdata2;
            pm_register_address_type reg_rdata3 = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->rdata3;

            // check if RTC HW is currently running (enabled) by looking at RTC_EN bit of SEC_RTC_CNTRL register.

            err_flag = pm_comm_read_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_rtc_en_ctl, 0x80, &rtcONOFF, 0);

            /* If RTC is enabled (ON), rtcONOFF should return "1". And "0" if it is disabled (OFF). */
            if (!(rtcONOFF & 0x80))
            {
                err_flag =  PM_ERR_FLAG_INVALID_PERIPHERAL;
            }
            else
            {
                //INTLOCK();

                //TransactionManager::StartTransaction(this, false) TBD;
                err_flag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_rdata3, &rtc_val3, 0);
                err_flag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_rdata2, &rtc_val2, 0);
                err_flag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_rdata1, &rtc_val1, 0);
                err_flag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, reg_rdata0, &rtc_val0, 0);
               //TransactionManager::EndTransaction(this, false) TBD;                            

                //INTFREE();
                /* calculations are performed outside of intlock to shorten time spent in inlock. */
                time_ptr->sec = pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL3_SHIFT, rtc_val3);
                time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL2_SHIFT, rtc_val2);
                time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL1_SHIFT, rtc_val1);
                time_ptr->sec += pm_rtc_counter_val_to_time_32_bits(PM_RTC_VAL0_SHIFT, rtc_val0);
            }
        }
    }
    return err_flag;
}


pm_err_flag_type pm_rtc_hal_rtc_set_time_adjust(uint32 pmic_index, uint8  time_adjust, uint8  internalResourceIndex)
{
    pm_err_flag_type      err_flag   = PM_ERR_FLAG_SUCCESS;

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_index);
    
    if (rtc_ptr == NULL)
    { 
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else 
    {
        pm_register_address_type reg_time_adj = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->time_adj;

        // do sanity checking on params ...
        /* TODO: Revist this API to fix input parameter "time adjust". This will affect CMI. */
        /* time_adjust should get changed from raw data to doc values. */
        /* Otherwise 0x7F needs to be defined in strucuture for cleaner code */
        /* Unless.. is there a reason why we return the data raw? without using the given equation? */
        if (time_adjust > 0x7F)
        {
            return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        //INTLOCK();

        err_flag = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id,  reg_time_adj, 0x7F, time_adjust, 0 );
        //INTFREE();
    }
    return err_flag;
}

pm_err_flag_type pm_rtc_hal_rtc_get_time_adjust(uint32 pmic_index, uint8  *time_adjust_ptr, uint8  internalResourceIndex)
{
    pm_err_flag_type      err_flag   = PM_ERR_FLAG_SUCCESS;

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_index);
    
    if (rtc_ptr == NULL)
    { 
        err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
    else 
    {
        // do sanity checking on params ...
        if (NULL == time_adjust_ptr)
        {
            return PM_ERR_FLAG_INVALID_PARAMETER;
        }

        pm_register_address_type reg_time_adj = (rtc_ptr->rtc_reg_table->base_address) + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset*internalResourceIndex) + rtc_ptr->rtc_reg_table->time_adj;

        //INTLOCK();
        // do sanity checking on params ...
        /* TODO: Revist this API to fix input parameter "time adjust". This will affect CMI. */
        /* time_adjust should get changed from raw data to doc values. */
        /* Otherwise 0x7F needs to be defined in strucuture for cleaner code */
        /* Unless.. is there a reason why we return the data raw? without using the given equation? */
        /* TODO: Also bit<7> is reserved and not used for time adjust, but from 7x30, it is included. Is there a reason why? */
        err_flag = pm_comm_read_byte_mask(rtc_ptr->comm_ptr->slave_id, reg_time_adj, 0x7F,  time_adjust_ptr, 0);
        //INTFREE();
    }
    return err_flag;
}


pm_err_flag_type pm_rtc_irq_enable(uint32 pmic_chip, pm_rtc_periph_type periph_index, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_address_type irq_reg;
    pm_register_data_type data = (periph_index == PM_RTC_ALARM)? 0x2 : 0x1 ;

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    if (enable)
    {
        irq_reg = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_en_set;
    }
    else
    {
        irq_reg = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_en_clr;
    }

    err_flag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, irq_reg, data, 0);
    return err_flag;
}

pm_err_flag_type pm_rtc_irq_clear(uint32 pmic_chip, pm_rtc_periph_type periph_index)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);
    pm_register_data_type data = (periph_index == PM_RTC_ALARM)? 0x2 : 0x1 ;
    
    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type int_latched_clr = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_latched_clr;

    err_flag = pm_comm_write_byte(rtc_ptr->comm_ptr->slave_id, int_latched_clr, data, 0);
    return err_flag;
}

pm_err_flag_type pm_rtc_irq_set_trigger(uint32 pmic_chip, pm_rtc_periph_type periph_index, pm_irq_trigger_type trigger)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type set_type, polarity_high, polarity_low;
    pm_register_data_type mask = (periph_index == PM_RTC_ALARM)? 0x2 : 0x1 ;

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);
    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type int_set_type = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_set_type;
    pm_register_address_type int_polarity_high = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_polarity_high;
    pm_register_address_type int_polarity_low = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_polarity_low;

    switch (trigger)
    {
    case PM_IRQ_TRIGGER_ACTIVE_LOW:
        set_type = 0x00;
        polarity_high = 0x00;
        polarity_low = 0xFF;
        break;
    case PM_IRQ_TRIGGER_ACTIVE_HIGH:
        set_type = 0x00;
        polarity_high = 0xFF;
        polarity_low = 0x00;
        break;
    case PM_IRQ_TRIGGER_RISING_EDGE:
        set_type = 0xFF;
        polarity_high = 0xFF;
        polarity_low = 0x00;
        break;
    case PM_IRQ_TRIGGER_FALLING_EDGE:
        set_type = 0xFF;
        polarity_high = 0x00;
        polarity_low = 0xFF;
        break;
    case PM_IRQ_TRIGGER_DUAL_EDGE:
        set_type = 0xFF;
        polarity_high = 0xFF;
        polarity_low = 0xFF;
        break;
    default:
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }
   err_flag  = pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, int_set_type, mask, set_type, 0);
   err_flag |= pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, int_polarity_high, mask, polarity_high, 0);
   err_flag |= pm_comm_write_byte_mask(rtc_ptr->comm_ptr->slave_id, int_polarity_low, mask, polarity_low, 0);

    return err_flag;
}



pm_err_flag_type pm_rtc_irq_status(uint32 pmic_chip, pm_rtc_periph_type periph_index, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data;
    pm_register_address_type int_sts;
    pm_register_data_type mask = (periph_index == PM_RTC_ALARM)? 0x2 : 0x1 ;

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);

    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    switch (type)
    {
    case PM_IRQ_STATUS_RT:
        int_sts = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_rt_sts;
        break;
    case PM_IRQ_STATUS_LATCHED:
        int_sts = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_latched_sts;
        break;
    case PM_IRQ_STATUS_PENDING:
        int_sts = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->int_pending_sts;
        break;
    default:
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_read_byte_mask(rtc_ptr->comm_ptr->slave_id, int_sts, mask, &data, 0);
    *status = data ? TRUE : FALSE;

    return err_flag;

}

pm_err_flag_type pm_rtc_alarm_enable_status(uint32 pmic_chip, pm_rtc_periph_type periph_index, boolean* enabled)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG_SUCCESS;
    pm_register_data_type data = 0;

    pm_rtc_data_type *rtc_ptr = pm_rtc_get_data(pmic_chip);
    if (rtc_ptr == NULL)
    {
        return PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type alarm_status1 = rtc_ptr->rtc_reg_table->base_address + (pm_register_address_type)(rtc_ptr->rtc_reg_table->peripheral_offset * periph_index) + rtc_ptr->rtc_reg_table->alarm_status1;

    err_flag = pm_comm_read_byte(rtc_ptr->comm_ptr->slave_id, alarm_status1, &data, 0);
    *enabled = (data & 0x80) ? TRUE : FALSE;

    return err_flag;
}



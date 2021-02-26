#ifndef __PM_BSI_H__
#define __PM_BSI_H__

/*! \file
*  \n
*  \brief  pm_bsi.h PMIC-BSI MODULE RELATED DECLARATION 
*  \details  This header file contains functions and variable declarations 
*  to support Qualcomm PMIC SMBB (Switch-Mode Battery Charger) module. The 
*  Switched-Mode Battery Charger (SMBB) module includes a buck regulated 
*  battery charger with integrated switches. The SMBB module, along with the 
*  Over Voltage Proection (OVP) module will majorly be used by charger 
*  appliation for charging Li-Ion batteries with high current (up to 2A).
*  \n &copy;
*  Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/01/14   al      Synced API name with one in source fille 
05/20/14   al      Architecture update 
04/18/14   al      Updated copyright 
03/04/14   va      API to Enable Pull up, Poll Tx Go Status, Enable Rx before Slave Hard Reset
01/13/14   al      Added API to set/release BCL line and read BIF status 
04/22/13   sm      Added pm_bsi_get_err_flag_status()
                   Corrected the order of INT in pm_bsi_irq_type
02/05/13   dy      Initial version. 
========================================================================== */
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \enum pm_smbc_cmd_type
   \brief Use these commands to enable or disable certain functionalities in
 *  the SMBB module.
 */
typedef enum
{
   /*! Disable the functionality.  */
   PM_BSI_CMD__DISABLE,
   /*! Enable the functionality.   */
   PM_BSI_CMD__ENABLE
}pm_bsi_cmd_type;


typedef enum
{
   PM_BSI_TAU_CFG__150_us = 0,
   PM_BSI_TAU_CFG__122_us,
   PM_BSI_TAU_CFG__61_us,
   PM_BSI_TAU_CFG__31_us,
   PM_BSI_TAU_CFG__15_us,
   PM_BSI_TAU_CFG__8_us,
   PM_BSI_TAU_CFG__4_us,
   PM_BSI_TAU_CFG__2_us
}pm_bsi_tau_cfg_type;

typedef enum
{
   PM_BSI_SAMPLING_RATE__4X,
   PM_BSI_SAMPLING_RATE__8X,
}pm_bsi_sampling_rate_type;

typedef enum
{
   PM_BSI_DATA_FORMAT__11_BIT,
   PM_BSI_DATA_FORMAT__17_BIT
}pm_bsi_data_format_type;

typedef enum
{
   PM_MIPI_BIF__NO_ERROR = 0,
   PM_MIPI_BIF__NO_STOP, //RX DATA BIT NUMBER IS OVER 17, STOP BIT NOT RECEIVED
   PM_MIPI_BIF__PARITY_ERR,
   PM_MIPI_BIF__INVERSION_ERR,
   PM_MIPI_BIF__FLOW_CONTROL_ERR,
   PM_MIPI_BIF__BUS_COLLISION_ERR,
   PM_MIPI_BIF__BIT_TIMING_2_TAU,
   PM_MIPI_BIF__BIT_TIMING_4_TAU,  //7
   PM_MIPI_BIF__BCF_EQUAL_TO_BCFN  //8
}pm_bsi_error_code_type;

typedef enum
{
   PM_BSI_MIPI_BIF__ERR_INT            = 0,  
   PM_BSI_MIPI_BIF__RX_INT             = 1,  
   PM_BSI_MIPI_BIF__TX_INT             = 2, 
   PM_BSI_MIPI_BIF__TX_DISABLE_INT     = 3,
   PM_BSI_MIPI_BIF__NODEB_BAT_PRES_INT = 4,
   PM_BSI_MIPI_BIF__DEB_BAT_PRES_INT   = 5, 
   PM_BSI_IRQ_INVALID
}pm_bsi_irq_type;

typedef enum
{
   PM_MIPI_BIF__DLY_0_TAU,
   PM_MIPI_BIF__DLY_5_TAU,
   PM_MIPI_BIF__DLY_10_TAU,
   PM_MIPI_BIF__DLY_15_TAU,
   PM_MIPI_BIF__DLY_TAU_INVALID
}pm_bsi_dly_type;

typedef struct pm_bsi_status_type
{
    pm_bsi_cmd_type            bsi_enable;
    pm_bsi_tau_cfg_type        tau_cfg;
    pm_bsi_data_format_type    rx_data_format;
    pm_bsi_data_format_type    tx_data_format;
    pm_bsi_cmd_type            rx_enable;
    pm_bsi_cmd_type            tx_enable;
    pm_bsi_sampling_rate_type  sampling_rate;
    boolean                    bcl_force_low;    
} pm_bsi_status_type;



/*===========================================================================

                 SMBB DRIVER FUNCTION PROTOTYPES

===========================================================================*/

/**
 * @brief This function enables/disables the BSI peripheral
 * 
 * @details
 *  This function enables/disables the BSI peripheral. 
 *  SW should check if all transactions are completed before disabling BSI.
 * 
 * @param[in] pmic_device_index. Primary: 0
 * @param[in] enable
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_bsi_enable(uint32 device_index, pm_bsi_cmd_type enable);


pm_err_flag_type pm_bsi_set_tau_cfg(uint32 device_index, pm_bsi_tau_cfg_type tau_cfg);
pm_err_flag_type pm_bsi_set_rx_data_format(uint32 device_index, pm_bsi_data_format_type data_format);
pm_err_flag_type pm_bsi_set_tx_data_format(uint32 device_index, pm_bsi_data_format_type data_format);
pm_err_flag_type pm_bsi_rx_enable(uint32 device_index, pm_bsi_cmd_type enable);
pm_err_flag_type pm_bsi_tx_enable(uint32 device_index, pm_bsi_cmd_type enable);
pm_err_flag_type pm_bsi_read_rx_data(uint32 device_index, uint32* rd_data);
pm_err_flag_type pm_bsi_write_tx_data(uint32 device_index, uint32 wr_data);
pm_err_flag_type pm_bsi_tx_go(uint32 device_index);
pm_err_flag_type pm_bsi_get_tx_go_status(uint32 device_index, boolean* go_sts);
pm_err_flag_type pm_bsi_get_err_flag_status(uint32 device_index, boolean* err_flag_sts);

pm_err_flag_type pm_bsi_set_sampling_rate(uint32 device_index, pm_bsi_sampling_rate_type sampling_rate);
pm_err_flag_type pm_bsi_clear_error(uint32 device_index);
pm_err_flag_type pm_bsi_get_error_code(uint32 device_index, pm_bsi_error_code_type* error);

/**
 * @brief This function enables or disables BSI irq
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                BSI irq type. Refer enum pm_bsi_irq_type.
 *  
 * @param[in] enable:  
 *                TRUE: Enable corresponding BSI interrupt 
 *                FALSE:Disable corresponding BSI interrupt
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_bsi_irq_enable(uint32 device_index, pm_bsi_irq_type irq, boolean enable);


/**
 * @brief This function clears the BSI irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SMBB irq type. Refer enum pm_bsi_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_bsi_irq_clear(uint32  device_index, pm_bsi_irq_type irq);


/**
 * @brief This function configures the BSI irq trigger 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                SMBB irq type. Refer enum pm_bsi_irq_type.  
 * @param[in] trigger:  
 *                One of different irq triggers. Refer enum pm_irq_trigger_type 
 *                from pm_uefi_irq.h for more details
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_bsi_irq_set_trigger(uint32 device_index, pm_bsi_irq_type irq, pm_irq_trigger_type trigger);


/**
 * @brief This function configures the BSI irq 
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] irq:  
 *                BSI irq type. Refer enum pm_bsi_irq_type. 
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_uefi_irq.h for more details
 * @param[out] status:  
 *                IRQ status.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_bsi_irq_status(uint32 device_index, pm_bsi_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * @brief This function forces/rleases BCL low
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] bcl_force_low  
 *                TRUE: Forces BCL to be low
 *                VALSE: Releases BCL line
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_bsi_set_bcl_force_low(uint32 device_index, boolean bcl_force_low);


/**
 * @brief This function reads the various BSI 
 *        settings
 * 
 * @param[in]  device_index. Primary: 0. Secondary: 1
 * @param[out] bsi_status  BSI setting status 
 *                
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_bsi_get_status(uint32 device_index, pm_bsi_status_type* bsi_status);

/**
 * @brief This function enables or disables Pull up register
 * 
 * @param[in] device_index. Primary: 0. Secondary: 1
 * @param[in] enable:  
 *                TRUE: Enable Pull up Register 
 *                FALSE:Disable Pull up Register 
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG_SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_bsi_pullup_enable(uint32 device_index, pm_bsi_cmd_type enable);

pm_err_flag_type pm_bsi_fsm_reset(uint32 device_index, boolean fsm_reset, boolean rx_fifo_reset);

pm_err_flag_type pm_bsi_rx_cfg(uint32 device_index, boolean fifo_en, uint8 rx_irq_freq);

pm_err_flag_type pm_bsi_cfg_tx_dly(uint32 device_index, pm_bsi_dly_type dly_type);

pm_err_flag_type pm_bsi_get_rx_fifo_word(uint32 device_index, uint64* words);

pm_err_flag_type pm_bsi_bat_gone_deb_cfg(uint32 device_index, boolean select_batt_miss_deb);

pm_err_flag_type pm_bsi_set_batt_rmv_deb_timer(uint32 device_index, uint8 time);

pm_err_flag_type pm_bsi_set_batt_pres_deb_timer(uint32 device_index, uint8 time);


#endif /* __PM_BSI_H__ */


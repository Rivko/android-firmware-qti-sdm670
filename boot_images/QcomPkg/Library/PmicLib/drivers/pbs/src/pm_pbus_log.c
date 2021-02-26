/*! \file pm_pbus_log.c
*  \n
*  \brief This file contains the implementation of the API for PBS logger support.
*  \n
*  \n &copy; Copyright 2015-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/PmicLib/drivers/pbs/src/pm_pbus_log.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/16   akm     Created.
========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
#include <string.h>
#include "busywait.h"
#include "pmio_bus.h"
#include "pm_err_flags.h"
#include "pm_comm.h"
#include "pm_pbus.h"
#include "device_info.h"
#include "pm_utils.h"

/*===========================================================================
Type Definitions
===========================================================================*/

/**
  * 64 bit number.
  */
typedef struct int64_t
{
  uint8 dummy_c; // keep compiler happy
  union {
    uint8  c_array[8];
    uint64 int64_n;
    struct {
      uint32 lsb;
      uint32 msb;
    } int32_n;
  }int64_val;

} int64_type;


/*===========================================================================
Data Declarations
===========================================================================*/

#define PMIO_RTC_RW_SEC_ACCESS_ADDR              0x000060d0
#define PMIO_RTC_RW_TEST2_ADDR                   0x000060e3
#define PMIO_RTC_RW_TEST2_TS_ENABLE_BMSK         0x1
#define PMIO_RTC_RW_SEC_ACCESS_SEC_UNLOCK_BMSK   0xa5


/*===========================================================================
Functions
===========================================================================*/

/**
 * @brief This function configures and captures the PBUS logger.
 *
 * @details
 *
 * @param[in/out] a pointer to a buffer that contains data from
 *        the PBUS logger in 64 bits format as below:
 *
 * @return
 * PM_ERR_FLAG_SUCCESS -- on success otherwise PMIC error code.
 */
pm_err_flag_type pm_pbus_logger
(
  uint8 device_index,
  pm_pbus_data_type* pbus_data_ptr
)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG_SUCCESS;

  uint16 buffer_size = 127;
  uint32 slave_id = 0;
  uint32 i;
  uint32 pbus_data;
  uint8 reg_val = 0, log_count   = 0;

  err_flag = pm_version_detect();
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return PM_ERR_FLAG_INVALID_PARAMETER;
  }

  /* Init pmic comm interface. */
  pm_comm_info_init();

  /* Get PMIC device Primary slave id */
  err_flag = pm_get_slave_id(device_index, 0, &slave_id);


  /* Initialize PBUS logger */

  /*Configure logger control:  PERPH_FILTER=no,TIMESTAMP=Yes,log R&W,No Wrap */
  err_flag = pm_comm_write_byte(slave_id, PMIO_BUS_LOGGER_CTL_ADDR, 0x0, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }
  /* Wait for the logger */
  busywait(20);

   /* Disable  the logger */
  err_flag = pm_comm_write_byte(slave_id, PMIO_BUS_LOGGER_EN_ADDR, 0x0, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

   /* Set MEM logger address to 0. */
  err_flag = pm_comm_write_byte(slave_id, PMIO_BUS_MEM_INTF_ADDR_ADDR, 0x0, 0);
   if (err_flag != PM_ERR_FLAG_SUCCESS)
   {
     return err_flag;
   }

  /* Enable the logger */
  err_flag = pm_comm_write_byte(slave_id, PMIO_BUS_LOGGER_EN_ADDR, 0x80, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }


  /* Start the logging. */


  /* Read status. */
  err_flag = pm_comm_read_byte(slave_id,PMIO_BUS_STATUS1_ADDR,&reg_val,0);
  if (err_flag)
  {
    return err_flag;
  }

  /* Read MEMPTR value. */
  err_flag = pm_comm_read_byte(slave_id,PMIO_BUS_MEM_INTF_ADDR_ADDR,&log_count , 0);

  /* Check logger full.
  if (!(reg_val& PMIO_BUS_STATUS1_LOGGER_FULL_BMSK))
  {
    return PM_ERR_FLAG_BUS_ERR;
  }*/

  /* Disable the logger to read MEMPTR */
  err_flag = pm_comm_write_byte(slave_id, PMIO_BUS_LOGGER_EN_ADDR, 0x0, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Read MEMPTR value. */
  err_flag = pm_comm_read_byte(slave_id,PMIO_BUS_MEM_INTF_ADDR_ADDR,&log_count , 0);

  /* Set MEM logger address to 0. */
  err_flag = pm_comm_write_byte(slave_id, PMIO_BUS_MEM_INTF_ADDR_ADDR, 0x0, 0);
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }

  /* Trigger data to read and increment MEMPTR. */
  err_flag |= pm_comm_write_byte( slave_id, PMIO_BUS_CAPTURE_INC_ADDR, 0x1,0);

  for (i=0; i < buffer_size; i++)
  {

    /* Read from MEM_INTF_DATA0-7,. */
    err_flag |= pm_comm_read_byte_array( slave_id,
                                         PMIO_BUS_MEM_INTF_DATA0_ADDR, 8,
                                          (uint8*)&(pbus_data), 1);

    /* Trigger data to read and increment MEMPTR. */
    err_flag |= pm_comm_write_byte( slave_id, PMIO_BUS_CAPTURE_INC_ADDR, 0x1,0);
    
    pbus_data_ptr->data[i] = (uint64)pbus_data;
    
  }
  if (err_flag != PM_ERR_FLAG_SUCCESS)
  {
    return err_flag;
  }
  return err_flag;

} /* pm_pbus_logger */

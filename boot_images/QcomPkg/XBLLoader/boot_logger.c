/*=============================================================================

                                boot logger 

GENERAL DESCRIPTION
  This file contains the function implementation for logging messages during boot.
  Time unit in boot logger is microsecond(10^-6 second).

Copyright 2010-2017 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  
when       who        what, where, why
--------   ---        ---------------------------------------------------------
09/22/17   rohik      Added support to log I-cache and D-cache
04/24/17   ds		  boot_log_pbl_clk_freq
01/20/17   kpa        Update .bss.BOOT_INTERNAL_HEAP to .bss.BOOT_INTERNAL_MEMZI
01/30/17   ds         added boot_log_pbl_version
06/15/16   kpa        Added boot_log_set_init_info boot_log_get_init_info
08/20/15   elt        Added boot_log_start_time_marker, boot_log_stop_time_marker,
                      boot_log_stop_time_marker_optional_data, and modified 
                      boot_log_relocate to move time marker data to ddr         
08/18/15   elt        Added call to boot_log_ram_to_uart for async. UART logging
07/30/15   ck         Added option to raw fuse entries to disable printing if secboot is enabled
06/24/15   ck         Added boot_log_boot_device_type
06/10/14   ck         Added boot_log_ apps_frequency, image_version, ddr_frequency,
                      boot_config_register, delta_time_optional_data, stop_timer_optional_data,
                      message_optional_data, message_raw, and insert_pbl_milestones
03/14/14   ck         Fixed missing uart logging
08/23/13   lm         Added UART Logger support
12/10/12   plc        Fix Klockwork Errors
08/30/12   dh         Change boot_log_copy to boot_log_relocate
05/10/12   dh         Change boot_log_set_ref_time to take an argument as reference time
06/21/11   kpa        Add boot_log_forever_loop_check api
11/10/10   dh         Add boot_log_copy api
09/27/10   dh         Add meta info structure to boot logger
08/11/10   dh         Initial port from 7x30 
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdio.h>
#include <string.h>
#include BOOT_PBL_H
#include "boot_logger.h"
#include "boot_logger_ram.h"
#include "boot_logger_jtag.h"
#include "boot_logger_timer.h"
#include "boot_logger_uart.h"
#include "boot_extern_clk_interface.h"
#include "image_version.h"
#include "boot_target.h"
#include "boot_msm.h"
#include "HALbootHWIO.h"
#include "boot_cache_mmu.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/**
 * Infinite loop
 */
#define INFINITE_LOOP() while(1)

/**
* Log message type(char) : absolute time since boot
*/
#define LOG_MSG_TYPE_BOOT  'B'

/**
* Log message type(char) : delta time
*/
#define LOG_MSG_TYPE_DELTA 'D'


/**
* Infinite loop magic number
*/
#define BOOT_LOGGER_LOOP_MAGIC_NUM 0xD1C2B3A3

/**
* Pointer to the meta info of current boot logger
*/
static struct boot_log_meta_info *log_meta_info_ptr;

/**
* start and size of initial log info
*/
uintnt boot_log_init_info_size = 0;
uint8 *boot_log_init_info_ptr = NULL;


/* Place bootlog buffer in internal heap to keep it off the stack. */
char bootlog_buffer[BOOT_LOG_TEMP_BUFFER_SIZE] SECTION(".bss.BOOT_INTERNAL_MEMZI");


struct boot_log_raw_fuse_entry
{
  uint32 fuse_address;
  char * fuse_name;
  boolean full_row;           /* Indicates full 64 bit row should be logged.  Default is 32 bits. */
  boolean secboot_protected;  /* If secboot is enabled then do not log this entry. */
};


/* Array of raw fuse addresses and names to be logged during boot log
   initialization.  Array must be null terminated. */
static struct boot_log_raw_fuse_entry boot_log_raw_fuse_entries[] =
  {
    { BOOT_LOGGER_BOOT_CONFIG_FUSE_ADDRESS, "Boot Config", FALSE, FALSE },
    { BOOT_LOGGER_JTAG_ID_FUSE_ADDRESS, "JTAG ID", FALSE, FALSE },
    { BOOT_LOGGER_OEM_ID_FUSE_ADDRESS, "OEM ID", FALSE, FALSE },
    { BOOT_LOGGER_SERIAL_NUM_FUSE_ADDRESS, "Serial Number", FALSE, FALSE },
    { BOOT_LOGGER_OEM_CONFIG_ROW_0_FUSE_ADDRESS, "OEM Config Row 0", TRUE, TRUE },
    { BOOT_LOGGER_OEM_CONFIG_ROW_1_FUSE_ADDRESS, "OEM Config Row 1", TRUE, TRUE },
    { BOOT_LOGGER_FEATURE_CONFIG_ROW_0_FUSE_ADDRESS, "Feature Config Row 0", TRUE, FALSE },
    { BOOT_LOGGER_FEATURE_CONFIG_ROW_1_FUSE_ADDRESS,"Feature Config Row 1", TRUE, FALSE },
    { 0, NULL, FALSE, FALSE }
  };


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_log_set_meta_info

** ==========================================================================
*/
/*!
* 
* @brief
*  Boot Logger uses a global pointer to store the address of meta info.
*  In the circumstances of any code initializes RW section of the load image 
*  after the boot logger is initialized, this function should be called to 
*  restore the meta info address that is kept in RW section of boot logger module.
*  By doing this we can log the time takes to initilize RW section of the image.
*
* @param[in] addr Address of boot logger's buffer
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_set_meta_info(void *addr)
{
  log_meta_info_ptr = (struct boot_log_meta_info *)addr;
}
/* boot_log_set_meta_info */


/*===========================================================================

**  Function :  boot_log_get_meta_info

** ==========================================================================
*/
/*!
* 
* @brief
*  This function returns a pointer to the meta info structure used by this logger.
*
* @par Dependencies
*   None
*   
* @retval
*   a pointer to the meta info structure used by this logger.
* 
* @par Side Effects
*   None
* 
*/
struct boot_log_meta_info* boot_log_get_meta_info()
{
  return log_meta_info_ptr;
}


/*===========================================================================

**  Function :  boot_log_init

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will set the address of logging buffer and initialize it.
*  This function will set the internal reference time to be 
*  the value of the old boot logger meta info structure,
*  which is passed through boot_log_init_data.
*  It will be set to 0 if this old meta info structure pointer is set to NULL 
*  in boot_log_init_data.
*  This function must be called before any other boot logger function is used.
*
* @param[in] init_data  Initialization data passed to boot logger
*
* @par Dependencies
*   boot_logger_ram
*   boot_logger_timer
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_init(boot_log_init_data *init_data)
{
  /*we must first set meta info becasue boot_log_init_ram and
   * boot_log_init_timer will use the meta info structure*/
  boot_log_set_meta_info(init_data->meta_info_start);
  boot_log_init_ram(init_data);
  boot_init_timer(init_data);
  boot_log_init_uart();

  boot_log_image_version();

  boot_log_boot_device_type();

  boot_log_secboot_status();

  boot_log_raw_fuse_values();

  boot_log_apps_frequency(0);
  
  boot_log_pbl_version();
  
  boot_log_pbl_clk_freq();
  
  boot_log_cache_status();
}
/* boot_log_init */


/*===========================================================================

**  Function :  boot_log_set_ref_time

** ==========================================================================
*/
/*!
* 
* @brief
* This function will set the  timestamp(unit is microsecond) passed in the argument 
* as a reference time to be used by all logging functions.
* the reference time can be set at the beginning in order to get an accurate result.
*
* @param[in] ref_time  reference time that all logging function will use. Unit should
*                      be microsecond
*
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_set_ref_time(uint32 ref_time)
{
  if(log_meta_info_ptr != NULL)
  {
    log_meta_info_ptr->ref_time = ref_time;
  }
}

/*===========================================================================

**  Function :  boot_log_message

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message using various logging methods.It will prefix a timestamp
*  to the message string. The timestamp is calculated as the current absolute time 
*  subtracts the reference time.
*
* @param[in] message         log message
*
* @par Dependencies
*   boot_logger_ram
*   boot_logger_timer  
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_message(char * message)
{
  /* Call boot_log_message_optional_data so two functions do not have
     redundant logic. */
  boot_log_message_optional_data(message,
                                 NULL);
}
/* boot_log_message */


/*===========================================================================

**  Function :  boot_log_message_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message using various logging methods.It will prefix a timestamp
*  to the message string. The timestamp is calculated as the current absolute time 
*  subtracts the reference time.
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*   boot_logger_ram
*   boot_logger_timer  
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_message_optional_data(char *message,
                                    char * optional_data)
{
  uint32 current_timestamp = boot_get_time();

  /*Logs message with time stamp in ram, must be initialized first.*/
  boot_log_message_ram(message,
                       current_timestamp,
                       LOG_MSG_TYPE_BOOT,
                       optional_data);

  /* Transmit the message with time stamp */
  boot_log_message_uart(message,
                        current_timestamp,
                        LOG_MSG_TYPE_BOOT,
                        optional_data);
}
/* boot_log_message_optional_data */


/*===========================================================================

**  Function :  boot_log_message_raw

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message.  Unlike boot_log_message, all data points must be
*  provided.
*
* @param[in] message  log message
*
* @param[in] timestamp  timestamp to log
*   Value of BOOT_DND_TIMESTAMP indicates timestamp will not be printed
*
* @param[in] log_type  type of the log
*
* @param[in] optional_data  Optional data that can be added to entry
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_message_raw(char * message,
                          uint32 timestamp,
                          char log_type,
                          char * optional_data)
{
  /*Logs message with time stamp in ram, must be initialized first.*/
  boot_log_message_ram(message,
                       timestamp,
                       log_type,
                       optional_data);

  /* Transmit the message with time stamp */
  boot_log_message_uart(message,
                        timestamp,
                        log_type,
                        optional_data);
}
/* boot_log_message_raw */


/*===========================================================================

**  Function :  boot_log_start_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will mark the beginning of the timed section and store the 
*  current timestamp interally.
*
* @par Dependencies
*   boot_logger_timer
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_start_timer()
{
  boot_start_timer();
}
/* boot_log_start_timer */

/*===========================================================================

**  Function :  boot_log_start_time_marker

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will try to allocate and start a new time marker
*  and mark the beginning of the timed section and store the current 
*  timestamp internally.
*
* @par Dependencies
*   boot_logger_timer
*   
* @retval
*   Handle to created time marker. Returns LOG_TIME_MARKER_OVERFLOW if space 
*   is not available for creating a new time marker
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_start_time_marker(void)
{
  return boot_start_time_marker();
}
/* boot_log_start_time_marker */

/*===========================================================================

**  Function :  boot_log_stop_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will subtract the current timestamp with the previus stored 
*  timestamp by boot_log_start_timer(if any) to get the delta time value and 
*  then prefix the delta time to the log message.
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*  boot_logger_timer
*  boot_logger_ram
*   
* @retval
*   delta time
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_stop_timer(char * message)
{
  /* Call boot_log_stop_timer_optional_data so two functions do not have
     redundant logic. */

  return boot_log_stop_timer_optional_data(message,
                                           NULL);
}
/* boot_log_stop_timer */
 

/*===========================================================================

**  Function :  boot_log_stop_timer_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will subtract the current timestamp with the previus stored 
*  timestamp by boot_log_start_timer(if any) to get the delta time value and 
*  then prefix the delta time to the log message.
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*  boot_logger_timer
*  boot_logger_ram
*   
* @retval
*   delta time
* 
* @par Side Effects
*   None
* 
*/
 uint32 boot_log_stop_timer_optional_data(char *message,
                                          char *optional_data)
{
  return boot_log_stop_time_marker_optional_data(LOG_BASE_TIME_MARKER, 
                                                 message, 
                                                 optional_data);
}
/* boot_log_stop_timer_optional_data */
 
/*===========================================================================

**  Function :  boot_log_stop_time_marker

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will substract the current timestamp with the previus stored 
*  timestamp for a time marker specified by handle to get the delta time 
*  value and then prefix the delta time to the log message.
*
* @param[in] handle  handle to time marker returned by 
*                    boot_log_start_time_marker
*
* @param[in] message  log message
*
* @par Dependencies
*  boot_logger_timer
*  boot_logger_ram
*   
* @retval
*   delta time
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_stop_time_marker(uint32 handle, char *message)
{
  return boot_log_stop_time_marker_optional_data(handle,
                                                 message,
                                                 NULL);
}
/* boot_log_stop_time_marker*/

/*===========================================================================

**  Function :  boot_log_stop_time_marker_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will substract the current timestamp with the previus stored 
*  timestamp for a time marker specified by handle to get the delta time 
*  value and then prefix the delta time to the log message.
*
* @param[in] handle  handle to time marker returned by 
*                    boot_log_start_time_marker
*
* @param[in] message  log message
*
* @param[in] optional_data  Optional data that can be added to entry
*
* @par Dependencies
*  boot_logger_timer
*  boot_logger_ram
*   
* @retval
*   delta time
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_stop_time_marker_optional_data(uint32 handle,
                                               char *message, 
                                               char *optional_data)
{
  /*Logs message and time into ram.*/
  uint32 delta_time = boot_stop_timer(handle);

  /* Logs message with time stamp in ram. */
  boot_log_message_ram(message,
                       delta_time,
                       LOG_MSG_TYPE_DELTA,
                       optional_data);

  /* Transmit the message with time stamp */
  boot_log_message_uart(message,
                        delta_time,
                        LOG_MSG_TYPE_DELTA,
                        optional_data);

  return delta_time;
}
/* boot_log_stop_time_marker_optional_data */
 
/*===========================================================================

**  Function :  boot_log_dump

** ==========================================================================
*/
/*!
* 
* @brief
* This function will dump the logged messages to various outputs.  Currently, 
* it only supports JTAG.  It may be expanded to support other methods of displaying,
* such as UART, etc.
*
* @param[in] init_data  boot logger intialization data which contains boot
*                       logger's buffer address
*
* @par Dependencies
*  boot_logger_jtag
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_dump(boot_log_init_data *init_data)
{
  /*Dumps RAM buffer to JTAG terminal*/
  boot_log_jtag_dump_buffer(init_data->log_buffer_start, init_data->log_buffer_size);

  /* Dumps RAM buffer to UART if enabled */
  boot_log_ram_to_uart();
}
/*boog_log_dump*/


 
/*===========================================================================

**  Function :  boot_log_forever_loop_check

** ==========================================================================
*/
/*!
* 
* @brief
* This function will loop infinitely to enable ram dump extraction from memory. 
* it checks a memory location for magic number value. If desired flag value found
* it loops infinitely so as to enable running Only till end of bootloaders.
* The magic number being checked by the api is set from a T32 cmm script that
* collects bootup times [boot_debug.cmm]
*
* @param[in]  loop_flag_value  Value from imem memory location to be checked.                                
*
* @par Dependencies
*  None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_forever_loop_check( uint32 loop_flag_value)
{

  /* Disable the UART logger to save power */
  boot_log_deinit_uart();
   
  /* if magic number set to run, only till end of bootloaders
   to collect boot-time, then loop forever */
  if( loop_flag_value == BOOT_LOGGER_LOOP_MAGIC_NUM )
  {
    INFINITE_LOOP();
  }
}
/*boot_log_forever_loop_check*/



/*===========================================================================

**  Function :  boot_log_get_time

** ==========================================================================
*/
/*!
* 
* @brief
* This will return the current timestamp which is the current absolute time 
*  subtracts the reference time. 
*
* @par Dependencies
*  boot_logger_timer
*   
* @retval
*   current timestamp, LOG_TIMESTAMP_OVERFLOW if overflow happens
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_get_time()
{
  return boot_get_time();
}
/*boot_log_get_time*/


/*===========================================================================

**  Function :  boot_log_delta_time

** ==========================================================================
*/
/*!
* 
* @brief
* This will log the delta time, the time between the old_time that was given
* and the time this function is executed.  It will also return this delta time. 
*
* @param[in] old_time          previous time reference point 
*
* @param[in] message           log message
*
* @par Dependencies
*  boot_logger_timer
*   
* @retval
*   delta time between the old_time that was given
*   and the time this function is executed.
*   LOG_TIMESTAMP_OVERFLOW if overflow happens
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_delta_time(uint32 old_time, 
                           char * message)
{
  /* Call boot_log_delta_time_optional_data so there isn't redundant logic. */
  return boot_log_delta_time_optional_data(old_time,
                                           message,
                                           NULL);
}
/*boot_log_delta_time*/


/*===========================================================================

**  Function :  boot_log_delta_time_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
* This will log the delta time, the time between the old_time that was given
* and the time this function is executed.  It will also return this delta time. 
*
* @param[in] old_time          previous time reference point 
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*  boot_logger_timer
*   
* @retval
*   delta time between the old_time that was given
*   and the time this function is executed.
*   LOG_TIMESTAMP_OVERFLOW if overflow happens
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_delta_time_optional_data(uint32 old_time, 
                                         char * message,
                                         char * optional_data)
{
  uint32 delta = boot_get_delta_time(old_time);

  boot_log_message_ram(message,
                       delta,
                       LOG_MSG_TYPE_DELTA,
                       optional_data);

  boot_log_message_uart(message,
                        delta,
                        LOG_MSG_TYPE_DELTA,
                        optional_data);

  return delta;
}
/*boot_log_delta_time_optional_data */


/*===========================================================================

**  Function :  boot_log_relocate

** ==========================================================================
*/
/*!
* 
* @brief
* Copy the log and meta info in one memory location to some other 
* memory location and continue to save logs at that location.
*
* @param[in] dst_meta_info_addr  destination address to save meta info
*
* @param[in] dst_log_buf_addr  destination address to save message log
*
* @param[in] dst_log_buf_size  size of the destination message log buf size
*
* @param[in] dst_time_marker_buf_addr  destination address to save time markers
*
* @param[in] dst_time_marker_buf_size  size of the destination time marker 
*                                      buffer
*
* @param[in] src_meta_info_addr  source address to read meta info
*
* @param[in] src_log_buf_addr  source address to read message log
*
* @param[in] src_log_buf_size  size of the source message log buf size
*
* @param[in] src_time_marker_buf_addr  source address to read time markers
*
* @param[in] src_time_marker_buf_size  size of the source time marker buffer
*
* @par Dependencies
*  boot_logger_ram
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_relocate
(
  void *dst_meta_info_addr,
  void *dst_log_buf_addr,
  uint32 dst_log_buf_size,
  void *dst_time_marker_buf_addr,
  uint32 dst_time_marker_buf_size,
  void *src_meta_info_addr,
  void *src_log_buf_addr,
  uint32 src_log_buf_size,
  void *src_time_marker_buf_addr,
  uint32 src_time_marker_buf_size
)
{
  
  boot_log_copy_ram(dst_meta_info_addr,
                    dst_log_buf_addr,
                    dst_log_buf_size,
                    src_meta_info_addr,
                    src_log_buf_addr,
                    src_log_buf_size);

  boot_copy_time_markers(dst_meta_info_addr,
                         dst_time_marker_buf_addr,
                         dst_time_marker_buf_size,
                         src_meta_info_addr,
                         src_time_marker_buf_addr,
                         src_time_marker_buf_size);
                    
  boot_log_set_meta_info(dst_meta_info_addr);
}
/* boot_log_relocate */


/*===========================================================================

**  Function : boot_log_ddr_frequency

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the ddr frequency.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_ddr_frequency(void)
{
  uint32 ddr_frequency_mhz = 0;

  /* boot_clock_get_ddr_speed returns DDR frequency in KHz, convert to Mhz
     and use. */
  ddr_frequency_mhz = boot_clock_get_ddr_speed() / 1000;

  snprintf(bootlog_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "DDR Frequency, %d MHz",
           ddr_frequency_mhz);

  /* Add entry to beginning of log */
  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}

/*===========================================================================

**  Function : boot_log_pbl_version

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the pbl patch version.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_pbl_version(void)
{
  uint32 pbl_patch_ver = 0;

  /* reads the pbl patch version register */
  pbl_patch_ver = HWIO_FEATURE_CONFIG8_INM(HWIO_FEATURE_CONFIG8_APPS_PBL_PATCH_VERSION_BMSK)>>HWIO_FEATURE_CONFIG8_APPS_PBL_PATCH_VERSION_SHFT;

  snprintf(bootlog_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "PBL Patch Ver: %d",
           pbl_patch_ver);

  /* Add entry to beginning of log */
  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}

/*===========================================================================

**  Function : boot_log_pbl_clk_freq

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the pbl clock frequency.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_pbl_clk_freq(void)
{
  uint32 pbl_clk_freq = 0;
  
  pbl_clk_freq =boot_pbl_get_clock_speed();

  snprintf(bootlog_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "PBL freq: %d MHZ",
           pbl_clk_freq/1000);

  /* Add entry to beginning of log */
  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}

/*===========================================================================

**  Function : boot_log_image_version

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the SBL image version.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_image_version(void)
{
  /* Log QC Image Version.  Truncate to BOOT_LOG_TEMP_BUFFER_SIZE */
  snprintf(bootlog_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "%s",
           QC_IMAGE_VERSION_STRING_AUTO_UPDATED);

  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);


  /* Log Image Variant.  Truncate to BOOT_LOG_TEMP_BUFFER_SIZE */
  snprintf(bootlog_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "%s",
           IMAGE_VARIANT_STRING_AUTO_UPDATED);

  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);


  /* Log OEM Image Version. Truncate to BOOT_LOG_TEMP_BUFFER_SIZE */
  snprintf(bootlog_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "%s",
           OEM_IMAGE_VERSION_STRING_AUTO_UPDATED);

  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}


/*===========================================================================

**  Function : boot_log_apps_frequency

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the clock frequency of apps core specified.
*
* @par
*   core - Core number to log frequency of.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_apps_frequency(uint32 core)
{
  uint32 core_frequency_mhz = 0;

  /* boot_clock_get_core_speed returns processor frequency in KHz,
     convert to Mhz and use. */
  core_frequency_mhz = boot_clock_get_apps_speed(core) / 1000;

  snprintf(bootlog_buffer,
           BOOT_LOG_TEMP_BUFFER_SIZE,
           "Core 0 Frequency, %d MHz",
           core_frequency_mhz);

  /* Add entry to beginning of log */
  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}


/*===========================================================================

**  Function : boot_log_boot_device_type

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the boot device type.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_boot_device_type(void)
{
  /* Get the boot device type */
  boot_flash_type boot_device = boot_pbl_get_flash_type();


  /* Write the preamble */
  strlcpy(bootlog_buffer,
          "Boot Interface: ",
          BOOT_LOG_TEMP_BUFFER_SIZE);


  switch (boot_device)
  {
    case NAND_FLASH:
      strlcat(bootlog_buffer,
              "NAND",
              BOOT_LOG_TEMP_BUFFER_SIZE);
      break;

    case MMC_FLASH:
      strlcat(bootlog_buffer,
              "eMMC",
              BOOT_LOG_TEMP_BUFFER_SIZE);
      break;

    case UFS_FLASH:
      strlcat(bootlog_buffer,
              "UFS",
              BOOT_LOG_TEMP_BUFFER_SIZE);
      break;

    case SPI_FLASH:
      strlcat(bootlog_buffer,
              "SPI",
              BOOT_LOG_TEMP_BUFFER_SIZE);
      break;

    case NO_FLASH:
      strlcat(bootlog_buffer,
              "None",
              BOOT_LOG_TEMP_BUFFER_SIZE);
      break;

    default:
      strlcat(bootlog_buffer,
              "Unknown",
              BOOT_LOG_TEMP_BUFFER_SIZE);
  }


  /* Add entry to beginning of log for boot device type */
  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}


/*===========================================================================

**  Function : boot_log_secboot_status

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the secboot status.
*
* @par Dependencies
*   Can only be called after boot_secboot_ftbl_init.
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_secboot_status(void)
{
  boot_boolean auth_enabled = FALSE;
  secboot_hw_etype status = E_SECBOOT_HW_FAILURE;


  /* Get the secboot enabled status. */
  status = boot_is_auth_enabled(&auth_enabled);
  BL_VERIFY((status == E_SECBOOT_HW_SUCCESS), (uint16)status|BL_ERROR_GROUP_SECURITY); 


  /* Write the preamble */
  strlcpy(bootlog_buffer,
          "Secure Boot: ",
          BOOT_LOG_TEMP_BUFFER_SIZE);

  if (auth_enabled)
  {
    strlcat(bootlog_buffer,
            "On",
            BOOT_LOG_TEMP_BUFFER_SIZE);
  }
  else
  {
    strlcat(bootlog_buffer,
            "Off",
            BOOT_LOG_TEMP_BUFFER_SIZE);
  }


  /* Add entry to beginning of log for boot device type */
  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);
}


/*===========================================================================

**  Function : boot_log_cache_status

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the I and D cache status.
*
* @par Dependencies
*   Can only be called after boot_logger_init.
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_cache_status(void)
{ 
  /* Write the preamble */
  strlcpy(bootlog_buffer,
          "I-cache: ",
          BOOT_LOG_TEMP_BUFFER_SIZE);

  strlcat(bootlog_buffer,
          (boot_is_icache_enabled() ? "On":"Off"),
          BOOT_LOG_TEMP_BUFFER_SIZE);


  /* Add entry to beginning of log for D-cache status */
  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);

  /* Write the preamble */
  strlcpy(bootlog_buffer,
          "D-cache: ",
          BOOT_LOG_TEMP_BUFFER_SIZE);

  strlcat(bootlog_buffer,
          (boot_is_dcache_enabled() ? "On":"Off"),
          BOOT_LOG_TEMP_BUFFER_SIZE);

  /* Add entry to beginning of log for I-cache status */
  boot_log_message_raw(bootlog_buffer,
                       BOOT_DND_TIMESTAMP,
                       LOG_MSG_TYPE_STATISTIC,
                       NULL);	   					   
}


/*===========================================================================

**  Function : boot_log_raw_fuse_values

** ==========================================================================
*/
/*!
* 
* @brief  
*   Logs the raw fuse values useful for customer debugging.
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_raw_fuse_values(void)
{
  boot_boolean auth_enabled = FALSE;
  secboot_hw_etype status = E_SECBOOT_HW_FAILURE;
  uint32 * current_fuse = NULL;
  uint64 * current_fuse_64 = NULL;
  uint32 count = 0;


  /* Get the secboot enabled status. */
  status = boot_is_auth_enabled(&auth_enabled);
  BL_VERIFY((status == E_SECBOOT_HW_SUCCESS), (uint16)status|BL_ERROR_GROUP_SECURITY); 


  while (TRUE)
  {
    /* Check for end of array */
    if (boot_log_raw_fuse_entries[count].fuse_address == 0)
    {
      break;
    }

    /* Entry is only to be printed if secboot_protected is false or secboot is disabled. */
    if ((boot_log_raw_fuse_entries[count].secboot_protected == FALSE) ||
        (auth_enabled == FALSE))
    {
    
      current_fuse_64 = (uint64 *)(uintnt)boot_log_raw_fuse_entries[count].fuse_address;
      current_fuse = (uint32 *)(uintnt)boot_log_raw_fuse_entries[count].fuse_address;

	   if(boot_log_raw_fuse_entries[count].full_row == TRUE)
      {
        snprintf(bootlog_buffer,
               BOOT_LOG_TEMP_BUFFER_SIZE,
               "%s @ 0x%08x = 0x%016llx",
               boot_log_raw_fuse_entries[count].fuse_name,
               boot_log_raw_fuse_entries[count].fuse_address,
               *current_fuse_64);
      }
      else
      {
      snprintf(bootlog_buffer,
               BOOT_LOG_TEMP_BUFFER_SIZE,
               "%s @ 0x%08x = 0x%08x",
               boot_log_raw_fuse_entries[count].fuse_name,
               boot_log_raw_fuse_entries[count].fuse_address,
               *current_fuse);
	  }		   
      
      boot_log_message_raw(bootlog_buffer,
                           BOOT_DND_TIMESTAMP,
                           LOG_MSG_TYPE_STATISTIC,
                           NULL);
    }

    count++;
  } 
}

/*===========================================================================

**  Function : boot_log_set_init_info_marker

** ==========================================================================
*/
/*!
* 
* @brief  
*   Sets the variables to enable init log info to be retrieved later.
*
* @par Dependencies
*   Must be called after or at end of boot_log_init
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
void boot_log_set_init_info_marker(void)
{
  BL_VERIFY((log_meta_info_ptr != NULL),
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT); 
            
  boot_log_init_info_size = 0;  
  if( log_meta_info_ptr->log_buf_init == TRUE && 
      log_meta_info_ptr->log_buf_ptr > log_meta_info_ptr->log_buf_start)
  {      
    boot_log_init_info_size = (log_meta_info_ptr->log_buf_ptr - log_meta_info_ptr->log_buf_start);
    boot_log_init_info_ptr = log_meta_info_ptr->log_buf_start ;
  }            
}

/*===========================================================================

**  Function : boot_log_get_init_info

** ==========================================================================
*/
/*!
* 
* @brief  
*   Function returns the base address and size of init boot logs.
*
* @par Dependencies
*   Must be called after boot_log_init.
*
* @retval
*   None
* 
* @par Side Effects
*   None
*  
*/
boolean boot_log_get_init_info(uint8 **log_base_addr_ptr, uintnt *size_ptr)
{
  boolean status = FALSE;
  BL_VERIFY(((log_base_addr_ptr != NULL) && (size_ptr != NULL)),
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT); 
  if(boot_log_init_info_size > 0)
  {
    *log_base_addr_ptr = boot_log_init_info_ptr;
    *size_ptr = boot_log_init_info_size;
    status = TRUE;
  }
  
  return status;
}

#ifndef BOOT_LOGGER_H
#define BOOT_LOGGER_H

/*===========================================================================

                                boot logger 
                                Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for Boot Logger
  functionality.
  Time unit in boot logger is microsecond(10^-6 second).

Copyright 2010-2012, 2014-2017 by Qualcomm Technologies, Incorporated.  
All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who          what, where, why
--------   --------     ------------------------------------------------------
04/24/17   ds           boot_log_pbl_clk_freq
01/30/17   ds           boot_log_pbl_version
06/15/16   kpa          Added boot_log_set_init_info boot_log_get_init_info
08/20/15   elt          Added boot_log_start_time_marker, boot_log_stop_time_marker,
                        boot_log_stop_time_marker_optional_data, and modified
                        data structures for supporting independent time markers
08/18/15   elt          Added BOOT_LOGGER_ASYNC_UART_MAGIC_NUM
06/24/15   ck           Added boot_log_raw_fuse_values
04/07/15   plc          Added note to BOOT_LOG_TEMP_BUFFER_SIZE definition
06/10/14   ck           Added boot_log_ apps_frequency, image_version, ddr_frequency,
                        boot_config_register, delta_time_optional_data,
                        stop_timer_option_data, and message_optional_data
06/02/14   ck           Added BOOT_LOG_TEMP_BUFFER_SIZE definition
06/02/14   ck           Moved defines from boot_logger.c to here
06/02/14   ck           Added boot_log_message_raw
08/30/12   dh           Change boot_log_copy to boot_log_relocate
05/10/12   dh           Change boot_log_set_ref_time to take an argument as reference time
06/21/11   kpa          Add boot_log_forever_loop_check api
11/10/10   dh           Add boot_log_copy api
09/27/10   dh           Add meta info structure for internal bookkeeping
08/11/10   dh           Initial port from 7x30 
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"

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
* Log message type(char) : statistical message
*/
#define LOG_MSG_TYPE_STATISTIC 'S'

/**
* Infinite loop magic number
*/
#define BOOT_LOGGER_LOOP_MAGIC_NUM 0xD1C2B3A3

/**
* Asynchronous uart logging magic number
*/
#define BOOT_LOGGER_ASYNC_UART_MAGIC_NUM 0xE1D50C10

/**
* return value when timestamp overflow happens
*/
#define LOG_TIMESTAMP_OVERFLOW  0xFFFFFFFF

/**
* Value indicating logger should not write timestamp to entry
*/
#define BOOT_DND_TIMESTAMP 0xFFFFFFFF

/**
* time marker handle returned to indicate no available time markers
*/
#define LOG_TIME_MARKER_OVERFLOW 0xFFFFFFFF

/**
* time marker handle for implementing base timer
*/
#define LOG_BASE_TIME_MARKER 0

/**
 * Size of temp buffer available for creating strings
 * Please note, buffers are generally placed on stack so use caution if 
 * adjusting BOOT_LOG_TEMP_BUFFER_SIZE value
 */
#define BOOT_LOG_TEMP_BUFFER_SIZE 96

/**
 * Structure that holds an entry in the boot time marker buffer.
 * Includes a start time value as well as a pointer to next free time marker
 * buffer entry.
 */
typedef struct boot_time_marker_entry_t
{
  /**
   * Start time this entry tracks
   */
  uint32 start_time;

  /**
   * Pointer to next free entry in time marker buffer. NULL if none exists
   */
  struct boot_time_marker_entry_t *next_free_entry;

}boot_time_marker_entry;


/**
 * This struct defines the structure of boot logger's meta info, which keeps
 * track of the logger's internal states.
 * It contains information about current logger's logging buffer and timer.
 * The size of this structure must be smaller than SCL_BOOT_LOG_META_INFO_SIZE
 */
struct boot_log_meta_info
{
  /**
   *pointer that points to beginning of the logging buffer
   */
  uint8  *log_buf_start;
  
  /**
   *pointer that points to the next empty byte in the logging buffer
   */
  uint8  *log_buf_ptr;
  
  /**
   * total size of the logging buffer in bytes
   */
  uint32 log_buf_size;
  
  /**
   *status of logging buffer, initialized(TRUE) or uninitialized(FALSE)
   */
  uint32 log_buf_init;
  
  /**
  * stores the timestamp which serves as a reference point
  */
  uint32 ref_time;
  
  /**
   * pointer to the beginning of the time marker buffer 
   */
  boot_time_marker_entry *time_marker_buf_start;

  /**
   * pointer to next free entry in time marker buffer
   */
  boot_time_marker_entry *time_marker_buf_ptr;

  /**
   * total size of time marker buffer in bytes
   */
  uint32 time_marker_buf_size;
  
  /**
   *the status of time marker buffer, initialized(TRUE) or uninitialized(FALSE)
   */
  uint32 time_marker_buf_init;
};


/**
 * Structure that holds the start address/size of 
 * the boot logger's logging buffer and meta data.
 * This structure can be expanded to store other information.
 * log_buffer_start and meta_info_start must be assigned to 
 * valid addresses prior to use boot logger.
 * boot_log_init must be called before using any
 * boot logger function.
 */
typedef struct boot_log_init_data_t
{
  /**
  * Start address of the current logging buffer 
  */
  void *log_buffer_start;
  
  /**
  * Size of the current logging buffer 
  */
  uint32 log_buffer_size;
  
  /**
  * Start address of the current time marker buffer
  */
  void *time_marker_buffer_start;

  /**
  * Size of the the current time marker buffer
  */
  uint32 time_marker_buffer_size;

  /**
  * Start address of the current meta info structure
  */
  void *meta_info_start;
  
  /**
  * Size of the current meta info structure
  */
  uint32 meta_info_size;
  
 /**
  * Start address of the old meta info structure.
  * It is used to retrieve reference time from the old boot logger
  * Set this value to NULL will default the reference time of current logger
  * to 0.
  */
  void *old_meta_info_start;
  
}boot_log_init_data;


/* Use the common bootlog_buffer declared in boot_logger.c */
extern char bootlog_buffer[BOOT_LOG_TEMP_BUFFER_SIZE];


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

**  Function :  boot_log_set_meta_info

** ==========================================================================
*/
/*!
* 
* @brief
*  Boot Logger uses a global pointer to store the address of meta info structure.
*  In the circumstances of any code initializes RW section of the load image 
*  after the boot logger is initialized, this function should be called to 
*  restore the meta info address that is kept in RW section of boot logger module.
*  By doing this we can log the time takes to initilize RW section of the image.
*
* @param[in] addr start address of this boot logger's meta info structure
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
void boot_log_set_meta_info(void *addr);


/*===========================================================================

**  Function :  boot_log_get_meta_info

** ==========================================================================
*/
/*!
* 
* @brief
*  This function returns a pointer to the meta info structure used by current logger.
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
struct boot_log_meta_info* boot_log_get_meta_info(void);


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
void boot_log_init(boot_log_init_data *init_data);


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

void boot_log_set_ref_time(uint32 ref_time);


/*===========================================================================

**  Function :  boot_log_message

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message.It will prefix a timestamp, which is the current 
*  absolute time minus the reference time, to the message string.
*
* @param[in] message  log message
*
* @par Dependencies
*   boot_logger_timer
*   boot_logger_ram
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_message(char *);


/*===========================================================================

**  Function :  boot_log_message_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
*  Logs the given message.It will prefix a timestamp, which is the current 
*  absolute time minus the reference time, to the message string.
*
* @param[in] message           log message
*
* @param[in] optional_data     Optional data that can be added to entry
*
* @par Dependencies
*   boot_logger_timer
*   boot_logger_ram
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_log_message_optional_data(char *,
                                    char *);


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
void boot_log_message_raw(char *,
                          uint32,
                          char,
                          char *);


/*===========================================================================

**  Function :  boot_log_start_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will start the base time marker and mark the beginning of the 
*  timed section and store the current timestamp internally.
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
void boot_log_start_timer(void);

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
*   Handle to created time marker. Returns LOG_TIME_MARKER_OVERFLOW if space is 
*   not available for creating a new timer.
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_start_time_marker(void);

/*===========================================================================

**  Function :  boot_log_stop_timer

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will substract the current timestamp with the previus stored 
*  timestamp by boot_log_start_timer(if any) to get the delta time value and 
*  then prefix the delta time to the log message.
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
uint32 boot_log_stop_timer(char *);


/*===========================================================================

**  Function :  boot_log_stop_timer_optional_data

** ==========================================================================
*/
/*!
* 
* @brief
*  This funcion will substract the current timestamp with the previus stored 
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
uint32 boot_log_stop_timer_optional_data(char *,
                                         char *);

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
uint32 boot_log_stop_time_marker(uint32, char *);


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
uint32 boot_log_stop_time_marker_optional_data(uint32, char *, char *);

/*===========================================================================

**  Function :  boot_log_dump

** ==========================================================================
*/
/*!
* 
* @brief
* This function will dump the logged messages to various outputs.  Currently, 
* it only supports JTAG.It may be expanded to support other methods of displaying,
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
void boot_log_dump(boot_log_init_data *);

/*===========================================================================

**  Function :  boot_log_get_time

** ==========================================================================
*/
/*!
* 
* @brief
* This will return the current timestamp which is the absolute time since boot
* subtracts the reference time.
*
* @par Dependencies
*  boot_logger_timer
*   
* @retval
*   current timestamp minus the reference time, LOG_TIMESTAMP_OVERFLOW 
*   if overflow happens
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_log_get_time(void);


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
uint32 boot_log_delta_time(uint32,
                           char *);


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
uint32 boot_log_delta_time_optional_data(uint32,
                                         char *,
                                         char *);


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
);

 
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
void boot_log_forever_loop_check( uint32 loop_flag_value);


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
void boot_log_ddr_frequency(void);

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
void boot_log_pbl_version(void);

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
void boot_log_pbl_clk_freq(void);

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
void boot_log_image_version(void);


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
void boot_log_apps_frequency(uint32);


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
void boot_log_boot_device_type(void);


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
void boot_log_secboot_status(void);


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
void boot_log_cache_status(void);


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
void boot_log_raw_fuse_values(void);

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
void boot_log_set_init_info_marker(void);

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
boolean boot_log_get_init_info(uint8 **log_base_addr_ptr, uintnt *size_ptr);

#endif  /* BOOT_LOGGER_H */

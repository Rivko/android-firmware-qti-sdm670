/*=============================================================================

                            boot logger ram

GENERAL DESCRIPTION
  This file contains the function interface for logging messages during boot
  to the interal buffer.

Copyright 2010-2012, 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  

when       who        what, where, why
--------   ---        ---------------------------------------------------------
08/20/14   kedara     Fix compilation warnings.
05/28/14   ck         Added optional data argument to boot_log_message_ram
05/28/14   ck         Updated header for new bootlogger layout
08/30/12   dh         add a size argument to boot_log_copy_ram
05/02/12   dh         change all local char array to global static variable
02/14/11   dh         fix klocwork warning
11/10/10   dh         Add boot_log_copy_ram api
09/27/10   dh         Add meta info structure to boot logger
08/11/10   dh         Initial port from 7x30 
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_util.h"
#include "boot_logger.h"
#include "boot_logger_ram.h"
#include "boot_error_if.h"

/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/

/**
 * Header and formating info.
 * Used to centralize the formatting strings so that if a change was made to
 * these it would propogate throughout the logger module correctly.
 */
static char LEGEND_HEADER[] = "Format: Log Type - Time(microsec) - Message - Optional Info";
static char LEGEND_KEY[]    = "Log Type: B - Since Boot(Power On Reset),  D - Delta,  S - Statistic";
static char FORMATTING_STRING_1[] = " - ";
static char OVERFLOW_MSG[] = " OVERFLOW";
static char NEWLINE[] = "\r\n";

/*Buffer to store timer digit*/
static char ascii_char[LOG_MAX_NUM_DIGIT_DISPLAY];

#define HEADER_LEN              (sizeof(LEGEND_HEADER) -1)
#define KEY_LEN                 (sizeof(LEGEND_KEY) -1)
#define FORMATTING_STRING_1_LEN (sizeof(FORMATTING_STRING_1) -1)
#define OVERFLOW_MSG_LEN        (sizeof(OVERFLOW_MSG) -1)
#define NEWLINE_LEN             (sizeof(NEWLINE) -1)

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  boot_log_ram_put_string

** ==========================================================================
*/
/*!
* 
* @brief
* This function will copy string with given length to internal buffer.
* It does not care if the string is NULL terminated or not, only blindly copy
* "len" bytes of chars to the buffer.
*
* @param[in] str  pointer to the start of string
*
* @param[in] len  length of the string in bytes
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
static void boot_log_ram_put_string(char *str, uint32 len)
{
  struct boot_log_meta_info* log_meta_info_ptr = boot_log_get_meta_info();
  qmemcpy(log_meta_info_ptr->log_buf_ptr, str, len);
  log_meta_info_ptr->log_buf_ptr += len;
}
/*boot_log_ram_put_string*/


/*===========================================================================

**  Function :  boot_log_ram_put_time

** ==========================================================================
*/
/*!
* 
* @brief
* This function will take a uint32 timestamp and turn it into a decimal
* ASCII number.  Then it will write it to the buffer.
 
* Note:  It will cap out at 999 seconds (16 minutes, 39 seconds).
* This cap was chosen because it should never be maxed out during boot
* and by keeping it shorter it saves space in the buffer.  The size
* can be changed by adjusting LOG_MAX_NUM_DIGIT_DISPLAY to the desired
* number of decimal digits.
*
* @param[in] timestamp  32bits timestamp
*
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
static void boot_log_ram_put_time(uint32 timestamp)
{
  /*Limits the number of decimals that the ASCII number is represented as. 
    In this case it can represent 999 seconds (999,999,999 microseconds).  */
  int ascii_char_len = LOG_MAX_NUM_DIGIT_DISPLAY;
  int i;
  
  /*check if timestamp is overflowed*/
  if(timestamp < LOG_MAX_TIMESTAMP_VAL)
  {
    /*Determine decimal ASCII string, starting 
      with the least siginificant digits.*/
    for(i = (ascii_char_len - 1); i >=0 ; i--)
    {
      ascii_char[i] = (char) (timestamp % 10) + '0';
      timestamp /= 10;
    }
    /*Replaced leading zeros with a space, leaving a single 0 if timestamp==0*/
    for(i = 0; ascii_char[i] == '0' && i <= (ascii_char_len - 2); i++)
    {
      ascii_char[i] = ' ';
    }
    
    boot_log_ram_put_string(ascii_char,ascii_char_len);
  }
  else
  {
    boot_log_ram_put_string(OVERFLOW_MSG, OVERFLOW_MSG_LEN);
  }
}
/*boot_log_ram_puttime*/


/*===========================================================================

**  Function :  boot_log_init_ram

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will initiate the logging buffer and meta info in ram.
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
void boot_log_init_ram(struct boot_log_init_data_t *init_data)
{ 
  uint32 init_str_len = HEADER_LEN + KEY_LEN + (3 * NEWLINE_LEN);
  struct boot_log_meta_info* log_meta_info_ptr = boot_log_get_meta_info();
    
  /*Ensure that there is enough space to copy header */
  if(log_meta_info_ptr != NULL && init_data->log_buffer_size > init_str_len) 
  {
    /*set meta data of the logging buffer*/
    log_meta_info_ptr->log_buf_size = init_data->log_buffer_size;
    log_meta_info_ptr->log_buf_start = init_data->log_buffer_start;
    log_meta_info_ptr->log_buf_ptr = init_data->log_buffer_start;
    log_meta_info_ptr->log_buf_init = TRUE;
    
    /*Zero the buffer*/
    qmemset(log_meta_info_ptr->log_buf_ptr, 0x0, init_data->log_buffer_size);
    
    /*Writes out the legend information to explain output formatting*/
    boot_log_ram_put_string(NEWLINE,NEWLINE_LEN);
    boot_log_ram_put_string(LEGEND_HEADER,HEADER_LEN);
    boot_log_ram_put_string(NEWLINE,NEWLINE_LEN);
    boot_log_ram_put_string(LEGEND_KEY,KEY_LEN);
    boot_log_ram_put_string(NEWLINE,NEWLINE_LEN);
  }
}
/* boot_log_init_ram*/


/*===========================================================================

**  Function :  boot_log_message_ram

** ==========================================================================
*/
/*!
* 
* @brief
* This funcion will copy the given message to logging buffer and prefix the
* message by a given timestamp.
* The log_type is generally used to show what type of timing is used, but
* it could technically be used for other type of info.
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
void boot_log_message_ram(char * message,
                          uint32 timestamp,
                          char log_type,
                          char * optional_data)
{
  struct boot_log_meta_info* log_meta_info_ptr = boot_log_get_meta_info();
  uint32 message_len = 0;
  uint32 optional_data_len = 0;
  uint32 total_message_len = 0;
  

  /* Calculate message and optional data lengths. */
  if(message != NULL)
  {
    message_len = boot_strlen(message);
  }

  if (optional_data != NULL)
  {
    optional_data_len = boot_strlen(optional_data);
  }

  
  /* Sum up the total length of the output to the buffer */
  if (timestamp != BOOT_DND_TIMESTAMP)
  {
    total_message_len += LOG_MAX_NUM_DIGIT_DISPLAY + FORMATTING_STRING_1_LEN;
  }

  total_message_len += sizeof(log_type)
                    + FORMATTING_STRING_1_LEN
                    + message_len
                    + NEWLINE_LEN;            

  /* If optional data exists then add to total message length. */
  if (optional_data != NULL)
  {
    total_message_len += optional_data_len
                      + FORMATTING_STRING_1_LEN;
  }
  
                         
  /* Buffer must be initialized. */
  if(log_meta_info_ptr != NULL && log_meta_info_ptr->log_buf_init == TRUE) 
  {
    /*If not enough spaces remain in the buffer*/
    if((log_meta_info_ptr->log_buf_ptr   - 
        log_meta_info_ptr->log_buf_start +
        total_message_len)               >= 
        log_meta_info_ptr->log_buf_size) 
    {
      /*Reset pointer to the beginning, overwrite the existing log*/
      log_meta_info_ptr->log_buf_ptr = log_meta_info_ptr->log_buf_start;
    }
     
    /* Copy log_type */
    boot_log_ram_put_string(&log_type,
                            sizeof(log_type));
    
    /* Formatting string, for readability */
    boot_log_ram_put_string(FORMATTING_STRING_1,
                            FORMATTING_STRING_1_LEN);
    
    if (timestamp != BOOT_DND_TIMESTAMP)
    {
      /* Copy timestamp */
      boot_log_ram_put_time(timestamp);

      /* Formatting string, for readability */
      boot_log_ram_put_string(FORMATTING_STRING_1,
                              FORMATTING_STRING_1_LEN);
    }

    /*Write out message to ram, checks for null ptr*/
    if(NULL != message) 
    {
      boot_log_ram_put_string(message,
                              message_len);
    }

    /* Write out optional data if applicable. */
    if (optional_data != NULL)
    {
      boot_log_ram_put_string(FORMATTING_STRING_1,
                              FORMATTING_STRING_1_LEN);

      boot_log_ram_put_string(optional_data,
                              optional_data_len);
    }

    boot_log_ram_put_string(NEWLINE,
                            NEWLINE_LEN);
  }
}
/* boot_log_message_ram */


/*===========================================================================

**  Function :  boot_log_copy_ram

** ==========================================================================
*/
/*!
* 
* @brief
* Copy the log and meta info in one memory location to some other 
* memory location. This can be used to save log between sbl
*
* @param[in] dst_meta_info_addr  destination address to save meta info
*
* @param[in] dst_log_buf_addr  destination address to save message log
*
* @param[in] src_meta_info_addr  source address to read meta info
*
* @param[in] src_log_buf_addr  source address to read message log
*
* @param[in] log_buf_size  size of the message log to copy in bytes
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
void boot_log_copy_ram
(
  void *dst_meta_info_addr,
  void *dst_log_buf_addr,
  uint32 dst_log_buf_size,  
  void *src_meta_info_addr,
  void *src_log_buf_addr,
  uint32 src_log_buf_size
)
{ 
  struct boot_log_meta_info *dst_meta_info_ptr = 
           (struct boot_log_meta_info *)dst_meta_info_addr;

  struct boot_log_meta_info *src_meta_info_ptr = 
           (struct boot_log_meta_info *)src_meta_info_addr;
           
  BL_VERIFY(dst_log_buf_addr   != NULL &&
            dst_meta_info_ptr != NULL &&
            src_log_buf_addr   != NULL &&
            src_meta_info_ptr != NULL  &&
            src_log_buf_size <= dst_log_buf_size,
            BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
  
  /*first we copy the source meta data info into the new location*/  
  qmemcpy(dst_meta_info_ptr, 
          src_meta_info_ptr, 
          sizeof(struct boot_log_meta_info));

  /*now copy the log message buffer to new location*/
  qmemcpy(dst_log_buf_addr, 
          src_log_buf_addr, 
          src_log_buf_size); 
  
  /* Zero initalize the rest of dst log buffer*/
  qmemset((void* )((uintnt)dst_log_buf_addr + src_log_buf_size),
          0, 
          dst_log_buf_size - src_log_buf_size);
  
  /*update the meta info to reflect the new size and start address*/
  dst_meta_info_ptr->log_buf_size = dst_log_buf_size;
                                                   
  dst_meta_info_ptr->log_buf_start = dst_log_buf_addr;
                                                   
  dst_meta_info_ptr->log_buf_ptr = (src_meta_info_ptr->log_buf_ptr - 
                                    src_meta_info_ptr->log_buf_start) + 
                                   dst_meta_info_ptr->log_buf_start;
   
  
}
/* boot_log_copy_ram */

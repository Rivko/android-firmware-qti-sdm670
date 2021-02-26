#ifndef __FLASH_PROG_H__
#define __FLASH_PROG_H__

/*==================================================================
 *
 * FILE:        flash_prog.h
 *
 * SERVICES:    Header for Common APIs used across all controllers
 *
 * DESCRIPTION: This header file contains the structures that are
 *              used to communicate between the clients that would
 *              like to use the functionality of the flash_prog
 *              flash programming tool.
 *
 * Copyright (c) 2008-2011,2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 04/04/13     eo      Add usb deinit support
 * 02/21/13     sv      Adding Erase NAND capability in EDL/QPST download
 * 08/02/11     sv      Add DSP3 support
 * 06/22/11     eo      Add SBL1,SBL2,RPM support
 * 09/02/10     sv      NOR tools support
 * 03/02/10     op      Added support OSBL, AMSS, DSP updated image, ADSP image
 * 10/02/09     mm      Added support for programming Images in BML Format
 * 03/20/09     jz      Moved the dispatch struct def from hostdl.h to flash_prog.h
 * 02/06/09     jz      Added support for SB2.0
 * 10/29/08     mm      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "comdef.h"
#include "flash_prog_error.h"


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Types of images that can be programmed */
typedef enum {
  FLASH_PROG_NO_IMG = 0,
  FLASH_PROG_QCSBLHD_CONFIGDAT_IMG,
  FLASH_PROG_QCSBL_IMG,
  FLASH_PROG_OEMSBL_IMG,
  FLASH_PROG_AMSS_IMG,
  FLASH_PROG_APPS_BOOT_IMG,
  FLASH_PROG_APPS_IMG,
  FLASH_PROG_FOTAUI_IMG,
  FLASH_PROG_MODEM_CEFS_IMG,
  FLASH_PROG_APPS_CEFS_IMG,
  FLASH_PROG_WM_IMG,
  FLASH_PROG_DSP1_IMG,
  FLASH_PROG_DSP2_IMG,
  FLASH_PROG_CUSTOM_IMG,
  FLASH_PROG_RAW_IMG,
  FLASH_PROG_FACTORY_IMG,
  FLASH_PROG_DBL_IMG,
  FLASH_PROG_FSBL_IMG,
  FLASH_PROG_OSBL_IMG,
  FLASH_PROG_ROFS1_IMG,
  FLASH_PROG_ROFS2_IMG,
  FLASH_PROG_ROFS3_IMG,
  FLASH_PROG_OSBL_UPDATE_IMG,			
  FLASH_PROG_AMSS_UPDATE_IMG,			
  FLASH_PROG_DSP_UPDATE_IMG, 			
  FLASH_PROG_ADSP_IMG,
  FLASH_PROG_SINGLE_IMG,
  FLASH_PROG_SBL1_IMG,
  FLASH_PROG_SBL2_IMG,
  FLASH_PROG_RPM_IMG,
  FLASH_PROG_DSP3_IMG,
  FLASH_PROG_UNKNOWN_IMG
  } flash_prog_img_type_t;

/* Bad Block Handling mode - Bad Block Skipping or Bad Block Replacement 
 * Qualcomm Using the Bad Block Skip Method
 * BML uses Bad Block Replacement Method 
 */
typedef enum {
  FLASH_PROG_BAD_BLOCK_SKIP,
  FLASH_PROG_BAD_BLOCK_REPLACE
}flash_prog_bad_block_mode_t;

/* Return code for type_flash_prog_flash_param.status variable. These values are
 * defined by TRACE-32 ICD and should not be changed. 
 */
typedef enum {
  FLASH_PROG_SUCCESS=0x0,
  FLASH_PROG_FAIL=0x100
} flash_prog_status_t;

/* Partition Table IDs */
typedef enum {
  FLASH_PROG_NO_PARTI=0,
  FLASH_PROG_USR_PARTI,
  FLASH_PROG_SYS_PARTI
}  flash_prog_parti_type_t;

typedef struct flash_parameter
{
  char flash_name[32];
  uint32 num_blocks;
  uint32 pages_in_block;
  uint32 page_size;
} flash_param_t;

#define FLASH_PROG_PARTI_NO_OVERRRIDE      0x0
#define FLASH_PROG_PARTI_OVERRRIDE         0x1


/*===========================================================================

                 STRUCTURE AND TYPE DEFINITIONS FOR MODULE

===========================================================================*/
/*
 *  Structure definition to hold function pointers for dispatching
 */
 typedef struct dispatch {
   boolean (*init)   (void);
   boolean (*active) (void);
   void    (*drain)  (void);
   void    (*check)  (void);
   void    (*xmit)   (byte chr);
   void    (*deinit) (void);
 } DISP, *DISPPTR;

/* This structure is used to communicate between clients and flash_prog flash
 * programing tool. For programming an image, clients will call functions
 * in this table to go through the complete process.
 */
typedef struct flash_prog_img_prg_table {

  /* Function to get name of the flash part */
  flash_prog_status_t (*get_flash_param)( void **device_params);

  /* Function to set security mode for programming */
  flash_prog_status_t (*set_security_mode)( uint32 sec_mode);

  /* Function to set security mode for programming */
  flash_prog_status_t (*init_partition_tbl)( void *parti_tbl, uint32 length,
                                       flash_prog_parti_type_t parti_tbl_type,
                                       uint32 override);

  /* Function to set which image to program */
  flash_prog_status_t (*set_image_type)( flash_prog_img_type_t img_type, void *hdr_data,
                                    uint32 hdr_len);

  /* Function to send the image data */

  /* This function should be called till all the data is transmitted. It can
   * called one or more times depending on how much image data needs to be sent
   * by the caller.
   */
  flash_prog_status_t (*program_img) (void *data, uint32 size, uint32 addr);

  /* Function to finalize and close the image */
  /* Called once only after the entire image has been sent */
  flash_prog_status_t (*finalize_img) (void);

  /* Function to get extended error code and message */
  flash_prog_status_t (*get_error_code)(fatal_code_type *errno, char **mesg);

  /* Function to erase the NAND */
  flash_prog_status_t (*erase_chip)(void);

}flash_prog_img_prg_table_t;

/* FLASH_PROG flash programmer external interface. This function table is used by
 * clients to access flash_prog core logic to program a particular image to the flash.
 */
extern flash_prog_img_prg_table_t flash_prog_function_tbl;


#endif  /* __FLASH_PROG_H__ */

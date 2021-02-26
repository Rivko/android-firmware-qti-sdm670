/*===========================================================================
                             cmd_db_aop_target.c

DESCRIPTION:
  Target specific functions for command DB in AOP image.

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//------------------- --------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include "cmd_db_int.h"
#include "cmd_db_aop_int.h"
#include "page_select.h"
#include "HALhwio.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

#define CMD_DB_PHYS_ADDR  (0x85FE0000)


//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Exeternal Declarations
//---------------------------------------------------------------------------
extern const uint16_t arc_db_headers_cnt;
extern const uint16_t arc_aux_size;
extern const uint8_t *arc_aux_ext;
extern const cmd_db_entry_header *arc_db_headers_ext;
extern const uint16_t bcm_db_headers_cnt;
extern const uint16_t bcm_aux_size;
extern const uint8_t *bcm_aux_ext;
extern const cmd_db_entry_header *bcm_db_headers_ext;


//===========================================================================
//                             Function Definitions
//===========================================================================
char *cmd_db_addr = NULL;

/**
 * <!-- cmd_db_target_aop_mem_init -->
 *
 * @brief Return target address for Command DB AOP data.
 *
 *  @return Command DB data address for success, NULL for
 *          failure.
 *
 */
char *cmd_db_target_aop_mem_init(void)
{
  char *ret_addr = NULL;
  cmd_db_addr = (char *)(CMD_DB_PHYS_ADDR);

  if (((void*)(cmd_db_addr)) != NULL)
  {
    /* Accessing SMEM region */
#ifdef CMD_DB_IN_DDR
    ret_addr = (char *)set_ddr_access((uint64_t)cmd_db_addr);
#else
    ret_addr = cmd_db_addr;
#endif
    memset((void*)ret_addr, 0x0, CMD_DB_SIZE);

#ifdef CMD_DB_IN_DDR
    set_page_select(0);
#endif
  }
  return ret_addr;
}

/**
 * <!-- cmd_db_aop_target_init -->
 *
 * @brief Init command DB target init from AOP.
 *
 *  @return CMD_DB_SUCCESS for success, otherwise
 *          cmd_db_err_type.
 */
cmd_db_err_type cmd_db_aop_target_init(void)
{
  cmd_db_err_type err = CMD_DB_SUCCESS;

  cmd_db_entry_header *aop_mem_space_entry_hdr = NULL;
  uint8_t *aop_mem_space_data = NULL;

  do
  {
#ifdef CMD_DB_IN_DDR
      aop_mem_space_entry_hdr =
        (cmd_db_entry_header *)set_ddr_access((uint64_t)arc_db_headers_ext);
      set_page_select(0);
      aop_mem_space_data = (uint8_t *)set_ddr_access((uint64_t)arc_aux_ext);
      set_page_select(0);
#else
      aop_mem_space_entry_hdr = (cmd_db_entry_header *)arc_db_headers_ext;
      aop_mem_space_data = (uint8_t *)arc_aux_ext;
#endif

      err = cmd_db_add_static_data(CMD_DB_SLV_ID_ARC, 0x1000,
                                   aop_mem_space_entry_hdr,
                                   arc_db_headers_cnt,
                                   aop_mem_space_data,
                                   arc_aux_size);
   if (err != CMD_DB_SUCCESS)
   {
      break;
   }

#ifdef CMD_DB_IN_DDR
      aop_mem_space_entry_hdr =
        (cmd_db_entry_header *)set_ddr_access((uint64_t)bcm_db_headers_ext);
      set_page_select(0);
      aop_mem_space_data = (uint8_t *)set_ddr_access((uint64_t)bcm_aux_ext);
      set_page_select(0);
#else
      aop_mem_space_entry_hdr = (cmd_db_entry_header *)bcm_db_headers_ext;
      aop_mem_space_data = (uint8_t *)bcm_aux_ext;
#endif
      /* Add static entries */
      err = cmd_db_add_static_data(CMD_DB_SLV_ID_BCM, 0x1000,
                                   aop_mem_space_entry_hdr,
                                   bcm_db_headers_cnt,
                                   aop_mem_space_data,
                                   bcm_aux_size);
   if (err != CMD_DB_SUCCESS)
   {
     break;
   }
  } while (0);
  return err;
}

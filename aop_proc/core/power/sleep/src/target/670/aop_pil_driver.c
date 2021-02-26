/*===========================================================================
                              aop_pil_driver.c

DESCRIPTION: PIL driver on HLOS sends AOP QMP messages when bringing 
             subsystems out of reset. This driver enables timers to be considered
             during solving for such images.

Copyright (c) 2017 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rex.h"
#include "msmhwio.h"
#include "aop_hwio.h"
#include "CoreVerify.h"
#include "rpmh_common.h"
#include "arc_rm_hwio.h"
#include "qmp_msg_driver.h"
#include "hwio_arc_vt.h"
#include "rpmh_arc.h"
#include "aop_ulog.h"
#include "drv_lookup.h"
#include "ChipInfo.h"
#include "pdc_global_hwio.h"
//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define NUM_VERSIONS  1
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
static uint32 sleep_ignore_mask = (0x0); //set to 1 if SS needs to be ignored
static uint32 timer_ignore_mask = (0x0); //set to 1 if SS needs to be ignored

//bit positions on the mask have changed per version
static uint32 bmsk_version_mapping[NUM_VERSIONS][RSC_DRV_GLOBAL_MAX] =
{{
  -1,  //TZ       0.No MASK
  -1,  //HYP      1.No MASK
  0,  //HLOS      2.
  1,  //SECPROC   3.
  2,  //AUDIO     4.
  3,  //SENSORS   5.
  4,  //AOP       6.
  5,  //DEBUG     7.
  6,  //GPU       8.
  7,  //DISPLAY   9.
  8, //COMPUTE    10.
  9, //MODEM SW   11. Modem shares a mask
  9, //MODEM HW   12.
  -1, //ARC CPRF  13. No MASK
  -1, //BCMCD     14. No MASK
  -1, //BCM       15. No MASK
}
};
//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
/**
 * <!-- enable_timers -->
 *
 * @brief enable/disable timers for rms to consider while solving
 *
 * @return none
 */
static void enable_timers(uint32 drv_id, boolean consider)
{
  int enable_bit = consider ? 1 : 0;

  HWIO_OUTI2(DRVd_RMm_TIMER,drv_id, RPMH_ARC_VDD_CX, enable_bit); //CX
  HWIO_OUTI2(DRVd_RMm_TIMER,drv_id, RPMH_ARC_VDD_MX, enable_bit); //MX
  HWIO_OUTI2(DRVd_RMm_TIMER,drv_id, RPMH_ARC_DDR_SS, enable_bit); //DDR
  HWIO_OUTI2(DRVd_RMm_TIMER,drv_id, RPMH_ARC_XO, enable_bit); //XO
}

static uint32 do_bmsk_lkup(uint32 drv_id)
{
  CORE_VERIFY(drv_id < RSC_DRV_GLOBAL_MAX);
  ChipInfoVersionType version = ChipInfo_GetChipVersion();
  uint32 major_version = MIN(version >> 16, NUM_VERSIONS);
  CORE_VERIFY(major_version > 0);
  return bmsk_version_mapping[major_version-1][drv_id]; 
  
}

static void enable_sleep_mask(uint32 drv_id, boolean consider)
{
  int index = do_bmsk_lkup(drv_id);
  int bitvalue = consider ? 0 : 1;
  CORE_VERIFY(index != -1);
  HWIO_OUTM(RPMH_PDC_SOC_SLEEP_LOCK_MASK_BANK0, 1 << index, bitvalue << index); //update sleep mask
  HWIO_OUTM(RPMH_PDC_CX_COLLAPSE_LOCK_MASK_BANK0, 1 << index, bitvalue << index); //update cx pc mask
}


/**
 * <!-- aop_pil_processMsg -->
 *
 * @brief callback to process PIL messages from HLOS.
 *        Enables/Disables ARC timers for the corresponding
 *        subsystem.
 * Message Examples:
 * {class: image, res: load_state, name: modem, val: on} 
 * {class: image, res: load_state, name: cdsp, val: off} 
 * {class: image, res: load_state, name: slpi, val: on}
 *
 *
 * @return none
 */
void aop_pil_processMsg( char* msg, EDEGES sub )
{
  char val[MAX_MSG_LEN];
  RSCSW_DRV_GLOBAL_V1_MAPPING image = RSC_DRV_GLOBAL_MAX;
  boolean bTurn_ON = false;

  //Expecting messages only from HLOS
  CORE_VERIFY(sub == _APPS);

  if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "res", strlen("res"), val, MAX_MSG_LEN) )
  {
    if ( strcmp(val, "load_state") == 0 )
    {
      if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "name", strlen("name"), val, MAX_MSG_LEN))
      {
        // map name to image
        if (strcmp(val, "modem")== 0)
        {
          image = RSC_DRV_GLOBAL_MODEM_SW;
        }
        else if (strcmp(val, "cdsp")== 0)
        {
          image = RSC_DRV_GLOBAL_COMPUTE_DSP;
        }
        else if (strcmp(val, "adsp")== 0)
        {
          image = RSC_DRV_GLOBAL_SENSORS;
        }
        else
        {
          SWEVENT(IMAGE_NAME_INVALID, val);
          CORE_VERIFY(0);
        }
      }

      // is it turning on?
      if ( JSON_PARSER_OK == json_parser_simple(msg, strlen(msg), "val", strlen("val"), val, MAX_MSG_LEN) )
      {
        if (strcmp(val, "on") == 0)
        {
          bTurn_ON = TRUE;
        }
        else
        {
          bTurn_ON = FALSE;
        }
      }
      //modify mask for corresponding image
      
      // lookup DRV_IDs based on version
      uint32 drv_id = 0;
      CORE_VERIFY(RSC_DRV_GLOBAL_MAX != image);
      CORE_VERIFY(DRV_ID_LOOKUP_SUCCESS == get_version_drv_id(image, &drv_id));

      // check if the image is in sleep ignore mask
      if (!(sleep_ignore_mask & (1 << image))) 
      {
        enable_sleep_mask(drv_id, bTurn_ON);
      }

      // check if the image is in timers ignore mask
      if (!(timer_ignore_mask & (1 << image))) 
      {
        // hw bug wa
        // cdsp's timers considered must be set in modem hw's timers
        if (image == RSC_DRV_GLOBAL_COMPUTE_DSP)
        {
          image = RSC_DRV_GLOBAL_MODEM_HW;
          CORE_VERIFY(DRV_ID_LOOKUP_SUCCESS == get_version_drv_id(image, &drv_id));
        }
        //if bit is set
      enable_timers(drv_id, bTurn_ON);
    }
      
    }
    else
    {
      //malformed JSON
      SWEVENT(PIL_MALFORMED_JSON);
      CORE_VERIFY(0);
    } //load_state invalid
  }
  else
  {
    //malformed JSON
    SWEVENT(PIL_MALFORMED_JSON);
    CORE_VERIFY(0);
  } // res invalid
}


static void build_consider_masks(void)
{
  timer_ignore_mask |= (1 << RSC_DRV_GLOBAL_SENSORS); //ignore sensors for timers considered
  sleep_ignore_mask |= (1 <<  RSC_DRV_GLOBAL_SECPROC); //ignore sproc for sleep
}

/**
 * <!-- pil_driver_init -->
 *
 * @brief registers callbacks for PIL messages
 *
 * @return none
 */
void pil_driver_init (void)
{
  build_consider_masks();
  CORE_VERIFY ( MSG_SUCESS == qmp_msg_register_cb(aop_pil_processMsg, "image") );
}


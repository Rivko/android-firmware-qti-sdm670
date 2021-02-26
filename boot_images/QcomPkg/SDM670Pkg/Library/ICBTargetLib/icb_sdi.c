/**

@file   icb_sdi.c

@brief  ICB support for SDI. Save/restore MEMNOC address translators
        
===========================================================================

                             Edit History


when         who     what, where, why
----------   ---     --------------------------------------------------------
2017/05/09   sds     Updated for v2
2016/11/18   sds     Created 

===========================================================================
       Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
                       All Rights Reserved.
                       QUALCOMM Proprietary
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "../../../Include/HALhwio.h"
#include "../../Include/msmhwiobase.h"
#include "../../../Library/ICBLib/icbcfg_hwio.h"
#include "icbcfg_hwio_target_v1.h"
#include "icbcfg_hwio_target_v2.h"
#include "ChipInfo.h"
#include "string.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define MAX_CHANNELS 4
#define SEGMENTS 6

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
typedef struct
{
  uint32_t base_low;
  uint32_t base_high;
  uint32_t offset_low;
  uint32_t offset_high;
} icb_segment_t;

typedef struct
{
  uint32_t      enable_low;
  uint32_t      error_low;
  icb_segment_t segs[SEGMENTS];
} icb_sdi_trans_t;

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */
static bool valid = false;
static bool use_llcc = true;
static uint32_t channels = 4;
static icb_sdi_trans_t trans[MAX_CHANNELS];
static uint32_t trans_bases_memnoc[MAX_CHANNELS] =
{
  HWIO_ADDR(MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW),
  HWIO_ADDR(MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW),
  HWIO_ADDR(MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW),
  HWIO_ADDR(MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW),
};
static uint32_t trans_bases_llcc[MAX_CHANNELS] =
{
  HWIO_ADDR(LLCC0_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
  HWIO_ADDR(LLCC1_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
  HWIO_ADDR(LLCC2_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
  HWIO_ADDR(LLCC3_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
};

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

//*============================================================================*/
/**
@brief 
      Save addr translators
*/ 
/*============================================================================*/
void icb_sdi_save( void )
{
  uint32_t ch, seg;

  /* Check the hardware version and update some variables. */
  if( CHIPINFO_ID_SDM845 == ChipInfo_GetChipFamily() )
  {
    /* Napali has 4 channels. */
    channels = 4;

    /* We use LLCC version only if we're 2.0 or greater. */
    if( CHIPINFO_VERSION(2,0) <= ChipInfo_GetChipVersion() )
    {
      use_llcc = true;
    }
    else
    {
      use_llcc = false;
    }
  }
  /* We must be Warlock.
   * This chip always uses LLCC and has 2 channels. */
  else
  {
    channels = 2;
    use_llcc = true;
  }

  /* Save segment decode registers. */
  memset(&trans, 0, sizeof(trans));
  for( ch = 0; ch < channels; ch ++ )
  {
    /* MEMNOC addr translator */
    if( !use_llcc )
    {
      trans[ch].enable_low = HWIO_INX(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW);
      trans[ch].error_low  = HWIO_INX(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_ERROR_LOW);
      for( seg = 0; seg < SEGMENTS; seg ++ )
      {
        if( seg != 0 )
        {
          trans[ch].segs[seg].base_low  = HWIO_INXI(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_BASEn_LOW,seg);
          trans[ch].segs[seg].base_high = HWIO_INXI(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH,seg);
        }
        trans[ch].segs[seg].offset_low  = HWIO_INXI(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW,seg);
        trans[ch].segs[seg].offset_high = HWIO_INXI(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH,seg);
      }
    }
    /* LLCC addr translator */
    else
    {
      trans[ch].enable_low = HWIO_INXF(trans_bases_llcc[ch],LLCC_BEAC_ADDR_TRANSLATOR_CFG,ENABLE);
      trans[ch].error_low  = HWIO_INXF(trans_bases_llcc[ch],LLCC_BEAC_ADDR_TRANSLATOR_CFG,ERROR);
      for( seg = 0; seg < SEGMENTS; seg ++ )
      {
        if( seg != 0 )
        {
          trans[ch].segs[seg].base_low  = HWIO_INXI(trans_bases_llcc[ch],LLCC_BEAC_ADDR_REGIONn_CFG2,seg);
          trans[ch].segs[seg].base_high = HWIO_INXI(trans_bases_llcc[ch],LLCC_BEAC_ADDR_REGIONn_CFG3,seg);
        }
        trans[ch].segs[seg].offset_low  = HWIO_INXI(trans_bases_llcc[ch],LLCC_BEAC_ADDR_REGIONn_CFG0,seg);
        trans[ch].segs[seg].offset_high = HWIO_INXI(trans_bases_llcc[ch],LLCC_BEAC_ADDR_REGIONn_CFG1,seg);
      }
    }
  }

  valid = true;
}

//*============================================================================*/
/**
@brief 
      Restore addr translators

@dependencies
      icb_sdi_save() must have been called first.
 
@return    
      true if successful
      false if no valid data to restore
*/ 
/*============================================================================*/
bool icb_sdi_restore( void )
{
  uint32_t ch, seg;

  if( false == valid )
  {
    return false;
  }

  /* MEMNOC addr translator */
  if( !use_llcc )
  {
    /* First, disable all address translators. */
    for( ch = 0; ch < channels; ch ++ )
    {
      HWIO_OUTX(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW,0);
    }

    /* Write translator configuration. */
    for( ch = 0; ch < channels; ch ++ )
    {
      HWIO_OUTX(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_ERROR_LOW,trans[ch].error_low);
      for( seg = 0; seg < SEGMENTS; seg ++ )
      {
        if( seg != 0 )
        {
          HWIO_OUTXI(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_BASEn_LOW,seg,trans[ch].segs[seg].base_low);
          HWIO_OUTXI(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_BASEn_HIGH,seg,trans[ch].segs[seg].base_high);
        }
        HWIO_OUTXI(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_OFFSETn_LOW,seg,trans[ch].segs[seg].offset_low);
        HWIO_OUTXI(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_OFFSETn_HIGH,seg,trans[ch].segs[seg].offset_high);
      }
    }

    /* Finally, set enable bits according to saved state. */
    for( ch = 0; ch < channels; ch ++ )
    {
      HWIO_OUTX(trans_bases_memnoc[ch],MEMNOC_ADDR_TRANSLATOR_ENABLE_LOW,trans[ch].enable_low);
    }
  }
  /* LLCC addr translator */
  else
  {
    /* First, disable all address translators. */
    for( ch = 0; ch < channels; ch ++ )
    {
      HWIO_OUTXF(trans_bases_llcc[ch],LLCC_BEAC_ADDR_TRANSLATOR_CFG,ENABLE,0);
    }

    /* Write translator configuration. */
    for( ch = 0; ch < channels; ch ++ )
    {
      HWIO_OUTXF(trans_bases_llcc[ch],LLCC_BEAC_ADDR_TRANSLATOR_CFG,ERROR,trans[ch].error_low);
      for( seg = 0; seg < SEGMENTS; seg ++ )
      {
        if( seg != 0 )
        {
          HWIO_OUTXI(trans_bases_llcc[ch],LLCC_BEAC_ADDR_REGIONn_CFG2,seg,trans[ch].segs[seg].base_low);
          HWIO_OUTXI(trans_bases_llcc[ch],LLCC_BEAC_ADDR_REGIONn_CFG3,seg,trans[ch].segs[seg].base_high);
        }
        HWIO_OUTXI(trans_bases_llcc[ch],LLCC_BEAC_ADDR_REGIONn_CFG0,seg,trans[ch].segs[seg].offset_low);
        HWIO_OUTXI(trans_bases_llcc[ch],LLCC_BEAC_ADDR_REGIONn_CFG1,seg,trans[ch].segs[seg].offset_high);
      }
    }

    /* Finally, set enable bits according to saved state. */
    for( ch = 0; ch < channels; ch ++ )
    {
      HWIO_OUTXF(trans_bases_llcc[ch],LLCC_BEAC_ADDR_TRANSLATOR_CFG,ENABLE,trans[ch].enable_low);
    }
  }

  return true;
}


/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_training_seq.c#3 $
$DateTime: 2018/02/15 02:48:22 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
02/19/16   tw      Initial Version
================================================================================*/
#include "ddrss.h"
#include "HAL_SNS_DDR.h"
#include "frwk_training.h"
#include "target_config.h"

frwk_training_struct pre_training[] =
{
  {
    0,
    "identify training frequencies",
    identify_training_frequencies,
    MANDATORY,
    {NULL},
  },
  {
    1,
    "pre training setup",
    pre_training_setup,
    MANDATORY,
    {NULL},
  },
  {
    2,
    "DDRSS_RCW",
    Training_DDRSS_RCW,
    MANDATORY,
    {NULL},
  }, 
  // null entry for the last entry in the table
  {0}
};

/* -------------------------------------------------------------------
Training name	    Dependency			  Mandatory Requirement
RCW				        No					      Yes, above a certain frequency
DCC        		    No					      No
CA				        No					      Yes
WR_TDQS2DQ  	    No					      No
WRLVL			        No            		No
RD_DQDQS		      No					      Yes
WR_DQDQS		      Yes, RD_DQDQS		  Yes
DIT				        No					      No
------------------------------------------------------------------- */

frwk_training_struct do_training[] =
{
  {
    0,
    "DDRSS_PLLPRECAL",
    Training_DDRSS_PLLPRECAL,
    NOT_MANDATORY,
    {NULL},
  },  
  
  {
    1,
    "DDRSS_DCC",
    Training_DDRSS_DCC,
    NOT_MANDATORY,
    {NULL},
  },
  //{
  //  1,
  //  "DDRSS_CA_VREF_LPDDR4",
  //  Training_DDRSS_CA_VREF_LPDDR4,
  //  MANDATORY,
  //  {NULL},
  //}, 
  
  {
    2,
    "DDRSS_WR_TDQS2DQ",
    Training_DDRSS_WR_TDQS2DQ,
    NOT_MANDATORY,
    {NULL},
	
  },
     
  {
    3,
    "DDRSS_WRLVL",
    Training_DDRSS_WRLVL,
    MANDATORY,
    {NULL},
  },     
  {
    4,
    "DDRSS_RD_DQDQS",
    Training_DDRSS_RD_DQDQS_LPDDR4,
    MANDATORY,
    {NULL},
  },  
  {
    5,
    "DDRSS_WR_DQDQS",
    Training_DDRSS_WR_DQDQS_LPDDR4,
    MANDATORY,
    {Training_DDRSS_RD_DQDQS_LPDDR4, NULL},
  }, 
  {
    6,
    "DDRSS_DIT",
    Training_DDRSS_DIT,
    NOT_MANDATORY,
    {NULL},
  }, 
  
  // null entry for the last entry in the table
  {0} 
};

frwk_training_struct post_training[] =
{
  {
    0,
    "post training setup",
    post_training_setup,
    MANDATORY,
    {NULL},
  },
  
  // null entry for the last entry in the table
  {0}  
};


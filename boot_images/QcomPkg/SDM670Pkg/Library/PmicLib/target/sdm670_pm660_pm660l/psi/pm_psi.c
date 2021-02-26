/*===========================================================================
GENERAL DESCRIPTION
  This file is just to give an entry point to the pmic elf image

 &copy; Copyright 2013-2015 Qualcomm Technologies Inc, All Rights Reserved
 Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/PmicLib/target/sdm670_pm660_pm660l/psi/pm_psi.c#1 $ 
$DateTime: 2017/09/18 09:26:13 $  $Author: pwbldsvc $


when         who     what, where, why
----------   ---     ----------------------------------------------------------
09/01/15     aab     Updated driver to support SBL Conditional configuration feature
06/23/15     aab     Added support for MSM8996 V2/V3. Removed support for V1
04/29/15     aab     Added support for PMK8001
06/06/14     akm     Created for psii elf image creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "com_dtypes.h"
#include "pm_config_sbl.h"
/*===========================================================================

                           DEFINITIONS

===========================================================================*/
extern pm_sbl_config_info_type   pm_sbl_config_info[];

extern pm_sbl_seq_type           pm_sbl_seq[];
extern pm_pbs_ram_data_type      pm_pbs_seq[];
extern pm_sbl_cond_seq_type      pm_sbl_cond_seq[];
pm_sbl_pdm_data_type    pm_psi_data_hdr   = {pm_sbl_seq, pm_pbs_seq, pm_sbl_cond_seq, /*pm_sbl_cond_seq_info*/};


pm_sbl_pdm_config_type pm_psi_header __attribute__((section("pm_psi_info"))) =
                                     { 
                                       pm_sbl_config_info,
                                       &pm_psi_data_hdr
                                     };


/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/



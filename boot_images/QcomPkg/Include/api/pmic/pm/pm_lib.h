#ifndef _PM_LIB__H
#define _PM_LIB__H
/*! \file 
 *  \n
 *  \brief
 *  \n
 *  \n This file is a master include file for PMIC library APIs 
 *  \n
 *  \n &copy; Copyright 2009-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/23/13   sv      Added MISC driver header to pm_misc.h  (CR-473539)
06/17/13   aab     Updated SMBB driver header to pm_smbb.h  
05/09/13   aab     Updated GPIO driver header to pm_gpio.h 
03/27/13   aab     Updated GPIO driver header to pm_gpio.h 
03/13/13   hw      Rearchitecting clk module driver to peripheral driver
03/07/13   aab     Added pm_rgb.h, pm_lpg.h and pm_vib.h
12/06/12   hw      Rearchitecturing module driver to peripheral driver
09/25/12   hw      Remove module header files that are not needed for sbl1 API support
04/16/12   hs      Removed irq files.
03/16/12   hs      Removed obsolete files.
03/14/12   hs      Replaced pm_mega_xo.h and pm_clocks.h with pm_xo_core.h
                   and pm_clk_buff.h
11/29/11   hs      Added pm_mega_xo.h
09/14/11   dy      Added pm_uicc.h
04/28/11   jtn     Added pm_smbc.h, pm_btm.h, pm_ovp.h
04/21/11   mpt     Added "pm_boot.h" and "pm_boot_chg.h"
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
11/05/10   hs      Added "pm_nfc.h" for NFC support.
07/05/10   wra     Modified Header includes to accomodate MSM8660
12/02/09   jtn     Add header file
11/19/09   jtn     Updated header files
06/30/09   jtn     Updated file documentation
05/01/09   jtn     New file
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_version.h"  /* include first for definition of pm_model_type */
#include "pm_smps.h"
#include "pm_vs.h"
#include "pm_ldo.h"
#include "pm_boot.h"
#include "pm_pon.h"
#include "pm_gpio.h"
#include "pm_mpp.h"
#include "pm_clk.h"
#include "pm_clk_xo.h"
#include "pm_clk_sleep.h"
#include "pm_rgb.h"
#include "pm_lpg.h"
#include "pm_vib.h"
#include "pm_pbs.h"
#include "pm_resources_and_types.h"
#endif /* _PM_LIB_API_H */

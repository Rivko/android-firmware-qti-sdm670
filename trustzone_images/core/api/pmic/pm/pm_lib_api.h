#ifndef __PM_LIB_API_H__
#define __PM_LIB_API_H__

/*! \file 
 *  \n
 *  \brief  pm_lib_api.h ---- PMIC library API header file 
 *  \n
 *  \n This file is a master include file for PMIC library APIs 
 *  \n
 *  \n &copy; Copyright 2009-2011 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tz/2.0.2/api/pmic/pm/pm_lib_api.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
05/01/12   hs      Removed pm_vbatt.h.
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


/*****************************************************************
* include all of the header files describing the external interface
* to the PMIC driver
*****************************************************************/
#include "pm_boot.h"
#include "pm_lib_err.h"
#include "pm_resources_and_types.h"
#include "pm_version.h"  /* include first for definition of pm_model_type */
#endif /* _PM_LIB_API_H */

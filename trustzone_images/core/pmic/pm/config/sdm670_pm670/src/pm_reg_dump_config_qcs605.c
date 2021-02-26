/*! \file
*  \n
*  \brief  pm_reg_dump_config.c
*  \n
*  \n This file contains pmic configuration data specific for PMIC debug APIs.
*  \n
*  \n &copy; Copyright 2014-2017 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.0.2/pmic/pm/config/sdm670_pm670/src/pm_reg_dump_config_qcs605.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/17   as      created.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_internal.h"

/*===========================================================================
Data Declarations
===========================================================================*/

/*
 * Array PMIC peripherals supply DDR rails.
 */
const pm_reg_dump_list_type pm_reg_dump_list_qcs605[] =
{
  /* VDD_CX*/
   {0x5, 0x1400, PM_PERIPH_TYPE_SMPS},    /* S1C_CTRL */
   {0x5, 0x1600, PM_PERIPH_TYPE_SMPS},    /* S1C_FREQ */

   /* VDD_MX*/
   {0x1, 0x1700, PM_PERIPH_TYPE_SMPS},    /* S2A_CTRL */
   {0x1, 0x1900, PM_PERIPH_TYPE_SMPS},    /* S2A_FREQ */

  /* LPDDR4 - VDDQ*/
   {0x1, 0x1A00, PM_PERIPH_TYPE_SMPS},    /* S3A_CTRL */
   {0x1, 0x1C00, PM_PERIPH_TYPE_SMPS},    /* S3A_FREQ */

  /* VDD_PX3*/
   {0x1, 0x4C00, PM_PERIPH_TYPE_SMPS},    /* LDO13A */


  /* VDD_PX11*/
   {0x1, 0x4900, PM_PERIPH_TYPE_SMPS},    /* LDO10A */
   

   /*RF Rails : S6A/L7A; S4A/L12A,L13A ;  DBU1/L1B/L2A/L3A */
   {0x1, 0x1D00, PM_PERIPH_TYPE_SMPS},    /* S4_CTRL */
   {0x1, 0x1F00, PM_PERIPH_TYPE_SMPS},    /* S4_FREQ */

   {0x1, 0x2000, PM_PERIPH_TYPE_SMPS},    /* S5_CTRL */
   {0x1, 0x2200, PM_PERIPH_TYPE_SMPS},    /* S5_FREQ */
   
   {0x1, 0x2300, PM_PERIPH_TYPE_SMPS},    /* S6_CTRL */
   {0x1, 0x2500, PM_PERIPH_TYPE_SMPS},    /* S6_FREQ */

   {0x1, 0x4500, PM_PERIPH_TYPE_LDO},    /* L6A */ 

   {0x1, 0x4C00, PM_PERIPH_TYPE_LDO},    /* L13A */


   /* PON: All Peripheral dump for PON */
   {0x0, 0x0800, PM_PERIPH_TYPE_FULL_PERI_REG},  /* PON */

   //Single PMIC Reg list to dump
   {0x1, 0x2649, PM_PERIPH_TYPE_SINGLE_REG},     /* Single PMIC Reg */
   {0x1, 0x264F, PM_PERIPH_TYPE_SINGLE_REG},     /* single PMIC Reg */

   {0xf, 0xffff, PM_PERIPH_TYPE_INVALID}      /* End of list */
};



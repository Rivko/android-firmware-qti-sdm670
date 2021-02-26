/*! \file pm_config_dep.c
 *  
 *  \brief This file contains pmic resource dependencies.
 *  
 *  &copy; Copyright 2016 Qualcomm Technologies Inc, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/config/qcs605/pm_config_dep.c#1 $ 

===========================================================================
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/19/14   as      Created.


===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_vrm.h"

// Component Resource Structure Creation
__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource
smps_a[] =
{
   {NULL,NULL}, // this is invalid place holder
   {"smpa1",NULL},
   {"smpa2","vrm.mx"}, // mx , lpi_mx
   {"smpa3","vrm.ebi"}, //vddq
   {"smpa4",NULL},
   {"smpa5",NULL},
   {"smpa6",NULL},
};

// Component Resource Structure Creation
__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource
smps_c[] =
{
   {NULL,NULL}, // this is invalid place holder
   {"smpc1","vrm.cx"},
   {"smpc2",NULL},
   {"smpc3","vrm.gfx"},
   {"smpc4",NULL},
};

__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource
ldo_a[] =
{
   {NULL,NULL}, // this is invalid place holder
   {"ldoa1",NULL},
   {"ldoa2",NULL},
   {"ldoa3",NULL},
   {"ldoa4",NULL},
   {"ldoa5",NULL},
   {"ldoa6",NULL},
   {"ldoa7","vrm.lcx"},
   {"ldoa8",NULL},
   {"ldoa9",NULL},
   {"ldoa10",NULL},
   {"ldoa11",NULL},
   {"ldoa12",NULL},
   {"ldoa13",NULL},
   {"ldoa14",NULL},
   {"ldoa15",NULL},
   {"ldoa16",NULL},
   {"ldoa17",NULL},
   {"ldoa18",NULL},
   {"ldoa19",NULL},
};

// Component Resource Structure Creation
__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource
clk_a[] =
{
   {NULL,NULL}, // this is invalid place holder
   {"lnbclka1","vrm.xob"},
   {"lnbclka2",NULL},
   {"lnbclka3",NULL},
   {"rfclka1",NULL},
   {"rfclka2",NULL},
   {"rfclka3",NULL},
   {NULL,NULL},
   {NULL,NULL},
   {NULL,NULL},
   {NULL,NULL},
   {"divclka1",NULL},
   {"divclka2",NULL},
   {"divclka3",NULL},
};


// Component Resource Child Depend Structure Creation


__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource *smps4_a_child_dep[] =
{
   &ldo_a[8],
   &ldo_a[9],
   &ldo_a[10],
   &ldo_a[11],
   &ldo_a[12],
   &ldo_a[13],
   &ldo_a[14],
};

__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource *smps5_a_child_dep[] =
{
   &ldo_a[1],
   &ldo_a[6],
   &ldo_a[7],
};

__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource *smps6_a_child_dep[] =
{
   &ldo_a[2],
   &ldo_a[3],
   &ldo_a[5],
};

// Component Resource Dependency Information
__attribute__((section("pm_ddr_reclaim_pool")))
pm_pwr_resource_dependency_info
smps_dep_a[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // S1
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // S2
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // S3
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // S4
   {
      NULL, // parent resource
      smps4_a_child_dep, // child resource dependencies
      7,  // num children
   },
   // S5
   {
      NULL, // parent resource
      smps5_a_child_dep, // child resource dependencies
      3,  // num children
   },
   // S6
   {
      NULL, // parent resource
      smps6_a_child_dep, // child resource dependencies
      3,  // num children
   },
};

__attribute__((section("pm_ddr_reclaim_pool")))
pm_pwr_resource_dependency_info
smps_dep_c[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // S1
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // S2
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // S3
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // S4
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
};

__attribute__((section("pm_ddr_reclaim_pool")))
pm_pwr_resource_dependency_info
ldo_dep_a[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // L1
   {
      &smps_a[5],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L2
   {
      &smps_a[6],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L3
   {
      &smps_a[6],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L4
   {
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
   // L5
   {
      &smps_a[6],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L6
   {
      &smps_a[5],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L7
   {
      &smps_a[5],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L8
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L9
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L10
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L11
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L12
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L13
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L14
   {
      &smps_a[4],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L15
   {
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
   // L16
   {
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
   // L17
   {
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
   // L18
   {
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
   // L19
   {
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
};

__attribute__((section("pm_ddr_reclaim_pool")))
pm_pwr_resource_dependency_info
clk_dep_a[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // C1
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C2
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C3
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C4
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C5
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C6
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C7
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C8
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C9
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C10
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C11
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C12
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   // C13
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
};

// resource dependency info
__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
smps_dep[] = 
{
    smps_dep_a, 
    NULL,
    smps_dep_c,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
ldo_dep[] = 
{
    ldo_dep_a,
    NULL,
    NULL,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
bob_dep[] = 
{
    NULL, 
    NULL,
    NULL,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
vs_dep[] = 
{
    NULL,
    NULL,
    NULL
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
clk_dep[] = 
{
    clk_dep_a,
    NULL,
    NULL
};

// resource info
__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
smps_rsrc[] = 
{
    smps_a,
    NULL,
    smps_c,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
ldo_rsrc[] = 
{
    ldo_a,
    NULL,
    NULL,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
bob_rsrc[] = 
{
    NULL, 
    NULL,
    NULL,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
clk_rsrc[] = 
{
    clk_a,
    NULL,
    NULL,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
vs_rsrc[] = 
{
    NULL,
    NULL,
    NULL,
};
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

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/config/sdm670/pm_config_dep.c#1 $ 

===========================================================================
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/17   as      Updating dep info for SDM670 (CR#2069199)
01/17/14   kt      Created.


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
   {"smpa2",NULL},
   {"smpa3",NULL},
   {"smpa4",NULL},
   {"smpa5","vrm.mss"},
   {"smpa6",NULL},
};

// Component Resource Structure Creation
__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource
smps_b[] =
{
   {NULL,NULL}, // this is invalid place holder
   {"smpb1","vrm.mx"},
   {"smpb2","vrm.gfx"},
   {"smpb3","vrm.cx"},
   {"smpb4",NULL},
   {"smpb5",NULL},
};

// Component Resource Structure Creation
__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource
bob_b[] =
{
   {NULL,NULL}, // this is invalid place holder
   {"bobb1",NULL},
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
   {"ldoa7",NULL},
   {"ldoa8",NULL},
   {"ldoa9",NULL},
   {"ldoa10",NULL},
   {"ldoa11",NULL},
   {"ldoa12",NULL},
   {"ldoa13","vrm.ebi"}, // TODO : WA has been added to support ebi rail
   {"ldoa14",NULL},
   {"ldoa15",NULL},
   {"ldoa16",NULL},
   {"ldoa17",NULL},
   {"ldoa18",NULL},
   {"ldoa19",NULL},
};

__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource
ldo_b[] =
{
   {NULL,NULL}, // this is invalid place holder
   {"ldob1",NULL},
   {"ldob2",NULL},
   {"ldob3",NULL},
   {"ldob4",NULL},
   {"ldob5",NULL},
   {"ldob6",NULL},
   {"ldob7",NULL},
   {"ldob8",NULL},
   {"ldob9","vrm.lcx"},
   {"ldob10","vrm.lmx"},
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

__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource
target_rsrc_name[] =
{
   {NULL,NULL},
   {"gpiob1",NULL},
};


// Component Resource Child Depend Structure Creation
__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource *smps6_a_child_dep[] =
{
   &ldo_a[1],
   &ldo_a[6],
   &ldo_a[7],
};

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
static pm_pwr_resource *bob1_b_child_dep[] =
{
   &ldo_a[15],
   &ldo_a[16],
   &ldo_a[17],
   &ldo_a[18],
   &ldo_a[19],
   &ldo_b[2],
   &ldo_b[3],
   &ldo_b[4],
   &ldo_b[5],
   &ldo_b[6],
   &ldo_b[7],
   &ldo_b[8],
};


__attribute__((section("pm_ddr_reclaim_pool")))
static pm_pwr_resource *dbu1_child_dep[] =
{
   &ldo_a[3],
   &ldo_a[2],
};


// Component Resource Dependency Information
pm_pwr_resource_dependency_info
target_dep[] =
{
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
   {
      NULL, // parent resource
      dbu1_child_dep, // child resource dependencies
      2,  // num children
   },
};

uint32 num_target_dep = sizeof(target_dep)/sizeof(pm_pwr_resource_dependency_info);

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
      NULL, // child resource dependencies
      0,  // num children
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
smps_dep_b[] =
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
    // S5
   {
      NULL, // parent resource
      NULL, // child resource dependencies
      0,  // num children
   },
};

__attribute__((section("pm_ddr_reclaim_pool")))
pm_pwr_resource_dependency_info
bob_dep_b[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // B1
   {
      NULL, // parent resource
      bob1_b_child_dep, // child resource dependencies
      12,  // num children
   },
};

__attribute__((section("pm_ddr_reclaim_pool")))
pm_pwr_resource_dependency_info
ldo_dep_a[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // L1
   {
      &smps_a[6],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L2
   {
      &target_rsrc_name[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L3
   {
      &target_rsrc_name[1],
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
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
   // L6
   {
      &smps_a[6],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L7
   {
      &smps_a[6],
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
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L16
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L17
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L18
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L19
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
};

__attribute__((section("pm_ddr_reclaim_pool")))
pm_pwr_resource_dependency_info
ldo_dep_b[] =
{
   { NULL, NULL, 0, }, // Invalid zeroth array - NOT USED
   // L1
   {
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
   // L2
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L3
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L4
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L5
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L6
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L7
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L8
   {
      &bob_b[1],
      NULL, // child resource dependencies
      0,  // num children
   },
   // L9
   {
      NULL,
      NULL, // child resource dependencies
      0,  // num children
   },
   // L10
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
    smps_dep_b,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
ldo_dep[] = 
{
    ldo_dep_a,
    ldo_dep_b,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
bob_dep[] = 
{
    NULL, 
    bob_dep_b,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
vs_dep[] = 
{
    NULL,
    NULL,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource_dependency_info* 
clk_dep[] = 
{
    clk_dep_a,
    NULL,
};

// resource info
__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
smps_rsrc[] = 
{
    smps_a,
    smps_b,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
ldo_rsrc[] = 
{
    ldo_a,
    ldo_b,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
bob_rsrc[] = 
{
    NULL, 
    bob_b,
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
clk_rsrc[] = 
{
    clk_a,
    NULL, 
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_pwr_resource* 
vs_rsrc[] = 
{
    NULL,
    NULL,
};



/*===========================================================================

            GLink RPM transport 8996 UEFI Configuration Structures

=============================================================================

  @file
    sport_rpm_config.c

    Contains structures to be used in Glink RPM trasnport configuration.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/15   an      fix FIFO IDs
12/09/14   bc      update incoming interrupt
10/20/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "xport_rpm_config.h"
#include "msmhwiobase.h"
#include "smem_type.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

void* XPORT_RPM_MSG_RAM_BASE = NULL;
uint32 XPORT_RPM_MSG_RAM_SIZE = 0;

/** Size of Glink RPM transport ToC in MSG RAM */
#define XPORT_RPM_MSG_TOC_SIZE        0x100

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
const xport_rpm_config_type xport_rpm_nazgul_config[] =
{
  /* apss->Rpm */
  {
    "rpm",                /* Remote host name        */
    XPORT_RPM_AP2R_ID,    /* Tx FIFO ID in RPM ToC   */
    XPORT_RPM_R2AP_ID,    /* Rx FIFO ID in RPM ToC   */
    {(uint32 *)(uintptr_t)0x17911008, 0x01},   /* Outgoing interrupt mask(same as SMD) */
    200                   /* Incoming interrupt(same as SMD) */
  }
  
};

const xport_rpm_config_type xport_rpm_honeybadger_config[] =
{
  /* apss->Rpm */
  {
    "rpm",                /* Remote host name        */
    XPORT_RPM_AP2R_ID,    /* Tx FIFO ID in RPM ToC   */
    XPORT_RPM_R2AP_ID,    /* Rx FIFO ID in RPM ToC   */
    {(uint32 *)(uintptr_t)0x09820010, 0x01},   /* Outgoing interrupt mask(same as SMD) */
    200                   /* Incoming interrupt(same as SMD) */
  }
};

uint32 xport_rpm_config_num = 1;

char* xport_rpm_msg_ram;
uint32* xport_rpm_msg_ram_toc;

void xport_rpm_config_init_toc_addr()
{
  uint32 rev = PcdGet32(PcdSmemInformation) & 0xff;

  if(rev == 3/*honeybadger*/)  
  {
    /* Set XPORT_RPM_MSG_RAM_BASE/SIZE based on chipset */
    XPORT_RPM_MSG_RAM_BASE = (void *)0x00068000;
    XPORT_RPM_MSG_RAM_SIZE = 0x6000;
  }
  else if(rev == 5/*nazgul*/)  
  {
    /* Set XPORT_RPM_MSG_RAM_BASE/SIZE based on chipset */
    XPORT_RPM_MSG_RAM_BASE = (void *)0x00778000;
    XPORT_RPM_MSG_RAM_SIZE = 0x7000;
  }
  else
  {
    ERR_FATAL("Unknown chipset revision", rev, 0, 0);
  }
  
  xport_rpm_msg_ram = (char*)XPORT_RPM_MSG_RAM_BASE;
  xport_rpm_msg_ram_toc = (uint32*)((char*)XPORT_RPM_MSG_RAM_BASE + XPORT_RPM_MSG_RAM_SIZE - XPORT_RPM_MSG_TOC_SIZE);
}

/*===========================================================================
FUNCTION      xport_rpm_get_num_config
===========================================================================*/
/**

  Returns the number of transports supported

  @return     Number of links this transport supports

  @sideeffects  None.
*/
/*=========================================================================*/
uint32 xport_rpm_get_num_config(void)
{
  uint32 rev = PcdGet32(PcdSmemInformation) & 0xff;

  if(rev == 3)
  {
    return ARRAY_LENGTH(xport_rpm_honeybadger_config);
  }
  if(rev == 5)
  {
    return ARRAY_LENGTH(xport_rpm_nazgul_config);
  }
  return 0;
}

/*===========================================================================
FUNCTION      xport_rpm_get_config
===========================================================================*/
/**

  Provides a pointer to transport config strucutre.

  @param[in]  ind    Index of the config

  @return     Pointer to transport config strucutre.

  @sideeffects  None.
*/
/*=========================================================================*/
const xport_rpm_config_type* xport_rpm_get_config(uint32 ind)
{
  uint32 rev = PcdGet32(PcdSmemInformation) & 0xff;

  if (ind >= xport_rpm_config_num)
  {
    return NULL;
  }

  if(rev == 3)
  {
    return &xport_rpm_honeybadger_config[ind];
  }
  if(rev == 5)
  {
    return &xport_rpm_nazgul_config[ind];
  }

  return NULL;
}


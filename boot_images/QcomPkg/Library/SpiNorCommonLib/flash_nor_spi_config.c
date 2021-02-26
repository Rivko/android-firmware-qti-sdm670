/*==================================================================
 *
 * FILE: flash_nor_spi_config.c
 *
 * SERVICES: Functions and data for Flash NOR Configurations
 *
 * DESCRIPTION: This file contain initialization and interface APIs
 *              of flash config data, which is used across all NOR
 *              like devices
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/21/12     sb      Add ST SPI NOR support for 9x25 RUMI 
 * 03/20/12     sv      Add Micron SPI NOR Support
 * 03/19/12     sv      Add SPI NOR Power management support
 * 02/02/12     sy      Fixing Linux compilation issue 
 * 11/01/11     eo      Add Winbond NOR support
 * 10/06/11     sv      Add 2MB Macronix SPI NOR Support
 * 08/24/11     sv      Disable SPI Input Packing and Output Unpacking
 * 05/20/11     bb/sv   Flash driver Optimization
 * 03/14/11     sv      Increase the SPI NOR clock freq to 20MHz(19.2) 
 * 03/03/11     sv      Enable SPI Input packing and Output unpacking
 * 02/01/11     eo      Add device configuration for Maxcronix SPI NOR
 * 10/04/10     sv      Include flash_nor_spi.h for SPI structure defines
 * 03/15/10     bb      Initial Revision
 *==================================================================*/

 /*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_config.h"
#include "flash_nor_spi.h"

#define NUM_ARRAY_ELEMENTS(array, type)  (sizeof(array)/sizeof(type))

/* Supported NOR devices */
static struct flash_nor_params nor_spi_cfgs[] =
{
  /* Micron 1Gbit 1.8v, but using 512Mbit of device only */
  {
    FLASH_NOR_SPI_MICRON,	    /* NOR family type */
    FLASH_NOR_SPI,			    /* Device Technology Type */
    FLASH_x16,				    /* Device Interface Width */
    FLASH_WRITES_SIMPLE,		/* Write style specific to Flash */
    "N25Q00AA13F", 			    /* Device name string */
    3,						    /* Num ids */
    {0x20, 0xBB, 0x21, 0x10},   /* Device ID */
     16384,						/* Number of total blocks in device */
     16,						/* Number of pages in a block */
     256,						/* Logical page size */
     64,						/* Total density in Mega Bytes */
     256,						/* Write Buffer size in bytes */
     0,						    /* Device specific flags */
     0,						    /* Device specific flags */
     0,						    /* Device base address */
     3,						    /* Delay for Deep Power Down */
     30,						/* Delay to release from Deep Power Down */
     0,						    /* Total banks */
     0,						    /* Total erase regions */
     {{0, 0} },				    /* Bank matrix */
     {{0, {{0, 0}}} },			/* Erase region matrix */
     {2, {{1024, 256}, {16384, 16} }}, /* Erase config info*/
  },
  /* Micron 1Gbit 3.3v, but using 512Mbit of device only */
    {
      FLASH_NOR_SPI_MICRON,	    /* NOR family type */
      FLASH_NOR_SPI,			    /* Device Technology Type */
      FLASH_x16,				    /* Device Interface Width */
      FLASH_WRITES_SIMPLE,		/* Write style specific to Flash */
      "N25Q00AA13F", 			    /* Device name string */
      3,						    /* Num ids */
      {0x20, 0xBA, 0x21, 0x10},   /* Device ID */
       16384,						/* Number of total blocks in device */
       16,						/* Number of pages in a block */
       256,						/* Logical page size */
       64,						/* Total density in Mega Bytes */
       256,						/* Write Buffer size in bytes */
       0,						    /* Device specific flags */
       0,						    /* Device specific flags */
       0,						    /* Device base address */
       3,						    /* Delay for Deep Power Down */
       30,						/* Delay to release from Deep Power Down */
       0,						    /* Total banks */
       0,						    /* Total erase regions */
       {{0, 0} },				    /* Bank matrix */
       {{0, {{0, 0}}} },			/* Erase region matrix */
       {2, {{1024, 256}, {16384, 16} }}, /* Erase config info*/
    },
  /* Micron 512Mbit 1.8v */
  {
	FLASH_NOR_SPI_MICRON, 	  /* NOR family type */
	FLASH_NOR_SPI,			  /* Device Technology Type */
	FLASH_x16,				  /* Device Interface Width */
	FLASH_WRITES_SIMPLE,		  /* Write style specific to Flash */
	"N25Q016A11E",			  /* Device name string */
	3,						  /* Num ids */
	{0x20, 0xBB, 0x20, 0x10},   /* Device ID */
	 16384,					  /* Number of total blocks in device */
	 16,						  /* Number of pages in a block */
	 256, 					  /* Logical page size */
	 64,						  /* Total density in Mega Bytes */
	 256, 					  /* Write Buffer size in bytes */
	 0,						  /* Device specific flags */
	 0,						  /* Device specific flags */
	 0,						  /* Device base address */
	 3,						  /* Delay for Deep Power Down */
	 30,						  /* Delay to release from Deep Power Down */
	 0,						  /* Total banks */
	 0,						  /* Total erase regions */
	 {{0, 0} },				  /* Bank matrix */
	 {{0, {{0, 0}}} },		  /* Erase region matrix */
	 {2, {{1024, 256}, {16384, 16} }}, /* Erase config info*/
  }
#if 0
  {
    FLASH_NOR_SPI_MACRONIX,     /* NOR family type */
    FLASH_NOR_SPI,              /* Device Technology Type */
    FLASH_x16,                  /* Device Interface Width */
    FLASH_WRITES_SIMPLE,        /* Write style specific to Flash */
    "MX25U1635E",               /* Device name string */
    3,                          /* Num ids */
    {0xC2, 0x25, 0x35, 0xC2},   /* Device ID */
    64,                         /* Number of total blocks in device */
    128,                        /* Number of pages in a block */
    256,                        /* Logical page size */
    2,                          /* Total density in Mega Bytes */
    256,                        /* Write Buffer size in bytes */
    0,                          /* Device specific flags */
    0,                          /* Device specific flags */
    0,                          /* Device base address */
    10,                         /* Delay for Deep Power Down */
    10,                         /* Delay to release from Deep Power Down */
    0,                          /* Total banks */
    0,                          /* Total erase regions */
    {{0, 0} },                  /* Bank matrix */
    {{0, {{0, 0}}} },             /* Erase region matrix */
    {3, {{32, 256}, {64, 128}, {512, 16} }}, /* Erase config info*/
  },
  {
    FLASH_NOR_SPI_WINBOND,      /* NOR family type */
    FLASH_NOR_SPI,              /* Device Technology Type */
    FLASH_x16,                  /* Device Interface Width */
    FLASH_WRITES_SIMPLE,        /* Write style specific to Flash */
    "W25Q16DW",                 /* Device name string */
    3,                          /* Num ids */
    {0xEF, 0x60, 0x15, 0x00},   /* Device ID */
    64,                         /* Number of total blocks in device */
    128,                        /* Number of pages in a block */
    256,                        /* Logical page size */
    2,                          /* Total density in Mega Bytes */
    256,                        /* Write Buffer size in bytes */
    0,                          /* Device specific flags */
    0,                          /* Device specific flags */
    0,                          /* Device base address */
    3,                          /* Delay for Deep Power Down */
    30,                         /* Delay to release from Deep Power Down */
    0,                          /* Total banks */
    0,                          /* Total erase regions */
    {{0, 0} },                  /* Bank matrix */
    {{0, {{0, 0}}} },             /* Erase region matrix */
    {3, {{32, 256}, {64, 128}, {512, 16} }}, /* Erase config info*/
  }
#endif
};

/* Supported Devices */
static struct flash_nor_cfg_data supported_nor_spi_cfgs[] =
{
  {
    FLASH_NOR_SPI,
    0,
    NUM_ARRAY_ELEMENTS(nor_spi_cfgs, flash_nor_params_type),
    nor_spi_cfgs
  },
  {
    FLASH_UNKNOWN,  /* TO MARK THE END - Mandatory - Used in driver */
    0,
    0,
    NULL
  }
};

/* NOR Probe table. Driver will be probing for the
   device in the order listed in the below table. */
static flash_probe_tbl nor_probe_table[] =
{
  {nor_spi_probe},
  {NULL}
};

/* Return all supported NOR configurations */
void flash_nor_spi_get_configs(struct flash_nor_cfg_data **cfg_data)
{
  *cfg_data = (struct flash_nor_cfg_data *) &supported_nor_spi_cfgs;
}

void flash_nor_spi_get_probe_table(flash_probe_tbl **probe_tbl)
{
  *probe_tbl = &nor_probe_table[0];
}

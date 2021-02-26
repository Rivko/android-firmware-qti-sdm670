#ifndef FLASH_DAL_H
#define FLASH_DAL_H
/*=============================================================================
 *
 * FILE:      flash_dal.h
 *
 * DESCRIPTION: Function and data structure declarations specific to Flash DAL
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/19/14     whc     add device operating frequency support for NOR 
 * 07/16/13     sb      add parti name member in flash client data
 * 09/05/12     bb/sv   Cleanup and Optimization
 * 03/20/12     sv      Add Micron SPI NOR support
 * 03/19/12     sv      Add SPI NOR power management support
 * 03/19/12     eo      Support for erase blocks by size
 * 11/01/11     eo      Add Winbond NOR support
 * 05/20/11     jz      hal_id cleanup
 * 02/01/11     eo      Add Maxcronix SPI NOR device support
 * 07/20/10     jz      Updated 2x support, removed flash_dal_write_2x_pages
 * 06/23/10     bb      Fixing all enum sizes to be 4 bytes
 * 05/11/10     eo      Add SPI NOR support Apis
 * 01/28/10     sb      Bad block optimization
 * 12/28/09     sc      Added 2x support in DAL
 * 09/23/09     mh      Add provision for multiple hal_ids per client
 * 02/23/09     bb      Read/Write API changes with iovec design changes
 * 11/25/08     mm      Add support for UUID
 * 11/15/08     mm      Remove dependency on custflash.h
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "DALStdErr.h"
#include "DALFramework.h"
#include "DALDeviceId.h"
#include "DALStdDef.h"
#include "DALSys.h"

#include "flash.h"

#ifdef FLASH_LOG_H
  #include FLASH_LOG_H
#else
  #define FLASHLOG(l, x)
#endif

#ifndef FLASH_NOR_BANKS_MAX
  #define FLASH_NOR_BANKS_MAX   2
#endif

#define DALFLASH_INTERFACE_VERSION DALINTERFACE_VERSION(1,0) /**< DAL interface
                                                                  version */

#define GET_FLASH_HANDLE_VTBL(handle) \
		(*((flash_handle_vtbl **)((void *)((UINTN *) handle + 1))))
	
#define INHERIT_FLASH_VTBL(type) \
  int (*device_open)(type *handle); \
  \
  int (*get_info)(type *handle, enum flash_info_type info_type, \
	void *flash_info); \
  \
  int (*erase_blocks)(type *handle, uint32 start_block, \
	uint32 block_count); \
  \
  int (*write_pages)(type* handle, \
	void *buffer, uint32 start_page, uint32 page_count); \
  \
  int (*write_partial_page)(type* handle, \
	void *buffer, \
	uint32 page, uint32 page_offset, uint32 byte_count); \
  \
  int (*read_pages)(type* handle, \
	void *buffer, uint32 start_page, uint32 page_count); \
  \
  int (*erase_blocks_by_size)(type *handle, \
	enum block_size_opcode block_size_type, uint32 start_block, \
	uint32 block_count); \
  \
  int (*set_power_mode)(type *handle, enum flash_power_mode power_mode) 

/*	Required structure as per requirements of the DAL framework */
typedef struct
{
  struct DalDevice DalDevice;		/**< Holds the DAL framework required APIs */
  uint32 (*AddRef)(flash_handle*);	/**< Holds the number of clients of the driver*/
  INHERIT_FLASH_VTBL(flash_handle); /**< Holds flash API for the client */
}flash_handle_vtbl;

#define FLASH_DEV_ENTRY_VTBL(client_ctxt) \
  (client_ctxt->flash_dev_ctxt->dev_entry_vtbl)
#define FLASH_DEV_TYPE_VTBL(client_ctxt)  \
  (client_ctxt->device_type_vtbl)

/* Declaring a "Flash" Driver,Device and Client Context */
typedef struct flash_drv_ctxt     flash_drv_ctxt;
typedef struct flash_dev_ctxt     flash_dev_ctxt;
typedef struct flash_client_ctxt  flash_client_ctxt;

/* Declaring a "Flash" Driver,Device and Client vtbl */
typedef  struct
{
   int  (*power_event)(flash_client_ctxt * handle, DalPowerCmd power_cmd,
     DalPowerDomain power_domain);
   int  (*close)(flash_client_ctxt * handle);
   int  (*deinit)(flash_client_ctxt * handle);

   INHERIT_FLASH_VTBL(flash_client_ctxt);
}flash_vtbl;

/* Declaring a private "flash_dal" vtable */
typedef struct
{
  int (*flash_driver_init)(flash_drv_ctxt *);
  int (*flash_driver_deinit)(flash_drv_ctxt *);
} flash_dal_vtbl;

/*-----------------------------------------------------------------------------
 |              F L A S H    D E V I C E    D A T A
 *----------------------------------------------------------------------------*/

/* Supported NOR families */
enum flash_nor_family
{
  FLASH_NOR_SPANSION,
  FLASH_NOR_INTEL,
  FLASH_NOR_SPI_ATMEL,
  FLASH_NOR_SPI_MACRONIX,
  FLASH_NOR_SPI_WINBOND,
  FLASH_NOR_SPI_MICRON,
  FLASH_NOR_UNKNOWN = 0x7FFFFFFF  /* Force 32-bit */
};

/* Flash device common data */
struct common_dev_data
{
  const char *device_name;            /* Device string */
  enum flash_type   type;             /* Device type */
  enum flash_width  width;            /* Interface width */
  enum flash_write_style write_style; /* Write style of device */
  void *dev_specific_data;            /* Runtime malloc'd device shared data */
};

/* NOR bank matrix info */
struct flash_nor_bank_info
{
  uint16 start_block;                 /* Physical start block */
  uint16 num_blocks;                  /* Block count */
};

struct nor_dev_data
{
  enum flash_nor_family family;       /* NOR family type */
  uint8  device_name[32];             /* Device name string */
  uint32 num_ids;                     /* Num ids */
  uint32 dev_ids[4];                  /* Device ID */
  uint32 block_count;                 /* Number of total blocks in device */
  uint32 pages_per_block;             /* Number of pages in a block */
  uint32 page_size_bytes;             /* Logical page size */
  uint32 device_size_MB;              /* Total density in Mega Bytes */
  uint32 wbuffer_size_bytes;          /* Write Buffer size in bytes */
  uint32 flags1;                      /* Device specific flags */
  uint32 clk;                         /* Device operating frequency in KHz */
  uint32 base_address;                /* Device base address */
  uint32 dpd_delay_time_us;           /* Delay to enter DPD mode */
  uint32 rel_dpd_delay_time_us;       /* Delay to release from DPD mode */
  uint32 num_addr_bytes;              /* Number of address bytes */};

/* Flash Device Info */
struct flash_dev_info
{
  struct common_dev_data common;      /* Common device data */
  struct nor_dev_data  nor;         /* NOR specific device data */
};

/*------------------------------
 * Flash Device Context
 *----------------------------*/
struct flash_dev_ctxt
{
  /* Base Members  */
  uint32      refs;                    /* Reference count */
  DALDEVICEID dev_id;                  /* Device id */
  uint32      dev_ctxt_ref_idx;        /* Device context reference index */
  flash_drv_ctxt  *flash_drv_ctxt;     /* Flash driver context */
  DALSYS_PROPERTY_HANDLE_DECLARE(handle_prop); /* Property handle */

  /* Flash Dev state can be added by developers here */
  DALSYS_SYNC_OBJECT(dev_sync_object); /* Synchronizaton object */
  DALSYSSyncHandle handle_dev_sync;    /* Handle to sync object */
  struct flash_dev_info dev_info;      /* Device info */
  flash_vtbl  *dev_entry_vtbl;         /* Device entry vtbl */
  flash_vtbl  *dev_base_vtbl;          /* Device base vtbl */
  void       *parti_ptr;               /* Partition table */
};

/*------------------------------
 * Flash Driver Context
 *----------------------------*/
struct flash_drv_ctxt
{
  /* Base Members  */
  flash_dal_vtbl flash_dal_vtbl;      /* DAL vtbl */
  uint32  num_dev;                    /* Total device count */
  uint32  size_dev_ctxt;              /* Size of device context */
  uint32  is_initialized;             /* Context initialized state */
  uint32  refs;                       /* Reference count */
  flash_dev_ctxt flash_dev_ctxt[2];   /* Device Context */
  /* Flash Drv state can be added by developers here  */
};

/*---------------------------------------------------------------------------
Structure to encapsulate the CS Interface object. The first member is the
falsh_handle_id followed by vtable pointer (CS requirement).
The last (third) member is a pointer to the Client  Context
----------------------------------------------------------------------------*/
struct  flash_handle
{
  uint32                    flash_handle_id;
  const flash_handle_vtbl   *vtbl;
  flash_client_ctxt         *flash_client_ctxt;
};

/*------------------------------
 * Flash Client Data
 *----------------------------*/
struct flash_client_data
{
  uint32 hal_id;                      /* flash hal id */
  uint32 which_cs;                    /* Chipselect */
  uint32 block_count;                 /* Number of blocks allocated for client */
  uint32 pages_per_block;             /* Number of pages in block */
  uint32 page_size_bytes;             /* Number of bytes not including spare */
  uint32 total_page_size_bytes;       /* Number of bytes including spare */
  uint32 sector_count;                /* Number of sectors - Needed ? */
  uint32 base_address;                /* Base address of device for client */
  uint32 max_spare_udata_bytes;       /* Max spare user data in bytes */
  uint32 block_size_shift;            /* Block <-> page conversion shift */
  uint32 page_size_shift;             /* log to base 2 of page size in bytes */
  //enum flash_ecc_state ecc_state;     /* ECC state of client */

  /*------------------------------
   * Partition specific data
   *----------------------------*/
  char  partition_name[32];          /* partition name string */
  uint32 partition_start_addr;        /* NOR partition start byte address */
  uint32 partition_limit_addr;        /* NOR partition end byte address */
  uint32 partition_attributes;        /* Partition attributes */
  uint32 partition_block_start;       /* Partition physical start block*/
  uint32 partition_block_limit;       /* Partition physical end address */
  uint32 partition_block_count;       /* Partition length in blocks */
  uint32 partition_block_size_bytes;  /* Block size in bytes */
  uint32 partition_last_page;         /* Last page ID in NOR partition */
  void *client_specific_data;         /* Additional client specific private data */
};
/*---------------------------------------------------------------------------
Declaring a "Flash" Client Context
----------------------------------------------------------------------------*/
struct flash_client_ctxt
{
  //Base Members
  uint32  refs;                          /* Reference count */
  uint32  access_mode;                   /* Access mode */
  void *port_ctxt;                       /* Port context for remoting */
  flash_dev_ctxt *flash_dev_ctxt;        /* Device context */
  flash_handle handle;                   /* Interface handle */

  /* Flash Client state can be added by developers here */
  struct flash_client_data client_data;  /* Client specific data */
  flash_vtbl  *device_type_vtbl;         /* Device type vtbl */
  struct flash_client_ctxt *next;        /* Next client for debug */
};

int flash_dal_driver_init(flash_drv_ctxt *);
int flash_dal_driver_deinit(flash_drv_ctxt *);
int flash_dal_device_init(flash_client_ctxt *);
int flash_dal_device_deinit(flash_client_ctxt *);

#endif /*FLASH_DAL_H*/

/*==================================================================================================

FILE: qup_common.c

DESCRIPTION: This module provides the common features for the QUPv3 protocols ( I2C/I3C/SPI/UART )

Copyright (c) 2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   qup_common_init
   qup_common_pram_tre_malloc
   qup_common_pram_tre_free

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/qup_common/src/qup_common.c#3 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/01/17   VV      Initial version
==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "qup_common.h"
#include "pram_mgr.h"
#include <string.h>
#include "fw_config.h"
#include "fw_devcfg.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "err.h"
#include "qurt_printf.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/
#define MAX_SES 6
#define TRE_SIZE 16 // 16 bytes

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/
typedef struct
{
   uint32        addr;
   boolean       used;
   SE_PROTOCOL   protocol;
}TRE_MEM;

typedef struct
{
   TRE_MEM tre_mem[MAX_SES * 2];
   uint8 tres_available[SE_PROTOCOL_MAX];
}TRE_POOL;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static TRE_POOL       tre_pool_256_bytes_aligned; // 16 TREs
static TRE_POOL       tre_pool_128_bytes_aligned; //  8 TRES
static TRE_POOL       tre_pool_64_bytes_aligned;  //  4 TREs
static qurt_mutex_t   qup_common_mutex;
static uint32         buses_pram_base;
static uint8          num_ses[SE_PROTOCOL_MAX];
static boolean        buses_pram_allocated;
static uint32         hw_version = 0;
/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
void qup_common_init(void)
{
   uint8   dev_name[]  = "/dev/qup_ssc";
   uint8   se_str[]    = "seN_cfg";
   se_cfg  *scfg = NULL;
   uint32  i;
   uint32  pram_size;
   uint32  num_tres;
   uint32  req_size;
   uint32  addr;

   DALSYS_PROPERTY_HANDLE_DECLARE (prop_handle);
   DALSYSPropertyVar               prop_var;
   DALResult                       result = DAL_SUCCESS;

   qurt_mutex_init (&qup_common_mutex);

   buses_pram_base = (uint32) pram_acquire_partition("BUSES", &pram_size);

   result = DALSYS_GetDALPropertyHandleStr((const char *) dev_name, prop_handle);
   if (result != DAL_SUCCESS) { return; }

   for (i = 0; i < MAX_SES; i++)
   {
       se_str[2] = '0' + i;

       result = DALSYS_GetPropertyValue(prop_handle, (const char *) se_str, 0, &prop_var);
       if (result != DAL_SUCCESS)  { continue; }

       scfg = (se_cfg *) prop_var.Val.pStruct;
       if (scfg == NULL)           { continue; }

       num_ses[scfg->protocol]++;
   }

   // SPI/I2C/I3C uses 16 TX TRES and 8 RX TREs
   // UART uses 4 TX TREs and 4 RX TREs
   // req_size = total tres * 16 bytes;
   req_size = (((num_ses[SE_PROTOCOL_SPI] +
                 num_ses[SE_PROTOCOL_I2C] +
                 num_ses[SE_PROTOCOL_I3C]) * (16 + 8)) +
               (num_ses[SE_PROTOCOL_UART] * (4 + 4))) * TRE_SIZE;

   if (req_size > pram_size)
   {
      ERR_FATAL("Not enough PRAM memory for QUP TRE allocations.",0,0,0);
   }

   // TODO: Make changes to work with different aligned base address.
   // For now it assumes we always get minimum of 256 byte aligned

   if (buses_pram_base & 0xFF)
   {
      ERR_FATAL("Buses PRAM base not aligned to 256 bytes",0,0,0);
   }

   // 16 TRE memory
   addr = buses_pram_base;
   num_tres = (num_ses[SE_PROTOCOL_SPI] + num_ses[SE_PROTOCOL_I2C] + num_ses[SE_PROTOCOL_I3C]);

   for (i = 0; i < num_tres; i++)
   {
      tre_pool_256_bytes_aligned.tre_mem[i].addr = addr;
      addr = addr + 256;
   }
   tre_pool_256_bytes_aligned.tres_available[SE_PROTOCOL_SPI] = num_ses[SE_PROTOCOL_SPI];
   tre_pool_256_bytes_aligned.tres_available[SE_PROTOCOL_I2C] = num_ses[SE_PROTOCOL_I2C];
   tre_pool_256_bytes_aligned.tres_available[SE_PROTOCOL_I3C] = num_ses[SE_PROTOCOL_I3C];

   // 8 TRE memory
   for (i = 0; i < num_tres; i++)
   {
      tre_pool_128_bytes_aligned.tre_mem[i].addr = addr;
      addr = addr + 128;
   }
   tre_pool_128_bytes_aligned.tres_available[SE_PROTOCOL_SPI] = num_ses[SE_PROTOCOL_SPI];
   tre_pool_128_bytes_aligned.tres_available[SE_PROTOCOL_I2C] = num_ses[SE_PROTOCOL_I2C];
   tre_pool_128_bytes_aligned.tres_available[SE_PROTOCOL_I3C] = num_ses[SE_PROTOCOL_I3C];

   // 4 TRE memory
   num_tres = num_ses[SE_PROTOCOL_UART] * 2;
   for (i = 0; i < num_tres; i++)
   {
      tre_pool_64_bytes_aligned.tre_mem[i].addr = addr;
      addr = addr + 64;
   }
   tre_pool_64_bytes_aligned.tres_available[SE_PROTOCOL_UART] = num_ses[SE_PROTOCOL_UART] * 2;

   // initialize hw_version by calling qup_fw api (non-island code)
   hw_version = qup_hw_version();

   buses_pram_allocated = TRUE;
}

void qup_common_pram_tre_free(void* mem_addr)
{
   uint32 i;
   uint32 size;
   TRE_POOL* tre_pool = NULL;
   uint32 addr = (uint32)mem_addr;
   boolean free_successful = FALSE;

   if (!buses_pram_allocated)
   {
      ERR_FATAL("Calling Buses TRE memory free before its initialization.", 0, 0, 0);
   }

   if (addr == 0)
   {
      ERR_FATAL("QUP TRE mem free failed. Freeing address 0x00.", 0, 0, 0);
   }

   if (addr >= tre_pool_64_bytes_aligned.tre_mem[0].addr)
   {
      tre_pool = &tre_pool_64_bytes_aligned;
      size = 64;
   }
   else if (addr >= tre_pool_128_bytes_aligned.tre_mem[0].addr)
   {
      tre_pool = &tre_pool_128_bytes_aligned;
      size = 128;
   }
   else if (addr >= tre_pool_256_bytes_aligned.tre_mem[0].addr)
   {
      tre_pool = &tre_pool_256_bytes_aligned;
      size = 256;
   }
   else
   {
      ERR_FATAL("QUP TRE mem free failed. Invalid address 0x%08x.", addr, 0, 0);
   }

   qurt_mutex_lock(&qup_common_mutex);

   for (i = 0; i < MAX_SES * 2; i++)
   {
      if (tre_pool->tre_mem[i].addr == addr)
      {
         if (!tre_pool->tre_mem[i].used)
         {
            ERR_FATAL("QUP TRE mem free failed. Freeing unused QUP TRE memory. Addr: 0x%08x",
                      addr,
                      0,
                      0);
         }
         tre_pool->tre_mem[i].used = FALSE;
         tre_pool->tres_available[tre_pool->tre_mem[i].protocol]++;
         tre_pool->tre_mem[i].protocol = 0;
         free_successful = TRUE;
         break;
      }
   }

   qurt_mutex_unlock(&qup_common_mutex);

   if (!free_successful)
   {
      ERR_FATAL("QUP TRE mem free failed. Invalid TRE memory. Addr: 0x%08x", addr, 0, 0);
   }

}

void* qup_common_pram_tre_malloc(SE_PROTOCOL protocol, uint32 size)
{
   uint32    i;
   TRE_POOL* tre_pool;
   uint8     index=0xff;

   if (!buses_pram_allocated)
   {
      ERR_FATAL("Calling Buses TRE memory free before its initialization.", 0, 0, 0);
   }

   switch (size)
   {
      case 256:
         tre_pool = &tre_pool_256_bytes_aligned;
         break;

      case 128:
         tre_pool = &tre_pool_128_bytes_aligned;
         break;

      case 64:
         tre_pool = &tre_pool_64_bytes_aligned;
         break;

   default:
         ERR_FATAL("QUP TRE alloc failed. Protocol requesting invalid size. Protocol: %d, Size: %d",
                   protocol,
                   size,
                   0);
   }

   qurt_mutex_lock(&qup_common_mutex);

   if (tre_pool->tres_available[protocol] == 0)
   {
      ERR_FATAL("QUP TRE alloc failed. Protocol used all its allocations. Protocol: %d, Size: %d",
                protocol,
                size,
                0);
   }

   for (i = 0; i < MAX_SES * 2; i++)
   {
      if ((tre_pool->tre_mem[i].addr != 0) && (!tre_pool->tre_mem[i].used))
      {
            index = i;
            break;
      }
   }

   if (index == 0xFF)
   {
      ERR_FATAL("QUP TRE alloc failed. No available memory in pool. Protocol: %d, Size: %d",
                protocol,
                size,
                0);
   }

   tre_pool->tres_available[protocol]--;
   tre_pool->tre_mem[index].used = TRUE;
   tre_pool->tre_mem[index].protocol = protocol;

   qurt_mutex_unlock(&qup_common_mutex);

   return (void*)(tre_pool->tre_mem[index].addr);

}

uint32 qup_common_get_hw_version ()
{
   return hw_version;
}

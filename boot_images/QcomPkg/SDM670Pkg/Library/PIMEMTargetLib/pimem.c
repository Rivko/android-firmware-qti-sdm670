/*
===========================================================================

FILE:         pimem.c

DESCRIPTION:
  XBL driver for pimem initialization

===========================================================================

                             Edit History


when       who     what, where, why
--------   ---     --------------------------------------------------------
10/05/15   tw      added window 1 configuration and new mmu code to map mmu 
                   in burst clear function
9/10/15    tw      added additiona logic to clear up false positive asserts
05/19/15   kpa     log pimem error status only when pimem is disabled
03/27/15   tw      Updated pimem workaround to disable workaround after init
02/23/15   tw      updated base address of pimem vault
02/11/15   kpa     Update debug registers before enabling pImem.
10/14/14   tw      Initial revision.

===========================================================================
                   Copyright (c) 2014, 2015 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
===========================================================================
*/

/*==============================================================================
                                    INCLUDES
==============================================================================*/
#include  "msmhwiobase.h"
#include  "pimem_hwio.h"
#include  "HALhwio.h"
#include  "pimem.h"
#include  "PrngML.h"
#include  "boot_error_if.h"
#include  "busywait.h"
#include  "boot_logger.h"
#include <stdio.h>
#include <stdarg.h>

/*==============================================================================
                                  DATA
==============================================================================*/
#define LOG_BUF_LEN_MAX		256
#define PIMEM_BASE_ADDR 0x1C000000
/*==============================================================================
                                   FUNCTIONS
==============================================================================*/
//Srini Commented 
void __burstClear(uint64, uint64, uint32, uint32 * );
//void __burstClear(uint32 * , uint32 );
void __pimem_scrub(uint32 * , uint32 );

void pimem_printf(const char* fmt, ...)
{
  char ddr_printf_buf[LOG_BUF_LEN_MAX]={0} ;
  va_list vargs;

  va_start(vargs, fmt);
  vsnprintf(ddr_printf_buf, LOG_BUF_LEN_MAX-1, fmt, vargs);
  va_end(vargs); 
	boot_log_message(ddr_printf_buf);
} /* ddr_printf */

/* ============================================================================
**  Function : pimem_init
** ============================================================================
*/
/*!
*   @brief
*   Initialize pimem for operation
*
*   @details
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void pimem_init(uint64 pimem_mmu_addr, uint64 pimem_mmu_entry)
{
  uint32 i;
  uint32 random_ptr[4] = {0};
  uint64 addr = PIMEM_BASE_ADDR;
  uint32 pimem_esr;
  
  /* check pimem esr prior to init */
  pimem_esr = HWIO_IN(RAMBLUR_PIMEM_ESR);
  if(pimem_esr != 0)
  {
   pimem_printf("pimem ESR detected to be none zero prior to init, possibly recovering from sdi pass 2 without download mode cookie set, try to re-initialize anyways");
  }
  /*
    pIMEM initialization
    PIMEM hardware is initialized during boot. Initialization involves the following steps 
    1.	Initialization of pIMEM keys 
    2.	Configuration of pIMEM windows 
    3.	Configuration of pIMEM vault
    4.	Enablement of pIMEM 
    5.	Initialization of pIMEM memory
  */  
  
  /* step 1, initialize pIMEM keys using HW RNG */
  /*
    RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn , n=[0..3]
    RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REG, n=[0..3]
    RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REG, n=[0..3]
    RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REG, n=[0..3]
  */  

   for(i = 0; i < 4; i++)
   {
     /* generate 4 32bit random numbers */
     if(PrngML_getdata_lite((uint8 *)&random_ptr[0], 16) == PRNGML_ERROR_NONE)
     {
       /* if successful use random number as keys */
       HWIO_OUTI(RAMBLUR_PIMEM_AES_XTS_DATA_KEY_REGn, i, random_ptr[0]);
       HWIO_OUTI(RAMBLUR_PIMEM_AES_XTS_TWEAK_KEY_REGn, i, random_ptr[1]);
       HWIO_OUTI(RAMBLUR_PIMEM_PRIMARY_AUTHENTICATION_KEY_REGn, i, random_ptr[2]);
       HWIO_OUTI(RAMBLUR_PIMEM_HKDF_PSEUDORANDOM_KEY_REGn, i, random_ptr[3]);
     }
     else
     {
      BL_ERR_FATAL(BL_ERR_CORE_VERIFY);
     }
   }
  
  /* configure the vault */
  /*
    RAMBLUR_PIMEM_VAULT_START_ADDR_HI
    RAMBLUR_PIMEM_VAULT_START_ADDR_LOW
  */
  
  HWIO_OUTF(RAMBLUR_PIMEM_VAULT_START_ADDR_HI, VAULT_START_ADDR_HI, 0);
  HWIO_OUTF(RAMBLUR_PIMEM_VAULT_START_ADDR_LOW, VAULT_START_ADDR_LOW, 0x86300);
  
   
  /* HW CR QCTDD01419440 
    To workaround this issue, SW needs to set the following registers to ‘1’ at pIMEM initialization time.  
    * RAMBLUR_PIMEM_DEBUG: FORCE_ASSUME_S_AFULL_EQUALS_1 
    * RAMBLUR_PIMEM_DEBUG: S_AFULL_CHECKER_EN   
    
    TODO: add check for v1 vs v2 as this workaround will have performance penalty
  */
  HWIO_OUTF(RAMBLUR_PIMEM_DEBUG, FORCE_ASSUME_S_AFULL_EQUALS_1, 0x1);
  HWIO_OUTF(RAMBLUR_PIMEM_DEBUG, S_AFULL_CHECKER_EN, 0x1);
  
  /* Enable Window 1 for 14mb */
  /* Enable confidentiality, integrity and anti-rollback protection */
  HWIO_OUTI(RAMBLUR_PIMEM_WINn_ALGORITHM_CONFIG, 1, 0x6);
  
  /* set window size to 14mb */
  HWIO_OUTFI(RAMBLUR_PIMEM_WINn_SIZE, 1, WIN_SIZE, 14);
  
  /* enable pimem ramblur */
  HWIO_OUTF(RAMBLUR_PIMEM_CTL, PIMEM_DISABLE, 0);
  
  /* tbd, rumi fails with pimem auth error if this delay is not here */
  busywait(100);
  
  for(i = 0; i < 16; i+=2)
  {
    /* update pimem mmu entry to reflect next 2mb chunk */
    pimem_mmu_entry &= ~(0x0ff00000);
    pimem_mmu_entry |= addr;
    
    /* perform burst clear on 2mb chunk */
	//commented by srini as it wont work correctly on RUMI 
     __burstClear(pimem_mmu_addr+4*i, pimem_mmu_entry, 2*1024*1024, (uint32 *)addr); 
//	 __burstClear(addr, 2*1024*1024);
    
    /* increment address */
    addr = addr + 2*1024*1024;
  }

  /* check that pimem tag init was successful */
  BL_VERIFY(HWIO_IN(RAMBLUR_PIMEM_ESR) == pimem_esr, BL_ERR_CORE_VERIFY);

  /* remove workaround after init */
  HWIO_OUTF(RAMBLUR_PIMEM_DEBUG, FORCE_ASSUME_S_AFULL_EQUALS_1, 0x0);
  HWIO_OUTF(RAMBLUR_PIMEM_DEBUG, S_AFULL_CHECKER_EN, 0x0);
} /* pimem_init */

void pimem_debug_init(uint64 pimem_mmu_addr, uint64 pimem_mmu_entry)
{
  uint32 * addr= (uint32 *)(PIMEM_BASE_ADDR);
  uint32 size_in_mb;
  uint32 pimem_esr_status;

  pimem_get_status(&size_in_mb);

  /* only perform debug init if pimem is not disabled */
  if(pimem_get_status(&size_in_mb) != PIMEM_DISABLE)
  {
    /* program debug bits to allow pimem to continue even after authentication error */
    HWIO_OUTF(RAMBLUR_PIMEM_ADV_CTL, DISABLE_ON_AUTH_ERROR, 0x0);
    HWIO_OUTF(RAMBLUR_PIMEM_DEBUG, IGNORE_ALL_AUTH_ERRORS_AFTER_FIRST_AUTH_ERROR, 0x1);

    /* perform scrub */
    __pimem_scrub(addr, size_in_mb*1024*1024);

    /* see if we scrubbed out any errors */
    pimem_esr_status = HWIO_IN(RAMBLUR_PIMEM_ESR);
    
    if(pimem_esr_status != 0)
    {
      pimem_printf("PIMEM ESR Detected: %x", pimem_esr_status);
    }
  }
  else
  {
    /* if pimem is disabled reinitialize it after logging */
    pimem_printf("PIMEM is disabled! Reinitializing, content will be lost");
    pimem_init(pimem_mmu_addr, pimem_mmu_entry);
  }
}
/* ============================================================================
**  Function : pimem_get_status
** ============================================================================
*/
/*!
*   @brief
*   Return current pimem status
*
*   @details
*
*   @param size_in_mb   -   [OUT] pointer to fill with actual pimem vault size
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  PIMEM_STATUS - return the current status of pimem
*
*   @sa None
*/

PIMEM_STATUS pimem_get_status(uint32 * size_in_mb)
{
  PIMEM_STATUS status = PIMEM_OK;
  uint32 pimem_disable;
  uint32 pimem_esr_status;
  pimem_disable = HWIO_INF(RAMBLUR_PIMEM_CTL, PIMEM_DISABLE);
  pimem_esr_status = HWIO_IN(RAMBLUR_PIMEM_ESR);
   
  /* check for pimem disable bit */
  if(pimem_disable != 0)
  {
    status = PIMEM_DISABLE;
  }
  /*else if(pimem_esr_status != 0)
  {
    status = PIMEM_ERROR;
    pimem_printf("PIMEM ESR Status: %x", pimem_esr_status);
  }*/
  else
  {
    if(size_in_mb != NULL)
    {
      /* no error detected, find total pimem size */
      *size_in_mb =  HWIO_INF(RAMBLUR_PIMEM_WIN0_SIZE, WIN_SIZE);
      *size_in_mb += HWIO_INFI(RAMBLUR_PIMEM_WINn_SIZE, 1, WIN_SIZE);
      *size_in_mb += HWIO_INFI(RAMBLUR_PIMEM_WINn_SIZE, 2, WIN_SIZE);
      *size_in_mb += HWIO_INFI(RAMBLUR_PIMEM_WINn_SIZE, 3, WIN_SIZE);
    }
  }
  
  return status;
}

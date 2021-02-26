/*=============================================================================
                        System Debug CPU System Register APIs 

GENERAL DESCRIPTION     
	The module defines functions for getting useful data out of co-processor registers
    or system registers
  
 Copyright 2012- 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/systemdebug/sysdbg_sysreg.h#2 $

when       who          what, where, why
--------   ---          --------------------------------------------------
07/09/14    AJCheriyan  Added new translation routines
09/03/13    AJCheriyan  Added new CP15 APIs
06/27/12	AJCheriyan	Created for Aragorn. Compatible with ARMv7 ISA. Written for Krait

===========================================================================*/
#ifndef _SYSDBG_SYSREG_H
#define _SYSDBG_SYSREG_H

#include "comdef.h"


/* Enumeration for various CPUs  */
typedef enum
{
   CPU_INVALID = 0,
   CORTEX_A53  = 1,
   CORTEX_A57  = 2,
   HYDRA       = 3,
   CPU_MAX     = 4,
}cpuname_type;
 


/*===========================================================================
**  Macro : CheckL2WordValid 
** ==========================================================================
*/
/*!
* 
* @brief
*	Check if the L2 word read is valid or not by reading the valid bit
*	There is one bit for a 64 byte granule. 2 bits per line
*/


#define CheckL2WordValid(tag, word, words) \
	((word) < (words >> 1) ? ( word & 0x1) : (word & 0x2))

/* Filler word to be used when cache line address is invalid. */
#define SDI_NULL_WORD	0xDEADBEEF



/*===========================================================================
**  Macro : CACHESW_FORMAT, L1CACHESW_FORMAT, L2CACHESW_FORMAT
** ==========================================================================
*/
/*!
* 
* @brief
*   Helper macro to write to the system register in a format defined by the word within
*   the way of a set which needs to be accessed / read
*
* @param[in] 
*	Enum for the type of the cache (future use), word, set, way number to be accessed.
*	Currently, same format being used for L1 and L2 cache debug
*
* @par Dependencies
*	Words_Per_Read is a constant which depends on architecture. Current, v8 provides two
*	result registers each of 32 bit size and hence, set to 2 below. This affects the number
*	of bits allocated for the "word" to be read from the line and hence, the format.
* 
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/
#define COMPLEMENT(x)	(31 - x)
#define RAMINDEXOP_FORMAT(set, way, word, sets, ways, words_per_read, cachetype) \
    ((cachetype << 24) |(way << (__clz(ways)))  | ( set << (4 + words_per_read)) | (word << (1 + words_per_read)))
#define CACHEOP_FORMAT(set, way, word, sets, ways, words_per_read) \
    ((way << (__clz(ways) + 1)) | (set << (4 + words_per_read)) | (word << (1 + words_per_read)))
/*===========================================================================
**  Macro : OCRAMOP_FORMAT
** ==========================================================================
*/
/*!
* 
* @brief
*   ARM supports dumping of various kinds of memories. To dump any memory, an
*   operation register must be written in the format
*   [31:24]  - RAM Type
*   [23:0]   - Data in a format defined by the memory you are trying to access
*   
* @param[in] 
*   RAM Type and data for above
*
* @par Dependencies
*   Responsibility of the caller to provide data in the correct format
*
* @retval
*   None
* 
* @par Side Effects
*	None
*
*/
#define OCRAMOP_FORMAT(ram_type, data)  ((ram_type << 24) | data)

/*===========================================================================
**  Macro : SETCACHEREADBYSW
** ==========================================================================
*/
/*!
* 
* @brief
*	Macro to general physical address from the L2 tag and sets
*/
#define MASKGEN(x)	((~x >> COMPLEMENT(__clz(x))) << COMPLEMENT(__clz(x)))
#define L2_TAG_TO_PHYS(tag, set, sets, ways, linesize) \
	((tag & MASKGEN((1 << (COMPLEMENT(__clz(sets)) + COMPLEMENT(__clz(linesize)) + 1)))) | (set << (COMPLEMENT(__clz(linesize)) + 2)))

/* L2 Co-Processor Register offsets */
#define L2ECR			0x200
#define L2ESR			0x204
#define L2ESYNR0		0x208
#define L2ESYNR1		0x209
#define L2EAR0			0x20C
#define L2EAR1			0x20D
#define L2DCRDR0		0x300
#define L2DCRDR1		0x301
#define L2DCRTR0		0x302
#define L2DCRTR1		0x303
#define L2CPUTEVSELR	0x00C
#define L2CR0			0x000
#define L2MPCR			0x008


/*===========================================================================
** 	Function: GET_ICACHEDATAREG / GET_DCACHEDATAREG
** ==========================================================================
*/
/*!
* 
* @brief
*	API to read the results of the cache operations. Instruction and Data caches
*	have different result registers
*
*
*@param[in]
*   The index from the set of result registers  
 	
*/
uint64 get_icachedatareg(uint32 count);
uint64 get_dcachedatareg(uint32 count);


/*===========================================================================
** 	Function: SET/GET methods to write to L2 register
** ==========================================================================
*/
/*!
* 
* @brief
*	In the SET API, the 'reg' argument is the offset of the L2 register to write 
*	'data'. 
* 	
*/
void set_l2data(uint32 reg, uint32 data);
uint64 get_l2data(uint32 reg);

/*===========================================================================
** 	Function: SET method to retrieve data from the cache
** ==========================================================================
*/
/*!
* 
* @brief
*	ICRSW - Instruction Cache Read by Set / Way
*	DCRSW - Data Cache Read by Set / Way 
*
*	Data should be written in the following format : 
*	Write to ICRSW should be in the following format
*		[0] 		 0
*		[1]			Level of the Cache trying to clean -1
*		[L-1:2]		Cache Word Index
*		[L+S-1:L]	Cache Set Index
*		[31-A:L+S]	SBZ
*		[31:32-A]	Way
*		
*	L = log2(linelength)
*	S = log2(sets)
*	A = log2(associativity)
*
* @param[in]
*	Data to be written which is formatted by the appropriate macro:
*	L1CACHESW_FORMAT or L2CACHESW_FORMAT
*		 	
*/
void set_icrsw(uint32 data);
void set_dcrsw(uint32 data);

/*===========================================================================
** 	Function: L0 Instruction Cache Tag and Data Registers
** ==========================================================================
*/
/* @brief
 *	Instruction Cache Tag Register 0
 *	 	[31]	Tag T-State
 *	 	[30]	Tag J-State
 *	 	[29]	NS-Tag
 *	 	[28]	Reserved
 *	 	[27:08]	Tag Virtual Address[31:12]
 *		[7]		User Execute Permission
 *		[6]		Privilege Execute Permission
 *		[5]		Valid
 *		[4:0]	Reserved
 *
 *	Instruction Cache Read Data Register 0
 *		[31:0]	INST[31:0] Low 32 bits of the transformed instruction
 *
 *	Instruction Cache Read Data Register 1 
 *		[31:14]	HWX[17:0]	
 *		[13:12]	Reserved
 *		[11:04]	PDCD[7:0]	Pre-decoded Information
 *		[3:0]	INST[35:32]	High 4 bits of the transformed instruction
 *
 */
uint64 get_l0icrtr0(void);
uint64 get_l0icrdr0(void);
uint64 get_l0icrdr1(void);

uint64 get_icrtr0(void);
uint64 get_icrdr0(void);
uint64 get_icrdr1(void);

/*===========================================================================
** 	Function: L0 Instruction Cache Tag and Data Registers
** ==========================================================================
*/
/* @brief
 *	Data Cache Tag Register 0
 *		[31:12]	Tag Virtual Address
 *		[11:08]	Reserved
 *		[7]		Valid
 *		[6]		NS-TAG
 *		[5:0]	Reserved		
 *
 *	Data Cache Data Register 0
 *		[31:0]	Data from the cache line word
 *
 */
uint64 get_dcrtr0(void);
uint64 get_dcrdr0(void);

uint64 get_l0dcrtr0(void);
uint64 get_l0dcrdr0(void);

/*===========================================================================
** 	Function: GETSETS, GETWAYS, GETLINESIZE
** ==========================================================================
*/
/* @brief
 *	GetSets returns the number of sets in a particular cache.
 *	GetWays returns the number of ways in a particular cache.
 *	GetLineSize returns the number of lines in a particular cache.
 *
 * @param[in]
 *	The type of the cache. 
 *
 * @retval
 *	The sets or ways or lines in a particular cache		
 *
 *
uint32 GetSets(sdi_cache_type cache_type);
uint32 GetWays(sdi_cache_type cache_type);
uint32 GetLineSize(sdi_cache_type cache_type);
*/

/* =================================================================================
 * Function : GET_RESETPC
 * =================================================================================
 *
 * @brief
 * 	It returns the PC of CPU when the reset happened. This is valid only on Qualcomm 
 * 	custom CPUs and not generic ARM CPUs. At the time of this writing, only Krait supports
 * 	this API. 
 *
 * @param[in]
 * 	None
 *
 * @retval
 *	The PC when the reset happened.	
 */
uint64 GetResetPC(void);

/* =================================================================================
 * Function : SET_TPIDRURW / GET_TPIDRURW
 * =================================================================================
 *
 * @brief
 * 	It returns the thread and process ID of SW in user mode.  
 *
 * @param[in]
 * 	Value to set (on a write)
 *
 * @retval
 *	Value read (on a read)
 */
void set_tpidrurw(uint64 data);
uint64 get_tpidrurw(void);

/* =================================================================================
 * Function : SET_TPIDRPRW / GET_TPIDRPRW
 * =================================================================================
 *
 * @brief
 * 	It returns the thread and process ID of SW in privileged mode .  
 *
 * @param[in]
 * 	Value to set (on a write)
 *
 * @retval
 *	Value read (on a read)
 */
void set_tpidrprw(uint64 data);
uint64 get_tpidrprw(void);


/* =================================================================================
 * Function : SET_TPIDRR0 / GET_TPIDRR0
 * =================================================================================
 *
 * @brief
 * 	It returns the thread and process ID of SW executing in EL0 .  
 *
 * @param[in]
 * 	Value to set (on a write)
 *
 * @retval
 *	Value read (on a read)
 */

void set_tpidrr0(void);
uint64 get_tpidrr0(void);

/*===========================================================================
** 	Function: SET_CACHEOP / CACHEOP_FORMAT
** ==========================================================================
*/
/*!
* 
* @brief
*	API to program the device for a cache operation i.e. a read of the cache tag, 
*	data from any of the cache levels supported by the system.
*
* @param[in]
*   The data to be written
*/
/* Set method for the Dcache Data Read operation*/
void set_ramindexop(uint64 data);


/*===========================================================================
** 	Function: GET_DCACHEDATAREG / GET_ICACHEDATAREG
** ==========================================================================
*/
/*!
* 
* @brief
*	API to read the results of the cache operations. Instruction and Data caches
*	have different result registers
*	* 	
*/
uint64 get_dcachedatareg(uint32 count);
uint64 get_icachedatareg(uint32 count);

/*===========================================================================
** 	Function: SET_DCACHExxx_OP / SET_ICACHExxx_OP
** ==========================================================================
*/
/*!
* 
* @brief
*   APIs to perform operations on the caches (either instruction or data ) 
*   to read the data and tag RAMs.
*
*
* *	* 	
*/

void set_dcachedata_op(uint64 data);
void set_dcachetag_op(uint64 data);
void set_icachedata_op(uint64 data);
void set_icachetag_op(uint64 data);
uint64 get_cacheop_result();

/*===========================================================================
** 	Function: GET_CURRENT_CPUNAME / CLUSTER / CPU / NODE
** ==========================================================================
*/
/*!
* 
* @brief
*	API to get the current cpu name. Implementation defined values are used 
*	by this api. This api will have to be updated when new cpus are used.
*
*
*@param[in]
*   None
*   ===========================================================================*/
uint32 get_current_cpuname();
uint32 get_current_cluster();
uint32 get_current_cpu_seq();
uint32 get_current_node();

#endif /* _SYSDBG_SYSREG_H */
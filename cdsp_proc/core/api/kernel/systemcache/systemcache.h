#ifndef SYSTEMCACHE_H
#define SYSTEMCACHE_H
/*=================================================================================================

                      S Y S T E M    C A C H E    H E A D E R    F I L E

DESCRIPTION
   APIs for the System Cache Driver


Copyright (c) 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
=================================================================================================*/

/*=================================================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/api/kernel/systemcache/systemcache.h#1 $

=================================================================================================*/

/*=================================================================================================

                           INCLUDE FILES

=================================================================================================*/
#include "comdef.h"
#include "errno_codes.h"

/*=================================================================================================

                            Definitions

=================================================================================================*/
/** 
Definition of system cache clients' use case identifiers
*/
#define  SC_USE_CASE_ID_CPUSS              1
#define  SC_USE_CASE_ID_VIDEO_SC0          2        
#define  SC_USE_CASE_ID_VIDEO_SC1          3        
#define  SC_USE_CASE_ID_ROTATOR            4        
#define  SC_USE_CASE_ID_VOICE              5 
#define  SC_USE_CASE_ID_AUDIO              6
#define  SC_USE_CASE_ID_MODEM_HIGH_PRIO    7 
#define  SC_USE_CASE_ID_MODEM              8         
#define  SC_USE_CASE_ID_MODEM_HW           9        
#define  SC_USE_CASE_ID_COMPUTE            10          
#define  SC_USE_CASE_ID_GPU_HTW            11       
#define  SC_USE_CASE_ID_GPU                12         
#define  SC_USE_CASE_ID_MMU_HTW            13      
#define  SC_USE_CASE_ID_SENSOR             14        
#define  SC_USE_CASE_ID_COMPUTE_DMA        15  
#define  SC_USE_CASE_ID_DISPLAY            16
#define  SC_USE_CASE_ID_VIDEO_FW           17
#define  SC_USE_CASE_ID_CAMERA_FW          18
#define  SC_USE_CASE_ID_MODEM_TCM          19
#define  SC_USE_CASE_ID_MODEM_HP_FIX       20
#define  SC_USE_CASE_ID_MODEM_PAGING       21
#define  SC_USE_CASE_ID_AUDIO_HW           22


/*==================================================================================================

                     FUNCTION DECLARATIONS

==================================================================================================*/
#ifdef __cplusplus
   extern "C"
   {
#endif

/*=================================================================================================
         REX TASK ATTRIBUTE APIs
=================================================================================================*/

/**
This function determines whether system cache is implemented in SoC and returns the result
in the boolean out parameter.

@param[out]    Valid pointer to hold the result.

@return
0               -- Indicates the operation was successful.
Negative Value  -- Indicates the operation failed due to an error.


@dependencies
None.
*/
int SysCacheIsImplemented(boolean *result);

/**
This function retrieves the sub-cache id and the maximum allocated size for the SCID
for a given usecase and returns that information.

@param[in]   Use case identifier.
@param[out]  SCID corresponding to the use-case ID.
@param[out]  Max size allocated for the use-case  in terms of bytes.

@return
0              -- Indicates the operation was successful.
Negative Value -- Indicates the operation failed due to an error.
-EOPNOTSUPP    -- Operation not supported, possibly because SC is not implemented

@dependencies
None.
*/
int SysCacheGetSCID(uint32 use_case, uint32 *scid, uint32 *max_size);

/**
This function retrieves the current allocation in terms of lines for a given SCID

@param[in]   SCID
@param[out]  Current size allocated for the SCID in terms of bytes.

@return
0                -- Indicates the operation was successful.
Negative Value   -- Indicates the operation failed due to an error.
-EOPNOTSUPP      -- Operation not supported, possibly because SC is not implemented


@dependencies
None.
*/
int SysCacheGetCurrAllocation(uint32 scid, uint32 *curr_size);

/**
This function activates or deactivates a given SCID in System Cache

@param[in]  SCID
@param[in]  boolean indicating whether to activate the SCID. TRUE for activate, false for deactivate.

@return
0                -- Indicates the operation was successful.
Negative Value   -- Indicates the operation failed due to an error.
EOPNOTSUPP       -- Operation not supported, possibly because SC is not implemented


@dependencies
None.
*/
int SysCacheActDeactSCID(uint32 scid, boolean activate);

/**
This function programs the SCID at the specified index of the SCID
generation register (ASCid_mapping_x) for Q6 in the DSP subsystem.

@param[in]  SCID
@param[in]  index at which the scid needs to be programmed.

@return
0                -- Indicates the operation was successful.
Negative Value   -- Indicates the operation failed due to an error.
EOPNOTSUPP       -- Operation not supported, possibly because SC is not implemented


@dependencies
None.
*/
int SysCacheProgramSCID(uint32 scid, uint32 index); 

/**
This function deactivates all SCIDs managed by the DSP subsystem. This API is 
indended to be invoked from fatal error handler.

@return
0                -- Indicates the operation was successful.
Negative Value   -- Indicates the operation failed due to an error.
EOPNOTSUPP       -- Operation not supported, possibly because SC is not implemented


@dependencies
None.
*/
int SysCacheDeactivateAllSCIDs(void);

#ifdef __cplusplus
   }
#endif

#endif /* END SYSTEMCACHE_H */

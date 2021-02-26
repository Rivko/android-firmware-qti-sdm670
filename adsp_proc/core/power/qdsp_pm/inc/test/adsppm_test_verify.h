/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_test.h
@brief: Header for ADSPPM wrapper for calls to MMPM.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/inc/test/adsppm_test_verify.h#1 $
*/


/**
 * ADSPPM Test Verify
 *
 * This file contains all the verification related functions. It
 * breaks down to three secontions:
 * 1. ENUMS and CONSTANTS: First section contains enums and
 * constants.
 *
 * 2. DATA STRUCTURE TYPE: contains the data structure types
 *
 * 3. FUNCTIONS & API: Contains functions and API
 * implementation.
 */



#ifndef ADSPPM_TEST_VERIFY_H
#define ADSPPM_TEST_VERIFY_H

#include "mmpm.h"
#include "adsppm.h"


/************************** ENUMS & CONSTANT *****************************
 ========================================================================*/

/* Bandwidth calculation:
1. Ext_Ahb_Master  -> DDR Slave   : 32bits
2. Ext Adsp_Master  -> DDR Slave  : 32bits
3. ADSP Master -> Preriphero NOC  : 16bits  */
#define ADSPPM_TEST_ADSP_DDR_WIDTH_LIMIT     32
#define ADSPPM_TEST_SNOC_REGULAR_WIDTH_LIMIT 16
#define ADSPPM_TEST_BIMC_REGULAR_WIDTH_LIMIT 8
#define ADSPPM_TEST_19_MHZ                   19200000

typedef enum
{
    ADSPPM_RESRC_CORE_CLK,
    ADSPPM_RESRC_LPASS_CLK,
    ADSPPM_RESRC_BW_BIMC,
    ADSPPM_RESRC_BW_SNOC,
    ADSPPM_RESRC_AHBIX_CLK,
    ADSPPM_RESRC_AHBE_CLK,
    ADSPPM_RESRC_SLEEP_LATENCY,
    ADSPPM_RESRC_MAX
}AdsppmResrcID;

typedef enum
{
    ADSPPM_RESRC_CLK_RGM_TYPE,
    ADSPPM_RESRC_NPA_TYPE,
    ADSPPM_RESRC_TYPE_MAX
}AdspResrcType;



/************************ DATA STRUCTURE TYPE ****************************
 ========================================================================*/

/* This function read the HW and compare the results with expected info */
typedef void (*AdspHWVerifyFn)(uint32 clientId, MmpmRscExtParamType * params, void *expectedParams, AdsppmStatusType * result);

/* This function compute the user input to the actual HW values. For example,
   computing from bandwidth mBytes/s to KHz for BIMC, SNOC, etc. */
typedef boolean (*AdspHWComputeFn)(MmpmRscExtParamType * params, void *expectedParams, void * infoParams, uint32 * result);

/* This function returns the hwValue read from devices or other sub-system */
typedef void (*AdspHWReadFn)(uint32 * hwValue, void * hwParams);

typedef struct
{
    AdspHWVerifyFn        utHWVerifyFn;
}AdsppmUTExtRscPropertiesType;

typedef struct
{
    AdspHWReadFn          utHWReadFn;
    AdspHWComputeFn       utHWComputeFn;
    AdspResrcType         type;
    char*                 npaNodeName;
}AdsppmUTRscPropertiesType;


typedef struct
{
    MmpmCoreIdType         coreId;
    MmpmCoreInstanceIdType instanceId;
    MmpmClkIdLpassType     clkId;
}AdsppmUT_Clk_Params;


typedef struct
{
    uint32                 busBW;
    AdsppmResrcID          busID;
    uint64                 snocFloor;
}AdsppmUT_Bus_Params;

typedef struct
{
    uint64 Ab;         //!< Average BW in bytes per second
    uint64 Ib;         //!< Instantaneous BW in bytes per second
    uint32 latencyNs;  //!< latency in nano sec
}AdsppmUT_Verify_BW_Params;
/************************** FUNCTIONS & API ******************************
 ========================================================================*/

boolean Adsppm_Verify(uint32 clientId, MmpmRscExtParamType * params, void * expectedParams);
void AdsppmUTInit(void );

#endif /* ADSPPM_TEST_H */


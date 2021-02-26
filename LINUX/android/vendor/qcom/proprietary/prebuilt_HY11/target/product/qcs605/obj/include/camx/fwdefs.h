//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// @file  fwdefs.h
// @brief Common FW driver data types.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// @Edit History
// when              who           what,where,why
// -----             ---           -------------
// 10/29/2015        opolonsk      Initial version
//-----------------------------------------------------------------------------
#ifndef _FWDEFS_H_
#define _FWDEFS_H_

//-----------------------------------------------------------------------------
//  Include Files
//-----------------------------------------------------------------------------
#include "cdmpayload.h"
#include "icpcomdef.h"

/**
 * @def ALLOW_STRIPING_IN_FW
 *
 * @brief When defined, FW call striping library in FW in case it was not executed prior to image process command.
 *        In not defined, and FW process image is called without SL output, FW will return with failure.
 */
//#define ALLOW_STRIPING_IN_FW

//-----------------------------------------------------------------------------
//  Definitions and Constants
//-----------------------------------------------------------------------------
#define MAX_HFR_GROUP   (16)

//------------------------------------------------------------------------
// FW return code value
//------------------------------------------------------------------------
typedef uint32_t FWSTATUS;

//------------------------------------------------------------------------
// An opaque user argument that can passed to the firmware
//------------------------------------------------------------------------
typedef uint64_t FWUSERARG;

//------------------------------------------------------------------------
// Data type for storing SMMU-mapped address provided by HLOS
//------------------------------------------------------------------------
typedef UINTPTR32 SMMU_ADDR;

//------------------------------------------------------------------------
// Possible FWSTATUS are defined below. They are separated into error codes
// from High-level FW driver and Low-level FW driver.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// High-level driver error codes
//------------------------------------------------------------------------
#define HLSTATUS_SUCCESS                0U
#define HLSTATUS_ERROR_BASE             (HLSTATUS_SUCCESS    + 0xA0000)
#define HLSTATUS_ERROR_GENERAL          (HLSTATUS_ERROR_BASE + 0x00000)
#define HLSTATUS_INVALID_PARAM          (HLSTATUS_ERROR_BASE + 0x00001)
#define HLSTATUS_OUT_OF_MEMORY          (HLSTATUS_ERROR_BASE + 0x00002)
#define HLSTATUS_NOT_SUPPORTED          (HLSTATUS_ERROR_BASE + 0x00003)
#define HLSTATUS_UNIMPLEMENTED          (HLSTATUS_ERROR_BASE + 0x00004)
#define HLSTATUS_PENDING                (HLSTATUS_ERROR_BASE + 0x00005)
#define HLSTATUS_ABORTED                (HLSTATUS_ERROR_BASE + 0x00006)
#define HLSTATUS_TIMEOUT                (HLSTATUS_ERROR_BASE + 0x00007)
#define HLSTATUS_CDM_ERROR              (HLSTATUS_ERROR_BASE + 0x00008)
#define HLSTATUS_HW_VIOLATION           (HLSTATUS_ERROR_BASE + 0x00009)
//------------------------------------------------------------------------
// Low-level driver error codes
//------------------------------------------------------------------------
#define LLSTATUS_SUCCESS                0U
#define LLSTATUS_ERROR_BASE             (LLSTATUS_SUCCESS    + 0xB0000)
#define LLSTATUS_ERROR_GENERAL          (LLSTATUS_ERROR_BASE + 0x00000)
#define LLSTATUS_INVALID_PARAM          (LLSTATUS_ERROR_BASE + 0x00001)
#define LLSTATUS_OUT_OF_MEMORY          (LLSTATUS_ERROR_BASE + 0x00002)
#define LLSTATUS_NOT_SUPPORTED          (LLSTATUS_ERROR_BASE + 0x00003)
#define LLSTATUS_UNIMPLEMENTED          (LLSTATUS_ERROR_BASE + 0x00004)
#define LLSTATUS_PENDING                (LLSTATUS_ERROR_BASE + 0x00005)
#define LLSTATUS_ABORTED                (LLSTATUS_ERROR_BASE + 0x00006)
#define LLSTATUS_MALLOC_ERROR           (LLSTATUS_ERROR_BASE + 0x00007)
#define LLSTATUS_UNINITIALIZED          (LLSTATUS_ERROR_BASE + 0x00008)
#define LLSTATUS_CDM_ERROR              (LLSTATUS_ERROR_BASE + 0x00009)
#define LLSTATUS_HW_TIMEOUT             (LLSTATUS_ERROR_BASE + 0x0000a)
#define LLSTATUS_HW_VIOLATION           (LLSTATUS_ERROR_BASE + 0x0000b)

//------------------------------------------------------------------------
// This is a special value used in CdmProgram uID field to indicate
// that the CdmProgram does not contain unique content. FW should not
// try to optimize by comparing current and previous UID to determine
// if re-programming can be avoided.
//------------------------------------------------------------------------
#define INVALID_UID                     0

//------------------------------------------------------------------------
// The values below are used in CdmProgramArray allocator field to
// indicate whether the CdmProgramArray is allocated by HLOS or from
// within firmware.
//------------------------------------------------------------------------
#define HLOS_ALLOCATOR                  0
#define HLFW_ALLOCATOR                  1

//-----------------------------------------------------------------------------
//  Type Definitions
//-----------------------------------------------------------------------------
typedef enum _ProgramType
{
    /* LUT type program types:
       Insert/Delete/Reorder with caution. The LUT type program types are used to
       index into another table which has corresponding LUT bank sel addresses.
       HLOS NEEDS to use these program types when programming LUTs to HW so that
       FW can properly handle bank flipping for each module.
    */
    PROGRAM_TYPE_LUT_START = 0,

    BPS_PEDESTAL_LUT_PROGRAM = PROGRAM_TYPE_LUT_START,
    BPS_LINEARIZATION_LUT_PROGRAM,
    BPS_BPC_PDPC_LUT_PROGRAM,
    BPS_GIC_LUT_PROGRAM,
    BPS_ABF_LUT_PROGRAM,
    BPS_LENS_ROLLOFF_LUT_PROGRAM,
    BPS_GTM_LUT_PROGRAM,
    BPS_GLUT_LUT_PROGRAM,
    BPS_HNR_LUT_PROGRAM,
    IPE_ICA1_LUT_PROGRAM,
    IPE_ICA2_LUT_PROGRAM,
    IPE_LTM_LUT_PROGRAM,
    IPE_GAMMA_GLUT_LUT_PROGRAM,
    IPE_2D_LUT_LUT_PROGRAM,
    IPE_ASF_LUT_PROGRAM,
    IPE_GRA_LUT_PROGRAM,

    PROGRAM_TYPE_LUT_MAX,

    /* Non-LUT type programs used by High Level Driver in FW */
    IPE_ICA1_PROGRAM = PROGRAM_TYPE_LUT_MAX,
    IPE_ICA2_PROGRAM,
    IPE_ANR_CYLPF_PROGRAM,
    IPE_TF_PROGRAM,

    /* Generic program type
       It can be used by anyone who does not want to specify what is being
       programmed in the CDMProgram. The 'uID' flag in the CDMProgram will
       be ignored and will always be programmed to HW without assuming
       the same content were programmed before. */
    PROGRAM_TYPE_GENERIC,

    /* HLOS can define its own ProgramType beyond this point in its own header file
       using PROGRAM_TYPE_HLOS_START as the starting value. */
    PROGRAM_TYPE_HLOS_START,


    /* Special program type to denote the CDM program should be skipped or ignored
       for programming by FW */
    PROGRAM_TYPE_SKIP = 0xFF

} ProgramType;

#pragma pack(push)
#pragma pack(1)

typedef struct _CdmProgram
{
    CDM_BASE_LENGHT cdmBaseAndLength;
    uint32_t        programType;                // Program Type, values defined by LLFW in
                                                // cdm_prog_xxx.h or it can be any custom
                                                // value defined by HLFW/HLOS
                                                // uint32_t is used only for packing
                                                // valid range: 0-255
    uint32_t        uID;                        // Unique program ID
                                                // uint32_t is used only for packing
                                                // valid range: 0-255
    uint32_t        hasSingleReg;               // The CDM program contains one or more
                                                // reg-writes which the destination
                                                // registers are non-double-buffered
                                                // uint32_t is used only for packing
                                                // valid value: 0 or 1
    uint32_t         bufferAllocatedInternally; // An indication whether the FW allocated this dynamically
                                                   // or used the cdm buffer uint32_t is used only for packing
                                                // valid value: 0 or 1
} CdmProgram;

typedef struct _CDMProgramArray
{
    uint32_t        allocator;      // Indicates where the original array is
                                    // coming from 0 – HLOS_ALLOCATOR, 1 – HLFW_ALLOCATOR
    uint32_t        numPrograms;    // Number of programs in the following array

    // Array of CDMProgram with 1 or more entries. User can allocate larger size by:
    // CDMProgramArray* array = (CDMProgramArray*)malloc(sizeof(CDMProgramArray) + (numPrograms-1) * sizeof(CdmProgram))
    // array->numPrograms = numPrograms;
    CdmProgram      programs[1];
} CDMProgramArray;

#pragma pack(pop)

#endif // _FWDEFS_H_

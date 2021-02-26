/** @file MemmapLib.h
   
  Implementation of interface function supported by MemmapLib

  Copyright (c) 2011-2012, Qualcomm Technologies Inc. All rights reserved. 
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/20/13   nk      Support for error fatal on XPU Violations
 10/29/12   nk      Added guid for DBI mem
 05/07/12   jz      Updated for subsystem memory report
 10/06/11   jz      Initial Revision

=============================================================================*/

#ifndef __MEMMAP_LIB_H__
#define __MEMMAP_LIB_H__

// UEFI preallocated/reserved DDR mem region
#define EFI_UEFI_MEM_GUID     { 0x9158ee69, 0xf5b6, 0x4ad7, { 0xa4, 0x3d, 0x9d, 0x15, 0x19, 0x32, 0x8e, 0xfa } }
// UEFI preallocated/reserved DDR mem region 2
#define EFI_UEFI_MEM_2_GUID   { 0x9aee84d5, 0x8d08, 0x4847, { 0xad, 0x90, 0x79, 0xd9, 0xbb, 0x6c, 0x26, 0xe4 } }
// Subsystem reserved DDR mem region
#define EFI_SUBSYS_RESERVED_MEM_GUID { 0x1978e5c0, 0x1584, 0x47e5, { 0x97, 0x90, 0x7f, 0x89, 0xba, 0x76, 0x8a, 0xe9 } }

#define EFI_SUBSYS_RESERVED_2_MEM_GUID { 0x1ed5a165, 0x4556, 0x4c2b, { 0xa7, 0xff, 0xd1, 0xcc, 0xe0, 0xc1, 0x6c, 0xa3} }

// System IMEM region
#define EFI_SYS_IMEM_GUID     { 0xad091e6f, 0xc9c5, 0x4e98, { 0x82, 0xd7, 0x9a, 0x51, 0x21, 0x11, 0x6d, 0x5f } }
// MM IMEM region
#define EFI_MM_IMEM_GUID      { 0x90f278b7, 0x219c, 0x4dd3, { 0x98, 0xc4, 0x30, 0x7d, 0xf7, 0x59, 0x91, 0xf6 } }
// RPM Code RAM region
#define EFI_RPM_CODE_RAM_GUID { 0x081b8ef5, 0x36cd, 0x4560, { 0x9f, 0x59, 0xf8, 0xb7, 0x14, 0xb5, 0x10, 0xd0 } }
// RPM Msg RAM region
#define EFI_RPM_MSG_RAM_GUID  { 0x3f8c9ac8, 0x32ff, 0x44ef, { 0xa1, 0x03, 0x6c, 0x0a, 0x1f, 0xcb, 0x5c, 0x72 } }
// SPS RAM region
#define EFI_SPS_RAM_GUID      { 0x86805103, 0x6525, 0x4d08, { 0x8c, 0x31, 0xd9, 0x99, 0x46, 0x43, 0x97, 0xeb } }
// SPS Buff RAM region
#define EFI_SPS_BUFF_RAM_GUID { 0xbd7180c0, 0x6a60, 0x47a7, { 0x8b, 0xe9, 0x09, 0x6d, 0x82, 0x2f, 0xf9, 0x60 } }
// SPS Pipe mem region
#define EFI_SPS_PIPE_RAM_GUID { 0xd078291e, 0x4fd7, 0x43ce, { 0xaa, 0xfe, 0xa9, 0x9d, 0x9c, 0xfb, 0xb7, 0xca } }
// MDSP RAM A region
#define EFI_MDSP_RAM_A_GUID   { 0x72cdc567, 0x4a06, 0x4cf6, { 0x9b, 0x8c, 0x23, 0x9e, 0xf1, 0x37, 0xbb, 0xc9 } }
// MDSP RAM B region
#define EFI_MDSP_RAM_B_GUID   { 0xef06b39e, 0x85ad, 0x4060, { 0xb2, 0x9d, 0x36, 0x74, 0x95, 0x2a, 0xfc, 0x57 } }
// MDSP RAM C region
#define EFI_MDSP_RAM_C_GUID   { 0x54974bf5, 0x831f, 0x4c9a, { 0x92, 0xcf, 0x09, 0xb0, 0x0e, 0x2d, 0x33, 0xb5 } }
// LPASS mem region
#define EFI_LPASS_MEM_GUID    { 0x05123732, 0x54cc, 0x4ca9, { 0x9d, 0x0a, 0x5b, 0x69, 0xe6, 0xaa, 0xec, 0x93 } }
// QDSS ETB registers
#define EFI_QDSS_ETB_REG_GUID { 0x7f0afa2b, 0x8cad, 0x4f28, { 0xaf, 0x0b, 0x54, 0x4a, 0xb0, 0xa6, 0x72, 0xe5 } }
// ETB region
#define EFI_ETB_REGION_GUID   { 0x2bf2065a, 0xcf3f, 0x4cea, { 0x81, 0xa0, 0xcb, 0x98, 0x0c, 0x96, 0xd6, 0x76 } }
// Misc. HW registers region
#define EFI_MISC_HW_REG_GUID  { 0xcc56f631, 0xb275, 0x4aeb, { 0x81, 0x2e, 0xb0, 0x25, 0x7e, 0x6f, 0xc1, 0xab } }
// DBI Imgae
#define EFI_DBI_MEM_GUID      { 0x286A649E, 0x4328, 0x1198, { 0x1E, 0x41, 0x48, 0x91, 0x6A, 0xBB, 0x58, 0x44 } }

extern EFI_GUID gEfiUEFIMemGuid;
extern EFI_GUID gEfiUEFIMem2Guid;
extern EFI_GUID gEfiSubsysMemGuid;
extern EFI_GUID gEfiSystemImemGuid;
extern EFI_GUID gEfiMmImemGuid;
extern EFI_GUID gEfiRpmCodeRamGuid;
extern EFI_GUID gEfiRpmMsgRamGuid;
extern EFI_GUID gEfiSpsRamGuid;
extern EFI_GUID gEfiSpsBuffRamGuid;
extern EFI_GUID gEfiSpsPipeMemGuid;
extern EFI_GUID gEfiMdspRamAGuid;
extern EFI_GUID gEfiMdspRamBGuid;
extern EFI_GUID gEfiMdspRamCGuid;
extern EFI_GUID gEfiLpassMemGuid;
extern EFI_GUID gEfiQdssEtbRegGuid;
extern EFI_GUID gEfiEtbGuid;
extern EFI_GUID gEfiMiscHWRegGuid;

//config for enabling/disabling xpu violations as error fatal
typedef enum
{
  TZBSP_XPU_VIOLATION_ERR_FATAL_ENABLE  = 0,
  TZBSP_XPU_VIOLATION_ERR_FATAL_DISABLE = 1,
  TZBSP_XPU_VIOLATION_ERR_FATAL_NOOP    = 2,
} tzbsp_xpu_violation_err_fatal_t;

//SCM Sys call request structure
typedef struct syscall_req_s
{
   /** The total length of available memory for request and response */
   UINT32 len;

   /** Start of the command to be executed. The offset is computed from
        the start of this structure. */
   UINT32 command_start_off;

   /* The start offset of memory where the response will be written to.
      The offset is computed from the start of this structure */
   UINT32 response_write_off;

   /* A unique identifier that established which command is followed after
      this structure */
   UINT32 command_id;
} __attribute__ ((packed)) syscall_req_t;

//SCM sys call response structure
typedef struct syscall_rsp_s
{
   /** The total length of the response */
   UINT32 len;

   /* The start offset of memory where the response info will be written to.
      The offset is computed from the start of this structure */
   UINT32 response_start_off;

   /* The status of the syscall_req executed.
    * 0 means command not complete, 1 means command is complete */
   INT32 status;
} __attribute__ ((packed)) syscall_rsp_t; 

//SCM sys call cmd structure for dump
typedef struct tzbsp_dump_req_s
{
  /** Common request structure */
  syscall_req_t           common_req;
  /** Address of WDT reset context dump buffer. When non-secure WDT expires,
   * the CPU contexts will be saved to this buffer. The entire buffer must be
   * in non-secure memory. */
  UINT32*                       addr;
  /** Size of the WDT reset context dump buffer in bytes. */
  UINT32                        size;
   /** Common response structure */
  syscall_rsp_t           common_rsp;
} __attribute__ ((packed)) tzbsp_dump_req_t , *ptzbsp_dump_req_t;

// SCM sys call cmd structure for XPU Violations

typedef struct tzbsp_xpu_req_s
{
  /** Common request structure */
  syscall_req_t                    common_req;
  /** xpu violation err fatal config parameter.
   *  Enable/Disable/Noop */
  tzbsp_xpu_violation_err_fatal_t  config;
  /** Not used  */
  UINT32                           spare;
   /** Common response structure */
  syscall_rsp_t                    common_rsp;
   /** Out Status 1-> violations are err fatals, 0->violations are not
 *                        err fatals */
  tzbsp_xpu_violation_err_fatal_t status;
} __attribute__ ((packed)) tzbsp_xpu_req_t , *ptzbsp_xpu_req_t;


/**
  Returns the memory map table with the data specified in PCDs
  defined in target specific .dsc file.
  
  @param Size            Pointer to the size in bytes of the MemoryMap buffer
  @param MemoryMap       Ponter to the buffer for subsystem memory map, which is
                         an array of EFI_SUBSYSTEM_MEMORY_DESCRIPTOR
  @param DescriptorCount Pointer to the location containing the descriptor count
  
  @retval RETURN_SUCCESS       The map was initialized.
  @retval RETURN_DEVICE_ERROR  EFI_INVALID_PARAMETER The parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL The supplied MemoryMap buffer was too small.
                               The current buffer size needed to hold the
                               memory map is returned in Size.
**/
EFI_STATUS
EFIAPI
GetSubsysMemMap(
  IN OUT UINTN                           *Size,
  IN OUT EFI_SUBSYSTEM_MEMORY_DESCRIPTOR *MemoryMap,
  OUT UINTN                              *DescriptorCount
);

/**
  Returns the memory map table with the data initialized
  
  @retval EFI_SUCCESS       The map was initialized.
**/
EFI_STATUS
EFIAPI
MemDumpInit(
  VOID
  );

#endif

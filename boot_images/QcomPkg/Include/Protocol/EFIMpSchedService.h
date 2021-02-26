/** @file EFIMpSchedService.h

  Provides scheduler service for MP subsystem.

**/

#ifndef _MPSCHED_SERVICE_PROTOCOL_H_
#define _MPSCHED_SERVICE_PROTOCOL_H_

#include <Uefi.h>
#include <Library/AuxKernel.h>

extern EFI_GUID gQcomMpSchedProtocolGuid;

#define EFI_MPSCHED_PROTOCOL_REVISION       0x00010000

#define EFI_MP_SCHED_PROTOCOL_GUID \
  { \
    0x0f8d728e, 0x3c6c, 0x11e5, {0xb8, 0xdc, 0x23, 0xdf, 0xe4, 0x87, 0x66, 0xbf} \
  }

typedef struct _EFI_MP_SCHED_PROTOCOL EFI_MP_SCHED_PROTOCOL;

///
/// This bit is used in the StatusFlag field of EFI_PROCESSOR_INFORMATION and
/// indicates whether the processor is playing the role of BSP. If the bit is 1,
/// then the processor is BSP. Otherwise, it is AP.
///
#define PROCESSOR_AS_BSP_BIT         0x00000001

///
/// This bit is used in the StatusFlag field of EFI_PROCESSOR_INFORMATION and
/// indicates whether the processor is enabled. If the bit is 1, then the
/// processor is enabled. Otherwise, it is disabled.
///
#define PROCESSOR_ENABLED_BIT        0x00000002

///
/// This bit is used in the StatusFlag field of EFI_PROCESSOR_INFORMATION and
/// indicates whether the processor has a task scheduled. If the bit is 1, then the
/// processor is busy. Otherwise, new task may be assigned
///
#define PROCESSOR_BUSY_BIT           0x00000004

///
/// Structure that describes information about a logical CPU.
///
typedef struct {
  /*
     Number from [0..NumCpus]
  */
  UINT32                     CpuNumber;
  /*
     Cpu Flags indicate the following status:
     Bit 0: If the cpu is BSP or AP
     Bit 1: If the cpu is enabled or disabled
     Bit 2: If the cpu is in use.
     Bits 3..31 are reserved and must be 0.
   */
  UINT32                     StatusFlag;
  /*
     CpuId: Affinity0[0:8], Affinity1[9:15], Affinity2[16:23], Affinity3[24:32]
   */
  UINT32                     CpuId;
} EFI_CPU_INFORMATION;

/**
  This service retrieves the number of logical processor in the platform
  and the number of those logical processors that are enabled on this boot.
  This service may only be called from the BSP.

  This function is used to retrieve the following information:
    - The number of logical processors that are present in the system.
    - The number of enabled logical processors in the system at the instant
      this call is made.

  Because MP Service Protocol provides services to enable and disable processors
  dynamically, the number of enabled logical processors may vary during the
  course of a boot session.

  If this service is called from an AP, then EFI_DEVICE_ERROR is returned.
  If NumberOfProcessors or NumberOfEnabledProcessors is NULL, then
  EFI_INVALID_PARAMETER is returned. Otherwise, the total number of processors
  is returned in NumberOfProcessors, the number of currently enabled processor
  is returned in NumberOfEnabledProcessors, and EFI_SUCCESS is returned.

  @param[in]  This                        A pointer to the
                                          EFI_MP_SHCED_PROTOCOL instance.
  @param[out] NumCpus                     Pointer to the total number of logical
                                          processors in the system, including the BSP
                                          and disabled APs.
  @param[out] NumberOfEnabledProcessors   Pointer to the number of enabled logical
                                          processors that exist in system, including
                                          the BSP.

  @retval EFI_SUCCESS             NumCpus and EnabledCpusMask hold valid data
  @retval EFI_INVALID_PARAMETER   NumCpus is NULL.
  @retval EFI_INVALID_PARAMETER   EnabledCpusMask is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MPSCHED_GET_ENABLED_CPUS)(
  IN  EFI_MP_SCHED_PROTOCOL  *This,
  OUT UINTN                  *NumCpus,
  OUT UINT32                 *EnabledCpusMask
  );

/**
  Gets detailed MP-related information on the requested processor at the
  instant this call is made. This service may only be called from the BSP.

  This service retrieves detailed MP-related information about any processor
  on the platform. Note the following:
    - The processor information may change during the course of a boot session.
    - The information presented here is entirely MP related.

  Information regarding the number of caches and their sizes, frequency of operation,
  slot numbers is all considered platform-related information and is not provided
  by this service.

  @param[in]  This                  A pointer to the EFI_MP_SERVICES_PROTOCOL
                                    instance.
  @param[in]  ProcessorNumber       The handle number of processor.
  @param[out] ProcessorInfoBuffer   A pointer to the buffer where information for
                                    the requested processor is deposited.

  @retval EFI_SUCCESS             Processor information was returned.
  @retval EFI_DEVICE_ERROR        The calling processor is an AP.
  @retval EFI_INVALID_PARAMETER   ProcessorInfoBuffer is NULL.
  @retval EFI_NOT_FOUND           The processor with the handle specified by
                                  ProcessorNumber does not exist in the platform.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MPSCHED_GET_CPU_INFO)(
  IN  EFI_MP_SCHED_PROTOCOL   *This,
  OUT EFI_CPU_INFORMATION     *CpuInfoBuffer
  );


/**
  This service lets the caller get one enabled AP to execute a caller-provided
  function. The caller can request the BSP to either wait for the completion
  of the AP or just proceed with the next task by using the EFI event mechanism.
  This service may only be called from the BSP.

  This function is used to dispatch one enabled AP to the function specified by
  Procedure passing in the argument specified by ProcedureArgument1,2.  If WaitEvent
  is NULL, execution is in blocking mode. The BSP waits until the AP finishes or
  TimeoutInMicroSecondss expires. Otherwise, execution is in non-blocking mode.
  BSP proceeds to the next task without waiting for the AP. If a non-blocking mode
  is requested after the UEFI Event EFI_EVENT_GROUP_READY_TO_BOOT is signaled,
  then EFI_UNSUPPORTED must be returned.

  If the timeout specified by TimeoutInMicroseconds expires before the AP returns
  from Procedure, then execution of Procedure by the AP is terminated. The AP is
  available for subsequent calls to EFI_MP_SERVICES_PROTOCOL.StartupThisAP().

  @param[in]  This                    A pointer to the EFI_MP_SERVICES_PROTOCOL
                                      instance.
  @param[in]  Procedure               A pointer to the function to be run on
                                      enabled APs of the system. See type
                                      EFI_AP_PROCEDURE.
  @param[in]  ProcessorNumber         The handle number of the AP. The range is
                                      from 0 to the total number of logical
                                      processors minus 1. The total number of
                                      logical processors can be retrieved by
                                      EFI_MP_SERVICES_PROTOCOL.GetNumberOfProcessors().
  @param[in]  WaitEvent               The event created by the caller with CreateEvent()
                                      service.  If it is NULL, then execute in
                                      blocking mode. BSP waits until all APs finish
                                      or TimeoutInMicroSeconds expires.  If it's
                                      not NULL, then execute in non-blocking mode.
                                      BSP requests the function specified by
                                      Procedure to be started on all the enabled
                                      APs, and go on executing immediately. If
                                      all return from Procedure or TimeoutInMicroSeconds
                                      expires, this event is signaled. The BSP
                                      can use the CheckEvent() or WaitForEvent()
                                      services to check the state of event.  Type
                                      EFI_EVENT is defined in CreateEvent() in
                                      the Unified Extensible Firmware Interface
                                      Specification.
  @param[in]  TimeoutInMicrosecsond   Indicates the time limit in microseconds for
                                      APs to return from Procedure, either for
                                      blocking or non-blocking mode. Zero means
                                      infinity.  If the timeout expires before
                                      all APs return from Procedure, then Procedure
                                      on the failed APs is terminated. All enabled
                                      APs are available for next function assigned
                                      by EFI_MP_SERVICES_PROTOCOL.StartupThisAP().
                                      If the timeout expires in blocking mode,
                                      BSP returns EFI_TIMEOUT.  If the timeout
                                      expires in non-blocking mode, WaitEvent
                                      is signaled with SignalEvent().
  @param[in]  ProcedureArgument       The parameter passed into Procedure for
                                      all APs.
  @param[out] Finished                If NULL, this parameter is ignored.  In
                                      blocking mode, this parameter is ignored.
                                      In non-blocking mode, if AP returns from
                                      Procedure before the timeout expires, its
                                      content is set to TRUE. Otherwise, the
                                      value is set to FALSE. The caller can
                                      determine if the AP returned from Procedure
                                      by evaluating this value.

  @retval EFI_SUCCESS             In blocking mode, specified AP finished before
                                  the timeout expires.
  @retval EFI_SUCCESS             In non-blocking mode, the function has been
                                  dispatched to specified AP.
  @retval EFI_UNSUPPORTED         A non-blocking mode request was made after the
                                  UEFI event EFI_EVENT_GROUP_READY_TO_BOOT was
                                  signaled.
  @retval EFI_DEVICE_ERROR        The calling processor is an AP.
  @retval EFI_TIMEOUT             In blocking mode, the timeout expired before
                                  the specified AP has finished.
  @retval EFI_NOT_READY           The specified AP is busy.
  @retval EFI_NOT_FOUND           The processor with the handle specified by
                                  ProcessorNumber does not exist.
  @retval EFI_INVALID_PARAMETER   ProcessorNumber specifies the BSP or disabled AP.
  @retval EFI_INVALID_PARAMETER   Procedure is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MPSCHED_STARTUP_THIS_AP)(
  IN  EFI_MP_SCHED_PROTOCOL     *This,
  IN  AUXKERNEL                 **ApAuxKernel,
  IN  EFI_AUXKERNEL_PROCEDURE   Procedure,
  IN  UINTN                     Argument                OPTIONAL,
  IN  UINTN                     Argument2               OPTIONAL,
  IN  UINTN                     CpuNumber,
  IN  UINTN                     TimeoutInMicroseconds,
  IN  EFI_EVENT                 WaitEvent               OPTIONAL,
  OUT BOOLEAN                   *Finished               OPTIONAL
  );

/**
  This return the handle number for the calling processor.  This service may be
  called from the BSP and APs.

  This service returns the processor handle number for the calling processor.
  The returned value is in the range from 0 to the total number of logical
  processors minus 1. The total number of logical processors can be retrieved
  with EFI_MP_SERVICES_PROTOCOL.GetNumberOfProcessors(). This service may be
  called from the BSP and APs. If ProcessorNumber is NULL, then EFI_INVALID_PARAMETER
  is returned. Otherwise, the current processors handle number is returned in
  ProcessorNumber, and EFI_SUCCESS is returned.

  @param[in] This              A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber   The handle number of AP that is to become the new
                               BSP. The range is from 0 to the total number of
                               logical processors minus 1. The total number of
                               logical processors can be retrieved by
                               EFI_MP_SERVICES_PROTOCOL.GetNumberOfProcessors().

  @retval EFI_SUCCESS             The current processor handle number was returned
                                  in ProcessorNumber.
  @retval EFI_INVALID_PARAMETER   ProcessorNumber is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_MPSCHED_WHOAMI)(
  IN EFI_MP_SCHED_PROTOCOL  *This,
  OUT UINTN                 *CpuNumber
  );

///
/// When installed, the MP Services Protocol produces a collection of services
/// that are needed for MP management.
///
struct _EFI_MP_SCHED_PROTOCOL {
  UINT32                                Revision;
  EFI_MPSCHED_GET_ENABLED_CPUS          GetEnabledCpus;
  EFI_MPSCHED_GET_CPU_INFO              GetCpuInfo;
  EFI_MPSCHED_STARTUP_THIS_AP           StartupThisAp;
  EFI_MPSCHED_WHOAMI                    WhoAmI;
};

#endif

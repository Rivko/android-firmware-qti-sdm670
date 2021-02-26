#ifndef __AUXKERNEL_LIB_H__
#define __AUXKERNEL_LIB_H__

#include <Uefi.h>
#include <Library/AuxKernel.h>

extern EFI_SYSTEM_TABLE  mAKSystemTable;

/**
 * Puts the core into a deadloop
 */
VOID EFIAPI
AuxKernelCpuDeadLoop (
  VOID
  );


/**
 * Returns core number
 */
UINT32 EFIAPI
AuxKernelWhoAmI (
  VOID
  );


/**
 * Invokes a function on the BSP over a Port
 * @param  Port               Port on BSP
 * @param  Service            Service supported by Port
 * @param  ParamsSize         Size of the parameter buffer
 * @param  Params             Buffer containing serialized params.
 *                            (Okay to pass pointers as long as
 *                             cache maintanence is properly handled)
 * @param  ResponseBufferSize Size of response buffer
 * @param  ResponseBuffer     Resposne buffer. If resposne buffer is
 *                            NULL, response of API call on BSP will
 *                            not be returned through ResponseBuffer
 * @return EFI_INVALID_PARAMETER Invalid parameter
 *         EFI_SUCCESS           Function call was successful
 *         Other EFI_STATUS error codes may also be returned
 */
EFI_STATUS EFIAPI
AuxKernelExecuteOnBsp(
  IN     UINT64         Port,
  IN     UINT32         Service,
  IN     UINTN          ParamsSize,
  IN     VOID           *Params,
  IN OUT UINTN          *ResponseBufferSize,
     OUT VOID           *ResponseBuffer
  );


/**
 * Send message to Port/Service on BSP
 * @param  Port       Port on BSP
 * @param  Service    Service supported by Port
 * @param  BufferSize Buffer Size
 * @param  Buffer     Buffer sent to BSP. May be NULL
 * @return EFI_INVALID_PARAMETER Invalid parameter
 *         EFI_SUCCESS           Function call was successful
 *         Other EFI_STATUS error codes may also be returned
 */
EFI_STATUS EFIAPI
AuxKernelSendMsgToBsp (
  IN     UINT64     Port,
  IN     UINT32     Service,
  IN     UINTN      BufferSize,
  IN     VOID       *Buffer
  );

/**
 * Receive message from Port/Service on BSP
 * @param  Port       Port on BSP
 * @param  Service    Service supported by Port
 * @param  BufferSize Buffer Size
 * @param  Buffer     Buffer; on return contains data
 *                    from BSP
 * @return EFI_INVALID_PARAMETER Invalid parameter
 *         EFI_SUCCESS           Function call was successful
 *         Other EFI_STATUS error codes may also be returned
 */
EFI_STATUS EFIAPI
AuxKernelRecvMsgFromBsp (
     OUT UINT64     *Port,
     OUT UINT32     *Service,
  IN OUT UINTN      *BufferSize,
     OUT VOID       *Buffer
  );

#endif //__AUXKERNEL_LIB_H__

#ifndef DDISLIMBUS_CODEC_H
#define DDISLIMBUS_CODEC_H
/*=========================================================================*/
/**
   @file  ddislimbus_codec.h

   This module provides the interface to the SLIMbus driver 
   codec-specific extensions.
*/
/*=========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/api/buses/ddislimbus_codec.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/25/13   MJS     Initial revision.

===========================================================================
             Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "DALSys.h"
#include "DALStdErr.h"
#include "ddislimbus.h"

#define SLIMBUS_VE_MAX_SLICE_SIZE_BYTES 16
#define SLIMBUS_VE_EC_SIZE_BYTES        2
#define SLIMBUS_VE_MAP_MAX_ADDR         0xfff

#define SLIMBUS_MSG_MC_CODEC_REPEAT_CHANGE_VALUE 0x0


/**
  @brief Format the element code for a value element message
 
  This function formats the element code for a value element 
  message and stores the code in the buffer parameter.  The 
  buffer size must be at least SLIMBUS_VE_EC_SIZE_BYTES. 
  
  @param[out] pucBuf  Location to store the element code
   
  @param[in] uByteAddr  The byte address for the value element 
        access
   
  @param[in] uSliceSize  The number of bytes to read or write 
        for the value element access
  
  @return  DAL_SUCCESS on success, an error code on error
 */
static __inline DALResult
DalSlimBus_FormatElementCode(uint8 *pucBuf, uint32 uByteAddr, uint32 uSliceSize)
{
  if ( NULL == pucBuf ||
       uSliceSize == 0 ||
       uSliceSize > SLIMBUS_VE_MAX_SLICE_SIZE_BYTES ||
       uByteAddr > SLIMBUS_VE_MAP_MAX_ADDR )
  {
    return DAL_ERROR;
  }

  if (uSliceSize <= 4)
  {
    pucBuf[0] = (uSliceSize-1);
  }
  else if (uSliceSize <= 8)
  {
    if (uSliceSize & 0x1)
    {
      return DAL_ERROR;
    }
    pucBuf[0] = ((uSliceSize>>1) + 1);
  }
  else
  {
    if (uSliceSize & 0x3)
    {
      return DAL_ERROR;
    }
    pucBuf[0] = ((uSliceSize>>2) + 3);
  }
  pucBuf[0] |= 0x8 | (uByteAddr<<4); /* 0x8 for byte-based access */
  pucBuf[1] = uByteAddr>>4;

  return DAL_SUCCESS;
}

/**
  @brief Write a single value element repeatedly to a Qualcomm 
         codec device.
  
  This function writes to a single value element repeatedly to a
  Qualcomm codec device using byte-based access.  This function 
  will perform a synchronous write. 
  
  @param[in] _h  Client handle to the SLIMbus driver 
   
  @param[in] LA  Logical address of the device to write the 
        value element to
   
  @param[in] uByteAddr  The byte address of the value element to
        write repeatedly
   
  @param[in] pucWriteData  Pointer to the buffer containing the 
        data to write to the value element
   
  @param[in] uWriteDataLen  Size of the write data buffer and 
        the number of bytes to write to the value element;
        allowed values for uWriteDataLen are 1, 2, 3, 4, 6, 8,
        12 and 16
   
  @param[in] hEvent  Event handle
  
  @return  DAL_SUCCESS on success, an error code on error
   
  @see DalSlimBus_WriteValueElement()
  */
static __inline DALResult
DalSlimBus_CodecRepeatWriteValueElement(DalDeviceHandle *_h, SlimBusLogicalAddrType LA, uint32 uByteAddr, const uint8* pucWriteData, uint32 uWriteDataLen, DALSYSEventHandle hEvent)
{
  uint8 ucaPayload[SLIMBUS_VE_MAX_SLICE_SIZE_BYTES+SLIMBUS_VE_EC_SIZE_BYTES];

  if ( NULL == pucWriteData )
  {
    return DAL_ERROR;
  }
  
  if ( DAL_SUCCESS != DalSlimBus_FormatElementCode(ucaPayload, uByteAddr, uWriteDataLen) )
  {
    return DAL_ERROR;
  }
  
  memcpy(ucaPayload+SLIMBUS_VE_EC_SIZE_BYTES, pucWriteData, uWriteDataLen);

  return DalSlimBus_SendUserDefinedMessage(_h, LA, SLIMBUS_MSG_MC_CODEC_REPEAT_CHANGE_VALUE,
                                           ucaPayload, uWriteDataLen+SLIMBUS_VE_EC_SIZE_BYTES, hEvent);
}

#endif /* DDISLIMBUS_CODEC_H */


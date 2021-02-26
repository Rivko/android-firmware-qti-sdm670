#ifndef __BIT_STREAM_READER_H_INCLUDED__
#define __BIT_STREAM_READER_H_INCLUDED__

/**
@file
@brief BitStreamReader.h file.

This file declares the class implementing bit stream reader.
*/

/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>

namespace slim
{
/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/

/**
@brief Binary stream parser class.

BitDivider takes an array of bytes and provides methods to read them
as a stream of bits.
*/
class BitDivider
{
public:
  /**
  @brief Constructs object

  Function creates the object and initialises internal fields.
  @sa GetStatus() for operation result indication.

  @param[in] puBytes pointer to an array of bytes
  @param[in] sLen    length of the array of bytes
  */
  BitDivider(const uint8_t* const puBytes, size_t sLen);

  /**
  @brief Read bits

  Function reads required number of bits form the stream. Read pointer moves by
  given number of bits so next read operation starts reading from the bit
  following the last read one.

  @sa GetStatus() for operation result indication.

  @param[in] qBitsNumber Number of bits to read. Must be in [1..32] range.

  @return unsigned value read from bit stream in lowest bits of the result
  */
  uint32_t Get(uint32_t qBitsNumber);

  /**
  @brief Set position for read operation

  Function sets the read pointer to the given offset so that next read operation
  start reading from the bit which offset is passed to this method.
  @sa GetStatus() for operation result indication.

  @param[in] qP Offset in bits from the beginning of the stream. Must be in
                [0..8*sLen-1] range.
  */
  void SetPos(uint32_t qP);

  /**
  @brief Check status of last operation

  Function returns status of the last operation.

  @return 0 if operation was successful, non-zero value otherwise.
  */
  uint32_t GetStatus() const;

  /**
  @brief Expand sign

  Function propagates sign bit to upper bits for negative values.
  If bit number qL-1 is 1, then bits 31 .. qL get set to 1.

  @param[in] qV Value to be modified
  @param[in] qL Number of meaningful bits.

  @return signed value with sign bit propagated to upper bits
  */
  static int32_t ExpandSign(uint32_t qV, uint32_t qL);

private:
  static const uint8_t  s_puMasks[];
  uint32_t              m_qByteIndex;
  uint32_t              m_qBitIndex;
  const uint8_t* const  m_puBytes;
  const size_t          m_sLen;
  uint32_t              m_qStatus;
};

}
#endif

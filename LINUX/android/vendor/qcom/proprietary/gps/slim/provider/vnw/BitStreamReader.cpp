/**
@file
@brief Bit Stream Reader file.

This file defines Bit Stream Reader.
*/

/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "BitStreamReader.h"

namespace slim
{

// See header file for field documentation
const uint8_t BitDivider::s_puMasks[] =
  {0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

// See header file for function documentation
BitDivider::BitDivider(const uint8_t* const puBytes, size_t sLen)
: m_qByteIndex(0)
, m_qBitIndex(0)
, m_puBytes(puBytes)
, m_sLen(sLen)
{
  m_qStatus = (0 != puBytes && 0 < m_sLen) ? 0 : 1;
}

// See header file for function documentation
uint32_t BitDivider::Get(uint32_t qBitsNumber)
{
  uint32_t qRes = 0;
  uint32_t qAddedBits = 0;

  if (0 != m_qStatus || 0 == qBitsNumber || 32 < qBitsNumber)
  {
    m_qStatus = 1;
    return 0;
  }

  while (qAddedBits < qBitsNumber)
  {
    if (m_qByteIndex >= m_sLen)
    {
      m_qStatus = 1;
      return 0;
    }

    uint32_t qNBits = 0;
    if ((qBitsNumber - qAddedBits) > (8 - m_qBitIndex))
    {
      qNBits = 8 - m_qBitIndex;
    }
    else
    {
      qNBits = qBitsNumber - qAddedBits;
    }

    qRes <<= qNBits;
    uint8_t uMask = s_puMasks[qNBits - 1] >> m_qBitIndex;
    uint8_t uAdded = (m_puBytes[m_qByteIndex] & uMask) >>
                     (8 - m_qBitIndex - qNBits);
    qRes += uAdded;

    qAddedBits += qNBits;
    m_qBitIndex += qNBits;

    if (7 < m_qBitIndex)
    {
      m_qByteIndex++;
      m_qBitIndex = 0;
    }
  }

  return qRes;
}

// See header file for function documentation
void BitDivider::SetPos(uint32_t qP)
{
  m_qBitIndex  = qP % 8;
  m_qByteIndex = qP / 8;
  m_qStatus = (0 != m_puBytes && m_qByteIndex < m_sLen) ? 0 : 1;
}

// See header file for function documentation
int32_t BitDivider::ExpandSign(uint32_t qV, uint32_t qL)
{
  if (2 > qL || 32 <= qL)
  {
    return (int32_t) qV;
  }

  if (0 == ((qV >> (qL - 1)) & 1))
  {
    return (int32_t) qV;
  }

  uint32_t qRes = 0xffffffff;
  qRes = (qRes << qL) | qV;
  return (int32_t)qRes;
}

// See header file for function documentation
uint32_t BitDivider::GetStatus() const
{
  return m_qStatus;
}

}


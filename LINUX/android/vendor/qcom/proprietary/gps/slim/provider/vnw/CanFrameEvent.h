#ifndef CAN_FRAME_EVENT_H
#define CAN_FRAME_EVENT_H

/**
@file
@brief Vehicle Network provider implementation file.

This file defines VNW provider object implementation.
*/

/*
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <CanWrapper.h>

namespace slim
{
/*----------------------------------------------------------------------------
 * Class Definitions
 * -------------------------------------------------------------------------*/

/**
@brief CAN frame event class.

CanFrameEvent class contains necessary data describe an event that new data
frame was received over CAN bus. It is intended to carry data received from
CamWrapper to the VNW provider's event procesing thread.
*/
class CanFrameEvent
{
public:
  /**
  @brief Constructs the object

  Function creates the object and initialises its internal fields.

  @param[in] pFrame pointer to a CAN frame (CwFrame) object
  @param[in] ifNo   interface number the frame was received from
  */
  CanFrameEvent(CwFrame* pFrame, int ifNo)
    : m_pzFrame(0)
    , m_lIfNo(ifNo)
    , mIsBuffFrame(false)
  {
    m_pzFrame = (0 != pFrame) ? pFrame->clone(1) : 0;
  }

  /**
  @brief Destroys the object.
  */
  ~CanFrameEvent()
  {
    delete m_pzFrame;
  }

  /**
  @brief Get interface number

  Function returns interface number that the frame was received from.

  @return interface number.
  */
  int32_t GetIfNo() const
  {
    return m_lIfNo;
  }

  /**
  @brief Get CAN frame

  Function returns pointer to the CAN frame object (CwFrame) that was passed
  by CanWrapper on a frame received over CAN bus.

  @return pointer to a CwFrame object
  */
  CwFrame* GetFrame() const
  {
    return m_pzFrame;
  }

  /**
  @brief Indicates whether frame is buffered one or not.
  */
  bool  mIsBuffFrame;
private:
  CwFrame* m_pzFrame;
  int32_t  m_lIfNo;

};

}
#endif

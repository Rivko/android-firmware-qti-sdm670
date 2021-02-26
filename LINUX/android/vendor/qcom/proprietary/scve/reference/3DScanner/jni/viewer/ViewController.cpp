/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <math.h>
#include "ViewController.h"
#ifndef WIN32
#include <jni.h>
#include "androidUtility.hpp"
#endif


/**********************************************************
 * Constructor
 **********************************************************/
ViewController::ViewController()
{
    Clear();
}

/**********************************************************
 * Destructor
 **********************************************************/
ViewController::~ViewController()
{
    Clear();
}

/**********************************************************
 * Clear
 **********************************************************/
void ViewController::Clear()
{
    /*
     * 3D Viewer variables
     */
    for (int i=0; i < 10; i++)
    {
        m_mouseX[0] = 0;
        m_mouseY[0] = 0;
    }

    m_mouseDown = false;
    m_rightMouseDown = false;

    m_lastMouse = false;
    m_startX = m_startY = 0;
    m_startRotateX = 0.;
    m_startRotateY = 0.;
    m_currentAngleX = 0.f;
    m_currentAngleY = 0.f;

    m_currentZoom = 45.f;
    m_rightMouseDown = false;
    m_lastRightMouseDown = false;
    m_startZoom = 1.f; 
    m_startZoomX = 0;
    m_startZoomY = 0;

    m_middleMouseDown = false;
    m_lastMiddleMouseDown = false;
    m_currentTrackHoriz = 0.f;
    m_currentTrackVert = 0.f;
    m_startTrackHoriz = 0.f;
    m_startTrackVert = 0.f;
    m_startTrackX = 0;
    m_startTrackY = 0;
    m_rotateMode = false;
    m_translateMode = false;
    m_zoomMode = false;
}


/**********************************************************
 * MousePositionCallback
 **********************************************************/
void ViewController::MousePositionCallback(int button, int x, int y)
{
    m_mouseX[button] = x;
    m_mouseY[button] = y;
}

/**********************************************************
 * MouseButtonCallback
 **********************************************************/
void ViewController::MouseButtonCallback(int button, int state, int x, int y)
{
    m_mouseX[button] = x;
    m_mouseY[button] = y;

    // left
    if (button == 0)
    {
        if (state == 1)
        {
            m_mouseDown = true;
        }
        else
        {
            m_mouseDown = false;
        }
    }

    // middle
    if (button == 1)
    {
        if (state == 1)
        {
            m_middleMouseDown = true;
        }
        else
        {
            m_middleMouseDown = false;
        }
    }

    // right
    if (button == 2)
    {
        if (state == 1)
        {
            m_rightMouseDown = true;
        }
        else
        {
            m_rightMouseDown = false;
        }
    }
}


/**********************************************************
 * ViewerUpdatePC - Variables updated for 3D camera purposes:
 * 
 * m_currentZoom, m_currentTrackHoriz, m_currentTrackVert
 * m_currentAngleX, m_currentAngleY
 **********************************************************/
void ViewController::ViewerUpdatePC()
{
    int viewerWidth = 1920;

    /*
     * Rotate object
     */
    if (m_lastMouse == false && m_mouseDown == true)
    {
        m_startX = m_mouseX[0];
        m_startY = m_mouseY[0];

        m_lastMouse = true;
        m_startRotateX = m_currentAngleX;
        m_startRotateY = m_currentAngleY;
    }
    else if (m_mouseDown == true)
    {
        m_currentAngleX = m_startRotateX + -((float)(m_mouseX[0] - m_startX) / (float)viewerWidth) * 90.f * 2.0f;
        m_currentAngleY = m_startRotateY + -((float)(m_mouseY[0] - m_startY) / (float)viewerWidth) * 90.f * 2.0f;
        m_lastMouse = true;
    }
    else if (m_lastMouse == true && m_mouseDown == false)
    {
        m_startRotateX = m_currentAngleX;
        m_startRotateY = m_currentAngleY;
        m_lastMouse = false;
    }

    /*
     * ZOOM
     */
    if (m_lastRightMouseDown == false && m_rightMouseDown == true)
    {
        m_lastRightMouseDown = true;
        m_startZoom = m_currentZoom;
        m_startZoomX = m_mouseX[0];
        m_startZoomY = m_mouseY[0];
    }
    else if (m_rightMouseDown == true)
    {
        if (fabsf((float)(m_mouseX[0] - m_startZoomX)) > fabsf((float)(m_mouseY[0] - m_startZoomY)))
        { 
            m_currentZoom = m_startZoom - ((float)(m_mouseX[0] - m_startZoomX) / (float)viewerWidth);
        }
        else
        {
            m_currentZoom = m_startZoom - ((float)(m_mouseY[0] - m_startZoomY) / (float)viewerWidth);
        }

        if (m_currentZoom < 0.1f)
        {
            m_currentZoom = 0.1f;
        }
        m_lastRightMouseDown = true;
    }
    else if (m_lastRightMouseDown == true && m_rightMouseDown == false)
    {
        m_startZoom = m_currentZoom;
        m_lastRightMouseDown = false;
    }


    /*
     * TRACK
     */
    if (m_lastMiddleMouseDown == false && m_middleMouseDown == true)
    {
        m_lastMiddleMouseDown = true;
        m_startTrackHoriz = m_currentTrackHoriz;
        m_startTrackVert = m_currentTrackVert;
        m_startTrackX = m_mouseX[0];
        m_startTrackY = m_mouseY[0];
    }
    else if (m_middleMouseDown == true)
    {
        m_currentTrackHoriz = m_startTrackHoriz - ((float)(m_mouseX[0] - m_startTrackX) * -0.0005f);
        m_currentTrackVert = m_startTrackVert - ((float)(m_mouseY[0] - m_startTrackY) * 0.0005f);
        m_lastMiddleMouseDown = true;
    }
    else if (m_lastMiddleMouseDown == true && m_middleMouseDown == false)
    {
        m_startTrackHoriz = m_currentTrackHoriz;
        m_startTrackVert = m_currentTrackVert;
        m_lastMiddleMouseDown = false;
    }
}

/**********************************************************
 * ViewerUpdateTablet - Variables updated for 3D camera purposes:
 * 
 * m_currentZoom, m_currentTrackHoriz, m_currentTrackVert
 * m_currentAngleX, m_currentAngleY
 **********************************************************/
void ViewController::ViewerUpdateTablet()
{
    int viewerWidth = 1920;


    /*
     * Rotate object
     */
    if (m_lastMouse == false && m_mouseDown == true)
    {
        m_startX = m_mouseX[0];
        m_startY = m_mouseY[0];

        m_lastMouse = true;
        m_startRotateX = m_currentAngleX;
        m_startRotateY = m_currentAngleY;

        m_rotateMode = true;
        m_translateMode = false;
        m_zoomMode = false;
    }
    else if (m_mouseDown == true && m_rotateMode == true)
    {
        m_currentAngleX = m_startRotateX + -((float)(m_mouseX[0] - m_startX) / (float)viewerWidth) * 90.f * 2.0f;
        m_currentAngleY = m_startRotateY + -((float)(m_mouseY[0] - m_startY) / (float)viewerWidth) * 90.f * 2.0f;
        m_lastMouse = true;
    }
    else if (m_lastMouse == true && m_mouseDown == false)
    {
        m_startRotateX = m_currentAngleX;
        m_startRotateY = m_currentAngleY;
        m_lastMouse = false;
    }

    /*
     * TRACK + ZOOM
     */
    if (m_lastMiddleMouseDown == false && m_middleMouseDown == true)
    {
        float dx = (float)(m_mouseX[0]- m_mouseX[1]); 
        float dy = (float)(m_mouseY[0]- m_mouseY[1]); 
        m_initZoomDistance = (float)sqrt((dx * dx) + (dy * dy));
        m_startZoom = m_currentZoom;

        m_lastMiddleMouseDown = true;
        m_startTrackHoriz = m_currentTrackHoriz;
        m_startTrackVert = m_currentTrackVert;

        m_startTrackX = (m_mouseX[1] + m_mouseX[0])/ 2;
        m_startTrackY = (m_mouseY[1] + m_mouseY[0])/ 2;

        m_rotateMode = false;
        m_translateMode = true;
        m_zoomMode = false;
    }
    else if (m_middleMouseDown == true && m_translateMode == true  && m_rightMouseDown == false)
    {
        float dx = (float)(m_mouseX[0]- m_mouseX[1]); 
        float dy = (float)(m_mouseY[0]- m_mouseY[1]); 
        float zoomDistance = (float)sqrt((dx * dx) + (dy * dy));

        int px = (m_mouseX[1] + m_mouseX[0])/ 2;
        int py = (m_mouseY[1] + m_mouseY[0])/ 2;

        m_currentTrackHoriz = m_startTrackHoriz - ((float)(px - m_startTrackX) * -0.0005f);
        m_currentTrackVert = m_startTrackVert - ((float)(py - m_startTrackY) * 0.0005f);

        float zoomRatio = zoomDistance / m_initZoomDistance;

        m_currentZoom = m_startZoom - (((zoomDistance - m_initZoomDistance)/(float)viewerWidth));//* 44.f);
//        m_currentZoom = m_startZoom - (((zoomDistance - m_initZoomDistance)/(float)viewerWidth)* 44.f);

        if (m_currentZoom < 0.1f)
        {
            m_currentZoom = 0.1f;
        }

        m_lastMiddleMouseDown = true;
    }
    else if (m_lastMiddleMouseDown == true && m_middleMouseDown == false  && m_rightMouseDown == false)
    {
        m_startTrackHoriz = m_currentTrackHoriz;
        m_startTrackVert = m_currentTrackVert;
        m_lastMiddleMouseDown = false;
        m_translateMode = false;

        m_startZoom = m_currentZoom;
    }
}

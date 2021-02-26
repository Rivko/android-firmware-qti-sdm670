#ifndef __VIEW_CONTROLLER_H__
#define __VIEW_CONTROLLER_H__
/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
class ViewController
{
public:
    ViewController();
    ~ViewController();

   void Clear();
	void ViewerUpdatePC();
	void ViewerUpdateTablet();
	void MousePositionCallback(int button, int x, int y);
	void MouseButtonCallback(int button, int state, int x, int y);


protected:

	bool m_mouseDown;
	int m_mouseX[10];
	int m_mouseY[10];
	bool m_lastMouse;
	int m_startX, m_startY;
	float m_startRotateX;
	float m_startRotateY;
	float m_currentAngleX;
	float m_currentAngleY;

	bool m_rightMouseDown;
	bool m_lastRightMouseDown;
	float m_startZoom;
	int m_startZoomX;
	int m_startZoomY;
	float m_currentZoom;
    int m_greyToggle;

	bool m_middleMouseDown;
	bool m_lastMiddleMouseDown;
    float m_currentTrackHoriz;
    float m_currentTrackVert;
    float m_startTrackHoriz;
    float m_startTrackVert;
    int m_startTrackX;
    int m_startTrackY;
    bool m_rotateMode;
    bool m_translateMode;
    bool m_zoomMode;
    float m_initZoomDistance;
};


#endif

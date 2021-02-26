#version 300 es
//Copyright (c) 2016 Qualcomm Technologies, Inc.
//All Rights Reserved.
//Confidential and Proprietary - Qualcomm Technologies, Inc.

precision mediump float;

uniform vec4 u_Color;

in float cullValue;

out vec4 FragColor;

void main()
{
    FragColor = u_Color;

   // gl_FragDepth = cullValue + gl_FragCoord.z;

}

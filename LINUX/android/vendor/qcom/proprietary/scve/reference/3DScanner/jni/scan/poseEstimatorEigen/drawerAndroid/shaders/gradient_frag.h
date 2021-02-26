/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
char gradient_frag [] =
"#version 300 es\n"
"\n"
"precision highp float;\n"
"\n"
"uniform vec3 gCS_GradCol0;\n"
"uniform vec3 gCS_GradCol1;\n"
"\n"
"uniform float gCS_Width;\n"
"uniform float gCS_Height;\n"
"\n"
"out vec4 FragColor;"
"\n"
"void main() {\n"
"    float normY = gl_FragCoord.y / gCS_Height;\n"
"    FragColor = mix(vec4(gCS_GradCol0, 1.0), vec4(gCS_GradCol1, 1.0), normY );\n"
"}\n"
; 

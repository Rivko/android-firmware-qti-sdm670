/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
char screen_space_vert [] =
"#version 300 es\n"
"\n"
"layout(location = 0) in vec2 iTexCoord;\n"
"\n"
"out highp vec2 vs_TC;\n"
"\n"
"void main() {\n"
"    vs_TC = iTexCoord;\n"
"    gl_Position = vec4(vs_TC * 2.0 - 1.0, -1.0, 1.0);\n"
"}\n"
; 

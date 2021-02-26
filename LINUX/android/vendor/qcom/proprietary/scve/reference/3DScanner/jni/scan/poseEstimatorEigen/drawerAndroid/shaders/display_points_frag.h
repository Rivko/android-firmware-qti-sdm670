/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
char display_points_frag [] =
"#version 300 es\n"
"\n"
"precision highp float;\n"
"in highp vec3 vs_color;\n"
"layout(location = 0) out vec4 fs_Color;\n"
"\n"
"void main() {\n"
"    fs_Color = vec4(vs_color, 1.0);\n"
"}\n"
; 

/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
char screen_space_vflip_frag [] =
"#version 300 es\n"
"\n"
"precision highp float;\n"
"\n"
"in highp vec2 vs_TC;\n"
"\n"
"uniform sampler2D gColor;\n"
"\n"
"layout(location = 0) out vec4 fs_Color;\n"
"\n"
"void main() {\n"
"    //vec2 uv = vs_TC;\n"
"    vec2 uv = vec2(vs_TC.x, 1.0 - vs_TC.y);\n"
"    fs_Color = texture(gColor, uv);\n"
"}\n"
; 

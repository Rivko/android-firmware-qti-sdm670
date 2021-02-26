/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
char surface_splatting_normalization_frag [] = 
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
"    vec4 color_unweighted = texture(gColor, vs_TC);\n"
"    fs_Color = vec4(color_unweighted.rgb / color_unweighted.a, 1);\n"
"}\n"
; 

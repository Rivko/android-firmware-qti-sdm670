/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
char gradient_vert [] =
"#version 300 es\n"
"\n"
"layout(location = 0) in vec3 iPosition;\n"
"\n"
"void main() {\n"
"    gl_Position = vec4(iPosition, 1.0f);\n"
"}\n"
; 

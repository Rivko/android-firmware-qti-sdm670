/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
char display_lines_vert [] = 
"#version 300 es\n"
"\n"
"layout(location = 0) in vec3 iPosition;\n"
"\n"
"uniform mat4 gCS_view;\n"
"uniform mat4 gCS_projection;\n"
"\n"
"uniform mat4 gBB_model;\n"
"\n"
"void main() {\n"
"    vec4 world_pos = gBB_model * vec4(iPosition, 1.0);\n"
"    gl_Position = gCS_projection * (gCS_view * world_pos);\n"
"}\n"
; 

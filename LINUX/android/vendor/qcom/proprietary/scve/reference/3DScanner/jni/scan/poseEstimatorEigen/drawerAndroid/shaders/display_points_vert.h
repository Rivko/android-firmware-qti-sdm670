/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
char display_points_vert [] =
"#version 300 es  \n"
"layout(location = 0) in vec3 iPosition;  \n"
"layout(location = 1) in vec3 iColor;  \n"
"uniform mat4 gCS_view;  \n"
"uniform mat4 gCS_projection;  \n"
"uniform mat4 gMS_model;  \n"
"uniform mat4 gBB_inverseModel;  \n"
"uniform vec3 gBB_max;  \n"
"uniform vec3 gBB_min;  \n"
"uniform float gBB_enableCulling;  \n"
"out highp vec3 vs_color;  \n"
"  \n"
"void main() {  \n"
"    vec4 world_pos = gMS_model * vec4(iPosition, 1.0);  \n"
"    vec4 aabb_space = gBB_inverseModel * world_pos;  \n"
"    if(gBB_enableCulling > 0.5 || (all(lessThanEqual(aabb_space.xyz, gBB_max.xyz)) &&  \n"
"            all(greaterThanEqual(aabb_space.xyz, gBB_min.xyz)))) {  \n"
"        //normalize to [0,1]  \n"
"        vs_color = iColor / 255.0;  \n"
"  \n"
"        //bring all coordinates into camera space  \n"
"        vec4 p_cs = gCS_view * (world_pos);  \n"
"        gl_Position = gCS_projection * p_cs;  \n"
"        gl_PointSize = 5.0;  \n"
"    } else  \n"
"        gl_Position = vec4(-2, -2, 0, 1.0);  \n"
"}  \n"
;
#version 300 es
//Copyright (c) 2016 Qualcomm Technologies, Inc.
//All Rights Reserved.
//Confidential and Proprietary - Qualcomm Technologies, Inc.


// Uniforms
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelMatrix;
uniform float u_CullVar;

in vec4 a_Position;

out float cullValue;


void main()
{
    vec4 v_Position = u_ModelMatrix * a_Position;

    vec4 glPos = u_ProjectionMatrix * v_Position;

    //this step function will return zero if cull variable is greater than this vertex
    //and 1 if it isn't
    cullValue = step(0.0 ,glPos.z - u_CullVar);

    gl_Position = glPos;
    gl_PointSize = 5.0;

}

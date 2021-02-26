#version 300 es
//Copyright (c) 2016 Qualcomm Technologies, Inc.
//All Rights Reserved.
//Confidential and Proprietary - Qualcomm Technologies, Inc.


// Uniforms
uniform mat4 uProjectionMatrix;
uniform mat4 uModelViewMatrix;
uniform float u_CullVar;

// Attributes
in vec3 aPosition;
in vec3 aNormal;

// Varying
out vec3 vNormal;
out float cullValue;

void main()
{

    //vNormal = mat3(uProjectionMatrix) * mat3(uModelViewMatrix) * aNormal; //using precalculated normal
    vNormal = mat3(uProjectionMatrix) * mat3(uModelViewMatrix) * aPosition; //calculate normal here

    vec4 vPosition = uModelViewMatrix * vec4(aPosition, 1.0f);

    vec4 glPos = uProjectionMatrix * vPosition;

    //this step function will return zero if cull variable is greater than this vertex
    //and 1 if it isn't
    cullValue = step(0.0 ,glPos.z - u_CullVar);

    gl_Position = glPos;


}

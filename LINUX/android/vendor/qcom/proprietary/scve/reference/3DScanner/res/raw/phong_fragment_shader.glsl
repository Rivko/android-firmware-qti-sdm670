#version 300 es
//Copyright (c) 2016 Qualcomm Technologies, Inc.
//All Rights Reserved.
//Confidential and Proprietary - Qualcomm Technologies, Inc.


// Varying
in highp vec3 vNormal;
in float cullValue;

// Uniforms
uniform highp vec3 uDiffuse;
uniform highp vec3 uSpecular;
uniform lowp float uAlpha;

out vec4 FragColor;

void main()
{
    // Material
    highp vec3 ka = vec3(0.05);
    highp vec3 kd = uDiffuse;
    highp vec3 ks = uSpecular;

    highp float alpha = 1.0;

    // Light
    highp vec3 ia = vec3(1.0);
    highp vec3 id = vec3(1.0);
    highp vec3 is = vec3(1.0);

    // Vectors
    //highp vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    highp vec3 lightDir = normalize(vec3(0.0, 0.0, -1.0));
    highp vec3 normal = normalize(vNormal);

    // Diffuse
    highp float diffuse = max(dot(lightDir, normal), 0.0);

    // Specular
    highp vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));

    ////phong
    //highp vec3 reflectDir = reflect(lightDir, normal);
    //highp float spec = pow(max(dot(reflectDir, viewDir), 0.0), alpha);

    //blinn-phong
    highp vec3 halfwayDir = normalize(lightDir + viewDir);
    highp float spec = pow(max(dot(normal, halfwayDir), 0.0), alpha);

    // Phong reflection equation
    highp vec3 Ip = ka*ia + kd*id*diffuse + ks*is*spec;

    FragColor = vec4(Ip, 1.0);
    FragColor.a = uAlpha;

    //uncomment to enable depth culling
   // gl_FragDepth = cullValue + gl_FragCoord.z;

}

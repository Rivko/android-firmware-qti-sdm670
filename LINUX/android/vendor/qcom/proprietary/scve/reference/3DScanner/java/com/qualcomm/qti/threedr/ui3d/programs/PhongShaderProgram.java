/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/***
 * Excerpted from "OpenGL ES for Android",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create training material, 
 * courses, books, articles, and the like. Contact us if you are in doubt.
 * We make no guarantees that this code is fit for any purpose. 
 * Visit http://www.pragmaticprogrammer.com/titles/kbogla for more book information.
***/
package com.qualcomm.qti.threedr.ui3d.programs;

import android.content.Context;

import com.qualcomm.qti.threedr.R;

import static android.opengl.GLES20.glUniform1f;
import static android.opengl.GLES30.glGetAttribLocation;
import static android.opengl.GLES30.glGetUniformLocation;
import static android.opengl.GLES30.glUniform3f;
import static android.opengl.GLES30.glUniformMatrix4fv;

public class PhongShaderProgram extends ShaderProgram {

    // Uniform locations
    private final int uProjectionMatrixLocation;
    private final int uModelViewMatrixLocation;
    private final int uDiffuseLocation;
    private final int uSpecularLocation;
    private final int uAlphaLocation;
    private final int uCullVar;

    // Attribute locations
    private final int aPositionLocation;


    public PhongShaderProgram(Context context) {

        super(context, R.raw.phong_vertex_shader,
                R.raw.phong_fragment_shader);

        // Retrieve uniform locations for the shader program.
        uProjectionMatrixLocation = glGetUniformLocation(program, "uProjectionMatrix");
        uModelViewMatrixLocation = glGetUniformLocation(program, "uModelViewMatrix");
        uDiffuseLocation = glGetUniformLocation(program, "uDiffuse");
        uSpecularLocation = glGetUniformLocation(program, "uSpecular");
        uAlphaLocation = glGetUniformLocation(program, "uAlpha");
        uCullVar = glGetUniformLocation(program, U_CULL_VAR);

        // Retrieve attribute locations for the shader program.
        aPositionLocation = glGetAttribLocation(program, "aPosition");
    }

    public void setUniforms(float[] projectionMatrix, float[] modelViewMatrix, float[] diffuse, float[] specular, float alpha, float cullVar) {

        glUniformMatrix4fv(uProjectionMatrixLocation, 1, false, projectionMatrix, 0);
        glUniformMatrix4fv(uModelViewMatrixLocation, 1, false, modelViewMatrix, 0);

        glUniform3f(uDiffuseLocation, diffuse[0], diffuse[1], diffuse[2]);
        glUniform3f(uSpecularLocation, specular[0], specular[1], specular[2]);
        glUniform1f(uAlphaLocation, alpha);
        glUniform1f(uCullVar, cullVar);

    }

    public int getPositionAttributeLocation() {
        return aPositionLocation;
    }


}

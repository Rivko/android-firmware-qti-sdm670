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

import com.qualcomm.qti.threedr.ui3d.util.ShaderHelper;
import com.qualcomm.qti.threedr.ui3d.util.TextResourceReader;

import static android.opengl.GLES30.glUseProgram;


abstract class ShaderProgram {
    // Uniform constants
    protected static final String U_PROJ_MATRIX = "u_ProjectionMatrix";
    protected static final String U_MODEL_MATRIX = "u_ModelMatrix";
    protected static final String U_COLOR = "u_Color";
    protected static final String U_CULL_VAR = "u_CullVar";

    // Attribute constants
    protected static final String A_POSITION = "a_Position";

    // Shader program
    protected final int program;

    protected ShaderProgram(Context context, int vertexShaderResourceId,
                            int fragmentShaderResourceId) {
        // Compile the shaders and link the program.
        program = ShaderHelper.buildProgram(
                TextResourceReader
                        .readTextFileFromResource(context, vertexShaderResourceId),
                TextResourceReader
                        .readTextFileFromResource(context, fragmentShaderResourceId));
    }

    public void useProgram() {
        // Set the current OpenGL shader program to this program.
        glUseProgram(program);
    }
}

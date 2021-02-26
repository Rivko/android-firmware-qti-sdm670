/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.ui3d.objects;

import android.util.Log;


import com.qualcomm.qti.threedr.ui3d.Constants;
import com.qualcomm.qti.threedr.ui3d.data.VertexArray;
import com.qualcomm.qti.threedr.ui3d.programs.ColorShaderProgram;

import static android.opengl.GLES30.*;

public class AxisLine {

    private static final String TAG = AxisLine.class.getSimpleName();

    private static float vertexData[];
    private final int vertexOffset;
    private final float start;

    private VertexArray vertexArray;

    double mLength;
    double mStep;
    private static double DEG = Math.PI/180;
    int numPoints;

    private static final int POSITION_COMPONENT_COUNT = 3;
    private static final int STRIDE = (POSITION_COMPONENT_COUNT * Constants.BYTES_PER_FLOAT);


    public enum AXIS {X, Y, Z}

    public AxisLine(float start, float length, float step, AxisLine.AXIS axis) {

        this.mLength = length;
        this.mStep = step;

        if (axis == AXIS.X)
            vertexOffset = 0;
        else if (axis == AXIS.Y)
            vertexOffset = 1;
        else
            vertexOffset = 2;

        this.start = start/2;

        vertexData = new float[(int) ((length/step) * POSITION_COMPONENT_COUNT)];
        build();
        vertexArray = new VertexArray(vertexData);

    }

    private void build() {


        int index = vertexOffset;

        for(float phi = start; phi < mLength - mStep; phi+=mStep) {

            vertexData[index] = phi;
            index += 3;

            numPoints++;

        }

        numPoints -= 1;

        Log.d(TAG,"numPoints = " + numPoints);

    }

    public void bindData(ColorShaderProgram colorProgram) {

        vertexArray.setVertexAttribPointer(0,
                colorProgram.getPositionAttributeLocation(),
                POSITION_COMPONENT_COUNT, STRIDE);

    }

    public void draw() {


        glDrawArrays(GL_POINTS, 0, numPoints);


    }
}

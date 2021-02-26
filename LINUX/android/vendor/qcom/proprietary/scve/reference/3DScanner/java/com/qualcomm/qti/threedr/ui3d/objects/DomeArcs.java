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

public class DomeArcs {

    private static final String TAG = DomeArcs.class.getSimpleName();

    private static float vertexData[];
    private final float mColumnAngleStart;
    private final float mColumnAngleEnd;

    private VertexArray vertexArray;

    double mRadius;
    double mStep;
    int numPoints;

    private static final int POSITION_COMPONENT_COUNT = 3;
    private static final int STRIDE = (POSITION_COMPONENT_COUNT * Constants.BYTES_PER_FLOAT);

    public DomeArcs(float radius, double step, float column_angle_start, float column_angle_end) {

        this.mRadius = radius;
        this.mStep = step;
        this.mColumnAngleStart = (float) Math.toRadians(column_angle_start);
        this.mColumnAngleEnd = (float) Math.toRadians(column_angle_end);

        vertexData = new float[4000 * POSITION_COMPONENT_COUNT];
        build();
        vertexArray = new VertexArray(vertexData);

    }

    private void build() {

        int index = 0;

        //first draw the top four arcs for the dome
        for(double phi = mColumnAngleStart; phi <= mColumnAngleEnd ; phi += (mStep * Math.PI/180) ) { //arcs
        //for(double phi = 0; phi <= 2*Math.PI ; phi+=dPhi) { //full arcs

                //arc 1
                vertexData[index++] = (float) (mRadius * Math.cos(phi));
                vertexData[index++] = (float) (mRadius * Math.sin(phi));
                vertexData[index++] = 0;

                //arc 2
                vertexData[index++] = (float) (mRadius * -Math.cos(phi));
                vertexData[index++] = (float) (mRadius * Math.sin(phi));
                vertexData[index++] = 0;

                //arc 3
                vertexData[index++] = 0;
                vertexData[index++] = (float) (mRadius * Math.sin(phi));
                vertexData[index++] = (float) (mRadius * Math.cos(phi));

                //arc 4
                vertexData[index++] = 0;
                vertexData[index++] = (float) (mRadius * Math.sin(phi));
                vertexData[index++] = (float) (mRadius * -Math.cos(phi));

                numPoints += 4;

        }

        //then draw the circle for the dome middle
        for(double phi = 0; phi <= 2*Math.PI; phi += (mStep * Math.PI/180) ) {

            vertexData[index++] = (float) (mRadius * Math.cos(phi));
            vertexData[index++] = 0;
            vertexData[index++] = (float) (mRadius * Math.sin(phi));

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

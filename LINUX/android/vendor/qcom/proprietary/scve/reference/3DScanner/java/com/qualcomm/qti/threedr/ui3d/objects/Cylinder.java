/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.ui3d.objects;


import com.qualcomm.qti.threedr.ui3d.Constants;
import com.qualcomm.qti.threedr.ui3d.data.VertexArray;
import com.qualcomm.qti.threedr.ui3d.programs.ColorShaderProgram;
import com.qualcomm.qti.threedr.ui3d.programs.PhongShaderProgramWithNormal;
import com.qualcomm.qti.threedr.ui3d.util.Geometry;
import com.qualcomm.qti.threedr.ui3d.objects.ObjectBuilder.*;

import java.util.List;

public class Cylinder {
    private static final int POSITION_COMPONENT_COUNT = 3;
    private static final int NORMAL_COMPONENT_COUNT = 3;

    private static final int STRIDE = ( (POSITION_COMPONENT_COUNT + NORMAL_COMPONENT_COUNT) * Constants.BYTES_PER_FLOAT);

    public final float radius;
    public final float height;

    private final VertexArray vertexArray;
    private final List<DrawCommand> drawList;

    public Cylinder(float yCenter, float radius, float height, int numPointsOnCircumference, boolean enableCaps) {

        GeneratedData generatedData = ObjectBuilder.createCylinder(new Geometry.Point(0f,
            yCenter, 0f), radius, height, numPointsOnCircumference, enableCaps);

        this.radius = radius;
        this.height = height;

        vertexArray = new VertexArray(generatedData.vertexData);
        drawList = generatedData.drawList;
    }

    public void bindDataColor(ColorShaderProgram colorProgram) {
        vertexArray.setVertexAttribPointer(0,
            colorProgram.getPositionAttributeLocation(),
            POSITION_COMPONENT_COUNT, STRIDE);
    }

    public void bindDataPhong(PhongShaderProgramWithNormal phongProgram) {
        vertexArray.setVertexAttribPointer(
                0,
                phongProgram.getPositionAttributeLocation(),
                POSITION_COMPONENT_COUNT,
                STRIDE);

        vertexArray.setVertexAttribPointer(
                POSITION_COMPONENT_COUNT,
                phongProgram.getNormalAttributeLocation(),
                NORMAL_COMPONENT_COUNT,
                STRIDE);

    }

    public void draw() {
        for (DrawCommand drawCommand : drawList) {
            drawCommand.draw();
        }
    }
}

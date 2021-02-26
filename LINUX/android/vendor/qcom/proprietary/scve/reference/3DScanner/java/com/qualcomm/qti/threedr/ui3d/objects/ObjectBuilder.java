/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.ui3d.objects;


import com.qualcomm.qti.threedr.ui3d.util.Geometry;

import java.util.ArrayList;
import java.util.List;

import static android.opengl.GLES30.GL_TRIANGLE_FAN;
import static android.opengl.GLES30.GL_TRIANGLE_STRIP;
import static android.opengl.GLES30.glDrawArrays;

class ObjectBuilder {

    //1private static final int FLOATS_PER_VERTEX = 3;
    private static final int FLOATS_PER_VERTEX = 6; //3 position, 3 normal

    static interface DrawCommand {
        void draw();
    }

    static class GeneratedData {
        final float[] vertexData;
        final List<DrawCommand> drawList;

        GeneratedData(float[] vertexData, List<DrawCommand> drawList) {
            this.vertexData = vertexData;
            this.drawList = drawList;
        }
    }

    static GeneratedData createCylinder(
            Geometry.Point center, float radius, float height, int numPoints, boolean enableCaps) {

        int size = sizeOfCircleInVertices(numPoints) * 2
                   + sizeOfOpenCylinderInVertices(numPoints);

        ObjectBuilder builder = new ObjectBuilder(size * 2);
        
        // First, generate the base.
        float halfHeight = height * 0.5f;

        if (enableCaps) {
            Geometry.Circle baseCircle = new Geometry.Circle(
                    center.translateY(-halfHeight),
                    radius);

            Geometry.Circle topCircle = new Geometry.Circle(
                    center.translateY(halfHeight),
                    radius);
        }

        Geometry.Cylinder cylinder = new Geometry.Cylinder(
            center,
            radius, height);


        //since face culling is enabled, the inside face is not visible, which is fine
        //because the cylinder is enclosed. So just have oposite orientations for the faces
        if (enableCaps) {

            Geometry.Circle baseCircle = new Geometry.Circle(
                    center.translateY(-halfHeight),
                    radius);

            Geometry.Circle topCircle = new Geometry.Circle(
                    center.translateY(halfHeight),
                    radius);

            builder.appendCircle(baseCircle, numPoints, 1);
            builder.appendCircle(topCircle, numPoints, -1);
        }

        builder.appendOpenCylinder(cylinder, numPoints);

        return builder.build();
    }    

    private static int sizeOfCircleInVertices(int numPoints) {
        return 1 + (numPoints + 1);
    }

    private static int sizeOfOpenCylinderInVertices(int numPoints) {
        return (numPoints + 1) * 2;
    }

    private final float[] vertexData;
    private final List<DrawCommand> drawList = new ArrayList<DrawCommand>();
    private int offset = 0;

    private ObjectBuilder(int sizeInVertices) {
        vertexData = new float[sizeInVertices * FLOATS_PER_VERTEX];
    }

    //For every point on the circle, vertexNormal = 0,0,1 or 0,0,-1 depending on direction
    private void appendCircle(Geometry.Circle circle, int numPoints, float direction) {
        final int startVertex = offset / FLOATS_PER_VERTEX;
        final int numVertices = sizeOfCircleInVertices(numPoints);

        // Center point of fan
        vertexData[offset++] = circle.center.x;
        vertexData[offset++] = circle.center.y;
        vertexData[offset++] = circle.center.z;

        //normal
        vertexData[offset++] = 0;
        vertexData[offset++] = -direction;
        vertexData[offset++] = 0;

        // Fan around center point. <= is used because we want to generate
        // the point at the starting angle twice to complete the fan.
        for (int i = 0; i <= numPoints; i++) {

            double angleInRadians =
                  (direction * (double) i / (double) numPoints)
                * ((double) Math.PI * 2f);

            float x = (float) (circle.center.x
                                + circle.radius * Math.cos(angleInRadians));
            float y = circle.center.y;
            float z = (float) (circle.center.z
                    + circle.radius * Math.sin(angleInRadians));

            vertexData[offset++] = x;
            vertexData[offset++] = y;
            vertexData[offset++] = z;

            //normal
            vertexData[offset++] = 0;
            vertexData[offset++] = -direction;
            vertexData[offset++] = 0;

        }

        drawList.add(new DrawCommand() {
            @Override
            public void draw() {
                glDrawArrays(GL_TRIANGLE_FAN, startVertex,
                    numVertices);
            }
        });
    }

    //For every point on a cylinder ring, vertexNormal = normalize(vertexPosition - ringCenter);
    private void appendOpenCylinder(Geometry.Cylinder cylinder, int numPoints) {
        final int startVertex = offset / FLOATS_PER_VERTEX;
        final int numVertices = sizeOfOpenCylinderInVertices(numPoints);

        final float yStart = cylinder.center.y - (cylinder.height / 2f);
        final float yEnd = cylinder.center.y + (cylinder.height / 2f);

        // Generate strip around center point. <= is used because we want to
        // generate the points at the starting angle twice, to complete the
        // strip.
        for (int i = 0; i <= numPoints; i++) {
            double angleInRadians =
                  ((float) i / (float) numPoints)
                * ((float) Math.PI * 2f);
            
            float xPosition =
                    (float) (cylinder.center.x
                                    + cylinder.radius * Math.cos(angleInRadians));
            
            float zPosition =
                    (float) (cylinder.center.z
                                    + cylinder.radius * Math.sin(angleInRadians));

            vertexData[offset++] = xPosition;
            vertexData[offset++] = yStart;
            vertexData[offset++] = zPosition;

            //calc normal
            Geometry.Vector v1 = new Geometry.Vector(xPosition, 0, zPosition);
            Geometry.Vector v1N = v1.normalize();
            vertexData[offset++] = v1N.x;
            vertexData[offset++] = v1N.y;
            vertexData[offset++] = v1N.z;

            vertexData[offset++] = xPosition;
            vertexData[offset++] = yEnd;
            vertexData[offset++] = zPosition;

            //normal again, same because y position doesn't affect
            vertexData[offset++] = v1N.x;
            vertexData[offset++] = v1N.y;
            vertexData[offset++] = v1N.z;

        }

        drawList.add(new DrawCommand() {
            @Override
            public void draw() {
                glDrawArrays(GL_TRIANGLE_STRIP, startVertex,
                    numVertices);
            }
        });        
    }

    private GeneratedData build() {
        return new GeneratedData(vertexData, drawList);
    }
}

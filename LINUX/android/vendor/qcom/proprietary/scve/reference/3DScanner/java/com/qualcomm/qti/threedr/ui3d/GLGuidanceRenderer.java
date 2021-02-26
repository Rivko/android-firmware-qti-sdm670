/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.ui3d;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView.Renderer;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.View;
import android.view.WindowManager;

import com.qualcomm.qti.threedr.GL2JNILib;
import com.qualcomm.qti.threedr.MainActivity;
import com.qualcomm.qti.threedr.ui3d.objects.AxisLine;
import com.qualcomm.qti.threedr.ui3d.objects.Cylinder;
import com.qualcomm.qti.threedr.ui3d.objects.DomeArcs;
import com.qualcomm.qti.threedr.ui3d.objects.Icosahedron_sub2;
import com.qualcomm.qti.threedr.ui3d.programs.ColorShaderProgram;
import com.qualcomm.qti.threedr.ui3d.programs.PhongShaderProgram;
import com.qualcomm.qti.threedr.ui3d.programs.PhongShaderProgramWithNormal;
import com.qualcomm.qti.threedr.ui3d.util.Geometry;
import com.qualcomm.qti.threedr.ui3d.util.Geometry.Point;
import com.qualcomm.qti.threedr.ui3d.util.Geometry.Ray;

import java.text.DecimalFormat;
import java.util.Arrays;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.opengl.GLES30.GL_DEPTH_BUFFER_BIT;
import static android.opengl.GLES30.glBlendEquation;
import static android.opengl.GLES30.glBlendFunc;
import static android.opengl.GLES30.glClear;
import static android.opengl.GLES30.glClearColor;
import static android.opengl.GLES30.glEnable;
import static android.opengl.GLES30.glViewport;
import static android.opengl.Matrix.invertM;
import static android.opengl.Matrix.multiplyMM;
import static android.opengl.Matrix.multiplyMV;
import static android.opengl.Matrix.rotateM;
import static android.opengl.Matrix.scaleM;
import static android.opengl.Matrix.setIdentityM;
import static android.opengl.Matrix.translateM;

/**
 * Created by matthewf on 2/1/16.
 */
public class GLGuidanceRenderer implements Renderer, SensorEventListener {

    private static final String TAG = "Scan3D";
    private boolean depthSensorOnLandscapePhone = true;
    private long startTime;
    private long endTime;

    private int keyframeCnt = 0;
    public boolean scanningQuit = false;

    private boolean renderTarget = true;
    private boolean renderOrigin = true;

    private boolean firstTarget = true;
    private int targetIndx = 0;

    private TargetAngles mTargetAngles;

    private float scanDistTooClose;
    private float scanDistTooFar;

    private static final float DOME_LINES_STEP = 0.2f;
    private static final float DOME_RADIUS = 0.4f;
    private static final float SPHERE_SCALE = 0.04f;

    private static final float TARGET_BALL_VISIBLE_ALPHA = 0.9f;
    private static final float CYLINDER_RADIUS = 0.03f; //0.015f;
    private static final float CYLINDER_HEIGHT = CYLINDER_RADIUS * 6f;//   / 4f;

    /*
     * Targeting tolerances for guidance system
     */
    private static final float TARGET_TOLERANCE_RADIUS = CYLINDER_RADIUS * 1;
    private static final float ORIGIN_TARGETING_TOLERANCE = 0.3f * DOME_RADIUS;
    private static final long ORIGIN_TARGETING_WARNING_FLASH_INTERVAL_MS = 250;


    private static final float[] diffuseVectorYellow = {1.0f, 1.0f, 0f}; //yellow reflectivity
    private static final float[] diffuseVectorBlue = {0.0f, 0.2f, 1.0f}; //blue reflectivity
    private static final float[] specularVector = {1.0f, 1.0f, 1.0f}; //normal white light

    private final Context context;
    private final float[] projectionMatrix = new float[16];

    private final float[] targetModelMatrix = new float[16];

    private final float[] modelMatrix = new float[16];
    private final float[] viewProjectionMatrix = new float[16];
    private final float[] invertedViewProjectionMatrix = new float[16];

    private float[] viewMatrix = new float[16];
    private float[] cameraMatrix = new float[16];
    private float[] inverseCameraMatrix = new float[16];

    private float[] testMatrix = new float[16];
    private float[] inverseTestCameraMatrix = new float[16];
    private Point testCameraPoint;

    private float[] bounding_box_info = new float[6];

    protected float avgDepthDistance = 0f;

    private static float STARTING_HEIGHT_ANGLE_KEY = 999f;

    private static float ABOVE_ANGLE_DEG = 45;

/* test short sequence */
    /*
    private static final float[] baseHeightTargetAngles = {
            -10f, STARTING_HEIGHT_ANGLE_KEY,
            10f, STARTING_HEIGHT_ANGLE_KEY
    };
    */

    private static final float[] baseHeightTargetAngles = {
            -25f, STARTING_HEIGHT_ANGLE_KEY,
            45f, STARTING_HEIGHT_ANGLE_KEY,
            90f, STARTING_HEIGHT_ANGLE_KEY,
            90f, ABOVE_ANGLE_DEG,
            90f, 0f,
            135f, STARTING_HEIGHT_ANGLE_KEY,
            180f, STARTING_HEIGHT_ANGLE_KEY, /*right half is done*/
            0f, STARTING_HEIGHT_ANGLE_KEY, /*backtrack on right side done*/
            0f, 0f,
            0f, ABOVE_ANGLE_DEG,
            -45f, STARTING_HEIGHT_ANGLE_KEY,
            -90f, STARTING_HEIGHT_ANGLE_KEY,
            -90f, ABOVE_ANGLE_DEG,
            -90f, 0f,
            -135f, STARTING_HEIGHT_ANGLE_KEY,
            -180f, STARTING_HEIGHT_ANGLE_KEY,
            -180f, ABOVE_ANGLE_DEG,
            -180f, 10f
    };


    private ColorShaderProgram colorProgram;
    private PhongShaderProgram phongProgram;
    private PhongShaderProgramWithNormal phongProgramWithNormal;
    private DomeArcs domeArcs;
    private AxisLine targetLine;
    private Cylinder cylinder;
    private Icosahedron_sub2 icosahedron;

    public Point originPointOfScan;

    private SensorManager mSensorManager;
    private float initial_pitch_deg;
    private float initial_roll_deg;

    private boolean initBBSet = false;

    private Point cameraPoint;
    private float camToOrigDistance;

    public boolean scanningStarted = false;
    public boolean scanningStop = false;

    public boolean newKeyFrameFlag = false;

    public boolean guidanceEnabled = true;

    private static float ORIGIN_DISTANCE_OFFSET = 0.1f;


    public GLGuidanceRenderer(Context context, float scanDistanceTooClose, float scanDistanceTooFar) {
        this.context = context;

        scanDistTooClose = scanDistanceTooClose;
        scanDistTooFar = scanDistanceTooFar;
        startTime = System.currentTimeMillis();

        Display display = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        if (Surface.ROTATION_0 == display.getRotation())
            depthSensorOnLandscapePhone = false;
        else
            depthSensorOnLandscapePhone = true;

    }


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        icosahedron = new Icosahedron_sub2();

        //domeArcs = new DomeArcs(DOME_RADIUS, DOME_LINES_STEP, 0, ABOVE_ANGLE_DEG);
        domeArcs = new DomeArcs(DOME_RADIUS, DOME_LINES_STEP, 0, 90);

        cylinder = new Cylinder(CYLINDER_HEIGHT / 2f, CYLINDER_RADIUS, CYLINDER_HEIGHT, 32, false);

        targetLine = new AxisLine(SPHERE_SCALE, DOME_RADIUS, 0.008f, AxisLine.AXIS.X);

        colorProgram = new ColorShaderProgram(context);
        phongProgram = new PhongShaderProgram(context);
        phongProgramWithNormal = new PhongShaderProgramWithNormal(context);

        mSensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);

        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR), SensorManager.SENSOR_DELAY_UI);

        originPointOfScan = new Point(0, 0, 0);

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {

        // Set the OpenGL viewport to fill the entire surface.
        glViewport(0, 0, width, height);

        setup_projection_matrix(width, height);

        //below two are for SSD
        GL2JNILib.initDrawer(width, height,
                MainActivity.ColorImageWidth, MainActivity.ColorImageHeight, //this width and height will be set based on LIVE_TRACKING flag in MainActivity
                MainActivity.DepthImageWidth, MainActivity.DepthImageHeight,
                context.getResources().getAssets());

        GL2JNILib.initPreview(context.getFilesDir().getPath(),
                context.getResources().getAssets());

    }

    private void setup_projection_matrix(int width, int height) {

        float aspect = (float) width / (float) height;
        float angle_rad = (float) Math.toRadians(90);

        float np = 0.1f;
        float fp = 10.0f;
        float w = (float) (2.0f * np * Math.tan(angle_rad / 2.0));
        float h = w / aspect;

        float depth = fp - np;
        float c = -(fp + np) / depth;
        float d = -(2.0f * fp * np) / depth;

        //copied from SurfaceSplattingDriver.cpp and Projection.cpp
        //transposed to switch to column-major
        float val0 = 2.0f * np / w; // this is == a
        float val5 = 2.0f * np / h; // this is == a * aspect

        projectionMatrix[0] = val0;
        projectionMatrix[5] = val5;
        projectionMatrix[10] = c;
        projectionMatrix[11] = -1.0f;
        projectionMatrix[14] = d;
    }

    private void printMatrix(float[] matrix) {

        Log.d(TAG, " [0  1   2  3] = " + matrix[0] + " " + matrix[1] + " " + matrix[2] + " " + matrix[3]);
        Log.d(TAG, " [4  5   6  7] = " + matrix[4] + " " + matrix[5] + " " + matrix[6] + " " + matrix[7]);
        Log.d(TAG, " [8  9  10 11] = " + matrix[8] + " " + matrix[9] + " " + matrix[10] + " " + matrix[11]);
        Log.d(TAG, " [12 13 14 15] = " + matrix[12] + " " + matrix[13] + " " + matrix[14] + " " + matrix[15]);

    }

    @Override
    public void onDrawFrame(GL10 gl) {

        if (!scanningQuit) {

            avgDepthDistance = GL2JNILib.getCameraData(cameraMatrix);

            if (scanningStarted) {

                //if (!scanningStop) {
                //camera matrix might not have changed, but that's fine, don't block the whole render
                //    avgDepthDistance = GL2JNILib.getCameraData(cameraMatrix);
                //    Log.i(TAG,"java avg depth reading = " + avgDepthDistance);
                //}

                boolean camMatrixChanged = !Arrays.equals(cameraMatrix, viewMatrix);
                GL2JNILib.step(scanningStop, camMatrixChanged); //for SSD "scanning" or "after scan" modes

            } else {
                //avgDepthDistance = GL2JNILib.getCameraData(cameraMatrix);
                GL2JNILib.stepPreview(); //for SSD preview mode
            }
        }

        if (scanningStarted) {

            //viewMatrix = cameraMatrix;
            viewMatrix = Arrays.copyOf(cameraMatrix, cameraMatrix.length);

            invertM(inverseCameraMatrix, 0, viewMatrix, 0);

            if (!initBBSet) {

                if (avgDepthDistance > 0) {

                    GL2JNILib.getBoundingBox(bounding_box_info);

                    originPointOfScan.x = bounding_box_info[0];
                    originPointOfScan.y = bounding_box_info[1];
                    originPointOfScan.z = bounding_box_info[2] + ORIGIN_DISTANCE_OFFSET; //pushes the origin point to slightly inside the object

                    Log.d(TAG, "placing blue origin point at X,Y,Z = " + originPointOfScan.x + "," + originPointOfScan.y + "," + originPointOfScan.z);

                    initBBSet = true;
                    mSensorManager.unregisterListener(this); //dont need to listen to pitch/roll updates when everything is set
                }


            }

            cameraPoint = new Point(inverseCameraMatrix[12], inverseCameraMatrix[13], inverseCameraMatrix[14]);

            //drawObjects = true;

            //camToOrigDistance = Geometry.TwoPointDistance(originPointOfScan, cameraPoint);
            //Log.d(TAG, "camToOrigDistance = " + camToOrigDistance);

        }

        Geometry.Ray ray = convertNormalized2DPointToRay(0, 0);
        float cameraRayToOriginDistance = Geometry.distanceBetween(originPointOfScan, ray);

        DecimalFormat df = new DecimalFormat("#.##");
        String avgDepthString = df.format(avgDepthDistance);

        if (cameraRayToOriginDistance > ORIGIN_TARGETING_TOLERANCE)
            setUIWarningMsg(avgDepthString + " angle too big");
        else if (avgDepthDistance > scanDistTooFar)
            setUIWarningMsg(avgDepthString + " avg dist too far");
        else if (avgDepthDistance < scanDistTooClose)
            setUIWarningMsg(avgDepthString + " avg dist too close");
        else
            setUIWarningMsg(avgDepthString);//clearUIWarningMsg();

        if (initBBSet && scanningStarted && !scanningStop & guidanceEnabled) {

            //Log.d(TAG,"drawing objects in Java OpenGL");

            if (newKeyFrameFlag) {
                newKeyFrameFlag = false; //clear the flag
                keyframeCnt++;
            }

            // Clear the rendering surface.
            //just clear depth because SSD already cleared color

            glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT);

            //glEnable(GLES30.GL_CULL_FACE);
            //glCullFace(GLES30.GL_BACK);

            glEnable(GLES30.GL_BLEND);
            glBlendEquation(GLES30.GL_FUNC_ADD);
            glBlendFunc(GLES30.GL_SRC_ALPHA, GLES30.GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GLES30.GL_DEPTH_TEST);

            // Update the viewProjection matrix, and create an inverted matrix for
            // touch picking.
            multiplyMM(viewProjectionMatrix, 0, projectionMatrix, 0,
                    viewMatrix, 0);

            invertM(invertedViewProjectionMatrix, 0, viewProjectionMatrix, 0);

            if (renderOrigin)
                drawOriginMarker();

            if (renderTarget)
                drawTarget();

            draw_arcs();

        }


    }

    private void setUIWarningMsg(final String warningMsg) {
        ((MainActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((MainActivity) context).infoTextView.setText(warningMsg);
            }
        });

    }


    private void draw_arcs() {

        setIdentityM(modelMatrix, 0);

        translateM(modelMatrix, 0, originPointOfScan.x, originPointOfScan.y, originPointOfScan.z);

        //account for the initial camera 180 degree rotation on the x axis
        //as well as the initial_pitch_deg of how the device is held when scan is started (90 + initial_pitch_deg)
        //rotateM(modelMatrix, 0, 180f, 1, 0, 0); //initial_pitch_deg about the x axis
        rotateM(modelMatrix, 0, 180f + initial_pitch_deg, 1, 0, 0); //initial_pitch_deg about the x axis

        if (depthSensorOnLandscapePhone)
            rotateM(modelMatrix, 0, 90f + initial_roll_deg, 0, 0, 1); //initial_roll_deg about the z axis
        else
            rotateM(modelMatrix, 0, initial_roll_deg, 0, 0, 1); //initial_roll_deg about the z axis


        colorProgram.useProgram();
        colorProgram.setUniforms(
                viewProjectionMatrix,
                modelMatrix,
                1,
                1,
                1,
                0.15f,
                camToOrigDistance);

        domeArcs.bindData(colorProgram);
        domeArcs.draw();

    }

    private void positionTarget() {

        setIdentityM(targetModelMatrix, 0);
        //first translate to the origin
        translateM(targetModelMatrix, 0, originPointOfScan.x, originPointOfScan.y, originPointOfScan.z);

        //adjust for initial device initial_pitch_deg/initial_roll_deg
        rotateM(targetModelMatrix, 0, initial_pitch_deg, 1, 0, 0); //initial_pitch_deg about the x axis

        if (depthSensorOnLandscapePhone)
            rotateM(targetModelMatrix, 0, -90f - initial_roll_deg, 0, 0, 1); //initial_roll_deg about the z axis
        else
            rotateM(targetModelMatrix, 0, -initial_roll_deg, 0, 0, 1); //initial_roll_deg about the z axis

        //to place the target at the dome radius tangential facing towards the origin
        //first rotate about the y axis according to base angle
        rotateM(targetModelMatrix, 0, -mTargetAngles.currentBaseAngle, 0, 1, 0);
        //then tilt 90 minus the angle height on x axis
        rotateM(targetModelMatrix, 0, 90f - mTargetAngles.currentHeightAngle, 1, 0, 0);
        //then push the target out to the radius by translating on the y
        translateM(targetModelMatrix, 0, 0, -DOME_RADIUS, 0);

    }

    private void calculateCamAngleFromBase() {

        //adjusting the camera matrix to align with the rotation of the hemisphere towards ground plane
        System.arraycopy(cameraMatrix, 0, testMatrix, 0, cameraMatrix.length);
        translateM(testMatrix, 0, originPointOfScan.x, originPointOfScan.y, originPointOfScan.z);
        rotateM(testMatrix, 0, initial_pitch_deg, 1, 0, 0); //initial_pitch_deg about the x axis
        rotateM(testMatrix, 0, -initial_roll_deg, 0, 0, 1);

        invertM(inverseTestCameraMatrix, 0, testMatrix, 0);
        testCameraPoint = new Point(inverseTestCameraMatrix[12], inverseTestCameraMatrix[13], inverseTestCameraMatrix[14]);

        if (testCameraPoint.isNonZero()) {
        /*
         * due to the nature of atan2, the angle calculated will be
         * 0 degrees at front,
         * 1 to 180 degrees as you walk to the right of the origin to the back
         * -1 to -180 degrees as you walk to the left of the origin to the back
         */
            float camAngleBaseRad = (float) Math.atan2(testCameraPoint.x, -testCameraPoint.z);
            float camAngleBaseDegFromFront = (float) Math.toDegrees(camAngleBaseRad);
            //final String formatted = String.format("Angle: %.1f KeyFrames: %d", camAngleBaseDegFromFront, keyframeCnt);

        }


    }

    private float getHeightAngle() {
        if (baseHeightTargetAngles[targetIndx + 1] == STARTING_HEIGHT_ANGLE_KEY)
            return initial_pitch_deg;
        else
            return baseHeightTargetAngles[targetIndx + 1];
    }

    private void drawTarget() {

        //calculateCamAngleFromBase();

        if (firstTarget) {
            firstTarget = false;

            mTargetAngles = new TargetAngles(baseHeightTargetAngles[targetIndx],
                    getHeightAngle(),
                    baseHeightTargetAngles[targetIndx],
                    getHeightAngle());

            positionTarget();
        }

        Point modelPoint = new Point(targetModelMatrix[12], targetModelMatrix[13], targetModelMatrix[14]);
        Geometry.Sphere targetSphere = new Geometry.Sphere(modelPoint, TARGET_TOLERANCE_RADIUS);
        Ray ray = convertNormalized2DPointToRay(0, 0);
        boolean targetIntersected = Geometry.intersects(targetSphere, ray) &&
                Geometry.TwoPointDistance(modelPoint, cameraPoint) < 1.0f;

        if (targetIntersected && mTargetAngles.converged) {

            ((MainActivity) context).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ((MainActivity) context).targetHit();
                }
            });

            targetIndx += 2; //go to next target

            if (targetIndx >= baseHeightTargetAngles.length) {
                //last target was hit so disable guidance object renders
                renderTarget = false;
                renderOrigin = false;

                ((MainActivity) context).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        ((MainActivity) context).showHintDialog();
                    }
                });

            } else {
                mTargetAngles.setNextBaseAngle(baseHeightTargetAngles[targetIndx]);
                mTargetAngles.setNextHeightAngle(getHeightAngle());
            }

        }

        if (renderTarget) {
            mTargetAngles.incrementAngles(); //update target location
            positionTarget();
            positionLineToTarget();

            phongProgramWithNormal.useProgram();
            phongProgramWithNormal.setUniforms(
                    viewProjectionMatrix,        //projection matrix
                    targetModelMatrix,
                    diffuseVectorYellow,
                    specularVector,
                    TARGET_BALL_VISIBLE_ALPHA,
                    camToOrigDistance);

            cylinder.bindDataPhong(phongProgramWithNormal);
            cylinder.draw();
        }
    }

    private void positionLineToTarget() {

        setIdentityM(modelMatrix, 0);
        translateM(modelMatrix, 0, originPointOfScan.x, originPointOfScan.y, originPointOfScan.z);
        rotateM(modelMatrix, 0, initial_pitch_deg, 1, 0, 0); //initial_pitch_deg about the x axis

        if (depthSensorOnLandscapePhone)
            rotateM(modelMatrix, 0, -90f - initial_roll_deg, 0, 0, 1); //initial_roll_deg about the z axis
        else
            rotateM(modelMatrix, 0, -initial_roll_deg, 0, 0, 1); //initial_roll_deg about the z axis

        rotateM(modelMatrix, 0, 90, 0, 1, 0); //bring to 0 degrees facing towards camera

        rotateM(modelMatrix, 0, -mTargetAngles.currentBaseAngle, 0, 1, 0);
        rotateM(modelMatrix, 0, -mTargetAngles.currentHeightAngle, 0, 0, 1);

        colorProgram.useProgram();
        colorProgram.setUniforms(
                viewProjectionMatrix,
                modelMatrix,
                1,
                1,
                0,
                0.5f,
                camToOrigDistance);

        targetLine.bindData(colorProgram);
        targetLine.draw();

    }


    private void drawOriginMarker() {

        setIdentityM(modelMatrix, 0);

        //first translate to the origin
        translateM(modelMatrix, 0, originPointOfScan.x, originPointOfScan.y, originPointOfScan.z);

        scaleM(modelMatrix, 0, SPHERE_SCALE, SPHERE_SCALE, SPHERE_SCALE);

        phongProgram.useProgram();

        phongProgram.setUniforms(
                viewProjectionMatrix,
                modelMatrix,
                diffuseVectorBlue,
                specularVector,
                1.0f,
                camToOrigDistance);

        icosahedron.bindData(phongProgram);
        icosahedron.draw();

    }

    private Geometry.Ray convertNormalized2DPointToRay(
            float normalizedX, float normalizedY) {
        // We'll convert these normalized device coordinates into world-space
        // coordinates. We'll pick a point on the near and far planes, and draw a
        // line between them. To do this transform, we need to first multiply by
        // the inverse matrix, and then we need to undo the perspective divide.
        final float[] nearPointNdc = {normalizedX, normalizedY, -1, 1};
        final float[] farPointNdc = {normalizedX, normalizedY, 1, 1};

        final float[] nearPointWorld = new float[4];
        final float[] farPointWorld = new float[4];

        multiplyMV(
                nearPointWorld, 0, invertedViewProjectionMatrix, 0, nearPointNdc, 0);
        multiplyMV(
                farPointWorld, 0, invertedViewProjectionMatrix, 0, farPointNdc, 0);

        // Why are we dividing by W? We multiplied our vector by an inverse
        // matrix, so the W value that we end up is actually the *inverse* of
        // what the projection matrix would create. By dividing all 3 components
        // by W, we effectively undo the hardware perspective divide.
        divideByW(nearPointWorld);
        divideByW(farPointWorld);

        // We don't care about the W value anymore, because our points are now
        // in world coordinates.
        Point nearPointRay =
                new Point(nearPointWorld[0], nearPointWorld[1], nearPointWorld[2]);

        Point farPointRay =
                new Point(farPointWorld[0], farPointWorld[1], farPointWorld[2]);

        return new Ray(nearPointRay,
                Geometry.vectorBetween(nearPointRay, farPointRay));
    }

    private void divideByW(float[] vector) {
        vector[0] /= vector[3];
        vector[1] /= vector[3];
        vector[2] /= vector[3];
    }


    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {

        if (sensorEvent.sensor.getType() == Sensor.TYPE_GAME_ROTATION_VECTOR) {

            float[] rotationValues = new float[3];
            float[] mRotationMatrix = new float[16];
            SensorManager.getRotationMatrixFromVector(mRotationMatrix, sensorEvent.values);

            SensorManager
                    .remapCoordinateSystem(mRotationMatrix,
                            SensorManager.AXIS_X, SensorManager.AXIS_Z,
                            mRotationMatrix);

            SensorManager.getOrientation(mRotationMatrix, rotationValues);

            initial_pitch_deg = (float) Math.toDegrees(rotationValues[1]);
            initial_roll_deg = (float) Math.toDegrees(rotationValues[2]);

            ((MainActivity) context).runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ((MainActivity) context).initialSensorPitchDeg = initial_pitch_deg;
                    ((MainActivity) context).initialSensorRollDeg = initial_roll_deg;
                }
            });


            //Log.d(TAG, "rot[0] = " + (float)Math.toDegrees(rotationValues[0]));
            //Log.d(TAG, "rot[1] = " + (float)Math.toDegrees(rotationValues[1]));
            //Log.d(TAG, "rot[2] = " + (float)Math.toDegrees(rotationValues[2]));

        }


    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        //do nothing
    }

    public native void setJava();


}

/************************************************************************************
 * Copyright (c) 2012 Paul Lawitzki
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ************************************************************************************/
package com.qualcomm.qti.clevercapture.hardware;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Handler;
import android.util.Log;
import java.math.RoundingMode;
import java.text.DecimalFormat;
import java.util.Timer;
import java.util.TimerTask;
import java.lang.Math;
import java.lang.ref.WeakReference;

public class SensorFusion
implements SensorEventListener {

    private SensorManager mSensorManager = null;

    private static CleverCaptureActor.EventListener mCleverCaptureCallback;

    // angular speeds from gyro
    private float[] gyro = new float[3];

    // rotation matrix from gyro data
    private float[] gyroMatrix = new float[9];

    // orientation angles from gyro matrix
    private float[] gyroOrientation = new float[3];

    // magnetic field vector
    private float[] magnet = new float[3];

    // accelerometer vector
    private float[] accel = new float[3];

    // orientation angles from accel and magnet
    private float[] accMagOrientation = new float[3];

    // final orientation angles from sensor fusion
    private float[] fusedOrientation = new float[3];

    // accelerometer and magnetometer based rotation matrix
    private float[] rotationMatrix = new float[9];

    public static final float EPSILON = 0.000000001f;
    private static final float NS2S = 1.0f / 1000000000.0f;
    private long timestamp;
    private boolean initState = true;

    public static final int TIME_CONSTANT = 30;
    public static final float FILTER_COEFFICIENT = 0.98f;
    private Timer fuseTimer = new Timer();

    // The following members are only for displaying the sensor output.
    public Handler mHandler;

    private DecimalFormat d = new DecimalFormat("#.##");

    private int gyroCurrentArrayLength=6;
    private float []gyroCurrentXArray= new float[gyroCurrentArrayLength];
    private float []gyroCurrentYArray= new float[gyroCurrentArrayLength];
    private float []gyroCurrentZArray= new float[gyroCurrentArrayLength];
    private float magLowValue = 0.01f;

     private float diffX=(float)0.0;
     private float diffY=(float)0.0;
     private float diffZ=(float)0.0;

     private float avgX=(float)0.0;
     private float avgY=(float)0.0;
     private float avgZ=(float)0.0;


    private int threshold = 30;
    protected static WeakReference<Context> appCtx=null;

    private static  SensorFusion instance = null;


    SensorFusion(Context ctx) {
        appCtx=new WeakReference<Context>(ctx.getApplicationContext());

        // get sensorManager and initialise sensor listeners
        mSensorManager = (SensorManager) appCtx.get().getSystemService(Context.SENSOR_SERVICE);
        //setParameters(1);
        threshold = 4;

        // GUI stuff
        mHandler = new Handler();
        d.setRoundingMode(RoundingMode.HALF_UP);
        d.setMaximumFractionDigits(3);
        d.setMinimumFractionDigits(3);

   }

    public static SensorFusion getInstance(Context ctx)
       {

           Log.e("SensorFusion","Get Instance Called");
             if(instance==null || !ctx.equals(appCtx.get()))
          {
             instance = new SensorFusion(ctx);
          }
          return instance;

       }

    public void onStop() {
        mSensorManager.unregisterListener(this);
        deinitListeners();
        fuseTimer.cancel();
        fuseTimer = null;
    }

    protected void onPause() {
        mSensorManager.unregisterListener(this);
        deinitListeners();
        fuseTimer.cancel();
        fuseTimer = null;
    }


    public void onResume()
    {
        initListeners();

        fuseTimer = new Timer();
        // wait for one second until gyroscope and magnetometer/accelerometer
        // data is initialised then scedule the complementary filter task
        fuseTimer.scheduleAtFixedRate(new calculateFusedOrientationTask(),
                                      1000, TIME_CONSTANT);
    }

    // This function registers sensor listeners for the accelerometer, magnetometer and gyroscope.
    public void initListeners(){
        gyroOrientation[0] = 0.0f;
        gyroOrientation[1] = 0.0f;
        gyroOrientation[2] = 0.0f;

        for(int i=0;i<gyroCurrentArrayLength;i++)
        {
            gyroCurrentXArray[i]= 0.0f;
            gyroCurrentYArray[i]= 0.0f;
            gyroCurrentZArray[i]= 0.0f;

        }

        diffX=0.0f;
        diffY=0.0f;
        diffZ=0.0f;

        avgX=0.0f;
        avgY=0.0f;
        avgZ=0.0f;

        // initialise gyroMatrix with identity matrix
        gyroMatrix[0] = 1.0f; gyroMatrix[1] = 0.0f; gyroMatrix[2] = 0.0f;
        gyroMatrix[3] = 0.0f; gyroMatrix[4] = 1.0f; gyroMatrix[5] = 0.0f;
        gyroMatrix[6] = 0.0f; gyroMatrix[7] = 0.0f; gyroMatrix[8] = 1.0f;

        mSensorManager.registerListener(this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
            SensorManager.SENSOR_DELAY_GAME);

        mSensorManager.registerListener(this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE),
            SensorManager.SENSOR_DELAY_GAME);

        mSensorManager.registerListener(this,
            mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD),
            SensorManager.SENSOR_DELAY_GAME);
    }

    public void deinitListeners() {
        mSensorManager.unregisterListener(this);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        switch(event.sensor.getType()) {
        case Sensor.TYPE_ACCELEROMETER:
            // copy new accelerometer data into accel array and calculate orientation
            System.arraycopy(event.values, 0, accel, 0, 3);
            calculateAccMagOrientation();
            break;

        case Sensor.TYPE_GYROSCOPE:
            // process gyro data
            gyroFunction(event);
            break;

        case Sensor.TYPE_MAGNETIC_FIELD:
            // copy new magnetometer data into magnet array
            System.arraycopy(event.values, 0, magnet, 0, 3);
            magnet[0]=magLowValue;
            magnet[1]=magLowValue;
            magnet[2]=magLowValue;
            break;
        }
    }

    // calculates orientation angles from accelerometer and magnetometer output
    public void calculateAccMagOrientation() {
        if(SensorManager.getRotationMatrix(rotationMatrix, null, accel, magnet)) {
            SensorManager.getOrientation(rotationMatrix, accMagOrientation);
        }
    }

    // This function is borrowed from the Android reference
    // at http://developer.android.com/reference/android/hardware/SensorEvent.html#values
    // It calculates a rotation vector from the gyroscope angular speed values.
    private void getRotationVectorFromGyro(float[] gyroValues,
            float[] deltaRotationVector,
            float timeFactor)
    {
        float[] normValues = new float[3];

        // Calculate the angular speed of the sample
        float omegaMagnitude =
        (float)Math.sqrt(gyroValues[0] * gyroValues[0] +
        gyroValues[1] * gyroValues[1] +
        gyroValues[2] * gyroValues[2]);

        // Normalize the rotation vector if it's big enough to get the axis
        if(omegaMagnitude > EPSILON) {
        normValues[0] = gyroValues[0] / omegaMagnitude;
        normValues[1] = gyroValues[1] / omegaMagnitude;
        normValues[2] = gyroValues[2] / omegaMagnitude;
        }

        // Integrate around this axis with the angular speed by the timestep
        // in order to get a delta rotation from this sample over the timestep
        // We will convert this axis-angle representation of the delta rotation
        // into a quaternion before turning it into the rotation matrix.
        float thetaOverTwo = omegaMagnitude * timeFactor;
        float sinThetaOverTwo = (float)Math.sin(thetaOverTwo);
        float cosThetaOverTwo = (float)Math.cos(thetaOverTwo);
        deltaRotationVector[0] = sinThetaOverTwo * normValues[0];
        deltaRotationVector[1] = sinThetaOverTwo * normValues[1];
        deltaRotationVector[2] = sinThetaOverTwo * normValues[2];
        deltaRotationVector[3] = cosThetaOverTwo;
    }

    // This function performs the integration of the gyroscope data.
    // It writes the gyroscope based orientation into gyroOrientation.
    public void gyroFunction(SensorEvent event) {
        // don't start until first accelerometer/magnetometer orientation has been acquired
        if (accMagOrientation == null)
            return;

        // initialisation of the gyroscope based rotation matrix
        if(initState) {
            float[] initMatrix = new float[9];
            initMatrix = getRotationMatrixFromOrientation(accMagOrientation);
            float[] test = new float[3];
            SensorManager.getOrientation(initMatrix, test);
            gyroMatrix = matrixMultiplication(gyroMatrix, initMatrix);
            initState = false;
        }

        // copy the new gyro values into the gyro array
        // convert the raw gyro data into a rotation vector
        float[] deltaVector = new float[4];
        if(timestamp != 0) {
            final float dT = (event.timestamp - timestamp) * NS2S;
        System.arraycopy(event.values, 0, gyro, 0, 3);
        getRotationVectorFromGyro(gyro, deltaVector, dT / 2.0f);
        }

        // measurement done, save current time for next interval
        timestamp = event.timestamp;

        // convert rotation vector into rotation matrix
        float[] deltaMatrix = new float[9];
        SensorManager.getRotationMatrixFromVector(deltaMatrix, deltaVector);

        // apply the new rotation interval on the gyroscope based rotation matrix
        gyroMatrix = matrixMultiplication(gyroMatrix, deltaMatrix);

        // get the gyroscope based orientation from the rotation matrix
        SensorManager.getOrientation(gyroMatrix, gyroOrientation);
    }

    private float[] getRotationMatrixFromOrientation(float[] o) {
        float[] xM = new float[9];
        float[] yM = new float[9];
        float[] zM = new float[9];

        float sinX = (float)Math.sin(o[1]);
        float cosX = (float)Math.cos(o[1]);
        float sinY = (float)Math.sin(o[2]);
        float cosY = (float)Math.cos(o[2]);
        float sinZ = (float)Math.sin(o[0]);
        float cosZ = (float)Math.cos(o[0]);

        // rotation about x-axis (pitch)
        xM[0] = 1.0f; xM[1] = 0.0f; xM[2] = 0.0f;
        xM[3] = 0.0f; xM[4] = cosX; xM[5] = sinX;
        xM[6] = 0.0f; xM[7] = -sinX; xM[8] = cosX;

        // rotation about y-axis (roll)
        yM[0] = cosY; yM[1] = 0.0f; yM[2] = sinY;
        yM[3] = 0.0f; yM[4] = 1.0f; yM[5] = 0.0f;
        yM[6] = -sinY; yM[7] = 0.0f; yM[8] = cosY;

        // rotation about z-axis (azimuth)
        zM[0] = cosZ; zM[1] = sinZ; zM[2] = 0.0f;
        zM[3] = -sinZ; zM[4] = cosZ; zM[5] = 0.0f;
        zM[6] = 0.0f; zM[7] = 0.0f; zM[8] = 1.0f;

        // rotation order is y, x, z (roll, pitch, azimuth)
        float[] resultMatrix = matrixMultiplication(xM, yM);
        resultMatrix = matrixMultiplication(zM, resultMatrix);
        return resultMatrix;
    }

    private float[] matrixMultiplication(float[] A, float[] B) {
        float[] result = new float[9];

        result[0] = A[0] * B[0] + A[1] * B[3] + A[2] * B[6];
        result[1] = A[0] * B[1] + A[1] * B[4] + A[2] * B[7];
        result[2] = A[0] * B[2] + A[1] * B[5] + A[2] * B[8];

        result[3] = A[3] * B[0] + A[4] * B[3] + A[5] * B[6];
        result[4] = A[3] * B[1] + A[4] * B[4] + A[5] * B[7];
        result[5] = A[3] * B[2] + A[4] * B[5] + A[5] * B[8];

        result[6] = A[6] * B[0] + A[7] * B[3] + A[8] * B[6];
        result[7] = A[6] * B[1] + A[7] * B[4] + A[8] * B[7];
        result[8] = A[6] * B[2] + A[7] * B[5] + A[8] * B[8];

        return result;
    }

    class calculateFusedOrientationTask extends TimerTask {
        public void run() {
            float oneMinusCoeff = 1.0f - FILTER_COEFFICIENT;

            /*
             * Fix for 179? <--> -179? transition problem:
             * Check whether one of the two orientation angles (gyro or accMag) is negative while the other one is positive.
             * If so, add 360? (2 * math.PI) to the negative value, perform the sensor fusion, and remove the 360? from the result
             * if it is greater than 180?. This stabilizes the output in positive-to-negative-transition cases.
             */

            // azimuth
            if (gyroOrientation[0] < -0.5 * Math.PI && accMagOrientation[0] > 0.0) {
                fusedOrientation[0] = (float) (FILTER_COEFFICIENT * (gyroOrientation[0] + 2.0 * Math.PI) + oneMinusCoeff * accMagOrientation[0]);
                fusedOrientation[0] -= (fusedOrientation[0] > Math.PI) ? 2.0 * Math.PI : 0;
            }
            else if (accMagOrientation[0] < -0.5 * Math.PI && gyroOrientation[0] > 0.0) {
                fusedOrientation[0] = (float) (FILTER_COEFFICIENT * gyroOrientation[0] + oneMinusCoeff * (accMagOrientation[0] + 2.0 * Math.PI));
                fusedOrientation[0] -= (fusedOrientation[0] > Math.PI)? 2.0 * Math.PI : 0;
            }
            else {
                fusedOrientation[0] = FILTER_COEFFICIENT * gyroOrientation[0] + oneMinusCoeff * accMagOrientation[0];
            }

            // pitch
            if (gyroOrientation[1] < -0.5 * Math.PI && accMagOrientation[1] > 0.0) {
                fusedOrientation[1] = (float) (FILTER_COEFFICIENT * (gyroOrientation[1] + 2.0 * Math.PI) + oneMinusCoeff * accMagOrientation[1]);
                fusedOrientation[1] -= (fusedOrientation[1] > Math.PI) ? 2.0 * Math.PI : 0;
            }
            else if (accMagOrientation[1] < -0.5 * Math.PI && gyroOrientation[1] > 0.0) {
                fusedOrientation[1] = (float) (FILTER_COEFFICIENT * gyroOrientation[1] + oneMinusCoeff * (accMagOrientation[1] + 2.0 * Math.PI));
                fusedOrientation[1] -= (fusedOrientation[1] > Math.PI)? 2.0 * Math.PI : 0;
            }
            else {
                fusedOrientation[1] = FILTER_COEFFICIENT * gyroOrientation[1] + oneMinusCoeff * accMagOrientation[1];
            }

            // roll
            if (gyroOrientation[2] < -0.5 * Math.PI && accMagOrientation[2] > 0.0) {
                fusedOrientation[2] = (float) (FILTER_COEFFICIENT * (gyroOrientation[2] + 2.0 * Math.PI) + oneMinusCoeff * accMagOrientation[2]);
                fusedOrientation[2] -= (fusedOrientation[2] > Math.PI) ? 2.0 * Math.PI : 0;
            }
            else if (accMagOrientation[2] < -0.5 * Math.PI && gyroOrientation[2] > 0.0) {
                fusedOrientation[2] = (float) (FILTER_COEFFICIENT * gyroOrientation[2] + oneMinusCoeff * (accMagOrientation[2] + 2.0 * Math.PI));
                fusedOrientation[2] -= (fusedOrientation[2] > Math.PI)? 2.0 * Math.PI : 0;
            }
            else {
                fusedOrientation[2] = FILTER_COEFFICIENT * gyroOrientation[2] + oneMinusCoeff * accMagOrientation[2];
            }

            // overwrite gyro matrix and orientation with fused orientation
            // to comensate gyro drift
            gyroMatrix = getRotationMatrixFromOrientation(fusedOrientation);
            System.arraycopy(fusedOrientation, 0, gyroOrientation, 0, 3);


            // update sensor output in GUI
            mHandler.post(updateOrientationDisplayTask);
        }
    }




    public void IIRFilter()
    {
        float factor = 0.95f;
        avgX= avgX*factor + (1-factor)*gyroCurrentXArray[0];
        avgY= avgY*factor + (1-factor)*gyroCurrentYArray[0];
        avgZ= avgZ*factor + (1-factor)*gyroCurrentZArray[0];
        diffX=gyroCurrentXArray[0]-avgX;
        diffY=gyroCurrentYArray[0]-avgY;
        diffZ=gyroCurrentZArray[0]-avgZ;


    }

    public void FIRFilter()
    {

        float sumX=0.0f,sumY=0.0f,sumZ=0.0f;
        for(int i=1;i<gyroCurrentArrayLength;i++)
        {
            sumX += gyroCurrentXArray[i];
            sumY += gyroCurrentYArray[i];
            sumZ += gyroCurrentZArray[i];
        }

        avgX = (sumX/(gyroCurrentArrayLength-1));
        avgY = (sumY/(gyroCurrentArrayLength-1));
        avgZ = (sumZ/(gyroCurrentArrayLength-1));


        diffX=gyroCurrentXArray[0]-avgX;
        diffY=gyroCurrentYArray[0]-avgY;
        diffZ=gyroCurrentZArray[0]-avgZ;


    }


public void updateOrientationDisplay() {

        //gyrocurrentx=(float)(gyroOrientation[0] * 180/Math.PI);
        //gyrocurrenty=(float)(gyroOrientation[1] * 180/Math.PI);
        //gyrocurrentz=(float)(gyroOrientation[2] * 180/Math.PI);

        gyroCurrentXArray[0]=(float)(fusedOrientation[0] * 180/Math.PI);
        gyroCurrentYArray[0]=(float)(fusedOrientation[1] * 180/Math.PI);
        gyroCurrentZArray[0]=(float)(fusedOrientation[2] * 180/Math.PI);

        //FIRFilter();

        float currentMeanDiff=Math.abs(diffX)+Math.abs(diffY)+Math.abs(diffZ);
        //Log.i("SensorFusion"," accelerometer="+acc+" magnetometer="+mag+" gyroscope="+gyr+" threshold="+threshold+"diffx="+diffx+" diffy="+diffy+" diffz="+diffz+" gyrocurrentx="+gyrocurrentx+" gyrocurrenty="+gyrocurrenty+" gyrocurrentz="+gyrocurrentz+" avgx="+avgx+" avgy="+avgy+" avgz="+avgz+" diffcurrentx="+diffcurrentx+" diffcurrenty="+diffcurrenty+" diffcurrentz="+diffcurrentz);
        //Log.i("SensorFusion"," currentMeanDiff="+currentMeanDiff+" currentDiff="+currentDiff+" avgx="+avgx+" avgy="+avgy+" avgz="+avgz+" gyrocurrentx="+gyrocurrentx+" gyrocurrenty="+gyrocurrenty+" gyrocurrentz="+gyrocurrentz+" gyroprevx="+gyroprevx+" gyroprevy="+gyroprevy+" gyroprevz="+gyroprevz+" gyroprevx1="+gyroprevx1+" gyroprevy1="+gyroprevy1+" gyroprevz1="+gyroprevz1+" gyroprevx2="+gyroprevx2+" gyroprevy2="+gyroprevy2+" gyroprevz2="+gyroprevz2+" gyroprevx3="+gyroprevx3+" gyroprevy3="+gyroprevy3+" gyroprevz3="+gyroprevz3+" diffcurrentx="+diffcurrentx+" diffcurrenty="+diffcurrenty+" diffcurrentz="+diffcurrentz);

        // Log.e("SensorFusion","Raw Gyro Values and Absolute Sum,"+System.currentTimeMillis()+","+(gyroCurrentXArray[0])+","+Math.abs(gyroCurrentYArray[0])+","+(gyroCurrentZArray[0])+","+currentMeanDiff);

        if(currentMeanDiff >= threshold)
        {
            if (mCleverCaptureCallback !=null) {
                //Log.i("SensorFusion", "High Motion Detected");
                mCleverCaptureCallback.onHighMotion(true);
            }
        }
        else
        {
            if (mCleverCaptureCallback !=null) {
                //Log.i("SensorFusion", "Low Motion Detected");
            mCleverCaptureCallback.onHighMotion(false);
            }
        }

        //Update Gyro Current for Previous Frames
        for(int i=gyroCurrentArrayLength-1;i>=1;i--)
        {
            gyroCurrentXArray[i]=gyroCurrentXArray[i-1];
            gyroCurrentYArray[i]=gyroCurrentYArray[i-1];
            gyroCurrentZArray[i]=gyroCurrentZArray[i-1];
        }

        IIRFilter();
   }

    public void registerCallbackCleverCapture(CleverCaptureActor.EventListener sb)
    {
        mCleverCaptureCallback = sb;
    }
    public void setParameters(int scale)
    {
        threshold = scale*6;

    }

    public void deRegisterCallbackCleverCapture(CleverCaptureActor.EventListener sb) {

        Log.i("SensorFusion","Deregister Called");

        mSensorManager.flush(this);
        mSensorManager.unregisterListener(this);
        mCleverCaptureCallback=null;

    }


    private Runnable updateOrientationDisplayTask = new Runnable() {
        public void run() {
            updateOrientationDisplay();
        }
    };
}

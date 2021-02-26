/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.face;

public class FaceLandmark {
    public int leftEyeX;
    public int leftEyeY;
    public int rightEyeX;
    public int rightEyeY;
    public int leftEyeInnerCornerX;
    public int leftEyeInnerCornerY;
    public int leftEyeOuterCornerX;
    public int leftEyeOuterCornerY;
    public int rightEyeInnerCornerX;
    public int rightEyeInnerCornerY;
    public int rightEyeOuterCornerX;
    public int rightEyeOuterCornerY;
    public int mouthLeftX;
    public int mouthLeftY;
    public int mouthRightX;
    public int mouthRightY;
    public int noseLeftX;
    public int noseLeftY;
    public int noseRightX;
    public int noseRightY;
    public int mouthUpX;
    public int mouthUpY;

    public FaceLandmark() {
        setLandmarkInfos(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    public FaceLandmark(FaceLandmark instance) {
        leftEyeX = instance.leftEyeX;
        leftEyeY = instance.leftEyeY;
        rightEyeX = instance.rightEyeX;
        rightEyeY = instance.rightEyeY;
        leftEyeInnerCornerX = instance.leftEyeInnerCornerX;
        leftEyeInnerCornerY = instance.leftEyeInnerCornerY;
        leftEyeOuterCornerX = instance.leftEyeOuterCornerX;
        leftEyeOuterCornerY = instance.leftEyeOuterCornerY;
        rightEyeInnerCornerX = instance.rightEyeInnerCornerX;
        rightEyeInnerCornerY = instance.rightEyeInnerCornerY;
        rightEyeOuterCornerX = instance.rightEyeOuterCornerX;
        rightEyeOuterCornerY = instance.rightEyeOuterCornerY;
        mouthLeftX = instance.mouthLeftX;
        mouthLeftY = instance.mouthLeftY;
        mouthRightX = instance.mouthRightX;
        mouthRightY = instance.mouthRightY;
        noseLeftX = instance.noseLeftX;
        noseLeftY = instance.noseLeftY;
        noseRightX = instance.noseRightX;
        noseRightY = instance.noseRightY;
        mouthUpX = instance.mouthUpX;
        mouthUpY = instance.mouthUpY;
    }

    public void setLandmarkInfos(FaceLandmark landmark) {
        leftEyeX = landmark.leftEyeX;
        leftEyeY = landmark.leftEyeY;
        rightEyeX = landmark.rightEyeX;
        rightEyeY = landmark.rightEyeY;
        leftEyeInnerCornerX = landmark.leftEyeInnerCornerX;
        leftEyeInnerCornerY = landmark.leftEyeInnerCornerY;
        leftEyeOuterCornerX = landmark.leftEyeOuterCornerX;
        leftEyeOuterCornerY = landmark.leftEyeOuterCornerY;
        rightEyeInnerCornerX = landmark.rightEyeInnerCornerX;
        rightEyeInnerCornerY = landmark.rightEyeInnerCornerY;
        rightEyeOuterCornerX = landmark.rightEyeOuterCornerX;
        rightEyeOuterCornerY = landmark.rightEyeOuterCornerY;
        mouthLeftX = landmark.mouthLeftX;
        mouthLeftY = landmark.mouthLeftY;
        mouthRightX = landmark.mouthRightX;
        mouthRightY = landmark.mouthRightY;
        noseLeftX = landmark.noseLeftX;
        noseLeftY = landmark.noseLeftY;
        noseRightX = landmark.noseRightX;
        noseRightY = landmark.noseRightY;
        mouthUpX = landmark.mouthUpX;
        mouthUpY = landmark.mouthUpY;
    }

    /**
     * Constructor for Facelandmark from int array values
     * 
     * @param faceData
     *            face data in int array
     * @param index
     *            starting position in index array
     */
    public FaceLandmark(int[] faceData, int index) {
        leftEyeX = faceData[index++];
        leftEyeY = faceData[index++];
        rightEyeX = faceData[index++];
        rightEyeY = faceData[index++];
        leftEyeInnerCornerX = faceData[index++];
        leftEyeInnerCornerY = faceData[index++];
        leftEyeOuterCornerX = faceData[index++];
        leftEyeOuterCornerY = faceData[index++];
        rightEyeInnerCornerX = faceData[index++];
        rightEyeInnerCornerY = faceData[index++];
        rightEyeOuterCornerX = faceData[index++];
        rightEyeOuterCornerY = faceData[index++];
        mouthLeftX = faceData[index++];
        mouthLeftY = faceData[index++];
        mouthRightX = faceData[index++];
        mouthRightY = faceData[index++];
        noseLeftX = faceData[index++];
        noseLeftY = faceData[index++];
        noseRightX = faceData[index++];
        noseRightY = faceData[index++];
        mouthUpX = faceData[index++];
        mouthUpY = faceData[index++];
    }

    public void setLandmarkInfos(int lx, int ly, int rx, int ry, int licx, int licy, int locx,
            int locy, int ricx, int ricy, int rocx, int rocy, int mlx, int mly, int mrx, int mry,
            int nlx, int nly, int nrx, int nry, int mux, int muy) {
        leftEyeX = lx;
        leftEyeY = ly;
        rightEyeX = rx;
        rightEyeY = ry;
        leftEyeInnerCornerX = licx;
        leftEyeInnerCornerY = licy;
        leftEyeOuterCornerX = locx;
        leftEyeOuterCornerY = locy;
        rightEyeInnerCornerX = ricx;
        rightEyeInnerCornerY = ricy;
        rightEyeOuterCornerX = rocx;
        rightEyeOuterCornerY = rocy;
        mouthLeftX = mlx;
        mouthLeftY = mly;
        mouthRightX = mrx;
        mouthRightY = mry;
        noseLeftX = nlx;
        noseLeftY = nly;
        noseRightX = nrx;
        noseRightY = nry;
        mouthUpX = mux;
        mouthUpY = muy;
    }

    public String toString() {
        return new String("LE:(" + leftEyeX + "," + leftEyeY + "), " + "RE:(" + rightEyeX + ","
                + rightEyeY + "), " + "LEIC(" + leftEyeInnerCornerX + "," + leftEyeInnerCornerY
                + "), " + "LEOC(" + leftEyeOuterCornerX + "," + leftEyeOuterCornerY + "), "
                + "REIC(" + rightEyeInnerCornerX + "," + rightEyeInnerCornerY + "), " + "REOC("
                + rightEyeOuterCornerX + "," + rightEyeOuterCornerY + "), " + "ML(" + mouthLeftX
                + "," + mouthLeftY + "), " + "MR(" + mouthRightX + "," + mouthRightY + "), "
                + "NL(" + noseLeftX + "," + noseLeftY + "), " + "NR(" + noseRightX + ","
                + noseRightY + "), " + "MU(" + mouthUpX + "," + mouthUpY + ")");
    }
}
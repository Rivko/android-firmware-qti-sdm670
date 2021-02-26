/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.face;

/**
 * Class for faceInfo structure
 */
public class FaceInfo {
    public int id; /* ID Number */
    public int leftTopX; /* Left-Top Face X Coordinates */
    public int leftTopY; /* Left-Top Face Y Coordinates */
    public int rightTopX; /* Right-Top Face X Coordinates */
    public int rightTopY; /* Right-Top Face Y Coordinates */
    public int leftBottomX; /* Left-Bottom Face X Coordinates */
    public int leftBottomY; /* Left-Bottom Face Y Coordinates */
    public int rightBottomX; /* Right-Bottom Face X Coordinates */
    public int rightBottomY; /* Right-Bottom Face Y Coordinates */
    public FaceLandmark faceLandmark;

    /**
     * Constructor for the class
     */
    public FaceInfo() {
        setFaceInfos(0, 0, 0, 0, 0, 0, 0, 0, 0, new FaceLandmark());
    }

    /**
     * Copy Constructor for the class
     */
    public void setFaceInfos(FaceInfo instance) {
        id = instance.id;
        leftTopX = instance.leftTopX;
        leftTopY = instance.leftTopY;
        rightTopX = instance.rightTopX;
        rightTopY = instance.rightTopY;
        leftBottomX = instance.leftBottomX;
        leftBottomY = instance.leftBottomY;
        rightBottomX = instance.rightBottomX;
        rightBottomY = instance.rightBottomY;
        faceLandmark.setLandmarkInfos(instance.faceLandmark);
    }

    /**
     * Constructs the faceinfo from the int array values
     * 
     * @param faceData
     *            face data as int array
     */
    public FaceInfo(int[] faceData) {
        int index = 0;
        id = faceData[index++];
        leftTopX = faceData[index++];
        leftTopY = faceData[index++];
        rightTopX = faceData[index++];
        rightTopY = faceData[index++];
        leftBottomX = faceData[index++];
        leftBottomY = faceData[index++];
        rightBottomX = faceData[index++];
        rightBottomY = faceData[index++];

        faceLandmark = new FaceLandmark(faceData, index);
    }

    /**
     * Set the face information
     * 
     * @param id
     *            face id
     * @param ltX
     *            left top x value
     * @param ltY
     *            left top y value
     * @param rtX
     *            right top x value
     * @param rtY
     *            right top y value
     * @param lbX
     *            left bottom x value
     * @param lbY
     *            left bottom y value
     * @param rbX
     *            right bottom x value
     * @param rbY
     *            right bottom y value
     * @param landmark
     *            landmark values
     */
    public void setFaceInfos(int id, int ltX, int ltY, int rtX, int rtY, int lbX, int lbY, int rbX,
            int rbY, FaceLandmark landmark) {
        this.id = id;
        leftTopX = ltX;
        leftTopY = ltY;
        rightTopX = rtX;
        rightTopY = rtY;
        leftBottomX = lbX;
        leftBottomY = lbY;
        rightBottomX = rbX;
        rightBottomY = rbY;
        faceLandmark = landmark;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public String toString() {
        return new String("ID: " + id + ", LT:(" + leftTopX + "," + leftTopY + "), RT:("
                + rightTopX + "," + rightTopY + "), LB:(" + leftBottomX + "," + leftBottomY
                + "), RB:(" + rightBottomX + "," + rightBottomY + ") " + faceLandmark.toString());
    }

    /**
     * Converting face Info to int array values
     * 
     * @return int array values
     */
    public int[] toArrayValues() {
        int[] faceData = new int[31];
        int index = 0;
        faceData[index++] = id;
        faceData[index++] = leftTopX;
        faceData[index++] = leftTopY;
        faceData[index++] = rightTopX;
        faceData[index++] = rightTopY;
        faceData[index++] = leftBottomX;
        faceData[index++] = leftBottomY;
        faceData[index++] = rightBottomX;
        faceData[index++] = rightBottomY;

        faceData[index++] = faceLandmark.leftEyeX;
        faceData[index++] = faceLandmark.leftEyeY;
        faceData[index++] = faceLandmark.rightEyeX;
        faceData[index++] = faceLandmark.rightEyeY;
        faceData[index++] = faceLandmark.leftEyeInnerCornerX;
        faceData[index++] = faceLandmark.leftEyeInnerCornerY;
        faceData[index++] = faceLandmark.leftEyeOuterCornerX;
        faceData[index++] = faceLandmark.leftEyeOuterCornerY;
        faceData[index++] = faceLandmark.rightEyeInnerCornerX;
        faceData[index++] = faceLandmark.rightEyeInnerCornerY;
        faceData[index++] = faceLandmark.rightEyeOuterCornerX;
        faceData[index++] = faceLandmark.rightEyeOuterCornerY;
        faceData[index++] = faceLandmark.mouthLeftX;
        faceData[index++] = faceLandmark.mouthLeftY;
        faceData[index++] = faceLandmark.mouthRightX;
        faceData[index++] = faceLandmark.mouthRightY;
        faceData[index++] = faceLandmark.noseLeftX;
        faceData[index++] = faceLandmark.noseLeftY;
        faceData[index++] = faceLandmark.noseRightX;
        faceData[index++] = faceLandmark.noseRightY;
        faceData[index++] = faceLandmark.mouthUpX;
        faceData[index++] = faceLandmark.mouthUpY;

        return faceData;
    }
}

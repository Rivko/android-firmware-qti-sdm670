/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.facerecognition;

public class FaceInfo
{
    public int id;                  /* ID Number */
    public int leftTopX;            /* Left-Top     Face X Coordinates  */
    public int leftTopY;            /* Left-Top     Face Y Coordinates  */
    public int rightTopX;           /* Right-Top    Face X Coordinates  */
    public int rightTopY;           /* Right-Top    Face Y Coordinates  */
    public int leftBottomX;         /* Left-Bottom  Face X Coordinates  */
    public int leftBottomY;         /* Left-Bottom  Face Y Coordinates  */
    public int rightBottomX;        /* Right-Bottom Face X Coordinates  */
    public int rightBottomY;        /* Right-Bottom Face Y Coordinates  */
    public FaceLandmark faceLandmark;

    public FaceInfo()
    {
        setFaceInfos(0,0,0,0,0,0,0,0,0,new FaceLandmark());
    }

    public void setFaceInfos(FaceInfo instance)
    {
        id            = instance.id;
        leftTopX      = instance.leftTopX;
        leftTopY      = instance.leftTopY;
        rightTopX     = instance.rightTopX;
        rightTopY     = instance.rightTopY;
        leftBottomX   = instance.leftBottomX;
        leftBottomY   = instance.leftBottomY;
        rightBottomX  = instance.rightBottomX;
        rightBottomY  = instance.rightBottomY;
        faceLandmark.setLandmarkInfos(instance.faceLandmark);
    }

    public void setFaceInfos(int id,
                             int ltX, int ltY,
                             int rtX, int rtY,
                             int lbX, int lbY,
                             int rbX, int rbY,
                             FaceLandmark landmark)
    {
        this.id       = id;
        leftTopX      = ltX;
        leftTopY      = ltY;
        rightTopX     = rtX;
        rightTopY     = rtY;
        leftBottomX   = lbX;
        leftBottomY   = lbY;
        rightBottomX  = rbX;
        rightBottomY  = rbY;
        faceLandmark  = landmark;
    }

    public String toString()
    {
        return new String("ID: " + id              +
                          "LT:(" + leftTopX        +
                          "," + leftTopY           +
                          "), RT:(" + rightTopX    +
                          "," + rightTopY          +
                          "), LB:(" + leftBottomX  +
                          "," + leftBottomY        +
                          "), RB:(" + rightBottomX +
                          "," + rightBottomY       +
                          ") " + faceLandmark.toString()
                          );
    }
}

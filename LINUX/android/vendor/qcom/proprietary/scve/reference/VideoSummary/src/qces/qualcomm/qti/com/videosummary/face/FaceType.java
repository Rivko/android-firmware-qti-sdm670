/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.face;

/**
 * Enum for Face Type
 */
public enum FaceType {
    NONE(0), GENERIC(1);

    public final int id;

    private FaceType(int id) {
        this.id = id;
    }

    /**
     * get the FaceType enum based on value
     * 
     * @param id
     *            FaceType value
     * @return FaceType
     */
    public static FaceType getById(int id) {
        for (FaceType faceType : values()) {
            if (faceType.id == id) {
                return faceType;
            }
        }
        return NONE;
    }
}

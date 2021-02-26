/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qces.imageedit.objectclone;

import android.graphics.Matrix;

/**
 * Created by mithun on 6/1/15.
 */
public class ObjectTransformation extends Matrix{
    /**
     * horizontalFlip & verticalFlip flags will be set to true if the object is flipped horizontally
     * and/or vertically respectively.
     */
    private boolean verticalFlipState;
    private boolean horizontalFlipState;

    public ObjectTransformation(){
        horizontalFlipState = false;
        verticalFlipState = false;
    }

    public ObjectTransformation(ObjectTransformation m){
        super(m);
        this.horizontalFlipState = m.horizontalFlipState;
        this.verticalFlipState = m.verticalFlipState;
    }

    public boolean getHorizontalFlipState(){
        return horizontalFlipState;
    }

    public void set(ObjectTransformation m){
        super.set(m);
        this.horizontalFlipState = m.horizontalFlipState;
        this.verticalFlipState = m.verticalFlipState;
    }

    public void setHorizontalFlipState(boolean value){
        horizontalFlipState = value;
    }

    public boolean getVerticalFlipState(){
        return verticalFlipState;
    }

    public void setVerticalFlipState(boolean value){
        verticalFlipState = value;
    }
}

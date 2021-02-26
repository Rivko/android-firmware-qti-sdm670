/* ====================================================================
 * Copyright (c) 2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =====================================================================
 * @file ColorManager.java
 *
 */

package com.qti.snapdragon.sdk.display;

import java.util.EnumSet;

/**
 * This class provides a storage for Picture Adjustment Config Attributes.
 */
public class PictureAdjustmentConfig{

    public enum PICTURE_ADJUSTMENT_PARAMS {
        /**Denotes hue set */
        HUE(0),
        /**Denotes saturation set */
        SATURATION(1),
        /**Denotes intensity value set */
        INTENSITY(2),
        /**Denotes contrast set */
        CONTRAST(3),
        /**Denotes saturation threshold set */
        SATURATION_THRESHOLD(4);
         private int value;


        private PICTURE_ADJUSTMENT_PARAMS(int value) {
            this.value = value;
        }


        protected int getValue() {
            return value;
        }
    };

    private EnumSet<PICTURE_ADJUSTMENT_PARAMS> paValues;
    private int hue;
    private int saturation;
    private int intensity;
    private int contrast;
    private int sat_threshold;
    protected boolean isDesaturation = false;
    protected boolean isGlobalPADisabled = false;

    /**
     * This constructor will create the instance with the required parameters
     * @param paFlags EnumSet of PICTURE_ADJUSTMENT_PARAMS indicating which of
     *  the parameters need to be set
     * @param hue hue value.
     * @param saturation saturation value
     * @param intensity the intensity value
     * @param contrast contrast value
     * @param sat_thresh global saturation threshold below which no adjustment happens
     */
    public PictureAdjustmentConfig(EnumSet<PICTURE_ADJUSTMENT_PARAMS> paramFlags,
            int hue, int saturation, int intensity, int contrast, int sat_thresh){
        paValues = paramFlags;
        this.hue = hue;
        this.saturation = saturation;
        this.intensity = intensity;
        this.contrast = contrast;
        this.sat_threshold = sat_thresh;
    }

    /**
     * This API will return the enumset of all parameters that are supposed to be set
     * @return EnumSet of parameters
     */
    public EnumSet<PICTURE_ADJUSTMENT_PARAMS> getParamFlags(){
        return paValues;
    }
    /**
     * This API will return the hue value in this instance.
     * @return hue value
     */
    public int getHue(){
        return hue;
    }

    /**
     * This API will return the saturation value in this instance.
     * @return saturation value
     */
    public int getSaturation(){
        return saturation;
    }

    /**
     * This API will return the intensity value in this instance.
     * @return intensity value
     */
    public int getIntensity(){
        return intensity;
    }

    /**
     * This API will return the contrast value in this instance.
     * @return contrast value
     */
    public int getContrast(){
        return contrast;
    }

    /**
     * This API will return the saturation threshold value in this instance.
     * @return saturation threshold value
     */
    public int getSaturationThreshold(){
        return sat_threshold;
    }

    /**
     * This API will return if desaturation is enabled
     * @return true if desaturation is enabled, false otherwise
     */
    public boolean isDesaturationEnabled(){
        return isDesaturation;
    }

    /**
     * This API will indicate if global Picture Adjustment is disabled or not.
     * @return true if Global Picture Adjustment is disabled, false otherwise
     */
    public boolean isGlobalPictureAdjustmentDisabled(){
        return isGlobalPADisabled;
    }
}

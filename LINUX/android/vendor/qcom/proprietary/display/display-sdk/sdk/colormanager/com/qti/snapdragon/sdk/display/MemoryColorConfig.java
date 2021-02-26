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
 * This class provides a storage for Memory Color Attributes.
 */
public class MemoryColorConfig{
    public enum MEMORY_COLOR_PARAMS {
        /**Denotes hue set */
        HUE(0),
        /**Denotes saturation set */
        SATURATION(1),
        /**Denotes intensity value set */
        INTENSITY(2);

        private int value;


        private MEMORY_COLOR_PARAMS(int value) {
            this.value = value;
        }


        protected int getValue() {
            return value;
        }
    };

    public enum MEMORY_COLOR_TYPE {
        /**Denotes memory color skin. This will alter the skin color
         *  palette based on the values provided */
        SKIN(0),
        /**Denotes memory color sky. This will alter the sky color palette
         *  based on the values provided */
        SKY(1),
        /**Denotes memory color foliage. This will alter the foliage (leaves)
         *  color palette based on the values provided */
        FOLIAGE(2);

        private int value;


        private MEMORY_COLOR_TYPE(int value) {
            this.value = value;
        }


        protected int getValue() {
            return value;
        }
    };

    private MEMORY_COLOR_TYPE memColorType;
    private int hue;
    private int saturation;
    private int intensity;
    protected boolean isEnabled = true;


    /**
     * This constructor will create the instance with the required parameters
     * @param type Enum value of MEMORY_COLOR_TYPE indicating which memory color
     *  type this object is associated with
     * @param hue hue value.
     * @param saturation saturation value
     * @param intensity the intensity value
     */
    public MemoryColorConfig(MEMORY_COLOR_TYPE type, int hue, int saturation,
            int intensity){
        memColorType = type;
        this.hue = hue;
        this.saturation = saturation;
        this.intensity = intensity;
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
     * This API will return the memory color type this object is associated with.
     * @return MEMORY_COLOR_TYPE enum value denoting the type
     */
    public MEMORY_COLOR_TYPE getMemoryColorType(){
        return memColorType;
    }
    /**
     * This API will indicate if memory color of the type this object is associated
     *  with enabled or not.
     * @return true if Memory color for the associated type is enabled, false otherwise
     */
    public boolean isMemoryColorEnabled(){
        return isEnabled;
    }
}

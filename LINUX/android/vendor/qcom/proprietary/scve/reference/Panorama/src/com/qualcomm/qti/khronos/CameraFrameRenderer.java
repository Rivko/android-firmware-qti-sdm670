/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.khronos;

import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.Locale;

import android.opengl.GLES30;
import android.opengl.Matrix;

/**
 * This class performs simple rendering of a camera frame to an output surface. It renders one quad,
 * and scales it contents according to {@link ScaleType#CENTER_INSIDE}
 */
public class CameraFrameRenderer {

    /**
     * The types of input textures that this class can accept
     */
    public enum TextureType {
        /**
         * Corresponds to the {@link GLES30#GL_TEXTURE_2D} texture target
         */
        TEXTURE_2D("sampler2D"),

        /**
         * Corresponds to the {@link GLOES2EGLImage#GL_TEXTURE_EXTERNAL_OES} texture target
         */
        TEXTURE_EXTERNAL("samplerExternalOES");

        public final String sampler;
        private TextureType(String sampler) {
            this.sampler = sampler;
        }
    };

    private static final String VERTEX_SHADER =
            "#version 300 es\n" +
            "uniform mat4 uProjection;" +
            "uniform mat4 uTexTransform;" +
            "in vec4 inPosition;" +
            "in vec2 inTexCoord;" +
            "out vec2 texCoord;" +
            "void main() {" +
            "  texCoord = (uTexTransform * vec4(inTexCoord, 0, 1)).xy;" +
            "  gl_Position = (uProjection * inPosition);" +
            "}";

    private static final String FRAGMENT_SHADER_YCBCR_BASE =
            "#version 300 es\n" +
            "#ifdef GL_OES_EGL_image_external_essl3\n" +
            "#extension GL_OES_EGL_image_external_essl3 : require\n" +
            "#else\n" +
            "#extension GL_OES_EGL_image_external : require\n" +
            "#endif\n" +
            "precision mediump float;" +
            "const mat3 RGB2YUV = mat3(0.256788, -0.148223, 0.439216," +
            "                          0.504129, -0.290993, -0.367788," +
            "                          0.097906, 0.439216, -0.071427);" +
            "const vec3 BT601_YUV_MIN = vec3(0.062745);" +
            "const vec3 BT601_YUV_MAX = vec3(0.921569, 0.941176, 0.941176);" +
            "const vec3 YCBCR_OFFSET = vec3(0.062745, 0.501961, 0.501961);" +
            "uniform %s uTexUnit;" +
            "in vec2 texCoord;" +
            "out vec2 fragColor[2];" +
            "void main() {" +
            "  vec3 yuv = clamp(RGB2YUV * texture(uTexUnit, texCoord).rgb + YCBCR_OFFSET, " +
            "      BT601_YUV_MIN, BT601_YUV_MAX);" +
            "  fragColor[0] = vec2(yuv.%s, 0);" +
            "  fragColor[1] = yuv.%s;" +
            "}";
    private static final String FRAGMENT_SHADER_YCBCR = String.format(Locale.US,
            FRAGMENT_SHADER_YCBCR_BASE, "%s", "x", "yz");
    private static final String FRAGMENT_SHADER_YCRCB = String.format(Locale.US,
            FRAGMENT_SHADER_YCBCR_BASE, "%s", "x", "zy");

    private static final String FRAGMENT_SHADER_RGBA_BASE =
            "#version 300 es\n" +
            "#ifdef GL_OES_EGL_image_external_essl3\n" +
            "#extension GL_OES_EGL_image_external_essl3 : require\n" +
            "#else\n" +
            "#extension GL_OES_EGL_image_external : require\n" +
            "#endif\n" +
            "precision mediump float;" +
            "uniform %s uTexUnit;" +
            "in vec2 texCoord;" +
            "out vec4 fragColor;" +
            "void main() {" +
            "  fragColor = texture(uTexUnit, texCoord).%s;" +
            "}";
    private static final String FRAGMENT_SHADER_RGBA = String.format(Locale.US,
            FRAGMENT_SHADER_RGBA_BASE, "%s", "xyzw");
    private static final String FRAGMENT_SHADER_BGRA = String.format(Locale.US,
            FRAGMENT_SHADER_RGBA_BASE, "%s", "zyxw");

    // Vertex order to draw a quad using a triangle strip, where the quad
    // vertices are numbered starting in the lower-left corner and proceed CCW
    private static final short[] QUAD_TRI_STRIP = new short[] { 0, 1, 3, 2 };

    // Texture coordinate of each quad vertex, where the quad vertices are
    // numbered starting in the lower-left corner and proceed CCW
    private static final float[] QUAD_TEX_COORDS = new float[] {
        0f, 0f, 1f, 0f, 1f, 1f, 0f, 1f };

    // Vertex coordinate of each quad vertex, where the quad vertices are
    // numbered starting in the lower-left corner and proceed CCW
    private static final float[] QUAD_VERTEX_COORDS = new float[] {
        -1f, -1f, 1f, -1f, 1f, 1f, -1f, 1f };

    private TextureType mTexType;

    // Global names
    private int mBElements;
    private int mBVertexCoords;
    private int mBTexCoords;

    // Program-relative names
    private int mProgramName;
    private int mSPVertexShader;
    private int mSPFragmentShader;
    private int mVSUProjection;
    private int mVSUTexTransform;
    private int mVSInPosition;
    private int mVSInTexCoord;
    private int mFSUTexUnit;

    /**
     * Call once, with the desired GLES30 context bound, to initialize the {@link
     * CameraFrameRenderer}
     * @param texType {@link TextureType} of the input textures that will be rendered via this
     *      {@link CameraFrameRenderer}
     * @param colourFmt Colour format of pixels to write into the bound framebuffer
     */
    public void init(TextureType texType, ColourFormat colourFmt) {
        mTexType = texType;

        int[] handles = new int[1];
        ShortBuffer sb;
        FloatBuffer fb;

        // Set the element order of the proceeding array buffers that define the quad
        GLES30.glGenBuffers(1, handles, 0);
        mBElements = handles[0];
        GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, mBElements);
        sb = ShortBuffer.wrap(QUAD_TRI_STRIP);
        GLES30.glBufferData(GLES30.GL_ELEMENT_ARRAY_BUFFER, sb.capacity() * 2,
                sb, GLES30.GL_STATIC_DRAW);

        // Set the quad to fill the entire clip space
        GLES30.glGenBuffers(1, handles, 0);
        mBVertexCoords = handles[0];
        fb = FloatBuffer.wrap(QUAD_VERTEX_COORDS);
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, mBVertexCoords);
        GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, fb.capacity() * 4,
                fb, GLES30.GL_STREAM_DRAW);

        // Set the texture coords of the quad, with (0,0) in the lower-left and
        // (1,1) in the upper-right
        GLES30.glGenBuffers(1, handles, 0);
        mBTexCoords = handles[0];
        fb = FloatBuffer.wrap(QUAD_TEX_COORDS);
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, mBTexCoords);
        GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, fb.capacity() * 4,
                fb, GLES30.GL_STREAM_DRAW);

        // Compile the shader program, and load the uniform & attribute locations
        mSPVertexShader = GLES30Helper.compileShader(GLES30.GL_VERTEX_SHADER, VERTEX_SHADER);
        String frag;
        switch (colourFmt) {
        case ABGR_8888:
            frag = FRAGMENT_SHADER_RGBA;
            break;

        case ARGB_8888:
            frag = FRAGMENT_SHADER_BGRA;
            break;

        case NV12:
            frag = FRAGMENT_SHADER_YCBCR;
            break;

        case NV21:
            frag = FRAGMENT_SHADER_YCRCB;
            break;

        default:
            throw new IllegalArgumentException("Unexpected ColourFormat");
        }
        mSPFragmentShader = GLES30Helper.compileShader(GLES30.GL_FRAGMENT_SHADER,
                String.format(Locale.US, frag, texType.sampler));
        mProgramName = GLES30.glCreateProgram();
        GLES30.glAttachShader(mProgramName, mSPVertexShader);
        GLES30.glAttachShader(mProgramName, mSPFragmentShader);
        GLES30.glLinkProgram(mProgramName);
        mVSUProjection = GLES30.glGetUniformLocation(mProgramName, "uProjection");
        mVSUTexTransform = GLES30.glGetUniformLocation(mProgramName, "uTexTransform");
        mVSInPosition = GLES30.glGetAttribLocation(mProgramName, "inPosition");
        mVSInTexCoord = GLES30.glGetAttribLocation(mProgramName, "inTexCoord");
        mFSUTexUnit = GLES30.glGetUniformLocation(mProgramName, "uTexUnit");

        // Select default tex unit (which is constant for the life of the program)
        GLES30.glUseProgram(mProgramName);
        GLES30.glUniform1i(mFSUTexUnit, 0);
    }

    /**
     * Call whenever either the output (viewport) or input (frame) sizes change. This will calculate
     * an appropriate projection to accomplish {@link ScaleType#CENTER_INSIDE} scaling.
     * @param viewportWidth Width of the viewport, in px
     * @param viewportHeight Height of the viewport, in px
     * @param frameWidth Width of the frame, in px
     * @param frameHeight Height of the frame, in px
     */
    public void resized(int viewportWidth, int viewportHeight, int frameWidth, int frameHeight) {
        float[] proj = new float[16];

        if (viewportWidth == 0 || viewportHeight == 0 || frameWidth == 0 || frameHeight == 0) {
            Matrix.setIdentityM(proj, 0);
        } else {
            // Calculate the relative aspect ratio of frame:viewport
            float aspect = (float)(frameWidth * viewportHeight) /
                    (float)(viewportWidth * frameHeight);
            if (aspect >= 1f) {
                // Frame has wider aspect than viewport
                Matrix.orthoM(proj, 0, -1f, 1f, -aspect, aspect, -1f, 1f);
            } else {
                // Frame has wider aspect than viewport
                float aspectInv = 1f / aspect;
                Matrix.orthoM(proj, 0, -aspectInv, aspectInv, -1f, 1f, -1f, 1f);
            }
        }

        GLES30.glUseProgram(mProgramName);
        GLES30.glUniformMatrix4fv(mVSUProjection, 1, false, proj, 0);
    }

    /**
     * Render the provided input texture to the bound output surface
     * @param tex Input texture to render. It should be compatible with the {@link TextureType}
     *      texture target passed to the {@link #init(TextureType)} method.
     * @param transform A 4x4 affine transform matrix (in column-major order) to apply to the
     *      texture coordinates addressing tex
     */
    public void render(int tex, float[] transform) {
        if (tex == 0) {
            throw new IllegalArgumentException("tex not valid (0)");
        } else if (transform == null || transform.length != 16) {
            throw new IllegalArgumentException("transform must be a 4x4 columnwise matrix");
        }

        GLES30.glClearColor(0f, 0f, 0f, 1f);
        GLES30.glClearDepthf(1f);
        GLES30.glClearStencil(0);
        GLES30.glClear(GLES30.GL_COLOR_BUFFER_BIT | GLES30.GL_DEPTH_BUFFER_BIT |
                GLES30.GL_STENCIL_BUFFER_BIT);

        resetGLContext();

        GLES30.glUseProgram(mProgramName);

        GLES30.glActiveTexture(GLES30.GL_TEXTURE0);
        GLES30.glBindTexture(mTexType == TextureType.TEXTURE_EXTERNAL ?
                GLOESEGLImage.GL_TEXTURE_EXTERNAL_OES : GLES30.GL_TEXTURE_2D, tex);

        GLES30.glUniformMatrix4fv(mVSUTexTransform, 1, false, transform, 0);

        GLES30.glEnableVertexAttribArray(mVSInPosition);
        GLES30.glEnableVertexAttribArray(mVSInTexCoord);

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, mBVertexCoords);
        GLES30.glVertexAttribPointer(mVSInPosition, 2, GLES30.GL_FLOAT, false,
                0, 0);

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, mBTexCoords);
        GLES30.glVertexAttribPointer(mVSInTexCoord, 2, GLES30.GL_FLOAT, false,
                0, 0);

        GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, mBElements);
        GLES30.glDrawElements(GLES30.GL_TRIANGLE_STRIP, QUAD_TRI_STRIP.length,
                GLES30.GL_UNSIGNED_SHORT, 0);

        GLES30.glDisableVertexAttribArray(mVSInPosition);
        GLES30.glDisableVertexAttribArray(mVSInTexCoord);
    }

    /**
     * Helper method to put the GLES30 context in a known state
     */
    private void resetGLContext() {
        GLES30.glDisable(GLES30.GL_BLEND);
        GLES30.glDisable(GLES30.GL_DEPTH_TEST);
        GLES30.glDisable(GLES30.GL_STENCIL_TEST);

        GLES30.glCullFace(GLES30.GL_BACK);
        GLES30.glEnable(GLES30.GL_CULL_FACE);
    }
}

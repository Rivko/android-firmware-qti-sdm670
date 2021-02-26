/*
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.objecttrackerdemo.renderer.primitives;

import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.Arrays;

import com.qualcomm.qti.khronos.GLES30Helper;
import com.qualcomm.qti.khronos.GLOES2EGLImage;

import android.opengl.GLES30;

/**
 * Abstract class which handles rendering a Quad
 */
abstract class BaseQuad {
    /**
     * This enumeration defines the shapes which can be used to mask the current Quad
     */
    public enum Mask {
        /** No masking shape will be applied to the current Quad */
        NONE,

        /** An oval, sized to fill the entire Quad, will be used to mask the Quad */
        OVAL
    }

    // 2D Orthographic projection vertex shader. Subclasses of Quad must implement shade_vertex()
    private static final String VERTEX_SHADER_SKELETON =
            "uniform mat4 uProjectionMatrix;" +
            "attribute vec4 aPosition;" +
            "attribute vec2 aTextureCoord;" +
            "varying vec2 vTextureCoord;" +
            "void shade_vertex();" +
            "void main() {" +
            "  gl_Position = (uProjectionMatrix * aPosition);" +
            "  vTextureCoord = aTextureCoord;" +
            "  shade_vertex();" +
            "}";

    // Skeleton fragment shader. Subclasses of Quad must implement shade_fragment()
    private static final String FRAGMENT_SHADER_SKELETON =
            "precision mediump float;" +
            "varying vec2 vTextureCoord;" +
            "vec4 shade_fragment();" +
            "void mask();" +
            "void main() {" +
            "  mask();" +
            "  gl_FragColor = shade_fragment();" +
            "}";

    // Fragment shader mask function for Mask.NONE
    private static final String FRAGMENT_SHADER_MASK_NONE =
            "void mask() {" +
            "}";

    // Fragment shader mask function for Mask.OVAL
    private static final String FRAGMENT_SHADER_MASK_OVAL =
            "const vec2 CENTER = vec2(0.5, 0.5);" +
            "void mask() {" +
            "  if (distance(vTextureCoord, CENTER) > 0.5) discard;" +
            "}";

    // Vertex order to draw a quad using a triangle strip, where the quad vertices are numbered
    // starting in the lower-left corner and proceed CCW. This will draw a double-sided quad, so
    // that one side will always be facing forward. This is useful in the case where the input
    // is mirrored horizontally or vertically.
    private static final short[] QUAD_TRI_STRIP = new short[] { 0, 1, 3, 2, 0, 1 };

    // Texture coordinate of each quad vertex, where the quad vertices are numbered starting in the
    // lower-left corner and proceed CCW
    private static final float[] QUAD_TEXTURE_COORDS = new float[] {
        0f, 0f, 1f, 0f, 1f, 1f, 0f, 1f };

    // An identity matrix to use as the default projection
    private static final float[] IDENTITY_PROJECTION = new float[] {
        1f, 0f, 0f, 0f, 0f, 1f, 0f, 0f, 0f, 0f, 1f, 0f, 0f, 0f, 0f, 1f };

    // GLSL handles
    private int programName;
    private int fragmentShader;
    private int vertexShader;
    private int vsuProjectionMatrix;
    private int vsaPosition;
    private int vsaTextureCoord;

    // Element & vertex buffers
    private int elements;
    private int vertices;
    private int textureCoords;

    private float posX;
    private float posY;
    private float width;
    private float height;
    private boolean posDirty;

    private final float[] projMatrix = new float[16];
    private boolean projMatrixDirty;

    private Mask maskType = Mask.NONE;
    private int nextAvailableTextureUnit;

    /**
     * Get the GLSL extension directives for the vertex shader. The GLSL spec requires that these
     * occur first in the GLSL source. Subclasses should override this method and add any GLSL
     * extension directives that they require.
     * @return the extension directives (separated by newlines) for the GLSL vertex shader
     */
    protected String getVertexShaderExtensionDirectives() {
        return "";
    }

    /**
     * Gets the source for the vertex shader to be applied to this Quad. Subclasses should override
     * this method and add additional GLSL to complete the vertex shader program. The vertex shader
     * source will be concatenated with the results of
     * {@link #getVertexShaderExtensionDirectives()}.
     * @return a complete GLSL vertex shader program
     */
    protected String getVertexShader() {
        return VERTEX_SHADER_SKELETON;
    }

    /**
     * Get the GLSL extension directives for the fragment shader. The GLSL spec requires that these
     * occur first in the GLSL source. Subclasses should override this method and add any GLSL
     * extension directives that they require.
     * @return the extension directives (separated by newlines) for the GLSL fragment shader
     */
    protected String getFragmentShaderExtensionDirectives() {
        return "";
    }

    /**
     * Get the source for the fragment shader to be applied to this Quad. Subclasses should
     * override this method and add additional GLSL to complete the fragment shader program. The
     * fragment shader source will be concatenated with the results of
     * {@link #getFragmentShaderExtensionDirectives()}.
     * @return a complete GLSL fragment shader program
     */
    protected String getFragmentShader() {
        return FRAGMENT_SHADER_SKELETON;
    }

    private String getFragmentShaderMask() {
        return (maskType == Mask.OVAL ? FRAGMENT_SHADER_MASK_OVAL : FRAGMENT_SHADER_MASK_NONE);
    }

    /**
     * Subclasses can override this method to initialize internal state, create OpenGL resources,
     * apply uniforms to their fragment shaders, etc. This is called at the end of {@link #init()}
     * @param programName Handle to the GLSL program (which will already be bound and ready to use)
     */
    protected abstract void onInit(int programName);

    /**
     * Subclasses can override this method to cleanup any state they created in {@link
     * #onInit(int)}. This will be the last method invoked on subclasses.
     */
    protected abstract void onCleanup();

    /**
     * Subclasses can override this method to set up any special rendering state required before the
     * Quad is rendered. For eg, create and bind any textures, etc, here. The shader programs for
     * this Quad will be bound and ready to use.
     */
    protected abstract void onRender();

    /**
     * Gets the X position of the lower-left corner of this Quad
     * @return X position of the lower-left corner in units relative to the local coordinate space
     */
    protected final float getX() {
        return posX;
    }

    /**
     * Gets the Y position of the lower-left corner of this Quad
     * @return Y position of the lower-left corner in units relative to the local coordinate space
     */
    protected final float getY() {
        return posY;
    }

    /**
     * Gets the width of this Quad, in units relative to the local coordinate space
     * @return width, in units relative to the local coordinate space
     */
    protected final float getWidth() {
        return width;
    }

    /**
     * Gets the height of this Quad, in units relative to the local coordinate space
     * @return height, in units relative to the local coordinate space
     */
    protected final float getHeight() {
        return height;
    }

    /**
     * Gets whether or not this Quad is opaque. Subclasses can override this method to indicate
     * whether alpha blending is required
     * @return true if Quad is opaque, else false
     */
    protected boolean isOpaque() {
        return true;
    }

    /**
     * Helper to allocate the next available texture unit
     * @param glTextureTarget GL texture target for the texture to be sampled using this texture
     *      unit. Some types of textures may require allocation of multiple texture units.
     * @return Allocated texture unit ID
     */
    protected int allocateTextureUnit(int glTextureTarget) {
        int textureUnit = nextAvailableTextureUnit;
        nextAvailableTextureUnit += (glTextureTarget == GLOES2EGLImage.GL_TEXTURE_EXTERNAL_OES ?
                3 : 1);
        return textureUnit;
    }

    /**
     * Set the {@link Mask} shape to be used when rendering this Quad. This method must be invoked
     * before {@link #init()} to have an effect on rendering.
     * @param maskType a {@link Mask} value
     * @throws IllegalArgumentException if maskType is null
     */
    public final void setMaskType(Mask maskType) {
        if (maskType == null) {
            throw new IllegalArgumentException("maskType may not be null");
        }

        this.maskType = maskType;
    }

    /**
     * Set the coordinate space used for this Quad
     * @param projMatrix A projection matrix that establishes the coordinate space for this Quad, or
     *      null to set an identity projection
     * @throws IllegalArgumentException if projMatrix is not of length 16
     */
    public final void setCoordinateSpace(float[] projMatrix) {
        if (projMatrix != null && projMatrix.length != 16) {
            throw new IllegalArgumentException("GL projection matrix must be 4x4");
        }

        if (projMatrix == null) {
            projMatrix = IDENTITY_PROJECTION;
        }

        if (!Arrays.equals(projMatrix, this.projMatrix)) {
            System.arraycopy(projMatrix, 0, this.projMatrix, 0, 16);
            projMatrixDirty = true;
        }
    }

    /**
     * Sets the position of this Quad within its coordinate space
     * @param x X coord of lower-left corner
     * @param y Y coord of lower-left corner
     * @param width Width of quad
     * @param height Height of quad
     */
    public final void setPosition(float x, float y, float width, float height) {
        if (x != posX || y != posY || width != this.width || height != this.height) {
            posX = x;
            posY = y;
            this.width = width;
            this.height = height;
            posDirty = true;
        }
    }

    /**
     * Initializes the {@link BaseQuad}. Must be called before any other methods of this class
     * (except for {@link #setMaskType(Mask)}. Will leave the GLES program active, so subclasses
     * which override this method can rely on this to set uniforms in their fragment shaders.
     */
    public final void init() {
        int[] handles = new int[1];

        vertexShader = GLES30Helper.compileShader(GLES30.GL_VERTEX_SHADER,
                getVertexShaderExtensionDirectives() + "\n" + getVertexShader());
        fragmentShader = GLES30Helper.compileShader(GLES30.GL_FRAGMENT_SHADER,
                getFragmentShaderExtensionDirectives() + "\n" + getFragmentShader() + "\n" +
                getFragmentShaderMask());
        programName = GLES30.glCreateProgram();
        GLES30.glAttachShader(programName, vertexShader);
        GLES30.glAttachShader(programName, fragmentShader);
        GLES30.glLinkProgram(programName);

        // VS uniforms and attributes
        GLES30.glUseProgram(programName);
        vsuProjectionMatrix = GLES30.glGetUniformLocation(programName, "uProjectionMatrix");
        vsaPosition = GLES30.glGetAttribLocation(programName, "aPosition");
        vsaTextureCoord = GLES30.glGetAttribLocation(programName, "aTextureCoord");

        // Element array buffer - define how the primitive vertex array is interpreted by glDraw*
        GLES30.glGenBuffers(1, handles, 0);
        elements = handles[0];
        GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, elements);
        ShortBuffer sb = ShortBuffer.wrap(QUAD_TRI_STRIP);
        GLES30.glBufferData(GLES30.GL_ELEMENT_ARRAY_BUFFER, sb.capacity() * 2, sb,
                GLES30.GL_STATIC_DRAW);

        // Vertex array buffer - define where the primitive is drawn
        GLES30.glGenBuffers(1, handles, 0);
        vertices = handles[0];

        // Vertex texture coord buffer - define how the primitive is textured
        GLES30.glGenBuffers(1, handles, 0);
        textureCoords = handles[0];
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, textureCoords);
        FloatBuffer fb = FloatBuffer.wrap(QUAD_TEXTURE_COORDS);
        GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, fb.capacity() * 4, fb, GLES30.GL_STATIC_DRAW);

        setCoordinateSpace(null);

        onInit(programName);
    }

    /**
     * Called before the {@link BaseQuad} is destroyed.
     */
    public final void cleanup() {
        GLES30.glDeleteProgram(programName);
        GLES30.glDeleteShader(vertexShader);
        GLES30.glDeleteShader(fragmentShader);

        int[] handles = new int[] { elements, vertices, textureCoords };
        GLES30.glDeleteBuffers(handles.length, handles, 0);
    }

    /**
     * Called to request that the {@link BaseQuad} render to the current
     * GLES30 context.
     */
    public final void render() {
        GLES30.glUseProgram(programName);

        onRender();

        if (projMatrixDirty) {
            GLES30.glUniformMatrix4fv(vsuProjectionMatrix, 1, false,
                    projMatrix, 0);
            projMatrixDirty = false;
        }

        if (posDirty) {
            FloatBuffer fb = FloatBuffer.wrap(new float[] {
                    posX, posY,
                    posX + width, posY,
                    posX + width, posY + height,
                    posX, posY + height
            });
            GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vertices);
            GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, fb.capacity() * 4, fb,
                    GLES30.GL_STREAM_DRAW);
            posDirty = false;
        }

        if (!isOpaque()) {
            GLES30.glEnable(GLES30.GL_BLEND);
        }

        GLES30.glEnableVertexAttribArray(vsaPosition);
        GLES30.glEnableVertexAttribArray(vsaTextureCoord);
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vertices);
        GLES30.glVertexAttribPointer(vsaPosition, 2, GLES30.GL_FLOAT, false, 0, 0);
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, textureCoords);
        GLES30.glVertexAttribPointer(vsaTextureCoord, 2, GLES30.GL_FLOAT, false, 0, 0);
        GLES30.glBindBuffer(GLES30.GL_ELEMENT_ARRAY_BUFFER, elements);
        GLES30.glDrawElements(GLES30.GL_TRIANGLE_STRIP, QUAD_TRI_STRIP.length,
                GLES30.GL_UNSIGNED_SHORT, 0);
        GLES30.glDisableVertexAttribArray(vsaPosition);
        GLES30.glDisableVertexAttribArray(vsaTextureCoord);

        GLES30.glDisable(GLES30.GL_BLEND);
    }
}

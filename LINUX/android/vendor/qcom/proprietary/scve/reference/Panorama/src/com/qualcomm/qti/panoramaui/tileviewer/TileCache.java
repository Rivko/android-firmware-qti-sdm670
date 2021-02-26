/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.panoramaui.tileviewer;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

/**
 * The {@link TileCache} manages a pool of {@link Bitmap}s, each of which is a subset of a larger
 * image. The tiles are portions of the input image, downsampled by a power of 2 (ie 1, 2, etc).
 */
public class TileCache {
    /**
     * Classes may implement the {@link Listener} interface to be notified when the contents of the
     * {@link TileCache} have been updated (for example, due to a deferred tile load operation).
     */
    public static interface Listener {
        /**
         * This method will be invoked when a tile in the {@link TileCache} is updated
         */
        public void onTileCacheUpdated();
    }

    /**
     * The size of each tile in the {@link TileCache}. All operations on the cache (ie
     * {@link #getTile(int, int, int)}) operate on even multiples of this value.
     */
    public static final int TILE_SIZE_PX = 128;

    private static final String TAG = TileCache.class.getSimpleName();
    private static final int INACTIVE_TILE_CACHE_MIN_SIZE = 16;
    private static final int INVALID_GENERATION = -1;

    private final Listener listener;
    private final Handler loadTileCompletedHandler;
    private final PriorityBlockingQueue<Runnable> executorWorkQueue;
    private final ThreadPoolExecutor executor;
    private final BlockingQueue<JpegTileDecoder> decoders;
    private final Bitmap placeholder;
    @SuppressLint("UseSparseArrays")
    private final HashMap<Integer, Tile> activeTiles = new HashMap<Integer, Tile>();
    private final ArrayList<LoadTileRunnable> completedLoads = new ArrayList<LoadTileRunnable>();
    private final Canvas scaleCanvas = new Canvas();
    private final Paint scalePaint = new Paint(Paint.ANTI_ALIAS_FLAG | Paint.FILTER_BITMAP_FLAG);
    private int generation;
    private int inactiveTileCacheMaxSize;
    private Map<Integer, Tile> inactiveTiles;
    private List<Bitmap> availableTileBitmaps;

    /**
     * Construct a new {@link TileCache} for the provided {@link JpegTileDecoder}
     * @param decoder a {@link JpegTileDecoder} from which to decode tiles
     * @param listener a {@link Listener} on which to notify the owner of this cache when tile(s)
     *      in the cache are updated
     * @throws IllegalArgumentException if decoder or listener are null
     */
    public TileCache(JpegTileDecoder decoder, Listener listener) {
        if (decoder == null) {
            throw new IllegalArgumentException("decoder cannot be null");
        } else if (listener == null) {
            throw new IllegalArgumentException("listener cannot be null");
        }

        this.listener = listener;

        final int numDecoderThreads = Math.min(Runtime.getRuntime().availableProcessors(), 4);
        loadTileCompletedHandler = new Handler(Looper.getMainLooper(), loadTileCompletedCallback);
        executorWorkQueue = new PriorityBlockingQueue<Runnable>();
        executor = new ThreadPoolExecutor(numDecoderThreads, numDecoderThreads,
                10, TimeUnit.SECONDS, executorWorkQueue);
        executor.allowCoreThreadTimeOut(true);
        decoders = new ArrayBlockingQueue<JpegTileDecoder>(numDecoderThreads);
        decoders.add(decoder);
        for (int i = 1; i < numDecoderThreads; i++) {
            decoders.add(new JpegTileDecoder(decoder));
        }

        setTiledAreaDimensions(0, 0);

        placeholder = createOrReuseBitmap(); // Will create, nothing to reuse at this point
        placeholder.eraseColor(Color.TRANSPARENT);
    }

    /**
     * Mark the current contents of the tile cache. Any tiles not referenced via
     * {@link #getTile(int, int, int)} before {@link #sweep()} is invoked are eligible to be
     * discarded. This should be called before a related set of tile fetch operations are performed.
     */
    public void mark() {
        generation++;
    }

    /**
     * Sweeps unreferenced tiles from the tile cache. The tiles may not be immediately discarded,
     * but they become eligible for cleanup. This should be called after a related set of tile fetch
     * operations have been performed.
     */
    public void sweep() {
        Iterator<Map.Entry<Integer, Tile>> it = activeTiles.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry<Integer, Tile> entry = it.next();
            Tile tile = entry.getValue();
            if (tile.generation != generation) {
                // If the tile is a placeholder, then the LoadTileRunnable responsible for
                // populating it is likely still in the work queue (unless it is actively being
                // processed). Remove it, as it is now unnecessary.
                if (tile.isPlaceholder()) {
                    executorWorkQueue.remove(tile.loadTileRunnable);
                }

                if (isReusableBitmap(tile.bitmap)) {
                    inactiveTiles.put(entry.getKey(), tile);
                }

                it.remove();
            }
        }
    }

    /**
     * Request a tile {@link Bitmap} be loaded through the cache. If it is already present (or can
     * be synthesized from other data in the cache), it will be returned immediately. Otherwise, a
     * placeholder {@link Bitmap} will be returned and a deferred tile load initiated. At some point
     * in the future, when the real tile is available,
     * {@link Listener#onTileCacheUpdated(int, int, int)} will be invoked to inform the owner that
     * the actual contents of the tile are now available.
     * @param x the X position of the tile to fetch (which must always be aligned on a
         *      {@link #TILE_SIZE_PX} boundary)
         * @param y the Y position of the tile to fetch (which must always be aligned on a
         *      {@link #TILE_SIZE_PX} boundary)
         * @param downsampleLog2 log2 of the downsample factor (ie 0=1x, 1=2x, 2=4x, etc)
     * @return a {@link Bitmap} representing either the contents of the requested tile, or a
     *      placeholder
     * @throws IllegalArgumentException if x or y are not aligned to a {@link #TILE_SIZE_PX}
     *      boundary, or if downsampleLog2 is negative
     */
    public Bitmap getTile(int x, int y, int downsampleLog2) {
        if (x % TILE_SIZE_PX != 0 || y % TILE_SIZE_PX != 0) {
            throw new IllegalArgumentException("Tiles must be on TILE_SIZE_PX boundaries");
        } else if (downsampleLog2 < 0) {
            throw new IllegalArgumentException("downsampleLog2 must be non-negative");
        }

        final int key = Tile.key(x, y, downsampleLog2);

        // Check active cache for tile
        Tile t = activeTiles.get(key);

        // If not found in active cache, check the inactive cache
        if (t == null && (t = inactiveTiles.remove(key)) != null) {
            activeTiles.put(key, t);
        }

        // If not found in inactive cache, see if we can synthesize it from higher-resolution tiles
        if (t == null && downsampleLog2 > 0) {
            final Tile[] subtiles = new Tile[4];
            boolean subtileMissing = false;
            for (int y0 = 0; y0 < 2 && !subtileMissing; y0++) {
                for (int x0 = 0; x0 < 2 && !subtileMissing; x0++) {
                    final int subtileKey = Tile.key(x * 2 + x0 * TILE_SIZE_PX,
                            y * 2 + y0 * TILE_SIZE_PX, downsampleLog2 - 1);
                    final int idx = y0 * 2 + x0;
                    subtiles[idx] = activeTiles.get(subtileKey);
                    if (subtiles[idx] == null) {
                        inactiveTiles.get(subtileKey);
                    }
                    if (subtiles[idx] == null || subtiles[idx].isPlaceholder()) {
                        subtileMissing = true;
                    }
                }
            }

            if (!subtileMissing) {
                Rect dest = new Rect();
                Bitmap b = createOrReuseBitmap();
                scaleCanvas.setBitmap(b);
                for (int y0 = 0; y0 < 2 && !subtileMissing; y0++) {
                    for (int x0 = 0; x0 < 2 && !subtileMissing; x0++) {
                        final int idx = y0 * 2 + x0;
                        dest.set(x0 * TILE_SIZE_PX / 2, y0 * TILE_SIZE_PX / 2,
                                (x0 + 1) * TILE_SIZE_PX / 2, (y0 + 1) * TILE_SIZE_PX / 2);
                        scaleCanvas.drawBitmap(subtiles[idx].bitmap, null, dest, scalePaint);
                    }
                }
                t = new Tile(b);
                activeTiles.put(key, t);
            }
        }

        // If not able to synthesize from higher resolution tiles, see if we can generate a temp
        // version from lower-res tiles. We will also issue a request to get the higher-res version.
        if (t == null) {
            final int x0 = (x / TILE_SIZE_PX) % 2;
            final int y0 = (y / TILE_SIZE_PX) % 2;
            final int supertileKey = Tile.key((x - x0 * TILE_SIZE_PX) / 2,
                    (y - y0 * TILE_SIZE_PX) / 2, downsampleLog2 + 1);
            Tile supertile = activeTiles.get(supertileKey);
            if (supertile == null) {
                supertile = inactiveTiles.get(supertileKey);
            }
            if (supertile != null && !supertile.isPlaceholder()) {
                // Found a supertile
                Rect src = new Rect(x0 * TILE_SIZE_PX / 2, y0 * TILE_SIZE_PX / 2,
                        (x0 + 1) * TILE_SIZE_PX / 2, (y0 + 1) * TILE_SIZE_PX / 2);
                Rect dst = new Rect(0, 0, TILE_SIZE_PX, TILE_SIZE_PX);
                Bitmap b = createOrReuseBitmap();
                scaleCanvas.setBitmap(b);
                scaleCanvas.drawBitmap(supertile.bitmap, src, dst, scalePaint);

                // Issue a load request for the full-resolution tile
                t = new Tile(b, new LoadTileRunnable(x, y, downsampleLog2, createOrReuseBitmap(),
                        LoadTileRunnable.PRIORITY_REPLACE_LOW_RES_TILE));
                executor.execute(t.loadTileRunnable);
                activeTiles.put(key, t);
            }
        }

        // If not able to synthesize from a lower-resolution tile, we're out of luck. Return the
        // placeholder bitmap and issue a request to load the tile.
        if (t == null) {
            t = new Tile(placeholder, new LoadTileRunnable(x, y, downsampleLog2,
                    createOrReuseBitmap(), LoadTileRunnable.PRIORITY_MISSING_TILE));
            executor.execute(t.loadTileRunnable);
            activeTiles.put(key, t);
        }

        t.generation = generation;
        return t.bitmap;
    }

    /**
     * Set the area which will be used to display tiles from this cache. This is used to calculate
     * the number of cached tiles which must be maintained at any given time.
     * @param width width of the display area, in px
     * @param height height of the display area, in px
     * @throws IllegalArgumentException if width or height are < 0
     */
    public void setTiledAreaDimensions(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("width and height must be >= 0");
        }

        final int numTilesHorz = (width + TILE_SIZE_PX - 1) / TILE_SIZE_PX;
        final int numTilesVert = (height + TILE_SIZE_PX - 1) / TILE_SIZE_PX;

        inactiveTileCacheMaxSize = Math.max(numTilesHorz * numTilesVert,
                INACTIVE_TILE_CACHE_MIN_SIZE);

        List<Bitmap> currentAvailableTiles = availableTileBitmaps;
        Map<Integer, Tile> currentInactiveTiles = inactiveTiles;

        availableTileBitmaps = new AvailableBitmapArrayList(inactiveTileCacheMaxSize);
        if (currentAvailableTiles != null) {
            availableTileBitmaps.addAll(currentAvailableTiles);
        }

        inactiveTiles = new LRUTileMap(inactiveTileCacheMaxSize, availableTileBitmaps);
        if (currentInactiveTiles != null) {
            inactiveTiles.putAll(currentInactiveTiles);
        }
    }

    private Bitmap createOrReuseBitmap() {
        final Bitmap b;
        final int size = availableTileBitmaps.size();
        if (size > 0) {
            b = availableTileBitmaps.remove(size - 1);
        } else {
            b = Bitmap.createBitmap(TILE_SIZE_PX, TILE_SIZE_PX, Config.ARGB_8888);
        }

        return b;
    }

    private boolean isReusableBitmap(Bitmap b) {
        return (b != placeholder);
    }

    private final Handler.Callback loadTileCompletedCallback = new Handler.Callback() {
        private final ArrayList<LoadTileRunnable> localCompletedLoads =
                new ArrayList<LoadTileRunnable>();

        @Override
        public boolean handleMessage(Message msg) {
            synchronized (completedLoads) {
                localCompletedLoads.addAll(completedLoads);
                completedLoads.clear();
            }

            boolean tilesLoaded = false;

            for (LoadTileRunnable ltr : localCompletedLoads) {
                tilesLoaded = true;
                final int key = Tile.key(ltr.x, ltr.y, ltr.downsampleLog2);

                // Replace the placeholder Tile (if any) in the activeTiles map with a new Tile
                Tile recycle = activeTiles.put(key, new Tile(ltr.bitmap));
                if (recycle != null && isReusableBitmap(recycle.bitmap)) {
                    availableTileBitmaps.add(recycle.bitmap);
                }

                // If an older variant of this Tile (such as a low-res placeholder) is in the
                // inactiveTiles map, recycle it
                recycle = inactiveTiles.remove(key);
                if (recycle != null && isReusableBitmap(recycle.bitmap)) {
                    availableTileBitmaps.add(recycle.bitmap);
                }
            }
            localCompletedLoads.clear();

            if (tilesLoaded) {
                listener.onTileCacheUpdated();
            }

            return true;
        }
    };

    private static class Tile {
        final Bitmap bitmap;
        final LoadTileRunnable loadTileRunnable; // if non-null, indicates this Tile is a placeholder
        int generation;

        Tile(Bitmap bitmap) {
            this(bitmap, null);
        }

        Tile(Bitmap bitmap, LoadTileRunnable loadTileRunnable) {
            this.bitmap = bitmap;
            generation = INVALID_GENERATION;
            this.loadTileRunnable = loadTileRunnable;
        }

        public boolean isPlaceholder() {
            return (loadTileRunnable != null);
        }

        public static int key(int x, int y, int downsampleLog2) {
            // N.B. 911 is a prime number
            int result = 911 + x;
            result = result * 911 + y;
            result = result * 911 + downsampleLog2;
            return result;
        }
    }

    private class LoadTileRunnable implements Runnable, Comparable<LoadTileRunnable> {
        // N.B. Higher priorities should have lower values
        public static final int PRIORITY_MISSING_TILE = 0;
        public static final int PRIORITY_REPLACE_LOW_RES_TILE = 1;

        final int x;
        final int y;
        final int downsampleLog2;
        final Bitmap bitmap;
        final int priority;

        public LoadTileRunnable(int x, int y, int downsampleLog2, Bitmap bitmap, int priority) {
            this.x = x;
            this.y = y;
            this.downsampleLog2 = downsampleLog2;
            this.bitmap = bitmap;
            this.priority = priority;
        }

        @Override
        public void run() {
            final JpegTileDecoder decoder;
            try {
                decoder = decoders.take();
            } catch (InterruptedException e) {
                // N.B. Silently drop this work item. This will leave us with a placeholder tile,
                // which is a soft failure.
                Log.w(TAG, "Interrupted while waiting for a JpegTileDecoder", e);
                Thread.currentThread().interrupt();
                return;
            }

            final int downsampledImageWidth = decoder.getWidth(downsampleLog2);
            final int downsampledImageHeight = decoder.getHeight(downsampleLog2);

            Rect region = new Rect(x, y, x + TILE_SIZE_PX, y + TILE_SIZE_PX);
            if (region.right > downsampledImageWidth) {
                region.right = downsampledImageWidth;
            }
            if (region.bottom > downsampledImageHeight) {
                region.bottom = downsampledImageHeight;
            }

            decoder.decodeTile(region, downsampleLog2, bitmap);

            try {
                decoders.put(decoder);
            } catch (InterruptedException e) {
                // N.B. This is a critical failure, since we just lost a decoder
                throw new RuntimeException("Interrupted while returning a decoder back to the " +
                        "queue", e);
        }

            Log.v(TAG, "Loaded tile [" + x + "," + y + "," + downsampleLog2 + "]");

            synchronized (completedLoads) {
                completedLoads.add(this);
            }

            if (!loadTileCompletedHandler.hasMessages(0)) {
                loadTileCompletedHandler.sendEmptyMessage(0);
        }
    }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            } else if (!(o instanceof LoadTileRunnable)) {
                return false;
            }

            final LoadTileRunnable other = (LoadTileRunnable)o;
            return (x == other.x && y == other.y && downsampleLog2 == other.downsampleLog2);
        }

        @Override
        public int compareTo(LoadTileRunnable another) {
            if (another == null) {
                return 1;
            } else if (priority != another.priority) {
                return (priority - another.priority);
            } else if (downsampleLog2 != another.downsampleLog2) {
                return (downsampleLog2 - another.downsampleLog2);
            } else if (y != another.y) {
                return (y - another.y);
            } else if (x != another.x) {
                return (x - another.x);
            }

            return 0;
        }
    }

    private static class LRUTileMap extends LinkedHashMap<Integer, Tile> {
        private static final long serialVersionUID = 5579413206250249780L;

        private final int maxEntries;
        private final Collection<Bitmap> evictDestination;

        public LRUTileMap(int maxEntries, Collection<Bitmap> evictDestination) {
            super(0, 0.75f, true);
            this.maxEntries = maxEntries;
            this.evictDestination = evictDestination;
        }

        @Override
        protected boolean removeEldestEntry(Map.Entry<Integer, Tile> eldest) {
            if (size() > maxEntries) {
                evictDestination.add(eldest.getValue().bitmap);
                return true;
            } else {
                return false;
            }
        }
    }

    private static class AvailableBitmapArrayList extends ArrayList<Bitmap> {
        private static final long serialVersionUID = -8624330643857650314L;

        private final int maxEntries;

        public AvailableBitmapArrayList(int maxEntries) {
            super(maxEntries);
            this.maxEntries = maxEntries;
        }

        @Override
        public boolean add(Bitmap object) {
            add(size(), object);
            return true;
        }

        @Override
        public void add(int index, Bitmap object) {
            if (index == maxEntries) {
                // Special case, adding to end of full list
                return;
            }

            super.add(index, object);

            final int size = size();
            if (size > maxEntries) {
                remove(size - 1);
            }
        }

        @Override
        public boolean addAll(Collection<? extends Bitmap> collection) {
            return addAll(size(), collection);
        }

        @Override
        public boolean addAll(int index, Collection<? extends Bitmap> collection) {
            if (index == maxEntries) {
                // Special case, adding to end of full list
                return false;
            }

            boolean res = super.addAll(index, collection);

            final int size = size();
            if (size > maxEntries) {
                subList(maxEntries, size).clear();
            }

            return res;
        }
    }
}

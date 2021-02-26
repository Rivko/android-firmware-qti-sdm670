/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.VideoPicker;

import android.app.Activity;
import android.content.ContentUris;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.v4.app.Fragment;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import qces.qualcomm.qti.com.videosummary.R;

/**
 * {@link RecyclerView} with a {@link GridLayoutManager}.
 */
public class VideoGridRecyclerFragment extends Fragment {

    private static final String TAG = VideoGridRecyclerFragment.class.getSimpleName();

    protected RecyclerView mRecyclerView;
    protected VideoGridAdapter mAdapter;
    boolean isActivityVisible = true;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    private void loadVideoInfo() {
        final Activity activity = getActivity();
        final String orderBy = MediaStore.Video.Media.DATE_TAKEN;

        final String[] columns = {MediaStore.Video.Media.DATA, MediaStore.Video.Media.LATITUDE,
                MediaStore.Video.Media.LONGITUDE, MediaStore.Video.Media._ID, MediaStore.Video.Media.DURATION, orderBy};

        String where = MediaStore.Video.Media.DATA + " LIKE ? ";
        String whereArgs[] = new String[]{"%DCIM%"};
        Cursor cursor = getActivity().getContentResolver().query(
                MediaStore.Video.Media.EXTERNAL_CONTENT_URI, columns,
                where, whereArgs, orderBy + " DESC");

        Uri uriBase = MediaStore.Video.Media.getContentUri("external");

        if (cursor != null && cursor.getCount() > 0) {
            Log.i(TAG, "Status - total no of videos found in DCIM folder = " + cursor.getCount());
            int counter = 1;
            while (cursor.moveToNext() && isActivityVisible) {
                VideoItem item = new VideoItem();

                int intID = cursor.getInt(cursor.getColumnIndexOrThrow(MediaStore.Video.Media._ID));
                item.info.contentUri = ContentUris.withAppendedId(uriBase, intID);
                item.info.createdDate = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATE_TAKEN));
                item.info.latitude = cursor.getDouble(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.LATITUDE));
                item.info.longitude = cursor.getDouble(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.LONGITUDE));
                item.info.duration = cursor.getLong(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DURATION));
                String data = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATA));

                Log.i(TAG, "Status - count:" + counter + " => uri:" + item.info.contentUri + ", data:" + data);

                if (isExist(item.info.contentUri)) {
                    item.selected = true;
                }
                //Get Thumbnail
                BitmapFactory.Options options = new BitmapFactory.Options();
                options.inDither = false;
                options.inPreferredConfig = Bitmap.Config.ARGB_8888;
                item.thumbBitmap = MediaStore.Video.Thumbnails.getThumbnail(
                        activity.getContentResolver(),
                        intID,
                        MediaStore.Images.Thumbnails.MINI_KIND,
                        options);

                mAdapter.addVideoItem(item);
                counter++;
            }
            Log.i(TAG, " Status -  getting info for video stopped.");

        }
    }

    boolean isExist(Uri uri) {
        VideoPickerActivity activity = (VideoPickerActivity) getActivity();
        if (activity.mediaInfoArrayList != null) {
            for (int i = 0; i < activity.mediaInfoArrayList.size(); i++) {
                if (uri.compareTo(activity.mediaInfoArrayList.get(i).contentUri) == 0) {
                    return true;
                }
            }
        }
        return false;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        View rootView = inflater.inflate(R.layout.vidpicker_recycler_view_frag, container, false);
        rootView.setTag(TAG);


        mRecyclerView = (RecyclerView) rootView.findViewById(R.id.vidpicker_recyclerView);

        mAdapter = new VideoGridAdapter(getActivity(), null);
        mRecyclerView.setAdapter(mAdapter);
        mRecyclerView.addItemDecoration(new MarginDecoration(getActivity()));

        //Load video info in a thread to populate the adapters videoItems ArrayList
        new Thread(new Runnable() {
            @Override
            public void run() {
                loadVideoInfo();
            }
        }).start();

        return rootView;
    }

}

/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.VideoPicker;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;

/**
 * Provide views to RecyclerView
 */
public class VideoGridAdapter extends RecyclerView.Adapter<VideoGridAdapter.ViewHolder> {

    private static final String TAG = VideoGridAdapter.class.getSimpleName();
    private ArrayList<VideoItem> videoItems;
    private Context context;

    /**
     * Provide a reference to the type of views that you are using (custom ViewHolder)
     */
    public class ViewHolder extends RecyclerView.ViewHolder {

        private final ImageView videoThumbImgView;
        private final ImageView videoThumbCheckView;
        private final TextView videoThumbDurationView;

        //private final ArrayList

        public ViewHolder(View v, final ArrayList<VideoItem> videoItems) {
            super(v);

            // Define click listener for the ViewHolder's View.
            v.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    //Log.d(TAG, "Element " + getPosition() + " clicked.");

                    ImageView videoThumbChecker = getVideoThumbCheckView();

                    boolean isSelected = videoItems.get(getAdapterPosition()).selected;

                    isSelected = !isSelected;

                    videoThumbChecker.setSelected(isSelected);
                    videoItems.get(getAdapterPosition()).selected = isSelected;

                    if (!((VideoPickerActivity) context).multiSelect) {
                        ((VideoPickerActivity) context).performDone(videoItems.get(getAdapterPosition()).info);
                    }

                    refreshDoneBtnVisibility();

                }
            });

            videoThumbImgView = (ImageView) v.findViewById(R.id.videoThumbImgView);
            videoThumbCheckView = (ImageView) v.findViewById(R.id.videoThumbChecker);
            videoThumbDurationView = (TextView) v.findViewById(R.id.videoThumbDuration);

        }

        public ImageView getThumbImgView() {
            return videoThumbImgView;
        }

        public TextView getVideoThumbDurationView() {
            return videoThumbDurationView;
        }

        public ImageView getVideoThumbCheckView() {
            return videoThumbCheckView;
        }
    }

    public VideoGridAdapter(Context context, ArrayList<VideoItem> videoItems) {
        this.context = context;
        this.videoItems = videoItems;
        if (this.videoItems == null) {
            this.videoItems = new ArrayList<VideoItem>();
        }
    }

    // Create new views (invoked by the layout manager)
    @Override
    public ViewHolder onCreateViewHolder(ViewGroup viewGroup, int viewType) {
        // Create a new view.
        View v = LayoutInflater.from(viewGroup.getContext())
                .inflate(R.layout.video_item, viewGroup, false);

        return new ViewHolder(v, videoItems);
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(ViewHolder viewHolder, final int position) {

        //Log.d(TAG, "Element " + position + " set.");

        // Get element from your dataset at this position and replace the contents of the view
        // with that element
        viewHolder.getThumbImgView().setImageBitmap(videoItems.get(position).thumbBitmap);
        viewHolder.getVideoThumbDurationView().setText(CalendarHelper.getTimeString(videoItems.get(position).info.duration / 1000));
        viewHolder.getVideoThumbCheckView().setSelected(videoItems.get(position).selected);

    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return videoItems.size();
    }

    public void addVideoItem(VideoItem item) {
        videoItems.add(item);
        ((VideoPickerActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemInserted(getItemCount());
            }
        });

    }

    private void refreshDoneBtnVisibility() {

        boolean doneVisible = false;
        for (int i = 0; i < getItemCount(); i++) {
            VideoItem video = getVideoItem(i);
            if (video.selected) { //if any video is selected then enable it
                doneVisible = true;
                break;
            }
        } //otherwise don't

        ((VideoPickerActivity) context).enableDoneBtn(doneVisible);
    }

    public VideoItem getVideoItem(int index) {
        return videoItems.get(index);
    }

}

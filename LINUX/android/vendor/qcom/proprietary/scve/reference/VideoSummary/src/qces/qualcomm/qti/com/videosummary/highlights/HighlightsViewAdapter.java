/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.highlights;

import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.net.Uri;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.support.v7.widget.Toolbar.OnMenuItemClickListener;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFaceModel;
import qces.qualcomm.qti.com.videosummary.MediaInfoGenerator.MediaInfo;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.CalendarHelper;
import qces.qualcomm.qti.com.videosummary.Util.Constants;
import qces.qualcomm.qti.com.videosummary.Util.ImageUtils;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.VideoPlayer.VideoPlayerActivity;
import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.QueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;
import qces.qualcomm.qti.com.videosummary.settings.SettingsActivity;
import qces.qualcomm.qti.com.videosummary.videoprocessing.VideoServices.VidProcManagerService;
import qces.qualcomm.qti.com.videosummary.watchdog.Watchdog;

/**
 * Created by mithun on 8/14/15.
 */
public class HighlightsViewAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private static final String TAG = HighlightsViewAdapter.class.getSimpleName();
    List<HighlightsModel> highlightVideos;
    private Context context;


    HighlightsViewAdapter(Context context) {
        this.context = context;
        if (this.highlightVideos == null) {
            this.highlightVideos = new ArrayList<HighlightsModel>();
        }
    }

    void deleteHighlightItemFromDb(HighlightsModel highlightItem) {
        QueryHelper.deleteFromHighlights(context, highlightItem.videoFilePath);
        deleteFromFileSystem(highlightItem);
        //Do not call  delete(getAdapterPosition()), as delete in DB query will send a notification with this adapters observer will receive and will delete it
    }

    void deleteFromFileSystem(HighlightsModel highlightItem) {
        Utils.deleteFile(highlightItem.videoFilePath);
        Utils.deleteFile(highlightItem.thumbImgFilePath);
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view;
        RecyclerView.ViewHolder holder = null;
        switch (HighlightsModel.CardType.values()[viewType]) {
            case SUMMARY:
                view = inflateViewForViewgroup(parent, R.layout.highlights_item);
                holder = new ViewHolderSummary(view);
                break;
            case DEMO:
                view = inflateViewForViewgroup(parent, R.layout.highlights_demo_item);
                holder = new ViewHolderDemo(view);
                break;
            case CREATING:
                view = inflateViewForViewgroup(parent, R.layout.highlights_creating_movie_item);
                holder = new ViewHolderCreatingMovie(view);
                break;
            case SAVED_TO_GALLERY:
                view = inflateViewForViewgroup(parent, R.layout.highlights_saved_to_gallery_item);
                holder = new ViewHolderSavedToGallery(view);
                break;
            case ERROR:
                view = inflateViewForViewgroup(parent, R.layout.highlights_creating_error_item);
                holder = new ViewHolderCreatingError(view);
                break;
        }
        return holder;
    }

    View inflateViewForViewgroup(ViewGroup parent, int layoutRes) {
        return LayoutInflater.from(parent.getContext()).inflate(layoutRes, parent, false);
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        HighlightsModel item = highlightVideos.get(position);
        switch (item.cardType) {
            case SUMMARY:
                loadSummaryCard(item, (ViewHolderSummary) holder);
                break;
            case DEMO:
                loadDemoCard((ViewHolderDemo) holder);
                break;
            case CREATING:
                loadCreatingMovieCard(item, (ViewHolderCreatingMovie) holder);
                break;
            case SAVED_TO_GALLERY:
                loadSavedToGalleryCard(item, (ViewHolderSavedToGallery) holder);
                break;
            case ERROR:
                loadErrorCard(item, (ViewHolderCreatingError) holder);
                break;
            default:
                throw new UnsupportedOperationException("TYPE unknown " + getItemViewType(position));
        }
    }

    void loadSummaryCard(HighlightsModel item, ViewHolderSummary holder) {

        if (item.thumbImgBmap != null) {
            holder.videoThumbnail.setImageBitmap(item.thumbImgBmap);
        } else {
            holder.videoThumbnail.setImageResource(R.drawable.img_herocard);
        }

        if (item.faceIdList != null) {
            holder.starringLayout.setVisibility(View.VISIBLE);
            if (item.faceIdList.size() <= 3) {
                int i = 0;
                for (; i < item.faceIdList.size(); i++) {
                    holder.starring[i].setVisibility(View.VISIBLE);
                    Bitmap bmap = (item.starringList.get(i)).photo;
                    if (bmap != null) {
                        holder.starring[i].setImageBitmap(bmap);
                    } else {
                        holder.starring[i].setVisibility(View.GONE);
                    }
                }

                //disable the remaining starring imageviews
                for (; i < 3; i++) {
                    holder.starring[i].setVisibility(View.GONE);
                }
                holder.starringText.setVisibility(View.GONE);

            } else {
                //run this for loop for 0 & 1 and for the last one show "+n" where n is remaining number of starring faces
                for (int i = 0; i < 2; i++) {
                    holder.starring[i].setVisibility(View.VISIBLE);
                    Bitmap bmap = (item.starringList.get(i)).photo;
                    if (bmap != null) {
                        holder.starring[i].setImageBitmap(bmap);
                    } else {
                        holder.starring[i].setVisibility(View.GONE);
                    }
                }

                //To show +n in another round icon next to the starring faces if there are more than 3 starring faces
                Bitmap bmap = Bitmap.createBitmap(80, 80, Bitmap.Config.ARGB_8888);
                bmap.eraseColor(context.getResources().getColor(R.color.primaryColorDark));
                holder.starringText.setBackground(new BitmapDrawable(ImageUtils.createRoundIcon(bmap, 80, 10)));
                holder.starringText.setText("+" + (item.faceIdList.size() - 2));
                holder.starringText.setVisibility(View.VISIBLE);
            }


        } else {
            //disable the starring layout and imageviews
            holder.starringLayout.setVisibility(View.GONE);
            for (int i = 0; i < 3; i++) {
                holder.starring[i].setVisibility(View.GONE);
            }
            holder.starringText.setVisibility(View.GONE);
        }

        holder.title.setText(item.title);
        String subTitle = item.subTitle;
        if (subTitle.length() > 0) {
            subTitle += " - ";
        }
        subTitle += CalendarHelper.getTimeString(Utils.getDuration(item.videoFilePath) / 1000);
        holder.subTitle.setText(subTitle);

        String timeStampString = "Created: " +
                CalendarHelper.timeStampToDateString(item.timeStampMillis) +
                "\nAvailable until: " +
                CalendarHelper.timeStampToDateString(CalendarHelper.addOneWeek(item.timeStampMillis));
        holder.timestampText.setText(timeStampString);
        holder.hideSpinner();
    }

    void loadDemoCard(HighlightsViewAdapter.ViewHolderDemo holder) {
        //nothing to dynamically load, everything is static
    }

    void loadCreatingMovieCard(HighlightsModel item, HighlightsViewAdapter.ViewHolderCreatingMovie holder) {
        String title = "", status = "";
        if (item.summaryType == HighlightsModel.SummaryType.HIGHLIGHTS) {
            title = context.getResources().getString(R.string.creating_highlights);
            if (item.status.progress == -1) {
                //in case of multiple create cards (i.e user tries to run 2 or more summary back to back even before the older onces are yet ot be complted, just display this message
                status = context.getResources().getString(R.string.video_queued_for_processing_status);
            } else if (item.status.currentUriIndex == -2) {
                status = context.getResources().getString(R.string.audio_processing_status);
            } else if (item.status.currentUriIndex == -1) {
                //its assembling for highlights
                status = context.getResources().getString(R.string.highlights_assembling_status);
            } else {
                //this means its scoring for highlights
                status = context.getResources().getString(R.string.highlights_scoring_status) + " " + item.status.currentUriIndex + "/" + item.status.maxUris;
            }

        } else {
            title = context.getResources().getString(R.string.creating_timelapse);
            if (item.status.progress == -1) {
                //in case of multiple create cards (i.e user tries to run 2 or more summary back to back even before the older onces are yet ot be complted, just display this message
                status = context.getResources().getString(R.string.video_queued_for_processing_status);
            } else if (item.status.currentUriIndex == -2) {
                status = context.getResources().getString(R.string.audio_processing_status);
            } else if (item.status.currentUriIndex == -1) {
                //its assembling for Timelapse
                status = context.getResources().getString(R.string.timelapse_assembling_status);
            } else {
                //this means its scoring for Timelapse
                status = context.getResources().getString(R.string.timelapse_scoring_status) + " " + item.status.currentUriIndex + "/" + item.status.maxUris;
            }
        }
        holder.title.setText(title);
        holder.statusText.setText(status);
        if (item.status.progress > 0) {
            holder.progressBar.setIndeterminate(false);
        } else {
            holder.progressBar.setIndeterminate(true);
        }
        holder.progressBar.setProgress(item.status.progress);

    }

    void loadSavedToGalleryCard(HighlightsModel item, HighlightsViewAdapter.ViewHolderSavedToGallery holder) {
        holder.summaryText.setText(item.title);
        holder.viewInGallery.setClickable(true);
        //TODO:load summary based on the video title which was saved and link the view button
    }

    //load customized error message and title
    void loadErrorCard(HighlightsModel item, HighlightsViewAdapter.ViewHolderCreatingError holder) {
        String title = "Error!";
        String errorMsg = "Unknown error";
        holder.cancel.setVisibility(View.VISIBLE);
        holder.restart.setVisibility(View.GONE);
        switch (Integer.parseInt(item.thumbImgFilePath)) {
            case Constants.SUMMARY_ERROR_INTERRUPTED:
                title = "Video creation interrupted";
                errorMsg = "Tap to continue making highlight video";
                holder.cancel.setVisibility(View.GONE);
                holder.restart.setVisibility(View.VISIBLE);
                break;
            case Constants.SUMMARY_ERROR_LENGTH_INSUFFICIENT:
                errorMsg = "Original media length too short.";
                break;
            case Constants.SUMMARY_ERROR_DISK_SPACE_EXCEEDED:
                errorMsg = "Not enough space on device.";
                break;
            default:
                break;
        }
        holder.errorTitle.setText(title);
        holder.errorMessage.setText(errorMsg);
    }


    HighlightsModel addCreatingMovieCard(final HighlightsModel.SummaryType summaryType, final ArrayList<MediaInfo> mediaInfoArrayList, long timeStampMillis) {
        String mp4FilePath = Utils.getMp4FilePath(context, timeStampMillis).toString();
        String thumbImgPath = Utils.getThumbImgFilePath(context, timeStampMillis).toString();
        String title = "Untitled.. Please edit your title";
        String subTitle = "";

        int speedOrLength = 0;
        int theme = Constants.DEFAULT_THEME;
        if (summaryType == HighlightsModel.SummaryType.HIGHLIGHTS) {
            int length = SettingsActivity.getPrefDefaultLength(context);
            speedOrLength = HighlightsModel.getSpeedOrLength(length, theme);
        } else {
            int speed = SettingsActivity.getPrefSpeed(context);
            boolean eventDetectEnabled = SettingsActivity.getPrefEventDetectionEnabled(context);
            int eventSpeed = SettingsActivity.getPrefEventSpeed(context);
            boolean motionEventDetectEnabled = SettingsActivity.getPrefMotionEventDetectionEnabled(context);
            boolean faceEventDetectEnabled = SettingsActivity.getPrefFaceEventDetectionEnabled(context);
            speedOrLength = HighlightsModel.getSpeedOrLength(speed, eventDetectEnabled, eventSpeed, motionEventDetectEnabled, faceEventDetectEnabled);
        }
        //using same theme for both Highlights and timelapse
        speedOrLength = HighlightsModel.updateTheme(speedOrLength, theme);

        final HighlightsModel item = new HighlightsModel(HighlightsModel.CardType.CREATING, summaryType,
                mp4FilePath, thumbImgPath, timeStampMillis, title, subTitle, mediaInfoArrayList, speedOrLength, null);

        if (highlightVideos.size() > 0 && highlightVideos.get(0).cardType == HighlightsModel.CardType.DEMO) {
            //Delete the Demo Card Item from adapter
            delete(0);
        }
        addHighlightsItemAtPos(item, 0);
        QueryHelper.insertIntoHighlights(context, item);

        //Update title in a thread which runs Geocoder (which inturn uses wifi or data) which is sometimes timeconsuming
        new Thread(new Runnable() {
            @Override
            public void run() {
                item.updateTitle(context);
                Utils.sendMessage(context, TAG, "Updating title as " + item.title + " & subTitle as " + item.subTitle);
                //This update has to be called after insert else it wont take effect
                QueryHelper.updateHighlights(context, item.videoFilePath, item.title, item.subTitle);
            }
        }).start();
        return item;
    }

    @Override
    public void onAttachedToRecyclerView(RecyclerView recyclerView) {
        super.onAttachedToRecyclerView(recyclerView);
    }

    @Override
    public int getItemCount() {
        return highlightVideos.size();
    }

    @Override
    public int getItemViewType(int position) {
        return highlightVideos.get(position).cardType.ordinal();
    }

    /**
     * This method will initialize the adapter array completely if array is empty, and can be called
     * multiple times in which case it will update only the newely added or deleted item.
     */
    synchronized public void loadHighlightInfo() {
        Cursor cursor = QueryHelper.queryHighlights(context);
        int adapterIndex = 0;

        /**
         * When the activity is getting created and launched, then the highlightVideos array is
         * anyways empty. So set this flag and just blindly fill the array with the items from
         * cursor as we know rows retrieved from db is already sorted.
         */
        boolean blindFill = false;

        if (getItemCount() == 0) {
            blindFill = true;
        } else if (highlightVideos.get(0).cardType == HighlightsModel.CardType.DEMO) {
            blindFill = true;
            highlightVideos.clear();
        }
        Utils.sendMessage(context, TAG, "blind fill = " + blindFill + " highlightVideos.size = " + highlightVideos.size() + " cursor count = " + cursor.getCount());

        if (cursor != null && cursor.getCount() > 0) {
            while (cursor.moveToNext()) {
                int cardType = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._CARD_TYPE));
                int summaryType = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._SUMMARY_TYPE));
                String videoFilePath = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._URI));
                String thumbImgFilePath = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._THUMBNAIL_URI));
                long createdTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._CREATED_DATE));
                String title = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._TITLE));
                String subTitle = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._SUB_TITLE));
                String location = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._LOCATION));
                ArrayList<MediaInfo> mediaInfoArrayList = Utils.getCompositionMediaArrayListFromString(cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._COMPOSITION_MEDIA_IDS)));
                int speedOrLength = cursor.getInt(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._SPEED_LENGTH));
                List<byte[]> faceIdList = QueryHelper.getFaceIdsAsArrayListOfByteArrayFromString(cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.Highlights._STARRING_IDS)));
                HighlightsModel highlightItem = new HighlightsModel(HighlightsModel.CardType.values()[cardType],
                        HighlightsModel.SummaryType.values()[summaryType],
                        videoFilePath, thumbImgFilePath, createdTimestamp,
                        title, subTitle,
                        mediaInfoArrayList, speedOrLength,
                        faceIdList);

                /*This bitmap decoding and saving into highlights model is to be done only for the Highlights activity to get smoother scrolling on the highlights inbox,
                * Do not move this to Highlights model as parceling a highlights model will take up too much memory by these bitmaps and during passing this item to other
                * activities will crash due to TooLargeTransaction*/
                //Start of bitmap loading
                if (highlightItem.cardType == HighlightsModel.CardType.SUMMARY) {
                    if (thumbImgFilePath != null) {
                        File imgFile = new File(thumbImgFilePath);
                        if (imgFile.exists()) {
                            highlightItem.thumbImgBmap = BitmapFactory.decodeFile(imgFile.getAbsolutePath());
                        }
                    }

                    for (int i = 0; faceIdList != null && i < faceIdList.size(); i++) {
                        Cursor c = FRQueryHelper.queryEnrolledFace(context, faceIdList.get(i));
                        String frDatasetId = c.getString(c.getColumnIndexOrThrow(VideoSummaryContract.FaceEnrollment._FR_DATASET_ID));
                        String name = c.getString(c.getColumnIndexOrThrow(VideoSummaryContract.FaceEnrollment._PERSON_NAME));
                        Bitmap photo = FRQueryHelper.BitmapUtilities.getImage(c.getBlob(c.getColumnIndexOrThrow(VideoSummaryContract.FaceEnrollment._PHOTO)));
                        Bitmap roundedBmap = null;
                        if (photo != null) {
                            roundedBmap = ImageUtils.createRoundIcon(photo, 80, 10);
                        }
                        //FaceModel faceItem = new FaceModel(frDatasetId, name, photo);
                        DetectedFaceModel faceItem = new DetectedFaceModel(frDatasetId.getBytes(), name, roundedBmap, DetectedFaceModel.FaceType.KNOWN);
                        if (highlightItem.starringList == null) {
                            highlightItem.starringList = new ArrayList<DetectedFaceModel>();
                        }
                        highlightItem.starringList.add(faceItem);
                    }
                }
                //End of bitmap loading

                if (blindFill) {
                    if (highlightItem.cardType == HighlightsModel.CardType.CREATING && (Watchdog.isWatchDogEnabled() && (Calendar.getInstance().getTimeInMillis() - createdTimestamp) > 500) && !Watchdog.isWatchDogActive()) {
                        addHighlightsItemAtPos(highlightItem, adapterIndex);
                        //if its a creating card and if watch dog is not running then its either dead or in the intentservice queue

                        //check the intent service queue.

                        //for now lets see if dog is not active and we are here, that means nothing is runnning and this is a dead CREATING card
                        highlightItem.cardType = HighlightsModel.CardType.ERROR;
                        highlightItem.thumbImgFilePath = Integer.toString(Constants.SUMMARY_ERROR_INTERRUPTED);


                        QueryHelper.updateHighlightsAsError(context, highlightItem.videoFilePath, Constants.SUMMARY_ERROR_INTERRUPTED);
                    } else {
                        addHighlightsItem(highlightItem);
                    }
                    adapterIndex++;
                } else {
                    if (adapterIndex >= highlightVideos.size()) {
                        throw new ArrayIndexOutOfBoundsException("adapterIdx=" + adapterIndex + " highlightsvideos.size =" + highlightVideos.size());
                    }
                    Utils.sendMessage(context, TAG, "highlightItem.path = " + highlightItem.videoFilePath + " adapter idx=" + adapterIndex + " adapter.path=" + highlightVideos.get(adapterIndex).videoFilePath);
                    //until cursor item == highlightvideos item, either delete or add items to make it equal
                    while (highlightItem.videoFilePath.compareTo(highlightVideos.get(adapterIndex).videoFilePath) != 0) {
                        if (highlightItem.timeStampMillis < highlightVideos.get(adapterIndex).timeStampMillis) {
                            delete(adapterIndex);
                        } else {
                            addHighlightsItemAtPos(highlightItem, adapterIndex);
                        }
                    }

                    if (highlightItem.cardType == HighlightsModel.CardType.CREATING && ((Calendar.getInstance().getTimeInMillis() - createdTimestamp) > 500) && !Watchdog.isWatchDogActive()) {
                        //if its a creating card and if watch dog is not running then its either dead or in the intentservice queue

                        //check in the intent service queue.

                        //for now lets see if dog is not active and we are here, that means nothing is runnning and this is a dead CREATING card
                        //since this is not first time fresh loading, only update the db and the db change observer will call load with error card
                        QueryHelper.updateHighlightsAsError(context, highlightItem.videoFilePath, Constants.SUMMARY_ERROR_INTERRUPTED);
                    }
                    if (highlightItem.videoFilePath.equalsIgnoreCase(highlightVideos.get(adapterIndex).videoFilePath)) {
                        //if equal, then check if CardType changed, if changed it could be from creating movie to summary
                        if (highlightItem.cardType != highlightVideos.get(adapterIndex).cardType) {
                            delete(adapterIndex);//delete the creating movie card (old card)
                            addHighlightsItemAtPos(highlightItem, adapterIndex);//add the new card i.e summary card
                        }
                        //If the title was changed
                        if (!highlightItem.title.equalsIgnoreCase(highlightVideos.get(adapterIndex).title)) {
                            Utils.sendMessage(context, TAG, "title changed from " + highlightVideos.get(adapterIndex).title + " to" + highlightItem.title);
                            delete(adapterIndex);//delete the summary card with old title (old card)
                            addHighlightsItemAtPos(highlightItem, adapterIndex);//add the new summary card with updated title
                        }
                    }
                    adapterIndex++;
                }
            }
        }
        while (adapterIndex < getItemCount()) {
            delete(adapterIndex);
        }

        if (highlightVideos.size() == 0) {
            fillWithDemoCard();
        }
    }

    public void fillWithDemoCard() {
        HighlightsModel demoItem = new HighlightsModel(HighlightsModel.CardType.DEMO, HighlightsModel.SummaryType.HIGHLIGHTS, null, null, 0, null, null, null, 0, null);
        addHighlightsItem(demoItem);
    }

    public void addHighlightsItemAtPos(HighlightsModel item, final int pos) {
        highlightVideos.add(pos, item);
        ((HighlightsActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemInserted(pos);
                //If you are viewing the top of the list, then animate it to scroll up after inserting above 1st position
                if (HighlightsActivity.llm.findFirstCompletelyVisibleItemPosition() == 0) {
                    HighlightsActivity.llm.scrollToPosition(0);
                }
            }
        });
    }

    public void addHighlightsItem(HighlightsModel item) {
        highlightVideos.add(item);
        ((HighlightsActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemInserted(getItemCount());
            }
        });
    }

    public void delete(final int position) {
        highlightVideos.remove(position);
        ((HighlightsActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemRemoved(position);
            }
        });

    }

    public void updateProgress(long timeStampMillis, CreatingStatus status) {
        for (int i = 0; i < highlightVideos.size(); i++) {

            if (highlightVideos.get(i).timeStampMillis == timeStampMillis) {
                if (i != HighlightsModel.swipingPos) {
                    updateProgress(i, status);
                }
                break;
            }
        }
    }

    public void updateProgress(final int pos, CreatingStatus creatingStatus) {
        updateStatusForItem(highlightVideos.get(pos), creatingStatus);
        ((HighlightsActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemChanged(pos);
            }
        });

    }

    //do a deep copy into the highlightsModel item
    void updateStatusForItem(HighlightsModel item, CreatingStatus currentStatus) {
        if (item.status != null) {
            item.status.currentUriIndex = currentStatus.currentUriIndex;
            item.status.maxUris = currentStatus.maxUris;
            item.status.progress = currentStatus.progress;
        } else {
            Log.e(TAG, "Status - Error!! status == null for uriIdx:" + currentStatus.currentUriIndex + ", maxUris:" + currentStatus.maxUris + ",progress:" + currentStatus.progress);
        }
    }

    void onItemDismiss(int pos) {
        HighlightsModel item = highlightVideos.get(pos);
        if (item.cardType == HighlightsModel.CardType.CREATING) {
            VidProcManagerService.stopActionProcess(item.timeStampMillis);
        }
        deleteHighlightItemFromDb(item);
    }

    void deleteSavedToGalleryCards() {
        QueryHelper.deleteSavedToGalleryCards(context);
    }

    void deleteExpiredSummaryVideos() {
        Calendar minusSevenDays = Calendar.getInstance();
        minusSevenDays.add(Calendar.DATE, -7);
        for (HighlightsModel item : highlightVideos) {
            if (item.timeStampMillis < minusSevenDays.getTimeInMillis()) {
                Log.i(TAG, "Deleting expired video created on: " + CalendarHelper.timeStampMillisToString(item.timeStampMillis));
                deleteHighlightItemFromDb(item);
            }
        }
    }

    public class ViewHolderSummary extends RecyclerView.ViewHolder implements View.OnClickListener, OnMenuItemClickListener {
        ImageView videoThumbnail;
        LinearLayout starringLayout;
        ImageView starring[];
        TextView starringText;

        TextView title;
        TextView subTitle;
        TextView timestampText;

        Toolbar toolbar;
        ImageView videoPlayIcon;
        TextView saveToGallery;
        LinearLayout progressBarLinearLayout;

        ViewHolderSummary(View itemView) {
            super(itemView);

            videoThumbnail = (ImageView) itemView.findViewById(R.id.video_thumbnail);
            videoThumbnail.setOnClickListener(this);

            starringLayout = (LinearLayout) itemView.findViewById(R.id.starring_layout);
            starring = new ImageView[3];
            starring[0] = (ImageView) itemView.findViewById(R.id.starring_1);
            starring[1] = (ImageView) itemView.findViewById(R.id.starring_2);
            starring[2] = (ImageView) itemView.findViewById(R.id.starring_3);
            starringText = (TextView) itemView.findViewById(R.id.starring_text_3);

            title = (TextView) itemView.findViewById(R.id.title);
            subTitle = (TextView) itemView.findViewById(R.id.sub_title);

            timestampText = (TextView) itemView.findViewById(R.id.time_stamp);

            toolbar = (Toolbar) itemView.findViewById(R.id.toolbar);
            toolbar.inflateMenu(R.menu.menu_highlight_cards);
            toolbar.setOnMenuItemClickListener(this);

            videoPlayIcon = (ImageView) itemView.findViewById(R.id.video_play);
            videoPlayIcon.setOnClickListener(this);

            saveToGallery = (TextView) itemView.findViewById(R.id.save_to_gallery);
            saveToGallery.setOnClickListener(this);

            progressBarLinearLayout = (LinearLayout) itemView.findViewById(R.id.progress_bar_linear_layout);
        }

        @Override
        public void onClick(View v) {

            if (progressBarLinearLayout.getVisibility() == View.VISIBLE) {
                //If spinner/progress bar is visible, then consume all onClicks (i.e dont perform any clicks on this view)
                return;
            }
            if (v.getId() == R.id.video_play || v.getId() == R.id.video_thumbnail) {
                playbackSummary();
            } else if (v.getId() == R.id.save_to_gallery) {
                showSpinner();
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        saveToGallery(getAdapterPosition());
                    }
                }).start();
            }

        }

        void playbackSummary() {
            HighlightsModel item = highlightVideos.get(getAdapterPosition());
            String videoFilePath = item.videoFilePath;
            Log.i(TAG, "Video file path = " + videoFilePath);
            Intent vidPlayer = new Intent(context, VideoPlayerActivity.class);
            vidPlayer.putExtra(Constants.EXTRA_HIGHLIGHTS_MODEL, item);
            context.startActivity(vidPlayer);
        }

        @Override
        public boolean onMenuItemClick(MenuItem item) {

            switch (item.getItemId()) {
                case R.id.action_view:
                playbackSummary();
                    break;
                case R.id.action_delete:
                Log.i(TAG, "delete item " + getAdapterPosition());
                if (getAdapterPosition() != -1) {
                    HighlightsModel highlightsItem = highlightVideos.get(getAdapterPosition());
                    deleteHighlightItemFromDb(highlightsItem);
                        break;
                }
                    return false;
                default:
                    return false;
            }

            return true;
        }

        void saveToGallery(int pos) {
            HighlightsModel highlightItem = highlightVideos.get(pos);
            try {
                Uri fileUri = Utils.saveToGallery(highlightItem.videoFilePath);
                context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, fileUri));
                HighlightsModel newItem = new HighlightsModel(HighlightsModel.CardType.SAVED_TO_GALLERY,
                        highlightItem.summaryType,
                        fileUri.toString(), fileUri.toString(),
                        highlightItem.timeStampMillis,
                        "The movie " + highlightItem.title + " has been saved to your gallery.",
                        highlightItem.subTitle,
                        highlightItem.compositonMediaList, highlightItem.speedOrLength,
                        highlightItem.faceIdList);
                QueryHelper.updateHighlights(context, highlightItem, newItem);
                deleteFromFileSystem(highlightItem);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        void showSpinner() {
            progressBarLinearLayout.setVisibility(View.VISIBLE);
            hideToolBar();
        }

        void hideSpinner() {
            progressBarLinearLayout.setVisibility(View.GONE);
            showToolBar();
        }

        void hideToolBar() {
            toolbar.setVisibility(View.INVISIBLE);
        }

        void showToolBar() {
            toolbar.setVisibility(View.VISIBLE);
        }

    }

    public class ViewHolderDemo extends RecyclerView.ViewHolder {

        ViewHolderDemo(View itemView) {
            super(itemView);
        }

    }

    public class ViewHolderCreatingMovie extends RecyclerView.ViewHolder {
        TextView title;
        TextView statusText;
        ProgressBar progressBar;

        ViewHolderCreatingMovie(View itemView) {
            super(itemView);
            title = (TextView) itemView.findViewById(R.id.title);
            statusText = (TextView) itemView.findViewById(R.id.status_text);
            progressBar = (ProgressBar) itemView.findViewById(R.id.progress_bar);
        }

    }

    public class ViewHolderSavedToGallery extends RecyclerView.ViewHolder {
        TextView summaryText;
        TextView viewInGallery;

        ViewHolderSavedToGallery(View itemView) {
            super(itemView);
            summaryText = (TextView) itemView.findViewById(R.id.saved_summary);

            viewInGallery = (TextView) itemView.findViewById(R.id.view_in_gallery);
            viewInGallery.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    viewInGallery.setClickable(false);
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            HighlightsModel item = highlightVideos.get(getAdapterPosition());
                            File file = new File(URI.create(item.videoFilePath));
                            deleteHighlightItemFromDb(item);
                            try {
                                //Wait for remove animation to complete
                                Thread.sleep(1000);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            Intent intent = new Intent(Intent.ACTION_VIEW);
                            intent.setDataAndType(Uri.fromFile(file), "video/*");
                            context.startActivity(intent);
                        }
                    }).start();
                }
            });
        }
    }

    public class ViewHolderCreatingError extends RecyclerView.ViewHolder {
        TextView errorTitle;
        TextView errorMessage;
        ImageView cancel;
        ImageView restart;
        View clickableView;

        ViewHolderCreatingError(View itemView) {
            super(itemView);
            errorTitle = (TextView) itemView.findViewById(R.id.title);
            errorMessage = (TextView) itemView.findViewById(R.id.error_message);
            cancel = (ImageView) itemView.findViewById(R.id.close);
            cancel.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    deleteHighlightItemFromDb(highlightVideos.get(getAdapterPosition()));
                }
            });
            restart = (ImageView) itemView.findViewById(R.id.restart);
            restart.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    clickableView.callOnClick();
                }
            });

            clickableView = (View) itemView.findViewById(R.id.view_clickable);
            clickableView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    HighlightsModel item = highlightVideos.get(getAdapterPosition());
                    if (item.thumbImgFilePath.equalsIgnoreCase(Integer.toString(Constants.SUMMARY_ERROR_INTERRUPTED))) {
                        //change the card type back to CREATING from ERROR
                        item.cardType = HighlightsModel.CardType.CREATING;

                        //Error message is stored in the thumbImageFilePath so reload the location
                        item.thumbImgFilePath = Utils.getThumbImgFilePath(context, item.timeStampMillis).toString();

                        VidProcManagerService.startActionProcess(context, item);
                        QueryHelper.updateHighlights(context, item);
                    }
                }
            });

        }

    }
}

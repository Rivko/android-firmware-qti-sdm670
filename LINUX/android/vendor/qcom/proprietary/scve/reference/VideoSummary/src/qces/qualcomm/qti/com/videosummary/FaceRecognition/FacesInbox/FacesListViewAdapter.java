/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.FacesInbox;

import android.app.Activity;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import qces.qualcomm.qti.com.videosummary.FaceRecognition.AddFace.AddFaceActivity;
import qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces.DetectedFaceModel;
import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.Util.ImageUtils;
import qces.qualcomm.qti.com.videosummary.Util.Utils;
import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;
import qces.qualcomm.qti.com.videosummary.providers.VideoSummaryContract;

/**
 * Created by mithun on 8/14/15.
 */
public class FacesListViewAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private static final String TAG = FacesListViewAdapter.class.getSimpleName();
    List<DetectedFaceModel> facesEnrolledList;
    private Context context;

    public FacesListViewAdapter(Context context) {
        this.context = context;
        if (this.facesEnrolledList == null) {
            this.facesEnrolledList = new ArrayList<DetectedFaceModel>();
        }
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v;
        RecyclerView.ViewHolder holder = null;
        switch (DetectedFaceModel.FaceType.values()[viewType]) {
            case DEMO:
                v = LayoutInflater.from(parent.getContext()).inflate(R.layout.face_demo_item, parent, false);
                holder = new ViewHolderFaceDemoCard(v);
                break;
            default:
                v = LayoutInflater.from(parent.getContext()).inflate(R.layout.face_item, parent, false);
                holder = new ViewHolderFaceCard(v);
        }

        return holder;
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        DetectedFaceModel item = facesEnrolledList.get(position);
        switch (item.faceType) {
            case DEMO:
                loadDemoCard(item, (ViewHolderFaceDemoCard) holder);
                break;
            default:
                loadFaceCard(item, (ViewHolderFaceCard) holder);
        }


    }

    @Override
    public void onAttachedToRecyclerView(RecyclerView recyclerView) {
        super.onAttachedToRecyclerView(recyclerView);
    }

    @Override
    public int getItemCount() {
        return facesEnrolledList.size();
    }

    @Override
    public int getItemViewType(int position) {
        return facesEnrolledList.get(position).faceType.ordinal();
    }

    private void loadDemoCard(DetectedFaceModel item, ViewHolderFaceDemoCard holder) {
        //holder.name.setText(item.name);
        //holder.photo.setImageBitmap(ImageUtils.createRoundIcon(item.photo, item.photo.getWidth(), 10));
    }

    private void loadFaceCard(DetectedFaceModel item, ViewHolderFaceCard holder) {
        holder.name.setText(item.name);
        holder.photo.setImageBitmap(ImageUtils.createRoundIcon(item.photo, item.photo.getWidth(), 10));
    }

    void deleteFaceItemFromDb(DetectedFaceModel faceItem) {
        FRQueryHelper.deleteFromFaceEnrollment(context, faceItem.frDatasetId);
        //Do not call  delete(getAdapterPosition()), as delete in DB query will send a notification with this adapters observer will receive and will delete it
    }

    public DetectedFaceModel getItem(int pos) {
        return facesEnrolledList.get(pos);
    }

    /**
     * This method will initialize the adapter array completely if array is empty, and can be called
     * multiple times in which case it will update only the newely added or deleted item.
     */
    synchronized public void loadFacesEnrollementInfo() {
        Log.i(TAG, "loadFacesEnrollementInfo called");
        Cursor cursor = FRQueryHelper.queryAllEnrolledFaces(context);
        int adapterIndex = 0;

        boolean showDemoCard = true;
        if (context.getClass().getSimpleName().compareTo(AddFaceActivity.class.getSimpleName()) == 0) {
            Log.i(TAG, "Add face activity so no demo");
            showDemoCard = false;
        }

        /**
         * When the activity is getting created and launched, then the facesEnrolledList array is
         * anyways empty. So set this flag and just blindly fill the array with the items from
         * cursor as we know rows retrieved from db is already sorted.
         */
        boolean blindFill = false;

        if (getItemCount() == 0) {
            blindFill = true;
        } else if (facesEnrolledList.get(0).faceType == DetectedFaceModel.FaceType.DEMO) {
            blindFill = true;
            facesEnrolledList.clear();
        }
        Utils.sendMessage(context, TAG, "blind fill = " + blindFill + " adapter.size = " + facesEnrolledList.size() + " cursor count = " + cursor.getCount());

        if (cursor != null && cursor.getCount() > 0) {
            while (cursor.moveToNext()) {
                String frDatasetId = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.FaceEnrollment._FR_DATASET_ID));
                String name = cursor.getString(cursor.getColumnIndexOrThrow(VideoSummaryContract.FaceEnrollment._PERSON_NAME));
                Bitmap photo = FRQueryHelper.BitmapUtilities.getImage(cursor.getBlob(cursor.getColumnIndexOrThrow(VideoSummaryContract.FaceEnrollment._PHOTO)));

                //FaceModel faceItem = new FaceModel(frDatasetId, name, photo);
                DetectedFaceModel faceItem = new DetectedFaceModel(frDatasetId.getBytes(), name, photo, DetectedFaceModel.FaceType.KNOWN);


                if (blindFill) {
                    addFaceItem(faceItem);
                    adapterIndex++;
                } else {
                    if (adapterIndex < facesEnrolledList.size()) {

                        //until cursor item == adapter item, either delete or add items to make it equal
                        while (new String(faceItem.frDatasetId).compareTo(new String(facesEnrolledList.get(adapterIndex).frDatasetId)) != 0) {
                            if (faceItem.name.compareTo(facesEnrolledList.get(adapterIndex).name) > 0) {
                                delete(adapterIndex);
                            } else {
                                addFaceItemAtPos(faceItem, adapterIndex);
                            }
                        }
                    } else {
                        //if we are here means that a photo was just added, so it will be there in the cursor but not in the adapterList, so add it.
                        addFaceItem(faceItem);
                    }
                    adapterIndex++;
                }
            }
        }

        while (adapterIndex < getItemCount()) {
            delete(adapterIndex);
        }

        if ((facesEnrolledList.size() == 0) && showDemoCard) {
            //add demo face card
            addFaceItem(new DetectedFaceModel(null, null, null, DetectedFaceModel.FaceType.DEMO));
        }

    }

    public void addFaceItemAtPos(DetectedFaceModel item, final int pos) {
        facesEnrolledList.add(pos, item);
        Activity activity = (Activity) context;
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemInserted(pos);
                //If you are viewing the top of the list, then animate it to scroll up after inserting above 1st position
                //if (FacesInboxActivity.llm.findFirstCompletelyVisibleItemPosition() == 0) {
                //FacesInboxActivity.llm.scrollToPosition(pos);
                //}
            }
        });
    }

    public void addFaceItem(DetectedFaceModel item) {
        facesEnrolledList.add(item);
        Activity activity = (Activity) context;
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemInserted(getItemCount());
                //FacesInboxActivity.llm.scrollToPosition(getItemCount());
            }
        });
    }

    public void delete(final int position) {
        facesEnrolledList.remove(position);
        Activity activity = (Activity) context;
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemRemoved(position);
            }
        });

    }

    public class ViewHolderFaceCard extends RecyclerView.ViewHolder {
        ImageView photo;
        TextView name;

        ViewHolderFaceCard(View itemView) {
            super(itemView);

            photo = (ImageView) itemView.findViewById(R.id.photo);
            name = (TextView) itemView.findViewById(R.id.name);
        }
    }

    public class ViewHolderFaceDemoCard extends RecyclerView.ViewHolder {
        ImageView photo;
        TextView name;

        ViewHolderFaceDemoCard(View itemView) {
            super(itemView);

        }
    }
}




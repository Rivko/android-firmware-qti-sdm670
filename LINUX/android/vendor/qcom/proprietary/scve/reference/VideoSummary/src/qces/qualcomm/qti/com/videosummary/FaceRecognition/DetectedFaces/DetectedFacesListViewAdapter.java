/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package qces.qualcomm.qti.com.videosummary.FaceRecognition.DetectedFaces;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import qces.qualcomm.qti.com.videosummary.R;
import qces.qualcomm.qti.com.videosummary.providers.FRQueryHelper;

/**
 * Created by mithun on 8/14/15.
 */
public class DetectedFacesListViewAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private static final String TAG = DetectedFacesListViewAdapter.class.getSimpleName();
    List<DetectedFaceModel> detectedFaceList;
    private Context context;

    DetectedFacesListViewAdapter(Context context) {
        this.context = context;
        if (this.detectedFaceList == null) {
            this.detectedFaceList = new ArrayList<DetectedFaceModel>();
        }
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v;
        RecyclerView.ViewHolder holder = null;
        switch (DetectedFaceModel.FaceType.values()[viewType]) {
            case UNKNOWN_TITLE:
                v = LayoutInflater.from(parent.getContext()).inflate(R.layout.detected_face_item, parent, false);
                holder = new ViewHolderUnknownFaceTitleCard(v);
                break;
            case UNKNOWN:
                v = LayoutInflater.from(parent.getContext()).inflate(R.layout.detected_face_item, parent, false);
                holder = new ViewHolderUnknownFaceCard(v);
                break;
            case KNOWN_TITLE:
                v = LayoutInflater.from(parent.getContext()).inflate(R.layout.detected_face_item, parent, false);
                holder = new ViewHolderKnownFaceTitleCard(v);
                break;
            case KNOWN:
                v = LayoutInflater.from(parent.getContext()).inflate(R.layout.detected_face_item, parent, false);
                holder = new ViewHolderKnownFaceCard(v);
                break;
            case DEMO:
                v = LayoutInflater.from(parent.getContext()).inflate(R.layout.detected_face_item, parent, false);
                holder = new ViewHolderKnownFaceCard(v);
                break;

        }


        return holder;
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        DetectedFaceModel item = detectedFaceList.get(position);

        switch (item.faceType) {
            case UNKNOWN_TITLE:
                loadUnknownFaceTitleCard(item, (ViewHolderUnknownFaceTitleCard) holder);
                break;
            case UNKNOWN:
                loadUnknownFaceCard(item, (ViewHolderUnknownFaceCard) holder);
                break;
            case KNOWN_TITLE:
                loadKnownFaceTitleCard(item, (ViewHolderKnownFaceTitleCard) holder);
                break;
            case KNOWN:
                loadKnownFaceCard(item, (ViewHolderKnownFaceCard) holder);
                break;
            case DEMO:
                loadDemoFaceCard(item, (ViewHolderKnownFaceCard) holder);
                break;
            default:
                throw new UnsupportedOperationException("TYPE unknown " + getItemViewType(position));

        }

    }

    @Override
    public void onAttachedToRecyclerView(RecyclerView recyclerView) {
        super.onAttachedToRecyclerView(recyclerView);
    }

    @Override
    public int getItemCount() {
        return detectedFaceList.size();
    }

    @Override
    public int getItemViewType(int position) {
        return detectedFaceList.get(position).faceType.ordinal();
    }

    private void loadUnknownFaceTitleCard(DetectedFaceModel item, ViewHolderUnknownFaceTitleCard holder) {
        holder.name.setText("Unknown Faces");
        holder.photo.setVisibility(View.GONE);
        holder.enrollIcon.setVisibility(View.GONE);
    }

    private void loadUnknownFaceCard(DetectedFaceModel item, ViewHolderUnknownFaceCard holder) {
        holder.photo.setVisibility(View.VISIBLE);
        holder.enrollIcon.setVisibility(View.VISIBLE);
        holder.name.setText(item.name);
        holder.photo.setImageBitmap(item.photo);
    }

    private void loadKnownFaceTitleCard(DetectedFaceModel item, ViewHolderKnownFaceTitleCard holder) {
        holder.name.setText("Known Faces");
        holder.photo.setVisibility(View.GONE);
        holder.enrollIcon.setVisibility(View.GONE);
    }

    private void loadKnownFaceCard(DetectedFaceModel item, ViewHolderKnownFaceCard holder) {
        holder.photo.setVisibility(View.VISIBLE);
        holder.name.setText(item.name);
        holder.photo.setImageBitmap(item.photo);
        holder.enrollIcon.setVisibility(View.GONE);
    }

    private void loadDemoFaceCard(DetectedFaceModel item, ViewHolderKnownFaceCard holder) {
        holder.photo.setVisibility(View.VISIBLE);
        holder.photo.setImageBitmap(null);
        holder.name.setText(item.name);
        holder.enrollIcon.setVisibility(View.GONE);
    }

    /**
     * This method will initialize the adapter array completely if array is empty, and can be called
     * multiple times in which case it will update only the newely added or deleted item.
     */
    synchronized public void loadDetectedFaces(List<DetectedFaceModel> list) {
        Log.i(TAG, "loadRecognisedFacesInfo called, list size=" + detectedFaceList.size());
        detectedFaceList.clear();


        //DetectedFaceModel itemUnknownFaceTitle = new DetectedFaceModel("id", "unknown title", null, DetectedFaceModel.FaceType.UNKNOWN_TITLE);
        //DetectedFaceModel itemknownFaceTitle = new DetectedFaceModel("id", "known title", null, DetectedFaceModel.FaceType.KNOWN_TITLE);
        DetectedFaceModel item = null;
        if (list != null && list.size() > 0) {
            // addItem(itemUnknownFaceTitle);
            Iterator it = list.iterator();
            while (it.hasNext()) {
                item = (DetectedFaceModel) it.next();
                if (item.faceType == DetectedFaceModel.FaceType.UNKNOWN) {
                    item.name = "Unknown";
                    addItem(item);
                } else {
                    item.name = FRQueryHelper.queryEnrolledFaceName(context, item.frDatasetId);
                    if (item.name == null) {
                        item.name = "recognised face, unknown in App db";
                    }
                    addItem(item);
                }

            }

           /* addItem(itemknownFaceTitle);
            it = list.iterator();
            while (it.hasNext()) {
                item = (DetectedFaceModel) it.next();
                if (item.faceType == DetectedFaceModel.FaceType.KNOWN) {
                    item.name = FRQueryHelper.queryEnrolledFaceName(context, item.frDatasetId);
                    addItem(item);
                }
            }*/
        } else {
            addItem(new DetectedFaceModel(null, "No faces detected", null, DetectedFaceModel.FaceType.DEMO));
        }
    }

    public void addItemAtPos(DetectedFaceModel item, final int pos) {
        detectedFaceList.add(pos, item);
        ((DetectedFacesListActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemInserted(pos);
            }
        });
    }

    public void addItem(DetectedFaceModel item) {
        detectedFaceList.add(item);
        ((DetectedFacesListActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemInserted(getItemCount());
            }
        });
    }

    public void delete(final int position) {
        detectedFaceList.remove(position);
        ((DetectedFacesListActivity) context).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                notifyItemRemoved(position);
            }
        });

    }


    public class ViewHolderUnknownFaceTitleCard extends RecyclerView.ViewHolder {
        ImageView photo;
        TextView name;
        ImageView enrollIcon;

        ViewHolderUnknownFaceTitleCard(View itemView) {
            super(itemView);

            photo = (ImageView) itemView.findViewById(R.id.photo);
            name = (TextView) itemView.findViewById(R.id.name);
            enrollIcon = (ImageView) itemView.findViewById(R.id.action_enroll_face_icon);
        }
    }

    public class ViewHolderUnknownFaceCard extends RecyclerView.ViewHolder {
        ImageView photo;
        TextView name;
        ImageView enrollIcon;

        ViewHolderUnknownFaceCard(View itemView) {
            super(itemView);

            photo = (ImageView) itemView.findViewById(R.id.photo);
            name = (TextView) itemView.findViewById(R.id.name);
            enrollIcon = (ImageView) itemView.findViewById(R.id.action_enroll_face_icon);
            enrollIcon.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    ((DetectedFacesListActivity) context).addFace(getAdapterPosition(), detectedFaceList.get(getAdapterPosition()));
                }
            });
        }
    }

    public class ViewHolderKnownFaceTitleCard extends RecyclerView.ViewHolder {
        ImageView photo;
        TextView name;
        ImageView enrollIcon;

        ViewHolderKnownFaceTitleCard(View itemView) {
            super(itemView);

            photo = (ImageView) itemView.findViewById(R.id.photo);
            name = (TextView) itemView.findViewById(R.id.name);
            enrollIcon = (ImageView) itemView.findViewById(R.id.action_enroll_face_icon);
        }
    }

    public class ViewHolderKnownFaceCard extends RecyclerView.ViewHolder {
        ImageView photo;
        TextView name;
        ImageView enrollIcon;

        ViewHolderKnownFaceCard(View itemView) {
            super(itemView);

            photo = (ImageView) itemView.findViewById(R.id.photo);
            name = (TextView) itemView.findViewById(R.id.name);
            enrollIcon = (ImageView) itemView.findViewById(R.id.action_enroll_face_icon);
        }
    }
}




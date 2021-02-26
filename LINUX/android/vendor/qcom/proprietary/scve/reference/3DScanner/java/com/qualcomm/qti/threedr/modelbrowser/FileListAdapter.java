/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.modelbrowser;

import android.content.Context;
import android.os.Parcelable;
import android.os.Vibrator;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.qualcomm.qti.threedr.R;
import com.qualcomm.qti.threedr.utils.ArrayAdapter;
import com.qualcomm.qti.threedr.utils.FileListGeneratorTask;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

public class FileListAdapter extends ArrayAdapter<ModelInfo, FileListAdapter.FileViewHolder> {

    Context context;
    File folder;
    String folderExtension;
    boolean selectionMode = false;
    public List<File> selectedFolderList;
    public List<Integer> selectedModelPosition;//to keep track of all the items which are selected and later to clear the selected boolean flag on them when selection mode is cancelled

    Comparator<ModelInfo> sortByName;
    Comparator<ModelInfo> sortByDate;
    Comparator<ModelInfo> currentComparator;

    String TAG = FileListAdapter.class.getSimpleName();

    public FileListAdapter(Context context, String parentFolderStr, String extension) {
        this.context = context;
        this.folder = new File(parentFolderStr);
        this.folderExtension = extension;

        sortByName = new Comparator<ModelInfo>() {
            @Override
            public int compare(ModelInfo x, ModelInfo y) {
                return x.getName().compareToIgnoreCase(y.getName());
            }
        };

        //sort by timestamp in descending order, latest show be on the top
        sortByDate = new Comparator<ModelInfo>() {
            @Override
            public int compare(ModelInfo x, ModelInfo y) {
                return (y.createdTimestamp == x.createdTimestamp) ? 0 : ((y.createdTimestamp > x.createdTimestamp) ? 1 : -1);
            }
        };

        currentComparator = sortByDate;

        selectedFolderList = new ArrayList<File>();
        selectedModelPosition = new ArrayList<Integer>();
    }

    public void setMode(int i) {
        switch (i) {
            case 0:
                currentComparator = sortByName;
                break;
            case 1:
                currentComparator = sortByDate;
                break;
            case 2:
                Log.i(TAG, "mode changed to selectable");
                selectionMode = true;
                nSelected = 0;
                selectedFolderList.clear();
                selectedModelPosition.clear();
                break;
            default:
                selectionMode = false;
                clearSelectedItems();
        }
    }

    void clearSelectedItems() {
        Iterator it = selectedModelPosition.iterator();
        while (it.hasNext()) {
            int pos = (Integer) it.next();
            ModelInfo item = (ModelInfo) get(pos);
            item.isSelected = false;
        }
    }

    /**
     * This method will delete the existing model item in the adapter at the given position and
     * reload the model item from same folderpath and add it to the same position in the adapter.
     * which should have all the updated values expecially the new name
     *
     * @param pos
     */


    public void updateItemForRename(int pos) {
        ModelInfo item = new ModelInfo(get(pos).file.getAbsolutePath());
        super.remove(pos);
        super.add(pos, item);
        notifyItemChanged(pos);
    }

    public void remove(int pos) {
        super.remove(pos);
        notifyItemRemoved(pos);
    }

    public int getPosition(File file) {
        int retVal = -1;
        for (int i = 0; i < getItemCount(); i++) {
            if (get(i).file.compareTo(file) == 0) {
                retVal = i;
                break;
            }
        }
        return retVal;
    }


    public boolean getSelectionMode() {
        return selectionMode;
    }

    public void reload() {
        Log.i(TAG, "onReload");
        new FileListGeneratorTask(new FileListGeneratorTask.Listener() {
            @Override
            public void onResult(final String[] modelNames) {

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        clear();
                        if (currentComparator != null) {
                            if (modelNames != null) {
                                List<ModelInfo> tempList = new ArrayList<ModelInfo>();
                                for (int i = 0; i < modelNames.length; i++) {
                                    ModelInfo item = new ModelInfo(folder.getAbsolutePath(), modelNames[i]);
                                    tempList.add(item);
                                }
                                Collections.sort(tempList, currentComparator);
                                addAll(tempList);
                            }
                        }
                        ((GalleryViewActivity) context).runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                notifyDataSetChanged();
                            }
                        });

                    }
                }).start();


            }
        }).execute(new FileListGeneratorTask.Param(folder, folderExtension));
    }

    @Override
    public FileViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        Log.i(TAG, "onCreateViewHolder");
        View view = LayoutInflater.from(context).inflate(R.layout.item, null);
        return new FileViewHolder(view);
    }

    @Override
    public void onBindViewHolder(final FileViewHolder holder, final int position) {
        Log.i(TAG, "onBindViewHolder @ pos " + position);
        ModelInfo item = get(position);

        holder.modelName.setText(item.name);
        holder.imageView.setImageBitmap(item.getImage());
        String date = new SimpleDateFormat("MM/dd/yyyy").format(new Date(item.createdTimestamp));
        holder.createdTimestamp.setText(date);
        if (selectionMode) {
            holder.modelThumbChecker.setVisibility(View.VISIBLE);
        } else {
            //hide the round check box
            holder.modelThumbChecker.setVisibility(View.GONE);
        }
        holder.modelThumbChecker.setSelected(item.isSelected);
    }


    public void selectItem(ModelInfo item, int position) {
        nSelected++;
        selectedFolderList.add(item.file);
        selectedModelPosition.add(position);
        ((GalleryViewActivity) context).setNSelected(nSelected);//to change the title on the action bar to show like "n Selected"
    }

    public void unSelectItem(ModelInfo item, int position) {
        nSelected--;
        if (!selectedFolderList.remove(item.file)) {
            Log.i(TAG, "unselect removing from selectList Failed!!!");
        }
        selectedModelPosition.remove(new Integer(position));
        ((GalleryViewActivity) context).setNSelected(nSelected);//to change the title on the action bar to show like "n Selected"
    }

    public class FileViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener, View.OnLongClickListener {
        TextView modelName;
        TextView createdTimestamp;
        View cardView;
        ImageView imageView;
        ImageView modelThumbChecker;

        public FileViewHolder(View itemView) {
            super(itemView);
            modelName = (TextView) itemView.findViewById(R.id.modelName);
            createdTimestamp = (TextView) itemView.findViewById(R.id.createdTimestamp);
            cardView = itemView.findViewById(R.id.card);
            imageView = (ImageView) itemView.findViewById(R.id.modelImageView);
            modelThumbChecker = (ImageView) itemView.findViewById(R.id.modelThumbChecker);

            cardView.setOnClickListener(this);
            cardView.setOnLongClickListener(this);
        }


        @Override
        public void onClick(View v) {
            updateSelection(getAdapterPosition());
        }

        void updateSelection(int position) {
            if (selectionMode) {
                boolean selected = get(position).isSelected;
                selected = !selected;
                modelThumbChecker.setSelected(selected);
                ModelInfo item = get(position);
                item.isSelected = selected;
                if (selected) {
                    selectItem(item, position);
                } else {
                    unSelectItem(item, position);
                }

                ((GalleryViewActivity) context).enableMenuItems(nSelected > 0);

            } else {
                BrowserUtils.LauncherModelViewer(context, position, get(position));
            }
        }

        @Override
        public boolean onLongClick(View v) {

            if (!selectionMode && ((GalleryViewActivity) (context)).mainFragment != null) {
                Vibrator vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
                vibrator.vibrate(100);
                final int pos = getAdapterPosition();
                ((GalleryViewActivity) (context)).mainFragment.setMode(2);
                //To save the recyclerview scroll state and then will restore this saved state after notifydatasetchange.
                Parcelable recyclerViewScrollState = ((GalleryViewActivity) (context)).mainFragment.getRecycleViewState();
                notifyDataSetChanged();
                ((GalleryViewActivity) (context)).mainFragment.setRecycleViewState(recyclerViewScrollState);
                updateSelection(pos);
                ((GalleryViewActivity) (context)).invalidateOptionsMenu();
            } else {
                onClick(v);
            }
            return true;
        }
    }
}

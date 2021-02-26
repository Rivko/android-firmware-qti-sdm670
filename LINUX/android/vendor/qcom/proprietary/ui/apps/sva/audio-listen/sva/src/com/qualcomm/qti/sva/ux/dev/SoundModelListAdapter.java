/*
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

package com.qualcomm.qti.sva.ux.dev;

import com.qualcomm.qti.sva.*;
import com.qualcomm.qti.sva.Global;

import java.util.ArrayList;

import android.content.Context;
import android.util.Log;
import android.view.*;
import android.view.View.OnClickListener;
import android.widget.*;


public class SoundModelListAdapter extends BaseAdapter {
    private final static String TAG = "ListenLog.SoundModelListAdapter";
    private Context context;
    private ArrayList<SoundModelItem> soundModelsInList;
    private OnItemActionListener onItemLeftActionListener;
    private OnItemActionListener onItemRightActionListener;

    public SoundModelListAdapter(Context context,
                                 ArrayList<SoundModelItem> inSoundModelsArrayList,
                                 OnItemActionListener leftListener,
                                 OnItemActionListener rightListener) {
        this.context = context;
        this.soundModelsInList = inSoundModelsArrayList;
        this.onItemLeftActionListener = leftListener;
        this.onItemRightActionListener = rightListener;
    }

    @Override
    public int getCount() {
        return soundModelsInList.size();
    }

    @Override
    public Object getItem(int position) {
        return soundModelsInList.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        //Log.v(TAG, "getView: entered");
        RowViewHolder rowViewHolder;

        if (null == convertView) {
            LayoutInflater inflater = (LayoutInflater) context.getSystemService(
                    Context.LAYOUT_INFLATER_SERVICE);
            convertView = inflater.inflate(R.layout.listitem_soundmodel, null);

            rowViewHolder = new RowViewHolder();
            convertView.setTag(rowViewHolder);

            View uiLeftLayout = convertView.findViewById(R.id.listitem_soundmodel_layoutleft);
            View uiRightLayout = convertView.findViewById(R.id.listitem_soundmodel_layoutright);
            rowViewHolder.checkBox = (CheckBox)
                    convertView.findViewById(R.id.listitem_soundmodel_checked);
            rowViewHolder.uiSmName = (TextView)
                    convertView.findViewById(R.id.listitem_soundmodel_smname);
            rowViewHolder.uiImageViewOrEditSM = (ImageView)
                    convertView.findViewById(R.id.listitem_soundmodel_image);

            uiLeftLayout.setTag(rowViewHolder);
            uiRightLayout.setTag(rowViewHolder);

            uiLeftLayout.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    RowViewHolder currentRowViewHolder = (RowViewHolder) v.getTag();
                    final boolean checked = !currentRowViewHolder.item.getChecked();

                    // Find number of checked SMs in list to prohibit >1 being checked.
                    int numSmsChecked = 0;
                    for (SoundModelItem item : soundModelsInList) {
                        if (item.checked) {
                            numSmsChecked++;
                        }
                    }
                    if (checked && numSmsChecked == 1) {
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "checked and numSmsCheck == 1");
                        showSmSelectionLimitToast();
                        return;
                    } else if (checked && numSmsChecked < 1) {
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "checked and numSmsCheck < 1");
                        numSmsChecked++;
                    } else if (false == checked) {
                        Log.v(TAG, "getView: uiLeftLayout.setOnClickListener- " +
                                "unchecked");
                        numSmsChecked--;
                    }

                    // Set the item as checked/unchecked in the underlying data model.
                    for (SoundModelItem item : soundModelsInList) {
                        if (item == currentRowViewHolder.item) {
                            item.setChecked(checked);
                            Log.v(TAG, "getView: clicked keyword= " +
                                    currentRowViewHolder.item.getSoundModelName() +
                                    " set checked b/c clicked while unchecked");
                            break;
                        }
                    }

                    // Now update the view to reflect the underlying data change
                    // (checkbox (un)checked).
                    currentRowViewHolder.checkBox.setChecked(checked);
                    onItemLeftActionListener.onOptionClicked(v, currentRowViewHolder.item);
                }
            });

            uiRightLayout.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    RowViewHolder holder = (RowViewHolder) v.getTag();
                    onItemRightActionListener.onOptionClicked(v, holder.item);
                }
            });
        } else {
            rowViewHolder = (RowViewHolder) convertView.getTag();
        }

        rowViewHolder.item = soundModelsInList.get(position);
        rowViewHolder.checkBox.setChecked(rowViewHolder.item.getChecked());
        rowViewHolder.uiSmName.setText(rowViewHolder.item.getSmPrettyName());


        if (Global.getInstance().getSmRepo().isKeyphraseSoundModel(
                rowViewHolder.item.getSoundModelName())) {
            rowViewHolder.uiImageViewOrEditSM.setImageResource(R.drawable.eye);
        } else {
            rowViewHolder.uiImageViewOrEditSM.setImageResource(R.drawable.pencil);
        }
        return convertView;
    }

    public void showSmSelectionLimitToast() {
        Toast.makeText(this.context, "Cannot select more than 1 sound model at a time.",
                Toast.LENGTH_LONG).show();
    }

    public interface OnItemActionListener {
        public void onOptionClicked(View v, SoundModelItem item);
    }

    private class RowViewHolder {
        public CheckBox checkBox;
        public TextView uiSmName;
        public ImageView uiImageViewOrEditSM;
        public SoundModelItem item;
    }

    public static class SoundModelItem implements Comparable<SoundModelItem> {
        private boolean checked;
        private String soundModelName;
        private String smPrettyName;

        public SoundModelItem(boolean checked, String inSoundModelName, String inSmPrettyName) {
            this.checked = checked;
            this.soundModelName = inSoundModelName;
            this.smPrettyName = inSmPrettyName;
        }

        public boolean getChecked() {
            return checked;
        }

        public String getSoundModelName() {
            return soundModelName;
        }

        public String getSmPrettyName() {
            return smPrettyName;
        }

        public void setChecked(boolean inChecked) {
            checked = inChecked;
        }

        @Override
        public int compareTo(SoundModelItem another) {
            return this.getSoundModelName().compareToIgnoreCase(another.getSoundModelName());
        }
    }
}

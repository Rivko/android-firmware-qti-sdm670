/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.sva.ux.user;

import java.util.ArrayList;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;

import com.qualcomm.qti.sva.R;


public class KeyphraseListAdapter extends BaseAdapter {
	private final static String TAG = "ListenLog.KeyphraseListAdapter";
	private final static int NUM_MAX_KEYPHRASES = 8;
	private Context context;
	private ArrayList<KeyphraseListItem> keyphrasesInList;
    private OnItemActionListener onItemLeftActionListener;
    private OnItemActionListener onItemRightActionListener;
    int numKeyphrasesChecked = 0;

	public KeyphraseListAdapter(Context context,
	        ArrayList<KeyphraseListItem> inKeyphrasesArrayList,
	        OnItemActionListener leftListener,
	        OnItemActionListener rightListener) {
		this.context = context;
		this.keyphrasesInList = inKeyphrasesArrayList;
		this.onItemLeftActionListener = leftListener;
		this.onItemRightActionListener = rightListener;
	}

	public int getNumKeyphrasesChecked() {
        return numKeyphrasesChecked;
    }

	@Override
	public int getCount() {
		return keyphrasesInList.size();
	}

	@Override
	public Object getItem(int position) {
		return keyphrasesInList.get(position);
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		RowViewHolder rowViewHolder;

		if (null == convertView) {
			LayoutInflater inflater = (LayoutInflater)context.getSystemService(
			        Context.LAYOUT_INFLATER_SERVICE);
			convertView = inflater.inflate(R.layout.listitem_user_keyphrase, null);

			rowViewHolder = new RowViewHolder();
			convertView.setTag(rowViewHolder);

			View uiLeftLayout = convertView.findViewById(R.id.listitem_user_keyphrase_layoutleft);
            View uiRightLayout = convertView.findViewById(R.id.listitem_keyphrase_layoutright);
			rowViewHolder.uiCheckBox = (CheckBox)
			        convertView.findViewById(R.id.listitem_user_keyphrase_checkbox);
			rowViewHolder.uiKeyphraseName = (TextView)
			        convertView.findViewById(R.id.listitem_keyphrase_name);
			rowViewHolder.uiAction = (TextView)
                    convertView.findViewById(R.id.listitem_keyphrase_action);
			rowViewHolder.uiImageViewEdit = (ImageView)
                    convertView.findViewById(R.id.listitem_keyphrase_image);

			uiLeftLayout.setTag(rowViewHolder);
			uiRightLayout.setTag(rowViewHolder);

			uiLeftLayout.setOnClickListener(new OnClickListener() {
				@Override
				public void onClick(View v) {
					RowViewHolder currentRowViewHolder = (RowViewHolder)v.getTag();
                    onItemLeftActionListener.onOptionClicked(currentRowViewHolder);
				}
			});

			uiRightLayout.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    RowViewHolder holder = (RowViewHolder)v.getTag();
                    onItemRightActionListener.onOptionClicked(holder);
                }
            });
		} else {
			rowViewHolder = (RowViewHolder)convertView.getTag();
		}

		rowViewHolder.listItem = keyphrasesInList.get(position);
		rowViewHolder.uiCheckBox.setChecked(rowViewHolder.listItem.getChecked());
		rowViewHolder.uiKeyphraseName.setText(rowViewHolder.listItem.getKeyphraseName());
		rowViewHolder.uiAction.setText(rowViewHolder.listItem.getAction());

        return convertView;
	}

	// Set a keyphrase in the list checked/unchecked based on the inChecked boolean.
	public void setKeyphraseChecked (RowViewHolder inRowViewHolder, boolean inChecked) {
	    KeyphraseListItem rowItem = inRowViewHolder.listItem;
        // Set the item as (un)checked in the underlying data model.
        for (KeyphraseListItem item : keyphrasesInList) {
            if (item == rowItem) {
                item.setChecked(inChecked);
                Log.v(TAG, "setKeyphraseChecked: clicked keyphrase= " + rowItem.getKeyphraseName() +
                        ". Set checked b/c clicked while unchecked.");
                break;
            }
        }
        Log.v(TAG, "setKeyphraseChecked: Now update the view.");
        // Now update the view to reflect the underlying data change
        // (checkbox (un)checked).
        inRowViewHolder.uiCheckBox.setChecked(inChecked);
	}

	public interface OnItemActionListener {
	    public void onOptionClicked(RowViewHolder rowViewHolder);
    }

	public class RowViewHolder {
		public CheckBox uiCheckBox;
		public TextView uiKeyphraseName;
		public TextView uiAction;
		public ImageView uiImageViewEdit;
		public KeyphraseListItem listItem;
	}

	public static class KeyphraseListItem implements Comparable<KeyphraseListItem> {
		private boolean checked;
		private String keyphraseName;
		private String action;

		public KeyphraseListItem(boolean checked, String inKeyphraseName, String inAction) {
			this.checked = checked;
            this.keyphraseName = inKeyphraseName;
            this.action = inAction;
		}

		public boolean getChecked() {
			return this.checked;
		}

        public String getKeyphraseName() {
            return this.keyphraseName;
        }

        public String getAction() {
            return this.action;
        }

        public void setAction(String inAction) {
            this.action = inAction;
        }

		public void setChecked(boolean inChecked) {
			this.checked = inChecked;
		}

		@Override
		public int compareTo(KeyphraseListItem another) {
			return this.getKeyphraseName().compareToIgnoreCase(another.getKeyphraseName());
		}
	}

    public ArrayList<String> getCheckedKeyphrases() {
        Log.v(TAG, "getCheckedKeyphrases");
        ArrayList<String> checkedKeyphrassNames = new ArrayList<String>();
        for (KeyphraseListItem item : keyphrasesInList) {
            Log.v(TAG, "getCheckedKeyphrases: checking keyphrase= " + item.getKeyphraseName());
            if (item.checked) {
                checkedKeyphrassNames.add(item.getKeyphraseName());
                Log.v(TAG, "getCheckedKeyphrases: added keyphrase= " + item.getKeyphraseName());
            }
        }
        return checkedKeyphrassNames;
    }

    public void setKeyphraseAction(String keyphrase, String actionName) {
        for (KeyphraseListItem item : keyphrasesInList) {
            Log.v(TAG, "setKeyphraseAction: checking keyphrase= " + item.getKeyphraseName());
            if (item.getKeyphraseName().equals(keyphrase)) {
                item.setAction(actionName);
                Log.v(TAG, "setKeyphraseAction: for keyphrase= " + item.getKeyphraseName() +
                        " added action= " + item.getAction());
                return;
            }
        }
    }

    public void deleteKeyphrase(String inKeyphraseName) {
        for (KeyphraseListItem item : keyphrasesInList) {
            Log.v(TAG, "deleteKeyphrase: checking keyphrase= " + item.getKeyphraseName());
            if (item.getKeyphraseName().equals(inKeyphraseName)) {
                keyphrasesInList.remove(item);
                Log.v(TAG, "deleteKeyphrase: deleted keyphrase= " + item.getKeyphraseName());
                return;
            }
        }
    }
}

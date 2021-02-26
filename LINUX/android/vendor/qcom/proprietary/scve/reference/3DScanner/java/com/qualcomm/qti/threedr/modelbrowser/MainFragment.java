/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.modelbrowser;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Parcelable;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.StaggeredGridLayoutManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.ProgressBar;

import com.qualcomm.qti.threedr.R;
import com.qualcomm.qti.threedr.utils.DynamicGridLayoutManager;
import com.qualcomm.qti.threedr.utils.Helper;

public class MainFragment extends Fragment {
    private static final String TAG = MainFragment.class.getSimpleName();
    LinearLayout onEmptyView;
    RecyclerView recyclerView;
    FileListAdapter adapter;
    ProgressBar progressBar;


    public class SpacesItemDecoration extends RecyclerView.ItemDecoration {
        private int space;

        public SpacesItemDecoration(int space) {
            this.space = space;
        }

        @Override
        public void getItemOffsets(Rect outRect, View view, RecyclerView parent, RecyclerView.State state) {
            outRect.top = space + 10;
            outRect.left = space;
            outRect.right = space;
            outRect.bottom = space;
        }
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.frag_main, container, false);
        onEmptyView = (LinearLayout) rootView.findViewById(R.id.textImageViewNone);
        recyclerView = (RecyclerView) rootView.findViewById(R.id.list);
        progressBar = (ProgressBar) rootView.findViewById(R.id.progressBar);
        
        int modelWidth = Helper.dpToPx(getContext(), 160);
        int displayWidth = Helper.getMaxDisplayWidth(getContext());
        int spanCount = displayWidth / modelWidth;
        final StaggeredGridLayoutManager sglm = new DynamicGridLayoutManager(this.getActivity(), spanCount, StaggeredGridLayoutManager.VERTICAL);        
        recyclerView.addItemDecoration(new SpacesItemDecoration(2));
        recyclerView.setLayoutManager(sglm);
        adapter = new FileListAdapter(this.getActivity(), BrowserUtils.ParentFolderPath, BrowserUtils.FolderSuffix);
        adapter.registerAdapterDataObserver(new RecyclerView.AdapterDataObserver() {
            @Override
            public void onChanged() {
                super.onChanged();
                Log.i(TAG, "onChanged in Adapter");
                if (progressBar != null)
                    progressBar.setVisibility(View.GONE);
                if (adapter.getItemCount() == 0) {
                    onEmptyView.setVisibility(View.VISIBLE);
                } else {
                    onEmptyView.setVisibility(View.GONE);
                }
            }
        });

        recyclerView.setAdapter(adapter);

        return rootView;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        reload();
    }

    /**
     * The below 2 method basically helps in keeping the scroll position of the recyclerview after
     * refreshing the displayed UI data (notifydatasetchanged())
     */
    public Parcelable getRecycleViewState() {
        Parcelable recycleviewState = ((StaggeredGridLayoutManager) recyclerView.getLayoutManager()).onSaveInstanceState();
        return recycleviewState;
    }

    public void setRecycleViewState(Parcelable recycleviewState) {
        ((StaggeredGridLayoutManager) recyclerView.getLayoutManager()).onRestoreInstanceState(recycleviewState);
    }

    @Override
    public void onResume() {
        super.onResume();
        //reload();
    }

    public void reload() {
        if (progressBar != null)
            progressBar.setVisibility(View.VISIBLE);
        adapter.reload();
    }

    public void actionDelete() {
        AlertDialog.Builder deleteAlert = new AlertDialog.Builder(getContext())
                .setTitle(R.string.delete_alert_title)
                .setMessage(R.string.delete_alert_message)
                .setPositiveButton("DELETE", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        BrowserUtils.delete(getContext(), adapter.selectedFolderList);
                    }
                })
                .setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        getActivity().onBackPressed();
                    }
                });
        deleteAlert.show();

    }

    public void actionRawShare() {
        BrowserUtils.shareRawObjDataFiles(getContext(), adapter.selectedFolderList);
        setMode(-1);
        getActivity().invalidateOptionsMenu();
        reload();
    }

    public void actionVideoShare() {
        BrowserUtils.shareVideoFiles(getContext(), adapter.selectedFolderList);
        setMode(-1);
        getActivity().invalidateOptionsMenu();
        reload();
    }

    public void setMode(int mode) {
        adapter.setMode(mode);
    }

    public boolean getSelectionMode() {
        return adapter.getSelectionMode();
    }
}

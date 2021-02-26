/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.utils;

import android.support.v7.widget.RecyclerView;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public abstract class ArrayAdapter<ModelInfo, AdapterType extends RecyclerView.ViewHolder> extends RecyclerView.Adapter<AdapterType> {

    private List<ModelInfo> list;
    public int nSelected = 0;

    protected ArrayAdapter() {
        list = new ArrayList<ModelInfo>();
    }

    @Override
    public int getItemCount() {
        return list.size();
    }

    public void add(ModelInfo item) {
        list.add(item);
    }

    public void add(int pos, ModelInfo item) {
        list.add(pos, item);
    }

    public void remove(int pos) {
        list.remove(pos);
    }

    @SuppressWarnings("unchecked")
    public void addAll(List<ModelInfo> items) {
        Iterator it = items.iterator();
        while (it.hasNext()) {
            list.add((ModelInfo) it.next());
        }
    }

    public ModelInfo get(int position) {
        return list.get(position);
    }

    public void clear() {
        list.clear();
    }
}

/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.tm;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.ExpandableListView;
import android.widget.ExpandableListView.OnGroupClickListener;
import com.qualcomm.qti.tm.TaskExpandableListAdapter;

public class TaskManagerActivity extends Activity {
    private ExpandableListView mTaskListView;
    private TaskExpandableListAdapter mTaskListViewAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.task_manager);
        initTaskListView();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mTaskListViewAdapter.unregisterReceiver();
    }

    private void initTaskListView() {
        mTaskListView = (ExpandableListView) findViewById(R.id.taskList);
        mTaskListViewAdapter = new TaskExpandableListAdapter(this);
        final TaskListManager taskListManager = new TaskListManager();
        mTaskListViewAdapter.setOnTaskActionListener(taskListManager);
        mTaskListView.setAdapter(mTaskListViewAdapter);
        mTaskListView.setOnGroupClickListener(mOnGroupClickListener);
        mTaskListViewAdapter.expandGroup(null, 0);
        mTaskListViewAdapter.onGroupExpanded(0);
        mTaskListView.expandGroup(0);
        mTaskListView.setGroupIndicator(null);
    }

    private final OnGroupClickListener mOnGroupClickListener = new OnGroupClickListener() {
        public boolean onGroupClick(ExpandableListView parent, View v,
                int groupPosition, long id) {
            return true;
        }
    };

    private class TaskListManager implements TaskExpandableListAdapter.OnTaskActionListener {
        public void onTaskKilled() {
        }
        public void onTaskBroughtToFront() {
        }
    }
}

/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm.encryption;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import com.qti.csm.R;
import com.qti.csm.encryption.system.KeyManagement;
import com.qti.csm.encryption.system.ProcessInfo;
import com.qti.csm.encryption.system.SystemInfo;

import java.io.File;
import java.text.Collator;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Stack;

import static com.qti.csm.encryption.system.SystemInfo.ENCRYPT_FILE_SUFFIX;
import static com.qti.csm.encryption.system.SystemInfo.INTERNAL_PROTECTED_PATH;
import static com.qti.csm.encryption.system.SystemInfo.INTERNAL_STORAGE;
import static com.qti.csm.encryption.system.SystemInfo.MODE_DECRYPT;
import static com.qti.csm.encryption.system.SystemInfo.MODE_DEPROTECT;
import static com.qti.csm.encryption.system.SystemInfo.MODE_ENCRYPT;
import static com.qti.csm.encryption.system.SystemInfo.MODE_PROTECT;
import static com.qti.csm.encryption.system.SystemInfo.PROTECTED_PATH;
import static com.qti.csm.encryption.system.SystemInfo.SDCARD_PROTECTED_PATH;

import static com.qti.csm.encryption.system.SystemInfo.SDCARD_ROOT;
import static com.qti.csm.encryption.system.SystemInfo.ENC_CHECK;
import static com.qti.csm.encryption.system.ProcessInfo.logd;


public class FileExplorerFragment extends Fragment implements
        AdapterView.OnItemClickListener {
    private static String mRootPath = ENC_CHECK;

    private ArrayList<File> mFileList = new ArrayList<File>();
    private Stack<File> mPathStack = new Stack<File>();
    private ListView mListView;
    private TextView mCurrentDirectoryTextView;
    private FileExplorerAdapter mFileExplorerAdapter;
    private static int mFileExplorerMode = MODE_ENCRYPT;
    @Override
    public void onResume() {
        logd("");
        super.onResume();
    }

    @Override
    public void onInflate(Activity activity, AttributeSet attrs,
                          Bundle savedInstanceState) {
        logd("");
        super.onInflate(activity, attrs, savedInstanceState);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        logd("");
        super.onCreate(savedInstanceState);
        mFileExplorerAdapter = new FileExplorerAdapter(getActivity(), mFileList);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.operation,
                container, false);

        mCurrentDirectoryTextView = (TextView) view
                .findViewById(R.id.current_directory);

        mListView = (ListView) view.findViewById(R.id.file_list);
        mListView.setAdapter(mFileExplorerAdapter);
        mListView.setOnItemClickListener(this);
        mPathStack.clear();
        refreshExplorer(new File(mRootPath));
        return view;
    }
    public static final Comparator<File> ALPHA_COMPARATOR = new Comparator<File>() {
        private final Collator sCollator = Collator.getInstance();

        @Override
        public int compare(File object1, File object2) {
            return sCollator.compare(object1.getName(), object2.getName());
        }
    };

    public void refreshExplorer() {
        File rootPath = null;
        if (mPathStack.empty())
            return;
        rootPath = mPathStack.pop();
        if (rootPath == null || !rootPath.exists()) {
            return;
        }
        mPathStack.push(rootPath);
        logd("open " + rootPath.getPath() + " current stack="
                + mPathStack.size());
        File[] files = rootPath.listFiles();
        mFileList.clear();
        if (ENC_CHECK.equals(rootPath.getPath())) {
            mFileList.add(new File(INTERNAL_STORAGE));
        }
        for (int i = 0; i < files.length; i++) {
            File file = files[i];
            if (isFiltered(file)) {
                continue;
            }
            mFileList.add(file);
        }

        Collections.sort(mFileList, ALPHA_COMPARATOR);
        mFileExplorerAdapter.notifyDataSetChanged();
    }

    private boolean isFiltered(File file) {

        if (!file.exists() || !file.canRead()) {
            return true;
        }
        String filePath = file.getPath();
        String fileName = file.getName();
        String parentPath = file.getParent();
        if (ENC_CHECK.equals(parentPath)
                && (!INTERNAL_STORAGE.equals(filePath) && !SDCARD_ROOT
                .equals(filePath))) {
            return true;
        }

        switch (mFileExplorerMode) {
            case MODE_PROTECT:
                if (file.isDirectory()
                        && (INTERNAL_PROTECTED_PATH.equals(filePath))
                        || SDCARD_PROTECTED_PATH.equals(filePath)) {
                    return true;
                }
                break;
            case MODE_DEPROTECT:
                if (!PROTECTED_PATH.equals(fileName)
                        && (INTERNAL_STORAGE.equals(parentPath) || SDCARD_ROOT
                        .equals(parentPath))) {
                    return true;
                }
                break;

            case MODE_DECRYPT:
                if (file.isFile() && !fileName.endsWith(ENCRYPT_FILE_SUFFIX)) {
                    return true;
                }
                break;
            default:
                break;
        }
        return false;
    }

    public void refreshExplorer(File topPath) {
        if (topPath == null || !topPath.exists()) {
            return;
        }
        logd("open " + topPath.getPath() + " current stack="
                + mPathStack.size());
        File[] files = topPath.listFiles();
        if (files == null)
            return;
        mPathStack.push(topPath);
        mFileList.clear();
        if (ENC_CHECK.equals(topPath.getPath())) {
            mFileList.add(new File(INTERNAL_STORAGE));
        }
        for (int i = 0; i < files.length; i++) {
            File file = files[i];

            if (isFiltered(file)) {
                continue;
            }
            mFileList.add(file);
        }

        Collections.sort(mFileList, ALPHA_COMPARATOR);
        mFileExplorerAdapter.notifyDataSetChanged();
        mCurrentDirectoryTextView.setText(topPath.getPath());
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position,
                            long id) {
        File file = mFileList.get(position);
        String filePath = file.getPath();
        switch (mFileExplorerMode) {
            case MODE_ENCRYPT:

                if (file.isFile() && MainApp.getInstance().mInternalKey == null) {
                    ProcessInfo.toast(getActivity(),
                            getString(R.string.need_password_toast));

                    Intent intent = new Intent(getActivity(),
                            KeyManagement.class);
                    intent.putExtra("mode", SystemInfo.INTENT_CREATE_PASSWORD);
                    startActivity(intent);
                    return;
                }
                if (file.isFile()
                        && !file.getName().endsWith(SystemInfo.ENCRYPT_FILE_SUFFIX)) {
                    doProcess(MODE_ENCRYPT, filePath, filePath
                            + ENCRYPT_FILE_SUFFIX);
                }

                break;
            case MODE_DECRYPT:
                if (file.isFile()) {
                    if (!filePath.endsWith(ENCRYPT_FILE_SUFFIX)) {
                        return;
                    }
                    int suffixIndex = filePath.lastIndexOf(ENCRYPT_FILE_SUFFIX);
                    doProcess(MODE_DECRYPT, filePath,
                            filePath.substring(0, suffixIndex));
                }
                break;

            default:
                break;
        }
        if (file.isDirectory()) {
            refreshExplorer(mFileList.get(position));
        }
    }

    @Override
    public void onPause() {
        logd();
        super.onPause();
    }

    @Override
    public void onDestroy() {
        logd();
        super.onDestroy();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        logd();
        super.onSaveInstanceState(outState);
        outState.putString("rootPath", mPathStack.get(mPathStack.size() - 1)
                .getPath());
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            if (mPathStack.size() >= 2) {
                mPathStack.pop();
                refreshExplorer(mPathStack.pop());
                return false;
            }
        }
        return true;
    }

    private void doProcess(int mode, String srcPath, String outPath) {
        new FileProtect(getActivity(), mode).execute(srcPath, outPath);
    }

    public static void init(int fileExplorerMode, String rootPath) {
        mFileExplorerMode = fileExplorerMode;
        mRootPath = rootPath;
    }

    public static int getFileExplorerMode() {
        return mFileExplorerMode;
    }
}

/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.modelbrowser;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Environment;
import android.os.Parcelable;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.util.Log;
import android.widget.EditText;
import android.widget.Toast;

import com.qualcomm.qti.threedr.MeshViewer.MeshViewerActivity;
import com.qualcomm.qti.threedr.MeshViewer.MeshViewerUtils;
import com.qualcomm.qti.threedr.R;
import com.qualcomm.qti.threedr.utils.CreateMultipleMP4Task;
import com.qualcomm.qti.threedr.utils.DeleteFileTask;
import com.qualcomm.qti.threedr.utils.Helper;
import com.qualcomm.qti.threedr.utils.ShareMultipleFileTask;
import com.qualcomm.qti.threedr.utils.ZipHelper;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class BrowserUtils {

    public final static String EXTRA_MODELINFO = "com.qualcomm.qti.threedr.EXTRA_MODELINFO";
    public final static String EXTRA_MODELINFO_POS = "com.qualcomm.qti.threedr.EXTRA_MODELINFO_POS";
    public final static String EXTRA_LAUNCH_FROM_GALLERY = "com.qualcomm.qti.threedr.EXTRA_LAUNCH_FROM_GALLERY";

    final static String _ModelViewerPackage = "com.qualcomm.qti.threedr";
    final static String _ModelViewerActivity = "com.qualcomm.qti.threedr.MeshViewer.MeshViewerActivity";
    final static String _3DRPackage = "com.qualcomm.qti.threedr";
    final static String _3DRActivity = "com.qualcomm.qti.threedr.MainActivity";

    public final static String FolderSuffix = "_done";
    public final static String ParentFolderPath = Environment.getExternalStorageDirectory().toString() + "/3DR";
    private static final String TAG = BrowserUtils.class.getSimpleName();
    public static final Integer PROGRESS_CODE_INCREMENT_MODEL = -1;

    private static int textureFileCount;

    /**
     * FileNameFilter used to pick only necessary files from the model directory , i.e <model_name>_3DR.obj and <model_name>_color_(.*).jpg's
     */
    public static FilenameFilter rawObjDataFileFilter = new FilenameFilter() {
        @Override
        public boolean accept(File dir, String filename) {
            boolean result = false;
            String modelName = dir.getName().replace(FolderSuffix, "");

            if ((filename.matches("metadata.txt") || filename.matches("output(.*).obj") || filename.matches("output_(.*).jpg") || filename.matches("output(.*).mtl")) && !filename.matches("(.*).bobj")) {
                result = true;
            }
            return result;
        }
    };

    /**
     * FileNameFilter used to pick only necessary files from the model directory , i.e <model_name>.*.mp4
     */
    public static FilenameFilter videoFileFilter = new FilenameFilter() {
        @Override
        public boolean accept(File dir, String filename) {
            boolean result = false;

            if (filename.matches("(.*)_3DR.mp4")) {
                result = true;
            }
            return result;
        }
    };

    public static void LauncherModelViewer(Context context, int pos, ModelInfo model) {


        Intent intent = new Intent()
                .setClassName(_ModelViewerPackage, _ModelViewerActivity)
                .putExtra(EXTRA_MODELINFO, model)
                .putExtra(EXTRA_MODELINFO_POS, pos)
                .putExtra(EXTRA_LAUNCH_FROM_GALLERY, true);


        if (context instanceof GalleryViewActivity) {
            ((GalleryViewActivity) context).startViewer(intent);
        } else {
            //TODO: to be removed as perviously this was also being called from even contextmenudialog but not anymore
            context.startActivity(intent);
        }
    }

    public static void Launch3DR(Context context) {
        Helper.LaunchActivity(context, _3DRPackage, _3DRActivity);
    }

    public static String ReadAssetFile(Context context, String fileName) {
        String result = null;
        StringBuilder stringBuilder = null;
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(context.getAssets().open(fileName)));

            stringBuilder = new StringBuilder();

            while (true) {
                String cLine = reader.readLine();
                if (cLine == null) break;
                stringBuilder.append(cLine);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (stringBuilder != null) {
            result = stringBuilder.toString();
        }

        return result;
    }

    public static String ReadFile(String fileName) {
        String result = null;
        StringBuilder stringBuilder = null;
        try {
            BufferedReader reader = new BufferedReader(new FileReader(fileName));

            stringBuilder = new StringBuilder();

            while (true) {
                String cLine = reader.readLine();
                if (cLine == null) break;
                stringBuilder.append(cLine);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (stringBuilder != null) {
            result = stringBuilder.toString();
        }

        return result;
    }
/*

    public static void ShareFile(Context context, File file) {
        ArrayList<Uri> uris = new ArrayList<Uri>();
        uris.add(Uri.fromFile(file));
        Log.e("X3", "zipFile = " + file.getAbsolutePath());
        String screenshotFilename = file.getAbsolutePath().replace(".roa", ".jpg");
        screenshotFilename = screenshotFilename.replace(".zip", ".jpg");
        screenshotFilename = screenshotFilename.replace(".mp4", ".jpg");
        Log.e("X3", "screenshotFile = " + screenshotFilename);
        File screenshotFile = new File(screenshotFilename);
        if (screenshotFile.exists()) {
            uris.add(Uri.fromFile(screenshotFile));
        } else {
            Log.e("X3", "screenshotFile doesn't exist - " + screenshotFile.getAbsolutePath());
        }
        EmailComposer.Send(context, file.getName(), uris);
    }


    public static void ShareFile(Context context, String fileStr) {
        ShareFile(context, new File(fileStr));
    }
*/

    /**
     * will zip and share only raw obj files i.e <model_name>.*.obj, <model_name>_color_.8.jpg's
     *
     * @param context
     * @param selectedFolderList
     */
    public static void shareRawObjDataFiles(final Context context, final List<File> selectedFolderList) {
        shareFile(context, selectedFolderList, rawObjDataFileFilter);
    }

    /**
     * will zip and share only the video file i.e .mp4
     *
     * @param context
     * @param selectedFolderList
     */
    public static void shareVideoFiles(final Context context, final List<File> selectedFolderList) {

        final ArrayList<ModelInfo> mp4CreateList = new ArrayList<>();

        for (File f : selectedFolderList) {
            ModelInfo modelInfo = new ModelInfo(f.getParent(), f.getName());
            if (!modelInfo.checkMP4Created())
                mp4CreateList.add(modelInfo);
        }

        new CreateMultipleMP4Task(new CreateMultipleMP4Task.Listener() {
            public int currentModel;
            ProgressDialog progressDialog;

            @Override
            public void onPreExecute() {
                progressDialog = new ProgressDialog(context);
                progressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);

                if (mp4CreateList.size() > 1)
                    progressDialog.setTitle(R.string.creating_mp4s_title);
                else
                    progressDialog.setTitle(R.string.creating_mp4_title);

                currentModel = 0;
                progressDialog.setMessage(createMsgString(currentModel));
                progressDialog.setIndeterminate(true);
                progressDialog.setMax(MeshViewerUtils.TOTAL_FRAMES);
                progressDialog.setCancelable(false);
                progressDialog.show();
            }

            /**
             * Constructs a string like "0 out of 2 models completed"
             */
            private String createMsgString(int modelCount) {
                return modelCount + " " + context.getString(R.string.creating_mp4_message_part1) + " "
                        + mp4CreateList.size() + " " + context.getString(R.string.creating_mp4_message_part2);
            }

            @Override
            public void onProgressUpdate(int progress) {
                if (progress == PROGRESS_CODE_INCREMENT_MODEL) {
                    progressDialog.setIndeterminate(true);
                    progressDialog.setMessage(createMsgString(++currentModel));
                } else {
                    progressDialog.setIndeterminate(false);
                    progressDialog.setProgress(progress);
                }
            }

            @Override
            public void onResult() {
                progressDialog.dismiss();
                EmailComposer.Send(context, "3d_models", getVideoUrisFromFolderList(selectedFolderList));
            }

        }).execute(new CreateMultipleMP4Task.Param(mp4CreateList));

    }

    public static ArrayList<Uri> getVideoUrisFromFolderList(List<File> selectedFolderList) {

        ArrayList<Uri> uris = new ArrayList<Uri>();
        for (File file : selectedFolderList) {
            //list all the .mp4 (video files) under the given folder, usually there should be only one video file for each model folder
            File[] videofiles = file.listFiles(videoFileFilter);
            for (File videoFile : videofiles) {

                //check if .mp4 file has the same name as model name, if not preform a rename
                File modelFolder = new File(videoFile.getParent());
                ModelInfo modelInfo = new ModelInfo(modelFolder.getParent(), modelFolder.getName());
                File newVideoFileFullPath = new File(modelInfo.getFile().getAbsolutePath() + "/" +
                        modelInfo.getName() + MeshViewerUtils.MP4_EXTENSION);

                if (!videoFile.getName().equals(newVideoFileFullPath.getName())) {
                    videoFile.renameTo(newVideoFileFullPath);
                }

                //add the renamed file for sharing
                uris.add(Uri.fromFile(newVideoFileFullPath));
                Log.e(TAG, "Adding video file " + newVideoFileFullPath.getAbsolutePath() + " for sharing");
            }
        }
        return uris;
    }

    public static void shareFile(final Context context, final List<File> selectedFolderList, FilenameFilter filter) {
        ArrayList<Uri> uris;

        new ShareMultipleFileTask(new ShareMultipleFileTask.Listener() {
            ProgressDialog progressDialog;

            @Override
            public void onPreExecute() {
                progressDialog = new ProgressDialog(context);
                progressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
                progressDialog.setTitle("Zipping files, please wait!");
                progressDialog.setProgress(0);
                progressDialog.setMax(selectedFolderList.size());
                progressDialog.setCancelable(false);
                progressDialog.show();
                if (selectedFolderList.size() == 1) {//if only one model to zip just show indeterminate progress bar
                    progressDialog.setIndeterminate(true);
                }
            }

            @Override
            public void onProgressUpdate(int numProcessed) {
                progressDialog.setProgress(numProcessed);
            }

            @Override
            public void onResult(ArrayList<Uri> uris) {
                Log.i(TAG, "zipping completed");
                progressDialog.dismiss();
                EmailComposer.Send(context, "3d_models", uris);
            }

        }).execute(new ShareMultipleFileTask.Param(selectedFolderList, filter));

    }


    public static boolean AllGranted(int[] grantResults) {
        for (int grant : grantResults) {
            if (grant == PackageManager.PERMISSION_DENIED)
                return false;
        }

        return true;
    }

    public static void Notify(final Context context, final String msg) {
        Log.e("X3", " Notify " + msg);
        Activity activity = (Activity) (context);
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(context, msg, Toast.LENGTH_LONG).show();
            }
        });

        Log.e("X3", " Notify " + msg);
    }

    private static boolean unzipFiles(String fullPath) {
        InputStream fis;
        ZipInputStream zis;
        textureFileCount = 0;
        String directory = fullPath.substring(0, fullPath.lastIndexOf(File.separator)) + "/";
        try {
            String filename;
            fis = new FileInputStream(fullPath);
            zis = new ZipInputStream(new BufferedInputStream(fis));
            ZipEntry zipEntry;
            byte[] buffer = new byte[1024];
            int count;

            while ((zipEntry = zis.getNextEntry()) != null) {
                filename = zipEntry.getName();
                if (filename.endsWith(".jpg"))
                    textureFileCount++;
                if (zipEntry.isDirectory()) {
                    File fmd = new File(directory + filename);
                    fmd.mkdirs();
                    continue;
                }
                FileOutputStream fout = new FileOutputStream(directory + filename);
                while ((count = zis.read(buffer)) != -1) {
                    fout.write(buffer, 0, count);
                }
                fout.close();
                zis.closeEntry();
            }
            zis.close();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }

        return true;
    }


    /**
     * provider:wsnell
     *
     * @param modelDirName
     * @return
     */
    // Zip model (zip the model dir) into a single zipfile (type .Q3d), (note so far does not support recursive folders)
    public static String zipModel(File modelDirName, FilenameFilter filter) {
        boolean result = false;
        String zipFilename = modelDirName.getParent() + "/temp/" + modelDirName.getName().replace(BrowserUtils.FolderSuffix, ".q3dr");
        result = zipModel(modelDirName, zipFilename, filter);

        if (!result) {
            zipFilename = null;
        }
        return zipFilename;
    }

    //provider:wsnell
    // Zip model (zip the model dir) into a single zipfile (type .Q3d), (note so far does not support recursive folders)
    public static boolean zipModel(File modelDirName, String zipFilename, FilenameFilter filter) {
        boolean result = false;

        String directory = modelDirName.getAbsolutePath();
        result = ZipHelper.zip(directory, zipFilename, filter);

        return result;
    }

    public static void rename(final Context context, final ModelInfo modelInfo) {
        final EditText inputName = new EditText(context);
        inputName.setText(modelInfo.getName());
        inputName.setSelectAllOnFocus(true);
        inputName.setInputType(InputType.TYPE_TEXT_FLAG_CAP_WORDS);
        final AlertDialog.Builder renameAlert = new AlertDialog.Builder(context)
                .setTitle("Rename!!!")
                .setMessage("Enter new name for this model")
                .setView(inputName)
                .setPositiveButton("RENAME", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        String newName = inputName.getText().toString();
                        if (!newName.equals(modelInfo.getName())) {
                            modelInfo.setName(newName);
                            if (context instanceof GalleryViewActivity) {
                                ((GalleryViewActivity) context).mainFragment.reload();
                            } else if (context instanceof MeshViewerActivity) {
                                ((MeshViewerActivity) context).setTitle(newName);
                            }
                        }
                    }
                })
                .setNegativeButton("CANCEL", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
        final AlertDialog dialog = renameAlert.create();
        dialog.show();
        if (inputName.getText().length() == 0) {
            dialog.getButton(DialogInterface.BUTTON_POSITIVE).setEnabled(false);
        }

        inputName.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {
                if (s.length() > 0) {
                    dialog.getButton(DialogInterface.BUTTON_POSITIVE).setEnabled(true);
                } else {
                    dialog.getButton(DialogInterface.BUTTON_POSITIVE).setEnabled(false);
                }
            }
        });

    }

    /**
     * Will rename modelDir directory and all files under modelDir with the newName retaining its files perfix, postfix and extension
     *
     * @param modelDir
     * @param newName
     */
    public static void rename(final File modelDir, String newName) {
        File[] files = modelDir.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                boolean retVal = false;
                String modelName = modelDir.getName().replace(BrowserUtils.FolderSuffix, "");
                if (filename.startsWith(modelName)) {
                    retVal = true;
                }
                return retVal;
            }
        });

        String modelName = modelDir.getName().replace(BrowserUtils.FolderSuffix, "");
        for (File file : files) {
            String fileName = file.getName();
            fileName = fileName.replace(modelName, newName);
            file.renameTo(new File(modelDir.getAbsolutePath() + "/" + fileName));
        }
        String dirName = modelDir.getName();
        dirName = dirName.replace(modelName, newName);
        modelDir.renameTo(new File(modelDir.getParent() + "/" + dirName));
    }

    public static void delete(final Context context, final List<File> folderList) {
        new DeleteFileTask(new DeleteFileTask.Listener() {
            ProgressDialog progressDialog;
            Parcelable recyclerViewScrollState;

            @Override
            public void onPreExecute() {
                progressDialog = new ProgressDialog(context);
                progressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
                progressDialog.setTitle(R.string.delete_progress_title);
                progressDialog.setProgress(0);
                progressDialog.setMax(folderList.size());
                progressDialog.setCancelable(false);
                progressDialog.show();
                if (context instanceof GalleryViewActivity) {
                    recyclerViewScrollState = ((GalleryViewActivity) (context)).mainFragment.getRecycleViewState();
                }
            }

            @Override
            public void onProgressUpdate(int numDeleted) {
                progressDialog.setProgress(numDeleted);
                if (context instanceof GalleryViewActivity) {
                    int pos = ((GalleryViewActivity) context).mainFragment.adapter.getPosition(folderList.get(numDeleted - 1));
                    ((GalleryViewActivity) context).mainFragment.adapter.remove(pos);
                    ((GalleryViewActivity) context).mainFragment.adapter.notifyItemRemoved(pos);
                    ((GalleryViewActivity) (context)).mainFragment.setRecycleViewState(recyclerViewScrollState);
                }
            }

            @Override
            public void onResult() {
                Log.i(TAG, "Delete completed");
                progressDialog.dismiss();
                if (context instanceof GalleryViewActivity) {
                    ((GalleryViewActivity) context).mainFragment.adapter.selectedModelPosition.clear();
                    ((GalleryViewActivity) context).mainFragment.adapter.selectedFolderList.clear();
                    ((GalleryViewActivity) context).mainFragment.adapter.setMode(-1);
                    ((GalleryViewActivity) context).invalidateOptionsMenu();
                    ((GalleryViewActivity) context).mainFragment.adapter.notifyDataSetChanged();
                    ((GalleryViewActivity) (context)).mainFragment.setRecycleViewState(recyclerViewScrollState);
                } else if (context instanceof MeshViewerActivity) {
                    ((MeshViewerActivity) context).onBackPressed();
                }
            }
        }).execute(new DeleteFileTask.Param(folderList));
    }


    /**
     * Recursively deletes files and sub directiries
     *
     * @param dirOrFile
     */
    public static void delete(File dirOrFile) {
        if (dirOrFile.isDirectory()) {
            for (File file : dirOrFile.listFiles()) {
                delete(file);
            }
        }
        dirOrFile.delete();
    }


}

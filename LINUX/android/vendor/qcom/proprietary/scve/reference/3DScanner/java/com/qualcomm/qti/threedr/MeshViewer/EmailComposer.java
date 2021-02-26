/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.threedr.MeshViewer;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.text.Html;
import android.text.Spanned;

import java.util.ArrayList;

public class EmailComposer {

    static String htmlSource;
    static boolean initialized;

    public static void initialize(Context context)
    {
        htmlSource = MeshViewerUtils.ReadFile("/sdcard/3DR/email.html");
        if( htmlSource == null ) {
            htmlSource = MeshViewerUtils.ReadAssetFile(context, "email.html");
        }
        initialized = true;
    }

    public static Spanned getText(Context context, String fileStr)
    {
        Spanned result = null;

        if( !initialized )
        {
            initialize(context);
        }

        if( htmlSource != null ) {
            String str = htmlSource.replace("&lt;filename&gt;", fileStr);
            result = Html.fromHtml(str);
        }

        return result;
    }

    public static void Send(Context context, String fileName, ArrayList<Uri> uris)
    {
        if( !initialized )
        {
            initialize(context);
        }

        Intent intent = new Intent(Intent.ACTION_SEND_MULTIPLE);
        intent.setType("*/*");
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
        intent.putExtra(Intent.EXTRA_SUBJECT, "Snapdragon 820 Prototype Structured Light 3D Scanning Demo");
        Spanned body = EmailComposer.getText(context, fileName);
        if( body != null ) {
            intent.putExtra(Intent.EXTRA_TEXT, body);
        }



        intent.putParcelableArrayListExtra(Intent.EXTRA_STREAM, uris);
        //intent.putExtra(Intent.EXTRA_STREAM, uri);

        context.startActivity(intent);
    }
}

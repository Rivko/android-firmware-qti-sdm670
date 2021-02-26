/*
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qti.csm;

import android.content.Intent;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.util.Log;

import com.qti.csm.antitheft.InitCheck;
import com.qti.csm.encryption.EncryptOption;
import com.qti.csm.permission.AutoStartActivity;
import com.qti.csm.permission.BgControlActivity;
import com.qti.csm.permission.PermissionActivity;
import com.qti.csm.securityLog.LogDetailsActivity;
import com.qti.csm.utils.L;
import com.qti.csm.utils.UtilsSystem;

public class EnterSet extends AppCompatActivity {
    private Button fea_file = null;
    private Button fea_antit = null;
    private Button btnPermission = null;
    private Button btnAuto = null;
    private Button fea_log = null;
    private Button btnBgControl = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.platf_info);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        fea_file = (Button) findViewById(R.id.filePro);
        fea_file.setOnClickListener(new View.OnClickListener() {
                                        @Override
                                        public void onClick(View v) {
                                            try {
                                                Intent intent = new Intent();
                                                intent.setClass(EnterSet.this, EncryptOption.class);
                                                startActivity(intent);
                                            } catch (Exception e) {
                                                e.printStackTrace();
                                            }
                                        }
                                    }
        );
        FloatingActionButton ui_info = (FloatingActionButton) findViewById(R.id.notice);
        ui_info.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view,R.string.pop_info, Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        fea_antit = (Button) findViewById(R.id.antit);
        fea_antit.setOnClickListener(new View.OnClickListener() {
                                         @Override
                                         public void onClick(View v) {
                                             try {
                                                 Intent intent = new Intent();
                                                 intent.setClass(EnterSet.this, InitCheck.class);
                                                 startActivity(intent);
                                             } catch (Exception e) {
                                                 e.printStackTrace();
                                             }
                                         }
                                     }
        );
        fea_log = (Button) findViewById(R.id.log);
        fea_log.setOnClickListener(new View.OnClickListener() {
                                         @Override
                                         public void onClick(View v) {
                                             try {
                                                 Intent intent = new Intent();
                                                 intent.setClass(EnterSet.this, LogDetailsActivity.class);
                                                 startActivity(intent);
                                             } catch (Exception e) {
                                                 e.printStackTrace();
                                             }
                                         }
                                     }
        );

        btnPermission = (Button) findViewById(R.id.permission);
        btnPermission.setOnClickListener(new View.OnClickListener() {
                                         @Override
                                         public void onClick(View v) {
                                             try {
                                                 Intent intent = new Intent();
                                                 intent.setClass(EnterSet.this, PermissionActivity.class);
                                                 startActivity(intent);
                                             } catch (Exception e) {
                                                 e.printStackTrace();
                                             }
                                         }
                                     }
        );
        btnAuto = (Button) findViewById(R.id.auto);
        btnAuto.setOnClickListener(new View.OnClickListener() {
                                         @Override
                                         public void onClick(View v) {
                                             try {
                                                 Intent intent = new Intent();
                                                 intent.setClass(EnterSet.this, AutoStartActivity.class);
                                                 startActivity(intent);
                                             } catch (Exception e) {
                                                 e.printStackTrace();
                                             }
                                         }
                                     }
        );
        btnBgControl = (Button) findViewById(R.id.bgControl);
        if (UtilsSystem.powerTest()) {
            btnBgControl.setOnClickListener(new View.OnClickListener() {
                                                @Override
                                                public void onClick(View v) {
                                                    try {
                                                        Intent intent = new Intent();
                                                        intent.setClass(EnterSet.this, BgControlActivity.class);
                                                        startActivity(intent);
                                                    } catch (Exception e) {
                                                        e.printStackTrace();
                                                    }
                                                }
                                            }
            );
        } else {
            ViewGroup parentView = (ViewGroup)btnBgControl.getParent();
            parentView.removeView(btnBgControl);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.conf_set, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        return super.onOptionsItemSelected(item);
    }
}

/*============================================================================
@file GraphView.java

@brief
Android View for the sensor graph.  Handles and determines look of graph data.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
package com.qualcomm.qti.sensors.ui.graph;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Path.Direction;
import android.graphics.RectF;
import android.graphics.Typeface;
import android.util.Log;
import android.view.View;

import com.qualcomm.qti.sensors.core.stream.SensorSample;

public class GraphView extends View {
  static private int colors[] = {Color.BLUE, Color.GREEN, Color.RED, Color.BLACK, Color.MAGENTA, Color.CYAN };

  private Paint paint;
  private GraphAdapter graphAdapter;
  private int sizeX, sizeY;
  private RectF bounds;
  private Path sensorEvents[];
  private Path path;
  private Matrix matrix;

  public GraphView(Context context, GraphAdapter graphAdapter) {
    super(context);

    this.path = new Path();
    this.sensorEvents = new Path[GraphAdapter.maxAxisCnt];
    for(int i = 0; i < GraphAdapter.maxAxisCnt; i++)
      sensorEvents[i] = new Path();

    this.bounds = new RectF();
    this.paint = new Paint();
    this.matrix = new Matrix();

    this.paint.setColor(Color.BLACK);
    this.paint.setAntiAlias(true);
    this.paint.setDither(false);
    this.paint.setStyle(Paint.Style.STROKE);
    this.paint.setStrokeJoin(Paint.Join.ROUND);
    this.paint.setStrokeWidth(5);
    this.paint.setTypeface(Typeface.MONOSPACE);
    this.paint.setTextSize(50);

    this.sizeX = 100;
    this.sizeY = 100;
    this.graphAdapter = graphAdapter;
  }

  @Override
  protected void onSizeChanged(int w, int h, int oldw, int oldh) {
    this.sizeX = w;
    this.sizeY = h;
    Log.d(SensorGraphActivity.TAG, "X Size: + " + sizeX + " Y Size: + " + sizeY);
    super.onSizeChanged(w, h, oldw, oldh);
  }

  @Override
  public void onDraw(Canvas canvas) {
    if(!this.graphAdapter.sensorData().isEmpty())
    {
      float top0 = Float.POSITIVE_INFINITY, top1 = Float.POSITIVE_INFINITY,
            bottom0 = Float.NEGATIVE_INFINITY, bottom1 = Float.NEGATIVE_INFINITY,
            left0 = Float.POSITIVE_INFINITY, left1 = Float.POSITIVE_INFINITY,
            right0 = Float.NEGATIVE_INFINITY, right1 = Float.NEGATIVE_INFINITY;

      for(int i = 0; i < this.graphAdapter.axisCnt(); i++)
        this.sensorEvents[i].rewind();

      SensorSample prevSample = null;
      for(SensorSample sample : this.graphAdapter.sensorData()) {
        float timeJump = sample.timestamp() - ((null != prevSample) ? prevSample.timestamp() : 0);

        for(int i = 0; i < this.graphAdapter.axisCnt(); i++) {
          if(null == prevSample || timeJump > 200000000) {
            this.sensorEvents[i].moveTo(sample.timestamp() / 1000000, -sample.values()[i]);
            if(null != prevSample)
              this.sensorEvents[i].addCircle(sample.timestamp() / 1000000, -sample.values()[i], 1, Direction.CW);
          }
          else
            this.sensorEvents[i].lineTo(sample.timestamp() / 1000000, -sample.values()[i]);
        }
        prevSample = sample;
      }

      // Determine maximum bounds of data from all axis
      for(int j = 0; j < this.graphAdapter.axisCnt(); j++) {
        this.sensorEvents[j].computeBounds(bounds, true);
        if(this.bounds.top < top0)
          top0 = this.bounds.top;
        if(this.bounds.bottom > bottom0)
          bottom0 = this.bounds.bottom;
        if(this.bounds.right > right0)
          right0 = this.bounds.right;
        if(this.bounds.left < left0)
          left0 = this.bounds.left;
      }

      // Transform and draw data from each axis
      for(int j = 0; j < this.graphAdapter.axisCnt(); j++) {
        this.matrix.reset();
        this.matrix.preTranslate(-left0, 0);
        this.matrix.preScale(1.0f, (this.sizeY * 0.8F) / (bottom0 - top0));
        this.matrix.postScale((this.sizeX * 0.9F) / (right0 - left0), 1.0f);
        this.matrix.postTranslate(0, -top0 * (this.sizeY * 0.8F) / (bottom0 - top0) + 50);

        this.sensorEvents[j].transform(this.matrix, this.path);

        this.path.computeBounds(this.bounds, true);
        if(this.bounds.top < top1)
          top1 = this.bounds.top;
        if(this.bounds.bottom > bottom1)
          bottom1 = this.bounds.bottom;
        if(this.bounds.right > right1)
          right1 = this.bounds.right;
        if(this.bounds.left < left1)
          left1 = this.bounds.left;

        this.paint.setColor(GraphView.colors[j]);
        canvas.drawPath(this.path, this.paint);
      }

      // Draw max/min values
      this.paint.setColor(Color.BLACK);
      canvas.drawText(Float.toString(-top0), left1, top1, this.paint);
      canvas.drawText(Float.toString(-bottom0), left1, bottom1, this.paint);

      // Draw all other information
      canvas.drawText(this.graphAdapter.sensor().toString(), left1, this.sizeY - 150, this.paint);

      for(int j = 0; j < this.graphAdapter.axisCnt(); j++)
      {
        SensorSample lastSample = this.graphAdapter.sensorData().get(this.graphAdapter.sensorData().size() - 1);
        int row = 2 - j / 3;
        float yLoc = this.sizeY - 50 * row;
        float xLoc = this.sizeX * .3f * (j % 3);

        this.paint.setColor(GraphView.colors[j]);
        canvas.drawText("[" + j + "]" + Float.toString(lastSample.values()[j]), xLoc, yLoc, this.paint);
      }
    }
  }
}

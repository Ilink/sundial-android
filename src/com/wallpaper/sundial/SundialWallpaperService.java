package com.wallpaper.sundial;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.os.Handler;
import android.service.wallpaper.WallpaperService;
import android.view.SurfaceHolder;
import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import java.util.List;
import java.util.Calendar;
import java.util.TimeZone;
import android.util.Log;
// import java.lang.Math;

public class SundialWallpaperService extends WallpaperService {

    @Override
    public Engine onCreateEngine() {
        return new SundialWallpaperEngine();
    }

    public native double[]  getSunPos(double lat, double lng);
    public native double[]  getMoonPos(int hour, int min, int sec, double lat, double lng);

    static {
        System.loadLibrary("sundial");
    }

    public double[] test = getSunPos(100.0, 100.0);
    
    private class SundialWallpaperEngine extends Engine {
        private boolean mVisible = false;
        private final Handler mHandler = new Handler();
        private double i = 0;

        // private LocationManager mgr = null;
        private LocationManager mgr = (LocationManager) getSystemService(LOCATION_SERVICE);
        private LocationListener onLocationChange = new LocationListener() {
            public void onLocationChanged(Location location) {
                update(location);
            }
            
            public void onProviderDisabled(String provider) {
              // required for interface, not used
            }
            
            public void onProviderEnabled(String provider) {
              // required for interface, not used
            }
            
            public void onStatusChanged(String provider, int status,
                                          Bundle extras) {
              // required for interface, not used
            }
        };

        private void update(Location location){
            double lng, lat;
            if(location != null){
                lat = location.getLatitude();
                lng = location.getLongitude();
            } else {
                lat = 37.871592;
                lng = -122.2937;
            }
            
            double[] sunPos = getSunPos(lat, lng);
        }

        @Override
        public void onCreate(SurfaceHolder surfaceHolder)
        {
            mgr.requestLocationUpdates(LocationManager.GPS_PROVIDER,
                                       3600000, 1000,
                                       onLocationChange);
        }

        private final Runnable mUpdateDisplay = new Runnable() {
        @Override
        public void run() {
            draw();
        }};
    
        private void draw() {
            
           SurfaceHolder holder = getSurfaceHolder();
           Canvas c = null;
           try {
              c = holder.lockCanvas();
              if (c != null) {
                double lng, lat;
                lat = 37.871592;
                lng = -122.2937;

                Calendar now = Calendar.getInstance(TimeZone.getDefault());
                int sec = now.get(Calendar.SECOND);
                int min = now.get(Calendar.MINUTE);
                int hour = now.get(Calendar.HOUR_OF_DAY); // 24h format
                hour = (int) i;

                double[] sunPos = getSunPos(lat, lng);
                double[] moonPos = getMoonPos(hour, min, sec, lat, lng);

                Log.v("com.wallpaper.sundial", "android log: hour " + hour + " min "+ min);

                Paint p = new Paint();
                p.setTextSize(20);
                p.setAntiAlias(true);
                double midpoint = c.getWidth() / 2.0;

                // int x = (int) Math.round(sunPos[1] / 360.0 * c.getWidth());
                // int _y = (int) Math.round(sunPos[0] / 90.0 * c.getHeight());

                int x = (int) Math.round(moonPos[1] / 360.0 * c.getWidth());
                int _y = (int) Math.round(moonPos[0] / 90.0 * c.getHeight());

                // Log.v("com.wallpaper.sundial", "android log: alt " + moonPos[0] + " azi "+ moonPos[1] + "i: "+i);

                // int _y = (int) Math.round(sunPos[0]);
                int y = c.getHeight() - _y;
                // y = 300;

                // String text = x+", "+y+", i: "+i;
                String text = moonPos[0]+", "+moonPos[1]+", i: "+i;
                // 0 = altitude, 1 = azimuth
                // text = moonPos[0]+"  "+moonPos[1];
                text = "O";

                p.setColor(Color.BLACK);
                c.drawRect(0, 0, c.getWidth(), c.getHeight(), p);
                p.setColor(Color.WHITE);
                c.drawText(text, x, y, p);
                // c.drawText(text, 0, 300, p);
              }
           } finally {
              if (c != null)
                 holder.unlockCanvasAndPost(c);
           }
           mHandler.removeCallbacks(mUpdateDisplay);
           if (mVisible) {
               mHandler.postDelayed(mUpdateDisplay, 100);
           }
           i+=0.05;
           if( i > 24) i = 0;
        }
        
        @Override
        public void onVisibilityChanged(boolean visible) {
            mVisible = visible;
            if (visible) {
                draw();
            } else {
                mHandler.removeCallbacks(mUpdateDisplay);
            }
        }
        
         @Override
          public void onSurfaceChanged(SurfaceHolder holder, int format, int width, int height) {
             draw();
          }
        
        @Override
        public void onSurfaceDestroyed(SurfaceHolder holder) {
            super.onSurfaceDestroyed(holder);
            mVisible = false;
            mHandler.removeCallbacks(mUpdateDisplay);
        }
        
        @Override
        public void onDestroy() {
             super.onDestroy();
             mVisible = false;
             mHandler.removeCallbacks(mUpdateDisplay);
        }
    }    
}
package com.wallpaper.sundial;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import android.content.Context;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import java.util.List;

public class Sundial extends Activity
{
    private TextView tv = null;
    private LocationManager mgr = null;
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

        tv.setText( "Elevation: " + sunPos[0] + 
                    "\nAzimuth: " + sunPos[1]);
        setContentView(tv);
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        mgr = (LocationManager) getSystemService(LOCATION_SERVICE);
        tv = new TextView(this);
        mgr.requestLocationUpdates(LocationManager.GPS_PROVIDER,
                                   3600000, 1000,
                                   onLocationChange);
        // lat = 37.871592;
        // lng = -122.2937;
    }

    @Override
    public void onResume() {
        super.onResume();

        
    }

    public native int stringFromJNI();
    public native double[]  getSunPos(double lat, double lng);

    static {
        System.loadLibrary("sundial");
    }
}

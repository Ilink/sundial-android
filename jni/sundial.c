#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include <time.h>
#include "types.h"
#include "sun.h"
#include "moon.h"

double get_jd_wrapper(){
	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = localtime(&rawtime);

	double tz = -8.0;
	int year = 1900 + ptm->tm_year;
	int day = ptm->tm_mday;
	int month = ptm->tm_mon+1;
	return get_jd(year, month, day);
}

double get_time(){
	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = gmtime(&rawtime);

	double sec = ptm->tm_sec;
	double min = ptm->tm_min;
	double hour = ptm->tm_hour;

	// UT = (22.0 + 30.0 / 60.0) / 24.0;
	// double time = (hour + min / 60.0) / 24.0;
	// double time = (14.0 + 23.0 / 60.0) / 24.0;

	// double time = hour / 24.0;
	double time = ((hour-8.0) + min / 60.0) / 24.0;
	__android_log_print(ANDROID_LOG_ERROR, "com.wallpaper.sundial", "hour %f, min %f, time %f", hour-8.0, min, time);
	return time;
}

jdoubleArray
Java_com_wallpaper_sundial_SundialWallpaperService_getSunPos( JNIEnv* env,
                                              jobject self,
                                              jdouble lat,
                                              jdouble lng )
{
	double tz = -8.0;
	double JD = get_jd_wrapper();

	double time = get_local();
	// time = 17.0*60.0;
	s_coord sun_pos = get_sun_pos(JD, lat, lng , time, tz);

	jdoubleArray jresult;
	jresult = (*env)->NewDoubleArray(env, 2);
	if (jresult == NULL) {
 		return NULL;  // out of memory error thrown 
	}
	
	jdouble result[2];
	result[0] = sun_pos.elevation;
	result[1] = sun_pos.azimuth;

	(*env)->SetDoubleArrayRegion(env, jresult, 0, 2, result);
	return jresult;
}

jdoubleArray
Java_com_wallpaper_sundial_SundialWallpaperService_getMoonPos( JNIEnv* env,
                                              jobject self,
                                              jdouble lat,
                                              jdouble lng )
{
	// double d = get_jd_wrapper();
	double d = timescale(2013, 1, 5);
	double Ms = 356.0470 + 0.9856002585 * d;
	Ms *= d2r;

	double ws = 282.9404 + 0.00000470935 * d;
	ws *= d2r;

	double rm;
	double RAm;
	double Decm;
	double AZm, Altm, UT;

	lng *= d2r;
	lat *= d2r;

	UT = (22.0 + 30.0 / 60.0) / 24.0;
	UT = 10.5 / 24.0;
	UT = get_time();
	moonposition(d + UT, Ms, ws, &rm, &RAm, &Decm);
	__android_log_print(ANDROID_LOG_ERROR, "com.wallpaper.sundial", "UT %f", UT);

	// printf("ms %f, ws %f, rm %f, ram %f, decl %f", Ms, ws, rm, RAm, Decm); // these are all correct
	altitude(UT*24.0, lng, lat, RAm, Decm, Ms, ws, &AZm, &Altm);

	double azTest = AZm;
	double altTest = Altm;

	Altm = topocentric_correction(rm, Altm, lat);
	AZm = modpi(AZm);
	AZm *= r2d;
	Altm *= r2d;

	/////////
	double time = get_local();
	// time = 22.0*60.0;
	double JD = get_jd_wrapper();
	double tz = -8.0;

	jdoubleArray jresult;
	jresult = (*env)->NewDoubleArray(env, 2);
	if (jresult == NULL) {
 		return NULL;  // out of memory error thrown 
	}
	jdouble result[2];
	result[0] = Altm;
	result[1] = AZm;

	(*env)->SetDoubleArrayRegion(env, jresult, 0, 2, result);
	return jresult;
}


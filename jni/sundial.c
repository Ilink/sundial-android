#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <time.h>
#include "types.h"
#include "sun.h"

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
	double time = get_local();
	double JD = get_jd_wrapper();
	double tz = -8.0;
	jdoubleArray jresult;
	jresult = (*env)->NewDoubleArray(env, 2);
	if (jresult == NULL) {
 		return NULL;  // out of memory error thrown 
	}
	jdouble result[2];
	result[0] = 0.0;
	result[1] = 0.0;

	(*env)->SetDoubleArrayRegion(env, jresult, 0, 2, result);
	return jresult;
}
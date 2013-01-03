#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <time.h>
#include "types.h"
#include "sun.h"

JNIEXPORT jint JNICALL
Java_com_wallpaper_sundial_Sundial_stringFromJNI( JNIEnv* env,
                                                  jobject self )
{
	return (jint) 1;
    return (*env)->NewStringUTF(env, "Hello from JNI !");
}

jdoubleArray
Java_com_wallpaper_sundial_SundialWallpaperService_getSunPos( JNIEnv* env,
                                              jobject self,
                                              jdouble lat,
                                              jdouble lng )
{
	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = localtime(&rawtime);

	double tz = -8.0;
	int year = 1900 + ptm->tm_year;
	int day = ptm->tm_mday;
	int month = ptm->tm_mon+1;
	double JD = get_jd(year, month, day);

	double time = get_local();
	// time = 11.0*60.0;
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
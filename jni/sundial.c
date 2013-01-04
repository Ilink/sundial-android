#include <string.h>
#include <jni.h>
#include <stdio.h>
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
	double d = timescale(2013, 1, 4);
	double Ms = 356.0470 + 0.9856002585 * d;
	Ms *= d2r;

	double ws = 282.9404 + 4.70935E-5 * d;
	ws *= d2r;

	double rm;
	double RAm;
	double Decm;

	moonposition(d, Ms, ws, &rm, &RAm, &Decm);
	double AZm, Altm;
	double UT = 3.0;
	// double UT = 0 + ((double)i)/(24*6);
	altitude(UT*24, lng, lat, RAm, Decm, Ms, ws, &AZm, &Altm);
	Altm = topocentric_correction(rm, Altm, lat);
	AZm = modpi(AZm);
	AZm *= r2d;
	Altm *= r2d;

	/////////
	double time = get_local();
	// time = 22.0*60.0;
	double JD = get_jd_wrapper();
	double tz = -8.0;

	s_coord moon_pos = get_moon_pos(JD, lat, lng, time, tz);

	jdoubleArray jresult;
	jresult = (*env)->NewDoubleArray(env, 2);
	if (jresult == NULL) {
 		return NULL;  // out of memory error thrown 
	}
	jdouble result[2];
	result[0] = Altm;
	result[1] = 0.0;

	(*env)->SetDoubleArrayRegion(env, jresult, 0, 2, result);
	return jresult;
}
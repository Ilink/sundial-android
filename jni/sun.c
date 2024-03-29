#include <stdio.h>
#include <math.h>
#include <time.h>
#include "types.h"
#include "sun.h"

double get_hla(double lat, double ha){
	return atan(cos(lat)*tan(ha*(PI/180)));
}

double get_ha(int hour){
	(hour - 12) * 15;
	return (hour - 12) * 15;
}

// returns hour + fraction
double get_ut(){
	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = gmtime(&rawtime);

	double sec = ptm->tm_sec / 3600.0;
	double min = ptm->tm_min / 60.0;
	double time = ptm->tm_hour + sec + min;

	return time;
}

double get_local(){
	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = localtime(&rawtime);

	double sec = ptm->tm_sec;
	double min = ptm->tm_min;
	double hour = ptm->tm_hour;

	double time = hour*60.0 + min + sec/60.0;
	return time;
}

double deg_to_rad(double x){
	return PI * x / 180.0;
}

double rad_to_deg(double x){
	return 180.0 * x / PI;
}

double au_to_km(double au){
	return au*149598000.0;
}

// uses UT, not local time
double get_jd(int year, int month, int day){
	double UT;
	UT = get_ut();	

	time_t t = time(NULL);
	struct tm *now = localtime(&t);

	int a = floor(year/100.0);
	int b = 2 - a + floor(a/4.0);
	
	double jd = floor(365.25*(year + 4716))+ floor(30.6001*(month+1))+day+b-1524.5;
	return jd;
}

// http://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs
unsigned int abs(int i){
	int const mask = i >> (sizeof(int) * 8 - 1);
	return ((i + mask) ^ mask);
}

int int_part(double x){
	return abs(x)*((int)x >> 31) | (((0 - (int)x) >> 31) & 1);
}

double range_deg(double x){
	double b = x / 360;
	double a = 360 * (b-int_part(b));
	if(a < 0) a += 360;
	return a;
}

// returns degrees
double calc_obliq_corr(double t){
	double seconds = 21.448 - t*(46.8150 + t*(0.00059 - t*(0.001813)));
	double e0 = 23.0 + (26.0 + (seconds/60.0))/60.0;
	double omega = 125.04 - 1934.136 * t;
	return e0 + 0.00256 * cos(deg_to_rad(omega));
}

// returns degrees
double calc_sun_center(double t) {
	double m = 357.52911 + t * (35999.05029 - 0.0001537 * t);
	double mrad = deg_to_rad(m);
	double sinm = sin(mrad);
	double sin2m = sin(mrad+mrad);
	double sin3m = sin(mrad+mrad+mrad);
	return sinm * (1.914602 - t * (0.004817 + 0.000014 * t)) + sin2m * (0.019993 - 0.000101 * t) + sin3m * 0.000289;
}

// returns degrees
double calc_mean_lng_sun(double t){
	double L0 = 280.46646 + t * (36000.76983 + t*(0.0003032));
	while(L0 > 360.0) L0 -= 360.0;
	while(L0 < 0.0) L0 += 360.0;
	return L0;
}

// returns degrees
double calc_sun_app_lng(double t){
	double l0 = calc_mean_lng_sun(t);
	double c = calc_sun_center(t);
	double o = l0 + c;
	double omega = 125.04 - 1934.136 * t;
	double lambda = o - 0.00569 - 0.00478 * sin(deg_to_rad(omega));
	return lambda;
}

double calc_earth_ecc(double t){
	return deg_to_rad(0.016708634 - t * (0.000042037 + 0.0000001267 * t));
}

// returns AUs
double calc_sun_rad_vector(double t) {
	double l0 = calc_mean_lng_sun(t);
	double c = calc_sun_center(t);
	double v = l0 + c;
	double e = calc_earth_ecc(t);
	return (1.000001018 * (1 - e * e)) / (1 + e * cos(deg_to_rad(v)));
}

double calc_mean_anomaly_sun(double t){
	return 357.52911 + t * (35999.05029 - 0.0001537 * t);
}

// returns degrees
double calc_declination(double t){
	double e = calc_obliq_corr(t);
	double lambda = calc_sun_app_lng(t);

	double sint = sin(deg_to_rad(e)) * sin(deg_to_rad(lambda));
	double theta = rad_to_deg(asin(sint));
	return theta;
}

double calc_eq_time(double t){
	double epsilon = calc_obliq_corr(t);
	double l = calc_mean_lng_sun(t);
	double e = 0.016708634 - t * (0.000042037 + 0.0000001267 * t);
	double m = calc_mean_anomaly_sun(t);

	double y = tan(deg_to_rad(epsilon)/2.0);
	y *= y;

	double test = 2.0 * deg_to_rad(l);
	double sin2l0 = sin(test);
	double sinm   = sin(deg_to_rad(m));
	double cos2l0 = cos(2.0 * deg_to_rad(l));
	double sin4l0 = sin(4.0 * deg_to_rad(l));
	double sin2m  = sin(2.0 * deg_to_rad(m));

	sin2l0 = -0.5486700881092892;
	sinm = -0.8678243339510023;
	cos2l0 = 0.8360389550817263;
	sin4l0 = -0.9174191342949778;
	sin2m = 0.8623937246406765;

	double Etime = y * sin2l0 - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2l0 - 0.5 * y * y * sin4l0 - 1.25 * e * e * sin2m;
	return rad_to_deg(Etime)*4.0; // in minutes of time
}

s_coord get_sun_pos(double jd, double lat, double lng, double hour, double tz){

	double time;
	time = jd + hour/1440.0 - tz/24.0; // adjust JD for the hour
	time = (time - 2451545.0)/36525.0;
	double eqtime = calc_eq_time(time);
	double delta = calc_declination(time);
	double solar_time_fix = eqtime + 4.0 * lng - 60.0 * tz;
	double true_solar_time = hour + solar_time_fix;
	while(true_solar_time > 1440) true_solar_time -= 1440.0;
	double ha = true_solar_time / 4.0 - 180.0;
	if(ha < -180) ha += 360.0;
	double r = calc_sun_rad_vector(time);
	double ha_rad = deg_to_rad(ha);
	double cos_zenith = sin(deg_to_rad(lat)) * sin(deg_to_rad(delta)) + cos(deg_to_rad(lat)) * cos(deg_to_rad(delta)) * cos(ha_rad);
	if(cos_zenith > 1.0) cos_zenith = 1.0;
	else if (cos_zenith < -1.0) cos_zenith = -1.0;
	double zenith = rad_to_deg(acos(cos_zenith));

	double azimuth_denom = cos(deg_to_rad(lat))*sin(deg_to_rad(zenith));
	double azimuth;
	if(fabs(azimuth_denom) > 0.001){
		double azimuth_rad = ((sin(deg_to_rad(lat)) * cos(deg_to_rad(zenith))) - sin(deg_to_rad(delta))) / azimuth_denom;
		if(fabs(azimuth_rad)>1.0) {
			if(azimuth_rad < 0.0) azimuth_rad = -1.0;
			else azimuth_rad = 1.0;
		}
		azimuth = 180.0 - rad_to_deg(acos(azimuth_rad));
		if(ha > 0.0) azimuth = -azimuth;
	} else {
		if(lat > 0.0) azimuth = 180.0;
		else azimuth = 0.0;
	}
	if (azimuth < 0) azimuth += 360.0;

	double ref_corr;
	double eo_ele = 90.0 - zenith;
	if(eo_ele > 85.0){
		ref_corr = 0.0;
	} else {
		double te = tan(deg_to_rad(eo_ele));
		if(eo_ele > 5.0) {
			ref_corr = 58.1 / te - 0.07 / (te*te*te) + 0.000086 / (te*te*te*te*te);
		} else if(eo_ele > -0.575) {
			ref_corr = 1735.0 + eo_ele * (-518.2 + eo_ele * (103.4 + eo_ele * (-12.79 + eo_ele * 0.711)));
		} else {
			ref_corr = -20.774 / te;
		}
		ref_corr /= 3600.0;
	}
	double solar_zenith = zenith - ref_corr;

	s_coord coord;
	coord.azimuth = azimuth;
	coord.r = r;
	double el = floor((90.0-solar_zenith)*100+0.5)/100.0;
	coord.elevation = el;

	return coord;
}
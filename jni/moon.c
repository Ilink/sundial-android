#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <android/log.h>
#include "types.h"
#include "sun.h"
#include "moon.h"

#define pi M_PI
#define r2d (180/M_PI)
#define d2r (M_PI/180)

#define DEFLAT 9999
#define DEFLON 9999

s_coord get_moon_pos(double jd, double lat, double lng, double hour, double tz){
	s_coord coord;

	// lat = -1.87;
	// lng = 131.52;

	double T = (jd - 2451545.0) / 36525.0;
	// T = -0.0862137805;
	double eps = 23.0 + 26.0/60.0 + 21.448/3600.0 - 
				(46.8150*T+ 0.00059*T*T- 0.001813*T*T*T)/3600.0;
	double X = cos(lat)*cos(lng);
	double Y = (cos(eps)*cos(lat)*sin(lng)) - sin(eps)*sin(lat);
	double Z = (sin(eps)*cos(lat)*sin(lng)) - cos(eps)*sin(lat);
	double R = sqrt(1.0-(Z*Z));

	double delta = (180.0/M_PI)*atan2(Z,R); // in degrees
	double RA = (24.0/M_PI)*atan2(Y,X+R); // in hours

	double theta0 = 280.46061837 + 360.98564736629*(jd-2451545.0) + 0.000387933*T*T - (T*T*T/38710000.0); // degrees

	double theta = theta0 + lng;
	double tau = theta - RA;

	double h = asin(sin(lat )*sin(delta) + cos(lat)*cos(delta)*cos(tau));
	double az = atan2(-sin(tau), cos(lat)*tan(delta) - sin(lat)*cos(tau));

	// note moon distance in AU
	double horParal = 8.794 / (384400.0/14959787E6); // horizontal parallax (arcseconds)
	double p = cos(h)*sin(horParal/3600.0); // parallax in altitude (degrees)

	coord.azimuth = T;
	coord.azimuth = delta;
	// coord.azimuth = h;

	return coord;
}

double eccentric_anomaly(double M, double e){
	/* E and M in radians */
    double E0 = M + e * sin(M) * ( 1.0 + e * cos(M) );
    double E1 = E0 - ( E0 - e * sin(E0) - M ) / ( 1 - e * cos(E0) );
	int iteration = 0;
	if (e > 0.05) {
		while (fabs(E1 - E0) > 0.001*d2r) {
    		E0 = E1;
    		E1 = E0 - ( E0 - e * sin(E0) - M ) / ( 1 - e * cos(E0) );
			if (++iteration >= 10) {
				return 0.0;
			}
		}
	}
	return E1;
}

void moonposition(double d,
				  double Ms,
				  double ws,
				  double* rm,
				  double* RAm,
				  double* Decm)
{
    double ecl = 23.4393 - 3.563E-7 * d;
	ecl *= d2r;

    double N = 125.1228 - 0.0529538083 * d;
    double i = 5.1454;
    double w = 318.0634 + 0.1643573223 * d;
    double a = 60.2666;				  /* Earth radii */
    double e = 0.054900;
    double M = 115.3654 + 13.0649929509 * d;

	N *= d2r;
	i *= d2r;
	w *= d2r;
	M *= d2r;

	double E = eccentric_anomaly(M, e);

	__android_log_print(ANDROID_LOG_ERROR, "com.wallpaper.sundial", "E %f", e);

    double xv = a * ( cos(E) - e );
    double yv = a * ( sqrt(1.0 - e*e) * sin(E) );
    double v = atan2( yv, xv );
    double r = sqrt( xv*xv + yv*yv );

    double xg = r * ( cos(N) * cos(v+w) - sin(N) * sin(v+w) * cos(i) );
    double yg = r * ( sin(N) * cos(v+w) + cos(N) * sin(v+w) * cos(i) );
    double zg = r * ( sin(v+w) * sin(i) );

    double lonecl = atan2( yg, xg );
    double latecl = atan2( zg, sqrt(xg*xg + yg*yg) );

	double Mm = M;
	double Nm = N;
	double wm = w;

    double Ls = Ms + ws;       /* Mean Longitude of the Sun  (Ns=0) */
    double Lm = Mm + wm + Nm;  /* Mean longitude of the Moon */
    double D = Lm - Ls;        /* Mean elongation of the Moon */
    double F = Lm - Nm;        /* Argument of latitude for the Moon */

	lonecl += (
    	-1.274 * sin(Mm - 2*D)          /* the Evection */
    	+0.658 * sin(2*D)               /* the Variation */
    	-0.186 * sin(Ms)                /* the Yearly Equation */
    	-0.059 * sin(2*Mm - 2*D)
    	-0.057 * sin(Mm - 2*D + Ms)
    	+0.053 * sin(Mm + 2*D)
    	+0.046 * sin(2*D - Ms)
    	+0.041 * sin(Mm - Ms)
    	-0.035 * sin(D)                 /* the Parallactic Equation */
    	-0.031 * sin(Mm + Ms)
    	-0.015 * sin(2*F - 2*D)
    	+0.011 * sin(Mm - 4*D))*d2r;

	latecl += (
    	-0.173 * sin(F - 2*D)
    	-0.055 * sin(Mm - F - 2*D)
    	-0.046 * sin(Mm + F - 2*D)
    	+0.033 * sin(F + 2*D)
    	+0.017 * sin(2*Mm + F))*d2r;

	r +=
    	-0.58 * cos(Mm - 2*D)
    	-0.46 * cos(2*D);

    xg = r * cos(lonecl) * cos(latecl);
    yg = r * sin(lonecl) * cos(latecl);
    zg = r               * sin(latecl);

    double xe = xg;
    double ye = yg * cos(ecl) - zg * sin(ecl);
    double ze = yg * sin(ecl) + zg * cos(ecl);
    double RA  = atan2( ye, xe );
    double Dec = atan2( ze, sqrt(xe*xe+ye*ye) );

	*rm = r;
	*RAm = RA;
	*Decm = Dec;
}

double topocentric_correction(double r, double alt_geoc, double lat)
{
    double mpar = asin( 1/r );
    double alt_topoc = alt_geoc - mpar * cos(alt_geoc);
	return alt_topoc;
}

void altitude(double UT,
			  double lng,
			  double lat,
			  double RA,
			  double Decl,
			  double Ms,
			  double ws,
			  double* AZ,
			  double* Alt)
{
    double L = Ms + ws;		/* the Sun's mean longitude */
    double GMST0 = L*r2d + 180;

    double LST = GMST0 + UT*15.0 + lng*r2d;
    double LHA = LST - RA*r2d;
    double sin_h = sin(lat) * sin(Decl) + cos(lat) * cos(Decl) * cos(LHA*d2r);
	double h = asin(sin_h);
	*Alt = h;

	double azim = atan2(-sin(LHA*d2r)*cos(Decl),
						cos(lat)*sin(Decl) - sin(lat)*cos(Decl)*cos(LHA*d2r)); 
	*AZ = azim;

	__android_log_print(ANDROID_LOG_ERROR, "com.wallpaper.sundial", "gmst0 %f, L %f, LST %f, LHA %f, sin_h %f, RA %f, decl %f, lng %f, lat %f", GMST0, L, LST, LHA, sin_h, RA, Decl, lng, lat);

	__android_log_print(ANDROID_LOG_ERROR, "com.wallpaper.sundial", "\n\nLNG %f !!!!!!!, LAT %f !!!!!!!\n\n", lng, lat);
}

double modpi(double a)
{
	while (a < 0) {
		a += 2*pi;
	}
	while (a > 2*pi) {
		a -= 2*pi;
	}
	return a;
}

int timescale(int y, int m, int d)
{
	return 367*y - 7*(y + (m+9)/12)/4 + 275*m/9 + d - 730530;
}

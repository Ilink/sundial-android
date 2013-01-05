#ifndef MOON_H
#define MOON_H

s_coord get_moon_pos(double jd, double lat, double lng, double hour, double tz);
int timescale(int y, int m, int d);
double eccentric_anomaly(double M, double e);
void sunposition(double d, double* Ms, double* ws, double* RAs, double* Decs);
void moonposition(double d,
					double Ms,
					double ws,
					double* rm,
					double* RAm,
					double* Decm);
double topocentric_correction(double r, double alt_geoc, double lat);
void altitude(double UT,
				double lon,
				double lat,
				double RA,
				double Decl,
				double Ms,
				double ws,
				double* AZ,
				double* Alt);
double modpi(double a);
static void usage();

#endif
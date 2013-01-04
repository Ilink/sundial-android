#ifndef MOON_H
#define MOON_H

s_coord get_moon_pos(double jd, double lat, double lng, double hour, double tz);
void moonposition(double d,
				  double Ms,
				  double ws,
				  double* rm,
				  double* RAm,
				  double* Decm);

#endif
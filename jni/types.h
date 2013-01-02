#ifndef TYPES_H
#define TYPES_H

extern double const PI;
extern double const jd2k;

typedef struct {
	int x; 
	int y;
} point;

typedef struct {
	double x; 
	double y;
} point_f;

typedef struct {
	double azimuth;
	double r;
	double elevation;
} s_coord;

typedef struct {
	int width;
	int height;
	double scale_x;
	double scale_y;
} screen_info;

typedef struct{
	double midpoint;
} scale_stuff;

typedef struct{
	double midpoint;
	double x_scale;
	double y_scale;
} scale_info;

typedef struct {
	double lat;
	double lng;
} geo_coord;

#endif
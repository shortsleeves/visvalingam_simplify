//
//
// 2013 (c) Mathieu Courtemanche

#ifndef OGR_H
#define OGR_H

#include "polyline.h"
#include <vector>
#include <string>

class OGRPoint;
class OGRLineString;
class OGRLinearRing;
class OGRPolygon;
class OGRMultiPolygon;

void from_ogr_shape(const OGRPoint& ogr_shape, Point* res);
void from_ogr_shape(const OGRLineString& ogr_shape, Linestring* res);
void from_ogr_shape(const OGRPolygon& ogr_shape, Polygon* res);
void from_ogr_shape(const OGRMultiPolygon& ogr_shape, MultiPolygon* res);

void to_ogr_shape(const Point& shape, OGRPoint* ogr_shape);
void to_ogr_shape(const Linestring& shape, OGRLinearRing* ogr_shape);
void to_ogr_shape(const MultiPolygon& shape, OGRMultiPolygon* ogr_shape);

void print_wkt(const OGRMultiPolygon& ogr_multi_poly);
int process_ogr(const char *filename, bool print_source);

#endif // OGR_H

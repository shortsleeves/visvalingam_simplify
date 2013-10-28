//
//
// 2013 (c) Mathieu Courtemanche

#ifndef GEO_TYPES_H
#define GEO_TYPES_H
#include <vector>
#include <string>

class OGRPoint;
class OGRLineString;
class OGRLinearRing;
class OGRPolygon;
class OGRMultiPolygon;

typedef size_t VertexIndex;

struct Point
{
    Point() {}
    Point(double inX, double inY) : X(inX), Y(inY) {}

    double X;
    double Y;
};
typedef std::vector<Point> Linestring;
typedef std::vector<Linestring> MultiLinestring;

struct Polygon
{
    Polygon() {}

    Linestring exterior_ring;
    MultiLinestring interior_rings;
};
typedef std::vector<Polygon> MultiPolygon;


void from_ogr_shape(const OGRPoint& ogr_shape, Point* res);
void from_ogr_shape(const OGRLineString& ogr_shape, Linestring* res);
void from_ogr_shape(const OGRPolygon& ogr_shape, Polygon* res);
void from_ogr_shape(const OGRMultiPolygon& ogr_shape, MultiPolygon* res);

void to_ogr_shape(const Point& shape, OGRPoint* ogr_shape);
void to_ogr_shape(const Linestring& shape, OGRLinearRing* ogr_shape);
void to_ogr_shape(const MultiPolygon& shape, OGRMultiPolygon* ogr_shape);

// returns cross product between two vectors: v1 ^ v2 in right handed coordinate
// E.g.: returned value on +z axis
double cross_product(const Point& v1, const Point& v2);

// A - B
Point vector_sub(const Point& A, const Point& B);

#endif // GEO_TYPES_H

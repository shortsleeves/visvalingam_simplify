#ifndef POLYLINE_H
#define POLYLINE_H

#include <vector>
#include <string>

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

// returns cross product between two vectors: v1 ^ v2 in right handed coordinate
// E.g.: returned value on +z axis
double cross_product(const Point& v1, const Point& v2);

// A - B
Point vector_sub(const Point& A, const Point& B);

#endif // POLYLINE_H

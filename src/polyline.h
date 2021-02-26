#ifndef POLYLINE_H
#define POLYLINE_H

#include <vector>
#include <string>

typedef size_t VertexIndex;

struct Point
{
    Point() : id(0), X(0.0), Y(0.0), Z(0.0) {}
    Point(double inX, double inY, double inZ = 0.0, int idx = 0) : id(idx), X(inX), Y(inY), Z(inZ) {}

    int id;
    double X;
    double Y;
    double Z;

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
double cross_product_norm(const Point& v1, const Point& v2);

// A - B
Point vector_sub(const Point& A, const Point& B);

#endif // POLYLINE_H

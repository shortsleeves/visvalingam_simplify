#include "polyline.h"

double cross_product(const Point& v1, const Point& v2)
{
    return (v1.X * v2.Y) - (v1.Y * v2.X);
}

Point vector_sub(const Point& A, const Point& B)
{
    Point res;
    res.X = A.X - B.X;
    res.Y = A.Y - B.Y;
    return res;
}

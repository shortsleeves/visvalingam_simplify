#include "polyline.h"
#include <cmath>

double cross_product_norm(const Point& v1, const Point& v2)
{
    double x = v1.Y*v2.Z - v1.Z*v2.Y;
    double y = v1.Z*v2.X - v1.X*v2.Z;
    double z = v1.X*v2.Y - v1.Y*v2.X;
    return std::sqrt(x*x+y*y+z*z);
}

Point vector_sub(const Point& A, const Point& B)
{
    Point res;
    res.X = A.X - B.X;
    res.Y = A.Y - B.Y;
    res.Z = A.Z - B.Z;
    return res;
}

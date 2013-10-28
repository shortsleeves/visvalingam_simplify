//
//
// 2013 (c) Mathieu Courtemanche
//
#include "geo_types.h"
#include <ogr_geometry.h>
#include <cassert>
#include <sstream>

void from_ogr_shape(const OGRPoint& ogr_shape, Point* res)
{
    res->X = ogr_shape.getX();
    res->Y = ogr_shape.getY();
}

void from_ogr_shape(const OGRLineString& ogr_shape, Linestring* res)
{
    size_t num_points = ogr_shape.getNumPoints();
    for (size_t i = 0; i < num_points; ++i)
    {
        OGRPoint ogr_point;
        ogr_shape.getPoint(i, &ogr_point);
        res->push_back(Point());
        from_ogr_shape(ogr_point, &res->back());
    }
}

void from_ogr_shape(const OGRPolygon& ogr_shape, Polygon* res)
{
    const OGRLinearRing* ogr_exterior = ogr_shape.getExteriorRing();
    assert(ogr_exterior);
    from_ogr_shape(*ogr_exterior, &res->exterior_ring);
    
    size_t interior_count = ogr_shape.getNumInteriorRings();
    for (size_t i = 0; i < interior_count; ++i)
    {
        const OGRLinearRing* ogr_interior = ogr_shape.getInteriorRing(i);
        assert(ogr_interior);
        res->interior_rings.push_back(Linestring());
        from_ogr_shape(*ogr_interior, &res->interior_rings.back());
    }
}

void from_ogr_shape(const OGRMultiPolygon& ogr_shape, MultiPolygon* res)
{
    size_t num_geom = ogr_shape.getNumGeometries();
    for (size_t i = 0; i < num_geom; ++i)
    {
        const OGRGeometry* ogr_geom = ogr_shape.getGeometryRef(i);
        assert (ogr_geom->getGeometryType() == wkbPolygon);
        const OGRPolygon& ogr_polygon = *(OGRPolygon*)ogr_geom;
        res->push_back(Polygon());
        from_ogr_shape(ogr_polygon, &res->back());
    }
}

void to_ogr_shape(const Point& shape, OGRPoint* ogr_shape)
{
    ogr_shape->setX(shape.X);
    ogr_shape->setY(shape.Y);
}

void to_ogr_shape(const Linestring& shape, OGRLinearRing* ogr_shape)
{
    for (VertexIndex i = 0; i < shape.size(); ++i)
    {
        OGRPoint pt;
        to_ogr_shape(shape[i], &pt);
        ogr_shape->addPoint(&pt);
    }
    ogr_shape->closeRings();
}

void to_ogr_shape(const MultiPolygon& shape, OGRMultiPolygon* ogr_shape)
{
    for (size_t i = 0; i < shape.size(); ++i)
    {
        const Polygon& poly = shape[i];
        OGRLinearRing ext_ring;
        to_ogr_shape(poly.exterior_ring, &ext_ring);
        OGRPolygon ogr_polygon;
        ogr_polygon.addRing(&ext_ring);

        for (size_t j = 0; j < poly.interior_rings.size(); ++j)
        {
            OGRLinearRing int_ring;
            to_ogr_shape(poly.interior_rings[j], &int_ring);
            ogr_polygon.addRing(&int_ring);
        }

        ogr_shape->addGeometry(&ogr_polygon);
    }
    ogr_shape->closeRings();
}

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


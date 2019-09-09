//
//
// 2013 (c) Mathieu Courtemanche
//

#include <iostream>
#include "ogr.h"

#ifdef HAVE_GDAL
#include <cassert>
#include <sstream>
#include <ogr_geometry.h>
#include <ogrsf_frmts.h>
#include <visvalingam_algorithm.h>

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

void print_wkt(const OGRMultiPolygon &ogr_multi_poly)
{
    char* wkt_text = NULL;
    ogr_multi_poly.exportToWkt(&wkt_text);
    std::cout << std::endl << wkt_text << std::endl;
    OGRFree(wkt_text);
}

int process_ogr(const char *filename, bool print_source)
{
    MultiPolygon multi_poly;

    // Parse shape files via OGR: http://gdal.org/ogr/index.html
    OGRRegisterAll();

    OGRDataSource* datasource = OGRSFDriverRegistrar::Open(filename, FALSE);
    if (datasource == NULL)
    {
        std::cerr << "Open failed for file: " << filename << std::endl;
        return 1;
    }

    size_t layer_count = datasource->GetLayerCount();
    for (size_t i=0; i < layer_count; ++i)
    {
        OGRLayer* layer = datasource->GetLayer(i);
        assert(layer);
        layer->ResetReading();
        layer->SetAttributeFilter("NAME LIKE 'united states%'");

        OGRFeature* feat;
        while ((feat = layer->GetNextFeature()) != NULL)
        {
            OGRGeometry* geometry = feat->GetGeometryRef();
            if (geometry == NULL)
            {
                continue;
            }
            switch (geometry->getGeometryType())
            {
            case wkbMultiPolygon:
            {
                OGRMultiPolygon* ogr_multi_poly = (OGRMultiPolygon*)geometry;

                if (print_source)
                {
                    std::cout << "SOURCE DATA: " << std::endl;
                    print_wkt(*ogr_multi_poly);
                    std::cout << std::endl;
                }

                from_ogr_shape(*ogr_multi_poly, &multi_poly);


                MultiPolygon res;
                run_visvalingam(multi_poly, res);

                // convert back to OGR shape
                OGRMultiPolygon ogr_multipolygon;
                to_ogr_shape(res, &ogr_multipolygon);
                std::cout << "SIMPLIFIED SHAPE: " << std::endl;
                print_wkt(ogr_multipolygon);

                break;
            }

            default:
            {
                break;
            }
            }
            OGRFeature::DestroyFeature(feat);
        }
    }
    OGRDataSource::DestroyDataSource(datasource);
}

#else

void from_ogr_shape(const OGRPoint& ogr_shape, Point* res) { }
void from_ogr_shape(const OGRLineString& ogr_shape, Linestring* res) { }
void from_ogr_shape(const OGRPolygon& ogr_shape, Polygon* res) { }
void from_ogr_shape(const OGRMultiPolygon& ogr_shape, MultiPolygon* res) { }

void to_ogr_shape(const Point& shape, OGRPoint* ogr_shape) { }
void to_ogr_shape(const Linestring& shape, OGRLinearRing* ogr_shape) { }
void to_ogr_shape(const MultiPolygon& shape, OGRMultiPolygon* ogr_shape) {}

void print_wkt(const OGRMultiPolygon& ogr_multi_poly) { }
int process_ogr(const char *filename, bool print_source)
{
    std::cerr << "error: utility was compiled without OGR file format support. Recompile with GDAL." << std::endl;
    return -1;
}

#endif


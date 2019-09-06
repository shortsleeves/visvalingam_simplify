//
//
// 2013 (c) Mathieu Courtemanche
//
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ogrsf_frmts.h>
#include "visvalingam_algorithm.h"
#include "ogr.h"
#include "heap.hpp"

static void print_wkt(const OGRMultiPolygon& ogr_multi_poly)
{
    char* wkt_text = NULL;
    ogr_multi_poly.exportToWkt(&wkt_text);
    std::cout << std::endl << wkt_text << std::endl;
    OGRFree(wkt_text);
}

static void run_visvalingam(const Linestring& shape, Linestring* res)
{
    Visvalingam_Algorithm vis_algo(shape);
    Linestring simplified_linestring;
    vis_algo.simplify(0.002, res);
}

static void run_visvalingam(const MultiPolygon& shape)
{
    MultiPolygon res;
    for (size_t i = 0; i < shape.size(); ++i)
    {
        const Polygon& poly = shape[i];
        res.push_back(Polygon());
        run_visvalingam(poly.exterior_ring, &res.back().exterior_ring);
        for (size_t j = 0; j < poly.interior_rings.size(); ++j)
        {
            res.back().interior_rings.push_back(Linestring());
            run_visvalingam(poly.interior_rings[j], &res.back().interior_rings.back());
        }
    }

    // convert back to OGR shape
    OGRMultiPolygon ogr_multipolygon;
    to_ogr_shape(res, &ogr_multipolygon);
    std::cout << "SIMPLIFIED SHAPE: " << std::endl;
    print_wkt(ogr_multipolygon);
}


int main(int argc, char **argv)
{
    bool print_source = false;
    const char* filename = NULL;
    for (int i=1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--file") == 0 && (i+1) < argc)
        {
            ++i;
            filename = argv[i];
        }
        else if (strcmp(argv[i], "--dump-source") == 0)
        {
            print_source = true;
        }
    }

    if (filename != NULL)
    {
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

                    MultiPolygon multi_poly;
                    from_ogr_shape(*ogr_multi_poly, &multi_poly);

                    run_visvalingam(multi_poly);
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
	return 0;
}

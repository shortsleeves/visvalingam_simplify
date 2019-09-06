//
//
// 2013 (c) Mathieu Courtemanche
//
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <ogrsf_frmts.h>
#include "visvalingam_algorithm.h"
#include "ogr.h"
#include "csv.h"
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

int process_csv(const char *filename, bool print_source)
{
    Linestring shape;
    Linestring shape_simplified;
    std::ifstream is(filename);

    if (!is.is_open()) {
        return 1;
    }

    CSVIterator iter(is);
    while(iter != CSVIterator()) {
        double x = std::stod((*iter)[0]);
        double y = std::stod((*iter)[1]);
        double z = std::stod((*iter)[2]);
        shape.push_back(Point(x, y, z));
        iter++;
    }

    std::cout << "original shape:   " << shape.size() << " points" << std::endl;
    Visvalingam_Algorithm vis_algo(shape);
    vis_algo.simplify(0.2, &shape_simplified);
    std::cout << "simplified shape: " << shape_simplified.size() << " points" << std::endl;

    std::ofstream os(std::string(filename) + ".out");
    for (const Point &p : shape_simplified) {
        os << p.X << "," << p.Y << "," << p.Z << "\n";
    }

    return 0;
}

enum InputFormat {
    FORMAT_OGR,
    FORMAT_CSV,
};

int main(int argc, char **argv)
{
    bool print_source = false;
    const char* filename = NULL;
    InputFormat file_format = FORMAT_OGR;
    int res = 0;
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
        else if (strcmp(argv[i], "--format=ogr") == 0)
        {
            file_format = FORMAT_OGR;
        }
        else if (strcmp(argv[i], "--format=csv") == 0)
        {
            file_format = FORMAT_CSV;
        }
        else
        {
            std::cout << "unkown argument: " << argv[i] << std::endl;
            return 1;
        }
    }

    if (!filename)
    {
        std::cout << "error: input file required" << std::endl;
        return 1;
    }

    switch(file_format)
    {
    case FORMAT_OGR:
        res = process_ogr(filename, print_source);
        break;
    case FORMAT_CSV:
        res = process_csv(filename, print_source);
        break;
    }

    if (res != 0)
    {
        std::cout << "error: input file loading failed" << std::endl;
        return res;
    }

    return res;
}

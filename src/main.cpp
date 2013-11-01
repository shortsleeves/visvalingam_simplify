//
//
// 2013 (c) Mathieu Courtemanche
//
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <ogrsf_frmts.h>
#include "visvalingam_algorithm.h"
#include "geo_types.h"
#include "heap.hpp"

void test_vector_sub()
{
    Point a, b;
    a.X = 10;
    a.Y = 20;
    b.X = 5;
    b.Y = 1;
    Point res = vector_sub(a, b);
    assert(res.X == 5);
    assert(res.Y == 19);
}

void test_cross_product()
{
    Point a, b, c;
    a.X = 10;
    a.Y = 8;
    b.X = 2;
    b.Y = 5;
    c.X = -100;
    c.Y = 8;

    double res1 = cross_product(a,b);
    double res2 = cross_product(b,a);
    double res3 = cross_product(a,c);
    assert(res1 == 34);
    assert(res2 == -34);
    assert(res3 == 880);
}

void test_heap_sort()
{
    Heap<int32_t> heap(5);
    heap.insert(1);
    heap.insert(4);
    heap.insert(8);
    heap.insert(-10);
    heap.insert(2);

    std::vector<int32_t> res;
    const int32_t expected[] = {-10,1,2,4,8};
    while (!heap.empty())
    {
        res.push_back(heap.pop());
    }
    assert(res.size() == (sizeof(expected)/sizeof(expected[0])));
    for (size_t i=0; i < res.size(); ++i)
    {
        assert(res[i] == expected[i]);
    }
}

void test_heap_insert_remove_mix()
{
    Heap<int32_t> heap(4);
    heap.insert(5);
    heap.insert(99);
    heap.insert(-10);
    assert(heap.top() == -10);
    heap.pop();

    heap.pop(); // 5
    heap.insert(100);
    assert(heap.top() == 99);

    heap.pop();
    heap.pop();
    assert(heap.empty());
}

struct HeapPtrTest
{
    explicit HeapPtrTest(int32_t v) : value(v)
    {}
    int32_t value;
};

struct HeapPtrTestComp
{
    bool operator()(const HeapPtrTest* lhs, const HeapPtrTest* rhs) const
    {
        return lhs->value < rhs->value;
    }
};

void test_heap_reheap()
{
    std::vector<HeapPtrTest*> test_values;
    test_values.push_back(new HeapPtrTest(2));
    test_values.push_back(new HeapPtrTest(8));
    test_values.push_back(new HeapPtrTest(1));
    Heap<HeapPtrTest*, HeapPtrTestComp> heap(3);
    for (size_t i=0; i < test_values.size(); ++i)
    {
        heap.insert(test_values[i]);
    }
    assert(heap.top()->value == 1);

    // Update the value in the last item (used to be 1, now will be 10)
    assert(test_values[2]->value == 1);
    test_values[2]->value = 10;
    // Notify heap that we've modified that node directly
    heap.reheap(test_values[2]);
    assert(heap.top()->value == 2);

    while (!heap.empty())
    {
        HeapPtrTest* t = heap.pop();
        delete t;
    }
    test_values.clear();
}

void test_linestring(Linestring* res)
{
    res->push_back(Point(0,0));
    res->push_back(Point(5,-10));
    res->push_back(Point(12,-6));
    res->push_back(Point(15,-7));
    res->push_back(Point(19,-6));
    res->push_back(Point(25,0));
}

void test_basic_visvalingam()
{
    Linestring line;
    test_linestring(&line);

    Visvalingam_Algorithm vis_algo(line);
    //vis_algo.print_areas();
}

bool unit_tests()
{
    try
    {
        test_vector_sub();
        test_cross_product();
        test_heap_sort();
        test_heap_insert_remove_mix();
        test_heap_reheap();
        //test_effective_area();
        test_basic_visvalingam();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

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
    bool run_unit_tests = false;
    bool print_source = false;
    const char* filename = NULL;
    for (int i=1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--check") == 0)
        {
            run_unit_tests = true;
        }
        else if (strcmp(argv[i], "--file") == 0 && (i+1) < argc)
        {
            ++i;
            filename = argv[i];
        }
        else if (strcmp(argv[i], "--dump-source") == 0)
        {
            print_source = true;
        }
    }

    if (run_unit_tests)
    {
        if (unit_tests())
        {
            std::cout << "All tests succeeded" << std::endl;
            return 0;
        }
        else
        {
            std::cout << "Unit test failure" << std::endl;
            return 1;
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

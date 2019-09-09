//
//
// 2013 (c) Mathieu Courtemanche
//
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include "visvalingam_algorithm.h"
#include "ogr.h"
#include "csv.h"
#include "heap.hpp"

int process_csv(const char *filename, bool print_source, uint ratio)
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
    vis_algo.simplify_ratio(ratio, &shape_simplified);
    std::cout << "simplified shape: " << shape_simplified.size() << " points" << std::endl;

    std::ofstream os_pts(std::string(filename) + ".out");
    for (const Point &p : shape_simplified) {
        os_pts << p.X << "," << p.Y << "," << p.Z << "\n";
    }

    std::ofstream os_areas(std::string(filename) + ".areas");
    for (int i = 1; i < shape.size() - 1; i++) {
        os_areas << effective_area(shape[i], shape[i-1], shape[i+1]) << "\n";
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
    uint ratio = 50;
    InputFormat file_format = FORMAT_OGR;
    int res = 0;
    for (int i=1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--file") == 0 && (i+1) < argc)
        {
            ++i;
            filename = argv[i];
        }
        else if (strcmp(argv[i], "--ratio") == 0 && (i+1) < argc)
        {
            ++i;
            ratio = std::atoi(argv[i]);
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

    if (ratio <= 0 || ratio >= 100)
    {
        std::cout << "error: bad ratio, must be between 1 and 99, got: " << ratio << std::endl;
        return 1;
    }

    switch(file_format)
    {
    case FORMAT_OGR:
        std::cout << "processing_ogr" << std::endl;
        res = process_ogr(filename, print_source);
        break;
    case FORMAT_CSV:
        std::cout << "processing_csv" << std::endl;
        res = process_csv(filename, print_source, ratio);
        break;
    }

    if (res != 0)
    {
        std::cout << "error: input file loading failed" << std::endl;
        return res;
    }

    return res;
}

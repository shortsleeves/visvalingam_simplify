//
//
// 2013 (c) Mathieu Courtemanche

#include "visvalingam_algorithm.h"
#include <cstdlib>
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>
#include <algorithm>
#include "heap.hpp"

static const double NEARLY_ZERO = 1e-7;

// Represents 3 vertices from the input line and its associated effective area.
struct VertexNode
{
    VertexNode(VertexIndex vertex_, VertexIndex prev_vertex_,
               VertexIndex next_vertex_, double area_)
        : vertex(vertex_)
        , prev_vertex(prev_vertex_)
        , next_vertex(next_vertex_)
        , area(area_)
    {
    }

    // ie: a triangle
    VertexIndex vertex;
    VertexIndex prev_vertex;
    VertexIndex next_vertex;
    // effective area
    double area;
};

struct VertexNodeCompare
{
    bool operator()(const VertexNode* lhs, const VertexNode* rhs) const
    {
        return lhs->area < rhs->area;
    }
};

double effective_area(const Point& c, const Point& p, const Point &n)
{
    const Point c_n = vector_sub(n, c);
    const Point c_p = vector_sub(p, c);
    const double norm = cross_product_norm(c_n, c_p);
    return 0.5 * norm;
}

static double effective_area(VertexIndex current, VertexIndex previous,
                            VertexIndex next, const Linestring& input_line)
{
    const Point& c = input_line[current];
    const Point& p = input_line[previous];
    const Point& n = input_line[next];
    return effective_area(c, p, n); 
}


static double effective_area(const VertexNode& node,
                            const Linestring& input_line)
{
    return effective_area(node.vertex, node.prev_vertex,
                            node.next_vertex, input_line);
}

Visvalingam_Algorithm::Visvalingam_Algorithm(const Linestring& input)
    : m_effective_areas(input.size(), 0.0)
    , m_input_line(input)
{
    // Compute effective area for each point in the input (except endpoints)
    std::vector<VertexNode*> node_list(input.size(), NULL);
    Heap<VertexNode*, VertexNodeCompare> min_heap(input.size());
    for (VertexIndex i=1; i < input.size()-1; ++i)
    {
        double area = effective_area(i, i-1, i+1, input);
        if (area > NEARLY_ZERO)
        {
            node_list[i] = new VertexNode(i, i-1, i+1, area);
            min_heap.insert(node_list[i]);
        }
    }

    double min_area = -std::numeric_limits<double>::max();
    while (!min_heap.empty())
    {
        VertexNode* curr_node = min_heap.pop();
        assert (curr_node == node_list[curr_node->vertex]);

        // If the current point's calculated area is less than that of the last
        // point to be eliminated, use the latter's area instead. (This ensures
        // that the current point cannot be eliminated without eliminating
        // previously eliminated points.)
        min_area = std::max(min_area, curr_node->area);

        VertexNode* prev_node = node_list[curr_node->prev_vertex];
        if (prev_node != NULL)
        {
            prev_node->next_vertex = curr_node->next_vertex;
            prev_node->area = effective_area(*prev_node, input);
            min_heap.reheap(prev_node);
        }
        
        VertexNode* next_node = node_list[curr_node->next_vertex];
        if (next_node != NULL)
        {
            next_node->prev_vertex = curr_node->prev_vertex;
            next_node->area = effective_area(*next_node, input);
            min_heap.reheap(next_node);
        }

        // store the final value for this vertex and delete the node.
        m_effective_areas[curr_node->vertex] = min_area;
        node_list[curr_node->vertex] = NULL;
        delete curr_node;
    }
    node_list.clear();
}

void Visvalingam_Algorithm::simplify(double area_threshold,
                                    Linestring* res) const
{
    assert(res);
    for (VertexIndex i=0; i < m_input_line.size(); ++i)
    {
        if (contains_vertex(i, area_threshold))
        {
            res->push_back(m_input_line[i]);
        }
    }
    if (res->size() < 4)
    {
        res->clear();
    }
}


void Visvalingam_Algorithm::simplify_ratio(size_t ratio,
                                    Linestring* res) const
{
    assert(res);
    // sort by area
    std::vector<double> ordered_area(m_effective_areas);
    std::sort(ordered_area.begin(), ordered_area.end());
    size_t idx = m_effective_areas.size() * ratio / 100;
    assert(idx >= 0);
    assert(idx < ordered_area.size());
    double threshold = ordered_area[idx];
    std::cout << "area threshold: " << threshold << std::endl;
    simplify(threshold, res);
}

void Visvalingam_Algorithm::print_areas(std::ostream &stream) const
{
    for (VertexIndex i=0; i < m_effective_areas.size(); ++i)
    {
        stream << i << ": " << m_effective_areas[i] << "\n";
    }
}

void run_visvalingam(const Linestring &shape, Linestring *res)
{
    Visvalingam_Algorithm vis_algo(shape);
    Linestring simplified_linestring;
    vis_algo.simplify(0.002, res);
}

void run_visvalingam(const MultiPolygon &shape, MultiPolygon &res)
{
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
}

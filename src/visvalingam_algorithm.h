//
//
// 2013 (c) Mathieu Courtemanche

#ifndef VISVALINGAM_ALGORITHM_H
#define VISVALINGAM_ALGORITHM_H

#include <vector>
#include <cassert>
#include "polyline.h"

class Visvalingam_Algorithm
{
public:
    Visvalingam_Algorithm(const Linestring& input);

    void simplify(double area_threshold, Linestring* res) const;

    void print_areas() const;

private:
    bool contains_vertex(VertexIndex vertex_index, double area_threshold) const;

    std::vector<double> m_effective_areas;
    const Linestring& m_input_line;
};

inline bool
Visvalingam_Algorithm::contains_vertex(VertexIndex vertex_index,
                                        double area_threshold) const
{
    assert(vertex_index < m_effective_areas.size());
    assert(m_effective_areas.size() != 0);
    if (vertex_index == 0 || vertex_index == m_effective_areas.size()-1)
    {
        // end points always kept since we don't evaluate their effective areas
        return true;
    }
    return m_effective_areas[vertex_index] > area_threshold;
}

#endif // VISVALINGAM_ALGORITHM_H

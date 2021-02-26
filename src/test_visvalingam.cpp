#include <iostream>
#include <sstream>
#include <cassert>
#include "polyline.h"
#include "heap.hpp"
#include "visvalingam_algorithm.h"
#include "csv.h"

void test_read_csv()
{
    std::string csv_text = "1,2,3,42,5\n"
                           "0,2,4,42,8\n"
                           "1,3,5,42,9\n";
    std::istringstream ss(csv_text);
    CSVIterator iter(ss);

    int nb_lines = 0;
    while(iter != CSVIterator()) {
        assert(stoi((*iter)[3]) == 42);
        ++iter;
        nb_lines++;
    }
    assert(nb_lines == 3);
}

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

void test_cross_product_norm()
{
    Point a, b, c;
    a.X = 10;
    a.Y = 8;
    b.X = 2;
    b.Y = 5;
    c.X = -100;
    c.Y = 8;

    double res1 = cross_product_norm(a,b);
    double res2 = cross_product_norm(b,a);
    double res3 = cross_product_norm(a,c);
    assert(res1 == 34);
    assert(res2 == 34);
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
        test_read_csv();
        test_vector_sub();
        test_cross_product_norm();
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

int main(int argc, char **argv)
{
    if (unit_tests())
    {
        std::cout << "All tests succeeded" << std::endl;
        return 0;
    }

    std::cout << "Unit test failure" << std::endl;
    return 1;
}

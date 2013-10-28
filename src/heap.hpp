//
//
// 2013 (c) Mathieu Courtemanche

#ifndef HEAP_HPP
#define HEAP_HPP

#include <cassert>
#include <algorithm>
#include <functional>
#include <boost/unordered_map.hpp>
#include <iostream>

#define CHECK_HEAP_CONSISTENCY 0

// Special Heap to store vertex data as we process the input linestring.
// We need a min-heap with:
//    standard: top(), pop(), push(elem)
//    non-standard: remove(elem) anywhere in heap, 
//                  or alternatively, re-heapify a single element.
//
// This implementation uses a reheap() function and maintains a map to quickly
// lookup a heap index given an element T.
//
template <typename T, typename Comparator = std::less<T> >
class Heap
{
public:
    typedef boost::unordered_map<T, size_t> NodeToHeapIndexMap;

    Heap(size_t fixed_size)
    :
    m_data(new T[fixed_size]),
    m_capacity(fixed_size),
    m_size(0),
    m_comp(Comparator()),
    m_node_to_heap()
    {
    }

    ~Heap()
    {
        delete[] m_data;
    }

    void insert(T node)
    {
        assert(m_size < m_capacity);

        // insert at left-most available leaf (i.e.: bottom)
        size_t insert_idx = m_size;
        m_size++;
        m_data[insert_idx] = node;

        // bubble up until it's in the right place
        bubble_up(insert_idx);

#if CHECK_HEAP_CONSISTENCY
        validate_heap_state();
#endif
    }

    const T top() const
    {
        assert(!empty());
        return m_data[NODE_TYPE_ROOT];
    }

    T pop()
    {
        T res = top();
        m_size--;
        clear_heap_index(res);
        if (!empty())
        {
            // take the bottom element, stick it at the root and let it
            // bubble down to its right place.
            m_data[NODE_TYPE_ROOT] = m_data[m_size];
            bubble_down(NODE_TYPE_ROOT);
        }
#if CHECK_HEAP_CONSISTENCY
        validate_heap_state();
#endif
        return res;
    }

    bool empty() const
    {
        return m_size == 0;
    }

    /** reheap re-enforces the heap property for a node that was modified
     *  externally. Logarithmic performance for a node anywhere in the tree.
     */
    void reheap(T node)
    {
        size_t heap_idx = get_heap_index(node);
        bubble_down(bubble_up(heap_idx));
#if CHECK_HEAP_CONSISTENCY
        validate_heap_state();
#endif
    }

private:
    Heap(const Heap& other);
    Heap& operator=(const Heap& other);

    enum NodeType
    {
        NODE_TYPE_ROOT = 0,
        NODE_TYPE_INVALID = ~0
    };

    size_t parent(size_t n) const
    {
        if (n != NODE_TYPE_ROOT)
        {
            return (n-1)/2;
        }
        else
        {
            return NODE_TYPE_INVALID;
        }
    }

    size_t left_child(size_t n) const
    {
        return 2*n + 1;
    }

    size_t bubble_up(size_t n)
    {
        size_t parent_idx = parent(n);
        while (n != NODE_TYPE_ROOT && parent_idx != NODE_TYPE_INVALID
                && m_comp(m_data[n], m_data[parent_idx]))
        {
            std::swap(m_data[n], m_data[parent_idx]);
            // update parent which is now at heap index 'n'
            set_heap_index(m_data[n], n);

            n = parent_idx;
            parent_idx = parent(n);
        }

        set_heap_index(m_data[n], n);
        return n;
    }

    size_t small_elem(size_t n, size_t a, size_t b) const
    {
        size_t smallest = n;
        if (a < m_size && m_comp(m_data[a], m_data[smallest]))
        {
            smallest = a;
        }
        if (b < m_size && m_comp(m_data[b], m_data[smallest]))
        {
            smallest = b;
        }
        return smallest;
    }

    size_t bubble_down(size_t n)
    {
        while (true)
        {
            size_t left = left_child(n);
            size_t right = 1+left;
            size_t smallest = small_elem(n, left, right);
            if (smallest != n)
            {
                std::swap(m_data[smallest], m_data[n]);
                // update 'smallest' which is now at 'n'
                set_heap_index(m_data[n], n);
                n = smallest;
            }
            else
            {
                set_heap_index(m_data[n], n);
                return n;
            }
        }
    }
    
    void set_heap_index(T node, size_t n)
    {
        m_node_to_heap[node] = n;
    }

    void clear_heap_index(T node)
    {
        size_t del_count = m_node_to_heap.erase(node);
        assert(del_count == 1);
    }

    size_t get_heap_index(T node) const
    {
        typename NodeToHeapIndexMap::const_iterator it = m_node_to_heap.find(node);
        assert (it != m_node_to_heap.end());
        size_t heap_idx = it->second;
        assert (m_data[heap_idx] == node);
        return heap_idx;
    }

#if CHECK_HEAP_CONSISTENCY
    void validate_heap_state() const
    {
        assert(m_node_to_heap.size() == m_size);
        for (size_t i = 0; i < m_size; ++i)
        {
            // ensure we cached the node properly
            assert(i == get_heap_index(m_data[i]));
            // make sure heap property is preserved
            size_t parent_index = parent(i);
            if (parent_index != NODE_TYPE_INVALID)
            {
                assert(m_comp(m_data[parent_index], m_data[i]));
            }
        }
    }
#endif

    void dump_state() const
    {
        std::cout << "heap state:" << std::endl;
        for (size_t i = 0; i < m_size; ++i)
        {
            if (i != 0)
                std::cout << ", ";
            std::cout << i << ": " << m_data[i];
        }
        std::cout << std::endl;

        std::cout << "node to heap state: " << std::endl;
        typename NodeToHeapIndexMap::const_iterator it;
        for (it = m_node_to_heap.begin(); it != m_node_to_heap.end(); ++it)
        {
            std::cout << it->first << " -> " << it->second << std::endl; 
        }
    }

private:
    T*  m_data;
    size_t m_capacity;
    size_t m_size;
    const Comparator m_comp;
    NodeToHeapIndexMap m_node_to_heap;
};

#endif // HEAP_HPP

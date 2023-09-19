#pragma once

#include <cstddef>

#include "tk_alloc.h"

namespace tk
{

    template <typename T>
    struct Array
    {
        // Returns the length of the array
        uint32_t Length() const {
            return _top;
        }

        uint32_t Capacity() const {
            return _capacity;
        }

        // Adds an item and return its index, growing the array if needed
        uint32_t Add( const T &item, IAllocator *alloc ) {
            if (_top >= _capacity) {
                _grow( alloc );
            }

            uint32_t ndx = _top;
            _elems[_top++] = item;
            return ndx;
        }



        Array() : _elems(nullptr), _top(0), _capacity(0) {}

        void _grow( IAllocator *alloc ) {

            // TODO: better resize strategy
            if (_capacity == 0) {
                _capacity = 8;
            } else if (_capacity <= 1024) {
                _capacity *= 2;
            } else {
                _capacity = _capacity + 1024;
            }
            _elems = (T*)realloc( _elems, sizeof(T) * _capacity );
        }

        T *_elems;
        size_t _top;
        size_t _capacity;

    };

}
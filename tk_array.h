#pragma once

#include <cstddef>

#include "tk_alloc.h"

namespace tk
{
    template <typename T>
    struct Array
    {

        // This is just passed along to the allocator, can be used for tracing
        // or categorizing allocations
        void SetAllocTag( uint64_t tag )
        {
            _alloctag = tag;
        }

        // Returns the length of the array
        uint32_t Length() const {
            return _top;
        }

        void SetLength(size_t target_len, IAllocator* alloc)
        {
            Resize(target_len, alloc);
            _top = target_len;
        }

        // Ensures capacity is target size if smaller
        void Resize( size_t target_size, IAllocator* alloc )
        {
            if (_capacity < target_size) {
                _grow_to_capacity( alloc, target_size );
            }
        }

        T Pop() {
            size_t ndx = _top;
            _top--;
            return _elems[ndx];

        }

        uint32_t Capacity() const {
            return _capacity;
        }

        void Clear() {
            _top = 0;
        }

        void ClearZero() {
            _top = 0;
            memset( _elems, 0, sizeof(T) * _capacity );
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

        T& operator[]( int index ) {
            return _elems[index];
        }

        T& operator[]( int index ) const {
            return _elems[index];
        }

        Array() : _elems(nullptr), _top(0), _capacity(0) {}

        void Free( IAllocator *alloc )
        {
            TK_FREE( alloc, _elems, alloctag );
            _elems = nullptr;
        }

        void _grow( IAllocator *alloc ) {
            // TODO: better resize strategy
            size_t grow_capacity;
            if (_capacity == 0) {
                grow_capacity = 8;
            } else if (_capacity <= 1024) {
                grow_capacity = _capacity * 2;
            } else {
                grow_capacity = _capacity + 1024;
            }
            _grow_to_capacity( alloc, grow_capacity );
        }

        void _grow_to_capacity( IAllocator *alloc, size_t grow_capacity ) {

            _capacity = grow_capacity;
            _elems = (T*)TK_REALLOC( alloc, _elems, sizeof(T) * _capacity, _alloctag );
        }

        T *_elems;
        size_t _top;
        size_t _capacity;
        uint64_t _alloctag;

    };

}
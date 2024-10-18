#pragma once
#include <stdint.h>
#include <assert.h>

#include "tk_alloc.h"

namespace tk
{
    union Handle
    {
        struct
        {
            uint64_t index : 32;
            uint64_t version : 32;
        } u;
        uint64_t h = 0;
    };

    // "Generational Index" or "handle table" based on the Handle Table from Randy Gaul
    template <typename T>
    struct SlotList
    {
        // A slotlist with a default allocator
        SlotList( IAllocator *alloc, size_t capacity, uint64_t alloc_tag = 0 );
        void Free( IAllocator *alloc );

        Handle AddItem( const T &data );
        void DropItem( Handle h );
        T Lookup(Handle h);
        T &LookupRef(Handle h);
        void Update(Handle h, const T& data);
        bool Validate(Handle h);

    private:

        union Entry
        {
            struct
            {
                uint64_t data_index : 32;
                uint64_t version : 32;
            } u;
            uint64_t h;
        };

        uint64_t _alloctag = 0;

        uint64_t _capacity = 0;
        uint64_t _freelist = 0;

        SlotList::Entry *_entries = NULL;
        T *_storage = NULL;

    };

    template <typename T>
    SlotList<T>::SlotList( IAllocator *alloc, size_t capacity, uint64_t alloctag )
    {
        _capacity = capacity;
        _entries = (Entry*)TK_ALLOC( alloc, sizeof(Entry) * _capacity, alloctag );
        _storage = (T*)TK_ALLOC( alloc, sizeof(T) * _capacity, alloctag );
        _alloctag = alloctag;

        for (int i=0; i < _capacity; i++) {
            _entries[i].u.data_index = i + 1; // next item in freelist
            _entries[i].u.version = 0; // initial version
        }
        _entries[capacity - 1].h = ~0;
    }

    template <typename T>
    void SlotList<T>::Free( IAllocator *alloc )
    {
        TK_FREE( alloc, _entries, alloctag );
        TK_FREE( alloc, _storage, alloctag );
    }

    template <typename T>
    Handle SlotList<T>::AddItem( const T &data )
    {
        if (_freelist == ~0) {
            assert(0);
            // TODO: Grow list
        }

        // Pop element off the free list
        uint64_t index = _freelist;
        _freelist = _entries[_freelist].u.data_index;

        // Create a new handle
        _entries[index].u.data_index = index; // TODO: do we need the Data here?
        _storage[index] = data;

        Handle hresult;
        hresult.u.index = index;
        hresult.u.version = _entries[index].u.version;

        return hresult;
    }

    template <typename T>
    void SlotList<T>::DropItem( Handle h )
    {
        // Put handle on the freelist
        uint64_t index = h.u.index;
        _entries[index].u.data_index = _freelist;
        _freelist = index;

        // Increment the count. This signifies a change in lifetime
        // (this particular handle `h` is now dead).
        // The next time this particular index is used in Alloc,
        // a new `count` will be used to uniquely identify.
        _entries[index].u.version++;
    }

    template <typename T>
    T SlotList<T>::Lookup(Handle h)
    {
        // make sure version is still valid
        uint64_t index = h.u.index;
        uint64_t version = h.u.version;
        assert(_entries[index].u.version == version);

        return _storage[index];
    }

    template <typename T>
    T& SlotList<T>::LookupRef(Handle h)
    {
        // make sure version is still valid
        uint64_t index = h.u.index;
        uint64_t version = h.u.version;
        assert(_entries[index].u.version == version);

        return _storage[index];
    }


    template <typename T>
    void SlotList<T>::Update(Handle h, const T& data)
    {
        uint64_t index = h.u.index;
        uint64_t count = h.u.version;
        assert(_entries[index].u.version == count);

        _storage[index] = data;
    }

    template <typename T>
    bool SlotList<T>::Validate(Handle h)
    {
        uint64_t index = h.u.index;
        uint64_t version = h.u.version;
        if (index >= _capacity) return false;
        return _entries[index].u.version == version;
    }


}; // namespace TK


#pragma once

#include <stdlib.h>

namespace tk {

    class IAllocator {
        virtual void* alloc( size_t size, uint64_t tag, const char *file, uint32_t line )=0;
        virtual void* realloc( void *ptr, size_t size, uint64_t tag, const char *file, uint32_t line )=0;
        virtual void free( void *ptr, const char *file, uint32_t line)=0;
    };

    // Default allocator that uses the system Malloc/Free
    class DefaultAllocator : public IAllocator {
        virtual void* alloc( size_t size, uint64_t tag, const char *file, uint32_t line );
        virtual void* realloc( void *ptr, size_t size, uint64_t tag, const char *file, uint32_t line );
        virtual void free( void *ptr, const char *file, uint32_t line);
    };

};